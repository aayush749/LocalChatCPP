#include <Message/ControlMessage.h>

ControlMessage ControlMessage::DeSerialize(_In_ const std::wstring_view buffer)
{
	std::wstring_view temp = buffer;

	int ind = temp.find_first_of(L'|');
	const std::wstring_view guidView = temp.substr(0, ind);
	temp = temp.substr(++ind);

	const std::wstring_view typeView = temp;

	return ControlMessage(xg::Guid(guidView), (ControlMessageType) std::stoi(typeView));
}