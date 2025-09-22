Arquitectura y Patrones de Diseño de dirac_solver

Este documento describe la arquitectura de software de la librería dirac_solver.
El diseño se basa en un conjunto de patrones de diseño canónicos para garantizar
que el código sea modular, extensible, mantenible y fácil de usar. El objetivo
principal es lograr una clara separación de conceptos (Separation of Concerns):
la definición del problema físico se mantiene desacoplada de los algoritmos
numéricos utilizados para resolverlo.
Tabla de Contenidos

  1. Patrón Builder - Configuración de la Simulación

  2. Patrón Strategy - Flexibilidad en los Algoritmos

  3. Patrones Template y Factory Method - Extensibilidad de Componentes Físicos

  4. Patrón Observer - Visualización y Monitoreo

  5. Patrón Facade - Interfaz Simplificada de I/O

---

1. Patrón Builder - Configuración de la Simulación

Propósito: Construir un objeto complejo (SimulationProblem) paso a paso,
proporcionando una API fluida y legible que separa la construcción de la
representación.

Aplicación: El usuario final define un problema completo ensamblando sus partes
constituyentes (geometría, potencial, condiciones, etc.) sin necesidad de un
constructor con múltiples y confusos argumentos. Un DiracProblemBuilder es el
punto de entrada principal para el usuario. Este objeto guía la creación de una
simulación válida y cohesiva.

Código de Ejemplo (examples/hydrogen_atom.py):

# API fluida gracias al Patrón Builder
simulation = (DiracProblemBuilder()
              .set_geometry(Radial(r_max=100, N=1024))
              .set_potential("coulomb", Z=1)
              .set_initial_condition(HydrogenGroundState())
              .set_solver("Arnoldi", num_eigenvalues=5)
              .build())

results = simulation.run()

2. Patrón Strategy - Flexibilidad en los Algoritmos

Propósito: Definir una familia de algoritmos, encapsular cada uno y hacerlos
intercambiables. Permite que el algoritmo varíe independientemente del cliente
que lo utiliza.

Aplicación: Este es el patrón más importante para el núcleo numérico en
solvers/. Permite al usuario cambiar el método de integración temporal (ej.
CrankNicolson vs. SplitOperator) o las condiciones de frontera (ej. Dirichlet
vs. Absorbing) sin modificar el código del bucle de simulación principal.

3. Patrones Template y Factory Method - Extensibilidad de Componentes Físicos

Propósito:

  - Template Method: Define el esqueleto de un algoritmo en una clase base,
	delegando la implementación de ciertos pasos a las subclases.
  - Factory Method: Proporciona una interfaz para crear objetos, pero permite
	que las subclases alteren el tipo de objetos que se crearán.
	
Aplicación: Se combinan en el módulo potentials/ (y análogamente en geometry/ y
conditions/).

  - potentials/base.py define una BasePotential (el Template) que requiere que
	todas las subclases implementen un método evaluate().
  - Una función create_potential() (el Factory Method) centraliza la lógica de
	instanciación, permitiendo crear potenciales a partir de un string (ej. desde
	un archivo de configuración).

4. Patrón Observer - Visualización y Monitoreo

Propósito: Definir una dependencia uno-a-muchos entre objetos, de modo que
cuando un objeto (Subject) cambia de estado, todos sus dependientes (Observers)
son notificados y actualizados automáticamente.

Aplicación: Desacopla el motor de la simulación de los componentes de
visualización y registro de datos. El objeto Simulation (Subject) no necesita
saber nada sobre cómo se muestran los datos. Simplemente notifica a sus
observadores (ej. WavefunctionPlotter, EnergyLogger) en cada paso de tiempo.

5. Patrón Facade - Interfaz Simplificada de I/O

Propósito: Proporcionar una interfaz unificada y simplificada a un conjunto de
interfaces en un subsistema.

Aplicación: El módulo io/ puede contener lógica compleja para guardar en
diferentes formatos (HDF5, npz), cargar, y parsear archivos de configuración. El
patrón Facade oculta esta complejidad detrás de una única clase IOManager con
métodos sencillos como save(simulation, path) y load(path).
