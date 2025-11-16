# Cómo Contribuir a Dirac-Solver

¡Gracias por tu interés en contribuir a Dirac-Solver! Tu ayuda es bienvenida. Para que el proceso sea sencillo para todos, aquí definimos algunas guías.

## Tipos de Contribuciones

Aceptamos varios tipos de contribuciones, incluyendo:
* Reportes de errores (bugs).
* Sugerencias de nuevas funcionalidades o mejoras.
* Contribuciones al código (correcciones o nuevas características).
* Mejoras en la documentación.

## Reporte de Errores (Bugs) y Sugerencias

Para reportar un error o sugerir una mejora, por favor **utiliza la pestaña "Issues"** de GitHub.

* **Revisa las issues existentes:** Antes de crear una nueva, mira si alguien más ya reportó el mismo problema o sugirió la misma idea.
* **Sé claro:**
    * **Para Bugs:** Describe el problema, los pasos para reproducirlo y qué resultado esperabas.
    * **Para Sugerencias:** Explica tu idea y por qué crees que sería útil para el proyecto.

## Contribuir con Código (Pull Requests)

Este es el flujo de trabajo que te pedimos seguir para enviar cambios al código:

### 1. Configura tu Entorno
Asegúrate de tener el proyecto listo para trabajar en él.

1.  **Haz un "Fork"** del repositorio `Dirac-Equation-Solver` a tu propia cuenta de GitHub.
2.  **Clona** tu fork en tu máquina local:
    ```bash
    git clone https://github.com/TU_USUARIO/Dirac-Equation-Solver.git
    cd Dirac-Solver
    ```
    *(Reemplaza `TU_USUARIO` con tu nombre de usuario de GitHub)*

3.  **Instala el proyecto en modo editable.** Esto es muy importante, ya que te permite probar tus cambios localmente.
    ```bash
    pip install -e .
    ```

### 2. Realiza tus Cambios

1.  **Crea una nueva rama** para tus cambios. No trabajes directamente en `main`.
    ```bash
    git checkout -b feature/mi-nueva-feature
    ```
    *(Usa un nombre descriptivo, como `fix/error-de-matriz` o `feature/nuevo-metodo`)*.

2.  **Escribe tu código** y realiza los cambios necesarios.

3.  **Actualiza la documentación (si es necesario).** Si tu cambio afecta la forma en que se usa el código o añade algo nuevo, por favor actualiza la documentación. Puedes generarla localmente para revisarla:
    ```bash
    Doxigen
    cd ./docs/latex
    make
    ```

### 3. Envía tus Cambios

1.  **Haz "commit"** de tus cambios con un mensaje claro.
    ```bash
    git add .
    git commit -m "feat: Añade el método X para calcular Y"
    ```
2.  **Sube tu rama** a tu fork en GitHub:
    ```bash
    git push origin feature/mi-nueva-feature
    ```
3.  **Abre un "Pull Request" (PR)** en GitHub desde tu rama a la rama `main` del repositorio original de `Dirac-Solver`.
4.  **Describe tu PR:** Explica qué cambios hiciste y por qué. Si tu PR resuelve una "Issue" existente, menciónala (ej. "Cierra #12").

¡Y eso es todo! Revisaremos tu contribución lo antes posible.
