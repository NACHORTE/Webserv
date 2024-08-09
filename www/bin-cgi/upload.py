#!/usr/bin/env python3

import os
import sys
import io
import cgi

ruta_archivo = "../upload/"
# Definir colores para la salida
RED = "\033[91m"
RESET = "\033[0m"
GREEN = "\033[92m"
CYAN = "\033[96m"

form = cgi.FieldStorage()
filename = form["files"].filename
filedata = form["files"].file.read()
content_type = form["files"].type  # Obtener el Content-Type del archivo

# Obtén el directorio del archivo
directorio = os.path.dirname(ruta_archivo)

# Crea los directorios si no existen
if not os.path.exists(directorio):
    os.makedirs(directorio)

# Guarda el archivo subido
with open(f"{ruta_archivo}/{filename}", "wb") as f:
    f.write(filedata)

# Verificar el tipo de archivo usando Content-Type
is_text_file = content_type.startswith('text/')
is_image_file = content_type.startswith('image/')

# Si es un archivo de texto, leer su contenido
text_content = ""
if is_text_file:
    with open(f"{ruta_archivo}/{filename}", "r", encoding="utf-8") as f:
        text_content = f.read()

print("Content-Type: text/html", end="\r\n")
print(end="\r\n")
print(f"""<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Archivo Subido</title>
    <link rel="stylesheet" href="/css/index.css"> 
    <style>
        img {{
            max-width: 500px;
            width: 100%;
            height: auto;
        }}
        .text-window {{
            display: flex;
            flex-direction: column;
            align-items: center; /* Centra el contenido dentro del contenedor */
            justify-content: center; /* Centra verticalmente si el contenedor tiene altura */
            width: 500px;  /* Ancho de la ventana de texto */
            height: 300px; /* Alto de la ventana de texto */
            overflow-y: scroll; /* Habilita el desplazamiento vertical */
            border: 1px solid #ccc; /* Borde de la ventana */
            padding: 10px; /* Espacio interno */
            background-color: #f9f9f9; /* Color de fondo */
            margin-top: 20px; /* Espacio superior */
        }}
    </style>
</head>
<body>
    <h1>Has subido este archivo:</h1>
    {(f'<img src="/upload/{filename}" alt="{filename}">' if is_image_file else '')}
    {(f'<div class="text-window"><pre>' + text_content + '</pre></div>' if is_text_file else '')}
    {(f'<p>Nombre del archivo: {filename}</p>' if not is_image_file and not is_text_file else '')}
    <p><a href="/upload/{filename}">Ver archivo subido</a></p>
    <p><a href="/">Volver a la Página Principal</a></p>
</body>
</html>
""")
