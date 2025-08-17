#include "Pch.hpp"
#include <Natrium/Main.hpp>

#include <Natrium/Core/Context.hpp>
#include <Natrium/Core/Window.hpp>
#include <Natrium/Core/Input.hpp>
#include <Natrium/Core/DeltaTime.hpp>

#include <Natrium/Graphics/Renderer.hpp>
#include <Natrium/Graphics/Pipelines.hpp>

#include <Natrium/Assets/AssetManager.hpp>

#include <Natrium/Math/Camera3dData.hpp>

using namespace Na::Primitives;

constexpr glm::vec4 k_ClearColor{ 0.1f, 0.08f, 0.15f, 1.0f };

struct VertexData {
	glm::vec3 pos;
	glm::vec3 color;
};
static constexpr std::array<VertexData, 3> k_Vertices = {
	VertexData{ glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.9f, 0.2f, 0.2f) },
	VertexData{ glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3(0.2f, 0.9f, 0.2f) },
	VertexData{ glm::vec3( 0.0f,  0.5f, 0.0f), glm::vec3(0.2f, 0.2f, 0.9f) }
};

int main(int argc, char* argv[])
{
	Na::ContextInitInfo context_info{};
	Na::Context context(context_info);

	Na::Graphics::DeviceInitInfo device_info
	{
		.backend = Na::Graphics::DeviceBackend::Vulkan
	};
	auto device = Na::Graphics::Device::Make(device_info);

	Na::AssetManager asset_manager("assets/engine/", "bin/shaders/");

	auto vs = asset_manager.load_shader(
		"assets/shaders/camera_vertex.glsl",
		Na::Graphics::ShaderStage::Vertex
	).value();

	auto fs = asset_manager.load_shader(
		"assets/shaders/basic_fragment.glsl",
		Na::Graphics::ShaderStage::Fragment
	).value();

	// if file is not found, it will be created with default settings
	auto renderer_settings = asset_manager.load_asset<Na::RendererSettingsAsset>("renderer_settings.json").value();

	// sets anisotropy limit to the maximum supported by the GPU
	renderer_settings->set_max_anisotropy(device->limits()->max_anisotropy());

	auto renderer = Na::Graphics::Renderer::Make(renderer_settings);

	auto window = Na::MakeRef<Na::Window>(1280, 720, "Camera Example");
	Na::Input input;

	auto render_target = Na::Graphics::SwapchainRenderTarget::Make(window, renderer_settings);
	renderer->bind_render_target(render_target);

	Na::Graphics::VertexAttributes vertex_attributes(2);

	vertex_attributes.add(0, Na::Graphics::VertexAttributeType::Vec3); // position
	vertex_attributes.add(1, Na::Graphics::VertexAttributeType::Vec3); // color

	auto vbo = Na::Graphics::MakeVertexBuffer(k_Vertices.size() * sizeof(VertexData));
	vbo->set_data(k_Vertices.data());

	Na::Camera3dData camera_data(glm::vec3(2.5f, 1.0f, 2.5f));
	camera_data.set_aspect_ratio((float)window->width() / (float)window->height());

	vs->set_push_constant_size((u32)camera_data.matrices().size());

	auto pipeline = Na::Graphics::TrianglePipeline::Make(render_target, vertex_attributes, {}, { vs, fs });

	Na::DeltaTime dt;

	while (true)
	{
		std::this_thread::sleep_for(16ms);

		for (Na::Event& e : Na::PollEvents())
		{
			input.on_event(e);
			switch (e.type)
			{
				case Na::EventType::MouseMoved:
				{
					if (e.window->mouse_captured())
						camera_data.rotate_with_mouse(glm::vec2(e.mouse_moved.x, e.mouse_moved.y));
					break;
				}
				case Na::EventType::MouseButtonPressed:
				{
					e.window->capture_mouse();
					camera_data.on_mouse_capture(glm::vec2(input.mouse_x(), input.mouse_y()));
					break;
				}
				case Na::EventType::KeyPressed:
				{
					if (e.key_pressed.key == Na::Keys::k_Escape)
					{
						camera_data.on_mouse_release();
						e.window->release_mouse();
					}

					break;
				}
				case Na::EventType::WindowResized:
				{
					camera_data.set_aspect_ratio((float)e.window_resized.width / (float)e.window_resized.height);
					break;
				}
				case Na::EventType::WindowClosed:
				{
					goto End;
					break;
				}
			}
		}

		dt.calculate();
		{
			float amount = 5.0f * (float)dt;
			glm::vec3 move(0.0f);

			if (input.key(Na::Keys::k_W))
				move.z = amount;
			if (input.key(Na::Keys::k_S))
				move.z = -amount;
			if (input.key(Na::Keys::k_D))
				move.x = amount;
			if (input.key(Na::Keys::k_A))
				move.x = -amount;

			camera_data.move(move);
		}

		// will crash if the window is minimized, so you should always check before rendering
		if (window->minimized())
			continue;

		// if returns false, it means you shouldn't continue rendering (e.g. window was resized)
		if (!render_target->acquire_next_image())
			continue;

		renderer->begin_frame();
		renderer->begin_render_pass(k_ClearColor);

		renderer->bind_pipeline(pipeline);

		const Na::CameraMatrices& camera_matrices = camera_data.matrices();
		renderer->set_push_constant(
			(u32)camera_matrices.size(),
			Na::Graphics::ShaderStage::Vertex,
			0,
			&camera_matrices,
			pipeline
		);

		renderer->draw_vertices(vbo, (u32)k_Vertices.size());

		renderer->end_render_pass();
		renderer->end_frame();

		render_target->present();
	}

End:
	// IMPORTANT: wait for the gpu to finish all operations before deleting any resources
	device->wait_all();

	return 0;
}
