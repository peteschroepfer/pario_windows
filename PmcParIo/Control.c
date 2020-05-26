/*++
Copyright (c) 2015 Dynamic Engineering All rights reserved.
   150 DuBois, Suite C
   Santa Cruz, CA 95060
   831-457-8891
   engineering@dyneng.com
   http://www.dyneng.com

Module Name:
   PmcParIoControl.c

Abstract:
   Callback function for Device I/O Control requests

Environment:
   Kernel mode
--*/

#include "Precomp.h"
#include "Control.tmh"

/*++
PmcParIoEvtIoDeviceControl
Routine Description:
   Called by the framework as soon as it receives a device control request.
   If the device is not ready, fail the request.
Arguments:
   Queue              - Handle to the framework queue object that is associated
                          with the I/O request.
   Request            - Handle to a framework request object.
   OutputBufferLength - length of the request's output buffer,
                        if an output buffer is available.
   InputBufferLength  - length of the request's input buffer,
                        if an input buffer is available.
   IoControlCode      - the driver-defined or system-defined I/O control code
                       (IOCTL) that is associated with the request.
Return Value:
   VOID
--*/
VOID PmcParIoEvtIoDeviceControl(__in WDFQUEUE   Queue,
    __in WDFREQUEST Request,
    __in size_t     OutputBufferLength,
    __in size_t     InputBufferLength,
    __in ULONG      IoControlCode)
{
    PDEVICE_EXTENSION                pDevExt;
    WDFDEVICE                        Device;
    PPMC_PAR_IO_DRIVER_DEVICE_INFO   pPmcParIoInfo = NULL;
    PPMC_PAR_IO_DATA                 pPmcParIoData = NULL;
    PPMC_PAR_IO_CLOCK_CONFIG         pPmcParIoClk = NULL;
    PPMC_PAR_IO_INT_CONFIG           pPmcParIoInt = NULL;
    PULONG                           pPmcParIoStatus = NULL;
    PULONG                           pEvent = NULL;
    PULONG                           pPmcParIoIsrStat = NULL;
    size_t                           bytesReturned = 0;
    NTSTATUS                         status = STATUS_INVALID_DEVICE_REQUEST;

    UNREFERENCED_PARAMETER(InputBufferLength);
    UNREFERENCED_PARAMETER(OutputBufferLength);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS,
        "--> PmcParIoEvtIoDeviceControl: Request %p", Request);
    Device = WdfIoQueueGetDevice(Queue);
    pDevExt = PmcParIoGetDeviceContext(Device);
    switch (IoControlCode) {

    case IOCTL_PMC_PAR_IO_GET_INFO:
        status = WdfRequestRetrieveOutputBuffer(Request,
            sizeof(*pPmcParIoInfo),
            &pPmcParIoInfo, NULL);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTLS,
                "User's output buffer is too small for this IOCTL, "
                "expecting an PMC_PAR_IO_DRIVER_DEVICE_INFO");
            bytesReturned = sizeof(*pPmcParIoInfo);
            break;
        }
        status = GetInfo(pDevExt, pPmcParIoInfo);
        if (NT_SUCCESS(status)) {
            bytesReturned = sizeof(*pPmcParIoInfo);
        }
        break;

    case IOCTL_PMC_PAR_IO_SET_OUT_DATA:
        status = WdfRequestRetrieveInputBuffer(Request,
            sizeof(*pPmcParIoData),
            &pPmcParIoData, NULL);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTLS,
                "User's input buffer is too small for this IOCTL, "
                "expecting a PMC_PAR_IO_DATA");
            bytesReturned = sizeof(*pPmcParIoData);
            break;
        }
        status = SetOutData(pDevExt, pPmcParIoData);
        break;

    case IOCTL_PMC_PAR_IO_GET_OUT_DATA:
        status = WdfRequestRetrieveOutputBuffer(Request,
            sizeof(*pPmcParIoData),
            &pPmcParIoData, NULL);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTLS,
                "User's output buffer is too small for this IOCTL, "
                "expecting a PMC_PAR_IO_DATA");
            bytesReturned = sizeof(*pPmcParIoData);
            break;
        }
        status = GetOutData(pDevExt, pPmcParIoData);
        if (NT_SUCCESS(status)) {
            bytesReturned = sizeof(*pPmcParIoData);
        }
        break;

    case IOCTL_PMC_PAR_IO_READ_IN_DATA:
        status = WdfRequestRetrieveOutputBuffer(Request,
            sizeof(*pPmcParIoData),
            &pPmcParIoData, NULL);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTLS,
                "User's output buffer is too small for this IOCTL, "
                "expecting a PMC_PAR_IO_DATA");
            bytesReturned = sizeof(*pPmcParIoData);
            break;
        }
        status = ReadInData(pDevExt, pPmcParIoData);
        if (NT_SUCCESS(status)) {
            bytesReturned = sizeof(*pPmcParIoData);
        }
        break;

    case IOCTL_PMC_PAR_IO_SET_CLOCK_CONFIG:
        status = WdfRequestRetrieveInputBuffer(Request,
            sizeof(*pPmcParIoClk),
            &pPmcParIoClk, NULL);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTLS,
                "User's input buffer is too small for this IOCTL, "
                "expecting a PMC_PAR_IO_CLOCK_CONFIG");
            bytesReturned = sizeof(*pPmcParIoClk);
            break;
        }
        status = SetClockConfig(pDevExt, pPmcParIoClk);
        break;

    case IOCTL_PMC_PAR_IO_GET_CLOCK_CONFIG:
        status = WdfRequestRetrieveOutputBuffer(Request,
            sizeof(*pPmcParIoClk),
            &pPmcParIoClk, NULL);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTLS,
                "User's output buffer is too small for this IOCTL, "
                "expecting a PMC_PAR_IO_CLOCK_CONFIG");
            bytesReturned = sizeof(*pPmcParIoClk);
            break;
        }
        status = GetClockConfig(pDevExt, pPmcParIoClk);
        if (NT_SUCCESS(status)) {
            bytesReturned = sizeof(*pPmcParIoClk);
        }
        break;

    case IOCTL_PMC_PAR_IO_SET_INT_CONFIG:
        status = WdfRequestRetrieveInputBuffer(Request,
            sizeof(*pPmcParIoInt),
            &pPmcParIoInt, NULL);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTLS,
                "User's input buffer is too small for this IOCTL, "
                "expecting a PMC_PAR_IO_CLOCK_CONFIG");
            bytesReturned = sizeof(*pPmcParIoInt);
            break;
        }
        status = SetIntConfig(pDevExt, pPmcParIoInt);
        break;

    case IOCTL_PMC_PAR_IO_GET_INT_CONFIG:
        status = WdfRequestRetrieveOutputBuffer(Request,
            sizeof(*pPmcParIoInt),
            &pPmcParIoInt, NULL);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTLS,
                "User's output buffer is too small for this IOCTL, "
                "expecting a PMC_PAR_IO_CLOCK_CONFIG");
            bytesReturned = sizeof(*pPmcParIoInt);
            break;
        }
        status = GetIntConfig(pDevExt, pPmcParIoInt);
        if (NT_SUCCESS(status)) {
            bytesReturned = sizeof(*pPmcParIoInt);
        }
        break;

    case IOCTL_PMC_PAR_IO_GET_INT_STATUS:
        status = WdfRequestRetrieveOutputBuffer(Request,
            sizeof(*pPmcParIoStatus),
            &pPmcParIoStatus, NULL);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTLS,
                "User's output buffer is too small for this IOCTL, "
                "expecting a ULONG");
            bytesReturned = sizeof(*pPmcParIoStatus);
            break;
        }
        status = GetIntStatus(pDevExt, pPmcParIoStatus);
        if (NT_SUCCESS(status)) {
            bytesReturned = sizeof(*pPmcParIoStatus);
        }
        break;

    case IOCTL_PMC_PAR_IO_REGISTER_EVENT:
        status = WdfRequestRetrieveInputBuffer(Request,
            sizeof(*pEvent),
            &pEvent, NULL);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTLS,
                "User's input buffer is too small for this IOCTL, "
                "expecting a ULONG");
            bytesReturned = sizeof(ULONG);
            break;
        }
        status = RegisterEvent(pDevExt, (PHANDLE)pEvent);
        break;

    case IOCTL_PMC_PAR_IO_ENABLE_INTERRUPT:
        status = EnableInterrupt(pDevExt);
        break;

    case IOCTL_PMC_PAR_IO_DISABLE_INTERRUPT:
        status = DisableInterrupt(pDevExt);
        break;

    case IOCTL_PMC_PAR_IO_FORCE_INTERRUPT:
        status = ForceInterrupt(pDevExt);
        break;

    case IOCTL_PMC_PAR_IO_GET_ISR_STATUS:
        status = WdfRequestRetrieveOutputBuffer(Request,
            sizeof(*pPmcParIoIsrStat),
            &pPmcParIoIsrStat, NULL);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTLS,
                "User's output buffer is too small for this IOCTL, "
                "expecting a PMC_PAR_IO_ISR_STATUS");
            bytesReturned = sizeof(ULONG);
            break;
        }
        status = GetIsrStatus(pDevExt, pPmcParIoIsrStat);
        if (NT_SUCCESS(status)) {
            bytesReturned = sizeof(ULONG);
        }
        break;

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
    }
    if (status != STATUS_PENDING) {
        WdfRequestCompleteWithInformation(Request, status, bytesReturned);
    }
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTLS,
        "<-- PmcParIoEvtIoDeviceControl");

    return;
}// PmcParIoEvtIoDeviceControl
