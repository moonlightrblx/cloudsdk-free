// https://github.com/kem0x/memcury

// this is infact edited from the original. i dont know how much tbh :D
#pragma once
#include <thread>
#include <string>
#include <sstream>
#include <format>
#include <vector>
#include <stdexcept>
#include <type_traits>
#include <intrin.h>
#include <Windows.h>
#include <source_location>
#include <DbgHelp.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <array>
#include <map>

#define MemcuryAssert(cond)                                              \
        if (!(cond))                                                         \
        {                                                                    \
            MessageBoxA(nullptr, #cond, __FUNCTION__, MB_ICONERROR | MB_OK); \
            Memcury::Safety::FreezeCurrentThread();                          \
        }

#define MemcuryAssertM(cond, msg)                                      \
        if (!(cond))                                                       \
        {                                                                  \
            MessageBoxA(nullptr, msg, __FUNCTION__, MB_ICONERROR | MB_OK); \
        }

#define MemcuryThrow(msg)                                          \
        MessageBoxA(nullptr, msg, __FUNCTION__, MB_ICONERROR | MB_OK); \
        Memcury::Safety::FreezeCurrentThread();

namespace Memcury
{
	extern "C" IMAGE_DOS_HEADER __ImageBase;

	inline auto GetCurrentModule() -> HMODULE
	{
		return reinterpret_cast<HMODULE>(&__ImageBase);
	}

	namespace Util
	{
		template <typename T>
		constexpr static auto IsInRange(T value, T min, T max) -> bool
		{
			return value >= min && value < max;
		}

		constexpr auto StrHash(const char* str, int h = 0) -> unsigned int
		{
			return !str[h] ? 5381 : (StrHash(str, h + 1) * 33) ^ str[h];
		}

		inline auto IsSamePage(void* A, void* B) -> bool
		{
			MEMORY_BASIC_INFORMATION InfoA;
			if (!VirtualQuery(A, &InfoA, sizeof(InfoA)))
			{
				return true;
			}

			MEMORY_BASIC_INFORMATION InfoB;
			if (!VirtualQuery(B, &InfoB, sizeof(InfoB)))
			{
				return true;
			}

			return InfoA.BaseAddress == InfoB.BaseAddress;
		}

		inline auto GetModuleStartAndEnd() -> std::pair<uintptr_t, uintptr_t>
		{
			auto HModule = GetCurrentModule();
			auto NTHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>((uintptr_t)HModule + reinterpret_cast<PIMAGE_DOS_HEADER>((uintptr_t)HModule)->e_lfanew);

			uintptr_t dllStart = (uintptr_t)HModule;
			uintptr_t dllEnd = (uintptr_t)HModule + NTHeaders->OptionalHeader.SizeOfImage;

			return { dllStart, dllEnd };
		}

		inline auto CopyToClipboard(std::string str)
		{
			auto mem = GlobalAlloc(GMEM_FIXED, str.size() + 1);
			memcpy(mem, str.c_str(), str.size() + 1);

			OpenClipboard(nullptr);
			EmptyClipboard();
			SetClipboardData(CF_TEXT, mem);
			CloseClipboard();

			GlobalFree(mem);
		}
	}

	namespace Safety
	{
		enum class ExceptionMode
		{
			None,
			CatchDllExceptionsOnly,
			CatchAllExceptions
		};

		static auto FreezeCurrentThread() -> void
		{
			SuspendThread(GetCurrentThread());
		}

		static auto PrintStack(CONTEXT* ctx) -> void
		{
			STACKFRAME64 stack;
			memset(&stack, 0, sizeof(STACKFRAME64));

			auto process = GetCurrentProcess();
			auto thread = GetCurrentThread();

			SymInitialize(process, NULL, TRUE);

			bool result;
			DWORD64 displacement = 0;

			char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)]{ 0 };
			char name[256]{ 0 };
			char module[256]{ 0 };

			PSYMBOL_INFO symbolInfo = (PSYMBOL_INFO)buffer;

			for (ULONG frame = 0;; frame++)
			{
				result = StackWalk64(
					IMAGE_FILE_MACHINE_AMD64,
					process,
					thread,
					&stack,
					ctx,
					NULL,
					SymFunctionTableAccess64,
					SymGetModuleBase64,
					NULL);

				if (!result)
					break;

				symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
				symbolInfo->MaxNameLen = MAX_SYM_NAME;
				SymFromAddr(process, (ULONG64)stack.AddrPC.Offset, &displacement, symbolInfo);

				HMODULE hModule = NULL;
				lstrcpyA(module, "");
				GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (const wchar_t*)(stack.AddrPC.Offset), &hModule);

				if (hModule != NULL)
					GetModuleFileNameA(hModule, module, 256);

				printf("[%lu] Name: %s - Address: %p  - Module: %s\n", frame, symbolInfo->Name, (void*)symbolInfo->Address, module);
			}
		}

		template <ExceptionMode mode>
		auto MemcuryGlobalHandler(EXCEPTION_POINTERS* ExceptionInfo) -> long
		{
			auto [dllStart, dllEnd] = Util::GetModuleStartAndEnd();

			if constexpr (mode == ExceptionMode::CatchDllExceptionsOnly)
			{
				if (!Util::IsInRange(ExceptionInfo->ContextRecord->Rip, dllStart, dllEnd))
				{
					return EXCEPTION_CONTINUE_SEARCH;
				}
			}

			auto message = std::format("Memcury caught an exception at [{:x}]\nPress Yes if you want the address to be copied to your clipboard", ExceptionInfo->ContextRecord->Rip);
			if (MessageBoxA(nullptr, message.c_str(), "Error", MB_ICONERROR | MB_YESNO) == IDYES)
			{
				std::string clip = std::format("{:x}", ExceptionInfo->ContextRecord->Rip);
				Util::CopyToClipboard(clip);
			}

			PrintStack(ExceptionInfo->ContextRecord);

			FreezeCurrentThread();

			return EXCEPTION_EXECUTE_HANDLER;
		}

		template <ExceptionMode mode>
		static auto SetExceptionMode() -> void
		{
			SetUnhandledExceptionFilter(MemcuryGlobalHandler<mode>);
		}
	}

	namespace Globals
	{
		constexpr const bool bLogging = true;

		static const char* moduleName = nullptr;
	}

	namespace ASM
	{
		//@todo: this whole namespace needs a rework, should somehow make this more modern and less ugly.
		enum MNEMONIC : uint8_t
		{
			JMP_REL8 = 0xEB,
			JMP_REL32 = 0xE9,
			JMP_EAX = 0xE0,
			CALL = 0xE8,
			LEA = 0x8D,
			CDQ = 0x99,
			CMOVL = 0x4C,
			CMOVS = 0x48,
			CMOVNS = 0x49,
			NOP = 0x90,
			INT3 = 0xCC,
			RETN_REL8 = 0xC2,
			RETN = 0xC3,
			NONE = 0x00
		};

		constexpr int SIZE_OF_JMP_RELATIVE_INSTRUCTION = 5;
		constexpr int SIZE_OF_JMP_ABSLOUTE_INSTRUCTION = 13;

		constexpr auto MnemonicToString(MNEMONIC e) -> const char*
		{
			switch (e)
			{
			case JMP_REL8:
				return "JMP_REL8";
			case JMP_REL32:
				return "JMP_REL32";
			case JMP_EAX:
				return "JMP_EAX";
			case CALL:
				return "CALL";
			case LEA:
				return "LEA";
			case CDQ:
				return "CDQ";
			case CMOVL:
				return "CMOVL";
			case CMOVS:
				return "CMOVS";
			case CMOVNS:
				return "CMOVNS";
			case NOP:
				return "NOP";
			case INT3:
				return "INT3";
			case RETN_REL8:
				return "RETN_REL8";
			case RETN:
				return "RETN";
			case NONE:
				return "NONE";
			default:
				return "UNKNOWN";
			}
		}

		constexpr auto Mnemonic(const char* s) -> MNEMONIC
		{
			switch (Util::StrHash(s))
			{
			case Util::StrHash("JMP_REL8"):
				return JMP_REL8;
			case Util::StrHash("JMP_REL32"):
				return JMP_REL32;
			case Util::StrHash("JMP_EAX"):
				return JMP_EAX;
			case Util::StrHash("CALL"):
				return CALL;
			case Util::StrHash("LEA"):
				return LEA;
			case Util::StrHash("CDQ"):
				return CDQ;
			case Util::StrHash("CMOVL"):
				return CMOVL;
			case Util::StrHash("CMOVS"):
				return CMOVS;
			case Util::StrHash("CMOVNS"):
				return CMOVNS;
			case Util::StrHash("NOP"):
				return NOP;
			case Util::StrHash("INT3"):
				return INT3;
			case Util::StrHash("RETN_REL8"):
				return RETN_REL8;
			case Util::StrHash("RETN"):
				return RETN;
			default:
				return NONE;
			}
		}

		inline auto byteIsA(uint8_t byte, MNEMONIC opcode) -> bool
		{
			return byte == opcode;
		}

		inline auto byteIsAscii(uint8_t byte) -> bool
		{
			static constexpr bool isAscii[0x100] = {
				false, false, false, false, false, false, false, false, false, true, true, false, false, true, false, false,
				false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
				true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
				true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
				true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
				true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
				true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
				true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false,
				false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
				false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
				false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
				false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
				false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
				false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
				false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
				false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false
			};

			return isAscii[byte];
		}

		inline bool isJump(uint8_t byte)
		{
			return byte >= 0x70 && byte <= 0x7F;
		}

		static auto pattern2bytes(const char* pattern) -> std::vector<int>
		{
			auto bytes = std::vector<int>{};
			const auto start = const_cast<char*>(pattern);
			const auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current)
			{
				if (*current == '?')
				{
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else
				{
					bytes.push_back(strtoul(current, &current, 16));
				}
			}
			return bytes;
		}
	}

	namespace PE
	{
		inline auto SetCurrentModule(const char* moduleName) -> void
		{
			Globals::moduleName = moduleName;
		}

		inline auto GetModuleBase() -> uintptr_t
		{
			return reinterpret_cast<uintptr_t>(GetModuleHandleA(Globals::moduleName));
		}

		inline auto GetDOSHeader() -> PIMAGE_DOS_HEADER
		{
			return reinterpret_cast<PIMAGE_DOS_HEADER>(GetModuleBase());
		}

		inline auto GetNTHeaders() -> PIMAGE_NT_HEADERS
		{
			return reinterpret_cast<PIMAGE_NT_HEADERS>(GetModuleBase() + GetDOSHeader()->e_lfanew);
		}

		class Address
		{
			uintptr_t _address;

		public:
			Address()
			{
				_address = 0;
			}

			Address(uintptr_t address)
				: _address(address)
			{
			}

			Address(void* address)
				: _address(reinterpret_cast<uintptr_t>(address))
			{
			}

			auto operator=(uintptr_t address) -> Address
			{
				_address = address;
				return *this;
			}

			auto operator=(void* address) -> Address
			{
				_address = reinterpret_cast<uintptr_t>(address);
				return *this;
			}

			auto operator+(uintptr_t offset) -> Address
			{
				return Address(_address + offset);
			}

			bool operator>(uintptr_t offset)
			{
				return _address > offset;
			}

			bool operator>(Address address)
			{
				return _address > address._address;
			}

			bool operator<(uintptr_t offset)
			{
				return _address < offset;
			}

			bool operator<(Address address)
			{
				return _address < address._address;
			}

			bool operator>=(uintptr_t offset)
			{
				return _address >= offset;
			}

			bool operator>=(Address address)
			{
				return _address >= address._address;
			}

			bool operator<=(uintptr_t offset)
			{
				return _address <= offset;
			}

			bool operator<=(Address address)
			{
				return _address <= address._address;
			}

			bool operator==(uintptr_t offset)
			{
				return _address == offset;
			}

			bool operator==(Address address)
			{
				return _address == address._address;
			}

			bool operator!=(uintptr_t offset)
			{
				return _address != offset;
			}

			bool operator!=(Address address)
			{
				return _address != address._address;
			}

			auto RelativeOffset(uint32_t offset, uint32_t off2 = 0) -> Address
			{
				if (_address) _address = ((_address + offset + 4 + off2) + *(int32_t*)(_address + offset));
				return *this;
			}

			auto AbsoluteOffset(uint32_t offset) -> Address
			{
				_address = _address + offset;
				return *this;
			}

			auto Jump() -> Address
			{
				if (ASM::isJump(*reinterpret_cast<UINT8*>(_address)))
				{
					UINT8 toSkip = *reinterpret_cast<UINT8*>(_address + 1);
					_address = _address + 2 + toSkip;
				}

				return *this;
			}

			auto Get() -> uintptr_t
			{
				return _address;
			}

			template <typename T>
			auto GetAs() -> T
			{
				return reinterpret_cast<T>(_address);
			}

			auto IsValid() -> bool
			{
				return _address != 0;
			}
		};

		class Section
		{
		public:
			std::string sectionName;
			IMAGE_SECTION_HEADER rawSection;

			static auto GetAllSections() -> std::vector<Section>
			{
				std::vector<Section> sections;

				auto sectionsSize = GetNTHeaders()->FileHeader.NumberOfSections;
				auto section = IMAGE_FIRST_SECTION(GetNTHeaders());

				for (WORD i = 0; i < sectionsSize; i++, section++)
				{
					auto secName = std::string((char*)section->Name);

					sections.push_back({ secName, *section });
				}

				return sections;
			}

			static auto GetSection(std::string sectionName) -> Section
			{
				for (auto& section : GetAllSections())
				{
					if (section.sectionName == sectionName)
					{
						return section;
					}
				}

				MemcuryThrow("Section not found");
				return Section{};
			}

			auto GetSectionSize() -> uint32_t
			{
				return rawSection.Misc.VirtualSize;
			}

			auto GetSectionStart() -> Address
			{
				return Address(GetModuleBase() + rawSection.VirtualAddress);
			}

			auto GetSectionEnd() -> Address
			{
				return Address(GetSectionStart() + GetSectionSize());
			}

			auto isInSection(Address address) -> bool
			{
				return address >= GetSectionStart() && address < GetSectionEnd();
			}
		};
	}

	class Scanner
	{
		PE::Address _address;

	public:
		Scanner(PE::Address address)
			: _address(address)
		{
		}

		static auto SetTargetModule(const char* moduleName) -> void
		{
			PE::SetCurrentModule(moduleName);
		}


		static auto FindPointerRef(void* Pointer, int useRefNum = 0, bool bUseFirstResult = false, bool bWarnIfNotFound = true) -> Scanner // credit me and ender
		{
			PE::Address add{ nullptr };

			if (!Pointer)
				return Scanner(add);

			auto textSection = PE::Section::GetSection(".text");

			const auto scanBytes = reinterpret_cast<std::uint8_t*>(textSection.GetSectionStart().Get());

			int aa = 0;

			// scan only text section
			for (DWORD i = 0x0; i < textSection.GetSectionSize(); i++)
			{
				if ((scanBytes[i] == ASM::CMOVL || scanBytes[i] == ASM::CMOVS) && (scanBytes[i + 1] == ASM::LEA || scanBytes[i + 1] == 0x8B))
				{
					if (PE::Address(&scanBytes[i]).RelativeOffset(3).GetAs<void*>() == Pointer)
					{
						add = PE::Address(&scanBytes[i]);

						// LOG_INFO(LogDev, "2add: 0x{:x}", add.Get() - __int64(GetModuleHandleW(0)));

						if (bUseFirstResult)
							return Scanner(add);

						/* if (++aa > useRefNum)
							break; */
					}
				}

				if (scanBytes[i] == ASM::CALL)
				{
					if (PE::Address(&scanBytes[i]).RelativeOffset(1).GetAs<void*>() == Pointer)
					{
						add = PE::Address(&scanBytes[i]);

						// LOG_INFO(LogDev, "1add: 0x{:x}", add.Get() - __int64(GetModuleHandleW(0)));

						if (bUseFirstResult)
							return Scanner(add);

						/* if (++aa > useRefNum)
							break; */
					}
				}
			}

			if (bWarnIfNotFound)
			{
				if (add == 0)
				{
					// MessageBoxA(0, "FindPointerRef return nullptr", "Memcury", MB_OK);
				}
				else
				{
					// MessageBoxA(0, std::format("FindPointerRef return 0x{:x}", add.Get() - __int64(GetModuleHandleW(0))).c_str(), "Memcury", MB_OK);
				}
			}

			return Scanner(add);
		}

		// Supports wide and normal strings both std and pointers
		template <typename T = const wchar_t*>
		static auto FindStringRef(T string, bool bWarnIfNotFound = true, int useRefNum = 0, bool bIsInFunc = false, bool bSkunky = false) -> Scanner
		{
			PE::Address add{ nullptr };

			constexpr auto bIsWide = std::is_same<T, const wchar_t*>::value;
			constexpr auto bIsChar = std::is_same<T, const char*>::value;

			constexpr auto bIsPtr = bIsWide || bIsChar;

			auto textSection = PE::Section::GetSection(".text");
			auto rdataSection = PE::Section::GetSection(".rdata");

			const auto scanBytes = reinterpret_cast<std::uint8_t*>(textSection.GetSectionStart().Get());

			int aa = 0;

			__m128i t = _mm_set1_epi8(0x48);
			__m128i s = _mm_set1_epi8((char)0xfb);
			// scan only text section
			DWORD i = 0x0;
			for (; i < textSection.GetSectionSize() - (textSection.GetSectionSize() % 16); i += 16)
			{
				auto bytes = _mm_load_si128((const __m128i*)(scanBytes + i));
				__m128i masked = _mm_and_si128(bytes, s);
				int offset = _mm_movemask_epi8(_mm_cmpeq_epi8(masked, t));

				if (offset == 0)
					continue;

				for (int q = 0; q < 16; q++)
				{
					int c = offset & (1 << q);
					if (c)
					{
						if (scanBytes[i + q + 1] == ASM::LEA)
						{
							auto stringAdd = PE::Address(&scanBytes[i + q]).RelativeOffset(3);

							// Check if the string is in the .rdata section
							if (rdataSection.isInSection(stringAdd))
							{
								auto strBytes = stringAdd.GetAs<std::uint8_t*>();

								// Check if the first char is printable
								if (ASM::byteIsAscii(strBytes[0]))
								{
									if constexpr (!bIsPtr)
									{
										// typedef T::value_type char_type;
										using char_type = std::decay_t<std::remove_pointer_t<T>>;

										auto lea = stringAdd.GetAs<char_type*>();

										T leaT(lea);

										if (leaT == string)
										{
											add = PE::Address(&scanBytes[i + q]);

											if (++aa > useRefNum)
												goto _out;
										}
									}
									else
									{
										auto lea = stringAdd.GetAs<T>();

										if constexpr (bIsWide)
										{
											if (wcscmp(string, lea) == 0)
											{
												add = PE::Address(&scanBytes[i + q]);

												if (++aa > useRefNum)
													goto _out;
											}
										}
										else
										{
											if (strcmp(string, lea) == 0)
											{
												add = PE::Address(&scanBytes[i + q]);

												if (++aa > useRefNum)
													goto _out;
											}
										}
									}
								}
							}
						}
					}
				}
			}
			for (; i < textSection.GetSectionSize(); i++)
			{
				if ((scanBytes[i] == ASM::CMOVL || scanBytes[i] == ASM::CMOVS) && scanBytes[i + 1] == ASM::LEA)
				{
					auto stringAdd = PE::Address(&scanBytes[i]).RelativeOffset(3);

					// Check if the string is in the .rdata section
					if (rdataSection.isInSection(stringAdd))
					{
						auto strBytes = stringAdd.GetAs<std::uint8_t*>();

						// Check if the first char is printable
						if (ASM::byteIsAscii(strBytes[0]))
						{
							if constexpr (!bIsPtr)
							{
								// typedef T::value_type char_type;
								using char_type = std::decay_t<std::remove_pointer_t<T>>;

								auto lea = stringAdd.GetAs<char_type*>();

								T leaT(lea);

								if (leaT == string)
								{
									add = PE::Address(&scanBytes[i]);

									if (++aa > useRefNum)
										break;
								}
							}
							else
							{
								auto lea = stringAdd.GetAs<T>();

								if constexpr (bIsWide)
								{
									if (wcscmp(string, lea) == 0)
									{
										add = PE::Address(&scanBytes[i]);

										if (++aa > useRefNum)
											break;
									}
								}
								else
								{
									if (strcmp(string, lea) == 0)
									{
										add = PE::Address(&scanBytes[i]);

										if (++aa > useRefNum)
											break;
									}
								}
							}
						}
					}
				}
			}
		_out:

			// MemcuryAssertM(add != 0, "FindStringRef return nullptr");

			if (bWarnIfNotFound)
			{
				if (add == 0)
				{
					if constexpr (bIsWide)
					{
						//std::wstring wstr = std::wstring(string);

						// auto aaa = (L"failed FindStringRef " + std::wstring(string));
						// MessageBoxA(0, std::string(aaa.begin(), aaa.end()).c_str(), "Memcury", MB_ICONERROR);
					}
					else
					{
						// MessageBoxA(0, ("failed FindStringRef " + std::string(string)).c_str(), "Memcury", MB_ICONERROR);
					}
				}
			}

			if (add.Get())
			{
				if (bIsInFunc)
				{
					for (int i = 0; i < 300; i++)
					{
						if (!bSkunky ? (*(uint8_t*)(add.Get() - i) == 0x48 && *(uint8_t*)(add.Get() - i + 1) == 0x83) :
							(*(uint8_t*)(add.Get() - i) == 0x4C && *(uint8_t*)(add.Get() - i + 1) == 0x8B && *(uint8_t*)(add.Get() - i + 2) == 0xDC))
						{
							// MessageBoxA(0, std::format("0x{:x}", (__int64(add.Get() - i) - __int64(GetModuleHandleW(0)))).c_str(), "Memcury", MB_OK);

							auto beginFunc = Scanner(add.Get() - i);

							auto ref = FindPointerRef(beginFunc.GetAs<void*>());

							return ref;
						}
					}
				}
			}

			return Scanner(add);
		}

		auto Jump() -> Scanner
		{
			_address.Jump();
			return *this;
		}

		inline auto ScanFor(std::vector<uint8_t> opcodesToFind, bool forward = true, int toSkip = 0, int bytesToSkip = 1, int Radius = 2048) -> Scanner
		{
			const auto scanBytes = _address.GetAs<std::uint8_t*>();
			if (!scanBytes) return *this;

			bool bFound = false;

			for (auto i = (forward ? bytesToSkip : -bytesToSkip); forward ? (i < Radius + bytesToSkip) : (i > -Radius - bytesToSkip); forward ? i++ : i--)
			{
				bool found = true;

				for (int k = 0; k < opcodesToFind.size() && found; k++)
				{
					auto& currentOpcode = opcodesToFind[k];

					// LOG_INFO(LogDev, "[{} 0x{:x}] 0x{:x}", i, __int64(&scanBytes[i]) - __int64(GetModuleHandleW(0)), currentOpcode);

					found = currentOpcode == scanBytes[i + k];
				}

				if (found)
				{
					_address = &scanBytes[i];
					if (toSkip != 0)
					{
						return ScanFor(opcodesToFind, forward, toSkip - 1);
					}

					bFound = true;

					break;
				}
			}

			if (!bFound)
			{
			}

			return *this;
		}

		inline auto ScanFor(const char* pattern, bool forward = true, int toSkip = 0, int bytesToSkip = 1, int Radius = 2048) -> Scanner
		{
			const auto scanBytes = _address.GetAs<std::uint8_t*>();

			bool bFound = false;

			auto opcodesToFind = ASM::pattern2bytes(pattern);
			for (auto i = (forward ? bytesToSkip : -bytesToSkip); forward ? (i < Radius + bytesToSkip) : (i > -Radius - bytesToSkip); forward ? i++ : i--)
			{
				bool found = true;

				for (int k = 0; k < opcodesToFind.size() && found; k++)
				{
					auto& currentOpcode = opcodesToFind[k];

					if (currentOpcode == -1)
						continue;

					// LOG_INFO(LogDev, "[{} 0x{:x}] 0x{:x}", i, __int64(&scanBytes[i]) - __int64(GetModuleHandleW(0)), currentOpcode);

					found = currentOpcode == scanBytes[i + k];
				}

				if (found)
				{
					_address = &scanBytes[i];
					if (toSkip != 0)
					{
						return ScanFor(pattern, forward, toSkip - 1);
					}

					bFound = true;

					break;
				}
			}

			if (!bFound)
			{
			}

			return *this;
		}

		auto FindFunctionBoundary(bool forward = false) -> Scanner
		{
			const auto scanBytes = _address.GetAs<std::uint8_t*>();

			for (auto i = (forward ? 1 : -1); forward ? (i < 2048) : (i > -2048); forward ? i++ : i--)
			{
				if ( // ASM::byteIsA(scanBytes[i], ASM::MNEMONIC::JMP_REL8) ||
					// ASM::byteIsA(scanBytes[i], ASM::MNEMONIC::JMP_REL32) ||
					// ASM::byteIsA(scanBytes[i], ASM::MNEMONIC::JMP_EAX) ||
					ASM::byteIsA(scanBytes[i], ASM::MNEMONIC::RETN_REL8) || ASM::byteIsA(scanBytes[i], ASM::MNEMONIC::RETN) || ASM::byteIsA(scanBytes[i], ASM::MNEMONIC::INT3))
				{
					_address = (uintptr_t)&scanBytes[i + 1];
					break;
				}
			}

			return *this;
		}

		auto RelativeOffset(uint32_t offset, uint32_t off2 = 0) -> Scanner
		{
			if (!_address.Get())
			{
				return *this;
			}

			_address.RelativeOffset(offset, off2);

			return *this;
		}

		auto AbsoluteOffset(uint32_t offset) -> Scanner
		{
			_address.AbsoluteOffset(offset);

			return *this;
		}

		template <typename T>
		auto GetAs() -> T
		{
			return _address.GetAs<T>();
		}

		auto Get() -> uintptr_t
		{
			return _address.Get();
		}

		auto IsValid() -> bool
		{
			return _address.IsValid();
		}
	};

}

