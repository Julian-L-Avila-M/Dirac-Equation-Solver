# Dirac-Solver: Solucionador Numérico de la Ecuación de Dirac

> 🌐 **Idioma / Language:** **Español** ([`README.es.md`](README.es.md)) | **[English](README.en.md)**

Este proyecto implementa un solucionador numérico de alto rendimiento para la ecuación de Dirac dependiente del tiempo en **1D, 2D y 3D**. El núcleo computacional está escrito en **C++17** y expuesto a **Python** mediante `pybind11`, combinando simulaciones numéricas eficientes con una interfaz científica limpia, modular y expresiva.

La ecuación resuelta en unidades naturales ($\hbar = c = 1$) es:

$$
(i\gamma^\mu \partial_\mu - m)\psi = 0 \quad \Longleftrightarrow \quad i\frac{\partial \psi(\mathbf{r}, t)}{\partial t} = \left( -i\boldsymbol{\alpha}\cdot\nabla + \beta m + V(\mathbf{r})I_4 \right)\psi(\mathbf{r}, t)
$$

donde el biespinor de Dirac $\psi(\mathbf{r}, t) \in \mathbb{C}^4$ es un vector columna complejo de 4 componentes. El proyecto implementa integración temporal explícita mediante diferencias finitas en el dominio del tiempo (**FDTD Leapfrog**), potenciales escalares analíticos y personalizables, condiciones de frontera intercambiables, cálculo de observables relativistas y persistencia de datos en **HDF5**.

---

## Características Principales

- **Motor FDTD Leapfrog en C++17:** Integración explícita de segundo orden en espacio y tiempo para mallas 1D, 2D y 3D.
- **Geometría Espacial Flexible (`Grid`):** Soporte unificado para dominios cartesianos 1D, 2D y 3D con cálculo automático del límite de estabilidad de Courant-Friedrichs-Lewy (CFL).
- **Interoperabilidad C++ ↔ Python (`pybind11`):** Transferencia directa de arreglos NumPy `(N, 4)` complejos (`complex128`) y funciones callback de Python hacia C++.
- **Estados Iniciales Relativistas (`InitialState`):** Construcción general de espinores de Pauli con orientación esférica arbitraria (`ConstantSpinor`), ondas planas (`PlaneWave`) y paquetes de onda gaussianos (`GaussianPacket`).
- **Potenciales Configurables (`Potential`):** Partícula libre (`FreeParticle`), Coulomb regularizado (`CoulombPotential`), Yukawa (`YukawaPotential`), pozo infinito (`InfiniteWellPotential`) y potenciales definidos por el usuario (`ScalarPotential`).
- **Condiciones de Frontera Modulares (`BoundaryCondition`):** Condiciones periódicas (`PeriodicBoundary`) y capas absorbentes cuadráticas tipo CAP/PML (`AbsorbingBoundary`).
- **Observables y Almacenamiento (`Observables`, `HDF5Storage`):** Cálculo de densidad de probabilidad ($\rho$), densidad de carga ($J^0$), corriente de probabilidad ($\mathbf{J}$) y valores esperados ($\langle \mathbf{r} \rangle$, $\langle \mathbf{p} \rangle$, $\langle H \rangle$), con exportación a HDF5 y generación de animaciones GIF/MP4 (`create_animation`, `create_animation_1d`).
- **Documentación y Ejemplos Bilingües:** Documentación completa, cuadernos Jupyter de ejemplo y documentos de arquitectura en **Español** e **Inglés**.

---

## Estructura del Proyecto

