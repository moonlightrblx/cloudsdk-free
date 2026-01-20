#pragma once
#include <windows.h>
#include "../prot/spoof_call.h"

class c_resolve {
public:
	template<typename t>
	static t get_func(const char* mod, const char* name, bool bshouldload) {  // todo: avoid calling winapi here
		HMODULE hMod = SPOOF_CALL(GetModuleHandleA)(mod);
		if (bshouldload && !mod) {
			SPOOF_CALL(LoadLibraryA)(mod);
		}
		if (mod) {
			return (t)SPOOF_CALL(GetProcAddress)(hMod, name);
		}
	}
};