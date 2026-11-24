# `Dirac-Solver` Tutorials and Examples (English)

> **Language / Idioma:** [Español](README.md) | **English**

This directory contains interactive Jupyter Notebooks (`.ipynb`) demonstrating step by step how to simulate the relativistic Dirac equation in 1D, 2D, and 3D with `Dirac-Solver`.

## English Tutorials (`examples/en/`)

| # | Notebook | Topics Covered |
| :-: | :--- | :--- |
| 01 | [`en/01_free_particle.ipynb`](en/01_free_particle.ipynb) | 1D/2D/3D free propagation, `GaussianPacket`, `PlaneWave`, `PeriodicBoundary` & `AbsorbingBoundary`, CFL limit. |
| 02 | [`en/02_infinite_potential_well.ipynb`](en/02_infinite_potential_well.ipynb) | Relativistic quantum confinement in `InfiniteWellPotential` (1D & 2D) and wall reflections. |
| 03 | [`en/03_quantum_wells_and_klein_tunneling.ipynb`](en/03_quantum_wells_and_klein_tunneling.ipynb) | Custom `ScalarPotential` profiles, Klein tunneling ($V_0 > E + m$), and finite double quantum wells. |
| 04 | [`en/04_coulomb_potential.ipynb`](en/04_coulomb_potential.ipynb) | Relativistic scattering on a regularized `CoulombPotential` and expectation values $\langle\mathbf{r}\rangle, \langle\mathbf{p}\rangle, \langle H\rangle$. |
| 05 | [`en/05_yukawa_potential_and_observables.ipynb`](en/05_yukawa_potential_and_observables.ipynb) | 2D `YukawaPotential` scattering, probability current quiver plots $\mathbf{J}$, `HDF5Storage`, and GIF animations. |
| 06 | [`en/06_physics_verification.ipynb`](en/06_physics_verification.ipynb) | Physical validation: plane-wave phase $e^{-iEt}$, probability conservation, and energy conservation. |

## Tutoriales en Español (`examples/es/`)

See [`README.md`](README.md) for the complete Spanish edition of all tutorials (`examples/es/01_particula_libre.ipynb` through `06_verificacion_fisica.ipynb`).
