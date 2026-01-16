#pragma once
#include <Windows.h>
#include <random>
#include <fstream>
#include <optional>
#include "globals.h"
#include "misc/nocrt.h"

namespace custom {
	__forceinline bool is_dev() {
		if (!g_ctx->cache->dev_cached) {
			if (free) {
				g_ctx->is_dev = false;
				g_ctx->cache->dev_cached = true;
				return false;
			}

			char username[256]{};
			DWORD size = sizeof(username);

			/*const char* dev_list[] = {
				"Cxfd",
				"conspiracy",
				"emili"
			};*/

			if (GetUserNameA(username, &size)) {
				for (auto name : g_cfg->dev_usernames) {
					auto enc = name;
					if (nocrt::strcmp(enc, username, true)) {
						g_ctx->is_dev = true;
						break;
					}
				}
			}
			g_ctx->cache->dev_cached = true;
		}
		return g_ctx->is_dev;
	}


	__forceinline inline std::wstring random_wstring(size_t length) {
		const std::wstring characters =
			L"0123456789"
			L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			L"abcdefghijklmnopqrstuvwxyz";

		std::wstring result;
		result.reserve(length);

		std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
		std::uniform_int_distribution<> dist(0, characters.size() - 1);

		for (size_t i = 0; i < length; ++i) {
			result += characters[dist(rng)];
		}

		return result;
	}
	__forceinline std::string random_string(size_t length) {
		const std::string characters =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		std::string result;
		result.reserve(length);
		std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
		std::uniform_int_distribution<> dist(0, characters.size() - 1);
		for (size_t i = 0; i < length; ++i) {
			result += characters[dist(rng)];
		}
		return result;
	}

	__forceinline std::string to_string(const wchar_t* wstr) {
		int len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
		std::string str(len, '\0');
		WideCharToMultiByte(CP_ACP, 0, wstr, -1, &str[0], len, nullptr, nullptr);
		if (!str.empty() && str.back() == '\0') str.pop_back();
		return str;
	}

	__forceinline std::wstring to_wstring(const char* str) {
		int len = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
		std::wstring wstr(len, L'\0');
		MultiByteToWideChar(CP_ACP, 0, str, -1, &wstr[0], len);
		if (!wstr.empty() && wstr.back() == L'\0') wstr.pop_back();
		return wstr;
	}

	__forceinline int random_int(int min, int max) {
		std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
		std::uniform_int_distribution<int> dist(min, max);
		return dist(rng);
	}
	__forceinline float random_float(float min, float max) {
		std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
		std::uniform_real_distribution<float> dist(min, max);
		return dist(rng);
	}
}
