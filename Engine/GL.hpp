#pragma once

struct Settings
{
	int width = 1280;
	int height = 720;
	bool fullscreen = false;
	bool vsync = true;
};
namespace GL
{
	extern Settings settings;
	void loadSettings();
	void saveSettings();
}