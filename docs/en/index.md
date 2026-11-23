# `Dirac-Solver` Documentation (English)

> **Language / Idioma:** [Español](../es/index.md) | **English**

Welcome to the official documentation of **`Dirac-Solver`**, a hybrid scientific library (**C++17** + **Python $\ge$ 3.8**) designed to numerically solve the time-dependent relativistic Dirac equation in **1D, 2D, and 3D**.

---

## Documentation Map

- **[Theoretical Foundations & Numerical Scheme](theory_background.md)** — Dirac equation, Clifford algebra, free-particle bispinors, FDTD Leapfrog discretization, CFL stability condition, and physical observables.
- **[Software Architecture & Design Patterns](architecture.md)** — Hybrid C++/Python design via `pybind11`, *Builder*, *Strategy*, *Template Method*, and *Facade* patterns, and memory layout conventions.
- **[API Reference](api_reference.md)** — Complete reference of all classes, methods, and parameters in Python (`dirac_solver`) and C++ (`_core`).
- **[User Guide & Practical Recipes](user_guide.md)** — Step-by-step recipes in 1D, 2D, and 3D, custom potentials (Klein tunneling), observables tracking, HDF5 storage, and animations.
- **[Complete User Manual (PDF)](../User-Manual-Dirac-Solver-EN.pdf)** — Compiled PDF manual ready for download or printing.
- **[Interactive Tutorials (Jupyter Notebooks)](../../examples/README.en.md)** — Runnable notebooks in English (`examples/en/`) and Spanish (`examples/es/`).
- **[Planning & Design Documents](../../planning/en/)** — Requirements engineering, UML diagrams, design patterns, and spinor definitions.

---

## Quick Installation

```bash
# Clone the repository
git clone https://github.com/Julian-L-Avila-M/Dirac-Equation-Solver.git
cd Dirac-Solver

# Editable install (automatically compiles the C++ _core module via scikit-build-core)
pip install -e .
```

### Manual C++ Core Compilation (Optional)

```bash
mkdir -p cpp_build && cd cpp_build
cmake ../cpp_src
make -j
```

---

## Minimal Quickstart Example

```python
from dirac_solver import (
    Grid,
    ConstantSpinor,
    GaussianPacket,
    DiracProblemBuilder,
    DiracSolver,
    FreeParticle,
    Observables,
    _core,
)

# 1. Define a 1D spatial grid
grid = Grid(shape=(200,), spacing=(0.1,))
print(f"Courant limit dt_max = {grid.courant_limit():.4f}")

# 2. Construct the initial relativistic spinor state
spinor = ConstantSpinor(momentum=[3.0, 0.0, 0.0], spin_polarization=[0, 0, 1])
psi0 = GaussianPacket(constant_spinor=spinor, center=[0.0, 0.0, 0.0], spatial_width=1.0)

# 3. Assemble the simulation problem
problem = (
    DiracProblemBuilder()
    .set_grid(grid)
    .set_initial_state(psi0)
    .set_potential(FreeParticle())
    .set_boundary_condition(_core.PeriodicBoundary())
    .set_time_parameters(time_step=0.002, total_time=2.0)
    .build()
)

# 4. Run time evolution and monitor observables
solver = DiracSolver(problem)
obs = Observables(solver)
solver.run_simulation(observables_handler=obs)

# 5. Plot the final probability density
solver.plot_probability_density()
```
