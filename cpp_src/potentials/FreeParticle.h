#pragma once

#include "Potential.h"

///////////////////////////////////////////////////////////////
/// @class FreeParticle
/// @brief Implementación concreta del potencial para una partícula libre.
///
/// Esta clase representa el caso en el que el potencial externo es nulo:
/// \f[
/// V(\mathbf{r}) = 0
/// \f]
///
/// Se utiliza como referencia base o caso de prueba para simulaciones
/// sin interacción, cumpliendo el requisito **FR-3** del modelo.
///
/// @see Potential, ScalarPotential, CoulombPotential
///////////////////////////////////////////////////////////////
class FreeParticle : public Potential {
public:
    ///////////////////////////////////////////////////////////
    /// @brief Evalúa el potencial en un punto espacial.
    ///
    /// @param r Vector de coordenadas espaciales (no se utiliza).
    /// @return Valor del potencial, siempre 0.0.
    ///
    /// @note
    /// La coordenada `r` se ignora explícitamente, ya que el potencial
    /// es uniforme y nulo en todo el dominio.
    ///////////////////////////////////////////////////////////
    double evaluate(const std::vector<double>& r) const override {
        (void)r; // Evitar advertencias por parámetro no utilizado.
        return 0.0;
    }

    ///////////////////////////////////////////////////////////
    /// @brief Devuelve el nombre simbólico del potencial.
    ///
    /// @return Cadena `"FreeParticle"`.
    ///
    /// @note
    /// Útil para introspección, registro de simulaciones o debugging.
    ///////////////////////////////////////////////////////////
    std::string get_name() const override {
        return "FreeParticle";
    }
};
