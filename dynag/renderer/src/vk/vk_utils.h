#ifndef VK_UTILS
#define VK_UTILS

#include <functional>

namespace Vulkan
{

	template<typename T, typename... Rest>
	void hashCombine(std::size_t& seed, const T& v, const Rest&... rest)
	{
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hashCombine(seed, rest), ...);
	};

}
#endif // !VK_UTILS