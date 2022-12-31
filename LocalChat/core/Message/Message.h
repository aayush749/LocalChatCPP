#pragma once

#include <Serializable/Serializable.h>

#include <iostream>
#include <string_view>

class Message : public Serializable
{
public:
	uint64_t GetSenderHash() const { return m_SenderHash; }
	uint64_t GetRecipientHash() const { return m_RecipientHash; }
	uint64_t GetGUID() const { return m_GUID; }
protected:
	Message(uint64_t senderHash, uint64_t recipientHash)
		:m_SenderHash(senderHash), m_RecipientHash(recipientHash)
	{}

protected:
	uint64_t m_GUID;
	uint64_t m_SenderHash;
	uint64_t m_RecipientHash;
};

std::wostream& operator<<(std::wostream&, const Message&);