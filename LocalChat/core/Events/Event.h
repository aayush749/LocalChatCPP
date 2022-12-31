#pragma once

#include <Events/EventManager.h>


template <EventName event>
class Event
{
private:
	Event();
public:
	template <typename... Args>
	static void Raise(Args... args)
	{
		EventManager::Dispatch<event>(std::forward<Args>(args)...);
	}
};