## Presentation Structure: Requirements Engineering for a Dirac Equation Solver

The core idea is to present the project not just as a piece of code, but as a
solution to a well-defined problem. Categorising the requirements into three
main areas: **Functional**, **Non-Functional**, and **System/Architectural**.

---

### **1. Introduction: Problem Statement & Scope**

- **Goal:** To develop a medium-high-performance, user-friendly Python library,
`Dirac-Solver` , for simulating the time evolution of a single-particle spinor
field $\psi(t, \vec{r})$ governed by the (3+1)D Dirac equation.
- **Motivation:** Address the need for an accessible tool for studying
relativistic quantum phenomena in contexts like graphene, topological
insulators, or for purely pedagogical purposes. The combination of a high-level
Python API with a low-level C++ core aims to provide both ease of use and
computational efficiency.
- **Core Equation:** The solver will find numerical solutions to the time-dependent Dirac equation:
$$i\hbar \frac{\partial \psi(\vec{r}, t)}{\partial t} = \hat{H} \psi(\vec{r}, t)$$
where the Dirac Hamiltonian $\hat{H}$ is:
$$\hat{H} = c\vec{\alpha} \cdot \hat{\vec{p}} + \beta m_0 c^2 + V(\vec{r})I_4$$
Here, $\vec{\alpha}$ and $\beta$ are the standard $4 \times 4$ Dirac matrices,
$\hat{\vec{p}} = -i\hbar\nabla$ is the momentum operator, and $V(\vec{r})$ is a
scalar potential.
- **Scope:** The initial release will focus on 1D and 2D systems for a single,
non-interacting particle. The scope explicitly excludes  many-body effects  and
field quantization (QED effects).

---

### **2. Functional Requirements (What the System *Must Do*)**

#### **A. Physics Engine Core (The C++ Backend)**

- **FR-1: Discretization Scheme:** Implement a Finite-Difference Time-Domain
(FDTD) scheme to solve the Dirac equation. A staggered-leapfrog algorithm for
the spatial and temporal derivatives is required to ensure numerical stability
and second-order accuracy.
- **FR-2: Boundary Conditions:** The user must be able to specify boundary
conditions. At a minimum, **Perfectly Matched Layers (PML)** or **Absorbing
Boundary Conditions (ABC)** must be implemented to prevent spurious reflections
from the grid edges. Simple periodic boundary conditions should also be an
option.
- **FR-3: Supported Potentials:** The solver must correctly model the following potential types:
  - **Free Particle:** $V(\vec{r}) = 0$.
  - **Scalar Potentials:** User-definable functions $V(\vec{r})$ representing
  barriers, steps, and quantum wells.
  - **Coulomb Potential:** $V(r) = -\frac{Z e^2}{4\pi\epsilon_0 r}$, with
  appropriate handling of the singularity at $r=0$ through regularization or a
  specialized grid.
- **FR-4: Initial State:** The library must provide helper functions to
initialize the spinor field $\psi(\vec{r}, t=0)$ as a Gaussian wavepacket with
user-specified momentum, position, and spin polarization.

#### **B. Python API (The User Interface)**

- **FR-5: Simulation Setup:** Users must be able to define, configure, and run a
simulation through a "Pythonic" API. This includes setting grid parameters
(dimensions, spacing $dx, dy, dz$), time step $dt$, total simulation time,
choice of potential, and initial conditions.
- **FR-6: Solver Execution:** A clear and simple method, e.g., `solver.run()`,
to execute the time-evolution loop. The method should provide progress feedback
for long simulations.
- **FR-7: Data Retrieval:** Post-simulation, the user must be able to access the
full 4-component complex spinor field $\psi(\vec{r}, t)$ at specified time
intervals.

#### **C. Outputs and Analysis (What the System *Must Produce*)**

- **FR-8: Observable Calculations:** The library must compute and provide access
to key physical observables derived from $\psi$:
  - **Probability Density:** $\rho(\vec{r}, t) = \psi^\dagger \psi$
  - **Probability Current Density:** $\vec{j}(\vec{r}, t) = c\psi^\dagger
  \vec{\alpha} \psi$
  - **Expectation Value of Energy:** $\langle E \rangle(t) = \int \psi^\dagger
  \hat{H} \psi \, d^V$
