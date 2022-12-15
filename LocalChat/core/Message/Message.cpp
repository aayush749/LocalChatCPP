#include <Message/Message.h>

Message::Message(const std::string_view msg, const std::string_view delim)
	:m_Body(msg), m_Delimiter(delim)
{}

// Output operator overloading
std::ostream& operator<<(std::ostream& os, const Message& msg)
{
	os << "Body: " << msg.GetBody() << ", Delimiter: " << msg.GetDelimeter() << std::endl;
	return os;
}