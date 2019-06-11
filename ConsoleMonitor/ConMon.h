#pragma once
#include "Console.h"


#define CONMON_NAME         LR"(ConMon)"
#define CONMON_DEVICE_NAME  LR"(\Device\)"      CONMON_NAME
#define CONMON_DOS_NAME     LR"(\DosDevices\)"  CONMON_NAME

enum { ConMonTag = 'CMon' };


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
)->BOOLEAN;

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
)->BOOLEAN;

