# Shooter Game

Un juego de plataformas 3D desarrollado en C++ utilizando un motor de juego personalizado y múltiples librerías modernas.

## 📋 Descripción

Este proyecto es un juego de plataformas 3D que utiliza un motor de juego personalizado (GLS) junto con varias librerías de terceros para proporcionar gráficos, física, audio e interfaz de usuario.

## 🛠️ Librerías Utilizadas

### Motor de Juego Propio

#### **GLS (Game Library System)**
- **Descripción**: Motor de juego personalizado que proporciona abstracciones de alto nivel para renderizado, física, audio y UI.
- **Ubicación**: `GLS/`
- **Componentes principales**:
  - Sistema de ventanas y renderizado OpenGL
  - Sistema de física integrado con Jolt
  - Gestor de audio
  - Gestor de UI con RmlUi
  - Sistema de componentes (Transform, Camera, Model, etc.)
  - Gestión de escenas y objetos del juego

### Motor de Física

#### **Jolt Physics**
- **Descripción**: Motor de física de alto rendimiento desarrollado por Jorrit Rouwé, diseñado para juegos modernos.
- **Versión**: Incluida en `GLS/lib/libJolt.a`
- **Uso**: Sistema de colisiones, detección de contactos, cuerpos rígidos (estáticos, cinemáticos y dinámicos)
- **Características**:
  - Soporte para formas geométricas (cajas, esferas, cápsulas)
  - Callbacks de colisión (onContactStart, onContactEnd)
  - Optimizaciones para alto rendimiento
