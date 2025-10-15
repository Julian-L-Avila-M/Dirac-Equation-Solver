#pragma once

#include "BoundaryCondition.h"
#include <vector>
#include "../DiracMatrices.h"

class AbsorbingBoundary : public BoundaryCondition {
public:
    AbsorbingBoundary(double strength) : strength_(strength) {}

    void apply(std::vector<Dirac::Spinor>& psi, const Grid& grid) const override {
        if (grid.get_dim() != 1) {
            throw std::runtime_error("AbsorbingBoundary solo está implementado para mallas 1D por el momento.");
        }

        size_t n_points = grid.get_total_points();
        if (n_points < 2) return;

        // Aplicar amortiguación a los puntos primero y último
        psi[0] = Dirac::complex(1.0 - strength_, 0.0) * psi[0];
        psi[n_points - 1] = Dirac::complex(1.0 - strength_, 0.0) * psi[n_points - 1];
    }

    std::string get_name() const override {
        return "AbsorbingBoundary";
    }

private:
    double strength_;
};