#pragma once

#include <Networking/Socket.h>
#include <Networking/WCharSocketStream.h>

#include <string>
#include <string_view>

class LCClient
{
	using StreamTp = ntwk::WCharSocketStream;
public:
	LCClient(const std::string& serverIP, uint16_t serverPort);

	uint64_t GetHash() const { return m_Hash; }
	StreamTp& GetStream() { return m_Stream; }
private:
	uint64_t m_Hash;
	ntwk::Socket m_Socket;
	StreamTp m_Stream;
};