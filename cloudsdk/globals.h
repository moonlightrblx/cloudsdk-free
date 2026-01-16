#pragma once
#include <cstdint>

struct config_t {

	const char* dev_usernames[3] = { "Cxfd", "conspiracy", "username" }; // remove all of these and add ur windows user :D (check custom.h)
	constexpr static uintptr_t xor_key = 0x000000babe; // change per project

};

config_t* g_cfg;

struct globals_t {
	uintptr_t base = 0;
	bool is_dev = false;

	struct cache_t { // cache
		bool dev_cached = false;
	} *cache;
};

globals_t* g_ctx;