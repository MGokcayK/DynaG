/*
* DynaG Renderer C++ / Mesh
*
* Mesh class for handling 3D object rendering, implementation based on https://learnopengl.com
*
* Author : @MGokcayK
*
* C.Date : 04/05/2021
* Update : 04/05/2021 - Create & Implementation // @MGokcayK
*/

#ifndef GL_MESH
#define GL_MESH

#include "gl_shader.h"

namespace OpenGL
{

    // Vertex structure for store related parameters.
    struct Vertex {
        // position
        glm::vec3 vPosition;
        // normal
        glm::vec3 vNormal;
        // texCoords
        glm::vec2 vTexCoords;
        // tangent
        glm::vec3 vTangent;
        // bitangent
        glm::vec3 vBitangent;
    };

    // Texture store structure.
    struct Texture {
        unsigned int iId;
        std::string sType;
        std::string sPath;
    };

    class Mesh {
    public:
        // Mesh Data
        std::vector<Vertex>       vVertices;
        std::vector<unsigned int> vIndices;
        std::vector<Texture>      vTextures;
        std::vector<float>        vOpacity;
        unsigned int iVao;
        std::string sOpacityName = "opacity";

        // Constructor
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, std::vector<float> opacity);

        // Render the mesh
        void draw(Shader& shader);

    private:
        // Render data 
        unsigned int iVbo, iEbo;

        // Initializes all the buffer objects/arrays
        void setupMesh();
    };

} // end of OpenGL namespace

#endif