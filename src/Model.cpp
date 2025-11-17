#include "Model.hpp"
#include "Utils.hpp"
#include <stb_image.h>

Model::Model(const std::string &model_name)
{

    std::filesystem::path model_path{std::filesystem::path{__FILE__}.parent_path().parent_path() / "models" / model_name};

    if (!std::filesystem::exists(model_path))
    {
        throw std::runtime_error("Model " + model_name + " not found");
    }

    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(model_path.string(),
                                             aiProcess_Triangulate |
                                                 aiProcess_FlipUVs |
                                                 aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error("Error de Assimp al importar el modelo: " + std::string(importer.GetErrorString()));
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
    //     aiMesh* mesh = scene->mMeshes[i];
    //     // Procesar los vértices, normales y coordenadas de textura
    //     // Aquí es donde copias los datos a VBO y VAO de OpenGL.
    // }

    // if (scene->mNumMeshes != 1)
    //{
    //     throw std::runtime_error("El modelo debe tener un solo objeto");
    // }

    // Cargar texturas
    load_textures(scene, model_path);

    // std::string mesh_name = model_path.stem().string();
    // process_mesh(scene->mMeshes[0], ,mesh_name);
    processNode(scene->mRootNode, scene, model_path);
}

std::shared_ptr<Mesh> Model::process_mesh(const aiMesh *mesh, const aiScene *scene, const std::filesystem::path &model_path)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::shared_ptr<Texture>> textures;

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
        else
        {
            // Inicializar coordenadas de textura por defecto si no existen
            vertex.texCoords.x = 0.0f;
            vertex.texCoords.y = 0.0f;
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

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        loadPBRTextures(material, scene, model_path, textures);
    }

    // Si el mesh no tiene texturas, usar las texturas globales o crear una por defecto
    if (textures.empty())
    {
        if (!this->textures.empty())
        {
            textures = this->textures;
        }
        else
        {
            create_default_texture();
            textures = this->textures;
        }
    }

    auto new_mesh = Mesh::create(vertices, indices, textures);
    new_mesh->set_name(model_path.stem().string() + "_mesh" + std::to_string(meshes.size()));
    return new_mesh;
}

void Model::render(GLuint texture_id) noexcept
{
    // Asegurarse de que siempre haya una textura por defecto disponible
    if (textures.empty() || !textures[0] || textures[0]->get_id() == 0)
    {
        create_default_texture();
    }
    std::shared_ptr<Texture> defaultTexture = textures[0];

    for (size_t i = 0; i < meshes.size(); i++)
    {
        auto &mesh = meshes[i];
        if (!mesh)
            continue;

        // Buscar textura difusa para este mesh, usar la por defecto si no se encuentra
        std::shared_ptr<Texture> textureToUse = findDiffuseTexture(mesh);
        if (!textureToUse || textureToUse->get_id() == 0)
        {
            textureToUse = defaultTexture;
        }
        
        // Vincular la textura y establecer el uniform
        if (textureToUse && textureToUse->get_id() != 0)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureToUse->get_id());
            if (texture_id != static_cast<GLuint>(-1))
            {
                glUniform1i(texture_id, 0);
            }
        }

        mesh->render();
    }

    glActiveTexture(GL_TEXTURE0);
}

void Model::clear() noexcept
{
}

