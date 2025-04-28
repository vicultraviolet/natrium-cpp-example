#include "Pch.hpp"
#include "Game.hpp"

namespace ExampleApp {
	struct InterleavedVertex {
		glm::vec3 pos;
		glm::vec2 tex_coord;
	};

	struct Instance {
		glm::mat4 model;
	};

	struct PushConstantData {
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	struct VertexBufferData {
		std::array<InterleavedVertex, 24> vertex_data = {
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
		[[nodiscard]] inline u32 size(void) const { return (u32)vertex_data.size() * sizeof(InterleavedVertex); }
	};
	static VertexBufferData vertexBufferData{};

	struct InstanceBufferData {
		std::array<Instance, 2> instance_data = {
			Instance(glm::mat4(1.0f)),
			Instance(glm::mat4(1.0f))
		};
		[[nodiscard]] inline u32 count(void) const { return (u32)instance_data.size(); }
		[[nodiscard]] inline u32 size(void) const { return (u32)instance_data.size() * sizeof(Instance); }
	};
	InstanceBufferData instanceBufferData{};
	
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
	m_Renderer(m_Window),
	m_Camera{glm::vec3(2.5f, 1.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.1f), 45.0f},
	m_AssetRegistry(Na::Context::GetExecDir() / "../../../assets/", Na::Context::GetExecDir())
	{
		Na::ShaderModule vs = m_AssetRegistry.create_shader_module_from_src(
			"shaders/vertex.glsl",
			Na::ShaderStageBits::Vertex,
			"main"
		);
		Na::ShaderModule fs = m_AssetRegistry.create_shader_module_from_src(
			"shaders/fragment.glsl",
			Na::ShaderStageBits::Fragment,
			"main"
		);

		Na::ShaderAttributeBinding binding0{
			.binding = 0,
			.input_rate = Na::AttributeInputRate::Vertex,
			.attributes = {
				Na::ShaderAttribute{
					.location = 0,
					.type = Na::ShaderAttributeType::Vec3
				},
				Na::ShaderAttribute{
					.location = 1,
					.type = Na::ShaderAttributeType::Vec2
				}
			}
		};
		Na::ShaderAttributeBinding binding1{
			.binding = 1,
			.input_rate = Na::AttributeInputRate::Instance,
			.attributes = {
				Na::ShaderAttribute{
					.location = 2,
					.type = Na::ShaderAttributeType::Vec3
				},
				Na::ShaderAttribute{
					.location = 3,
					.type = Na::ShaderAttributeType::Float
				}
			}
		};

		m_Pipeline = Na::Pipeline(
			m_Renderer,
			Na::PipelineShaderInfos{
				vs.pipeline_shader_info(),
				fs.pipeline_shader_info()
			},
			Na::ShaderAttributeLayout{
				binding0
			},
			Na::ShaderUniformLayout{
				Na::ShaderUniform{
					.binding = 0,
					.type = Na::ShaderUniformType::StorageBuffer,
					.shader_stage = Na::ShaderStageBits::Vertex
				},
				Na::ShaderUniform{
					.binding = 1,
					.type = Na::ShaderUniformType::Texture,
					.shader_stage = Na::ShaderStageBits::Fragment
				}
			},
			Na::PushConstantLayout{
				Na::PushConstant{
					.shader_stage = Na::ShaderStageBits::Vertex,
					.size = sizeof(PushConstantData)
				}
			}
		);

		m_Renderer.bind_pipeline(m_Pipeline.handle());

		Na::AssetHandle<Na::Model> model = m_AssetRegistry.load_asset<Na::Model>("model.obj");

		m_VertexBuffer = Na::VertexBuffer(model->vertex_data_size(), model->vertices().ptr(), m_Renderer);
		m_IndexBuffer = Na::IndexBuffer((u32)model->index_count(), model->indices().ptr(), m_Renderer);

		m_InstanceBuffer = Na::StorageBuffer(instanceBufferData.size(), 0, m_Renderer);

		Na::AssetHandle<Na::Image> img = m_AssetRegistry.load_asset<Na::Image>("texture.png");
		m_Texture = Na::Texture(*img, 1, m_Renderer);

		m_AssetRegistry.free_asset("texture.png");
		m_AssetRegistry.free_asset("tree/model.obj");
	}

	Game::~Game(void)
	{

	}

	void Game::on_event(Na::Event& e)
	{
		m_Input.on_event(e);
		switch (e.type)
		{
		case Na::Event_Type::WindowClosed:
			Na::App::should_close = true;
			break;
		}
	}

	void Game::update(double dt)
	{
		float amount = 3.0f * (float)dt;
		if (m_Input.key(NA_KEY_Q))
		{
			m_Camera.pos.y += amount;
			m_Camera.eye.y += amount;
		}
		if (m_Input.key(NA_KEY_E))
		{
			m_Camera.pos.y -= amount;
			m_Camera.eye.y -= amount;
		}
		if (m_Input.key(NA_KEY_W))
		{
			m_Camera.pos.x -= amount;
			m_Camera.eye.x -= amount;
			m_Camera.pos.z -= amount;
			m_Camera.eye.z -= amount;
		}
		if (m_Input.key(NA_KEY_A))
		{
			m_Camera.pos.x -= amount;
			m_Camera.eye.x -= amount;
			m_Camera.pos.z += amount;
			m_Camera.eye.z += amount;
		}
		if (m_Input.key(NA_KEY_S))
		{
			m_Camera.pos.z += amount;
			m_Camera.eye.z += amount;
			m_Camera.pos.x += amount;
			m_Camera.eye.x += amount;
		}
		if (m_Input.key(NA_KEY_D))
		{
			m_Camera.pos.x += amount;
			m_Camera.eye.x += amount;
			m_Camera.pos.z -= amount;
			m_Camera.eye.z -= amount;
		}
		if (m_Input.key(NA_KEY_MINUS))
		{
			m_Camera.fov -= amount * 20.0f;
		}
		if (m_Input.key(NA_KEY_EQUAL))
		{
			m_Camera.fov += amount * 20.0f;
		}
		if (m_Input.key(NA_KEY_UP))
		{
			m_Camera.eye.y += amount;
		}
		if (m_Input.key(NA_KEY_DOWN))
		{
			m_Camera.eye.y -= amount;
		}
		if (m_Input.key(NA_KEY_LEFT))
		{
			m_Camera.eye.z += amount;
		}
		if (m_Input.key(NA_KEY_RIGHT))
		{
			m_Camera.eye.z -= amount;
		}
		//g_Logger.fmt(Na::Trace, "fps: {}", (u32)(1.0 / dt));
	}

	void Game::draw(void)
	{
		if (m_Window.minimized())
			return;

		static auto x_StartTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::steady_clock::now() - x_StartTime).count();

		Na::FrameData& fd = m_Renderer.begin_frame(glm::vec4(0.11f, 0.11f, 0.13f, 1.0f));
		if (fd.skipped)
			return;

		PushConstantData pcd{
			.view = glm::lookAt(
				m_Camera.pos,
				m_Camera.eye,
				glm::vec3(0.0f, 1.0f, 0.0f)
			),
			.proj = glm::perspective(
				glm::radians(m_Camera.fov),
				(float)m_Window.width() / (float)m_Window.height(),
				0.1f, 10.0f
			),
		};
		m_Renderer.set_push_constant(
			Na::PushConstant{
				.shader_stage = Na::ShaderStageBits::Vertex,
				.size = sizeof(glm::mat4) * 2
			},
			&pcd
		);

		glm::mat4& model0 = instanceBufferData.instance_data[0].model;
		glm::mat4& model1 = instanceBufferData.instance_data[1].model;

		model0 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.5f, 0.2f));
		model0 = glm::rotate(model0, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model1 = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.7f));
		model1 = glm::scale(model1, glm::vec3(0.3f, 0.3f, 0.3f));
		model1 = glm::rotate(model1, time * glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model1 = glm::rotate(model1, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		m_InstanceBuffer.set_data(&instanceBufferData, m_Renderer);
		
		m_IndexBuffer.draw(m_VertexBuffer, instanceBufferData.count(), m_Renderer);

		m_Renderer.end_frame();
	}
} // namespace ExampleApp
