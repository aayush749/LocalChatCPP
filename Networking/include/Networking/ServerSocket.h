#pragma once

#include <Networking/Networking.h>
#include <Networking/Socket.h>

#include <iostream>
#include <string_view>

namespace ntwk {
	class NTWK_EXPORT ServerSocket
	{
	public:
		ServerSocket(int addressFamily, uint16_t port, std::string_view ipAddress = "localhost");
		
		Socket Accept();
		NTWK_EXPORT friend std::ostream& operator<<(std::ostream&, const ServerSocket&);
	
		~ServerSocket();
	private:
		Socket m_Socket;
	};
}