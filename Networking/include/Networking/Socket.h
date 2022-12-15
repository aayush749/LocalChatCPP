#pragma once

#include <Networking/Networking.h>

#include <iostream>
#include <string>
#include <string_view>


namespace ntwk {
	class NTWK_EXPORT Socket
	{
	public:
		Socket(std::string_view ip, uint16_t port, int addressFamily, int type);
		Socket(SOCKET);
		Socket(Socket&&) noexcept;
		Socket(Socket&) = delete;
		SOCKET& GetSocket() { return m_NativeSocket; }
		PCSTR GetIPAddress() const { return m_IPAddress.c_str(); }
		PCSTR GetPortStr() const { return m_PortStr.c_str(); }
		void Listen();

		int ReceiveBytes(char* buf, int len);
		int SendBytes(const std::string_view message);
		bool IsOpen() const { return m_IsOpen; }
		bool IsListening() const;
		void Create();
		void Close();

		~Socket();

		bool operator==(const Socket& other) const { return m_NativeSocket == other.m_NativeSocket; }
		bool operator==(const SOCKET& other) const { return m_NativeSocket == other; }

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
	};

}