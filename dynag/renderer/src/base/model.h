#ifndef BASE_MODEL
#define BASE_MODEL

#include <glm/gtc/type_ptr.hpp>

#include <string>

class ModelBase
{
public:
	ModelBase() {};

    ModelBase(std::string const& path,
        std::string vertex_shader_file_path,
        std::string fragment_shader_file_path
    ) {};

    // Draws the model, and thus all its meshes
    virtual void draw() = 0;

    // Translate the model to `translation` points.
    virtual void translate(glm::vec3 translation) = 0;

    // Rotate the model to that angle.
    virtual void rotate(float angle, glm::vec3 rotation) = 0;

    // Scale the model
    virtual void scale(glm::vec3 scaling) = 0;

};

#endif // !BASE_MODEL