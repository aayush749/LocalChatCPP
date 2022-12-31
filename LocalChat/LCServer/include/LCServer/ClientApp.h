#pragma once

#include <Networking/Socket.h>
#include <Networking/WCharSocketStream.h>
#include <Message/Message.h>

#include <memory>
#include <thread>
#include <mutex>
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
	ClientApp(ntwk::Socket&& socket);
	ClientApp(uint64_t hash, ntwk::Socket&& socket, const std::list<MessageSPtr>& pendingMessages);
	ClientApp(ClientApp&& other) noexcept; // move constructor
	ClientApp(const ClientApp& other) = delete; // deleted copy constructor (since stream can't be copied)
	virtual ~ClientApp();
	ntwk::Socket& GetSocket() { return m_Socket; }
	StreamTp& GetStream() { return m_Stream; }
	std::list<MessageSPtr>& GetPendingMessages() { return m_PendingMessages; }

	std::mutex& GetPendingMsgListMutex() { return m_PendingMsgMutex; }
	std::mutex& GetStrmMutex() { return m_StrmMutex; }
	bool IsValidClient() const { return m_IsValidClient; }
	uint64_t GetHash() const { return m_Hash; }

	/*
	Listens for incoming messages
	NOTE: Currently terminates messages with only first character of delimiter!!
	*/
	void Listen();

	void RemoveMessage(std::list<MessageSPtr>::iterator iterator);
	void ProcessMessage(const Message& msg);
private:
	bool m_IsValidClient;
	uint64_t m_Hash;
	std::mutex m_PendingMsgMutex, m_StrmMutex;
	std::list<MessageSPtr> m_PendingMessages;
	StreamTp m_Stream;
	ntwk::Socket m_Socket;
	std::thread m_ListenerThread;
	bool m_ClientShouldStop;
};