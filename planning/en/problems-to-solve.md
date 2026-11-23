# Project Plan for a Numerical Dirac Equation Solver

> 🌐 **Language / Idioma:** **[Español](../es/problems-to-solve.md)** | **English**

This document outlines the project plan for a numerical solver of the time-dependent Dirac equation. The solver handles the equation

$$
(i\gamma^\mu \partial_\mu - m)\psi = 0
$$

in natural units ($\hbar = c = 1$), where the bispinor $\psi \in \mathbb{C}^4$ is a 4-component complex column vector. The following sections detail the canonical physical problems addressed by the library along with their computational requirements.

---

## 1. Free Particle

The free-particle case is the foundational benchmark problem, serving as the reference testbed for the core time-integration engine.

### Problem Description
Solve the Dirac equation for a relativistic spin-$1/2$ particle in the absence of an external potential ($V(\mathbf{r}) = 0$). Initial validation starts in one spatial dimension (1+1D)—since any free-particle motion can be related to 1D via a Lorentz boost—and generalizes seamlessly to 2D and 3D spatial grids.

### Input Parameters
* **Particle Rest Mass** ($m$)
* **Initial Momentum** ($\mathbf{p}_0$)
* **Spatial Distribution**: Initial states include both an analytical plane wave (`PlaneWave`) and a physically localized Gaussian wavepacket (`GaussianPacket`).
* **Spin Configuration**: Initial spin polarization axis and complex superposition weights (`ConstantSpinor`).

### Outputs
* **Probability and Current Densities**: Time evolution of the probability density $\rho = \psi^\dagger \psi$ and probability current density $\mathbf{J} = \psi^\dagger \boldsymbol{\alpha} \psi$.
* **Expectation Values**: Time series of expected energy $\langle H \rangle$, momentum $\langle \mathbf{p} \rangle$, and position $\langle \mathbf{r} \rangle$, used to verify probability conservation, group-velocity dispersion, and relativistic phenomena such as *Zitterbewegung*.

---

## 2. Infinite Potential Well

This problem introduces spatial confinement, testing the solver's capability to handle impenetrable boundaries.

### Problem Description
Solve the Dirac equation for a particle confined within a finite box of widths $L_x, [L_y], [L_z]$ (`InfiniteWellPotential`). The bispinor $\psi$ vanishes outside the well walls where $V(\mathbf{r}) = \infty$.

### Input Parameters
* **Well Widths** ($L_x, [L_y], [L_z]$)
* **Particle Mass** ($m$) and Initial Spin/Momentum State.

### Outputs
* **Quantized Confinement Dynamics**: Discrete standing-wave modes and energy expectation values of the confined system.
* **Spinor Wavefunctions**: Spatial profiles of the 4 bispinor components inside the box.
* **Time Evolution**: Time evolution of the probability density for an initially localized wavepacket, demonstrating internal reflections and interference oscillations inside the well.

---

## 3. Piecewise Potential (Klein Effect / Klein Paradox)

This benchmark is essential for demonstrating the purely relativistic phenomenon of Klein tunneling.

### Problem Description
Simulate a relativistic wavepacket impinging on a potential step or rectangular barrier (`ScalarPotential`). The objective is to demonstrate that a Dirac particle can penetrate and transmit through a potential barrier even when the barrier height exceeds its total energy by more than twice the rest mass ($V_0 > E + m$), due to coupling between positive-energy particle states and negative-energy antiparticle states inside the barrier.

### Input Parameters
* **Potential Barrier Height** ($V_0$) and geometry
* **Particle Energy** ($E$) and Initial Momentum ($\mathbf{p}_0$)
* **Particle Mass** ($m$) and Initial Spin State.

### Outputs
* **Reflection ($R$) and Transmission ($T$) Coefficients**: Quantitative verification that non-zero transmission ($T > 0$) persists in the Klein regime ($V_0 > E + m$) while conserving total probability ($R + T \approx 1$).
* **Wavefunction Visualization**: Plots and animations of the bispinor density showing the incident, reflected, and transmitted wavepackets.

---

## 4. Central Potential (Coulomb Interaction)

This problem addresses a cornerstone application of the Dirac equation in atomic and relativistic scattering physics.

### Problem Description
Solve the Dirac equation for an electron interacting with a regularized central electrostatic Coulomb potential (`CoulombPotential`):

$$
V(\mathbf{r}) = -\frac{Z}{\|\mathbf{r}\| + \epsilon}
$$

### Input Parameters
* **Effective Central Charge** ($Z$) (e.g., $Z\alpha$ in natural units).
* **Regularization Parameter** ($\epsilon$) to remove the numerical singularity at $\mathbf{r} = \mathbf{0}$.
* **Particle Mass** ($m$) and Initial Wavepacket State.

### Outputs
* **Scattering and Bound-State Dynamics**: Time evolution of the expected trajectory $\langle \mathbf{r} \rangle(t)$ and wavepacket deflection angle.
* **Bispinor Components**: Spatial evolution of upper and lower spinor components under a central field.
* **Spin-Orbit Coupling**: Observation of relativistic coupling between orbital motion and spin polarization.

---

## 5. Yukawa Potential

This problem provides a short-range screened generalization of the Coulomb potential, relevant to nuclear physics and screened plasmas.

### Problem Description
Simulate a Dirac particle in a short-range Yukawa potential (`YukawaPotential`):

$$
V(\mathbf{r}) = -V_0 \frac{e^{-a \|\mathbf{r}\|}}{\|\mathbf{r}\|}
$$

### Input Parameters
* **Coupling Strength** ($V_0$, `strength`) and **Inverse Screening Range** ($a$, `range`).
* **Particle Mass** ($m$) and Initial State.

### Outputs
* **Short-Range Scattering & Confinement**: Comparison of wavepacket scattering and localization against the long-range Coulomb potential.
* **Wavefunctions**: Visualization of the bispinor components illustrating the effect of the exponential screening factor $e^{-a r}$.
* **Consistency Verification**: Demonstration that simulation results converge smoothly to the Coulomb case in the limit $a \to 0$.
