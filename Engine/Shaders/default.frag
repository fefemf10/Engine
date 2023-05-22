#version 450 core

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexCoord;
layout (location = 2) in vec3 fragNormal;

layout (location = 0) out vec4 color;

layout (set = 1, binding = 0) uniform sampler2D material;

const vec4 sunColor = vec4(1.0);
const vec3 sunDirection = normalize(vec3(1.0, 1.0, -1.0));

void main()
{
	color = sunColor * max(0.0, dot(fragNormal, -sunDirection)) * vec4(fragColor, 1.0f) * texture(material, fragTexCoord);
}