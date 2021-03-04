#pragma once

#include "Banan/Core/Base.h"

#include <glm/glm.hpp>

#include <string>

namespace Banan
{

	class Texture2D
	{
	public:
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::string& path);

		virtual boolean operator==(const Texture2D& other) const = 0;

	};

}