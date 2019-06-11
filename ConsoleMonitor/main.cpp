#include "stdafx.h"
#include "ConMon.h"


struct ConMonDeviceExt
{
    PDEVICE_OBJECT  AttachedDevice;
};


extern"C"   DRIVER_INITIALIZE   DriverEntry;
static      PDEVICE_OBJECT      s_ConMon = nullptr;


static 
auto InvalidDeviceRequest(
    PDEVICE_OBJECT  /*aDeviceObject*/,
    PIRP            aIrp)
    -> NTSTATUS
{
    if (IRP_MJ_POWER == (IoGetCurrentIrpStackLocation(aIrp))->MajorFunction)
    {
        PoStartNextPowerIrp(aIrp);
    }

    aIrp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
    IoCompleteRequest(aIrp, IO_NO_INCREMENT);
    return STATUS_INVALID_DEVICE_REQUEST;
}

static
auto DriverDispatchDefault(PDEVICE_OBJECT aDeviceObject, PIRP aIrp) -> NTSTATUS
{
    auto vResult        = STATUS_SUCCESS;
    auto vNextDevice    = IoGetLowerDeviceObject(aDeviceObject);

    if (vNextDevice != nullptr)
    {
        IoSkipCurrentIrpStackLocation(aIrp);
        vResult = IoCallDriver(vNextDevice, aIrp);
    }
    else
    {
        vResult = InvalidDeviceRequest(aDeviceObject, aIrp);
    }

    return ObDereferenceObject(vNextDevice), vResult;
}

static
auto DriverFastIoDeviceControl(
    _In_        PFILE_OBJECT    aFileObject,
    _In_        BOOLEAN         aWait,
    _In_opt_    PVOID           aInputBuffer,
    _In_        ULONG           aInputBufferLength,
    _Out_opt_   PVOID           aOutputBuffer,
    _In_        ULONG           aOutputBufferLength,
    _In_        ULONG           aIoControlCode,
    _Out_       PIO_STATUS_BLOCK aIoStatus,
    _In_        PDEVICE_OBJECT  aDeviceObject
) -> BOOLEAN
{
    auto vResult        = BOOLEAN(TRUE);
    auto vNextDevice    = IoGetLowerDeviceObject(aDeviceObject);
    auto vContext       = PVOID(nullptr);

    for (;;)
    {
        if (vNextDevice == nullptr)
        {
            vResult = FALSE;
            break;
        }

        auto vFastIODispatch = vNextDevice->DriverObject->FastIoDispatch;
        if (!CheckFastIODispatch(vFastIODispatch, FastIoDeviceControl))
        {
            vResult = FALSE;
            break;
        }

        ConMonFastIoDeviceControlPre(
            aFileObject,
            aWait,
            aInputBuffer,
            aInputBufferLength,
            aOutputBuffer,
            aOutputBufferLength,
            aIoControlCode,
            aIoStatus,
            aDeviceObject,
            &vContext);

        vResult = vFastIODispatch->FastIoDeviceControl(
            aFileObject,
            aWait,
            aInputBuffer,
            aInputBufferLength,
            aOutputBuffer,
            aOutputBufferLength,
            aIoControlCode,
            aIoStatus,
            aDeviceObject);

        ConMonFastIoDeviceControlPost(
            aFileObject,
            aWait,
            aInputBuffer,
            aInputBufferLength,
            aOutputBuffer,
            aOutputBufferLength,
            aIoControlCode,
            aIoStatus,
            aDeviceObject,
            vContext);

        break;
    }
    if (vNextDevice)
    {
        ObDereferenceObject(vNextDevice);
    }

    return vResult;
}

static
auto DriverUnload(PDRIVER_OBJECT aDriverObject) -> void
{
    if (s_ConMon)
    {
        auto vNextDevice = static_cast<ConMonDeviceExt*>(s_ConMon->DeviceExtension)->AttachedDevice;
        IoDetachDevice(vNextDevice);
        IoDeleteDevice(s_ConMon);

        s_ConMon = nullptr;
    }

    if (aDriverObject->FastIoDispatch)
    {
        ExFreePoolWithTag(aDriverObject->FastIoDispatch, ConMonTag);

        aDriverObject->FastIoDispatch = nullptr;
    }

    ConMonPrint(DPFLTR_ERROR_LEVEL, "[" __FUNCTION__ "] GoodBye!\n");
}

