#pragma once
#include "../windows/defs.h"
extern "C" {
	void* __getpeb();
	void* __getbase();
}
namespace ASM {
	windef::RPEB* get_peb() {
		return reinterpret_cast<windef::RPEB*>(__getpeb());
	}
	uintptr_t get_base() {
		return reinterpret_cast<uintptr_t>(__getbase());
	}
}