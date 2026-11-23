# Ingeniería de Requisitos para un Solucionador de la Ecuación de Dirac

> 🌐 **Idioma / Language:** **Español** | **[English](../en/requirements-engineering.md)**

La idea central es presentar el proyecto no solo como un fragmento de código, sino como una solución de ingeniería de software a un problema físico-computacional bien definido. Los requisitos se clasifican en tres áreas principales: **Funcionales**, **No Funcionales** y **De Sistema / Arquitectónicos**.

---

## 1. Introducción: Planteamiento del Problema y Alcance

- **Objetivo:** Desarrollar una librería en Python de rendimiento medio-alto y fácil de usar, `Dirac-Solver`, para simular la evolución temporal de un campo espinorial de una sola partícula $\psi(\mathbf{r}, t)$ gobernado por la ecuación de Dirac en (1+1)D, (2+1)D y (3+1)D.
- **Motivación:** Cubrir la necesidad de una herramienta accesible y rápida para estudiar fenómenos cuánticos relativistas en contextos como el grafeno, aislantes topológicos, dispersión atómica/nuclear o con fines pedagógicos. La combinación de una API en Python de alto nivel con un núcleo en C++17 de bajo nivel proporciona facilidad de uso y eficiencia computacional.
- **Ecuación Fundamental:** El solucionador calcula soluciones numéricas de la ecuación de Dirac dependiente del tiempo:

$$
i\hbar \frac{\partial \psi(\mathbf{r}, t)}{\partial t} = \hat{H} \psi(\mathbf{r}, t)
$$

donde el Hamiltoniano de Dirac $\hat{H}$ es:

$$
\hat{H} = c\boldsymbol{\alpha} \cdot \hat{\mathbf{p}} + \beta m_0 c^2 + V(\mathbf{r})I_4
$$

Aquí, $\boldsymbol{\alpha} = (\alpha_x, \alpha_y, \alpha_z)$ y $\beta$ son las matrices de Dirac estándar de $4 \times 4$, $\hat{\mathbf{p}} = -i\hbar\nabla$ es el operador de momento lineal y $V(\mathbf{r})$ es un potencial escalar.
- **Alcance:** El sistema abarca dominios espaciales 1D, 2D y 3D para una sola partícula relativista en unidades naturales ($\hbar = c = 1$). El alcance excluye efectos de muchos cuerpos y cuantización de campos (efectos de QED perturbativa).

---

## 2. Requisitos Funcionales (Lo que el Sistema *Debe Hacer*)

### A. Núcleo del Motor Físico (El Backend en C++)

- **FR-1: Esquema de Discretización:** Implementar un esquema de Diferencias Finitas en el Dominio del Tiempo (**FDTD**) para resolver la ecuación de Dirac. Se requiere un algoritmo *Leapfrog* (salto de rana) con diferencias centradas para las derivadas espaciales y temporales que garantice estabilidad numérica bajo la condición CFL y precisión de segundo orden.
- **FR-2: Condiciones de Frontera:** El usuario debe poder especificar condiciones de frontera:
  - **Condiciones de Frontera Absorbentes (`AbsorbingBoundary`):** Capas de amortiguamiento cuadrático (CAP/PML simplificado) para evitar reflexiones espurias en los bordes de la malla.
  - **Condiciones de Frontera Periódicas (`PeriodicBoundary`):** Topología toroidal en todas las dimensiones activas.
- **FR-3: Potenciales Soportados:** El solucionador debe modelar correctamente los siguientes tipos de potencial:
  - **Partícula Libre (`FreeParticle`):** $V(\mathbf{r}) = 0$.
  - **Potenciales Escalares Personalizados (`ScalarPotential`):** Funciones definibles por el usuario $V(\mathbf{r})$ para representar barreras, escalones de Klein, osciladores armónicos y pozos cuánticos.
  - **Potencial de Coulomb (`CoulombPotential`):** $V(\mathbf{r}) = -Z / (\|\mathbf{r}\| + \epsilon)$, con manejo de la singularidad en $\mathbf{r} = \mathbf{0}$ mediante el parámetro de regularización $\epsilon$.
  - **Potencial de Yukawa (`YukawaPotential`):** $V(\mathbf{r}) = -V_0 e^{-a\|\mathbf{r}\|} / \|\mathbf{r}\|$.
  - **Pozo de Potencial Infinito (`InfiniteWellPotential`):** Confinamiento en cajas 1D, 2D o 3D.
- **FR-4: Estado Inicial:** La librería debe proporcionar clases para inicializar el campo espinorial $\psi(\mathbf{r}, t=0)$ como una onda plana (`PlaneWave`) o un paquete de ondas gaussiano (`GaussianPacket`) con momento, posición, ancho espacial y polarización/superposición de espín especificados por el usuario (`ConstantSpinor`).

### B. API de Python (La Interfaz de Usuario)

- **FR-5: Configuración de la Simulación:** Los usuarios deben poder definir, configurar y validar una simulación mediante una API fluida en Python (`DiracProblemBuilder`). Esto incluye parámetros de malla (`Grid`), paso de tiempo $\Delta t$, tiempo total $T$, potencial, condición de frontera y estado inicial.
- **FR-6: Ejecución del Solucionador:** Un método claro y sencillo, `solver.run_simulation()`, que ejecute el bucle de evolución temporal, delegue el cálculo de observables y guardado de datos, e informe el progreso periódicamente.
- **FR-7: Recuperación de Datos:** Durante y después de la simulación, el usuario debe poder acceder al campo espinorial complejo completo de 4 componentes $\psi(\mathbf{r}, t)$ como un arreglo de NumPy de forma `(N, 4)` mediante `solver.get_psi()`.

### C. Salidas y Análisis (Lo que el Sistema *Debe Producir*)

