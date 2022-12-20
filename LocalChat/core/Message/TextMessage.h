#pragma once
#include <Message/Message.h>

#include <string_view>

class TextMessage : public Message
{
public:
	TextMessage(uint64_t recipientHash);
	TextMessage(uint64_t recipientHash, const std::wstring_view);
	TextMessage(uint64_t recipientHash, const std::wstring_view, const std::wstring_view delimiter);
	TextMessage(const TextMessage&); // copy constructor
	TextMessage(TextMessage&&) noexcept;	// move constructor

	virtual void Serialize(_Out_ std::wstring& buffer) const override;
	virtual void DeSerialize(_Inout_ std::wstring& buffer) const override;

	TextMessage& operator=(const TextMessage&);	// copy assignment operator
	TextMessage& operator=(TextMessage&&) noexcept; // move assignment operator

private:
	std::wstring m_Body;
};