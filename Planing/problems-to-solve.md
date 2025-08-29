# Plan del Proyecto para un Solucionador de la Ecuación de Dirac
Este documento describe el plan del proyecto para un solucionador de la
ecuación de Dirac. El solucionador manejará la ecuación
$(i\gamma^\mu \partial_\mu - m)\psi=0$ en unidades naturales, donde el bispinor
$\psi \in \mathbb{C}^4$ es un vector columna de valores complejos. Las
siguientes secciones detallan los problemas a abordar, junto con sus
respectivos requisitos computacionales.

---

## 1. Partícula Libre
El caso de la partícula libre es el problema fundamental, sirviendo como
referencia para la funcionalidad central del solucionador.

### Descripción del Problema
Resolver la ecuación de Dirac para una partícula en ausencia de un potencial
externo. El desarrollo inicial se centrará en el caso unidimensional (1+1D), ya
que cualquier movimiento de una partícula libre puede ser transformado a esta
configuración mediante un boost de Lorentz.

### Parámetros de Entrada
* **Masa de la Partícula** ($m$)
* **Momento Inicial** ($p$)
* **Distribución Espacial**: Los estados iniciales incluirán tanto una simple
  onda plana como un paquete de ondas Gaussiano más físicamente realista.
* **Estado de Espín**: La configuración inicial del espín del bispinor.

### Salidas
* **Densidades de Probabilidad y Corriente**: La evolución temporal de la
  densidad de probabilidad $\rho = \psi^\dagger \psi$ y la densidad de corriente
  $J = \psi^\dagger \alpha \psi$.
* **Valores Esperados**: Los valores esperados para la energía, el momento y la
  posición a lo largo del tiempo, que se utilizarán para observar fenómenos
  relativistas como la Zitterbewegung.

---

## 2. Pozo de Potencial Infinito
Este problema introduce condiciones de contorno, probando la capacidad del
solucionador para manejar el confinamiento.

### Descripción del Problema
Resolver la ecuación de Dirac para una partícula confinada dentro de un pozo de
potencial infinito de ancho $L$. El bispinor $\psi$ debe anularse en las
paredes del pozo.

### Parámetros de Entrada
* **Ancho del Pozo** ($L$)
* **Masa de la Partícula** ($m$) y Estado de Espín Inicial.

### Salidas
* **Niveles de Energía Cuantizados**: Los valores propios de energía discretos
	del	sistema.
* **Autofunciones**: Las funciones de onda del bispinor correspondientes a
  cada nivel de energía.
* **Evolución Temporal**: La evolución temporal de la densidad de probabilidad
  para un estado inicial, demostrando el comportamiento oscilatorio dentro del
  pozo.

---

## 3. Potencial a Trozos (Efecto Klein)
Este caso es crucial para demostrar el fenómeno relativista del Efecto Klein.

### Descripción del Problema
Resolver el comportamiento de una partícula que encuentra un escalón de
potencial. El objetivo es demostrar que una partícula relativista puede
atravesar una barrera de potencial incluso cuando su energía es menor que la
altura de la barrera, un resultado clásicamente imposible.

### Parámetros de Entrada
* **Altura del Potencial** ($V_0$)
* **Energía de la Partícula** ($E$) y Momento Inicial ($p$)
* **Masa de la Partícula** ($m$) y Estado de Espín Inicial.

### Salidas
* **Coeficientes de Reflexión (R) y Transmisión (T)**: La salida principal,
  que demuestra que la transmisión puede ocurrir incluso cuando $V_0 > E+m$.
* **Visualización de la Función de Onda**: Un gráfico de la función de onda del
  bispinor que muestra los componentes entrante, reflejado y transmitido.

---

## 4. Potencial Central (Interacción de Coulomb)
Este problema es una aplicación fundamental de la ecuación de Dirac, abordando
la estructura fina del átomo de hidrógeno.

### Descripción del Problema
Resolver la ecuación de Dirac para un electrón en un potencial central,
específicamente el potencial de Coulomb $V(r) = -Z\alpha/r$.

### Parámetros de Entrada
* **Carga Central** ($Z$) (p.ej., $Z=1$ para el hidrógeno).
* **Masa de la Partícula** ($m$) (la masa del electrón).
* **Número Cuántico del Momento Angular** ($j$): La solución se maneja mejor en
  coordenadas esféricas.

### Salidas
* **Niveles de Energía Cuantizados**: Los valores propios de energía discretos,
	que deben coincidir con la fórmula de la estructura fina de Sommerfeld.
* **Funciones de Onda**: Los componentes radiales de la función de onda del
  bispinor.
* **Desdoblamiento Espín-Órbita**: El solucionador debe reproducir
  correctamente el desdoblamiento de los niveles de energía debido a la
  interacción espín-órbita.

---

## 5. Potencial de Yukawa
Este problema proporciona una generalización crucial del potencial de Coulomb,
relevante para la física nuclear.

### Descripción del Problema
Resolver el comportamiento de una partícula en un potencial de Yukawa de corto
alcance $V(r) = -g \frac{e^{-Mr}}{r}$.

### Parámetros de Entrada
* **Constante de Acoplamiento** ($g$) y **Masa de la Partícula Mediadora**
  ($M$): Estos parámetros definen la fuerza y el alcance del potencial.
* **Masa de la Partícula** ($m$) y Estado Inicial.

### Salidas
* **Energías de los Estados Ligados**: Los valores propios de energía discretos
	para los estados ligados.
* **Funciones de Onda**: Una visualización de las funciones de onda del bispinor,
  mostrando cómo difieren del caso de Coulomb debido al decaimiento
  exponencial del potencial.
* **Verificación de Consistencia**: La salida debe demostrar que los resultados
  convergen al caso de Coulomb cuando la masa de la partícula mediadora
  $M \to 0$.
