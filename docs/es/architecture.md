# Arquitectura del Software y Patrones de Diseño

> **Idioma / Language:** **Español** | [English](../en/architecture.md)

`Dirac-Solver` utiliza una **arquitectura híbrida de dos capas** para combinar la productividad y el ecosistema científico de Python con el rendimiento nativo de C++17.

---

## 1. Visión General de Capas

```text
+-------------------------------------------------------------------+
|               FRONTEND PYTHON (dirac_solver/)                     |
|  - geometry.py       : Grid (mallas 1D/2D/3D, gradiente, CFL)     |
|  - initial_state.py  : ConstantSpinor, GaussianPacket, PlaneWave  |
|  - potentials.py     : Envoltorios Python para potenciales        |
|  - core.py           : DiracProblemBuilder, SimulationProblem,    |
|                        DiracSolver                                |
|  - observables.py    : Observables locales y valores esperados    |
|  - storage.py        : HDF5Storage, create_animation(_1d)         |
+---------------------------------+---------------------------------+
                                  | pybind11 (_core)
+---------------------------------v---------------------------------+
|                 BACKEND C++17 (cpp_src/)                          |
|  - DiracMatrices.h   : struct Spinor, Matrix4x4, beta, alpha_k    |
|  - Grid.h            : Grid C++ con indexación row-major C-order  |
|  - TimeIntegrator.h  : TimeIntegrator, FDTDLeapfrogIntegrator     |
|  - potentials/       : Potential + 5 implementaciones concretas   |
|  - boundaries/       : BoundaryCondition + Periodic & Absorbing   |
|  - bindings.cpp      : Exportación PYBIND11_MODULE(_core, m)      |
+-------------------------------------------------------------------+
```

---

## 2. Patrones de Diseño Aplicados

### Builder (`DiracProblemBuilder` → `SimulationProblem`)
Permite ensamblar paso a paso la configuración física (malla, estado inicial, potencial, frontera y parámetros temporales) con una interfaz fluida y validación centralizada en `.build()`.

### Strategy (`TimeIntegrator` y `BoundaryCondition`)
Desacopla el bucle de simulación del algoritmo numérico específico y del tratamiento de los bordes. Gracias a las clases trampolín (`PyTimeIntegrator`, `PyBoundaryCondition`) en `bindings.cpp`, incluso es posible definir nuevas estrategias heredando en Python.

### Template Method (`Potential` e `InitialState`)
Define la interfaz común (`evaluate(r)`, `evaluate_on_grid(grid)`) en las clases base abstractas, delegando la física particular a cada subclase concreta (`FreeParticle`, `ScalarPotential`, `CoulombPotential`, `YukawaPotential`, `InfiniteWellPotential`, `GaussianPacket`, `PlaneWave`).

### Facade (`HDF5Storage`)
Encapsula el manejo de `h5py` (datasets extensibles con *chunking*, almacenamiento de atributos de malla, escritura de grupos de observables y manejo de contexto `with`) tras una API minimalista.

---

## 3. Convención de Memoria e Indexación

Tanto la capa Python (`np.meshgrid(..., indexing="ij")`) como la capa C++ (`Grid.h`, `TimeIntegrator.h`, `AbsorbingBoundary.h`) utilizan **ordenamiento *row-major* (estilo C)**:
- **1D:** `idx = ix`
- **2D:** `idx = ix * ny + iy`
- **3D:** `idx = (ix * ny + iy) * nz + iz`

El campo espinorial se transfiere entre NumPy y C++ como un arreglo contiguo de forma `(N_total, 4)` y tipo `complex128` (`std::complex<double>`).
