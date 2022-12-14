#pragma once
#include <GLm/glm.hpp>
#include <GLFW/glfw3.h>
#include <GLm/ext/matrix_transform.hpp>
#include <GLm/ext/matrix_clip_space.hpp>
#include "Player.hpp"
#include "Frustum.hpp"
#include "../IMGUI/imgui.h"

class Camera
{
public:
	Camera();
	void update(const float& dt);
	const glm::vec3& getUp() const;
	glm::mat4 getView() const;
	const glm::mat4& getProjection() const;
	const glm::vec3& getLook() const;
	Frustum& getFrustum();
private:
	glm::vec3 position;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 look;
	const glm::mat4 projection;
	Frustum frustum;
	float pitch, yaw;
	float speed;
	bool changeSpeed = false;
	bool fly = true;
};

