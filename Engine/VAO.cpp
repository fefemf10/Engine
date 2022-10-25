#include "VAO.hpp"

VAO::VAO()
{
	glCreateVertexArrays(1, &vao);
	++GL::vaos[vao];
}

VAO::~VAO()
{
	--GL::vaos[vao];
	if (GL::vaos[vao] == 0)
	{
		glDeleteBuffers(1, &ibo);
		glDeleteVertexArrays(1, &vao);
	}
}

void VAO::bind() const
{
	glBindVertexArray(vao);
}

void VAO::draw(const GLenum& mode) const
{
	glBindVertexArray(vao);
	glDrawElements(mode, indicesCount, typeIndices, nullptr);
}

void VAO::addVAO()
{
	glCreateVertexArrays(1, &vao);
	++GL::vaos[vao];
}

void VAO::addVBO(GLuint count)
{
	buffers.resize(buffers.size() + count);
	for (size_t i = 0; i < buffers.size(); i++)
	{
		glEnableVertexArrayAttrib(vao, i);
		glVertexArrayAttribBinding(vao, i, i);
	}
}

void VAO::load(GLenum componentType, GLuint componentCount, GLenum target, GLuint buffer, const std::span<std::byte> data)
{
	if (target == GL_ELEMENT_ARRAY_BUFFER)
	{
		typeIndices = componentType;
		if (this->indicesCount != indicesCount)
		{
			glDeleteBuffers(1, &ibo);
			glCreateBuffers(1, &ibo);
			this->indicesCount = componentCount;
			glNamedBufferStorage(ibo, data.size(), data.data(), GL_DYNAMIC_STORAGE_BIT);
			glVertexArrayElementBuffer(vao, ibo);
		}
		else
		{
			glNamedBufferSubData(ibo, 0, data.size(), data.data());
		}
	}
	else
	{
		buffers[buffer].load(componentType, componentCount, data);
	}
}

void VAO::bindBuffer(const GLuint buffer)
{
	if (VBO::isIntegerType(buffers[buffer].componentType))
		glVertexArrayAttribIFormat(vao, buffer, buffers[buffer].componentSize, buffers[buffer].componentType, 0);
	else
		glVertexArrayAttribFormat(vao, buffer, buffers[buffer].componentSize, buffers[buffer].componentType, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vao, buffer, buffers[buffer].id, 0, buffers[buffer].componentSize * VBO::getSizeAttribute(buffers[buffer].componentType));
}

void VAO::bindAllBuffers()
{
	for (size_t i = 0; i < buffers.size(); ++i)
	{
		if (VBO::isIntegerType(buffers[i].componentType))
			glVertexArrayAttribIFormat(vao, i, buffers[i].componentSize, buffers[i].componentType, 0);
		else
			glVertexArrayAttribFormat(vao, i, buffers[i].componentSize, buffers[i].componentType, GL_FALSE, 0);
		glVertexArrayVertexBuffer(vao, i, buffers[i].id, 0, buffers[i].componentSize * VBO::getSizeAttribute(buffers[i].componentType));
	}
}

void VAO::setTypeIndices(const GLenum type)
{
	typeIndices = type;
}

void VAO::clear()
{
	for (size_t i = 0; i < buffers.size(); i++)
	{
		buffers[i].size = 0u;
	}
	indicesCount = 0;
}

VBO& VAO::operator[](size_t index)
{
	return buffers[index];
}