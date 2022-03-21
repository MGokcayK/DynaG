/*
* DynaG Renderer C++ / Camera
*
* Camera class for handling camera implementation based on https://learnopengl.com
*
* Author : @MGokcayK
*
* C.Date : 04/05/2021
* Update : 04/05/2021 - Create & Implementation // @MGokcayK
*/

/*
* DIFFERENCE BETWEEN DIRECTION COSINE MATRIX (DCM) & ROTATION MATRIX (ROTMAT)
* 
* Rotmat rotates the point in fixed frame. On the other hand, DCM rotates the 
* frame. This make the difference between them. As a mathematical expression,
* DCM is a conjugate of Rotmat.
*/

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. 
// Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    BOOST
};

// An abstract camera class that processes input and calculates 
// the corresponding Euler Angles, Vectors and Matrices for use in OpenGL.
class Camera
{
public:
    // Camera Attributes
    glm::vec3 vPosition;
    glm::vec3 vFront;
    glm::vec3 vUp;
    glm::vec3 vRight;
    glm::vec3 vWorldUp;

    // Euler Angles for camera view
    float fYaw;
    float fPitch;

    // Camera options
    float fMovementSpeed = 1.5f;
    float fMouseSensitivity = 0.1f;
    float fZoom = 45.0f;
    float fBoost = 0.0f;

    // Constructor with vectors
    Camera(glm::vec3 vPosition,
        glm::vec3 vUp,
        float fYaw = -90.0f,
        float fPitch = 0.0f);

    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix();

    // Processes input received from any keyboard-like input system. 
    // Accepts input parameter in the form of camera defined 
    // ENUM (to abstract it from windowing systems)
    void processKeyboard(Camera_Movement direction, float deltaTime);

    // Processes input received from a mouse input system. 
    // Expects the offset value in both the x and y direction.
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // Processes input received from a mouse scroll-wheel event. 
    // Only requires input on the vertical wheel-axis
    void processMouseScroll(float yoffset);

    // Calculates the front vector from the Camera's (updated) Euler Angles.
    void updateCameraVectors();
};
#endif