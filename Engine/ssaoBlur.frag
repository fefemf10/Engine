#version 460 core

in vec2 UV;
out float color;

layout (binding=0) uniform sampler2D textureSampler;

void main()
{
	vec2 texelSize = 1.0 / vec2(textureSize(textureSampler, 0));
	float result = 0.0;
	for (int x = -2; x < 2; ++x)
	{
		for (int y = -2; y < 2; ++y)
		{
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(textureSampler, UV + offset).r;
		}
	}
	color = result / 16.0;
}