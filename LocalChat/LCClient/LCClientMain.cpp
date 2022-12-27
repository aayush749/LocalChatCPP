#include <Logger/Logger.h>
#include <Networking/Networking.h>
#include <Networking/WCharSocketStream.h>
#include <Networking/ServerSocket.h>

#include <Message/TextMessage.h>

#include <LCClient/LCCLient.h>

#include <iostream>

const std::string HOST = "127.0.0.1";
const uint16_t PORT = 7777;

int main()
{
	Logger::logfmt<Log::INFO>("Hello from the LCClient\n");
	
	try
	{
		Logger::logfmt<Log::WARNING>("Trying to connect to server at: %s:%d", HOST.c_str(), PORT);
		
		LCClient client(HOST, PORT);
	
		ntwk::WCharSocketStream& stream = client.GetStream();

		std::wstring message = L"";

		while (message != L"quit")
		{
			std::cout << "Enter your message Client" << client.GetHash() << " : ";
			getline(std::wcin, message);
			TextMessage tm(2, message);
			std::wstring serializedMsg = L"";
			tm.Serialize(serializedMsg);
			stream << serializedMsg;
		}
	}
	catch (const std::runtime_error& e)
	{
		Logger::logfmt<Log::ERR>("Could not connect to Local Chat Server : %s", e.what());
	}
}