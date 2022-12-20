#include <Networking/Socket.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Web.Syndication.h>>

#include <sstream>

namespace ntwk {
	Socket::Socket(std::string_view ip, uint16_t port, int addressFamily, int type)
		:m_IPAddress(ip), m_Port(port), m_AddressFamily(addressFamily), m_Type(type),
		m_PortStr(std::to_string(port)), m_NativeSocket(INVALID_SOCKET),
		m_IsOpen(false), m_IsListening(false), m_SocketAddress(nullptr), m_SocketAddressLength(0)
	{
		Create();
	}


	Socket::Socket(SOCKET nativeSocket)
		:m_NativeSocket(nativeSocket), m_SocketAddress(nullptr), m_SocketAddressLength(0)
	{
		// Check if the native socket provided is a valid socket
		if (nativeSocket == INVALID_SOCKET)
		{
			char MSG[1024] = "accept failed: %d\n";
			int ec = WSAGetLastError();
			snprintf(MSG, sizeof(MSG), MSG, ec);
			throw std::runtime_error(MSG);
		}

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
		m_NativeSocket(other.m_NativeSocket),
		m_SocketAddress(other.m_SocketAddress),
		m_SocketAddressLength(other.m_SocketAddressLength)
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
	
	NTWK_EXPORT int Socket::SendNBytes(const char* str, int n)
	{
		int iResult = -1;
		iResult = send(m_NativeSocket, str, n, 0);
		if (iResult == SOCKET_ERROR)
		{
			char MSG[512] = "Could not send message! Error: %ld\n";
			snprintf(MSG, sizeof(MSG), MSG, WSAGetLastError());
			throw std::runtime_error(MSG);
		}
		return iResult;
	}

	int Socket::SendBytes(const std::string_view message)
	{
		return SendNBytes(message.data(), message.size());
	}

	int Socket::SendWideBytes(const std::wstring_view message)
	{
		// Convert wide chars to chars
		
		const wchar_t* wbuffer = message.data();
		// determine the required buffer size

		size_t buffer_size;
		wcstombs_s(&buffer_size, NULL, 0, wbuffer, _TRUNCATE);

		// do the actual conversion
		std::unique_ptr<char[]> buffer = std::make_unique<char[]>(buffer_size);
		
		wcstombs_s(&buffer_size, buffer.get(), buffer_size, wbuffer, _TRUNCATE);

		// send the data using normal char version
		return SendBytes(buffer.get());
	}
	
	int Socket::SendNWideBytes(const wchar_t* message, int n)
	{
		// Convert wide chars to chars
		
		const wchar_t* wbuffer = message;
		// determine the required buffer size

		size_t buffer_size = n;
		wcstombs_s(&buffer_size, NULL, 0, wbuffer, _TRUNCATE);

		// do the actual conversion
		std::unique_ptr<char[]> buffer = std::make_unique<char[]>(buffer_size);
		
		wcstombs_s(&buffer_size, buffer.get(), buffer_size, wbuffer, _TRUNCATE);

		// send the data using normal char version
		return SendBytes(buffer.get());
	}

	int Socket::ReceiveBytes(char* buf, int len)
	{
		int result = recv(m_NativeSocket, buf, len - 1, 0);
		buf[len - 1] = NULL;
		return result;
	}

	static std::wstring ConvertNarrowStr2WideStr(const std::string& as)
	{
		size_t outSize = as.size() * 2 + 2;
		auto buf = std::make_unique<wchar_t[]>(outSize);
		swprintf(buf.get(), L"%S", as.c_str());
		std::wstring rval = buf.get();

		rval.resize(rval.capacity());

		return rval;
	}

	int Socket::ReceiveWideBytes(wchar_t* buf, int len)
	{
		// Receive the bytes first
		std::string buffer = winrt::to_string(std::wstring(len, 0).data());
		
		buffer.resize(buffer.capacity());

		int iResult = ReceiveBytes(buffer.data(), buffer.capacity());

		if (iResult == SOCKET_ERROR)
		{
			char MSG[256] = "Could not receive bytes: %d";
			auto ec = WSAGetLastError();
			snprintf(MSG, sizeof(MSG), MSG, ec);
			throw std::runtime_error(MSG);
		}
		
		std::wstring out = ConvertNarrowStr2WideStr(buffer);
		for (int i = 0; i < out.length(); i++)
		{
			buf[i] = out[i];
		}
		
		return iResult;
	}

	bool Socket::IsListening() const
	{
		int iResult = -1;
		BOOL isListening = false;
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

		return m_IsListening;
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
		if (m_NativeSocket == INVALID_SOCKET)
		{
			char MSG[1024] = "Error at socket(): %ld\n";
			snprintf(MSG, sizeof(MSG), MSG, WSAGetLastError());
			freeaddrinfo(result);
			throw std::runtime_error(MSG);
		}

		m_SocketAddress = result->ai_addr;
		m_SocketAddressLength = result->ai_addrlen;

		m_IsOpen = true;
	}

	int Socket::Connect(const std::string_view ipAddress, uint16_t port, int addressFamily)
	{
		int iResult = SOCKET_ERROR;
		if (addressFamily == AF_INET)
		{
			sockaddr_in clientService = { 0 };
			clientService.sin_family = AF_INET;
			clientService.sin_addr.s_addr = inet_addr(ipAddress.data());
			clientService.sin_port = htons(port);

			iResult = connect(m_NativeSocket, (SOCKADDR*)&clientService, sizeof(clientService));
		}
		else if (addressFamily == AF_INET6)
		{
			// TODO: Unimplemented for now!
		}

		if (iResult == SOCKET_ERROR)
		{
			char MSG[1024] = "Could not connect to %s:%d. Error: %ld\n";
			auto ec = WSAGetLastError();
			snprintf(MSG, sizeof(MSG), MSG, ipAddress.data(), port, ec);

			throw std::runtime_error(MSG);
		}

		return iResult;
	}

	void Socket::Bind()
	{
		// Bind to a particular port (ie. m_Port)
		int iResult = SOCKET_ERROR;
		iResult = bind(m_NativeSocket, m_SocketAddress, (int)m_SocketAddressLength);
		if (iResult == SOCKET_ERROR)
		{
			char MSG[1024] = "bind failed with error: %d\n";
			snprintf(MSG, sizeof(MSG), MSG, WSAGetLastError());
			//freeaddrinfo(result);
			throw std::runtime_error(MSG);
		}
	}

	void Socket::Listen()
	{
		if (m_IsListening)
			return;

		if (listen(m_NativeSocket, SOMAXCONN) == SOCKET_ERROR) 
		{
			char MSG[1024] = "Listen failed with error: %ld\n";
			auto ec = WSAGetLastError();
			snprintf(MSG, sizeof(MSG), MSG, ec);
			throw std::runtime_error(MSG);
		}
		m_IsListening = true;
	}

	void Socket::Close()
	{
		if (m_NativeSocket != INVALID_SOCKET)
		{
			shutdown(m_NativeSocket, SD_BOTH);
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
		SOCKADDR_STORAGE socketAddress = { 0 };
		int sockAddrSz = sizeof(socketAddress);
		iResult = getpeername(m_NativeSocket, (SOCKADDR*) & socketAddress, &sockAddrSz);

		if (iResult == SOCKET_ERROR)
		{
			char MSG[1024] = "Error encountered while creating Socket object from native socket: %ld\n";
			snprintf(MSG, sizeof(MSG), MSG, WSAGetLastError());
			throw std::runtime_error(MSG);
		}
		
		m_AddressFamily = socketAddress.ss_family;

		switch (m_AddressFamily)
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