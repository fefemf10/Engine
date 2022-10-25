#include "GL.hpp"
#include <fstream>
Settings GL::settings;
namespace GL
{
	
	void loadSettings()
	{
		std::ifstream file("settings.bin", std::ios::binary);
		if (file.is_open())
		{
			file.read(reinterpret_cast<char*>(&settings), sizeof(Settings));
			/*file.read(reinterpret_cast<char*>(&settings.playerColor), sizeof(glm::vec3));
			file.read(reinterpret_cast<char*>(&settings.width), sizeof(int));
			file.read(reinterpret_cast<char*>(&settings.height), sizeof(int));
			file.read(reinterpret_cast<char*>(&settings.fullscreen), sizeof(bool));
			file.read(reinterpret_cast<char*>(&settings.vsync), sizeof(bool));
			uint8_t size;
			file.read(reinterpret_cast<char*>(&size), sizeof(uint8_t));
			settings.nickname.resize(size);
			file.read(reinterpret_cast<char*>(settings.nickname.data()), size);*/
			file.close();
		}
	}

	void saveSettings()
	{
		std::ofstream file("settings.bin", std::ios::binary);
		if (file.is_open())
		{
			file.write(reinterpret_cast<char*>(&settings), sizeof(Settings));
			/*file.write(reinterpret_cast<char*>(&settings.playerColor), sizeof(glm::vec3));
			file.write(reinterpret_cast<char*>(&settings.width), sizeof(int));
			file.write(reinterpret_cast<char*>(&settings.height), sizeof(int));
			file.write(reinterpret_cast<char*>(&settings.fullscreen), sizeof(bool));
			file.write(reinterpret_cast<char*>(&settings.vsync), sizeof(bool));
			uint8_t size = settings.nickname.size();
			file.write(reinterpret_cast<char*>(&size), sizeof(uint8_t));
			file.write(reinterpret_cast<char*>(settings.nickname.data()), size);*/
			file.close();
		}
	}
}