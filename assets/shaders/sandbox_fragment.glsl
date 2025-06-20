#version 440
#pragma shader_stage(fragment)

layout(binding = 1) uniform sampler2DArray u_Textures;

layout(location = 0) in vec2 v_FragTexCoord;
layout(location = 1) flat in uint v_TextureIndex;

layout(location = 0) out vec4 o_ScreenColor;

void main(void)
{ 
	o_ScreenColor = texture(u_Textures, vec3(v_FragTexCoord, v_TextureIndex)); 
}
