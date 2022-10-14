#pragma once
#include "../IMGUI/imgui.h"
#include <GLM/glm.hpp>
#include <GLFW/glfw3.h>


class Player
{
public:
	Player();
	void update(const float& dt);
	void setPitch(float pitch);
	void setYaw(float yaw);
	void setPosition(const glm::vec3& position);
	void move(const glm::vec3& position);
	const glm::vec3& getPosition() const;
	const float& getPitch() const;
	const glm::vec3& getLook() const;
	const glm::vec3& getRight() const;
	const float& getYaw() const;
	const bool& onGround() const;
	const float& getVerticalAcceleration() const;
	void setGround(const bool& flag);
	void draw();
	glm::bvec4 collide;
	float speed;
	float verticalAcceleration = 0.f;
	bool fly = true;
private:
	
	glm::vec3 position;
	glm::vec3 look;
	glm::vec3 right;
	
	float pitch, yaw;
	bool ground;
	bool changeSpeed = false;
};