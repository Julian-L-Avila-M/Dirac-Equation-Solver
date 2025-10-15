#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "Grid.h"
#include "potentials/Potential.h"
#include "boundaries/BoundaryCondition.h"
#include "DiracMatrices.h"

namespace py = pybind11;

/**
 * @class TimeIntegrator
 * @brief Clase base abstracta para algoritmos de evolución temporal (patrón Strategy).
 *
 * Esta clase define la interfaz para todas las estrategias de integrador temporal.
 * Permite que el solver principal se desacople del método numérico específico
 * utilizado para avanzar la simulación en el tiempo.
 */
class TimeIntegrator {
public:
    virtual ~TimeIntegrator() = default;

    /**
     * @brief Realiza un único paso de tiempo para avanzar la simulación.
     * Este es el método principal de la estrategia.
     */
    virtual void step() = 0;

    /**
     * @brief Devuelve el nombre de la estrategia de integración.
     * @return Una cadena que contiene el nombre de la clase.
     */
    virtual std::string get_name() const = 0;
};

/**
 * @class FDTDLeapfrogIntegrator
 * @brief Una implementación concreta de TimeIntegrator que utiliza el método FDTD Leapfrog.
 *
 * Esta clase contendrá la lógica principal para el algoritmo FDTD de salto de rana escalonado.
 */
class FDTDLeapfrogIntegrator : public TimeIntegrator {
public:
    /**
     * @brief Construye el integrador FDTD Leapfrog.
     * @param initial_psi El estado inicial del campo espinorial como un array de NumPy de forma (N, 4).
     * @param grid El objeto de la malla de simulación.
     * @param potential El objeto de potencial.
     * @param dt El paso de tiempo.
     * @param mass La masa de la partícula.
     */
    FDTDLeapfrogIntegrator(
        py::array_t<std::complex<double>, py::array::c_style | py::array::forcecast> initial_psi,
        const Grid& grid,
        std::shared_ptr<Potential> potential,
        std::shared_ptr<BoundaryCondition> boundary_condition,
        double dt,
        double mass)
        : grid_(grid), potential_(potential), boundary_condition_(boundary_condition), dt_(dt), mass_(mass)
    {
        // 1. Validar las dimensiones del array de entrada
        py::buffer_info psi_info = initial_psi.request();
        if (psi_info.ndim != 2 || psi_info.shape[1] != 4) {
            throw std::runtime_error("El psi inicial debe ser un array de NumPy de forma (N, 4)");
        }
        if (static_cast<size_t>(psi_info.shape[0]) != grid_.get_total_points()) {
            throw std::runtime_error("El número de puntos en psi debe coincidir con el total de puntos de la malla.");
        }

        // 2. Asignar memoria para los campos espinoriales
        size_t n_points = grid_.get_total_points();
        psi_curr_.resize(n_points);
        psi_prev_.resize(n_points);
        psi_next_.resize(n_points);

        // 3. Copiar datos iniciales del array de NumPy a psi_curr_
        auto ptr = static_cast<Dirac::complex*>(psi_info.ptr);
        for (size_t i = 0; i < n_points; ++i) {
            for (size_t j = 0; j < 4; ++j) {
                psi_curr_[i].components[j] = ptr[i * 4 + j];
            }
        }

        // 4. Realizar el primer paso utilizando un esquema más simple y no centrado (Euler hacia adelante)
        // para generar el estado en el tiempo t=dt, que es necesario para que el salto de rana comience.
        // Esta es una forma común de inicializar un integrador de salto de rana.
        // Por ahora, solo copiamos el estado actual al estado anterior.
        // Se necesitaría un inicializador más preciso para una simulación real.
        psi_prev_ = psi_curr_;
    }

