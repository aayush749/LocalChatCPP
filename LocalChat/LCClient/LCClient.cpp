#include <Networking/Networking.h>
#include <Networking/CharSocketStream.h>
#include <Networking/ServerSocket.h>

#include <iostream>

int main()
{
	std::cout << "Hello from the LCClient\n";
	try
	{
		/*ntwk::Socket sockt("localhost", 7778, AF_INET, SOCK_STREAM);

		sockt.Bind();
		sockt.Connect("localhost", 7777, AF_INET);

		ntwk::CharSocketStream css(sockt);

		std::string input;
		while (input != "quit")
		{
			input.clear();
			getline(css, input, (char)12);
			input = input.substr(2);
			std::cout << input << std::endl;
		}*/
		ntwk::ServerSocket ss(AF_INET, 7777);

		std::cout << "Accepting incoming connections...\n";
		ntwk::Socket client = ss.Accept();

		ntwk::CharSocketStream css(client);

		std::string ip = "";

		do
		{
			if (ip == "quit")
				continue;
			std::cout << "Enter your message: ";
			getline(std::cin, ip);
			
			// Write to network
			css << ip << std::endl;
		} while (ip != "quit");
	}
	catch (const std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
}