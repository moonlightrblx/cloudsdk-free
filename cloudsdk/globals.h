#pragma once
#include <cstdint>
#include <windows.h>

struct config_t {
    const char* dev_usernames[2] = { "Cxfd", "conspiracy" }; // remove all of these and add ur windows user :D (check custom.h)
    constexpr static uintptr_t xor_key = 0x000000babe; // change per project
};

// global sdk context
struct globals_t {
    uintptr_t base = 0;
    bool is_dev = false;

    struct cache_t { // cache
        bool dev_cached = false;
    } *cache;

    void init() {
        base = (uintptr_t)GetModuleHandleA(0);
        // todo: check dev user
    }
};

extern globals_t* g_ctx;
extern config_t* g_cfg;
