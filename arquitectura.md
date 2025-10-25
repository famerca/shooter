# **Documento de Arquitectura del Motor Gráfico OpenGL**

Esta arquitectura se basa en el patrón de **Arquitectura Basada en Componentes (Component-Based Architecture \- CBA)** para lograr escalabilidad, eficiencia y flexibilidad. La separación de responsabilidades es fundamental: la geometría (Model) no conoce el sombreado (Shader), y el sistema de dibujo (Renderer) es el encargado de orquestar la ejecución.

## **I. Fundamentos del Sistema de Componentes**

### **1\. Clase Base: Component**

Es la clase abstracta de la que derivarán todas las funcionalidades específicas (físicas, renderizado, *scripts*). Cada componente gestiona una parte del comportamiento del GameObject al que está adjunto.

| Atributo | Tipo | Descripción |
| :---- | :---- | :---- |
| m\_owner | GameObject\* | Puntero al GameObject padre al que pertenece este componente. |

| Método | Función |
| :---- | :---- |
| virtual void update(float deltaTime) | Se llama una vez por *frame* para actualizar la lógica del componente. |
| virtual void start() | Se llama una vez al inicio del ciclo de vida del componente (para inicialización). |
| void setOwner(GameObject\* owner) | Vincula el componente a su GameObject padre. |
| GameObject\* getOwner() const | Devuelve el GameObject al que pertenece. |

### **2\. Clase Transformación: TransformComponent**

Encapsula la posición, rotación y escala de un objeto. Su principal responsabilidad es calcular y exponer la Matriz de Modelo, que se utiliza para posicionar el objeto en el mundo 3D. **Deriva de Component**.

| Atributo | Tipo | Descripción |
| :---- | :---- | :---- |
| m\_position | glm::vec3 | Vector de posición. |
| glm::quat m\_rotation | Cuaternión para manejar la rotación. |  |
| glm::vec3 m\_scale | Vector de escala. |  |
| glm::mat4 m\_modelMatrix | Matriz de Modelo (se recalcula bajo demanda). |  |
| m\_isDirty | bool | Flag para optimizar: la matriz solo se recalcula si la posición/rotación/escala ha cambiado. |

| Método | Función |
| :---- | :---- |
| translate(x, y, z) | Traslada la posición. |
| rotate(angle, axis) | Rota el objeto. |
| scale(x, y, z) | Escala el objeto. |
| getModelMatrix() | Devuelve la Matriz de Modelo, asegurando que esté actualizada. |

### **3\. Clase Entidad: GameObject**

El contenedor universal para cualquier objeto en la escena. Gestiona la colección de sus componentes, su transformación esencial y su estado de renderizado básico.

| Atributo | Tipo | Descripción |
| :---- | :---- | :---- |
| m\_transform | std::shared\_ptr\<TransformComponent\> | **Referencia obligatoria** al componente de transformación. |
| m\_components | std::vector\<std::shared\_ptr\<Component\>\> | Lista de funcionalidades adjuntas. |
| m\_isVisible | bool | **Flag de visibilidad.** Si es false, el Renderer salta este objeto para optimización. |

| Método | Función |
| :---- | :---- |
| addComponent\<T\>(Args...) | Crea y adjunta un componente. |
| getComponent\<T\>() | Busca y devuelve un componente del tipo $T$. |
| getTransform() | Devuelve el shared\_ptr al componente de transformación. |
| update(float deltaTime) | Llama a update() en todos los componentes. |
| isVisible() | Devuelve el estado del flag de visibilidad. |
| setVisible(bool visible) | Establece el estado del flag de visibilidad. |

## **II. Componentes Clave y Entidades Gráficas**

### **1\. Clase Contenedora de Geometría: Model**

Clase de alto nivel responsable de encapsular los datos de geometría (VAOs/VBOs) y sus materiales. Esta clase no tiene conocimiento de *shaders* ni de la escena.

| Atributo | Tipo | Descripción |
| :---- | :---- | :---- |
| m\_meshes | std::vector\<Mesh\> | Lista de mallas que componen el modelo (una malla contiene los datos de vértice). |
| m\_textures | std::vector\<Texture\> | Texturas cargadas para los materiales del modelo. |

