#pragma once

#include <Networking/Networking.h>

#include <iostream>
#include <string>
#include <string_view>


namespace ntwk {
	class Socket
	{
	public:
		NTWK_EXPORT Socket(std::string_view ip, uint16_t port, int addressFamily, int type);
		NTWK_EXPORT Socket(SOCKET);
		NTWK_EXPORT Socket(Socket&&) noexcept;
		NTWK_EXPORT Socket(Socket&) = delete;
		NTWK_EXPORT SOCKET& GetSocket() { return m_NativeSocket; }
		NTWK_EXPORT PCSTR GetIPAddress() const { return m_IPAddress.c_str(); }
		NTWK_EXPORT PCSTR GetPortStr() const { return m_PortStr.c_str(); }
		NTWK_EXPORT void Listen();
		NTWK_EXPORT void Bind();
		NTWK_EXPORT int Connect(const std::string_view ipAddress, uint16_t port, int addressFamily);
		NTWK_EXPORT bool SetNonBlockingMode(bool shouldBeNonBlocking);

		NTWK_EXPORT int ReceiveBytes(char* buf, int len);
		NTWK_EXPORT int ReceiveWideBytes(wchar_t* buf, int len);

		NTWK_EXPORT int SendNBytes(const char* str, int n);
		NTWK_EXPORT int SendBytes(const std::string_view message);
		NTWK_EXPORT int SendNWideBytes(const wchar_t* str, int n);
		NTWK_EXPORT int SendWideBytes(const std::wstring_view message);

		NTWK_EXPORT bool IsOpen() const { return m_IsOpen; }
		NTWK_EXPORT bool IsListening() const;
		NTWK_EXPORT void Create();
		NTWK_EXPORT void Close();

		NTWK_EXPORT ~Socket();

		NTWK_EXPORT bool operator==(const Socket& other) const { return m_NativeSocket == other.m_NativeSocket; }
		NTWK_EXPORT bool operator==(const SOCKET& other) const { return m_NativeSocket == other; }

		NTWK_EXPORT std::string ToString() const;
		NTWK_EXPORT friend std::ostream& operator<<(std::ostream&, const Socket&);
	private:
		void FillPortAddrFamAndIP();
	private:
		int m_AddressFamily;
		int m_Type;
		std::string m_IPAddress;
		uint16_t m_Port;
		std::string m_PortStr;
		bool m_IsOpen;
		mutable bool m_IsListening;
		SOCKET m_NativeSocket;
		SOCKADDR* m_SocketAddress;
		std::size_t m_SocketAddressLength;
	};

}