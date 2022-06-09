#pragma once

#include "ComponentMask.h"

#include <unordered_map>
#include <typeindex>
#include <vector>
#include <any>
#include <algorithm>

namespace Banan::ECS
{

	namespace internal
	{

		class ComponentStorage
		{
		public:
			ComponentStorage() {}

			template<typename T>
			T& Get()
			{
				std::any& component = m_components[m_offsets[typeid(T)]];
				return std::any_cast<T&>(component);
			}

			template<typename T>
			const T& Get() const
			{
				const std::any& component = m_components[m_offsets[typeid(T)]];
				return std::any_cast<const T&>(component);
			}

		private:
			std::vector<std::any>							m_components;
			std::unordered_map<std::type_index, uint64_t>	m_offsets;
		};

	}

}