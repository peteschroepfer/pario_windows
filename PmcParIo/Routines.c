/*++
Copyright (c) 2015 Dynamic Engineering All rights reserved.
   150 DuBois, Suite C
   Santa Cruz, CA 95060
   831-457-8891
   engineering@dyneng.com
   http://www.dyneng.com

Module Name:
   PmcParIoRoutines.c

Abstract:
   Functions called by PmcParIoEvtIoDeviceControl to complete IOCTLs

Environment:
   Kernel mode
--*/

#include "Precomp.h"
#include "Routines.tmh"

extern CONST HANDLE _INVALID_FILE_HANDLE;
extern PCHAR   XsvfErrorName[];
extern UCHAR   TdoByteExpc;
extern UCHAR   TdoByteRcvd;
extern UCHAR   TdoByteMask;
extern USHORT  TdoIndex;

/*++
GetInfo
Routine Description:
   Called by PmcParIoEvtIoDeviceControl in response to
   IOCTL_PMC_PAR_IO_GET_INFO device control request.
Arguments:
   pDevExt  - Pointer to Device Extension
   pPmcParIoInfo - Pointer to the output structure
Return Value:
   NTSTATUS
--*/
NTSTATUS GetInfo(__in  PDEVICE_EXTENSION           pDevExt,
    __out PPMC_PAR_IO_DRIVER_DEVICE_INFO pPmcParIoInfo)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "--> GetInfo");
    pPmcParIoInfo->DriverVersion = PMC_PAR_IO_DRIVER_VERSION;
    pPmcParIoInfo->InstanceNumber = pDevExt->instance;
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS,
        "drv:%d",
        pPmcParIoInfo->DriverVersion);
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "<-- GetInfo");

    return STATUS_SUCCESS;
}// GetInfo

/*++
SetOutData
Routine Description:
   Called by PmcParIoEvtIoDeviceControl in response to
   IOCTL_PMC_PAR_IO_SET_OUT_DATA device control request.
Arguments:
   pDevExt     - Pointer to Device Extension
   pParIoOutData - Pointer to the input structure
Return Value:
   NTSTATUS
--*/
NTSTATUS SetOutData(__in PDEVICE_EXTENSION   pDevExt,
    __in PPMC_PAR_IO_DATA     pParIoOutData)
{

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "--> SetOutData");

    WRITE_REGISTER_ULONG(pDevExt->RegsBase + PAR_IO_DATA_OUT_0, pParIoOutData->LoWord);
    WRITE_REGISTER_ULONG(pDevExt->RegsBase + PAR_IO_DATA_OUT_1, pParIoOutData->HiWord);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "<-- SetOutData");

    return STATUS_SUCCESS;
}// SetOutData

/*++
GetOutData
Routine Description:
   Called by PmcParIoEvtIoDeviceControl in response to
   IOCTL_PMC_PAR_IO_GET_OUT_DATA device control request.
Arguments:
   pDevExt     - Pointer to Device Extension
   pParIoOutData - Pointer to the output structure
Return Value:
   NTSTATUS
--*/
NTSTATUS GetOutData(__in  PDEVICE_EXTENSION  pDevExt,
    __out PPMC_PAR_IO_DATA    pParIoOutData)
{

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "--> GetOutData");

    pParIoOutData->LoWord = READ_REGISTER_ULONG(pDevExt->RegsBase + PAR_IO_DATA_OUT_0);
    pParIoOutData->HiWord = READ_REGISTER_ULONG(pDevExt->RegsBase + PAR_IO_DATA_OUT_1);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "<-- GetOutData");

    return STATUS_SUCCESS;
}// GetOutData

/*++
ReadInData
Routine Description:
   Called by PmcParIoEvtIoDeviceControl in response to
   IOCTL_PMC_PAR_IO_GET_OUT_DATA device control request.
Arguments:
   pDevExt     - Pointer to Device Extension
   pParIoOutData - Pointer to the output structure
Return Value:
   NTSTATUS
--*/
NTSTATUS ReadInData(__in  PDEVICE_EXTENSION  pDevExt,
    __out PPMC_PAR_IO_DATA    pParIoOutData)
{

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "--> ReadInData");

    pParIoOutData->LoWord = READ_REGISTER_ULONG(pDevExt->RegsBase + PAR_IO_DATA_IN_0);
    pParIoOutData->HiWord = READ_REGISTER_ULONG(pDevExt->RegsBase + PAR_IO_DATA_IN_1);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "<-- ReadInData");

    return STATUS_SUCCESS;
}// ReadInData

