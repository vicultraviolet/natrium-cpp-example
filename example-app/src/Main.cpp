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
	app.create_layer<ExampleApp::MainLayer>();
	app.run();
#else
	auto app = Na::UniqueRef<Na::Application>::Make(
		1280, 720, "Natrium Example", // window
		"assets/engine/", "bin/shaders/",
		"renderer_settings.json"
	);
	app->create_layer<ExampleApp::MainLayer>();
	app->run();
#endif // APP_HEAP
	return 0;
}
