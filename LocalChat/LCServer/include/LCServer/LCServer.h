#pragma once

#include <Networking/Networking.h>
#include <Networking/ServerSocket.h>
#include <Networking/Socket.h>
#include <Message/Message.h>
#include <Networking/WCharSocketStream.h>

#include <Events/Event.h>
#include <LCServer/ClientApp.h>
#include <Message/ControlMessage.h>

#include <optional>
#include <thread>
#include <mutex>
#include <queue>
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
	using PendingControlMsgTp = std::pair<ClientHashTp, ControlMessage>;

public:
	LCServer(const std::optional<unsigned int> maxClients, const std::string_view ipAddress = "0.0.0.0", const int addressFamily = AF_INET, const uint16_t port = 7777);

	// Makes the server listen to incoming client connections
	void ListenForClients();

	ntwk::Socket& GetClientSockFromClientHash(uint64_t clientHash);
	ClientAppSPtr GetClientAppFromHash(ClientHashTp clientHash) { return m_ServerDB.at(clientHash); }

	const bool ClientExists(uint64_t clientHash) { std::lock_guard<std::mutex> guard(m_ServerDBMutex);  return m_ServerDB.find(clientHash) != m_ServerDB.end(); }

	// Process incoming messages and act according to it
	bool MessageDispatcher(uint64_t clientHash);
	void RemoveClient(ClientHashTp clientHash);
	bool MsgSentInfoHandler(MsgSPtr message);
	bool MsgDeliveredInfoHandler(MsgSPtr message);

	static uint64_t GetNewClientHash();

	virtual ~LCServer();
private:
	void SendMsgToClient(Message& msgRef, ServerDB::const_iterator clientIt);
	void AddClient(ClientHashTp clientHash, ClientAppSPtr app);
	void CreateThenAddNewClient(ntwk::Socket&& clientSocket);
private:
	ntwk::ServerSocket m_ServerSock;
	
	std::mutex m_ServerDBMutex;
	ServerDB m_ServerDB;
	
	std::optional<unsigned int> m_MaxClients;
	bool m_ServerShouldStop;

	std::mutex m_ServerControlMsgQMtx;
	std::queue<PendingControlMsgTp> m_PendingControlMessagesQueue;


	inline static std::mutex s_clientCountMutex;
	inline static ClientHashTp s_ClientCtr = 0;
	inline static ClientHashTp s_BaseClientHash = 1000;

private:
	std::vector<std::future<void>> m_ClientCreationFuturesVec;
private:
	std::thread m_ListenerThread;
};