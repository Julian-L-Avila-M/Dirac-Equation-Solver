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

// PYBIND11_MODULE define el punto de entrada para el módulo de extensión de Python.
// El primer argumento es el nombre del módulo (debe coincidir con el de CMakeLists.txt).
// El segundo argumento, 'm', es una variable de tipo py::module_ que es la interfaz principal
// para agregar nuevos enlaces.
PYBIND11_MODULE(_core, m) {
    m.doc() = "Plugin de pybind11 para dirac_solver_core"; // docstring opcional del módulo

    // Enlazar la interfaz de estrategia TimeIntegrator
    // Creamos una clase trampolín para que pybind11 maneje funciones virtuales
    class PyTimeIntegrator : public TimeIntegrator {
    public:
        using TimeIntegrator::TimeIntegrator; // Heredar constructores
        void step() override { PYBIND11_OVERRIDE_PURE(void, TimeIntegrator, step); }
        std::string get_name() const override { PYBIND11_OVERRIDE_PURE(std::string, TimeIntegrator, get_name); }
    };

    py::class_<TimeIntegrator, PyTimeIntegrator, std::shared_ptr<TimeIntegrator>>(m, "TimeIntegrator")
        .def(py::init<>())
        .def("step", &TimeIntegrator::step)
        .def("get_name", &TimeIntegrator::get_name);

    // Enlazar la clase Grid
    py::class_<Grid>(m, "Grid")
        .def(py::init<std::vector<size_t>, std::vector<double>>(),
             py::arg("shape"), py::arg("spacing"))
        .def("get_dim", &Grid::get_dim)
        .def("get_shape", &Grid::get_shape)
        .def("get_spacing", &Grid::get_spacing)
        .def("get_total_points", &Grid::get_total_points);

    // Enlazar la estrategia concreta FDTDLeapfrogIntegrator
    py::class_<FDTDLeapfrogIntegrator, TimeIntegrator, std::shared_ptr<FDTDLeapfrogIntegrator>>(m, "FDTDLeapfrogIntegrator")
        .def(py::init<py::array_t<std::complex<double>, py::array::c_style | py::array::forcecast>, const Grid&, std::shared_ptr<Potential>, std::shared_ptr<BoundaryCondition>, double, double>(),
             py::arg("initial_psi"), py::arg("grid"), py::arg("potential"), py::arg("boundary_condition"), py::arg("dt"), py::arg("mass"))
        .def("step", &FDTDLeapfrogIntegrator::step)
        .def("get_psi", &FDTDLeapfrogIntegrator::get_psi, "Devuelve el campo espinorial actual como un array de NumPy");

    // Enlazar la clase base BoundaryCondition (Estrategia)
    class PyBoundaryCondition : public BoundaryCondition {
    public:
        using BoundaryCondition::BoundaryCondition; // Heredar constructores
        void apply(std::vector<Dirac::Spinor>& psi, const Grid& grid) const override {
            PYBIND11_OVERRIDE_PURE(void, BoundaryCondition, apply, psi, grid);
        }
        std::string get_name() const override {
            PYBIND11_OVERRIDE_PURE(std::string, BoundaryCondition, get_name);
        }
    };

    py::class_<BoundaryCondition, PyBoundaryCondition, std::shared_ptr<BoundaryCondition>>(m, "BoundaryCondition")
        .def(py::init<>())
        .def("apply", &BoundaryCondition::apply, "Aplica la condición de borde", py::arg("psi"), py::arg("grid"))
        .def("get_name", &BoundaryCondition::get_name);

    // Enlazar la condición de borde concreta PeriodicBoundary
    py::class_<PeriodicBoundary, BoundaryCondition, std::shared_ptr<PeriodicBoundary>>(m, "PeriodicBoundary")
        .def(py::init<>())
        .def("get_name", &PeriodicBoundary::get_name);

    // Enlazar la clase base Potential (Plantilla)
    class PyPotential : public Potential {
    public:
        using Potential::Potential; // Heredar constructores
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
        .def("get_name", &Potential::get_name);

    // Enlazar el potencial concreto FreeParticle
    py::class_<FreeParticle, Potential, std::shared_ptr<FreeParticle>>(m, "FreeParticle")
        .def(py::init<>())
        .def("get_name", &FreeParticle::get_name); // evaluate() se hereda y funciona

    // Enlazar la clase ScalarPotential
    py::class_<ScalarPotential, Potential, std::shared_ptr<ScalarPotential>>(m, "ScalarPotential")
        .def(py::init<std::function<double(const std::vector<double>&)>>(),
             py::arg("func"))
        .def("get_name", &ScalarPotential::get_name);

    // Enlazar la clase CoulombPotential
    py::class_<CoulombPotential, Potential, std::shared_ptr<CoulombPotential>>(m, "CoulombPotential")
        .def(py::init<double, double>(),
             py::arg("Z"), py::arg("epsilon") = 1e-6)
        .def("get_name", &CoulombPotential::get_name);

    // Enlazar la clase AbsorbingBoundary
    py::class_<AbsorbingBoundary, BoundaryCondition, std::shared_ptr<AbsorbingBoundary>>(m, "AbsorbingBoundary")
        .def(py::init<double>(),
             py::arg("strength"))
        .def("get_name", &AbsorbingBoundary::get_name);
}