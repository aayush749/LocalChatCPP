#pragma once

#include <Serializable/Serializable.h>

#include <iostream>
#include <string_view>

class Message : public Serializable
{
public:
	uint64_t GetRecipientHash() const { return m_RecipientHash; }
protected:
	Message(uint64_t recipientHash)
		:m_RecipientHash(recipientHash)
	{}

protected:
	uint64_t m_RecipientHash;
};

std::wostream& operator<<(std::wostream&, const Message&);