#include "stdafx.h"
#include "ConMon.h"


auto ConMonIsEmptyUnicodeString(PUNICODE_STRING aString) -> bool
{
    if (!NT_SUCCESS(RtlUnicodeStringValidate(aString)))
    {
        return true;
    }

    if (aString->Length != 2)
    {
        return false;
    }

    if (aString->Buffer[0] == L'\r' && 
        aString->Buffer[1] == L'\n')
    {
        return true;
    }

    return false;
}

auto ConMonConsoleDispatchRequest(_In_ PCONSOLE_MSG_PACKET aMsg, _In_ PIO_STATUS_BLOCK aIoStatus) -> NTSTATUS
{
    ConMonPrint(DPFLTR_ERROR_LEVEL,
        "[%s][Status: 0x%X][CurrentPId: %u]: \n"
        "\tLayerNumber  : %u\n"
        "\tApiNumber    : %u\n",
        __FUNCTION__,
        aIoStatus->Status,
        PsGetCurrentProcessId(),
        (aMsg->msgHeader.ApiNumber >> 24) - 1,
        (aMsg->msgHeader.ApiNumber & 0xffffff));

    return STATUS_SUCCESS;
}

auto ConMonConsoleConnectRequest(_In_ PCONSOLE_MSG_PACKET aMsg, _In_ PIO_STATUS_BLOCK aIoStatus) -> NTSTATUS
{
    ConMonPrint(DPFLTR_ERROR_LEVEL,
        "[%s][Status: 0x%X][CurrentPId: %u]: \n"
        "\tProcessId    : %Iu\n"
        "\tThreadId     : %Iu\n",
        __FUNCTION__,
        aIoStatus->Status,
        PsGetCurrentProcessId(),
        aMsg->Descriptor.Process,
        aMsg->Descriptor.Object);

    return STATUS_SUCCESS;
}

auto ConMonClientDisconnectRequest(_In_ PCONSOLE_MSG_PACKET /*aMsg*/, _In_ PIO_STATUS_BLOCK /*aIoStatus*/) -> NTSTATUS
{
    return STATUS_NOT_IMPLEMENTED;
}

auto ConMonConsoleCreateObject(_In_ PCONSOLE_MSG_PACKET /*aMsg*/, _In_ PIO_STATUS_BLOCK /*aIoStatus*/) -> NTSTATUS
{
    return STATUS_NOT_IMPLEMENTED;
}

auto ConMonConsoleCloseObject(_In_ PCONSOLE_MSG_PACKET /*aMsg*/, _In_ PIO_STATUS_BLOCK /*aIoStatus*/) -> NTSTATUS
{
    return STATUS_NOT_IMPLEMENTED;
}

auto ConMonReadIO(
    _In_opt_    PCD_IO_COMPLETE     /*aInputBuffer*/,
    _In_        ULONG               /*aInputBufferLength*/,
    _Out_opt_   PCONSOLE_MSG_PACKET aOutputBuffer,
    _In_        ULONG               aOutputBufferLength,
    _Out_       PIO_STATUS_BLOCK    aIoStatus
) -> NTSTATUS
{
    auto vResult = STATUS_SUCCESS;

    if (aOutputBuffer == nullptr || aOutputBufferLength < sizeof(*aOutputBuffer))
    {
        return STATUS_UNSUCCESSFUL;
    }
    
    ConMonPrint(DPFLTR_ERROR_LEVEL,
        "[%s][Status: 0x%X][CurrentPId: %u]: \n"
        "\tMessageId    : %llX\n"
        "\tFunction     : %u\n"
        "\tInputSize    : %u\n"
        "\tOutputSize   : %u\n",
        __FUNCTION__,
        aIoStatus->Status,
        PsGetCurrentProcessId(),
        aOutputBuffer->Descriptor.Identifier,
        aOutputBuffer->Descriptor.Function,
        aOutputBuffer->Descriptor.InputSize,
        aOutputBuffer->Descriptor.OutputSize);

    switch (aOutputBuffer->Descriptor.Function)
    {
    default:
        break;

    case CONSOLE_IO_USER_DEFINED:
        vResult = ConMonConsoleDispatchRequest(aOutputBuffer, aIoStatus);
        break;

    case CONSOLE_IO_CONNECT:
        vResult = ConMonConsoleConnectRequest(aOutputBuffer, aIoStatus);
        break;

    case CONSOLE_IO_DISCONNECT:
        vResult = ConMonClientDisconnectRequest(aOutputBuffer, aIoStatus);
        break;

    case CONSOLE_IO_CREATE_OBJECT:
        vResult = ConMonConsoleCreateObject(aOutputBuffer, aIoStatus);
        break;

    case CONSOLE_IO_CLOSE_OBJECT:
        vResult = ConMonConsoleCloseObject(aOutputBuffer, aIoStatus);
        break;

    case CONSOLE_IO_RAW_WRITE:
        break;

    case CONSOLE_IO_RAW_READ:
        break;

    case CONSOLE_IO_RAW_FLUSH:
        break;

    }

    return vResult;
}

