# init_geometry.py 
"""
@file init_geometry.py
@brief Definición de la malla espacial para la simulacíones de la ecuación de Dirac.
"""

# se importan las librerias. 

import numpy as np
from .constants import Constants

# Grid class definition. 

class Grid:
  """
  @brief lase que representa una malla espacial (1D ,2D ,3D).
  """

  def __init__(self, shape,spacing, origin=None):
    """
    @param shape: tupla con el numero de puntos en cada direccion (nx , [ny],[nz] ).
    @param  spacing: tupla con el tamaño de la celda (dx, [dy], [dz]).
    @param origin: punto de inicio de la malla (default: centrado en 0)
    @param units: "natural" (ħ=c=1) o "si"
    @param c: velocidad de la luz (default: 1.0 en unidades naturales)
    @param hbar: constante de Planck reducida (default: 1.0 en unidades naturales)
    """
    
    # self.shape guarda el numero de puntos en cada direccion del espacio 
    self.shape = shape

    # self.dim almacena cuantas dimensiones tiene la malla (1, 2 o 3), en este caso la cantidad de entradas de shape.
    self.dim = len(shape) 

    # self.spacing, guarda el tamaño de la celda en cada dirección, en funcion de la dimension definida.
    if isinstance(spacing, (float, int)):
      """
      @brief Se genera un espaciado igual en todas las direcciones 
      """
      self.spacing = (spacing,)*self.dim
    else:
      """
      @brief Se genera un espaciado diferente en cada direccion de la fomra  "En 3D (self.dim=3): (spacing,) * 3 → (0.1, 0.1, 0.1)"
      """
      self.spacing = tuple(spacing)
    
    # self.origin guarda el punto inicial desde el cual se construye la malla. (coordenada minima en cada eje).
    if origin is None:
      """
      @brief Por defecto se define el limite negativo en cada eje, dejando el centrop de la malla en O.
      """
      self.origin = tuple(-0.5 * (n-1) * dx for n, dx in zip(shape, self.spacing) )
    else:
      """
      @brief se toman llos valores de la tupla (x,y,z)
      """
      self.origin = tuple(origin)

    # Toma las constantes globales ya configuradas
    consts = Constants()
    self.c = consts.get("c")
    self.hbar = consts.get("HBAR")

    # self.coods guarda toda la información de las coordenadas de la malla en un arreglo Numpy con la fomra (nx, [ny], [nz], dim)
    self.coords = self._generate_coords()
  
  def _generate_coords(self):
    """
    @brief construye las coordenadas de la malla en funcíon de la dimensiión.
    """
    axes = [self.origin[i] + np.arangue(self.shape[i]) * self.spacing[i] for i in range(self.dim)]
    grids = np.meshgrid(*axes, indexing="ij")

    # self.coords se construyó en __init__ llamando a esta función.
    coords = np.stack(grids, axis=-1)  # shape: (nx, [ny], [nz], dim)
    return coords

  @property
  def dv(self):
      """
      @brief Elemento de volumen (dx * dy * dz).
      """
      # self.spacing contiene los espaciamientos en cada dirección
      # se multiplican para obtener el volumen elemental ΔV.
      dv = 1.0
      for dx in self.spacing:
          dv *= dx
      return dv

  def courant_limit(self):
      """
      @brief Calcula el paso de tiempo máximo por condición de estabilidad de Courant.
      """
      # self.spacing guarda (dx,dy,dz) → usamos cada valor para la fórmula
      # self.c es la velocidad de la luz en las unidades seleccionadas
      inv_sq = sum((1.0/dx)**2 for dx in self.spacing)
      return 1.0 / (self.c * np.sqrt(inv_sq))
    
    

  

    



