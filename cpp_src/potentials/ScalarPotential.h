#pragma once

#include "Potential.h"
#include <pybind11/pybind11.h>
#include <functional>

namespace py = pybind11;

class ScalarPotential : public Potential {
public:
    ScalarPotential(std::function<double(const std::vector<double>&)> func)
        : func_(func) {}

    double evaluate(const std::vector<double>& r) const override {
        return func_(r);
    }

    std::string get_name() const override {
        return "ScalarPotential";
    }

private:
    std::function<double(const std::vector<double>&)> func_;
};
