#pragma once

#include "Banan/ECS/Archetype.h"

namespace Banan::ECS
{
	class Registry;

	template<typename T, typename... Ts>
	class View;

	template<typename T, typename... Ts>
	class ViewIterator
	{
	private:
		ViewIterator(
			std::unordered_map<internal::ComponentMask, internal::Archetype>* archetypes,
			const internal::ComponentMask& componentMask,
			std::unordered_map<internal::ComponentMask, internal::Archetype>::iterator current
		) :
			m_archetypes(archetypes),
			m_componentMask(componentMask),
			m_currentArchetype(current)
		{
			GetValidArchetype();
		}

	public:
		ViewIterator() :
			m_archetypes(nullptr),
			m_componentMask(internal::ComponentMask())
		{}

		ViewIterator(const ViewIterator<T, Ts...>& other) :
			m_archetypes(other.m_archetypes),
			m_componentMask(other.m_componentMask),
			m_currentArchetype(other.m_currentArchetype),
			m_currentEntity(other.m_currentEntity)
		{
			std::memcpy(m_componentOffsets, other.m_componentOffsets, (1 + sizeof...(Ts) * sizeof(uint64_t)));
		}

		ViewIterator(ViewIterator<T, Ts...>&& other) :
			m_archetypes(other.m_archetypes),
			m_componentMask(other.m_componentMask),
			m_componentOffsets(other.m_componentOffsets),
			m_currentArchetype(other.m_currentArchetype),
			m_currentEntity(other.m_currentEntity)
		{
			other.m_archetypes = nullptr;
		}

		ViewIterator<T, Ts...> operator=(const ViewIterator<T, Ts...>& other)
		{
			return ViewIterator<T, Ts...>(other);
		}

		T& operator*()
		{
			return GetHelper<T&>(0ULL);
			//
			//if constexpr (sizeof...(Ts) == 0)
			//{
			//	return GetHelper<T&>(0ULL);
			//}
			//else
			//{
			//	uint64_t index = 0;
			//	return std::tie(GetHelper<T&>(index++), GetHelper<Ts&>(index++)...);
			//}
		}

		ViewIterator<T, Ts...>& operator++()
		{
			auto atEnd = m_currentArchetype->second.GetComponents().end();

			if (++m_currentEntity == atEnd)
			{
				++m_currentArchetype;
				GetValidArchetype();
			}

			return *this;
		}

		ViewIterator<T, Ts...> operator++(int)
		{
			ViewIterator<T, Ts...> temp = *this;
			++(*this);
			return temp;
		}

		bool operator==(const ViewIterator<T, Ts...>& other) const
		{
			if (m_archetypes == nullptr || other.m_archetypes == nullptr)
				return false;
			if (m_archetypes != other.m_archetypes)
				return false;
			if (m_currentArchetype != other.m_currentArchetype)
				return false;
			return m_currentArchetype == m_archetypes->end()
				|| m_currentEntity == other.m_currentEntity;
		}

		bool operator!=(const ViewIterator<T, Ts...>& other) const
		{
			return !(*this == other);
		}

	private:
		void GetComponentOffsets()
		{
			internal::ComponentMask& atCompMask = m_currentArchetype->second.GetComponentMask();

			uint64_t index = 0;
			  m_componentOffsets[index++] = atCompMask.count(0, internal::GetComponentID<T>());
			((m_componentOffsets[index++] = atCompMask.count(0, internal::GetComponentID<Ts>())), ...);
		}

		void GetValidArchetype()
		{
			while (m_currentArchetype != m_archetypes->end())
			{
				auto& at = m_currentArchetype->second;
				if (at.GetComponents().size() > 0 && at.HasAllOf(m_componentMask))
				{
					m_currentEntity = at.GetComponents().begin();
					GetComponentOffsets();
					break;
				}
				++m_currentArchetype;
			}
		}

		template<typename U>
		U GetHelper(uint64_t index)
		{
			return std::any_cast<U>(**(m_currentEntity->second.data() + m_componentOffsets[index]));
		}

	private:
		std::unordered_map<internal::ComponentMask, internal::Archetype>* m_archetypes;
		const internal::ComponentMask& m_componentMask;

		std::unordered_map<internal::ComponentMask, internal::Archetype>::iterator	m_currentArchetype;
		std::unordered_map<Entity, std::vector<Ref<std::any>>>::iterator			m_currentEntity;
		//std::vector<uint64_t>														m_componentOffsets;
		uint64_t																	m_componentOffsets[1 + sizeof...(Ts)];

		friend class View<T, Ts...>;
	};


	template<typename T, typename... Ts>
	class View
	{
	public:
		using iterator = ViewIterator<T, Ts...>;

	private:
		View(std::unordered_map<internal::ComponentMask, internal::Archetype>& archetypes) :
			m_componentMask(internal::GetComponentMask<T>()),
			m_archetypes(archetypes),
			m_begin(&m_archetypes, m_componentMask, m_archetypes.begin()),
			m_end  (&m_archetypes, m_componentMask, m_archetypes.end())
		{ }

	public:
		iterator begin() { return m_begin; }
		iterator end()   { return m_end; }

	private:
		internal::ComponentMask												m_componentMask;
		std::unordered_map<internal::ComponentMask, internal::Archetype>&	m_archetypes;

		iterator															m_begin;
		iterator															m_end;

		friend class Registry;
	};
}