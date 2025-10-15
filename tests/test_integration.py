import sys
sys.path.append('..')
import numpy as np
from dirac_solver import (
    Grid,
    ConstantSpinor,
    GaussianPacket,
    DiracProblemBuilder,
    DiracSolver,
    _core,
)

def test_1d_simulation():
    """
    Tests a simple 1D simulation of a free particle with periodic boundaries.
    """
    py_grid = Grid(shape=(100,), spacing=(0.1,))
    constant_spinor = ConstantSpinor(momentum=[5.0, 0, 0])
    initial_state = GaussianPacket(constant_spinor, center=[0.0, 0, 0])

    potential = _core.FreeParticle()
    boundary_condition = _core.PeriodicBoundary()

    problem = (
        DiracProblemBuilder()
        .set_grid(py_grid)
        .set_initial_state(initial_state)
        .set_potential(potential)
        .set_boundary_condition(boundary_condition)
        .set_time_parameters(time_step=0.001, total_time=0.1)
        .build()
    )

    solver = DiracSolver(problem)
    solver.run_simulation()

    psi_final = solver.get_psi()

    expected_shape = (py_grid.shape[0], 4)
    assert psi_final.shape == expected_shape, f"Final psi has incorrect shape: {psi_final.shape}, expected {expected_shape}"

    norm = np.linalg.norm(psi_final)
    assert norm > 1e-9, f"Final wave function appears to be zero (norm = {norm:.4f})."
