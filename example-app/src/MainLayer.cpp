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
	: Na::Layer(priority), m_Camera(glm::vec3(2.5f, 1.0f, 2.5f))
	{
		Na::Renderer& main_renderer = GameContext::MainRenderer();
		Na::AssetRegistry& asset_registry = GameContext::AssetRegistry();

		Na::AssetHandle<Na::Model> model = asset_registry.load_asset<Na::Model>("assets/model.obj");
		Na::AssetHandle<Na::Image> img1  = asset_registry.load_asset<Na::Image>("assets/texture.png");
		Na::AssetHandle<Na::Image> img2  = asset_registry.load_asset<Na::Image>("assets/texture2.png");

		Na::ShaderModule vs = asset_registry.create_shader_module_from_src(
			"assets/shaders/vertex.glsl",
			Na::ShaderStageBits::Vertex,
			"main"
		);
		Na::ShaderModule fs = asset_registry.create_shader_module_from_src(
			"assets/shaders/fragment.glsl",
			Na::ShaderStageBits::Fragment,
			"main"
		);

		m_Pipeline = Na::GraphicsPipeline(
			main_renderer.core(),
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

		m_VertexBuffer = Na::VertexBuffer(model->vertex_data_size(), model->vertices().ptr());
		m_IndexBuffer = Na::IndexBuffer(model->index_count(), model->indices().ptr());

		m_InstanceBuffer = Na::StorageBuffer(instanceBufferData.size(), main_renderer.core().settings());
		m_Pipeline.bind_uniform(0, m_InstanceBuffer);
		
		m_Texture = Na::Texture({ img1, img2 }, main_renderer.core().settings());
		m_Pipeline.bind_uniform(1, m_Texture);

		GameContext::AssetRegistry().free_asset("assets/texture.png");
		GameContext::AssetRegistry().free_asset("assets/texture2.png");
		GameContext::AssetRegistry().free_asset("assets/model.obj");
	}

	void MainLayer::on_event(Na::Event& e)
	{
		m_Input.on_event(e);

		switch (e.type)
		{
		case Na::EventType::MouseButtonPressed:
			this->_on_mouse_button_press(e.mouse_button_pressed);
			break;
		case Na::EventType::KeyPressed:
			this->_on_key_press(e.key_pressed);
			break;
		case Na::EventType::MouseMoved:
			this->_on_mouse_move(e.mouse_moved);
			break;
		}
	}

	void MainLayer::_on_mouse_button_press(Na::Event_MouseButtonPressed& e)
	{
		if (e.button == Na::MouseButtons::k_Left)
		{
			GameContext::MainWindow().capture_mouse();

			m_Camera.on_mouse_capture(glm::vec2(m_Input.mouse_x(), m_Input.mouse_y()));
		}
	}

	void MainLayer::_on_key_press(Na::Event_KeyPressed& e)
	{
		switch (e.key)
		{
		case Na::Keys::k_Escape:
			GameContext::MainWindow().release_mouse();
			m_Camera.on_mouse_release();
			break;
		}
	}

	void MainLayer::_on_mouse_move(Na::Event_MouseMoved& e)
	{
		if (GameContext::MainWindow().mouse_captured())
			m_Camera.rotate_with_mouse(glm::vec2(e.x, e.y));
	}

	void MainLayer::update(double dt)
	{
		float amount = 5.0f * (float)dt;
		glm::vec3 move(0.0f);

		if (m_Input.key(Na::Keys::k_W))
			move.z = amount;
		if (m_Input.key(Na::Keys::k_S))
			move.z = -amount;
		if (m_Input.key(Na::Keys::k_D))
			move.x = amount;
		if (m_Input.key(Na::Keys::k_A))
			move.x = -amount;

		m_Camera.move(move);
	}

	void MainLayer::draw(void)
	{
		Na::Window& main_window = GameContext::MainWindow();
		Na::Renderer& main_renderer = GameContext::MainRenderer();

		static auto x_StartTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - x_StartTime).count();

		main_renderer.bind_pipeline(m_Pipeline);

		PushConstantData pcd{
			.view = m_Camera.calculate_view(),
			.proj = m_Camera.calculate_projection(
				(float)main_window.width() / (float)main_window.height(),
				0.01f, 100.0f
			)
		};
		main_renderer.set_push_constant(
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

		main_renderer.set_descriptor_buffer(&m_InstanceBuffer, &instanceBufferData);
		main_renderer.draw_indexed(m_VertexBuffer, m_IndexBuffer, instanceBufferData.count());
	}
} // namespace ExampleApp
