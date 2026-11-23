# API Reference (`dirac_solver`)

> **Language / Idioma:** [Español](../es/api_reference.md) | **English**

---

## 1. Constants and Matrices (`dirac_solver` & `dirac_solver.DiracMatrices`)

- `hbar = 1.0`, `c = 1.0`, `electron_mass = 1.0`, `electron_charge = 1.0` (natural units).
- `sigma_1`, `sigma_2`, `sigma_3`: $2\times 2$ Pauli matrices (`np.complex128`).
- In `dirac_solver.DiracMatrices`: `I`, `gamma0` (`beta`), `gamma1`, `gamma2`, `gamma3`, `gamma`, `alpha1`, `alpha2`, `alpha3`, `alpha`, `gamma5`.

---

## 2. Geometry (`dirac_solver.geometry.Grid`)

```python
Grid(shape, spacing, origin=None)
```
- **`shape`** (`tuple[int, ...]`): Number of grid points per axis `(nx,)`, `(nx, ny)`, or `(nx, ny, nz)`.
- **`spacing`** (`float | tuple[float, ...]`): Physical cell spacing per axis.
- **`origin`** (`tuple[float, ...] | None`): Minimum coordinate of the grid (defaults to centered at zero).

**Methods:**
- `get_cell_volume() -> float`: Differential volume element $\Delta V$.
- `get_total_points() -> int`: Total number of grid points $N_{\text{total}}$.
- `gradient(psi) -> np.ndarray`: Spatial gradient array of shape `(N_total, dim, 4)`.
- `courant_limit() -> float`: Maximum stable time step under the CFL condition.

---

## 3. Initial States (`dirac_solver.initial_state`)

### `ConstantSpinor`
```python
ConstantSpinor(
    spin_polarization=[0, 0, 1],
    spin_superposition=[1 + 0j, 0 + 0j],
    momentum=[1, 0, 0],
    mass=electron_mass
)
```
Constructs the constant bispinor $u(\mathbf{p}_0, s)$ stored in `self.constant_spinor` (shape `(4, 1)`).

### `GaussianPacket(InitialState)`
```python
GaussianPacket(constant_spinor, center=[0, 0, 0], spatial_width=1.0)
```
*(Also accepts optional aliases `spinor=` and `width=`.)*
- `evaluate(grid_position) -> np.ndarray`
- `evaluate_on_grid(grid) -> np.ndarray` (shape `(N_total, 4)`).

### `PlaneWave(InitialState)`
```python
PlaneWave(constant_spinor)
```
- `evaluate(grid_position) -> np.ndarray`
- `evaluate_on_grid(grid) -> np.ndarray` (shape `(N_total, 4)`).

---

## 4. Potentials (`dirac_solver.potentials` & `dirac_solver._core`)

| Class | Constructor | Description |
| :--- | :--- | :--- |
| `FreeParticle` | `FreeParticle()` | $V(\mathbf{r}) = 0$. |
| `ScalarPotential` | `ScalarPotential(func)` | Custom potential via Python callable `func(r: list[float]) -> float`. |
| `CoulombPotential` | `CoulombPotential(Z, epsilon=1e-6)` | $V(\mathbf{r}) = -Z / (\|\mathbf{r}\| + \epsilon)$. |
| `YukawaPotential` | `YukawaPotential(strength, range)` | $V(\mathbf{r}) = -V_0 e^{-a r}/r$. |
| `InfiniteWellPotential` | `InfiniteWellPotential(widths)` | $0$ if $|r_k| < L_k/2$, $\infty$ outside the well. |

All Python wrappers expose `.core_potential`, `.evaluate(r)`, and `.get_name()`.

---

## 5. Boundary Conditions (`dirac_solver._core`)

- `_core.PeriodicBoundary()` *(or shorthand `"periodic"` in builder)*: Toroidal periodic boundaries.
- `_core.AbsorbingBoundary(strength, width=20)` *(or shorthand `"absorbing"` in builder)*: Quadratic absorbing layer of width `width` nodes and maximum damping `strength`.

---

## 6. Simulation Core (`dirac_solver.core`)

### `DiracProblemBuilder`
Chainable methods:
- `.set_grid(grid)`
- `.set_initial_state(initial_state)`
- `.set_potential(potential)`
- `.set_boundary_condition(boundary_condition)`
- `.set_time_parameters(time_step, total_time)`
- `.build() -> SimulationProblem`

### `DiracSolver`
- `DiracSolver(problem: SimulationProblem)`
- `run_simulation(storage_handler=None, save_every_n_steps=1, observables_handler=None)`
- `get_psi() -> np.ndarray`: Returns current spinor array of shape `(N_total, 4)`.
- `plot_probability_density()`: Plots $\rho$ in 1D, 2D, or 3D.

---

## 7. Observables and Storage (`observables.py`, `storage.py`)

### `Observables(solver)`
- `compute_all(time)`
- `probability_density(psi)`, `charge_density(psi)`, `probability_current(psi)`
- `expected_value_position(prob_density)`, `expected_value_momentum(psi)`, `expected_value_energy(psi)`
- `get_history(key) -> np.ndarray`

### `HDF5Storage(filepath, problem)`
- Context manager support (`with HDF5Storage(...) as storage:`).
- `write_snapshot(psi, time)`, `write_observables(observables_handler)`, `close()`.
- Animation utilities: `create_animation(storage_path, output_path, interval=50)` and `create_animation_1d(storage_path, output_path, interval=50)`.
