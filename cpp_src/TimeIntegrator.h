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
 * Define la interfaz genérica para los integradores temporales utilizados
 * en el solver de la ecuación de Dirac. Cada subclase concreta implementa
 * su propio esquema numérico de evolución temporal.
 *
 * Este diseño permite desacoplar el método numérico del resto de la
 * arquitectura del simulador.
 */
class TimeIntegrator {
public:
    virtual ~TimeIntegrator() = default;

    /**
     * @brief Realiza un único paso de evolución temporal.
     *
     * Este método debe ser implementado por cada estrategia de integración,
     * actualizando los estados internos del campo espinorial según el esquema elegido.
     */
    virtual void step() = 0;

    /**
     * @brief Devuelve el nombre del integrador.
     * @return Una cadena con el nombre de la clase.
     */
    virtual std::string get_name() const = 0;
};

/**
 * @class FDTDLeapfrogIntegrator
 * @brief Implementación concreta de TimeIntegrator mediante el esquema FDTD de salto de rana (Leapfrog).
 *
 * Este integrador emplea el método *Finite-Difference Time-Domain (FDTD)* con
 * un esquema temporal tipo "leapfrog", ampliamente usado para ecuaciones de onda relativistas
 * como la ecuación de Dirac.
 *
 * El algoritmo alterna entre los estados ψ(t - Δt), ψ(t) y ψ(t + Δt) para avanzar
 * la solución sin necesidad de almacenar todas las configuraciones temporales.
 *
 * ### Características principales
 * - Soporta dominios 1D, 2D y 3D.
 * - Compatible con potenciales arbitrarios `Potential`.
 * - Integra condiciones de frontera mediante la clase `BoundaryCondition`.
 * - Expone la función de onda al entorno Python vía pybind11.
 *
 * ### Referencias
 * - Taflove, A., & Hagness, S. C. (2005). *Computational Electrodynamics: The Finite-Difference Time-Domain Method.*
 * - Thaller, B. (1992). *The Dirac Equation.*
 */
