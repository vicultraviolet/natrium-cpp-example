#include "Pch.hpp"
#include <Natrium-Core/Context.hpp>
#include <Natrium-Core/Logger.hpp>

#include <Natrium-Core/Window.hpp>
#include <Natrium-Renderer/Renderer.hpp>

int main(int argc, char* argv[])
{
	Na::Context context = Na::Context::Initialize();

	Na::Logger<> logger{"ExampleApp", &std::clog};
	logger(Na::Info, "Hello, world!");

	Na::Window window(1280, 720, "ExampleApp");
	Na::Renderer renderer(window);

	bool should_close = false;
	while (!should_close)
	{
		std::this_thread::sleep_for(1ms);

		for (Na::Event& e : Na::PollEvents())
		{
			switch (e.type)
			{
			case Na::Event_Type::WindowClosed:
				should_close = true;
				break;
			case Na::Event_Type::WindowResized:
				renderer.update_size();
				break;
			}
		}
		if (window.width() == 0 || window.height() == 0)
			continue;

		renderer.clear();

		renderer.present();
	}
	Na::VkContext::GetLogicalDevice().waitIdle();

	renderer.destroy();
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
