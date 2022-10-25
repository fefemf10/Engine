#pragma once
#include "GLTF/glTF2.hpp"
#include "Mesh.hpp"

class Model
{
public:
	std::vector<Texture> textures;
	std::vector<Mesh> meshes;
	glm::mat4 model{1.f};
	void loadFromFile(const std::string& path);
	void draw() const;
};