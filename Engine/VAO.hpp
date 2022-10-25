#pragma once
#include <span>
#include <vector>
#include <GLM/glm.hpp>
#include <GLFW/glfw3.h>
#include "VBO.hpp"
#include "GL.hpp"
class VAO
{
public:
	VAO();
	VAO(const VAO& other) = default;
	~VAO();
	void bind() const;
	void draw(const GLenum& mode = GL_TRIANGLES) const;
	void addVAO();
	void addVBO(GLuint count);
	void load(GLenum componentType, GLuint componentCount, GLenum target, GLuint buffer, const std::span<std::byte> data);
	void bindBuffer(const GLuint buffer);
	void bindAllBuffers();
	void setTypeIndices(const GLenum type);
	void clear();
	VBO& operator [](size_t index);
private:
	GLuint vao{ 0 }, ibo{ 0 }, indicesCount{ 0 };
	GLenum typeIndices = GL_UNSIGNED_INT;
	std::vector<VBO> buffers;
};

