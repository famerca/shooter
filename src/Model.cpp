#include "Model.hpp"
#include "Utils.hpp"
#include <stb_image.h>
#include <algorithm>
#include <cctype>

Model::Model(const std::string &model_name)
{
    std::filesystem::path model_path{std::filesystem::path{__FILE__}.parent_path().parent_path() / "models" / model_name};
    loadModel(model_path);
}

void Model::loadModel(const std::filesystem::path& path)
{
    if (!std::filesystem::exists(path))
    {
        throw std::runtime_error("Model " + path.string() + " not found");
    }

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.string(),
                                             aiProcess_Triangulate |
                                                 aiProcess_FlipUVs |
                                                 aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error("Error de Assimp al importar el modelo: " + std::string(importer.GetErrorString()));
    }

    std::cout << "Cargando modelo: " << path.filename() << std::endl;
    std::cout << "  Meshes: " << scene->mNumMeshes << ", Materiales: " << scene->mNumMaterials << std::endl;

    // Cargar texturas (método legacy, mantenido para compatibilidad)
    load_textures(scene, path);
    
    // Procesar el nodo raíz y recursivamente todos los nodos hijos
    processNode(scene->mRootNode, scene, path);
    
    std::cout << "  ✓ Modelo cargado: " << meshes.size() << " meshes totales" << std::endl;
}

void Model::processNode(aiNode* node, const aiScene* scene, const std::filesystem::path& model_path)
{
    // Procesar todos los meshes del nodo actual
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        auto processedMesh = processMesh(mesh, scene, model_path);
        if (processedMesh)
        {
            meshes.push_back(processedMesh);
        }
    }
    
    // Procesar recursivamente todos los nodos hijos
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, model_path);
    }
}

std::shared_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene, const std::filesystem::path& model_path)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<MeshTexture> textures;

    // Procesar vértices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        
        if (mesh->HasNormals())
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }
        
        if (mesh->mTextureCoords[0])
        {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }
        
        vertices.push_back(vertex);
    }

    // Procesar índices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Procesar material y texturas PBR
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        
        // Cargar texturas PBR según el plan
        std::vector<MeshTexture> albedoMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_albedo", model_path, scene);
        textures.insert(textures.end(), albedoMaps.begin(), albedoMaps.end());
        
        std::vector<MeshTexture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", model_path, scene);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        
        std::vector<MeshTexture> metallicMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_metallic", model_path, scene);
        textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
        
        std::vector<MeshTexture> roughnessMaps = loadMaterialTextures(material, aiTextureType_SHININESS, "texture_roughness", model_path, scene);
        textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
        
        // Nota: Assimp no tiene aiTextureType_AMBIENT_OCCLUSION directamente, usamos AMBIENT como aproximación
        std::vector<MeshTexture> aoMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ao", model_path, scene);
        textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
        
        // Si no se encontraron texturas PBR, intentar cargar texturas difusas estándar
        if (textures.empty())
        {
            std::vector<MeshTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", model_path, scene);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        }
    }

    // Crear y retornar el mesh
    auto new_mesh = Mesh::create(vertices, indices, textures);
    std::string mesh_name = model_path.stem().string() + "_mesh" + std::to_string(meshes.size());
    new_mesh->set_name(mesh_name);
    
    std::cout << "    Mesh procesado: " << vertices.size() << " vértices, " << indices.size() << " índices, " << textures.size() << " texturas" << std::endl;
    
    return new_mesh;
}

