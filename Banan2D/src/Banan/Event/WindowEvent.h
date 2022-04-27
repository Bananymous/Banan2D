#pragma once

#include "./Event.h"

namespace Banan
{

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height) :
			m_width(width), m_height(height)
		{ }

		uint32_t GetWidth() const	{ return m_width; }
		uint32_t GetHeight() const	{ return m_height; }

		static EventType GetStaticType()			{ return EventType::WindowResize; }
		virtual EventType GetType() const override	{ return GetStaticType(); }
		virtual int GetCategory() const override	{ return EventCategory::window; }

	private:
		uint32_t m_width, m_height;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent()
		{ }

		static EventType GetStaticType()			{ return EventType::WindowClose; }
		virtual EventType GetType() const override	{ return GetStaticType(); }
		virtual int GetCategory() const override	{ return EventCategory::window; }
	};

}