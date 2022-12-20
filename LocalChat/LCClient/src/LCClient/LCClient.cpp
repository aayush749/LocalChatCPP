#include <LCClient/LCClient.h>

LCClient::LCClient(const std::string& serverIP, uint16_t serverPort)
	:m_Socket("localhost", 7778, AF_INET, SOCK_STREAM), m_Stream(m_Socket)
{
	m_Socket.Bind();
	try
	{
		m_Socket.Connect(serverIP, serverPort, AF_INET);
	}
	catch (const std::runtime_error& e)
	{
		std::cout << "Could not connect to LocalChatServer: " << e.what();
	}

	// Connected

	// Get the client hash
	m_Hash = 0;
	m_Stream >> m_Hash;
}
