# Requirements Engineering for a Dirac Equation Solver

> 🌐 **Language / Idioma:** **[Español](../es/requirements-engineering.md)** | **English**

The core objective is to structure the project not merely as a collection of scripts, but as a well-engineered software solution to a rigorous computational physics problem. The requirements are categorized into three primary areas: **Functional**, **Non-Functional**, and **System/Architectural**.

---

## 1. Introduction: Problem Statement & Scope

- **Goal:** To develop a medium-to-high-performance, user-friendly Python library, `Dirac-Solver`, for simulating the time evolution of a single-particle 4-component spinor field $\psi(\mathbf{r}, t)$ governed by the Dirac equation in (1+1)D, (2+1)D, and (3+1)D.
- **Motivation:** Address the need for an accessible, fast numerical tool for studying relativistic quantum phenomena in contexts such as graphene, topological insulators, atomic/nuclear scattering, and advanced physics education. Combining a high-level Python API with a low-level C++17 core provides both ease of use and computational efficiency.
- **Core Equation:** The solver computes numerical solutions to the time-dependent Dirac equation:

$$
i\hbar \frac{\partial \psi(\mathbf{r}, t)}{\partial t} = \hat{H} \psi(\mathbf{r}, t)
$$

where the Dirac Hamiltonian $\hat{H}$ is:

$$
\hat{H} = c\boldsymbol{\alpha} \cdot \hat{\mathbf{p}} + \beta m_0 c^2 + V(\mathbf{r})I_4
$$

Here, $\boldsymbol{\alpha} = (\alpha_x, \alpha_y, \alpha_z)$ and $\beta$ are the standard $4 \times 4$ Dirac matrices, $\hat{\mathbf{p}} = -i\hbar\nabla$ is the momentum operator, and $V(\mathbf{r})$ is a scalar potential.
- **Scope:** The system supports 1D, 2D, and 3D spatial grids for a single relativistic spin-$1/2$ particle in natural units ($\hbar = c = 1$). The scope explicitly excludes many-body interactions and perturbative QED field quantization.

---

## 2. Functional Requirements (What the System *Must Do*)

### A. Physics Engine Core (The C++ Backend)

- **FR-1: Discretization Scheme:** Implement a Finite-Difference Time-Domain (**FDTD**) scheme to solve the Dirac equation. A centered-difference Leapfrog algorithm for spatial and temporal derivatives is required to guarantee numerical stability under the CFL condition and second-order accuracy.
- **FR-2: Boundary Conditions:** The user must be able to specify boundary conditions:
  - **Absorbing Boundary Conditions (`AbsorbingBoundary`):** Quadratic damping layers (simplified CAP/PML) to prevent spurious reflections from grid edges.
  - **Periodic Boundary Conditions (`PeriodicBoundary`):** Toroidal wrap-around topology across all active dimensions.
- **FR-3: Supported Potentials:** The solver must accurately model the following potential types:
  - **Free Particle (`FreeParticle`):** $V(\mathbf{r}) = 0$.
  - **User-Defined Scalar Potentials (`ScalarPotential`):** Arbitrary functions $V(\mathbf{r})$ representing barriers, Klein steps, harmonic traps, and quantum wells.
  - **Coulomb Potential (`CoulombPotential`):** $V(\mathbf{r}) = -Z / (\|\mathbf{r}\| + \epsilon)$, handling the singularity at $\mathbf{r} = \mathbf{0}$ via regularization parameter $\epsilon$.
  - **Yukawa Potential (`YukawaPotential`):** $V(\mathbf{r}) = -V_0 e^{-a\|\mathbf{r}\|} / \|\mathbf{r}\|$.
  - **Infinite Potential Well (`InfiniteWellPotential`):** Box confinement in 1D, 2D, or 3D.
- **FR-4: Initial State:** The library must provide classes to initialize the spinor field $\psi(\mathbf{r}, t=0)$ as a plane wave (`PlaneWave`) or Gaussian wavepacket (`GaussianPacket`) with user-specified momentum, center, spatial width, and arbitrary 3D spin polarization/superposition (`ConstantSpinor`).

### B. Python API (The User Interface)

- **FR-5: Simulation Setup:** Users must be able to define, configure, and validate a simulation through a fluent Pythonic API (`DiracProblemBuilder`), including grid parameters (`Grid`), time step $\Delta t$, total simulation time $T$, potential, boundary condition, and initial state.
- **FR-6: Solver Execution:** A clear and simple method, `solver.run_simulation()`, to execute the time-evolution loop, delegate observable computation and data storage, and report progress feedback.
- **FR-7: Data Retrieval:** During and post-simulation, the user must be able to access the full 4-component complex spinor field $\psi(\mathbf{r}, t)$ as an `(N, 4)` NumPy array via `solver.get_psi()`.

### C. Outputs and Analysis (What the System *Must Produce*)

