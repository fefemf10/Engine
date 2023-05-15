#pragma once
#include <vulkan/vulkan.hpp>
#include <unordered_map>
#include "Memory.hpp"
enum class MeshType
{
	TRIANGLE,
	SQUARE,
	STAR
};
struct FinalizationChunk
{
	vk::PhysicalDevice physicalDevice;
	vk::Device device;
	vk::Queue queue;
	vk::CommandBuffer cmdBuffer;
};
class VertexManager
{
public:
	VertexManager();
	~VertexManager();
	void consume(MeshType type, const std::vector<float>& vertexData, const std::vector<uint32_t>& indicesData);
	void finalize(FinalizationChunk& finalizationChunk);
	VulkanUtils::Buffer vertexBuffer;
	VulkanUtils::Buffer indexBuffer;
	std::unordered_map<MeshType, int> firstIndices;
	std::unordered_map<MeshType, int> indexCounts;
private:
	int indexOffset;
	vk::Device device;
	std::vector<float> vertexLump;
	std::vector<uint32_t> indexLump;
};

