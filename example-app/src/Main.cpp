#include "Pch.hpp"
#include <Natrium-Core/Context.hpp>
#include <Natrium-Core/Logger.hpp>

#include <Natrium-Core/Window.hpp>

int main(int argc, char* argv[])
{
	Na::Context context = Na::Context::Initialize();

	Na::Logger<> logger{"ExampleApp", &std::clog};
	logger(Na::Info, "Hello, world!");

	bool should_close = false;

	Na::Window window(1280, 720, "ExampleApp");

	while (!should_close)
	{
		for (Na::Event& e : Na::PollEvents())
		{
			if (e.type == Na::Event_Type::WindowClosed)
			{
				should_close = true;
				break;
			}
		}

		std::this_thread::sleep_for(1ms);
	}

	window.destroy();

	Na::Context::Shutdown();

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
