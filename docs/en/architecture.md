# Software Architecture and Design Patterns

> **Language / Idioma:** [Español](../es/architecture.md) | **English**

`Dirac-Solver` adopts a **two-layer hybrid architecture** combining Python's scientific usability with native C++17 numerical performance.

---

## 1. Architectural Layer Overview

```text
+-------------------------------------------------------------------+
|               PYTHON FRONTEND (dirac_solver/)                     |
|  - geometry.py       : Grid (1D/2D/3D meshes, gradient, CFL)      |
|  - initial_state.py  : ConstantSpinor, GaussianPacket, PlaneWave  |
|  - potentials.py     : High-level Python potential wrappers       |
|  - core.py           : DiracProblemBuilder, SimulationProblem,    |
|                        DiracSolver                                |
|  - observables.py    : Local observables & expectation values     |
|  - storage.py        : HDF5Storage, create_animation(_1d)         |
+---------------------------------+---------------------------------+
                                  | pybind11 (_core)
+---------------------------------v---------------------------------+
|                 C++17 BACKEND (cpp_src/)                          |
|  - DiracMatrices.h   : struct Spinor, Matrix4x4, beta, alpha_k    |
|  - Grid.h            : C++ Grid with row-major C-order indexing   |
|  - TimeIntegrator.h  : TimeIntegrator, FDTDLeapfrogIntegrator     |
|  - potentials/       : Potential + 5 concrete implementations     |
|  - boundaries/       : BoundaryCondition + Periodic & Absorbing   |
|  - bindings.cpp      : PYBIND11_MODULE(_core, m) definitions      |
+-------------------------------------------------------------------+
```

---

## 2. Implemented Design Patterns

### Builder (`DiracProblemBuilder` → `SimulationProblem`)
Allows fluent, step-by-step assembly of a simulation (grid, initial state, potential, boundary condition, and time parameters) with centralized validation in `.build()`.

### Strategy (`TimeIntegrator` & `BoundaryCondition`)
Decouples the time-evolution loop from specific numerical integrators and boundary-handling schemes. Through `pybind11` trampoline classes (`PyTimeIntegrator`, `PyBoundaryCondition`) in `bindings.cpp`, users can even subclass C++ interfaces directly from Python.

### Template Method (`Potential` & `InitialState`)
Defines the common interface (`evaluate(r)`, `evaluate_on_grid(grid)`) in abstract base classes while delegating physics-specific calculations to concrete subclasses (`FreeParticle`, `ScalarPotential`, `CoulombPotential`, `YukawaPotential`, `InfiniteWellPotential`, `GaussianPacket`, `PlaneWave`).

### Facade (`HDF5Storage`)
Encapsulates `h5py` dataset creation, chunking, dynamic axis resizing, metadata attributes, and context-manager cleanup (`with`) behind a clean API.

---

## 3. Memory Layout and Indexing Convention

Both Python (`np.meshgrid(..., indexing="ij")`) and C++ (`Grid.h`, `TimeIntegrator.h`, `AbsorbingBoundary.h`) use **row-major (C-order)** indexing:
- **1D:** `idx = ix`
- **2D:** `idx = ix * ny + iy`
- **3D:** `idx = (ix * ny + iy) * nz + iz`

The spinor field crosses the Python/C++ boundary as a contiguous `(N_total, 4)` array of `complex128` (`std::complex<double>`).
