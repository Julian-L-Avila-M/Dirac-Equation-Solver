#pragma once

#include "Potential.h"
#include <cmath>
#include <numeric>

///////////////////////////////////////////////////////////////
/// @class CoulombPotential
/// @brief Implementación concreta de un potencial de Coulomb regularizado.
///
/// Esta clase deriva de la clase abstracta @ref Potential y define
/// un potencial electrostático tipo Coulomb con una regularización
/// en el origen (r = 0) para evitar singularidades numéricas.
///
/// La expresión implementada es:
/// 
/// \\[ V(r) = -\\frac{Z}{\\|r\\| + \\epsilon} \\] 
/// 
///
/// donde:
/// - \\( Z \\) es la carga nuclear o constante de acoplamiento.
/// - \\( \\epsilon \\) es un parámetro de regularización pequeño para estabilizar el cálculo.
///
/// @note Este potencial puede emplearse tanto en simulaciones 1D, 2D o 3D.
/// @warning Si se usa un valor de `epsilon_` demasiado pequeño, el integrador
/// podría experimentar inestabilidad numérica en r≈0.
///
/// @see Potential
///////////////////////////////////////////////////////////////
class CoulombPotential : public Potential {
public:
    ///////////////////////////////////////////////////////////
    /// @brief Constructor principal del potencial de Coulomb.
    ///
    /// @param Z Carga efectiva del centro (p. ej. número atómico).
    /// @param epsilon Parámetro de regularización para evitar división por cero.
    ///                Por defecto se usa 1e-6.
    ///
    /// @note En la práctica, `epsilon` controla la suavidad del potencial
    /// cerca del origen. Valores mayores suavizan más la singularidad.
    ///////////////////////////////////////////////////////////
    CoulombPotential(double Z, double epsilon = 1e-6)
        : Z_(Z), epsilon_(epsilon) {}

    ///////////////////////////////////////////////////////////
    /// @brief Evalúa el potencial en un punto espacial.
    ///
    /// @param r Vector de posición (1D, 2D o 3D).
    /// @return Valor escalar del potencial \\( V(r) \\).
    ///
    /// @details
    /// Calcula la norma euclidiana \\( \\|r\\| \\) del vector `r`
    /// y devuelve el valor regularizado del potencial de Coulomb:
    /// \\( -Z / (\\|r\\| + \\epsilon) \\).
    ///
    /// @warning No se asume normalización previa del vector `r`.
    ///////////////////////////////////////////////////////////
    double evaluate(const std::vector<double>& r) const override {
        double r_norm = 0.0;
        for (double val : r) {
            r_norm += val * val;
        }
        r_norm = std::sqrt(r_norm);

        // Regularizar el potencial en r=0
        return -Z_ / (r_norm + epsilon_);
    }

    ///////////////////////////////////////////////////////////
    /// @brief Devuelve el nombre simbólico del potencial.
    ///
    /// @return Cadena con el nombre `"CoulombPotential"`.
    ///
    /// @note Útil para registro o introspección de tipos de potenciales.
    ///////////////////////////////////////////////////////////
    std::string get_name() const override {
        return "CoulombPotential";
    }

private:
    ///////////////////////////////////////////////////////////
    /// @brief Carga efectiva del potencial (Z).
    ///
    /// Representa la intensidad del campo coulombiano.
    ///////////////////////////////////////////////////////////
    double Z_;

    ///////////////////////////////////////////////////////////
    /// @brief Parámetro de regularización para r → 0.
    ///
    /// Previene singularidades en la evaluación numérica del potencial.
    ///////////////////////////////////////////////////////////
    double epsilon_;
};
