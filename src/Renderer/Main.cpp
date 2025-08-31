#include "Pch.hpp"
#include <Natrium/Main.hpp>

#include <Natrium/Core/Context.hpp>
#include <Natrium/Core/Window.hpp>

#include <Natrium/Graphics/Renderer.hpp>

#include <Natrium/Assets/AssetManager.hpp>

constexpr glm::vec4 k_ClearColor{ 0.1f, 0.08f, 0.15f, 1.0f };

int main(int argc, char* argv[])
{
	Na::ContextInitInfo context_info{};
	Na::Context context(context_info);

	Na::Graphics::DeviceInitInfo device_info
	{
		.backend = Na::Graphics::DeviceBackend::Vulkan,
		.required_extensions = {
			Na::Graphics::DeviceExtension::Swapchain
		}
	};
	auto device = Na::Graphics::Device::Make(device_info);

	Na::AssetManager asset_manager("assets/engine/", "bin/shaders/");

	// if file is not found, it will be created with default settings
	auto renderer_settings = asset_manager.load_renderer_settings("renderer_settings.json").value();

	// sets anisotropy limit to the maximum supported by the GPU
	renderer_settings->set_max_anisotropy(device->limits()->max_anisotropy());

	auto renderer = Na::Graphics::Renderer::Make(renderer_settings);

	auto window = Na::Ref<Na::Window>::Make(1280, 720, "Renderer Example");

	auto render_target = Na::Graphics::SwapchainRenderTarget::Make(window, renderer_settings);
	renderer->bind_render_target(render_target);

	while (true)
	{
		std::this_thread::sleep_for(16ms);

		for (Na::Event& e : Na::PollEvents())
		{
			switch (e.type)
			{
			case Na::EventType::WindowClosed:
				goto End;
				break;
			}
		}

		// will crash if the window is minimized, so you should always check before rendering
		if (window->minimized())
			continue;

		// if returns false, it means you shouldn't continue rendering (e.g. window was resized)
		if (!render_target->acquire_next_image())
			continue;

		renderer->begin_frame();
		renderer->begin_render_pass(k_ClearColor);

		// here you would typically bind your pipeline and draw your objects

		renderer->end_render_pass();
		renderer->end_frame();

		render_target->present();
	}

End:
	// IMPORTANT: wait for the gpu to finish all operations before deleting any resources
	device->wait_all();

	return 0;
}
