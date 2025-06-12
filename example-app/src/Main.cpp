#include "Pch.hpp"
#include "MainLayer.hpp"

int main(int argc, char* argv[])
{
#if !defined(APP_HEAP)
	Na::Application app(
		1280, 720, "Natrium Example", // window
		"assets/engine/", "bin/shaders/",
		"renderer_settings.json"
	);
#else
	auto _app = Na::UniqueRef<Na::Application>::Make(
		1280, 720, "Natrium Example", // window
		"assets/engine/", "bin/shaders/",
		"renderer_settings.json"
	);
	Na::Application& app = *_app;
#endif // APP_HEAP

	app.create_layer<ExampleApp::MainLayer>(0);
	app.create_layer<Na::ImGuiLayer>(app.renderer(), 1);
	app.run();
	return 0;
}
