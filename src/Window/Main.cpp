#include <Natrium/PchBase.hpp>
#include <Natrium/Natrium.hpp>

int main(int argc, char* argv[])
{
	Na::InitInfo init_info{};
	Na::Context context(init_info);

	Na::Window window(1280, 720, "Example");

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
				return 0;
			}
		}
	}

	return 0;
}
