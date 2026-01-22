#define _CRT_SECURE_NO_WARNINGS // use this instead of freopen_s cause im lazy
#include <iostream>
#include "cloudsdk/include.h"
// gonna make the example acc smth useful for free one??!?

// example cloudsdk showcase :D
auto __stdcall DllMain(HINSTANCE dll, DWORD reason, LPVOID) -> int
{
	SPOOF_FUNC; // spoofs the return address of the function :D

	// this will find the stringref in ANY ue game with no string encryption or weird protections.
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	uintptr_t sfo;
	auto sRef = Memcury::Scanner::FindStringRef(L"Illegal call to StaticFindObject() while serializing object data!", false, 1).Get();

	for (int i = 0; i < 1000; i++) {
		auto Ptr = (uint8_t*)(sRef - i);
		// yada yada magic numbers from ue4 :D
		if (*Ptr == 0x48 && *(Ptr + 1) == 0x89 && *(Ptr + 2) == 0x5c) {
			sfo = uint64_t(Ptr);
			break;
		}
	}

	printf("sfo found at 0x%p", sfo - (uintptr_t)GetModuleHandleA(0));

	// blehg

}
