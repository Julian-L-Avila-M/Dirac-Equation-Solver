import sqlite3
import os

# 1. Localizamos la base de datos del historial
db_path = os.path.expanduser('~/.ipython/profile_default/history.sqlite')

print(f"Buscando historial en: {db_path}")

try:
    # 2. Conectamos usando la librería nativa de Python
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()

    # 3. Ejecutamos la consulta SQL
    # Buscamos código que no sea un simple 'save' y tenga contenido sustancial
    query = """
    SELECT source_raw 
    FROM history 
    WHERE source_raw NOT LIKE '%checkpoint%' 
    AND length(source_raw) > 5
    ORDER BY session DESC, line DESC 
    LIMIT 3000;
    """
    
    cursor.execute(query)
    results = cursor.fetchall()

    # 4. Escribimos el resultado en un archivo limpio
    output_file = os.path.expanduser('~/codigo_rescatado_final.py')
    
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(f"# REPORTE DE RECUPERACIÓN - {len(results)} BLOQUES ENCONTRADOS\n")
        f.write("# Los bloques más recientes están al principio del archivo.\n")
        f.write("# ========================================================\n\n")
        
        for i, row in enumerate(results):
            code = row[0]
            # Separador visual para que puedas distinguir ejecuciones
            f.write(f"# [BLOQUE {i+1}] --------------------------------------\n")
            f.write(code)
            f.write("\n\n")

    print(f"\n¡EXITO! Se recuperaron {len(results)} fragmentos de código.")
    print(f"El archivo se guardó en: {output_file}")

except Exception as e:
    print(f"\nERROR CRÍTICO: {e}")

finally:
    if 'conn' in locals():
        conn.close()
