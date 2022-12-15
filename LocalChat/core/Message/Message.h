#include <Networking/Networking.h>

#include <iostream>
#include <string_view>

class Message
{
public:
	Message(const std::string_view content, const std::string_view delimiter);

	std::string_view GetBody() const { return m_Body; }
	std::string_view GetDelimeter() const { return m_Delimiter; }

private:
	std::string_view m_Body;
	std::string_view m_Delimiter;
};

std::ostream& operator<<(std::ostream&, const Message&);