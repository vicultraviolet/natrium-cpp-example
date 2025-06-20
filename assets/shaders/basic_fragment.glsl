#version 440
#pragma shader_stage(fragment)

layout(location = 0) in vec4 v_Color;

layout(location = 0) out vec4 o_ScreenColor;

void main(void)
{
	o_ScreenColor = v_Color; 
}
