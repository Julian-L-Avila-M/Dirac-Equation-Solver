# Arquitectura y Patrones de Diseño de dirac_solver

Este documento describe la arquitectura de software de la librería dirac_solver.
El diseño se basa en un conjunto de patrones de diseño canónicos para garantizar
que el código sea modular, extensible, mantenible y fácil de usar. El objetivo
principal es lograr una clara separación de conceptos (Separation of Concerns):
la definición del problema físico se mantiene desacoplada de los algoritmos
numéricos utilizados para resolverlo.
Tabla de Contenidos


  1. Resumen - Tabla Comparativa: Patrones y Problemas que Resuelven

  2. Patrón Builder - Configuración de la Simulación

  3. Patrón Strategy - Flexibilidad en los Algoritmos

  4. Patrones Template y Factory Method - Extensibilidad de Componentes Físicos

  5. Patrón Observer - Visualización y Monitoreo

  6. Patrón Facade - Interfaz Simplificada de I/O

  7. Patrón Decorator – Extensión Dinámica de Funcionalidades

  8. Patrón Adapter – Interoperabilidad con Librerías Externas

  9. Ecosistema - Relaciones entre patrones

  10. Alternativas -  Consideradas y Descartadas

  11. Conclusión 


 


## 1. Tabla Comparativa: Patrones y Problemas que Resuelven

| Patrón                        | Problema que resuelve                                                                                 |
| ----------------------------- | ----------------------------------------------------------------------------------------------------- |
| **Builder**                   | Construcción de objetos complejos sin constructores con demasiados parámetros.                        |
| **Strategy**                  | Permite intercambiar algoritmos (ej. integradores, condiciones de frontera) sin modificar el cliente. |
| **Template + Factory Method** | Extender nuevos componentes físicos de forma coherente y centralizada.                                |
| **Observer**                  | Desacoplar la simulación de la visualización y el monitoreo de datos.                                 |
| **Facade**                    | Ocultar la complejidad del sistema de entrada/salida detrás de una interfaz sencilla.                 |
| **Decorator**                 | Extender funcionalidades dinámicamente (ej. logging, validaciones, profiling).                        |
| **Adapter**                   | Interoperar con librerías externas que tienen APIs diferentes.                                        |

---


---

## 2. Patrón Builder - Configuración de la Simulación

Propósito: Construir un objeto complejo (SimulationProblem) paso a paso,
proporcionando una API fluida y legible que separa la construcción de la
representación.

Aplicación: El usuario final define un problema completo ensamblando sus partes
constituyentes (geometría, potencial, condiciones, etc.) sin necesidad de un
constructor con múltiples y confusos argumentos. Un DiracProblemBuilder es el
punto de entrada principal para el usuario. Este objeto guía la creación de una
simulación válida y cohesiva.

Código de Ejemplo (examples/hydrogen_atom.py):

### API fluida gracias al Patrón Builder
```text
simulation = (DiracProblemBuilder()
              .set_geometry(Radial(r_max=100, N=1024))
              .set_potential("coulomb", Z=1)
              .set_initial_condition(HydrogenGroundState())
              .set_solver("Arnoldi", num_eigenvalues=5)
              .build())

results = simulation.run()
```
---

## 3. Patrón Strategy - Flexibilidad en los Algoritmos

Propósito: Definir una familia de algoritmos, encapsular cada uno y hacerlos
intercambiables. Permite que el algoritmo varíe independientemente del cliente
que lo utiliza.

Aplicación: Este es el patrón más importante para el núcleo numérico en
solvers/. Permite al usuario cambiar el método de integración temporal (ej.
CrankNicolson vs. SplitOperator) o las condiciones de frontera (ej. Dirichlet
vs. Absorbing) sin modificar el código del bucle de simulación principal.

---

## 4. Patrones Template y Factory Method - Extensibilidad de Componentes Físicos

Propósito:

  - Template Method: Define el esqueleto de un algoritmo en una clase base,
	delegando la implementación de ciertos pasos a las subclases.
  - Factory Method: Proporciona una interfaz para crear objetos, pero permite
	que las subclases alteren el tipo de objetos que se crearán.
	
Aplicación: Se combinan en el módulo potentials/ (y análogamente en geometry/ y
conditions/).

  - potentials/base.py define una BasePotential (el Template) que requiere que
	todas las subclases implementen un método evaluate().
  - Una función create_potential() (el Factory Method) centraliza la lógica de
	instanciación, permitiendo crear potenciales a partir de un string (ej. desde
	un archivo de configuración).
  
---

## 5. Patrón Observer - Visualización y Monitoreo

Propósito: Definir una dependencia uno-a-muchos entre objetos, de modo que
cuando un objeto (Subject) cambia de estado, todos sus dependientes (Observers)
son notificados y actualizados automáticamente.

Aplicación: Desacopla el motor de la simulación de los componentes de
visualización y registro de datos. El objeto Simulation (Subject) no necesita
saber nada sobre cómo se muestran los datos. Simplemente notifica a sus
observadores (ej. WavefunctionPlotter, EnergyLogger) en cada paso de tiempo.

---

## 6. Patrón Facade - Interfaz Simplificada de I/O

