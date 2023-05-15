#pragma once
#include <vulkan/vulkan.hpp>
#include "Memory.hpp"
class TriangleMesh
{
public:
	TriangleMesh(vk::PhysicalDevice& physicalDevice, vk::Device& device);
	~TriangleMesh();
	VulkanUtils::Buffer vertexBuffer;
private:
	vk::Device device;
};

