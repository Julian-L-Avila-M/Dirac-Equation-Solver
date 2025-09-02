# Estructura del Proyecto dirac_solver

Este documento describe la estructura modular del proyecto `dirac_solver`,
inspirada en el diseño de librerías como **DeepXDE**. La idea es mantener una
organización clara, extensible y reutilizable para resolver la ecuación de Dirac
en diferentes escenarios físicos.

---

## 📂 Esquema Visual

```text
dirac_solver/
├── pyproject.toml
├── README.md
├── LICENSE
├── src/
│   └── dirac_solver/
│       ├── __init__.py
│       │
│       ├── solvers/
│       │   ├── __init__.py
│       │   ├── time_integrator.py
│       │   ├── eigensolver.py
│       │   └── observables.py
│       │
│       ├── physics/
│       │   ├── __init__.py
│       │   ├── gamma_matrices.py
│       │   ├── spinors.py
│       │   └── relativistic_identities.py
│       │
│       ├── potentials/
│       │   ├── __init__.py
│       │   ├── base.py
│       │   ├── coulomb.py
│       │   ├── yukawa.py
│       │   ├── step.py
│       │   ├── infinite_well.py
│       │   └── custom.py
│       │
│       ├── geometry/
│       │   ├── __init__.py
│       │   ├── interval.py
│       │   ├── radial.py
│       │   └── grid.py
│       │
│       ├── conditions/
│       │   ├── __init__.py
│       │   ├── initial.py
│       │   └── boundary.py
│       │
│       ├── io/
│       │   ├── __init__.py
│       │   ├── saver.py
│       │   ├── loader.py
│       │   └── config.py
│       │
│       ├── visualization/
│       │   ├── __init__.py
│       │   ├── plot_wavefunction.py
│       │   ├── plot_density.py
│       │   └── animations.py
│       │
│       └── utils/
│           ├── __init__.py
│           ├── math_ops.py
│           ├── validators.py
│           └── constants.py
│
├── tests/
│   ├── test_potentials.py
│   ├── test_geometry.py
│   ├── test_boundary.py
│   ├── test_solver_free.py
│   ├── test_solver_infinite_well.py
│   ├── test_solver_klein.py
│   ├── test_solver_coulomb.py
│   └── test_solver_yukawa.py
│
├── examples/
│   ├── free_particle.py
│   ├── infinite_well.py
│   ├── klein_paradox.py
│   ├── hydrogen_atom.py
│   └── yukawa_bound_states.py
│
└── docs/
    ├── index.md
    ├── api_reference.md
    └── theory_background.md

```


## Raíz del Proyecto

- **pyproject.toml**  
  Archivo de configuración para empaquetar e instalar la librería (`pip install
  dirac_solver`). Define dependencias, metadatos y backend de construcción.

- **README.md**  
  Introducción general al proyecto, instalación, ejemplos rápidos de uso y
  enlaces a documentación.

- **LICENSE**  
  Licencia del proyecto (ejemplo: MIT, GPL, BSD).

---

## `src/dirac_solver/`

Contiene el **código fuente principal** de la librería.

### 1. `solvers/` – Núcleo numérico
- **time_integrator.py**: Métodos de evolución temporal (Crank–Nicolson, Split-Operator, Runge–Kutta).  
- **eigensolver.py**: Resolución de problemas estacionarios (autovalores/autofunciones, bound states).  
- **observables.py**: Cálculo de valores esperados: energía, posición, momento, corrientes.

### 2. `physics/` – Definiciones físicas
- **gamma_matrices.py**: Representaciones de matrices de Dirac en distintas dimensiones.  
- **spinors.py**: Construcción de espinores iniciales (partícula libre, polarización de espín).  
- **relativistic_identities.py**: Utilidades analíticas (Zitterbewegung, relaciones de conmutación).

### 3. `potentials/` – Potenciales
- **base.py**: Clase abstracta `Potential` para herencia.  
- **coulomb.py**: Potencial de Coulomb (átomo de hidrógeno).  
- **yukawa.py**: Potencial de Yukawa (interacciones de corto alcance).  
- **step.py**: Escalón de potencial (Efecto Klein).  
- **infinite_well.py**: Pozo de potencial infinito.  
- **custom.py**: Permite definir potenciales personalizados por el usuario.

### 4. `geometry/` – Geometrías y dominios
- **interval.py**: Intervalo unidimensional con discretización uniforme.  
- **radial.py**: Coordenada radial (casos centrales como Coulomb/Yukawa).  
- **grid.py**: Utilidades para crear mallas, espaciamientos y operadores de diferencias finitas.

### 5. `conditions/` – Condiciones
- **initial.py**: Estados iniciales (onda plana, paquete gaussiano, superposición).  
- **boundary.py**: Condiciones de frontera (Dirichlet, Neumann, absorbentes).

### 6. `io/` – Entrada/Salida
- **saver.py**: Guardar snapshots de la evolución temporal.  
- **loader.py**: Cargar simulaciones previas.  
- **config.py**: Configuración desde archivos YAML/JSON (ejecución reproducible).

### 7. `visualization/` – Gráficos y análisis
- **plot_wavefunction.py**: Visualización de las componentes del bispinor.  
- **plot_density.py**: Densidades de probabilidad y corrientes.  
- **animations.py**: Animaciones temporales (propagación, confinamiento).

### 8. `utils/` – Utilidades generales
- **math_ops.py**: Operaciones auxiliares numéricas y algebraicas.  
- **validators.py**: Validación de entradas y parámetros físicos.  
- **constants.py**: Constantes físicas (ħ, c, α, etc.).
- **coolsftuf.py**: Comentarios chistosos. 

---

## `tests/`

Tests unitarios organizados por módulos. Verifican consistencia, conservación de
probabilidad y reproducción de fenómenos físicos.

- **test_potentials.py**: Comprueba la implementación de los potenciales.  
- **test_geometry.py**: Valida discretizaciones espaciales.  
- **test_boundary.py**: Verifica condiciones de frontera.  
- **test_solver_free.py**: Propagación de partícula libre.  
- **test_solver_infinite_well.py**: Cuantización en pozo infinito.  
- **test_solver_klein.py**: Coeficientes de reflexión y transmisión (efecto Klein).  
- **test_solver_coulomb.py**: Niveles del átomo de hidrógeno relativista.  
- **test_solver_yukawa.py**: Estados ligados en potencial de Yukawa.

---

## `examples/`

Ejemplos reproducibles de uso del solver en diferentes escenarios.

- **free_particle.py**: Partícula libre en 1+1D.  
- **infinite_well.py**: Cuantización en un pozo infinito.  
- **klein_paradox.py**: Simulación del efecto Klein.  
- **hydrogen_atom.py**: Estructura fina del átomo de hidrógeno.  
- **yukawa_bound_states.py**: Estados ligados en potencial de Yukawa.

---

## `docs/`

Documentación del proyecto.

- **index.md**: Página principal de la documentación.  
- **api_reference.md**: Referencia de clases, funciones y módulos.  
- **theory_background.md**: Fundamentos teóricos de la ecuación de Dirac y fenómenos relativistas.

---
