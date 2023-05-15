#include "Scene.hpp"

Scene::Scene()
{
	float x = -0.6f;
	for (float y = -1.0f; y < 1.0f; y += 0.2f)
		trianglePos.push_back(glm::vec3(x, y, 0.0f));
	x = 0.0f;
	for (float y = -1.0f; y < 1.0f; y += 0.2f)
		squarePos.push_back(glm::vec3(x, y, 0.0f));
	x = 0.6f;
	for (float y = -1.0f; y < 1.0f; y += 0.2f)
		starPos.push_back(glm::vec3(x, y, 0.0f));
}
