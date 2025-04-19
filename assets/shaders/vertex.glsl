#version 450
#pragma shader_stage(vertex)

layout(binding = 0) uniform Transform {
    mat4 transform;
} u_Transform;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) out vec2 o_FragTexCoord;

void main(void)
{
    gl_Position = u_Transform.transform * vec4(a_Position, 1.0);
    o_FragTexCoord = a_TexCoord;
}
