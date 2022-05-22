#pragma once

#include "Banan/ECS/Archetype.h"
#include "Banan/ECS/View.h"

#include <unordered_set>

namespace Banan::ECS
{

	class Registry
	{
	public:
		Registry() :
			m_emptyArchetype(internal::ComponentMask())
		{ }

		Registry(const Registry& registry) = delete;
		Registry& operator=(const Registry& registry) = delete;

		Registry& operator=(Registry&& registry) noexcept
		{
			m_archetypes = std::move(registry.m_archetypes);
			m_entities = std::move(registry.m_entities);
			m_emptyArchetype = std::move(registry.m_emptyArchetype);

			registry.m_emptyArchetype.Clear();
			registry.m_archetypes.clear();
			registry.m_entities.clear();
		}
		Registry(Registry&& registry) noexcept :
			m_archetypes(std::move(registry.m_archetypes)),
			m_entities(std::move(registry.m_entities)),
			m_emptyArchetype(registry.m_emptyArchetype)
		{
			registry.m_emptyArchetype.Clear();
			registry.m_archetypes.clear();
			registry.m_entities.clear();
		}


		/* ################ ENTITIES ################ */

		// Creates an entity
		Entity Create()
		{
			static uint32_t ID = 1;
			Entity entity(ID++);
			m_emptyArchetype.AddEntity(entity, std::vector<Ref<std::any>>());
			m_entities.insert(entity);
			return entity;
		}

		// Destroys an entity and its components
		void Destroy(Entity entity)
		{
			m_entities.erase(entity);
			GetArchetype(entity)->DestroyEntity(entity);
		}

		// Checks if an entity has not been destroyed
		bool Valid(Entity entity)
		{
			return m_entities.find(entity) != m_entities.end();
		}


		/* ############### COMPONENTS ############### */

		// Add a component to an entity
		template<typename T>
		void Add(Entity entity, const T& component)
		{
			Ref<std::any>* pComp = EmplaceBase<T>(entity);
			*pComp = CreateRef<std::any>(component);
		}

		template<typename T>
		T& Emplace(Entity entity)
		{
			Ref<std::any>* pComp = EmplaceBase<T>(entity);
			*pComp = CreateRef<std::any>(std::make_any<T>());
			return std::any_cast<T&>(*pComp->get());
		}

		// Add and construct a component in-place for an entity
		template<typename T, typename... Args>
		T& Emplace(Entity entity, Args&&... args)
		{
			Ref<std::any>* pComp = EmplaceBase<T>(entity);
			*pComp = CreateRef<std::any>(std::in_place_type<T>, std::forward<Args>(args)...);
			return std::any_cast<T&>(*pComp->get());
		}

		// Replace a component of an entity in-place
		template<typename T, typename Fn>
		void Patch(Entity entity, const Fn& function)
		{
			GetArchetype(entity)->Patch<T>(entity, function);
		}

		// Replace a component of an entity
		template<typename T, typename... Args>
		void Replace(Entity entity, Args... args)
		{
			GetArchetype(entity)->Replace<T>(entity, std::forward<Args>(args)...);
		}

		// Replace a component of an entity if it exists. Otherwise emplace
		template<typename T, typename... Args>
		void EmplaceOrReplace(Entity entity, Args... args)
		{
			internal::Archetype* pAt = GetArchetype(entity);
			if (pAt->HasAllOf(internal::GetComponentMask<T>()))
				pAt->Replace<T>(entity, std::forward<Args>(args)...);
			else
				Emplace<T>(entity, std::forward<Args>(args)...);
		}

