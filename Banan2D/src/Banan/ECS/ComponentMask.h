#pragma once

#include <vector>
#include <typeindex>

namespace Banan::ECS
{
	namespace internal
	{
		struct ComponentMask;

		uint64_t										g_componentCount = 0;
		std::unordered_map<std::type_index, uint64_t>	g_componentIDs;

		template<typename T>
		uint64_t GetComponentID()
		{
			auto it = g_componentIDs.find(typeid(T));
			if (it == g_componentIDs.end())
			{
				uint64_t ID = ++g_componentCount;
				g_componentIDs[typeid(T)] = ID;
				return ID;
			}
			return it->second;
		}
		template<typename... Comps>
		ComponentMask GetComponentMask()
		{
			ComponentMask mask;
			((mask.set(GetComponentID<Comps>())), ...);
			return mask;
		}

		struct ComponentMask
		{
			ComponentMask() :
				m_count(0),
				m_bits(g_componentCount, false)
			{ }

			ComponentMask(const ComponentMask& other) :
				m_bits(other.m_bits),
				m_count(other.m_count)
			{ }

			void set(uint64_t bit, bool state = true)
			{
				if (bit >= m_bits.size())
					m_bits.resize(bit + 1);
				if (state)	m_count += !test(bit);
				else		m_count -= test(bit);
				m_bits[bit] = state;
			}

			bool test(uint64_t bit) const
			{
				if (bit >= m_bits.size())
					m_bits.resize(bit + 1);
				return m_bits[bit];
			}

			uint64_t count() const { return m_count; }

			// counts set bits in range [from, to[
			uint64_t count(uint64_t from, uint64_t to) const
			{
				if (m_bits.size() < to)
					m_bits.resize(to);

				auto begin = m_bits.begin() + from;
				auto end = m_bits.begin() + to;

				uint64_t result = 0;
				for (; begin != end; begin++)
					result += *begin;

				return result;
			}
			
			bool has_all(const ComponentMask& other) const
			{
				if (m_bits.size() < other.m_bits.size())
					m_bits.resize(other.m_bits.size());
				for (uint64_t i = 0; i < other.m_bits.size(); i++)
					if (other.test(i) && !test(i))
						return false;
				return true;
			}

			bool operator==(const ComponentMask& other) const
			{
				if (m_bits.size() != other.m_bits.size())
				{
					uint64_t size = std::max(m_bits.size(), other.m_bits.size());
					m_bits.resize(size);
					other.m_bits.resize(size);
				}

				return m_bits == other.m_bits;
			}

			uint64_t size() const { return m_bits.size(); }

		private:
			mutable std::vector<bool> m_bits;
			uint64_t m_count;

			friend struct std::hash<ComponentMask>;
		};
	}
}

namespace std
{
	template<>
	struct hash<Banan::ECS::internal::ComponentMask>
	{
		size_t operator()(const Banan::ECS::internal::ComponentMask mask) const
		{
			return std::hash<std::vector<bool>>()(mask.m_bits);
		}
	};

}