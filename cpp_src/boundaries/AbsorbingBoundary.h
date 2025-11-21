#pragma once

#include "BoundaryCondition.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include "../DiracMatrices.h"
#include "../Grid.h"

/**
 * @class AbsorbingBoundary
 * @brief Implementación de fronteras absorbentes mediante Potencial Complejo Absorbente (CAP/PML simplificado).
 *
 * En lugar de una "pared dura" que multiplica por un factor constante, esta clase aplica 
 * un perfil de absorción cuadrático que aumenta gradualmente desde el interior hacia el borde.
 * Esto emula una Capa Perfectamente Adaptada (PML) en el régimen de baja energía,
 * minimizando las reflexiones numéricas artificiales.
 *
 * El factor de amortiguamiento sigue la forma:
 *   \\[ \Gamma(d) = 1 - \sigma_{max} \left( \frac{W - d}{W} \right)^2 \\]
 * donde \\(d \\) es la distancia al borde y \\(W\\) es el ancho de la capa.
 */
class AbsorbingBoundary : public BoundaryCondition {
public:
    /**
     * @brief Constructor.
     * @param strength Coeficiente máximo de absorción en el borde exacto (0 < strength < 1).
     * Un valor típico es 0.3 - 0.5 para evitar inestabilidades numéricas.
     * @param width Ancho de la capa absorbente en número de puntos de malla (default: 20).
     */
    AbsorbingBoundary(double strength, int width = 20) 
        : strength_(strength), width_(width) {}

    /**
     * @brief Aplica el perfil de absorción espacialmente dependiente.
     */
    void apply(std::vector<Dirac::Spinor>& psi, const Grid& grid) const override {
        const auto& shape = grid.get_shape();
        size_t dim = grid.get_dim();
        size_t total_points = grid.get_total_points();

        // ------------------------------------------------------------
        // CASO 1D
        // ------------------------------------------------------------
        if (dim == 1) {
            size_t nx = shape[0];
            // Optimización: Solo iterar sobre las capas, no todo el centro
            // Sin embargo, iterar todo es más seguro para evitar errores de índice y 
            // el costo en 1D es despreciable.
            for (size_t i = 0; i < nx; ++i) {
                int dist = get_min_distance_1d(i, nx);
                if (dist < width_) {
                    apply_damping(psi, i, dist);
                }
            }
        }
        // ------------------------------------------------------------
        // CASO 2D
        // ------------------------------------------------------------
        else if (dim == 2) {
            size_t nx = shape[0];
            size_t ny = shape[1];
            
            // Barrido completo (simple y seguro). 
            // Para mallas muy grandes (>1024^2), se podría optimizar iterando solo los bordes.
            for (size_t i = 0; i < nx; ++i) {
                for (size_t j = 0; j < ny; ++j) {
                    int dist_x = get_min_distance_1d(i, nx);
                    int dist_y = get_min_distance_1d(j, ny);
                    int min_dist = std::min(dist_x, dist_y);
                    
                    if (min_dist < width_) {
                        size_t idx = i * ny + j;
                        apply_damping(psi, idx, min_dist);
                    }
                }
            }
        }
        // ------------------------------------------------------------
        // CASO 3D
        // ------------------------------------------------------------
        else if (dim == 3) {
            size_t nx = shape[0];
            size_t ny = shape[1];
            size_t nz = shape[2];

            for (size_t i = 0; i < nx; ++i) {
                for (size_t j = 0; j < ny; ++j) {
                    for (size_t k = 0; k < nz; ++k) {
                        int dist_x = get_min_distance_1d(i, nx);
                        int dist_y = get_min_distance_1d(j, ny);
                        int dist_z = get_min_distance_1d(k, nz);
                        
                        // La distancia al borde más cercano determina la absorción
                        int min_dist = std::min({dist_x, dist_y, dist_z});

                        if (min_dist < width_) {
                            size_t idx = (i * ny + j) * nz + k;
                            apply_damping(psi, idx, min_dist);
                        }
                    }
                }
            }
        }
    }

    std::string get_name() const override {
        return "AbsorbingBoundary";
    }

private:
    double strength_; ///< Fuerza máxima de absorción (sigma max).
    int width_;       ///< Ancho de la capa en celdas.

    /**
     * @brief Calcula la distancia mínima a los extremos 0 o N-1 en una dimensión.
     */
    inline int get_min_distance_1d(size_t index, size_t size) const {
        // Distancia al inicio: index
        // Distancia al final: size - 1 - index
        return static_cast<int>(std::min(index, size - 1 - index));
    }

    /**
     * @brief Aplica el factor de amortiguamiento a un espinor específico.
     * * Usa un perfil cuadrático: (1 - damping)
     * \\[ damping = strength * ((width - dist) / width)^2 \\]
     */
    inline void apply_damping(std::vector<Dirac::Spinor>& psi, size_t idx, int dist) const {
        double normalized_pos = static_cast<double>(width_ - dist) / static_cast<double>(width_);
        // Perfil cuadrático para suavizar la transición
        double damping = strength_ * (normalized_pos * normalized_pos);
        
        // Factor multiplicativo final (ej. 0.99...)
        Dirac::complex factor(1.0 - damping, 0.0);
        
        psi[idx] = factor * psi[idx];
    }
};
