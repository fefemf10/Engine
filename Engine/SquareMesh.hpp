#pragma once
#include <vulkan/vulkan.hpp>
#include "Memory.hpp"
class SquareMesh
{
public:
	SquareMesh(vk::PhysicalDevice& physicalDevice, vk::Device& device);
	~SquareMesh();
	VulkanUtils::Buffer vertexBuffer;
private:
	vk::Device device;
};

