# Diagramas UML para la Arquitectura de `dirac_solver`

> 🌐 **Idioma / Language:** **Español** | **[English](../en/ulm-diagrams.md)**

Este documento contiene los diagramas de clases UML para los principales patrones de diseño utilizados en la librería `dirac_solver`.

---

## 1. Patrón Builder

**Aplicación:** `DiracProblemBuilder` construye paso a paso un objeto `SimulationProblem` validado, permitiendo una configuración fluida y legible antes de inicializar `DiracSolver`.

```mermaid
classDiagram
    class DiracProblemBuilder {
        -_grid: Grid
        -_initial_state: InitialState
        -_potential: Potential
        -_boundary_condition: BoundaryCondition
        -_time_step: float
        -_total_time: float
        +set_grid(grid) DiracProblemBuilder
        +set_initial_state(initial_state) DiracProblemBuilder
        +set_potential(potential) DiracProblemBuilder
        +set_boundary_condition(bc) DiracProblemBuilder
        +set_time_parameters(time_step, total_time) DiracProblemBuilder
        +build() SimulationProblem
    }
    class SimulationProblem {
        +grid: Grid
        +initial_state: InitialState
        +potential: Potential
        +boundary_condition: BoundaryCondition
        +time_step: float
        +total_time: float
    }
    class DiracSolver {
        +problem: SimulationProblem
        +integrator: FDTDLeapfrogIntegrator
        +run_simulation(storage_handler, save_every_n_steps, observables_handler)
        +get_psi() ndarray
        +plot_probability_density()
    }

    DiracProblemBuilder ..> SimulationProblem : creates
    DiracSolver --> SimulationProblem : uses
```

---

## 2. Patrón Strategy (Integradores Temporales y Condiciones de Frontera)

**Aplicación:** `DiracSolver` delega la evolución temporal a la interfaz C++ `TimeIntegrator` (`FDTDLeapfrogIntegrator`) y el tratamiento de bordes a `BoundaryCondition` (`PeriodicBoundary`, `AbsorbingBoundary`).

```mermaid
classDiagram
    class DiracSolver {
        -integrator: TimeIntegrator
        +run_simulation()
        +get_psi()
    }
    class TimeIntegrator {
        <<interface>>
        +step()* void
        +get_name()* string
    }
    class FDTDLeapfrogIntegrator {
        -grid_: Grid
        -potential_: shared_ptr~Potential~
        -boundary_condition_: shared_ptr~BoundaryCondition~
        -dt_: double
        -mass_: double
        +step() void
        +get_psi() ndarray
        +get_name() string
    }
    class BoundaryCondition {
        <<interface>>
        +apply(psi, grid)* void
        +get_name()* string
    }
    class PeriodicBoundary {
        +apply(psi, grid) void
        +get_name() string
    }
    class AbsorbingBoundary {
        -strength_: double
        -width_: int
        +apply(psi, grid) void
        +get_name() string
    }

    DiracSolver o-- TimeIntegrator : delegates to
    FDTDLeapfrogIntegrator --|> TimeIntegrator : implements
    FDTDLeapfrogIntegrator o-- BoundaryCondition : uses
    PeriodicBoundary --|> BoundaryCondition : implements
    AbsorbingBoundary --|> BoundaryCondition : implements
```

---

## 3. Patrones Template Method y Jerarquía de Potenciales / Estados Iniciales

**Aplicación:** Las clases base abstractas `Potential` (en C++) e `InitialState` (en Python) definen el contrato que implementan todos los potenciales escalares y configuraciones espinoriales iniciales.

```mermaid
classDiagram
    class Potential {
        <<Abstract>>
        +evaluate(r)* double
        +get_name()* string
    }
    class FreeParticle {
        +evaluate(r) double
        +get_name() string
    }
    class CoulombPotential {
        -Z_: double
        -epsilon_: double
        +evaluate(r) double
        +get_name() string
    }
    class YukawaPotential {
        -strength_: double
        -range_: double
        +evaluate(r) double
        +get_name() string
    }
    class InfiniteWellPotential {
        -widths_: vector~double~
        +evaluate(r) double
        +get_name() string
    }
    class ScalarPotential {
        -func_: function
        +evaluate(r) double
        +get_name() string
    }

    FreeParticle --|> Potential
    CoulombPotential --|> Potential
    YukawaPotential --|> Potential
    InfiniteWellPotential --|> Potential
    ScalarPotential --|> Potential
```