auto ConMonCompleteIO(
    _In_opt_    PCD_IO_COMPLETE     aInputBuffer,
    _In_        ULONG               aInputBufferLength,
    _Out_       PIO_STATUS_BLOCK    aIoStatus
) -> NTSTATUS
{
    auto vResult = STATUS_SUCCESS;

    for (;;)
    {
        if (aInputBuffer == nullptr || aInputBufferLength == 0)
        {
            break;
        }

        auto vMessage           = UNICODE_STRING();
        vMessage.Buffer         = static_cast<PWCH>  (aInputBuffer->Write.Data);
        vMessage.Length         = static_cast<USHORT>(aInputBuffer->Write.Size);
        vMessage.MaximumLength  = static_cast<USHORT>(aInputBuffer->Write.Size);

        if (ConMonIsEmptyUnicodeString(&vMessage))
        {
            vMessage = UNICODE_STRING();
        }

        ConMonPrint(DPFLTR_ERROR_LEVEL,
            "[%s][Status: 0x%X][CurrentPId: %u]: \n"
            "\tStatus       : 0x%08X\n"
            "\tMessageId    : %llX\n"
            "\tData         : %wZ\n",
            __FUNCTION__,
            aIoStatus->Status,
            PsGetCurrentProcessId(),
            aInputBuffer->IoStatus.Status,
            aInputBuffer->Identifier,
            &vMessage);

        break;
    }

    return vResult;
}

auto ConMonReadInput(
    _In_opt_    PCD_IO_OPERATION    aInputBuffer,
    _In_        ULONG               aInputBufferLength,
    _Out_       PIO_STATUS_BLOCK    aIoStatus
) -> NTSTATUS
{
    auto vResult = STATUS_SUCCESS;

    for (;;)
    {
        if (aInputBuffer == nullptr || aInputBufferLength == 0)
        {
            break;
        }

        auto vMessage           = UNICODE_STRING();
        vMessage.Buffer         = static_cast<PWCH>  (aInputBuffer->Buffer.Data);
        vMessage.Length         = static_cast<USHORT>(aInputBuffer->Buffer.Size);
        vMessage.MaximumLength  = static_cast<USHORT>(aInputBuffer->Buffer.Size);

        if (ConMonIsEmptyUnicodeString(&vMessage))
        {
            vMessage = UNICODE_STRING();
        }

        ConMonPrint(DPFLTR_ERROR_LEVEL,
            "[%s][Status: 0x%X][CurrentPId: %u]: \n"
            "\tMessageId    : %llX\n"
            "\tData         : %wZ\n",
            __FUNCTION__,
            aIoStatus->Status,
            PsGetCurrentProcessId(),
            aInputBuffer->Identifier,
            &vMessage);

        break;
    }

    return vResult;
}