| Método | Función |
| :---- | :---- |
| loadModel(path) | Carga la geometría y texturas desde un archivo (ej., FBX/OBJ), poblando las mallas. |
| render() | Ejecuta la lógica de OpenGL para dibujar todas las mallas internas. |

### **2\. ModelRendererComponent**

Componente que adjunta el modelo dibujable al GameObject. Su principal función es referenciar el Model y, opcionalmente, indicar si se necesita un *shader* no estándar.

| Atributo | Tipo | Descripción |
| :---- | :---- | :---- |
| m\_model | std::unique\_ptr\<Model\> | Referencia a la geometría y texturas cargadas. |
| m\_shaderName | std::string | Nombre del *shader* especial requerido. **Si está vacío, el Renderer usa el *shader* por defecto.** |

| Método | Función |
| :---- | :---- |
| getModel() | Devuelve el puntero al Model para que el Renderer pueda dibujarlo. |
| getShaderName() | Devuelve el nombre del *shader* deseado. |

### **3\. LightSourceComponent (y sus derivados)**

Base para definir una luz, permitiendo que los objetos de juego actúen como fuentes de iluminación.

| Atributo | Descripción |
| :---- | :---- |
| color | Color de la luz (glm::vec3). |
| intensity | Intensidad general (float). |

| Derivado | Propiedades Adicionales |
| :---- | :---- |
| **DirectionalLightComponent** | Su dirección se obtiene de m\_owner-\>getTransform()-\>getRotation(). |
| **PointLightComponent** | Parámetros de atenuación: constant, linear, quadratic. |

### **4\. ScriptComponent**

Permite adjuntar lógica de juego personalizada (movimiento, IA, etc.).

| Método | Función |
| :---- | :---- |
| update(float deltaTime) | Sobrescribe el método base para implementar la lógica de juego. |

## **III. Sistemas Gráficos y de Perspectiva**

### **1\. Clase Gráfica: Shader**

Encapsula un programa GLSL de OpenGL (Vertex, Fragment, opcionalmente Geometry/Tessellation). Es esencial para el renderizado y el envío de datos a la GPU.

| Atributo | Tipo | Descripción |
| :---- | :---- | :---- |
| m\_programID | GLuint | Identificador del programa OpenGL. |
| m\_uniformsCache | std::unordered\_map\<std::string, GLint\> | Caché de ubicaciones de uniformes para optimizar el rendimiento. |

| Método | Función |
| :---- | :---- |
| Shader(vertexPath, fragmentPath) | Constructor que compila y *linkea* los *shaders*. |
| use() | Activa el *shader* llamando a glUseProgram(). |
| setUniform(name, value) | Método genérico para establecer *uniforms* (e.g., matrices, vectores, floats). |
| getUniformLocation(name) | Busca y cachea la ubicación de un *uniform*. |

### **2\. Clase Gráfica: Camera**

Responsable de generar las matrices de Vista (View) y Proyección (Projection) necesarias para la perspectiva 3D.

| Atributo | Tipo | Descripción |
| :---- | :---- | :---- |
| m\_aspectRatio | float | Relación de aspecto (width/height del viewport). |
| m\_fovy | float | Campo de visión vertical (para Proyección Perspectiva). |
| m\_near, m\_far | float | Planos de recorte (clipping planes). |
| m\_isOrthographic | bool | Determina si usa Proyección Ortográfica o Perspectiva. |

| Método | Función |
| :---- | :---- |
| getViewMatrix() | Calcula y devuelve la Matriz de Vista (basada en la posición y dirección de la cámara). |
| getProjectionMatrix() | Calcula y devuelve la Matriz de Proyección (Perspectiva u Ortográfica). |
| updateProjection(w, h) | Actualiza el m\_aspectRatio y recalcula la matriz de Proyección. |

### **3\. Clase de Sistema: ShadowMapSystem (Gestor de Sombras)**

Este sistema es responsable de la técnica de generación de mapas de profundidad (*depth maps*). **Separa la lógica de renderizado de sombras de la lógica general de dibujado.**

| Atributo | Tipo | Descripción |
| :---- | :---- | :---- |
| m\_fboID | GLuint | Framebuffer Object (FBO) donde se renderiza la profundidad. |
| m\_depthMapTextureID | GLuint | Textura que almacena el mapa de profundidad generado. |
| m\_lightSpaceMatrix | glm::mat4 | Matriz de transformación (Proyección x Vista) calculada desde la perspectiva de la luz. |

