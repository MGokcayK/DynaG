#include "gl_model.h"
#include "base/images.h"
#include "stb/stb_image.h"

namespace OpenGL
{

    Model::Model(std::string const& path,
        std::string vertex_shader_file_path,
        std::string fragment_shader_file_path
    )
    {
        // Load model from path
        loadModel(path);

        // Create shader from paths
        create_shader(vertex_shader_file_path, fragment_shader_file_path);

        // Get unifrom Block Index from shader for UBObjects block
        unsigned int uniformBlockIndex = glGetUniformBlockIndex(shader->ID, "UBObjects");
        // Link shader's uniform block to uniform binding point
        glUniformBlockBinding(shader->ID, uniformBlockIndex, 0);

        // Get unifrom Block Index from shader for LightBlock block
        unsigned int uniformLightBlockIndex = glGetUniformBlockIndex(shader->ID, "LightBlock");
        // Link shader's uniform block to uniform binding point
        glUniformBlockBinding(shader->ID, uniformLightBlockIndex, 1);

        // Get unifrom Block Index from shader for LightBlock block
        unsigned int uniformFogBlockIndex = glGetUniformBlockIndex(shader->ID, "FogBlock");
        // Link shader's uniform block to uniform binding point
        glUniformBlockBinding(shader->ID, uniformFogBlockIndex, 2);

    }

    void Model::create_shader(std::string vertex_shader_file_path, std::string fragment_shader_file_path)
    {
        // Create, build and compile shader program
        shader = new Shader(vertex_shader_file_path.c_str(), fragment_shader_file_path.c_str());
    }


    void Model::draw()
    {
        // Use model's shader and set some uniforms.
        shader->use();

        applyContainers();

        shader->setMat4("model", mModel);
        shader->setMat4("inversedTransposedModel", glm::inverseTranspose(mModel));

        // Draw meshs of the model.
        for (unsigned int i = 0; i < vMeshes.size(); i++)
        {
            vMeshes[i].draw(*shader);
        }

        mModel = mBaseModel;
        shader->setMat4("model", mModel);

        resetContainers();
    }

    void Model::translate(glm::vec3 translation)
    {
        mModel = glm::translate(mModel, translation);
    }

    void Model::rotate(float angle, glm::vec3 rotation)
    {
        mModel = glm::rotate(mModel, angle, rotation);
    }

    void Model::scale(glm::vec3 scaling)
    {
        mModel = glm::scale(mModel, scaling);
    }


    void Model::loadModel(std::string const& path)
    {
        // Read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        // Check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }

        // Retrieve the directory path of the filepath
        sDirectory = path.substr(0, path.find_last_of('/'));

        // Process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode* node, const aiScene* scene)
    {
        // Process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // The node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            vMeshes.push_back(processMesh(mesh, scene));
        }

        // Process each of the children nodes.
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // Data to fill
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        std::vector<float> opacity;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // Declaring a placeholder vector since assimp uses 
            // its own vector class that doesn't directly convert to glm's vec3 
            // class so we transfer the data to this placeholder glm::vec3 first.

            // Positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.vPosition = vector;

            // Normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.vNormal = vector;
            }

            // Texture coordinates
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                // A vertex can contain up to 8 different texture coordinates. 
                // Thus make the assumption that it won't use models where a 
                // vertex can have multiple texture coordinates, so always take 
                // the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.vTexCoords = vec;

                // Tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.vTangent = vector;

                // Bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.vBitangent = vector;
            }
            else
                vertex.vTexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        // Retrieve each of the mesh's faces and the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];

            // Retrieve all indices of the face and store them in the indices vector.
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // Process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // A convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN    

        // 1. Diffuse maps
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // 2. Specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        // 3. Normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        // 4. Height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // 5. Opacity
        float opacity_val;
        material->Get(AI_MATKEY_OPACITY, opacity_val);
        opacity.push_back(opacity_val);

        // Return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures, opacity);
    }


    std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
    {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < vTexturesLoaded.size(); j++)
            {
                if (std::strcmp(vTexturesLoaded[j].sPath.data(), str.C_Str()) == 0)
                {
                    textures.push_back(vTexturesLoaded[j]);
                    skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {   // If texture hasn't been loaded already, load it
                Texture texture;
                texture.iId = TextureFromFile(str.C_Str(), sDirectory);
                texture.sType = typeName;
                texture.sPath = str.C_Str();
                textures.push_back(texture);
                vTexturesLoaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }

    void Model::applyContainers()
    {
        for (int b = 0; b < vBooleanContainer.size(); b++)
        {
            shader->setBool(vBooleanContainer[b]->name, vBooleanContainer[b]->data);
        }

        for (int i = 0; i < vIntegerContainer.size(); i++)
        {
            shader->setInt(vIntegerContainer[i]->name, vIntegerContainer[i]->data);
        }

        for (int f = 0; f < vFloatContainer.size(); f++)
        {
            shader->setFloat(vFloatContainer[f]->name, vFloatContainer[f]->data);
        }

        for (int v2 = 0; v2 < vVec2Container.size(); v2++)
        {
            shader->setVec2(vVec2Container[v2]->name, vVec2Container[v2]->data);
        }

        for (int v3 = 0; v3 < vVec3Container.size(); v3++)
        {
            shader->setVec3(vVec3Container[v3]->name, vVec3Container[v3]->data);
        }

        for (int v4 = 0; v4 < vVec4Container.size(); v4++)
        {
            shader->setVec4(vVec4Container[v4]->name, vVec4Container[v4]->data);
        }

        for (int m2 = 0; m2 < vMat2Container.size(); m2++)
        {
            shader->setMat2(vMat2Container[m2]->name, vMat2Container[m2]->data);
        }

        for (int m3 = 0; m3 < vMat3Container.size(); m3++)
        {
            shader->setMat3(vMat3Container[m3]->name, vMat3Container[m3]->data);
        }

        for (int m4 = 0; m4 < vMat4Container.size(); m4++)
        {
            shader->setMat4(vMat4Container[m4]->name, vMat4Container[m4]->data);
        }
    }

    void Model::resetContainers()
    {
        for (int b = 0; b < vBooleanContainer.size(); b++)
        {
            shader->setBool(vBooleanContainer[b]->name, vBooleanContainer[b]->reset);
        }
        vBooleanContainer.clear();

        for (int i = 0; i < vIntegerContainer.size(); i++)
        {
            shader->setInt(vIntegerContainer[i]->name, vIntegerContainer[i]->reset);
        }
        vIntegerContainer.clear();

        for (int f = 0; f < vFloatContainer.size(); f++)
        {
            shader->setFloat(vFloatContainer[f]->name, vFloatContainer[f]->reset);
        }
        vFloatContainer.clear();

        for (int v2 = 0; v2 < vVec2Container.size(); v2++)
        {
            shader->setVec2(vVec2Container[v2]->name, vVec2Container[v2]->data);
        }
        vVec2Container.clear();

        for (int v3 = 0; v3 < vVec3Container.size(); v3++)
        {
            shader->setVec3(vVec3Container[v3]->name, vVec3Container[v3]->data);
        }
        vVec3Container.clear();

        for (int v4 = 0; v4 < vVec4Container.size(); v4++)
        {
            shader->setVec4(vVec4Container[v4]->name, vVec4Container[v4]->data);
        }
        vVec4Container.clear();

        for (int m2 = 0; m2 < vMat2Container.size(); m2++)
        {
            shader->setMat2(vMat2Container[m2]->name, vMat2Container[m2]->data);
        }
        vMat2Container.clear();

        for (int m3 = 0; m3 < vMat3Container.size(); m3++)
        {
            this->shader->setMat3(vMat3Container[m3]->name, vMat3Container[m3]->data);
        }
        vMat3Container.clear();

        for (int m4 = 0; m4 < vMat4Container.size(); m4++)
        {
            shader->setMat4(vMat4Container[m4]->name, vMat4Container[m4]->data);
        }
        vMat4Container.clear();
    }

    unsigned int TextureFromFile(const char* path, const std::string& directory)
    {
        std::string filename = std::string(path);
        filename = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

} // end of OpenGL namespace