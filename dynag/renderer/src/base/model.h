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
    virtual void draw() {};

    // Translate the model to `translation` points.
    virtual void translate(glm::vec3 translation) {};

    // Rotate the model to that angle.
    virtual void rotate(float angle, glm::vec3 rotation) {};

    // Scale the model
    virtual void scale(glm::vec3 scaling) {};

};

#endif // !BASE_MODEL