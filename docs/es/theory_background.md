# Fundamentos Teóricos y Esquema Numérico

> **Idioma / Language:** **Español** | [English](../en/theory_background.md)

---

## 1. La Ecuación de Dirac en Unidades Naturales

En el espacio-tiempo de Minkowski con signatura métrica $g^{\mu\nu} = \mathrm{diag}(+1, -1, -1, -1)$ y adoptando unidades naturales ($\hbar = c = 1$), la ecuación de Dirac libre en forma covariante es:

$$(i \gamma^\mu \partial_\mu - m)\psi(x) = 0$$

donde $\psi(\mathbf{r}, t) = (\psi_1, \psi_2, \psi_3, \psi_4)^T \in \mathbb{C}^4$ es un biespinor de cuatro componentes y las matrices $\gamma^\mu$ generan el álgebra de Clifford $Cl_{1,3}(\mathbb{R})$:

$$\{\gamma^\mu, \gamma^\nu\} = \gamma^\mu \gamma^\nu + \gamma^\nu \gamma^\mu = 2 g^{\mu\nu} I_4$$

Al acoplar un potencial escalar externo $V(\mathbf{r})$ y reescribir la ecuación en forma de evolución temporal (hamiltoniana):

$$i \frac{\partial \psi(\mathbf{r}, t)}{\partial t} = \hat{H} \psi(\mathbf{r}, t) = \left( -i \boldsymbol{\alpha} \cdot \nabla + m\beta + V(\mathbf{r}) I_4 \right) \psi(\mathbf{r}, t)$$

### Representación Estándar de Dirac-Pauli

`Dirac-Solver` utiliza la representación estándar de Dirac tanto en C++ (`cpp_src/DiracMatrices.h`) como en Python (`dirac_solver/DiracMatrices.py`):

$$\beta = \gamma^0 = \begin{pmatrix} I_2 & 0 \\ 0 & -I_2 \end{pmatrix}, \qquad \alpha^k = \gamma^0 \gamma^k = \begin{pmatrix} 0 & \sigma_k \\ \sigma_k & 0 \end{pmatrix}, \qquad \gamma^5 = i \gamma^0 \gamma^1 \gamma^2 \gamma^3$$

donde $\sigma_1, \sigma_2, \sigma_3$ son las matrices de Pauli definidas en `dirac_solver/__init__.py`.

---

## 2. Estados Iniciales de Partícula Libre

### Espinor de Pauli y Polarización Arbitraria

Dado un vector unitario de polarización $\hat{\mathbf{n}} = (\sin\theta\cos\phi, \sin\theta\sin\phi, \cos\theta)$ y coeficientes complejos normalizados $(\alpha, \beta)$ con $|\alpha|^2 + |\beta|^2 = 1$, `ConstantSpinor` construye el espinor de Pauli:

$$\chi_s = \alpha \begin{pmatrix} \cos(\theta/2) \\ e^{i\phi}\sin(\theta/2) \end{pmatrix} + \beta \begin{pmatrix} \sin(\theta/2) \\ -e^{i\phi}\cos(\theta/2) \end{pmatrix}$$

### Biespinor Constante de Dirac $u(\mathbf{p}_0, s)$

Para un momento inicial $\mathbf{p}_0$ y masa $m$, la energía relativista es $E = \sqrt{\|\mathbf{p}_0\|^2 + m^2}$ y el biespinor positivo se construye como:

$$u(\mathbf{p}_0, s) = \begin{pmatrix} \sqrt{E + m}\, \chi_s \\ \sqrt{E - m}\, (\hat{\mathbf{p}}_0 \cdot \boldsymbol{\sigma})\chi_s \end{pmatrix}$$

Sobre la malla espacial $\Lambda$:
- **Paquete Gaussiano (`GaussianPacket`):** $\psi(\mathbf{r}, 0) = u(\mathbf{p}_0, s)\, e^{i \mathbf{p}_0 \cdot \mathbf{r}}\, e^{-\|\mathbf{r} - \mathbf{r}_0\|^2 / (4\sigma_0^2)}$.
- **Onda Plana (`PlaneWave`):** $\psi(\mathbf{r}, 0) = u(\mathbf{p}_0, s)\, e^{i \mathbf{p}_0 \cdot \mathbf{r}}$.

---

## 3. Esquema Numérico FDTD Leapfrog

### Discretización Espacial

En los nodos interiores de la malla cartesiana 1D, 2D o 3D, las derivadas espaciales se aproximan mediante diferencias finitas centradas de segundo orden $\mathcal{O}(\Delta x_k^2)$:

$$\left(\frac{\partial \psi}{\partial x_k}\right)_j \approx \frac{\psi_{j+1} - \psi_{j-1}}{2 \Delta x_k}$$

Con fronteras periódicas (`PeriodicBoundary`), los extremos se conectan cíclicamente. Con fronteras abiertas/absorbentes (`AbsorbingBoundary`), los nodos extremos usan diferencias laterales hacia adelante/atrás.

### Integración Temporal Leapfrog

El motor `FDTDLeapfrogIntegrator` evoluciona el campo en tres niveles temporales ($\psi^{n-1}, \psi^n, \psi^{n+1}$):

1. **Inicialización ($t = -\Delta t$):** Paso de Euler hacia atrás:
   $$\psi_i^{-1} = \psi_i^{0} + i\,\Delta t\,(H\psi^0)_i$$
2. **Paso iterativo ($t \to t + \Delta t$):**
   $$\psi_i^{n+1} = \psi_i^{n-1} - 2i\,\Delta t\,(H\psi^n)_i$$

### Condición de Estabilidad CFL

Para preservar la estabilidad numérica y la causalidad relativista en dimensión $d \in \{1, 2, 3\}$:

$$\Delta t \le \Delta t_{\max} = \frac{1}{c \sqrt{\sum_{k=1}^d \frac{1}{\Delta x_k^2}}}$$

Este límite se calcula directamente mediante `grid.courant_limit()`.

---

## 4. Condiciones de Frontera y Observables Físicos

### Frontera Absorbente Cuadrática (`AbsorbingBoundary`)

Dentro de una banda de ancho $W$ celdas junto a los límites de la malla, el campo se atenúa suavemente según la distancia mínima al borde $d < W$:

$$\Gamma(d) = 1 - \sigma_{\max}\left(\frac{W - d}{W}\right)^2, \qquad \psi_{\text{idx}} \leftarrow \Gamma(d)\,\psi_{\text{idx}}$$

### Observables Cuánticos (`Observables`)

| Observable | Expresión Matemática | Método |
| :--- | :--- | :--- |
| Densidad de probabilidad | $\rho(\mathbf{r}, t) = \psi^\dagger \psi$ | `probability_density(psi)` |
| Densidad de carga | $J^0(\mathbf{r}, t) = \psi^\dagger \gamma^0 \psi$ | `charge_density(psi)` |
| Corriente de probabilidad | $J^k(\mathbf{r}, t) = c\,\psi^\dagger \alpha^k \psi$ | `probability_current(psi)` |
| Posición esperada | $\langle \mathbf{r} \rangle = \int \mathbf{r}\,\rho(\mathbf{r}, t)\,dV$ | `expected_value_position(rho)` |
| Momento esperado | $\langle \mathbf{p} \rangle = \int \psi^\dagger (-i\hbar \nabla)\psi\,dV$ | `expected_value_momentum(psi)` |
| Energía esperada | $\langle H \rangle = \int \psi^\dagger \hat{H}\psi\,dV$ | `expected_value_energy(psi)` |
