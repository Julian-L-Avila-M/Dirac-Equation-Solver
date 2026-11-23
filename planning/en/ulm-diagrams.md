# UML Class Diagrams for the `dirac_solver` Architecture

> 🌐 **Language / Idioma:** **[Español](../es/ulm-diagrams.md)** | **English**

This document presents the UML class diagrams for the primary object-oriented design patterns implemented in the `dirac_solver` library.

---

## 1. Builder Pattern

**Application:** `DiracProblemBuilder` constructs and validates a `SimulationProblem` instance step by step, providing a fluent configuration API before initializing `DiracSolver`.

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

## 2. Strategy Pattern (Time Integrators and Boundary Conditions)

**Application:** `DiracSolver` delegates time evolution to the C++ `TimeIntegrator` interface (`FDTDLeapfrogIntegrator`) and boundary handling to `BoundaryCondition` (`PeriodicBoundary`, `AbsorbingBoundary`).

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

## 3. Template Method Pattern & Potential / Initial State Hierarchies

**Application:** The abstract base classes `Potential` (in C++) and `InitialState` (in Python) define the uniform contract implemented by all concrete scalar potentials and initial spinor distributions.

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

## 4. Observer Pattern (Observable Monitoring and Snapshot Recording)

**Application:** During `DiracSolver.run_simulation()`, the solver notifies `Observables` at every time step and `HDF5Storage` every $N$ steps without coupling the C++ integrator to analysis or disk I/O.

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

## 5. Facade Pattern (HDF5 Storage and Animation Generation)

**Application:** `HDF5Storage` and the functions `create_animation` / `create_animation_1d` provide a clean facade hiding the low-level management of extensible `h5py` datasets and `matplotlib.animation.FuncAnimation`.

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

## 6. Decorator Pattern

**Application:** Enables lightweight instrumentation (execution timing, logging, probability norm conservation checks) around solver execution without altering its core implementation.

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

## 7. Adapter Pattern

**Application:** The wrapper classes in `dirac_solver/potentials.py` adapt Python's interface to the `pybind11`-bound C++ classes (`_core`), and the architecture supports adapting external numerical backends (SciPy, CuPy, PETSc).

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
