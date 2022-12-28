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
		std::ifstream configFStrm(CONFIG_FILE_PATH.c_str());
		if (!configFStrm.is_open())
		{
			Logger::logfmt<Log::WARNING>("Config file not found at path, creating new");
			// TODO: Create new config file
			Logger::LogWarning("Quitting for now!");
			std::exit(1);
		}
		else
		{
			inipp::Ini<char> ini;
			ini.parse(configFStrm);
			auto server = ini.sections["LCServerInfo"];
			auto user = ini.sections["UserProfile"];
			Logger::logfmt<Log::INFO>("LCServer Host Address: %s, LCServer Port: %d", server.at("host").c_str(), std::stoi(server.at("port")));
			Logger::logfmt<Log::INFO>("Hash#: %ld, UserName: %s", std::stoull(user.at("hash")), user.at("name").c_str());
		}
	}
	catch (const std::exception& e)
	{
		Logger::logfmt<Log::ERR>("Error processing config file: %s", e.what());
	}
}