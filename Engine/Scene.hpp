#pragma once
#include <vector>
#include <glm/glm.hpp>
class Scene
{
public:
	Scene();
	std::vector<glm::vec3> trianglePos;
	std::vector<glm::vec3> squarePos;
	std::vector<glm::vec3> starPos;
};

