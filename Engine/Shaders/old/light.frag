#version 460 core
#define MAX_LIGHT_COUNT 1024
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define WORK_GROUP_SIZE 16
#define NUM_GROUPS_X (SCREEN_WIDTH / WORK_GROUP_SIZE)
#define NUM_GROUPS_Y (SCREEN_HEIGHT / WORK_GROUP_SIZE)
struct Light {
    vec3 position;
	float radius;
    vec3 diffuse;
	float linear;
	float quadratic;
};

in vec2 UV;
out vec4 color;

layout (binding=0)uniform sampler2D gPosition;
layout (binding=1)uniform sampler2D gNormal;
layout (binding=2)uniform sampler2D gColor;

layout(std430, binding = 1) buffer BufferLight
{
	int countLight;
	Light lights[];
};

layout(std430, binding = 2) buffer BufferLightVisible
{ 
     int lightVisible[];
};

void main()
{
	const vec3 fragPos = texture(gPosition, UV).rgb;
	const vec3 normal = texture(gNormal, UV).rgb;
	const vec3 vColor = texture(gColor, UV).rgb;
	vec3 lightning = vColor * 0.1;
	
	const ivec2 location = ivec2(gl_FragCoord.xy);
	const ivec2 tileID = location / ivec2(16, 16);
	const uint offset = (tileID.y * NUM_GROUPS_X + tileID.x) * MAX_LIGHT_COUNT;
	
	for(int i = 0; i < MAX_LIGHT_COUNT && lightVisible[offset + i] != -1; ++i)
	{
		const uint lightIndex = lightVisible[offset + i];
		const vec3 a = lights[lightIndex].position - fragPos;
		const float distance = length(a);

		const vec3 diffuse = lights[lightIndex].diffuse * vColor * max(dot(normal, normalize(a)), 0.0);
		const float attenuation = 1.0 / (1.0 + lights[lightIndex].linear * distance + lights[lightIndex].quadratic * (distance * distance));
		lightning += diffuse * attenuation;
	}
    color = vec4(lightning, 1.0);
}