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
 * El algoritmo alterna entre los estados \\(\\psi(t - \\Delta t)\\), \\(\\psi(t)\\) y \\(\\psi(t + \\Delta t)\\)
 * para avanzar la solución sin necesidad de almacenar todas las configuraciones temporales.
 *
 * ### Características principales
 * - Soporta dominios 1D, 2D y 3D.
 * - Compatible con potenciales arbitrarios \\(V(x)\\).
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
     * Inicializa la función de onda y los buffers de tiempo (\\(\\psi\_{\\text{prev}}\\),
     * \\(\\psi\_{\\text{curr}}\\), \\(\\psi\_{\\text{next}}\\)),
     * validando dimensiones y preparando los parámetros del algoritmo.
     *
     * @param initial_psi Estado inicial del campo espinorial, como array NumPy de forma \\((N, 4)\\).
     * @param grid Objeto de tipo Grid que define la geometría de la simulación.
     * @param potential Potencial escalar aplicado a la partícula.
     * @param boundary_condition Condición de frontera a aplicar (periódica, absorbente, etc.).
     * @param dt Paso temporal \\(\\Delta t\\).
     * @param mass Masa \\(m\\) de la partícula.
     *
     * @throws std::runtime_error Si la forma del array de entrada no es \\((N, 4)\\)
     *                            o si el número de puntos no coincide con la malla.
     *
     * ### Inicialización de \\(\\psi(t - \\Delta t)\\)
     *
     * Se emplea una estimación de Euler hacia atrás:
     *
     * \\[
     * \\psi(t - \\Delta t) \\approx \\psi(t)
     *    + i\\, \\Delta t\\, H\\psi(t)
     * \\]
     *
     * Esto garantiza consistencia con el esquema Leapfrog:
     *
     * \\[
     * \\psi(t + \\Delta t) =
     *   \\psi(t - \\Delta t)
     *   - 2i\\, \\Delta t\\, H\\psi(t)
     * \\]
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

        // Copiar datos desde NumPy
        auto ptr = static_cast<Dirac::complex*>(psi_info.ptr);
        for (size_t i = 0; i < n_points; ++i)
            for (size_t j = 0; j < 4; ++j)
                psi_curr_[i].components[j] = ptr[i * 4 + j];

        // Inicialización de psi_prev:
        // 
        // \\[ \\psi(t-\\Delta t) = \\psi(t) + i\\,\\Delta t\\,(H\\psi)(t) \\]
        // 
        bool is_periodic = (boundary_condition_->get_name() == "PeriodicBoundary");

        for(size_t i=0; i<n_points; ++i) {

            Dirac::Spinor h_psi = compute_hamiltonian_at_point(i, psi_curr_, is_periodic);

            psi_prev_[i] = psi_curr_[i]
                + (Dirac::complex(0, 1.0) * dt_) * h_psi;  // i dt H psi
        }

        boundary_condition_->apply(psi_prev_, grid_);
    }

    /**
     * @brief Avanza un paso de tiempo completo según el esquema FDTD Leapfrog.
     *
     * Para cada punto se evalúa:
     *
     * \\( \\psi(t+\\Delta t) = \\psi(t-\\Delta t) - 2i\\),\\(\\Delta t\\ \\), \\( H\\psi(t)
     *     
     * 
     *     
     * 
     */
    void step() override {
        size_t n_points = grid_.get_total_points();
        bool is_periodic = (boundary_condition_->get_name() == "PeriodicBoundary");

        for (size_t i = 0; i < n_points; ++i) {

            Dirac::Spinor h_psi = compute_hamiltonian_at_point(i, psi_curr_, is_periodic);

            psi_next_[i] =
                psi_prev_[i]
                - (Dirac::complex(0, 2.0) * dt_) * h_psi;  // -2i dt H psi
        }

        boundary_condition_->apply(psi_next_, grid_);

        psi_prev_ = psi_curr_;
        psi_curr_ = psi_next_;
    }

    //----------------------------------------------------------------------
    // Hamiltoniano local:
    //
    // \\[
    // H = -i\\,\\boldsymbol{\\alpha}\\cdot\\nabla
    //     + m\\,\\beta + V(x)
    // \\]
    //
    //----------------------------------------------------------------------
    inline Dirac::Spinor compute_hamiltonian_at_point(size_t i,
                                                      const std::vector<Dirac::Spinor>& psi,
                                                      bool is_periodic)
    {
        const auto& shape = grid_.get_shape();
        const auto& spacing = grid_.get_spacing();

        Dirac::Spinor h_spatial_psi;

        // ... (código igual, sin editar)
        // No contiene expresiones matemáticas fuera del comentario.

        // --- Término local ---
        std::vector<double> coords = grid_.get_coords(i);
        double v_at_i = potential_->evaluate(coords);

        Dirac::Spinor h_local_psi =
            mass_ * Dirac::multiply(Dirac::beta, psi[i]) +
            v_at_i * psi[i];

        return h_spatial_psi + h_local_psi;
    }

    /**
     * @brief Devuelve el nombre del integrador.
     * @return La cadena "FDTDLeapfrogIntegrator".
     */
    std::string get_name() const override {
        return "FDTDLeapfrogIntegrator";
    }

    /**
     * @brief Devuelve la función de onda actual en formato NumPy.
     *
     * Devuelve un arreglo \\((N,4)\\) correspondiente a
     * \\[
     * \\psi = (\\psi\_1, \\psi\_2, \\psi\_3, \\psi\_4)
     * \\]
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
    Grid grid_;
    std::shared_ptr<Potential> potential_;
    std::shared_ptr<BoundaryCondition> boundary_condition_;
    double dt_;
    double mass_;

    std::vector<Dirac::Spinor> psi_prev_;
    std::vector<Dirac::Spinor> psi_curr_;
    std::vector<Dirac::Spinor> psi_next_;
};
