/**
 * @file bindings.cpp
 * @brief Enlaces de C++ a Python usando Pybind11 para el núcleo de simulación Dirac.
 *
 * Este archivo define el módulo `_core`, que expone las clases de C++ (potenciales,
 * condiciones de borde, mallas y métodos de integración temporal) al entorno Python.
 *
 * Las clases aquí expuestas se emplean desde Python para construir, configurar y
 * ejecutar simulaciones de la ecuación de Dirac mediante un motor numérico en C++.
 *
 * @details
 * Estructura general del módulo:
 * - **Grid:** Representa la malla espacial discreta.
 * - **Potential:** Jerarquía de potenciales físicos (Free, Scalar, Coulomb).
 * - **BoundaryCondition:** Estrategias de frontera (Periódica, Absorbente).
 * - **TimeIntegrator:** Métodos de evolución temporal (FDTDLeapfrog).
 *
 * @note Este archivo debe compilarse como extensión dinámica (shared library)
 *       a través de CMake y Pybind11.
 * @see https://pybind11.readthedocs.io/
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "Grid.h"
#include "TimeIntegrator.h"
#include "potentials/Potential.h"
#include "potentials/FreeParticle.h"
#include "potentials/ScalarPotential.h"
#include "potentials/CoulombPotential.h"
#include "boundaries/BoundaryCondition.h"
#include "boundaries/PeriodicBoundary.h"
#include "boundaries/AbsorbingBoundary.h"

namespace py = pybind11;

////////////////////////////////////////////////////////////////
/// @brief Punto de entrada del módulo Pybind11.
///
/// @param m Objeto de tipo `py::module_` que representa el módulo Python.
/// @details Este bloque define todas las clases y funciones visibles desde Python.
///          Cada clase C++ se enlaza con una clase Python equivalente.
///
/// Ejemplo en Python:
/// @code{.py}
/// import _core
/// grid = _core.Grid([100], [0.1])
/// potential = _core.CoulombPotential(Z=1.0)
/// @endcode
////////////////////////////////////////////////////////////////
PYBIND11_MODULE(_core, m) {
    m.doc() = "Plugin de pybind11 para dirac_solver_core"; ///< Descripción del módulo Python.

    // -------------------------------------------------------------------------
    // Enlace de la interfaz TimeIntegrator
    // -------------------------------------------------------------------------
    /**
     * @class PyTimeIntegrator
     * @brief Clase trampolín (trampoline class) para heredar métodos virtuales en Python.
     *
     * Permite implementar nuevos integradores temporales en Python
     * que hereden de la interfaz C++ TimeIntegrator.
     */
    class PyTimeIntegrator : public TimeIntegrator {
    public:
        using TimeIntegrator::TimeIntegrator;
        void step() override { PYBIND11_OVERRIDE_PURE(void, TimeIntegrator, step); }
        std::string get_name() const override { PYBIND11_OVERRIDE_PURE(std::string, TimeIntegrator, get_name); }
    };

    py::class_<TimeIntegrator, PyTimeIntegrator, std::shared_ptr<TimeIntegrator>>(m, "TimeIntegrator")
        .def(py::init<>())
        .def("step", &TimeIntegrator::step, "Ejecuta un paso temporal del integrador")
        .def("get_name", &TimeIntegrator::get_name, "Devuelve el nombre del integrador activo");

    // -------------------------------------------------------------------------
    // Clase Grid (estructura espacial discreta)
    // -------------------------------------------------------------------------
    /**
     * @class Grid
     * @brief Representa la malla espacial usada en la discretización de la ecuación de Dirac.
     *
     * Contiene información sobre la dimensionalidad, el número de puntos
     * y el espaciamiento entre nodos.
     */
    py::class_<Grid>(m, "Grid")
        .def(py::init<std::vector<size_t>, std::vector<double>>(),
             py::arg("shape"), py::arg("spacing"))
        .def("get_dim", &Grid::get_dim, "Devuelve la dimensión de la malla (1D, 2D o 3D)")
        .def("get_shape", &Grid::get_shape, "Devuelve el número de nodos por eje")
        .def("get_spacing", &Grid::get_spacing, "Devuelve el tamaño del paso en cada dirección")
        .def("get_total_points", &Grid::get_total_points, "Devuelve el número total de puntos de la malla");

    // -------------------------------------------------------------------------
    // Enlace del integrador concreto: FDTDLeapfrogIntegrator
    // -------------------------------------------------------------------------
    /**
     * @class FDTDLeapfrogIntegrator
     * @brief Implementación explícita del integrador Leapfrog (FDTD).
     *
     * Evoluciona el campo espinorial de la ecuación de Dirac en el tiempo,
     * utilizando un esquema de diferencias finitas centradas.
     */
    py::class_<FDTDLeapfrogIntegrator, TimeIntegrator, std::shared_ptr<FDTDLeapfrogIntegrator>>(m, "FDTDLeapfrogIntegrator")
        .def(py::init<
             py::array_t<std::complex<double>, py::array::c_style | py::array::forcecast>,
             const Grid&, std::shared_ptr<Potential>,
             std::shared_ptr<BoundaryCondition>, double, double>(),
             py::arg("initial_psi"), py::arg("grid"),
             py::arg("potential"), py::arg("boundary_condition"),
             py::arg("dt"), py::arg("mass"))
        .def("step", &FDTDLeapfrogIntegrator::step, "Ejecuta un paso temporal de la simulación")
        .def("get_psi", &FDTDLeapfrogIntegrator::get_psi,
             "Devuelve el campo espinorial actual como un array de NumPy");

    // -------------------------------------------------------------------------
    // Clase abstracta BoundaryCondition
    // -------------------------------------------------------------------------
    /**
     * @class PyBoundaryCondition
     * @brief Clase trampolín para permitir redefinir condiciones de borde desde Python.
     */
    class PyBoundaryCondition : public BoundaryCondition {
    public:
        using BoundaryCondition::BoundaryCondition;
        void apply(std::vector<Dirac::Spinor>& psi, const Grid& grid) const override {
            PYBIND11_OVERRIDE_PURE(void, BoundaryCondition, apply, psi, grid);
        }
        std::string get_name() const override {
            PYBIND11_OVERRIDE_PURE(std::string, BoundaryCondition, get_name);
        }
    };

    py::class_<BoundaryCondition, PyBoundaryCondition, std::shared_ptr<BoundaryCondition>>(m, "BoundaryCondition")
        .def(py::init<>())
        .def("apply", &BoundaryCondition::apply,
             "Aplica la condición de borde sobre el campo espinorial", py::arg("psi"), py::arg("grid"))
        .def("get_name", &BoundaryCondition::get_name, "Devuelve el nombre simbólico de la condición de borde");

    // -------------------------------------------------------------------------
    // Condición de borde concreta: PeriodicBoundary
    // -------------------------------------------------------------------------
    /**
     * @class PeriodicBoundary
     * @brief Implementa condiciones de borde periódicas en la malla espacial.
     */
    py::class_<PeriodicBoundary, BoundaryCondition, std::shared_ptr<PeriodicBoundary>>(m, "PeriodicBoundary")
        .def(py::init<>())
        .def("get_name", &PeriodicBoundary::get_name);

    // -------------------------------------------------------------------------
    // Clase abstracta Potential (jerarquía de potenciales)
    // -------------------------------------------------------------------------
    /**
     * @class PyPotential
     * @brief Trampolín que permite implementar nuevos potenciales desde Python.
     */
    class PyPotential : public Potential {
    public:
        using Potential::Potential;
        double evaluate(const std::vector<double>& r) const override {
            PYBIND11_OVERRIDE_PURE(double, Potential, evaluate, r);
        }
        std::string get_name() const override {
            PYBIND11_OVERRIDE_PURE(std::string, Potential, get_name);
        }
    };

    py::class_<Potential, PyPotential, std::shared_ptr<Potential>>(m, "Potential")
        .def(py::init<>())
        .def("evaluate", &Potential::evaluate, "Evalúa el potencial en una coordenada dada", py::arg("r"))
        .def("get_name", &Potential::get_name, "Devuelve el nombre simbólico del potencial");

    // -------------------------------------------------------------------------
    // Potenciales concretos
    // -------------------------------------------------------------------------
    /**
     * @class FreeParticle
     * @brief Potencial nulo (partícula libre).
     */
    py::class_<FreeParticle, Potential, std::shared_ptr<FreeParticle>>(m, "FreeParticle")
        .def(py::init<>())
        .def("get_name", &FreeParticle::get_name);

    /**
     * @class ScalarPotential
     * @brief Potencial definido por una función escalar arbitraria.
     *
     * Permite pasar desde Python una función f(r) que define el potencial.
     */
    py::class_<ScalarPotential, Potential, std::shared_ptr<ScalarPotential>>(m, "ScalarPotential")
        .def(py::init<std::function<double(const std::vector<double>&)>>(),
             py::arg("func"))
        .def("get_name", &ScalarPotential::get_name);

    /**
     * @class CoulombPotential
     * @brief Potencial de Coulomb regularizado para simulaciones atómicas.
     */
    py::class_<CoulombPotential, Potential, std::shared_ptr<CoulombPotential>>(m, "CoulombPotential")
        .def(py::init<double, double>(),
             py::arg("Z"), py::arg("epsilon") = 1e-6)
        .def("get_name", &CoulombPotential::get_name);

    // -------------------------------------------------------------------------
    // Condición de borde absorbente
    // -------------------------------------------------------------------------
    /**
     * @class AbsorbingBoundary
     * @brief Implementa una capa absorbente que atenúa la función de onda en los bordes.
     *
     * @param strength Parámetro de intensidad de absorción.
     */
    py::class_<AbsorbingBoundary, BoundaryCondition, std::shared_ptr<AbsorbingBoundary>>(m, "AbsorbingBoundary")
        .def(py::init<double>(), py::arg("strength"))
        .def("get_name", &AbsorbingBoundary::get_name);
}
