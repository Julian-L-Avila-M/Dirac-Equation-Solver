"""
@file observables.py
@brief Módulo para calcular observables físicos a partir del estado de la simulación.
@details Este módulo proporciona la clase `Observables` que, dado un estado
         de la simulación (la función de onda `psi`), calcula diversas
         cantidades físicas como densidades de probabilidad, corrientes,
         y valores esperados de operadores.
"""

import numpy as np
from . import DiracMatrices as dm

class Observables:
    """
    Calcula y almacena observables físicos derivados de la función de onda (psi).
    """

    def __init__(self, solver):
        """
        Inicializa el calculador de observables.

         solver: Una instancia de `DiracSolver` para acceder a `psi` y la `grid`.
        """
        self.solver = solver
        self.grid = solver.problem.grid
        self.history = {} # Almacena la evolución temporal de los observables

    def compute_all(self, time):
        """
        Calcula todos los observables para un tiempo dado y los almacena.

         time: El tiempo actual de la simulación (float).
        """
        psi = self.solver.get_psi()

        # --- Cantidades Locales (dependen de la posición) ---
        prob_density = self.probability_density(psi)
        charge_density = self.charge_density(psi)
        prob_current = self.probability_current(psi)

        # --- Valores Esperados (integrados en todo el espacio) ---
        expected_position = self.expected_value_position(prob_density)
        expected_momentum = self.expected_value_momentum(psi)
        expected_energy = self.expected_value_energy(psi)

        # --- Almacenamiento ---
        self._record_history('time', time)
        self._record_history('probability_density', prob_density)
        self._record_history('charge_density', charge_density)
        self._record_history('probability_current', prob_current)
        self._record_history('expected_position', expected_position)
        self._record_history('expected_momentum', expected_momentum)
        self._record_history('expected_energy', expected_energy)

    def _record_history(self, key, value):
        """Añade un valor al historial de un observable."""
        if key not in self.history:
            self.history[key] = []
        self.history[key].append(value)

    # --- Métodos para Cantidades Locales ---

    def probability_density(self, psi):
        """
        Calcula la densidad de probabilidad: ρ = ψ† * ψ
        Suma sobre los componentes del espinor.
        """
        # np.conj(psi) es ψ† (adjunto Hermítico para un vector)
        # Sumamos sobre el eje de los espinores (axis=1)
        return np.sum(np.conj(psi) * psi, axis=1).real

    def charge_density(self, psi):
        """
        Calcula la densidad de carga: J^0 = ψ† * γ^0 * ψ
        """
        # J^0 = ψ† * γ^0 * ψ
        # Esto se calcula eficientemente con np.einsum
        term = np.einsum('ij,jk,ik->i', np.conj(psi), dm.gamma0, psi, optimize=True)
        return term.real

    def probability_current(self, psi):
        """
        Calcula la corriente de probabilidad: J^k = c * ψ† * α^k * ψ
        donde α^k = γ^0 * γ^k.
        """
        # J^k = ψ† * α^k * ψ
        # Asumimos c=1
        current = np.zeros((self.grid.get_total_points(), self.grid.dim))

        for k in range(self.grid.dim): # 0=x, 1=y, 2=z
            # α_k = γ_0 * γ_k
            alpha_k = dm.alpha[k]

            # (ψ† * α^k * ψ)_i = sum_j,l (ψ†_j * α^k_jl * ψ_l)
            # Esto se puede hacer con np.einsum para eficiencia
            term = np.einsum('ij,jk,ik->i', np.conj(psi), alpha_k, psi, optimize=True)
            current[:, k] = term.real

        return current

    # --- Métodos para Valores Esperados ---

    def expected_value_position(self, prob_density):
        """
        Calcula el valor esperado de la posición: <r> = ∫ r * ρ(r) dV
        """
        dV = self.grid.get_cell_volume()

        # Obtener las coordenadas de cada punto de la malla
        # self.grid.coords tiene forma (nx, ny, ..., dim), debemos aplanarla
        # para que coincida con prob_density (N_points,)
        coords = self.grid.coords.reshape(-1, self.grid.dim)

        # <x_k> = ∫ x_k * ρ(x) dV
        expected_pos = np.zeros(self.grid.dim)
        for k in range(self.grid.dim):
            expected_pos[k] = np.sum(coords[:, k] * prob_density) * dV

        return expected_pos

    def expected_value_momentum(self, psi):
        """
        Calcula el valor esperado del momento: <p> = ∫ ψ† * (-iħ∇) * ψ dV
        """
        dV = self.grid.get_cell_volume()

        # El operador momento es -iħ∇. Usamos diferencias finitas para el gradiente.
        grad_psi = self.grid.gradient(psi) # Retorna un array de forma (N_points, dim, 4)

        # <p_k> = ∫ ψ† * (-iħ * ∂_k) * ψ dV
        expected_mom = np.zeros(self.grid.dim)
        for k in range(self.grid.dim):
            # Suma sobre los componentes del espinor
            integrand = np.sum(np.conj(psi) * (-1j) * grad_psi[:, k, :], axis=1)
            expected_mom[k] = np.sum(integrand).real * dV

        return expected_mom

    def expected_value_energy(self, psi):
        """
        Calcula el valor esperado de la energía: <E> = ∫ ψ† * H * ψ dV
        donde H es el Hamiltoniano de Dirac.
        H = -iħc * α·∇ + βmc² + V(r)
        """
        dV = self.grid.get_cell_volume()

        # --- Término cinético: -iħc * α·∇ ---
        grad_psi = self.grid.gradient(psi) # (N_points, dim, 4)

        kinetic_term = np.zeros_like(psi)
        for k in range(self.grid.dim):
            alpha_k = dm.alpha[k]
            term = np.einsum('jk,ik->ij', alpha_k, grad_psi[:, k, :], optimize=True)
            kinetic_term += term

        kinetic_term *= -1j # Asumimos ħ=c=1

        # --- Término de masa: βmc² ---
        mass = self.solver.problem.initial_state.mass
        mass_term = np.einsum('jk,ik->ij', dm.beta, psi, optimize=True) * mass # m=1

        # --- Término de potencial: V(r) ---
        V = self.solver.problem.potential
        potential_term = np.zeros_like(psi)
        if V is not None:
            # V_grid debe tener forma (N_points,)
            # Aplanamos coords para iterar sobre cada punto
            coords_flat = self.grid.coords.reshape(-1, self.grid.dim)
            V_grid = np.array([V.evaluate(r) for r in coords_flat])
            # Multiplicar V en cada componente del espinor
            potential_term = (V_grid[:, np.newaxis] * psi)

        # --- Hamiltoniano aplicado a psi: Hψ ---
        H_psi = kinetic_term + mass_term + potential_term

        # --- Valor esperado: ∫ ψ† * Hψ dV ---
        integrand = np.sum(np.conj(psi) * H_psi, axis=1)

        return np.sum(integrand).real * dV

    def get_history(self, key):
        """
        Devuelve el historial de un observable como un array de NumPy.
        """
        return np.array(self.history.get(key, []))