void Model::load_textures(const aiScene *scene, const std::filesystem::path &model_path) noexcept
{
    std::cout << "Analizando modelo: " << model_path.filename() << std::endl;
    std::cout << "Número de materiales: " << scene->mNumMaterials << std::endl;
    std::cout << "Número de texturas embebidas: " << scene->mNumTextures << std::endl;

    // Procesar materiales para cargar texturas embebidas
    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial *material = scene->mMaterials[i];
        unsigned int diffuse_count = material->GetTextureCount(aiTextureType_DIFFUSE);
        std::cout << "Material " << i << " - Texturas difusas referenciadas en el FBX: " << diffuse_count << std::endl;

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
                        aiTexture *embedded_texture = scene->mTextures[texture_index];
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
                        const aiTexture *namedEmbedded = scene->GetEmbeddedTexture(str.C_Str());
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
                        const aiTexture *embedded_texture = scene->mTextures[0];
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
                        auto texture = Texture::create_from_file(texture_path, "", texture_path.string());
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

    // Si no se cargaron texturas embebidas, intentar buscar en la carpeta del modelo
    if (textures.empty())
    {
        std::cout << "  No se encontraron texturas referenciadas en el material del FBX, buscando en archivos..." << std::endl;
        std::filesystem::path model_directory = model_path.parent_path();
        std::vector<std::filesystem::path> search_dirs;
        std::filesystem::path textures_dir = model_directory / "textures";
        
        if (std::filesystem::exists(textures_dir))
        {
            search_dirs.push_back(textures_dir);
        }
        search_dirs.push_back(model_directory); // También buscar en la misma carpeta del modelo
        
        // Buscar texturas por patrón común
        std::vector<std::pair<std::string, std::string>> patterns = {
            {"_diffuse", "texture_diffuse"},
            {"_albedo", "texture_albedo"},
            {"texture_diffuse", "texture_diffuse"}
        };
        
        bool found_texture = false;
        for (const auto& search_dir : search_dirs)
        {
            if (found_texture) break;
            
            for (const auto& entry : std::filesystem::directory_iterator(search_dir))
            {
                if (entry.is_regular_file())
                {
                    std::string entryName = entry.path().filename().string();
                    std::string entryNameLower = entryName;
                    std::transform(entryNameLower.begin(), entryNameLower.end(), entryNameLower.begin(), ::tolower);
                    
                    // Buscar cualquier archivo de imagen que pueda ser una textura
                    std::string ext = entry.path().extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                    
                    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga")
                    {
                        // Verificar si coincide con algún patrón
                        for (const auto& pattern : patterns)
                        {
                            std::string patternLower = pattern.first;
                            std::transform(patternLower.begin(), patternLower.end(), patternLower.begin(), ::tolower);
                            
                            if (entryNameLower.find(patternLower) != std::string::npos)
                            {
                                auto texture = Texture::create_from_file(entry.path(), pattern.second, entry.path().string());
                                if (texture)
                                {
                                    textures.push_back(texture);
                                    std::cout << "  ✓ Textura encontrada y cargada desde archivo: " << entry.path().filename() << std::endl;
                                    found_texture = true;
                                    break;
                                }
                            }
                        }
                        if (found_texture) break;
                    }
                }
            }
        }
        
        // Si aún no se encontraron texturas, crear una textura por defecto
        if (textures.empty())
        {
            std::cout << "  ✗ No se encontraron archivos de textura, usando textura por defecto" << std::endl;
            create_default_texture();
        }
        else
        {
            std::cout << "  ✓ Texturas cargadas exitosamente desde archivos (no estaban embebidas en el FBX)" << std::endl;
        }
    }
    else
    {
        std::cout << "  ✓ Texturas embebidas cargadas exitosamente desde el FBX" << std::endl;
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

    // Crear una textura de 1x1 píxeles gris por defecto (RGBA)
    unsigned char data[4] = {128, 128, 128, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    texture->width = 1;
    texture->height = 1;
    texture->channels = 4;

    glBindTexture(GL_TEXTURE_2D, 0);

    texture->set_type("texture_diffuse");
    textures.push_back(texture);
}

std::shared_ptr<Texture> Model::create_texture_from_embedded_data(const aiTexture *embedded_texture) noexcept
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
    unsigned char *data = stbi_load_from_memory(
        reinterpret_cast<const unsigned char *>(embedded_texture->pcData),
        embedded_texture->mWidth,
        &width, &height, &channels, 0);

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

std::shared_ptr<Texture> Model::create_texture_from_uncompressed_data(const aiTexture *embedded_texture) noexcept
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

std::shared_ptr<Texture> Model::findDiffuseTexture(const std::shared_ptr<Mesh> &mesh) const noexcept
{
    if (!mesh)
        return nullptr;

    const auto &meshTextures = mesh->get_textures();

    // Buscar textura difusa en las texturas del mesh
    for (const auto &tex : meshTextures)
    {
         if (tex && (tex->get_type() == "texture_diffuse" || tex->get_type() == "texture_albedo"))
        {
        return tex;
        }
    }

    // Si no se encontró, usar la primera textura disponible
    if (!meshTextures.empty() && meshTextures[0])
    {
        return meshTextures[0];
    }

    // Si aún no hay, usar texturas globales
    if (!textures.empty() && textures[0])
    {
        return textures[0];
    }

    return nullptr;
}

void Model::processNode(aiNode *node, const aiScene *scene, const std::filesystem::path &model_path)
{
    // Procesar todos los meshes del nodo actual
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        auto processedMesh = process_mesh(mesh, scene, model_path);
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

void Model::loadPBRTextures(aiMaterial* material, const aiScene* scene, const std::filesystem::path& model_path, std::vector<std::shared_ptr<Texture>>& textures) noexcept
{
    // Cargar texturas PBR según el plan
    std::vector<std::shared_ptr<Texture>> albedoMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_albedo", model_path, scene);
    textures.insert(textures.end(), albedoMaps.begin(), albedoMaps.end());
    
    std::vector<std::shared_ptr<Texture>> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", model_path, scene);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    
    std::vector<std::shared_ptr<Texture>> metallicMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_metallic", model_path, scene);
    textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
    
    std::vector<std::shared_ptr<Texture>> roughnessMaps = loadMaterialTextures(material, aiTextureType_SHININESS, "texture_roughness", model_path, scene);
    textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
    
    // Nota: Assimp no tiene aiTextureType_AMBIENT_OCCLUSION directamente, usamos AMBIENT como aproximación
    std::vector<std::shared_ptr<Texture>> aoMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ao", model_path, scene);
    textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
    
    // Si no se encontraron texturas PBR en el material, buscar automáticamente
    // Primero en la subcarpeta textures/, luego en la misma carpeta del modelo
    std::vector<std::filesystem::path> search_dirs;
    std::filesystem::path textures_dir = model_path.parent_path() / "textures";
    std::filesystem::path model_dir = model_path.parent_path();
    
    if (std::filesystem::exists(textures_dir))
    {
        search_dirs.push_back(textures_dir);
    }
    // También buscar en la misma carpeta del modelo
    if (std::filesystem::exists(model_dir))
    {
        search_dirs.push_back(model_dir);
    }
    
    if (!search_dirs.empty())
    {
        // Buscar texturas PBR por patrón si no se encontraron en el material
        std::vector<std::pair<std::string, std::string>> patterns = {
            {"_albedo", "texture_albedo"},
            {"_normal", "texture_normal"},
            {"_metallic", "texture_metallic"},
            {"_roughness", "texture_roughness"},
            {"_ao", "texture_ao"},
            {"_diffuse", "texture_diffuse"}  // También buscar texturas difusas
        };
        
        for (const auto& pattern : patterns)
        {
            // Verificar si ya tenemos una textura de este tipo
            bool hasType = false;
            for (const auto& tex : textures)
            {
                if (tex && tex->get_type() == pattern.second)
                {
                    hasType = true;
                    break;
                }
            }
            
            // Si no tenemos esta textura, buscarla por patrón en todas las carpetas
            if (!hasType)
            {
                for (const auto& search_dir : search_dirs)
                {
                    bool found = false;
                    for (const auto& entry : std::filesystem::directory_iterator(search_dir))
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
                                auto texture = Texture::create_from_file(entry.path(), pattern.second, entry.path().string());
                                if (texture)
                                {
                                    textures.push_back(texture);
                                    found = true;
                                    break; // Solo tomar la primera que coincida
                                }
                            }
                        }
                    }
                    if (found) break; // Si encontramos en una carpeta, no buscar en las demás
                }
            }
        }
    }
    
    // Si no se encontraron texturas PBR, intentar cargar texturas difusas estándar
    if (textures.empty())
    {
        std::vector<std::shared_ptr<Texture>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", model_path, scene);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    }
}


std::vector<std::shared_ptr<Texture>> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, const std::filesystem::path& model_path, const aiScene* scene)
{
    std::vector<std::shared_ptr<Texture>> textures;
    
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
            std::string texturePath = str.C_Str();
            
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
                        texture->set_type(typeName);
                        texture->set_path(texturePath);
                        textures.push_back(texture);
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
                        texture->set_type(typeName);
                        texture->set_path(texturePath);
                        textures.push_back(texture);
                        continue;
                    }
                }
                
                // Intentar cargar desde archivo
                // Primero intentar la ruta directa
                if (std::filesystem::exists(texture_path))
                {
                    auto texture = Texture::create_from_file(texture_path, typeName, texturePath);
                    if (texture)
                    {
                        textures.push_back(texture);
                        continue;
                    }
                }
                
                // Si no existe, intentar buscar en subcarpeta "textures" y luego en la carpeta del modelo
                std::vector<std::filesystem::path> search_dirs;
                std::filesystem::path textures_dir = model_directory / "textures";
                
                // Primero buscar en subcarpeta textures, luego en la carpeta del modelo
                if (std::filesystem::exists(textures_dir))
                {
                    search_dirs.push_back(textures_dir);
                }
                search_dirs.push_back(model_directory); // También buscar en la misma carpeta del modelo
                
                bool texture_found = false;
                for (const auto& search_dir : search_dirs)
                {
                    if (texture_found) break; // Si ya encontramos la textura, no buscar más
                    
                    // Buscar archivo con el nombre exacto
                    std::filesystem::path texture_path_in_dir = search_dir / str.C_Str();
                    if (std::filesystem::exists(texture_path_in_dir))
                    {
                        auto texture = Texture::create_from_file(texture_path_in_dir, typeName, texturePath);
                        if (texture)
                        {
                            textures.push_back(texture);
                            texture_found = true;
                            break; // Salir del bucle de carpetas
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
                        searchPattern = "_ao";
                    else if (typeName == "texture_diffuse")
                        searchPattern = "_diffuse"; // También buscar texturas difusas
                    
                    if (!searchPattern.empty() && !texture_found)
                    {
                        // Buscar archivos que contengan el patrón
                        for (const auto& entry : std::filesystem::directory_iterator(search_dir))
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
                                    auto texture = Texture::create_from_file(entry.path(), typeName, entry.path().string());
                                    if (texture)
                                    {
                                        textures.push_back(texture);
                                        texture_found = true;
                                        break;
                                    }
                                }
                            }
                        }
                        if (texture_found) break; // Si encontramos en esta carpeta, no buscar en las demás
                    }
                }
            }
        }
    }
    
    return textures;
}
