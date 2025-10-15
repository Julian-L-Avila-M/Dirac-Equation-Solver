/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "dirac_solver", "index.html", [
    [ "Dirac-Solver: Solucionador numérico de la Ecuación de Dirac.", "index.html", "index" ],
    [ "Actualización de la librería Dirac Solver", "md_planning_2Actualizaci_xC3_xB3n__constants.html", [
      [ "Resumen", "md_planning_2Actualizaci_xC3_xB3n__constants.html#autotoc_md5", null ],
      [ "Cambios realizados", "md_planning_2Actualizaci_xC3_xB3n__constants.html#autotoc_md7", [
        [ "1. Eliminación de <tt>constants.py</tt>", "md_planning_2Actualizaci_xC3_xB3n__constants.html#autotoc_md8", null ],
        [ "2. Nuevas constantes globales definidas en <tt>__init__.py</tt>", "md_planning_2Actualizaci_xC3_xB3n__constants.html#autotoc_md9", null ]
      ] ],
      [ "Conclusión", "md_planning_2Actualizaci_xC3_xB3n__constants.html#autotoc_md11", null ]
    ] ],
    [ "Arquitectura y Patrones de Diseño de dirac_solver", "md_planning_2design-pattern.html", [
      [ "1. Tabla Comparativa: Patrones y Problemas que Resuelven", "md_planning_2design-pattern.html#autotoc_md13", null ],
      [ "2. Patrón Builder - Configuración de la Simulación", "md_planning_2design-pattern.html#autotoc_md15", [
        [ "API fluida gracias al Patrón Builder", "md_planning_2design-pattern.html#autotoc_md16", null ]
      ] ],
      [ "3. Patrón Strategy - Flexibilidad en los Algoritmos", "md_planning_2design-pattern.html#autotoc_md18", null ],
      [ "4. Patrones Template y Factory Method - Extensibilidad de Componentes Físicos", "md_planning_2design-pattern.html#autotoc_md20", null ],
      [ "5. Patrón Observer - Visualización y Monitoreo", "md_planning_2design-pattern.html#autotoc_md22", null ],
      [ "6. Patrón Facade - Interfaz Simplificada de I/O", "md_planning_2design-pattern.html#autotoc_md24", null ],
      [ "7. Patrón Decorator – Extensión Dinámica de Funcionalidades", "md_planning_2design-pattern.html#autotoc_md25", null ],
      [ "8. Patrón Adapter – Interoperabilidad con Librerías Externas", "md_planning_2design-pattern.html#autotoc_md27", null ],
      [ "9. Relaciones entre los Patrones", "md_planning_2design-pattern.html#autotoc_md29", null ],
      [ "10. Alternativas Consideradas y Descartadas", "md_planning_2design-pattern.html#autotoc_md31", [
        [ "1. Builder", "md_planning_2design-pattern.html#autotoc_md32", null ],
        [ "2. Strategy", "md_planning_2design-pattern.html#autotoc_md33", null ],
        [ "3. Template + Factory Method", "md_planning_2design-pattern.html#autotoc_md34", null ],
        [ "4. Observer", "md_planning_2design-pattern.html#autotoc_md35", null ],
        [ "5. Facade", "md_planning_2design-pattern.html#autotoc_md36", null ],
        [ "6. Decorator", "md_planning_2design-pattern.html#autotoc_md37", null ],
        [ "7. Adapter", "md_planning_2design-pattern.html#autotoc_md38", null ]
      ] ],
      [ "11. Conclusión", "md_planning_2design-pattern.html#autotoc_md40", null ]
    ] ],
    [ "Plan del Proyecto para un Solucionador de la Ecuación de Dirac", "md_planning_2problems-to-solve.html", [
      [ "1. Partícula Libre", "md_planning_2problems-to-solve.html#autotoc_md43", [
        [ "Descripción del Problema", "md_planning_2problems-to-solve.html#autotoc_md44", null ],
        [ "Parámetros de Entrada", "md_planning_2problems-to-solve.html#autotoc_md45", null ],
        [ "Salidas", "md_planning_2problems-to-solve.html#autotoc_md46", null ]
      ] ],
      [ "2. Pozo de Potencial Infinito", "md_planning_2problems-to-solve.html#autotoc_md48", [
        [ "Descripción del Problema", "md_planning_2problems-to-solve.html#autotoc_md49", null ],
        [ "Parámetros de Entrada", "md_planning_2problems-to-solve.html#autotoc_md50", null ],
        [ "Salidas", "md_planning_2problems-to-solve.html#autotoc_md51", null ]
      ] ],
      [ "3. Potencial a Trozos (Efecto Klein)", "md_planning_2problems-to-solve.html#autotoc_md53", [
        [ "Descripción del Problema", "md_planning_2problems-to-solve.html#autotoc_md54", null ],
        [ "Parámetros de Entrada", "md_planning_2problems-to-solve.html#autotoc_md55", null ],
        [ "Salidas", "md_planning_2problems-to-solve.html#autotoc_md56", null ]
      ] ],
      [ "4. Potencial Central (Interacción de Coulomb)", "md_planning_2problems-to-solve.html#autotoc_md58", [
        [ "Descripción del Problema", "md_planning_2problems-to-solve.html#autotoc_md59", null ],
        [ "Parámetros de Entrada", "md_planning_2problems-to-solve.html#autotoc_md60", null ],
        [ "Salidas", "md_planning_2problems-to-solve.html#autotoc_md61", null ]
      ] ],
      [ "5. Potencial de Yukawa", "md_planning_2problems-to-solve.html#autotoc_md63", [
        [ "Descripción del Problema", "md_planning_2problems-to-solve.html#autotoc_md64", null ],
        [ "Parámetros de Entrada", "md_planning_2problems-to-solve.html#autotoc_md65", null ],
        [ "Salidas", "md_planning_2problems-to-solve.html#autotoc_md66", null ]
      ] ]
    ] ],
    [ "Presentation Structure: Requirements Engineering for a Dirac Equation Solver", "md_planning_2requirements-engineering.html", null ],
    [ "Construcción del Espinor de Pauli General", "md_planning_2spinor-definition.html", null ],
    [ "Estructura del Proyecto dirac_solver", "md_planning_2structure.html", [
      [ "📂 Esquema Visual", "md_planning_2structure.html#autotoc_md97", null ],
      [ "Raíz del Proyecto", "md_planning_2structure.html#autotoc_md98", null ],
      [ "<tt>src/dirac_solver/</tt>", "md_planning_2structure.html#autotoc_md100", [
        [ "1. <tt>solvers/</tt> – Núcleo numérico", "md_planning_2structure.html#autotoc_md101", null ],
        [ "2. <tt>physics/</tt> – Definiciones físicas", "md_planning_2structure.html#autotoc_md102", null ],
        [ "3. <tt>potentials/</tt> – Potenciales", "md_planning_2structure.html#autotoc_md103", null ],
        [ "4. <tt>geometry/</tt> – Geometrías y dominios", "md_planning_2structure.html#autotoc_md104", null ],
        [ "5. <tt>conditions/</tt> – Condiciones", "md_planning_2structure.html#autotoc_md105", null ],
        [ "6. <tt>io/</tt> – Entrada/Salida", "md_planning_2structure.html#autotoc_md106", null ],
        [ "7. <tt>visualization/</tt> – Gráficos y análisis", "md_planning_2structure.html#autotoc_md107", null ],
        [ "8. <tt>utils/</tt> – Utilidades generales", "md_planning_2structure.html#autotoc_md108", null ]
      ] ],
      [ "<tt>tests/</tt>", "md_planning_2structure.html#autotoc_md110", null ],
      [ "<tt>examples/</tt>", "md_planning_2structure.html#autotoc_md112", null ],
      [ "<tt>docs/</tt>", "md_planning_2structure.html#autotoc_md114", null ]
    ] ],
    [ "Diagramas UML para la Arquitectura de dirac_solver", "md_planning_2ulm-diagrams.html", [
      [ "1. Patrón Builder", "md_planning_2ulm-diagrams.html#autotoc_md117", null ],
      [ "2. Patrón Strategy", "md_planning_2ulm-diagrams.html#autotoc_md118", null ],
      [ "3. Patrones Template y Factory Method", "md_planning_2ulm-diagrams.html#autotoc_md119", null ],
      [ "4. Patrón Observer", "md_planning_2ulm-diagrams.html#autotoc_md120", null ],
      [ "5. Patrón Facade", "md_planning_2ulm-diagrams.html#autotoc_md121", null ],
      [ "6. Patrón Decorator", "md_planning_2ulm-diagrams.html#autotoc_md122", null ],
      [ "7. Patrón Adapter", "md_planning_2ulm-diagrams.html#autotoc_md124", null ]
    ] ],
    [ "Espacios de nombres", "namespaces.html", [
      [ "Lista de espacios de nombres", "namespaces.html", "namespaces_dup" ],
      [ "Miembros del espacio de nombres ", "namespacemembers.html", [
        [ "Todos", "namespacemembers.html", null ],
        [ "Funciones", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "typedefs", "namespacemembers_type.html", null ]
      ] ]
    ] ],
    [ "Clases", "annotated.html", [
      [ "Lista de clases", "annotated.html", "annotated_dup" ],
      [ "Índice de clases", "classes.html", null ],
      [ "Jerarquía de clases", "hierarchy.html", "hierarchy" ],
      [ "Miembros de clases", "functions.html", [
        [ "Todos", "functions.html", null ],
        [ "Funciones", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Archivos", "files.html", [
      [ "Lista de archivos", "files.html", "files_dup" ],
      [ "Miembros de los archivos", "globals.html", [
        [ "Todos", "globals.html", null ],
        [ "Funciones", "globals_func.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"AbsorbingBoundary_8h.html",
"md_planning_2design-pattern.html#autotoc_md35"
];

var SYNCONMSG = 'pulsar para deshabilitar sincronización';
var SYNCOFFMSG = 'pulsar para habilitar sincronización';