- **FR-8: Observable Calculations (`Observables`):** The library must compute and record time series of key physical observables derived from $\psi$:
  - **Probability Density:** $\rho(\mathbf{r}, t) = \psi^\dagger \psi$
  - **Charge Density:** $J^0(\mathbf{r}, t) = \psi^\dagger \gamma^0 \psi$
  - **Probability Current Density:** $\mathbf{J}(\mathbf{r}, t) = c\psi^\dagger \boldsymbol{\alpha} \psi$
  - **Expectation Values:** Expected position $\langle \mathbf{r} \rangle(t)$, expected momentum $\langle \mathbf{p} \rangle(t)$, and expected energy $\langle H \rangle(t) = \int \psi^\dagger \hat{H} \psi \, dV$.
- **FR-9: Data Persistence (`HDF5Storage`):** Simulation results (spinor field snapshots, observable time series, and grid metadata attributes) must be exportable to the **HDF5** format with chunking and dynamic dataset resizing.
- **FR-10: Visualization:** The library must include `matplotlib`-based visualization utilities to generate:
  - Static plots in 1D (line plots), 2D (heatmaps), and 3D (volumetric subsampled scatter plots with density-dependent opacity and logarithmic sizing) via `plot_probability_density()`.
  - Time-evolution animations in `.gif` and `.mp4` formats via `create_animation()` and `create_animation_1d()`.

### D. Advanced Performance Requirements (Roadmap)

- **FR-11:** Multi-core parallelization with **OpenMP** in C++ to accelerate 2D and 3D grid sweeps.
- **FR-12:** Future **GPU (CUDA/OpenCL)** support for large-scale 3D problems.
- **FR-13:** Execution time and memory footprint logging for each simulation run.

---

## 3. Non-Functional Requirements (How *Well* the System Performs)

- **NFR-1: Performance:** The C++ core must avoid unnecessary memory allocations during time-stepping by operating on three pre-allocated spinor buffers (`psi_prev_`, `psi_curr_`, `psi_next_`).
- **NFR-2: Accuracy & Validation:**
  - Conservation of total probability ($\int \psi^\dagger \psi \, dV$) within the numerical tolerance of the Leapfrog scheme for periodic or confined systems.
  - Agreement with canonical relativistic phenomena (*Zitterbewegung*, Klein tunneling, Coulomb and Yukawa scattering) with second-order error convergence $O(\Delta x^2, \Delta t^2)$.
  - Robust error handling (dimension validation, default potential warnings, and safe handling of infinite potential values).
- **NFR-3: Usability:**
  - Comprehensive docstrings and interactive Jupyter Notebook tutorials in `examples/en/` and `examples/es/`.
  - Straightforward installation via `pip install -e .` powered by `scikit-build-core`.
- **NFR-4: Portability:** Must compile and run on Linux, macOS, and Windows with standard C++17 compilers (GCC, Clang, MSVC) and Python $\ge 3.8$.
- **NFR-5: Documentation:** User guides, theoretical manual, API reference, and automated Doxygen configs in English (`Doxyfile.en`) and Spanish (`Doxyfile`).
- **NFR-6: Community & Licensing:** Published on GitHub under the **GNU GPL v3.0** license, with bilingual contribution guidelines and Code of Conduct.

---

## 4. System & Architectural Requirements

- **SR-1: Two-Layer Hybrid Architecture:**
  - **Backend (`cpp_src/`):** C++17 library containing the FDTD Leapfrog integrator, `Dirac::Spinor` algebra, potentials, and boundary conditions.
  - **Frontend (`dirac_solver/`):** Python package providing the Builder API, initial states, vectorized observable calculations (`np.einsum`), HDF5 storage, and visualization.
- **SR-2: Interoperability Layer (`pybind11`):** Bindings in `cpp_src/bindings.cpp` exposing the `_core` module, including trampoline classes (`PyTimeIntegrator`, `PyBoundaryCondition`, `PyPotential`) for polymorphic extensibility from Python.
- **SR-3: Testing Framework:** Automated test suite using **pytest** (`tests/`) covering geometry, spinor initialization, potentials, boundary conditions, time integration, observables, and HDF5 I/O.
- **SR-4: Modularity:** Strict separation of concerns across geometry, initial states, potentials, time integrator, observables, and storage via canonical design patterns (Builder, Strategy, Template Method, Facade).
- **SR-5: Versioning:** Semantic Versioning (**SemVer**).

---

## 5. Project Management Requirements

- **PM-1: Phased Timeline:**
  - Phase 1: 1D solver + core API and natural units standardization.
  - Phase 2: Extension to 2D and 3D + Coulomb, Yukawa, Infinite Well potentials, absorbing boundaries, HDF5 persistence, and visualization.
  - Phase 3: Full bilingual documentation suite, example notebooks, and public release.
- **PM-2: Quality Metrics:** Unit and integration test coverage in `tests/` and reproducible physics notebooks in `examples/`.
- **PM-3: Risk Mitigation:** Courant stability enforcement via `Grid.courant_limit()` and singularity regularization in central potentials.
