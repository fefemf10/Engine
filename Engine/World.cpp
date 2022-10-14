#include "World.hpp"

World::World(const std::string& name, Player& player, Camera& camera) : player(player), camera(camera)
{
	if (!std::filesystem::exists(std::filesystem::current_path() / "saves"))
		std::filesystem::create_directory(std::filesystem::current_path() / "saves");
	std::filesystem::path worldDir = std::filesystem::current_path() / "saves" / name;
	if (!std::filesystem::exists(worldDir))
		std::filesystem::create_directory(worldDir);
	if (std::filesystem::exists(worldDir / "level.dat"))
	{
		std::ifstream level(worldDir / "level.dat", std::ios::binary | std::ios::in);
		if (level.is_open())
		{
			glm::vec3 pos;
			float pitch, yaw;
			level.read((char*)&pos, sizeof(pos));
			level.read((char*)&pitch, sizeof(pitch));
			level.read((char*)&yaw, sizeof(yaw));
			player.setPosition(pos);
			player.setPitch(pitch);
			player.setYaw(yaw);
			level.close();
		}
	}
	else
	{
		std::ofstream level(worldDir / "level.dat", std::ios::binary | std::ios::out);
		if (level.is_open())
		{
			glm::vec3 pos = player.getPosition();
			float pitch = player.getPitch();
			float yaw = player.getYaw();
			level.write((char*)&pos, sizeof(pos));
			level.write((char*)&pitch, sizeof(pitch));
			level.write((char*)&yaw, sizeof(yaw));
			level.close();
		}
	}
	this->worldDir = worldDir;
}

void World::saveWorld()
{
	std::ofstream level(worldDir / "level.dat", std::ios::binary | std::ios::in | std::ios::out);
	glm::vec3 pos = player.getPosition();
	float pitch = player.getPitch();
	float yaw = player.getYaw();
	level.write((char*)&pos, sizeof(pos));
	level.write((char*)&pitch, sizeof(pitch));
	level.write((char*)&yaw, sizeof(yaw));
	level.close();
}


void World::update(const float& dt)
{
	int x = player.getPosition().x;
	int z = player.getPosition().z;
	glm::ivec2 playerPos = glm::ivec2(x >> 4, z >> 4);
	/*if (playerLastPosition != playerPos)
		loadChunks();*/
	if (player.fly)
	{
		if (ImGui::IsKeyDown(GLFW_KEY_W))
		{
			player.move(player.getLook() * player.speed * 4.f * dt);
		}
		if (ImGui::IsKeyDown(GLFW_KEY_S))
		{
			player.move(player.getLook() * -player.speed * 4.f * dt);
		}
		if (ImGui::IsKeyDown(GLFW_KEY_A))
		{
			player.move(player.getRight() * player.speed * 4.f * dt);
		}
		if (ImGui::IsKeyDown(GLFW_KEY_D))
		{
			player.move(player.getRight() * -player.speed * 4.f * dt);
		}
	}
}

void World::draw()
{
	
}