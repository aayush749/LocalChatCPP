#include <Message/TextMessage.h>

TextMessage::TextMessage()
	:Message(Message::s_DefaultDelimiter)
{}

TextMessage::TextMessage(const std::wstring_view content)
	:Message(Message::s_DefaultDelimiter)
	,m_Body(content)
{}

TextMessage::TextMessage(const std::wstring_view content, const std::wstring_view delimiter)
	:Message(delimiter)
	,m_Body(content)
{}

TextMessage::TextMessage(const TextMessage& other)
	:Message(other.m_Delimiter)
	,m_Body(other.m_Body)
{}

TextMessage::TextMessage(TextMessage&& other)
	:Message(other.m_Delimiter)
	,m_Body(std::move(other.m_Body))
{}

void TextMessage::Serialize(std::wstring& buffer) const
{
	buffer = m_Body;
}

void TextMessage::DeSerialize(std::wstring& buffer) const
{
	// nothing to do
}

TextMessage& TextMessage::operator=(const TextMessage& other)
{
	m_Body = other.m_Body;
	m_Delimiter = other.m_Delimiter;
	return *this;
}

TextMessage& TextMessage::operator=(TextMessage&& other)
{
	m_Body = std::move(other.m_Body);
	std::swap(m_Delimiter, other.m_Delimiter);
	return *this;
}
