#pragma once
#define WIN9X_COMPAT_SPINLOCK
#include <ntddk.h>
#pragma warning(disable:4201) // nameless struct/union warning

#include <stdarg.h>
#include <wdf.h>
#include <ntstrsafe.h>

#pragma warning(default:4201)

#include <initguid.h>         // required for GUID definitions
#include <wdmguid.h>          // required for WMILIB_CONTEXT

#include "Trace.h"
#include "Reg.h"
#include "Public.h"
#include "Private.h"
#include "Device.h"