auto ConMonWriteOutput(
    _In_opt_    PCD_IO_OPERATION    aInputBuffer,
    _In_        ULONG               aInputBufferLength,
    _Out_       PIO_STATUS_BLOCK    aIoStatus
) -> NTSTATUS
{
    auto vResult = STATUS_SUCCESS;

    for (;;)
    {
        if (aInputBuffer == nullptr || aInputBufferLength == 0)
        {
            break;
        }

        auto vMessage           = UNICODE_STRING();
        vMessage.Buffer         = static_cast<PWCH>  (aInputBuffer->Buffer.Data);
        vMessage.Length         = static_cast<USHORT>(aInputBuffer->Buffer.Size);
        vMessage.MaximumLength  = static_cast<USHORT>(aInputBuffer->Buffer.Size);

        if (ConMonIsEmptyUnicodeString(&vMessage))
        {
            vMessage = UNICODE_STRING();
        }

        ConMonPrint(DPFLTR_ERROR_LEVEL,
            "[%s][Status: 0x%X][CurrentPId: %u]: \n"
            "\tMessageId    : %llX\n"
            "\tData         : %wZ\n",
            __FUNCTION__,
            aIoStatus->Status,
            PsGetCurrentProcessId(),
            aInputBuffer->Identifier,
            &vMessage);

        break;
    }

    return vResult;
}

auto ConMonLaunchServer(
    _In_opt_    PRTL_USER_PROCESS_PARAMETERS    aInputBuffer,
    _In_        ULONG                           aInputBufferLength,
    _Out_       PIO_STATUS_BLOCK                aIoStatus
) -> NTSTATUS
{
    auto vResult = STATUS_SUCCESS;

    for (;;)
    {
        if (aInputBuffer == nullptr || aInputBufferLength == 0)
        {
            break;
        }

        ConMonPrint(DPFLTR_ERROR_LEVEL,
            "[%s][Status: 0x%X][CurrentPId: %u]: \n"
            "\tConsoleHandle: %p\n"
            "\tConsoleFlags : %u\n"
            "\tStdInput     : %p\n"
            "\tStdOutput    : %p\n"
            "\tStdError     : %p\n"
            "\tImagePathName: %wZ\n"
            "\tCommandLine  : %wZ\n",
            __FUNCTION__,
            aIoStatus->Status,
            PsGetCurrentProcessId(),
            aInputBuffer->ConsoleHandle,
            aInputBuffer->ConsoleFlags,
            aInputBuffer->StandardInput,
            aInputBuffer->StandardOutput,
            aInputBuffer->StandardError,
            aInputBuffer->ImagePathName,
            aInputBuffer->CommandLine);

        break;
    }

    return vResult;
}

auto ConMonFastIoDeviceControlPre(
    _In_        PFILE_OBJECT    aFileObject,
    _In_        BOOLEAN         aWait,
    _In_opt_    PVOID           aInputBuffer,
    _In_        ULONG           aInputBufferLength,
    _Out_opt_   PVOID           aOutputBuffer,
    _In_        ULONG           aOutputBufferLength,
    _In_        ULONG           aIoControlCode,
    _Out_       PIO_STATUS_BLOCK aIoStatus,
    _In_        PDEVICE_OBJECT  aDeviceObject,
    _Out_opt_   PVOID*          aContext
)->BOOLEAN
{
    UNREFERENCED_PARAMETER(aFileObject);
    UNREFERENCED_PARAMETER(aWait);
    UNREFERENCED_PARAMETER(aInputBuffer);
    UNREFERENCED_PARAMETER(aInputBufferLength);
    UNREFERENCED_PARAMETER(aOutputBuffer);
    UNREFERENCED_PARAMETER(aOutputBufferLength);
    UNREFERENCED_PARAMETER(aIoControlCode);
    UNREFERENCED_PARAMETER(aIoStatus);
    UNREFERENCED_PARAMETER(aDeviceObject);
    UNREFERENCED_PARAMETER(aContext);

    auto vResult = STATUS_SUCCESS;

    switch (aIoControlCode)
    {
    default:
        break;
    
    case IOCTL_CONDRV_READ_IO:
        break;
    
    case IOCTL_CONDRV_COMPLETE_IO:
        break;
    
    case IOCTL_CONDRV_READ_INPUT:
        break;
    
    case IOCTL_CONDRV_WRITE_OUTPUT:
        break;
    
    case IOCTL_CONDRV_ISSUE_USER_IO:
        break;
    
    case IOCTL_CONDRV_DISCONNECT_PIPE:
        break;
    
    case IOCTL_CONDRV_SET_SERVER_INFORMATION:
        break;
    
    case IOCTL_CONDRV_GET_SERVER_PID:
        break;
    
    case IOCTL_CONDRV_GET_DISPLAY_SIZE:
        break;
    
    case IOCTL_CONDRV_UPDATE_DISPLAY:
        break;
    
    case IOCTL_CONDRV_SET_CURSOR:
        break;
    
    case IOCTL_CONDRV_ALLOW_VIA_UIACCESS:
        break;
    
    case IOCTL_CONDRV_LAUNCH_SERVER:
        break;
    }

    return NT_SUCCESS(vResult);
}

