#version 440
#pragma shader_stage(fragment)

layout(binding = 0) uniform sampler2D u_Textures;

layout(location = 0) in vec2 v_TexCoord;

layout(location = 0) out vec4 o_ScreenColor;

void main(void)
{
	o_ScreenColor = texture(u_Textures, v_TexCoord);
}