inline bool IsBadReadPtr(void* p)
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	if (::VirtualQuery(p, &mbi, sizeof(mbi)))
	{
		DWORD mask = (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
		bool b = !(mbi.Protect & mask);
		// check the page is not a guard page
		if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) b = true;

		return b;
	}
	return true;
}

static void VirtualSwap(void** VTable, int Idx, void* NewFunc)
{
	DWORD dwProtection;
	VirtualProtect(&VTable[Idx], 8, PAGE_EXECUTE_READWRITE, &dwProtection);

	VTable[Idx] = NewFunc;

	DWORD dwTemp;
	VirtualProtect(&VTable[Idx], 8, dwProtection, &dwTemp);
}

inline uintptr_t FindNameRef(const wchar_t* Name, int skip = 0, bool bWarnStringNotFound = true)
{
	auto StringRef = Memcury::Scanner::FindStringRef(Name, true, skip);

	if (!StringRef.Get())
		return 0;

	auto FunctionPtr = StringRef.ScanFor({ 0x48, 0x8D, 0x0D }).RelativeOffset(3).GetAs<void*>();

	auto PtrRef = Memcury::Scanner::FindPointerRef(FunctionPtr);

	return PtrRef.Get();
}

// Finds a string ref, then goes searches xref of the function that it's in and returns that address.
inline uintptr_t FindFunctionCall(const wchar_t* Name, const std::vector<uint8_t>& Bytes = std::vector<uint8_t>{ 0x48, 0x89, 0x5C }, int skip = 0, bool bWarnStringNotFound = true) // credit ender & me
{
	auto NameRef = FindNameRef(Name, skip, bWarnStringNotFound);

	if (!NameRef)
		return 0;

	return Memcury::Scanner(NameRef).ScanFor(Bytes, false).Get();
}

inline bool IsNullSub(uint64_t Addr)
{
	// if (*(uint8_t*)(Addr) == 0xEB && *(uint8_t*)(Addr + 1) == 0xF7) // positive sp value has been detected, the output may be wrong!
	   // return true;

	return *(uint8_t*)(Addr) == 0xC3 || *(uint8_t*)(Addr) == 0xC2;
}