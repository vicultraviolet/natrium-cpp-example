#include "Pch.hpp"
#include <Natrium/Main.hpp>

#include <Natrium/Core/Context.hpp>
#include <Natrium/Core/Window.hpp>

#include <Natrium/HL/Renderer_HL.hpp>
#include <Natrium/HL/UniformManager_HL.hpp>
#include <Natrium/HL/Pipeline_HL.hpp>
#include <Natrium/HL/DeviceMesh_HL.hpp>

#include <Natrium/Assets/AssetManager.hpp>

using namespace Na::Primitives;

constexpr glm::vec4 k_ClearColor{ 0.1f, 0.08f, 0.15f, 1.0f };

static constexpr std::array<Na::Vertex, 4> k_Vertices = {
	Na::Vertex{ glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 1.0f) },
	Na::Vertex{ glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 1.0f) },
	Na::Vertex{ glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec2(1.0f, 0.0f) },
	Na::Vertex{ glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec2(0.0f, 0.0f) }
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

	auto img = asset_manager.load_asset<Na::ImageAsset>("assets/vulkan.png"s).value();

	// if file is not found, it will be created with default settings
	auto renderer_settings = asset_manager.load_renderer_settings("renderer_settings.json").value();

	// sets anisotropy limit to the maximum supported by the GPU
	renderer_settings->set_max_anisotropy(device->limits()->max_anisotropy());

	auto renderer = Na::Graphics::Renderer::Make(renderer_settings);

	auto window = Na::MakeRef<Na::Window>(1280, 720, "Texture Example");

	auto display = Na::HL::Display::Make(window, renderer_settings);
	renderer->bind_render_target(display);

	Na::HL::DeviceMesh mesh(
		k_Vertices.data(), (u32)k_Vertices.size(),
		k_Indices.data(), (u32)k_Indices.size()
	);

	Na::HL::UniformManager uniform_manager;

	uniform_manager.init_layout(
		0, // layout index
		{ // bindings
			Na::Graphics::UniformBinding{
				.binding = 0,
				.type = Na::Graphics::UniformType::Texture,
				.shader_stage = Na::Graphics::ShaderStage::Fragment
			}
		}
	);

	auto texture = Na::Graphics::Texture::Make(img, renderer->settings());

	uniform_manager.create_set(
		0, // layout index
		renderer
	);

	Na::Graphics::UniformSetTextureBindingInfo texture_binding_info;

	texture_binding_info.binding = 0;
	texture_binding_info.texture = texture;

	uniform_manager.set(0)->bind(texture_binding_info);

	Na::HL::TrianglePipelineCreateInfo pipeline_info
	{
		.render_target = display,
		.shaders = { vs, fs },
		.vertex_attributes = &Na::HL::DeviceMesh::GetVertexAttributes(),
		.uniform_set_layouts = &uniform_manager.set_layouts()
	};
	Na::HL::Pipeline pipeline(pipeline_info);

	while (true)
	{
		std::this_thread::sleep_for(16ms);

		for (Na::Event& e : Na::PollEvents())
		{
			switch (e.type)
			{
			case Na::EventType::WindowClosed:
				goto End;
				break;
			}
		}

		// will crash if the window is minimized, so you should always check before rendering
		if (window->minimized())
			continue;

		// if returns false, it means you shouldn't continue rendering (e.g. window was resized)
		if (!display->acquire_next_image())
			continue;

		renderer->begin_frame();
		renderer->begin_render_pass(k_ClearColor);

		renderer->bind_pipeline(pipeline.native());
		renderer->bind_uniform_set(uniform_manager.set(0), pipeline.native());

		renderer->bind_vertex_buffer(mesh.vertex_buffer());
		renderer->bind_index_buffer(mesh.index_buffer());

		renderer->draw_indexed(mesh.index_count());

		renderer->end_render_pass();
		renderer->end_frame();

		display->present();
	}

End:
	// IMPORTANT: wait for the gpu to finish all operations before deleting any resources
	device->wait_all();

	return 0;
}
