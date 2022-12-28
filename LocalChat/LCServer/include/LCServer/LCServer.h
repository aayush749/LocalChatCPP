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
#include <vector>
#include <future>
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
	using ClientAppSPtr = std::shared_ptr<ClientApp>;
	using ServerDB = std::unordered_map<ClientHashTp, ClientAppSPtr>;

public:
	LCServer(const std::optional<unsigned int> maxClients, const std::string_view ipAddress = "0.0.0.0", const int addressFamily = AF_INET, const uint16_t port = 7777);

	// Makes the server listen to incoming client connections
	void ListenForClients();

	ntwk::Socket& GetClientSockFromClientHash(uint64_t clientHash);

	// Process incoming messages and act according to it
	void MessageDispatcher();
	void RemoveClient(ClientHashTp clientHash);

	virtual ~LCServer();
private:
	void SendMsgToClient(Message& msgRef, ClientHashTp clientHash);
	void AddClient(ClientHashTp clientHash, ClientAppSPtr app);
	void CreateThenAddNewClient(uint64_t clientHash, ntwk::Socket&& clientSocket);
private:
	ntwk::ServerSocket m_ServerSock;
	ServerDB m_ServerDB;
	std::optional<unsigned int> m_MaxClients;
	bool m_ServerShouldStop;

	inline static ClientHashTp s_ClientCtr = 0;
	inline static ClientHashTp s_BaseClientHash = 1000;

private:
	std::vector<std::future<void>> m_ClientCreationFuturesVec;
private:
	std::thread m_ListenerThread;
	std::thread m_MessageDispatcherThread;
};