```text
Dirac-Solver/
├── cpp_src/                  # Núcleo numérico en C++17 y enlaces pybind11
│   ├── bindings.cpp          # Definición del módulo C++ _core
│   ├── CMakeLists.txt        # Configuración de compilación CMake
│   ├── DiracMatrices.h       # Álgebra de espinores y matrices α, β de 4x4
│   ├── Grid.h                # Malla espacial C++ 1D/2D/3D
│   ├── TimeIntegrator.h      # Interfaz TimeIntegrator y FDTDLeapfrogIntegrator
│   ├── boundaries/           # BoundaryCondition, PeriodicBoundary, AbsorbingBoundary
│   └── potentials/           # Potential, FreeParticle, Coulomb, Yukawa, InfiniteWell, Scalar
├── dirac_solver/             # Paquete principal de la API en Python
│   ├── __init__.py           # Constantes naturales, matrices de Pauli y exportaciones
│   ├── core.py               # SimulationProblem, DiracProblemBuilder, DiracSolver
│   ├── geometry.py           # Clase Grid y cálculo de límite de Courant y gradientes
│   ├── initial_state.py      # InitialState, ConstantSpinor, GaussianPacket, PlaneWave
│   ├── potentials.py         # Envoltorios Python para potenciales escalares
│   ├── observables.py        # Clase Observables (ρ, J⁰, J, <r>, <p>, <H>)
│   ├── storage.py            # HDF5Storage, create_animation, create_animation_1d
│   └── DiracMatrices.py      # Matrices de Dirac (γ^μ, α^k, β, γ⁵) en NumPy
├── docs/                     # Documentación del proyecto
│   ├── es/                   # Guías y referencia técnica en Español
│   └── en/                   # Guías y referencia técnica en Inglés
├── examples/                 # Notebooks de Jupyter y scripts de demostración
│   ├── es/                   # Notebooks interactivos en Español
│   └── en/                   # Notebooks interactivos en Inglés
├── planning/                 # Documentos de ingeniería de requisitos y arquitectura
│   ├── es/                   # Documentación de diseño y planificación en Español
│   └── en/                   # Documentación de diseño y planificación en Inglés
├── tests/                    # Suite de pruebas unitarias e integración (pytest)
├── save_data/                # Directorio por defecto para resultados HDF5 y animaciones
├── Doxyfile                  # Configuración de Doxygen (Español)
├── Doxyfile.en               # Configuración de Doxygen (Inglés)
├── CONTRIBUTING.md           # Guía de contribución (Español)
├── CONTRIBUTING.en.md        # Guía de contribución (Inglés)
├── CODE_OF_CONDUCT.md        # Código de conducta (Inglés)
├── CODE_OF_CONDUCT.es.md     # Código de conducta (Español)
├── LICENSE                   # Licencia GNU GPL v3.0
└── pyproject.toml            # Configuración de empaquetado (scikit-build-core)
```

---

## Instalación

### Requisitos Previos

- **Python** $\ge 3.8$
- Compilador **C++17** (`g++`, `clang++` o `MSVC`)
- **CMake** $\ge 3.15$ y `ninja` / `make`
- Dependencias de Python: `numpy`, `matplotlib`, `h5py`, `pybind11`, `scikit-build-core`

### Instalación Local Editable (Recomendada)

```bash
pip install -e .
```

### Compilación Manual del Núcleo C++ con CMake

Si deseas compilar manualmente la extensión `_core`:

```bash
mkdir -p cpp_build
cd cpp_build
cmake ../cpp_src
make -j
```

---

## Uso Básico

A continuación se muestra un ejemplo completo utilizando la API real de `dirac_solver` (patrón `DiracProblemBuilder`, evolución temporal con `DiracSolver`, cálculo de `Observables` y almacenamiento en `HDF5Storage`):

```python
import dirac_solver as ds
from dirac_solver import _core
from dirac_solver.storage import create_animation_1d

# 1. Definir la malla espacial 1D (400 puntos, dx = 0.05)
grid = ds.Grid(shape=(400,), spacing=0.05)

# Determinar un paso de tiempo seguro según la condición CFL
dt = 0.5 * grid.courant_limit()
total_time = 5.0

# 2. Configurar el estado inicial: paquete de ondas gaussiano con espín +z
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

# 3. Definir un potencial (ej. Coulomb regularizado o potencial personalizado)
potential = ds.CoulombPotential(Z=1.0, epsilon=0.1)

# Alternativa con condición de frontera explícita de _core o cadena ("absorbing" / "periodic")
boundary = _core.AbsorbingBoundary(strength=0.3, width=20)

# 4. Construir el problema mediante DiracProblemBuilder
problem = (
    ds.DiracProblemBuilder()
    .set_grid(grid)
    .set_initial_state(initial_state)
    .set_potential(potential)
    .set_boundary_condition(boundary)
    .set_time_parameters(time_step=dt, total_time=total_time)
    .build()
)

# 5. Inicializar el solucionador, manejador de observables y almacenamiento HDF5
solver = ds.DiracSolver(problem)
obs = ds.Observables(solver)
storage = ds.HDF5Storage("save_data/coulomb_1d.hdf5", problem)

# 6. Ejecutar la simulación
solver.run_simulation(
    storage_handler=storage,
    save_every_n_steps=5,
    observables_handler=obs,
)

# 7. Acceder a observables o graficar resultados
psi_final = solver.get_psi()
rho_final = obs.probability_density(psi_final)
tiempos = obs.get_history("time")
energias = obs.get_history("expected_energy")

print(f"Norma final: {rho_final.sum() * grid.get_cell_volume():.6f}")
print(f"Energía esperada final: {energias[-1]:.6f}")
```

