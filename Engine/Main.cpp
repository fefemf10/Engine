#include "Window.hpp"

import Log;

int main()
{
	Log::init();
	Window window("Game");

	window.loop();
	glfwTerminate();
	Log::deinit();
	return 0;
}