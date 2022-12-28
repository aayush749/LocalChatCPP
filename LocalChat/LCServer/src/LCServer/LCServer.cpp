#include <LCServer/LCServer.h>
#include <Logger/Logger.h>

LCServer::LCServer(const std::optional<unsigned int> maxClients, const std::string_view ipAddress, const int addressFamily, const uint16_t port)
	:m_MaxClients(maxClients), m_ServerShouldStop(false), m_ServerSock(addressFamily, port, ipAddress),
	m_ListenerThread(&LCServer::ListenForClients, this), m_MessageDispatcherThread(&LCServer::MessageDispatcher, this)
{}

void LCServer::ListenForClients()
{
	while (!m_ServerShouldStop)
	{
		ntwk::Socket clientSocket = m_ServerSock.Accept();
		clientSocket.SetNoDelay(true);
		Logger::logfmt<Log::INFO>("Connected to client: %s", clientSocket.ToString().c_str());
		s_ClientCtr++;
		uint64_t clientHash = s_BaseClientHash + s_ClientCtr;

		ClientAppSPtr appSPtr = std::make_shared<ClientApp>(clientHash, std::move(clientSocket));

		AddClient(clientHash, appSPtr);
		Logger::logfmt<Log::INFO>("Added client with Hash: %ld", clientHash);
	}
}

ntwk::Socket& LCServer::GetClientSockFromClientHash(uint64_t clientHash)
{
	return m_ServerDB.at(clientHash)->GetSocket();
}

void LCServer::MessageDispatcher()
{
	while (!m_ServerShouldStop)
	{
		for (const auto& [clientHash, app] : m_ServerDB)
		{
			auto& pendingMsgs = app->GetPendingMessages();
			for (auto it = pendingMsgs.begin();
				it != pendingMsgs.end(); it++)
			{
				auto& msg = *it;
				uint64_t recipient = msg->GetRecipientHash();
				try
				{
					SendMsgToClient(*msg, recipient);
				
					// Message sent, now remove the message from the pending message list
					app->RemoveMessage(it);
				}
				catch (const std::runtime_error& e)
				{
					Logger::logfmt<Log::ERR>("Error sending message to Client%ld : %s", recipient, e.what());
				}
			}
		}
	}
}

void LCServer::AddClient(ClientHashTp clientHash, ClientAppSPtr app)
{
	// Check if max number of clients is reached or not
	if (m_MaxClients.has_value() && m_ServerDB.size() == m_MaxClients)
	{
		// Send a "Max Clients Reached" message to client
		app->GetStream() << (int) LCServerError::MAX_CLIENTS_REACHED;

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

	ClientApp& app = *(clientIt->second);

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

	if (m_ListenerThread.joinable())
		m_ListenerThread.join();

	if (m_MessageDispatcherThread.joinable())
		m_MessageDispatcherThread.join();
}