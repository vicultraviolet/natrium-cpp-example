#include <Natrium/PchBase.hpp>
#include <Natrium/Natrium.hpp>

int main(int argc, char* argv[])
{
	// the template argument is optional
	// in Distribution builds, it defaults to false, disabling all logging
	Na::Logger<> logger{ "Example" };

	// when you don't specify the output stream, it defaults to std::cout
	
	logger.print_header(Na::Info);

	logger.print(Na::None,   "Hello, world!");
	logger.print(Na::Trace,  "Hello, world!");
	logger.print(Na::Debug,  "Hello, world!");
	logger.print(Na::Info,   "Hello, world!");
	logger.print(Na::Notice, "Hello, world!");
	logger.print(Na::Warn,   "Hello, world!");
	logger.print(Na::Error,  "Hello, world!");
	logger.print(Na::Fatal,  "Hello, world!");

	logger.print_raw("This is a raw message!\n");

	logger.printf(Na::Info, "This is a formatted message: {}", 42);
	logger.printf_raw("This is a raw formatted message: {}\n", 42);

	logger.print_to(std::cerr, Na::Info, "This message was printed to std::cerr!");
	logger.print_raw_to(std::cerr, "This message was printed raw to std::cerr!\n");

	return 0;
}
