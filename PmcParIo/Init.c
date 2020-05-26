/*++
Copyright (c) 2015 Dynamic Engineering All rights reserved.
   150 DuBois, Suite C
   Santa Cruz, CA 95060
   831-457-8891
   engineering@dyneng.com
   http://www.dyneng.com

Module Name:
   PmcParIoInit.c

Abstract:
   Contains most of initialization functions

Environment:
   Kernel mode
--*/

#include "Precomp.h"
#include "Init.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, PmcParIoInitializeDeviceExtension)
#pragma alloc_text (PAGE, PmcParIoPrepareHardware)
#pragma alloc_text (PAGE, PmcParIoInitializeHardware)
#endif

CONST HANDLE _INVALID_FILE_HANDLE = (HANDLE)(0xFFFFFFFF);

/*++
PmcParIoInitializeDeviceExtension
Routine Description:
   This routine is called by EvtDeviceAdd. Here the device context is
   initialized and all the software resources required by the device are
   allocated.
Arguments:
   pDevExt - Pointer to the Device Extension
Return Value:
   NTSTATUS
--*/
NTSTATUS PmcParIoInitializeDeviceExtension(__in PDEVICE_EXTENSION pDevExt)
{
    NTSTATUS             status;
    static int           devNum = 0;
    WDF_IO_QUEUE_CONFIG  queueConfig;

    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP,
        "--> PmcParIoInitializeDeviceExtension");
    // Store device instance number
    pDevExt->instance = devNum++;
    // Create a new IO Queue for IRP_MJ_DEVICE_CONTROL requests in sequential mode.
    WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, WdfIoQueueDispatchSequential);
    queueConfig.EvtIoDeviceControl = PmcParIoEvtIoDeviceControl;
    status = WdfIoQueueCreate(pDevExt->Device,
        &queueConfig,
        WDF_NO_OBJECT_ATTRIBUTES,
        &pDevExt->DevCtlQueue);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
            "WdfIoQueueCreate failed: %!STATUS!", status);
        return status;
    }
    // Set the Device Control Queue forwarding for IRP_MJ_DEVICE_CONTROL requests.
    status = WdfDeviceConfigureRequestDispatching(pDevExt->Device,
        pDevExt->DevCtlQueue,
        WdfRequestTypeDeviceControl);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
            "DeviceConfigureRequestDispatching failed: %!STATUS!", status);
        return status;
    }

    // Create a WDFINTERRUPT object.
    status = PmcParIoInterruptCreate(pDevExt);
    if (!NT_SUCCESS(status)) {
        return status;
    }
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP,
        "<-- PmcParIoInitializeDeviceExtension %!STATUS!", status);
    return status;
}// PmcParIoInitializeDeviceExtension

