#include "Pch.hpp"
#include "MainLayer.hpp"

int main(int argc, char* argv[])
{
	Na::InitInfo init_info{};
	Na::Context context(init_info);

	auto app = Na::UniqueRef<Na::Application>::Make(
		1280, 720, "Natrium Example", // window
		"assets/engine/", "bin/shaders/",
		"renderer_settings.json"
	);

	app->create_layer<ExampleApp::MainLayer>(0);
	app->create_layer<Na::ImGuiLayer>(app->renderer(), 1);

	app->run();

	return 0;
}
