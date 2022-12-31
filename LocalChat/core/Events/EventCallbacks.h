#pragma once

#include <Message/Message.h>
#include <Events/EventName.h>

#include <functional>
#include <memory>

using MsgSPtr = std::shared_ptr<Message>;

template <EventName event>
struct EventFn
{
	using type = std::function<bool(void)>;
};

template <>
struct EventFn<EventName::MSG_SENT>
{
	using type = std::function<bool(MsgSPtr)>;
};

template <>
struct EventFn<EventName::MSG_DELIVERED>
{
	using type = std::function<bool(MsgSPtr)>;
};

template <>
struct EventFn<EventName::CLIENT_ACTIVE>
{
	using type = std::function<bool(void)>;
};

template <EventName event>
EventFn_t = typename EventFn<event>::type;