#pragma once

#include "Potential.h"

/**
 * @class FreeParticle
 * @brief Una implementación concreta de Potential para una partícula libre (V=0).
 *
 * Esta clase representa el caso en el que no hay potencial externo.
 * Cumple con el requisito FR-3 para una partícula libre.
 */
class FreeParticle : public Potential {
public:
    /**
     * @brief Evalúa el potencial, que siempre es cero para una partícula libre.
     * @param r La coordenada espacial (ignorada).
     * @return Siempre devuelve 0.0.
     */
    double evaluate(const std::vector<double>& r) const override {
        // Para una partícula libre, el potencial es siempre cero.
        (void)r; // Marcar 'r' como no utilizado para evitar advertencias del compilador.
        return 0.0;
    }

    /**
     * @brief Devuelve el nombre del potencial.
     * @return La cadena "FreeParticle".
     */
    std::string get_name() const override {
        return "FreeParticle";
    }
};