#!/bin/python3

import signal
import sys
import time

def signal_handler(sig, frame):
    print("\033[91mSeñal recibida, saliendo...\033[0m", file=sys.stderr)
    sys.exit(0)

# Configurar el manejador de señales
signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)

# Escribir un mensaje en stderr cuando el programa empiece a ejecutarse
print("\033[91mEl programa ha comenzado a ejecutarse.\033[0m", file=sys.stderr)

# Bucle infinito
while True:
    print("\033[93mEl programa sigue ejecutándose...\033[0m", file=sys.stderr)
    time.sleep(1)
