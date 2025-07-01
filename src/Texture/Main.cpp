#include "Pch.hpp"

#include <Natrium/Core/Context.hpp>
#include <Natrium/Core/Window.hpp>

#include <Natrium/Graphics/Renderer.hpp>
#include <Natrium/Graphics/Pipeline.hpp>

#include <Natrium/Assets/AssetManager.hpp>

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

	Na::DeviceInitInfo device_info
	{
		.backend = Na::DeviceBackend::Vulkan
	};
	Na::Device device(device_info);

	Na::AssetManager asset_manager("assets/engine/", "bin/shaders/");

	auto vs = asset_manager.load_shader(
		"assets/shaders/textured_vertex.glsl",
		Na::Graphics::ShaderStage::Vertex
	);

	auto fs = asset_manager.load_shader(
		"assets/shaders/textured_fragment.glsl",
		Na::Graphics::ShaderStage::Fragment
	);

	auto img = asset_manager.load_asset<Na::ImageAsset>("assets/vulkan.png"s);

	// if file is not found, it will be created with default settings
	auto renderer_settings = asset_manager.load_asset<Na::RendererSettingsAsset>("renderer_settings.json");

	// sets anisotropy limit to the maximum supported by the GPU
	renderer_settings->set_max_anisotropy(Na::Device::Limits::Anisotropy());

	Na::Window window(1280, 720, "Texture Example");
	auto renderer = Na::Graphics::Renderer::Make(window, renderer_settings);

	Na::Graphics::VertexAttributes vertex_attributes(2);

	vertex_attributes.add(0, Na::Graphics::VertexAttributeType::Vec3);
	vertex_attributes.add(1, Na::Graphics::VertexAttributeType::Vec2);

	auto vbo = Na::Graphics::VertexBuffer::Make(k_Vertices.size() * sizeof(VertexData), k_Vertices.data());
	auto ibo = Na::Graphics::IndexBuffer::Make((u32)k_Indices.size(), k_Indices.data());

	auto texture = Na::Graphics::Texture::Make(img, renderer->settings());
	fs->add_uniform(0, texture);

	auto pipeline = Na::Graphics::Pipeline::Make(renderer, vertex_attributes, { vs, fs });

	while (true)
	{
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
		if (window.minimized())
			continue;

		// if returns false, it means you should continue rendering (e.g. window was resized)
		if (!renderer->begin_frame(k_ClearColor))
			continue;

		renderer->bind_pipeline(pipeline);
		renderer->draw_indexed(vbo, ibo);

		renderer->end_frame();
	}

End:
	// IMPORTANT: wait for the gpu to finish all operations before deleting any resources
	device.wait_all();

	return 0;
}
