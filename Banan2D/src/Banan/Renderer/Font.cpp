#include "bgepch.h"
#include "Font.h"

#include "RendererAPI.h"

#include <fstream>
#include <iomanip>

#include <filesystem>

#include <cstring>

namespace Banan
{

	Font::Font(const std::string& font_file)
	{
		std::filesystem::path path(font_file);
		BANAN_ASSERT(std::filesystem::exists(path), "File \"%s\" could not be found.\n", font_file.c_str());
		if (path.extension() != ".fnt")
		{
			BANAN_WARN("Only .fnt files with image texture are supported.\n");
			m_texture = nullptr;
			return;
		}
				
		std::ifstream file(font_file);
		std::string image_path = font_file.substr(0, font_file.find_last_of("/\\") + 1);
		std::string word;

		float font_size, img_w, img_h;

		// Get font size
		while (strncmp(word.c_str(), "size", 4))
			file >> word;
		font_size = (float)std::atof(word.c_str() + 5);

		// Get image size
		while (strncmp(word.c_str(), "scaleW", 6))
			file >> word;
		img_w = (float)std::atof(word.c_str() + 7);
		file >> word;
		img_h = (float)std::atof(word.c_str() + 7);

		// Get font image path
		while (strncmp(word.c_str(), "file", 4))
			file >> word;
		image_path.append(word.c_str() + 6);
		while (word.back() != '"') {
			file >> word;
			image_path.append(word);
		}
		image_path.pop_back();
			
		// Read character count
		while (strncmp(word.c_str(), "count", 5))
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
		if (word == "kernings")
		{
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

		file.close();
		m_texture = Texture2D::Create(image_path);
	}


	Ref<Font> Font::Create(const std::string& font_file)
	{
		return CreateRef<Font>(font_file);
	}

}