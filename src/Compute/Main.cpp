#include "Pch.hpp"
#include <Natrium/Main.hpp>

#include <Natrium/Core/Context.hpp>
#include <Natrium/Core/Window.hpp>
#include <Natrium/Core/Input.hpp>

#include <Natrium/Graphics/Renderer.hpp>
#include <Natrium/HL/Pipeline_HL.hpp>

#include <Natrium/Assets/HostImage.hpp>
#include <Natrium/Graphics/DeviceImage.hpp>
#include <Natrium/Graphics/Sampler.hpp>

#include <Natrium/HL/DeviceMesh_HL.hpp>

#include <Natrium/Assets/AssetManager.hpp>

#include <stb/include/stb/stb_image.h>
#include <stb/include/stb/stb_image_write.h>

using namespace Na::Primitives;

constexpr glm::vec4 k_ClearColor{ 0.1f, 0.08f, 0.15f, 1.0f };

static constexpr std::array<Na::Vertex, 4> k_Vertices = {
	Na::Vertex{ glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
	Na::Vertex{ glm::vec3( 1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
	Na::Vertex{ glm::vec3( 1.0f,  1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
	Na::Vertex{ glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec2(0.0f, 0.0f) }
};

static constexpr std::array<u32, 6> k_Indices = {
	0, 1, 2,
	2, 3, 0
};

int main(int argc, char* argv[])
{
	Na::ContextInitInfo context_info{};
	Na::Context context(context_info);

	Na::Graphics::DeviceInitInfo device_info
	{
		.backend = Na::Graphics::DeviceBackend::Vulkan,
		.required_extensions = {
			Na::Graphics::DeviceExtension::Swapchain
		}
	};
	auto device = Na::Graphics::Device::Make(device_info);

	Na::AssetManager asset_manager("assets/engine/", "bin/shaders/");

	auto vs = asset_manager.load_shader(
		"assets/shaders/textured_vertex.glsl",
		Na::Graphics::ShaderStage::Vertex
	).value();

	auto fs = asset_manager.load_shader(
		"assets/shaders/textured_fragment.glsl",
		Na::Graphics::ShaderStage::Fragment
	).value();

	auto cs = asset_manager.load_shader(
		"assets/shaders/basic_compute.glsl",
		Na::Graphics::ShaderStage::Compute
	).value();

	// if file is not found, it will be created with default settings
	auto renderer_settings = asset_manager.load_renderer_settings("renderer_settings.json").value();

	// sets anisotropy limit to the maximum supported by the GPU
	renderer_settings->set_max_anisotropy(device->limits()->max_anisotropy());

	auto renderer = Na::Graphics::Renderer::Make(renderer_settings);

	auto window = Na::Ref<Na::Window>::Make(1280, 720, "Renderer Example");

	auto render_target = Na::Graphics::SwapchainRenderTarget::Make(window, renderer_settings);
	renderer->bind_render_target(render_target);

	Na::Graphics::DeviceImageCreateInfo img_info
	{
		.width = 1280,
		.height = 720,

		.layer_count = 1,

		.format = Na::Graphics::ImageFormat::Rgba8,

		.type = Na::Graphics::DeviceImageTypeFlags::Storage
			  | Na::Graphics::DeviceImageTypeFlags::Sampled
			  | Na::Graphics::DeviceImageTypeFlags::TransferDst
			  | Na::Graphics::DeviceImageTypeFlags::TransferSrc
	};
	auto img = Na::Graphics::DeviceImage::Make(img_info);
	auto img2 = Na::Graphics::DeviceImage::Make(img_info);

	Na::Graphics::SamplerCreateInfo sampler_info
	{
		.oversampling_filter = Graphics::SamplerFilter::Linear,
		.undersampling_filter = Graphics::SamplerFilter::Linear,

		.adress_mode_u = Graphics::SamplerAddressMode::Repeat,
		.adress_mode_v = Graphics::SamplerAddressMode::Repeat,

		.renderer_settings = renderer_settings
	};
	auto sampler = Na::Graphics::Sampler::Make(sampler_info);

	auto ubo = Na::Graphics::MakeUniformBuffer(sizeof(glm::ivec2), renderer_settings->max_frames_in_flight());

	Na::HL::UniformManager uniform_manager_compute;

	uniform_manager_compute.init_layout(
		0, // set 0,
		{
			Na::Graphics::UniformBinding
			{
				.binding = 0,
				.type = Na::Graphics::UniformType::StorageImage,
				.shader_stage = Na::Graphics::ShaderStage::Compute
			},
			Na::Graphics::UniformBinding
			{
				.binding = 1,
				.type = Na::Graphics::UniformType::UniformMultibuffer,
				.shader_stage = Na::Graphics::ShaderStage::Compute
			},
		}
	);

	uniform_manager_compute.create_set(
		0, // set 0
		renderer
	);

	Na::Graphics::UniformSetStorageImageBindingInfo storage_img_bind_info;

	storage_img_bind_info.binding = 0;
	storage_img_bind_info.img = img;

	uniform_manager_compute.set(0)->bind(storage_img_bind_info);

	Na::Graphics::UniformSetBufferBindingInfo ubo_bind_info;

	ubo_bind_info.binding = 1;
	ubo_bind_info.buffer = ubo;
	ubo_bind_info.type = Na::Graphics::BufferTypeFlags::UniformBuffer;

	uniform_manager_compute.set(0)->bind(ubo_bind_info);

	Na::HL::ComputePipelineCreateInfo compute_pipeline_info
	{
		.shader = cs,
		.uniform_set_layouts = &uniform_manager_compute.set_layouts()
	};
	Na::HL::Pipeline compute_pipeline(compute_pipeline_info);

	glm::uvec3 compute_workgroup_count(
		((float)img->width()  + 7.0f) / 8.0f,
		((float)img->height() + 7.0f) / 8.0f,
		1
	);

	Na::HL::DeviceMesh mesh(
		k_Vertices.data(), (u32)k_Vertices.size(),
		k_Indices.data(), (u32)k_Indices.size()
	);

	Na::HL::UniformManager uniform_manager_triangle;

	uniform_manager_triangle.init_layout(
		0, // layout index
		{ // bindings
			Na::Graphics::UniformBinding{
				.binding = 0,
				.type = Na::Graphics::UniformType::Texture,
				.shader_stage = Na::Graphics::ShaderStage::Fragment
			}
		}
	);

	uniform_manager_triangle.create_set(
		0, // layout index
		renderer
	);

	Na::Graphics::UniformSetTextureBindingInfo texture_binding_info;

	texture_binding_info.binding = 0;
	texture_binding_info.texture_info = { img2, sampler };

	uniform_manager_triangle.set(0)->bind(texture_binding_info);

	Na::HL::TrianglePipelineCreateInfo triangle_pipeline_info
	{
		.render_target = render_target,
		.shaders = { vs, fs },
		.vertex_attributes = &Na::HL::DeviceMesh::GetVertexAttributes(),
		.uniform_set_layouts = &uniform_manager_triangle.set_layouts()
	};
	Na::HL::Pipeline triangle_pipeline(triangle_pipeline_info);

	Na::Input input;

	Na::Graphics::DeviceImageBarrierInfo barrier_info;

	while (true)
	{
		std::this_thread::sleep_for(16ms);

		for (Na::Event& e : Na::PollEvents())
		{
			switch (e.type)
			{
				case Na::EventType::KeyPressed:
				{
					barrier_info = {
						.new_img_state = Na::Graphics::DeviceImageState::TransferDst,

						.before = {
							Na::Graphics::BarrierStageBits::Earliest,
							Na::Graphics::BarrierOperationBits::None
						},

						.after = {
							Na::Graphics::BarrierStageBits::Transfer,
							Na::Graphics::BarrierOperationBits::TransferWrite
						}
					};

					img2->barrier(barrier_info);

					barrier_info = {
						.new_img_state = Na::Graphics::DeviceImageState::TransferSrc,

						.before = {
							Na::Graphics::BarrierStageBits::FragmentShader,
							Na::Graphics::BarrierOperationBits::ShaderRead
						},

						.after = {
							Na::Graphics::BarrierStageBits::Transfer,
							Na::Graphics::BarrierOperationBits::TransferRead
						}
					};

					img->barrier(barrier_info);

					img2->copy_from_img(img);

					barrier_info = {
						.new_img_state = Na::Graphics::DeviceImageState::StorageImage,

						.before = {
							Na::Graphics::BarrierStageBits::Transfer,
							Na::Graphics::BarrierOperationBits::TransferRead
						},

						.after = {
							Na::Graphics::BarrierStageBits::ComputeShader,
							Na::Graphics::BarrierOperationBits::ShaderWrite
						}
					};
					img->barrier(barrier_info);
				}

				img2->copy_from_img(img);
				break;
			case Na::EventType::WindowClosed:
				goto End;
				break;
			}

			input.on_event(e);
		}

		// will crash if the window is minimized, so you should always check before rendering
		if (window->minimized())
			continue;

		// if returns false, it means you shouldn't continue rendering (e.g. window was resized)
		if (!render_target->acquire_next_image())
			continue;

		renderer->begin_frame();

		if (input.mouse_button(Na::MouseButtons::k_Left))
		{
			glm::ivec2 mouse_pos = { (int)input.mouse_x(), (int)input.mouse_y() };

			ubo->set_subdata(&mouse_pos, renderer->current_frame_index());

			barrier_info = {
				.new_img_state = Na::Graphics::DeviceImageState::StorageImage,

				.before = {
					Na::Graphics::BarrierStageBits::FragmentShader,
					Na::Graphics::BarrierOperationBits::ShaderRead
				},

				.after = {
					Na::Graphics::BarrierStageBits::ComputeShader,
					Na::Graphics::BarrierOperationBits::ShaderWrite
				}
			};
			renderer->img_barrier(img, barrier_info);

			renderer->bind_pipeline(compute_pipeline.native());
			renderer->bind_uniform_set(
				uniform_manager_compute.set(0),
				compute_pipeline.native()
			);

			renderer->dispatch_compute(compute_workgroup_count);
		}

		barrier_info = {
			.new_img_state = Na::Graphics::DeviceImageState::Texture,

			.before = {
				Na::Graphics::BarrierStageBits::Transfer,
				Na::Graphics::BarrierOperationBits::TransferWrite
			},

			.after = {
				Na::Graphics::BarrierStageBits::FragmentShader,
				Na::Graphics::BarrierOperationBits::ShaderRead
			}
		};
		renderer->img_barrier(img2, barrier_info);

		renderer->begin_render_pass(k_ClearColor);

		renderer->bind_pipeline(triangle_pipeline.native());
		renderer->bind_uniform_set(
			uniform_manager_triangle.set(0),
			triangle_pipeline.native()
		);

		renderer->bind_vertex_buffer(mesh.vertex_buffer());
		renderer->bind_index_buffer(mesh.index_buffer());

		renderer->draw_indexed(mesh.index_count());

		renderer->end_render_pass();

		renderer->end_frame();

		render_target->present();
	}
End:

	barrier_info = {
		.new_img_state = Na::Graphics::DeviceImageState::TransferSrc,

		.before = {
			Na::Graphics::BarrierStageBits::Earliest,
			Na::Graphics::BarrierOperationBits::None
		},

		.after = {
			Na::Graphics::BarrierStageBits::Transfer,
			Na::Graphics::BarrierOperationBits::TransferRead
		}
	};

	img2->barrier(barrier_info);
	
	auto buf = img2->copy_to_buffer();

	stbi_write_png(
		"out.png",
		img2->width(),
		img2->height(),
		Na::Graphics::ImageFormat_GetChannelCount(img2->format()),
		buf->map(),
		img2->width()* Na::Graphics::ImageFormat_GetChannelCount(img2->format())
	);

	// IMPORTANT: wait for the gpu to finish all operations before deleting any resources
	device->wait_all();

	return 0;
}
