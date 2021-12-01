#include "bgepch.h"
#include "Font.h"

#include "RendererAPI.h"

#include <fstream>
#include <iomanip>

namespace Banan
{

	Font::Font(const std::string& font_data, const std::string& font_image)
	{
		m_texture = Texture2D::Create(font_image);

		std::ifstream file(font_data);
		std::string word;

		float font_size, img_w, img_h;

		file >> word;
		file >> word;

		file >> word;
		font_size = (float)std::atof(word.c_str() + 5);

		while (word.substr(0, 4) != "base")
			file >> word;

		file >> word;
		img_w = (float)std::atof(word.c_str() + 7);

		file >> word;
		img_h = (float)std::atof(word.c_str() + 7);

		while (word != "chars")
			file >> word;

		// Read character count
		file >> word;
		uint32_t count = std::atoi(word.c_str() + 6);

		for (uint32_t i = 0; i < count; i++)
		{
			char key;
			float x, y, w, h, xoff, yoff, advance;

			// Check that this is a characher
			file >> word;
			if (word != "char")
				continue;

			// Get character
			file >> word;
			key = std::atoi(word.c_str() + 3);

			// Get x
			file >> word;
			x = (float)std::atof(word.c_str() + 2);

			// Get y
			file >> word;
			y = (float)std::atof(word.c_str() + 2);

			// Get width
			file >> word;
			w = (float)std::atof(word.c_str() + 6);

			// Get height
			file >> word;
			h = (float)std::atof(word.c_str() + 7);

			// Get xoffset
			file >> word;
			xoff = (float)std::atof(word.c_str() + 8);

			// Get yoffset
			file >> word;
			yoff = (float)std::atof(word.c_str() + 8);

			// Get advance
			file >> word;
			advance = (float)std::atof(word.c_str() + 9);

			// Skip last 2 things
			file >> word;
			file >> word;

			m_characters[key].min = glm::vec2(x + 0, img_h - (y + h)) / glm::vec2(img_w, img_h);
			m_characters[key].max = glm::vec2(x + w, img_h - (y + 0)) / glm::vec2(img_w, img_h);
			m_characters[key].size = glm::vec2(w, h) / font_size;
			m_characters[key].offset = glm::vec2(xoff, yoff) / font_size;
			m_characters[key].advance = advance / font_size;
		}

		file >> word;
		if (word != "kernings")
			return;

		uint32_t kerning_count;

		file >> word;
		kerning_count = std::atoi(word.c_str() + 6);

		for (uint32_t i = 0; i < kerning_count; i++)
		{
			char first, second;
			float value;

			file >> word;

			file >> word;
			first = std::atoi(word.c_str() + 6);

			file >> word;
			second = std::atoi(word.c_str() + 7);

			file >> word;
			value = (float)std::atof(word.c_str() + 7);

			m_kernings[first][second] = value / font_size;
		}
	}


	Ref<Font> Font::Create(const std::string& font_data, const std::string& font_image)
	{
		return CreateRef<Font>(font_data, font_image);
	}

}