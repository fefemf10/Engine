#include "Mesh.hpp"

vk::VertexInputBindingDescription VulkanMesh::getPosColorBindingDescription()
{
	vk::VertexInputBindingDescription bindningDescription(0, 7 * sizeof(float), vk::VertexInputRate::eVertex);
	return bindningDescription;
}

std::vector<vk::VertexInputAttributeDescription> VulkanMesh::getPosColorAttributeDescriptions()
{
	std::vector<vk::VertexInputAttributeDescription> attributes(3);
	//Pos
	attributes[0].binding = 0;
	attributes[0].location = 0;
	attributes[0].format = vk::Format::eR32G32Sfloat;
	attributes[0].offset = 0;
	//Color
	attributes[1].binding = 0;
	attributes[1].location = 1;
	attributes[1].format = vk::Format::eR32G32B32Sfloat;
	attributes[1].offset = 2 * sizeof(float);
	//UV
	attributes[2].binding = 0;
	attributes[2].location = 2;
	attributes[2].format = vk::Format::eR32G32Sfloat;
	attributes[2].offset = 5 * sizeof(float);
	return attributes;
}
