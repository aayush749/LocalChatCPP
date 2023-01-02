#pragma once

#include <Networking/Socket.h>
#include <Networking/WCharSocketStream.h>

#include <string>
#include <string_view>
#include <filesystem>
#include <thread>
#include <mutex>


class LCClient
{
	using StreamTp = ntwk::WCharSocketStream;
public:
	LCClient(const std::filesystem::path& configFilePath);
	LCClient(const std::string& serverIP, uint16_t serverPort, const char* userName = nullptr);
	virtual ~LCClient();
	
	void ListenIncomingMsgs();
	
	uint64_t GetHash() const { return m_Hash; }
	StreamTp& GetStream() { return m_Stream; }

private:
	void StartListenerThread()
	{
		// Start listener thread to listen for incoming messages
		m_ShouldStopListening = false;
		m_IncomingMsgListenerThread = std::thread(&LCClient::ListenIncomingMsgs, this);
	}

private:
	std::string m_UsrName;
	uint64_t m_Hash;
	std::mutex m_StrmMtx;
	std::thread m_IncomingMsgListenerThread;
	ntwk::Socket m_Socket;
	StreamTp m_Stream;

	bool m_ShouldStopListening;
};