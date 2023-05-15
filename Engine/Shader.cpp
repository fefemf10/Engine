#include "Shader.hpp"
#include "Log.hpp"

std::vector<unsigned int> Vulkan::readFile(const std::string& filepath)
{
	std::ifstream file(filepath, std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		std::streampos filesize = file.tellg();
		file.seekg(file.beg, 0);
		std::vector<unsigned int> data(filesize >> 2);
		file.read((char*)data.data(), filesize);
		file.close();
		return std::move(data);
	}
	else
		return {};
}

vk::ShaderModule Vulkan::createModule(const std::string& filename, vk::Device& device)
{
	const std::vector<uint32_t>& sourceCode = readFile(filename);
	vk::ShaderModuleCreateInfo shaderModuleCreateInfo(vk::ShaderModuleCreateFlags(), sourceCode);
	vk::ShaderModule shaderModule;
	Log::debug("{}\n", device.createShaderModule(&shaderModuleCreateInfo, nullptr, &shaderModule) == vk::Result::eSuccess ? "Successfully loaded shader" : "Failed to load shader");
	return shaderModule;
}