#pragma once

#include "Potential.h"
#include <vector>
#include <limits>

///////////////////////////////////////////////////////////////
/// @class InfiniteWellPotential
/// @brief Implementación de un potencial de pozo infinito.
///
/// Esta clase deriva de la clase abstracta @ref Potential y define
/// un potencial de pozo infinito, que es cero dentro de una caja
/// y infinito fuera de ella.
///
/// La expresión implementada es:
/// \\[
/// V(r) = 0 \\text{ if } |r_i| < L_i/2 \\text{ for all } i
/// \\]
/// \\[
/// V(r) = \\infty \\text{ otherwise}
/// \\]
///
/// donde:
/// - \\( L_i \\) es el ancho del pozo en la dimensión i.
///
/// @note Este potencial puede emplearse tanto en simulaciones 1D, 2D o 3D.
///////////////////////////////////////////////////////////////
class InfiniteWellPotential : public Potential {
public:
    ///////////////////////////////////////////////////////////
    /// @brief Constructor principal del potencial de pozo infinito.
    ///
    /// @param widths Un vector con los anchos del pozo en cada dimensión.
    ///////////////////////////////////////////////////////////
    InfiniteWellPotential(const std::vector<double>& widths)
        : widths_(widths) {}

    ///////////////////////////////////////////////////////////
    /// @brief Evalúa el potencial en un punto espacial.
    ///
    /// @param r Vector de posición (1D, 2D o 3D).
    /// @return Valor escalar del potencial \\( V(r) \\).
    ///////////////////////////////////////////////////////////
    double evaluate(const std::vector<double>& r) const override {
        for (size_t i = 0; i < r.size(); ++i) {
            if (std::abs(r[i]) >= widths_[i] / 2.0) {
                return std::numeric_limits<double>::infinity();
            }
        }
        return 0.0;
    }

    ///////////////////////////////////////////////////////////
    /// @brief Devuelve el nombre simbólico del potencial.
    ///
    /// @return Cadena con el nombre `"InfiniteWellPotential"`.
    ///////////////////////////////////////////////////////////
    std::string get_name() const override {
        return "InfiniteWellPotential";
    }

private:
    std::vector<double> widths_;
};
