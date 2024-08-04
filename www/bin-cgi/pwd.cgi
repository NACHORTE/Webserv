#!/bin/env python3

import os

# Obtener el directorio de trabajo actual
current_directory = os.getcwd()

# Definir el contenido HTML de la página, incluyendo el directorio de trabajo
html_content = f"""Content-Type: text/html\r\n\r\n<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Página Bonita</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
            margin: 0;
            padding: 0;
        }}
        .container {{
            max-width: 800px;
            margin: 50px auto;
            background-color: #fff;
            padding: 20px;
            border-radius: 5px;
            box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1);
        }}
        h1 {{
            color: #333;
        }}
        p {{
            color: #666;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>Bienvenido a mi Página Bonita</h1>
        <p>¡Espero que te guste este diseño!</p>
        <p>Puedes modificar este HTML como desees para hacerlo aún más bonito.</p>
        <p>Directorio de trabajo actual: <strong>{current_directory}</strong></p>
    </div>
</body>
</html>
"""

# Escribir el contenido HTML en stdout
print(html_content)
