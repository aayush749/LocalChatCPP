#include "Networking/ServerSocket.h"

namespace ntwk {
	ServerSocket::ServerSocket(int addressFamily, uint16_t port, std::string_view ipAddress)
		:m_Socket(ipAddress, port, addressFamily, SOCK_STREAM)
	{
		m_Socket.Create();
		m_Socket.Listen();
	}	

	Socket ServerSocket::Accept()
	{
		SOCKADDR clientInfo = { 0 };
		int size = sizeof(clientInfo);
		Socket clientSocket = accept(m_Socket.GetSocket(), 0, 0);

		return clientSocket;
	}

	ServerSocket::~ServerSocket()
	{
		m_Socket.Close();
	}
}

std::ostream& ntwk::operator<<(std::ostream& os, const ntwk::ServerSocket& ss)
{
	os << ss.m_Socket;
	return os;
}
