/*++
Copyright (c) 2015 Dynamic Engineering All rights reserved.
   150 DuBois, Suite C
   Santa Cruz, CA 95060
   831-457-8891
   engineering@dyneng.com
   http://www.dyneng.com

Module Name:
   PmcParIoIsrDpc.c

Abstract:
   Contains routines related to interrupt and dpc handling.

Environment:
   Kernel mode
--*/

#include "Precomp.h"
#include "IsrDpc.tmh"

/*++
PmcParIoInterruptCreate
Routine Description:
   Configure and create the WDFINTERRUPT object.
   This routine is called by EvtDeviceAdd callback.
Arguments:
   pDevExt - Pointer to our DEVICE_EXTENSION
Return Value:
   NTSTATUS code
--*/
NTSTATUS PmcParIoInterruptCreate(__in PDEVICE_EXTENSION pDevExt)
{
    NTSTATUS             status;
    WDF_INTERRUPT_CONFIG InterruptConfig;

    WDF_INTERRUPT_CONFIG_INIT(&InterruptConfig,
        PmcParIoEvtInterruptIsr,
        PmcParIoEvtInterruptDpc);

    InterruptConfig.EvtInterruptEnable = PmcParIoEvtInterruptEnable;
    InterruptConfig.EvtInterruptDisable = PmcParIoEvtInterruptDisable;
    
    // Create a WDFSPINLOCK object.
    status = WdfSpinLockCreate(WDF_NO_OBJECT_ATTRIBUTES,
        &pDevExt->IntSpinLock);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
            "PmcParIoInterruptCreate failed to create spinlock: %!STATUS!",
            status);
        return status;
    }

    // Enable DpcForIsr Synchronization
    InterruptConfig.AutomaticSerialization = TRUE;
    // Create interrupt object in EvtDeviceAdd. The framework connects and enables
    //   after EvtDeviceD0Entry, disables and disconnects before EvtDeviceD0Exit.
    status = WdfInterruptCreate(pDevExt->Device,
        &InterruptConfig,
        WDF_NO_OBJECT_ATTRIBUTES,
        &pDevExt->Interrupt);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
            "PmcParIoInterruptCreate failed: %!STATUS!", status);
    }

    return status;
}// PmcParIoInterruptCreate

/*++
PmcParIoEvtInterruptIsr
Routine Description:
   Interrupt handler for this driver. Called at DIRQL level when the
   device or another device sharing the same interrupt line asserts
   the interrupt. The driver first checks the device to make sure whether
   this interrupt is generated by its device and if so clear the interrupt
   register to disable further generation of interrupts and queue a
   DPC to do other I/O work related to interrupt - such as reading
   the device memory, starting a DMA transaction, coping it to
   the request buffer and completing the request, etc.
Arguments:
   Interrupt - Handle to WDFINTERRUPT Object for this device.
   MessageID - MSI message ID (always 0 in this configuration)
Return Value:
   TRUE  -- This device generated the interrupt.
   FALSE -- This device did not generated this interrupt.
--*/
BOOLEAN PmcParIoEvtInterruptIsr(__in WDFINTERRUPT Interrupt,
    __in ULONG        MessageID)
{
    PDEVICE_EXTENSION pDevExt;
    INT_PLX_REG       PlxIntReg;
    BOOLEAN           isRecognized = FALSE;

    UNREFERENCED_PARAMETER(MessageID);
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INTERRUPT,
        "--> PmcParIoInterruptHandler");
    pDevExt = PmcParIoGetDeviceContext(WdfInterruptGetDevice(Interrupt));

    // Read the Interrupt Status register
    PlxIntReg.val32 = READ_REGISTER_ULONG(pDevExt->PlxRegsBase + PLX_INT_CONT_STAT);

    if (pDevExt->PlxIntContStat.bits.PciIntEn && PlxIntReg.bits.LocIntStat1) {
        TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INTERRUPT,
            " User interrupt stat %x", PlxIntReg.val32);
        pDevExt->IntConfig.bits.IntEn0 = FALSE;
        pDevExt->IsrStatus.val32 = PlxIntReg.val32;
        isRecognized = TRUE;
        TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INTERRUPT,
            "Uint IsrStat:0x%08x", pDevExt->IsrStatus.val32);
    }

    if (pDevExt->PlxIntContStat.bits.PciIntEn && PlxIntReg.bits.LocIntStat2) {
        TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INTERRUPT,
            " User interrupt stat %x", PlxIntReg.val32);
        pDevExt->IntConfig.bits.IntEn1 = FALSE;
        pDevExt->IsrStatus.val32 = PlxIntReg.val32;
        isRecognized = TRUE;
        TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INTERRUPT,
            "Uint IsrStat:0x%08x", pDevExt->IsrStatus.val32);
    }

    if (isRecognized) {
        WRITE_REGISTER_ULONG((PULONG)&pDevExt->Regs->IntConfigCntl,
            pDevExt->IntConfig.val32);
        // Queue the DPC
        if (!WdfInterruptQueueDpcForIsr(pDevExt->Interrupt)) {
            TraceEvents(TRACE_LEVEL_WARNING, DBG_INTERRUPT,
                "DPC enqueue failed!");
        }
    }
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INTERRUPT, " User interrupt");

    WdfInterruptQueueDpcForIsr(pDevExt->Interrupt);
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INTERRUPT,
        "<-- PmcParIoInterruptHandler");
    return TRUE;
}// PmcParIoEvtInterruptIsr

