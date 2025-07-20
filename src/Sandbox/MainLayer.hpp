#if !defined(EXAMPLE_APP_MAIN_LAYER_HPP)
#define EXAMPLE_APP_MAIN_LAYER_HPP

#include <Natrium/Natrium.hpp>

namespace Sandbox {
	using namespace Na::Primitives;

	class MainLayer : public Na::Layer {
	public:
		MainLayer(i64 priority = 0);
		~MainLayer(void) = default;

		void on_event(Na::Event& e) override;
		void update(double dt) override;
		void draw(void) override;
		void imgui_draw(void) override;
	private:
		void _on_mouse_button_press(Na::Event_MouseButtonPressed& e);
		void _on_key_press(Na::Event_KeyPressed& e);
		void _on_mouse_move(Na::Event_MouseMoved& e);
	private:
		Na::Input m_Input;

		Na::ArrayList<Na::UniqueRef<Na::Graphics::UniformSetLayout>> m_UniformSetLayouts;
		Na::UniqueRef<Na::Graphics::Pipeline>      m_Pipeline;

		Na::UniqueRef<Na::Graphics::VertexBuffer>  m_VertexBuffer;
		Na::UniqueRef<Na::Graphics::IndexBuffer>   m_IndexBuffer;

		Na::UniqueRef<Na::Graphics::StorageBuffer> m_InstanceBuffer;
		Na::UniqueRef<Na::Graphics::Texture>       m_Texture;
		Na::UniqueRef<Na::Graphics::Texture>       m_Texture2;

		Na::ArrayList<Na::UniqueRef<Na::Graphics::UniformSet>> m_UniformSets;

		i32 m_TextureIndex = 0;

		Na::Camera3dData m_Camera;

		glm::vec3 m_Instance0_Position{ -1.0f,  0.5f,  0.2f };
		glm::vec3 m_Instance0_Scale{ 1.0f,  1.0f,  1.0f };
		glm::vec3 m_Instance1_Position{ 1.0f,  0.5f,  0.7f };
		glm::vec3 m_Instance1_Scale{ 0.5f,  0.5f,  0.5f };
	};
} // namespace Sandbox

#endif // EXAMPLE_APP_MAIN_LAYER_HPP