#pragma once


#include <Console\condrv.h>
#include <Console\conapi.h>


#pragma warning(push)
#pragma warning(disable: 4201)
typedef struct _CONSOLE_MSG_PACKET
{
    // From here down is the actual packet data sent/received.
    CD_IO_DESCRIPTOR Descriptor;
    union
    {
        struct
        {
            CD_CREATE_OBJECT_INFORMATION CreateObject;
            CONSOLE_CREATESCREENBUFFER_MSG CreateScreenBuffer;
        };
        struct
        {
            CONSOLE_MSG_HEADER msgHeader;
            union
            {
                CONSOLE_MSG_BODY_L1 consoleMsgL1;
                CONSOLE_MSG_BODY_L2 consoleMsgL2;
                CONSOLE_MSG_BODY_L3 consoleMsgL3;
            } u;
        };
    };
    // End packet data
}CONSOLE_MSG_PACKET, *PCONSOLE_MSG_PACKET;
#pragma warning(pop)
