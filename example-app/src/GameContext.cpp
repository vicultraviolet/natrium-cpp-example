#include "Pch.hpp"
#include "GameContext.hpp"

#include "MainLayer.hpp"

namespace ExampleApp {
	struct GameContextData {
		Na::Context library_context;

		Na::AssetRegistry asset_registry;
		Na::LayerManager layer_manager;

		Na::Window main_window;
		Na::Renderer main_renderer;

		bool running;
	};
	static GameContextData* gameContextData = nullptr;

	GameContext::GameContext(void)
	{
		GameContextData*& data = gameContextData;

		NA_VERIFY(!data, "Failed to create GameContext: An existing context already exists!");

		data = Na::tcalloc<GameContextData>();

		new(&data->library_context) Na::Context(Na::Context::Initialize());

		new(&data->asset_registry) Na::AssetRegistry("assets/engine/", "bin/shaders/");

		new(&data->layer_manager) Na::LayerManager(1);

		new(&data->main_window) Na::Window(1280, 720, "Example Application");

		auto renderer_settings = data->asset_registry.load_renderer_settings("renderer_settings.json");
		renderer_settings->set_max_anisotropy(renderer_settings->AnisotropyLimit());

		new(&data->main_renderer) Na::Renderer(data->main_window, renderer_settings);

		data->layer_manager.attach_layer(Na::CreateLayer<MainLayer>());
	}

	GameContext::~GameContext(void)
	{
		GameContextData*& data = gameContextData;

		Na::VkContext::WaitForRemainingDeviceTasks();

		data->layer_manager.detach_all();

		data->main_renderer.destroy();
		data->main_window.destroy();

		data->layer_manager.destroy();
		data->asset_registry.destroy();

		data->library_context.Shutdown();

		free(data);
		data = nullptr;
	}

	void GameContext::run(void)
	{
		GameContextData*& data = gameContextData;

		data->running = true;

		Na::DeltaTime dt;
		while (data->running)
		{
			//g_Logger.fmt(Na::Trace, "average fps: {}", (u32)(1.0 / dt));

			for (Na::Event& e : Na::PollEvents())
			{
				if (e.type == Na::EventType::WindowClosed)
					return;

				for (Na::LayerHandle<> layer : data->layer_manager)
					if (!e.handled)
						layer->on_event(e);
			}

			dt.calculate();
			for (Na::LayerHandle<> layer : data->layer_manager)
				layer->update(dt);

			if (data->main_window.minimized())
				continue;

			if (!data->main_renderer.begin_frame())
				continue;

			for (Na::LayerHandle<> layer : data->layer_manager)
				layer->draw();

			data->main_renderer.end_frame();
		}
	}

	bool& GameContext::Running(void)
	{
		return gameContextData->running;
	}

	Na::AssetRegistry& GameContext::AssetRegistry(void)
	{
		return gameContextData->asset_registry;
	}

	Na::LayerManager& GameContext::LayerManager(void)
	{
		return gameContextData->layer_manager;
	}

	Na::Window& GameContext::MainWindow(void)
	{
		return gameContextData->main_window;
	}

	Na::Renderer& GameContext::MainRenderer(void)
	{
		return gameContextData->main_renderer;
	}

} // namespace ExampleApp