/*++
SetClockConfig
Routine Description:
   Called by PmcParIoEvtIoDeviceControl in response to
   IOCTL_PMC_PAR_IO_SET_OUT_DATA device control request.
Arguments:
   pDevExt     - Pointer to Device Extension
   pParIoClockConfig - Pointer to the input structure
Return Value:
   NTSTATUS
--*/
NTSTATUS SetClockConfig(__in PDEVICE_EXTENSION   pDevExt,
    __in PPMC_PAR_IO_CLOCK_CONFIG     pParIoClockConfig)
{
    CLK_CNTRL_REG  config;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "--> SetClockConfig");

    config.val32 = pDevExt->ClockCntrl.val32;
    config.bits.ClkDivisor = pParIoClockConfig->FreqSelect & 0x7;

    if (pParIoClockConfig->ExtClockSelect) {
        config.bits.ClkSel = TRUE;
    }
    else {
        config.bits.ClkSel = FALSE;
    }

    if (pParIoClockConfig->ExtEnableSelect) {
        config.bits.ClkEnSel = TRUE;
    }
    else {
        config.bits.ClkEnSel = FALSE;
    }

    if (pParIoClockConfig->IntEnableOn) {
        config.bits.IntClkEn = TRUE;
    }
    else {
        config.bits.IntClkEn = FALSE;
    }

    WRITE_REGISTER_ULONG(pDevExt->RegsBase + PAR_CLK_CNTRL, config.val32);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, " - Registers %p", pDevExt->RegsBase + PAR_CLK_CNTRL);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "<-- SetClockConfig");

    return STATUS_SUCCESS;
}// SetClockConfig

/*++
GetClockConfig
Routine Description:
   Called by PmcParIoEvtIoDeviceControl in response to
   IOCTL_PMC_PAR_IO_GET_OUT_DATA device control request.
Arguments:
   pDevExt     - Pointer to Device Extension
   pParIoClockConfig - Pointer to the output structure
Return Value:
   NTSTATUS
--*/
NTSTATUS GetClockConfig(__in  PDEVICE_EXTENSION  pDevExt,
    __out PPMC_PAR_IO_CLOCK_CONFIG    pParIoClockConfig)
{
    CLK_CNTRL_REG  config;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "--> GetClockConfig");

    config.val32 = READ_REGISTER_ULONG(pDevExt->RegsBase + PAR_CLK_CNTRL);

    pParIoClockConfig->FreqSelect = config.bits.ClkDivisor & 0x7;

    if (config.bits.ClkSel) {
        pParIoClockConfig->ExtClockSelect = TRUE;
    }
    else {
        pParIoClockConfig->ExtClockSelect = FALSE;
    }

    if (config.bits.ClkEnSel) {
        pParIoClockConfig->ExtEnableSelect = TRUE;
    }
    else {
        pParIoClockConfig->ExtEnableSelect = FALSE;
    }

    if (config.bits.IntClkEn) {
        pParIoClockConfig->IntEnableOn = TRUE;
    }
    else {
        pParIoClockConfig->IntEnableOn = FALSE;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, " - Registers %p", pDevExt->RegsBase + PAR_CLK_CNTRL);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "<-- GetClockConfig");

    return STATUS_SUCCESS;
}// GetClockConfig

/*++
SetIntConfig
Routine Description:
   Called by PmcParIoEvtIoDeviceControl in response to
   IOCTL_PMC_PAR_IO_SET_OUT_DATA device control request.
Arguments:
   pDevExt     - Pointer to Device Extension
   pParIoIntConfig - Pointer to the input structure
Return Value:
   NTSTATUS
--*/
NTSTATUS SetIntConfig(__in PDEVICE_EXTENSION       pDevExt,
    __in PPMC_PAR_IO_INT_CONFIG  pParIoIntConfig)
{
    INT_REG_REG   config;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "--> SetIntConfig");

    config.val32 = pDevExt->IntConfig.val32;

    if (pParIoIntConfig->Int0Enabled) {
        config.bits.IntEn0 = TRUE;
    }
    else {
        config.bits.IntEn0 = FALSE;
    }

    if (pParIoIntConfig->Int0Edge_Level) {
        config.bits.EdgeTrig0 = TRUE;
    }
    else {
        config.bits.EdgeTrig0 = FALSE;
    }

    if (pParIoIntConfig->Int0PolHi_Lo) {
        config.bits.PolarityHi0 = TRUE;
    }
    else {
        config.bits.PolarityHi0 = FALSE;
    }

    if (pParIoIntConfig->Int1Enabled) {
        config.bits.IntEn1 = TRUE;
    }
    else {
        config.bits.IntEn1 = FALSE;
    }

    if (pParIoIntConfig->Int1Edge_Level) {
        config.bits.EdgeTrig1 = TRUE;
    }
    else {
        config.bits.EdgeTrig1 = FALSE;
    }

    if (pParIoIntConfig->Int1PolHi_Lo) {
        config.bits.PolarityHi1 = TRUE;
    }
    else {
        config.bits.PolarityHi1 = FALSE;
    }

    WRITE_REGISTER_ULONG(pDevExt->RegsBase + PAR_INT_CONFIG_CNTL, config.val32);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, " - Registers %p", pDevExt->RegsBase + PAR_INT_CONFIG_CNTL);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "<-- SetIntConfig");

    return STATUS_SUCCESS;
}// SetIntConfig

