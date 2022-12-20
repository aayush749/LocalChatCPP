#include <LCClient/LCClient.h>

LCClient::LCClient(const std::string& serverIP, uint16_t serverPort)
	:m_Socket("localhost", 7778, AF_INET, SOCK_STREAM), m_Stream(m_Socket)
{
	m_Socket.Bind();
	m_Socket.Connect(serverIP, serverPort, AF_INET);
	// Connected

	// Get the client hash
	m_Hash = 0;
	//m_Stream >> m_Hash;
	std::wstring buff;
	getline(m_Stream, buff, L'♀');
	m_Hash = *reinterpret_cast<decltype(m_Hash)*>(buff.data());
}
