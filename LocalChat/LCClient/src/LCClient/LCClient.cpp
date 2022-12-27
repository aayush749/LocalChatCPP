#include <LCClient/LCClient.h>

#include <utils/Conversion.h>

LCClient::LCClient(const std::string& serverIP, uint16_t serverPort)
	:m_Socket("localhost", 0, AF_INET, SOCK_STREAM), m_Stream(m_Socket)
{
	m_Socket.Bind();
	m_Socket.Connect(serverIP, serverPort, AF_INET);

	// Tell the server to start
	m_Stream << L"start\0";
	
	// Get the client hash
	m_Hash = 0;
	
	std::wstring buff;
	getline(m_Stream, buff, L'\0');
	
	m_Hash = cnvrt::To<uint64_t>(buff);
}

LCClient::~LCClient()
{
	m_Stream << L"close\0";
	m_Socket.Close();
}