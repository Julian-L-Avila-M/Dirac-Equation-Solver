#pragma once

#include "Potential.h"
#include <cmath>
#include <numeric>

///////////////////////////////////////////////////////////////
/// @class YukawaPotential
/// @brief Implementación de un potencial de Yukawa.
///
/// Esta clase deriva de la clase abstracta @ref Potential y define
/// un potencial de Yukawa, que es un potencial de corto alcance.
///
/// La expresión implementada es:
/// 
///   \\[   V(r) = -V_0 \\frac{e^{-ar}}{r} \\]
/// 
///
/// donde:
/// - \\( V_0 \\) es la fuerza del potencial.
/// - \\( a \\) es el parámetro de rango.
///
/// @note Este potencial puede emplearse tanto en simulaciones 1D, 2D o 3D.
///////////////////////////////////////////////////////////////
class YukawaPotential : public Potential {
public:
    ///////////////////////////////////////////////////////////
    /// @brief Constructor principal del potencial de Yukawa.
    ///
    /// @param strength La fuerza del potencial (V_0).
    /// @param range El rango del potencial (a).
    ///////////////////////////////////////////////////////////
    YukawaPotential(double strength, double range)
        : strength_(strength), range_(range) {}

    ///////////////////////////////////////////////////////////
    /// @brief Evalúa el potencial en un punto espacial.
    ///
    /// @param r Vector de posición (1D, 2D o 3D).
    /// @return Valor escalar del potencial \\( V(r) \\).
    ///////////////////////////////////////////////////////////
    double evaluate(const std::vector<double>& r) const override {
        double r_norm = 0.0;
        for (double val : r) {
            r_norm += val * val;
        }
        r_norm = std::sqrt(r_norm);

        if (r_norm == 0.0) {
            return -strength_;
        }

        return -strength_ * std::exp(-range_ * r_norm) / r_norm;
    }

    ///////////////////////////////////////////////////////////
    /// @brief Devuelve el nombre simbólico del potencial.
    ///
    /// @return Cadena con el nombre `"YukawaPotential"`.
    ///////////////////////////////////////////////////////////
    std::string get_name() const override {
        return "YukawaPotential";
    }

private:
    double strength_;
    double range_;
};
