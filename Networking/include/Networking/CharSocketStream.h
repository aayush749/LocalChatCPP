#include <Networking/Socket.h>

namespace ntwk {
	class CharSocketStream : public std::iostream, private std::streambuf
	{
	public:
		NTWK_EXPORT CharSocketStream(Socket& socket);
		NTWK_EXPORT virtual ~CharSocketStream();
		
	protected:

		// To write to buffer
		virtual std::streamsize xsputn(const char* s, std::streamsize size) override;
		
		// To read from buffer
		virtual std::streambuf::int_type underflow() override;


	private:
		Socket& m_Socket;
		std::string m_OutputBuf, m_InputBuf;
	};
}