---

## 4. Patrón Observer (Monitoreo de Observables y Snapshots)

**Aplicación:** Durante el bucle de `DiracSolver.run_simulation()`, el solver notifica a `Observables` y `HDF5Storage` en los pasos configurados sin acoplar el motor C++ con el análisis o la escritura en disco.

```mermaid
classDiagram
    class DiracSolver {
        +run_simulation(storage_handler, save_every_n_steps, observables_handler)
        +get_psi() ndarray
    }
    class Observables {
        +history: dict
        +compute_all(time)
        +probability_density(psi)
        +charge_density(psi)
        +probability_current(psi)
        +expected_value_position(prob_density)
        +expected_value_momentum(psi)
        +expected_value_energy(psi)
        +get_history(key) ndarray
    }
    class HDF5Storage {
        +filepath: str
        +write_snapshot(psi, time)
        +write_observables(observables_handler)
        +close()
    }

    DiracSolver --> Observables : notifies every step
    DiracSolver --> HDF5Storage : notifies every N steps
    HDF5Storage --> Observables : persists history
```

---

## 5. Patrón Facade (Almacenamiento HDF5 y Animaciones)

**Aplicación:** `HDF5Storage` y las funciones `create_animation` / `create_animation_1d` ofrecen una fachada limpia que oculta la gestión interna de datasets extensibles de `h5py` y de `matplotlib.animation.FuncAnimation`.

```mermaid
classDiagram
    class Client
    class HDF5Storage {
        +write_snapshot(psi, time)
        +write_observables(obs)
        +close()
    }
    class AnimationFacade {
        +create_animation(storage_path, output_path, interval)
        +create_animation_1d(storage_path, output_path, interval)
    }
    class h5py_File {
        +create_dataset()
        +create_group()
        +resize()
    }
    class Matplotlib_FuncAnimation {
        +save()
    }

    Client --> HDF5Storage
    Client --> AnimationFacade
    HDF5Storage --> h5py_File
    AnimationFacade --> h5py_File
    AnimationFacade --> Matplotlib_FuncAnimation
```

---

## 6. Patrón Decorator

**Aplicación:** Permite añadir instrumentación ligera (medición de tiempos, logging, validación de conservación de probabilidad) alrededor de la ejecución del solver sin alterar su implementación base.

```mermaid
classDiagram
    class SolverInterface {
        <<interface>>
        +run_simulation()
    }
    class DiracSolver {
        +run_simulation()
    }
    class SolverDecorator {
        -wrapped: SolverInterface
        +run_simulation()
    }
    class TimingDecorator {
        +run_simulation()
    }
    class NormValidationDecorator {
        +run_simulation()
    }

    DiracSolver --|> SolverInterface
    SolverDecorator --|> SolverInterface
    TimingDecorator --|> SolverDecorator
    NormValidationDecorator --|> SolverDecorator
    SolverDecorator o-- SolverInterface : wraps
```

---

## 7. Patrón Adapter

**Aplicación:** Las clases envoltorio en `dirac_solver/potentials.py` adaptan la interfaz de Python a las clases C++ enlazadas con `pybind11` (`_core`), y el diseño permite adaptar futuros backends externos (SciPy, CuPy, PETSc).

```mermaid
classDiagram
    class PythonPotentialWrapper {
        -_potential: _core.Potential
        +core_potential() _core.Potential
        +evaluate(r) float
        +get_name() str
    }
    class CppCorePotential {
        +evaluate(vector~double~) double
        +get_name() string
    }
    class DiracSolver {
        +__init__(problem)
    }

    DiracSolver --> PythonPotentialWrapper : extracts core_potential
    PythonPotentialWrapper --> CppCorePotential : adapts
```
