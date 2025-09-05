## Construcción del Espinor de Pauli General
### Entradas
- **`spin_polarization`**: Un vector unitario $\hat{n} = (n_1, n_2, n_3)$.
- **`spin_superposition`**: Un par de números complejos $\alpha, \beta$ donde
$|\alpha|^2 + |\beta|^2 = 1$.
### Salida
- **`pauli_spinor`**: El espinor $\chi_s$ que representa el estado en la base
computacional, dado por:

$$\chi_s = \alpha \chi_{\text{up}} + \beta \chi_{\text{down}}$$

### Espinores de la Base
Los espinores de la base, correspondientes a espín-arriba y espín-abajo a lo
largo del eje $\hat{n}$, son:

$$\chi_{\text{up}} = \pmatrix{ \cos{\frac{\theta}{2}} \\ e^{i \phi} \sin{\frac{\theta}{2}} } \, , \quad
\chi_{\text{down}} = \pmatrix{ \sin{\frac{\theta}{2}} \\ - e^{i \phi} \cos{\frac{\theta}{2}} }$$

### Definiciones de los Ángulos
Los ángulos se derivan de las componentes del vector unitario $\hat{n} = \{n_1,
n_2, n_3\}$ de la siguiente manera:
- **Ángulo Polar ($\theta$)**: $\theta = \arccos{(n_3)}$
- **Ángulo Azimutal ($\phi$)**: $\phi = \mathrm{atan2}{(n_2, n_1)}$

---

## Espinor Constante
### Entradas
- `pauli_spinor`: Espinor de Pauli $\chi_s$.
- `momentum_initial`: Momento inicial $p_0 = (p_1, p_2, p_3)$.
- `rest_mass`: Masa en reposo de la partícula $m$.
### Salida
- `constant_spinor`: El espinor constante $u(p_0)$, dado por:

$$u(p_0) = \pmatrix{ \sqrt{E + m} \chi_s \\ \sqrt{E - m} (\hat{p} \cdot \sigma) \chi_s}$$

donde:
- $E = \sqrt{p^2_0 + m^2}$ es la energía positiva.
- $\hat{p} \cdot \sigma = \frac{1}{|p|} (p_1 \sigma_1 + p_2 \sigma_2 + p_3
\sigma_3)$ es la proyección del momento con el espín.

---

## Paquete Gaussiano
### Entradas
- `constant_spinor`: El espinor constante $u(p_0)$.
- `momentum_initial`: Momento inicial $p_0 = (p_1, p_2, p_3)$.
- `distribution_center`: Centro de la distribución $x_0$.
- `spatial_width`: Ancho espacial de la distribución $\sigma_0$.
- `grid_position`: Punto de posición en la malla $x = (x_1, x_2, x_3)$.
### Salida
- `dirac_spinor`: El espinor de Dirac $\psi(x, 0)$ para la Ecuación de Dirac.

$$\psi(x, 0) = \mathcal{N} u(p_0) e^{i p_0 \cdot x} e^{- \frac{|x - x_0|^2}{4 \sigma^2_0}}$$

donde $\mathcal{N}$ es una constante de normalización, la cual podemos
determinar por integración numérica sobre la malla.
