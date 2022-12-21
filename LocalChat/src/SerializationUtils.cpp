#include <utils/SerializationUtils.h>

#include <stdexcept>

template <typename OutTp, typename InTp>
OutTp Serialize(const InTp& value)
{
	throw std::runtime_error("Fuction not implemented");
}

template<>
std::wstring Serialize<std::wstring, int>(const int& value)
{
	std::wstring out = std::to_wstring(value);
	return out;
}

template<>
std::wstring Serialize<std::wstring, uint64_t>(const uint64_t& value)
{
	std::wstring out = std::to_wstring(value);
	return out;
}