- **FR-9: Data Persistence:** Simulation results (the final spinor field and
time-series of observables) must be exportable to a standard, high-performance
file format like **HDF5** or a simple set of NumPy binary files (`.npy`).
- **FR-10: Visualization:** The library must include a visualization module that
uses `matplotlib` to generate:
  - 2D plots (e.g., probability density $\rho(x, t)$ for a 1D simulation).
  - Animations of the time evolution of $\rho(x,y,t)$.
  - Vector field plots for the probability current density $\vec{j}$.

#### **D Advanced Performance Requirements**

- **FR-11:** Parallelization with **OpenMP** in C++ to accelerate simulations.

- **FR-12:** Future support for **GPU (CUDA/OpenCL)** for large-scale problems.

- **FR-13:** Logging of memory usage and execution time for each simulation.

---

### **3. Non-Functional Requirements (How *Well* the System Performs)**

- **NFR-1: Performance:** For a benchmark 2D problem (e.g., a $512 \times 512$
grid), the C++ core must achieve a target number of timesteps per second on
standard consumer hardware. This defines the "medium-high-performance" goal.
- **NFR-2: Accuracy & Validation:** The solver's accuracy must be verifiable.
  - It must demonstrate conservation of total probability ($\int \psi^\dagger
  \psi \, dV = 1$) to within a specified tolerance for bound states.
  - Results for canonical problems (e.g., Zitterbewegung for a free particle,
  Klein tunneling for a potential step) must agree with analytical solutions to
  within a predictable error margin related to the grid spacing ($O(\Delta x^2,
  \Delta t^2)$).
  - Robust error handling (e.g., invalid user inputs, numerical divergences).
- **NFR-3: Usability:**
  - The API must be well-documented with docstrings and a tutorial in the form
  of a Jupyter Notebook.
  - Installation must be straightforward via `pip`.
- **NFR-4: Portability:** The software must compile and run on major operating
systems (Linux, macOS, and Windows) with common C++ compilers (GCC, Clang, MSVC)
and supported Python versions (e.g., 3.8+).
-  **NFR-5:Documentation:** User guides (Getting Started, Advanced Tutorial),
Technical manual of the C++ backend (including class diagram and FDTD algorithm
flow) and use Doxygen for documentation.
- **NFR-6: Community:** Code published on **GitHub** with a clear license (e.g.,
MIT or GPL). Issues and pull requests managed with priority labels.

---

### **4. System & Architectural Requirements**

- **SR-1: Hybrid Architecture:** A two-layer architecture is required:
  - **Backend:** A C++ library containing the FDTD algorithm, data structures
  for the fields, and physics computations. This ensures maximum performance.
  - **Frontend:** A Python wrapper that provides the high-level API for users
  with examples.
- **SR-2: Interoperability Layer:** **pybind11** (or a similar tool like
SWIG/CFFI) will be used to create the bindings between C++ and Python. This
layer must efficiently handle the transfer of NumPy arrays to and from the C++
backend without significant memory copying overhead.
- **SR-3: Testing Framework:** A comprehensive testing suite is required, using
**Google Test** for the C++ backend and **pytest** for the Python API and
integration tests. Continuous Integration (CI) via GitHub Actions should be set
up to run these tests automatically.
- **SR-4: Modularity:** Each module (potentials, boundary conditions,
visualization) must be independent and loosely coupled.
- **SR-5: Versioning:** Define a semantic versioning scheme (**SemVer**) to plan
future releases.

---

### **5. Project Management Requirements**

- **PM-1:** Initial timeline:
  - Phase 1: 1D solver + minimal API.
  - Phase 2: Extension to 2D + visualization , magnetic vector potentials implementation.
  - Phase 3: Documentation and public release.

- **PM-2:** Quality metrics (test coverage percentage, reproducible benchmarks).

- **PM-3:** Risk plan (e.g., numerical instability, memory bottlenecks).
