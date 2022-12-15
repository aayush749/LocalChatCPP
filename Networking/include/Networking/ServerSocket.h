#pragma once

#include <Networking/Networking.h>
#include <Networking/Socket.h>

#include <iostream>
#include <string_view>

namespace ntwk {

	WSADATA wsaData;
	bool isWSAInit = false;
	
	NTWK_EXPORT void InitWSA()
	{
		// Initialize Winsock
		if (isWSAInit)
			return;

		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) 
		{
			char MSG[512] = "WSAStartup failed: %d\n";
			snprintf(MSG, sizeof(MSG), MSG, iResult);
			throw std::runtime_error(MSG);
		}
		isWSAInit = true;
	}

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