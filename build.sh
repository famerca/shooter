#!/bin/bash

# Este script automatiza el proceso de configuración y compilación de CMake.
# Asume que estás ejecutando el script desde el directorio raíz del proyecto.

# 1. Definir los directorios de origen (source) y construcción (build)
SOURCE_DIR="."
BUILD_DIR="./build"

echo "--- Iniciando el proceso de compilación con CMake ---"

# 2. Configuración: Generar los archivos de Make en el directorio 'build'.
# -B: Directorio de construcción.
# -S: Directorio de origen (el actual).
echo "1. Ejecutando la configuración de CMake..."
cmake -B "${BUILD_DIR}" -S "${SOURCE_DIR}"

# Verificar si la configuración fue exitosa
if [ $? -ne 0 ]; then
    echo "¡Error! La configuración de CMake falló."
    exit 1
fi

# 3. Compilación: Construir el proyecto usando los archivos generados en 'build'.
echo "2. Compilando el proyecto..."
# El argumento -- -j$(nproc) usa todos los núcleos disponibles para acelerar la compilación.
cmake --build "${BUILD_DIR}" -- -j$(nproc)

# Verificar si la compilación fue exitosa
if [ $? -ne 0 ]; then
    echo "¡Error! La compilación falló."
    exit 1
fi

echo "--- Compilación finalizada exitosamente ---"