void Model::process_mesh(const aiMesh *mesh, const aiScene* scene, const std::filesystem::path& model_path)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<MeshTexture> textures;

    // Recorre los datos del aiMesh y los copia
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // Copia las posiciones
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        // Copia las normales
        if (mesh->HasNormals())
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }
        // Copia las coordenadas de textura
        if (mesh->mTextureCoords[0])
        {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        }
        vertices.push_back(vertex);
    }

    // Recorre las caras (faces) para obtener los índices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Procesar material y cargar texturas PBR
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::cout << "Procesando material índice: " << mesh->mMaterialIndex << std::endl;
        
        // Cargar texturas PBR
        // Albedo (Diffuse)
        std::cout << "  Buscando texturas albedo..." << std::endl;
        std::vector<MeshTexture> albedoMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_albedo", model_path, scene);
        std::cout << "  Encontradas " << albedoMaps.size() << " texturas albedo" << std::endl;
        textures.insert(textures.end(), albedoMaps.begin(), albedoMaps.end());
        
        // Normal maps (usando HEIGHT como convención común)
        std::cout << "  Buscando texturas normal..." << std::endl;
        std::vector<MeshTexture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", model_path, scene);
        std::cout << "  Encontradas " << normalMaps.size() << " texturas normal" << std::endl;
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        
        // Metallic (usando SPECULAR como aproximación)
        std::cout << "  Buscando texturas metallic..." << std::endl;
        std::vector<MeshTexture> metallicMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_metallic", model_path, scene);
        std::cout << "  Encontradas " << metallicMaps.size() << " texturas metallic" << std::endl;
        textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
        
        // Roughness (usando SHININESS como aproximación)
        std::cout << "  Buscando texturas roughness..." << std::endl;
        std::vector<MeshTexture> roughnessMaps = loadMaterialTextures(material, aiTextureType_SHININESS, "texture_roughness", model_path, scene);
        std::cout << "  Encontradas " << roughnessMaps.size() << " texturas roughness" << std::endl;
        textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
        
        // AO (Ambient Occlusion) - Assimp no tiene un tipo específico, intentamos con AMBIENT
        std::cout << "  Buscando texturas AO..." << std::endl;
        std::vector<MeshTexture> aoMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ao", model_path, scene);
        std::cout << "  Encontradas " << aoMaps.size() << " texturas AO" << std::endl;
        textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
        
        std::cout << "Total de texturas PBR cargadas: " << textures.size() << std::endl;
        
        // Si no se encontraron texturas, intentar buscar todas las texturas PBR en la carpeta textures
        if (textures.empty())
        {
            std::cout << "No se encontraron texturas en materiales, buscando en carpeta textures..." << std::endl;
            std::filesystem::path textures_dir = model_path.parent_path() / "textures";
            if (std::filesystem::exists(textures_dir))
            {
                // Buscar todas las texturas PBR por patrón
                std::vector<std::pair<std::string, std::string>> patterns = {
                    {"_albedo", "texture_albedo"},
                    {"_normal", "texture_normal"},
                    {"_metallic", "texture_metallic"},
                    {"_roughness", "texture_roughness"},
                    {"_AO", "texture_ao"}
                };
                
                for (const auto& pattern : patterns)
                {
                    for (const auto& entry : std::filesystem::directory_iterator(textures_dir))
                    {
                        if (entry.is_regular_file())
                        {
                            std::string entryName = entry.path().filename().string();
                            std::string entryNameLower = entryName;
                            std::transform(entryNameLower.begin(), entryNameLower.end(), entryNameLower.begin(), ::tolower);
                            std::string patternLower = pattern.first;
                            std::transform(patternLower.begin(), patternLower.end(), patternLower.begin(), ::tolower);
                            
                            if (entryNameLower.find(patternLower) != std::string::npos)
                            {
                                std::cout << "  Encontrada textura por patrón: " << entryName << " -> " << pattern.second << std::endl;
                                auto texture = Texture::create_from_file(entry.path());
                                if (texture)
                                {
                                    MeshTexture meshTexture;
                                    meshTexture.type = pattern.second;
                                    meshTexture.path = entry.path().string();
                                    meshTexture.texture = texture;
                                    textures.push_back(meshTexture);
                                    break; // Solo tomar la primera que coincida
                                }
                            }
                        }
                    }
                }
                std::cout << "Total de texturas PBR encontradas por patrón: " << textures.size() << std::endl;
            }
        }
    }
    else
    {
        std::cout << "Advertencia: Mesh sin material asignado" << std::endl;
    }

    // NOTA: Esta función process_mesh ya no se usa.
    // El código ahora procesa los meshes directamente en el constructor de Model.
    // Esta función se mantiene por compatibilidad pero no debería ser llamada.
    // Si se llama, los datos procesados se perderían ya que no hay this->mesh.
    
    // std::string mesh_name = model_path.stem().string();
    // auto new_mesh = Mesh::create(vertices, indices, textures);
    // new_mesh->set_name(mesh_name);
    // meshes.push_back(new_mesh);
}

void Model::render(GLuint texture_id) noexcept
{
    // Renderizar todos los meshes
    for (size_t i = 0; i < meshes.size(); i++)
    {
        auto& mesh = meshes[i];
        if (!mesh) continue;
        
        // Buscar la primera textura difusa disponible (para shader estándar)
        std::shared_ptr<Texture> diffuseTexture = nullptr;
        const auto& meshTextures = mesh->get_textures();
        
        // Buscar textura difusa en las texturas del mesh
        for (const auto& tex : meshTextures)
        {
            if (tex.texture && (tex.type == "texture_diffuse" || tex.type == "texture_albedo"))
            {
                diffuseTexture = tex.texture;
                break;
            }
        }
        
        // Si no se encontró, usar la primera textura disponible
        if (!diffuseTexture && !meshTextures.empty() && meshTextures[0].texture)
        {
            diffuseTexture = meshTextures[0].texture;
        }
        
        // Si aún no hay, usar texturas globales
        if (!diffuseTexture && !textures.empty() && textures[0])
        {
            diffuseTexture = textures[0];
        }
        
        // Configurar y vincular textura
        if (texture_id != 0 && diffuseTexture)
        {
            glActiveTexture(GL_TEXTURE0);
            diffuseTexture->bind(0);
            glUniform1i(texture_id, 0);
        }
        
        mesh->render();
    }
    
    // Resetear estado de texturas
    glActiveTexture(GL_TEXTURE0);
}

