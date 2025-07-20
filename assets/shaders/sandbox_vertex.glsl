#version 440
#pragma shader_stage(vertex)

#extension GL_KHR_vulkan_glsl : enable

layout(push_constant) uniform pc {
    mat4 view;
    mat4 proj;
};

layout(set = 0, binding = 0) readonly buffer InstanceBuffer {
    mat4 model[];
} u_InstanceBuffer;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) out vec2 v_FragTexCoord;

void main(void)
{
    gl_Position = proj * view * u_InstanceBuffer.model[gl_InstanceIndex] * vec4(a_Position, 1.0);
    v_FragTexCoord = a_TexCoord;
}
