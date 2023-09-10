#version 450
#extension GL_ARB_separate_shader_objects : enable

const int numInstances = 30;
const int numPointLights = 1;
const float beta = 4.0f;
const float g = 1.5;	

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec3 fragColor;
 
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
	vec3 DlightDir;
	vec3 DlightColor;
	vec3 AmbLightColor;	
	vec3 eyePos;
	vec3 PlightPos[numPointLights];
	vec3 PlightColor;	
} gubo;

layout(set = 1, binding = 0) uniform UniformBufferObject {
	float visible;
	float amb;
	float gamma;
	vec3 sColor;
	mat4 mvpMat[numInstances];
	mat4 mMat[numInstances];
	mat4 nMat[numInstances];
	vec3 emission;
} ubo;

vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 MD, float sigma);

void main() {
	float sigma = 1.1f;
	vec3 N = normalize(fragNorm);				
	vec3 V = normalize(gubo.eyePos - fragPos);	

	vec3 finalColor = vec3(0.0);
	vec3 MA = fragColor.rgb * gubo.AmbLightColor;
	vec3 MD = fragColor.rgb;

	for(int i = 0; i < numPointLights; i++){
		vec3 L = normalize(gubo.PlightPos[i] - fragPos);
		vec3 lightColor = gubo.PlightColor.xyz * pow(g/length(gubo.PlightPos[i] - fragPos), beta);			
	
		//vec3 MD = fragColor.rgb * 0.99f * clamp(dot(N, L),0.0,1.0);
		//vec3 MS = vec3(pow(clamp(dot(N, normalize(L + V)),0.0,1.0), 160.0f));

		vec3 DiffSpec = BRDF(V, N, L, MD, sigma);
		finalColor += DiffSpec * lightColor.rgb;
	}
	finalColor = clamp(finalColor + MA, 0.0, 1.0);

	outColor = vec4(finalColor, 1.0f);
}

vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 MD, float sigma) {
	//vec3 V  - direction of the viewer
	//vec3 N  - normal vector to the surface
	//vec3 L  - light vector (from the light model)
	//vec3 Md - main color of the surface
	//float sigma - Roughness of the model
	float thetai=acos(dot(L,N));
	float thetar=acos(dot(V,N));
	float alpha=max(thetai,thetar);
	float beta=min(thetai,thetar);
	float A = 1-0.5*(pow(sigma,2)/(pow(sigma,2)+0.33));
	float B = 0.45*(pow(sigma,2)/(pow(sigma,2)+0.09));

	vec3 vi=normalize(L-(L*N)*N);
	vec3 vr=normalize(V-(V*N)*N);
	float G = max(0,dot(vi,vr));

	vec3 l = MD * clamp(dot(L,N),0.0,1.0);
	
	return l*(A+B*G*sin(alpha)*tan(beta));
}