void Model::clear() noexcept
{
}

void Model::load_textures(const aiScene* scene, const std::filesystem::path& model_path) noexcept
{
    std::cout << "Analizando modelo: " << model_path.filename() << std::endl;
    std::cout << "Número de materiales: " << scene->mNumMaterials << std::endl;
    std::cout << "Número de texturas embebidas: " << scene->mNumTextures << std::endl;
    
    // Procesar materiales para cargar texturas embebidas
    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial* material = scene->mMaterials[i];
        std::cout << "Material " << i << " - Texturas difusas: " << material->GetTextureCount(aiTextureType_DIFFUSE) << std::endl;
        
        // Buscar texturas difusas embebidas
        for (unsigned int j = 0; j < material->GetTextureCount(aiTextureType_DIFFUSE); j++)
        {
            aiString str;
            aiTextureMapping mapping;
            unsigned int uvindex;
            ai_real blend;
            aiTextureOp op;
            aiTextureMapMode mapmode[2];
            
            if (material->GetTexture(aiTextureType_DIFFUSE, j, &str, &mapping, &uvindex, &blend, &op, mapmode) == AI_SUCCESS)
            {
                // Verificar si la textura está embebida
                if (str.data[0] == '*')
                {
                    // Textura embebida - extraer el índice
                    std::string texture_index_str = str.C_Str() + 1; // Saltar el '*'
                    unsigned int texture_index = std::stoi(texture_index_str);
                    
                    std::cout << "  Textura embebida encontrada: " << str.C_Str() << " (índice: " << texture_index << ")" << std::endl;
                    
                    if (texture_index < scene->mNumTextures)
                    {
                        aiTexture* embedded_texture = scene->mTextures[texture_index];
                        std::cout << "  Textura embebida - Ancho: " << embedded_texture->mWidth 
                                  << ", Alto: " << embedded_texture->mHeight 
                                  << ", Formato: " << embedded_texture->achFormatHint << std::endl;
                        
                        if (embedded_texture->mHeight == 0)
                        {
                            // Textura comprimida (formato común en FBX)
                            std::cout << "Cargando textura embebida comprimida: " << texture_index << std::endl;
                            
                            // Crear textura desde datos comprimidos
                            auto texture = create_texture_from_embedded_data(embedded_texture);
                            if (texture)
                            {
                                textures.push_back(texture);
                                std::cout << "Textura embebida cargada exitosamente" << std::endl;
                            }
                        }
                        else
                        {
                            // Textura no comprimida
                            std::cout << "Cargando textura embebida no comprimida: " << texture_index << std::endl;
                            
                            auto texture = create_texture_from_uncompressed_data(embedded_texture);
                            if (texture)
                            {
                                textures.push_back(texture);
                                std::cout << "Textura embebida cargada exitosamente" << std::endl;
                            }
                        }
                    }
                }
                else
                {
                    // Textura externa - intentar cargar desde archivo o embebida por nombre (.fbm)
                    std::cout << "  Textura externa referenciada: " << str.C_Str() << std::endl;

                    // Intentar obtener textura embebida por nombre (Assimp puede mapear nombres como .fbm)
#if defined(AI_SCENE_H_INC)
                    if (scene)
                    {
                        const aiTexture* namedEmbedded = scene->GetEmbeddedTexture(str.C_Str());
                        if (namedEmbedded != nullptr)
                        {
                            std::cout << "  Encontrada textura embebida por nombre: " << str.C_Str() << std::endl;
                            std::shared_ptr<Texture> texture;
                            if (namedEmbedded->mHeight == 0)
                            {
                                texture = create_texture_from_embedded_data(namedEmbedded);
                            }
                            else
                            {
                                texture = create_texture_from_uncompressed_data(namedEmbedded);
                            }
                            if (texture)
                            {
                                textures.push_back(texture);
                                std::cout << "  Textura embebida (por nombre) cargada exitosamente" << std::endl;
                                continue;
                            }
                        }
                    }
#endif

                    std::filesystem::path model_directory = model_path.parent_path();
                    std::filesystem::path texture_path = model_directory / str.C_Str();

                    std::cout << "  Buscando en: " << texture_path << std::endl;

                    std::string extension = texture_path.extension().string();

                    if (extension == ".fbm" && scene->mNumTextures > 0)
                    {
                        // Fallback: usar la primera textura embebida si existe
                        std::cout << "  Referencia .fbm detectada, intentando usar textura embebida del FBX" << std::endl;
                        const aiTexture* embedded_texture = scene->mTextures[0];
                        std::shared_ptr<Texture> texture;
                        if (embedded_texture->mHeight == 0)
                        {
                            texture = create_texture_from_embedded_data(embedded_texture);
                        }
                        else
                        {
                            texture = create_texture_from_uncompressed_data(embedded_texture);
                        }
                        if (texture)
                        {
                            textures.push_back(texture);
                            std::cout << "  Textura embebida (fallback) cargada exitosamente" << std::endl;
                            continue;
                        }
                    }

                    if (extension == ".jpg" || extension == ".jpeg" || extension == ".png" ||
                        extension == ".bmp" || extension == ".tga" || extension == ".hdr")
                    {
                        auto texture = Texture::create_from_file(texture_path);
                        if (texture)
                        {
                            textures.push_back(texture);
                            std::cout << "  Textura externa cargada exitosamente: " << texture_path << std::endl;
                        }
                        else
                        {
                            std::cout << "  No se pudo cargar la textura externa: " << texture_path << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "  Formato de textura no soportado: " << extension << std::endl;
                    }
                }
            }
        }
    }
    
    // Si no se cargaron texturas, crear una textura por defecto
    if (textures.empty())
    {
        std::cout << "No se encontraron texturas embebidas, usando textura por defecto" << std::endl;
        create_default_texture();
    }
}

