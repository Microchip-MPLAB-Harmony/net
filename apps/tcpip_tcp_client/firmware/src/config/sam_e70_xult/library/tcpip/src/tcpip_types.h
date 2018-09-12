/*******************************************************************************
  TCPIP types helper file

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_types.h

  Summary:
    Specific types definitions for the TCPIP stack
    
  Description:
    This header file contains the definitions of the 
    internal bit fields and other internal structures needed in the TCPIP stack
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2012 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef _TCPIP_TYPES_H_
#define _TCPIP_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> 



typedef union
{
    uint8_t Val;
    struct __attribute__((packed))
    {
         uint8_t b0:1;
         uint8_t b1:1;
         uint8_t b2:1;
         uint8_t b3:1;
         uint8_t b4:1;
         uint8_t b5:1;
         uint8_t b6:1;
         uint8_t b7:1;
    } bits;
} TCPIP_UINT8_VAL, TCPIP_UINT8_BITS;

typedef union 
{
    uint16_t Val;
    uint8_t v[2];
    struct __attribute__((packed))
    {
        uint8_t LB;
        uint8_t HB;
    } byte;
    struct __attribute__((packed))
    {
         uint8_t b0:1;
         uint8_t b1:1;
         uint8_t b2:1;
         uint8_t b3:1;
         uint8_t b4:1;
         uint8_t b5:1;
         uint8_t b6:1;
         uint8_t b7:1;
         uint8_t b8:1;
         uint8_t b9:1;
         uint8_t b10:1;
         uint8_t b11:1;
         uint8_t b12:1;
         uint8_t b13:1;
         uint8_t b14:1;
         uint8_t b15:1;
    } bits;
} TCPIP_UINT16_VAL, TCPIP_UINT16_BITS;


typedef union
{
    uint32_t Val;
    uint16_t w[2] __attribute__((packed));
    uint8_t  v[4];
    struct __attribute__((packed))
    {
        uint16_t LW;
        uint16_t HW;
    } word;
    struct __attribute__((packed))
    {
        uint8_t LB;
        uint8_t HB;
        uint8_t UB;
        uint8_t MB;
    } byte;
    struct __attribute__((packed))
    {
        TCPIP_UINT16_VAL low;
        TCPIP_UINT16_VAL high;
    }wordUnion;
    struct __attribute__((packed))
    {
         uint8_t b0:1;
         uint8_t b1:1;
         uint8_t b2:1;
         uint8_t b3:1;
         uint8_t b4:1;
         uint8_t b5:1;
         uint8_t b6:1;
         uint8_t b7:1;
         uint8_t b8:1;
         uint8_t b9:1;
         uint8_t b10:1;
         uint8_t b11:1;
         uint8_t b12:1;
         uint8_t b13:1;
         uint8_t b14:1;
         uint8_t b15:1;
         uint8_t b16:1;
         uint8_t b17:1;
         uint8_t b18:1;
         uint8_t b19:1;
         uint8_t b20:1;
         uint8_t b21:1;
         uint8_t b22:1;
         uint8_t b23:1;
         uint8_t b24:1;
         uint8_t b25:1;
         uint8_t b26:1;
         uint8_t b27:1;
         uint8_t b28:1;
         uint8_t b29:1;
         uint8_t b30:1;
         uint8_t b31:1;
    } bits;
} TCPIP_UINT32_VAL;

typedef union
{
    uint64_t Val;
    uint32_t d[2] __attribute__((packed));
    uint16_t w[4] __attribute__((packed));
    uint8_t v[8];
    struct __attribute__((packed))
    {
        uint32_t LD;
        uint32_t HD;
    } dword;
    struct __attribute__((packed))
    {
        uint16_t LW;
        uint16_t HW;
        uint16_t UW;
        uint16_t MW;
    } word;
    struct __attribute__((packed))
    {
         uint8_t b0:1;
         uint8_t b1:1;
         uint8_t b2:1;
         uint8_t b3:1;
         uint8_t b4:1;
         uint8_t b5:1;
         uint8_t b6:1;
         uint8_t b7:1;
         uint8_t b8:1;
         uint8_t b9:1;
         uint8_t b10:1;
         uint8_t b11:1;
         uint8_t b12:1;
         uint8_t b13:1;
         uint8_t b14:1;
         uint8_t b15:1;
         uint8_t b16:1;
         uint8_t b17:1;
         uint8_t b18:1;
         uint8_t b19:1;
         uint8_t b20:1;
         uint8_t b21:1;
         uint8_t b22:1;
         uint8_t b23:1;
         uint8_t b24:1;
         uint8_t b25:1;
         uint8_t b26:1;
         uint8_t b27:1;
         uint8_t b28:1;
         uint8_t b29:1;
         uint8_t b30:1;
         uint8_t b31:1;
         uint8_t b32:1;
         uint8_t b33:1;
         uint8_t b34:1;
         uint8_t b35:1;
         uint8_t b36:1;
         uint8_t b37:1;
         uint8_t b38:1;
         uint8_t b39:1;
         uint8_t b40:1;
         uint8_t b41:1;
         uint8_t b42:1;
         uint8_t b43:1;
         uint8_t b44:1;
         uint8_t b45:1;
         uint8_t b46:1;
         uint8_t b47:1;
         uint8_t b48:1;
         uint8_t b49:1;
         uint8_t b50:1;
         uint8_t b51:1;
         uint8_t b52:1;
         uint8_t b53:1;
         uint8_t b54:1;
         uint8_t b55:1;
         uint8_t b56:1;
         uint8_t b57:1;
         uint8_t b58:1;
         uint8_t b59:1;
         uint8_t b60:1;
         uint8_t b61:1;
         uint8_t b62:1;
         uint8_t b63:1;
    } bits;
} TCPIP_UINT64_VAL;


#endif // _TCPIP_TYPES_H_ 


