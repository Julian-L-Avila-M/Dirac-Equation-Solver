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



import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

def create_animation(storage_path: str, output_path: str, interval=50):
    """
    Crea una animación (GIF o MP4) a partir de un archivo HDF5 de almacenamiento.
    """
    with h5py.File(storage_path, 'r') as f:
        psi_dset = f['psi']
        time_dset = f['time']

        grid_shape = f.attrs['grid_shape']
        grid_dim = f.attrs['grid_dim']

        fig = plt.figure(figsize=(10, 6))
        ax = fig.add_subplot(1, 1, 1)

        def update(frame):
            ax.clear()
            psi = psi_dset[frame]
            rho = np.sum(np.conj(psi) * psi, axis=1).real
            rho_grid = rho.reshape(grid_shape)

            # ------------------------
            #   Dimensión 1D
            # ------------------------
            if grid_dim == 1:
                ax.plot(rho_grid)
                ax.set_title(f"Time: {time_dset[frame]:.2f}")
                ax.set_xlabel("Position (x)")
                ax.set_ylabel("Probability Density")

            # ------------------------
            #   Dimensión 2D
            # ------------------------
            elif grid_dim == 2:
                ax.imshow(rho_grid.T, origin='lower', cmap='viridis')
                ax.set_title(f"Time: {time_dset[frame]:.2f}")
                ax.set_xlabel("Position (x)")
                ax.set_ylabel("Position (y)")

            # ------------------------
            #   Dimensión 3D
            # ------------------------
            elif grid_dim == 3:
                Nx, Ny, Nz = grid_shape

                # Cortes centrales
                slice_x = rho_grid[Nx // 2, :, :]
                slice_y = rho_grid[:, Ny // 2, :]
                slice_z = rho_grid[:, :, Nz // 2]

                fig.clear()

                ax1 = fig.add_subplot(1, 3, 1)
                ax2 = fig.add_subplot(1, 3, 2)
                ax3 = fig.add_subplot(1, 3, 3)

                im1 = ax1.imshow(slice_x.T, origin='lower', cmap='viridis')
                ax1.set_title(f"X-mid\n t={time_dset[frame]:.2f}")
                ax1.set_xlabel("y")
                ax1.set_ylabel("z")

                im2 = ax2.imshow(slice_y.T, origin='lower', cmap='viridis')
                ax2.set_title("Y-mid")
                ax2.set_xlabel("x")
                ax2.set_ylabel("z")

                im3 = ax3.imshow(slice_z.T, origin='lower', cmap='viridis')
                ax3.set_title("Z-mid")
                ax3.set_xlabel("x")
                ax3.set_ylabel("y")

        anim = FuncAnimation(fig, update, frames=len(time_dset), interval=interval)

        if output_path.endswith('.gif'):
            anim.save(output_path, writer='imagemagick')
        elif output_path.endswith('.mp4'):
            anim.save(output_path, writer='ffmpeg')
        else:
            raise ValueError("Unsupported output format. Use .gif or .mp4")

        plt.close(fig)
        print(f"Animation saved to {output_path}")
