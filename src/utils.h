#ifndef H_UTILS
#define H_UTILS

#include "include.h"
#include "mathlib/mathlib.h"
#include "sdk/usercmd.h"

struct QColor
{
	unsigned char r, g, b, a;
};

typedef void* (*CreateInterfaceFn)(const char *name, int *ret);

namespace Utils
{
	template <class T>
	T* GetInterface(const char* moduleName, const char* interfaceVersion)
	{
		CreateInterfaceFn CreateInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandle(moduleName), "CreateInterface");
		return reinterpret_cast<T*>(CreateInterface(interfaceVersion, NULL));
	}

	template <class T>
	T GetExport(const char* moduleName, const char* exportName)
	{
		return reinterpret_cast<T>(GetProcAddress(GetModuleHandle(moduleName), exportName));
	}

	std::uint8_t* PatternScan(const char* moduleName, const char* signature)
	{
		void* module = GetModuleHandle(moduleName);

		static auto pattern_to_byte = [](const char* pattern) {
			auto bytes = std::vector<int>{};
			auto start = const_cast<char*>(pattern);
			auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current) {
				if (*current == '?') {
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else {
					bytes.push_back(strtoul(current, &current, 16));
				}
			}
			return bytes;
		};

		auto dosHeader = (PIMAGE_DOS_HEADER)module;
		auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

		auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto patternBytes = pattern_to_byte(signature);
		auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

		auto s = patternBytes.size();
		auto d = patternBytes.data();

		for (auto i = 0ul; i < sizeOfImage - s; ++i) {
			bool found = true;
			for (auto j = 0ul; j < s; ++j) {
				if (scanBytes[i + j] != d[j] && d[j] != -1) {
					found = false;
					break;
				}
			}
			if (found) {
				return &scanBytes[i];
			}
		}
		return nullptr;
	}
}

#endif