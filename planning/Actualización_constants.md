



# Actualización de la librería Dirac Solver

## Resumen

Se realizaron modificaciones en la librería **Dirac Solver** para simplificar el manejo de constantes y matrices de Pauli, eliminando la dependencia de la clase `Constants()`. La librería ahora:

- Funciona únicamente en **unidades naturales**.
- Permite acceder a constantes globales directamente desde el paquete.
- Mantiene compatibilidad con las clases `Grid`, `ConstantSpinor` y `GaussianPacket`.
- Incluye documentación Doxygen en `__init__.py` y `initial_state.py`.

---

## Cambios realizados

### 1. Eliminación de `constants.py`

- Se removió la clase `Constants()`.
- Todas las referencias a `Constants().get("c")`, `Constants().get("HBAR")`, etc., fueron reemplazadas por **constantes globales**.

### 2. Nuevas constantes globales definidas en `__init__.py`

```python
c = 1.0
hbar = 1.0
electron_mass = 1.0
electron_charge = 1.0

sigma_1 = np.array([[0,1],[1,0]], dtype=np.complex128)
sigma_2 = np.array([[0,-1j],[1j,0]], dtype=np.complex128)
sigma_3 = np.array([[1,0],[0,-1]], dtype=np.complex128)
````

- Todas las clases que necesiten estas constantes pueden importarlas directamente:
    

```python
import dirac_solver as ds

print(ds.c)
print(ds.hbar)
print(ds.electron_mass)
```

---

### 3. Modificaciones en `initial_state.py`

- **Uso de constantes globales** en lugar de `Constants()`.
    
- Eliminación de definiciones locales de matrices de Pauli; ahora se usan `sigma_1`, `sigma_2`, `sigma_3` importadas desde el paquete.
    
- Por defecto, los spinors se crean en **unidades naturales** (`mass=electron_mass`).
    
- Limpieza de redundancias y comentarios innecesarios.
    
- Documentación Doxygen añadida a:
    
    - Clases: `ConstantSpinor`, `GaussianPacket`
        
    - Métodos: `__init__`, `_base`, `_pauli_spinor`, `_constant_spinor`, `evaluate`.
        

**Ejemplo de creación de un spinor constante:**

```python
spinor = ds.ConstantSpinor(spin_polarization=[0,0,1], spin_superposition=[1,0], momentum=[1,0,0])
gauss = ds.GaussianPacket(constant_spinor=spinor, center=[0,0,0], spatial_width=1.0)
```

---

### 4. Modificaciones en `geometry.py`

- Eliminación de `Constants()`.
    
- Uso directo de `c` y `hbar` importadas desde el paquete en la clase `Grid`.
    
- Métodos como `courant_limit()` y `dv` funcionan igual, ahora usando las constantes globales.
    

**Ejemplo de prueba interna de Courant 1D:**

```python
grid_test = ds.Grid(shape=(11,), spacing=0.1)
dt_calc = grid_test.courant_limit()
print(dt_calc)
```

- La prueba confirma que `Grid.c` usa correctamente la constante global `c`.
    

---

### 5. Creación del notebook de prueba definitiva

Se diseñó un **notebook de Jupyter** para validar que todas las clases y funciones funcionan correctamente con las **constantes globales**, sin depender de `Constants()`.

#### Contenido del notebook:

1. **Importar librería y constantes globales**
    
    ```python
    import dirac_solver as ds
    print(ds.c, ds.hbar, ds.electron_mass)
    ```
    
2. **Pruebas de Grid en 1D, 2D y 3D**
    
    - Validación de `coords` y `courant_limit()`.
        
    - Confirmación de dimensiones correctas de la malla.
        
3. **Prueba de ConstantSpinor**
    
    - Creación de un spinor constante.
        
    - Impresión del spinor 4x1 resultante.
        
4. **Prueba de GaussianPacket**
    
    - Evaluación del paquete de ondas en múltiples puntos.
        
    - Confirmación de que el cálculo del spinor y la parte espacial funcionan correctamente.
        
5. **Prueba de DiracSolver (básica)**
    
    - Inicialización del solver con `Grid` y `GaussianPacket`.
        
    - Ejecución de `run_simulation` (placeholder) y graficación de densidad de probabilidad.
        
6. **Validación de consistencia de constantes**
    
    - Se verifica que todas las clases usan las **constantes globales** correctamente.
        
    
    ```python
    assert np.isclose(ds.c, solver.c)
    assert np.isclose(ds.hbar, solver.hbar)
    assert np.isclose(ds.electron_mass, solver.mass)
    ```
    

El notebook sirve como **prueba integral** de la actualización de la librería enfocada en las constantes globales.

---

## Conclusión

- La librería **ya no depende de sistemas de unidades ni de `Constants()`**.
    
- Todas las funciones de `Grid`, `ConstantSpinor` y `GaussianPacket` funcionan correctamente con las **constantes globales en unidades naturales**.
    
- Se creó un **notebook de prueba definitiva** que permite validar rápidamente la correcta propagación de las constantes y el comportamiento esperado de los objetos de la librería.
    
