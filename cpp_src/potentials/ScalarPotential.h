#pragma once

#include "Potential.h"
#include <pybind11/pybind11.h>
#include <functional>

namespace py = pybind11;

///////////////////////////////////////////////////////////////
/// @class ScalarPotential
/// @brief Implementación flexible de un potencial escalar definido por el usuario.
///
/// Esta clase permite definir un potencial arbitrario a partir de una función
/// escalar proporcionada por el usuario, ya sea desde C++ o desde Python (gracias
/// a la interoperabilidad con **pybind11**).
///
/// La función debe tener la forma:
/// \code{.cpp}
/// double f(const std::vector<double>& r);
/// \endcode
///
/// donde `r` es el vector de coordenadas espaciales.
///
/// ### Ejemplo (en Python)
/// ```python
/// import numpy as np
/// from dirac_solver_core import ScalarPotential
///
/// def harmonic_potential(r):
///     x, y, z = r
///     return 0.5 * (x**2 + y**2 + z**2)
///
/// V = ScalarPotential(harmonic_potential)
/// print(V.evaluate([1.0, 0.0, 0.0]))  # 0.5
/// ```
///
/// @see Potential, CoulombPotential, FreeParticle
///////////////////////////////////////////////////////////////
class ScalarPotential : public Potential {
public:
    ///////////////////////////////////////////////////////////
    /// @brief Constructor que acepta una función escalar arbitraria.
    ///
    /// @param func Función que define el valor del potencial en un punto `r`.
    ///
    /// @details
    /// La función `func` puede ser un **lambda** de C++ o una función
    /// de Python si el módulo fue expuesto con **pybind11**.
    ///
    /// @note
    /// La evaluación se realiza directamente en cada llamada a `evaluate`,
    /// por lo que funciones muy complejas podrían impactar en el rendimiento.
    ///////////////////////////////////////////////////////////
    ScalarPotential(std::function<double(const std::vector<double>&)> func)
        : func_(func) {}

    ///////////////////////////////////////////////////////////
    /// @brief Evalúa el potencial escalar en una posición dada.
    ///
    /// @param r Vector de coordenadas espaciales.
    /// @return Valor escalar del potencial.
    ///
    /// @note
    /// La función interna `func_` se invoca directamente con el vector `r`.
    ///////////////////////////////////////////////////////////
    double evaluate(const std::vector<double>& r) const override {
        return func_(r);
    }

    ///////////////////////////////////////////////////////////
    /// @brief Devuelve el nombre simbólico de la clase.
    ///
    /// @return Cadena `"ScalarPotential"`.
    ///////////////////////////////////////////////////////////
    std::string get_name() const override {
        return "ScalarPotential";
    }

private:
    ///////////////////////////////////////////////////////////
    /// @brief Función escalar que define el potencial.
    ///
    /// Puede provenir de una lambda de C++ o de un callable de Python.
    ///////////////////////////////////////////////////////////
    std::function<double(const std::vector<double>&)> func_;
};
