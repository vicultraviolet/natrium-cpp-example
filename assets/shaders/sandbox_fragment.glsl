#version 440
#pragma shader_stage(fragment)

#extension GL_KHR_vulkan_glsl : enable

layout(set = 1, binding = 0) uniform sampler2D u_Textures;

layout(location = 0) in vec2 v_FragTexCoord;

layout(location = 0) out vec4 o_ScreenColor;

void main(void)
{ 
	o_ScreenColor = texture(u_Textures, v_FragTexCoord); 
}
