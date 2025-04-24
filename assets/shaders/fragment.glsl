#version 450
#pragma shader_stage(fragment)

layout(binding = 0) uniform sampler2D u_TextureSampler;

layout(location = 0) in vec2 o_FragTexCoord;

layout(location = 0) out vec4 o_ScreenColor;

void main(void)
{ 
	o_ScreenColor = texture(u_TextureSampler, o_FragTexCoord); 
}
