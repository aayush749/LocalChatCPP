#pragma once

#include <Message/Message.h>
#include <Events/EventName.h>
#include <Networking/WCharSocketStream.h>

#include <functional>
#include <memory>
#include <chrono>

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
	// You would want to send all pending messages intended for the client when he raises a CLIENT_ACTIVE event
	using type = std::function<bool(uint64_t)>;
};

template <EventName event>
using EventFn_t = typename EventFn<event>::type;