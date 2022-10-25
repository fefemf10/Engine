#version 460 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gColor;

in vec3 UV;
in vec3 normal;
in vec3 vColor;
in vec3 fragPos;

uniform sampler2DArray textureSampler;

void main()
{
	gPosition = fragPos;
	gNormal = normal;
	gColor = texture(textureSampler, UV).rgb * vColor;
}