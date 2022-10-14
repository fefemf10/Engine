#include "RM.hpp"
#include "Logger.hpp"
#include "Shader.hpp"
#include "Window.hpp"

void loadShaders();

int main()
{
	Logger::init();
	Window window("Game");
	loadShaders();

	window.loop();
	glfwTerminate();
	Logger::deinit();
	return 0;
}

void loadShaders()
{
	RM::loadShader("crosshair", Shader({ "crosshair.vert", "crosshair.frag" }));
	Shader& crosshairShader = RM::getShader("crosshair");
	crosshairShader.link();

	RM::loadShader("ssao", Shader({ "rectangle.vert", "ssao.frag" }));
	Shader& ssao = RM::getShader("ssao");
	ssao.link();

	RM::loadShader("ssaoBlur", Shader({ "rectangle.vert", "ssaoBlur.frag" }));
	Shader& ssaoBlur = RM::getShader("ssaoBlur");
	ssaoBlur.link();

	RM::loadShader("fbo_debug", Shader({ "rectangle.vert", "fbo_debug.frag" }));
	Shader& fbo_debug = RM::getShader("fbo_debug");
	fbo_debug.link();

	RM::loadShader("light", Shader({ "rectangle.vert", "light.frag" }));
	Shader& light_pass = RM::getShader("light");
	light_pass.link();

	RM::loadShader("lightSSAO", Shader({ "rectangle.vert", "lightSSAO.frag" }));
	Shader& light_passSSAO = RM::getShader("lightSSAO");
	light_passSSAO.link();

	RM::loadShader("gbuffer", Shader({ "gbuffer.vert", "gbuffer.frag" }));
	Shader& shader = RM::getShader("gbuffer");
	shader.link();

	RM::loadShader("heatmap", Shader({ "heatmap.comp" }));
	Shader& heatmap = RM::getShader("heatmap");
	heatmap.link();

	RM::loadShader("heatmapold", Shader({ "heatmapold.comp" }));
	Shader& heatmapold = RM::getShader("heatmapold");
	heatmapold.link();
}