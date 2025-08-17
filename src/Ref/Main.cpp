#include "Pch.hpp"
#include <Natrium/Main.hpp>

#include <Natrium/Template/References.hpp>

static Na::Logger<> logger{ "Ref" };

struct Dummy {
	Dummy(const std::string& name = "")
	: name(name)
	{
		logger.printf(Na::Debug, "Dummy named \"{}\" constructed!", name);
	}
	~Dummy(void)
	{
		logger.printf(Na::Debug, "Dummy named \"{}\" destroyed!", this->name);
	}

	void say_hello(void)
	{
		logger.printf(Na::Debug, "Hello from Dummy named \"{}\"!", this->name);
	}

	std::string name;
};

int main(int argc, char* argv[])
{
	{
		auto unique = Na::MakeUnique<Dummy>("Unique");
	}

	Na::WeakRef<Dummy> weak;
	{
		Na::Ref<Dummy> ref;

		{
			auto ref2 = Na::MakeShared<Dummy>("Shared");

			ref = ref2;
		}

		weak = ref;

		if (auto dummy = weak.lock())
		{
			logger.printf(Na::Debug, "Dummy named {} locked!", dummy->name);
		}
	}

	if (auto dummy = weak.lock())
	{
		logger.printf(Na::Debug, "Dummy named {} locked again!", dummy->name);
	}

	Na::ViewRef<Dummy> view;
	{
		Na::Ref<Dummy> ref = Na::MakeUnique<Dummy>("Shared from Unique");

		view = ref;

		view->say_hello();
	}

	logger.printf(Na::Debug, "End of main!");
	return 0;
}
