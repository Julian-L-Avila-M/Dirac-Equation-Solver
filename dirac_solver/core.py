import numpy as np
from .geometry import Grid
from .initial_state import GaussianPacket

class SimulationProblem:
    """
    Clase de datos para contener todos los parámetros para una simulación de Dirac.
    Este objeto es construido por el DiracProblemBuilder.
    """
    def __init__(self, grid, initial_state, potential, boundary_condition, time_step, total_time):
        self.grid = grid
        self.initial_state = initial_state
        self.potential = potential
        self.boundary_condition = boundary_condition
        self.time_step = time_step
        self.total_time = total_time

class DiracProblemBuilder:
    """
    Implementa el patrón Builder para construir un objeto SimulationProblem.
    Esto proporciona una API fluida y legible para configurar una simulación.
    """
    def __init__(self):
        self._grid = None
        self._initial_state = None
        self._potential = None
        self._boundary_condition = None
        self._time_step = None
        self._total_time = None

    def set_grid(self, grid: Grid):
        """Establece la malla espacial para la simulación."""
        self._grid = grid
        return self

    def set_initial_state(self, initial_state: GaussianPacket):
        """Establece la configuración inicial del campo espinorial."""
        self._initial_state = initial_state
        return self

    def set_potential(self, potential):
        """Establece el potencial escalar para la simulación."""
        self._potential = potential
        return self

    def set_boundary_condition(self, boundary_condition):
        """Establece la estrategia de condición de borde para la simulación."""
        self._boundary_condition = boundary_condition
        return self

    def set_time_parameters(self, time_step: float, total_time: float):
        """Establece el paso de tiempo (dt) y el tiempo total de la simulación."""
        self._time_step = time_step
        self._total_time = total_time
        return self

    def build(self) -> SimulationProblem:
        """
        Construye y devuelve el objeto SimulationProblem configurado.
        Realiza una validación para asegurar que todos los parámetros requeridos estén establecidos.
        """
        if self._grid is None:
            raise ValueError("La malla debe ser establecida antes de construir el problema.")
        if self._initial_state is None:
            raise ValueError("El estado inicial debe ser establecido antes de construir.")
        if self._time_step is None or self._total_time is None:
            raise ValueError("Los parámetros de tiempo deben ser establecidos.")
        if self._boundary_condition is None:
            raise ValueError("La condición de borde debe ser establecida.")

        # Por ahora, el potencial es opcional (por defecto, partícula libre si no se establece)
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

import matplotlib.pyplot as plt
from . import _core, electron_mass

class DiracSolver:
    """
    La clase principal del solver que orquesta la simulación.
    Toma un SimulationProblem e inicializa el backend de C++.
    """
    def __init__(self, problem: SimulationProblem):
        self.problem = problem

        # Generar la función de onda inicial en la malla
        psi_0 = problem.initial_state.evaluate_on_grid(problem.grid)

        # Crear objeto Grid de C++
        cpp_grid = _core.Grid(problem.grid.shape, problem.grid.spacing)

        # Instanciar el integrador FDTD de C++
        self.integrator = _core.FDTDLeapfrogIntegrator(
            psi_0,
            cpp_grid,
            problem.potential,
            problem.boundary_condition,
            problem.time_step,
            electron_mass # Usando la constante global
        )
        print(f"DiracSolver inicializado con el motor C++ '{self.integrator.get_name()}'.")


    def run_simulation(self):
        """
        Ejecuta el bucle de evolución temporal llamando a la función de paso de C++.
        """
        num_steps = int(self.problem.total_time / self.problem.time_step)
        print(f"Ejecutando simulación por {num_steps} pasos...")
        for i in range(num_steps):
            self.integrator.step()
            if (i + 1) % 10 == 0:
                print(f"  Paso {i+1}/{num_steps} completado.")
        print("Simulación finalizada.")

    def get_psi(self):
        """Devuelve el campo espinorial actual desde el backend de C++."""
        return self.integrator.get_psi()

    def plot_probability_density(self):
        """
        Calcula y grafica la densidad de probabilidad del campo espinorial.
        """
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