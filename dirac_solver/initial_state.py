"""
@file initial_state.py
@brief Creacion del estado inicial sin geometria del espacio.
"""

"""
@ brief librerias importadas.
"""
import numpy as np
#Importar constantes 
from . import c, hbar, electron_mass, sigma_1, sigma_2, sigma_3

"""
@brief Aqui se prepara el estado inicial (sin considerar la geometria del espacio).
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
class GaussianPacket:
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
        if len(x) < len(p0):
            padded_x = np.zeros(len(p0), dtype=float)
            padded_x[:len(x)] = x.flatten()
            x = padded_x

        sigma0 = self.spatial_width
        u_p0 = self.constant_spinor.constant_spinor

        p0_dot_x = np.dot(p0, x)
        exponent_norm = np.linalg.norm(x - x0)**2

        spatial_part = np.exp(1j * p0_dot_x) * np.exp(-exponent_norm / (4 + sigma0**2))

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
        n_points = grid.shape[0] # Assuming 1D for now
        psi_0 = np.zeros((n_points, 4), dtype=np.complex128)

        for i in range(n_points):
            position = grid.coords[i]
            spinor_at_point = self.evaluate(position)
            psi_0[i, :] = spinor_at_point.flatten()

        return psi_0

# """
# @class GaussianWavePacket
# @brief Clase para inicializar el paquete de ondas gaussiano.
# """
# 
# 
# class GaussianWavePacket:
#     """
#     @brief En esta clase se construye un paquete de ondas gaussiano.
#     """
#     def __init__(self, momentum=[0,0,0], mass=1.0, packet_width=1.0, spin_orientation='up_z'):
#         """
#         @brief Inicializa los parametros fisicos del paquete de ondas
#         @param momemtum:  Momentum del paquete de ondas (array), por defecto: [0,0,0]
#         @param mass: Masa en reposo de la particula (float), por defecto: 1.0
#         @param packet_width: Ancho inicial del paquete (float), por defecto: 1.0
#         @param spin_orientation: Orientación del spin. (char), por defecto: 'up_z'
#         """
# 
#         self.p_vec = np.array(momentum)
#         self.mass = mass
#         self.x0 = packet_width 
#         self.spin = spin_orientation
# 
#         consts = Constants()
#         self.c = consts.get("c")
#         self.HBAR = consts.get("HBAR")
# 
# 
# 
# 
#         self._precompute_spinor()
# 
#     def _precompute_spinor(self):
#         """
#         @brief En este metodo se calcula la parte espinorial constante del paquete de ondas
#         @note Se ejecuta al crearse el objeto, es de uso interno.
#         """
#         """
#         @brief Aqui calculamos la energia relativista del paquete de ondas:
#         """
# 
#         p_mag = np.linalg.norm(self.p_vec)
# 
#         if p_mag == 0:
#             """
#             @todo Este condicional se podria eliminar si el sqrt eliminara la potencia cuadrada al ser p_mag = 0
#             """
#             self.energy = self.mass * (self.c**2)
#         else:
#             self.energy = np.sqrt((p_mag * self.c)**2 + (self.mass * self.c**2)**2)
# 
#         """
#         @brief Se calcula la constante de normalización N:
#         """
#         N = ( (2 * np.pi )**(3/2) * (self.x0)**3 )**(-1/2)
# 
#         """
#         @brief Se calcula el factor de normalización:
#         """
#         norm_factor = np.sqrt((self.energy + self.mass*(self.c**2)) / (2 * self.energy))
# 
#         """
#         @brief Se construye el spinor base:
#         """
# 
#         px, py, pz = self.p_vec
#         spinor = np.zeros(4, dtype=np.complex128)
# 
#         if self.spin == 'up_z':
#             spinor[0] = 1.0 
#             spinor[1] = 0.0
#             spinor[2] = ((pz * self.c)/(self.energy + (self.mass * (self.c**2)))) 
#             spinor[3] = ( ((px - (1j * py))*self.c ) / (self.energy + (self.mass*(self.c**2))) )
#         else:
#             raise NotImplementedError(f"Spin orientation '{self.spin}' is not supported.")
#         """
#             @warning Es necesario implementar las otras componentes
#             """
# 
#         """
#         @brief Se aplica la normalizacion a nuestro vertor 4x1
#         @note Es necesario construir el termino faltante con al geometria.
#         """
# 
#         self.spinor_vector = N * norm_factor * spinor
#         print("Objeto creado (falta aplicarle la geometria)")

