#version 440
#pragma shader_stage(vertex)

layout(push_constant) uniform pc {
    mat4 view;
    mat4 proj;
};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;

layout(location = 0) out vec4 v_Color;

void main(void)
{
	gl_Position = proj * view * vec4(a_Position, 1.0);
	v_Color = vec4(a_Color, 1.0);
}
