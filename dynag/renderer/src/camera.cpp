#include "camera.h"
#include <iostream>

Camera::Camera(glm::vec3 vPosition, 
				glm::vec3 vUp, 
				float fYaw, 
				float fPitch)
{
	vFront = glm::vec3(0.0f, 0.0f, -1.0f);
	this->vPosition = vPosition;
	vWorldUp = vUp;
	this->fYaw = fYaw;
	this->fPitch = fPitch;
	updateCameraVectors();
}

Camera::Camera(
		float posX, 
		float posY, 
		float posZ, 
		float upX, 
		float upY, 
		float upZ, 
		float yaw, 
		float pitch) 
{
	vFront = glm::vec3(0.0f, 0.0f, -1.0f);
	vPosition = glm::vec3(posX, posY, posZ);
	vWorldUp = glm::vec3(upX, upY, upZ);
	fYaw = yaw;
	fPitch = pitch;
	updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(vPosition, vPosition + vFront, vUp);
}

void Camera::processKeyboard(Camera_Movement direction, float deltaTime)
{
	// Set boost of camera. 
	fBoost = 1.0f;
	if (direction == BOOST)
		fBoost *= 50.5f;

	// Set velocity of the camera.
	float velocity = fMovementSpeed * deltaTime * fBoost;
	
	// Set new position of camera.
	if (direction == FORWARD)
		vPosition += vFront * velocity;
	if (direction == BACKWARD)
		vPosition -= vFront * velocity;
	if (direction == LEFT)
		vPosition -= vRight * velocity;
	if (direction == RIGHT)
		vPosition += vRight * velocity;
	if (direction == UP)
		vPosition += vUp * velocity;
	if (direction == DOWN)
		vPosition -= vUp * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	// Set new offset of mouse w.r.t mouse sensitivity.
	xoffset *= fMouseSensitivity;
	yoffset *= fMouseSensitivity;

	// Change camera angles.
	fYaw += xoffset;
	fPitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped.
	if (constrainPitch)
	{
		if (fPitch > 89.0f)
			fPitch = 89.0f;
		if (fPitch < -89.0f)
			fPitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
	// Set zoom value of camera.
	fZoom -= (float)yoffset;
	if (fZoom < 1.0f)
		fZoom = 1.0f;
	if (fZoom > 45.0f)
		fZoom = 45.0f;
}

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(fYaw)) * cos(glm::radians(fPitch));
	front.y = sin(glm::radians(fPitch));
	front.z = sin(glm::radians(fYaw)) * cos(glm::radians(fPitch));
	vFront = glm::normalize(front);

	// also re-calculate the Right and Up vector
	vRight = glm::normalize(glm::cross(vFront, vWorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	vUp = glm::normalize(glm::cross(vRight, vFront));
}