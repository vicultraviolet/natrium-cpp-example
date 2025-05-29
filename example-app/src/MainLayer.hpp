#if !defined(EXAMPLE_APP_MAIN_LAYER_HPP)
#define EXAMPLE_APP_MAIN_LAYER_HPP

#include <Natrium/Natrium.hpp>

namespace ExampleApp {
	class MainLayer : public Na::Layer {
	public:
		MainLayer(i64 priority = 0);

		void on_event(Na::Event& e) override;
		void update(double dt) override;
		void draw(void) override;
	private:
		void _on_mouse_button_press(Na::Event_MouseButtonPressed& e);
		void _on_key_press(Na::Event_KeyPressed& e);
		void _on_mouse_move(Na::Event_MouseMoved& e);
	private:
		Na::Input m_Input;

		Na::GraphicsPipeline m_Pipeline;

		Na::VertexBuffer m_VertexBuffer;
		Na::IndexBuffer m_IndexBuffer;
		Na::StorageBuffer m_InstanceBuffer;

		Na::Texture m_Texture;

		Na::CameraData m_CameraData;
	};
} // namespace ExampleApp

#endif // EXAMPLE_APP_MAIN_LAYER_HPP