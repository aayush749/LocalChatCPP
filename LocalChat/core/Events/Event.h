#pragma once

#include <Events/EventManager.h>


template <EventName event>
class Event
{
private:
	Event();
public:
	template <typename... Args>
	static void Raise(Args... args);
};

template <EventName event, typename... Args>
void Event<event>::Raise<Args>(Args... args)
{
	EventManager::Dispatch<event>(std::forward<Args>(args)...);
}