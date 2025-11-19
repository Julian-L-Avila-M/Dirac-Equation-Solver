from . import _core

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

    El potencial es cero dentro de una caja y infinito fuera de ella.

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
