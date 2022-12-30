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
	
		m_ClientCreationFuturesVec.emplace_back(std::async(std::launch::async, &LCServer::CreateThenAddNewClient, this, std::move(clientSocket)));
	}
}

void LCServer::CreateThenAddNewClient(ntwk::Socket&& clientSocket)
{
	ClientAppSPtr appSPtr = std::make_shared<ClientApp>(std::move(clientSocket));

	if (appSPtr->IsValidClient())
	{
		uint64_t clientHash = appSPtr->GetHash();
		// Proceed to add the valid client
		try
		{
			AddClient(clientHash, appSPtr);
			Logger::logfmt<Log::INFO>("Added client with Hash: %ld", clientHash);
		}
		catch (const std::runtime_error& e)
		{
			Logger::logfmt<Log::ERR>("Error while adding client: %s", e.what());
		}
	}
}

ntwk::Socket& LCServer::GetClientSockFromClientHash(uint64_t clientHash)
{
	std::lock_guard<std::mutex> guard(m_ServerDBMutex);
	return m_ServerDB.at(clientHash)->GetSocket();
}

void LCServer::MessageDispatcher()
{
	std::vector<std::future<void>> sendMsgTaskFutures;

	while (!m_ServerShouldStop)
	{
		std::lock_guard<std::mutex> guard(m_ServerDBMutex);
		for (const auto& [clientHash, app] : m_ServerDB)
		{
			auto& pendingMsgs = app->GetPendingMessages();
			if (!app->IsValidClient() || pendingMsgs.size() == 0)
				continue;
			for (auto it = pendingMsgs.begin();
				it != pendingMsgs.end(); it++)
			{
				auto& msg = *it;
				uint64_t recipient = msg->GetRecipientHash();
				try
				{
					ServerDB::const_iterator clientIt = m_ServerDB.find(recipient);
					if (clientIt != m_ServerDB.end())
					{
						sendMsgTaskFutures.emplace_back(
							std::async(std::launch::async, 
								&LCServer::SendMsgToClient, 
								this, std::ref(*msg), clientIt)
						);
				
						// Message sent, now remove the message from the pending message list
						app->RemoveMessage(it);
					}
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
	std::lock_guard<std::mutex> guard(m_ServerDBMutex);

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
	std::lock_guard<std::mutex> guard(m_ServerDBMutex);

	auto clientIt = m_ServerDB.find(clientHash);
	if (clientIt == m_ServerDB.end())
		throw std::runtime_error("Client does not exist!");

	m_ServerDB.erase(clientIt);
}

uint64_t LCServer::GetNewClientHash()
{
	std::lock_guard<std::mutex> guard(s_clientCountMutex);
	s_ClientCtr++;
	return s_BaseClientHash + s_ClientCtr;
}

void LCServer::SendMsgToClient(Message& msgRef, ServerDB::const_iterator clientIt)
{
	std::wstring serializedMsg;
	msgRef.Serialize(serializedMsg);

	ClientApp& app = *(clientIt->second);

	ntwk::Socket& clientSock = app.GetSocket();

	if (clientSock == INVALID_SOCKET)
		throw std::runtime_error("Can't send message, invalid socket");
	else
	{
		std::lock_guard<std::mutex> guard(app.GetStrmMutex());
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