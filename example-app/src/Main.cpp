#include "Pch.hpp"
#include "Game.hpp"

int main(int argc, char* argv[])
{
	Na::Context context = Na::Context::Initialize();

	{
		// performance is better with heap allocation, i profiled
		std::unique_ptr<ExampleApp::Game> game = std::make_unique<ExampleApp::Game>();
		game->run();
	}

	context.Shutdown();
	return 0;
}
