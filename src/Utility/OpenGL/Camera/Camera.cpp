#include <OpenGL/Camera.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace CppUtility::OpenGL;

const float Camera::RATIO_WH = 1.0f;
const float Camera::NEAR_PLANE = 0.01f;
const float Camera::FAR_PLANE = 100.0f;
const float Camera::YAW = -90.0f;
const float Camera::PITCH = 0.0f;
const float Camera::FOV = 45.0f;
const Camera::ENUM_Projection Camera::PROJECTION_MODE = Camera::PROJECTION_PERSEPCTIVE;

// Constructor with vectors
Camera::Camera(glm::vec3 position, float yaw, float pitch , float rationWH , float nearPlane , float farPlane , glm::vec3 up, ENUM_Projection projectionMode)
	:
	position(position),
	yaw(yaw),
	pitch(pitch),
	rationWH(rationWH),
	nearPlane(nearPlane),
	farPlane(farPlane),
	worldUp(up),
	projectionMode(projectionMode),
	fov(FOV)
{
	updateCameraVectors();
}

void Camera::SetPerspective() {
	projectionMode = ENUM_Projection::PROJECTION_PERSEPCTIVE;
}

void Camera::SetOrtho() {
	projectionMode = ENUM_Projection::PROJECTION_ORTHO;
}

glm::vec3 & Camera::GetPos() {
	return position;
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() {
	return glm::lookAt(position, position + front, up);
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() {
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	right = glm::normalize(glm::cross(front, worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, front));
}

// Returns the projection matrix calculated using zoom, ratioWH, nearPlane, farPlane
glm::mat4 Camera::GetProjectionMatrix() {
	switch (projectionMode)
	{
	case OpenGL::Camera::PROJECTION_PERSEPCTIVE:
		return glm::perspective(glm::radians(fov), rationWH, nearPlane, farPlane);
		break;
	case OpenGL::Camera::PROJECTION_ORTHO:
		return glm::ortho(-fov / 4.0f, fov / 4.0f, -fov / 4.0f / rationWH, fov / 4.0f / rationWH, nearPlane, farPlane);
		break;
	default:
		return glm::mat4(1.0f);
		break;
	}
}