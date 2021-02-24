#pragma once

#include "BGE/Event/Event.h"

#include <string>
#include <functional>

namespace Banan
{

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate(bool minimized) = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetVSync(bool enable) = 0;
		virtual bool GetVSync() const = 0;

		virtual void SetTitle(const std::wstring& title) = 0;

		static Scope<Window> Create(const std::wstring& title, uint32_t width, uint32_t height, bool vsync, const EventCallbackFn& callback);

	};

}