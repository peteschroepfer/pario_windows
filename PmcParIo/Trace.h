#pragma once
/*++
Copyright (c) 2015 Dynamic Engineering All rights reserved.
   150 DuBois, Suite C
   Santa Cruz, CA 95060
   831-457-8891
   engineering@dyneng.com
   http://www.dyneng.com

Module Name:
   PmcParIoTrace.h

Abstract:
   Header file for the debug tracing related function defintions and macros.

Environment:
   Kernel mode
--*/



//
// Define the tracing flags.
//
// Tracing GUID - c7f2d697-f524-44b5-9535-420cd7aa139d
//

#define WPP_CONTROL_GUIDS                                              \
    WPP_DEFINE_CONTROL_GUID(                                           \
        PARIOWIN10TraceGuid, (c7f2d697,f524,44b5,9535,420cd7aa139d), \
                                                                            \
      WPP_DEFINE_BIT(DBG_INIT)             /* bit  0 = 0x00000001 */ \
      WPP_DEFINE_BIT(DBG_PNP)              /* bit  1 = 0x00000002 */ \
      WPP_DEFINE_BIT(DBG_POWER)            /* bit  2 = 0x00000004 */ \
      WPP_DEFINE_BIT(DBG_WMI)              /* bit  3 = 0x00000008 */ \
      WPP_DEFINE_BIT(DBG_CREATE_CLOSE)     /* bit  4 = 0x00000010 */ \
      WPP_DEFINE_BIT(DBG_IOCTLS)           /* bit  5 = 0x00000020 */ \
      WPP_DEFINE_BIT(DBG_WRITE)            /* bit  6 = 0x00000040 */ \
      WPP_DEFINE_BIT(DBG_READ)             /* bit  7 = 0x00000080 */ \
      WPP_DEFINE_BIT(DBG_DPC)              /* bit  8 = 0x00000100 */ \
      WPP_DEFINE_BIT(DBG_INTERRUPT)        /* bit  9 = 0x00000200 */ \
      WPP_DEFINE_BIT(DBG_LOCKS)            /* bit 10 = 0x00000400 */ \
      WPP_DEFINE_BIT(DBG_QUEUEING)         /* bit 11 = 0x00000800 */ \
      WPP_DEFINE_BIT(DBG_HW_ACCESS)        /* bit 12 = 0x00001000 */ \
      WPP_DEFINE_BIT(DBG_PETE)                                       \
        )                             

#define WPP_FLAG_LEVEL_LOGGER(flag, level)                                  \
    WPP_LEVEL_LOGGER(flag)

#define WPP_FLAG_LEVEL_ENABLED(flag, level)                                 \
    (WPP_LEVEL_ENABLED(flag) &&                                             \
     WPP_CONTROL(WPP_BIT_ ## flag).Level >= level)

#define WPP_LEVEL_FLAGS_LOGGER(lvl,flags) \
           WPP_LEVEL_LOGGER(flags)

#define WPP_LEVEL_FLAGS_ENABLED(lvl, flags) \
           (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)

//           
// WPP orders static parameters before dynamic parameters. To support the Trace function
// defined below which sets FLAGS=MYDRIVER_ALL_INFO, a custom macro must be defined to
// reorder the arguments to what the .tpl configuration file expects.
//
#define WPP_RECORDER_FLAGS_LEVEL_ARGS(flags, lvl) WPP_RECORDER_LEVEL_FLAGS_ARGS(lvl, flags)
#define WPP_RECORDER_FLAGS_LEVEL_FILTER(flags, lvl) WPP_RECORDER_LEVEL_FLAGS_FILTER(lvl, flags)

//
// This comment block is scanned by the trace preprocessor to define our
// Trace function.
//
// begin_wpp config
// FUNC Trace{FLAGS=MYDRIVER_ALL_INFO}(LEVEL, MSG, ...);
// FUNC TraceEvents(LEVEL, FLAGS, MSG, ...);
// end_wpp
//
