#include "Camera.hpp"

Camera::Camera() :  projection(glm::perspective(glm::radians(55.f), 16.f / 9.f, 0.1f, 1500.f)), frustum(), look(), right(), up(), yaw{ -90.f }, pitch{},
{

}

void Camera::update(const float& dt)
{
	yaw += ImGui::GetIO().MouseDelta.x * 0.1f;
	pitch += ImGui::GetIO().MouseDelta.y * -0.1f;
	if (pitch > 89.0f)
		pitch = 89.0f;
	else if (pitch < -89.0f)
		pitch = -89.0f;

	look = glm::normalize(glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)), sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
	right = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), look));

	if (ImGui::IsKeyPressed(GLFW_KEY_F))
	{
		fly ^= 1;
	}
	else if (ImGui::IsKeyPressed(GLFW_KEY_3))
	{
		changeSpeed ^= 1;
		if (changeSpeed)
			speed = 32.f;
		else
			speed = 1.f;
	}
	up = glm::normalize(glm::cross(look, right));
	frustum.clip = glm::transpose(projection * getView());
	frustum.update();
}

const glm::vec3& Camera::getUp() const
{
	return up;
}

glm::mat4 Camera::getView() const
{
	return glm::lookAt(position + glm::vec3(0.f, 1.8f, 0.f), position + look + glm::vec3(0.f, 1.8f, 0.f), up);
}

const glm::mat4& Camera::getProjection() const
{
	return projection;
}

const glm::vec3& Camera::getLook() const
{
	return look;
}

Frustum& Camera::getFrustum()
{
	return frustum;
}