auto DriverEntry(PDRIVER_OBJECT aDriverObject, PUNICODE_STRING /*aRegistryPath*/) -> NTSTATUS
{
    auto vResult        = STATUS_SUCCESS;
    auto& vConMon       = s_ConMon;
    auto vConDrvDevice  = PDEVICE_OBJECT(nullptr);
    auto vConDrvDriver  = PDRIVER_OBJECT(nullptr);

    for (;;)
    {
        // Assigning IRP Major Functions
        for (size_t i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
        {
            aDriverObject->MajorFunction[i] = DriverDispatchDefault;
        }
        aDriverObject->DriverUnload = DriverUnload;
        
        // Assigning Fast I/O
        auto vFastIoDispatch = (PFAST_IO_DISPATCH)ExAllocatePoolWithTag(
            NonPagedPoolNx, sizeof(FAST_IO_DISPATCH), ConMonTag);
        if (vFastIoDispatch == nullptr)
        {
            vResult = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }
        RtlSecureZeroMemory(vFastIoDispatch, sizeof(*vFastIoDispatch));

        vFastIoDispatch->SizeOfFastIoDispatch   = sizeof(*vFastIoDispatch);
        vFastIoDispatch->FastIoDeviceControl    = DriverFastIoDeviceControl;
        aDriverObject->FastIoDispatch = vFastIoDispatch;

        // Get DriverObject of ConDrv.sys
        // Note: Name must be case sensitive!
        auto vConDrvDriverName = UNICODE_STRING(RTL_CONSTANT_STRING(CONSOLE_DRIVER_NAME));
        vResult = ObReferenceObjectByName(
            &vConDrvDriverName,
            OBJ_CASE_INSENSITIVE,
            nullptr,
            FILE_ANY_ACCESS,
            *IoDriverObjectType,
            KernelMode,
            nullptr,
            (void**)& vConDrvDriver);
        if (!NT_SUCCESS(vResult))
        {
            break;
        }
        vConDrvDevice = vConDrvDriver->DeviceObject;
        ObReferenceObject(vConDrvDevice);

        // Create ConMon DeviceObject
        auto vConMonDeviceName = UNICODE_STRING(RTL_CONSTANT_STRING(CONMON_DEVICE_NAME));
        vResult = IoCreateDevice(
            aDriverObject,
            sizeof(ConMonDeviceExt),
            &vConMonDeviceName,
            FILE_DEVICE_CONSOLE,
            FILE_DEVICE_ALLOW_APPCONTAINER_TRAVERSAL,
            FALSE,
            &vConMon);
        if (!NT_SUCCESS(vResult))
        {
            break;
        }
        auto vDeviceExt = static_cast<ConMonDeviceExt*>(vConMon->DeviceExtension);
        
        // ConMon Attached To ConDrv
        vResult = IoAttachDeviceToDeviceStackSafe(
            vConMon,
            vConDrvDevice,
            &vDeviceExt->AttachedDevice);
        if (!NT_SUCCESS(vResult))
        {
            break;
        }
        
        vConMon->DeviceType         = vConDrvDevice->DeviceType;
        vConMon->Characteristics    = vConDrvDevice->Characteristics;
        vConMon->Flags              = (vConDrvDevice->Flags & (DO_BUFFERED_IO | DO_DIRECT_IO));
        vConMon->Flags              &= (~DO_DEVICE_INITIALIZING);

        ConMonPrint(DPFLTR_ERROR_LEVEL, "[" __FUNCTION__ "] Hi!\n");
        break;
    }
    if (vConDrvDevice)
    {
        ObDereferenceObject(vConDrvDevice);
    }
    if (vConDrvDriver)
    {
        ObDereferenceObject(vConDrvDriver);
    }
    if (!NT_SUCCESS(vResult))
    {
        DriverUnload(aDriverObject);
    }

    return vResult;
}
