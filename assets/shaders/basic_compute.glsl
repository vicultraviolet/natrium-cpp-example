#version 450
#pragma shader_stage(compute)

#extension GL_KHR_vulkan_glsl : enable

layout(local_size_x = 8, local_size_y = 8) in;

layout(set = 0, binding = 0, rgba8) uniform writeonly image2D u_StorageImage;
layout(set = 0, binding = 1) uniform UniformBuffer {
	ivec2 mouse_coord;
} u_UniformBuffer;

void main(void)
{
	ivec2 tex_coord = ivec2(gl_GlobalInvocationID.xy);

	imageStore(u_StorageImage, u_UniformBuffer.mouse_coord, vec4(1.0, 1.0, 1.0, 1.0));
}