Propósito: Proporcionar una interfaz unificada y simplificada a un conjunto de
interfaces en un subsistema.

Aplicación: El módulo io/ puede contener lógica compleja para guardar en
diferentes formatos (HDF5, npz), cargar, y parsear archivos de configuración. El
patrón Facade oculta esta complejidad detrás de una única clase IOManager con
métodos sencillos como save(simulation, path) y load(path).



## 7. Patrón Decorator – Extensión Dinámica de Funcionalidades

**Propósito:** Añadir responsabilidades a un objeto de manera dinámica sin modificar su estructura.

**Aplicación:** Permite extender solvers o visualizadores sin cambiar su código base. Por ejemplo:

* Medir tiempos de ejecución de cualquier solver.
* Añadir logging detallado a un `Potential` o `Solver` ya existente.
* Incorporar validaciones numéricas (ej. estabilidad de la norma de la función de onda).

Esto permite una instrumentación ligera y flexible, útil para depuración y optimización.

Ejemplo hipotético:

```python
@log_runtime
def solve(self):
    # método original decorado con medición de tiempo
    ...
```

---

## 8. Patrón Adapter – Interoperabilidad con Librerías Externas

**Propósito:** Convertir la interfaz de una clase existente en otra que el cliente espera, permitiendo la colaboración entre clases incompatibles.

**Aplicación:**
El solver puede necesitar interoperar con **bibliotecas externas** (SciPy, PETSc, CuPy, PyTorch). Cada una tiene interfaces distintas para vectores, matrices y rutinas de eigenvalores.

El Adapter proporciona una capa intermedia que traduce entre la API de `dirac_solver` y la API de la librería de backend. Así, el código cliente no depende de la librería específica.

Ejemplo hipotético:

```python
class PETScAdapter(LinearSolver):
    def __init__(self, petsc_solver):
        self._solver = petsc_solver
    
    def solve(self, A, b):
        return self._solver.solve(A.to_petsc(), b.to_petsc())
```

Esto facilita ofrecer múltiples backends (NumPy, SciPy, GPU) sin que el usuario final deba cambiar su código.


---

## 9. Relaciones entre los Patrones

Los patrones no funcionan de forma aislada; se complementan para mantener la arquitectura coherente:

* **Builder → Strategy / Template-Factory**:
  El `DiracProblemBuilder` ensambla la simulación seleccionando estrategias numéricas y componentes físicos generados mediante fábricas.

* **Strategy ↔ Observer**:
  Cada estrategia de integración puede notificar cambios en la simulación que luego son procesados por observadores como `EnergyLogger`.

* **Observer ↔ Decorator**:
  Los observadores mismos pueden ser decorados para añadir validaciones o mediciones de rendimiento sin tocar su código base.

* **Facade ↔ Adapter**:
  El `IOManager` (Facade) puede usar adaptadores internos para guardar datos en distintos backends (HDF5, NumPy, etc.).

* **Decorator ↔ Strategy**:
  Un `Solver` puede envolverse con decoradores para medir tiempos de ejecución de cada algoritmo concreto.

---

## 10. Alternativas Consideradas y Descartadas

### 1. Builder

* **Alternativa:** Constructores telescópicos con muchos parámetros.
* **Descartado porque:** Generan código ilegible, difícil de mantener y propenso a errores.

### 2. Strategy

* **Alternativa:** Uso de *flags* o `if/else` en el bucle principal.
* **Descartado porque:** Acopla la lógica de alto nivel con algoritmos específicos, reduce extensibilidad.

### 3. Template + Factory Method

* **Alternativa:** Clases concretas instanciadas directamente por el cliente.
* **Descartado porque:** Rompe la consistencia (el usuario tendría que conocer detalles internos para instanciar).

### 4. Observer

* **Alternativa:** Llamadas directas a funciones de visualización/logging dentro del bucle principal.
* **Descartado porque:** Acopla la simulación con la visualización, volviendo el código rígido.

### 5. Facade

* **Alternativa:** Acceso directo a módulos `saver`, `loader`, `parser`.
* **Descartado porque:** El usuario tendría que conocer la complejidad interna de I/O, aumentando la curva de aprendizaje.

### 6. Decorator

* **Alternativa:** Herencia múltiple para añadir funcionalidades adicionales.
* **Descartado porque:** La herencia rígida genera duplicación de código y combinaciones explosivas (ej. `LoggingSolverWithValidation`).

### 7. Adapter

* **Alternativa:** Reescribir código para cada librería externa (SciPy, PETSc, CuPy).
* **Descartado porque:** Rompe portabilidad, obliga a duplicar código cliente y reduce mantenibilidad.

---

## 11. Conclusión

El uso de patrones de diseño en `dirac_solver` asegura:

* **Modularidad:** cada componente cumple una responsabilidad clara.
* **Extensibilidad:** fácil añadir nuevos algoritmos, potenciales o backends.
* **Mantenibilidad:** menor duplicación de código y menor acoplamiento.
* **Flexibilidad:** integración fluida con librerías externas y herramientas de depuración.

Esto permite que la librería crezca de manera ordenada y soporte tanto investigación académica como aplicaciones en entornos de alto rendimiento.
