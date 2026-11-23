# Dirac-Solver: A Numerical Dirac Equation Solver in 1D, 2D, and 3D

> 🌐 **Language / Idioma:** **[Español](README.md)** ([`README.es.md`](README.es.md)) | **English**

This project implements a high-performance numerical solver for the time-dependent Dirac equation in **1D, 2D, and 3D**. The computational core is written in **C++17** and exposed to **Python** via `pybind11`, combining fast numerical time-stepping with a clean, modular, and expressive scientific Python interface.

The solved equation in natural units ($\hbar = c = 1$) is:

$$
(i\gamma^\mu \partial_\mu - m)\psi = 0 \quad \Longleftrightarrow \quad i\frac{\partial \psi(\mathbf{r}, t)}{\partial t} = \left( -i\boldsymbol{\alpha}\cdot\nabla + \beta m + V(\mathbf{r})I_4 \right)\psi(\mathbf{r}, t)
$$

where the 4-component Dirac bispinor $\psi(\mathbf{r}, t) \in \mathbb{C}^4$ is a complex column vector. The library implements explicit Finite-Difference Time-Domain (**FDTD Leapfrog**) integration, analytical and user-defined scalar potentials, pluggable boundary conditions, relativistic observable computations, and **HDF5** data persistence.

---

## Key Features

- **C++17 FDTD Leapfrog Engine:** Explicit second-order accurate spatial and temporal integration for 1D, 2D, and 3D Cartesian grids.
- **Flexible Spatial Geometry (`Grid`):** Unified support for 1D, 2D, and 3D domains with automatic Courant-Friedrichs-Lewy (CFL) stability limit calculation.
- **C++ ↔ Python Interoperability (`pybind11`):** Direct transfer of complex `(N, 4)` NumPy arrays (`complex128`) and Python callables into the C++ engine.
- **Relativistic Initial States (`InitialState`):** General Pauli spinor construction along arbitrary spherical polarization axes (`ConstantSpinor`), plane waves (`PlaneWave`), and localized Gaussian wavepackets (`GaussianPacket`).
- **Configurable Potentials (`Potential`):** Free particle (`FreeParticle`), regularized Coulomb (`CoulombPotential`), Yukawa (`YukawaPotential`), infinite potential well (`InfiniteWellPotential`), and arbitrary user-defined functions (`ScalarPotential`).
- **Modular Boundary Conditions (`BoundaryCondition`):** Periodic boundaries (`PeriodicBoundary`) and quadratic damping Complex Absorbing Potential / PML-like layers (`AbsorbingBoundary`).
- **Observables & Storage (`Observables`, `HDF5Storage`):** Automated calculation of probability density ($\rho$), charge density ($J^0$), probability current ($\mathbf{J}$), and expectation values ($\langle \mathbf{r} \rangle$, $\langle \mathbf{p} \rangle$, $\langle H \rangle$), with HDF5 export and GIF/MP4 animations (`create_animation`, `create_animation_1d`).
- **Bilingual Documentation & Examples:** Full documentation suite, Jupyter notebooks, and architectural planning documents in both **English** and **Spanish**.

---

## Project Structure