void Model::create_default_texture() noexcept
{
    auto texture = std::make_shared<Texture>();
    
    // Generar textura
    glGenTextures(1, &texture->texture_id);
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);
    
    // Configurar parámetros de la textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Crear una textura de 1x1 píxeles con color basado en el nombre del modelo
    unsigned char data[4];
    
    // Color gris por defecto
    data[0] = 128; data[1] = 128; data[2] = 128; data[3] = 255; // RGBA gris
    std::cout << "  Creando textura gris por defecto" << std::endl;
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    texture->width = 1;
    texture->height = 1;
    texture->channels = 4;
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    textures.push_back(texture);
    std::cout << "  Textura por defecto creada" << std::endl;
}

std::shared_ptr<Texture> Model::create_texture_from_embedded_data(const aiTexture* embedded_texture) noexcept
{
    auto texture = std::make_shared<Texture>();
    
    // Generar textura
    glGenTextures(1, &texture->texture_id);
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);
    
    // Configurar parámetros de la textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Los datos están comprimidos, usar stb_image para decodificar
    int width, height, channels;
    unsigned char* data = stbi_load_from_memory(
        reinterpret_cast<const unsigned char*>(embedded_texture->pcData),
        embedded_texture->mWidth,
        &width, &height, &channels, 0
    );
    
    if (data)
    {
        GLenum format = GL_RGB;
        if (channels == 4)
            format = GL_RGBA;
        else if (channels == 1)
            format = GL_RED;
        
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        texture->width = width;
        texture->height = height;
        texture->channels = channels;
        
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Error al decodificar textura embebida comprimida" << std::endl;
        glBindTexture(GL_TEXTURE_2D, 0);
        return nullptr;
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

std::shared_ptr<Texture> Model::create_texture_from_uncompressed_data(const aiTexture* embedded_texture) noexcept
{
    auto texture = std::make_shared<Texture>();
    
    // Generar textura
    glGenTextures(1, &texture->texture_id);
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);
    
    // Configurar parámetros de la textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Los datos están en formato ARGB8888 (formato común de Assimp)
    GLenum format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, embedded_texture->mWidth, embedded_texture->mHeight, 
                 0, format, GL_UNSIGNED_BYTE, embedded_texture->pcData);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    texture->width = embedded_texture->mWidth;
    texture->height = embedded_texture->mHeight;
    texture->channels = 4; // ARGB
    
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