class FDTDLeapfrogIntegrator : public TimeIntegrator {
public:
    /**
     * @brief Constructor principal del integrador Leapfrog.
     *
     * Inicializa la función de onda y los buffers de tiempo (ψ_prev, ψ_curr, ψ_next),
     * validando dimensiones y preparando los parámetros del algoritmo.
     *
     * @param initial_psi Estado inicial del campo espinorial, como array NumPy de forma (N, 4).
     * @param grid Objeto de tipo Grid que define la geometría de la simulación.
     * @param potential Potencial escalar aplicado a la partícula.
     * @param boundary_condition Condición de frontera a aplicar (e.g. periódica, absorbente).
     * @param dt Paso temporal.
     * @param mass Masa de la partícula.
     *
     * @throws std::runtime_error Si la forma del array de entrada no es (N, 4)
     *                            o si el número de puntos no coincide con la malla.
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
        // Validación de dimensiones de entrada
        py::buffer_info psi_info = initial_psi.request();
        if (psi_info.ndim != 2 || psi_info.shape[1] != 4) {
            throw std::runtime_error("El psi inicial debe ser un array de NumPy de forma (N, 4)");
        }
        if (static_cast<size_t>(psi_info.shape[0]) != grid_.get_total_points()) {
            throw std::runtime_error("El número de puntos en psi debe coincidir con el total de puntos de la malla.");
        }

        // Asignación de memoria
        size_t n_points = grid_.get_total_points();
        psi_curr_.resize(n_points);
        psi_prev_.resize(n_points);
        psi_next_.resize(n_points);

        // Copiar datos desde el array de NumPy a psi_curr_
        auto ptr = static_cast<Dirac::complex*>(psi_info.ptr);
        for (size_t i = 0; i < n_points; ++i)
            for (size_t j = 0; j < 4; ++j)
                psi_curr_[i].components[j] = ptr[i * 4 + j];

        // Inicialización básica del buffer previo
        // Para el primer paso, necesitamos estimar psi(t - dt).
        // Usamos una aproximación de Euler hacia atrás:
        // psi(t - dt) approx psi(t) - (-i * H * psi(t)) * (-dt)
        //             = psi(t) - i * dt * H * psi(t)
        // Pero Leapfrog dice: psi(t+dt) = psi(t-dt) - 2i dt H psi(t)
        // Si queremos que el primer paso sea equivalente a un paso Euler:
        // psi(t+dt) = psi(t) - i dt H psi(t)
        // Entonces: psi(t) - i dt H psi(t) = psi(t-dt) - 2i dt H psi(t)
        // Implica: psi(t-dt) = psi(t) + i dt H psi(t)

        bool is_periodic = (boundary_condition_->get_name() == "PeriodicBoundary");

        for(size_t i=0; i<n_points; ++i) {
            // Calcular H * psi en t=0 para inicializar psi(-dt)
            Dirac::Spinor h_psi = compute_hamiltonian_at_point(i, psi_curr_, is_periodic);

            // psi_prev = psi_curr + (i * dt) * H_psi
            psi_prev_[i] = psi_curr_[i] + (Dirac::complex(0, 1.0) * dt_) * h_psi;
        }

        // Aplicar condiciones de borde a psi_prev también para consistencia
        boundary_condition_->apply(psi_prev_, grid_);
    }

    /**
     * @brief Avanza un paso de tiempo completo según el esquema FDTD Leapfrog.
     *
     * Calcula el operador espacial y el término local del Hamiltoniano para
     * cada punto de la malla, aplicando las matrices de Dirac correspondientes
     * a cada dimensión. Luego, actualiza ψ(t+Δt) a partir de ψ(t) y ψ(t-Δt).
     *
     * También aplica la condición de frontera definida por `boundary_condition_`.
     *
     * @note Actualmente implementa derivadas centrales de segundo orden
     *       y admite condiciones periódicas o absorbentes.
     */
    void step() override {
        size_t n_points = grid_.get_total_points();
        bool is_periodic = (boundary_condition_->get_name() == "PeriodicBoundary");

        for (size_t i = 0; i < n_points; ++i) {
            // Calcular H * psi_curr localmente (sin vector intermedio)
            Dirac::Spinor h_psi = compute_hamiltonian_at_point(i, psi_curr_, is_periodic);

            // --- Actualización temporal ---
            // psi(t+dt) = psi(t-dt) - 2i * dt * H * psi(t)
            psi_next_[i] = psi_prev_[i] - (Dirac::complex(0, 2.0) * dt_) * h_psi;
        }

        // Aplicar condición de frontera
        boundary_condition_->apply(psi_next_, grid_);

        // Rotar buffers de tiempo
        psi_prev_ = psi_curr_;
        psi_curr_ = psi_next_;
    }

