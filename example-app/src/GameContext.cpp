#include "Pch.hpp"
#include "GameContext.hpp"

#include "MainLayer.hpp"

namespace ExampleApp {
	struct GameContextData {
		Na::Context library_context;

		Na::AssetRegistry asset_registry;
		Na::LayerManager layer_manager;

		Na::Window main_window;
		Na::RendererCore main_renderer_core;
		Na::Renderer main_renderer;

		bool running;
	};
	static GameContextData* gameContextData = nullptr;

	GameContext::GameContext(void)
	{
		if (gameContextData)
			throw std::runtime_error("Cannot create more than one game context!");

		gameContextData = Na::tcalloc<GameContextData>();

		new (&gameContextData->library_context)    Na::Context(Na::Context::Initialize());

		new (&gameContextData->asset_registry)     Na::AssetRegistry("assets/", Na::Context::GetExecDir());
		new (&gameContextData->layer_manager)      Na::LayerManager(1);

		new (&gameContextData->main_window)        Na::Window(1280, 720, "Example Application");
		new (&gameContextData->main_renderer_core) Na::RendererCore(gameContextData->main_window);
		new (&gameContextData->main_renderer)      Na::Renderer(gameContextData->main_renderer_core);

		gameContextData->layer_manager.attach_layer(Na::CreateLayer<MainLayer>());
	}

	GameContext::~GameContext(void)
	{
		Na::VkContext::WaitForRemainingDeviceTasks();

		gameContextData->layer_manager.detach_all();

		gameContextData->main_renderer.destroy();
		gameContextData->main_renderer_core.destroy();
		gameContextData->main_window.destroy();

		gameContextData->layer_manager.~LayerManager();
		gameContextData->asset_registry.~AssetRegistry();

		gameContextData->library_context.Shutdown();

		free(gameContextData);
		gameContextData = nullptr;
	}

	void GameContext::run(void)
	{
		gameContextData->running = true;

		Na::DeltaTime dt;
		while (gameContextData->running)
		{
			//g_Logger.fmt(Na::Trace, "average fps: {}", (u32)(1.0 / dt));

			for (Na::Event& e : Na::PollEvents())
			{
				if (e.type == Na::EventType::WindowClosed)
					return;

				for (Na::LayerHandle<> layer : gameContextData->layer_manager)
					if (!e.handled)
						layer->on_event(e);
			}

			dt.calculate();
			for (Na::LayerHandle<> layer : gameContextData->layer_manager)
				layer->update(dt);

			if (gameContextData->main_window.minimized())
				continue;

			if (!gameContextData->main_renderer.begin_frame())
				continue;

			for (Na::LayerHandle<> layer : gameContextData->layer_manager)
				layer->draw();

			gameContextData->main_renderer.end_frame();
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

	Na::RendererCore& GameContext::MainRendererCore(void)
	{
		return gameContextData->main_renderer_core;
	}

	Na::Renderer& GameContext::MainRenderer(void)
	{
		return gameContextData->main_renderer;
	}

} // namespace ExampleApp
