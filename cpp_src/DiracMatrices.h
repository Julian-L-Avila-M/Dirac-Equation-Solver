#ifndef DIRAC_MATRICES_H
#define DIRAC_MATRICES_H

#include <complex>
#include <vector>
#include <array>
#include <numeric>

/**
 * @namespace Dirac
 * @brief Espacio de nombres que contiene definiciones y utilidades para el álgebra de Dirac.
 */
namespace Dirac {

    using complex = std::complex<double>;
    using Matrix4x4 = std::array<std::array<complex, 4>, 4>;

    /**
     * @struct Spinor
     * @brief Representa un espinor de Dirac de 4 componentes.
     *
     * Esta estructura encapsula un array de 4 números complejos, que corresponde
     * a la función de onda de una partícula de espín 1/2 en la teoría de Dirac.
     */
    struct Spinor {
        /**
         * @brief Componentes del espinor (psi_1, psi_2, psi_3, psi_4).
         */
        std::array<complex, 4> components;

        /**
         * @brief Constructor por defecto. Inicializa todas las componentes a cero.
         */
        Spinor() : components{} {}

        /**
         * @brief Constructor que inicializa el espinor con un array dado.
         * @param comps Array de 4 complejos.
         */
        Spinor(const std::array<complex, 4>& comps) : components(comps) {}

        /**
         * @brief Acceso a las componentes del espinor (lectura/escritura).
         * @param i Índice de la componente (0-3).
         * @return Referencia al número complejo en la posición i.
         */
        complex& operator[](size_t i) { return components[i]; }

        /**
         * @brief Acceso a las componentes del espinor (solo lectura).
         * @param i Índice de la componente (0-3).
         * @return Referencia constante al número complejo en la posición i.
         */
        const complex& operator[](size_t i) const { return components[i]; }
    };

    // --- Sobrecarga de operadores para álgebra de espinores ---

    /**
     * @brief Suma de dos espinores.
     * @param a Primer espinor.
     * @param b Segundo espinor.
     * @return Nuevo espinor resultado de la suma componente a componente.
     */
    inline Spinor operator+(const Spinor& a, const Spinor& b) {
        return Spinor({a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]});
    }

    /**
     * @brief Resta de dos espinores.
     * @param a Primer espinor (minuendo).
     * @param b Segundo espinor (sustraendo).
     * @return Nuevo espinor resultado de la resta.
     */
    inline Spinor operator-(const Spinor& a, const Spinor& b) {
        return Spinor({a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]});
    }

    /**
     * @brief Multiplicación de un escalar por un espinor.
     * @param c Escalar complejo.
     * @param s Espinor.
     * @return Nuevo espinor escalado.
     */
    inline Spinor operator*(const complex& c, const Spinor& s) {
        return Spinor({c * s[0], c * s[1], c * s[2], c * s[3]});
    }

    /**
     * @brief Multiplicación de un espinor por un escalar (conmutativa).
     * @param s Espinor.
     * @param c Escalar complejo.
     * @return Nuevo espinor escalado.
     */
    inline Spinor operator*(const Spinor& s, const complex& c) {
        return c * s;
    }

    /**
     * @brief División de un espinor por un escalar real.
     * @param s Espinor.
     * @param d Divisor real.
     * @return Nuevo espinor dividido.
     */
    inline Spinor operator/(const Spinor& s, double d) {
        return Spinor({s[0] / d, s[1] / d, s[2] / d, s[3] / d});
    }

    /**
     * @brief Multiplicación de una matriz 4x4 por un espinor.
     * @param M Matriz compleja 4x4.
     * @param s Espinor de entrada.
     * @return Espinor resultante de la operación M * s.
     */
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

    /**
     * @brief Matriz identidad 4x4.
     */
    const Matrix4x4 I4 = {{
        {complex(1,0), complex(0,0), complex(0,0), complex(0,0)},
        {complex(0,0), complex(1,0), complex(0,0), complex(0,0)},
        {complex(0,0), complex(0,0), complex(1,0), complex(0,0)},
        {complex(0,0), complex(0,0), complex(0,0), complex(1,0)}
    }};

    /**
     * @brief Matriz beta (o gamma^0) de Dirac en representación estándar.
     *
     * Utilizada para el término de masa en el Hamiltoniano.
     */
    const Matrix4x4 beta = {{
        {complex(1,0), complex(0,0), complex(0,0), complex(0,0)},
        {complex(0,0), complex(1,0), complex(0,0), complex(0,0)},
        {complex(0,0), complex(0,0), complex(-1,0), complex(0,0)},
        {complex(0,0), complex(0,0), complex(0,0), complex(-1,0)}
    }};

    /**
     * @brief Matriz alpha_x de Dirac.
     *
     * Definida como alpha_x = gamma^0 * gamma^1.
     */
    const Matrix4x4 alpha_x = {{
        {complex(0,0), complex(0,0), complex(0,0), complex(1,0)},
        {complex(0,0), complex(0,0), complex(1,0), complex(0,0)},
        {complex(0,0), complex(1,0), complex(0,0), complex(0,0)},
        {complex(1,0), complex(0,0), complex(0,0), complex(0,0)}
    }};

    /**
     * @brief Matriz alpha_y de Dirac.
     *
     * Definida como alpha_y = gamma^0 * gamma^2.
     */
    const Matrix4x4 alpha_y = {{
        {complex(0,0), complex(0,0), complex(0,0), complex(0,-1)},
        {complex(0,0), complex(0,0), complex(0,1), complex(0,0)},
        {complex(0,0), complex(0,-1), complex(0,0), complex(0,0)},
        {complex(0,1), complex(0,0), complex(0,0), complex(0,0)}
    }};

    /**
     * @brief Matriz alpha_z de Dirac.
     *
     * Definida como alpha_z = gamma^0 * gamma^3.
     */
    const Matrix4x4 alpha_z = {{
        {complex(0,0), complex(0,0), complex(1,0), complex(0,0)},
        {complex(0,0), complex(0,0), complex(0,0), complex(-1,0)},
        {complex(1,0), complex(0,0), complex(0,0), complex(0,0)},
        {complex(0,-1), complex(0,0), complex(0,0), complex(0,0)}
    }};

} // namespace Dirac

#endif // DIRAC_MATRICES_H
