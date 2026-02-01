#pragma once
#include <iostream>
#include <windows.h>
#include "nocrt.h"

// just some funky colors for the console
#define FOREGROUND_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define FOREGROUND_YELLOW (FOREGROUND_RED | FOREGROUND_GREEN)
#define FOREGROUND_CYAN (FOREGROUND_GREEN | FOREGROUND_BLUE)

namespace logger {
    inline void init() {
        HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);
        if (h_out == INVALID_HANDLE_VALUE) return;

        DWORD dw_mode = 0;
        if (!GetConsoleMode(h_out, &dw_mode)) return;

        dw_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(h_out, dw_mode);
    }

    template <typename ...args_t>
    inline void log(const char* fmt, args_t... args) {
        printf("\033[36m[cloudsdk]\033[0m "); // cyan tag
        printf(fmt, args...);
        printf("\n");
    }

    template <typename ...args_t>
    inline void warn(const char* fmt, args_t... args) {
        printf("\033[33m[cloudsdk!]\033[0m "); // yellow tag
        printf(fmt, args...);
        printf("\n");
    }

    template <typename ...args_t>
    inline void error(const char* fmt, args_t... args) {
        printf("\033[31m[cloudsdk fail]\033[0m "); // red tag
        printf(fmt, args...);
        printf("\n");
    }
}

#define LOG(fmt, ...) logger::log(fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) logger::warn(fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) logger::error(fmt, ##__VA_ARGS__)
