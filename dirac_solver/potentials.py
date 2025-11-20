from . import _core
from typing import Callable

class FreeParticle:
    """
    Potencial nulo para una partícula libre.
    """
    def __init__(self):
        self._potential = _core.FreeParticle()

    @property
    def core_potential(self):
        return self._potential

class ScalarPotential:
    """
    Potencial escalar definido por una función arbitraria.

    Parámetros
    ----------
    func : callable
        Una función que toma un vector de posición (lista de floats) y devuelve
        un valor escalar para el potencial.
    """
    def __init__(self, func: Callable[[list], float]):
        self._potential = _core.ScalarPotential(func)

    @property
    def core_potential(self):
        return self._potential

class CoulombPotential:
    """
    Potencial de Coulomb regularizado.

    Parámetros
    ----------
    Z : float
        La carga efectiva del centro (e.g., número atómico).
    epsilon : float, opcional
        Parámetro de regularización para evitar la singularidad en r=0.
        Por defecto es 1e-6.
    """
    def __init__(self, Z: float, epsilon: float = 1e-6):
        self._potential = _core.CoulombPotential(Z, epsilon)

    @property
    def core_potential(self):
        return self._potential

class YukawaPotential:
    """
    Potencial de Yukawa, que describe interacciones de corto alcance.

    Este potencial tiene la forma:
    V(r) = -V_0 * exp(-a*r) / r

    Parámetros
    ----------
    strength : float
        La fuerza del potencial (V_0).
    range : float
        El rango de la interacción (a).
    """
    def __init__(self, strength: float, range: float):
        self._potential = _core.YukawaPotential(strength, range)

    @property
    def core_potential(self):
        return self._potential

class InfiniteWellPotential:
    """
    Potencial de pozo infinito, que confina una partícula a una región del espacio.

    El potencial es cero dentro de una caja e infinito fuera de ella.

    Parámetros
    ----------
    widths : list of float
        Una lista con los anchos del pozo en cada dimensión.
    """
    def __init__(self, widths: list):
        self._potential = _core.InfiniteWellPotential(widths)

    @property
    def core_potential(self):
        return self._potential
