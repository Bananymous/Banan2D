#pragma once

#include <vector>
#include <typeindex>

namespace Banan::ECS
{
	namespace internal
	{
		uint64_t										g_componentCount = 0;
		std::unordered_map<std::type_index, uint64_t>	g_componentIDs;

		template<typename T>
		uint64_t GetComponentID()
		{
			auto it = g_componentIDs.find(typeid(T));
			if (it == g_componentIDs.end())
			{
				uint64_t ID = g_componentCount++;
				g_componentIDs[typeid(T)] = ID;
				return ID;
			}
			return it->second;
		}

		struct ComponentMask
		{
			ComponentMask() :
				m_count(0)
			{ }

			ComponentMask(const ComponentMask& other) :
				m_bits(other.m_bits),
				m_count(other.m_count)
			{ }

			void set(uint64_t bit, bool state = true)
			{
				if (state)
				{
					if (bit >= m_bits.size())
					{
						m_bits.resize(bit + 1);
						m_bits[bit] = true;
						m_count++;
					}
					else if (!m_bits[bit])
					{
						m_bits[bit] = true;
						m_count++;
					}
				}
				else
				{
					if (bit < m_bits.size() && m_bits[bit])
					{
						m_bits[bit] = false;
						m_count--;
					}
				}
			}

			bool test(uint64_t bit) const
			{
				if (bit >= m_bits.size())
					return false;
				return m_bits[bit];
			}

			uint64_t count() const { return m_count; }

			// counts set bits in range [from, to[
			uint64_t count(uint64_t from, uint64_t to) const
			{
				if (to > m_bits.size())
					to = m_bits.size();

				uint64_t result = 0;
				for (uint64_t i = from; i < to; i++)
					result += m_bits[i];

				return result;
			}
			
			bool has_all(const ComponentMask& other) const
			{
				for (uint64_t i = this->size(); i < other.size(); i++)
					if (other.m_bits[i])
						return false;

				uint64_t min_size = this->size() < other.size() ? this->size() : other.size();

				for (uint64_t i = 0; i < min_size; i++)
					if (other.m_bits[i] && !m_bits[i])
						return false;

				return true;
			}

			bool operator==(const ComponentMask& other) const
			{
				const ComponentMask& s = this->size() < other.size() ? *this : other;
				const ComponentMask& b = this->size() > other.size() ? *this : other;

				for (uint64_t i = 0; i < s.size(); i++)
					if (s.m_bits[i] != b.m_bits[i])
						return false;

				for (uint64_t i = s.size(); i < b.size(); i++)
					if (b.m_bits[i])
						return false;

				return true;
			}

			uint64_t size() const { return m_bits.size(); }

			template<typename... Comps>
			static ComponentMask GetComponentMask()
			{
				ComponentMask mask;
				((mask.set(GetComponentID<Comps>())), ...);
				return mask;
			}

		private:
			std::vector<bool> m_bits;
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