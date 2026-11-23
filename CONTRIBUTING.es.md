# Cómo Contribuir a Dirac-Solver

> 🌐 **Idioma / Language:** **[Español (Principal)](CONTRIBUTING.md)** | **[English](CONTRIBUTING.en.md)**

¡Gracias por tu interés en contribuir a **Dirac-Solver**! Tu ayuda es bienvenida. Para que el proceso sea sencillo y ordenado para todos, aquí definimos nuestras guías de contribución.

## Tipos de Contribuciones

Aceptamos varios tipos de contribuciones, incluyendo:
* Reportes de errores (bugs).
* Sugerencias de nuevas funcionalidades o mejoras de rendimiento.
* Contribuciones al código (correcciones o nuevas características en C++ o Python).
* Mejoras en la documentación y cuadernos de ejemplo (en Español e Inglés).

## Reporte de Errores (Bugs) y Sugerencias

Para reportar un error o sugerir una mejora, por favor **utiliza la pestaña "Issues"** de GitHub.

* **Revisa las issues existentes:** Antes de crear una nueva, verifica si alguien más ya reportó el mismo problema o sugirió la misma idea.
* **Sé claro:**
    * **Para Bugs:** Describe el problema, los pasos para reproducirlo, el entorno (sistema operativo, versión de Python y compilador C++) y qué resultado esperabas.
    * **Para Sugerencias:** Explica tu idea, su fundamento físico o computacional y por qué sería útil para el proyecto.

## Contribuir con Código (Pull Requests)

Este es el flujo de trabajo recomendado para enviar cambios al código:

### 1. Configura tu Entorno

Asegúrate de tener el proyecto listo para trabajar localmente.

1.  **Haz un "Fork"** del repositorio `Dirac-Equation-Solver` a tu propia cuenta de GitHub.
2.  **Clona** tu fork en tu máquina local:
    ```bash
    git clone https://github.com/TU_USUARIO/Dirac-Equation-Solver.git
    cd Dirac-Solver
    ```
    *(Reemplaza `TU_USUARIO` con tu nombre de usuario de GitHub)*.

3.  **Instala el proyecto en modo editable.** Esto compila la extensión C++ `_core` mediante `scikit-build-core` y permite probar tus cambios de inmediato:
    ```bash
    pip install -e .
    ```

### 2. Realiza tus Cambios

1.  **Crea una nueva rama** para tus cambios. No trabajes directamente en `main`:
    ```bash
    git checkout -b feature/mi-nueva-feature
    ```
    *(Usa un nombre descriptivo, como `fix/error-de-matriz` o `feature/nuevo-potencial`)*.

2.  **Escribe tu código** y ejecuta las pruebas unitarias con `pytest` para asegurarte de no introducir regresiones:
    ```bash
    pytest
    ```

3.  **Actualiza la documentación (si es necesario).** Si tu cambio afecta la forma en que se usa el código o añade una funcionalidad nueva, por favor actualiza la documentación bilingüe (`docs/es/` y `docs/en/`) y los comentarios Doxygen. Puedes generar la documentación localmente para revisarla:
    ```bash
    doxygen Doxyfile
    cd ./docs/latex
    make
    ```
    *(Para la versión en inglés, ejecuta `doxygen Doxyfile.en`)*.

### 3. Envía tus Cambios

1.  **Haz "commit"** de tus cambios con un mensaje claro siguiendo convenciones semánticas:
    ```bash
    git add .
    git commit -m "feat: Añade el método X para calcular Y"
    ```
2.  **Sube tu rama** a tu fork en GitHub:
    ```bash
    git push origin feature/mi-nueva-feature
    ```
3.  **Abre un "Pull Request" (PR)** en GitHub desde tu rama hacia la rama `main` del repositorio original de `Dirac-Solver`.
4.  **Describe tu PR:** Explica qué cambios hiciste y por qué. Si tu PR resuelve una "Issue" existente, menciónala (ej. `"Cierra #12"`).

¡Y eso es todo! Revisaremos tu contribución lo antes posible.
