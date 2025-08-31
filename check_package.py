import os

base_path = "src/dirac_solver"
init_path = os.path.join(base_path, "__init__.py")

print("=== Verificación de estructura del paquete ===")
if os.path.isdir(base_path):
    print(f"[OK] Carpeta encontrada: {base_path}")
else:
    print(f"[ERROR] No existe la carpeta {base_path}")

if os.path.isfile(init_path):
    print(f"[OK] Se encontró {init_path}")
    print("\n--- Contenido de __init__.py ---")
    with open(init_path, "r", encoding="utf-8") as f:
        print(f.read())
    print("--- Fin de archivo ---\n")
else:
    print(f"[ERROR] No existe {init_path}")

print("=== Fin de la verificación ===")
