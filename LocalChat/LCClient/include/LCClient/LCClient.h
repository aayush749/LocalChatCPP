#pragma once

#include <Networking/Socket.h>
#include <Networking/WCharSocketStream.h>

#include <string>
#include <string_view>
#include <filesystem>


class LCClient
{
	using StreamTp = ntwk::WCharSocketStream;
public:
	LCClient(const std::filesystem::path& configFilePath);
	LCClient(const std::string& serverIP, uint16_t serverPort, const char* userName = nullptr);
	virtual ~LCClient();
	uint64_t GetHash() const { return m_Hash; }
	StreamTp& GetStream() { return m_Stream; }
private:
	std::string m_UsrName;
	uint64_t m_Hash;
	ntwk::Socket m_Socket;
	StreamTp m_Stream;
};