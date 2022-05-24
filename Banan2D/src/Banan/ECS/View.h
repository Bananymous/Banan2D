#pragma once

#include "Banan/ECS/Archetype.h"

namespace Banan::ECS
{
	class Registry;

	template<typename T, typename... Ts>
	class View;

	namespace internal
	{

		template<bool B, typename... Ts>
		class ViewIterator_Impl;

		// ViewIterator for single type
		template<typename T>
		class ViewIterator_Impl<true, T>
		{
		protected:
			ViewIterator_Impl(
				std::unordered_map<ComponentMask, Archetype>* archetypes,
				const ComponentMask& componentMask,
				std::unordered_map<ComponentMask, Archetype>::iterator currentArchetype
			) :
				m_archetypes(archetypes),
				m_currentArchetype(currentArchetype),
				m_componentMask(componentMask)
			{
				FindValidArchetype();
			}

		public:
			ViewIterator_Impl() :
				m_archetypes(nullptr)
			{}

			T&		 operator*()		{ return GetR(); }
			T		 operator*()  const	{ return GetCR(); }
			T*		 operator->()		{ return &GetR(); }
			const T* operator->() const	{ return &GetCR(); }

			bool operator==(const ViewIterator_Impl<true, T>& other) const
			{
				if (!m_archetypes || !other.m_archetypes)
					return false;
				if (m_archetypes != other.m_archetypes)
					return false;
				if (m_currentArchetype != other.m_currentArchetype)
					return false;
				return m_currentArchetype == m_archetypes->end()
					|| m_currentEntity == other.m_currentEntity;
			}
			bool operator!=(const ViewIterator_Impl<true, T>& other) const
			{
				return !(*this == other);
			}

			ViewIterator_Impl<true, T>& operator++()
			{
				Archetype& at = m_currentArchetype->second;
				if (++m_currentEntity == at.GetEntities().end())
				{
					m_currentArchetype++;
					FindValidArchetype();
				}
				return *this;
			}
			ViewIterator_Impl<true, T> operator++(int)
			{
				auto temp = *this;
				++(*this);
				return temp;
			}

		private:
			void GetComponentOffset()
			{
				ComponentMask& atCompMask = m_currentArchetype->second.GetComponentMask();
				m_componentOffset = atCompMask.count(0, GetComponentID<T>());
			}

			void FindValidArchetype()
			{
				while (m_currentArchetype != m_archetypes->end())
				{
					Archetype& at = m_currentArchetype->second;
					if (!at.GetEntities().empty() && at.HasAllOf(m_componentMask))
					{
						m_currentEntity = at.GetEntities().begin();
						GetComponentOffset();
						return;
					}
					m_currentArchetype++;
				}
			}

			T& GetR()
			{
				return std::any_cast<T&>(*(m_currentEntity->second[m_componentOffset]));
			}
			const T& GetCR() const
			{
				return std::any_cast<const T&>(*(m_currentEntity->second[m_componentOffset]));
			}

		private:
			std::unordered_map<ComponentMask, Archetype>*						m_archetypes;
			std::unordered_map<ComponentMask, Archetype>::iterator				m_currentArchetype;
			std::unordered_map<Entity, std::vector<Ref<std::any>>>::iterator	m_currentEntity;

			uint64_t															m_componentOffset;
			const ComponentMask&												m_componentMask;
		};

		// ViewIterator for multiple types
		template<typename... Ts>
		class ViewIterator_Impl<false, Ts...>
		{
		protected:
			ViewIterator_Impl(
				std::unordered_map<ComponentMask, Archetype>* archetypes,
				const ComponentMask& componentMask,
				std::unordered_map<ComponentMask, Archetype>::iterator currentArchetype
			) :
				m_archetypes(archetypes),
				m_currentArchetype(currentArchetype),
				m_componentMask(componentMask)
			{
				FindValidArchetype();
			}

		public:
			ViewIterator_Impl() :
				m_archetypes(nullptr)
			{}

			std::tuple<Ts&...>	operator*()			{ return std::tie(GetR<Ts>()...); }
			std::tuple<Ts...>	operator*()  const	{ return std::make_tuple(Get<Ts>()...); }
			
			bool operator==(const ViewIterator_Impl<false, Ts...>& other) const
			{
				if (!m_archetypes || !other.m_archetypes)
					return false;
				if (m_archetypes != other.m_archetypes)
					return false;
				if (m_currentArchetype != other.m_currentArchetype)
					return false;
				return m_currentArchetype == m_archetypes->end()
					|| m_currentEntity == other.m_currentEntity;
			}
			bool operator!=(const ViewIterator_Impl<false, Ts...>& other) const
			{
				return !(*this == other);
			}

