#include <Natrium/PchBase.hpp>
#include <Natrium/Natrium.hpp>

static Na::Logger<> logger{ "Arena" };

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

	std::string name;
};

int main(int argc, char* argv[])
{
	// will reallocate if needed, although should be avoided for performance
	Na::Arena<Dummy> arena(1024 * 1024);

	{
		u64 index = arena.emplace("Index");

		// do awesome stuff here

		arena.remove_at(index);
	}

	{
		auto unique = arena.make_unique("Unique");
	}

	Na::Arena<Dummy>::WeakHandle weak;
	{
		Na::Arena<Dummy>::SharedHandle shared;

		{
			auto shared2 = arena.make_shared("Shared");
			shared = shared2;
		}

		weak = shared;

		if (auto dummy = weak.lock())
		{
			logger.printf(Na::Debug, "Dummy named {} locked!", dummy->name);
		}
	}

	if (auto dummy = weak.lock())
	{
		logger.printf(Na::Debug, "Dummy named {} locked!", dummy->name);
	}

	{
		Na::Arena<Dummy>::ViewHandle view = arena.make_view("View");
	}

	{
		Na::Arena<Dummy>::ViewHandle view2 = arena.make_view("View2");
		arena.remove_at(view2.index());
	}

	logger.printf(Na::Debug, "End of main!");
	return 0;
}
