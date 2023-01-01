#pragma once

#include <Events/EventName.h>
#include <Events/EventCallbacks.h>

#include <unordered_map>
#include <vector>
#include <any>

class EventManager
{
private:
	EventManager();
	using EventCallbackTp = std::any;
	using EventCallbackStorageTp = std::vector<EventCallbackTp>;
public:
	template <EventName event>
	static void AddListener(EventFn_t<event> callback);

	template <EventName event, typename... Args>
	static void EmplaceListener(Args... args)
	{
		s_Callbacks[event].emplace_back(std::forward<Args>(args)...);
	}

	template <EventName event, typename... Args>
	static void Dispatch(Args... args)
	{
		for (auto& callbackVariant : s_Callbacks[event])
		{
			EventFn_t<event>& callback = std::any_cast<EventFn_t<event>>(callbackVariant);
			bool result = callback(std::forward<Args>(args)...);
			if (result)
				break;
		}
	}
private:
	inline static std::unordered_map<EventName, EventCallbackStorageTp> s_Callbacks;
};


template <EventName event>
void EventManager::AddListener(EventFn_t<event> callback)
{
	s_Callbacks[event].push_back(callback);
}