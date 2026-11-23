# Actualización de Constantes Globales en Dirac-Solver

> 🌐 **Idioma / Language:** **Español** | **[English](../en/constants-update.md)**

## Resumen

Se realizaron modificaciones arquitectónicas en la librería **Dirac-Solver** para simplificar el manejo de constantes físicas y matrices de Pauli, eliminando la dependencia de la antigua clase auxiliar `Constants()`. La librería ahora:

- Opera de manera nativa y consistente en **unidades naturales** ($\hbar = c = m_e = e = 1.0$).
- Permite acceder a las constantes globales y matrices de Pauli ($\sigma_1, \sigma_2, \sigma_3$) directamente desde la raíz del paquete `dirac_solver`.
- Mantiene compatibilidad total con las clases `Grid`, `ConstantSpinor`, `GaussianPacket`, `PlaneWave` y `DiracSolver`.
- Incluye documentación estilo Doxygen en `__init__.py` e `initial_state.py`.

---

## Cambios Realizados

### 1. Eliminación de `constants.py`

- Se removió la clase `Constants()`.
- Todas las llamadas del tipo `Constants().get("c")`, `Constants().get("HBAR")`, etc., fueron reemplazadas por **constantes globales a nivel de paquete**.

### 2. Nuevas Constantes Globales Definidas en `dirac_solver/__init__.py`

```python
hbar: float = 1.0
c: float = 1.0
electron_mass: float = 1.0
electron_charge: float = 1.0

sigma_1 = np.array([[0, 1], [1, 0]], dtype=np.complex128)
sigma_2 = np.array([[0, -1j], [1j, 0]], dtype=np.complex128)
sigma_3 = np.array([[1, 0], [0, -1]], dtype=np.complex128)
```

- Cualquier módulo o usuario puede importarlas directamente desde el paquete principal:

```python
import dirac_solver as ds

print(ds.c)
print(ds.hbar)
print(ds.electron_mass)
```

---

### 3. Modificaciones en `initial_state.py`

- **Uso de constantes globales** en lugar de `Constants()`.
- Eliminación de definiciones locales redundantes de matrices de Pauli; ahora se importan `sigma_1`, `sigma_2`, `sigma_3` desde `dirac_solver`.
- Por defecto, los espinores se inicializan en **unidades naturales** (`mass=electron_mass`).
- Limpieza de código y adición de documentación Doxygen a:
  - Clases: `InitialState`, `ConstantSpinor`, `GaussianPacket`, `PlaneWave`.
  - Métodos: `__init__`, `_base`, `_pauli_spinor`, `_constant_spinor`, `evaluate`, `evaluate_on_grid`.

**Ejemplo de creación de un espinor constante y paquete gaussiano:**

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

### 4. Modificaciones en `geometry.py`

- Eliminación de `Constants()`.
- Uso directo de `c` y `hbar` importadas desde `dirac_solver` dentro de la clase `Grid`.
- Métodos como `courant_limit()` y `get_cell_volume()` emplean directamente las constantes globales.

**Ejemplo de verificación del límite de Courant 1D:**

```python
grid_test = ds.Grid(shape=(11,), spacing=0.1)
dt_calc = grid_test.courant_limit()
print(dt_calc)  # 0.1 / c = 0.1
```

---

### 5. Creación del Notebook de Prueba Integral (`examples/test.ipynb`)

Se diseñó un **notebook de Jupyter** para validar que todas las clases y funciones operan de forma coherente con las **constantes globales**, sin depender de `Constants()`.

#### Contenido del notebook:

1. **Importación de la librería y constantes globales:**
   ```python
   import dirac_solver as ds
   print(ds.c, ds.hbar, ds.electron_mass)
   ```
2. **Pruebas de `Grid` en 1D, 2D y 3D:**
   - Validación de `coords`, `get_cell_volume()` y `courant_limit()`.
   - Confirmación de dimensiones espaciales de la malla.
3. **Prueba de `ConstantSpinor`:**
   - Creación de un espinor constante con polarización y momento dados.
   - Verificación del vector columna complejo $4 \times 1$ resultante.
4. **Prueba de `GaussianPacket` y `PlaneWave`:**
   - Evaluación puntual con `evaluate()` y sobre toda la malla con `evaluate_on_grid()`.
5. **Prueba de `DiracSolver`:**
   - Construcción del problema con `DiracProblemBuilder` e inicialización del motor C++ `FDTDLeapfrogIntegrator`.
   - Ejecución de `run_simulation()` y graficación de la densidad de probabilidad.

---

## Conclusión

- La librería **opera íntegramente en unidades naturales sin sobrecarga de clases contenedoras de constantes**.
- Todas las rutinas de `Grid`, `ConstantSpinor`, `GaussianPacket`, `PlaneWave` y `DiracSolver` comparten una única fuente de verdad en `dirac_solver/__init__.py`.
