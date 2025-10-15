#pragma once

#include "BoundaryCondition.h"

/**
 * @class PeriodicBoundary
 * @brief Implementa condiciones de borde periódicas.
 *
 * Esta estrategia trata la malla como un toro, donde el valor al
 * inicio de un eje está vinculado al valor al final. Para una malla 1D,
 * esto significa que psi[0] está vinculado a psi[N-1].
 */
class PeriodicBoundary : public BoundaryCondition {
public:
    /**
     * @brief Aplica condiciones de borde periódicas al campo.
     * En el esquema FDTD, esto generalmente se maneja en el propio bucle de actualización,
     * donde el vecino del punto 0 es N-1, y el vecino de N-1 es 0.
     * Esta función de aplicación explícita se puede usar si el bucle principal no
     * lo maneja, por ejemplo, para forzar la condición en los puntos de borde
     * después de un paso. Para nuestra implementación FDTD actual, la lógica se
     * integrará directamente en la función `step` por eficiencia.
     *
     * @param psi Una referencia al vector de espinores (el campo).
     * @param grid La malla de simulación.
     */
    void apply(std::vector<Dirac::Spinor>& psi, const Grid& grid) const override {
        if (psi.size() < 2) return; // No aplicable para mallas con menos de 2 puntos.

        // En un caso simple, copiaríamos los valores, por ejemplo:
        // psi.front() = psi.back();
        // Pero para una malla escalonada, esta lógica es más compleja y se maneja mejor
        // dentro del bucle numérico. Esta función sirve como un marcador de posición
        // y confirmación de la implementación del patrón de estrategia.
    }

    /**
     * @brief Devuelve el nombre de la estrategia de condición de borde.
     * @return La cadena "PeriodicBoundary".
     */
    std::string get_name() const override {
        return "PeriodicBoundary";
    }
};