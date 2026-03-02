#pragma once

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#define NOMINMAX

#include <RE/Skyrim.h>
#include <REX/REX.h>
#include <SKSE/SKSE.h>

#include <Windows.h>
#include <chrono>
#include <future>
#include <map>
#include <vector>

namespace logger = SKSE::log;
using namespace std::literals;

template <typename T>
class SINGLETON
{
public:
	static T* getInstance()
	{
		static T instance;
		return std::addressof(instance);
	}

protected:
	SINGLETON() = default;
	~SINGLETON() = default;

	SINGLETON(const SINGLETON&) = delete;
	SINGLETON(SINGLETON&&) = delete;
	SINGLETON& operator=(const SINGLETON&) = delete;
	SINGLETON& operator=(SINGLETON&&) = delete;
};

template <typename T>
class SingletonWithData : public SINGLETON<T>
{
public:
	union variable
	{
		int   inum;
		float fnum;
		void* ptr;
	};
	static std::map<std::string, variable>& getData()
	{
		static std::map<std::string, variable> storage;
		return storage;
	}
};

template <typename T>
class HookTemplate
{
protected:
	HookTemplate() = delete;
	HookTemplate(const HookTemplate&) = delete;
	HookTemplate(HookTemplate&&) = delete;
	~HookTemplate() = delete;

	HookTemplate& operator=(const HookTemplate&) = delete;
	HookTemplate& operator=(HookTemplate&&) = delete;
};

#ifdef SKYRIM_AE
#	define OFFSET(se, ae) ae
#	define OFFSET_3(se, ae, vr) ae
#elif SKYRIMVR
#	define OFFSET(se, ae) se
#	define OFFSET_3(se, ae, vr) vr
#else
#	define OFFSET(se, ae) se
#	define OFFSET_3(se, ae, vr) se
#endif
