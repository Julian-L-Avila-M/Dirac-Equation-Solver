#ifndef DIRAC_MATRICES_H
#define DIRAC_MATRICES_H

#include <complex>
#include <vector>
#include <array>
#include <numeric>

namespace Dirac {

    using complex = std::complex<double>;
    using Matrix4x4 = std::array<std::array<complex, 4>, 4>;

    // Definición de un espinor de 4 componentes como una estructura
    struct Spinor {
        std::array<complex, 4> components;

        Spinor() : components{} {} // Correct zero-initialization
        Spinor(const std::array<complex, 4>& comps) : components(comps) {}

        complex& operator[](size_t i) { return components[i]; }
        const complex& operator[](size_t i) const { return components[i]; }
    };

    // Sobrecarga de operadores para el álgebra de espinores
    inline Spinor operator+(const Spinor& a, const Spinor& b) {
        return Spinor({a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]});
    }

    inline Spinor operator-(const Spinor& a, const Spinor& b) {
        return Spinor({a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]});
    }

    inline Spinor operator*(const complex& c, const Spinor& s) {
        return Spinor({c * s[0], c * s[1], c * s[2], c * s[3]});
    }

    inline Spinor operator*(const Spinor& s, const complex& c) {
        return c * s;
    }

    inline Spinor operator/(const Spinor& s, double d) {
        return Spinor({s[0] / d, s[1] / d, s[2] / d, s[3] / d});
    }

    inline Spinor multiply(const Matrix4x4& M, const Spinor& s) {
        std::array<complex, 4> result_comps;
        for (int i = 0; i < 4; ++i) {
            result_comps[i] = 0;
            for (int j = 0; j < 4; ++j) {
                result_comps[i] += M[i][j] * s[j];
            }
        }
        return Spinor(result_comps);
    }

    // Matriz identidad 4x4
    const Matrix4x4 I4 = {{
        {complex(1,0), complex(0,0), complex(0,0), complex(0,0)},
        {complex(0,0), complex(1,0), complex(0,0), complex(0,0)},
        {complex(0,0), complex(0,0), complex(1,0), complex(0,0)},
        {complex(0,0), complex(0,0), complex(0,0), complex(1,0)}
    }};

    // Matriz beta (gamma^0)
    const Matrix4x4 beta = {{
        {complex(1,0), complex(0,0), complex(0,0), complex(0,0)},
        {complex(0,0), complex(1,0), complex(0,0), complex(0,0)},
        {complex(0,0), complex(0,0), complex(-1,0), complex(0,0)},
        {complex(0,0), complex(0,0), complex(0,0), complex(-1,0)}
    }};

    // Matrices alpha
    const Matrix4x4 alpha_x = {{
        {complex(0,0), complex(0,0), complex(0,0), complex(1,0)},
        {complex(0,0), complex(0,0), complex(1,0), complex(0,0)},
        {complex(0,0), complex(1,0), complex(0,0), complex(0,0)},
        {complex(1,0), complex(0,0), complex(0,0), complex(0,0)}
    }};
    const Matrix4x4 alpha_y = {{
        {complex(0,0), complex(0,0), complex(0,0), complex(0,-1)},
        {complex(0,0), complex(0,0), complex(0,1), complex(0,0)},
        {complex(0,0), complex(0,-1), complex(0,0), complex(0,0)},
        {complex(0,1), complex(0,0), complex(0,0), complex(0,0)}
    }};
    const Matrix4x4 alpha_z = {{
        {complex(0,0), complex(0,0), complex(1,0), complex(0,0)},
        {complex(0,0), complex(0,0), complex(0,0), complex(-1,0)},
        {complex(1,0), complex(0,0), complex(0,0), complex(0,0)},
        {complex(0,-1), complex(0,0), complex(0,0), complex(0,0)}
    }};

} // namespace Dirac

#endif // DIRAC_MATRICES_H
