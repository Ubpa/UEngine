#version 330 core

#define PI 3.1415926
#define EPSILON 0.000001

out vec4 FragColor;

in vec2 TexCoords;

// layout
//      x   y   z   w
// 0    [albedo]  roughness
// 1    [normal]  metalness
// 2    [ pos  ]    0
// 3    0   0   0   0

uniform sampler2D gbuffer0;
uniform sampler2D gbuffer1;
uniform sampler2D gbuffer2;
uniform sampler2D gbuffer3;

uniform vec3 camera_pos;
uniform vec3 pointlight_pos;
uniform vec3 pointlight_radiance;

struct PointLight {
	vec3 position;
	vec3 radiance;
};

#define MAX_POINT_LIGHT_NUM 8
uniform unsigned int pointlight_num;
uniform PointLight pointlights[MAX_POINT_LIGHT_NUM];

vec3 fresnel(vec3 albedo, float metalness, float cos_theta) {
	float reflectance = 0.04;
	vec3 F0 = mix(vec3(reflectance), albedo, metalness);
	float x = 1 - cos_theta;
	float x2 = x * x;
	float x5 = x2 * x2 * x;
	return F0 + (1-F0)*x5;
}

float GGX_G(float alpha, vec3 L, vec3 V, vec3 N) {
	float alpha2 = alpha * alpha;
	
	float cos_sthetai = dot(L, N);
	float cos_sthetao = dot(V, N);
	
	float tan2_sthetai = 1 / (cos_sthetai * cos_sthetai) - 1;
	float tan2_sthetao = 1 / (cos_sthetao * cos_sthetao) - 1;
	
	return step(cos_sthetai, 0) * step(cos_sthetai, 0) * 2 / (sqrt(1 + alpha2*tan2_sthetai) + sqrt(1 + alpha2*tan2_sthetao));
}

float GGX_D(float alpha, vec3 N, vec3 H) {
	float alpha2 = alpha * alpha;
	float cos_stheta = dot(H, N);
	float x = 1 + (alpha2 - 1) * cos_stheta * cos_stheta;
	float denominator = PI * x * x;
	return step(cos_stheta, 0) * alpha2 / denominator;
}

void main() {
	vec4 data0 = texture(gbuffer0, TexCoords);
	vec4 data1 = texture(gbuffer1, TexCoords);
	vec4 data2 = texture(gbuffer2, TexCoords);
	vec4 data3 = texture(gbuffer3, TexCoords);
	
	vec3 albedo = data0.xyz;
	float roughness = data0.w;
	vec3 N = data1.xyz;
	float metalness = data1.w;
	vec3 pos = data2.xyz;
	
	float alpha = roughness * roughness;
	vec3 V = normalize(camera_pos - pos); // frag to camera
	
	vec3 Lo = vec3(0);
	
	// point light
	for(unsigned int i = 0u; i < pointlight_num; i++) {
		vec3 fragTolight = pointlights[i].position - pos; // frag to light
		float dist2 = dot(fragTolight, fragTolight);
		float dist = sqrt(dist2);
		vec3 L = fragTolight / dist; // normalized
		vec3 H = normalize(L + V);
		
		float cos_theta = dot(N, L);
		
		vec3 fr = fresnel(albedo, metalness, cos_theta);
		float D = GGX_D(alpha, N, H);
		float G = GGX_G(alpha, L, V, N);
		
		vec3 diffuse = (1-fr)*(1-metalness)*albedo/PI;
		
		vec3 specular = fr * D * G / (4 * max(dot(L, N)*dot(V, N), EPSILON));
		
		vec3 brdf = diffuse + specular;
		Lo += brdf * pointlights[i].radiance * max(cos_theta, 0) / dist2;
	}
	
	FragColor = vec4(Lo, 1);
}
