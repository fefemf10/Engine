#pragma once
#include "Engine.hpp"
#include "Scene.hpp"
class App
{
public:
	App();
	~App();
	void run();
private:
	Engine* engine;
	GLFWwindow* window;
	Scene* scene;
	double lastTime;
	double currentTime;
	int numFrames;
	float frameTime;
	int width;
	int height;
	void createWindow();
	void calcFramerate();
};

