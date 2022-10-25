#pragma once
#include <string>
#include <GLM/ext/vector_int3.hpp>
#include <GLM/ext/vector_int2.hpp>
#include "stb_image.hpp"
#include "GL.hpp"

class Texture
{
public:
	Texture();
	virtual ~Texture();
	Texture(const Texture& other);
	Texture& operator=(const Texture& other);
	void editTexture(const glm::ivec2& offset, const glm::ivec2& size, GLenum format, GLenum type, const char* buffer);
	void loadFromFile(const std::string& name);
	void generateMipmaps() const;
	GLuint getID() const noexcept;
	GLenum getTarget() const noexcept;
	GLenum getSizedFormat() const noexcept;
	GLsizei getMipmaps() const noexcept;
	void bind(GLuint unit) const;
	void unbind(GLuint unit) const;
private:
	glm::ivec2 size;
	GLuint id;
	GLenum target;
	GLenum sizedFormat;
	GLsizei mipmaps;
};