#include "Pch.hpp"
#include "Game.hpp"

namespace ExampleApp {
	struct InterleavedVertex {
		glm::vec3 pos;
		glm::vec2 tex_coord;
	};

	struct UniformBufferData {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	static std::array<InterleavedVertex, 24> interleavedVertexData = {
		InterleavedVertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)),
		InterleavedVertex(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)),
		InterleavedVertex(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f)),
		InterleavedVertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f)),
		InterleavedVertex(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)),
		InterleavedVertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f)),
		InterleavedVertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)),
		InterleavedVertex(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)),
		InterleavedVertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f)),
		InterleavedVertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)),
		InterleavedVertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f)),
		InterleavedVertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)),
		InterleavedVertex(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)),
		InterleavedVertex(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f)),
		InterleavedVertex(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)),
		InterleavedVertex(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f)),
		InterleavedVertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f)),
		InterleavedVertex(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f)),
		InterleavedVertex(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 0.0f)),
		InterleavedVertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 0.0f)),
		InterleavedVertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)),
		InterleavedVertex(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f)),
		InterleavedVertex(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 1.0f)),
		InterleavedVertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 1.0f))
	};
	static std::array<u32, 36> indexData = {
		0, 1, 2,
		2, 3, 0,
		4, 5, 6,
		6, 7, 4,
		8, 9, 10,
		10, 11, 8,
		12, 13, 14,
		14, 15, 12,
		16, 17, 18,
		18, 19, 16,
		20, 21, 22,
		22, 23, 20
	};

	Game::Game(void)
	: m_Window(1280, 720, "ExampleApp"),
	m_Renderer(m_Window)
	{
		std::filesystem::path assets_dir = Na::Context::GetExecDir() / "../../../assets/";

		Na::Shader vertex_shader(assets_dir / "shaders/vertex.glsl", Na::ShaderStageBits::Vertex);
		Na::Shader fragment_shader(assets_dir / "shaders/fragment.glsl", Na::ShaderStageBits::Fragment);

		m_Pipeline = Na::Pipeline(
			m_Renderer,
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
					Na::ShaderUniformType::TextureSampler,
					Na::ShaderStageBits::Fragment
				}
			},
			Na::PushConstantLayout{
				Na::PushConstant{
					Na::ShaderStageBits::Vertex,
					sizeof(UniformBufferData)
				}
			}
		);

		vertex_shader.destroy();
		fragment_shader.destroy();

		m_Renderer.bind_pipeline(m_Pipeline.handle());

		m_Vbo = Na::VertexBuffer(
			(u32)interleavedVertexData.size(),
			sizeof(InterleavedVertex),
			interleavedVertexData.data(),
			m_Renderer
		);
		m_Ibo = Na::IndexBuffer((u32)indexData.size(), indexData.data(), m_Renderer);

		Na::Image img = Na::Image::Load(assets_dir / "texture.png");

		m_Texture = Na::Texture(img, 0, m_Renderer);

		img.destroy();
	}

	Game::~Game(void)
	{

	}

	void Game::on_event(Na::Event& e)
	{
		switch (e.type)
		{
		case Na::Event_Type::WindowClosed:
			Na::App::should_close = true;
			break;
		}
	}

	void Game::update(double dt)
	{
		//g_Logger.fmt(Na::Trace, "fps: {}", (u32)(1.0 / dt));
	}

	void Game::draw(void)
	{
		if (m_Window.minimized())
			return;

		static auto x_StartTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::steady_clock::now() - x_StartTime).count();

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
				(float)m_Window.width() / (float)m_Window.height(),
				0.1f, 10.0f
			)
		};
		uniform_buffer_data.proj[1] *= -1.0f;

		if (!m_Renderer.clear(glm::vec4(0.11f, 0.11f, 0.13f, 1.0f)))
			return;

		m_Renderer.set_push_constant(
			Na::PushConstant{
				Na::ShaderStageBits::Vertex,
				sizeof(UniformBufferData)
			},
			&uniform_buffer_data
		);

		if (m_Texture)
			m_Ibo.draw(m_Vbo, m_Renderer);

		m_Renderer.present();
	}
} // namespace ExampleApp
