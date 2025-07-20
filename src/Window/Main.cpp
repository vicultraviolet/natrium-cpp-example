#include "Pch.hpp"
#include <Natrium/Main.hpp>

#include <Natrium/Core/Context.hpp>
#include <Natrium/Core/Window.hpp>
#include <Natrium/Core/DeltaTime.hpp>

using namespace Na::Primitives;

int main(int argc, char* argv[])
{
	Na::ContextInitInfo context_info{};
	Na::Context context(context_info);

	Na::Window window(1280, 720, "Window Example");

	Na::DeltaTime dt;

	while (true)
	{
		std::this_thread::sleep_for(16ms);

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
			}
		}

		dt.calculate();
		u32 average_fps = (u32)(1.0 / dt);

		window.set_title(NA_FORMAT("FPS: {}", average_fps));
	}

End:
	return 0;
}