- **FR-8: Cálculo de Observables (`Observables`):** La librería debe calcular y registrar series de tiempo de observables físicos clave derivados de $\psi$:
  - **Densidad de Probabilidad:** $\rho(\mathbf{r}, t) = \psi^\dagger \psi$
  - **Densidad de Carga:** $J^0(\mathbf{r}, t) = \psi^\dagger \gamma^0 \psi$
  - **Densidad de Corriente de Probabilidad:** $\mathbf{J}(\mathbf{r}, t) = c\psi^\dagger \boldsymbol{\alpha} \psi$
  - **Valores Esperados:** Posición $\langle \mathbf{r} \rangle(t)$, momento $\langle \mathbf{p} \rangle(t)$ y energía total $\langle H \rangle(t) = \int \psi^\dagger \hat{H} \psi \, dV$.
- **FR-9: Persistencia de Datos (`HDF5Storage`):** Los resultados de la simulación (snapshots del campo espinorial y series temporales de observables, junto con metadatos de la malla) deben exportarse al formato **HDF5** con soporte de *chunking* y redimensionamiento dinámico.
- **FR-10: Visualización:** La librería debe incluir herramientas basadas en `matplotlib` para generar:
  - Gráficos estáticos en 1D (curvas), 2D (mapas de calor) y 3D (nubes de puntos volumétricas con transparencia y escala logarítmica) mediante `plot_probability_density()`.
  - Animaciones temporales en formato `.gif` y `.mp4` mediante `create_animation()` y `create_animation_1d()`.

### D. Requisitos Avanzados de Rendimiento (Hoja de Ruta)

- **FR-11:** Paralelización multinúcleo con **OpenMP** en C++ para acelerar barridos en mallas 2D y 3D.
- **FR-12:** Soporte futuro para **GPU (CUDA/OpenCL)** en problemas de gran escala.
- **FR-13:** Registro de tiempos de ejecución y uso de memoria en cada simulación.

---

## 3. Requisitos No Funcionales (Qué tan *Bien* Funciona el Sistema)

- **NFR-1: Rendimiento:** El núcleo en C++ debe minimizar copias de memoria innecesarias durante los pasos de integración temporal, manteniendo tres buffers preasignados (`psi_prev_`, `psi_curr_`, `psi_next_`).
- **NFR-2: Precisión y Validación:**
  - Conservación de la probabilidad total ($\int \psi^\dagger \psi \, dV$) dentro de la tolerancia numérica del esquema Leapfrog para condiciones periódicas o estados confinados.
  - Reproducción fiel de fenómenos canónicos (*Zitterbewegung*, paradoja de Klein, dispersión de Coulomb y Yukawa) con convergencia de segundo orden $O(\Delta x^2, \Delta t^2)$.
  - Manejo robusto de errores (validación de dimensiones, advertencias de potencial por defecto y verificación de valores no finitos en pozos infinitos).
- **NFR-3: Usabilidad:**
  - API documentada con *docstrings* y cuadernos interactivos de Jupyter en `examples/es/` y `examples/en/`.
  - Instalación directa y reproducible mediante `pip install -e .` con `scikit-build-core`.
- **NFR-4: Portabilidad:** Compilación estándar en Linux, macOS y Windows con compiladores compatibles con C++17 (GCC, Clang, MSVC) y Python $\ge 3.8$.
- **NFR-5: Documentación:** Guías de usuario, manual teórico, referencia de API y generación automatizada con Doxygen en Español (`Doxyfile`) e Inglés (`Doxyfile.en`).
- **NFR-6: Comunidad y Licenciamiento:** Código publicado en GitHub bajo licencia libre **GNU GPL v3.0**, con guía de contribución y código de conducta bilingües.

---

## 4. Requisitos del Sistema y Arquitectónicos

- **SR-1: Arquitectura Híbrida de Dos Capas:**
  - **Backend (`cpp_src/`):** Librería C++17 con el integrador FDTD, álgebra de espinores `Dirac::Spinor`, potenciales y condiciones de frontera.
  - **Frontend (`dirac_solver/`):** Paquete Python que provee el patrón Builder, estados iniciales, cálculo vectorizado de observables con `np.einsum`, persistencia HDF5 y visualización.
- **SR-2: Capa de Interoperabilidad (`pybind11`):** Enlaces en `cpp_src/bindings.cpp` que exponen el módulo `_core`, incluyendo clases trampolín (`PyTimeIntegrator`, `PyBoundaryCondition`, `PyPotential`) para permitir extensibilidad polimórfica desde Python.
- **SR-3: Framework de Pruebas:** Suite de pruebas automatizadas con **pytest** (`tests/`) que valida geometría, espinores, potenciales, condiciones de frontera, observables y almacenamiento.
- **SR-4: Modularidad:** Desacoplamiento estricto entre geometría, estados iniciales, potenciales, integrador temporal, observables y almacenamiento mediante patrones de diseño (Builder, Strategy, Template Method, Facade).
- **SR-5: Versionado:** Esquema de versionado semántico (**SemVer**).

---

## 5. Requisitos de Gestión del Proyecto

- **PM-1: Cronograma por Fases:**
  - Fase 1: Solucionador 1D + API mínima y constantes en unidades naturales.
  - Fase 2: Extensión a 2D y 3D + potenciales de Coulomb, Yukawa, pozo infinito, fronteras absorbentes, HDF5 y visualización.
  - Fase 3: Documentación bilingüe completa, notebooks demostrativos y publicación.
- **PM-2: Métricas de Calidad:** Cobertura de pruebas unitarias e integración en `tests/` y benchmarks reproducibles en `examples/`.
- **PM-3: Gestión de Riesgos:** Control de inestabilidad numérica mediante `Grid.courant_limit()` y regularización de singularidades en potenciales centrales.
