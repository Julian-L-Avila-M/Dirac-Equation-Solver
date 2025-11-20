"""
@file initial_state.py
@brief Creación del estado inicial sin geometría del espacio.
"""

import numpy as np
from abc import ABC, abstractmethod
from . import c, hbar, electron_mass, sigma_1, sigma_2, sigma_3


class InitialState(ABC):
    """
    Clase Base Abstracta para todos los estados iniciales.
    Define el "contrato" que todos los estados iniciales deben cumplir.
    """
    def __init__(self, mass=electron_mass):
        self.mass = mass

    @abstractmethod
    def evaluate(self, grid_position):
        """
        Calcula el espinor en un punto específico del espacio.
        """
        pass

    @abstractmethod
    def evaluate_on_grid(self, grid):
        """
        Calcula el espinor en toda la malla.
        """
        pass


class ConstantSpinor:
    """
    Prepara el estado inicial de un espinor constante (sin considerar la geometría del espacio).
    """
    def __init__(self, spin_polarization=[0,0,1], spin_superposition=[1 + 0j, 0 + 0j], momentum=[1,0,0], mass=electron_mass):
        """
        Constructor del espinor constante.
        """
        # Se normaliza el vector (al necesitarse un vector unitario)
        norm_n = np.linalg.norm(spin_polarization)
        self.spin_polarization = np.array(spin_polarization) / norm_n if norm_n > 0 else np.array([0,0,1])

        # Se comprueba que la suma del módulo al cuadrado sea 1
        norm_s = np.linalg.norm(spin_superposition)
        self.spin_superposition = np.array(spin_superposition, dtype=np.complex128) / norm_s if norm_s > 0 else np.array([1,0])

        self.momentum = np.array(momentum)
        self.mass = mass

        self._base()
        self._pauli_spinor()
        self._constant_spinor()

    def _base(self):
        """
        Calcula la base del espinor de Pauli.
        """
        theta = np.arccos(self.spin_polarization[2])
        phi = np.arctan2(self.spin_polarization[1], self.spin_polarization[0])

        # Construcción base (vector columna 2x1) del espinor de Pauli
        self.chi_up = np.array([
            [np.cos(theta / 2)],
            [np.exp(1j * phi) * np.sin(theta / 2)]
        ], dtype=np.complex128)

        self.chi_down = np.array([
            [np.sin(theta / 2)],
            [-np.exp(1j * phi) * np.cos(theta / 2)]
        ], dtype=np.complex128)

    def _pauli_spinor(self):
        """
        Construcción del espinor de Pauli.
        """
        alpha = self.spin_superposition[0]
        beta = self.spin_superposition[1]
        self.pauli_spinor = (alpha * self.chi_up) + (beta * self.chi_down)

    def _constant_spinor(self):
        """
        Construcción del espinor constante (por partes).
        """
        # Proyección del spin en la dirección del momento
        norm_momentum = np.linalg.norm(self.momentum)

        if norm_momentum == 0:
            proyection_spin = np.zeros((2, 2), dtype=np.complex128)
        else:
            proyection_spin = ((self.momentum[0] * sigma_1 +
                                self.momentum[1] * sigma_2 +
                                self.momentum[2] * sigma_3) / norm_momentum)

        # Energía relativista
        energy = np.sqrt((norm_momentum)**2 + (self.mass)**2)

        # Partes superior e inferior del espinor
        upper_part = np.sqrt(energy + self.mass) * self.pauli_spinor
        lower_part = np.sqrt(energy - self.mass) * (proyection_spin @ self.pauli_spinor)

        self.constant_spinor = np.vstack((upper_part, lower_part))


class GaussianPacket(InitialState):
    """
    Clase que construye el estado inicial como un paquete de ondas gaussianas.
    """
    def __init__(self, constant_spinor, center=[0,0,0], spatial_width=1):
        """
        Constructor.
        @param constant_spinor: El espinor constante base.
        @param center: Centro del paquete gaussiano.
        @param spatial_width: Ancho espacial del paquete.
        """
        super().__init__(constant_spinor.mass)
        self.constant_spinor = constant_spinor
        self.center = np.array(center)
        self.spatial_width = spatial_width

    def evaluate(self, grid_position):
        """
        Calcula el estado base en un punto exacto.
        """
        x = np.array(grid_position)
        p0 = self.constant_spinor.momentum
        x0 = self.center

        # Rellenar el vector de posición 'x' para que coincida con la dimensión del vector de momento y centro
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

        return dirac_spinor

    def evaluate_on_grid(self, grid):
        """
        Calcula el estado inicial en toda la malla.
        @param grid: Objeto Grid sobre el que se evaluará el paquete.
        @return: Un array de NumPy con la forma (n_points, 4).
        """
        n_points = np.prod(grid.shape)
        psi_0 = np.zeros((n_points, 4), dtype=np.complex128)

        coords_flat = grid.coords.reshape(n_points, grid.dim)

        for i in range(n_points):
            position = coords_flat[i]
            spinor_at_point = self.evaluate(position)
            psi_0[i, :] = spinor_at_point.flatten()

        return psi_0


class PlaneWave(InitialState):
    """
    Clase que construye el estado inicial como una Onda Plana.
    Hereda de InitialState.
    """
    def __init__(self, constant_spinor):
        """
        Constructor.
        @param constant_spinor: El espinor constante base.
        """
        super().__init__(constant_spinor.mass)
        self.constant_spinor = constant_spinor

    def evaluate(self, grid_position):
        """
        Calcula el estado base en un punto exacto.
        """
        x = np.array(grid_position)
        p0 = self.constant_spinor.momentum

        if len(x) < len(p0):
            padded_x = np.zeros(len(p0), dtype=float)
            padded_x[:len(x)] = x.flatten()
            x = padded_x

        u_p0 = self.constant_spinor.constant_spinor

        p0_dot_x = np.dot(p0, x)

        # Parte espacial de la Onda plana
        spatial_part = np.exp(1j * p0_dot_x)

        dirac_spinor = u_p0 * spatial_part

        return dirac_spinor


    def evaluate_on_grid(self, grid):
        """
        Calcula el estado inicial en toda la malla.
        @param grid: Objeto Grid sobre el que se evaluará el paquete.
        @return: Un array de NumPy con la forma (n_points, 4).
        """
        n_points = np.prod(grid.shape)
        psi_0 = np.zeros((n_points, 4), dtype=np.complex128)

        coords_flat = grid.coords.reshape(n_points, grid.dim)

        for i in range(n_points):
            position = coords_flat[i]
            spinor_at_point = self.evaluate(position)
            psi_0[i, :] = spinor_at_point.flatten()

        return psi_0
