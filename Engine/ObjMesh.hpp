#pragma once
#include <span>
#include <vector>
#include <string_view>
#include <unordered_map>
#include <fstream>
#include <string>
#include <ranges>
#include <glm/glm.hpp>
class ObjMesh
{
public:
	std::vector<float> vertices;
	std::vector<uint32_t> indices;
	std::unordered_map<const char*, uint32_t> history;
	std::unordered_map<const char*, glm::vec3> colors;
	glm::vec3 brushColor;

	std::vector<glm::vec3> v, vn;
	std::vector<glm::vec2> vt;
	glm::mat4 transform;

	ObjMesh(std::string_view objFilepath, std::string_view mtlFilepath, const glm::mat4& transform);
	void readVertexData(std::span<std::string> words);
	void readTexcoordData(std::span<std::string> words);
	void readNormalData(std::span<std::string> words);
	void readFaceData(const std::vector<const char*>& words, bool skip);
	void readCornerData(const char* vertexDescription, bool skip);
};

