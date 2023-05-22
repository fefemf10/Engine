#include "ObjMesh.hpp"
#include "Helper.hpp"
#include <cstdio>
#include <cstdlib>
#pragma warning(disable:4996)

bool start_with(const char* str, const char* find)
{
	while (*str == *find && *find != 0)
	{
		++find, ++str;
	}
	return *find == 0;
}

ObjMesh::ObjMesh(std::string_view objFilepath, std::string_view mtlFilepath, const glm::mat4& transform)
{
	vertices.reserve(10240);
	this->transform = transform;
	FILE* fp;
	fp = fopen(mtlFilepath.data(), "r");
	if (fp == NULL)
		exit(EXIT_FAILURE);

	char line[128]{};

	
	//std::ifstream file;
	//file.open(mtlFilepath.data());
	//std::string line;
	char* materialName = new char[128]{};
	std::vector<std::string> words;
	while ((fgets(line, 128, fp)))
	{
		if (start_with(line, "newmtl"))
			sscanf(line, "newmtl %s", materialName);
		else if (start_with(line, "Kd"))
		{
			sscanf(line, "Kd %f %f %f", &brushColor.x, &brushColor.y, &brushColor.z);
			colors.insert({ materialName, brushColor });
		}
	}
	fclose(fp);
	//file.open(objFilepath.data());
	fp = fopen(objFilepath.data(), "r");
	if (fp == NULL)
		exit(EXIT_FAILURE);
	glm::vec4 position(0.f, 0.f, 0.f, 1.f);
	glm::vec4 normal;
	glm::vec4 texcoord;
	glm::vec3 transformedVertex;
	while ((fgets(line, 128, fp)))
	{
		if (start_with(line, "vt"))
		{
			sscanf(line, "vt %f %f", &texcoord.x, &texcoord.y);
			vt.emplace_back(texcoord);
		}
		else if (start_with(line, "vn"))
		{
			sscanf(line, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
			transformedVertex = glm::vec3(transform * normal);
			vn.emplace_back(transformedVertex);
		}
		else if (start_with(line, "v"))
		{
			sscanf(line, "v %f %f %f", &position.x, &position.y, &position.z);
			transformedVertex = glm::vec3(transform * position);
			v.emplace_back(transformedVertex);
		}
		if (start_with(line, "usemtl"))
		{
			char buf[128]{};
			sscanf(line, "usemtl %s", buf);
			if (colors.contains(buf))
			{
				brushColor = colors[buf];
			}
			else
			{
				brushColor = glm::vec3(1.0f);
			}
		}
		if (start_with(line, "f"))
		{
			if (strstr(line, "//") != nullptr)
			{
				readFaceData(Helper::splitc(line, " "), true);
			}
			else
			{
				readFaceData(Helper::splitc(line, " "), false);
			}
		}
	}
	fclose(fp);
}

void ObjMesh::readVertexData(std::span<std::string> words)
{
	const glm::vec4& newVertex = glm::vec4(std::stof(words[1]), std::stof(words[2]), std::stof(words[3]), 1.f);
	const glm::vec3& transformedVertex = glm::vec3(transform * newVertex);
	v.emplace_back(transformedVertex);
}

void ObjMesh::readTexcoordData(std::span<std::string> words)
{
	vt.emplace_back(glm::vec2(std::stof(words[1]), std::stof(words[2])));
}

void ObjMesh::readNormalData(std::span<std::string> words)
{
	const glm::vec4& newVertex = glm::vec4(std::stof(words[1]), std::stof(words[2]), std::stof(words[3]), 0.f);
	const glm::vec3& transformedVertex = glm::vec3(transform * newVertex);
	vn.emplace_back(transformedVertex);
}

void ObjMesh::readFaceData(const std::vector<const char*>& words, bool skip)
{
	size_t triangleCount = words.size() - 3;
	for (size_t i = 0; i < triangleCount; i++)
	{
		readCornerData(words[1], skip);
		readCornerData(words[2 + i], skip);
		readCornerData(words[3 + i], skip);
	}
}

void ObjMesh::readCornerData(const char* vertexDescription, bool skip)
{
	if (history.contains(vertexDescription))
	{
		indices.emplace_back(history[vertexDescription]);
		return;
	}
	uint32_t index = history.size();
	history.emplace(std::pair{ vertexDescription, index });
	indices.emplace_back(index);
	char* s = const_cast<char*>(vertexDescription);
	int indexPos{}, indexTex{}, indexNormal{};
	if (skip)
	{
		const char* pos = strchr(vertexDescription, '/');
		const_cast<char*>(pos)[0] = 0;
		indexPos = atoi(vertexDescription);
		indexNormal = atoi(pos + 2);
		const_cast<char*>(pos)[0] = '/';
	}
	else
	{
		const char* firstSlesh, *secondSlesh;
		const char* pos = strchr(vertexDescription, '/');
		firstSlesh = pos;
		const_cast<char*>(firstSlesh)[0] = 0;
		indexPos = atoi(vertexDescription);

		pos = strchr(pos + 1, '/');
		secondSlesh = pos;
		indexTex = atoi(firstSlesh + 1);
		indexNormal = atoi(secondSlesh + 1);
		//sscanf(vertexDescription, "%d/%d/%d", &indexPos, &indexTex, &indexNormal);
		const_cast<char*>(secondSlesh)[0] = '/';
		const_cast<char*>(firstSlesh)[0] = '/';
	}
	glm::vec3 pos = v[indexPos - 1];
	vertices.emplace_back(pos[0]);
	vertices.emplace_back(pos[1]);
	vertices.emplace_back(pos[2]);

	vertices.emplace_back(brushColor[0]);
	vertices.emplace_back(brushColor[1]);
	vertices.emplace_back(brushColor[2]);

	glm::vec2 texcoord;
	if (!skip)
		texcoord = vt[indexTex - 1];
	vertices.emplace_back(texcoord[0]);
	vertices.emplace_back(texcoord[1]);

	glm::vec3 normal = vn[indexNormal - 1];
	vertices.emplace_back(normal[0]);
	vertices.emplace_back(normal[1]);
	vertices.emplace_back(normal[2]);
}
