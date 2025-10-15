#pragma once

#include <vector>
#include <numeric>
#include <stdexcept>
#include <cmath>

/**
 * @class Grid
 * @brief Representa una malla espacial (1D, 2D o 3D) para la simulación.
 *
 * Esta clase almacena los parámetros de la malla de simulación, como sus
 * dimensiones, número de puntos y espaciado de celda. Es la contraparte en C++
 * de la clase Grid de Python.
 */
class Grid {
public:
    /**
     * @brief Construye un objeto Grid.
     * @param shape Un vector que contiene el número de puntos en cada dimensión (por ejemplo, {nx, ny}).
     * @param spacing Un vector que contiene el espaciado de celda en cada dimensión (por ejemplo, {dx, dy}).
     */
    Grid(std::vector<size_t> shape, std::vector<double> spacing)
        : shape_(shape), spacing_(spacing), dim_(shape.size()) {
        if (shape.size() != spacing.size()) {
            throw std::invalid_argument("Shape y spacing deben tener el mismo número de dimensiones.");
        }
        if (dim_ < 1 || dim_ > 3) {
            throw std::invalid_argument("La malla debe ser 1D, 2D o 3D.");
        }
        // Calcula el origen para que coincida con la implementación de Python (malla centrada)
        origin_.resize(dim_);
        for (size_t i = 0; i < dim_; ++i) {
            origin_[i] = -0.5 * (static_cast<double>(shape_[i] - 1)) * spacing_[i];
        }
    }

    // Getters para las propiedades de la malla
    size_t get_dim() const { return dim_; }
    const std::vector<size_t>& get_shape() const { return shape_; }
    const std::vector<double>& get_spacing() const { return spacing_; }

    /**
     * @brief Calcula el número total de puntos en la malla.
     * @return El producto del número de puntos en cada dimensión.
     */
    size_t get_total_points() const {
        size_t total = 1;
        for (size_t n : shape_) {
            total *= n;
        }
        return total;
    }

    /**
     * @brief Obtiene las coordenadas espaciales para un índice de malla plano dado.
     * @param index El índice plano del punto de la malla.
     * @return Un vector de dobles que representa las coordenadas (por ejemplo, {x, y, z}).
     */
    std::vector<double> get_coords(size_t index) const {
        if (index >= get_total_points()) {
            throw std::out_of_range("El índice de la malla está fuera de rango.");
        }

        std::vector<double> coords(dim_);
        size_t temp_index = index;

        if (dim_ == 1) {
            coords[0] = origin_[0] + static_cast<double>(index) * spacing_[0];
        } else if (dim_ == 2) {
            size_t nx = shape_[0];
            size_t ix = temp_index % nx;
            size_t iy = temp_index / nx;
            coords[0] = origin_[0] + static_cast<double>(ix) * spacing_[0];
            coords[1] = origin_[1] + static_cast<double>(iy) * spacing_[1];
        } else if (dim_ == 3) {
            size_t nx = shape_[0];
            size_t ny = shape_[1];
            size_t ix = temp_index % nx;
            size_t iy = (temp_index / nx) % ny;
            size_t iz = temp_index / (nx * ny);
            coords[0] = origin_[0] + static_cast<double>(ix) * spacing_[0];
            coords[1] = origin_[1] + static_cast<double>(iy) * spacing_[1];
            coords[2] = origin_[2] + static_cast<double>(iz) * spacing_[2];
        }
        return coords;
    }

private:
    std::vector<size_t> shape_;  // Número de puntos en cada dimensión
    std::vector<double> spacing_;// Espaciado de celda en cada dimensión
    std::vector<double> origin_; // La coordenada mínima en cada dimensión
    size_t dim_;                 // Número de dimensiones
};