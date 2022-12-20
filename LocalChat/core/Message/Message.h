#pragma once

#include <Serializable/Serializable.h>

#include <iostream>
#include <string_view>

class Message : public Serializable
{
public:
	std::wstring_view GetDelimiter() const { return m_Delimiter; }
	uint64_t GetRecipientHash() const { return m_RecipientHash; }
protected:
	Message(uint64_t recipientHash, std::wstring_view delimiter)
		:m_RecipientHash(recipientHash), m_Delimiter(delimiter)
	{}

protected:
	inline static std::wstring_view s_DefaultDelimiter = L"♀";
	std::wstring_view m_Delimiter;
	uint64_t m_RecipientHash;
};

std::wostream& operator<<(std::wostream&, const Message&);