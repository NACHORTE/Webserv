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

# Obtén el directorio del archivo
directorio = os.path.dirname(ruta_archivo)

# Crea los directorios si no existen
if not os.path.exists(directorio):
    os.makedirs(directorio)

with open(f"{ruta_archivo}/{filename}", "wb") as f:
	f.write(filedata)
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
    </style>
</head>
<body>
    <h1>Has subido este archivo:</h1>
    <img src="/upload/{filename}" alt="{filename}">
    <p><a href="/">Volver a la Página Principal</a></p>
</body>
</html>
""")
