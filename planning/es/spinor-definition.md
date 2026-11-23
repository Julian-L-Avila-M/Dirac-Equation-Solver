# Definición y Construcción de Espinores en Dirac-Solver

> 🌐 **Idioma / Language:** **Español** | **[English](../en/spinor-definition.md)**

Este documento especifica la formulación matemática exacta implementada en `dirac_solver/initial_state.py` para construir espinores de Pauli de 2 componentes, biespinores constantes de Dirac de 4 componentes y paquetes de onda localizados.

---

## 1. Construcción del Espinor de Pauli General

### Entradas
- **`spin_polarization`**: Un vector de orientación espacial $\mathbf{n} = (n_1, n_2, n_3)$, que se normaliza automáticamente a un vector unitario $\hat{n}$.
- **`spin_superposition`**: Un par de amplitudes complejas $(\alpha, \beta) \in \mathbb{C}^2$ normalizadas de forma que $|\alpha|^2 + |\beta|^2 = 1$.

### Salida
- **`pauli_spinor`**: El espinor de dos componentes $\chi_s \in \mathbb{C}^2$ que representa el estado de espín en la base computacional ($\hat{z}$):

$$
\chi_s = \alpha \chi_{\text{up}}(\hat{n}) + \beta \chi_{\text{down}}(\hat{n})
$$

### Espinores de la Base Orientada ($\hat{n}\cdot\boldsymbol{\sigma}$)
Los autoestados de $\hat{n}\cdot\boldsymbol{\sigma}$ con autovalores $+1$ (espín-arriba) y $-1$ (espín-abajo) a lo largo del eje $\hat{n}$ son:

$$
\chi_{\text{up}}(\hat{n}) = \begin{pmatrix} \cos\frac{\theta}{2} \\ e^{i \phi} \sin\frac{\theta}{2} \end{pmatrix}, \qquad
\chi_{\text{down}}(\hat{n}) = \begin{pmatrix} \sin\frac{\theta}{2} \\ -e^{i \phi} \cos\frac{\theta}{2} \end{pmatrix}
$$

### Definiciones de los Ángulos Esféricos
Los ángulos esféricos $(\theta, \phi)$ se obtienen de las componentes cartesianas del vector unitario $\hat{n} = (n_1, n_2, n_3)$:
- **Ángulo Polar ($\theta$)**: $\theta = \arccos(n_3) \in [0, \pi]$
- **Ángulo Azimutal ($\phi$)**: $\phi = \operatorname{atan2}(n_2, n_1) \in (-\pi, \pi]$

---

## 2. Biespinor Constante de Dirac (`ConstantSpinor`)

### Entradas
- `pauli_spinor`: Espinor de Pauli de 2 componentes $\chi_s$.
- `momentum`: Momento espacial inicial $\mathbf{p}_0 = (p_1, p_2, p_3)$.
- `mass`: Masa en reposo de la partícula $m$.

### Salida
- `constant_spinor`: El biespinor de energía positiva de partícula libre $u(\mathbf{p}_0) \in \mathbb{C}^4$:

$$
u(\mathbf{p}_0) = \begin{pmatrix} \sqrt{E + m}\, \chi_s \\ \sqrt{E - m}\, (\hat{\mathbf{p}}_0 \cdot \boldsymbol{\sigma}) \chi_s \end{pmatrix}
$$

donde:
- $E = \sqrt{\|\mathbf{p}_0\|^2 + m^2}$ es la energía relativista en unidades naturales ($\hbar = c = 1$).
- $\hat{\mathbf{p}}_0 \cdot \boldsymbol{\sigma} = \frac{1}{\|\mathbf{p}_0\|} (p_1 \sigma_1 + p_2 \sigma_2 + p_3 \sigma_3)$ es el operador de helicidad (proyección de las matrices de Pauli sobre la dirección del momento; si $\|\mathbf{p}_0\| = 0$, el bloque inferior se anula).

---

## 3. Paquete de Ondas Gaussiano (`GaussianPacket`) y Onda Plana (`PlaneWave`)

### Entradas
- `constant_spinor`: El biespinor constante base $u(\mathbf{p}_0)$.
- `center`: Centro espacial de la distribución gaussiana $\mathbf{x}_0 = (x_{0,1}, x_{0,2}, x_{0,3})$.
- `spatial_width`: Ancho espacial del paquete $\sigma_0$.
- `grid_position`: Coordenada espacial $\mathbf{x}$ en la malla (1D, 2D o 3D, completada con ceros a 3 componentes para el producto punto $\mathbf{p}_0 \cdot \mathbf{x}$).

### Salida
- `dirac_spinor`: El campo espinorial inicial $\psi(\mathbf{x}, 0) \in \mathbb{C}^4$ evaluado en cada nodo de la malla:

$$
\psi_{\text{Gauss}}(\mathbf{x}, 0) = u(\mathbf{p}_0)\, e^{i \mathbf{p}_0 \cdot \mathbf{x}}\, \exp\left( -\frac{\|\mathbf{x} - \mathbf{x}_0\|^2}{4 \sigma_0^2} \right)
$$

Para el caso de una onda plana (`PlaneWave`), la envolvente gaussiana se omite:

$$
\psi_{\text{Plana}}(\mathbf{x}, 0) = u(\mathbf{p}_0)\, e^{i \mathbf{p}_0 \cdot \mathbf{x}}
$$
