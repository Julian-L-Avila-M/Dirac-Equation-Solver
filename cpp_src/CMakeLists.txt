cmake_minimum_required(VERSION 3.12)
project(dirac_solver_core)

# Add pybind11
# You can install this system-wide or add it as a submodule.
# For this example, we assume it's findable by CMake.
find_package(pybind11 REQUIRED)

# Python Extension Module
# This command creates a Python module named "_core" from the source file "bindings.cpp".
# The "MODULE" keyword is crucial for building a loadable Python module.
pybind11_add_module(_core
    bindings.cpp
)

# Add include directories to the module target
target_include_directories(_core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/potentials
    ${CMAKE_CURRENT_SOURCE_DIR}/boundaries
)

# Install the created module into the dirac_solver package directory
install(TARGETS _core DESTINATION dirac_solver)