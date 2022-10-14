#include "Transformable.hpp"

Transformable::Transformable() : rotation(0.f), origin(0.f), position(0.f), scalev(1.f), transform(1.f), transformUpdate(true), inverseTransform(1.f), inverseTransformUpdate(true)
{
}

void Transformable::setOrigin(const glm::vec3& origin)
{
	this->origin = origin;
	transformUpdate = true;
	inverseTransformUpdate = true;
}

void Transformable::setPosition(const glm::vec3& position)
{
	this->position = position;
	transformUpdate = true;
	inverseTransformUpdate = true;
}

void Transformable::setRotation(const float& rotation)
{
	this->rotation = std::fmodf(rotation, 360.f);
	if (this->rotation < 0.f)
		this->rotation += 360.f;
	transformUpdate = true;
	inverseTransformUpdate = true;
}

void Transformable::setScale(const glm::vec3& scale)
{
	this->scalev = scale;
	transformUpdate = true;
	inverseTransformUpdate = true;
}

const glm::vec3& Transformable::getOrigin() const
{
	return origin;
}

const glm::vec3& Transformable::getPosition() const
{
	return position;
}

const float& Transformable::getRotation() const
{
	return rotation;
}

const glm::vec3& Transformable::getScale() const
{
	return scalev;
}

void Transformable::move(const glm::vec3& offset)
{
	setPosition(position + offset);
}

void Transformable::rotate(const float& angle)
{
	setRotation(rotation + angle);
}

void Transformable::scale(const glm::vec3& factor)
{
	setScale(scalev * factor);
}

const glm::mat4& Transformable::getTransform() const
{
	if (transformUpdate)
	{
		transform = glm::translate(glm::mat4(1.f), position - origin);
		transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0.f));
		transform = glm::translate(transform, origin);
		transform = glm::scale(transform, scalev);
		transformUpdate = false;
	}
	return transform;
}

const glm::mat4& Transformable::getInverseTransform() const
{
	if(inverseTransformUpdate)
	{
		inverseTransform = glm::inverse(transform);
		inverseTransformUpdate = false;
	}
	return inverseTransform;
}