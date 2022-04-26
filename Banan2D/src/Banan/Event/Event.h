#pragma once

#include "Banan/Core/Base.h"

#pragma warning(push)
#pragma warning(disable : 26812)

namespace Banan
{

	enum class EventType
	{
		None = 0,
		KeyPressed, KeyReleased, KeyTyped,
		MousePressed, MouseReleased, MouseMoved, MouseScrolled,
		WindowResize, WindowClose
	};

	enum EventCategory
	{
		None		= 0,
		window		= BIT(0),
		Keyboard	= BIT(1),
		Mouse		= BIT(2)
	};

	class Event
	{
	public:
		virtual ~Event() = default;

		virtual EventType GetType() const = 0;
		virtual int GetCategory() const = 0;

		bool IsInCategory(EventCategory category) { return GetCategory() & category; }
	
	public:
		bool handled = false;

	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& e) :
			m_event(e)
		{}

		template<typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (m_event.GetType() == T::GetStaticType())
			{
				m_event.handled |= func(static_cast<T&>(m_event));
				return true;
			}
			return false;
		}

	private:
		Event& m_event;

	};

}

#pragma warning(pop)