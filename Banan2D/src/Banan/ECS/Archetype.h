#pragma once

#include "../Core/Base.h"

#include "Entity.h"
#include "ComponentMask.h"

#include <any>
#include <tuple>
#include <vector>
#include <stdexcept>
#include <unordered_map>

namespace Banan::ECS
{
	namespace internal
	{
		class Archetype
		{
		public:
			Archetype(const ComponentMask& componentMask) :
				m_componentMask(componentMask)
			{ }

			Ref<std::any>* AddEntity(Entity entity, const std::vector<Ref<std::any>>& componentPointers)
			{
				auto [it, _] = m_components.emplace(std::make_pair(entity, componentPointers));
				return it->second.data();
			}

			void DestroyEntity(Entity entity)
			{
				m_components.erase(entity);
			}

			template<typename T, typename Fn>
			void Patch(Entity entity, const Fn& function)
			{
				Ref<std::any>& comp = GetComponent<T>(entity);
				function(std::any_cast<T&>(*comp));
			}

			template<typename T, typename... Args>
			void Replace(Entity entity, Args&&... args)
			{
				Ref<std::any>& comp = GetComponent<T>(entity);
				comp = CreateRef<std::any>(std::in_place_type<T>, std::forward<Args>(args)...);
			}

			bool HasEntity(Entity entity) const
			{
				return m_components.find(entity) != m_components.end();
			}

			bool HasAllOf(const ComponentMask& mask) const
			{
				return m_componentMask.has_all(mask);
			}

			const std::vector<Ref<std::any>>& GetEntityComponents(Entity entity) const
			{
				return m_components.at(entity);
			}

			template<typename T>
			T& Get(Entity entity)
			{
				std::vector<Ref<std::any>>& comps = m_components.at(entity);
				return GetHelper<T>(comps);
			}
			template<typename T>
			const T& Get(Entity entity) const
			{
				const std::vector<Ref<std::any>>& comps = m_components.at(entity);
				return GetHelper<T>(comps);
			}

			template<typename T1, typename T2, typename... Ts>
			auto Get(Entity entity)
			{
				std::vector<Ref<std::any>>& comps = m_components.at(entity);
				return std::tie(GetHelper<T1>(comps), GetHelper<T2>(comps), GetHelper<Ts>(comps)...);
			}
			template<typename T1, typename T2, typename... Ts>
			auto Get(Entity entity) const
			{
				const std::vector<Ref<std::any>>& comps = m_components.at(entity);
				return std::tie(GetHelper<T1>(comps), GetHelper<T2>(comps), GetHelper<Ts>(comps)...);
			}

					ComponentMask& GetComponentMask()		{ return m_componentMask; }
			const	ComponentMask& GetComponentMask() const	{ return m_componentMask; }

			std::unordered_map<Entity, std::vector<Ref<std::any>>>& GetComponents()
			{
				return m_components;
			}

			void Clear()
			{
				m_components.clear();
			}

		private:
			template<typename T>
			T& GetHelper(std::vector<Ref<std::any>>& comps)
			{
				for (auto& comp : comps)
					if (comp->type() == typeid(T))
						return std::any_cast<T&>(*comp.get());
				throw std::out_of_range("Component not found\n" __FUNCTION__);
			}

			template<typename T>
			const T& GetHelper(const std::vector<Ref<std::any>>& comps) const
			{
				for (auto& comp : comps)
					if (comp->type() == typeid(T))
						return std::any_cast<T&>(*comp.get());
				throw std::out_of_range("Component not found\n" __FUNCTION__);
			}

			template<typename T>
			Ref<std::any>& GetComponent(Entity entity)
			{
				std::vector<Ref<std::any>>& comps = m_components.at(entity);
				for (auto& comp : comps)
					if (comp->type() == typeid(T))
						return comp;
				throw std::out_of_range("Component not found\n" __FUNCTION__);
			}

		private:
			mutable ComponentMask									m_componentMask;
			std::unordered_map<Entity, std::vector<Ref<std::any>>>	m_components;
		};
	}
}