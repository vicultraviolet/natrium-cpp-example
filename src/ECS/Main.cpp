#include "Pch.hpp"
#include <Natrium/Main.hpp>

#include <Natrium/Core/Context.hpp>

#include <Natrium/ECS/Entity.hpp>
#include <Natrium/ECS/Component.hpp>
#include <Natrium/ECS/ECS_System.hpp>

#include <Natrium/ECS/ECS_Registry.hpp>
#include <Natrium/ECS/ECS_SystemManager.hpp>

#include <Natrium/ECS/ECS_RegistrySerializer.hpp>

#include <Natrium/HL/Scene_HL.hpp>
#include <Natrium/HL/EntityHandle_HL.hpp>

using namespace Na::Primitives;

static Na::Logger<> logger{ "ECS" };

class Position : public Na::ECS::Component {
public:
	glm::vec3 pos{};

	Position(void) = default;
	~Position(void) = default;

	Position(float x, float y, float z) : pos(x, y, z) {}

	nlohmann::json serialize(void) const override
	{
		return nlohmann::json::array({ pos.x, pos.y, pos.z });
	}

	void deserialize(const nlohmann::json& j) override
	{
		for (u64 i = 0; const auto& coord : j)
		{
			glm::value_ptr(pos)[i++] = coord.get<float>();
		}
	}

	NA_DEFINE_COMPONENT_TYPE_NAME("pos");
};

class Health : public Na::ECS::Component {
public:
	i64 hp = -1;

	Health(void) = default;
	~Health(void) = default;

	Health(i64 hp) : hp(hp) {}

	nlohmann::json serialize(void) const override
	{
		return nlohmann::json(hp);
	}

	void deserialize(const nlohmann::json& j) override
	{
		hp = j.get<i64>();
	}

	NA_DEFINE_COMPONENT_TYPE_NAME("hp");
};

class System : public Na::ECS::System<Position, Health> {
public:
	void update(double dt, Na::View<Position> pos, Na::View<Health> hp) override
	{
		logger.printf(Na::Debug, "pos: {}, {}, {}", pos->pos.x, pos->pos.y, pos->pos.z);
		logger.printf(Na::Debug, "hp: {}", hp->hp);
	}
};

int main(int argc, char* argv[])
{
	Na::ContextInitInfo context_info{};
	Na::Context context(context_info);

	Na::ECS::RegistrySerializer serializer;
	serializer.bind();

	serializer.register_component<Position>();
	serializer.register_component<Health>();

	Na::ECS::SystemManager system_manager;

	auto scene = MakeRef<Na::HL::Scene>();

	system_manager.registry = &scene->registry();

	Na::HL::EntityHandle e1(scene);
	Na::HL::EntityHandle e2(scene);
	Na::HL::EntityHandle e3(scene);

	e1.emplace_component<Position>(1.f, 2.f, 3.f);
	e1.emplace_component<Health>(1000);

	e2.emplace_component<Health>(2000);

	e3.emplace_component<Position>(.1f, .2f, .3f);
	e3.emplace_component<Health>(1);

	System system;

	system_manager.update(system, 0.5);
	system_manager.draw(system);

	g_Logger.printf(Na::Debug, "{}", *scene);

	return 0;
}
