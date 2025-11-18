"""
@file initial_state.py
@brief Creacion del estado inicial sin geometria del espacio.
"""

"""
@ brief librerias importadas.
"""
import numpy as np

# importamos abc para crear una plantilla para los estados iniciales
from abc import ABC, abstractmethod
#Importar constantes 
from . import c, hbar, electron_mass, sigma_1, sigma_2, sigma_3


"""
@brief Clase Base Abstracta para todos los estados iniciales.
Define el "contrato" que todos los estados iniciales deben cumplir.
"""

class InitialState(ABC):

    @abstractmethod
    def evaluate(self, grid_position):
        """
        @brief Calcula el spinor en un punto especifico del espacio.
        """
        pass

    @abstractmethod
    def evaluate_on_grid(self, grid):
        """
        @brief Calcula el spinor en toda la malla.
        """
        pass

"""
@brief Aqui se prepara el estado inicial (sin considerar la geometria del espacio)
"""
class ConstantSpinor:
    def __init__(self, spin_polarization=[0,0,1], spin_superposition=[1 + 0j, 0 + 0j], momentum=[1,0,0], mass=electron_mass):

        """
        @brief se normaliza el vector (al necesitarse un vector unitario)
        """
        norm_n = np.linalg.norm(spin_polarization)
        self.spin_polarization = np.array(spin_polarization) / norm_n if norm_n > 0 else np.array([0,0,1])

        """
        @brief se comprueba que la suma del modulo al cuadrado sea 1
        """
        norm_s = np.linalg.norm(spin_superposition)
        self.spin_superposition = np.array(spin_superposition, dtype=np.complex128) / norm_s if norm_s > 0 else np.array([1,0])

        self.momentum = np.array(momentum)
        self.mass = mass

        self._base()

        self._pauli_spinor()

        self._constant_spinor()

    def _base(self):
        theta = np.arccos(self.spin_polarization[2])
        phi = np.arctan2(self.spin_polarization[1],self.spin_polarization[0])


        """
        @brief Construcción base (vector columna 2x1) del spinor de Pauli
        """
        self.chi_up = np.array([
            [np.cos(theta /2)],
            [np.exp(1j * phi) * np.sin(theta / 2)]
            ], dtype=np.complex128)

        self.chi_down = np.array([
            [np.sin(theta /2)],
            [-np.exp(1j * phi) * np.cos(theta / 2)]
            ], dtype=np.complex128)


    def _pauli_spinor(self):
        """
        @brief Construcción del spinor de pauli
        """
        alpha = self.spin_superposition[0]
        beta = self.spin_superposition[1]
        self.pauli_spinor = (alpha * self.chi_up) + (beta * self.chi_down)

    def _constant_spinor(self):


        #proyeccion del spin en la direccion del momentum 
        norm_momentum = np.linalg.norm(self.momentum)

        if norm_momentum == 0:
            proyection_spin = np.zeros((2,2), dtype=np.complex128)
        else:
            proyection_spin = ((self.momentum[0] * sigma_1 +
                                self.momentum[1] * sigma_2 +
                                self.momentum[2] * sigma_3) / norm_momentum)

        # Energía relativista
        energy = np.sqrt((norm_momentum)**2 + (self.mass)**2)

        # Partes superior e inferior del spinor
        upper_part = np.sqrt(energy + self.mass) * self.pauli_spinor
        lower_part = np.sqrt(energy - self.mass) * (proyection_spin @ self.pauli_spinor)
        """
        @brief Construcción del spinor constante (por partes)
        """
        self.constant_spinor = np.vstack((upper_part, lower_part))


"""
@brief Clase que construye el estado inicial como un paquete de ondas gausianas
"""
class GaussianPacket(InitialState):
    def __init__(self, constant_spinor, center = [0,0,0], spatial_width = 1):
        """
        @brief el spinor constante entra como parametro
        """
        self.constant_spinor = constant_spinor
        self.center = np.array(center)
        self.spatial_width = spatial_width

    def evaluate(self, grid_position):
        """
        @brief calcula el estado base en un punto exacto
        """
        x = np.array(grid_position)
        p0 = self.constant_spinor.momentum
        x0 = self.center

        # Pad the position vector 'x' to match the dimension of momentum and center vectors
        if x.shape[0] < p0.shape[0]:
            padded_x = np.zeros(p0.shape[0], dtype=float)
            padded_x[:x.shape[0]] = x
            x = padded_x

        if x0.shape[0] < p0.shape[0]:
            padded_x0 = np.zeros(p0.shape[0], dtype=float)
            padded_x0[:x0.shape[0]] = x0
            x0 = padded_x0

        sigma0 = self.spatial_width
        u_p0 = self.constant_spinor.constant_spinor

        p0_dot_x = np.dot(p0, x)
        exponent_norm = np.linalg.norm(x - x0)**2

        spatial_part = np.exp(1j * p0_dot_x) * np.exp(-exponent_norm / (4 * sigma0**2))

        dirac_spinor = u_p0 * spatial_part

        """
        @brief Devuelve el spinor calculado en el punto
        """
        return dirac_spinor

    def evaluate_on_grid(self, grid):
        """
        @brief Calcula el estado inicial en toda la malla.
        @param grid: Objeto Grid sobre el que se evaluará el paquete.
        @return: Un array de NumPy con la forma (n_points, 4)
        """
        n_points = np.prod(grid.shape)
        psi_0 = np.zeros((n_points, 4), dtype=np.complex128)

        coords_flat = grid.coords.reshape(n_points, grid.dim)

        for i in range(n_points):
            position = coords_flat[i]
            spinor_at_point = self.evaluate(position)
            psi_0[i, :] = spinor_at_point.flatten()

        return psi_0


"""
@brief Clase que construye el estado inicial como una Onda Plana.
Hereda de InitialState
"""
class PlaneWave(InitialState):
    def __init__(self, constant_spinor):
        """
        @brief el spinor constante entra como parametro
        """
        self.constant_spinor = constant_spinor

    def evaluate(self, grid_position):
        """
        @brief calcula el estado base en un punto exacto
        """
        x = np.array(grid_position)
        p0 = self.constant_spinor.momentum

        if len(x) < len(p0):
            padded_x = np.zeros(len(p0), dtype=float)
            padded_x[:len(x)] = x.flatten()
            x = padded_x

        u_p0 = self.constant_spinor.constant_spinor

        p0_dot_x = np.dot(p0, x)

        #parte espacial de la Onda plana
        spacial_part = np.exp(1j * p0_dot_x)

        dirac_spinor = u_p0 * spacial_part

        """
        @brief Devuelve el spinor calculado en el punto
        """
        return dirac_spinor


    def evaluate_on_grid(self, grid):
        """
        @brief Calcula el estado inicial en toda la malla
        @param grid: Objeto Grid sobre el que se evaluará el paquete.
        @return: Un array de NumPy con la forma (n_points, 4)
        """

        n_points = np.prod(grid.shape)
        psi_0 = np.zeros((n_points, 4), dtype = np.complex128)

        coords_flat = grid.coords.reshape(n_points, grid.dim)

        for i in range(n_points):
            position = coords_flat[i]
            spinor_at_point = self.evaluate(position)
            psi_0[i, :] = spinor_at_point.flatten()

        return psi_0
