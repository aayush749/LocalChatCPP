#pragma once

#include <string_view>

namespace cnvrt{

	template <typename OutTp>
	OutTp To(const std::wstring_view sv);

	template <>
	uint64_t To(const std::wstring_view sv);
}