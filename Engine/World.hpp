#pragma once
#include <filesystem>
#include <fstream>
#include <thread>
#include "Shader.hpp"
#include "Player.hpp"
#include "Camera.hpp"

class World
{
public:
	World(const std::string& name, Player& player, Camera& camera);
	void saveWorld();
	void update(const float& dt);
	void draw();
private:
	Player& player;
	Camera& camera;
	glm::ivec2 playerLastPosition;
	std::filesystem::path worldDir;
};

