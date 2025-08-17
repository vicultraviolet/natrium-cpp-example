#include "Pch.hpp"
#include "MainLayer.hpp"

namespace Sandbox {
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
	  m_Camera(glm::vec3(2.5f, 1.0f, 2.5f)),
	  m_MainWindow(Na::Application::Get().window()),
	  m_Renderer(Na::Application::Get().renderer()),
	  m_RenderTarget(Na::Application::Get().render_target())
	{
		Na::AssetManager& asset_manager = Na::Application::Get().asset_manager();

		auto renderer_settings = asset_manager.load_asset<Na::RendererSettingsAsset>("renderer_settings.json").value();

		auto img1 = asset_manager.load_asset<Na::ImageAsset>("assets/texture.png").value();
		auto img2 = asset_manager.load_asset<Na::ImageAsset>("assets/texture2.png").value();

		auto model = asset_manager.load_asset<Na::ModelAsset>("assets/model.obj").value();

		auto vs = asset_manager.load_shader(
			"assets/shaders/sandbox_vertex.glsl",
			Na::Graphics::ShaderStage::Vertex
		).value();

		auto fs = asset_manager.load_shader(
			"assets/shaders/sandbox_fragment.glsl",
			Na::Graphics::ShaderStage::Fragment
		).value();

		m_Camera.set_aspect_ratio(
			(float)m_MainWindow->width() / (float)m_MainWindow->height()
		);
		vs->set_push_constant_size((u32)m_Camera.matrices().size());

		m_VertexBuffer = Na::Graphics::MakeVertexBuffer(model->vertex_data_size());
		m_VertexBuffer->set_data(model->vertices().ptr());

		m_IndexBuffer = Na::Graphics::MakeIndexBuffer(model->index_count());
		m_IndexBuffer->set_data(model->indices().ptr());

		m_IndexCount = model->index_count();

		m_UniformSetLayouts.emplace(Na::Graphics::UniformSetLayout::Make({
			Na::Graphics::UniformBinding{
				.binding = 0,
				.type = Na::Graphics::UniformType::UniformMultibuffer,
				.shader_stage = Na::Graphics::ShaderStage::Vertex
			}
		}));

		m_UniformSetLayouts.emplace(Na::Graphics::UniformSetLayout::Make({
			Na::Graphics::UniformBinding{
				.binding = 0,
				.type = Na::Graphics::UniformType::Texture,
				.shader_stage = Na::Graphics::ShaderStage::Fragment
			}
		}));

		m_Pipeline = Na::Graphics::TrianglePipeline::Make(
			m_RenderTarget,
			Na::ModelAsset::VertexAttributes(),
			{ m_UniformSetLayouts[0], m_UniformSetLayouts[1] },
			{ vs, fs }
		);

		m_InstanceBuffer = Na::Graphics::MakeUniformBuffer(
			instanceBufferData.size(),
			renderer_settings->max_frames_in_flight()
		);
		m_InstanceBuffer->map();

		m_UniformSets.emplace(Na::Graphics::UniformSet::Make(
			m_UniformSetLayouts[0],
			m_Renderer
		));
		m_UniformSets.back()->bind_at(0, m_InstanceBuffer, Na::Graphics::BufferTypeFlags::UniformBuffer);

		m_Texture = Na::Graphics::Texture::Make(img1, renderer_settings);

		m_UniformSets.emplace(Na::Graphics::UniformSet::Make(
			m_UniformSetLayouts[1],
			m_Renderer
		));
		m_UniformSets.back()->bind_at(0, m_Texture);

		m_Texture2 = Na::Graphics::Texture::Make(img2, renderer_settings);

		m_UniformSets.emplace(Na::Graphics::UniformSet::Make(
			m_UniformSetLayouts[1],
			m_Renderer
		));
		m_UniformSets.back()->bind_at(0, m_Texture2);
	}

	void MainLayer::on_event(Na::Event& e)
	{
		m_Input.on_event(e);

		switch (e.type)
		{
		case Na::EventType::WindowResized:
			m_Camera.set_aspect_ratio(
				(float)e.window_resized.width / (float)e.window_resized.height
			);
			break;
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
			m_MainWindow->capture_mouse();

			if (auto imgui_layer = Na::Application::Get().imgui_layer().lock())
				imgui_layer->set_enabled(false);

			m_Camera.on_mouse_capture(glm::vec2(m_Input.mouse_x(), m_Input.mouse_y()));
		}
	}

	void MainLayer::_on_key_press(Na::Event_KeyPressed& e)
	{
		switch (e.key)
		{
		case Na::Keys::k_Escape:
			m_MainWindow->release_mouse();

			m_Camera.on_mouse_release();

			if (auto imgui_layer = Na::Application::Get().imgui_layer().lock())
				imgui_layer->set_enabled(true);

			break;
		}
	}

	void MainLayer::_on_mouse_move(Na::Event_MouseMoved& e)
	{
		if (m_MainWindow->mouse_captured())
			m_Camera.rotate_with_mouse(glm::vec2(e.x, e.y));
	}

	void MainLayer::update(double dt)
	{
		if (m_MainWindow->mouse_captured())
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
	}

	void MainLayer::draw(void)
	{
		static auto x_StartTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - x_StartTime).count();

		m_Renderer->bind_pipeline(m_Pipeline);

		m_Renderer->bind_uniform_sets(
			{
				m_UniformSets[0],
				m_UniformSets[1 + m_TextureIndex]
			},
			m_Pipeline
		);

		const Na::CameraMatrices& camera_matrices = m_Camera.matrices();
		m_Renderer->set_push_constant(
			(u32)camera_matrices.size(),
			Na::Graphics::ShaderStage::Vertex,
			0,
			&camera_matrices,
			m_Pipeline
		);

		glm::mat4& model0 = instanceBufferData.instance_data[0].model;
		glm::mat4& model1 = instanceBufferData.instance_data[1].model;

		model0 = glm::translate(glm::mat4(1.0f), m_Instance0_Position);
		model0 = glm::rotate(model0, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		model0 = glm::scale(model0, m_Instance0_Scale);

		model1 = glm::translate(glm::mat4(1.0f), m_Instance1_Position);
		model1 = glm::rotate(model1, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		model1 = glm::rotate(model1, time * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model1 = glm::scale(model1, m_Instance1_Scale);

		m_InstanceBuffer->set_subdata(&instanceBufferData, m_Renderer->current_frame_index());

		m_Renderer->draw_indexed(m_VertexBuffer, m_IndexBuffer, m_IndexCount, instanceBufferData.count());
	}

	void MainLayer::imgui_draw(void)
	{
		ImGui::Begin("Debug");

		ImGui::Text("Average FPS: %llu", Na::Application::Get().average_fps());

		ImGui::End();

		ImGui::Begin("Instance Data");

		ImGui::DragFloat3("Model 0 Position", glm::value_ptr(m_Instance0_Position), 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat3("Model 0 Scale", glm::value_ptr(m_Instance0_Scale), 0.01f, -10.0f, 10.0f);

		ImGui::DragFloat3("Model 1 Position", glm::value_ptr(m_Instance1_Position), 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat3("Model 1 Scale", glm::value_ptr(m_Instance1_Scale), 0.01f, -10.0f, 10.0f);

		ImGui::RadioButton("Texture 1", &m_TextureIndex, 0); ImGui::SameLine();
		ImGui::RadioButton("Texture 2", &m_TextureIndex, 1);

		ImGui::End();
	}
} // namespace Sandbox
