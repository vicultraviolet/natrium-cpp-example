#if !defined(EXAMPLE_APP_GAME_HPP)
#define EXAMPLE_APP_GAME_HPP

#include <Natrium/Natrium.hpp>

namespace ExampleApp {
	class GameContext {
	public:
		GameContext(void);
		~GameContext(void);

		void run(void);
		
		[[nodiscard]] static bool&              Running(void);
		[[nodiscard]] static Na::AssetRegistry& AssetRegistry(void);
		[[nodiscard]] static Na::LayerManager&  LayerManager(void);
		[[nodiscard]] static Na::Window&        MainWindow(void);
		[[nodiscard]] static Na::RendererCore&  MainRendererCore(void);
		[[nodiscard]] static Na::Renderer&      MainRenderer(void);
	};
	inline Na::Logger<> g_Logger{"ExampleApp", &std::cout};
} // namespace ExampleApp

#endif // EXAMPLE_APP_GAME_HPP