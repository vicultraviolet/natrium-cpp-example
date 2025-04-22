#version 450
#pragma shader_stage(vertex)

#extension GL_NV_uniform_buffer_std430_layout : enable

layout(push_constant, std430) uniform pc {
    mat4 model;
    mat4 view;
    mat4 proj;
};

layout(binding = 0) uniform Transform {
    mat4 model;
    mat4 view;
    mat4 proj;
} u_Transform;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) out vec2 o_FragTexCoord;

void main(void)
{
    gl_Position = proj * view * model * vec4(a_Position, 1.0);
    o_FragTexCoord = a_TexCoord;
}
