#include <LCServer/LCServer.h>
#include <Logger/Logger.h>

#include <iostream>

constexpr const char* IP = "0.0.0.0";
constexpr const int PORT = 7777;

LCServer GLOBAL_SERVER({}, IP, AF_INET, PORT);

int main()
{
	Logger::init();
	Logger::log("Enter \"quit\" to terminate server!", Log::INFO);
	Logger::logfmt<Log::WARNING>("Listening to incoming clients on port %d...", PORT);

	std::string input;
	while (input != "quit")
	{
		input.clear();
		getline(std::cin, input);
	}
}