#include "Pch.hpp"
#include "MainLayer.hpp"

int main(int argc, char* argv[])
{
	Na::ContextInitInfo context_info{};
	Na::Context context(context_info);

	Na::Graphics::UniformIndexingInfo uniform_indexing_info;

	uniform_indexing_info.array_types.insert(Na::Graphics::UniformType::Texture);
	uniform_indexing_info.runtime_array = true;

	Na::Graphics::DeviceInitInfo device_info
	{
		.backend = Na::Graphics::DeviceBackend::Vulkan,
		.required_extensions = {
			Na::Graphics::DeviceExtension::Swapchain,
			Na::Graphics::DeviceExtension::UniformIndexing
		},
		.uniform_indexing_info = std::move(uniform_indexing_info)
	};
	auto device = Na::Graphics::Device::Make(device_info);

	Na::Audio::ContextInitInfo audio_info{};
	Na::Audio::Context audio_context(audio_info);

	Na::AssetManagerCreateInfo asset_info
	{
		.engine_assets_dir   = "assets/engine",
		.shader_output_dir   = "bin/shaders",
		.asset_registry_path = "assets/asset_registry.json"
	};
	Na::AssetManager asset_manager(asset_info);

	auto renderer_settings = asset_manager.create_asset<Na::RendererSettings>("renderer_settings");
	renderer_settings->load("renderer_settings.json");

	renderer_settings->set_max_anisotropy(32767.0f);

	renderer_settings->save("renderer_settings.json");

	Na::HL::AppSettings app_settings
	{
		.window_width = 1280,
		.window_height = 720,
		.window_title = "Natrium Sandbox",
		.renderer_settings = renderer_settings
	};
	Na::HL::App app(app_settings);

	app.create_layer<Sandbox::MainLayer>(0);
	app.create_imgui_layer(app.render_target(), 1);

	app.run();

	return 0;
}
