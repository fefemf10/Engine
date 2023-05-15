#include "App.hpp"
#include "Log.hpp"
import GL;

App::App()
{
	Log::init();
	createWindow();
	engine = new Engine(width, height, window);
	scene = new Scene();
}

App::~App()
{
	delete engine;
	delete scene;
	Log::deinit();
}

void App::run()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		engine->render(scene);
		calcFramerate();
	}
}

void App::createWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	GL::loadSettings();
	if (GL::settings.fullscreen)
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		window = glfwCreateWindow(glfwGetVideoMode(monitor)->width, glfwGetVideoMode(monitor)->height, "", GL::settings.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
		if (window)
		{
			Log::debug("Successfully made a glfw window width: {}, height: {}\n", glfwGetVideoMode(monitor)->width, glfwGetVideoMode(monitor)->height);
		}
		else
		{
			Log::debug("GLFW window creation failed\n");
		}
		width = glfwGetVideoMode(monitor)->width;
		height = glfwGetVideoMode(monitor)->height;
	}
	else
	{
		window = glfwCreateWindow(GL::settings.width, GL::settings.height, "", nullptr, nullptr);
		if (window)
		{
			Log::debug("Successfully made a glfw window width: {}, height: {}\n", GL::settings.width, GL::settings.height);
		}
		else
		{
			Log::debug("GLFW window creation failed\n");
		}
		width = GL::settings.width;
		height = GL::settings.height;
	}
}

void App::calcFramerate()
{
	currentTime = glfwGetTime();
	double delta = currentTime - lastTime;

	if (delta >= 1.0)
	{
		int framerate{std::max(1, int(numFrames / delta))};
		glfwSetWindowTitle(window, std::format("Running at {} fps", framerate).c_str());
		lastTime = currentTime;
		numFrames = -1;
		frameTime = float(1000.0 / framerate);
	}
	++numFrames;
}