| Método | Función |
| :---- | :---- |
| initialize() | Crea el FBO y la textura del mapa de profundidad. |
| startDepthPass(const Scene& scene, const DirectionalLightComponent\* light) | Configura el *viewport*, enlaza el FBO y calcula la m\_lightSpaceMatrix para el paso de sombras. |
| endDepthPass() | Desenlaza el FBO y restaura el *viewport*. |
| getLightSpaceMatrix() | Devuelve la matriz de espacio de luz para enviarla al *shader* principal. |
| bindDepthMapTexture() | Enlaza la textura de profundidad para su muestreo en el *shader* principal. |

### **4\. Renderer (El Bucle de Dibujo Optimizado)**

El sistema de orquestación de dibujo de OpenGL. Utiliza el ShadowMapSystem para obtener el mapa de sombras y el ShaderManager para obtener los programas.

| Atributo | Tipo | Descripción |
| :---- | :---- | :---- |
| m\_defaultShader | std::shared\_ptr\<Shader\> | El *shader* principal y por defecto. |
| m\_depthShader | std::shared\_ptr\<Shader\> | *Shader* simple para generar el mapa de profundidad. |
| m\_shadowSystem | std::unique\_ptr\<ShadowMapSystem\> | Referencia al sistema que gestiona el pase de sombras. |
| m\_shaderManager | ShaderManager\* | Referencia al gestor de *shaders*. |

| Método | Función |
| :---- | :---- |
| initialize() | Carga todos los *shaders* principales y el ShadowMapSystem. |
| render(const Scene& scene) | Ejecuta el flujo de dibujo completo. |
| setGlobalUniforms(Shader\* shader, const Scene& scene) | Pasa la cámara y las luces a un *shader* específico. |

**Flujo de Renderer::render():**

1. **Paso de Sombras:** Llama a m\_shadowSystem-\>startDepthPass(...). Itera sobre los GameObjects, activa m\_depthShader, y dibuja su geometría usando la Matriz de Modelo y la Matriz de Espacio de Luz. Llama a m\_shadowSystem-\>endDepthPass().  
2. **Paso de Renderizado Principal:** Activa m\_defaultShader. Vincula los *uniforms* globales, incluyendo la Matriz de Espacio de Luz de m\_shadowSystem-\>getLightSpaceMatrix() y la textura de profundidad (m\_shadowSystem-\>bindDepthMapTexture()).  
3. **Dibujo:** Itera sobre los objetos visibles y dibuja los modelos.

### **5\. ShaderManager**

Clase central para el manejo de los programas GLSL. Garantiza que los *shaders* se carguen, compilen y se reutilicen de manera eficiente.

| Método | Función |
| :---- | :---- |
| loadShader(name, vertPath, fragPath) | Carga, compila y *linkea* un *shader* y lo guarda en caché. |
| getShader(name) | Devuelve un std::shared\_ptr\<Shader\> previamente cargado. |

## **IV. Sistemas de Gestión Global**

### **1\. Scene (El Mundo)**

El gestor principal del estado del juego y del mundo 3D.

| Atributo | Tipo | Descripción |
| :---- | :---- | :---- |
| m\_gameObjects | std::vector\<std::shared\_ptr\<GameObject\>\> | Lista principal de todos los objetos en la escena. |
| m\_activeCamera | std::shared\_ptr\<Camera\> | La única cámara activa cuyas matrices Vista/Proyección se usan para el renderizado. |
| m\_ambientLight | AmbientLight | El color/intensidad de la luz global constante. |

| Método | Función |
| :---- | :---- |
| createGameObject() | Crea un GameObject (con TransformComponent por defecto) y lo añade a la lista. |
| update(float deltaTime) | Llama a update() en todos los GameObjects. |

### **2\. Integración de Input y UI**

| Sistema | Integración |
| :---- | :---- |
| **Input** | El **InputSystem** gestiona los *callbacks* de GLFW y expone los estados del teclado/ratón, que luego son leídos por los ScriptComponent. |
| **UI** | Un **GUISystem** (como ImGui) gestiona la interfaz de usuario, que se dibuja **después** del ciclo de renderizado 3D para evitar problemas de *depth testing*. |

