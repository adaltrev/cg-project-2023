#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragUV;
layout(location = 1) in float alt;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D tex;
layout(binding = 2) uniform sampler2D texAlt;

void main() {
	if(alt==1.0){
		outColor = texture(texAlt, fragUV);
	}
	else{
		outColor = texture(tex, fragUV);
	}
}