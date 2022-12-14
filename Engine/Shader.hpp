#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include "GL.hpp"

class Shader
{
public:
	Shader();
	Shader(const Shader& other);
	Shader& operator=(const Shader& other);
	Shader(const std::initializer_list<std::string> shaderFilePaths);
	void loadFromFile(const std::initializer_list<std::string> shaderFilePaths);
	~Shader();
	void bindAttrib(GLuint index, const std::string& name);
	void link();
	void use();
	void setFloat(const std::string& name, const float& value);
	void setInt(const std::string& name, const int& value);
	void setVec2(const std::string& name, const glm::vec2& value);
	void setVec3(const std::string& name, const glm::vec3& value);
	void setMatrix(const std::string& name, const size_t& count, const bool& traspose, const glm::mat4& mat);
	GLint getLocation(const std::string& name);
	GLint getID() const;
private:
	GLuint program, vs, fs, cs;
	std::map<std::string, GLint> locations;
	GLuint compileShader(GLenum type, const std::string& source);
	std::string getStringShader(const std::string& path);
};
