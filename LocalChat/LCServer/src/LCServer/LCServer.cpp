#include <LCServer/LCServer.h>
#include <Logger/Logger.h>

#include <functional>

LCServer::LCServer(const std::optional<unsigned int> maxClients, const std::string_view ipAddress, const int addressFamily, const uint16_t port)
	:m_MaxClients(maxClients), m_ServerShouldStop(false), m_ServerSock(addressFamily, port, ipAddress),
	m_ListenerThread(&LCServer::ListenForClients, this)
{
	// Add event listeners
	EventManager::AddListener<EventName::CLIENT_ACTIVE>(std::bind(&LCServer::MessageDispatcher, this, std::placeholders::_1));
	EventManager::AddListener<EventName::MSG_SENT>(std::bind(&LCServer::MsgSentInfoHandler, this, std::placeholders::_1));
}

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

bool LCServer::MessageDispatcher(uint64_t clientHash)
{
	for (auto& [client, app] : m_ServerDB)
	{
		// Client can't be sending messages to itself for now
		if (client == clientHash)
			continue;
		
		for (auto& msg : app->GetPendingMessages())
		{
			if (msg->GetRecipientHash() == clientHash)
			{
				std::wstring serialized;
				msg->Serialize(serialized);
				std::lock_guard<std::mutex> guard(app->GetStrmMutex());
				app->GetStream() << serialized;
			}
		}
		return true;
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

	auto clientIt = m_ServerDB.find(clientHash);
	if (clientIt != m_ServerDB.end())
	{
		// Check if the existing client app has gone inactive
		const auto& existingApp = clientIt->second;
		if (existingApp->IsValidClient() && !existingApp->IsActiveClient())
		{
			// This is a valid client (already registered with the server, ie. its entry exists; that's why we're executing this code)
			// So create a new app, and replace the existing one, but copy over the pending messages from the old app
			ClientAppSPtr newApp = std::make_shared<ClientApp>(
				existingApp->GetHash(),
				std::move(app->GetSocket()),
				existingApp->GetPendingMessages()
				);

			// Update the app stored in the server db
			clientIt->second = newApp;

			Logger::logfmt<Log::WARNING>("Client#%ld logged back on", clientHash);
			return;
		}
		else
			Logger::logfmt<Log::ERR>("Attempt made to add an existing active client, Client#%ld", clientHash);
	}


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

bool LCServer::MsgSentInfoHandler(MsgSPtr message)
{
	uint64_t senderHash = message->GetSenderHash();
	
	try
	{
		// Obtain the client app of the sender
		auto& app = m_ServerDB.at(senderHash);
	
		// Send a "message sent" message back to the sender
		std::wstring buffer = L"SENT|";
		buffer += message->GetGUID();
		buffer += L'\0';
		
		std::lock_guard<std::mutex> guard(app->GetStrmMutex());
		app->GetStream() << buffer;
		return true;
	}
	catch (const std::out_of_range& e)
	{
		Logger::logfmt<Log::ERR, 2048>("Could not send \"Msg SENT\" receipt to %d. Adding to pending message list: Currently not implemented Control Messages", senderHash);		
		return true;
	}
	catch (const std::exception& e)
	{
		return false;
	}
	return false;
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
}