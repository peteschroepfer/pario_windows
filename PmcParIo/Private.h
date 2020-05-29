#pragma once
/*++
Copyright (c) 2015 Dynamic Engineering All rights reserved.
   150 DuBois, Suite C
   Santa Cruz, CA 95060
   831-457-8891
   engineering@dyneng.com
   http://www.dyneng.com

Module Name:
   PmcParIoPrivate.h

Abstract: Definition of device extension structure and function prototypes.

Environment:
   Kernel mode
--*/


// The device extension for the device object
typedef struct _DEVICE_EXTENSION {
    WDFDEVICE         Device;
    WDFREQUEST        CurrentRequest;
    PDEVICE_OBJECT    Pdo;
    PHYSICAL_ADDRESS        BaseAddress;
    // Following fields are specific to the hardware configuration
    // HW Resources
    PPMC_PAR_IO_REGS  Regs;          // Control/Status Register structure
    PULONG            RegsBase;      // Registers base address
    ULONG             RegsLength;    // Registers base length
    PPCI_9052_REGS    PlxRegs;       // Plx Pci-9052 Control/Status Register structure
    PULONG            PlxRegsBase;   // Plx Pci-9052 Registers base address
    ULONG             PlxRegsLength; // Plx Pci-9052 Registers base length
    WDFINTERRUPT      Interrupt;     // Returned by InterruptCreate
    WDFSPINLOCK       IntSpinLock;
    CLK_CNTRL_REG     ClockCntrl;
    INT_REG_REG       IntConfig;
    INT_PLX_REG       PlxIntContStat;
    INT_PLX_REG       IsrStatus;
    ULONG             DataOutConfig0;
    ULONG             DataOutConfig1;
    UNICODE_STRING    FileName;
    HANDLE            FileHandle;
    WDFWORKITEM       Worker;
    WDFFILEOBJECT     XsvfFile;
    PUCHAR            pBuffer;       // Buffer to hold XSVF file contents

  // IOCTL
    WDFQUEUE          DevCtlQueue;
    ULONG             instance;
    UCHAR             dsgn_id;       // Xilinx design ID
    UCHAR             dsgn_rev;      // Xilinx design revision
    UCHAR             minr_rev;      // Xilinx minor revision
    PKEVENT           pEventObject;  // Pointer to the interrupt synchronization event object
    ULONG             DataGenCntrl;

} DEVICE_EXTENSION, * PDEVICE_EXTENSION;

// This will generate the function named PmcParIoGetDeviceContext to be used
//  for retreiving the DEVICE_EXTENSION pointer.
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_EXTENSION, PmcParIoGetDeviceContext)

/* Workitem context used in dispatching *.bit-file read to system worker
    thread to be executed at PASSIVE_LEVEL. */
    typedef struct _WORKITEM_CONTEXT {
    PDEVICE_EXTENSION pDevExt;
    PUNICODE_STRING   FileName;
    WDFREQUEST        Request;
} WORKITEM_CONTEXT, * PWORKITEM_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WORKITEM_CONTEXT, PmcParIoGetWorkItemContext)

// Current WDF driver version - unsigned char Ver - format as two hex digits
//   (major.minor -> (Ver >> 4 & 0x0f) + 1 = major, Ver & 0x0f = minor)
//#define PMC_PAR_IO_DRIVER_VERSION      0  // 07/25/17 0->Version 1.0, 1->1.1, 16->2.0, etc.
#define PMC_PAR_IO_DRIVER_VERSION      1  // 07/28/17 Fixed interrupt issues.
#define MAX_ID_LEN                     80
#define PCI_64_LOCAL_RAM_ADDRESS_SPACE 0x00000080  // Pci9052 local register address space
#define PMC_PAR_IO_LOCAL_ADDRESS_SPACE 0x100000  // Local address space length = 1Mb
#define PMC_PAR_IO_POOL_TAG            'XMDP'

 // Function prototypes
    DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD PmcParIoEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP PmcParIoEvtDriverContextCleanup;
