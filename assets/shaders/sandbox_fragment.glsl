#version 440
#pragma shader_stage(fragment)

#extension GL_KHR_vulkan_glsl : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 0, binding = 1) uniform UniformBuffer {
	int texture_index;
} u_UniformBuffer;

layout(set = 1, binding = 0) uniform sampler2D u_Textures[];

layout(location = 0) in vec2 v_FragTexCoord;

layout(location = 0) out vec4 o_ScreenColor;

void main(void)
{ 
	o_ScreenColor = texture(
		u_Textures[u_UniformBuffer.texture_index],
		v_FragTexCoord
	); 
}
