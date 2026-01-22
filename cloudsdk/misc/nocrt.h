#pragma once
#include <cstdint>
#include <ctype.h>
// todo: add more c functions cause yea..
namespace nocrt {
	__forceinline void* __memset(void* ptr, int value, size_t num) // yes this is pasted 100% from cons :3
	{
		unsigned char* p = (unsigned char*)ptr;
		for (size_t i = 0; i < num; i++) {
			p[i] = (unsigned char)value;
		}
		return ptr;
	}

	__forceinline size_t __strlen(const char* str)
	{
		const char* s = str;
		while (*s)
			++s;
		return (size_t)(s - str);
	}

	__forceinline void* __memcpy(void* destination, const void* source, size_t num)
	{
		unsigned char* dest = (unsigned char*)destination;
		const unsigned char* src = (const unsigned char*)source;
		for (size_t i = 0; i < num; i++) {
			dest[i] = src[i];
		}
		return destination;
	}
	__forceinline void* __memset(void* ptr, uint8_t value, size_t num)
	{
		auto* p = static_cast<uint8_t*>(ptr);
		for (size_t i = 0; i < num; ++i)
		{
			p[i] = value;
		}
		return ptr;
	}

	__forceinline bool memcmp(const void* a, const void* b, size_t size)
	{
		const auto* p1 = static_cast<const uint8_t*>(a);
		const auto* p2 = static_cast<const uint8_t*>(b);

		for (size_t i = 0; i < size; ++i)
		{
			if (p1[i] != p2[i])
				return false;
		}

		return true;
	}

	__forceinline bool strcmp(const char* a, const char* b, bool caseInsensitive = false, size_t maxLen = (size_t)-1)
	{
		size_t count = 0;

		while (*a && *b && count < maxLen) {
			char ca = *a;
			char cb = *b;

			if (caseInsensitive) {
				ca = (char)tolower((unsigned char)ca);
				cb = (char)tolower((unsigned char)cb);
			}

			if (ca != cb)
				return false;

			++a;
			++b;
			++count;
		}
		return (count == maxLen) || (*a == *b);
	}
}