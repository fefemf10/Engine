#pragma once
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

class Transformable
{
public:
	Transformable();
	void setOrigin(const glm::vec3& origin);
	void setPosition(const glm::vec3& position);
	void setRotation(const float& rotation);
	void setScale(const glm::vec3& scale);
	const glm::vec3& getOrigin() const;
	const glm::vec3& getPosition() const;
	const float& getRotation() const;
	const glm::vec3& getScale() const;
	void move(const glm::vec3& offset);
	void rotate(const float& angle);
	void scale(const glm::vec3& factor);
	const glm::mat4& getTransform() const;
	const glm::mat4& getInverseTransform() const;
private:
	mutable glm::mat4 transform;
	mutable glm::mat4 inverseTransform;
	glm::vec3 origin;
	glm::vec3 position;
	glm::vec3 scalev;
	float rotation;
	mutable bool transformUpdate;
	mutable bool inverseTransformUpdate;
	
};