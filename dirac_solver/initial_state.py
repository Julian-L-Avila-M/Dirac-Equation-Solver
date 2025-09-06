"""
@file initial_state.py
@brief Creacion del estado inicial sin geometria del espacio.
"""

"""
@ brief librerias importadas.
"""
import numpy as np

"""
@brief Aqui se prepara el estado inicial (sin considerar la geometria del espacio).
@param HBAR: constante de planck
@param c: velocidad de la luz
@param ELECTRON_MASS: masa del electron.
"""

HBAR = 1.0
c = 137.0 

"""
@class GaussianWavePacket
@brief Clase para inicializar el paquete de ondas gaussiano.
"""

class GaussianWavePacket:
    """
    @brief En esta clase se construye un paquete de ondas gaussiano.
    """
    def __init__(self, momentum=[0,0,0], mass=1.0, packet_width=1.0, spin_orientation='up_z'):
        """
        @brief Inicializa los parametros fisicos del paquete de ondas
        @param momemtum:  Momentum del paquete de ondas (array), por defecto: [0,0,0]
        @param mass: Masa en reposo de la particula (float), por defecto: 1.0
        @param packet_width: Ancho inicial del paquete (float), por defecto: 1.0
        @param spin_orientation: Orientación del spin. (char), por defecto: 'up_z'
        """

        self.p_vec = np.array(momentum)
        self.mass = mass
        self.x0 = packet_width 
        self.spin = spin_orientation

        self._precompute_spinor()

    def _precompute_spinor(self):
        """
        @brief En este metodo se calcula la parte espinorial constante del paquete de ondas
        @note Se ejecuta al crearse el objeto, es de uso interno.
        """
        """
        @brief Aqui calculamos la energia relativista del paquete de ondas:
        """

        p_mag = np.linalg.norm(self.p_vec)

        if p_mag == 0:
            """
            @todo Este condicional se podria eliminar si el sqrt eliminara la potencia cuadrada al ser p_mag = 0
            """
            self.energy = self.mass * (c**2)
        else:
            self.energy = np.sqrt((p_mag * c)**2 + (self.mass * c**2)**2)

        """
        @brief Se calcula la constante de normalización N:
        """
        N = ( (2 * np.pi )**(3/2) * (self.x0)**3 )**(-1/2)

        """
        @brief Se calcula el factor de normalización:
        """
        norm_factor = np.sqrt((self.energy + self.mass*(c**2)) / (2 * self.energy))

        """
        @brief Se construye el spinor base:
        """

        px, py, pz = self.p_vec
        spinor = np.zeros(4, dtype=np.complex128)

        if self.spin == 'up_z':
            spinor[0] = 1.0 
            spinor[1] = 0.0
            spinor[2] = ((pz * c)/(self.energy + (self.mass * (c**2)))) 
            spinor[3] = ( ((px - (1j * py))*c ) / (self.energy + (self.mass*(c**2))) )
        else:
            raise NotImplementedError(f"Spin orientation '{self.spin}' is not supported.")
        """
            @warning Es necesario implementar las otras componentes
            """

        """
        @brief Se aplica la normalizacion a nuestro vertor 4x1
        @note Es necesario construir el termino faltante con al geometria.
        """

        self.spinor_vector = N * norm_factor * spinor
        print("Objeto creado (falta aplicarle la geometria)")
