#include <Networking/ServerSocket.h>

#include "LocalChat.h"

#include <array>
#include <Message/Message.h>

int main()
{
	ntwk::ServerSocket serverSock(AF_INET, 7777, "localhost");
	std::cout << serverSock << std::endl;

	ntwk::Socket client = serverSock.Accept();
	std::cout << "Client Details: " << client << std::endl;
	std::array<char, 400> Buffer = { 0 };
	std::string msg;
	while (client.ReceiveBytes(Buffer.data(), Buffer.size()) > 0)
	{
		if (Buffer[0] == 'c')
			break;
		std::cout << "Terminal: "<< Buffer.data();
		std::cout << "\nYou: ";
		getline(std::cin, msg);
		client.SendBytes(msg);
	}
	client.Close();

	Message mesg("Hello", "*end*");
	std::cout << mesg << std::endl;
}
