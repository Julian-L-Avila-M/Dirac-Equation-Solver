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

    def write_observables(self, observables_handler):
        """
        Escribe el historial de todos los observables calculados a datasets.
        """
        if not self.file:
            print("Error: El archivo HDF5 está cerrado. No se pueden escribir observables.")
            return

        print("Guardando historial de observables en HDF5...")
        try:
            # Crear un grupo para los observables para mantener el orden
            obs_group = self.file.create_group("observables")

            for key, history in observables_handler.history.items():
                # Convertir la lista de historial a un array de NumPy
                data = np.array(history)
                # Crear un dataset para cada observable
                obs_group.create_dataset(key, data=data)

            print("Historial de observables guardado correctamente.")

        except Exception as e:
            print(f"Error al guardar los observables en HDF5: {e}")



import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from mpl_toolkits.mplot3d import Axes3D

def create_animation(storage_path: str, output_path: str, interval=50):
    """
    Crea una animación (GIF o MP4) a partir de un archivo HDF5 de almacenamiento.
    """
    with h5py.File(storage_path, 'r') as f:
        psi_dset = f['psi']
        time_dset = f['time']

        grid_shape = f.attrs['grid_shape']
        grid_dim = f.attrs['grid_dim']
        grid_spacing = f.attrs['grid_spacing']
        grid_origin = f.attrs['grid_origin']

        fig = plt.figure(figsize=(10, 8))
        ax = fig.add_subplot(111, projection='3d' if grid_dim == 3 else None)

        def update(frame):
            ax.clear()
            psi = psi_dset[frame]
            rho = np.sum(np.conj(psi) * psi, axis=1).real
            rho_grid = rho.reshape(grid_shape)

            if grid_dim == 1:
                ax.plot(rho_grid)
                ax.set_title(f"Time: {time_dset[frame]:.2f}")
                ax.set_xlabel("Position (x)")
                ax.set_ylabel("Probability Density")

            elif grid_dim == 2:
                ax.imshow(rho_grid.T, origin='lower', cmap='viridis',
                          extent=[grid_origin[0], grid_origin[0] + (grid_shape[0] - 1) * grid_spacing[0],
                                  grid_origin[1], grid_origin[1] + (grid_shape[1] - 1) * grid_spacing[1]])
                ax.set_title(f"Time: {time_dset[frame]:.2f}")
                ax.set_xlabel("Position (x)")
                ax.set_ylabel("Position (y)")

            elif grid_dim == 3:
                ax.set_facecolor('black')
                coords = np.mgrid[grid_origin[0]:grid_origin[0] + grid_shape[0] * grid_spacing[0]:grid_spacing[0],
                                  grid_origin[1]:grid_origin[1] + grid_shape[1] * grid_spacing[1]:grid_spacing[1],
                                  grid_origin[2]:grid_origin[2] + grid_shape[2] * grid_spacing[2]:grid_spacing[2]]

                skip = 2
                x = coords[0, ::skip, ::skip, ::skip].ravel()
                y = coords[1, ::skip, ::skip, ::skip].ravel()
                z = coords[2, ::skip, ::skip, ::skip].ravel()

                rho_flat = rho_grid[::skip, ::skip, ::skip].ravel()

                sc = ax.scatter(x, y, z, c=rho_flat, cmap='viridis', alpha=0.3)

                ax.set_xlabel("Position (x)", color="white")
                ax.set_ylabel("Position (y)", color="white")
                ax.set_zlabel("Position (z)", color="white")
                ax.tick_params(colors='white')
                ax.set_title(f"Time: {time_dset[frame]:.2f}", color="white")


        anim = FuncAnimation(fig, update, frames=len(time_dset), interval=interval)

        if output_path.endswith('.gif'):
            anim.save(output_path, writer='imagemagick')
        elif output_path.endswith('.mp4'):
            anim.save(output_path, writer='ffmpeg')
        else:
            raise ValueError("Unsupported output format. Use .gif or .mp4")

        plt.close(fig)
        print(f"Animation saved to {output_path}")

