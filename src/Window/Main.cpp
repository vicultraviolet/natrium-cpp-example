#include <Natrium/PchBase.hpp>
#include <Natrium/Natrium.hpp>

int main(int argc, char* argv[])
{
	Na::ContextInitInfo context_init_info{};
	Na::Context context(context_init_info);

	Na::Window window(1280, 720, "Example");

	Na::DeltaTime dt;

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
			}
		}

		dt.calculate();
		u32 average_fps = 1.0 / dt;
	}

End:
	return 0;
}
