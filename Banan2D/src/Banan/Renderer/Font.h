#pragma once

#include "Banan/Core/Base.h"

#include "Banan/Renderer/Texture2D.h"

#include <glm/glm.hpp>

#include <unordered_map>

namespace Banan
{

	class Font
	{
	public:
		struct Character
		{
			glm::vec2 min;
			glm::vec2 max;
			glm::vec2 offset;
			float advance;
			glm::vec2 size;
		};

	public:
		Font(const std::string& font_file);

		static Ref<Font> Create(const std::string& font_file);

		void Bind(uint32_t slot = 0) const { m_texture->Bind(); }

		Ref<Texture2D> GetTexture() const { return m_texture; }

		const std::unordered_map<char, Character>& GetCharacters() const { return m_characters; }
		const std::unordered_map<char, std::unordered_map<char, float>>& GetKernings() const { return m_kernings; }

	private:
		Ref<Texture2D> m_texture;
		std::unordered_map<char, Character> m_characters;
		std::unordered_map<char, std::unordered_map<char, float>> m_kernings;
	};

}