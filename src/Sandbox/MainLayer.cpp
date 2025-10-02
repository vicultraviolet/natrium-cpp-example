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
	  m_MainWindow(Na::HL::App::Get().window()),
	  m_Renderer(Na::HL::App::Get().renderer()),
	  m_RenderTarget(Na::HL::App::Get().render_target())
	{
		auto asset_manager = Na::AssetManager::Get();

		auto renderer_settings = asset_manager->get_by_name<Na::RendererSettings>("renderer_settings");

		auto img1 = asset_manager->create_asset<Na::HostImage>("img1");
		auto img2 = asset_manager->create_asset<Na::HostImage>("img2");

		img1->load("assets/texture.png");
		img2->load("assets/texture2.png");

		auto host_mesh = asset_manager->create_asset<Na::HostMesh>("host_mesh");
		host_mesh->load("assets/model.obj");

		auto vs = Na::Graphics::Shader::Make(
			"assets/shaders/sandbox_vertex.glsl",
			Na::Graphics::ShaderStage::Vertex
		).value();

		auto fs = Na::Graphics::Shader::Make(
			"assets/shaders/sandbox_fragment.glsl",
			Na::Graphics::ShaderStage::Fragment
		).value();

		auto audio = asset_manager->create_asset<Na::Audio::Wav>("bell");
		audio->load("assets/bell_mono.wav");

		Na::Audio::Context::GetBound()->listener().set_position(m_Camera.pos());

		m_AudioBuffer = Na::Audio::Buffer(audio);
		m_AudioSource = Na::Audio::Source(Na::Audio::SourceCreateInfo{});

		m_AudioSource.attach_buffer(m_AudioBuffer);

		m_Camera.set_aspect_ratio(
			(float)m_MainWindow->width() / (float)m_MainWindow->height()
		);
		vs->set_push_constant_size((u32)m_Camera.matrices().size());

		m_Mesh = asset_manager->create_asset<Na::HL::DeviceMesh>("mesh", host_mesh);

		m_UniformManager.init_layout(
			Na::HL::UniformSetIndices::k_Global, // set 0
			{
				Na::Graphics::UniformBinding{
					.binding = 0,
					.type = Na::Graphics::UniformType::UniformMultibuffer,
					.shader_stage = Na::Graphics::ShaderStage::Vertex
				},
				Na::Graphics::UniformBinding{
					.binding = 1,
					.type = Na::Graphics::UniformType::UniformMultibuffer,
					.shader_stage = Na::Graphics::ShaderStage::Fragment
				}
			}
		);

		m_UniformManager.init_layout(
			Na::HL::UniformSetIndices::k_Material, // set 1
			{
				Na::Graphics::UniformBinding
				{
					.binding = 0,
					.type = Na::Graphics::UniformType::Texture,
					.shader_stage = Na::Graphics::ShaderStage::Fragment,

					.count = 2,
					.partially_bound = false,
					.dynamic_count = false
				}
			}
		);

		m_UniformBuffer = Na::Graphics::MakeUniformBuffer(
			sizeof(i32),
			renderer_settings->max_frames_in_flight
		);
		m_UniformBuffer->map();

		m_InstanceBuffer = Na::Graphics::MakeUniformBuffer(
			instanceBufferData.size(),
			renderer_settings->max_frames_in_flight
		);
		m_InstanceBuffer->map();

		m_Texture = asset_manager->create_asset<Na::HL::Texture>(
			"texture1",
			Na::HL::TextureDimensions(img1->width(), img1->height()),
			renderer_settings,
			true // saveable
		); 
		m_Texture->set_data(img1);

		m_Texture2 = asset_manager->create_asset<Na::HL::Texture>(
			"texture2",
			Na::HL::TextureDimensions(img2->width(), img2->height()),
			renderer_settings,
			true // saveable
		);
		m_Texture2->set_data(img2);

		m_UniformManager.create_set(
			Na::HL::UniformSetIndices::k_Global, // set 0
			m_Renderer
		);

		Na::Graphics::UniformSetBufferBindingInfo buffer_binding_info;

		buffer_binding_info.type = Na::Graphics::BufferTypeFlags::UniformBuffer;

		buffer_binding_info.binding = 0;
		buffer_binding_info.buffer = m_InstanceBuffer;

		m_UniformManager.set(0)->bind(buffer_binding_info);

		buffer_binding_info.binding = 1;
		buffer_binding_info.buffer = m_UniformBuffer;

		m_UniformManager.set(0)->bind(buffer_binding_info);

		m_UniformManager.create_set(
			Na::HL::UniformSetIndices::k_Material, // set 1
			m_Renderer
		);

		Na::Graphics::UniformSetTextureInfo texture_infos[2] = { *m_Texture, *m_Texture2 };

		Na::Graphics::UniformSetTextureBindingInfo2 texture_binding_info;

		texture_binding_info.binding = 0;

		texture_binding_info.texture_infos = texture_infos;
		texture_binding_info.texture_count = 2;

		m_UniformManager.set(1)->bind_array(texture_binding_info);

		Na::HL::TrianglePipelineCreateInfo pipeline_info
		{
			.render_target = m_RenderTarget,
			.shaders = { vs, fs },
			.vertex_attributes = &Na::HostMesh::GetVertexAttributes(),
			.uniform_set_layouts = &m_UniformManager.set_layouts()
		};
		m_Pipeline = Na::HL::Pipeline(pipeline_info);

		asset_manager->save_registry();

		g_Logger.print(Na::LogLevel::Trace, "MainLayer initialized!");

		m_Transform0.set_position(glm::vec3(-1.0f, 0.5f, 0.2f));
		m_Transform0.set_uniform_scale(1.0f);

		m_Transform1.set_position(glm::vec3(1.0f, 0.5f, 0.7f));
		m_Transform1.set_uniform_scale(0.5f);

		m_Transform0.set_rotation(glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
		m_Transform1.set_rotation(glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
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

			if (auto imgui_layer = Na::HL::App::Get().imgui_layer().lock())
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

			if (auto imgui_layer = Na::HL::App::Get().imgui_layer().lock())
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

			Na::Audio::Context::GetBound()->listener().set_position(m_Camera.pos());
		}

		glm::vec3 forward = glm::normalize(m_Camera.eye() - m_Camera.pos());
		glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 up = glm::normalize(glm::cross(right, forward));

		Na::Audio::Context::GetBound()->listener().set_orientation({ forward, up });

		m_Transform1.rotate((float)dt * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		m_AudioSource.set_position(m_Transform0.position());
	}

	void MainLayer::draw(void)
	{
		static auto x_StartTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - x_StartTime).count();

		m_Renderer->bind_pipeline(m_Pipeline.native());

		m_Renderer->bind_uniform_sets(
			{
				m_UniformManager.set(0),
				m_UniformManager.set(1)
			},
			m_Pipeline.native()
		);

		const Na::CameraMatrices& camera_matrices = m_Camera.matrices();
		m_Renderer->set_push_constant(
			(u32)camera_matrices.size(),
			Na::Graphics::ShaderStage::Vertex,
			0,
			&camera_matrices,
			m_Pipeline.native()
		);

		instanceBufferData.instance_data[0].model = m_Transform0.mat();
		instanceBufferData.instance_data[1].model = m_Transform1.mat();

		m_InstanceBuffer->set_subdata(&instanceBufferData, m_Renderer->current_frame_index());
		m_UniformBuffer->set_subdata(&m_TextureIndex, m_Renderer->current_frame_index());

		m_Renderer->bind_vertex_buffer(m_Mesh->vertex_buffer());
		m_Renderer->bind_index_buffer(m_Mesh->index_buffer());
		m_Renderer->draw_indexed(m_Mesh->index_count(), instanceBufferData.count());
	}

	void MainLayer::imgui_draw(void)
	{
		ImGui::Begin("Debug");

		ImGui::Text("Average FPS: %llu", Na::HL::App::Get().average_fps());

		ImGui::Separator();

		ImGui::Text(
			"Camera's Position: %f, %f, %f",
			m_Camera.pos().x,
			m_Camera.pos().y,
			m_Camera.pos().z
		);
		ImGui::Text(
			"Camera's Eye: %f, %f, %f",
			m_Camera.eye().x,
			m_Camera.eye().y,
			m_Camera.eye().z
		);

		ImGui::End();

		ImGui::Begin("window");

		ImGui::DragFloat3(
			"Model 0 Position",
			glm::value_ptr(m_Transform0.position()),
			0.01f, // speed
			-10.0f, 10.0f
		);
		ImGui::DragFloat3(
			"Model 0 Scale",
			glm::value_ptr(m_Transform0.scale()),
			0.01f, // speed
			-10.0f, 10.0f
		);

		ImGui::DragFloat3(
			"Model 1 Position",
			glm::value_ptr(m_Transform1.position()),
			0.01f, // speed
			-10.0f, 10.0f
		);
		ImGui::DragFloat3(
			"Model 1 Scale",
			glm::value_ptr(m_Transform1.scale()),
			0.01f, // speed
			-10.0f, 10.0f
		);

		m_Transform0.mark_dirty();
		m_Transform1.mark_dirty();

		ImGui::Separator();

		ImGui::RadioButton("Texture 1", &m_TextureIndex, 0); ImGui::SameLine();
		ImGui::RadioButton("Texture 2", &m_TextureIndex, 1);

		ImGui::Separator();

		if (ImGui::Button("Play Sound", { 140, 40 }))
			m_AudioSource.play();

		ImGui::End();
	}
} // namespace Sandbox
