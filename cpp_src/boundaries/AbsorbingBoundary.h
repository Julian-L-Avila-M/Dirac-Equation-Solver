#pragma once

#include "BoundaryCondition.h"
#include <vector>
#include "../DiracMatrices.h"

/**
 * @class AbsorbingBoundary
 * @brief Implementación de una condición de frontera absorbente (ABC) para simulaciones 1D.
 *
 * Esta clase aplica un amortiguamiento en los extremos del dominio espacial
 * para minimizar reflexiones artificiales en la frontera. Se utiliza típicamente
 * en simulaciones de la ecuación de Dirac o de Schrödinger cuando se desea
 * emular un dominio abierto.
 *
 * Actualmente, la clase sólo soporta dominios unidimensionales (1D).
 *
 * ### Ejemplo de uso
 * @code
 * AbsorbingBoundary abc(0.05);  // Condición de frontera con fuerza 0.05
 * abc.apply(psi, grid);         // Aplica el amortiguamiento a los extremos
 * @endcode
 */
class AbsorbingBoundary : public BoundaryCondition {
public:
    /**
     * @brief Constructor de la condición de frontera absorbente.
     * @param strength Factor de absorción (0 < strength < 1). 
     * Valores mayores implican una absorción más fuerte en los extremos.
     */
    AbsorbingBoundary(double strength) : strength_(strength) {}

    /**
     * @brief Aplica la condición de frontera absorbente a la función de onda.
     *
     * Multiplica los valores del spinor en los extremos de la malla por un
     * factor complejo reductor de magnitud `(1 - strength)`, lo que atenúa
     * progresivamente la amplitud del campo.
     *
     * @param psi Vector de spinors de Dirac que representan el campo en la malla.
     * @param grid Objeto que define la malla espacial asociada a la simulación.
     * @throws std::runtime_error Si la malla no es unidimensional (dim ≠ 1).
     */
    void apply(std::vector<Dirac::Spinor>& psi, const Grid& grid) const override {
        if (grid.get_dim() != 1) {
            throw std::runtime_error("AbsorbingBoundary solo está implementado para mallas 1D por el momento.");
        }

        size_t n_points = grid.get_total_points();
        if (n_points < 2) return;

        // Aplicar amortiguamiento a los puntos primero y último
        psi[0] = Dirac::complex(1.0 - strength_, 0.0) * psi[0];
        psi[n_points - 1] = Dirac::complex(1.0 - strength_, 0.0) * psi[n_points - 1];
    }

    /**
     * @brief Devuelve el nombre de la condición de frontera.
     * @return La cadena `"AbsorbingBoundary"`.
     */
    std::string get_name() const override {
        return "AbsorbingBoundary";
    }

private:
    /**
     * @brief Coeficiente de absorción que determina la intensidad del amortiguamiento.
     */
    double strength_;
};