/*++
GetIntConfig
Routine Description:
   Called by PmcParIoEvtIoDeviceControl in response to
   IOCTL_PMC_PAR_IO_GET_OUT_DATA device control request.
Arguments:
   pDevExt     - Pointer to Device Extension
   pParIoIntConfig - Pointer to the output structure
Return Value:
   NTSTATUS
--*/
NTSTATUS GetIntConfig(__in  PDEVICE_EXTENSION      pDevExt,
    __out PPMC_PAR_IO_INT_CONFIG pParIoIntConfig)
{
    INT_REG_REG   config;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "--> GetIntConfig");

    config.val32 = READ_REGISTER_ULONG(pDevExt->RegsBase + PAR_INT_CONFIG_CNTL);

    if (config.bits.PolarityHi0) {
        pParIoIntConfig->Int0PolHi_Lo = TRUE;
    }
    else {
        pParIoIntConfig->Int0PolHi_Lo = FALSE;
    }

    if (config.bits.EdgeTrig0) {
        pParIoIntConfig->Int0Edge_Level = TRUE;
    }
    else {
        pParIoIntConfig->Int0Edge_Level = FALSE;
    }

    if (config.bits.IntEn0) {
        pParIoIntConfig->Int0Enabled = TRUE;
    }
    else {
        pParIoIntConfig->Int0Enabled = FALSE;
    }

    if (config.bits.PolarityHi1) {
        pParIoIntConfig->Int1PolHi_Lo = TRUE;
    }
    else {
        pParIoIntConfig->Int1PolHi_Lo = FALSE;
    }

    if (config.bits.EdgeTrig1) {
        pParIoIntConfig->Int1Edge_Level = TRUE;
    }
    else {
        pParIoIntConfig->Int1Edge_Level = FALSE;
    }

    if (config.bits.IntEn1) {
        pParIoIntConfig->Int1Enabled = TRUE;
    }
    else {
        pParIoIntConfig->Int1Enabled = FALSE;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, " - Registers %p", pDevExt->RegsBase + PAR_INT_CONFIG_CNTL);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "<-- GetIntConfig");

    return STATUS_SUCCESS;
}// GetIntConfig

/*++
GetIntStatus
Routine Description:
   Called by PmcParIoEvtIoDeviceControl in response to
   IOCTL_PMC_PAR_IO_GET_INT_STATUS device control request.
Arguments:
   pDevExt       - Pointer to Device Extension
   pPmcParIoStatus - Pointer to the output structure
Return Value:
   NTSTATUS
--*/
NTSTATUS GetIntStatus(__in  PDEVICE_EXTENSION pDevExt,
    __out PULONG            pPmcParIoStatus)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "--> GetIntStatus");
    // Read the interrupt status register
    *pPmcParIoStatus = READ_REGISTER_ULONG(pDevExt->PlxRegsBase + PLX_INT_CONT_STAT);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, " - Registers %p", pDevExt->PlxRegsBase + PLX_INT_CONT_STAT);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "<-- GetIntStatus");
    return STATUS_SUCCESS;
}// GetIntStatus

/*++
RegisterEvent
Routine Description:
   Called by PmcParIoEvtIoDeviceControl in response to IOCTL_PMC_PAR_IO_REGISTER_EVENT
   device control request.
Arguments:
   pDevExt - Pointer to Device Extension
   pEvent  - Pointer to Event Handle
Return Value:
   NTSTATUS
--*/
NTSTATUS RegisterEvent(__in PDEVICE_EXTENSION pDevExt,
    __in PHANDLE           pEvent)
{
    PVOID       pTemp;
    NTSTATUS    status;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "--> RegisterEvent");

    if (pDevExt->pEventObject != NULL) {
        // Dereference old pointer
        ObDereferenceObject((PVOID)pDevExt->pEventObject);
    }

    if (*pEvent == NULL) {
        // Unregister event
        pDevExt->pEventObject = NULL;
        return STATUS_SUCCESS;
    }

    // Get kernel Event Object pointer
    status = ObReferenceObjectByHandle(*pEvent, SYNCHRONIZE, NULL,
        UserMode, &pTemp, NULL);

    // Save in data member
    pDevExt->pEventObject = (PKEVENT)pTemp;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "<-- RegisterEvent");

    return status;
}// RegisterEvent

