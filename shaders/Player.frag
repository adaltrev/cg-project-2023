#version 450
#extension GL_ARB_separate_shader_objects : enable


const int numPointLights = 10;
const float beta = 2.0f;
const float g = 1.5;	

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

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
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
} ubo;

layout(set = 1, binding = 1) uniform sampler2D tex;

void main() {
	vec3 N = normalize(fragNorm);				
	vec3 V = normalize(gubo.eyePos - fragPos);	
	vec3 MA = texture(tex, fragUV).rgb * gubo.AmbLightColor;
	vec3 MD = texture(tex, fragUV).rgb;
	vec3 finalColor = vec3(0);
	

	for(int i = 0; i < numPointLights; i++){
		vec3 L = normalize(gubo.PlightPos[i] - fragPos);
		vec3 lightColor = gubo.PlightColor.xyz * pow(g/length(gubo.PlightPos[i] - fragPos), beta);			
	
		vec3 r = -reflect(L,N);
		vec3 fdiffuse = MD*clamp(dot(L,N),0.0,1.0);
		vec3 fspecular = vec3(1.0f) * pow(clamp(dot(V,r),0.0,1.0), ubo.gamma);
		
		finalColor += (fdiffuse + fspecular) * lightColor.rgb;
	}
	finalColor = clamp(finalColor + MA, 0.0, 1.0);

	outColor = vec4(finalColor, 1.0f);

	
}

