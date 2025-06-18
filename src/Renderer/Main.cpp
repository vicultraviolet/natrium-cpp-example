#include <Natrium/PchBase.hpp>
#include <Natrium/Natrium.hpp>

constexpr glm::vec4 k_ClearColor{ 0.1f, 0.08f, 0.15f, 1.0f };

int main(int argc, char* argv[])
{
	Na::InitInfo init_info{};
	Na::Context context(init_info);

	Na::AssetRegistry asset_registry("assets/engine/", "bin/shaders/");

	// if file is not found, it will be created with default settings
	auto renderer_settings = asset_registry.load_asset<Na::RendererSettings>("renderer_settings.json");

	// sets anisotropy limit to the maximum supported by the GPU
	renderer_settings->set_max_anisotropy(Na::RendererSettings::AnisotropyLimit());

	Na::Window window(1280, 720, "Example");
	Na::Renderer renderer(window, renderer_settings);

	while (true)
	{
		for (Na::Event& e : Na::PollEvents())
		{
			switch (e.type)
			{
			case Na::EventType::MouseButtonPressed:
				e.window->capture_mouse();
				break;
			case Na::EventType::KeyPressed:
				if (e.key_pressed.key == Na::Keys::k_Escape)
					e.window->release_mouse();
				break;
			case Na::EventType::WindowClosed:
				goto End;
				break;
			}
		}

		// will crash if the window is minimized, so you should always check before rendering
		if (window.minimized())
			continue;

		// if returns false, it means you should continue rendering (e.g. window was resized)
		if (!renderer.begin_frame(k_ClearColor))
			continue;

		renderer.end_frame();
	}

End:
	// IMPORTANT: wait for the gpu to finish all operations before deleting any resources
	VkContext::Get().wait_for_device();

	return 0;
}
