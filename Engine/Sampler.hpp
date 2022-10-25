#pragma once
#include "GL.hpp"
class Sampler
{
public:
	Sampler();
	virtual ~Sampler();
	void setParami(GLenum pname, GLint param);
	void setParamf(GLenum pname, GLfloat param);
private:
	GLuint id;
};
