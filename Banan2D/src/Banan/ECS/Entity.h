#pragma once

#include <functional>

namespace Banan::ECS
{

	class Registry;

	class Entity
	{
	public:
		operator uint32_t() const { return m_id; }
		bool operator==(const Entity& other) const { return m_id == other.m_id; }

	private:
		Entity(uint32_t id)	: m_id(id)	{}

	private:
		uint32_t m_id;
		friend class Registry;
	};

}

namespace std
{

	template<>
	struct hash<Banan::ECS::Entity>
	{
		size_t operator()(const Banan::ECS::Entity& entity) const
		{
			return hash<uint32_t>()(entity);
		}
	};

}