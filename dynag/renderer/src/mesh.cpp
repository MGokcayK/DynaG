#include "mesh.h"


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, std::vector<float> opacity)
{
    vVertices = vertices;
    vIndices = indices;
    vTextures = textures;
    vOpacity = opacity;

    setupMesh();
}


void Mesh::draw(Shader &shader) 
{
    // Bind appropriate textures.
    unsigned int diffuseNr  = 0;
    unsigned int specularNr = 0;
    unsigned int normalNr   = 0;
    unsigned int heightNr   = 0;
    for(unsigned int i = 0; i < vTextures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = vTextures[i].sType;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to stream
        else if(name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to stream
        else if(name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to stream

        // Set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
        // Set opacity        
        glUniform1f(glGetUniformLocation(shader.ID, sOpacityName.c_str()), vOpacity[i]);
        // Bind the texture
        glBindTexture(GL_TEXTURE_2D, vTextures[i].iId);
    }
    
    // Draw mesh
    glBindVertexArray(iVao);
    glDrawElements(GL_TRIANGLES, (int)vIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
    // Create buffers/arrays
    glGenVertexArrays(1, &iVao);
    glGenBuffers(1, &iVbo);
    glGenBuffers(1, &iEbo);

    // Bind vertex array.
    glBindVertexArray(iVao);

    // Load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, iVbo);
    glBufferData(GL_ARRAY_BUFFER, vVertices.size() * sizeof(Vertex), &vVertices[0], GL_STATIC_DRAW);  

    // Load data into element buffer object.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vIndices.size() * sizeof(unsigned int), &vIndices[0], GL_STATIC_DRAW);

    // Set the vertex attribute pointers.
    
    // vertex Positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vNormal));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vTexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vTangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vBitangent));

    glBindVertexArray(0);
}
