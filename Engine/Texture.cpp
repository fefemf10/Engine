#include "Texture.hpp"

Texture::Texture() : target{GL_TEXTURE_2D}, mipmaps{1}, size{}, sizedFormat{}
{
	glCreateTextures(target, 1, &id);
	++GL::textures[id];
}

Texture::~Texture()
{
	--GL::textures[id];
	if (GL::textures[id] == 0)
		glDeleteTextures(1, &GL::textures[id]);
}

Texture::Texture(const Texture& other) : id(other.id), target(other.target), mipmaps(other.mipmaps), size(other.size), sizedFormat(other.sizedFormat)
{
	++GL::textures[id];
}

Texture& Texture::operator=(const Texture& other)
{
	id = other.id;
	target = other.target;
	mipmaps = other.mipmaps;
	size = other.size;
	sizedFormat = other.sizedFormat;
	++GL::textures[id];
	return *this;
}

void Texture::editTexture(const glm::ivec2& offset, const glm::ivec2& size, GLenum format, GLenum type, const char* buffer)
{
	glTextureSubImage2D(GL::textures[id], 0, offset.x, offset.y, size.x, size.y, format, type, static_cast<const void*>(buffer));
}

void Texture::loadFromFile(const std::string& name)
{
	stbi_set_flip_vertically_on_load(true);
	int bpp{}, format{}, bit16{};
	void* data;
	if (stbi_is_16_bit(name.c_str()))
	{
		data = stbi_load_16(name.c_str(), &size.x, &size.y, &bpp, 0);
		bit16 = 1;
	}
	else
		data = stbi_load(name.c_str(), &size.x, &size.y, &bpp, 0);
	sizedFormat = GL_RGBA8;
	switch (bpp)
	{
	case 1:
		format = GL_RED;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		break;
	case 2:
		format = GL_RG;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
		break;
	case 3:
		format = GL_RGB;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		break;
	case 4:
		format = GL_RGBA;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		break;
	}
	glTextureStorage2D(id, 1, sizedFormat, size.x, size.y);
	generateMipmaps();
	glTextureSubImage2D(GL::textures[id], 0, 0, 0, size.x, size.y, format, bit16 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
}

void Texture::generateMipmaps() const
{
	glGenerateTextureMipmap(id);
}

GLuint Texture::getID() const noexcept
{
	return id;
}

GLenum Texture::getTarget() const noexcept
{
	return target;
}

GLenum Texture::getSizedFormat() const noexcept
{
	return sizedFormat;
}

GLsizei Texture::getMipmaps() const noexcept
{
	return mipmaps;
}

void Texture::bind(GLuint unit) const
{
	glBindTextureUnit(unit, id);
}

void Texture::unbind(GLuint unit) const
{
	glBindTextureUnit(unit, 0);
}