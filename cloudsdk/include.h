#pragma once
// pretty much dont touch this unless you want errors to stack :D

#include "custom.h"
#include "globals.h"
#include "misc/nocrt.h"
#include "mem/sigscan.h"
#include "mem/memcum.h"

#include "prot/xor.h"
#include "windows/defs.h"
#include "windows/windows.h"


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
