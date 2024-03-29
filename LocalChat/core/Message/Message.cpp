#include <Message/Message.h>

// Output operator overloading
std::wostream& operator<<(std::wostream& wos, const Message& msg)
{
	std::wstring tempBuffer;
	msg.Serialize(tempBuffer);
	wos << L"GUID: " << msg.GetGUIDWStrView() << L"Sender#: " << msg.GetSenderHash() << L", Recipient# : " << msg.GetRecipientHash() << L", Body : " << tempBuffer;
	return wos;
}