    // Método auxiliar en línea para calcular H * psi en un punto específico
    // Devuelve (H * psi)_i
    inline Dirac::Spinor compute_hamiltonian_at_point(size_t i, const std::vector<Dirac::Spinor>& psi, bool is_periodic) {
        const auto& shape = grid_.get_shape();
        const auto& spacing = grid_.get_spacing();
        size_t n_points = grid_.get_total_points();

        Dirac::Spinor h_spatial_psi;

        // --- Operador diferencial espacial según la dimensionalidad ---
        if (grid_.get_dim() == 1) {
            size_t nx = shape[0];
            Dirac::Spinor dx_psi;

            if (is_periodic) {
                size_t i_prev = (i == 0) ? n_points - 1 : i - 1;
                size_t i_next = (i == n_points - 1) ? 0 : i + 1;
                dx_psi = (psi[i_next] - psi[i_prev]) / (2.0 * spacing[0]);
            } else {
                if (i == 0) {
                    dx_psi = (psi[i + 1] - psi[i]) / spacing[0];
                } else if (i == n_points - 1) {
                    dx_psi = (psi[i] - psi[i - 1]) / spacing[0];
                } else {
                    dx_psi = (psi[i + 1] - psi[i - 1]) / (2.0 * spacing[0]);
                }
            }
            h_spatial_psi = Dirac::complex(0, -1.0) * Dirac::multiply(Dirac::alpha_x, dx_psi);
        }
        else if (grid_.get_dim() == 2) {
            size_t nx = shape[0];
            size_t ny = shape[1];
            size_t ix = i % nx;
            size_t iy = i / nx;

            Dirac::Spinor dx_psi_term, dy_psi_term;

            // X derivative
            if (is_periodic) {
                size_t ix_prev = (ix == 0) ? nx - 1 : ix - 1;
                size_t ix_next = (ix == nx - 1) ? 0 : ix + 1;
                dx_psi_term = (psi[iy * nx + ix_next] - psi[iy * nx + ix_prev]) / (2.0 * spacing[0]);
            } else {
                if (ix == 0) {
                    dx_psi_term = (psi[iy * nx + (ix + 1)] - psi[iy * nx + ix]) / spacing[0];
                } else if (ix == nx - 1) {
                    dx_psi_term = (psi[iy * nx + ix] - psi[iy * nx + (ix - 1)]) / spacing[0];
                } else {
                    dx_psi_term = (psi[iy * nx + (ix + 1)] - psi[iy * nx + (ix - 1)]) / (2.0 * spacing[0]);
                }
            }

            // Y derivative
            if (is_periodic) {
                size_t iy_prev = (iy == 0) ? ny - 1 : iy - 1;
                size_t iy_next = (iy == ny - 1) ? 0 : iy + 1;
                dy_psi_term = (psi[iy_next * nx + ix] - psi[iy_prev * nx + ix]) / (2.0 * spacing[1]);
            } else {
                if (iy == 0) {
                    dy_psi_term = (psi[(iy + 1) * nx + ix] - psi[iy * nx + ix]) / spacing[1];
                } else if (iy == ny - 1) {
                    dy_psi_term = (psi[iy * nx + ix] - psi[(iy - 1) * nx + ix]) / spacing[1];
                } else {
                    dy_psi_term = (psi[(iy + 1) * nx + ix] - psi[(iy - 1) * nx + ix]) / (2.0 * spacing[1]);
                }
            }

            h_spatial_psi = Dirac::complex(0, -1.0) * (Dirac::multiply(Dirac::alpha_x, dx_psi_term) + Dirac::multiply(Dirac::alpha_y, dy_psi_term));
        }
        else if (grid_.get_dim() == 3) {
            size_t nx = shape[0];
            size_t ny = shape[1];
            size_t nz = shape[2];
            size_t ix = i % nx;
            size_t iy = (i / nx) % ny;
            size_t iz = i / (nx * ny);

            Dirac::Spinor dx_psi_term, dy_psi_term, dz_psi_term;

            // X derivative
            if (is_periodic) {
                size_t ix_prev = (ix == 0) ? nx - 1 : ix - 1;
                size_t ix_next = (ix == nx - 1) ? 0 : ix + 1;
                dx_psi_term = (psi[iz * nx * ny + iy * nx + ix_next] - psi[iz * nx * ny + iy * nx + ix_prev]) / (2.0 * spacing[0]);
            } else {
                if (ix == 0) {
                    dx_psi_term = (psi[iz * nx * ny + iy * nx + (ix + 1)] - psi[iz * nx * ny + iy * nx + ix]) / spacing[0];
                } else if (ix == nx - 1) {
                    dx_psi_term = (psi[iz * nx * ny + iy * nx + ix] - psi[iz * nx * ny + iy * nx + (ix - 1)]) / spacing[0];
                } else {
                    dx_psi_term = (psi[iz * nx * ny + iy * nx + (ix + 1)] - psi[iz * nx * ny + iy * nx + (ix - 1)]) / (2.0 * spacing[0]);
                }
            }

            // Y derivative
            if (is_periodic) {
                size_t iy_prev = (iy == 0) ? ny - 1 : iy - 1;
                size_t iy_next = (iy == ny - 1) ? 0 : iy + 1;
                dy_psi_term = (psi[iz * nx * ny + iy_next * nx + ix] - psi[iz * nx * ny + iy_prev * nx + ix]) / (2.0 * spacing[1]);
            } else {
                if (iy == 0) {
                    dy_psi_term = (psi[iz * nx * ny + (iy + 1) * nx + ix] - psi[iz * nx * ny + iy * nx + ix]) / spacing[1];
                } else if (iy == ny - 1) {
                    dy_psi_term = (psi[iz * nx * ny + iy * nx + ix] - psi[iz * nx * ny + (iy - 1) * nx + ix]) / spacing[1];
                } else {
                    dy_psi_term = (psi[iz * nx * ny + (iy + 1) * nx + ix] - psi[iz * nx * ny + (iy - 1) * nx + ix]) / (2.0 * spacing[1]);
                }
            }

            // Z derivative
            if (is_periodic) {
                size_t iz_prev = (iz == 0) ? nz - 1 : iz - 1;
                size_t iz_next = (iz == nz - 1) ? 0 : iz + 1;
                dz_psi_term = (psi[(iz_next * nx * ny) + iy * nx + ix] - psi[(iz_prev * nx * ny) + iy * nx + ix]) / (2.0 * spacing[2]);
            } else {
                if (iz == 0) {
                    dz_psi_term = (psi[(iz + 1) * nx * ny + iy * nx + ix] - psi[iz * nx * ny + iy * nx + ix]) / spacing[2];
                } else if (iz == nz - 1) {
                    dz_psi_term = (psi[iz * nx * ny + iy * nx + ix] - psi[(iz - 1) * nx * ny + iy * nx + ix]) / spacing[2];
                } else {
                    dz_psi_term = (psi[(iz + 1) * nx * ny + iy * nx + ix] - psi[(iz - 1) * nx * ny + iy * nx + ix]) / (2.0 * spacing[2]);
                }
            }

            h_spatial_psi = Dirac::complex(0, -1.0) * (Dirac::multiply(Dirac::alpha_x, dx_psi_term) + Dirac::multiply(Dirac::alpha_y, dy_psi_term) + Dirac::multiply(Dirac::alpha_z, dz_psi_term));
        }

        // --- Término local del Hamiltoniano ---
        std::vector<double> coords = grid_.get_coords(i);
        double v_at_i = potential_->evaluate(coords);
        Dirac::Spinor h_local_psi = mass_ * Dirac::multiply(Dirac::beta, psi[i]) + v_at_i * psi[i];

        return h_spatial_psi + h_local_psi;
    }

