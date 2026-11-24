# Theoretical Foundations and Numerical Scheme

> **Language / Idioma:** [Español](../es/theory_background.md) | **English**

---

## 1. The Dirac Equation in Natural Units

In Minkowski spacetime with metric signature $g^{\mu\nu} = \mathrm{diag}(+1, -1, -1, -1)$ and natural units ($\hbar = c = 1$), the covariant free Dirac equation is:

$$(i \gamma^\mu \partial_\mu - m)\psi(x) = 0$$

where $\psi(\mathbf{r}, t) = (\psi_1, \psi_2, \psi_3, \psi_4)^T \in \mathbb{C}^4$ is a four-component bispinor and the $\gamma^\mu$ matrices generate the spacetime Clifford algebra $Cl_{1,3}(\mathbb{R})$:

$$\{\gamma^\mu, \gamma^\nu\} = \gamma^\mu \gamma^\nu + \gamma^\nu \gamma^\mu = 2 g^{\mu\nu} I_4$$

Coupling an external scalar potential $V(\mathbf{r})$ yields the time-dependent Hamiltonian form:

$$i \frac{\partial \psi(\mathbf{r}, t)}{\partial t} = \hat{H} \psi(\mathbf{r}, t) = \left( -i \boldsymbol{\alpha} \cdot \nabla + m\beta + V(\mathbf{r}) I_4 \right) \psi(\mathbf{r}, t)$$

### Standard Dirac-Pauli Representation

`Dirac-Solver` uses the standard Dirac representation in both C++ (`cpp_src/DiracMatrices.h`) and Python (`dirac_solver/DiracMatrices.py`):

$$\beta = \gamma^0 = \begin{pmatrix} I_2 & 0 \\ 0 & -I_2 \end{pmatrix}, \qquad \alpha^k = \gamma^0 \gamma^k = \begin{pmatrix} 0 & \sigma_k \\ \sigma_k & 0 \end{pmatrix}, \qquad \gamma^5 = i \gamma^0 \gamma^1 \gamma^2 \gamma^3$$

where $\sigma_1, \sigma_2, \sigma_3$ are the Pauli matrices exported from `dirac_solver/__init__.py`.

---

## 2. Free-Particle Initial States

### Pauli Spinor with Arbitrary Polarization

Given a unit polarization axis $\hat{\mathbf{n}} = (\sin\theta\cos\phi, \sin\theta\sin\phi, \cos\theta)$ and normalized complex superposition weights $(\alpha, \beta)$ satisfying $|\alpha|^2 + |\beta|^2 = 1$, `ConstantSpinor` constructs the two-component Pauli spinor:

$$\chi_s = \alpha \begin{pmatrix} \cos(\theta/2) \\ e^{i\phi}\sin(\theta/2) \end{pmatrix} + \beta \begin{pmatrix} \sin(\theta/2) \\ -e^{i\phi}\cos(\theta/2) \end{pmatrix}$$

### Constant Dirac Bispinor $u(\mathbf{p}_0, s)$

For initial momentum $\mathbf{p}_0$ and mass $m$, the relativistic energy is $E = \sqrt{\|\mathbf{p}_0\|^2 + m^2}$ and the positive-energy bispinor is:

$$u(\mathbf{p}_0, s) = \begin{pmatrix} \sqrt{E + m}\, \chi_s \\ \sqrt{E - m}\, (\hat{\mathbf{p}}_0 \cdot \boldsymbol{\sigma})\chi_s \end{pmatrix}$$

Over the spatial lattice $\Lambda$:
- **Gaussian Wavepacket (`GaussianPacket`):** $\psi(\mathbf{r}, 0) = u(\mathbf{p}_0, s)\, e^{i \mathbf{p}_0 \cdot \mathbf{r}}\, e^{-\|\mathbf{r} - \mathbf{r}_0\|^2 / (4\sigma_0^2)}$.
- **Plane Wave (`PlaneWave`):** $\psi(\mathbf{r}, 0) = u(\mathbf{p}_0, s)\, e^{i \mathbf{p}_0 \cdot \mathbf{r}}$.

---

## 3. FDTD Leapfrog Numerical Scheme

### Spatial Discretization

At interior nodes of the 1D, 2D, or 3D Cartesian grid, spatial derivatives are approximated using second-order centered finite differences $\mathcal{O}(\Delta x_k^2)$:

$$\left(\frac{\partial \psi}{\partial x_k}\right)_j \approx \frac{\psi_{j+1} - \psi_{j-1}}{2 \Delta x_k}$$

Under `PeriodicBoundary`, edge indices wrap around cyclically. Under `AbsorbingBoundary`, boundary nodes use one-sided forward/backward finite differences.

### Leapfrog Time Integration

The `FDTDLeapfrogIntegrator` maintains three time levels ($\psi^{n-1}, \psi^n, \psi^{n+1}$):

1. **Initialization ($t = -\Delta t$):** Backward Euler startup step:
   $$\psi_i^{-1} = \psi_i^{0} + i\,\Delta t\,(H\psi^0)_i$$
2. **Iterative update ($t \to t + \Delta t$):**
   $$\psi_i^{n+1} = \psi_i^{n-1} - 2i\,\Delta t\,(H\psi^n)_i$$

### CFL Stability Condition

To maintain numerical stability and relativistic causality in dimension $d \in \{1, 2, 3\}$:

$$\Delta t \le \Delta t_{\max} = \frac{1}{c \sqrt{\sum_{k=1}^d \frac{1}{\Delta x_k^2}}}$$

This limit is computed directly by `grid.courant_limit()`.

---

## 4. Boundary Conditions and Physical Observables

### Quadratic Absorbing Boundary Layer (`AbsorbingBoundary`)

Within a buffer layer of width $W$ grid nodes adjacent to each boundary, the spinor field is damped smoothly according to the minimum distance $d < W$ to the edge:

$$\Gamma(d) = 1 - \sigma_{\max}\left(\frac{W - d}{W}\right)^2, \qquad \psi_{\text{idx}} \leftarrow \Gamma(d)\,\psi_{\text{idx}}$$

### Quantum Observables (`Observables`)

| Observable | Mathematical Definition | Method |
| :--- | :--- | :--- |
| Probability density | $\rho(\mathbf{r}, t) = \psi^\dagger \psi$ | `probability_density(psi)` |
| Charge density | $J^0(\mathbf{r}, t) = \psi^\dagger \gamma^0 \psi$ | `charge_density(psi)` |
| Probability current | $J^k(\mathbf{r}, t) = c\,\psi^\dagger \alpha^k \psi$ | `probability_current(psi)` |
| Expected position | $\langle \mathbf{r} \rangle = \int \mathbf{r}\,\rho(\mathbf{r}, t)\,dV$ | `expected_value_position(rho)` |
| Expected momentum | $\langle \mathbf{p} \rangle = \int \psi^\dagger (-i\hbar \nabla)\psi\,dV$ | `expected_value_momentum(psi)` |
| Expected energy | $\langle H \rangle = \int \psi^\dagger \hat{H}\psi\,dV$ | `expected_value_energy(psi)` |
