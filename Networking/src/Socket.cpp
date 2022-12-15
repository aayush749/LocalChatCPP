#include <Networking/Socket.h>

namespace ntwk {
	Socket::Socket(std::string_view ip, uint16_t port, int addressFamily, int type)
		:m_IPAddress(ip), m_Port(port), m_AddressFamily(addressFamily), m_Type(type),
		m_PortStr(std::to_string(port)), m_NativeSocket(INVALID_SOCKET),
		m_IsOpen(false), m_IsListening(false)
	{
	}


	Socket::Socket(SOCKET nativeSocket)
		:m_NativeSocket(nativeSocket)
	{
		// Fill the IP addr, Address Family and Port
		FillPortAddrFamAndIP();

		int iResult = -1;
		
		// Check if its listening
		IsListening();

		m_IsOpen = m_NativeSocket != INVALID_SOCKET;

		int m_TypeSz = sizeof(m_TypeSz);
		iResult = getsockopt(m_NativeSocket, SOL_SOCKET,
			SO_TYPE, (char*)&m_Type, &m_TypeSz);

		if (iResult == SOCKET_ERROR)
		{
			char MSG[1024] = "Error encountered while creating Socket object from native socket: %ld\n";
			snprintf(MSG, sizeof(MSG), MSG, WSAGetLastError());
			throw std::runtime_error(MSG);
		}
	}

	Socket::Socket(Socket&& other) noexcept
		:m_AddressFamily(other.m_AddressFamily),
		m_Type(other.m_Type),
		m_IPAddress(std::move(other.m_IPAddress)),
		m_Port(other.m_Port), m_PortStr(std::move(other.m_PortStr)),
		m_IsOpen(other.m_IsOpen),
		m_IsListening(other.m_IsListening),
		m_NativeSocket(other.m_NativeSocket)
	{
		//move constructor
		other.m_AddressFamily = -1;
		other.m_Type = -1;
		other.m_IPAddress.clear();
		other.m_Port = 0;
		other.m_PortStr.clear();
		other.m_IsOpen = false;
		other.m_IsListening = false;
		other.m_NativeSocket = INVALID_SOCKET;
	}
	
	bool Socket::IsListening() const
	{
		int iResult = -1;
		BOOL isListening;
		int isListeningSz = sizeof(isListening);
		iResult = getsockopt(m_NativeSocket, SOL_SOCKET,
			SO_ACCEPTCONN, (char*)&isListening, &isListeningSz);
		if (iResult == SOCKET_ERROR)
		{
			char MSG[1024] = "Error encountered while creating Socket object from native socket: %ld\n";
			snprintf(MSG, sizeof(MSG), MSG, WSAGetLastError());
			throw std::runtime_error(MSG);
		}

		m_IsListening = (bool)isListening;
	}

	void Socket::Create()
	{
		if (m_NativeSocket != INVALID_SOCKET)
			return;

		int iResult = -1;
		addrinfo* result = nullptr, * ptr = nullptr, hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = m_AddressFamily;
		hints.ai_flags = AI_PASSIVE;
		hints.ai_socktype = m_Type;

		if ((m_AddressFamily == AF_INET || m_AddressFamily == AF_INET6) && m_Type == SOCK_STREAM)
			hints.ai_protocol = IPPROTO_TCP;

		else if ((m_AddressFamily == AF_INET || m_AddressFamily == AF_INET6) && m_Type == SOCK_DGRAM)
			hints.ai_protocol = IPPROTO_UDP;
		else if (m_AddressFamily == AF_INET && m_Type == SOCK_RDM)
			hints.ai_protocol = IPPROTO_PGM;

		else
			throw std::runtime_error("Could not determine transmission protocol");

		iResult = getaddrinfo(NULL, GetPortStr(), &hints, &result);
		if (iResult != 0)
		{
			char MSG[64] = "getaddrinfo failed: %d\n";
			snprintf(MSG, sizeof(MSG), MSG, iResult);
			throw std::runtime_error(MSG);
		}

		m_NativeSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (m_NativeSocket== INVALID_SOCKET)
		{
			char MSG[1024] = "Error at socket(): %ld\n";
			snprintf(MSG, sizeof(MSG), MSG, WSAGetLastError());
			freeaddrinfo(result);
			throw std::runtime_error(MSG);
		}

		// Bind
		iResult = SOCKET_ERROR;
		iResult = bind(m_NativeSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			char MSG[1024] = "bind failed with error: %d\n";
			snprintf(MSG, sizeof(MSG), MSG, WSAGetLastError());
			freeaddrinfo(result);
			throw std::runtime_error(MSG);
		}

		m_IsOpen = true;
	}

	void Socket::Listen()
	{
		if (m_IsListening)
			return;

		if (listen(m_NativeSocket, SOMAXCONN) == SOCKET_ERROR) 
		{
			char MSG[1024] = "Listen failed with error: %ld\n";
			snprintf(MSG, sizeof(MSG), MSG, WSAGetLastError());
			throw std::runtime_error(MSG);
		}
		m_IsListening = true;
	}

	void Socket::Close()
	{
		if (m_NativeSocket != INVALID_SOCKET)
		{
			closesocket(m_NativeSocket);
			m_NativeSocket = INVALID_SOCKET;
			
			m_AddressFamily = -1;
			m_Type = -1;
			m_IsListening = false;
			m_IsOpen = false;
			m_IPAddress = "";
			m_Port = -1;
			m_PortStr.clear();
		}
	}

	Socket::~Socket()
	{
		Close();
	}

	void Socket::FillPortAddrFamAndIP()
	{
		int iResult = -1;
		SOCKADDR socketAddress = { 0 };
		int sockAddrSz = sizeof(socketAddress);
		iResult = getpeername(m_NativeSocket, &socketAddress, &sockAddrSz);

		if (iResult == SOCKET_ERROR)
		{
			char MSG[1024] = "Error encountered while creating Socket object from native socket: %ld\n";
			snprintf(MSG, sizeof(MSG), MSG, WSAGetLastError());
			throw std::runtime_error(MSG);
		}
		
		m_AddressFamily = socketAddress.sa_family;

		switch (socketAddress.sa_family)
		{
		case AF_INET:
			{
				SOCKADDR_IN* addr = (SOCKADDR_IN*) &socketAddress;
				// Also fill the port detail
				m_Port = addr->sin_port;
				m_PortStr = std::to_string(m_Port);

				m_IPAddress = inet_ntoa(addr->sin_addr);
				break;
			}
		case AF_INET6:
			{
				m_IPAddress.resize(INET6_ADDRSTRLEN + 1);
				SOCKADDR_IN6* addr = (SOCKADDR_IN6*)&socketAddress;
				// Also fill the port detail
				m_Port = addr->sin6_port;
				m_PortStr = std::to_string(m_Port);

				inet_ntop(AF_INET6, &(addr->sin6_addr), m_IPAddress.data(), INET6_ADDRSTRLEN);
				break;
			}
		default:
			break;
		}
	}
}

std::ostream& ntwk::operator<<(std::ostream& os, const ntwk::Socket& socket)
{
	os << "IP: "<<socket.m_IPAddress<<", PORT: "<<socket.m_Port;
	return os;
}