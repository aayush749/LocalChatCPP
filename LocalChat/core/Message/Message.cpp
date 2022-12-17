#include <Message/Message.h>

// Output operator overloading
std::wostream& operator<<(std::wostream& wos, const Message& msg)
{
	std::wstring tempBuffer;
	msg.Serialize(tempBuffer);
	wos << "Body: " << tempBuffer << ", Delimiter: " << msg.GetDelimiter() << std::endl;
	return wos;
}