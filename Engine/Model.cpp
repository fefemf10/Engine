#include "Model.hpp"

void Model::loadFromFile(const std::string& path)
{
	gltf2::Asset asset = gltf2::load(path);
	const std::vector<gltf2::Image>& images = asset.images;
	textures.resize(images.size());
	for (size_t i = 0; i < images.size(); ++i)
	{
		textures[i].loadFromFile(images[i].uri);
	}
	for (size_t i = 0; i < asset.nodes.size(); ++i)
	{
		const gltf2::Mesh& mesh = asset.meshes[asset.nodes[i].mesh];
		const std::vector<gltf2::Primitive>& primitives = mesh.primitives;
		size_t meshesCount = meshes.size();
		meshes.resize(meshes.size() + primitives.size());
		for (size_t j = 0; j < primitives.size(); ++j)
		{
			const gltf2::Material& material = asset.materials[primitives[j].material];
			meshes[meshesCount + j].material.pbr.baseColorTexture.index = textures[asset.textures[material.pbr.baseColorTexture.index].source].getID();
			meshes[meshesCount + j].material.pbr.metallicRoughnessTexture.index = textures[asset.textures[material.pbr.metallicRoughnessTexture.index].source].getID();
			meshes[meshesCount + j].material.emissiveTexture.index = textures[asset.textures[material.emissiveTexture.index].source].getID();
			meshes[meshesCount + j].material.normalTexture.index = textures[asset.textures[material.normalTexture.index].source].getID();
			meshes[meshesCount + j].material.occlusionTexture.index = textures[asset.textures[material.occlusionTexture.index].source].getID();
			const gltf2::Accessor& accessorIndices = asset.accessors[primitives[j].indices];
			const gltf2::BufferView& bufferViewIndices = asset.bufferViews[accessorIndices.bufferView];
			const gltf2::Buffer& bufferIndices = asset.buffers[bufferViewIndices.buffer];
			
			const gltf2::Accessor& accessorPosition = asset.accessors[primitives[j].attributes.at("POSITION")];
			const gltf2::BufferView& bufferViewPosition = asset.bufferViews[accessorPosition.bufferView];
			const gltf2::Buffer& bufferPosition = asset.buffers[bufferViewPosition.buffer];

			const gltf2::Accessor& accessorNormal = asset.accessors[primitives[j].attributes.at("NORMAL")];
			const gltf2::BufferView& bufferViewNormal = asset.bufferViews[accessorNormal.bufferView];
			const gltf2::Buffer& bufferNormal = asset.buffers[bufferViewNormal.buffer];

			const gltf2::Accessor& accessorUv = asset.accessors[primitives[j].attributes.at("TEXCOORD_0")];
			const gltf2::BufferView& bufferViewUv = asset.bufferViews[accessorUv.bufferView];
			const gltf2::Buffer& bufferUv = asset.buffers[bufferViewUv.buffer];

			//meshes[meshesCount + j].vertices.insert(meshes[meshesCount + j].vertices.end(), reinterpret_cast<glm::vec3*>(bufferPosition.data + bufferViewPosition.byteOffset), reinterpret_cast<glm::vec3*>(bufferPosition.data + bufferViewPosition.byteOffset + bufferViewPosition.byteLength));
			//meshes[meshesCount + j].normal.insert(meshes[meshesCount + j].normal.end(), reinterpret_cast<glm::vec3*>(bufferNormal.data), reinterpret_cast<glm::vec3*>(bufferNormal.data + bufferNormal.byteLength));
			//meshes[meshesCount + j].uv.insert(meshes[meshesCount + j].uv.end(), reinterpret_cast<glm::vec3*>(bufferUv.data), reinterpret_cast<glm::vec3*>(bufferUv.data + bufferUv.byteLength));
			meshes[meshesCount + j].vao.load(static_cast<GLenum>(accessorIndices.componentType), accessorIndices.count, static_cast<GLenum>(bufferViewIndices.target), 0, std::span<std::byte>(reinterpret_cast<std::byte*>(bufferIndices.data + bufferViewIndices.byteOffset), bufferViewIndices.byteLength));
			meshes[meshesCount + j].vao.load(static_cast<GLenum>(accessorPosition.componentType), accessorPosition.count, static_cast<GLenum>(bufferViewPosition.target), 0, std::span<std::byte>(reinterpret_cast<std::byte*>(bufferPosition.data + bufferViewPosition.byteOffset), bufferViewPosition.byteLength));
			meshes[meshesCount + j].vao.load(static_cast<GLenum>(accessorNormal.componentType), accessorNormal.count, static_cast<GLenum>(bufferViewNormal.target), 0, std::span<std::byte>(reinterpret_cast<std::byte*>(bufferNormal.data + bufferViewNormal.byteOffset), bufferViewNormal.byteLength));
			meshes[meshesCount + j].vao.load(static_cast<GLenum>(accessorUv.componentType), accessorUv.count, static_cast<GLenum>(bufferViewUv.target), 0, std::span<std::byte>(reinterpret_cast<std::byte*>(bufferUv.data + bufferViewUv.byteOffset), bufferViewUv.byteLength));
			meshes[meshesCount + j].vao.bindAllBuffers();
		}
	}
}

void Model::draw() const
{
	for (const auto& mesh : meshes)
	{
		mesh.draw();
	}
}
