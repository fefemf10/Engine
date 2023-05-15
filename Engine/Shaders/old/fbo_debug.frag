#version 460 core

in vec2 UV;
out vec4 color;

layout (binding = 0)uniform sampler2D textureSampler;

void main()
{
	//float r = texture(textureSampler, UV).r;
	//color = vec4(r,r,r,1.0);
	color = texture(textureSampler, UV);
}