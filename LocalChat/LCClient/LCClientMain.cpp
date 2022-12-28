#include <Logger/Logger.h>
#include <Networking/Networking.h>
#include <Networking/WCharSocketStream.h>
#include <Networking/ServerSocket.h>

#include <Message/TextMessage.h>

#include <LCClient/LCCLient.h>

#include <inipp/inipp.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

const std::string HOST = "127.0.0.1";
const uint16_t PORT = 7777;

std::filesystem::path CONFIG_FILE_PATH = std::filesystem::path(getenv("USERPROFILE")).concat("/Documents/LocalChatCpp/config/config.ini");

int main()
{
	try
	{
		LCClient client(CONFIG_FILE_PATH);
	}
	catch (const std::exception& e)
	{
		Logger::logfmt<Log::ERR>("Error processing config file: %s", e.what());
	}
}