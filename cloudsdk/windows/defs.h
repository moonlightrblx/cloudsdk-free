#pragma once

#include <windows.h>
#include <winnt.h>
#include <winternl.h>

// defs for the a bunch of real structs cause their not in the windows sdk for some reason
// zero clue why, but its microsoft so obviously make it as hard as possible
// at least its not mac lmao

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// custom defs
#define VK_KB_W 0x57
#define VK_KB_A 0x41
#define VK_KB_B 0x42
#define VK_KB_C 0x43
#define VK_KB_D 0x44
#define VK_KB_E 0x45
#define VK_KB_F 0x46
#define VK_KB_G 0x47
#define VK_KB_H 0x48
#define VK_KB_I 0x49
#define VK_KB_J 0x4A
#define VK_KB_K 0x4B
#define VK_KB_L 0x4C
#define VK_KB_M 0x4D
#define VK_KB_N 0x4E
#define VK_KB_O 0x4F
#define VK_KB_P 0x50
#define VK_KB_Q 0x51
#define VK_KB_R 0x52
#define VK_KB_S 0x53
#define VK_KB_T 0x54
#define VK_KB_U 0x55
#define VK_KB_V 0x56
#define VK_KB_W 0x57
#define VK_KB_X 0x58
#define VK_KB_Y 0x59
#define VK_KB_Z 0x5A


// windows defs
namespace windef {
	// https://ntdoc.m417z.com/
	// very useful resource for structs / function defs :D

