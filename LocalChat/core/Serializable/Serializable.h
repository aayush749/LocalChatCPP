#pragma once

#include <sal.h>

#include <string>

class Serializable
{
public:
	virtual void Serialize(_Out_ std::wstring&) const = 0;
	virtual void DeSerialize(_Inout_ std::wstring&) const = 0;
};