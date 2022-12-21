#include <utils/Conversion.h>

#include <stdexcept>

namespace cnvrt {
	template <typename OutTp>
	OutTp To(const std::wstring_view sv)
	{
		throw std::runtime_error("Conversion routine not implemented");
	}

	template <>
	uint64_t To(const std::wstring_view sv)
	{
		std::wstring_view temp = sv;
		wchar_t* buffEndPtr = (wchar_t*) temp.data() + sv.size();
		uint64_t val = std::wcstoull(sv.data(), &buffEndPtr, 10);
		return val;
	}
}