/*++
PmcParIoEvtInterruptDpc
Routine Description:
   DPC callback for ISR. Please note that on a multiprocessor system,
   you could have more than one DPCs running simulataneously on
   multiple processors. So if you are accesing any global resources
   make sure to synchrnonize the accesses with a spinlock.
Arguments:
   Interrupt - Handle to WDFINTERRUPT Object for this device.
   Device    - WDFDEVICE object passed to InterruptCreate
Return Value:
   VOID
--*/
VOID PmcParIoEvtInterruptDpc(WDFINTERRUPT Interrupt,
    WDFDEVICE    Device)
{
    PDEVICE_EXTENSION pDevExt;

    UNREFERENCED_PARAMETER(Device);
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_DPC, "--> EvtInterruptDpc");
    pDevExt = PmcParIoGetDeviceContext(WdfInterruptGetDevice(Interrupt));

    // Acquire this device's interrupt spinlock.
    WdfSpinLockAcquire(pDevExt->IntSpinLock);


    // Release our interrupt spinlock
    WdfSpinLockRelease(pDevExt->IntSpinLock);
    // Signal Event if not NULL
    if (pDevExt->pEventObject != NULL) {
        KeSetEvent(pDevExt->pEventObject, EVENT_INCREMENT, FALSE);
    }
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_DPC, "<-- EvtInterruptDpc");

    return;
}// PmcParIoEvtInterruptDpc

/*++
PmcParIoEvtInterruptEnable
Routine Description:
   Called by the framework at DIRQL immediately after registering the ISR
   with the kernel by calling IoConnectInterrupt.
Return Value:
   NTSTATUS
--*/
NTSTATUS PmcParIoEvtInterruptEnable(__in WDFINTERRUPT Interrupt,
    __in WDFDEVICE    Device)
{
    ULONG reg_val;
    PDEVICE_EXTENSION pDevExt;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INTERRUPT,
        "PmcParIoEvtInterruptEnable: Interrupt 0x%p, Device 0x%p",
        Interrupt, Device);

    pDevExt = PmcParIoGetDeviceContext(WdfInterruptGetDevice(Interrupt));
    //enable plx to pass interrupts through
    reg_val = READ_REGISTER_ULONG(pDevExt->PlxRegsBase + PLX_INT_CONT_STAT);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PETE,
        "PLX INT CONFIG REG: %x",
        reg_val);    
    

    WRITE_REGISTER_ULONG(pDevExt->PlxRegsBase + PLX_INT_CONT_STAT, 0x49);

    WRITE_REGISTER_ULONG(pDevExt->RegsBase + PAR_CLK_CNTRL, 0x6);

    WRITE_REGISTER_ULONG(pDevExt->RegsBase + PAR_INT_CONFIG_CNTL, 0x44);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PETE,
        "PLX_INT_CONT_STAT: 0x%x",
        READ_REGISTER_ULONG(pDevExt->PlxRegsBase + PLX_INT_CONT_STAT));

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PETE,
        "PAR_CLK_CNTRL: 0x%x",
        READ_REGISTER_ULONG(pDevExt->RegsBase + PAR_CLK_CNTRL));

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PETE,
        "PAR_INT_CONFIG_CNTL: 0x%x",
        READ_REGISTER_ULONG(pDevExt->RegsBase + PAR_INT_CONFIG_CNTL));

    return STATUS_SUCCESS;
}// PmcParIoEvtInterruptEnable

/*++
PmcParIoEvtInterruptDisable
Routine Description:
    Called by the framework at DIRQL before Deregistering the ISR with
    the kernel by calling IoDisconnectInterrupt.
Return Value:
    NTSTATUS
--*/
NTSTATUS PmcParIoEvtInterruptDisable(__in WDFINTERRUPT Interrupt,
    __in WDFDEVICE    Device)
{
    PDEVICE_EXTENSION pDevExt;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INTERRUPT,
        "PmcParIoEvtInterruptDisable: Interrupt 0x%p, Device 0x%p",
        Interrupt, Device);

    pDevExt = PmcParIoGetDeviceContext(WdfInterruptGetDevice(Interrupt));
    pDevExt->PlxIntContStat.bits.PciIntEn = FALSE;
    WRITE_REGISTER_ULONG(pDevExt->PlxRegsBase + + PLX_INT_CONT_STAT,
        pDevExt->PlxIntContStat.val32);

    return STATUS_SUCCESS;
}// PmcParIoEvtInterruptDisable

