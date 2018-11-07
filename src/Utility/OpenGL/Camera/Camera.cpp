#include <OpenGL/Camera.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace CppUtility::OpenGL;

const float Camera::RATIO_WH = 1.0f;
const float Camera::NEAR_PLANE = 0.01f;
const float Camera::FAR_PLANE = 100.0f;
const float Camera::YAW = -90.0f;
const float Camera::PITCH = 0.0f;
const float Camera::ZOOM = 45.0f;
const Camera::ENUM_Projection Camera::PROJECTION_MODE = Camera::PROJECTION_PERSEPCTIVE;

// Constructor with vectors
Camera::Camera(float rationWH, float moveSpeed, float rotateSpeed, glm::vec3 position, float nearPlane, float farPlane, glm::vec3 up, float yaw, float pitch, ENUM_Projection projectionMode)
	: rationWH(rationWH), nearPlane(nearPlane), farPlane(farPlane), Front(glm::vec3(0.0f, 0.0f, -1.0f)), Zoom(ZOOM), projectionMode(projectionMode) {
	//------------
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	MovementSpeed = moveSpeed;
	MouseSensitivity = rotateSpeed;
	updateCameraVectors();
}

void Camera::SetPerspective() {
	projectionMode = ENUM_Projection::PROJECTION_PERSEPCTIVE;
}

void Camera::SetOrtho() {
	projectionMode = ENUM_Projection::PROJECTION_ORTHO;
}

glm::vec3 & Camera::GetPos() {
	return Position;
}

glm::vec3 & Camera::GetFront() {
	return Front;
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
	if (Zoom >= 1.0f && Zoom <= 45.0f)
		Zoom -= yoffset;
	else if (Zoom <= 1.0f)
		Zoom = 1.0f;
	else if (Zoom >= 45.0f)
		Zoom = 45.0f;
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(ENUM_Movement direction, float deltaTime) {
	float velocity = MovementSpeed * deltaTime;
	if (direction == MOVE_FORWARD)
		Position += Front * velocity;
	if (direction == MOVE_BACKWARD)
		Position -= Front * velocity;
	if (direction == MOVE_LEFT)
		Position -= Right * velocity;
	if (direction == MOVE_RIGHT)
		Position += Right * velocity;
	if (direction == MOVE_UP)
		Position += Up * velocity;
	if (direction == MOVE_DOWN)
		Position -= Up * velocity;
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() {
	return glm::lookAt(Position, Position + Front, Up);
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() {
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
}

// Returns the projection matrix calculated using zoom, ratioWH, nearPlane, farPlane
glm::mat4 Camera::GetProjectionMatrix() {
	switch (projectionMode)
	{
	case OpenGL::Camera::PROJECTION_PERSEPCTIVE:
		return glm::perspective(glm::radians(Zoom), rationWH, nearPlane, farPlane);
		break;
	case OpenGL::Camera::PROJECTION_ORTHO:
		return glm::ortho(-Zoom / 4.0f, Zoom / 4.0f, -Zoom / 4.0f / rationWH, Zoom / 4.0f / rationWH, nearPlane, farPlane);
		break;
	default:
		return glm::mat4(1.0f);
		break;
	}
}