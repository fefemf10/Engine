#pragma once
#include <vector>
#include <GLM/vec2.hpp>
#include <GLM/vec3.hpp>
#include "Texture.hpp"
#include "VAO.hpp"
#include "Material.hpp"

class Mesh
{
public:
	Material material;
	void draw() const;
	VAO vao;
};