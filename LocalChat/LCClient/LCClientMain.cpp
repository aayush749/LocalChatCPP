#include <Networking/Networking.h>
#include <Networking/WCharSocketStream.h>
#include <Networking/ServerSocket.h>

#include <LCClient/LCCLient.h>

#include <iostream>

const std::string HOST = "192.168.1.101";
const uint16_t PORT = 7777;

int main()
{
	std::wcout << L"Hello from the LCClient\n";
	//try
	//{
	//	/*ntwk::Socket sockt("localhost", 7778, AF_INET, SOCK_STREAM);

	//	sockt.Bind();
	//	sockt.Connect("127.0.0.1", 7777, AF_INET);

	//	ntwk::WCharSocketStream css(sockt);

	//	std::wstring input;
	//	while (input != L"quit")
	//	{
	//		input.clear();
	//		getline(css, input, (wchar_t)12);
	//		std::wcout << input << std::endl;
	//	}*/
	//	ntwk::ServerSocket ss(AF_INET, 7777, "0.0.0.0");

	//	std::cout << "Accepting incoming connections...\n";
	//	ntwk::Socket client = ss.Accept();

	//	ntwk::WCharSocketStream css(client);

	//	std::wstring ip = L"";

	//	do
	//	{
	//		if (ip == L"quit")
	//			continue;
	//		std::wcout << L"Enter your message: ";
	//		getline(std::wcin, ip);
	//		
	//		// Write to network
	//		css << ip << std::endl;
	//	} while (ip != L"quit");
	//}
	//catch (const std::exception& e)
	//{
	//	std::cout << "Error: " << e.what() << std::endl;
	//}

	LCClient client(HOST, PORT);
	
	ntwk::WCharSocketStream& stream = client.GetStream();

	std::wstring message = L"";

	while (message != L"quit")
	{
		std::cout << "Enter your message Client" << client.GetHash() << " : ";
		getline(std::wcin, message);
		stream << message + wchar_t(12);
	}
}