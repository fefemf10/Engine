#include "Mesh.hpp"

void Mesh::draw() const
{
	glBindTextureUnit(0, material.pbr.baseColorTexture.index);
	glBindTextureUnit(1, material.occlusionTexture.index);
	glBindTextureUnit(2, material.emissiveTexture.index);
	glBindTextureUnit(3, material.pbr.metallicRoughnessTexture.index);
	glBindTextureUnit(4, material.normalTexture.index);
	vao.draw();
}
