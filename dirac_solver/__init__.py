## @package dirac_solver
#  Librería para simulación de la ecuación de Dirac en unidades naturales.
#
#  Contiene definiciones de constantes físicas, matrices de Pauli y clases
#  principales para construir mallas, estados iniciales y ejecutar simulaciones.
#
#  @author Sebastian Rodriguez, Camilo Huertas, Julian Avila
#  @date 2025-09-11

import numpy as np

## Constante de Planck reducida (ħ) en unidades naturales
hbar = 1.0

## Velocidad de la luz (c) en unidades naturales
c = 1.0

## Masa del electrón en unidades naturales
electron_mass = 1.0

## Carga del electrón en unidades naturales
electron_charge = 1.0

## Matriz de Pauli sigma_x
sigma_1 = np.array([[0, 1],
                    [1, 0]], dtype=np.complex128)

## Matriz de Pauli sigma_y
sigma_2 = np.array([[0, -1j],
                    [1j, 0]], dtype=np.complex128)

## Matriz de Pauli sigma_z
sigma_3 = np.array([[1, 0],
                    [0, -1]], dtype=np.complex128)

# Importar clases principales del paquete
from .geometry import Grid
from .initial_state import ConstantSpinor, GaussianPacket
from .core import DiracSolver, DiracProblemBuilder, SimulationProblem
from .storage import HDF5Storage

# Lista de símbolos exportados al usar: from dirac_solver import *
__all__ = [
    "hbar", "c", "electron_mass", "electron_charge",
    "sigma_1", "sigma_2", "sigma_3",
    "Grid", "ConstantSpinor", "GaussianPacket", "DiracSolver",
    "DiracProblemBuilder", "SimulationProblem",
    "HDF5Storage"
]
