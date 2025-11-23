#!/bin/bash

# 1. Obtener la ruta absoluta del directorio donde está ESTE script (.sh)
# Esto funciona incluso si lo llamas desde otra carpeta o a través de un enlace simbólico.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# 2. Cambiar el directorio de trabajo a la raíz del proyecto
cd "$SCRIPT_DIR"

# 3. (Opcional) Verificar si el ejecutable existe antes de intentar correrlo
if [ ! -f "./build/Game" ]; then
    echo "❌ Error: No se encontró el ejecutable en ./build/Game"
    echo "   Asegúrate de haber compilado el proyecto."
    exit 1
fi

# 4. Imprimir feedback (Opcional, para que sepas que funcionó el cambio de ruta)
echo "📂 Directorio de trabajo establecido en: $(pwd)"
echo "🚀 Ejecutando GLSX..."
echo "----------------------------------------"

# 5. Ejecutar el juego
# "$@" pasa cualquier argumento extra que le des al script (ej: ./run.sh -windowed)
./build/Game "$@"