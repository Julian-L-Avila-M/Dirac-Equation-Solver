cmake_minimum_required(VERSION 3.12)

###############################################################
# @file CMakeLists.txt
# @brief Configuración de compilación para el módulo nativo `dirac_solver_core`.
#
# Este archivo define las reglas de construcción del backend C++ del solver de Dirac,
# incluyendo el módulo Python `_core` mediante **pybind11**.
#
# Estructura general:
# - Definición del proyecto y versión mínima de CMake.
# - Detección de dependencias (pybind11).
# - Creación del módulo Python con pybind11_add_module().
# - Inclusión de directorios locales para headers.
# - Instalación del módulo en el paquete `dirac_solver`.
#
# @ingroup build_system
###############################################################

project(dirac_solver_core LANGUAGES CXX)

###############################################################
# Dependencia: pybind11
#
# Se asume que pybind11 está disponible en el sistema (vía `pip install pybind11`
# o como submódulo de git). Esta dependencia provee las macros y funciones
# necesarias para generar módulos Python desde código C++.
#
# @note
# Si no se encuentra pybind11, CMake mostrará un error en esta etapa.
###############################################################
find_package(pybind11 REQUIRED)

###############################################################
# Módulo Python `_core`
#
# El siguiente bloque construye un módulo Python compartido llamado `_core`,
# a partir del archivo fuente `bindings.cpp`, donde se definen todos los
# enlaces (bindings) entre C++ y Python.
#
# @note
# La palabra clave `MODULE` es esencial: indica a CMake que debe generar
# una librería compartida compatible con la carga dinámica de Python
# (`import _core`).
###############################################################
pybind11_add_module(_core
    bindings.cpp
)

###############################################################
# Inclusión de directorios
#
# Se añaden los directorios que contienen los headers locales del proyecto:
# - `potentials/` : Implementaciones de potenciales escalares (Coulomb, libre, escalar, etc.)
# - `boundaries/` : Implementaciones de condiciones de frontera (periódicas, absorbentes, etc.)
#
# Esto permite a `bindings.cpp` y a otras unidades de compilación incluir
# estos headers con rutas relativas simples.
###############################################################
target_include_directories(_core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/potentials
    ${CMAKE_CURRENT_SOURCE_DIR}/boundaries
)

###############################################################
# Instalación del módulo
#
# Una vez compilado, el módulo `_core` se instalará en el subdirectorio
# `dirac_solver/` dentro del entorno Python activo.
#
# Esto permite importarlo directamente desde Python:
# ```python
# from dirac_solver import _core
# ```
#
# @see bindings.cpp, pybind11_add_module
###############################################################
install(TARGETS _core DESTINATION dirac_solver)
