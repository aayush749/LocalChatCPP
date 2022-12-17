#pragma once

#include <Networking/Networking.h>
#include <Networking/Socket.h>

#include <iostream>
#include <string_view>

namespace ntwk {
	class ServerSocket
	{
	public:
		NTWK_EXPORT ServerSocket(int addressFamily, uint16_t port, std::string_view ipAddress = "localhost");
		
		NTWK_EXPORT Socket Accept();
		NTWK_EXPORT friend std::ostream& operator<<(std::ostream&, const ServerSocket&);
	
		NTWK_EXPORT ~ServerSocket();
	private:
		Socket m_Socket;
	};
}