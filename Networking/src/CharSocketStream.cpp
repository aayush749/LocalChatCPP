#include <Networking/CharSocketStream.h>

ntwk::CharSocketStream::CharSocketStream(ntwk::Socket& socket)
	:std::iostream(this), std::streambuf(),
	 m_Socket(socket), m_InputBuf(""), m_OutputBuf("")
{}

ntwk::CharSocketStream::~CharSocketStream()
{}

std::streamsize ntwk::CharSocketStream::xsputn(const char* buffer, std::streamsize size)
{
	// Send the data
	int iResult = m_Socket.SendNBytes(buffer, static_cast<int>(size));

	// Set up the buffer
	m_OutputBuf = buffer;

	// Set up the pointer
	if (m_OutputBuf.size() == 0)
	{
		// Maybe throw an error
		// setp(&m_OutputBuf[0],&m_OutputBuf[0],&m_OutputBuf[0]);
	}
	else
	{
		setp(&m_OutputBuf[0], &m_OutputBuf[0], &m_OutputBuf[m_OutputBuf.size() - 1]);
	}
	return size;
}

std::streambuf::int_type ntwk::CharSocketStream::underflow()
{
	const int readSize = 30;

	// gptr() -> returns get pointer
	// egptr() -> returns pointer to the element one past the last element of the buffer

	// First check to make sure that the buffer is not exhausted
	if (gptr() < egptr())
	{
		return std::streambuf::traits_type::to_int_type(*gptr());
	}

	// clear the bufffer
	m_InputBuf.clear();
	m_InputBuf.resize(readSize + 1);

	// Let's read 
	int bytesRead = m_Socket.ReceiveBytes(&m_InputBuf[0], static_cast<int>(readSize));

	// return the end of file if we read no bytes
	if (bytesRead == 0)
	{
		return std::streambuf::traits_type::eof();
	}

	if (bytesRead < 0)
	{
		// Throw an exception maybe, for now return EOF
		return std::streambuf::traits_type::eof();
	}

	// Set the pointers for the buffer
	setg(&m_InputBuf[0], &m_InputBuf[0], &m_InputBuf[bytesRead]);

	// Finaly, let's return the data type
	return std::streambuf::traits_type::to_int_type(*gptr());
}