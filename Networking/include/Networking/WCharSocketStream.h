#pragma once

#include <Networking/Socket.h>

namespace ntwk {
	class WCharSocketStream : public std::wiostream, private std::wstreambuf
	{
	public:
		NTWK_EXPORT WCharSocketStream(Socket& socket);
		NTWK_EXPORT virtual ~WCharSocketStream();

	protected:

		// To write to buffer
		virtual std::streamsize xsputn(const wchar_t* s, std::streamsize size) override;

		// To read from buffer
		virtual std::wstreambuf::int_type underflow() override;


	private:
		Socket& m_Socket;
		std::wstring m_OutputBuf, m_InputBuf;
	};
}