#include <LCClient/LCClient.h>

#include <utils/Conversion.h>
#include <utils/SerializationUtils.h>

#include <Logger/Logger.h>

#include <inipp/inipp.h>

#include <fstream>

LCClient::LCClient(const std::string& serverIP, uint16_t serverPort, const char* userName)
	:m_Socket("localhost", 0, AF_INET, SOCK_STREAM), m_Stream(m_Socket), m_UsrName(userName), m_Hash(0)
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
}

LCClient::LCClient(const std::filesystem::path& configFilePath)
	:m_Socket("localhost", 0, AF_INET, SOCK_STREAM), m_Stream(m_Socket), m_Hash(0)
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
		}
	}
}

LCClient::~LCClient()
{
	m_Stream << L"close\0";
	m_Socket.Close();
}