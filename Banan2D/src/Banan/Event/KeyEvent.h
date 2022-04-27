#pragma once

#include "./Event.h"
#include "../Core/KeyCode.h"

namespace Banan
{

	class KeyPressEvent : public Event
	{
	public:
		KeyPressEvent(KeyCode keycode, uint16_t repeatCount) :
			m_keycode(keycode), m_repeatCount(repeatCount)
		{ }

		KeyCode GetKeyCode() const		{ return m_keycode; }
		uint16_t GetRepeatCount() const	{ return m_repeatCount; }

		static EventType GetStaticType()			{ return EventType::KeyPressed; }
		virtual EventType GetType() const override	{ return GetStaticType(); }
		virtual int GetCategory() const override	{ return EventCategory::Keyboard; }

	private:
		KeyCode m_keycode;
		uint16_t m_repeatCount;
	};

	class KeyReleaseEvent : public Event
	{
	public:
		KeyReleaseEvent(KeyCode keycode) :
			m_keycode(keycode)
		{ }

		KeyCode GetKeyCode() const { return m_keycode; }

		static EventType GetStaticType()			{ return EventType::KeyReleased; }
		virtual EventType GetType() const override	{ return GetStaticType(); }
		virtual int GetCategory() const override	{ return EventCategory::Keyboard; }

	private:
		KeyCode m_keycode;
	};

	class KeyTypeEvent : public Event
	{
	public:
		KeyTypeEvent(KeyCode keycode) :
			m_keycode(keycode)
		{ }

		KeyCode GetKeyCode() const { return m_keycode; }

		static EventType GetStaticType()			{ return EventType::KeyTyped; }
		virtual EventType GetType() const override	{ return GetStaticType(); }
		virtual int GetCategory() const override	{ return EventCategory::Keyboard; }

	private:
		KeyCode m_keycode;
	};

}