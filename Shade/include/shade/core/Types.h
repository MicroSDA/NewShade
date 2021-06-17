#pragma once
#include <memory>
#include <iostream>

#define SHADE_EXPAND_MACRO(x) x
#define SHADE_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)
#define SHADE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace shade
{
	template<typename T>
	using Shared = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Shared<T> CreateShared(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
	template<typename T>
	using Weak = std::weak_ptr<T>;

	template<typename T>
	using Unique = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Unique<T> CreateUnique(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
	using Tag = std::string;

	//using Model3DComponent = ShadeShared<shade::Model3D>;
}