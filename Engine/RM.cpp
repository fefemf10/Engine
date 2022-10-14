#include "RM.hpp"

std::unordered_map<std::string, Shader> RM::shaders;
std::unordered_map<std::string, Texture> RM::textures;

void RM::loadShader(const std::string& name, Shader&& shader)
{
	shaders.try_emplace(name, std::move(shader));
}

void RM::loadTexture(const std::string& name, Texture&& texture)
{
	textures.try_emplace(name, std::move(texture));
}

void RM::unloadShader(const std::string& name)
{
	shaders.erase(name);
}

void RM::unloadTexture(const std::string& name)
{
	textures.erase(name);
}

Shader& RM::getShader(const std::string& name)
{
	return shaders[name];
}

Texture& RM::getTexture(const std::string& name)
{
	return textures[name];
}

GLuint RM::getIDShader(const std::string& name)
{
	return shaders[name].getID();
}

GLuint RM::getIDTexture(const std::string& name)
{
	return textures[name].getID();
}
//void ResourceManager::useShader(const std::string& name)
//{
//	if (currentShader != name)
//	{
//		currentShader = name;
//		shaders[name].use();
//	}
//}
//
//void ResourceManager::useTexture(const std::string& name)
//{
//	if (currentTexture != name)
//	{
//		currentTexture = name;
//		currentAtlas = name;
//		textures[name].bind();
//	}
//}
//
//void ResourceManager::useTexture(GLuint id, GLuint unit)
//{
//	if (idTexture != id)
//	{
//		idTexture = id;
//		currentTexture = "";
//		currentAtlas = "";
//		glBindSampler(unit, GL_SAMPLER_2D);
//		glBindTextureUnit(unit, id);
//		
//		//glBindTextureUnit(unit, id);
//	}
//}
//
//void ResourceManager::useAtlas(const std::string& name)
//{
//	if (currentAtlas != name)
//	{
//		currentAtlas = name;
//		currentTexture = name;
//		atlases[name].bind();
//	}
//}