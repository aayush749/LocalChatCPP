#pragma once

#include <sal.h>

#include <string>

class Serializable
{
public:
	virtual void Serialize(_Out_ std::wstring&) const = 0;
};