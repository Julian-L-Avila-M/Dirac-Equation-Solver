# constants.py
"""
@file constants.py
@brief Configuración global de constantes físicas y sistema de unidades, para el uso en otras clases se debe usar (self), para llamar a las constantes dentro de la clase como: consts = Constants()
        self.c = consts.get("c")
        self.HBAR = consts.get("HBAR")
       para la librería Dirac Solver.
"""

class Constants:
    _instance = None   # Singleton global

    _SYSTEMS = {
        "SI": {
            "HBAR": 1.054571817e-34,
            "c": 2.99792458e8,
            "MASS_ELECTRON": 9.10938356e-31,
            "ELECTRON_CHARGE": 1.602176634e-19,
        },
        "NATURAL": {
            "HBAR": 1.0,
            "c": 1.0,
            "MASS_ELECTRON": 1.0,
            "ELECTRON_CHARGE": 1.0,
        },
        "ATOMIC": {
            "HBAR": 1.0,
            "c": 137.035999084,
            "MASS_ELECTRON": 1.0,
            "ELECTRON_CHARGE": 1.0,
        },
    }

    def __new__(cls, system="SI"):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance.set_system(system)
        return cls._instance

    def set_system(self, system):
        system = system.upper()
        if system not in Constants._SYSTEMS:
            raise ValueError(f"Sistema de unidades '{system}' no reconocido.")
        self.constants = Constants._SYSTEMS[system].copy()
        self.system = system

    def get(self, name):
        return self.constants.get(name)

    def as_dict(self):
        return self.constants.copy()
