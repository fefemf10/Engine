#include "Mesh.hpp"

vk::VertexInputBindingDescription VulkanMesh::getPosColorBindingDescription()
{
	vk::VertexInputBindingDescription bindningDescription(0, 11 * sizeof(float), vk::VertexInputRate::eVertex);
	return bindningDescription;
}

std::vector<vk::VertexInputAttributeDescription> VulkanMesh::getPosColorAttributeDescriptions()
{
	std::vector<vk::VertexInputAttributeDescription> attributes(4);
	//Pos
	attributes[0].binding = 0;
	attributes[0].location = 0;
	attributes[0].format = vk::Format::eR32G32B32Sfloat;
	attributes[0].offset = 0;
	//Color
	attributes[1].binding = 0;
	attributes[1].location = 1;
	attributes[1].format = vk::Format::eR32G32B32Sfloat;
	attributes[1].offset = 3 * sizeof(float);
	//UV
	attributes[2].binding = 0;
	attributes[2].location = 2;
	attributes[2].format = vk::Format::eR32G32Sfloat;
	attributes[2].offset = 6 * sizeof(float);
	//UV
	attributes[3].binding = 0;
	attributes[3].location = 3;
	attributes[3].format = vk::Format::eR32G32B32Sfloat;
	attributes[3].offset = 8 * sizeof(float);
	return attributes;
}
