#include "Pch.hpp"
#include "MainLayer.hpp"

int main(int argc, char* argv[])
{
	Na::Application app(
		1280, 720, "Natrium Example", // window
		"assets/engine/", "bin/shaders/",
		"renderer_settings.json"
	);
	app.create_layer<ExampleApp::MainLayer>();
	app.run();
	return 0;
}
