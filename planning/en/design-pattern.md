# Software Architecture and Design Patterns of `dirac_solver`

> 🌐 **Language / Idioma:** **[Español](../es/design-pattern.md)** | **English**

This document describes the software architecture of the `dirac_solver` library. The design is built upon canonical object-oriented design patterns to ensure that the codebase is modular, extensible, maintainable, and intuitive to use. The primary goal is a strict **Separation of Concerns**: the physical problem definition is cleanly decoupled from the numerical algorithms used to solve it and the utilities used to analyze or store the results.

## Table of Contents

1. [Summary - Comparative Table: Patterns and Problems Solved](#1-comparative-table-patterns-and-problems-solved)
2. [Builder Pattern - Simulation Configuration](#2-builder-pattern---simulation-configuration)
3. [Strategy Pattern - Algorithmic Flexibility](#3-strategy-pattern---algorithmic-flexibility)
4. [Template and Factory Method Patterns - Extensibility of Physical Components](#4-template-and-factory-method-patterns---extensibility-of-physical-components)
5. [Observer Pattern - Visualization and Monitoring](#5-observer-pattern---visualization-and-monitoring)
6. [Facade Pattern - Simplified I/O and Animation Interface](#6-facade-pattern---simplified-io-and-animation-interface)
7. [Decorator Pattern - Dynamic Feature Extension](#7-decorator-pattern--dynamic-feature-extension)
8. [Adapter Pattern - Python/C++ and External Library Interoperability](#8-adapter-pattern--interoperability-with-external-libraries)
9. [Ecosystem - Relationships Between Patterns](#9-relationships-between-patterns)
10. [Alternatives Considered and Discarded](#10-alternatives-considered-and-discarded)
11. [Conclusion](#11-conclusion)

---

## 1. Comparative Table: Patterns and Problems Solved

| Pattern | Problem Solved | Implementation in `dirac_solver` |
| :--- | :--- | :--- |
| **Builder** | Step-by-step construction of complex objects without telescoping constructors. | `DiracProblemBuilder` validates and builds `SimulationProblem`. |
| **Strategy** | Interchangeable algorithms (time integrators, boundary conditions) without modifying client code. | `TimeIntegrator` (`FDTDLeapfrogIntegrator`) and `BoundaryCondition` (`PeriodicBoundary`, `AbsorbingBoundary`). |
| **Template + Factory Method** | Coherent extension and instantiation of physical components. | `Potential` (C++), `InitialState` (Python), and string-based boundary instantiation in `DiracProblemBuilder.build()`. |
| **Observer** | Decoupling time evolution from observable computation and snapshot recording. | `Observables` and `HDF5Storage` notified periodically by `DiracSolver.run_simulation()`. |
| **Facade** | Hiding complex HDF5 dataset management and Matplotlib animation pipelines behind a simple interface. | `HDF5Storage`, `create_animation`, and `create_animation_1d`. |
| **Decorator** | Dynamically adding cross-cutting concerns (logging, timing, validation). | Runtime profiling and norm conservation checks. |
| **Adapter** | Bridging Python (`dirac_solver`) wrappers with C++ (`_core`) classes or external backends. | Wrapper classes in `potentials.py` (`core_potential`) and `pybind11` bindings. |

---

## 2. Builder Pattern - Simulation Configuration

**Purpose:** Construct a complex configuration object (`SimulationProblem`) step by step through a fluent, readable API that separates construction and validation from representation.

**Application:** Users define a complete simulation by assembling its constituent components (`Grid`, `InitialState`, `Potential`, `BoundaryCondition`, and time parameters) without a brittle constructor taking many positional arguments. `DiracProblemBuilder` serves as the primary entry point and validates completeness inside `.build()`.

**Actual Code Example (`dirac_solver.core`):**

```python
import dirac_solver as ds

grid = ds.Grid(shape=(400,), spacing=0.05)
spinor = ds.ConstantSpinor(momentum=[2.0, 0.0, 0.0])
packet = ds.GaussianPacket(constant_spinor=spinor, center=[-2.0, 0.0, 0.0], spatial_width=0.8)

problem = (
    ds.DiracProblemBuilder()
    .set_grid(grid)
    .set_initial_state(packet)
    .set_potential(ds.CoulombPotential(Z=1.0, epsilon=0.1))
    .set_boundary_condition("absorbing")
    .set_time_parameters(time_step=0.01, total_time=5.0)
    .build()
)

solver = ds.DiracSolver(problem)
solver.run_simulation()
```

---

## 3. Strategy Pattern - Algorithmic Flexibility

**Purpose:** Define a family of algorithms, encapsulate each one behind a common interface, and make them interchangeable independently of the client using them.

**Application:** This is the foundational pattern of the C++ numerical core in `cpp_src/`:
1. **Time Integration (`TimeIntegrator`):** Defines the abstract interface with `step()` and `get_name()`. The concrete strategy `FDTDLeapfrogIntegrator` implements the explicit centered-difference Leapfrog scheme. Via the `PyTimeIntegrator` trampoline class in `pybind11`, custom integrators can even be subclassed in Python.
2. **Boundary Conditions (`BoundaryCondition`):** Defines the abstract interface `apply(psi, grid)`. Concrete strategies `PeriodicBoundary` and `AbsorbingBoundary` can be swapped seamlessly without altering `DiracSolver`.

---

## 4. Template and Factory Method Patterns - Extensibility of Physical Components

**Purpose:**
- **Template Method:** Defines the abstract contract of a physical component in a base class while delegating concrete mathematical evaluation to subclasses.
- **Factory Method:** Centralizes object instantiation from high-level identifiers.

**Application:**
- In C++, the abstract `Potential` class mandates `evaluate(r)` and `get_name()` across `FreeParticle`, `CoulombPotential`, `YukawaPotential`, `InfiniteWellPotential`, and `ScalarPotential`.
- In Python, `InitialState` (`initial_state.py`) defines the abstract contract `evaluate(grid_position)` and `evaluate_on_grid(grid)`, implemented by `GaussianPacket` and `PlaneWave`.
- In `DiracProblemBuilder.build()`, string identifiers `"periodic"` and `"absorbing"` act as a factory method that instantiates `_core.PeriodicBoundary()` or `_core.AbsorbingBoundary(0.3, 20)`.

---

## 5. Observer Pattern - Visualization and Monitoring

**Purpose:** Establish a notification mechanism between the simulation loop and analysis/storage handlers so that state updates automatically trigger observable evaluation and disk snapshots without coupling the C++ integrator to I/O logic.

**Application:** Inside `DiracSolver.run_simulation(storage_handler, save_every_n_steps, observables_handler)`, each time step notifies `Observables` (`compute_all(current_time)`) and every $N$ steps notifies `HDF5Storage` (`write_snapshot(psi, current_time)`).

---

## 6. Facade Pattern - Simplified I/O and Animation Interface

**Purpose:** Provide a unified, simplified interface to a complex subsystem.

**Application:** `dirac_solver/storage.py` encapsulates HDF5 group creation, metadata attributes, chunking, and dynamic dataset resizing behind the `HDF5Storage` class (supporting Python's `with` context manager) and the high-level animation functions `create_animation` and `create_animation_1d`.

---

## 7. Decorator Pattern – Dynamic Feature Extension

**Purpose:** Attach additional responsibilities to an object or method dynamically without modifying its underlying implementation.

**Application:** Allows wrapping solver methods or potentials with runtime profiling, detailed logging, or numerical assertions (such as verifying wavepacket norm conservation).

```python
@log_runtime
def run_simulation(self, ...):
    # Method decorated with execution timing
    ...
```

---

## 8. Adapter Pattern – Interoperability with External Libraries

**Purpose:** Convert the interface of a class into another interface expected by the client, enabling classes with distinct interfaces to collaborate.

**Application:**
In `dirac_solver/potentials.py`, Python classes (`FreeParticle`, `CoulombPotential`, `YukawaPotential`, `InfiniteWellPotential`, `ScalarPotential`) act as lightweight adapters exposing `.core_potential` to provide the underlying C++ `_core` objects while converting Python sequences (`tuple`, `np.ndarray`) into `std::vector<double>`. This pattern also enables future adapters for external backends (SciPy, CuPy, PETSc).

---

## 9. Relationships Between Patterns

The patterns work together to form a cohesive architecture:

- **Builder → Strategy / Template-Factory:** `DiracProblemBuilder` assembles `SimulationProblem` by combining the selected `BoundaryCondition` strategy with `InitialState` and `Potential` instances.
- **Strategy ↔ Observer:** `DiracSolver` advances the `FDTDLeapfrogIntegrator.step()` strategy and immediately notifies `Observables` and `HDF5Storage`.
- **Facade ↔ Adapter:** `DiracSolver` extracts `core_potential` and `core_boundary` via property adapters and delegates persistence to the `HDF5Storage` facade.

---

## 10. Alternatives Considered and Discarded

### 1. Builder
- **Alternative:** Telescoping constructors with many positional parameters.
- **Discarded because:** They produce error-prone, hard-to-read client code and complicate validation.

### 2. Strategy
- **Alternative:** Boolean flags or large `if/elif/else` blocks inside the time-stepping loop.
- **Discarded because:** Violates the Open/Closed Principle (OCP) and tightly couples high-level orchestration to specific numerical schemes.

### 3. Template + Factory Method
- **Alternative:** Standalone functions without a common base class for potentials and initial states.
- **Discarded because:** Prevents C++ polymorphism (`std::shared_ptr<Potential>`) and breaks API uniformity.

### 4. Observer
- **Alternative:** Computing observables and calling `h5py` directly inside the C++ integrator.
- **Discarded because:** Couples the high-speed C++ engine to I/O libraries and degrades performance when storage is not needed.

### 5. Facade
- **Alternative:** Requiring users to manually create and resize `h5py` datasets at every time step.
- **Discarded because:** Dramatically increases boilerplate and the learning curve for end users.

---

## 11. Conclusion

Applying canonical design patterns in `dirac_solver` ensures:
- **Modularity:** Every Python and C++ component has a single, well-defined responsibility.
- **Extensibility:** New potentials, initial states, boundary conditions, or time integrators can be added without modifying existing code.
- **Maintainability:** Low coupling between the C++17 numerical core and Python analysis utilities.
- **Performance & Expressiveness:** Native C++ computational speed paired with a clean, fluent Python scientific API.
