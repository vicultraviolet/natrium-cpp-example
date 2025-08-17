#include "Pch.hpp"
#include "MainLayer.hpp"

int main(int argc, char* argv[])
{
	Na::ContextInitInfo context_info{};
	Na::Context context(context_info);

	Na::Graphics::DeviceInitInfo device_info
	{
		.backend = Na::Graphics::DeviceBackend::Vulkan
	};
	auto device = Na::Graphics::Device::Make(device_info);

	Na::ApplicationSettings app_settings
	{
		.window_width = 1280,
		.window_height = 720,
		.window_title = "Natrium Sandbox",
		.engine_assets_directory = "assets/engine",
		.shader_output_directory = "bin/shaders",
		.renderer_settings_path = "renderer_settings.json"
	};
	Na::Application app(app_settings);

	app.create_layer<Sandbox::MainLayer>(0);
	app.create_imgui_layer(app.render_target(), 1);

	app.run();

	return 0;
}
