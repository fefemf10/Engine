#pragma once
#include "Camera.hpp"
#include "Model.hpp"
class World
{
public:
	void update(const float& dt);
	void draw();
	Model m;
	Camera c;
};

