#include "VBO.hpp"

VBO::VBO()
{
	glCreateBuffers(1, &id);
}

VBO::VBO(uint32_t size, GLenum componentType, uint32_t componentSize) : size(size), componentType(componentType), componentSize(componentSize)
{
	glCreateBuffers(1, &id);
	glNamedBufferStorage(id, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
}

VBO::VBO(const VBO& other) : size(other.size), componentType(other.componentType), componentSize(other.componentSize)
{
	glCreateBuffers(1, &id);
	glNamedBufferStorage(id, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glCopyNamedBufferSubData(other.id, id, 0, 0, size);
}

VBO VBO::operator=(const VBO& other)
{
	size = other.size;
	componentType = other.componentType;
	componentSize = other.componentSize;
	glDeleteBuffers(1, &id);
	glCreateBuffers(1, &id);
	glNamedBufferStorage(id, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glCopyNamedBufferSubData(other.id, id, 0, 0, size);
	return *this;
}

VBO::~VBO()
{
	glDeleteBuffers(1, &id);
}

void VBO::setup(uint32_t size, GLenum componentType, uint32_t componentSize)
{
	this->size = size;
	this->componentType = componentType;
	this->componentSize = componentSize;
	glNamedBufferStorage(id, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
}

void VBO::load(GLenum componentType, GLuint componentCount, const std::span<std::byte> data)
{
	if (data.size() != size)
	{
		glDeleteBuffers(1, &id);
		glCreateBuffers(1, &id);
		size = static_cast<GLuint>(data.size());
		glNamedBufferStorage(id, data.size(), data.data(), GL_DYNAMIC_STORAGE_BIT);
	}
	else
	{
		if (data.size())
			glNamedBufferSubData(id, 0, data.size(), data.data());
	}
}