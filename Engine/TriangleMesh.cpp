#include "TriangleMesh.hpp"

TriangleMesh::TriangleMesh(vk::PhysicalDevice& physicalDevice, vk::Device& device) : device(device)
{
	std::vector<float> vertices = {
		0.0f, -0.05f, 0.0f, 1.0f, 0.0f,
		0.05f, 0.05f, 0.0f, 1.0f, 0.0f,
		-0.05f, 0.05f, 0.0f, 1.0f, 0.0f
	};
	VulkanUtils::BufferInput inputChunk;
	inputChunk.physicalDevice = physicalDevice;
	inputChunk.device = device;
	inputChunk.size = sizeof(float) * vertices.size();
	inputChunk.usage = vk::BufferUsageFlagBits::eVertexBuffer;

	vertexBuffer = VulkanUtils::createBuffer(inputChunk);

	void* memoryLocation = device.mapMemory(vertexBuffer.bufferMemory, 0, inputChunk.size);
	memcpy(memoryLocation, vertices.data(), inputChunk.size);
	device.unmapMemory(vertexBuffer.bufferMemory);
}

TriangleMesh::~TriangleMesh()
{
	device.destroyBuffer(vertexBuffer.buffer);
	device.freeMemory(vertexBuffer.bufferMemory);
}
