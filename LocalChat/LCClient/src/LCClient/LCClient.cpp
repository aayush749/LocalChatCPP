#include <LCClient/LCClient.h>

#include <utils/Conversion.h>

LCClient::LCClient(const std::string& serverIP, uint16_t serverPort)
	:m_Socket("localhost", 7778, AF_INET, SOCK_STREAM), m_Stream(m_Socket)
{
	m_Socket.Bind();
	m_Socket.Connect(serverIP, serverPort, AF_INET);
	// Connected

	// Get the client hash
	m_Hash = 0;
	
	std::wstring buff;
	getline(m_Stream, buff, L'\r');
	
	m_Hash = cnvrt::To<uint64_t>(buff);

	// Send the default delimiter to be used
	m_Stream << L"\r ";
}

LCClient::~LCClient()
{
	m_Socket.Close();
}