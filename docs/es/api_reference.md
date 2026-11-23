# Referencia de la API (`dirac_solver`)

> **Idioma / Language:** **Español** | [English](../en/api_reference.md)

---

## 1. Constantes y Matrices (`dirac_solver` y `dirac_solver.DiracMatrices`)

- `hbar = 1.0`, `c = 1.0`, `electron_mass = 1.0`, `electron_charge = 1.0` (unidades naturales).
- `sigma_1`, `sigma_2`, `sigma_3`: Matrices de Pauli $2\times 2$ (`np.complex128`).
- En `dirac_solver.DiracMatrices`: `I`, `gamma0` (`beta`), `gamma1`, `gamma2`, `gamma3`, `gamma`, `alpha1`, `alpha2`, `alpha3`, `alpha`, `gamma5`.

---

## 2. Geometría (`dirac_solver.geometry.Grid`)

```python
Grid(shape, spacing, origin=None)
```
- **`shape`** (`tuple[int, ...]`): Número de puntos por eje `(nx,)`, `(nx, ny)` o `(nx, ny, nz)`.
- **`spacing`** (`float | tuple[float, ...]`): Espaciado físico entre nodos.
- **`origin`** (`tuple[float, ...] | None`): Coordenada mínima de la malla (por defecto centrada en cero).

**Métodos:**
- `get_cell_volume() -> float`: Elemento diferencial de volumen $\Delta V$.
- `get_total_points() -> int`: Número total de nodos $N_{\text{total}}$.
- `gradient(psi) -> np.ndarray`: Gradiente espacial de forma `(N_total, dim, 4)`.
- `courant_limit() -> float`: Paso temporal máximo estable según la condición CFL.

---

## 3. Estados Iniciales (`dirac_solver.initial_state`)

### `ConstantSpinor`
```python
ConstantSpinor(
    spin_polarization=[0, 0, 1],
    spin_superposition=[1 + 0j, 0 + 0j],
    momentum=[1, 0, 0],
    mass=electron_mass
)
```
Genera el biespinor constante $u(\mathbf{p}_0, s)$ en `self.constant_spinor` (forma `(4, 1)`).

### `GaussianPacket(InitialState)`
```python
GaussianPacket(constant_spinor, center=[0, 0, 0], spatial_width=1.0)
```
*(También admite los alias opcionales `spinor=` y `width=`.)*
- `evaluate(grid_position) -> np.ndarray`
- `evaluate_on_grid(grid) -> np.ndarray` (forma `(N_total, 4)`).

### `PlaneWave(InitialState)`
```python
PlaneWave(constant_spinor)
```
- `evaluate(grid_position) -> np.ndarray`
- `evaluate_on_grid(grid) -> np.ndarray` (forma `(N_total, 4)`).

---

## 4. Potenciales (`dirac_solver.potentials` y `dirac_solver._core`)

| Clase | Constructor | Descripción |
| :--- | :--- | :--- |
| `FreeParticle` | `FreeParticle()` | $V(\mathbf{r}) = 0$. |
| `ScalarPotential` | `ScalarPotential(func)` | Función arbitraria `func(r: list[float]) -> float`. |
| `CoulombPotential` | `CoulombPotential(Z, epsilon=1e-6)` | $V(\mathbf{r}) = -Z / (\|\mathbf{r}\| + \epsilon)$. |
| `YukawaPotential` | `YukawaPotential(strength, range)` | $V(\mathbf{r}) = -V_0 e^{-a r}/r$. |
| `InfiniteWellPotential` | `InfiniteWellPotential(widths)` | $0$ si $|r_k| < L_k/2$, $\infty$ fuera del pozo. |

Todos los envoltorios Python exponen `.core_potential`, `.evaluate(r)` y `.get_name()`.

---

## 5. Condiciones de Frontera (`dirac_solver._core`)

- `_core.PeriodicBoundary()` *(o cadena `"periodic"` en el builder)*: Topología toroidal.
- `_core.AbsorbingBoundary(strength, width=20)` *(o cadena `"absorbing"` en el builder)*: Capa absorbente cuadrática de ancho `width` nodos e intensidad `strength`.

---

## 6. Núcleo de Simulación (`dirac_solver.core`)

### `DiracProblemBuilder`
Métodos encadenables:
- `.set_grid(grid)`
- `.set_initial_state(initial_state)`
- `.set_potential(potential)`
- `.set_boundary_condition(boundary_condition)`
- `.set_time_parameters(time_step, total_time)`
- `.build() -> SimulationProblem`

### `DiracSolver`
- `DiracSolver(problem: SimulationProblem)`
- `run_simulation(storage_handler=None, save_every_n_steps=1, observables_handler=None)`
- `get_psi() -> np.ndarray`: Retorna el campo actual `(N_total, 4)`.
- `plot_probability_density()`: Grafica $\rho$ en 1D, 2D o 3D.

---

## 7. Observables y Almacenamiento (`observables.py`, `storage.py`)

### `Observables(solver)`
- `compute_all(time)`
- `probability_density(psi)`, `charge_density(psi)`, `probability_current(psi)`
- `expected_value_position(prob_density)`, `expected_value_momentum(psi)`, `expected_value_energy(psi)`
- `get_history(key) -> np.ndarray`

### `HDF5Storage(filepath, problem)`
- Soporta gestor de contexto (`with HDF5Storage(...) as storage:`).
- `write_snapshot(psi, time)`, `write_observables(observables_handler)`, `close()`.
- Funciones de animación: `create_animation(storage_path, output_path, interval=50)` y `create_animation_1d(storage_path, output_path, interval=50)`.
