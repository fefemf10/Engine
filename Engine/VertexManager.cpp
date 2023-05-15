#include "VertexManager.hpp"

VertexManager::VertexManager()
{
	indexOffset = 0;
}

VertexManager::~VertexManager()
{
	device.destroyBuffer(vertexBuffer.buffer);
	device.freeMemory(vertexBuffer.bufferMemory);
	device.destroyBuffer(indexBuffer.buffer);
	device.freeMemory(indexBuffer.bufferMemory);
}

void VertexManager::consume(MeshType type, const std::vector<float>& vertexData, const std::vector<uint32_t>& indicesData)
{
	int indexCount = indicesData.size();
	int vertexCount = vertexData.size() / 7;
	int lastIndex = indexLump.size();
	firstIndices.insert({ type, lastIndex });
	indexCounts.insert({ type, indexCount });

	for (float attribute : vertexData)
		vertexLump.push_back(attribute);
	for (uint32_t index : indicesData)
		indexLump.push_back(index + indexOffset);
	indexOffset += vertexCount;
}

void VertexManager::finalize(FinalizationChunk& finalizationChunk)
{
	this->device = finalizationChunk.device;

	VulkanUtils::BufferInput inputChunk;
	inputChunk.physicalDevice = finalizationChunk.physicalDevice;
	inputChunk.device = finalizationChunk.device;
	inputChunk.size = sizeof(float) * vertexLump.size();
	inputChunk.usage = vk::BufferUsageFlagBits::eTransferSrc;
	inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	VulkanUtils::Buffer stagingBuffer = VulkanUtils::createBuffer(inputChunk);

	void* memoryLocation = device.mapMemory(stagingBuffer.bufferMemory, 0, inputChunk.size);
	memcpy(memoryLocation, vertexLump.data(), inputChunk.size);
	device.unmapMemory(stagingBuffer.bufferMemory);

	inputChunk.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
	inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
	vertexBuffer = VulkanUtils::createBuffer(inputChunk);

	VulkanUtils::copyBuffer(stagingBuffer, vertexBuffer, inputChunk.size, finalizationChunk.queue, finalizationChunk.cmdBuffer);

	device.destroyBuffer(stagingBuffer.buffer);
	device.freeMemory(stagingBuffer.bufferMemory);



	inputChunk.size = sizeof(uint32_t) * indexLump.size();
	inputChunk.usage = vk::BufferUsageFlagBits::eTransferSrc;
	inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	stagingBuffer = VulkanUtils::createBuffer(inputChunk);

	memoryLocation = device.mapMemory(stagingBuffer.bufferMemory, 0, inputChunk.size);
	memcpy(memoryLocation, indexLump.data(), inputChunk.size);
	device.unmapMemory(stagingBuffer.bufferMemory);

	inputChunk.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
	inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
	indexBuffer = VulkanUtils::createBuffer(inputChunk);

	VulkanUtils::copyBuffer(stagingBuffer, indexBuffer, inputChunk.size, finalizationChunk.queue, finalizationChunk.cmdBuffer);

	device.destroyBuffer(stagingBuffer.buffer);
	device.freeMemory(stagingBuffer.bufferMemory);
}
