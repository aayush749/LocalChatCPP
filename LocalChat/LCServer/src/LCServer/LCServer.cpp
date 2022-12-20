#include <LCServer/LCServer.h>

LCServer::LCServer(const std::optional<unsigned int> maxClients, const std::string_view ipAddress, const int addressFamily, const uint16_t port)
	:m_MaxClients(maxClients), m_ServerShouldStop(false), m_ServerSock(addressFamily, port, ipAddress)
{}

void LCServer::ListenForClients()
{
	while (!m_ServerShouldStop)
	{
		ntwk::Socket clientSocket = m_ServerSock.Accept();

		s_ClientCtr++;
		uint64_t clientHash = s_BaseClientHash + s_ClientCtr;
		clientSocket.ReceiveBytes((char*)&clientHash, sizeof(clientHash));

		ClientApp app(clientHash, std::move(clientSocket));
		
		AddClient(clientHash, std::move(app));
	}
}

void LCServer::AddClient(ClientHashTp clientHash, ClientApp&& app)
{
	// Check if max number of clients is reached or not
	if (m_MaxClients.has_value() && m_ServerDB.size() == m_MaxClients)
	{
		// Send a "Max Clients Reached" message to client
		app.GetStream() << (int) LCServerError::MAX_CLIENTS_REACHED;

		return;
	}

	if (m_ServerDB.find(clientHash) != m_ServerDB.end())
		throw std::runtime_error("Client already added!");


	m_ServerDB.insert({ clientHash, app });
}

void LCServer::RemoveClient(ClientHashTp clientHash)
{
	auto clientIt = m_ServerDB.find(clientHash);
	if (clientIt == m_ServerDB.end())
		throw std::runtime_error("Client does not exist!");

	m_ServerDB.erase(clientIt);
}

void LCServer::SendMsgToClient(Message& msgRef, ClientHashTp clientHash)
{
	auto clientIt = m_ServerDB.find(clientHash);
	
	if (clientIt == m_ServerDB.end())
		throw std::runtime_error("Can't send message to client, Socket not found.");
	
	std::wstring serializedMsg;
	msgRef.Serialize(serializedMsg);

	ClientApp& app = clientIt->second;

	ntwk::Socket& clientSock = app.GetSocket();

	if (clientSock == INVALID_SOCKET)
		throw std::runtime_error("Can't send message, invalid socket");
	else
	{
		app.GetStream() << serializedMsg;
	}
}

LCServer::~LCServer()
{
	m_ServerShouldStop = true;
}