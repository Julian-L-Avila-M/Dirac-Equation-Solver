# Spinor Definition and Construction in Dirac-Solver

> 🌐 **Language / Idioma:** **[Español](../es/spinor-definition.md)** | **English**

This document specifies the exact mathematical formulation implemented in `dirac_solver/initial_state.py` for constructing 2-component Pauli spinors, 4-component constant Dirac bispinors, and localized Gaussian wavepackets.

---

## 1. General Pauli Spinor Construction

### Inputs
- **`spin_polarization`**: A 3D spatial orientation vector $\mathbf{n} = (n_1, n_2, n_3)$, automatically normalized to a unit vector $\hat{n}$.
- **`spin_superposition`**: A pair of complex amplitudes $(\alpha, \beta) \in \mathbb{C}^2$ normalized such that $|\alpha|^2 + |\beta|^2 = 1$.

### Output
- **`pauli_spinor`**: The 2-component complex spinor $\chi_s \in \mathbb{C}^2$ representing the spin state in the computational ($\hat{z}$) basis:

$$
\chi_s = \alpha \chi_{\text{up}}(\hat{n}) + \beta \chi_{\text{down}}(\hat{n})
$$

### Oriented Basis Spinors ($\hat{n}\cdot\boldsymbol{\sigma}$)
The eigenstates of the helicity/projection operator $\hat{n}\cdot\boldsymbol{\sigma}$ corresponding to spin-up ($+1$) and spin-down ($-1$) along the axis $\hat{n}$ are:

$$
\chi_{\text{up}}(\hat{n}) = \begin{pmatrix} \cos\frac{\theta}{2} \\ e^{i \phi} \sin\frac{\theta}{2} \end{pmatrix}, \qquad
\chi_{\text{down}}(\hat{n}) = \begin{pmatrix} \sin\frac{\theta}{2} \\ -e^{i \phi} \cos\frac{\theta}{2} \end{pmatrix}
$$

### Spherical Angle Definitions
The spherical angles $(\theta, \phi)$ are derived from the Cartesian components of the unit vector $\hat{n} = (n_1, n_2, n_3)$:
- **Polar Angle ($\theta$)**: $\theta = \arccos(n_3) \in [0, \pi]$
- **Azimuthal Angle ($\phi$)**: $\phi = \operatorname{atan2}(n_2, n_1) \in (-\pi, \pi]$

---

## 2. Constant Dirac Bispinor (`ConstantSpinor`)

### Inputs
- `pauli_spinor`: 2-component Pauli spinor $\chi_s$.
- `momentum`: Initial 3-momentum $\mathbf{p}_0 = (p_1, p_2, p_3)$.
- `mass`: Particle rest mass $m$.

### Output
- `constant_spinor`: The positive-energy free-particle Dirac bispinor $u(\mathbf{p}_0) \in \mathbb{C}^4$:

$$
u(\mathbf{p}_0) = \begin{pmatrix} \sqrt{E + m}\, \chi_s \\ \sqrt{E - m}\, (\hat{\mathbf{p}}_0 \cdot \boldsymbol{\sigma}) \chi_s \end{pmatrix}
$$

where:
- $E = \sqrt{\|\mathbf{p}_0\|^2 + m^2}$ is the positive relativistic energy in natural units ($\hbar = c = 1$).
- $\hat{\mathbf{p}}_0 \cdot \boldsymbol{\sigma} = \frac{1}{\|\mathbf{p}_0\|} (p_1 \sigma_1 + p_2 \sigma_2 + p_3 \sigma_3)$ is the projection of the Pauli matrices along the momentum direction (when $\|\mathbf{p}_0\| = 0$, the lower two components vanish).

---

## 3. Gaussian Wavepacket (`GaussianPacket`) and Plane Wave (`PlaneWave`)

### Inputs
- `constant_spinor`: Base constant Dirac bispinor $u(\mathbf{p}_0)$.
- `center`: Spatial center of the Gaussian envelope $\mathbf{x}_0 = (x_{0,1}, x_{0,2}, x_{0,3})$.
- `spatial_width`: Spatial width parameter $\sigma_0$.
- `grid_position`: Spatial coordinate $\mathbf{x}$ on the grid (1D, 2D, or 3D, zero-padded to 3 components for the inner product $\mathbf{p}_0 \cdot \mathbf{x}$).

### Output
- `dirac_spinor`: The initial 4-component Dirac spinor field $\psi(\mathbf{x}, 0) \in \mathbb{C}^4$ evaluated at each grid node:

$$
\psi_{\text{Gauss}}(\mathbf{x}, 0) = u(\mathbf{p}_0)\, e^{i \mathbf{p}_0 \cdot \mathbf{x}}\, \exp\left( -\frac{\|\mathbf{x} - \mathbf{x}_0\|^2}{4 \sigma_0^2} \right)
$$

For a plane wave (`PlaneWave`), the Gaussian envelope is omitted:

$$
\psi_{\text{Plane}}(\mathbf{x}, 0) = u(\mathbf{p}_0)\, e^{i \mathbf{p}_0 \cdot \mathbf{x}}
$$
