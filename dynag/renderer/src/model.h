/*
* DynaG Renderer C++ / Model
*
* Model class for handling 3D object rendering, implementation based on https://learnopengl.com
*
* Author : @MGokcayK
*
* C.Date : 04/05/2021
* Update : 04/05/2021 - Create & Implementation // @MGokcayK
*/
#ifndef MODEL_H
#define MODEL_H

#include <stb/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include <glm/gtx/string_cast.hpp> // for string handling of glm.

unsigned int TextureFromFile(const char *path, const std::string &directory);

template <typename T>
struct container
{
    std::string name;
    T data;
    T reset;
};

class Model 
{
public:
    // Model data 
    std::vector<Texture> vTexturesLoaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh> vMeshes;
    std::string sDirectory;
    glm::mat4 mModel = glm::mat4(1.0f);
    glm::mat4 mBaseModel = glm::mat4(1.0f);
    std::vector<container<bool>*> vBooleanContainer;
    std::vector<container<int>*> vIntegerContainer;
    std::vector<container<float>*> vFloatContainer;
    std::vector<container<glm::vec2>*> vVec2Container;
    std::vector<container<glm::vec3>*> vVec3Container;
    std::vector<container<glm::vec4>*> vVec4Container;
    std::vector<container<glm::mat2>*> vMat2Container;
    std::vector<container<glm::mat3>*> vMat3Container;
    std::vector<container<glm::mat4>*> vMat4Container;
    Shader* shader;

    // To have different shader for different objecs, shader should be created 
    // for each model. To handle it, shader should be created.
    void create_shader(std::string vertex_shader_file_path, std::string fragment_shader_file_path);

    // Constructor, expects a filepath to a 3D model.
    Model(std::string const &path, 
          std::string vertex_shader_file_path, 
          std::string fragment_shader_file_path
          );

    // Draws the model, and thus all its meshes
    void draw();
    
    // Translate the model to `translation` points.
    void translate(glm::vec3 translation);

    // Rotate the model to that angle.
    void rotate(float angle, glm::vec3 rotation);

    // Scale the model
    void scale(glm::vec3 scaling);
    
private:
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string const &path);

    // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene);

    // Create mesh for the model.
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    // Checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

    // Appling uniform containers which filled from Python side.
    void applyContainers();

    // Reset uniform containers which filled from Python side.
    void resetContainers();
};

#endif
