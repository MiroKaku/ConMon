#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <ntddk.h>
#include <wdm.h>
#include <ntstrsafe.h>

#include "Native.h"


inline auto __cdecl ConMonPrint(ULONG aLevel, PCSTR aFormat, ...) -> ULONG
{
    auto vResult    = 0ul;
    auto vArgs      = va_list();

    va_start(vArgs, aFormat);
    vResult = vDbgPrintExWithPrefix("[ConMon] ", DPFLTR_IHVDRIVER_ID, aLevel, aFormat, vArgs);
    va_end(vArgs);

    return vResult;
}
