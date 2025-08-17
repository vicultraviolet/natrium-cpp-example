#include "Pch.hpp"
#include <Natrium/Main.hpp>

#include <Natrium/Core/Context.hpp>
#include <Natrium/Core/Window.hpp>

#include <Natrium/Graphics/Renderer.hpp>
#include <Natrium/Graphics/Pipelines.hpp>

#include <Natrium/Assets/AssetManager.hpp>

using namespace Na::Primitives;

constexpr glm::vec4 k_ClearColor{ 0.1f, 0.08f, 0.15f, 1.0f };

struct VertexData {
	glm::vec3 pos;
	glm::vec2 uv_coord;
};

static constexpr std::array<VertexData, 4> k_Vertices = {
	VertexData{ glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 1.0f) },
	VertexData{ glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 1.0f) },
	VertexData{ glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec2(1.0f, 0.0f) },
	VertexData{ glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec2(0.0f, 0.0f) }
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
		.backend = Na::Graphics::DeviceBackend::Vulkan
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
	auto renderer_settings = asset_manager.load_asset<Na::RendererSettingsAsset>("renderer_settings.json").value();

	// sets anisotropy limit to the maximum supported by the GPU
	renderer_settings->set_max_anisotropy(device->limits()->max_anisotropy());

	auto renderer = Na::Graphics::Renderer::Make(renderer_settings);

	auto window = Na::MakeRef<Na::Window>(1280, 720, "Texture Example");

	auto render_target = Na::Graphics::SwapchainRenderTarget::Make(window, renderer_settings);
	renderer->bind_render_target(render_target);

	Na::Graphics::VertexAttributes vertex_attributes(2);

	vertex_attributes.add(0, Na::Graphics::VertexAttributeType::Vec3); // position
	vertex_attributes.add(1, Na::Graphics::VertexAttributeType::Vec2); // uv coord

	auto vbo = Na::Graphics::MakeVertexBuffer(k_Vertices.size() * sizeof(VertexData));
	vbo->set_data(k_Vertices.data());

	auto ibo = Na::Graphics::MakeIndexBuffer((u32)k_Indices.size());
	ibo->set_data(k_Indices.data());

	auto uniform_set_layout = Na::Graphics::UniformSetLayout::Make({
		Na::Graphics::UniformBinding{
			.binding = 0,
			.type = Na::Graphics::UniformType::Texture,
			.shader_stage = Na::Graphics::ShaderStage::Fragment
		}
	});

	auto pipeline = Na::Graphics::TrianglePipeline::Make(
		render_target,
		vertex_attributes,
		{ uniform_set_layout },
		{ vs, fs }
	);

	auto texture = Na::Graphics::Texture::Make(img, renderer->settings());
	auto uniform_set = Na::Graphics::UniformSet::Make(uniform_set_layout, renderer);
	uniform_set->bind_at(0, texture);

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
		if (!render_target->acquire_next_image())
			continue;

		renderer->begin_frame();
		renderer->begin_render_pass(k_ClearColor);

		renderer->bind_pipeline(pipeline);
		renderer->bind_uniform_set(uniform_set, pipeline);

		renderer->draw_indexed(vbo, ibo, (u32)k_Indices.size());

		renderer->end_render_pass();
		renderer->end_frame();

		render_target->present();
	}

End:
	// IMPORTANT: wait for the gpu to finish all operations before deleting any resources
	device->wait_all();

	return 0;
}
