#pragma once
#include <ctime>
#include <random>
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include "Shader.hpp"
#include "Framebuffer.hpp"
#include "Logger.hpp"
#include "Texture.hpp"
#include "Player.hpp"
#include "Camera.hpp"
#include "SSBO.hpp"
#include "UBO.hpp"
#include "VAO.hpp"
#include "RM.hpp"
#include "GL.hpp"
#include "World.hpp"
#include "../IMGUI/imgui.h"
#include "../IMGUI/imgui_impl_glfw.h"
#include "../IMGUI/imgui_impl_opengl3.h"
class Window
{
public:
	Window(const std::string& title);
	~Window();
	void loop();
private:
	static void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);
	GLFWwindow* window;
};