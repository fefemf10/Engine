#include "Sampler.hpp"

Sampler::Sampler()
{
	++GL::samplers[id];
	glCreateSamplers(1, &id);
}

Sampler::~Sampler()
{
	--GL::samplers[id];
	if (GL::samplers[id] == 0)
		glDeleteSamplers(1, &id);
}

void Sampler::setParami(GLenum pname, GLint param)
{
	glSamplerParameteri(id, pname, param);
}

void Sampler::setParamf(GLenum pname, GLfloat param)
{
	glSamplerParameterf(id, pname, param);
}
