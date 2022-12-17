#pragma once

#include <Serializable/Serializable.h>

#include <iostream>
#include <string_view>

class Message : public Serializable
{
public:
	std::wstring_view GetDelimiter() const { return m_Delimiter; }

protected:
	Message(std::wstring_view delimiter)
		:m_Delimiter(delimiter)
	{}

protected:
	inline static std::wstring_view s_DefaultDelimiter = L"♀";
	std::wstring_view m_Delimiter;
};

std::wostream& operator<<(std::wostream&, const Message&);