#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>

namespace VulkanMesh
{
	vk::VertexInputBindingDescription getPosColorBindingDescription();
	std::vector<vk::VertexInputAttributeDescription> getPosColorAttributeDescriptions();
};