    void step() override {
        size_t n_points = grid_.get_total_points();
        const auto& shape = grid_.get_shape();
        const auto& spacing = grid_.get_spacing();
        bool is_periodic = (boundary_condition_->get_name() == "PeriodicBoundary");

        for (size_t i = 0; i < n_points; ++i) {
            Dirac::Spinor h_spatial_psi;
            if (grid_.get_dim() == 1) {
                size_t i_prev = (is_periodic && i == 0) ? n_points - 1 : i - 1;
                size_t i_next = (is_periodic && i == n_points - 1) ? 0 : i + 1;
                h_spatial_psi = Dirac::multiply(Dirac::alpha_z, psi_curr_[i_next] - psi_curr_[i_prev]);
                h_spatial_psi = (Dirac::complex(0, -1.0) / (2.0 * spacing[0])) * h_spatial_psi;
            } else if (grid_.get_dim() == 2) {
                size_t nx = shape[0];
                size_t ny = shape[1];
                size_t ix = i % nx;
                size_t iy = i / nx;
                size_t ix_prev = (is_periodic && ix == 0) ? nx - 1 : ix - 1;
                size_t ix_next = (is_periodic && ix == nx - 1) ? 0 : ix + 1;
                size_t iy_prev = (is_periodic && iy == 0) ? ny - 1 : iy - 1;
                size_t iy_next = (is_periodic && iy == ny - 1) ? 0 : iy + 1;
                Dirac::Spinor dx_psi = Dirac::multiply(Dirac::alpha_x, psi_curr_[iy * nx + ix_next] - psi_curr_[iy * nx + ix_prev]);
                Dirac::Spinor dy_psi = Dirac::multiply(Dirac::alpha_y, psi_curr_[iy_next * nx + ix] - psi_curr_[iy_prev * nx + ix]);
                h_spatial_psi = (Dirac::complex(0, -1.0) / (2.0 * spacing[0])) * dx_psi +
                                (Dirac::complex(0, -1.0) / (2.0 * spacing[1])) * dy_psi;
            } else if (grid_.get_dim() == 3) {
                size_t nx = shape[0];
                size_t ny = shape[1];
                size_t nz = shape[2];
                size_t ix = i % nx;
                size_t iy = (i / nx) % ny;
                size_t iz = i / (nx * ny);
                size_t ix_prev = (is_periodic && ix == 0) ? nx - 1 : ix - 1;
                size_t ix_next = (is_periodic && ix == nx - 1) ? 0 : ix + 1;
                size_t iy_prev = (is_periodic && iy == 0) ? ny - 1 : iy - 1;
                size_t iy_next = (is_periodic && iy == ny - 1) ? 0 : iy + 1;
                size_t iz_prev = (is_periodic && iz == 0) ? nz - 1 : iz - 1;
                size_t iz_next = (is_periodic && iz == nz - 1) ? 0 : iz + 1;
                Dirac::Spinor dx_psi = Dirac::multiply(Dirac::alpha_x, psi_curr_[iz * nx * ny + iy * nx + ix_next] - psi_curr_[iz * nx * ny + iy * nx + ix_prev]);
                Dirac::Spinor dy_psi = Dirac::multiply(Dirac::alpha_y, psi_curr_[iz * nx * ny + iy_next * nx + ix] - psi_curr_[iz * nx * ny + iy_prev * nx + ix]);
                Dirac::Spinor dz_psi = Dirac::multiply(Dirac::alpha_z, psi_curr_[(iz_next * nx * ny) + iy * nx + ix] - psi_curr_[(iz_prev * nx * ny) + iy * nx + ix]);
                h_spatial_psi = (Dirac::complex(0, -1.0) / (2.0 * spacing[0])) * dx_psi +
                                (Dirac::complex(0, -1.0) / (2.0 * spacing[1])) * dy_psi +
                                (Dirac::complex(0, -1.0) / (2.0 * spacing[2])) * dz_psi;
            }
            std::vector<double> coords = grid_.get_coords(i);
            double v_at_i = potential_->evaluate(coords);
            Dirac::Spinor h_local_psi = mass_ * Dirac::multiply(Dirac::beta, psi_curr_[i]) + v_at_i * psi_curr_[i];

            Dirac::Spinor h_psi = h_spatial_psi + h_local_psi;
            psi_next_[i] = psi_prev_[i] - (Dirac::complex(0, 2.0) * dt_) * h_psi;
        }

        boundary_condition_->apply(psi_next_, grid_);

        psi_prev_ = psi_curr_;
        psi_curr_ = psi_next_;
    }

    std::string get_name() const override {
        return "FDTDLeapfrogIntegrator";
    }

    // Getter para devolver la función de onda actual a Python
    py::array_t<std::complex<double>> get_psi() const {
        py::array_t<std::complex<double>> result({grid_.get_total_points(), 4ul});
        auto buf = result.request();
        auto* ptr = static_cast<std::complex<double>*>(buf.ptr);
        for (size_t i = 0; i < grid_.get_total_points(); ++i) {
            for (size_t j = 0; j < 4; ++j) {
                ptr[i * 4 + j] = psi_curr_[i].components[j];
            }
        }
        return result;
    }

private:
    Grid grid_;
    std::shared_ptr<Potential> potential_;
    std::shared_ptr<BoundaryCondition> boundary_condition_;
    double dt_;
    double mass_;

    // Campos de tres pasos de tiempo para el algoritmo de salto de rana
    std::vector<Dirac::Spinor> psi_prev_;
    std::vector<Dirac::Spinor> psi_curr_;
    std::vector<Dirac::Spinor> psi_next_;
};