#include "Pch.hpp"
#include "MainLayer.hpp"

#include "GameContext.hpp"

namespace ExampleApp {
	struct PushConstantData {
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	struct Instance {
		glm::mat4 model;
	};

	struct InstanceBufferData {
		std::array<Instance, 2> instance_data = {
			Instance(glm::mat4(1.0f)),
			Instance(glm::mat4(1.0f))
		};
		[[nodiscard]] inline u32 count(void) const { return (u32)instance_data.size(); }
		[[nodiscard]] inline u32 size(void) const { return (u32)instance_data.size() * sizeof(Instance); }
	};
	InstanceBufferData instanceBufferData{};

	MainLayer::MainLayer(i64 priority)
	: Na::Layer(priority),
	m_Camera{ glm::vec3(2.5f, 1.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.1f), 45.0f }
	{
		Na::Renderer& main_renderer = GameContext::MainRenderer();

		Na::AssetHandle<Na::Model> model = GameContext::AssetRegistry().load_asset<Na::Model>("model.obj");
		Na::AssetHandle<Na::Image> img = GameContext::AssetRegistry().load_asset<Na::Image>("texture.png");

		Na::ShaderModule vs = GameContext::AssetRegistry().create_shader_module_from_src(
			"shaders/vertex.glsl",
			Na::ShaderStageBits::Vertex,
			"main"
		);
		Na::ShaderModule fs = GameContext::AssetRegistry().create_shader_module_from_src(
			"shaders/fragment.glsl",
			Na::ShaderStageBits::Fragment,
			"main"
		);

		m_Pipeline = Na::GraphicsPipeline(
			main_renderer,
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
							.type = Na::ShaderAttributeType::Vec2
						}
					}
				}
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

		m_VertexBuffer = Na::VertexBuffer(model->vertex_data_size(), model->vertices().ptr(), main_renderer);
		m_IndexBuffer = Na::IndexBuffer((u32)model->index_count(), model->indices().ptr(), main_renderer);

		m_InstanceBuffer = Na::StorageBuffer(instanceBufferData.size(), main_renderer);
		m_InstanceBuffer.bind_to_pipeline(0, m_Pipeline);

		m_Texture = Na::Texture(*img, main_renderer);
		m_Texture.bind_to_pipeline(1, m_Pipeline);

		GameContext::AssetRegistry().free_asset("texture.png");
		GameContext::AssetRegistry().free_asset("tree/model.obj");
	}

	void MainLayer::on_event(Na::Event& e)
	{
		m_Input.on_event(e);
	}

	void MainLayer::update(double dt)
	{
		float amount = 3.0f * (float)dt;
		if (m_Input.key(Na::Keys::k_Q))
		{
			m_Camera.pos.y += amount;
			m_Camera.eye.y += amount;
		}
		if (m_Input.key(Na::Keys::k_E))
		{
			m_Camera.pos.y -= amount;
			m_Camera.eye.y -= amount;
		}
		if (m_Input.key(Na::Keys::k_W))
		{
			m_Camera.pos.x -= amount;
			m_Camera.eye.x -= amount;
			m_Camera.pos.z -= amount;
			m_Camera.eye.z -= amount;
		}
		if (m_Input.key(Na::Keys::k_A))
		{
			m_Camera.pos.x -= amount;
			m_Camera.eye.x -= amount;
			m_Camera.pos.z += amount;
			m_Camera.eye.z += amount;
		}
		if (m_Input.key(Na::Keys::k_S))
		{
			m_Camera.pos.z += amount;
			m_Camera.eye.z += amount;
			m_Camera.pos.x += amount;
			m_Camera.eye.x += amount;
		}
		if (m_Input.key(Na::Keys::k_D))
		{
			m_Camera.pos.x += amount;
			m_Camera.eye.x += amount;
			m_Camera.pos.z -= amount;
			m_Camera.eye.z -= amount;
		}
		if (m_Input.key(Na::Keys::k_Minus))
		{
			m_Camera.fov -= amount * 20.0f;
		}
		if (m_Input.key(Na::Keys::k_Equal))
		{
			m_Camera.fov += amount * 20.0f;
		}
		if (m_Input.key(Na::Keys::k_Up))
		{
			m_Camera.eye.y += amount;
		}
		if (m_Input.key(Na::Keys::k_Down))
		{
			m_Camera.eye.y -= amount;
		}
		if (m_Input.key(Na::Keys::k_Left))
		{
			m_Camera.eye.z += amount;
		}
		if (m_Input.key(Na::Keys::k_Right))
		{
			m_Camera.eye.z -= amount;
		}
	}

	void MainLayer::draw(Na::FrameData& fd)
	{
		Na::Window& main_window = GameContext::MainWindow();

		static auto x_StartTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - x_StartTime).count();

		fd.bind_pipeline(m_Pipeline);

		PushConstantData pcd{
			.view = glm::lookAt(
				m_Camera.pos,
				m_Camera.eye,
				glm::vec3(0.0f, 1.0f, 0.0f)
			),
			.proj = glm::perspective(
				glm::radians(m_Camera.fov),
				(float)main_window.width() / (float)main_window.height(),
				0.1f, 10.0f
			),
		};
		fd.set_push_constant(
			Na::PushConstant{
				.shader_stage = Na::ShaderStageBits::Vertex,
				.size = sizeof(glm::mat4) * 2
			},
			&pcd,
			m_Pipeline
		);

		glm::mat4& model0 = instanceBufferData.instance_data[0].model;
		glm::mat4& model1 = instanceBufferData.instance_data[1].model;

		model0 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.5f, 0.2f));
		model0 = glm::rotate(model0, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model1 = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.5f, 0.7f));
		model1 = glm::scale(model1, glm::vec3(0.3f, 0.3f, 0.3f));
		model1 = glm::rotate(model1, time * glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model1 = glm::rotate(model1, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		m_InstanceBuffer.set_data(&instanceBufferData, fd);

		m_IndexBuffer.draw(m_VertexBuffer, instanceBufferData.count(), fd);
	}
} // namespace ExampleApp
