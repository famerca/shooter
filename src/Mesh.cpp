#include <Mesh.hpp>
#include <Shader.hpp>
#include <Texture.hpp>

std::shared_ptr<Mesh> Mesh::create(const std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) noexcept
{
    return create(vertices, indices, std::vector<std::shared_ptr<Texture>>());
}

std::shared_ptr<Mesh> Mesh::create(const std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const std::vector<std::shared_ptr<Texture>>& textures) noexcept
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

    // Obtener o inicializar texturas por defecto
    auto& defaults = getDefaultTextures();
    if (defaults.white == 0) {
        initializeDefaultTextures(defaults);
    }

    // Estado de texturas encontradas
    bool hasAlbedo = false;
    bool hasNormal = false;
    bool hasMetallic = false;
    bool hasRoughness = false;
    bool hasAO = false;
    
    // Vincular texturas PBR cargadas
    unsigned int textureSlot = 0;
    bindPBRTextures(shader, textureSlot, hasAlbedo, hasNormal, hasMetallic, hasRoughness, hasAO);
    
    // Vincular texturas por defecto para las que faltan
    bindDefaultTextures(shader, textureSlot, hasAlbedo, hasMetallic, hasRoughness, hasAO, defaults);

    // Renderizar la malla
    glBindVertexArray(VAO_id);
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Resetear el estado de texturas
    glActiveTexture(GL_TEXTURE0);
}

Mesh::DefaultTextures& Mesh::getDefaultTextures() noexcept
{
    static DefaultTextures defaults;
    return defaults;
}

void Mesh::initializeDefaultTextures(DefaultTextures& defaults) noexcept
{
    // Textura blanca para albedo
    glGenTextures(1, &defaults.white);
    glBindTexture(GL_TEXTURE_2D, defaults.white);
    unsigned char white[4] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Textura normal por defecto (128, 128, 255) = (0.5, 0.5, 1.0) en espacio normalizado
    glGenTextures(1, &defaults.normal);
    glBindTexture(GL_TEXTURE_2D, defaults.normal);
    unsigned char normal[4] = {128, 128, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, normal);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Textura negra para metallic/roughness (0 = no metálico, 0.5 = roughness medio)
    glGenTextures(1, &defaults.black);
    glBindTexture(GL_TEXTURE_2D, defaults.black);
    unsigned char black[4] = {0, 128, 0, 255}; // R=0 (metallic), G=128 (roughness 0.5), B=0, A=255
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, black);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Mesh::bindPBRTextures(std::shared_ptr<Shader> shader, unsigned int& textureSlot, bool& hasAlbedo, bool& hasNormal, bool& hasMetallic, bool& hasRoughness, bool& hasAO) const noexcept
{
    unsigned int albedoNr = 1;
    unsigned int normalNr = 1;
    unsigned int metallicNr = 1;
    unsigned int roughnessNr = 1;
    unsigned int aoNr = 1;

    // Activar y vincular las texturas PBR cargadas
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        if (!textures[i]) continue;

        glActiveTexture(GL_TEXTURE0 + i);
        
        std::string number;
        std::string name = textures[i]->get_type();
        
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
        textures[i]->bind(i);
    }
    
    textureSlot = textures.size();
}

void Mesh::bindDefaultTextures(std::shared_ptr<Shader> shader, unsigned int& textureSlot, bool hasAlbedo, bool hasMetallic, bool hasRoughness, bool hasAO, const DefaultTextures& defaults) const noexcept
{
    // Vincular texturas por defecto para los samplers que no tienen textura
    if (!hasAlbedo)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, defaults.white);
        shader->setInt("texture_albedo", textureSlot);
        shader->setInt("texture_albedo1", textureSlot);
        textureSlot++;
    }
    
    if (!hasMetallic)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, defaults.black);
        shader->setInt("texture_metallic", textureSlot);
        shader->setInt("texture_metallic1", textureSlot);
        textureSlot++;
    }
    
    if (!hasRoughness)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, defaults.black);
        shader->setInt("texture_roughness", textureSlot);
        shader->setInt("texture_roughness1", textureSlot);
        textureSlot++;
    }
    
    if (!hasAO)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, defaults.white);
        shader->setInt("texture_ao", textureSlot);
        shader->setInt("texture_ao1", textureSlot);
        textureSlot++;
    }
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
