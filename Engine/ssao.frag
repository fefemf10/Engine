#version 460 core

in vec2 UV;
out float color;

layout(std140, binding = 0) uniform BufferTransform
{
	mat4 projection;
	mat4 view;
};

layout (binding=0) uniform sampler2D gPosition;
layout (binding=1) uniform sampler2D gNormal;
layout (binding=3) uniform sampler2D noise;

uniform vec3 samples[192];

uniform int kernelSize = 64;
uniform float radius = 0.5;
uniform float bias = 0.05;
uniform float power = 1;

uniform vec2 noiseScale;

void main()
{
	vec3 fragPos = texture(gPosition, UV).xyz;
	vec3 normal = texture(gNormal, UV).xyz * 2.0 - 1.0;
	vec3 randomVec = texture(gNormal, UV * noiseScale).xyz;
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);
	float occlusion = 0.0;
	for (int i = 0; i < kernelSize; ++i)
	{
		vec3 samplee = TBN * samples[i];
		samplee = fragPos + samplee * radius;
		vec4 offset = projection * view * vec4(samplee, 1.0);
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;
		float sampleDepth = texture(gPosition, offset.xy).z;
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplee.z + bias ? 1.0 : 0.0) * rangeCheck;
	}
	color = pow(1.0 - (occlusion / kernelSize), power);
}