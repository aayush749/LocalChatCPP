#pragma once

#include <Serializable/Serializable.h>
#include <Message/ControlMessageTypes.h>
#include <crossguid/guid.hpp>
#include <utils/Conversion.h>

#include <string>
#include <string_view>

class ControlMessage : public Serializable
{
public:
	ControlMessage(const xg::Guid& guid, ControlMessageType type)
		:m_Guid(guid), m_Type(type)
	{}

	void Serialize(_Out_ std::wstring& buffer) const override
	{
		buffer.clear();
		std::string_view guidStr = m_Guid.str();

		buffer += std::wstring(guidStr.begin(), guidStr.end());
		buffer += L'|';
		buffer += std::to_wstring((int) m_Type);

		buffer += L'\0';
	}

	static ControlMessage DeSerialize(_In_ const std::wstring_view buffer);

private:
	xg::Guid m_Guid;
	ControlMessageType m_Type;
};