#pragma once
// pretty much dont touch this unless you want errors to stack :D

#include "custom.h"
#include "globals.h"
#include "misc/nocrt.h"
#include "misc/logger.h" // [NEW] Logger

#include "mem/sigscan.h"
#include "mem/memcum.h"

#include "math/vector.h" // [NEW] Math
#include "ue4/core.h"    // [NEW] UE4

#include "prot/xor.h"
#include "prot/spoof_call.h"
#include "windows/defs.h"
#include "windows/windows.h"

#include "asm/asm.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
