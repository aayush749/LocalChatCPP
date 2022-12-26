#pragma once
#include <Message/Message.h>

#include <string_view>

class TextMessage : public Message
{
public:
	TextMessage(uint64_t recipientHash);
	TextMessage(uint64_t recipientHash, const std::wstring_view);
	TextMessage(const TextMessage&); // copy constructor
	TextMessage(TextMessage&&) noexcept;	// move constructor

	virtual void Serialize(_Out_ std::wstring& buffer) const override;
	
	// Currently, only handles for default delimiter
	static TextMessage DeSerialize(_In_ const std::wstring_view& buffer);

	TextMessage& operator=(const TextMessage&);	// copy assignment operator
	TextMessage& operator=(TextMessage&&) noexcept; // move assignment operator

private:
	std::wstring m_Body;
};