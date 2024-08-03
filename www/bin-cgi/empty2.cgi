#!/bin/env python3
import sys
RED = "\033[31m"
RESET = "\033[0m"
print(f"{RED}############EMPTY CGI ################{RESET}", file=sys.stderr)

# Definir el contenido HTML de la página con un contador
html_content = """<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Página Bonita</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
            margin: 0;
            padding: 0;
        }
        .container {
            max-width: 800px;
            margin: 50px auto;
            background-color: #fff;
            padding: 20px;
            border-radius: 5px;
            box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1);
        }
        h1 {
            color: #333;
        }
        p {
            color: #666;
        }
        #counter {
            font-size: 24px;
            color: #333;
            margin-top: 20px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Bienvenido a mi Página Bonita</h1>
        <p>¡Espero que te guste este diseño!</p>
        <p>Puedes modificar este HTML como desees para hacerlo aún más bonito.</p>
        <div id="counter">Contador: 0</div>
    </div>
    <script>
        let count = 0;
        const counterElement = document.getElementById('counter');
        setInterval(() => {
            count++;
            counterElement.textContent = 'Contador: ' + count;
        }, 1000);
    </script>
</body>
</html>
"""

# Escribir el contenido HTML en stdout
print("Content-Type: text/html\r\n\r\n", end="")
print(html_content)