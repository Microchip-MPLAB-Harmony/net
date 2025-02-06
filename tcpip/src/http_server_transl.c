/*******************************************************************************
  HyperText Transfer Protocol (HTTP) Server

  File Name:
    http_server_transl.c

  Summary:
    Translation module for Microchip TCP/IP Stack
    
  Description:
    Provides compatibility between the http_server and the http_net custom files

*******************************************************************************/

/*
Copyright (C) 2023-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/



#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_HTTP_SERVER_V2

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_HTTP_SERVER_V2) && defined(HTTP_SERVER_V2_NET_COMPATIBILITY)

bool TCPIP_HTTP_NET_SSIVariableSet(const char* varName, TCPIP_HTTP_DYN_ARG_TYPE varType, const char* strValue, int32_t intValue)
{
    TCPIP_HTTP_DYN_ARG_DCPT varDcpt;
    varDcpt.argType = varType;
    if(varType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
    {
        varDcpt.argInt32 = intValue;
    }
    else if(varType == TCPIP_HTTP_DYN_ARG_TYPE_STRING)
    {
        varDcpt.argStr = strValue;
    }
    else
    {   // unsupported type
        return false;
    }


    return TCPIP_HTTP_SSIVariableSet(0, varName, &varDcpt);
}

const char* TCPIP_HTTP_NET_SSIVariableGet(const char* varName, TCPIP_HTTP_DYN_ARG_TYPE* pVarType, int32_t* pVarInt)
{
    TCPIP_HTTP_DYN_ARG_DCPT varDcpt;
    if(TCPIP_HTTP_SSIVariableGet(0, varName, &varDcpt))
    {
        if(pVarType)
        {
            *pVarType = (TCPIP_HTTP_DYN_ARG_TYPE)varDcpt.argType;
        }
        if(pVarInt)
        {
            *pVarInt = varDcpt.argInt32;
        }
        return varDcpt.argStr;
    }

    return 0;
}

const char*   TCPIP_HTTP_NET_SSIVariableGetByIndex(int varIndex, const char** pVarName, TCPIP_HTTP_DYN_ARG_TYPE* pVarType, int32_t* pVarInt)
{
    TCPIP_HTTP_DYN_ARG_DCPT varDcpt;

    if(TCPIP_HTTP_SSIVariableGetByIndex(0, varIndex, pVarName, &varDcpt))
    {
        if(pVarType)
        {
            *pVarType = (TCPIP_HTTP_DYN_ARG_TYPE)varDcpt.argType;
        }
        if(varDcpt.argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32 && pVarInt != 0)
        {
            *pVarInt = varDcpt.argInt32;
        } 
        if(pVarName)
        {
            *pVarName = varDcpt.argStr;
        }
        return varDcpt.argStr;
    }

    return 0;
}

#endif  // defined(TCPIP_STACK_USE_HTTP_SERVER_V2) && defined(HTTP_SERVER_V2_NET_COMPATIBILITY)