def create_animation_1d(storage_path: str, output_path: str, interval=50):
    """
    Crea una animación para una simulación 1D, mostrando la densidad de probabilidad
    y la posición esperada a lo largo del tiempo.
    """
    with h5py.File(storage_path, 'r') as f:
        # --- Cargar datos de la simulación ---
        psi_dset = f['psi']
        time_dset = f['time']

        # --- Cargar datos de observables ---
        expected_pos_dset = f['observables/expected_position']

        # --- Cargar metadatos de la malla ---
        grid_shape = f.attrs['grid_shape']
        grid_spacing = f.attrs['grid_spacing']
        grid_origin = f.attrs['grid_origin']

        # Crear coordenadas espaciales para el eje x
        x_coords = np.linspace(grid_origin[0],
                               grid_origin[0] + (grid_shape[0] - 1) * grid_spacing[0],
                               grid_shape[0])

        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8), gridspec_kw={'height_ratios': [3, 1]})
        fig.suptitle("Simulación 1D de la Ecuación de Dirac")

        # --- Gráfico 1: Densidad de Probabilidad ---
        ax1.set_xlabel("Posición (x)")
        ax1.set_ylabel("Densidad de Probabilidad (ρ)")
        ax1.grid(True)
        line, = ax1.plot([], [], lw=2)
        # Línea vertical para la posición esperada
        pos_marker, = ax1.plot([], [], 'r--', lw=2, label="Posición Esperada <x>")
        time_text = ax1.text(0.05, 0.9, '', transform=ax1.transAxes)

        # --- Gráfico 2: Evolución de la Posición Esperada ---
        ax2.set_xlabel("Tiempo")
        ax2.set_ylabel("Posición Esperada <x>")
        ax2.grid(True)
        # Línea que muestra la historia de la posición esperada
        expected_pos_line, = ax2.plot([], [], 'r-', lw=2)

        def init():
            # Establecer límites de los ejes
            rho_max = np.max(np.sum(np.abs(psi_dset[0])**2, axis=1))
            ax1.set_xlim(x_coords[0], x_coords[-1])
            ax1.set_ylim(0, rho_max * 1.1)

            ax2.set_xlim(0, time_dset[-1])
            min_pos, max_pos = np.min(expected_pos_dset), np.max(expected_pos_dset)
            ax2.set_ylim(min_pos - 0.1, max_pos + 0.1)

            line.set_data([], [])
            pos_marker.set_data([], [])
            expected_pos_line.set_data([], [])
            time_text.set_text('')
            ax1.legend()
            return line, pos_marker, time_text, expected_pos_line

        def update(frame):
            # Actualizar densidad de probabilidad
            psi = psi_dset[frame]
            rho = np.sum(np.abs(psi)**2, axis=1)
            line.set_data(x_coords, rho)

            # Actualizar marcador de posición esperada
            current_expected_pos = expected_pos_dset[frame][0]
            pos_marker.set_data([current_expected_pos, current_expected_pos], ax1.get_ylim())

            # Actualizar historial de posición esperada
            expected_pos_line.set_data(time_dset[:frame+1], expected_pos_dset[:frame+1, 0])

            time_text.set_text(f'Tiempo: {time_dset[frame]:.2f}')
            return line, pos_marker, time_text, expected_pos_line

        anim = FuncAnimation(fig, update, frames=len(time_dset),
                               init_func=init, blit=True, interval=interval)

        # Guardar animación
        if output_path.endswith('.gif'):
            anim.save(output_path, writer='imagemagick')
        elif output_path.endswith('.mp4'):
            anim.save(output_path, writer='ffmpeg')
        else:
            raise ValueError("Formato de salida no soportado. Use .gif o .mp4")

        plt.close(fig)
        print(f"Animación guardada en {output_path}")