		// Remove a component from an entity
		template<typename T>
		void Remove(Entity entity)
		{
			using namespace internal;

			Archetype* pAtOld = GetArchetype(entity);
			
			// Component masks
			const ComponentMask&			compMaskOld		= pAtOld->GetComponentMask();
			uint64_t						compIdNew		= GetComponentID<T>();
			ComponentMask					compMaskNew		= compMaskOld;
			uint64_t						compIndex		= compMaskOld.count(0, compIdNew);
			compMaskNew.set(compIdNew, false);

			// Get and possibly update Archetype pointers
			Archetype* pAtNew = GetArchetype(compMaskNew);
			if (pAtNew == nullptr)
			{
				auto[it, _] = m_archetypes.emplace(std::make_pair(compMaskNew, Archetype(compMaskNew)));
				pAtNew = &it->second;
				pAtOld = GetArchetype(entity);
			}

			// Component pointers
			std::vector<Ref<std::any>> compPointers = pAtOld->GetEntityComponents(entity);
			compPointers.erase(compPointers.begin() + compIndex);

			// Add and destroy entites from corresponding archetypes
			pAtOld->DestroyEntity(entity);
			pAtNew->AddEntity(entity, compPointers);
		}

		template<typename T>
		T& Get(Entity entity)
		{
			return GetArchetype(entity)->Get<T>(entity);
		}
		template<typename T>
		const T& Get(Entity entity) const
		{
			return GetArchetype(entity)->Get<T>(entity);
		}

		template<typename T1, typename T2, typename... Ts>
		auto Get(Entity entity)
		{
			return GetArchetype(entity)->Get<T1, T2, Ts...>(entity);
		}
		template<typename T1, typename T2, typename... Ts>
		auto Get(Entity entity) const
		{
			return GetArchetype(entity)->Get<T1, T2, Ts...>(entity);
		}

		template<typename T, typename... Ts>
		bool HasAllOf(Entity entity) const
		{
			return GetArchetype(entity)->HasAllOf(internal::GetComponentMask<T, Ts...>());
		}

		template<typename T, typename... Ts>
		Banan::ECS::View<T, Ts...> View()
		{
			return Banan::ECS::View<T, Ts...>(m_archetypes);
		}

	private:
		template<typename T>
		Ref<std::any>* EmplaceBase(Entity entity)
		{
			using namespace internal;

			Archetype* pAtOld = GetArchetype(entity);

			// Components masks
			const ComponentMask&	compMaskOld		= pAtOld->GetComponentMask();
			uint64_t				compIdNew		= GetComponentID<T>();
			ComponentMask			compMaskNew		= compMaskOld;
			uint64_t				newCompIndex	= compMaskOld.count(0, compIdNew);
			compMaskNew.set(compIdNew);

			// Get and possibly update Archetype pointers
			Archetype* pAtNew = GetArchetype(compMaskNew);
			if (pAtNew == nullptr)
			{
				auto [it, _] = m_archetypes.emplace(std::make_pair(compMaskNew, Archetype(compMaskNew)));
				pAtNew = &it->second;
				pAtOld = GetArchetype(entity);
			}

			// Component pointers
			std::vector<Ref<std::any>> componentPtrs = pAtOld->GetEntityComponents(entity);
			componentPtrs.insert(componentPtrs.begin() + newCompIndex, nullptr);

			pAtOld->DestroyEntity(entity);
			Ref<std::any>* pComp = pAtNew->AddEntity(entity, componentPtrs);			
			
			return pComp + newCompIndex;
		}

	private:
		internal::Archetype* GetArchetype(Entity entity)
		{
			if (m_emptyArchetype.HasEntity(entity))
				return &m_emptyArchetype;
			for (auto& [_, at] : m_archetypes)
				if (at.HasEntity(entity))
					return &at;
			throw nullptr;
		}

		const internal::Archetype* GetArchetype(Entity entity) const
		{
			if (m_emptyArchetype.HasEntity(entity))
				return &m_emptyArchetype;
			for (const auto& [_, at] : m_archetypes)
				if (at.HasEntity(entity))
					return &at;
			return nullptr;
		}

		internal::Archetype* GetArchetype(const internal::ComponentMask& mask)
		{
			if (m_emptyArchetype.GetComponentMask() == mask)
				return &m_emptyArchetype;
			if (m_archetypes.find(mask) == m_archetypes.end())
				return nullptr;
			return &m_archetypes.at(mask);
		}

	private:
		std::unordered_set<Entity>											m_entities;

		internal::Archetype m_emptyArchetype;
		std::unordered_map<internal::ComponentMask, internal::Archetype>	m_archetypes;
	};

}