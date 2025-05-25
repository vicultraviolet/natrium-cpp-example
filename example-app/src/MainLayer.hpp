#if !defined(EXAMPLE_APP_MAIN_LAYER_HPP)
#define EXAMPLE_APP_MAIN_LAYER_HPP

#include <Natrium/Natrium.hpp>

namespace ExampleApp {
	struct Camera {
		glm::vec3 pos;
		glm::vec3 eye;

		float fov;

		float yaw, pitch;
		float last_x, last_y;
	};

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

		void _reset_mouse(float x, float y);
		void _update_camera(float mouse_x, float mouse_y);
	private:
		Na::Input m_Input;

		Na::GraphicsPipeline m_Pipeline;

		Na::VertexBuffer m_VertexBuffer;
		Na::IndexBuffer m_IndexBuffer;
		Na::StorageBuffer m_InstanceBuffer;

		Na::Texture m_Texture;

		Camera m_Camera;
		bool m_FirstMouse = true;
	};
} // namespace ExampleApp

#endif // EXAMPLE_APP_MAIN_LAYER_HPP