

# Se importan las librerias.
import numpy as np
from . import c, hbar

class Grid:
    """
    Clase que representa una malla espacial (1D, 2D, 3D).
    """

    def __init__(self, shape, spacing, origin=None):
        """
        Constructor de la malla.

        shape: Tupla con el número de puntos en cada dirección (nx, [ny], [nz]).
        spacing: Tupla o escalar con el tamaño de la celda (dx, [dy], [dz]).
        origin: Punto de inicio de la malla (default: centrado en 0).
        """

        ## @brief Número de puntos en cada dirección del espacio.
        self.shape = shape

        ## @brief Dimensión de la malla (1, 2 o 3).
        self.dim = len(shape)

        # self.spacing guarda el tamaño de la celda en cada dirección.
        if isinstance(spacing, (float, int)):
            # Se genera un espaciado igual en todas las direcciones
            self.spacing = (float(spacing),) * self.dim
        else:
            # Se genera un espaciado diferente en cada dirección
            self.spacing = tuple(spacing)

        # self.origin guarda el punto inicial desde el cual se construye la malla (coordenada mínima).
        if origin is None:
            # Por defecto se centra la malla en 0
            self.origin = tuple(-0.5 * (n - 1) * dx for n, dx in zip(shape, self.spacing))
        else:
            self.origin = tuple(origin)

        ## @brief Velocidad de la luz en unidades naturales.
        self.c = c
        ## @brief Constante de Planck reducida en unidades naturales.
        self.hbar = hbar

        ## @brief Coordenadas de la malla en un arreglo NumPy con forma (nx, [ny], [nz], dim).
        self.coords = self._generate_coords()

    def _generate_coords(self):
        """
        Construye las coordenadas de la malla en función de la dimensión.
        """
        axes = [self.origin[i] + np.arange(self.shape[i]) * self.spacing[i] for i in range(self.dim)]
        grids = np.meshgrid(*axes, indexing="ij")
        coords = np.stack(grids, axis=-1)  # shape: (nx, [ny], [nz], dim)
        return coords

    def get_cell_volume(self):
        """
        Calcula el elemento de volumen (dx * dy * dz).
        """
        dv = 1.0
        for dx in self.spacing:
            dv *= dx
        return dv

    def gradient(self, psi):
        """
        Calcula el gradiente de un campo espinorial psi sobre la malla.
        """
        grad = np.zeros((self.get_total_points(), self.dim, 4), dtype=np.complex128)
        psi_grid = psi.reshape(*self.shape, 4)

        for i in range(4):  # Para cada componente del espinor
            # np.gradient devuelve una lista de arrays, uno por cada dimensión
            comp_grad = np.gradient(psi_grid[..., i], *self.spacing, edge_order=2)
            for k in range(self.dim):
                grad[:, k, i] = comp_grad[k].ravel()

        return grad

    def get_total_points(self):
        """
        Devuelve el número total de puntos en la malla.
        """
        return int(np.prod(self.shape))

    def courant_limit(self):
        """
        Calcula el paso de tiempo máximo por condición de estabilidad de Courant.
        """
        inv_sq = sum((1.0 / dx)**2 for dx in self.spacing)
        return 1.0 / (self.c * np.sqrt(inv_sq))