auto ConMonFastIoDeviceControlPost(
    _In_        PFILE_OBJECT    aFileObject,
    _In_        BOOLEAN         aWait,
    _In_opt_    PVOID           aInputBuffer,
    _In_        ULONG           aInputBufferLength,
    _Out_opt_   PVOID           aOutputBuffer,
    _In_        ULONG           aOutputBufferLength,
    _In_        ULONG           aIoControlCode,
    _Out_       PIO_STATUS_BLOCK aIoStatus,
    _In_        PDEVICE_OBJECT  aDeviceObject,
    _In_opt_    PVOID           aContext
)->BOOLEAN
{
    UNREFERENCED_PARAMETER(aFileObject);
    UNREFERENCED_PARAMETER(aWait);
    UNREFERENCED_PARAMETER(aInputBuffer);
    UNREFERENCED_PARAMETER(aInputBufferLength);
    UNREFERENCED_PARAMETER(aOutputBuffer);
    UNREFERENCED_PARAMETER(aOutputBufferLength);
    UNREFERENCED_PARAMETER(aIoControlCode);
    UNREFERENCED_PARAMETER(aIoStatus);
    UNREFERENCED_PARAMETER(aDeviceObject);
    UNREFERENCED_PARAMETER(aContext);

    auto vResult = STATUS_SUCCESS;

    switch (aIoControlCode)
    {
    default:
        break;

    case IOCTL_CONDRV_READ_IO:
        vResult = ConMonReadIO(
            static_cast<PCD_IO_COMPLETE>(aInputBuffer),
            aInputBufferLength,
            static_cast<PCONSOLE_MSG_PACKET>(aOutputBuffer),
            aOutputBufferLength,
            aIoStatus);
        break;

    case IOCTL_CONDRV_COMPLETE_IO:
        vResult = ConMonCompleteIO(
            static_cast<PCD_IO_COMPLETE>(aInputBuffer),
            aInputBufferLength,
            aIoStatus);
        break;

    case IOCTL_CONDRV_READ_INPUT:
        vResult = ConMonReadInput(
            static_cast<PCD_IO_OPERATION>(aInputBuffer),
            aInputBufferLength,
            aIoStatus);
        break;

    case IOCTL_CONDRV_WRITE_OUTPUT:
        vResult = ConMonWriteOutput(
            static_cast<PCD_IO_OPERATION>(aInputBuffer),
            aInputBufferLength,
            aIoStatus);
        break;

    case IOCTL_CONDRV_ISSUE_USER_IO:
        break;

    case IOCTL_CONDRV_DISCONNECT_PIPE:
        break;

    case IOCTL_CONDRV_SET_SERVER_INFORMATION:
        break;

    case IOCTL_CONDRV_GET_SERVER_PID:
        break;

    case IOCTL_CONDRV_GET_DISPLAY_SIZE:
        break;

    case IOCTL_CONDRV_UPDATE_DISPLAY:
        break;

    case IOCTL_CONDRV_SET_CURSOR:
        break;

    case IOCTL_CONDRV_ALLOW_VIA_UIACCESS:
        break;

    case IOCTL_CONDRV_LAUNCH_SERVER:
        vResult = ConMonLaunchServer(
            static_cast<PRTL_USER_PROCESS_PARAMETERS>(aInputBuffer),
            aInputBufferLength,
            aIoStatus);
        break;
    }

    return NT_SUCCESS(vResult);
}

