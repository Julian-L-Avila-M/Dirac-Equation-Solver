# User Guide and Practical Recipes

> **Language / Idioma:** [Español](../es/user_guide.md) | **English**

This guide provides ready-to-run recipes for common workflows in `Dirac-Solver`.

---

## 1. Choosing a Safe Time Step (CFL Condition)

Always inspect the Courant limit before selecting `time_step`:

```python
from dirac_solver import Grid

grid = Grid(shape=(100, 100), spacing=(0.1, 0.1))
dt_max = grid.courant_limit()
dt = 0.2 * dt_max  # Recommended safety factor (10% - 30% of dt_max)
```

---

## 2. 1D Simulation: Klein Tunneling with `ScalarPotential`

```python
from dirac_solver import (
    Grid, ConstantSpinor, GaussianPacket,
    DiracProblemBuilder, DiracSolver, ScalarPotential, _core
)

grid = Grid(shape=(400,), spacing=(0.05,))
spinor = ConstantSpinor(momentum=[3.0, 0.0, 0.0], mass=1.0)
packet = GaussianPacket(spinor, center=[-4.0, 0.0, 0.0], spatial_width=0.8)

# Supercritical potential step V0 > E + m
V0 = 5.0
step_potential = ScalarPotential(lambda r: V0 if r[0] >= 0.0 else 0.0)

problem = (
    DiracProblemBuilder()
    .set_grid(grid)
    .set_initial_state(packet)
    .set_potential(step_potential)
    .set_boundary_condition(_core.AbsorbingBoundary(strength=0.2, width=25))
    .set_time_parameters(time_step=0.002, total_time=4.0)
    .build()
)

solver = DiracSolver(problem)
solver.run_simulation()
solver.plot_probability_density()
```

---

## 3. 2D Simulation: Yukawa Scattering, Observables, HDF5 & GIF

```python
from dirac_solver import (
    Grid, ConstantSpinor, GaussianPacket,
    DiracProblemBuilder, DiracSolver, YukawaPotential,
    Observables, HDF5Storage, _core
)
from dirac_solver.storage import create_animation

grid = Grid(shape=(100, 100), spacing=(0.1, 0.1))
spinor = ConstantSpinor(momentum=[4.0, 1.0, 0.0])
packet = GaussianPacket(spinor, center=[-2.5, -0.5, 0.0], spatial_width=1.0)

problem = (
    DiracProblemBuilder()
    .set_grid(grid)
    .set_initial_state(packet)
    .set_potential(YukawaPotential(strength=1.5, range=0.5))
    .set_boundary_condition(_core.AbsorbingBoundary(strength=0.1, width=15))
    .set_time_parameters(time_step=0.002, total_time=5.0)
    .build()
)

solver = DiracSolver(problem)
obs = Observables(solver)
storage = HDF5Storage("yukawa_2d.hdf5", problem)

solver.run_simulation(
    storage_handler=storage,
    save_every_n_steps=25,
    observables_handler=obs
)

create_animation("yukawa_2d.hdf5", "yukawa_2d.gif", interval=80)
```

---

## 4. Querying Observable Histories

```python
times = obs.get_history("time")
pos = obs.get_history("expected_position")   # Shape (N_t, dim)
mom = obs.get_history("expected_momentum")   # Shape (N_t, dim)
energy = obs.get_history("expected_energy")  # Shape (N_t,)
```

For full interactive notebooks with visualizations, see [`examples/en/`](../../examples/en/).