- **Documentación**: [Jolt Physics GitHub](https://github.com/jrouwe/JoltPhysics)

### Gráficos y Renderizado

#### **OpenGL**
- **Descripción**: API estándar de la industria para renderizado de gráficos 2D y 3D.
- **Versión**: OpenGL 3.3+
- **Uso**: Renderizado de modelos 3D, shaders, texturas, skybox
- **Características**:
  - Pipeline de renderizado programable con shaders
  - Soporte para texturas PBR (Physically Based Rendering)
  - Renderizado de skybox

#### **GLEW (OpenGL Extension Wrangler Library)**
- **Descripción**: Librería que simplifica el acceso a extensiones de OpenGL.
- **Uso**: Inicialización de funciones de OpenGL y acceso a extensiones
- **Instalación**:
  - **macOS**: `brew install glew`
  - **Linux (Ubuntu/Debian)**: `sudo apt-get install libglew-dev`
  - **Linux (Fedora)**: `sudo dnf install glew-devel`

#### **GLFW (Graphics Library Framework)**
- **Descripción**: Librería multiplataforma para crear ventanas, contextos OpenGL y manejar entrada de usuario.
- **Uso**: 
  - Creación y gestión de ventanas
  - Manejo de eventos de teclado y mouse
  - Gestión del contexto OpenGL
- **Instalación**:
  - **macOS**: `brew install glfw`
  - **Linux (Ubuntu/Debian)**: `sudo apt-get install libglfw3-dev`
  - **Linux (Fedora)**: `sudo dnf install glfw-devel`

#### **GLM (OpenGL Mathematics)**
- **Descripción**: Librería de matemáticas para gráficos, proporciona tipos y funciones matemáticas para OpenGL.
- **Uso**: 
  - Vectores (vec2, vec3, vec4)
  - Matrices (mat4)
  - Transformaciones (translate, rotate, scale)
  - Operaciones matemáticas para gráficos 3D
- **Características**:
  - Sintaxis similar a GLSL
  - Optimizaciones para gráficos
  - Extensiones experimentales habilitadas (`GLM_ENABLE_EXPERIMENTAL`)

### Carga de Modelos 3D

#### **Assimp (Open Asset Import Library)**
- **Descripción**: Librería para importar modelos 3D en varios formatos (FBX, OBJ, DAE, etc.).
- **Uso**: Carga de modelos 3D desde archivos FBX
- **Características**:
  - Soporte para múltiples formatos de modelos
  - Carga de mallas, materiales y texturas
  - Procesamiento de escenas 3D complejas
- **Instalación**:
  - **macOS**: `brew install assimp`
  - **Linux (Ubuntu/Debian)**: `sudo apt-get install libassimp-dev`
  - **Linux (Fedora)**: `sudo dnf install assimp-devel`

### Interfaz de Usuario

#### **RmlUi**
- **Descripción**: Librería de interfaz de usuario basada en HTML/CSS, diseñada para juegos.
- **Ubicación**: `GLS/lib/librmlui.a` y `GLS/lib/librmlui_debugger.a`
- **Uso**: 
  - Menús del juego (principal, pausa, game over)
  - Sistema de UI modular con plantillas RML
- **Características**:
  - Sintaxis similar a HTML/CSS
  - Sistema de eventos con callbacks
  - Debugger integrado para desarrollo
  - Backend OpenGL3 personalizado
- **Archivos UI**: `ui/*.rml`
- **Documentación**: [RmlUi GitHub](https://github.com/mikke89/RmlUi)

#### **FreeType**
- **Descripción**: Librería para renderizar fuentes TrueType y otros formatos de fuentes.
- **Uso**: Renderizado de texto en la interfaz de usuario
- **Instalación**:
  - **macOS**: `brew install freetype`
  - **Linux (Ubuntu/Debian)**: `sudo apt-get install libfreetype6-dev`
  - **Linux (Fedora)**: `sudo dnf install freetype-devel`

### Audio

#### **miniaudio**
- **Descripción**: Librería de audio de un solo archivo, multiplataforma y de código abierto.
- **Ubicación**: `build/third_party/miniaudio/miniaudio.h`
- **Uso**: Reproducción de música de fondo y efectos de sonido
- **Características**:
  - Soporte para múltiples formatos de audio (MP3, WAV, OGG, etc.)
  - Reproducción de audio 3D espacial
  - Gestión de audio players y listeners
- **Documentación**: [miniaudio GitHub](https://github.com/mackron/miniaudio)

### Utilidades

#### **STB (Sean T. Barrett's Libraries)**
- **Descripción**: Colección de librerías de un solo archivo header para diversas tareas.
- **Ubicación**: `build/third_party/stb/`
- **Componentes utilizados**:
  - `stb_image.h`: Carga de imágenes (texturas)
  - `stb_image_write.h`: Escritura de imágenes
  - Otras utilidades según necesidad

#### **BSlogger**
- **Descripción**: Librería de logging simple y eficiente.
- **Ubicación**: `build/third_party/BSlogger/`
- **Uso**: Sistema de logging para depuración

## 📦 Dependencias del Sistema

### Requisitos Previos

#### macOS
- **Homebrew** (gestor de paquetes): Si no lo tienes instalado, ejecuta:
  ```bash
  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
  ```
- **Xcode Command Line Tools**:
  ```bash
  xcode-select --install
  ```

#### Linux
- **Herramientas de compilación**: `gcc`, `g++`, `make`
- **CMake**: Versión 3.20 o superior

### Instalación de Dependencias

#### macOS (usando Homebrew)

```bash
# Actualizar Homebrew
brew update

# Gráficos y ventanas
brew install glew
brew install glfw
brew install freetype

# Carga de modelos 3D
brew install assimp

# Herramientas de compilación
brew install cmake
```

#### Linux (Ubuntu/Debian)

```bash
# Actualizar lista de paquetes
sudo apt-get update

# Instalar herramientas de compilación
sudo apt-get install build-essential cmake

# Instalar dependencias del proyecto
sudo apt-get install libglew-dev \
                     libglfw3-dev \
                     libfreetype6-dev \
                     libassimp-dev \
                     libgl1-mesa-dev \
                     libxrandr-dev \
                     libxi-dev
```

#### Linux (Fedora/RHEL/CentOS)

```bash
# Instalar herramientas de compilación
sudo dnf install gcc gcc-c++ make cmake

# Instalar dependencias del proyecto
sudo dnf install glew-devel \
                 glfw-devel \
                 freetype-devel \
                 assimp-devel \
                 mesa-libGL-devel \
                 libXrandr-devel \
                 libXi-devel
```

#### Linux (Arch Linux)

```bash
# Instalar herramientas de compilación
sudo pacman -S base-devel cmake

# Instalar dependencias del proyecto
sudo pacman -S glew \
               glfw \
               freetype2 \
               assimp \
               mesa \
               libxrandr \
               libxi
```

### Verificación de Instalación

Para verificar que todas las dependencias están instaladas correctamente:

```bash
# Verificar CMake
cmake --version

# Verificar compilador (macOS)
g++ --version

# Verificar compilador (Linux)
g++ --version
```

## 🏗️ Estructura del Proyecto

```
shooter/
├── assets/              # Recursos del juego
│   ├── audios/         # Archivos de audio
│   ├── models/         # Modelos 3D (FBX)
│   └── textures/       # Texturas y skybox
├── build/              # Directorio de compilación
├── GLS/                # Motor de juego personalizado
│   ├── include/        # Headers del motor
│   └── lib/           # Librerías compiladas
├── include/           # Headers del juego
├── src/               # Código fuente del juego
├── ui/                # Archivos de interfaz RML
├── shaders/           # Shaders GLSL
├── CMakeLists.txt     # Configuración de CMake
└── main.cpp           # Punto de entrada
```

## 🔨 Compilación

### Requisitos Previos

Asegúrate de tener todas las dependencias instaladas (ver sección [Dependencias del Sistema](#-dependencias-del-sistema)).

### Compilación en macOS

#### Usando el script de build

```bash
chmod +x build.sh
./build.sh
```

#### Manualmente con CMake

```bash
mkdir -p build
cd build
cmake ..
make -j$(sysctl -n hw.ncpu)  # Usa todos los núcleos disponibles
```

### Compilación en Linux

#### Usando el script de build

```bash
chmod +x build.sh
./build.sh
```

#### Manualmente con CMake

```bash
mkdir -p build
cd build
cmake ..
make -j$(nproc)  # Usa todos los núcleos disponibles
```

### Ejecutar el juego

Una vez compilado, ejecuta el juego desde el directorio `build`:

```bash
cd build
./Game
```

### Solución de Problemas de Compilación

#### macOS
- Si encuentras errores relacionados con OpenGL, asegúrate de tener Xcode Command Line Tools instalados
- Si CMake no encuentra las librerías, verifica que Homebrew esté en tu PATH:
  ```bash
  echo 'export PATH="/opt/homebrew/bin:$PATH"' >> ~/.zshrc
  source ~/.zshrc
  ```

#### Linux
- Si faltan headers de OpenGL, instala: `sudo apt-get install libgl1-mesa-dev` (Ubuntu/Debian)
- Si CMake no encuentra las librerías, instala los paquetes `-dev` correspondientes
- Para distribuciones basadas en RPM, asegúrate de tener los repositorios EPEL habilitados si es necesario

## 🎮 Características del Juego

- Sistema de física realista con Jolt Physics
- Renderizado 3D con OpenGL y shaders personalizados
- Sistema de UI modular con RmlUi
- Audio espacial con miniaudio
- Sistema de componentes para objetos del juego
- Carga de modelos 3D con Assimp
- Sistema de colisiones y callbacks
- Menús interactivos (principal, pausa, game over)

## 📝 Notas de Desarrollo

- El proyecto requiere C++17
- Las librerías GLS, Jolt y RmlUi están precompiladas en `GLS/lib/`
- Los archivos de UI usan sintaxis RML (similar a HTML/CSS)
- Los shaders están en `shaders/` y se cargan en tiempo de ejecución
- El sistema de audio soporta formatos MP3, WAV y otros

## 📄 Licencias

- **Jolt Physics**: MIT License
- **RmlUi**: MIT License
- **GLM**: MIT License
- **miniaudio**: Public Domain / MIT-0
- **STB**: Public Domain
- **GLS**: Propietario (motor personalizado)

## 🤝 Contribuciones

Este es un proyecto educativo/demostrativo. Las contribuciones son bienvenidas.

## 📧 Contacto

Para preguntas o problemas, por favor abre un issue en el repositorio.

