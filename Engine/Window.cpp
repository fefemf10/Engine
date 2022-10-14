#include "Window.hpp"

Window::Window(const std::string& title)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GL::loadSettings();
	if (GL::settings.fullscreen)
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		window = glfwCreateWindow(glfwGetVideoMode(monitor)->width, glfwGetVideoMode(monitor)->height, title.c_str(), GL::settings.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
	}
	else
	{
		window = glfwCreateWindow(GL::settings.width, GL::settings.height, title.c_str(), nullptr, nullptr);
	}
	if (!window)
	{
		std::cout << "Window don't created" << std::endl;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(GL::settings.vsync);
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "glew dont Init" << std::endl;
	}
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 430 core");
	ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\comicbd.ttf", 18, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
}

Window::~Window()
{
	glfwDestroyWindow(window);
}

void Window::loop()
{
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(Window::message_callback, nullptr);
	
	VAO crosshair;
	crosshair.bind();
	crosshair.addVBO(1);
	crosshair.setTypeIndices(GL_UNSIGNED_BYTE);
	crosshair.loadData(0, std::vector<glm::vec2>{ {-0.01f, -0.01f }, { 0.01f, 0.01f }, { -0.01f, 0.01f }, { 0.01f, -0.01f } });
	crosshair.loadIndices(std::vector<uint8_t>{ 0, 1, 2, 3 });

	VAO fboBuffer;
	fboBuffer.bind();
	fboBuffer.addVBO(2);
	fboBuffer.setTypeIndices(GL_UNSIGNED_BYTE);
	fboBuffer.loadData(0, std::vector<glm::vec2>{ glm::vec2(-1.f, 1.f), glm::vec2(-1.f, -1.f), glm::vec2(1.f, -1.f), glm::vec2(1.f, 1.f) });
	fboBuffer.loadData(1, std::vector<glm::vec2>{ glm::vec2(0.f, 1.f), glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(1.f, 1.f) });
	fboBuffer.loadIndices(std::vector<uint8_t>{ 0, 1, 2, 2, 3, 0 });

	VAO fboBuffer1;
	fboBuffer1.bind();
	fboBuffer1.addVBO(2);
	fboBuffer1.setTypeIndices(GL_UNSIGNED_BYTE);
	fboBuffer1.loadData(0, std::vector<glm::vec2>{ glm::vec2(-1.f, 1.f), glm::vec2(-1.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 1.f) });
	fboBuffer1.loadData(1, std::vector<glm::vec2>{ glm::vec2(0.f, 1.f), glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(1.f, 1.f) });
	fboBuffer1.loadIndices(std::vector<uint8_t>{ 0, 1, 2, 2, 3, 0 });
	VAO fboBuffer2;
	fboBuffer2.bind();
	fboBuffer2.addVBO(2);
	fboBuffer2.setTypeIndices(GL_UNSIGNED_BYTE);
	fboBuffer2.loadData(0, std::vector<glm::vec2>{ glm::vec2(-1.f, 0.f), glm::vec2(-1.f, -1.f), glm::vec2(0.f, -1.f), glm::vec2(0.f, 0.f) });
	fboBuffer2.loadData(1, std::vector<glm::vec2>{ glm::vec2(0.f, 1.f), glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(1.f, 1.f) });
	fboBuffer2.loadIndices(std::vector<uint8_t>{ 0, 1, 2, 2, 3, 0 });
	VAO fboBuffer3;
	fboBuffer3.bind();
	fboBuffer3.addVBO(2);
	fboBuffer3.setTypeIndices(GL_UNSIGNED_BYTE);
	fboBuffer3.loadData(0, std::vector<glm::vec2>{ glm::vec2(0.f, 0.f), glm::vec2(0.f, -1.f), glm::vec2(1.f, -1.f), glm::vec2(1.f, 0.f) });
	fboBuffer3.loadData(1, std::vector<glm::vec2>{ glm::vec2(0.f, 1.f), glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(1.f, 1.f) });
	fboBuffer3.loadIndices(std::vector<uint8_t>{ 0, 1, 2, 2, 3, 0 });
	VAO fboBuffer4;
	fboBuffer4.bind();
	fboBuffer4.addVBO(2);
	fboBuffer4.setTypeIndices(GL_UNSIGNED_BYTE);
	fboBuffer4.loadData(0, std::vector<glm::vec2>{ glm::vec2(0.f, 1.f), glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(1.f, 1.f) });
	fboBuffer4.loadData(1, std::vector<glm::vec2>{ glm::vec2(0.f, 1.f), glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(1.f, 1.f) });
	fboBuffer4.loadIndices(std::vector<uint8_t>{ 0, 1, 2, 2, 3, 0 });

	Player player;
	Camera camera(player);
	World world("1", player, camera);
	const int cLights = 512;
	struct light
	{
		glm::vec3 position;
		float radius;
		glm::vec3 diffuse;
		float linear;
		float quadratic;
		float pad[3]{};
	};
	std::vector<light> lights(cLights);
	std::vector<GLint> vecLightVisibility(cLights);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distColor1(0.0, 0.01);
	std::uniform_real_distribution<float> distColor2(0.2, 1.0);
	std::uniform_real_distribution<float> distLight1(0.22, 0.35);
	std::uniform_real_distribution<float> distLight2(0.02, 0.44);
	std::uniform_real_distribution<float> distHeight(50, 60);
	for (size_t i = 0; i < cLights; ++i)
	{
		lights[i].position = glm::vec3(player.getPosition().x, distHeight(gen), player.getPosition().z);
		//lights[i].ambient = glm::vec3(distColor1(gen), distColor1(gen), distColor1(gen));
		lights[i].diffuse = glm::vec3(distColor2(gen), distColor2(gen), distColor2(gen));
		lights[i].linear = distLight1(gen);
		lights[i].quadratic = distLight2(gen);
		float lightMax = std::fmaxf(std::fmaxf(lights[i].diffuse.r, lights[i].diffuse.g), lights[i].diffuse.b);
		lights[i].radius = (-lights[i].linear + std::sqrtf(lights[i].linear * lights[i].linear - 4 * lights[i].quadratic * (1.0f - (256.f / 5.f) * lightMax))) / (2.f * lights[i].quadratic);
	}
	bool menu = true;
	bool polygon = true;
	bool vsync = true;
	bool ssaoRender = false;
	bool cameraUpdate = true;
	bool heatmapoldbool = false;
	int buffer = 0;
	int kernelSizeSSAO = 64;
	float radiusSSAO = 0.5f;
	float biasSSAO = 0.05f;
	float powerSSAO = 1.f;
	Framebuffer gbufferFBO, ssaoFBO, ssaoBlurFBO;
	const int maxLightCountPerTile = 1024;
	const int workGroups = 16;
	const int numGroupsX = GL::settings.width / workGroups;
	const int numGroupsY = GL::settings.height / workGroups;
	SSBO bufferLight(16 + sizeof(light) * cLights, 1), lightVisibility(4 * maxLightCountPerTile * numGroupsX * numGroupsY, 2);
	UBO bufferTransform(sizeof(glm::mat4) * 2, 0);
	Texture heatmapTex(1, { numGroupsX, numGroupsY, 0 }, GL_R32F);
	glBindImageTexture(4, heatmapTex.getID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	bufferLight.bind();
	bufferLight.loadData(0, std::vector<int>{ cLights });
	bufferLight.loadData(16, lights);
	bufferLight.unbind();
	lightVisibility.bind();
	lightVisibility.loadData(0, std::vector<int>(maxLightCountPerTile* numGroupsX * numGroupsY, 0));
	lightVisibility.unbind();
	bufferTransform.bind();
	bufferTransform.loadData(0, camera.getProjection());
	bufferTransform.unbind();
	{
		gbufferFBO.bind();
		gbufferFBO.attachTexture(GL_COLOR_ATTACHMENT0, GL_RGB16F, GL::settings.width, GL::settings.height);
		gbufferFBO.attachTexture(GL_COLOR_ATTACHMENT1, GL_RGB8, GL::settings.width, GL::settings.height);
		gbufferFBO.attachTexture(GL_COLOR_ATTACHMENT2, GL_RGB8, GL::settings.width, GL::settings.height);
		gbufferFBO.attachDepth(GL::settings.width, GL::settings.height);
		gbufferFBO.useBuffer();
		assert(gbufferFBO.checkStatus() == GL_FRAMEBUFFER_COMPLETE && "not complite");
	}
	{
		ssaoFBO.bind();
		ssaoFBO.attachTexture(GL_COLOR_ATTACHMENT0, GL_R8, GL::settings.width, GL::settings.height);
		ssaoFBO.useBuffer();
		assert(ssaoFBO.checkStatus() == GL_FRAMEBUFFER_COMPLETE && "not complite");
	}
	{
		ssaoBlurFBO.bind();
		ssaoBlurFBO.attachTexture(GL_COLOR_ATTACHMENT0, GL_R8, GL::settings.width, GL::settings.height);
		ssaoBlurFBO.useBuffer();
		assert(ssaoBlurFBO.checkStatus() == GL_FRAMEBUFFER_COMPLETE && "not complite");
	}
	std::uniform_real_distribution<float> distNoise(-1.0, 1.0);
	{
		GL::useShader(RM::getIDShader("ssao"));
		for (size_t i = 0; i < 64; ++i)
		{
			glm::vec3 sample(distNoise(gen), distNoise(gen), (distNoise(gen) + 1.f) / 2.f);
			sample = glm::normalize(sample);
			sample *= (distNoise(gen) + 1.f) / 2.f;
			float scale = i / 64.0;
			sample *= std::lerp(0.1f, 1.f, scale * scale);
			RM::getShader("ssao").setVec3("samples[" + std::to_string(i) + "]", sample);
		}
		RM::getShader("ssao").setVec2("noiseScale", { GL::settings.width / 4.f, GL::settings.height / 4.f });
	}
	Texture noise(1, { 4, 4, 0 }, GL_RGB32F);
	std::vector<glm::vec3> ssaoNoise;
	for (size_t i = 0; i < 16; ++i)
	{
		ssaoNoise.push_back({ distNoise(gen), distNoise(gen), 0.f });
	}
	noise.editTexture({ 0, 0, 0 }, { 4, 4 }, GL_RGB, GL_FLOAT, reinterpret_cast<const char*>(ssaoNoise.data()));
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		/*{
			ImGui::ShowDemoWindow();
			ImGui::Begin("Hotbar###MainHotbar", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNavFocus);
			ImGui::SetWindowPos("Hotbar###MainHotbar", { 0.25f * width, 0.9f * height });
			ImGui::SetWindowSize("Hotbar###MainHotbar", { 0.5f * width, 0.1f * height });
			ImGui::End();
		}*/
		if (menu)
		{
			ImGui::Begin("Debug###Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus);
			ImGui::SetWindowPos("Debug###Debug", {});
			ImGui::Text("Look player x = %.2f y = %.2f z = %.2f", player.getLook().x, player.getLook().y, player.getLook().z);
			ImGui::Text("Position player x = %.2f y = %.2f z = %.2f", player.getPosition().x, player.getPosition().y, player.getPosition().z);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Checkbox("SSAO", &ssaoRender);
			ImGui::Checkbox("Heatmap Old", &heatmapoldbool);
			if (ImGui::SliderFloat("Radius SSAO", &radiusSSAO, 0.f, 10.f))
			{
				GL::useShader(RM::getIDShader("ssao"));
				RM::getShader("ssao").setFloat("radius", radiusSSAO);
			}
			if (ImGui::SliderFloat("Bias SSAO", &biasSSAO, -1.f, 1.f))
			{
				GL::useShader(RM::getIDShader("ssao"));
				RM::getShader("ssao").setFloat("bias", biasSSAO);
			}
			if (ImGui::SliderFloat("Power SSAO", &powerSSAO, 0.f, 10.f))
			{
				GL::useShader(RM::getIDShader("ssao"));
				RM::getShader("ssao").setFloat("power", powerSSAO);
			}
			if (ImGui::SliderInt("kernelSize", &kernelSizeSSAO, 1.f, 192.f))
			{
				GL::useShader(RM::getIDShader("ssao"));
				RM::getShader("ssao").setInt("kernelSize", kernelSizeSSAO);
				for (size_t i = 0; i < kernelSizeSSAO; ++i)
				{
					glm::vec3 sample(distNoise(gen), distNoise(gen), (distNoise(gen) + 1.f) / 2.f);
					sample = glm::normalize(sample);
					sample *= (distNoise(gen) + 1.f) / 2.f;
					float scale = float(i) / kernelSizeSSAO;
					sample *= std::lerp(0.1f, 1.f, scale * scale);
					RM::getShader("ssao").setVec3("samples[" + std::to_string(i) + "]", sample);
				}
			}
			ImGui::End();
		}

		if (ImGui::IsKeyReleased(GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(window, true);
		if (ImGui::IsKeyReleased(GLFW_KEY_F3))
			menu ^= 1;
		if (ImGui::IsKeyReleased(GLFW_KEY_1))
		{
			polygon ^= 1;
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE + polygon);
		}

		if (ImGui::IsKeyReleased(GLFW_KEY_2))
		{
			vsync ^= 1;
			glfwSwapInterval(vsync);
		}
		if (ImGui::IsKeyReleased(GLFW_KEY_4))
		{
			++buffer;
			buffer %= 5;
		}
		if (ImGui::IsKeyReleased(GLFW_KEY_5))
		{
			cameraUpdate ^= 1;
			if (cameraUpdate)
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			else
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		float dt = ImGui::GetIO().DeltaTime;
		player.update(dt);
		if (cameraUpdate)
			camera.update(dt);
		world.update(dt);

		{
			gbufferFBO.bind();
			glEnable(GL_DEPTH_TEST);
			//glClearColor(0.0, 0.749, 1.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			GL::useShader(RM::getIDShader("gbuffer"));
			GL::useTexture(RM::getIDTexture("blocks"));
			bufferTransform.bind();
			world.draw();
			bufferTransform.unbind();
			gbufferFBO.unbind();
		}
		{
			GL::useShader(heatmapoldbool ? RM::getIDShader("heatmapold") : RM::getIDShader("heatmap"));
			GL::useTexture(gbufferFBO.getDepthTexture(), 0);
			bufferTransform.bind();
			bufferLight.bind();
			lightVisibility.bind();
			bufferTransform.loadData(sizeof(glm::mat4), camera.getView());
			glDispatchComputeGroupSizeARB(numGroupsX, numGroupsY, 1, workGroups, workGroups, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
			bufferTransform.unbind();
			bufferLight.unbind();
			lightVisibility.unbind();
		}
		if (ssaoRender)
		{
			{
				ssaoFBO.bind();
				GL::useShader(RM::getIDShader("ssao"));
				glClear(GL_COLOR_BUFFER_BIT);
				bufferTransform.bind();
				GL::useTexture(gbufferFBO.getTexture(0), 0);
				GL::useTexture(gbufferFBO.getTexture(1), 1);
				GL::useTexture(noise.getID(), 2);
				fboBuffer.draw();
				bufferTransform.unbind();
				ssaoFBO.unbind();
			}
			{
				ssaoBlurFBO.bind();
				GL::useShader(RM::getIDShader("ssaoBlur"));
				glClear(GL_COLOR_BUFFER_BIT);
				GL::useTexture(ssaoFBO.getTexture(0), 0);
				fboBuffer.draw();
				ssaoBlurFBO.unbind();
			}
		}
		lightVisibility.bind();
		bufferLight.bind();
		if (buffer == 0)
		{
			if (ssaoRender)
			{
				GL::useShader(RM::getIDShader("lightSSAO"));
				GL::useTexture(ssaoBlurFBO.getTexture(0), 3);
			}
			else
				GL::useShader(RM::getIDShader("light"));
			GL::useTexture(gbufferFBO.getTexture(0), 0);
			GL::useTexture(gbufferFBO.getTexture(1), 1);
			GL::useTexture(gbufferFBO.getTexture(2), 2);
			fboBuffer1.draw();

			GL::useShader(RM::getIDShader("fbo_debug"));
			GL::useTexture(gbufferFBO.getTexture(1), 0);
			fboBuffer2.draw();
			GL::useTexture(heatmapTex.getID(), 0);
			fboBuffer3.draw();
			if (ssaoRender)
			{
				GL::useTexture(ssaoBlurFBO.getTexture(0), 0);
				fboBuffer4.draw();
			}
		}
		else if (buffer == 1)
		{
			if (ssaoRender)
			{
				GL::useShader(RM::getIDShader("lightSSAO"));
				GL::useTexture(ssaoBlurFBO.getTexture(0), 3);
			}
			else
				GL::useShader(RM::getIDShader("light"));
			GL::useTexture(gbufferFBO.getTexture(0), 0);
			GL::useTexture(gbufferFBO.getTexture(1), 1);
			GL::useTexture(gbufferFBO.getTexture(2), 2);

			fboBuffer.draw();
		}
		else if (buffer == 2)
		{
			GL::useShader(RM::getIDShader("fbo_debug"));
			GL::useTexture(gbufferFBO.getTexture(1), 0);
			fboBuffer.draw();
		}
		else if (buffer == 3)
		{
			GL::useShader(RM::getIDShader("fbo_debug"));
			GL::useTexture(heatmapTex.getID(), 0);
			fboBuffer.draw();
		}
		else if (buffer == 4)
		{
			if (ssaoRender)
			{
				GL::useShader(RM::getIDShader("fbo_debug"));
				GL::useTexture(ssaoBlurFBO.getTexture(0), 0);
				fboBuffer.draw();
			}
		}
		lightVisibility.unbind();
		bufferLight.unbind();
		GL::useShader(RM::getIDShader("crosshair"));
		crosshair.draw(GL_LINES);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}
	world.saveWorld();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Window::message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
	std::string const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		}
	}();

	std::string const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		}
	}();

	std::string const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		}
	}();

	Logger::log(std::string(src_str) + ", " + std::string(type_str) + ", " + std::string(severity_str) + ", " + std::to_string(id) + ": " + std::string(message));
}