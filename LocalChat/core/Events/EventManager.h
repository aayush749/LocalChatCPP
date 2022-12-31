#pragma once

#include <Events/EventName.h>
#include <Events/EventCallbacks.h>

#include <unordered_map>
#include <vector>
#include <variant>

class EventManager
{
private:
	EventManager();
	using EventCallbackVariant = std::variant<EventFn_t<EventName::MSG_SENT>, EventFn_t<EventName::MSG_DELIVERED>, EventFn_t<EventName::CLIENT_ACTIVE>>;
public:
	template <EventName event>
	static void AddListener(EventFn_t<event> callback);

	template <EventName event, typename... Args>
	static void EmplaceListener(Args... args);

	template <EventName event, typename... Args>
	static void Dispatch(Args... args);
private:
	inline static std::unordered_map<EventName, std::vector<EventCallbackVariant>> s_Callbacks;
};

template <EventName event, typename... Args>
void EventManager::Dispatch<event, Args>(Args... args)
{
	for (auto callbackVariant : s_Callbacks[event])
	{
		auto callback = std::get<EventFn_t<event>>(callbackVariant);
		bool result = callback(std::forward<Args>(args)...);
		if (result)
			break;
	}
}

template <EventName event>
void EventManager::AddListener<event>(EventFn_t<event> callback)
{
	s_Callbacks[event].push_back(callback);
}

template <EventName event, typename... Args>
void EventManager::EmplaceListener<event, Args>(Args... args)
{
	s_Callbacks[event].emplace_back(std::forward<Args>(args)...);
}