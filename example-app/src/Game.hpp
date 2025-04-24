#if !defined(EXAMPLE_APP_GAME_HPP)
#define EXAMPLE_APP_GAME_HPP

#include "Natrium-Core/Natrium.hpp"

namespace ExampleApp {
	class Game : public Na::App {
	public:
		Game(void);
		~Game(void) override;

		void on_event(Na::Event& e) override;
		void update(double dt) override;
		void draw(void) override;
	private:
		Na::Window m_Window;
		Na::Input m_Input;

		Na::Renderer m_Renderer;
		Na::Pipeline m_Pipeline;

		Na::VertexBuffer m_Vbo;
		Na::IndexBuffer m_Ibo;

		Na::Texture m_Texture;
	};
	inline Na::Logger<> g_Logger{"ExampleApp", &std::cout};
} // namespace ExampleApp

#endif // EXAMPLE_APP_GAME_HPP