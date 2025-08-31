# Dirac-Solver: Solucionador numérico de la Ecuación de Dirac.

Este documento describe el plan del proyecto para un solucionador de la
ecuación de Dirac. El solucionador manejará la ecuación
$(i\gamma^\mu \partial_\mu - m)\psi=0$ en unidades naturales, donde el bispinor
$\psi \in \mathbb{C}^4$ es un vector columna de valores complejos. Las
siguientes secciones detallan los problemas a abordar, junto con sus
respectivos requisitos computacionales.


## Documentación

Para compilar la documentación, en el directorio principal ejecutar:

-> Doxigen

Esto creará los subdirectorios ./docs/html y ./docs/latex

Para obtener el .pdf de latex, desde el directorio principal:

-> cd ./docs/html 
-> make

## Instalación de la libreria

En el entorno principal ejecutar:
 
-> pip install .

Para desarrollo:

-> pip install -e .

"-e": modo editable

