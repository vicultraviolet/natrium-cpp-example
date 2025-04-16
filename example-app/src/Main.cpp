#include "Pch.hpp"
#include <Natrium-Core/Core.hpp>

int main(int argc, char* argv[])
{
	Na::HelloWorld();

	return 0;
}

#if (defined(NA_PLATFORM_WINDOWS) && defined(NA_CONFIG_DIST))
int APIENTRY WinMain(
	HINSTANCE hInst,
	HINSTANCE hInstPrev,
	PSTR cmdline,
	int cmdshow)
{
	return main(__argc, __argv);
}
#endif 