/*++
PmcParIoPrepareHardware
Routine Description:
   Gets the HW resources assigned by the bus driver from the start-irp
   and maps it to system address space.
Arguments:
   pDevExt - Pointer to our DEVICE_EXTENSION
Return Value:
   NTSTATUS
--*/
NTSTATUS PmcParIoPrepareHardware(__in PDEVICE_EXTENSION pDevExt,
    __in WDFCMRESLIST      ResourcesTranslated)
{
    ULONG                            i, j=0;
    CHAR* bar;
    BOOLEAN                          foundRegs = FALSE;
    BOOLEAN                          foundPLXRegs = FALSE;
    //BOOLEAN                          foundtRegs = FALSE;
    PHYSICAL_ADDRESS                 regsBasePA = { 0 };
    ULONG                            regsLength = 0;    
    PHYSICAL_ADDRESS                 PlxRegsBasePA = { 0 };
    ULONG                            PlxRegsLength = 0;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR  desc;

    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "--> PmcParIoPrepareHardware");
    // Parse the resource list and save the resource information.
    for (i = 0; i < WdfCmResourceListGetCount(ResourcesTranslated); i++) {
        TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "Resource Translated item number: %d\n",i);
        desc = WdfCmResourceListGetDescriptor(ResourcesTranslated, i);
        if (!desc) {
            TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
                "WdfResourceCmGetDescriptor failed");
            return STATUS_DEVICE_CONFIGURATION_ERROR;
        }
        switch (desc->Type) {
        case CmResourceTypeMemory:
            bar = NULL;
            TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "main: %d, memory event %d\n", i, j);
            

            if (!foundPLXRegs && desc->u.Memory.Length == PCI_64_LOCAL_RAM_ADDRESS_SPACE) {
           
                PlxRegsBasePA = desc->u.Memory.Start;
                PlxRegsLength = desc->u.Memory.Length;
                foundPLXRegs = TRUE;
                TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "Bar 0: len: %d\n",PlxRegsLength);
                bar = "BAR0";

            }
            


            if (!foundRegs && desc->u.Memory.Length == PMC_PAR_IO_LOCAL_ADDRESS_SPACE) {
                regsBasePA = desc->u.Memory.Start;
                regsLength = desc->u.Memory.Length;
                foundRegs = TRUE;
                TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "Bar 1: len: %d\n", regsLength);
                bar = "BAR1";
            }

            break;
        default:
            // Ignore all other descriptors
            break;
        }
    }

    if (!foundPLXRegs) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
            "PmcParIoPrepareHardware: Missing resources");
        return STATUS_DEVICE_CONFIGURATION_ERROR;
    }
    // Map in the Registers Memory resource: BAR0
    pDevExt->PlxRegsBase = (PULONG)MmMapIoSpace(PlxRegsBasePA,
        PlxRegsLength,
        MmNonCached);    
   
    if (!pDevExt->PlxRegsBase) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
            " - Unable to map Registers memory %08I64X, length %d",
            PlxRegsBasePA.QuadPart, PlxRegsLength);
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    pDevExt->PlxRegsLength = PlxRegsLength;
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP,
        " - Registers %p, length %d",
        pDevExt->PlxRegsBase, pDevExt->PlxRegsLength);

    // Set separate pointer to PCI_9052_REGS structure
    pDevExt->PlxRegs = (PPCI_9052_REGS)pDevExt->PlxRegsBase;
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP,
        " - Registers %p, length %d",
        pDevExt->PlxRegs + PLX_LOC_ADDR_SPACE_0, pDevExt->PlxRegsLength);
    if (!foundRegs) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
            "PmcParIoPrepareHardware: Missing resources");
        return STATUS_DEVICE_CONFIGURATION_ERROR;
    }
    // Map in the Registers Memory resource: BAR1
    pDevExt->RegsBase = (PULONG)MmMapIoSpace(regsBasePA,
        regsLength, MmNonCached);
    if (!pDevExt->RegsBase) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
            " - Unable to map Registers memory %08I64X, length %d",
            regsBasePA.QuadPart, regsLength);
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    pDevExt->RegsLength = regsLength;
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP,
        " - Registers %p, length %d",
        pDevExt->RegsBase, pDevExt->RegsLength);

    // Set seperated pointer to PMC_PAR_IO_REGS structure.
    pDevExt->Regs = (PPMC_PAR_IO_REGS)pDevExt->RegsBase;
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP,
        " - Registers %p, length %d",
        pDevExt->RegsBase + PAR_IO_DATA_OUT_0, pDevExt->RegsLength);
    
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "<-- PmcParIoPrepareHardware");
    return STATUS_SUCCESS;
}// PmcParIoPrepareHardware

