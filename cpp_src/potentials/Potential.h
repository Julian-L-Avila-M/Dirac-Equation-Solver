#pragma once

#include <vector>
#include <string>
#include <memory>

/**
 * @class Potential
 * @brief Clase base abstracta para todos los potenciales escalares (patrón Template).
 *
 * Esta clase define la interfaz para cualquier potencial V(r) que se utilizará en
 * la ecuación de Dirac. Permite que diferentes funciones de potencial sean
 * implementadas y utilizadas indistintamente por el solver.
 */
class Potential {
public:
    virtual ~Potential() = default;

    /**
     * @brief Evalúa el potencial en una coordenada espacial dada.
     * @param r Un vector que representa la coordenada espacial (por ejemplo, [x, y, z]).
     * @return El valor escalar del potencial en ese punto.
     */
    virtual double evaluate(const std::vector<double>& r) const = 0;

    /**
     * @brief Devuelve el nombre del potencial.
     * @return Una cadena que contiene el nombre de la clase.
     */
    virtual std::string get_name() const = 0;
};