#pragma once

#include "./Event.h"
#include "../Core/MouseCode.h"

namespace Banan
{

	class MouseMoveEvent : public Event
	{
	public:
		MouseMoveEvent(int mouseX, int mouseY) :
			m_mouseX(mouseX), m_mouseY(mouseY)
		{ }

		int GetX() const { return m_mouseX; }
		int GetY() const { return m_mouseY; }

		static EventType GetStaticType()			{ return EventType::MouseMoved; }
		virtual EventType GetType() const override	{ return GetStaticType(); }
		virtual int GetCategory() const override	{ return EventCategory::Mouse; }

	private:
		int32_t m_mouseX, m_mouseY;
	};

	class MouseScrollEvent : public Event
	{
	public:
		MouseScrollEvent(float offset) :
			m_offset(offset)
		{ }

		float GetOffset() const { return m_offset; }

		static EventType GetStaticType()			{ return EventType::MouseScrolled; }
		virtual EventType GetType() const override	{ return GetStaticType(); }
		virtual int GetCategory() const override	{ return EventCategory::Mouse; }

	private:
		float m_offset;
	};

	class MousePressEvent : public Event
	{
	public:
		MousePressEvent(MouseCode mouseButton) :
			m_mouseButton(mouseButton) 
		{ }

		MouseCode GetButton() const { return m_mouseButton; }

		static EventType GetStaticType()			{ return EventType::MousePressed; }
		virtual EventType GetType() const override	{ return GetStaticType(); }
		virtual int GetCategory() const override	{ return EventCategory::Mouse; }

	private:
		MouseCode m_mouseButton;
	};

	class MouseReleaseEvent : public Event
	{
	public:
		MouseReleaseEvent(MouseCode mouseButton) :
			m_mouseButton(mouseButton)
		{ }

		MouseCode GetButton() const { return m_mouseButton; }

		static EventType GetStaticType()			{ return EventType::MouseReleased; }
		virtual EventType GetType() const override	{ return GetStaticType(); }
		virtual int GetCategory() const override	{ return EventCategory::Mouse; }

	private:
		MouseCode m_mouseButton;
	};

}