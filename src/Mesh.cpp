#include <Mesh.hpp>
#include <Shader.hpp>
#include <Texture.hpp>

std::shared_ptr<Mesh> Mesh::create(const std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) noexcept
{
    return create(vertices, indices, std::vector<MeshTexture>());
}

std::shared_ptr<Mesh> Mesh::create(const std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const std::vector<MeshTexture>& textures) noexcept
{
    auto mesh = std::make_shared<Mesh>();

    mesh->index_count = indices.size();
    mesh->textures = textures;

    glGenVertexArrays(1, &mesh->VAO_id);
    glBindVertexArray(mesh->VAO_id);

    glGenBuffers(1, &mesh->IBO_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->IBO_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    //vértices
    glGenBuffers(1, &mesh->VBO_id);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Configura los atributos de los vértices (posiciones, normales, texturas)
    // Posiciones
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Normales
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // Texturas
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    // Es seguro desbindear el VBO, pero mantener el EBO ligado al VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return mesh;
}

Mesh::~Mesh()
{
    clear();
}

void Mesh::render() const noexcept
{
    glBindVertexArray(VAO_id);
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::render(std::shared_ptr<Shader> shader) const noexcept
{
    if (!shader) {
        render(); // Fallback al método básico
        return;
    }

    // Crear texturas por defecto si no hay texturas cargadas
    static GLuint defaultWhiteTexture = 0;
    static GLuint defaultNormalTexture = 0;
    static GLuint defaultBlackTexture = 0;
    
    if (defaultWhiteTexture == 0)
    {
        // Textura blanca para albedo
        glGenTextures(1, &defaultWhiteTexture);
        glBindTexture(GL_TEXTURE_2D, defaultWhiteTexture);
        unsigned char white[4] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        // Textura normal por defecto (128, 128, 255) = (0.5, 0.5, 1.0) en espacio normalizado
        glGenTextures(1, &defaultNormalTexture);
        glBindTexture(GL_TEXTURE_2D, defaultNormalTexture);
        unsigned char normal[4] = {128, 128, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, normal);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        // Textura negra para metallic/roughness (0 = no metálico, 0.5 = roughness medio)
        glGenTextures(1, &defaultBlackTexture);
        glBindTexture(GL_TEXTURE_2D, defaultBlackTexture);
        unsigned char black[4] = {0, 128, 0, 255}; // R=0 (metallic), G=128 (roughness 0.5), B=0, A=255
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, black);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    unsigned int albedoNr = 1;
    unsigned int normalNr = 1;
    unsigned int metallicNr = 1;
    unsigned int roughnessNr = 1;
    unsigned int aoNr = 1;
    
    // Mapeo de qué texturas se han encontrado
    bool hasAlbedo = false;
    bool hasNormal = false;
    bool hasMetallic = false;
    bool hasRoughness = false;
    bool hasAO = false;

    // Activar y vincular las texturas PBR cargadas
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        if (!textures[i].texture) continue;

        glActiveTexture(GL_TEXTURE0 + i);
        
        std::string number;
        std::string name = textures[i].type;
        
        if (name == "texture_albedo")
        {
            number = std::to_string(albedoNr++);
            hasAlbedo = true;
        }
        else if (name == "texture_normal")
        {
            number = std::to_string(normalNr++);
            hasNormal = true;
        }
        else if (name == "texture_metallic")
        {
            number = std::to_string(metallicNr++);
            hasMetallic = true;
        }
        else if (name == "texture_roughness")
        {
            number = std::to_string(roughnessNr++);
            hasRoughness = true;
        }
        else if (name == "texture_ao")
        {
            number = std::to_string(aoNr++);
            hasAO = true;
        }
        else
        {
            number = std::to_string(i + 1); // Fallback
        }

        // Usar shader.setInt() como en el plan
        std::string uniformName = name + number;
        shader->setInt(uniformName, i);
        
        // También intentar sin número para compatibilidad
        if (number != "1")
        {
            shader->setInt(name, i);
        }
        
        // Vincular la textura usando el método bind
        textures[i].texture->bind(i);
    }
    
    // Vincular texturas por defecto para los samplers que no tienen textura
    unsigned int textureSlot = textures.size();
    
    if (!hasAlbedo)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, defaultWhiteTexture);
        shader->setInt("texture_albedo", textureSlot);
        shader->setInt("texture_albedo1", textureSlot);
        textureSlot++;
    }
    
    if (!hasMetallic)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, defaultBlackTexture);
        shader->setInt("texture_metallic", textureSlot);
        shader->setInt("texture_metallic1", textureSlot);
        textureSlot++;
    }
    
    if (!hasRoughness)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, defaultBlackTexture);
        shader->setInt("texture_roughness", textureSlot);
        shader->setInt("texture_roughness1", textureSlot);
        textureSlot++;
    }
    
    if (!hasAO)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, defaultWhiteTexture);
        shader->setInt("texture_ao", textureSlot);
        shader->setInt("texture_ao1", textureSlot);
        textureSlot++;
    }

    // Renderizar la malla
    glBindVertexArray(VAO_id);
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Resetear el estado de texturas
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::clear() noexcept
{
    if (IBO_id != 0)
    {
        glDeleteBuffers(1, &IBO_id);
        IBO_id = 0;
    }

    if (VBO_id != 0)
    {
        glDeleteBuffers(1, &VBO_id);
        VBO_id = 0;
    }

    if (VAO_id != 0)
    {
        glDeleteVertexArrays(1, &VAO_id);
        VAO_id = 0;
    }
}
