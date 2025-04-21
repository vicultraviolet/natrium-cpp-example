#include "Pch.hpp"
#include <Natrium-Core/Context.hpp>
#include <Natrium-Core/Logger.hpp>

#include <Natrium-Core/Window.hpp>
#include <Natrium-Renderer/Image.hpp>
#include <Natrium-Renderer/Renderer.hpp>
#include <Natrium-Renderer/Shader.hpp>
#include <Natrium-Renderer/Pipeline.hpp>
#include <Natrium-Renderer/Buffers/VertexBuffer.hpp>
#include <Natrium-Renderer/Buffers/IndexBuffer.hpp>
#include <Natrium-Renderer/Buffers/UniformBuffer.hpp>
#include <Natrium-Renderer/Texture.hpp>

struct InterleavedVertex {
	glm::vec3 pos;
	glm::vec2 tex_coord;
};

struct NonInterleavedVertexData {
	glm::vec3 positions[24];
	glm::vec2 tex_coords[24];
};

struct UniformBufferData {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

int main(int argc, char* argv[])
{
	Na::Context context = Na::Context::Initialize();

	std::filesystem::path workspace_dir = context.GetExecDir() / "../../../";
	workspace_dir.make_preferred();
	std::filesystem::path assets_dir = workspace_dir / "assets/";

	Na::Logger<> logger{"ExampleApp", &std::clog};
	logger(Na::Info, "Hello, world!");

	Na::Window window(1280, 720, "ExampleApp");
	Na::Renderer renderer(window);

	Na::Shader vertex_shader(assets_dir / "shaders/vertex.glsl", Na::ShaderStageBits::Vertex);
	Na::Shader fragment_shader(assets_dir / "shaders/fragment.glsl", Na::ShaderStageBits::Fragment);

	Na::Pipeline pipeline(
		renderer,
		{ vertex_shader.pipeline_shader_info(), fragment_shader.pipeline_shader_info() },
		Na::ShaderAttributeLayout{
			Na::ShaderAttributeBinding{ // binding 0
				Na::ShaderAttribute{
					0, // location
					Na::ShaderAttributeType::Vec3
				},
				Na::ShaderAttribute{
					1, // location
					Na::ShaderAttributeType::Vec2
				}
			},
		},
		Na::ShaderUniformLayout{
			Na::ShaderUniform{
				0, // binding
				Na::ShaderUniformType::UniformBuffer,
				Na::ShaderStageBits::Vertex
			},
			Na::ShaderUniform{
				1, // binding
				Na::ShaderUniformType::TextureSampler,
				Na::ShaderStageBits::Fragment
			}
		}
	);
	renderer.bind_pipeline(pipeline.handle());

	InterleavedVertex interleaved_vertex_data[24] = {
		{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f) }, // 0
		{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f) }, // 1
		{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f) }, // 2
		{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f) }, // 3
		{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f) }, // 4
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f) }, // 5
		{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f) }, // 6
		{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f) }, // 7
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f) }, // 8
		{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f) }, // 9
		{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f) }, // 10
		{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f) }, // 11
		{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f) }, // 12
		{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f) }, // 13
		{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f) }, // 14
		{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f) }, // 15
		{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f) }, // 16
		{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f) }, // 17
		{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 0.0f) }, // 18
		{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 0.0f) }, // 19
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f) }, // 20
		{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f) }, // 21
		{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 1.0f) }, // 22
		{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 1.0f) }  // 23
	};														  

	NonInterleavedVertexData non_interleaved_vertex_data{
		.positions = {
			glm::vec3(-0.5f, -0.5f,  0.5f),
			glm::vec3(0.5f, -0.5f,  0.5f),
			glm::vec3(0.5f,  0.5f,  0.5f),
			glm::vec3(-0.5f,  0.5f,  0.5f),
			glm::vec3(0.5f, -0.5f, -0.5f),
			glm::vec3(-0.5f, -0.5f, -0.5f),
			glm::vec3(-0.5f,  0.5f, -0.5f),
			glm::vec3(0.5f,  0.5f, -0.5f),
			glm::vec3(-0.5f, -0.5f, -0.5f),
			glm::vec3(-0.5f, -0.5f,  0.5f),
			glm::vec3(-0.5f,  0.5f,  0.5f),
			glm::vec3(-0.5f,  0.5f, -0.5f),
			glm::vec3(0.5f, -0.5f,  0.5f),
			glm::vec3(0.5f, -0.5f, -0.5f),
			glm::vec3(0.5f,  0.5f, -0.5f),
			glm::vec3(0.5f,  0.5f,  0.5f),
			glm::vec3(-0.5f,  0.5f,  0.5f),
			glm::vec3(0.5f,  0.5f,  0.5f),
			glm::vec3(0.5f,  0.5f, -0.5f),
			glm::vec3(-0.5f,  0.5f, -0.5f),
			glm::vec3(-0.5f, -0.5f, -0.5f),
			glm::vec3(0.5f, -0.5f, -0.5f),
			glm::vec3(0.5f, -0.5f,  0.5f),
			glm::vec3(-0.5f, -0.5f,  0.5f)
		},
		.tex_coords = {
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f)
		}
	};

	u32 index_data[36] = {
		// Front face (0, 1, 2, 3)
		0, 1, 2,
		2, 3, 0,
		
		// Back face (4, 5, 6, 7)
		4, 5, 6,
		6, 7, 4,
		
		// Left face (8, 9, 10, 11)
		8, 9, 10,
		10, 11, 8,
		
		// Right face (12, 13, 14, 15)
		12, 13, 14,
		14, 15, 12,
		
		// Top face (16, 17, 18, 19)
		16, 17, 18,
		18, 19, 16,
		
		// Bottom face (20, 21, 22, 23)
		20, 21, 22,
		22, 23, 20
	};

	Na::VertexBuffer   vertex_buffer(&interleaved_vertex_data, 24 * sizeof(InterleavedVertex), 24, renderer);
	Na::IndexBuffer     index_buffer(index_data, 36, renderer);
	Na::UniformBuffer uniform_buffer(sizeof(UniformBufferData), 0, renderer);
	Na::Image  img = Na::Image::Load(assets_dir / "texture.png");
	Na::Texture              texture(img, 1, renderer);

	std::chrono::steady_clock::time_point now, last;

	bool should_close = false;
	while (!should_close)
	{
		std::this_thread::sleep_for(1ms);

		for (Na::Event& e : Na::PollEvents())
		{
			switch (e.type)
			{
			case Na::Event_Type::WindowClosed:
				should_close = true;
				break;
			case Na::Event_Type::WindowResized:
				break;
			}
		}

		now = std::chrono::steady_clock::now();
		auto difference = std::chrono::duration_cast<std::chrono::microseconds>(now - last);
		last = now;
		double dt = difference.count() / 1e+6;
		logger.fmt(Na::Trace, "fps: {}", (u32)(1.0 / dt));

		if (window.minimized())
			continue;

		static auto x_StartTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(now - x_StartTime).count();

		UniformBufferData uniform_buffer_data{
			.model = glm::rotate(
				glm::mat4(1.0f),
				time * glm::radians(90.0f),
				glm::vec3(0.0f, 0.0f, 1.0f)
			),
			.view = glm::lookAt(
				glm::vec3(2.5f, 2.5f, 2.5f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 1.0f)
			),
			.proj = glm::perspective(
				glm::radians(45.0f),
				(float)window.width() / (float)window.height(),
				0.1f, 10.0f
			)
		};
		uniform_buffer_data.proj[1] *= -1;

		uniform_buffer.set_data(&uniform_buffer_data, renderer);

		if(!renderer.clear(glm::vec4(0.11f, 0.11f, 0.13f, 1.0f)))
			continue;

		index_buffer.draw(vertex_buffer, renderer);

		renderer.present();
	}
	Na::VkContext::GetLogicalDevice().waitIdle();

	vertex_buffer.destroy();
	index_buffer.destroy();
	uniform_buffer.destroy();
	img.destroy();
	texture.destroy();

	pipeline.destroy();

	fragment_shader.destroy();
	vertex_shader.destroy();

	renderer.destroy();
	window.destroy();

	Na::Context::Shutdown();

	return 0;
}

#if (defined(NA_PLATFORM_WINDOWS) && defined(NA_CONFIG_DIST))
int APIENTRY WinMain(
	HINSTANCE hInst,
	HINSTANCE hInstPrev,
	PSTR cmdline,
	int cmdshow)
{
	return main(__argc, __argv);
}
#endif 
