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

    if (scene->mNumMeshes != 1)
    {
        throw std::runtime_error("El modelo debe tener un solo objeto");
    }

    // Cargar texturas
    load_textures(scene, model_path);
    
    std::string mesh_name = model_path.stem().string();
    process_mesh(scene->mMeshes[0], mesh_name);
}

void Model::process_mesh(const aiMesh *mesh, const std::string& model_name)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

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

    this->mesh = Mesh::create(vertices, indices);
    this->mesh->set_name(model_name);
}

void Model::render() noexcept
{
    this->transforms();

    auto shader = context->get_shader_list(0);

    shader->use();

    glUniformMatrix4fv(shader->get_uniform_projection_id(), 1, GL_FALSE, glm::value_ptr(context->get_projection()));
    glUniformMatrix4fv(shader->get_uniform_model_id(), 1, GL_FALSE, glm::value_ptr(model_matrix));
    
    // Configurar textura
    glUniform1i(shader->get_uniform_diffuse_texture_id(), 0); // Usar textura en slot 0
    
    // Activar texturas si existen
    if (!textures.empty())
    {
        textures[0]->bind(0); // Usar la primera textura en el slot 0
    }
    
    mesh->render();
    
    // Desactivar texturas
    if (!textures.empty())
    {
        textures[0]->unbind();
    }
}

void Model::clear() noexcept
{
}

void Model::transforms() noexcept
{
    model_matrix = glm::mat4(1.f);

    model_matrix = glm::translate(model_matrix, m_translate);
    model_matrix = glm::rotate(model_matrix, m_rotate_angle, m_rotate_axis);
    model_matrix = glm::scale(model_matrix, m_scale);
}

void Model::rotate(glm::vec3 ax, float angle) noexcept
{
    m_rotate_angle = Utils::to_radian(angle);
    m_rotate_axis = ax;
}

void Model::rotate(Model::AXIS ax, float angle) noexcept
{
    if(ax == Model::X)
    {
        m_rotate_angle = Utils::to_radian(angle);
        m_rotate_axis = glm::vec3(1.f, 0.f, 0.f);
    }

    else if(ax == Model::Y)
    {
        m_rotate_angle = Utils::to_radian(angle);
        m_rotate_axis = glm::vec3(0.f, 1.f, 0.f);
    }

    else if(ax == Model::Z)
    {
        m_rotate_angle = Utils::to_radian(angle);
        m_rotate_axis = glm::vec3(0.f, 0.f, 1.f);
    }
    
}

void Model::translate(float x = 0.f, float y = 0.f, float z = 0.f) noexcept
{
    m_translate = glm::vec3(x, y, z);
  
}

void Model::scale(float x = 0.f, float y = 0.f, float z = 0.f) noexcept
{
    m_scale = glm::vec3(x, y, z);
}
void Model::setContext(std::shared_ptr<Context> context) noexcept
{
    this->context = context;
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