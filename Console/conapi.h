/*++
Copyright (c) Microsoft Corporation

Module Name:
- conapi.h

Abstract:
- This module contains the internal structures and definitions used by the console server.

Author:
- Therese Stowell (ThereseS) 12-Nov-1990

Revision History:
--*/

#pragma once

#define STATUS_SHARING_VIOLATION         ((NTSTATUS)0xC0000043L)
#include "wincontypes.h"

//
// Console APIs
//

/* Logical Font */
#define LF_FACESIZE         32

/* Selection flags */
#define CONSOLE_NO_SELECTION            0x0000
#define CONSOLE_SELECTION_IN_PROGRESS   0x0001   // selection has begun
#define CONSOLE_SELECTION_NOT_EMPTY     0x0002   // non-null select rectangle
#define CONSOLE_MOUSE_SELECTION         0x0004   // selecting with mouse
#define CONSOLE_MOUSE_DOWN              0x0008   // mouse is down

typedef struct _CONSOLE_SELECTION_INFO {
    DWORD dwFlags;
    COORD dwSelectionAnchor;
    SMALL_RECT srSelection;
} CONSOLE_SELECTION_INFO, * PCONSOLE_SELECTION_INFO;

//
// Console Message Packet
//

#include "conmsgl1.h"
#include "conmsgl2.h"
#include "conmsgl3.h"
