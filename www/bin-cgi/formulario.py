#!/usr/bin/env python3

import cgi

# Crear un objeto FieldStorage
form = cgi.FieldStorage()
# Cabecera de la respuesta HTTP
print("Content-Type: text/html", end="\r\n")
print(end="\r\n")  # Línea en blanco para finalizar la cabecera

# Obtener los datos del formulario
data1 = form.getvalue('data1', 'No se proporcionó data1')
data2 = form.getvalue('data2', 'No se proporcionó data2')

# Generar HTML de respuesta
print(f"""<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Datos Enviados</title>
	<link rel="stylesheet" href="/css/index.css">
</head>
<body>
    <h2>Datos Enviados</h2>
    <p><strong>Primer Dato:</strong> {data1}</p>
    <p><strong>Segundo Dato:</strong> {data2}</p>
    <p><a href="/">Volver a la Página Principal</a></p>
</body>
</html>
""")