---

## Documentación

La documentación detallada se encuentra disponible en dos idiomas dentro del directorio `docs/`:

- **Español (`docs/es/`):**
  - [Índice General](docs/es/index.md)
  - [Fundamentos Teóricos y Esquema Numérico](docs/es/theory_background.md)
  - [Arquitectura de Software y Patrones de Diseño](docs/es/architecture.md)
  - [Referencia Completa de la API](docs/es/api_reference.md)
  - [Guía Práctica de Usuario](docs/es/user_guide.md)
- **Inglés (`docs/en/`):**
  - [Documentation Index](docs/en/index.md)
  - [Theoretical Background & Numerical Scheme](docs/en/theory_background.md)
  - [Software Architecture & Design Patterns](docs/en/architecture.md)
  - [Complete API Reference](docs/en/api_reference.md)
  - [User Guide](docs/en/user_guide.md)

### Generación de Documentación con Doxygen

Para compilar la documentación de código (HTML y LaTeX) en **Español**:

```bash
doxygen Doxyfile
```

Para compilar la documentación en **Inglés**:

```bash
doxygen Doxyfile.en
```

---

## Ejemplos y Planificación

- **Notebooks de Ejemplo (`examples/`):**
  - `examples/es/` (Español) y `examples/en/` (Inglés), además de los cuadernos en la raíz de `examples/`:
    - Partícula libre en 1D, 2D y 3D (`free_particle.ipynb`)
    - Pozo de potencial infinito en 1D, 2D y 3D (`infinite_well.ipynb`)
    - Dispersión en potencial de Coulomb (`coulomb_scattering.ipynb`)
    - Dispersión en potencial de Yukawa (`yukawa_scattering.ipynb`)
    - Paradoja de Klein y barreras de potencial (`klein_paradox.ipynb`)
    - Construcción de espinores y paquetes gaussianos (`spinor_construction.ipynb`)
    - Pruebas integrales del paquete (`test.ipynb`)
- **Documentos de Planificación y Arquitectura (`planning/`):**
  - Disponibles en Español ([`planning/es/`](planning/es/)) y en Inglés ([`planning/en/`](planning/en/)).

---

## Contribuir

Las contribuciones son bienvenidas. Por favor revisa nuestras guías antes de enviar un Pull Request:

- [Guía de Contribución (Español)](CONTRIBUTING.md) | [Contribution Guide (English)](CONTRIBUTING.en.md)
- [Código de Conducta (Español)](CODE_OF_CONDUCT.es.md) | [Code of Conduct (English)](CODE_OF_CONDUCT.md)

Para ejecutar la suite de pruebas:

```bash
pytest
```

---

## Autores

- **Sebastián Rodríguez García** (`sebrodriguezg@udistrital.edu.co`)
- **Julián Leonardo Ávila Martínez** (`jlavilam@udistrital.edu.co`)
- **Camilo Andrés Huertas Archila** (`cahuertasa@udistrital.edu.co`)

*Grupo de Física e Informática (FISINFOR) — Universidad Distrital Francisco José de Caldas.*

---

## Licencia

Este proyecto es software libre y se distribuye bajo los términos de la licencia **GNU General Public License v3.0 (GPL-3.0-or-later)**. Consulta el archivo [`LICENSE`](LICENSE) para más detalles.
