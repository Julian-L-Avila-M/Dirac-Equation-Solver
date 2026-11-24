# Plan del Proyecto para un Solucionador de la Ecuación de Dirac

> 🌐 **Idioma / Language:** **Español** | **[English](../en/problems-to-solve.md)**

Este documento describe el plan del proyecto para un solucionador numérico de la ecuación de Dirac. El solucionador aborda la ecuación

$$
(i\gamma^\mu \partial_\mu - m)\psi = 0
$$

en unidades naturales ($\hbar = c = 1$), donde el biespinor $\psi \in \mathbb{C}^4$ es un vector columna de valores complejos. Las siguientes secciones detallan los problemas físicos canónicos a resolver, junto con sus respectivos requisitos computacionales.

---

## 1. Partícula Libre

El caso de la partícula libre es el problema fundamental y sirve como punto de referencia para validar la funcionalidad central del solucionador.

### Descripción del Problema
Resolver la ecuación de Dirac para una partícula en ausencia de potencial externo ($V(\mathbf{r}) = 0$). El desarrollo inicial parte del caso unidimensional (1+1D), dado que cualquier movimiento de una partícula libre puede transformarse a esta configuración mediante una transformación de Lorentz (boost), y luego se extiende a 2D y 3D.

### Parámetros de Entrada
* **Masa de la Partícula** ($m$)
* **Momento Inicial** ($\mathbf{p}_0$)
* **Distribución Espacial**: Los estados iniciales incluyen tanto una onda plana (`PlaneWave`) como un paquete de ondas gaussiano localizado (`GaussianPacket`).
* **Estado de Espín**: La configuración inicial de polarización y superposición de espín del biespinor (`ConstantSpinor`).

### Salidas
* **Densidades de Probabilidad y Corriente**: La evolución temporal de la densidad de probabilidad $\rho = \psi^\dagger \psi$ y la densidad de corriente de probabilidad $\mathbf{J} = \psi^\dagger \boldsymbol{\alpha} \psi$.
* **Valores Esperados**: Los valores esperados de la energía $\langle H \rangle$, el momento $\langle \mathbf{p} \rangle$ y la posición $\langle \mathbf{r} \rangle$ a lo largo del tiempo, utilizados para verificar la conservación de la norma y observar fenómenos relativistas como la *Zitterbewegung*.

---

## 2. Pozo de Potencial Infinito

Este problema introduce condiciones de confinamiento espacial, probando la capacidad del solucionador para manejar barreras infinitas.

### Descripción del Problema
Resolver la ecuación de Dirac para una partícula confinada dentro de una región espacial finita de ancho $L$ (`InfiniteWellPotential`). El biespinor $\psi$ se anula fuera de las paredes del pozo.

### Parámetros de Entrada
* **Anchos del Pozo** ($L_x, [L_y], [L_z]$)
* **Masa de la Partícula** ($m$) y Estado de Espín Inicial.

### Salidas
* **Niveles de Energía Cuantizados**: Los valores propios de energía discretos del sistema.
* **Autofunciones**: Las funciones de onda del biespinor correspondientes a los estados estacionarios o cuasi-estacionarios.
* **Evolución Temporal**: La evolución temporal de la densidad de probabilidad para un paquete inicial, demostrando reflexiones internas y comportamiento oscilatorio dentro del pozo.

---

## 3. Potencial a Trozos (Efecto Klein / Paradoja de Klein)

Este caso es crucial para demostrar el fenómeno puramente relativista del túnel de Klein.

### Descripción del Problema
Resolver el comportamiento de un paquete de ondas relativista que incide sobre un escalón o barrera de potencial (`ScalarPotential`). El objetivo es demostrar que una partícula de Dirac puede penetrar y transmitirse a través de una barrera de potencial incluso cuando la altura de la barrera supera su energía total ($V_0 > E + m$), debido al acoplamiento con estados del continuo de energía negativa (creación par partícula-antipartícula).

### Parámetros de Entrada
* **Altura del Potencial** ($V_0$)
* **Energía de la Partícula** ($E$) y Momento Inicial ($\mathbf{p}_0$)
* **Masa de la Partícula** ($m$) y Estado de Espín Inicial.

### Salidas
* **Coeficientes de Reflexión ($R$) y Transmisión ($T$)**: Demostración cuantitativa de que $T > 0$ en el régimen de Klein ($V_0 > E + m$) y $R + T \approx 1$.
* **Visualización de la Función de Onda**: Gráficos y animaciones de la función de onda del biespinor que muestran las componentes incidente, reflejada y transmitida.

---

## 4. Potencial Central (Interacción de Coulomb)

Este problema es una aplicación fundamental de la ecuación de Dirac en física atómica.

### Descripción del Problema
Resolver la ecuación de Dirac para un electrón en un potencial electrostático central tipo Coulomb regularizado (`CoulombPotential`):

$$
V(\mathbf{r}) = -\frac{Z}{\|\mathbf{r}\| + \epsilon}
$$

### Parámetros de Entrada
* **Carga Efectiva Central** ($Z$) (p. ej., $Z\alpha$ en unidades naturales).
* **Parámetro de Regularización** ($\epsilon$) para evitar la singularidad numérica en $\mathbf{r} = \mathbf{0}$.
* **Masa de la Partícula** ($m$) y Estado Inicial del Paquete.

### Salidas
* **Dispersión y Estados Ligados**: Evolución de la trayectoria esperada $\langle \mathbf{r} \rangle(t)$ y deflexión del paquete de ondas.
* **Funciones de Onda**: Evolución de las 4 componentes del biespinor bajo la interacción central.
* **Acoplamiento Espín-Órbita**: Observación de la dinámica acoplada entre el momento angular orbital y el espín relativista.

---

## 5. Potencial de Yukawa

Este problema proporciona una generalización de corto alcance del potencial de Coulomb, fundamental en física nuclear y plasmas apantallados.

### Descripción del Problema
Resolver el comportamiento de una partícula de Dirac en un potencial de Yukawa de corto alcance (`YukawaPotential`):

$$
V(\mathbf{r}) = -V_0 \frac{e^{-a \|\mathbf{r}\|}}{\|\mathbf{r}\|}
$$

### Parámetros de Entrada
* **Intensidad del Potencial** ($V_0$, `strength`) y **Parámetro de Alcance Inverso** ($a$, `range`).
* **Masa de la Partícula** ($m$) y Estado Inicial.

### Salidas
* **Dinámica de Dispersión y Confinamiento**: Comparación de la deflexión y distribución de densidad frente al caso de Coulomb.
* **Funciones de Onda**: Visualización de las componentes del biespinor mostrando el efecto del apantallamiento exponencial $e^{-a r}$.
* **Verificación de Consistencia**: Demostración de que los resultados convergen al límite de Coulomb cuando el parámetro de apantallamiento $a \to 0$.
