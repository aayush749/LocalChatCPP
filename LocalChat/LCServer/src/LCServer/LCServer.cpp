#include <LCServer/LCServer.h>

LCServer::LCServer(const std::optional<unsigned int> maxClients, const std::string_view ipAddress, const int addressFamily, const uint16_t port)
	:m_MaxClients(maxClients), m_ServerShouldStop(false), m_ServerSock(addressFamily, port, ipAddress)
{}

void LCServer::ListenForClients()
{
	while (!m_ServerShouldStop)
	{
		ntwk::Socket clientSocket = m_ServerSock.Accept();

		// Receive the client Hash Number
		std::string tempBuf(sizeof(uint64_t) + 1, 0);
		clientSocket.ReceiveBytes(tempBuf.data(), sizeof(uint64_t));

		uint64_t clientHash = std::stoull(tempBuf);

		AddClient(clientHash, std::move(clientSocket));
	}
}

void LCServer::AddClient(ClientHashTp clientHash, ntwk::Socket&& clientSocket)
{
	if (m_ServerDB.size() == m_MaxClients)
	{
		// Send a "Max Clients Reached" message to client
		
		std::string_view busyMessage(std::to_string((int)LCServerError::MAX_CLIENTS_REACHED));
		clientSocket.SendBytes(busyMessage);

		return;
	}

	if (m_ServerDB.find(clientHash) != m_ServerDB.end())
		throw std::runtime_error("Client already added!");


	IPAddressTp ipAddr = clientSocket.GetIPAddress();

	m_ServerDB.insert(
			{
				clientHash,
				{
					ipAddr, {} //empty list of messages
				}
	   		}
	);

	m_IP2SockMap.insert({ clientSocket.GetIPAddress(), clientSocket });
}

void LCServer::RemoveClient(ClientHashTp clientHash)
{
	auto clientIt = m_ServerDB.find(clientHash);
	if (clientIt == m_ServerDB.end())
		throw std::runtime_error("Client does not exist!");

	IPAddressTp ipAddr = clientIt->second.first;
	auto clientIP2SockMapIt = m_IP2SockMap.find(ipAddr);
	
	m_ServerDB.erase(clientIt);
	m_IP2SockMap.erase(clientIP2SockMapIt);
}

void LCServer::SendMsgToClient(Message& msgRef, IPAddressTp ipAddress)
{
	auto clientIt = m_IP2SockMap.find(ipAddress);
	
	if (clientIt == m_IP2SockMap.end())
		throw std::runtime_error("Can't send message to client, Socket not found.");
	
	std::wstring serializedMsg;
	msgRef.Serialize(serializedMsg);

	ntwk::Socket& clientSock = clientIt->second;
	if (clientSock == INVALID_SOCKET)
		throw std::runtime_error("Can't send message, invalid socket");
	else
	{
		int result = clientSock.SendWideBytes(serializedMsg);
		if (result == SOCKET_ERROR)
		{
			auto ec = WSAGetLastError();
			char MSG[1024] = "Could not send message to client: %d";
			snprintf(MSG, sizeof(MSG), MSG, ec);
			throw std::runtime_error(MSG);
		}
	}
}