std::vector<MeshTexture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, const std::filesystem::path& model_path, const aiScene* scene)
{
    std::vector<MeshTexture> textures;
    
    unsigned int textureCount = mat->GetTextureCount(type);
    
    for (unsigned int i = 0; i < textureCount; i++)
    {
        aiString str;
        aiTextureMapping mapping;
        unsigned int uvindex;
        ai_real blend;
        aiTextureOp op;
        aiTextureMapMode mapmode[2];
        
        if (mat->GetTexture(type, i, &str, &mapping, &uvindex, &blend, &op, mapmode) == AI_SUCCESS)
        {
            MeshTexture meshTexture;
            meshTexture.type = typeName;
            meshTexture.path = str.C_Str();
            
            // Verificar si la textura está embebida
            if (str.data[0] == '*')
            {
                // Textura embebida - extraer el índice
                std::string texture_index_str = str.C_Str() + 1;
                unsigned int texture_index = std::stoi(texture_index_str);
                
                if (texture_index < scene->mNumTextures)
                {
                    aiTexture* embedded_texture = scene->mTextures[texture_index];
                    std::shared_ptr<Texture> texture;
                    
                    if (embedded_texture->mHeight == 0)
                    {
                        texture = create_texture_from_embedded_data(embedded_texture);
                    }
                    else
                    {
                        texture = create_texture_from_uncompressed_data(embedded_texture);
                    }
                    
                    if (texture)
                    {
                        meshTexture.texture = texture;
                        textures.push_back(meshTexture);
                    }
                }
            }
            else
            {
                // Textura externa
                std::filesystem::path model_directory = model_path.parent_path();
                std::filesystem::path texture_path = model_directory / str.C_Str();
                
                // Intentar obtener textura embebida por nombre
                const aiTexture* namedEmbedded = scene->GetEmbeddedTexture(str.C_Str());
                if (namedEmbedded != nullptr)
                {
                    std::shared_ptr<Texture> texture;
                    if (namedEmbedded->mHeight == 0)
                    {
                        texture = create_texture_from_embedded_data(namedEmbedded);
                    }
                    else
                    {
                        texture = create_texture_from_uncompressed_data(namedEmbedded);
                    }
                    
                    if (texture)
                    {
                        meshTexture.texture = texture;
                        textures.push_back(meshTexture);
                        continue;
                    }
                }
                
                // Intentar cargar desde archivo
                // Primero intentar la ruta directa
                if (std::filesystem::exists(texture_path))
                {
                    auto texture = Texture::create_from_file(texture_path);
                    if (texture)
                    {
                        meshTexture.texture = texture;
                        textures.push_back(meshTexture);
                        continue;
                    }
                }
                
                // Si no existe, intentar buscar en subcarpeta "textures"
                std::filesystem::path textures_dir = model_directory / "textures";
                if (std::filesystem::exists(textures_dir))
                {
                    // Buscar archivo en la carpeta textures
                    std::filesystem::path texture_path_in_textures = textures_dir / str.C_Str();
                    if (std::filesystem::exists(texture_path_in_textures))
                    {
                        auto texture = Texture::create_from_file(texture_path_in_textures);
                        if (texture)
                        {
                            meshTexture.texture = texture;
                            textures.push_back(meshTexture);
                            continue;
                        }
                    }
                    
                    // Si el nombre del archivo no coincide exactamente, intentar buscar por patrón
                    // Por ejemplo, si buscamos "texture_albedo" y el archivo se llama "TVback_albedo.jpg"
                    std::string filename = str.C_Str();
                    std::string searchPattern = "";
                    
                    // Determinar patrón de búsqueda basado en el tipo
                    if (typeName == "texture_albedo")
                        searchPattern = "_albedo";
                    else if (typeName == "texture_normal")
                        searchPattern = "_normal";
                    else if (typeName == "texture_metallic")
                        searchPattern = "_metallic";
                    else if (typeName == "texture_roughness")
                        searchPattern = "_roughness";
                    else if (typeName == "texture_ao")
                        searchPattern = "_AO";
                    
                    if (!searchPattern.empty())
                    {
                        // Buscar archivos que contengan el patrón
                        for (const auto& entry : std::filesystem::directory_iterator(textures_dir))
                        {
                            if (entry.is_regular_file())
                            {
                                std::string entryName = entry.path().filename().string();
                                // Convertir a minúsculas para comparación
                                std::string entryNameLower = entryName;
                                std::transform(entryNameLower.begin(), entryNameLower.end(), entryNameLower.begin(), ::tolower);
                                std::string patternLower = searchPattern;
                                std::transform(patternLower.begin(), patternLower.end(), patternLower.begin(), ::tolower);
                                
                                if (entryNameLower.find(patternLower) != std::string::npos)
                                {
                                    auto texture = Texture::create_from_file(entry.path());
                                    if (texture)
                                    {
                                        meshTexture.texture = texture;
                                        meshTexture.path = entry.path().string();
                                        textures.push_back(meshTexture);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return textures;
}