# Documentación de `Dirac-Solver` (Español)

> **Idioma / Language:** **Español** | [English](../en/index.md)

Bienvenido a la documentación oficial de **`Dirac-Solver`**, una librería científica híbrida (**C++17** + **Python $\ge$ 3.8**) diseñada para resolver numéricamente la ecuación relativista de Dirac dependiente del tiempo en **1D, 2D y 3D**.

---

## Mapa de la Documentación

- **[Fundamentos Teóricos y Esquema Numérico](theory_background.md)** — Ecuación de Dirac, álgebra de Clifford, biespinores de partícula libre, discretización FDTD Leapfrog, criterio de estabilidad CFL y observables físicos.
- **[Arquitectura del Software y Patrones de Diseño](architecture.md)** — Diseño híbrido C++/Python vía `pybind11`, patrones *Builder*, *Strategy*, *Template Method* y *Facade*, y disposición en memoria.
- **[Referencia de la API](api_reference.md)** — Referencia detallada de todas las clases, funciones y parámetros en Python (`dirac_solver`) y C++ (`_core`).
- **[Guía de Usuario y Casos Prácticos](user_guide.md)** — Ejemplos paso a paso en 1D, 2D y 3D, potenciales personalizados (Efecto Klein), cálculo de observables, persistencia HDF5 y animaciones.
- **[Manual de Usuario Completo (PDF)](../Manual-Usuario-Dirac-Solver-ES.pdf)** — Versión PDF compilada lista para descarga o impresión.
- **[Tutoriales Interactivos (Jupyter Notebooks)](../../examples/README.md)** — Cuadernos ejecutables en español (`examples/es/`) e inglés (`examples/en/`).
- **[Documentos de Planeación y Diseño](../../planning/es/)** — Requisitos, diagramas UML, patrones y definición de espinores.

---

## Instalación Rápida

```bash
# Clonar el repositorio
git clone https://github.com/Julian-L-Avila-M/Dirac-Equation-Solver.git
cd Dirac-Solver

# Instalación editable (compila automáticamente el módulo nativo _core en C++)
pip install -e .
```

### Compilación manual del núcleo C++ (Opcional)

```bash
mkdir -p cpp_build && cd cpp_build
cmake ../cpp_src
make -j
```

---

## Ejemplo Mínimo de Inicio Rápido

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

# 1. Definir la malla 1D
grid = Grid(shape=(200,), spacing=(0.1,))
print(f"Límite de Courant dt_max = {grid.courant_limit():.4f}")

# 2. Definir estado espinorial inicial (paquete gaussiano relativista)
spinor = ConstantSpinor(momentum=[3.0, 0.0, 0.0], spin_polarization=[0, 0, 1])
psi0 = GaussianPacket(constant_spinor=spinor, center=[0.0, 0.0, 0.0], spatial_width=1.0)

# 3. Ensamblar el problema
problem = (
    DiracProblemBuilder()
    .set_grid(grid)
    .set_initial_state(psi0)
    .set_potential(FreeParticle())
    .set_boundary_condition(_core.PeriodicBoundary())
    .set_time_parameters(time_step=0.002, total_time=2.0)
    .build()
)

# 4. Ejecutar simulación y monitorear observables
solver = DiracSolver(problem)
obs = Observables(solver)
solver.run_simulation(observables_handler=obs)

# 5. Graficar la densidad de probabilidad final
solver.plot_probability_density()
```
