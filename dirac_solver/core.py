import numpy as np
import time
from .geometry import Grid

from .initial_state import InitialState
import matplotlib.pyplot as plt
from . import _core, electron_mass

###############################################################
## @class SimulationProblem
## @brief Contenedor de datos que agrupa todos los parámetros necesarios
##        para una simulación del ecuación de Dirac.
##
## Esta clase no ejecuta la simulación directamente. Su propósito es
## almacenar de forma consistente todos los parámetros de entrada del problema:
## geometría, estado inicial, potencial, condiciones de borde y parámetros de tiempo.
##
## @note Este objeto es usualmente construido por @ref DiracProblemBuilder.
###############################################################
class SimulationProblem:
    """
    Clase de datos para contener todos los parámetros para una simulación de Dirac.
    Este objeto es construido por el DiracProblemBuilder.
    """
    def __init__(self, grid, initial_state, potential, boundary_condition, time_step, total_time):
        ## @brief Malla espacial de la simulación.
        self.grid = grid

        ## @brief Estado inicial (paquete gaussiano u otro tipo de campo espinorial).
        self.initial_state = initial_state

        ## @brief Potencial escalar aplicado al campo de Dirac.
        self.potential = potential

        ## @brief Condición de borde usada en los límites espaciales.
        self.boundary_condition = boundary_condition

        ## @brief Paso de tiempo utilizado en la integración temporal.
        self.time_step = time_step

        ## @brief Tiempo total de evolución de la simulación.
        self.total_time = total_time


###############################################################
## @class DiracProblemBuilder
## @brief Implementación del patrón Builder para crear objetos @ref SimulationProblem.
##
## Esta clase ofrece una interfaz fluida para configurar los componentes
## de una simulación del ecuación de Dirac. Facilita el proceso de validación
## y permite un flujo de construcción más legible.
##
## Ejemplo de uso:
## @code{.python}
## builder = DiracProblemBuilder()
## problem = (builder
##           .set_grid(my_grid)
##           .set_initial_state(my_packet)
##           .set_potential(V)
##           .set_boundary_condition("absorbing")
##           .set_time_parameters(dt, T)
##           .build())
## @endcode
###############################################################
class DiracProblemBuilder:
    def __init__(self):
        self._grid = None
        self._initial_state = None
        self._potential = None
        self._boundary_condition = None
        self._time_step = None
        self._total_time = None

    ## @brief Define la malla espacial a utilizar.
    ## @param grid Objeto @ref Grid que describe el dominio espacial.
    ## @return Referencia al propio builder para encadenamiento de métodos.
    def set_grid(self, grid: Grid):
        self._grid = grid
        return self

    ## @brief Define el estado inicial del campo espinorial.
    ## @param initial_state Instancia de @ref GaussianPacket u otro estado inicial compatible.
    ## @return Referencia al propio builder.
    def set_initial_state(self, initial_state: InitialState):
        """Establece la configuración inicial del campo espinorial."""
        self._initial_state = initial_state
        return self

    ## @brief Define el potencial escalar.
    ## @param potential Potencial espacialmente dependiente o constante (puede ser None).
    ## @return Referencia al propio builder.
    def set_potential(self, potential):
        self._potential = potential
        return self

    ## @brief Define la condición de borde.
    ## @param boundary_condition Cadena o estructura que especifica el tipo de condición (e.g. "absorbing").
    ## @return Referencia al propio builder.
    def set_boundary_condition(self, boundary_condition):
        self._boundary_condition = boundary_condition
        return self

    ## @brief Define los parámetros de tiempo de la simulación.
    ## @param time_step Paso de tiempo (Δt) utilizado por el integrador.
    ## @param total_time Tiempo total de evolución.
    ## @return Referencia al propio builder.
    def set_time_parameters(self, time_step: float, total_time: float):
        self._time_step = time_step
        self._total_time = total_time
        return self

    ## @brief Construye el objeto @ref SimulationProblem con los parámetros actuales.
    ## @return Instancia de @ref SimulationProblem.
    ## @throws ValueError Si algún parámetro requerido no fue establecido.
    ## @warning Si el potencial no fue definido, se asume partícula libre (V=0).
    def build(self) -> SimulationProblem:
        if self._grid is None:
            raise ValueError("La malla debe ser establecida antes de construir el problema.")
        if self._initial_state is None:
            raise ValueError("El estado inicial debe ser establecido antes de construir.")
        if self._time_step is None or self._total_time is None:
            raise ValueError("Los parámetros de tiempo deben ser establecidos.")
        if self._boundary_condition is None:
            raise ValueError("La condición de borde debe ser establecida.")

        if self._potential is None:
            print("Advertencia: Potencial no establecido. Usando partícula libre (V=0) por defecto.")

        return SimulationProblem(
                grid=self._grid,
                initial_state=self._initial_state,
                potential=self._potential,
                boundary_condition=self._boundary_condition,
                time_step=self._time_step,
                total_time=self._total_time,
                )


