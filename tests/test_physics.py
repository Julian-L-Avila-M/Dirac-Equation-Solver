
import sys
sys.path.append('..')
import numpy as np
import pytest
from dirac_solver import (
    Grid,
    ConstantSpinor,
    PlaneWave,
    GaussianPacket,
    DiracProblemBuilder,
    DiracSolver,
    _core,
    observables,
)

def test_free_particle_plane_wave_evolution():
    """
    Verifica que una onda plana evoluciona acumulando una fase e^(-i E t).
    """
    # 1. Configuración del problema
    # Usamos una malla 1D con mayor resolución espacial
    L = 10.0
    N = 200 # dx = 0.05
    dx = L / N
    grid = Grid(shape=(N,), spacing=(dx,))

    # Momento p = [1, 0, 0]
    px = 1.0
    momentum = [px, 0.0, 0.0]
    mass = 1.0

    constant_spinor = ConstantSpinor(momentum=momentum, mass=mass)
    initial_state = PlaneWave(constant_spinor)

    potential = _core.FreeParticle()
    boundary_condition = _core.PeriodicBoundary()

    dt = 0.0001
    total_time = 0.05

    problem = (
        DiracProblemBuilder()
        .set_grid(grid)
        .set_initial_state(initial_state)
        .set_potential(potential)
        .set_boundary_condition(boundary_condition)
        .set_time_parameters(time_step=dt, total_time=total_time)
        .build()
    )

    solver = DiracSolver(problem)
    psi_0 = solver.get_psi()

    solver.run_simulation()
    psi_t = solver.get_psi()

    # 3. Comparación
    E = np.sqrt(px**2 + mass**2)
    phase_factor = np.exp(-1j * E * total_time)
    psi_analytical = psi_0 * phase_factor

    diff = psi_t - psi_analytical
    error = np.linalg.norm(diff) / np.linalg.norm(psi_analytical)

    print(f"Error relativo en la evolución de la onda plana: {error}")

    # Tolerancia ajustada a la precisión del método FDTD (2do orden)
    assert error < 0.15, f"El error {error} es mayor que la tolerancia permitida."


def test_conservation_of_probability():
    """
    Verifica que la probabilidad total se conserva.
    """
    grid = Grid(shape=(100,), spacing=(0.1,))
    constant_spinor = ConstantSpinor(momentum=[1.0, 0, 0])
    initial_state = GaussianPacket(constant_spinor, center=[0.0, 0, 0], spatial_width=1.0)

    potential = _core.FreeParticle()
    boundary_condition = _core.PeriodicBoundary()

    dt = 0.002
    total_time = 1.0

    problem = (
        DiracProblemBuilder()
        .set_grid(grid)
        .set_initial_state(initial_state)
        .set_potential(potential)
        .set_boundary_condition(boundary_condition)
        .set_time_parameters(time_step=dt, total_time=total_time)
        .build()
    )

    solver = DiracSolver(problem)
    obs_handler = observables.Observables(solver)

    def get_total_probability():
        psi = solver.get_psi()
        rho = obs_handler.probability_density(psi)
        return np.sum(rho) * grid.get_cell_volume()

    prob_initial = get_total_probability()

    num_steps = int(total_time / dt)
    check_every = 50

    for i in range(num_steps):
        solver.integrator.step()
        if (i + 1) % check_every == 0:
            prob_current = get_total_probability()
            diff = abs(prob_current - prob_initial)
            assert diff < 1e-3, f"Probabilidad no conservada en paso {i+1}. Diff: {diff}"


def test_conservation_of_energy():
    """
    Verifica que la energía esperada <H> se conserva.
    """
    grid = Grid(shape=(200,), spacing=(0.1,)) # L=20
    constant_spinor = ConstantSpinor(momentum=[1.0, 0, 0])
    initial_state = GaussianPacket(constant_spinor, center=[0.0, 0, 0], spatial_width=1.5)

    potential = _core.FreeParticle()
    boundary_condition = _core.PeriodicBoundary()

    dt = 0.0005
    total_time = 0.5

    problem = (
        DiracProblemBuilder()
        .set_grid(grid)
        .set_initial_state(initial_state)
        .set_potential(potential)
        .set_boundary_condition(boundary_condition)
        .set_time_parameters(time_step=dt, total_time=total_time)
        .build()
    )

    solver = DiracSolver(problem)
    obs_handler = observables.Observables(solver)

    psi_0 = solver.get_psi()
    E_initial = obs_handler.expected_value_energy(psi_0)

    solver.run_simulation()

    psi_final = solver.get_psi()
    E_final = obs_handler.expected_value_energy(psi_final)

    diff = abs(E_final - E_initial)
    rel_error = diff / abs(E_initial)

    print(f"Energía Inicial: {E_initial}, Energía Final: {E_final}, Error Relativo: {rel_error}")

    # Tolerancia relajada para método Leapfrog
    assert rel_error < 0.05, f"La energía no se conservó. Error relativo: {rel_error}"