    /**
     * @brief Devuelve el nombre del integrador.
     * @return La cadena `"FDTDLeapfrogIntegrator"`.
     */
    std::string get_name() const override {
        return "FDTDLeapfrogIntegrator";
    }

    /**
     * @brief Devuelve la función de onda actual en formato NumPy.
     *
     * Convierte internamente el vector de spinors `psi_curr_` a un array bidimensional
     * compatible con Python, de forma (N, 4).
     *
     * @return Un objeto `py::array_t<std::complex<double>>` con la función de onda actual.
     */
    py::array_t<std::complex<double>> get_psi() const {
        py::array_t<std::complex<double>> result({grid_.get_total_points(), 4ul});
        auto buf = result.request();
        auto* ptr = static_cast<std::complex<double>*>(buf.ptr);

        for (size_t i = 0; i < grid_.get_total_points(); ++i)
            for (size_t j = 0; j < 4; ++j)
                ptr[i * 4 + j] = psi_curr_[i].components[j];

        return result;
    }

private:
    Grid grid_;  /**< Malla espacial de la simulación. */
    std::shared_ptr<Potential> potential_; /**< Potencial escalar aplicado. */
    std::shared_ptr<BoundaryCondition> boundary_condition_; /**< Condición de frontera activa. */
    double dt_;   /**< Paso temporal del integrador. */
    double mass_; /**< Masa de la partícula. */

    std::vector<Dirac::Spinor> psi_prev_; /**< Estado de la función de onda en t - Δt. */
    std::vector<Dirac::Spinor> psi_curr_; /**< Estado actual de la función de onda en t. */
    std::vector<Dirac::Spinor> psi_next_; /**< Estado calculado de la función de onda en t + Δt. */
};
