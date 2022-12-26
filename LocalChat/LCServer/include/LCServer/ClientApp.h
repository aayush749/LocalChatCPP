#pragma once

#include <Networking/Socket.h>
#include <Networking/WCharSocketStream.h>
#include <Message/Message.h>

#include <memory>
#include <thread>
#include <list>

/*
A container for a client, which manages sending and receiving messages to and from the server side
*/
class ClientApp
{
public:
	using StreamTp = ntwk::WCharSocketStream;
	using MessageSPtr = std::shared_ptr<Message>;
public:
	ClientApp(uint64_t clientHash, ntwk::Socket&& socket);
	ClientApp(ClientApp&& other) noexcept; // move constructor
	ClientApp(const ClientApp& other) = delete; // deleted copy constructor (since stream can't be copied)
	virtual ~ClientApp();
	ntwk::Socket& GetSocket() { return m_Socket; }
	StreamTp& GetStream() { return m_Stream; }
	std::list<MessageSPtr>& GetPendingMessages() { return m_PendingMessages; }

	/*
	Listens for incoming messages
	NOTE: Currently terminates messages with only first character of delimiter!!
	*/
	void Listen();

	void RemoveMessage(std::list<MessageSPtr>::iterator iterator);
	void ProcessMessage(const Message& msg);
private:
	uint64_t m_Hash;
	std::list<MessageSPtr> m_PendingMessages;
	StreamTp m_Stream;
	ntwk::Socket m_Socket;
	std::thread m_ListenerThread;
	bool m_ClientShouldStop;
};