			ViewIterator_Impl<false, Ts...>& operator++()
			{
				Archetype& at = m_currentArchetype->second;
				if (++m_currentEntity == at.GetEntities().end())
				{
					m_currentArchetype++;
					FindValidArchetype();
				}
				return *this;
			}
			ViewIterator_Impl<false, Ts...>	operator++(int)
			{
				auto temp = *this;
				++(*this);
				return temp;
			}

		private:
			void GetComponentOffset()
			{
				ComponentMask& atCompMask = m_currentArchetype->second.GetComponentMask();
				((m_componentOffsets[typeid(Ts)] = atCompMask.count(0, GetComponentID<Ts>())), ...);
			}

			void FindValidArchetype()
			{
				while (m_currentArchetype != m_archetypes->end())
				{
					Archetype& at = m_currentArchetype->second;
					if (!at.GetEntities().empty() && at.HasAllOf(m_componentMask))
					{
						m_currentEntity = at.GetEntities().begin();
						GetComponentOffset();
						return;
					}
					m_currentArchetype++;
				}
			}

			template<typename T>
			T& GetR()
			{
				return std::any_cast<T&>(*(m_currentEntity->second[m_componentOffsets[typeid(T)]]));
			}
			template<typename T>
			T Get() const
			{
				return std::any_cast<T>(*(m_currentEntity->second[m_componentOffsets[typeid(T)]]));
			}

		private:
			std::unordered_map<ComponentMask, Archetype>*						m_archetypes;
			std::unordered_map<ComponentMask, Archetype>::iterator				m_currentArchetype;
			std::unordered_map<Entity, std::vector<Ref<std::any>>>::iterator	m_currentEntity;

			std::unordered_map<std::type_index, uint64_t>						m_componentOffsets;
			const ComponentMask& m_componentMask;
		};

	}

	template<typename T, typename... Ts>
	class ViewIterator : public internal::ViewIterator_Impl<(sizeof...(Ts) == 0), T, Ts...>
	{
	private:
		ViewIterator(
			std::unordered_map<internal::ComponentMask, internal::Archetype>* archetypes,
			const internal::ComponentMask& componentMask,
			std::unordered_map<internal::ComponentMask, internal::Archetype>::iterator current
		)
			: internal::ViewIterator_Impl<(sizeof...(Ts) == 0), T, Ts...>(archetypes, componentMask, current)
		{}

	public:
		ViewIterator() {}

		friend class View<T, Ts...>;
	};

#if 0
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
			std::memcpy(m_componentOffsets, other.m_componentOffsets, (1 + sizeof...(Ts)) * sizeof(uint64_t));
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

		template<typename = std::enable_if<sizeof...(Ts) == 0, bool>::type = true>
		T& operator*()
		{
			return GetCR<T&>(0);
		}

		template<typename = std::enable_if<sizeof...(Ts) >= 1, bool>::type = false>
		std::tuple<T&, Ts&...> operator*()
		{
			uint64_t index = 0;
			return std::tie(GetCR<T&>(index++), GetCR<Ts&>(index++)...);
		}

		ViewIterator<T, Ts...>& operator++()
		{
			auto atEnd = m_currentArchetype->second.GetEntities().end();

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
				if (at.GetEntities().size() > 0 && at.HasAllOf(m_componentMask))
				{
					m_currentEntity = at.GetEntities().begin();
					GetComponentOffsets();
					break;
				}
				++m_currentArchetype;
			}
		}

		template<typename U>
		U GetCR(uint64_t index)
		{
			return std::any_cast<U>(**(m_currentEntity->second.data() + m_componentOffsets[index]));
		}

	private:
		std::unordered_map<internal::ComponentMask, internal::Archetype>* m_archetypes;
		const internal::ComponentMask& m_componentMask;

		std::unordered_map<internal::ComponentMask, internal::Archetype>::iterator	m_currentArchetype;
		std::unordered_map<Entity, std::vector<Ref<std::any>>>::iterator			m_currentEntity;
		uint64_t																	m_componentOffsets[1 + sizeof...(Ts)];

		friend class View<T, Ts...>;
	};
#endif


	template<typename T, typename... Ts>
	class View
	{
	public:
		using iterator = ViewIterator<T, Ts...>;

	private:
		View(std::unordered_map<internal::ComponentMask, internal::Archetype>& archetypes) :
			m_componentMask(internal::ComponentMask::GetComponentMask<T, Ts...>()),
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