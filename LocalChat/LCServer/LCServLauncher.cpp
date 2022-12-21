#include <LCServer/LCServer.h>

#include <iostream>

LCServer GLOBAL_SERVER({}, "0.0.0.0", AF_INET, 7777);

int main()
{
	std::cout << "Enter \"quit\" to terminate server!\n\n";
	std::cout << "Listening to incoming clients on port 7777...\n";
	std::string ip;
	while (ip != "quit")
	{
		ip.clear();
		getline(std::cin, ip);
	}
}