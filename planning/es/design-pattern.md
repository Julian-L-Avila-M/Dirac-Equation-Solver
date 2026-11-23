# Arquitectura y Patrones de Diseño de `dirac_solver`

> 🌐 **Idioma / Language:** **Español** | **[English](../en/design-pattern.md)**

Este documento describe la arquitectura de software de la librería `dirac_solver`. El diseño se basa en un conjunto de patrones de diseño canónicos para garantizar que el código sea modular, extensible, mantenible y fácil de usar. El objetivo principal es lograr una clara separación de conceptos (*Separation of Concerns*): la definición del problema físico se mantiene desacoplada de los algoritmos numéricos utilizados para resolverlo.

## Tabla de Contenidos

1. [Resumen - Tabla Comparativa: Patrones y Problemas que Resuelven](#1-tabla-comparativa-patrones-y-problemas-que-resuelven)
2. [Patrón Builder - Configuración de la Simulación](#2-patrón-builder---configuración-de-la-simulación)
3. [Patrón Strategy - Flexibilidad en los Algoritmos](#3-patrón-strategy---flexibilidad-en-los-algoritmos)
4. [Patrones Template y Factory Method - Extensibilidad de Componentes Físicos](#4-patrones-template-y-factory-method---extensibilidad-de-componentes-físicos)
5. [Patrón Observer - Visualización y Monitoreo](#5-patrón-observer---visualización-y-monitoreo)
6. [Patrón Facade - Interfaz Simplificada de E/S](#6-patrón-facade---interfaz-simplificada-de-es)
7. [Patrón Decorator - Extensión Dinámica de Funcionalidades](#7-patrón-decorator--extensión-dinámica-de-funcionalidades)
8. [Patrón Adapter - Interoperabilidad entre Python y C++ / Librerías Externas](#8-patrón-adapter--interoperabilidad-con-librerías-externas)
9. [Ecosistema - Relaciones entre Patrones](#9-relaciones-entre-los-patrones)
10. [Alternativas Consideradas y Descartadas](#10-alternativas-consideradas-y-descartadas)
11. [Conclusión](#11-conclusión)

---

## 1. Tabla Comparativa: Patrones y Problemas que Resuelven

| Patrón | Problema que resuelve | Implementación en `dirac_solver` |
| :--- | :--- | :--- |
| **Builder** | Construcción de objetos complejos sin constructores telescópicos con demasiados parámetros. | `DiracProblemBuilder` construye y valida `SimulationProblem`. |
| **Strategy** | Permite intercambiar algoritmos (integradores temporales, condiciones de frontera) sin modificar el cliente. | `TimeIntegrator` (`FDTDLeapfrogIntegrator`) y `BoundaryCondition` (`PeriodicBoundary`, `AbsorbingBoundary`). |
| **Template + Factory Method** | Extender nuevos componentes físicos de forma coherente y centralizada. | `Potential` (C++), `InitialState` (Python) y resolución de fronteras por nombre en `DiracProblemBuilder.build()`. |
| **Observer** | Desacoplar la evolución temporal del monitoreo de observables físicos y almacenamiento. | `Observables` y `HDF5Storage` invocados periódicamente desde `DiracSolver.run_simulation()`. |
| **Facade** | Ocultar la complejidad del subsistema de entrada/salida HDF5 y renderizado detrás de una interfaz sencilla. | `HDF5Storage`, `create_animation` y `create_animation_1d`. |
| **Decorator** | Extender funcionalidades dinámicamente (ej. logging, validaciones, profiling). | Instrumentación de métodos de simulación y medición de tiempos. |
| **Adapter** | Traducir interfaces entre Python (`dirac_solver`) y el motor C++ (`_core`) o librerías externas. | Clases envoltorio en `potentials.py` (`core_potential`) y enlaces `pybind11`. |

---

## 2. Patrón Builder - Configuración de la Simulación

**Propósito:** Construir un objeto complejo (`SimulationProblem`) paso a paso, proporcionando una API fluida y legible que separa la construcción y validación de la representación final.

**Aplicación:** El usuario final define un problema completo ensamblando sus partes constituyentes (malla `Grid`, estado inicial `InitialState`, potencial `Potential`, condición de frontera `BoundaryCondition` y parámetros temporales) sin necesidad de un constructor con múltiples argumentos posicionales propensos a error. `DiracProblemBuilder` actúa como punto de entrada principal y valida la coherencia del problema en `.build()`.

**Código de Ejemplo Real (`dirac_solver.core`):**

```python
import dirac_solver as ds

grid = ds.Grid(shape=(400,), spacing=0.05)
spinor = ds.ConstantSpinor(momentum=[2.0, 0.0, 0.0])
packet = ds.GaussianPacket(constant_spinor=spinor, center=[-2.0, 0.0, 0.0], spatial_width=0.8)

problem = (
    ds.DiracProblemBuilder()
    .set_grid(grid)
    .set_initial_state(packet)
    .set_potential(ds.CoulombPotential(Z=1.0, epsilon=0.1))
    .set_boundary_condition("absorbing")
    .set_time_parameters(time_step=0.01, total_time=5.0)
    .build()
)

solver = ds.DiracSolver(problem)
solver.run_simulation()
```

---

## 3. Patrón Strategy - Flexibilidad en los Algoritmos

**Propósito:** Definir una familia de algoritmos, encapsular cada uno bajo una interfaz común y hacerlos intercambiables. Permite que el algoritmo varíe independientemente del cliente que lo utiliza.

**Aplicación:** Es el patrón central del núcleo numérico en `cpp_src/`:
1. **Integración Temporal (`TimeIntegrator`):** Define la interfaz abstracta con `step()` y `get_name()`. La estrategia concreta `FDTDLeapfrogIntegrator` implementa el esquema explícito de diferencias finitas centradas. Gracias a la clase trampolín `PyTimeIntegrator` en `pybind11`, incluso se pueden implementar nuevos integradores desde Python.
2. **Condiciones de Frontera (`BoundaryCondition`):** Define la interfaz abstracta `apply(psi, grid)`. Las estrategias concretas `PeriodicBoundary` y `AbsorbingBoundary` pueden intercambiarse transparentemente sin alterar la estructura de `DiracSolver`.

---

## 4. Patrones Template y Factory Method - Extensibilidad de Componentes Físicos

**Propósito:**
- **Template Method:** Define el contrato o esqueleto de un componente físico en una clase base abstracta, delegando la evaluación matemática concreta a las subclases.
- **Factory Method:** Centraliza la lógica de instanciación de objetos a partir de identificadores descriptivos.

**Aplicación:**
- En C++, la clase abstracta `Potential` exige implementar `evaluate(r)` y `get_name()` en `FreeParticle`, `CoulombPotential`, `YukawaPotential`, `InfiniteWellPotential` y `ScalarPotential`.
- En Python, `InitialState` (`initial_state.py`) define la clase base abstracta con `evaluate(grid_position)` y `evaluate_on_grid(grid)`, implementada por `GaussianPacket` y `PlaneWave`.
- En `DiracProblemBuilder.build()`, los identificadores en cadena `"periodic"` y `"absorbing"` actúan como método fábrica que instancia automáticamente `_core.PeriodicBoundary()` o `_core.AbsorbingBoundary(0.3, 20)`.

---

## 5. Patrón Observer - Visualización y Monitoreo

**Propósito:** Definir una relación de notificación entre el motor de simulación y los manejadores de análisis/almacenamiento, de modo que cada avance en el estado del campo espinorial actualice automáticamente los registros sin acoplar el integrador C++ con la lógica de análisis.

**Aplicación:** Durante `DiracSolver.run_simulation(storage_handler, save_every_n_steps, observables_handler)`, el bucle de evolución temporal notifica en cada paso al manejador `Observables` (`compute_all(current_time)`) y cada $N$ pasos al manejador `HDF5Storage` (`write_snapshot(psi, current_time)`).

---

## 6. Patrón Facade - Interfaz Simplificada de E/S

**Propósito:** Proporcionar una interfaz unificada y sencilla para un subsistema complejo.

**Aplicación:** El módulo `dirac_solver/storage.py` encapsula la creación de grupos, atributos de metadatos, *chunking* y redimensionamiento dinámico de datasets extensibles de `h5py` detrás de la clase `HDF5Storage` (con soporte de *context manager* `with`) y las funciones de alto nivel `create_animation` y `create_animation_1d`.

---

## 7. Patrón Decorator – Extensión Dinámica de Funcionalidades

**Propósito:** Añadir responsabilidades adicionales a un objeto o método de manera dinámica sin modificar su estructura interna.

**Aplicación:** Permite extender métodos del solver o potenciales con medición de tiempos de ejecución (*profiling*), registro detallado (*logging*) o validaciones numéricas (como monitoreo de conservación de la norma).

```python
@log_runtime
def run_simulation(self, ...):
    # Método decorado con medición de tiempo de ejecución
    ...
```

---

## 8. Patrón Adapter – Interoperabilidad con Librerías Externas

**Propósito:** Convertir la interfaz de una clase en otra que el cliente espera, permitiendo que clases con interfaces distintas trabajen juntas.

**Aplicación:**
En `dirac_solver/potentials.py`, las clases de Python (`FreeParticle`, `CoulombPotential`, `YukawaPotential`, `InfiniteWellPotential`, `ScalarPotential`) actúan como adaptadores ligeros que exponen la propiedad `.core_potential` hacia las instancias subyacentes de C++ (`_core.*`), traduciendo secuencias de Python (`tuple`, `np.ndarray`) a `std::vector<double>`. Asimismo, abre la puerta a integrar futuros backends externos (SciPy, CuPy, PETSc).

---

## 9. Relaciones entre los Patrones

Los patrones colaboran estrechamente para mantener una arquitectura cohesiva:

- **Builder → Strategy / Template-Factory:** `DiracProblemBuilder` ensambla el objeto `SimulationProblem` combinando la estrategia de frontera (`BoundaryCondition`) y los componentes físicos (`InitialState`, `Potential`).
- **Strategy ↔ Observer:** `DiracSolver` ejecuta un paso de la estrategia `FDTDLeapfrogIntegrator.step()` y notifica inmediatamente a `Observables` y `HDF5Storage`.
- **Facade ↔ Adapter:** `DiracSolver` extrae `core_potential` y `core_boundary` mediante el adaptador de propiedades y delega la persistencia a la fachada `HDF5Storage`.

---

## 10. Alternativas Consideradas y Descartadas

### 1. Builder
- **Alternativa:** Constructores telescópicos con muchos parámetros posicionales.
- **Descartado porque:** Generan código difícil de leer, propenso a errores de orden de argumentos y difícil de extender.

### 2. Strategy
- **Alternativa:** Uso de banderas booleanas o bloques `if/elif/else` gigantes dentro del bucle principal.
- **Descartado porque:** Viola el principio Abierto/Cerrado (OCP) y acopla la lógica de alto nivel con detalles numéricos.

### 3. Template + Factory Method
- **Alternativa:** Funciones sueltas sin jerarquía común para potenciales y estados iniciales.
- **Descartado porque:** Impide el polimorfismo en C++ (`std::shared_ptr<Potential>`) y rompe la consistencia de la API.

### 4. Observer
- **Alternativa:** Calcular observables e incrustar llamadas de `h5py` directamente dentro del integrador C++.
- **Descartado porque:** Acoplaría el motor numérico C++ con librerías de E/S y análisis, reduciendo el rendimiento cuando no se requiere almacenamiento.

### 5. Facade
- **Alternativa:** Obligar al usuario a crear y redimensionar manualmente datasets de `h5py` en cada paso.
- **Descartado porque:** Aumenta drásticamente la curva de aprendizaje y el código repetitivo (*boilerplate*) en los scripts del usuario.

---

## 11. Conclusión

El uso disciplinado de patrones de diseño en `dirac_solver` garantiza:
- **Modularidad:** Cada componente en Python y C++ cumple una única responsabilidad clara.
- **Extensibilidad:** Es sencillo añadir nuevos potenciales, estados iniciales, condiciones de frontera o integradores temporales.
- **Mantenibilidad:** Bajo acoplamiento entre el motor numérico C++17 y las utilidades de análisis en Python.
- **Rendimiento y Expresividad:** Velocidad nativa en C++ con una interfaz científica fluida en Python.
