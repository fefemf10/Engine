#include "Scene.hpp"

Scene::Scene()
{
	positions.insert({ MeshType::GROUND, {} });
	positions.insert({ MeshType::GIRL, {} });
	positions.insert({ MeshType::SKULL, {} });

	positions[MeshType::GROUND].push_back(glm::vec3(10.f, 0.f, 0.0f));
	positions[MeshType::GIRL].push_back(glm::vec3(17.f, 0.f, 0.0f));
	positions[MeshType::SKULL].push_back(glm::vec3(15.f, -5.f, 0.0f));
	positions[MeshType::SKULL].push_back(glm::vec3(15.f, 5.f, 0.0f));
}
