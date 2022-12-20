#pragma once

#include <Networking/Socket.h>
#include <Networking/WCharSocketStream.h>
#include <Message/Message.h>

#include <memory>
#include <forward_list>

/*
A container for a client, which manages sending messages to and from the server
*/
class ClientApp
{
public:
	using StreamTp = ntwk::WCharSocketStream;
	using MessageSPtr = std::shared_ptr<Message>;
public:
	ClientApp(uint64_t clientHash, ntwk::Socket&& socket);
	
	ntwk::Socket& GetSocket() { return m_Socket; }
	StreamTp& GetStream() { return m_Stream; }
	
	void ProcessMessage(const Message& msg);
private:
	uint64_t m_Hash;
	std::forward_list<MessageSPtr> m_PendingMessages;
	StreamTp m_Stream;
	ntwk::Socket m_Socket;
};