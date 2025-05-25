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
	: Na::Layer(priority)
	{
		m_Camera.pos = glm::vec3(2.5f, 1.0f, 2.5f);
		m_Camera.eye = glm::vec3(0.0f, 0.0f, 0.0f);

		m_Camera.fov = 45.0f;

		glm::vec3 dir = glm::normalize(m_Camera.eye - m_Camera.pos);
		m_Camera.yaw = glm::degrees(atan2(dir.z, dir.x));
		m_Camera.pitch = glm::degrees(asin(dir.y));

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

			this->_reset_mouse(m_Input.mouse_x(), m_Input.mouse_y());
		}
	}

	void MainLayer::_on_key_press(Na::Event_KeyPressed& e)
	{
		switch (e.key)
		{
		case Na::Keys::k_Escape:
			GameContext::MainWindow().release_mouse();
			m_FirstMouse = true;
			break;
		}
	}

	void MainLayer::_on_mouse_move(Na::Event_MouseMoved& e)
	{
		if (GameContext::MainWindow().mouse_captured())
			this->_update_camera(e.x, e.y);	
	}

	void MainLayer::_reset_mouse(float x, float y)
	{
		m_Camera.last_x = x;
		m_Camera.last_y = y;
		m_FirstMouse = false;
	}

	void MainLayer::_update_camera(float x, float y)
	{
		if (m_FirstMouse)
			this->_reset_mouse(x, y);

		float x_offset = x - m_Camera.last_x;
		float y_offset = m_Camera.last_y - y;
		m_Camera.last_x = x;
		m_Camera.last_y = y;

		float sensitivity = 0.1f;
		x_offset *= sensitivity;
		y_offset *= sensitivity;

		m_Camera.yaw += x_offset;
		m_Camera.pitch += y_offset;

		if (m_Camera.pitch > 89.0f)
			m_Camera.pitch = 89.0f;

		if (m_Camera.pitch < -89.0f)
			m_Camera.pitch = -89.0f;

		glm::vec3 direction(
			cos(glm::radians(m_Camera.yaw)) * cos(glm::radians(m_Camera.pitch)),
			sin(glm::radians(m_Camera.pitch)),
			sin(glm::radians(m_Camera.yaw)) * cos(glm::radians(m_Camera.pitch))
		);
		m_Camera.eye = m_Camera.pos + glm::normalize(direction);
	}

	void MainLayer::update(double dt)
	{
		float amount = 5.0f * (float)dt;

		glm::vec3 forward = glm::normalize(m_Camera.eye - m_Camera.pos);
		forward = glm::normalize(forward);

		glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

		glm::vec3 move(0.0f);

		if (m_Input.key(Na::Keys::k_W))
			move += forward;
		if (m_Input.key(Na::Keys::k_S))
			move -= forward;
		if (m_Input.key(Na::Keys::k_D))
			move += right;
		if (m_Input.key(Na::Keys::k_A))
			move -= right;

		if (glm::length(move) > 0.0f)
		{
			move = glm::normalize(move) * amount;
			m_Camera.pos += move;
			m_Camera.eye += move;
		}

		if (m_Input.key(Na::Keys::k_Minus))
			m_Camera.fov -= amount * 20.0f;
		if (m_Input.key(Na::Keys::k_Equal))
			m_Camera.fov += amount * 20.0f;
	}

	void MainLayer::draw(void)
	{
		Na::Window& main_window = GameContext::MainWindow();
		Na::Renderer& main_renderer = GameContext::MainRenderer();

		static auto x_StartTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - x_StartTime).count();

		main_renderer.bind_pipeline(m_Pipeline);

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
