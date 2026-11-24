# Estructura del Proyecto `dirac_solver`

> 🌐 **Idioma / Language:** **Español** | **[English](../en/structure.md)**

Este documento describe tanto la arquitectura modular conceptual del proyecto `dirac_solver` (inspirada en librerías científicas como **DeepXDE**) como su organización real implementada en la arquitectura híbrida **Python + C++17 (`pybind11`)**.

---

## 1. Estructura Real Implementada en el Repositorio

```text
Dirac-Solver/
├── pyproject.toml                # Configuración de compilación (scikit-build-core + pybind11)
├── README.md                     # Documentación principal en Español (con selector de idioma)
├── README.es.md                  # Documentación principal en Español
├── README.en.md                  # Documentación principal en Inglés
├── CONTRIBUTING.md               # Guía de contribución (Español)
├── CONTRIBUTING.en.md            # Guía de contribución (Inglés)
├── CODE_OF_CONDUCT.md            # Código de conducta (Inglés)
├── CODE_OF_CONDUCT.es.md         # Código de conducta (Español)
├── Doxyfile                      # Configuración de Doxygen en Español
├── Doxyfile.en                   # Configuración de Doxygen en Inglés
├── LICENSE                       # Licencia GNU General Public License v3.0
│
├── cpp_src/                      # Motor numérico de alto rendimiento en C++17
│   ├── CMakeLists.txt            # Script de compilación CMake para el módulo _core
│   ├── bindings.cpp              # Enlaces pybind11 que exponen _core a Python
│   ├── DiracMatrices.h           # Struct Dirac::Spinor y matrices 4x4 (I4, β, α_x, α_y, α_z)
│   ├── Grid.h                    # Malla espacial C++ 1D/2D/3D e indexación plana a coordenadas
│   ├── TimeIntegrator.h          # Interfaz TimeIntegrator y motor FDTDLeapfrogIntegrator
│   ├── boundaries/               # Estrategias de condiciones de frontera
│   │   ├── BoundaryCondition.h   # Clase base abstracta BoundaryCondition
│   │   ├── PeriodicBoundary.h    # Frontera periódica
│   │   └── AbsorbingBoundary.h   # Frontera absorbente con perfil de amortiguamiento cuadrático
│   └── potentials/               # Jerarquía polimórfica de potenciales escalares
│       ├── Potential.h           # Clase base abstracta Potential
│       ├── FreeParticle.h        # Potencial nulo V(r) = 0
│       ├── CoulombPotential.h    # Potencial de Coulomb regularizado -Z / (|r| + ε)
│       ├── YukawaPotential.h     # Potencial de Yukawa -V_0 exp(-a|r|) / |r|
│       ├── InfiniteWellPotential.h # Pozo de potencial infinito 1D/2D/3D
│       └── ScalarPotential.h     # Potencial escalar arbitrario mediante callback std::function
│
├── dirac_solver/                 # Paquete Python de alto nivel
│   ├── __init__.py               # Constantes en unidades naturales (ħ=c=m=e=1) y matrices de Pauli
│   ├── core.py                   # SimulationProblem, DiracProblemBuilder y DiracSolver
│   ├── geometry.py               # Clase Grid (coordenadas, volumen de celda, gradiente, límite CFL)
│   ├── initial_state.py          # InitialState, ConstantSpinor, GaussianPacket y PlaneWave
│   ├── potentials.py             # Envoltorios Python para los potenciales de _core
│   ├── observables.py            # Clase Observables (ρ, J⁰, J, <r>, <p>, <H>)
│   ├── storage.py                # HDF5Storage, create_animation y create_animation_1d
│   └── DiracMatrices.py          # Matrices de Dirac (γ^μ, α^k, β, γ⁵) en NumPy
│
├── docs/                         # Documentación técnica bilingüe
│   ├── es/                       # Documentación en Español (index, theory, architecture, api, guide)
│   └── en/                       # Documentación en Inglés (index, theory, architecture, api, guide)
│
├── examples/                     # Cuadernos interactivos de Jupyter
│   ├── es/                       # Notebooks comentados en Español
│   └── en/                       # Notebooks comentados en Inglés
│
├── planning/                     # Documentos de ingeniería de software y diseño
│   ├── es/                       # Planificación y arquitectura en Español
│   └── en/                       # Planificación y arquitectura en Inglés
│
└── tests/                        # Suite de pruebas automatizadas con pytest
    ├── test_integration.py       # Pruebas de integración Python <-> C++
    ├── test_physics.py           # Pruebas de conservación física y evolución analítica
    └── test_potentials.py        # Pruebas unitarias de potenciales físicos
```

---

## 2. Descripción de los Módulos Principales

### Raíz del Proyecto
- **`pyproject.toml`**: Define el sistema de construcción basado en `scikit-build-core` y `pybind11`, metadatos del paquete, autores, licencia `GPL-3.0-or-later` y dependencias (`numpy`, `matplotlib`, `h5py`).
- **`README.md` / `README.es.md` / `README.en.md`**: Puerta de entrada bilingüe con instrucciones de instalación, visión general y ejemplo rápido funcional.
- **`Doxyfile` / `Doxyfile.en`**: Archivos de configuración para generar la documentación de referencia HTML y LaTeX en Español e Inglés.

### Núcleo C++ (`cpp_src/`)
1. **`DiracMatrices.h`**: Define el tipo `Dirac::Spinor` (arreglo complejo de 4 componentes con operadores aritméticos sobrecargados) y las matrices $\beta, \alpha_x, \alpha_y, \alpha_z$ en la representación estándar de Dirac.
2. **`Grid.h`**: Gestiona la topología discreta 1D, 2D y 3D centrada en el origen y traduce índices planos $i \in [0, N-1]$ a coordenadas espaciales $(x, y, z)$.
3. **`TimeIntegrator.h`**: Implementa el motor `FDTDLeapfrogIntegrator`, que almacena tres estados temporales ($\psi_{\text{prev}}, \psi_{\text{curr}}, \psi_{\text{next}}$), inicializa el paso previo mediante Euler hacia atrás y evalúa el Hamiltoniano local mediante diferencias finitas centradas.
4. **`potentials/` y `boundaries/`**: Jerarquías polimórficas para potenciales escalares y condiciones de frontera.
5. **`bindings.cpp`**: Expone todas las clases C++ bajo el módulo compilado `dirac_solver._core`.

### API Python (`dirac_solver/`)
1. **`geometry.py`**: Implementa `Grid` en Python, incluyendo generación de mallas mediante `np.meshgrid`, cálculo de gradientes espaciales de segundo orden (`np.gradient`) y límite de estabilidad de Courant (`courant_limit()`).
2. **`initial_state.py`**: Construye espinores de Pauli orientados en cualquier dirección $(\theta, \phi)$, biespinores constantes $u(\mathbf{p}_0)$, ondas planas y paquetes gaussianos.
3. **`core.py`**: Contiene `SimulationProblem`, `DiracProblemBuilder` y `DiracSolver`.
4. **`observables.py`**: Calcula densidades locales ($\rho, J^0, \mathbf{J}$) y valores esperados integrados ($\langle \mathbf{r} \rangle, \langle \mathbf{p} \rangle, \langle H \rangle$) usando contracciones tensoriales con `np.einsum`.
5. **`storage.py`**: Gestiona la escritura incremental en HDF5 (`HDF5Storage`) y la generación de animaciones MP4/GIF (`create_animation`, `create_animation_1d`).