EVT_WDF_DEVICE_D0_ENTRY PmcParIoEvtDeviceD0Entry;
EVT_WDF_DEVICE_D0_EXIT PmcParIoEvtDeviceD0Exit;
EVT_WDF_DEVICE_PREPARE_HARDWARE PmcParIoEvtDevicePrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE PmcParIoEvtDeviceReleaseHardware;
EVT_WDF_IO_QUEUE_IO_READ PmcParIoEvtIoRead;
EVT_WDF_IO_QUEUE_IO_WRITE PmcParIoEvtIoWrite;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL PmcParIoEvtIoDeviceControl;
EVT_WDF_INTERRUPT_ISR PmcParIoEvtInterruptIsr;
EVT_WDF_INTERRUPT_DPC PmcParIoEvtInterruptDpc;
EVT_WDF_INTERRUPT_ENABLE PmcParIoEvtInterruptEnable;
EVT_WDF_INTERRUPT_DISABLE PmcParIoEvtInterruptDisable;
EVT_WDF_WORKITEM PmcParIoXsvfWorkItem;
/*VOID
PmcParIoXsvfWorkItem(
   __in WDFWORKITEM WorkItem
   )*/
NTSTATUS PmcParIoSetIdleAndWakeSettings(__in PDEVICE_EXTENSION FdoData);
NTSTATUS PmcParIoInitializeDeviceExtension(__in PDEVICE_EXTENSION pDevExt);
NTSTATUS PmcParIoPrepareHardware(__in PDEVICE_EXTENSION pDevExt, __in WDFCMRESLIST ResourcesTranslated);
// WDFINTERRUPT Support
NTSTATUS PmcParIoInterruptCreate(__in PDEVICE_EXTENSION pDevExt);
// Hardware init/shutdown
NTSTATUS PmcParIoInitializeHardware(__in PDEVICE_EXTENSION pDevExt);
VOID     PmcParIoShutdown(__in PDEVICE_EXTENSION pDevExt);
VOID     PmcParIoHardwareReset(__in PDEVICE_EXTENSION pDevExt);
// IOCTL functions
NTSTATUS GetInfo(__in PDEVICE_EXTENSION pDevExt, __out PPMC_PAR_IO_DRIVER_DEVICE_INFO pPmcParIoInfo);
NTSTATUS SetOutData(__in PDEVICE_EXTENSION pDevExt, __in PPMC_PAR_IO_DATA pPmcParIoData);
NTSTATUS GetOutData(__in PDEVICE_EXTENSION pDevExt, __out PPMC_PAR_IO_DATA pPmcParIoData);
NTSTATUS ReadInData(__in PDEVICE_EXTENSION pDevExt, __out PPMC_PAR_IO_DATA pPmcParIoData);
NTSTATUS SetClockConfig(__in PDEVICE_EXTENSION pDevExt, __in PPMC_PAR_IO_CLOCK_CONFIG pPmcParIoClk);
NTSTATUS GetClockConfig(__in PDEVICE_EXTENSION pDevExt, __out PPMC_PAR_IO_CLOCK_CONFIG pPmcParIoClk);
NTSTATUS SetIntConfig(__in PDEVICE_EXTENSION pDevExt, __in PPMC_PAR_IO_INT_CONFIG pPmcParIoInt);
NTSTATUS GetIntConfig(__in PDEVICE_EXTENSION pDevExt, __out PPMC_PAR_IO_INT_CONFIG pPmcParIoInt);
NTSTATUS GetIntStatus(__in PDEVICE_EXTENSION pDevExt, __out PULONG pPmcParIoStatus);
NTSTATUS RegisterEvent(__in PDEVICE_EXTENSION pDevExt, __in PHANDLE pEvent);
NTSTATUS EnableInterrupt(__in PDEVICE_EXTENSION pDevExt);
NTSTATUS DisableInterrupt(__in PDEVICE_EXTENSION pDevExt);
NTSTATUS ForceInterrupt(__in PDEVICE_EXTENSION pDevExt);
NTSTATUS GetIsrStatus(__in PDEVICE_EXTENSION pDevExt, __out PULONG pPmcParIoIsrStat);

#pragma warning(disable:4127) // avoid conditional expression is constant error with W4