```text
Dirac-Solver/
├── cpp_src/                  # C++17 numerical core and pybind11 bindings
│   ├── bindings.cpp          # Definition of the _core C++ extension module
│   ├── CMakeLists.txt        # CMake build configuration
│   ├── DiracMatrices.h       # Spinor algebra and 4x4 α, β Dirac matrices
│   ├── Grid.h                # C++ 1D/2D/3D spatial grid
│   ├── TimeIntegrator.h      # TimeIntegrator interface and FDTDLeapfrogIntegrator
│   ├── boundaries/           # BoundaryCondition, PeriodicBoundary, AbsorbingBoundary
│   └── potentials/           # Potential, FreeParticle, Coulomb, Yukawa, InfiniteWell, Scalar
├── dirac_solver/             # Main Python API package
│   ├── __init__.py           # Natural constants, Pauli matrices, and package exports
│   ├── core.py               # SimulationProblem, DiracProblemBuilder, DiracSolver
│   ├── geometry.py           # Grid class, Courant limit, and spatial gradient calculation
│   ├── initial_state.py      # InitialState, ConstantSpinor, GaussianPacket, PlaneWave
│   ├── potentials.py         # Python wrappers for scalar potentials
│   ├── observables.py        # Observables class (ρ, J⁰, J, <r>, <p>, <H>)
│   ├── storage.py            # HDF5Storage, create_animation, create_animation_1d
│   └── DiracMatrices.py      # Dirac matrices (γ^μ, α^k, β, γ⁵) in NumPy
├── docs/                     # Project documentation
│   ├── en/                   # Technical reference and user guides in English
│   └── es/                   # Technical reference and user guides in Spanish
├── examples/                 # Jupyter Notebooks and demonstration scripts
│   ├── en/                   # Interactive Jupyter notebooks in English
│   └── es/                   # Interactive Jupyter notebooks in Spanish
├── planning/                 # Requirements engineering and architecture design docs
│   ├── en/                   # Design and planning documents in English
│   └── es/                   # Design and planning documents in Spanish
├── tests/                    # Unit and integration test suite (pytest)
├── save_data/                # Default output directory for HDF5 files and animations
├── Doxyfile                  # Doxygen configuration (Spanish)
├── Doxyfile.en               # Doxygen configuration (English)
├── CONTRIBUTING.md           # Contribution guide (Spanish)
├── CONTRIBUTING.en.md        # Contribution guide (English)
├── CODE_OF_CONDUCT.md        # Code of Conduct (English)
├── CODE_OF_CONDUCT.es.md     # Code of Conduct (Spanish)
├── LICENSE                   # GNU GPL v3.0 License
└── pyproject.toml            # Python packaging configuration (scikit-build-core)
```

---

## Installation

### Prerequisites

- **Python** $\ge 3.8$
- **C++17** compiler (`g++`, `clang++`, or `MSVC`)
- **CMake** $\ge 3.15$ and `ninja` / `make`
- Python dependencies: `numpy`, `matplotlib`, `h5py`, `pybind11`, `scikit-build-core`

### Editable Local Installation (Recommended)

```bash
pip install -e .
```

### Manual C++ Core Compilation via CMake

If you want to compile the `_core` C++ extension manually:

```bash
mkdir -p cpp_build
cd cpp_build
cmake ../cpp_src
make -j
```

---

## Basic Usage

Below is a complete example using the actual `dirac_solver` API (`DiracProblemBuilder`, time evolution via `DiracSolver`, physical `Observables`, and `HDF5Storage` persistence):

```python
import dirac_solver as ds
from dirac_solver import _core
from dirac_solver.storage import create_animation_1d

# 1. Define a 1D spatial grid (400 points, dx = 0.05)
grid = ds.Grid(shape=(400,), spacing=0.05)

# Pick a safe time step based on the CFL condition
dt = 0.5 * grid.courant_limit()
total_time = 5.0

# 2. Configure the initial state: Gaussian wavepacket with spin +z
spinor = ds.ConstantSpinor(
    spin_polarization=[0, 0, 1],
    spin_superposition=[1.0 + 0j, 0.0 + 0j],
    momentum=[2.0, 0.0, 0.0],
    mass=ds.electron_mass,
)
initial_state = ds.GaussianPacket(
    constant_spinor=spinor,
    center=[-3.0, 0.0, 0.0],
    spatial_width=0.8,
)

# 3. Define a potential (e.g. regularized Coulomb or custom potential)
potential = ds.CoulombPotential(Z=1.0, epsilon=0.1)

# Boundary condition using an explicit _core object or string ("absorbing" / "periodic")
boundary = _core.AbsorbingBoundary(strength=0.3, width=20)

# 4. Build the simulation problem via DiracProblemBuilder
problem = (
    ds.DiracProblemBuilder()
    .set_grid(grid)
    .set_initial_state(initial_state)
    .set_potential(potential)
    .set_boundary_condition(boundary)
    .set_time_parameters(time_step=dt, total_time=total_time)
    .build()
)

# 5. Initialize the solver, observables handler, and HDF5 storage
solver = ds.DiracSolver(problem)
obs = ds.Observables(solver)
storage = ds.HDF5Storage("save_data/coulomb_1d.hdf5", problem)

# 6. Run the simulation loop
solver.run_simulation(
    storage_handler=storage,
    save_every_n_steps=5,
    observables_handler=obs,
)

# 7. Retrieve observables or plot results
psi_final = solver.get_psi()
rho_final = obs.probability_density(psi_final)
times = obs.get_history("time")
energies = obs.get_history("expected_energy")

print(f"Final norm: {rho_final.sum() * grid.get_cell_volume():.6f}")
print(f"Final expected energy: {energies[-1]:.6f}")
```

