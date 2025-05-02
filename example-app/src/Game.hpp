#if !defined(EXAMPLE_APP_GAME_HPP)
#define EXAMPLE_APP_GAME_HPP

#include <Natrium/Natrium.hpp>

namespace ExampleApp {
	struct Camera {
		glm::vec3 pos;
		glm::vec3 eye;
		float fov;
	};

	class Game : public Na::App {
	public:
		Game(void);
		~Game(void) override;

		void on_event(Na::Event& e) override;
		void update(double dt) override;
		void draw(void) override;
	private:
		Na::AssetRegistry m_AssetRegistry;

		Na::Window m_Window;
		Na::Input m_Input;

		Na::Renderer m_Renderer;
		Na::GraphicsPipeline m_Pipeline;

		Na::VertexBuffer m_VertexBuffer;
		Na::IndexBuffer m_IndexBuffer;
		Na::StorageBuffer m_InstanceBuffer;

		Na::Texture m_Texture;

		Camera m_Camera;
	};
	inline Na::Logger<> g_Logger{"ExampleApp", &std::cout};
} // namespace ExampleApp

#endif // EXAMPLE_APP_GAME_HPP