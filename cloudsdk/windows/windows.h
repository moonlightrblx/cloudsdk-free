#pragma once
#include <Windows.h>
#include <iostream>
#include <thread>
#include <random>
#include <VersionHelpers.h>
#include <string>
#include <sstream>
#include <TlHelp32.h>
#include "defs.h"

// When you say "I wrote a program that crashed Windows", people just stare at you blankly and say 
// "Hey, I got those with the system, *for free*"
// - Linus Torvalds
// real asl ngl

namespace windows {

	__forceinline bool is_admin()
	{
		HANDLE token;

		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
		{
			TOKEN_ELEVATION elevation{};
			DWORD size;


			if (GetTokenInformation(token, TokenElevation, &elevation, sizeof elevation, &size))
				return elevation.TokenIsElevated;
		}
		return false;
	}

	__forceinline INT32 get_pid(LPCTSTR process_name) { // the method >.<

		// Software is like sex; it's better when it's free.  - Linus Torvalds
		// thx linus <3
		PROCESSENTRY32 pt;
		HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		pt.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hsnap, &pt)) {
			do {
				if (!lstrcmpi(pt.szExeFile, process_name)) {
					CloseHandle(hsnap);
					return pt.th32ProcessID;
				}
			} while (Process32Next(hsnap, &pt));
		}
		CloseHandle(hsnap);

		return { NULL };
	}

	__forceinline void bsod() {
		BOOLEAN bEnabled;
		ULONG uResp;
		LPVOID lpFuncAddress = GetProcAddress(LoadLibraryA("ntdll.dll"), "RtlAdjustPrivilege");
		LPVOID lpFuncAddress2 = GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtRaiseHardError");
		windef::funcs::RtlAdjustPrivilege NtCall = (windef::funcs::RtlAdjustPrivilege)lpFuncAddress;
		windef::funcs::NtRaiseHardError NtCall2 = (windef::funcs::NtRaiseHardError)lpFuncAddress2;
		NTSTATUS NtRet = NtCall(19, TRUE, FALSE, &bEnabled);
		NtCall2(STATUS_FLOAT_MULTIPLE_FAULTS, 0, 0, 0, 6, &uResp);
	}

	__forceinline std::unique_ptr<void, decltype(&CloseHandle)> OpenHandle(DWORD ProcessId) {
		auto Handle = std::unique_ptr<void, decltype(&CloseHandle)>(OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId), CloseHandle);

		if (!Handle)
			throw std::runtime_error("OpenProcess failed: " + std::to_string(GetLastError()));

		// belgh i hate this >:(

		return Handle;
	}

	std::string get_windowsver() {

		HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
		if (hMod) {
			auto fxPtr = (windef::funcs::RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");
			if (fxPtr) {
				RTL_OSVERSIONINFOW rovi = { sizeof(rovi) };
				if (fxPtr(&rovi) == 0) {
					char buf[128];
					snprintf(buf, sizeof(buf), "Windows %lu.%lu (Build %lu)",
						rovi.dwMajorVersion, rovi.dwMinorVersion, rovi.dwBuildNumber);
					return buf;
				}
			}
		}
		return "Unknown Windows version";
	}

	__forceinline bool is_x64() { return sizeof(void*) == 8; }

	__forceinline bool is_windows10_or_greater() {
		if (get_windowsver().find("Windows 10") != std::string::npos || get_windowsver().find("Windows 11") != std::string::npos) {
			return true;
		}
		return false;
		// return IsWindows10OrGreater(); // i love when winapi functions js dont work </3
		// https://learn.microsoft.com/en-us/windows/win32/api/versionhelpers/nf-versionhelpers-iswindows10orgreater
	}
}