---

## Documentation

Comprehensive documentation is available in both English and Spanish inside `docs/`:

- **English (`docs/en/`):**
  - [Documentation Index](docs/en/index.md)
  - [Theoretical Background & Numerical Scheme](docs/en/theory_background.md)
  - [Software Architecture & Design Patterns](docs/en/architecture.md)
  - [Complete API Reference](docs/en/api_reference.md)
  - [User Guide](docs/en/user_guide.md)
- **Spanish (`docs/es/`):**
  - [Índice General](docs/es/index.md)
  - [Fundamentos Teóricos y Esquema Numérico](docs/es/theory_background.md)
  - [Arquitectura de Software y Patrones de Diseño](docs/es/architecture.md)
  - [Referencia Completa de la API](docs/es/api_reference.md)
  - [Guía Práctica de Usuario](docs/es/user_guide.md)

### Generating Doxygen Documentation

To build the API reference (HTML and LaTeX) in **English**:

```bash
doxygen Doxyfile.en
```

To build the API reference in **Spanish**:

```bash
doxygen Doxyfile
```

---

## Examples & Planning

- **Example Notebooks (`examples/`):**
  - Available in English ([`examples/en/`](examples/en/)) and Spanish ([`examples/es/`](examples/es/)), as well as the root `examples/` directory:
    - Free particle in 1D, 2D, and 3D (`free_particle.ipynb`)
    - Infinite potential well in 1D, 2D, and 3D (`infinite_well.ipynb`)
    - Coulomb potential scattering (`coulomb_scattering.ipynb`)
    - Yukawa potential scattering (`yukawa_scattering.ipynb`)
    - Klein paradox and potential barriers (`klein_paradox.ipynb`)
    - Spinor and Gaussian wavepacket construction (`spinor_construction.ipynb`)
    - Comprehensive library test suite (`test.ipynb`)
- **Planning & Architecture Documents (`planning/`):**
  - Available in English ([`planning/en/`](planning/en/)) and Spanish ([`planning/es/`](planning/es/)).

---

## Contributing

Contributions are welcome! Please read our guidelines before opening a Pull Request:

- [Contribution Guide (English)](CONTRIBUTING.en.md) | [Guía de Contribución (Español)](CONTRIBUTING.md)
- [Code of Conduct (English)](CODE_OF_CONDUCT.md) | [Código de Conducta (Español)](CODE_OF_CONDUCT.es.md)

To run the unit and integration test suite:

```bash
pytest
```

---

## Authors

- **Sebastián Rodríguez García** (`sebrodriguezg@udistrital.edu.co`)
- **Julián Leonardo Ávila Martínez** (`jlavilam@udistrital.edu.co`)
- **Camilo Andrés Huertas Archila** (`cahuertasa@udistrital.edu.co`)

*Physics and Informatics Research Group (FISINFOR) — Universidad Distrital Francisco José de Caldas.*

---

## License

This project is free software distributed under the terms of the **GNU General Public License v3.0 (GPL-3.0-or-later)**. See [`LICENSE`](LICENSE) for full details.
