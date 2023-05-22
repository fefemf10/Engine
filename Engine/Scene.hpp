#pragma once
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "VertexManager.hpp"
class Scene
{
public:
	Scene();
	std::unordered_map<MeshType, std::vector<glm::vec3>> positions;
};

