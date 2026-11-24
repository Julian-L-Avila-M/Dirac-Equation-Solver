# Global Constants Update in Dirac-Solver

> 🌐 **Language / Idioma:** **[Español](../es/constants-update.md)** | **English**

## Summary

Architectural updates were made to the **Dirac-Solver** library to streamline the handling of physical constants and Pauli matrices by removing the legacy `Constants()` helper class. The library now:

- Operates natively and consistently in **natural units** ($\hbar = c = m_e = e = 1.0$).
- Exposes global physical constants and Pauli matrices ($\sigma_1, \sigma_2, \sigma_3$) directly at the root `dirac_solver` package level.
- Maintains full compatibility across `Grid`, `ConstantSpinor`, `GaussianPacket`, `PlaneWave`, and `DiracSolver`.
- Includes Doxygen-compatible docstrings in `__init__.py` and `initial_state.py`.

---

## Changes Implemented

### 1. Removal of `constants.py`

- Removed the `Constants()` wrapper class.
- Replaced all calls to `Constants().get("c")`, `Constants().get("HBAR")`, etc., with **package-level global constants**.

### 2. New Global Constants Defined in `dirac_solver/__init__.py`

```python
hbar: float = 1.0
c: float = 1.0
electron_mass: float = 1.0
electron_charge: float = 1.0

sigma_1 = np.array([[0, 1], [1, 0]], dtype=np.complex128)
sigma_2 = np.array([[0, -1j], [1j, 0]], dtype=np.complex128)
sigma_3 = np.array([[1, 0], [0, -1]], dtype=np.complex128)
```

- All internal modules and external scripts can import them directly from the top-level package:

```python
import dirac_solver as ds

print(ds.c)
print(ds.hbar)
print(ds.electron_mass)
```

---

### 3. Updates in `initial_state.py`

- **Direct use of global constants** instead of `Constants()`.
- Removed redundant local definitions of Pauli matrices; `sigma_1`, `sigma_2`, and `sigma_3` are now imported from `dirac_solver`.
- Spinors default to **natural units** (`mass=electron_mass`).
- Code cleanup and Doxygen documentation added to:
  - Classes: `InitialState`, `ConstantSpinor`, `GaussianPacket`, `PlaneWave`.
  - Methods: `__init__`, `_base`, `_pauli_spinor`, `_constant_spinor`, `evaluate`, `evaluate_on_grid`.

**Example of creating a constant spinor and Gaussian wavepacket:**

```python
spinor = ds.ConstantSpinor(
    spin_polarization=[0, 0, 1],
    spin_superposition=[1, 0],
    momentum=[1, 0, 0],
)
gauss = ds.GaussianPacket(
    constant_spinor=spinor,
    center=[0, 0, 0],
    spatial_width=1.0,
)
```

---

### 4. Updates in `geometry.py`

- Removed `Constants()` dependency.
- Direct use of `c` and `hbar` imported from `dirac_solver` inside the `Grid` class.
- Methods such as `courant_limit()` and `get_cell_volume()` operate seamlessly using the global constants.

**Example 1D Courant limit check:**

```python
grid_test = ds.Grid(shape=(11,), spacing=0.1)
dt_calc = grid_test.courant_limit()
print(dt_calc)  # 0.1 / c = 0.1
```

---

### 5. Comprehensive Validation Notebook (`examples/test.ipynb`)

A **Jupyter Notebook** was created to verify that all classes and numerical routines operate coherently with the **global constants** without relying on `Constants()`.

#### Notebook Contents:

1. **Importing the library and global constants:**
   ```python
   import dirac_solver as ds
   print(ds.c, ds.hbar, ds.electron_mass)
   ```
2. **Testing `Grid` in 1D, 2D, and 3D:**
   - Validating `coords`, `get_cell_volume()`, and `courant_limit()`.
   - Verifying grid shapes and centered origins.
3. **Testing `ConstantSpinor`:**
   - Constructing a constant bispinor for a given polarization and momentum.
   - Inspecting the resulting $4 \times 1$ complex column vector.
4. **Testing `GaussianPacket` and `PlaneWave`:**
   - Pointwise evaluation via `evaluate()` and full-grid evaluation via `evaluate_on_grid()`.
5. **Testing `DiracSolver`:**
   - Building a `SimulationProblem` with `DiracProblemBuilder` and initializing the C++ `FDTDLeapfrogIntegrator`.
   - Running `run_simulation()` and plotting probability densities.

---

## Conclusion

- The library **operates cleanly in natural units without the boilerplate of a `Constants()` class**.
- All methods across `Grid`, `ConstantSpinor`, `GaussianPacket`, `PlaneWave`, and `DiracSolver` share a single source of truth in `dirac_solver/__init__.py`.
