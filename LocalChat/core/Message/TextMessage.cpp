#include <Message/TextMessage.h>
#include <utils/Conversion.h>

TextMessage::TextMessage(uint64_t senderHash, uint64_t recipientHash)
	:Message(senderHash, recipientHash)
{}

TextMessage::TextMessage(uint64_t senderHash, uint64_t recipientHash, const std::wstring_view content)
	:Message(senderHash, recipientHash)
	,m_Body(content)
{}

TextMessage::TextMessage(xg::Guid guid, uint64_t senderHash, uint64_t recipientHash, const std::wstring_view content)
	:Message(guid, senderHash, recipientHash)
	,m_Body(content)
{
}

TextMessage::TextMessage(const std::wstring_view guidWStrView, uint64_t senderHash, uint64_t recipientHash, const std::wstring_view content)
	:Message(guidWStrView, senderHash, recipientHash)
	,m_Body(content)
{
}

TextMessage::TextMessage(const TextMessage& other)
	:Message(other.m_GUID, other.m_SenderHash, other.m_RecipientHash)
	,m_Body(other.m_Body)
{}

TextMessage::TextMessage(TextMessage&& other) noexcept
	:Message(other.m_GUID, other.m_SenderHash, other.m_RecipientHash)
	,m_Body(std::move(other.m_Body))
{
	other.m_GUID = xg::Guid();	// empty guid
	other.m_SenderHash = 0;
	other.m_RecipientHash = 0;
	other.m_Body.clear();
}

void TextMessage::Serialize(_Out_ std::wstring& buffer) const
{
	// Add the delimiter (NULL char) as well
	std::string guidStr = m_GUID.str();
	buffer = L"TxtMsg|";
	buffer += GetGUIDWStrView();
	buffer += L'|';
	buffer += std::to_wstring(m_SenderHash);
	buffer += L'|';
	buffer += std::to_wstring(m_RecipientHash);
	buffer += L'|';
	buffer += m_Body;
	buffer += L'\0';
}

TextMessage TextMessage::DeSerialize(_In_ const std::wstring_view& buffer)
{
	std::wstring_view temp = buffer;
	size_t ind = temp.find_first_of(L'|') + 1;
	temp = temp.substr(ind);

	ind = temp.find_first_of(L'|');
	std::wstring_view guidView = temp.substr(0, ind);
	temp = temp.substr(ind + 1);
	
	ind = temp.find_first_of(L'|');
	std::wstring_view senderView = temp.substr(0, ind);
	temp = temp.substr(ind + 1);
	
	ind = temp.find_first_of(L'|');
	std::wstring_view recipientView = temp.substr(0, ind);
	temp = temp.substr(ind + 1);

	std::wstring_view msgView = temp;
	
	uint64_t sender = cnvrt::To<uint64_t>(senderView);
	uint64_t recipient = cnvrt::To<uint64_t>(recipientView);
	std::wstring_view content = msgView;

	return TextMessage(guidView, sender, recipient, content);
}

TextMessage& TextMessage::operator=(const TextMessage& other)
{
	m_GUID = other.m_GUID;
	m_GuidWStr = other.m_GuidWStr;
	m_SenderHash = other.m_SenderHash;
	m_RecipientHash = other.m_RecipientHash;
	m_Body = other.m_Body;
	return *this;
}

TextMessage& TextMessage::operator=(TextMessage&& other) noexcept
{
	m_GUID = std::move(m_GUID);
	m_GuidWStr = std::move(other.m_GuidWStr);
	m_SenderHash = other.m_SenderHash;
	m_RecipientHash = other.m_RecipientHash;
	m_Body = std::move(other.m_Body);

	other.m_GUID = xg::Guid();	// empty guid
	other.m_SenderHash = 0;
	other.m_RecipientHash = 0;
	other.m_GuidWStr.clear();
	other.m_Body.clear();
	return *this;
}