#pragma once

#include "Potential.h"
#include <cmath>
#include <numeric>

class CoulombPotential : public Potential {
public:
    CoulombPotential(double Z, double epsilon = 1e-6)
        : Z_(Z), epsilon_(epsilon) {}

    double evaluate(const std::vector<double>& r) const override {
        double r_norm = 0.0;
        for (double val : r) {
            r_norm += val * val;
        }
        r_norm = std::sqrt(r_norm);

        // Regularizar el potencial en r=0
        return -Z_ / (r_norm + epsilon_);
    }

    std::string get_name() const override {
        return "CoulombPotential";
    }

private:
    double Z_;
    double epsilon_; // Pequeño parámetro de regularización
};
