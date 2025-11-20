import pytest
from dirac_solver.potentials import YukawaPotential, InfiniteWellPotential
import numpy as np

def test_yukawa_potential():
    """
    Testa la correcta evaluación del potencial de Yukawa.
    """
    potential = YukawaPotential(strength=1.0, range=0.5)
    core_potential = potential.core_potential

    # Test at r=0
    assert np.isclose(core_potential.evaluate([0.0]), -1.0)

    # Test at a known distance
    r = np.array([2.0, 0.0, 0.0])
    expected_value = -1.0 * np.exp(-0.5 * 2.0) / 2.0
    assert np.isclose(core_potential.evaluate(r.tolist()), expected_value)

def test_infinite_well_potential():
    """
    Testa la correcta evaluación del potencial de pozo infinito.
    """
    potential = InfiniteWellPotential(widths=[2.0, 2.0])
    core_potential = potential.core_potential

    # Test inside the well
    assert core_potential.evaluate([0.5, -0.5]) == 0.0

    # Test outside the well
    assert core_potential.evaluate([1.5, 0.0]) == np.inf
    assert core_potential.evaluate([0.0, 2.5]) == np.inf
