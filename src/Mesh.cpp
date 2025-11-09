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
    if (defaults.albedo == 0) {
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
    bindDefaultTextures(shader, textureSlot, hasAlbedo, hasNormal, hasMetallic, hasRoughness, hasAO, defaults);

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
    // Textura blanca para albedo (valor por defecto: blanco = sin color)
    glGenTextures(1, &defaults.albedo);
    glBindTexture(GL_TEXTURE_2D, defaults.albedo);
    unsigned char albedo[4] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, albedo);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Textura normal por defecto (128, 128, 255) = (0.5, 0.5, 1.0) en espacio normalizado = normal hacia arriba
    glGenTextures(1, &defaults.normal);
    glBindTexture(GL_TEXTURE_2D, defaults.normal);
    unsigned char normal[4] = {128, 128, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, normal);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Textura negra para metallic (R=0 = no metálico)
    glGenTextures(1, &defaults.metallic);
    glBindTexture(GL_TEXTURE_2D, defaults.metallic);
    unsigned char metallic[4] = {0, 0, 0, 255}; // R=0 (no metálico)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, metallic);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Textura gris para roughness (R=128 = roughness 0.5)
    glGenTextures(1, &defaults.roughness);
    glBindTexture(GL_TEXTURE_2D, defaults.roughness);
    unsigned char roughness[4] = {128, 128, 128, 255}; // R=128 (roughness 0.5)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, roughness);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Textura blanca para AO (R=255 = AO 1.0 = sin oclusión)
    glGenTextures(1, &defaults.ao);
    glBindTexture(GL_TEXTURE_2D, defaults.ao);
    unsigned char ao[4] = {255, 255, 255, 255}; // R=255 (AO 1.0 = sin oclusión)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, ao);
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
            continue; // Saltar texturas que no son PBR
        }

        // Activar el slot de textura correcto
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        
        // Vincular la textura usando el método bind
        textures[i]->bind(textureSlot);
        
        // Usar shader.setInt() para establecer los uniforms
        std::string uniformName = name + number;
        shader->setInt(uniformName, textureSlot);
        
        // También establecer sin número para compatibilidad
        shader->setInt(name, textureSlot);
        
        textureSlot++;
    }
}

void Mesh::bindDefaultTextures(std::shared_ptr<Shader> shader, unsigned int& textureSlot, bool hasAlbedo, bool hasNormal, bool hasMetallic, bool hasRoughness, bool hasAO, const DefaultTextures& defaults) const noexcept
{
    // Vincular texturas por defecto para los samplers que no tienen textura
    if (!hasAlbedo)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, defaults.albedo);
        shader->setInt("texture_albedo", textureSlot);
        shader->setInt("texture_albedo1", textureSlot);
        textureSlot++;
    }
    
    if (!hasNormal)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, defaults.normal);
        shader->setInt("texture_normal", textureSlot);
        shader->setInt("texture_normal1", textureSlot);
        textureSlot++;
    }
    
    if (!hasMetallic)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, defaults.metallic);
        shader->setInt("texture_metallic", textureSlot);
        shader->setInt("texture_metallic1", textureSlot);
        textureSlot++;
    }
    
    if (!hasRoughness)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, defaults.roughness);
        shader->setInt("texture_roughness", textureSlot);
        shader->setInt("texture_roughness1", textureSlot);
        textureSlot++;
    }
    
    if (!hasAO)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, defaults.ao);
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
