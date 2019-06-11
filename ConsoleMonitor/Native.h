#pragma once


// Macro


#define CheckFastIODispatch(aFastIoDispatch, aRoutines) (                               \
    (aFastIoDispatch != nullptr)                                                     && \
    (aFastIoDispatch->SizeOfFastIoDispatch >= offsetof(FAST_IO_DISPATCH, aRoutines)) && \
    (aFastIoDispatch->aRoutines != nullptr)                                             \
)


// Struct


typedef struct _CURDIR
{
    struct _UNICODE_STRING DosPath;
    HANDLE Handle;
} CURDIR, * PCURDIR; /* size: 0x0018 */ /* size: 0x000c */
static_assert(sizeof(CURDIR) == (sizeof(SIZE_T) == sizeof(UINT64) ? 0x0018 : 0x000c), "");


typedef struct _RTL_DRIVE_LETTER_CURDIR
{
    USHORT Flags;
    USHORT Length;
    ULONG TimeStamp;
    struct _STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, * PRTL_DRIVE_LETTER_CURDIR; /* size: 0x0018 */ /* size: 0x0010 */
static_assert(sizeof(RTL_DRIVE_LETTER_CURDIR) == (sizeof(SIZE_T) == sizeof(UINT64) ? 0x0018 : 0x0010), "");


typedef struct _RTL_USER_PROCESS_PARAMETERS
{
    ULONG MaximumLength;
    ULONG Length;
    ULONG Flags;
    ULONG DebugFlags;
    HANDLE ConsoleHandle;
    ULONG  ConsoleFlags;
    HANDLE StandardInput;
    HANDLE StandardOutput;
    HANDLE StandardError;
    struct _CURDIR CurrentDirectory;
    struct _UNICODE_STRING DllPath;
    struct _UNICODE_STRING ImagePathName;
    struct _UNICODE_STRING CommandLine;
    PVOID Environment;
    ULONG StartingX;
    ULONG StartingY;
    ULONG CountX;
    ULONG CountY;
    ULONG CountCharsX;
    ULONG CountCharsY;
    ULONG FillAttribute;
    ULONG WindowFlags;
    ULONG ShowWindowFlags;
    struct _UNICODE_STRING WindowTitle;
    struct _UNICODE_STRING DesktopInfo;
    struct _UNICODE_STRING ShellInfo;
    struct _UNICODE_STRING RuntimeData;
    struct _RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32];
    SIZE_T EnvironmentSize;
    SIZE_T EnvironmentVersion;
    PVOID PackageDependencyData;
    ULONG ProcessGroupId;
    ULONG LoaderThreads;
    struct _UNICODE_STRING RedirectionDllName;
    struct _UNICODE_STRING HeapPartitionName;
    UINT64* DefaultThreadpoolCpuSetMasks;
    ULONG DefaultThreadpoolCpuSetMaskCount;
} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS; /* size: 0x0440 */ /* size: 0x02bc */
static_assert(sizeof(RTL_USER_PROCESS_PARAMETERS) == (sizeof(SIZE_T) == sizeof(UINT64) ? 0x0440 : 0x02bc), "");


// Extern


extern"C"
{
    extern POBJECT_TYPE* IoDriverObjectType;
    extern POBJECT_TYPE* IoDeviceObjectType;

    NTSTATUS NTAPI
        ObReferenceObjectByName(
            _In_ PUNICODE_STRING ObjectName,
            _In_ ULONG Attributes,
            _In_opt_ PACCESS_STATE AccessState,
            _In_opt_ ACCESS_MASK DesiredAccess,
            _In_ POBJECT_TYPE ObjectType,
            _In_ KPROCESSOR_MODE AccessMode,
            _Inout_opt_ PVOID ParseContext,
            _Out_ PVOID* Object
        );

    PDEVICE_OBJECT NTAPI
        IoGetLowerDeviceObject(
            _In_ PDEVICE_OBJECT DeviceObject
        );
}
