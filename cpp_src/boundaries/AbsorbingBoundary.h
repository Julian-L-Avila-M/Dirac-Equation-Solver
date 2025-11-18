#pragma once

#include "BoundaryCondition.h"
#include <vector>
#include "../DiracMatrices.h"

/**
 * @class AbsorbingBoundary
 * @brief Implementación de una condición de frontera absorbente (ABC) para simulaciones 1D, 2D y 3D.
 *
 * Esta clase aplica un amortiguamiento en los extremos del dominio espacial
 * para minimizar reflexiones artificiales en la frontera. Se utiliza típicamente
 * en simulaciones de la ecuación de Dirac o de Schrödinger cuando se desea
 * emular un dominio abierto.
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
     */
    void apply(std::vector<Dirac::Spinor>& psi, const Grid& grid) const override {
        const auto& shape = grid.get_shape();
        const Dirac::complex absorption_factor = Dirac::complex(1.0 - strength_, 0.0);

        if (grid.get_dim() == 1) {
            size_t nx = shape[0];
            if (nx < 2) return;
            psi[0] = absorption_factor * psi[0];
            psi[nx - 1] = absorption_factor * psi[nx - 1];
        } else if (grid.get_dim() == 2) {
            size_t nx = shape[0];
            size_t ny = shape[1];
            if (nx < 2 || ny < 2) return;

            for (size_t i = 0; i < nx; ++i) {
                psi[i * ny] = absorption_factor * psi[i * ny]; // j = 0
                psi[i * ny + ny - 1] = absorption_factor * psi[i * ny + ny - 1]; // j = ny-1
            }
            for (size_t j = 1; j < ny - 1; ++j) {
                psi[j] = absorption_factor * psi[j]; // i = 0
                psi[(nx - 1) * ny + j] = absorption_factor * psi[(nx - 1) * ny + j]; // i = nx-1
            }
        } else if (grid.get_dim() == 3) {
            size_t nx = shape[0];
            size_t ny = shape[1];
            size_t nz = shape[2];
            if (nx < 2 || ny < 2 || nz < 2) return;

            for (size_t i = 0; i < nx; ++i) {
                for (size_t j = 0; j < ny; ++j) {
                    psi[(i * ny + j) * nz] = absorption_factor * psi[(i * ny + j) * nz]; // k=0
                    psi[(i * ny + j) * nz + nz - 1] = absorption_factor * psi[(i * ny + j) * nz + nz - 1]; // k=nz-1
                }
            }

            for (size_t i = 0; i < nx; ++i) {
                for (size_t k = 1; k < nz - 1; ++k) {
                    psi[(i * ny) * nz + k] = absorption_factor * psi[(i * ny) * nz + k]; // j=0
                    psi[(i * ny + ny - 1) * nz + k] = absorption_factor * psi[(i * ny + ny - 1) * nz + k]; // j=ny-1
                }
            }

            for (size_t j = 1; j < ny - 1; ++j) {
                for (size_t k = 1; k < nz - 1; ++k) {
                    psi[j * nz + k] = absorption_factor * psi[j * nz + k]; // i=0
                    psi[((nx - 1) * ny + j) * nz + k] = absorption_factor * psi[((nx - 1) * ny + j) * nz + k]; // i=nx-1
                }
            }
        }
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
