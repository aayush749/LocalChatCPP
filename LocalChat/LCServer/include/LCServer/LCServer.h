#include <Networking/Networking.h>
#include <Networking/ServerSocket.h>
#include <Networking/Socket.h>
#include <Message/Message.h>

#include <optional>
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
	using ServerDB = std::unordered_map<ClientHashTp, std::pair<IPAddressTp, std::forward_list<MessageSPtr>>>;

public:
	LCServer(const std::optional<unsigned int> maxClients, const std::string_view ipAddress = "localhost", const int addressFamily = AF_INET, const uint16_t port = 7777);

	// Makes the server listen to incoming client connections
	void ListenForClients();


	void RemoveClient(ClientHashTp clientHash);
private:
	void SendMsgToClient(Message& msgRef, IPAddressTp ipAddress);
	void AddPendingMessageForClient();
	void AddClient(ClientHashTp clientHash, ntwk::Socket&& clientSocket);
private:
	ntwk::ServerSocket m_ServerSock;
	ServerDB m_ServerDB;
	std::unordered_map<IPAddressTp, ntwk::Socket&> m_IP2SockMap;
	std::optional<unsigned int> m_MaxClients;
	bool m_ServerShouldStop;
};