###############################################################
## @class DiracSolver
## @brief Clase principal que orquesta la ejecución numérica del problema de Dirac.
##
## Este solver inicializa la función de onda, comunica los datos al backend C++,
## ejecuta el bucle de integración temporal (FDTD) y proporciona utilidades
## para análisis y visualización del resultado.
###############################################################
class DiracSolver:
    ## @brief Constructor del solver.
    ## @param problem Instancia de @ref SimulationProblem previamente configurada.
    ## @note Internamente inicializa el integrador C++ (FDTDLeapfrogIntegrator).
    def __init__(self, problem: SimulationProblem):
        self.problem = problem

        # Evaluar estado inicial sobre la malla
        psi_0 = problem.initial_state.evaluate_on_grid(problem.grid)

        self._initial_psi_for_storage = psi_0
        cpp_grid = _core.Grid(problem.grid.shape, problem.grid.spacing)

        # Inicializar el integrador Leapfrog de C++
        self.integrator = _core.FDTDLeapfrogIntegrator(
            psi_0,
            cpp_grid,
            problem.potential,
            problem.boundary_condition,
            problem.time_step,
            electron_mass
        )

        print(f"DiracSolver inicializado con el motor C++ '{self.integrator.get_name()}'.")

    ## @brief Ejecuta la simulación completa de tiempo.
    ## @details Llama iterativamente al método `step()` del integrador C++.
    ## @note Imprime información de progreso cada 10 pasos.
    def run_simulation(self):
        num_steps = int(self.problem.total_time / self.problem.time_step)
        dt = self.problem.time_step
        print(f"Ejecutando simulación por {num_steps} pasos...")

        # --- Lógica de almacenamiento DELEGADA ---
        if storage_handler:
            try:
                # Guardar estado inicial (t=0)
                storage_handler.write_snapshot(self._initial_psi_for_storage, 0.0)
                print(f"Guardando snapshot inicial (t=0.0).")
            except Exception as e:
                print(f"Error al guardar estado inicial: {e}")
                storage_handler = None # Desactivar si falla

        start_time = time.time() # Medir tiempo

        # --- Bucle de simulación ---
        for i in range(num_steps):

            # 1. Avanzar la simulación C++
            self.integrator.step()

            current_step = i + 1
            current_time = current_step * dt

            # 2. Lógica de guardado DELEGADA
            # Se guarda si es el paso N o si es el último paso
            if storage_handler and (current_step % save_every_n_steps == 0 or current_step == num_steps):
                storage_handler.write_snapshot(self.get_psi(), current_time)

            # 3. Imprimir progreso
            if current_step % 10 == 0:
                print(f"  Paso {current_step}/{num_steps} completado.") 

        end_time = time.time()
        print(f"Simulación finalizada en {end_time - start_time:.2f} segundos.")

        # --- Cerrar el manejador de almacenamiento ---
        if storage_handler:
            storage_handler.close()


    ## @brief Obtiene el estado actual del campo espinorial.
    ## @return np.ndarray con la función de onda actual (espinor completo).
    def get_psi(self):
        return self.integrator.get_psi()

    ## @brief Calcula y grafica la densidad de probabilidad asociada al campo espinorial.
    ##
    ## La representación visual depende de la dimensionalidad de la malla:
    ## - 1D: gráfico lineal
    ## - 2D: mapa de calor
    ## - 3D: nube de puntos coloreada
    ##
    ## @warning Puede consumir mucha memoria para mallas 3D grandes.
    def plot_probability_density(self):
        print("Graficando densidad de probabilidad...")
        psi = self.get_psi()
        rho = np.sum(psi.conj() * psi, axis=1).real

        grid = self.problem.grid
        dim = grid.dim

        if dim == 1:
            x_coords = grid.coords[:, 0]
            plt.figure(figsize=(10, 6))
            plt.plot(x_coords, rho)
            plt.xlabel("Posición (z)")
            plt.ylabel("Densidad de Probabilidad")
            plt.title("Densidad de Probabilidad del Campo Espinorial")
            plt.grid(True)
            plt.show()
        elif dim == 2:
            shape = grid.shape
            rho_grid = rho.reshape(shape)
            plt.figure(figsize=(10, 8))
            plt.imshow(rho_grid.T, origin='lower', aspect='auto',
                       extent=[grid.origin[0], grid.origin[0] + (shape[0] - 1) * grid.spacing[0],
                               grid.origin[1], grid.origin[1] + (shape[1] - 1) * grid.spacing[1]])
            plt.colorbar(label="Densidad de Probabilidad")
            plt.xlabel("Posición (x)")
            plt.ylabel("Posición (y)")
            plt.title("Densidad de Probabilidad 2D")
            plt.show()
        elif dim == 3:
            fig = plt.figure(figsize=(10, 8))
            ax = fig.add_subplot(111, projection='3d')
            coords = grid.coords
            x = coords[:, 0]
            y = coords[:, 1]
            z = coords[:, 2]
            p = ax.scatter(x, y, z, c=rho, cmap='viridis')
            fig.colorbar(p, label="Densidad de Probabilidad")
            ax.set_xlabel("Posición (x)")
            ax.set_ylabel("Posición (y)")
            ax.set_zlabel("Posición (z)")
            plt.title("Densidad de Probabilidad 3D")
            plt.show()
