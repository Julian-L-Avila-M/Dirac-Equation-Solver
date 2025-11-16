#pragma once

#include <complex>
#include <array>
#include <vector>

namespace Dirac {

using complex = std::complex<double>;

// Una estructura envolvente para el espinor para asegurar que ADL funcione para sobrecargas de operadores.
struct Spinor {
    std::array<complex, 4> components;

    Spinor() : components({complex(0,0), complex(0,0), complex(0,0), complex(0,0)}) {}
    Spinor(const std::array<complex, 4>& comps) : components(comps) {}

    complex& operator[](size_t i) { return components[i]; }
    const complex& operator[](size_t i) const { return components[i]; }
};

using Matrix4x4 = std::array<std::array<complex, 4>, 4>;

// Matriz identidad
const Matrix4x4 I4 = {{
    {{{1, 0}, {0, 0}, {0, 0}, {0, 0}}},
    {{{0, 0}, {1, 0}, {0, 0}, {0, 0}}},
    {{{0, 0}, {0, 0}, {1, 0}, {0, 0}}},
    {{{0, 0}, {0, 0}, {0, 0}, {1, 0}}},
}};

// Matriz beta
const Matrix4x4 beta = {{
    {{{1, 0}, {0, 0}, {0, 0}, {0, 0}}},
    {{{0, 0}, {1, 0}, {0, 0}, {0, 0}}},
    {{{0, 0}, {0, 0}, {-1, 0}, {0, 0}}},
    {{{0, 0}, {0, 0}, {0, 0}, {-1, 0}}},
}};

// Matrices alfa (representación estándar)
const Matrix4x4 alpha_x = {{
    {{{0,0}, {0,0}, {0,0}, {1,0}}},
    {{{0,0}, {0,0}, {1,0}, {0,0}}},
    {{{0,0}, {1,0}, {0,0}, {0,0}}},
    {{{1,0}, {0,0}, {0,0}, {0,0}}},
}};

const Matrix4x4 alpha_y = {{
    {{{0,0}, {0,0}, {0,0}, {0,-1}}},
    {{{0,0}, {0,0}, {0,1}, {0,0}}},
    {{{0,0}, {0,-1}, {0,0}, {0,0}}},
    {{{0,1}, {0,0}, {0,0}, {0,0}}},
}};

const Matrix4x4 alpha_z = {{
    {{{0,0}, {0,0}, {1,0}, {0,0}}},
    {{{0,0}, {0,0}, {0,0}, {-1,0}}},
    {{{1,0}, {0,0}, {0,0}, {0,0}}},
    {{{0,0}, {-1,0}, {0,0}, {0,0}}},
}};

// Función auxiliar para la multiplicación matriz-espinor
inline Spinor multiply(const Matrix4x4& M, const Spinor& s) {
    Spinor result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.components[i] += M[i][j] * s.components[j];
        }
    }
    return result;
}

// Sobrecargas de operadores para la aritmética de espinores
inline Spinor operator+(const Spinor& a, const Spinor& b) {
    Spinor result;
    for (int i = 0; i < 4; ++i) {
        result.components[i] = a.components[i] + b.components[i];
    }
    return result;
}

inline Spinor operator-(const Spinor& a, const Spinor& b) {
    Spinor result;
    for (int i = 0; i < 4; ++i) {
        result.components[i] = a.components[i] - b.components[i];
    }
    return result;
}

inline Spinor operator*(const complex& c, const Spinor& s) {
    Spinor result;
    for (int i = 0; i < 4; ++i) {
        result.components[i] = c * s.components[i];
    }
    return result;
}

inline Spinor operator/(const Spinor& s, const double& d) {
    Spinor result;
    for (int i = 0; i < 4; ++i) {
        result.components[i] = s.components[i] / d;
    }
    return result;
}

} // namespace Dirac