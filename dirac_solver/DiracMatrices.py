"""
@file DiracMatrices.py
@brief Definiciones de las matrices de Dirac y alfa en la representación estándar.
@details Este módulo proporciona las matrices como arrays de NumPy para ser
         utilizadas en cálculos de observables y en el propio solver.
"""

import numpy as np

# --- Representación Estándar (de Dirac) ---

# Matriz identidad 4x4
I = np.identity(4, dtype=np.complex128)

# gamma^0 (beta)
gamma0 = np.array([
    [1, 0, 0, 0],
    [0, 1, 0, 0],
    [0, 0, -1, 0],
    [0, 0, 0, -1]
], dtype=np.complex128)

beta = gamma0

# gamma^1
gamma1 = np.array([
    [0, 0, 0, 1],
    [0, 0, 1, 0],
    [0, -1, 0, 0],
    [-1, 0, 0, 0]
], dtype=np.complex128)

# gamma^2
gamma2 = np.array([
    [0, 0, 0, -1j],
    [0, 0, 1j, 0],
    [0, 1j, 0, 0],
    [-1j, 0, 0, 0]
], dtype=np.complex128)

# gamma^3
gamma3 = np.array([
    [0, 0, 1, 0],
    [0, 0, 0, -1],
    [-1, 0, 0, 0],
    [0, 1, 0, 0]
], dtype=np.complex128)

# Colección de matrices gamma espaciales (para producto punto)
gamma = np.array([gamma1, gamma2, gamma3])

# --- Matrices alpha (α^k = γ^0 * γ^k) ---

# alpha^1
alpha1 = gamma0 @ gamma1

# alpha^2
alpha2 = gamma0 @ gamma2

# alpha^3
alpha3 = gamma0 @ gamma3

# Colección de matrices alpha
alpha = np.array([alpha1, alpha2, alpha3])

# --- Matriz gamma^5 ---
gamma5 = 1j * gamma0 @ gamma1 @ gamma2 @ gamma3
