#include <LCClient/LCClient.h>

#include <utils/Conversion.h>
#include <utils/SerializationUtils.h>
#include <Message/ControlMessage.h>

#include <Logger/Logger.h>

#include <inipp/inipp.h>

#include <fstream>

LCClient::LCClient(const std::string& serverIP, uint16_t serverPort, const char* userName)
	:m_Socket("localhost", 0, AF_INET, SOCK_STREAM), m_Stream(m_Socket), m_UsrName(userName), m_Hash(0)
	,m_ShouldStopListening(true) // don't listen immediately for incoming messages
	,m_IncomingMsgListenerThread()
{
	m_Socket.Bind();
	m_Socket.Connect(serverIP, serverPort, AF_INET);

	m_Socket.SetNoDelay(true);

	// Tell the server to start
	m_Stream << L"start\0";
	
	// Get the client hash
	std::wstring buff;
	getline(m_Stream, buff, L'\0');
	
	m_Hash = cnvrt::To<uint64_t>(buff);

	StartListenerThread();
}

LCClient::LCClient(const std::filesystem::path& configFilePath)
	:m_Socket("localhost", 0, AF_INET, SOCK_STREAM), m_Stream(m_Socket), m_Hash(0)
	,m_ShouldStopListening(true) // don't listen immediately for incoming messages
	,m_IncomingMsgListenerThread()
{
	std::ifstream configFStrm(configFilePath.c_str());
	
	if (configFStrm.is_open())
	{

		inipp::Ini<char> parsedIni;
		parsedIni.parse(configFStrm);

		auto& server = parsedIni.sections.at("LCServerInfo");

		const std::string_view serverIP = server.at("host");
		const uint16_t serverPort = std::stoi(server.at("port"));

		m_Socket.Bind();
		int res = m_Socket.Connect(serverIP, serverPort, AF_INET);

		if (res == SOCKET_ERROR)
			std::exit(1);

		Logger::logfmt<Log::INFO>("Successfully connected to server at : %s:%d", serverIP.data(), serverPort);

		m_Socket.SetNoDelay(true);

		try
		{
			auto& user = parsedIni.sections.at("UserProfile");
			m_Hash = std::stoull(user.at("hash"));
			m_UsrName = user.at("name");

			bool userActuallyExists = false;

			std::wstring hashStr = Serialize<std::wstring>(m_Hash);
			hashStr += L'\0';
			m_Stream << hashStr;

			std::wstring response;
			getline(m_Stream, response, L'\0');

			userActuallyExists = (response == L"OK");

			if (userActuallyExists)
			{
				// Show a welcome message to the user
				Logger::logfmt<Log::INFO>("Welcome %s", m_UsrName.c_str());
				

				StartListenerThread();
			}
			else
			{
				// Show error
				Logger::logfmt<Log::ERR>("Could not verify user :-(");
				Logger::LogWarning("Closing...");
				
				m_Socket.Close();
				std::exit(1);
			}
		}
		catch (std::out_of_range& e)
		{
			// Existing user profile not found
			Logger::LogInfo("Existing user profile not found, creating new");
	
			m_Stream << L"start\0";

			// Get the client hash
			m_Hash = 0;

			std::wstring buff;
			getline(m_Stream, buff, L'\0');

			m_Hash = cnvrt::To<uint64_t>(buff);

			// Modify the ini file accordingly
			std::ofstream outConfigFStrm(configFilePath.c_str(), std::ios::app);
			
			std::ostringstream bufferToWrite;

			
			std::string temp = "";
			bufferToWrite << "\n"
					        << "[UserProfile]\n"
							<< "hash = " << m_Hash << "\n";
			
			std::string name;
			std::cout << "Enter your user name: ";
			getline(std::cin, name);

			bufferToWrite << "name = " << name << "\n";
			
			outConfigFStrm << bufferToWrite.str();

			StartListenerThread();
		}
	}
}

LCClient::~LCClient()
{
	m_Stream << L"close\0";
	m_ShouldStopListening = true;
	m_Socket.Close();

	if (m_IncomingMsgListenerThread.joinable())
		m_IncomingMsgListenerThread.join();
}

void LCClient::ListenIncomingMsgs()
{
	while (!m_ShouldStopListening)
	{
		std::wstring buffer;
		if (m_Socket == INVALID_SOCKET)
			return;
		getline(m_Stream, buffer, L'\0');
		
		if (buffer._Starts_with(L"CTRL|"))
		{
			// It is a control message
			ControlMessage cm = ControlMessage::DeSerialize(buffer);
			const auto&guid = cm.GetGUID();
			auto type = cm.GetType();
			if (type == ControlMessageType::MSG_SENT)
			{
				Logger::logfmt<Log::INFO>("Message with id: %s is sent", guid.str().c_str());
			}
		}

		Logger::logfmt<Log::INFO>("Received bytes: %s", std::string(buffer.begin(), buffer.end()).c_str());
	}
}