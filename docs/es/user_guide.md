# Guía de Usuario y Casos Prácticos

> **Idioma / Language:** **Español** | [English](../en/user_guide.md)

Esta guía presenta recetas listas para usar que cubren los flujos más comunes con `Dirac-Solver`.

---

## 1. Elección Segura del Paso Temporal (Criterio CFL)

Siempre verifique el límite de Courant antes de definir `time_step`:

```python
from dirac_solver import Grid

grid = Grid(shape=(100, 100), spacing=(0.1, 0.1))
dt_max = grid.courant_limit()
dt = 0.2 * dt_max  # Margen de seguridad recomendado (10% - 30% de dt_max)
```

---

## 2. Simulación 1D: Efecto Túnel de Klein con `ScalarPotential`

```python
from dirac_solver import (
    Grid, ConstantSpinor, GaussianPacket,
    DiracProblemBuilder, DiracSolver, ScalarPotential, _core
)

grid = Grid(shape=(400,), spacing=(0.05,))
spinor = ConstantSpinor(momentum=[3.0, 0.0, 0.0], mass=1.0)
packet = GaussianPacket(spinor, center=[-4.0, 0.0, 0.0], spatial_width=0.8)

# Escalón supercrítico V0 > E + m
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

## 3. Simulación 2D: Dispersión de Yukawa, Observables, HDF5 y GIF

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

## 4. Consultando el Historial de Observables

```python
times = obs.get_history("time")
pos = obs.get_history("expected_position")   # Forma (N_t, dim)
mom = obs.get_history("expected_momentum")   # Forma (N_t, dim)
energy = obs.get_history("expected_energy")  # Forma (N_t,)
```

Para tutoriales completos con visualizaciones interactivas, consulte la carpeta [`examples/es/`](../../examples/es/).
