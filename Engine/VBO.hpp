#pragma once
#include <cstdint>
#include <span>

class VBO
{
public:
	friend class VAO;
	VBO();
	VBO(uint32_t size, GLenum attribType, uint32_t attribSize);
	VBO(const VBO& other);
	VBO operator=(const VBO& other);
	virtual ~VBO();

	void setup(uint32_t size, GLenum attribType, uint32_t attribSize);
	void load(GLenum componentType, GLuint componentCount, const std::span<std::byte> data);
	static bool isIntegerType(GLenum type) noexcept
	{
		switch (type)
		{
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
		case GL_INT:
		case GL_UNSIGNED_INT:
			return true;
		default:
			return false;
		}
	}
	static uint8_t getSizeAttribute(GLenum type) noexcept
	{
		switch (type)
		{
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			return 1u;
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
			return 2u;
		case GL_INT:
		case GL_UNSIGNED_INT:
		case GL_FLOAT:
			return 4u;
		case GL_DOUBLE:
			return 8u;
		}
	}
private:
	GLuint id;
	uint32_t size;
	GLenum componentType;
	uint32_t componentSize;
};

