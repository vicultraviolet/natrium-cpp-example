#include "Pch.hpp"
#include "Game.hpp"

int main(int argc, char* argv[])
{
	using namespace ExampleApp;
	try {
		Na::Context context = Na::Context::Initialize();

		{
			// performance is better with heap allocation, i profiled
			std::unique_ptr<Game> game = std::make_unique<Game>();
			game->run();
		}

		context.Shutdown();
	} catch (const std::exception& e)
	{
		ExampleApp::g_Logger.fmt(Na::Fatal, "Unhandled exception reached main! {}", e.what());
		return -1;
	}
	return 0;
}

#if (defined(NA_PLATFORM_WINDOWS) && defined(NA_CONFIG_DIST))
int APIENTRY WinMain(
	HINSTANCE hInst,
	HINSTANCE hInstPrev,
	PSTR cmdline,
	int cmdshow)
{
	return main(__argc, __argv);
}
#endif 
