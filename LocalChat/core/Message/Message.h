#pragma once

#include <Serializable/Serializable.h>

#include <crossguid/guid.hpp>

#include <iostream>
#include <string_view>

class Message : public Serializable
{
public:
	uint64_t GetSenderHash() const { return m_SenderHash; }
	uint64_t GetRecipientHash() const { return m_RecipientHash; }
	const xg::Guid& GetGUID() const { return m_GUID; }
	const std::wstring_view GetGUIDWStrView() const { return m_GuidWStr; }

	bool IsSent() const { return m_IsSent; }
	bool IsDelivered() const { return m_IsDelivered; }

	void SetSentStatus(bool status) { m_IsSent = status; }
	void SetDeliveredStatus(bool status) { m_IsSent = status; }

protected:
	Message(uint64_t senderHash, uint64_t recipientHash)
		:m_SenderHash(senderHash), m_RecipientHash(recipientHash), m_GUID(xg::newGuid())
	{
		auto guidStr = m_GUID.str();
		m_GuidWStr = std::wstring(guidStr.begin(), guidStr.end());
	}
	
	Message(xg::Guid guid, uint64_t senderHash, uint64_t recipientHash)
		:m_GUID(guid), m_SenderHash(senderHash), m_RecipientHash(recipientHash)
	{
		auto guidStr = m_GUID.str();
		m_GuidWStr = std::wstring(guidStr.begin(), guidStr.end());
	}

	Message(const std::wstring_view guidWStrView, uint64_t senderHash, uint64_t recipientHash)
		:m_GuidWStr(guidWStrView)
		,m_GUID(std::string(guidWStrView.begin(), guidWStrView.end()))
		,m_SenderHash(senderHash)
		,m_RecipientHash(recipientHash)
	{}

protected:
	xg::Guid m_GUID;
	uint64_t m_SenderHash;
	uint64_t m_RecipientHash;

	std::wstring m_GuidWStr;

	bool m_IsSent, m_IsDelivered;
};

std::wostream& operator<<(std::wostream&, const Message&);