	namespace funcs {
		typedef NTSTATUS(NTAPI* NtSetInformationProcess)(HANDLE ProcessHandle, PROCESS_INFORMATION_CLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength);
		typedef NTSTATUS(NTAPI* NtQueryInformationThread)(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength, PULONG ReturnLength);
		typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
		typedef NTSTATUS(NTAPI* NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
		typedef NTSTATUS(NTAPI* RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
	}

	typedef struct _ACTIVATION_CONTEXT_DATA* PACTIVATION_CONTEXT_DATA;

	// https://github.com/winsiderss/systeminformer/blob/addd49805cb586a891f7c6f31d88a43545cc1c80/phnt/include/ntsxs.h#L36
	// https://github.com/winsiderss/systeminformer/blob/addd49805cb586a891f7c6f31d88a43545cc1c80/phnt/include/ntsxs.h#L445
	typedef struct _ASSEMBLY_STORAGE_MAP_ENTRY {
		ULONG Flags;
		UNICODE_STRING DosPath;
		HANDLE Handle;
	} ASSEMBLY_STORAGE_MAP_ENTRY, * PASSEMBLY_STORAGE_MAP_ENTRY;

	typedef struct _ASSEMBLY_STORAGE_MAP {
		ULONG Flags;
		ULONG AssemblyCount;
		PASSEMBLY_STORAGE_MAP_ENTRY* AssemblyArray;
	} ASSEMBLY_STORAGE_MAP, * PASSEMBLY_STORAGE_MAP;

	typedef struct R_PEB {
		// https://github.com/winsiderss/systeminformer/blob/master/phnt/include/ntpebteb.h#L346 
		//  its now documented better but its the same struct just make sure to change it to R_ and RPPEB
		//  if we ever start crashing look at the repo above
		BOOLEAN InheritedAddressSpace;
		BOOLEAN ReadImageFileExecOptions;
		BOOLEAN BeingDebugged;
		union
		{
			BOOLEAN BitField;
			struct
			{
				BOOLEAN ImageUsesLargePages : 1;
				BOOLEAN IsProtectedProcess : 1;
				BOOLEAN IsImageDynamicallyRelocated : 1;
				BOOLEAN SkipPatchingUser32Forwarders : 1;
				BOOLEAN IsPackagedProcess : 1;
				BOOLEAN IsAppContainer : 1;
				BOOLEAN IsProtectedProcessLight : 1;
				BOOLEAN IsLongPathAwareProcess : 1;
			};
		};

		HANDLE Mutant;
		PVOID ImageBaseAddress;
		PPEB_LDR_DATA Ldr;
		void* ProcessParameters;
		PVOID SubSystemData;
		PVOID ProcessHeap;
		void* FastPebLock;
		void* AtlThunkSListPtr;
		PVOID IFEOKey;

		union
		{
			ULONG CrossProcessFlags;
			struct
			{
				ULONG ProcessInJob : 1;
				ULONG ProcessInitializing : 1;
				ULONG ProcessUsingVEH : 1;
				ULONG ProcessUsingVCH : 1;
				ULONG ProcessUsingFTH : 1;
				ULONG ProcessPreviouslyThrottled : 1;
				ULONG ProcessCurrentlyThrottled : 1;
				ULONG ProcessImagesHotPatched : 1; // RS5
				ULONG ReservedBits0 : 24;
			};
		};
		union
		{
			PVOID KernelCallbackTable;
			PVOID UserSharedInfoPtr;
		};
		ULONG SystemReserved;
		ULONG AtlThunkSListPtr32;
		PVOID ApiSetMap;
		ULONG TlsExpansionCounter;
		PVOID TlsBitmap;
		ULONG TlsBitmapBits[2];

		PVOID ReadOnlySharedMemoryBase;
		struct _SILO_USER_SHARED_DATA* SharedData;
		PVOID* ReadOnlyStaticServerData;

		PVOID AnsiCodePageData;
		PVOID OemCodePageData;
		PVOID UnicodeCaseTableData;

		ULONG NumberOfProcessors;
		ULONG NtGlobalFlag;

		void* CriticalSectionTimeout;
		size_t HeapSegmentReserve;
		size_t HeapSegmentCommit;
		size_t HeapDeCommitTotalFreeThreshold;
		size_t HeapDeCommitFreeBlockThreshold;

		ULONG NumberOfHeaps;
		ULONG MaximumNumberOfHeaps;
		PVOID* ProcessHeaps;

		PVOID GdiSharedHandleTable;
		PVOID ProcessStarterHelper;
		ULONG GdiDCAttributeList;

		void* LoaderLock;

		ULONG OSMajorVersion;
		ULONG OSMinorVersion;
		unsigned short OSBuildNumber;
		unsigned short OSCSDVersion;
		ULONG OSPlatformId;
		ULONG ImageSubsystem;
		ULONG ImageSubsystemMajorVersion;
		ULONG ImageSubsystemMinorVersion;
		void* ActiveProcessAffinityMask;
		void* GdiHandleBuffer;
		PVOID PostProcessInitRoutine;

		PVOID TlsExpansionBitmap;
		ULONG TlsExpansionBitmapBits[32];

		ULONG SessionId;

		void* AppCompatFlags;
		void* AppCompatFlagsUser;
		PVOID pShimData;
		PVOID AppCompatInfo;

		void* CSDVersion;

		PACTIVATION_CONTEXT_DATA ActivationContextData;
		PASSEMBLY_STORAGE_MAP ProcessAssemblyStorageMap;
		PACTIVATION_CONTEXT_DATA SystemDefaultActivationContextData;
		PASSEMBLY_STORAGE_MAP SystemAssemblyStorageMap;

		size_t MinimumStackCommit;

		PVOID SparePointers[2];
		PVOID PatchLoaderData;
		PVOID ChpeV2ProcessInfo;

		ULONG AppModelFeatureState;
		ULONG SpareUlongs[2];

		unsigned short ActiveCodePage;
		unsigned short OemCodePage;
		unsigned short UseCaseMapping;
		unsigned short UnusedNlsField;

		PVOID WerRegistrationData;
		PVOID WerShipAssertPtr;

		union
		{
			PVOID pContextData;
			PVOID pUnused;
			PVOID EcCodeBitMap;
		};

		PVOID pImageHeaderHash;
		union
		{
			ULONG TracingFlags;
			struct
			{
				ULONG HeapTracingEnabled : 1;
				ULONG CritSecTracingEnabled : 1;
				ULONG LibLoaderTracingEnabled : 1;
				ULONG SpareTracingBits : 29;
			};
		};
		ULONGLONG CsrServerReadOnlySharedMemoryBase;
		void* TppWorkerpListLock;
		LIST_ENTRY64 TppWorkerpList;
		PVOID WaitOnAddressHashTable[128];
		PVOID TelemetryCoverageHeader; // RS3
		ULONG CloudFileFlags;
		ULONG CloudFileDiagFlags; // RS4
		char PlaceholderCompatibilityMode;
		char PlaceholderCompatibilityModeReserved[7];
		struct _LEAP_SECOND_DATA* LeapSecondData; // RS5
		union
		{
			ULONG LeapSecondFlags;
			struct
			{
				ULONG SixtySecondEnabled : 1;
				ULONG Reserved : 31;
			};
		};
		ULONG NtGlobalFlag2;
		ULONGLONG ExtendedFeatureDisableMask; // since WIN11
	} RPEB, * RPPEB;

	namespace ntdll {
		enum NT_STATUS {
			STATUS_SUCCESS = 0x00000000,                // success
			STATUS_FLOAT_MULTIPLE_FAULTS_ = 0xC00002B4, // floating point multiple faults
			STATUS_FLOAT_MULTIPLE_TRAPS_ = 0xC00002B5,  // floating point multiple traps
			STATUS_INVALID_PARAMETER_ = 0xC000000D,     // general invalid parameter
			STATUS_INVALID_SYSTEM_SERVICE_ = 0xC0000017,// this is returned when syscall idx is invalid
		};
	}
}
