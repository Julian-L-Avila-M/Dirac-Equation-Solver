# Dirac-Solver: Solucionador numérico de la Ecuación de Dirac

Este proyecto implementa un solucionador numérico para la ecuación de Dirac en 1D, 2D y 3D. El núcleo computacional está escrito en C++ y expuesto a Python mediante `pybind11`, permitiendo simulaciones eficientes junto con una interfaz simple y flexible.

La ecuación resuelta es:


$$ (i\gamma^\mu \partial_\mu - m)\psi = 0 $$


en unidades naturales, donde el bispinor \\(\psi \in \mathbb{C}^4\\) es un vector columna complejo. El proyecto implementa métodos explícitos tipo FDTD (Leapfrog), potenciales personalizables, condiciones de frontera modulares y manejo de datos en HDF5.

---

## Características principales

- Implementación explícita del esquema **Leapfrog (FDTD)**.
- Mallas 1D/2D/3D mediante la clase `Grid`.
- Interfaz C++ ↔ Python mediante **pybind11**.
- Potenciales configurables: Coulomb, pozo, Yukawa, y potenciales definidos por el usuario.
- Condiciones de frontera intercambiables.
- Módulo Python para estados iniciales, observables, geometría y almacenamiento.
- Ejemplos en Jupyter listos para ejecutar.
- Documentación generada con **Doxygen**.

---

## Estructura del proyecto


dirac_solver/
├── cpp_src/              # Código fuente C++ (núcleo numérico)
├── dirac_solver/         # API Python
├── examples/             # Notebooks de ejemplos
├── docs/                 # Documentación Doxygen (HTML y LaTeX)
├── tests/                # Pruebas unitarias
├── save_data/            # Resultados de simulaciones (HDF5)
├── planning/             # Diseño, requisitos, notas técnicas
└── pyproject.toml        # Configuración del paquete Python


---

## Instalación

### Instalación local editable

```bash
pip install -e .
````

### Compilación del núcleo C++

```bash
mkdir -p cpp_build
cd cpp_build
cmake ../cpp_src
make -j
```

---

## Uso básico

Ejemplo mínimo en Python:

```python
from dirac_solver import core, potentials, initial_state, observables

grid = core.Grid(shape=(200,), spacing=0.05)

V = potentials.Coulomb(Z=1)

psi0 = initial_state.gaussian(grid)

solver = core.TimeIntegrator(grid, V)
psi_t = solver.evolve(psi0, dt=0.001, steps=1000)

obs = observables.Observables(psi_t)
density = obs.probability_density()
```

---

## Documentación

Para compilar la documentación, en el directorio principal ejecutar:

```bash
doxygen
```

La salida se genera en:

```
docs/html/index.html
docs/latex/
```

---

## Ejemplos

En el directorio `examples/` encontrarás notebooks que cubren:

* Partícula libre
* Pozo cuadrado
* Potencial Coulomb
* Potencial Yukawa
* Ondas gaussianas

---

## Contribuir

Las contribuciones son bienvenidas.

* Revisar `CONTRIBUTING.md`
* Mantener consistencia en el estilo del código
* Agregar pruebas unitarias si se añaden características nuevas
* Actualizar la documentación cuando corresponda

---

## Licencia

Este proyecto se distribuye bajo la licencia **MIT**. Ver `LICENSE` para más detalles.

