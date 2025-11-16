"""
@file storage.py
@brief Módulo para manejar la entrada/salida (E/S) de datos de simulación.
@details Contiene clases para guardar los resultados de la simulación,
"""

import h5py
import numpy as np
from .core import SimulationProblem 

class HDF5Storage:
    """
    Maneja el almacenamiento de snapshots de simulación en un archivo HDF5.
    """
    
    def __init__(self, filepath: str, problem: SimulationProblem):
        """
        Inicializa el archivo de salida y guarda los metadatos de la simulación.

        Parámetros:
        - filepath (str): Ruta al archivo .hdf5 que se creará.
        - problem (SimulationProblem): El objeto del problema para extraer metadatos.
        """
        self.filepath = filepath
        self.problem = problem
        # 'w' = modo escritura (crea un archivo nuevo o sobrescribe si existe)
        self.file = h5py.File(filepath, 'w') 
        self.psi_dataset = None
        self.time_dataset = None
        
        print(f"HDF5Storage inicializado. Guardando datos en: {self.filepath}")

        # Guardar metadatos útiles como atributos en la raíz del archivo HDF5
        try:
            self.file.attrs['grid_dim'] = problem.grid.dim
            self.file.attrs['grid_shape'] = problem.grid.shape
            self.file.attrs['grid_spacing'] = problem.grid.spacing
            self.file.attrs['grid_origin'] = problem.grid.origin
            self.file.attrs['total_time'] = problem.total_time
            self.file.attrs['time_step'] = problem.time_step
        except Exception as e:
            print(f"Advertencia: No se pudieron guardar algunos metadatos. {e}")

    def write_snapshot(self, psi: np.ndarray, time: float):
        """
        Escribe un nuevo snapshot (psi y tiempo) al archivo.
        Crea los datasets en la primera llamada, y luego los extiende.
        """
        try:
            if self.psi_dataset is None:
                # --- Primera escritura: crear los datasets ---
                # Shape inicial (1, N_points, 4)
                self.psi_dataset = self.file.create_dataset(
                    "psi",
                    shape=(1,) + psi.shape,       
                    # Habilita 'append' (eje 0 extensible)
                    maxshape=(None,) + psi.shape, 
                    dtype='complex128',
                    chunks=True  # Habilita chunking para I/O eficiente
                )
                self.time_dataset = self.file.create_dataset(
                    "time",
                    shape=(1,),
                    maxshape=(None,),
                    dtype='float64'
                )
                # Guardar los primeros datos (t=0)
                self.psi_dataset[0] = psi
                self.time_dataset[0] = time
            
            else:
                # --- Escrituras subsiguientes: redimensionar y añadir ---
                new_size = self.psi_dataset.shape[0] + 1
                
                # Redimensionar el eje 0 (el de los snapshots)
                self.psi_dataset.resize(new_size, axis=0) 
                self.psi_dataset[-1] = psi # Añadir en la última posición
                
                self.time_dataset.resize(new_size, axis=0)
                self.time_dataset[-1] = time
        
        except Exception as e:
            print(f"Error al escribir snapshot en HDF5 (t={time}): {e}")

    def close(self):
        """
        Cierra el archivo HDF5. Es crucial llamar a esto al final.
        """
        if self.file:
            self.file.close()
            self.file = None
            print(f"Datos guardados y archivo cerrado: {self.filepath}")

    def __enter__(self):
        # Esto permite usar: 'with HDF5Storage(...) as storage:'
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        # Esto asegura que .close() se llame automáticamente al salir del 'with'
        self.close()
