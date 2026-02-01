#define _CRT_SECURE_NO_WARNINGS  // fun fact: this define is older than most fortnite players
#include <iostream>
#include "cloudsdk/include.h"

// define globals here
globals_t* g_ctx = new globals_t();
config_t* g_cfg = new config_t();

// example cloudsdk showcase :)
auto __stdcall DllMain(HINSTANCE dll, DWORD reason, LPVOID) -> int
{
    SPOOF_FUNC;  // spoofs the return address of the function to some legit module :D

    if (reason != DLL_PROCESS_ATTACH) return 0;

    // 1. initialize console & logger
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

    logger::init();  // enable cool colors
    LOG("cloudsdk loaded!");

    // 2. initialize globals
    g_ctx->init();
    LOG("base address: 0x%p", g_ctx->base);

    // 3. string search example
    // "illegal call to staticfindobject() while serializing object data!"
    // we search for this string ref to find staticfindobject (sfo).

    LOG("scanning for sfo...");
    uintptr_t sfo = 0;

    // using memcury scanner
    auto s_ref = Memcury::Scanner::FindStringRef(
                     L"Illegal call to StaticFindObject() while serializing object data!", false, 1)
                     .Get();

    if (!s_ref) {
        LOG_ERROR("failed to find sfo string ref :(");
        return 0;
    }

    // heuristically scan backwards from string ref to find function start
    for (int i = 0; i < 1000; i++) {
        auto ptr = (uint8_t*)(s_ref - i);
        // 0x48 0x89 0x5c = mov [rsp+...], ... (prologue)
        if (*ptr == 0x48 && *(ptr + 1) == 0x89 && *(ptr + 2) == 0x5c) {
            sfo = uint64_t(ptr);
            break;
        }
    }

    if (sfo) {
        LOG("sfo found at 0x%p (offset: 0x%X)", sfo, sfo - g_ctx->base);
    } else {
        LOG_WARN("could not locate sfo prologue.");
    }

    // 4. done
    LOG("init complete. have fun!");
    return 1;
}
