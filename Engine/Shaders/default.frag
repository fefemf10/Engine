#version 450 core

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexCoord;

layout (location = 0) out vec4 color;

layout (set = 1, binding = 0) uniform sampler2D material;

void main()
{
	color = vec4(fragColor, 1.0f) * texture(material, fragTexCoord);
}