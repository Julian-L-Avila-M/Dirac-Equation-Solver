# Project Structure of `dirac_solver`

> 🌐 **Language / Idioma:** **[Español](../es/structure.md)** | **English**

This document describes both the modular architectural layout of the `dirac_solver` project (inspired by scientific computing libraries such as **DeepXDE**) and its actual implementation in the hybrid **Python + C++17 (`pybind11`)** codebase.

---

## 1. Actual Repository Directory Structure

```text
Dirac-Solver/
├── pyproject.toml                # Build configuration (scikit-build-core + pybind11)
├── README.md                     # Primary README in Spanish (with language switcher)
├── README.es.md                  # Spanish README
├── README.en.md                  # English README
├── CONTRIBUTING.md               # Contribution guidelines (Spanish)
├── CONTRIBUTING.en.md            # Contribution guidelines (English)
├── CODE_OF_CONDUCT.md            # Code of Conduct (English)
├── CODE_OF_CONDUCT.es.md         # Code of Conduct (Spanish)
├── Doxyfile                      # Doxygen configuration (Spanish)
├── Doxyfile.en                   # Doxygen configuration (English)
├── LICENSE                       # GNU General Public License v3.0
│
├── cpp_src/                      # High-performance C++17 numerical core
│   ├── CMakeLists.txt            # CMake build script for the _core extension module
│   ├── bindings.cpp              # pybind11 bindings exposing _core to Python
│   ├── DiracMatrices.h           # Dirac::Spinor struct and 4x4 matrices (I4, β, α_x, α_y, α_z)
│   ├── Grid.h                    # C++ 1D/2D/3D spatial grid and flat-index-to-coordinate mapping
│   ├── TimeIntegrator.h          # TimeIntegrator interface and FDTDLeapfrogIntegrator engine
│   ├── boundaries/               # Boundary condition strategies
│   │   ├── BoundaryCondition.h   # Abstract base class BoundaryCondition
│   │   ├── PeriodicBoundary.h    # Periodic boundary condition
│   │   └── AbsorbingBoundary.h   # Absorbing boundary with quadratic damping profile
│   └── potentials/               # Polymorphic hierarchy of scalar potentials
│       ├── Potential.h           # Abstract base class Potential
│       ├── FreeParticle.h        # Zero potential V(r) = 0
│       ├── CoulombPotential.h    # Regularized Coulomb potential -Z / (|r| + ε)
│       ├── YukawaPotential.h     # Yukawa potential -V_0 exp(-a|r|) / |r|
│       ├── InfiniteWellPotential.h # 1D/2D/3D infinite potential well
│       └── ScalarPotential.h     # Arbitrary user-defined scalar potential via std::function
│
├── dirac_solver/                 # High-level Python API package
│   ├── __init__.py               # Natural units constants (ħ=c=m=e=1) and Pauli matrices
│   ├── core.py                   # SimulationProblem, DiracProblemBuilder, and DiracSolver
│   ├── geometry.py               # Grid class (coordinates, cell volume, gradient, CFL limit)
│   ├── initial_state.py          # InitialState, ConstantSpinor, GaussianPacket, and PlaneWave
│   ├── potentials.py             # Python wrapper classes for _core potentials
│   ├── observables.py            # Observables class (ρ, J⁰, J, <r>, <p>, <H>)
│   ├── storage.py                # HDF5Storage, create_animation, and create_animation_1d
│   └── DiracMatrices.py          # 4x4 Dirac matrices (γ^μ, α^k, β, γ⁵) in NumPy
│
├── docs/                         # Bilingual technical documentation
│   ├── en/                       # English documentation (index, theory, architecture, api, guide)
│   └── es/                       # Spanish documentation (index, theory, architecture, api, guide)
│
├── examples/                     # Interactive Jupyter Notebooks
│   ├── en/                       # Annotated notebooks in English
│   └── es/                       # Annotated notebooks in Spanish
│
├── planning/                     # Software engineering and architecture documents
│   ├── en/                       # Planning and design documents in English
│   └── es/                       # Planning and design documents in Spanish
│
└── tests/                        # Automated unit and integration test suite (pytest)
    ├── test_integration.py       # Python <-> C++ integration tests
    ├── test_physics.py           # Physical conservation and analytical phase evolution tests
    └── test_potentials.py        # Unit tests for built-in potentials
```

---

## 2. Core Module Descriptions

### Project Root
- **`pyproject.toml`**: Configures the build system via `scikit-build-core` and `pybind11`, package metadata, authors, `GPL-3.0-or-later` license, and runtime dependencies (`numpy`, `matplotlib`, `h5py`).
- **`README.md` / `README.es.md` / `README.en.md`**: Bilingual entry point featuring installation instructions, project overview, and a working quickstart example.
- **`Doxyfile` / `Doxyfile.en`**: Doxygen configuration files for building HTML and LaTeX reference manuals in Spanish and English.

### C++ Core (`cpp_src/`)
1. **`DiracMatrices.h`**: Defines the `Dirac::Spinor` struct (4-component complex array with overloaded arithmetic operators) and the standard-representation Dirac matrices $\beta, \alpha_x, \alpha_y, \alpha_z$.
2. **`Grid.h`**: Manages 1D, 2D, and 3D discrete Cartesian grids centered at the origin and maps flat indices $i \in [0, N-1]$ to spatial coordinates $(x, y, z)$.
3. **`TimeIntegrator.h`**: Implements `FDTDLeapfrogIntegrator`, maintaining three time buffers ($\psi_{\text{prev}}, \psi_{\text{curr}}, \psi_{\text{next}}$), bootstrapping the backward step via backward Euler, and evaluating the local Dirac Hamiltonian using centered finite differences.
4. **`potentials/` and `boundaries/`**: Polymorphic class hierarchies for scalar potentials and boundary conditions.
5. **`bindings.cpp`**: Exposes all C++ classes under the compiled `dirac_solver._core` extension module.

### Python API (`dirac_solver/`)
1. **`geometry.py`**: Implements the Python `Grid` class, including coordinate mesh generation (`np.meshgrid`), second-order spatial gradients (`np.gradient`), and the Courant stability limit (`courant_limit()`).
2. **`initial_state.py`**: Constructs Pauli spinors oriented along arbitrary spherical angles $(\theta, \phi)$, constant free-particle bispinors $u(\mathbf{p}_0)$, plane waves, and Gaussian wavepackets.
3. **`core.py`**: Implements `SimulationProblem`, `DiracProblemBuilder`, and `DiracSolver`.
4. **`observables.py`**: Computes local densities ($\rho, J^0, \mathbf{J}$) and integrated expectation values ($\langle \mathbf{r} \rangle, \langle \mathbf{p} \rangle, \langle H \rangle$) using fast `np.einsum` contractions.
5. **`storage.py`**: Manages incremental HDF5 snapshot persistence (`HDF5Storage`) and MP4/GIF animation rendering (`create_animation`, `create_animation_1d`).
