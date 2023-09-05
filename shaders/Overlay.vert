#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	float visible;
	float alternative;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec2 outUV;
layout(location = 1) out float alt;

void main() {
	gl_Position = vec4(inPosition * ubo.visible, 0.5f, 1.0f);
	outUV = inUV;
	alt = ubo.alternative;
}