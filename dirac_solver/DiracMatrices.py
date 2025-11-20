"""
@file DiracMatrices.py
@brief Definiciones de las matrices de Dirac y alfa en la representación estándar.
@details Este módulo proporciona las matrices como arrays de NumPy para ser
         utilizadas en cálculos de observables y en el propio solver.
"""

import numpy as np

# --- Representación Estándar (de Dirac) ---

## @var I
# Matriz identidad 4x4
I = np.identity(4, dtype=np.complex128)

## @var gamma0
# gamma^0 (beta)
gamma0 = np.array([
    [1, 0, 0, 0],
    [0, 1, 0, 0],
    [0, 0, -1, 0],
    [0, 0, 0, -1]
], dtype=np.complex128)

## @var beta
# Alias para gamma0
beta = gamma0

## @var gamma1
# gamma^1
gamma1 = np.array([
    [0, 0, 0, 1],
    [0, 0, 1, 0],
    [0, -1, 0, 0],
    [-1, 0, 0, 0]
], dtype=np.complex128)

## @var gamma2
# gamma^2
gamma2 = np.array([
    [0, 0, 0, -1j],
    [0, 0, 1j, 0],
    [0, 1j, 0, 0],
    [-1j, 0, 0, 0]
], dtype=np.complex128)

## @var gamma3
# gamma^3
gamma3 = np.array([
    [0, 0, 1, 0],
    [0, 0, 0, -1],
    [-1, 0, 0, 0],
    [0, 1, 0, 0]
], dtype=np.complex128)

## @var gamma
# Colección de matrices gamma espaciales (para producto punto)
gamma = np.array([gamma1, gamma2, gamma3])

# --- Matrices alpha (α^k = γ^0 * γ^k) ---

## @var alpha1
# alpha^1 = gamma0 * gamma1
alpha1 = gamma0 @ gamma1

## @var alpha2
# alpha^2 = gamma0 * gamma2
alpha2 = gamma0 @ gamma2

## @var alpha3
# alpha^3 = gamma0 * gamma3
alpha3 = gamma0 @ gamma3

## @var alpha
# Colección de matrices alpha
alpha = np.array([alpha1, alpha2, alpha3])

# --- Matriz gamma^5 ---
## @var gamma5
# Matriz de quiralidad gamma^5
gamma5 = 1j * gamma0 @ gamma1 @ gamma2 @ gamma3
