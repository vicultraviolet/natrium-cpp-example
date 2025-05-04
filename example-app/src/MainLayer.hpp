#if !defined(EXAMPLE_APP_MAIN_LAYER_HPP)
#define EXAMPLE_APP_MAIN_LAYER_HPP

#include <Natrium/Natrium.hpp>

namespace ExampleApp {
	struct Camera {
		glm::vec3 pos;
		glm::vec3 eye;
		float fov;
	};

	class MainLayer : public Na::Layer {
	public:
		MainLayer(i64 priority = 0);

		void on_event(Na::Event& e) override;
		void update(double dt) override;
		void draw(Na::FrameData& fd) override;
	private:
		Na::Input m_Input;

		Na::GraphicsPipeline m_Pipeline;

		Na::VertexBuffer m_VertexBuffer;
		Na::IndexBuffer m_IndexBuffer;
		Na::StorageBuffer m_InstanceBuffer;

		Na::Texture m_Texture;

		Camera m_Camera;
	};
} // namespace ExampleApp

#endif // EXAMPLE_APP_MAIN_LAYER_HPP