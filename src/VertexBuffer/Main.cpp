#include <Natrium/PchBase.hpp>
#include <Natrium/Natrium.hpp>

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
	Na::ContextInitInfo context_init_info{};
	Na::Context context(context_init_info);

	Na::DeviceInitInfo device_init_info{};
	Na::Device device(device_init_info);

	Na::AssetRegistry asset_registry("assets/engine/", "bin/shaders/");

	Na::ShaderModule vs = asset_registry.create_shader_module_from_src(
		"assets/shaders/basic_vertex.glsl",
		Na::ShaderStageBits::Vertex
	);

	Na::ShaderModule fs = asset_registry.create_shader_module_from_src(
		"assets/shaders/basic_fragment.glsl",
		Na::ShaderStageBits::Fragment
	);

	// if file is not found, it will be created with default settings
	auto renderer_settings = asset_registry.load_asset<Na::RendererSettings>("renderer_settings.json");

	// sets anisotropy limit to the maximum supported by the GPU
	renderer_settings->set_max_anisotropy(Na::Device::Limits::Anisotropy());

	Na::Window window(1280, 720, "Example");
	Na::Renderer renderer(window, renderer_settings);

	Na::GraphicsPipeline pipeline(
		renderer.core(),
		Na::PipelineShaderInfos{
			vs.pipeline_shader_info(),
			fs.pipeline_shader_info()
		},
		Na::ShaderAttributeLayout{
			Na::ShaderAttributeBinding{
				.binding = 0,
				.input_rate = Na::AttributeInputRate::Vertex,
				.attributes = {
					Na::ShaderAttribute{
						.location = 0,
						.type = Na::ShaderAttributeType::Vec3
					},
					Na::ShaderAttribute{
						.location = 1,
						.type = Na::ShaderAttributeType::Vec3
					}
				}
			}
		}
	);

	Na::VertexBuffer vbo(
		k_Vertices.size() * sizeof(VertexData),
		k_Vertices.data()
	);

	Na::DeltaTime dt;

	while (true)
	{
		for (Na::Event& e : Na::PollEvents())
		{
			switch (e.type)
			{
			case Na::EventType::MouseButtonPressed:
				e.window->capture_mouse();
				break;
			case Na::EventType::KeyPressed:
				if (e.key_pressed.key == Na::Keys::k_Escape)
					e.window->release_mouse();
				break;
			case Na::EventType::WindowClosed:
				goto End;
				break;
			}
		}

		dt.calculate();
		u32 average_fps = (u32)(1.0 / dt);

		window.set_title(NA_FORMAT("FPS: {}", average_fps));

		// will crash if the window is minimized, so you should always check before rendering
		if (window.minimized())
			continue;

		// if returns false, it means you should continue rendering (e.g. window was resized)
		if (!renderer.begin_frame(k_ClearColor))
			continue;

		renderer.bind_pipeline(pipeline);
		renderer.draw_vertices(vbo, (u32)k_Vertices.size());

		renderer.end_frame();
	}

End:
	// IMPORTANT: wait for the gpu to finish all operations before deleting any resources
	device.wait_all();

	return 0;
}
