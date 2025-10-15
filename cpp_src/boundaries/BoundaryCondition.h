#pragma once

#include <vector>
#include <string>
#include <memory>
#include "../DiracMatrices.h" // Para Dirac::Spinor
#include "../Grid.h"

/**
 * @class BoundaryCondition
 * @brief Clase base abstracta para condiciones de borde (patrón Strategy).
 *
 * Esta clase define la interfaz para todas las estrategias de condiciones de borde.
 * Permite que el solver principal se desacople del método específico
 * utilizado para manejar los bordes de la malla de simulación.
 */
class BoundaryCondition {
public:
    virtual ~BoundaryCondition() = default;

    /**
     * @brief Aplica la condición de borde a un campo espinorial dado.
     * @param psi Una referencia al vector de espinores que representa el campo.
     * @param grid La malla de simulación.
     */
    virtual void apply(std::vector<Dirac::Spinor>& psi, const Grid& grid) const = 0;

    /**
     * @brief Devuelve el nombre de la estrategia de condición de borde.
     * @return Una cadena que contiene el nombre de la clase.
     */
    virtual std::string get_name() const = 0;
};