/*++
EnableInterrupt
Routine Description:
   Called by PmcParIoEvtIoDeviceControl in response to
   IOCTL_PMC_PAR_IO_ENABLE_INTERRUPT device control request.
Arguments:
   pDevExt - Pointer to Device Extension
Return Value:
   NTSTATUS
--*/
NTSTATUS EnableInterrupt(__in PDEVICE_EXTENSION pDevExt)
{

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "--> EnableInterrupt");

    // Write configuration register
    pDevExt->PlxIntContStat.bits.LocIntEn1 = TRUE;
    pDevExt->PlxIntContStat.bits.LocIntEn2 = TRUE;
    pDevExt->PlxIntContStat.bits.PciIntEn = TRUE;

    WRITE_REGISTER_ULONG(pDevExt->PlxRegsBase + PLX_INT_CONT_STAT, pDevExt->PlxIntContStat.val32);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, " - Registers %p", pDevExt->PlxRegsBase + PLX_INT_CONT_STAT);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "<-- EnableInterrupt");

    return STATUS_SUCCESS;
}// EnableInterrupt

/*++
DisableInterrupt
Routine Description:
   Called by PmcParIoEvtIoDeviceControl in response to
   IOCTL_PMC_PAR_IO_DISABLE_INTERRUPT device control request.
Arguments:
   pDevExt - Pointer to Device Extension
Return Value:
   NTSTATUS
--*/
NTSTATUS DisableInterrupt(__in PDEVICE_EXTENSION pDevExt)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "--> DisableInterrupt");

    // Disable interrupt

    pDevExt->PlxIntContStat.bits.LocIntEn1 = FALSE;
    pDevExt->PlxIntContStat.bits.LocIntEn2 = FALSE;
    pDevExt->PlxIntContStat.bits.PciIntEn = FALSE;

    WRITE_REGISTER_ULONG(pDevExt->PlxRegsBase + PLX_INT_CONT_STAT, pDevExt->PlxIntContStat.val32);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, " - Registers %p", pDevExt->PlxRegsBase + PLX_INT_CONT_STAT);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "<-- DisableInterrupt");

    return STATUS_SUCCESS;
}// DisableInterrupt

/*++
ForceInterrupt
Routine Description:
   Called by PmcParIoEvtIoDeviceControl in response to
   IOCTL_PMC_PAR_IO_FORCE_INTERRUPT device control request.
Arguments:
   pDevExt - Pointer to Device Extension
Return Value:
   NTSTATUS
--*/
NTSTATUS ForceInterrupt(__in PDEVICE_EXTENSION pDevExt)
{
    INT_PLX_REG    PlxIntConfig;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "--> ForceInterrupt");

    PlxIntConfig.val32 = pDevExt->PlxIntContStat.val32;
    PlxIntConfig.bits.ForceInt = TRUE;
    WRITE_REGISTER_ULONG(pDevExt->PlxRegsBase + PLX_INT_CONT_STAT, PlxIntConfig.val32);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_PNP, " - Registers %p", pDevExt->PlxRegsBase + PLX_INT_CONT_STAT);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "<-- ForceInterrupt");

    return STATUS_SUCCESS;
}// ForceInterrupt

/*++
GetIsrStatus
Routine Description:
   Called by PmcParIoEvtIoDeviceControl in response to
   IOCTL_PMC_PAR_IO_GET_ISR_STATUS device control request.
Arguments:
   pDevExt        - Pointer to Device Extension
   pPmcParIoIsrStat - Pointer to the output parameter
Return Value:
   NTSTATUS
--*/
NTSTATUS GetIsrStatus(__in  PDEVICE_EXTENSION   pDevExt,
    __out PULONG              pPmcParIoIsrStat)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "--> GetIsrStatus");

    *pPmcParIoIsrStat = pDevExt->IsrStatus.val32;
    //   pPmcParIoIsrStat->PlxLocIntStat1 = (BOOLEAN)(pPmcParIoIsrStat->IsrStatLong & PLX_ICS_LOC_INT1ACTV);
    //   pPmcParIoIsrStat->PlxLocIntStat2 = (BOOLEAN)(pPmcParIoIsrStat->IsrStatLong & PLX_ICS_LOC_INT2ACTV);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS, "<-- GetIsrStatus");
    return STATUS_SUCCESS;
}// GetIsrStatus


