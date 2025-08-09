#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model {
public:
    // ... constructor, etc.
    Model(string const &path) {
        loadModel(path);
    }
    void Draw(Shader &shader);

private:
    vector<Mesh> meshes;
    string directory;

    void loadModel(string const &path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    // ... para texturas: vector<Texture> loadMaterialTextures(...)
};


// --- En Model.cpp (o similar) ---
void Model::loadModel(string const &path) {
    // 1. Crear una instancia del importador de Assimp
    Assimp::Importer importer;

    // 2. Leer el archivo. Le pasamos unas banderas de post-procesamiento.
    // aiProcess_Triangulate: Si el modelo no está hecho de triángulos, los convierte. ¡Esencial!
    // aiProcess_FlipUVs: Invierte las coordenadas de textura en el eje Y. Necesario para OpenGL.
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    // 3. Comprobar errores
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    // 4. Iniciar el procesamiento recursivo desde el nodo raíz
    processNode(scene->mRootNode, scene);
}

// Procesa un nodo recursivamente. Un nodo contiene índices a las mallas de la escena.
void Model::processNode(aiNode *node, const aiScene *scene) {
    // Procesa todas las mallas del nodo actual
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        // El nodo solo contiene el índice a la malla en el array global de la escena.
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        // La función processMesh devuelve un objeto Mesh construido con los datos.
        meshes.push_back(processMesh(mesh, scene));
    }

    // Llama recursivamente a esta misma función para todos los hijos del nodo
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}