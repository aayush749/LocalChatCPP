#include <Message/TextMessage.h>

TextMessage::TextMessage(uint64_t recipientHash)
	:Message(recipientHash, Message::s_DefaultDelimiter)
{}

TextMessage::TextMessage(uint64_t recipientHash, const std::wstring_view content)
	:Message(recipientHash, Message::s_DefaultDelimiter)
	,m_Body(content)
{}

TextMessage::TextMessage(uint64_t recipientHash, const std::wstring_view content, const std::wstring_view delimiter)
	:Message(recipientHash, delimiter)
	,m_Body(content)
{}

TextMessage::TextMessage(const TextMessage& other)
	:Message(other.m_RecipientHash, other.m_Delimiter)
	,m_Body(other.m_Body)
{}

TextMessage::TextMessage(TextMessage&& other) noexcept
	:Message(other.m_RecipientHash, other.m_Delimiter)
	,m_Body(std::move(other.m_Body))
{}

void TextMessage::Serialize(_Out_ std::wstring& buffer) const
{
	// Add the delimiter as well
	buffer = m_Body + m_Delimiter.data();
}

void TextMessage::DeSerialize(_Inout_ std::wstring& buffer) const
{
	buffer = m_Body;
}

TextMessage& TextMessage::operator=(const TextMessage& other)
{
	m_Body = other.m_Body;
	m_Delimiter = other.m_Delimiter;
	return *this;
}

TextMessage& TextMessage::operator=(TextMessage&& other) noexcept
{
	m_Body = std::move(other.m_Body);
	std::swap(m_Delimiter, other.m_Delimiter);
	return *this;
}