/*++
PmcParIoInitializeHardware
Routine Description:
   Initialize hardware resources such as PLL etc. Store relevant parameters
Arguments:
   pDevExt - Pointer to Device Extension
Return Value:
   NTSTATUS
--*/
NTSTATUS PmcParIoInitializeHardware(__in PDEVICE_EXTENSION pDevExt)
{

    ULONG       plx_config;

    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT,
        "--> PmcParIoInitializeHardware");
    // Initialize data members
    pDevExt->pEventObject = NULL;
    pDevExt->FileHandle = _INVALID_FILE_HANDLE;
    pDevExt->PlxIntContStat.val32 = 0;
    pDevExt->DataOutConfig0 = 0xffffffff;
    pDevExt->DataOutConfig1 = 0xffffffff;
    pDevExt->ClockCntrl.val32 = CLKEN_INTRN_ON;
    pDevExt->IntConfig.val32 = 0x0;

    // Set up the PLX local address space.
    WRITE_REGISTER_ULONG(pDevExt->PlxRegsBase + PLX_LOC_ADDR_SPACE_0, PLX_LOC_ADDR_EN);

    plx_config = PLX_LOC_RDY_EN | PLX_LOC_PF_CNTEN | PLX_LOC_MODE_32;
    plx_config |= 3 << PLX_LOC_XA_WT_SHFT & PLX_LOC_XA_WT_MSK;
    WRITE_REGISTER_ULONG(pDevExt->PlxRegsBase + PLX_LOC_ADD_SPACE_BRD_0, plx_config);

    plx_config = PLX_CS_ENABLE | PLX_CS_SIZE_256;
    WRITE_REGISTER_ULONG(pDevExt->PlxRegsBase + PLX_CHIP_SEL_BASE_0, plx_config);

    // Disable Plx interrupts
    WRITE_REGISTER_ULONG(pDevExt->PlxRegsBase + PLX_INT_CONT_STAT, pDevExt->PlxIntContStat.val32);

    // Configure local bus access behavior
    plx_config = PLX_RETRY_DLY_CNT << PLX_CNTRL_RTRYDL_SHFT & PLX_CNTRL_RTRYDL_MASK;
    WRITE_REGISTER_ULONG(pDevExt->PlxRegsBase + PLX_CONTROL, plx_config);

    // Set all output data bits high
    WRITE_REGISTER_ULONG(pDevExt->RegsBase + PAR_IO_DATA_OUT_0, pDevExt->DataOutConfig0);
    WRITE_REGISTER_ULONG(pDevExt->RegsBase + PAR_IO_DATA_OUT_1, pDevExt->DataOutConfig1);

    // Set clock to 8.25 MHz internal
    WRITE_REGISTER_ULONG(pDevExt->RegsBase + PAR_CLK_CNTRL, pDevExt->ClockCntrl.val32);

    // Disable PmcPario interrupts
    WRITE_REGISTER_ULONG(pDevExt->RegsBase + PAR_INT_CONFIG_CNTL, pDevExt->IntConfig.val32);


    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT,
        "<-- PmcParIoInitializeHardware");

    return STATUS_SUCCESS;
}// PmcParIoInitializeHardware

/*++
PmcParIoShutdown
Routine Description:
   Reset the device to put the device in a known initial state.
   This is called from D0Exit when the device is torn down or
   when the system is shutdown. Note that Wdf has already
   called out EvtDisable callback to disable the interrupt.
Arguments:
   pDevExt -  Pointer to our adapter
Return Value:
   VOID
--*/
VOID PmcParIoShutdown(__in PDEVICE_EXTENSION pDevExt)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "--> PmcParIoShutdown");
    // WdfInterrupt is already disabled so issue a full reset
    if (pDevExt->Regs) {
        PmcParIoHardwareReset(pDevExt);
    }
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "<-- PmcParIoShutdown");
}// PmcParIoShutdown

/*++
PmcParIoHardwareReset
Routine Description:
   Called by D0Exit when the device is being disabled or when the system is
   shutdown to put the device in a known initial state.
Arguments:
   pDevExt - Pointer to Device Extension
Return Value:
   VOID
--*/
VOID PmcParIoHardwareReset(__in PDEVICE_EXTENSION pDevExt)
{
    LARGE_INTEGER delay;
    UNREFERENCED_PARAMETER(pDevExt);
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "--> PmcParIoIssueFullReset");
    // Wait 100 msec.
    delay.QuadPart = WDF_REL_TIMEOUT_IN_MS(100);
    KeDelayExecutionThread(KernelMode, TRUE, &delay);
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, "<-- PmcParIoIssueFullReset");
}// PmcParIoHardwareReset
