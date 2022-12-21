#pragma once

#include <string>

template <typename OutTp, typename InTp>
OutTp Serialize(const InTp& value);

// Int -> WString serialization
template<>
std::wstring Serialize<std::wstring, int>(const int& value);

// UInt64_t -> WString serialization
template<>
std::wstring Serialize<std::wstring, uint64_t>(const uint64_t& value);