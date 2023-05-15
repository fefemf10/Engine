#pragma once
#include <vulkan/vulkan.hpp>

namespace Vulkan
{
	std::vector<unsigned int> readFile(const std::string& filepath);
	vk::ShaderModule createModule(const std::string& filename, vk::Device& device);
}