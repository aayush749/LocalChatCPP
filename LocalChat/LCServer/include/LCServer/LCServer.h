#pragma once

#include <Networking/Networking.h>
#include <Networking/ServerSocket.h>
#include <Networking/Socket.h>
#include <Message/Message.h>
#include <Networking/WCharSocketStream.h>

#include <LCServer/ClientApp.h>

#include <optional>
#include <thread>
#include <memory>
#include <utility>
#include <unordered_map>
#include <forward_list>

enum class LCServerError
{
	SERVER_BUSY,
	MAX_CLIENTS_REACHED
};

class LCServer
{
	// Store the client's IP address and the undelivered message list corresponding to the ClientHash
	using ClientHashTp = uint64_t;
	using IPAddressTp = std::string_view;
	using MessageSPtr = std::shared_ptr<Message>;
	using ServerDB = std::unordered_map<ClientHashTp, ClientApp&>;

public:
	LCServer(const std::optional<unsigned int> maxClients, const std::string_view ipAddress = "0.0.0.0", const int addressFamily = AF_INET, const uint16_t port = 7777);

	// Makes the server listen to incoming client connections
	void ListenForClients();

	ntwk::Socket& GetClientSockFromClientHash(uint64_t clientHash);

	// Process incoming messages and act according to it
	void MessageDispatcher();
	void RemoveClient(ClientHashTp clientHash);

	// Temporary Getters for thread
	std::thread& GetListenerThread() { return m_ListenerThread; }
	std::thread& GetMsgDispatcherThread() { return m_MessageDispatcherThread; }

	virtual ~LCServer();
private:
	void SendMsgToClient(Message& msgRef, ClientHashTp clientHash);
	void AddClient(ClientHashTp clientHash, ClientApp&& app);
private:
	ntwk::ServerSocket m_ServerSock;
	ServerDB m_ServerDB;
	std::optional<unsigned int> m_MaxClients;
	bool m_ServerShouldStop;

	inline static ClientHashTp s_ClientCtr = 0;
	inline static ClientHashTp s_BaseClientHash = 1000;

private:
	std::thread m_ListenerThread;
	std::thread m_MessageDispatcherThread;
};