# Diagramas UML para la Arquitectura de dirac_solver

Este documento contiene los diagramas de clase UML para los principales patrones
de diseño utilizados en la librería dirac_solver.


## 1. Patrón Builder

Aplicación: El DiracProblemBuilder construye un objeto SimulationProblem
complejo, permitiendo una configuración fluida y legible.

```text
classDiagram
    class DiracProblemBuilder {
        -problem: SimulationProblem
        +set_geometry(geom)
        +set_potential(pot)
        +set_solver(solver)
        +build(): SimulationProblem
    }
    class SimulationProblem {
        +geometry
        +potential
        +solver
    }
    class Client {
        +create_simulation()
    }

    Client --> DiracProblemBuilder : uses
    DiracProblemBuilder ..> SimulationProblem : creates
```

## 2. Patrón Strategy

Aplicación: El contexto (TimeEvolution) delega la ejecución de un paso de tiempo
a una estrategia concreta (CrankNicolson, SplitOperator) a través de la interfaz
TimeIntegrator.

```text
classDiagram
    class TimeEvolution {
        -integrator: TimeIntegrator
        +set_integrator(integrator)
        +run_step()
    }
    <<interface>> TimeIntegrator
    class TimeIntegrator {
        +step(spinor, hamiltonian, dt)
    }
    class CrankNicolson {
        +step(spinor, hamiltonian, dt)
    }
    class SplitOperator {
        +step(spinor, hamiltonian, dt)
    }

    TimeEvolution o-- TimeIntegrator : has a
    CrankNicolson --|> TimeIntegrator : implements
    SplitOperator --|> TimeIntegrator : implements
```

## 3. Patrones Template y Factory Method

Aplicación: Una PotentialFactory crea instancias de potenciales concretos
(CoulombPotential, YukawaPotential) que heredan de una clase base abstracta
BasePotential (el método plantilla).

```text
classDiagram
    class PotentialFactory {
        +create_potential(name, params): BasePotential
    }
    class BasePotential {
        <<Abstract>>
        +evaluate(coords)
    }
    class CoulombPotential {
        +evaluate(coords)
    }
    class YukawaPotential {
        +evaluate(coords)
    }

    PotentialFactory ..> CoulombPotential : creates
    PotentialFactory ..> YukawaPotential : creates
    CoulombPotential --|> BasePotential
    YukawaPotential --|> BasePotential
```

## 4. Patrón Observer

Aplicación: El Simulation (Subject) notifica a sus Observers
(WavefunctionPlotter, EnergyLogger) cada vez que su estado cambia (ej. después
de un paso de tiempo), sin estar acoplado a ellos.

```text
classDiagram
    class Simulation {
        -observers: list
        +attach(observer)
        +detach(observer)
        +notify()
        +run_step()
    }
    <<interface>> SimulationObserver
    class SimulationObserver {
        +update(simulation_state)
    }
    class WavefunctionPlotter {
        +update(simulation_state)
    }
    class EnergyLogger {
        +update(simulation_state)
    }

    Simulation o-- "0..*" SimulationObserver : maintains
    WavefunctionPlotter --|> SimulationObserver
    EnergyLogger --|> SimulationObserver
```

## 5. Patrón Facade

Aplicación: El cliente interactúa con una IOManager (Facade) simple, la cual
oculta la complejidad del subsistema de entrada/salida (Saver, Loader,
ConfigParser).

```text
classDiagram
    class Client
    class IOManager {
        +save(sim, path)
        +load(path): Simulation
    }
    class Saver {
        -save_to_hdf5()
    }
    class Loader {
        -load_from_hdf5()
    }
    class ConfigParser {
        -parse_yaml()
    }

    Client --> IOManager
    IOManager --> Saver
    IOManager --> Loader
    IOManager --> ConfigParser
```
