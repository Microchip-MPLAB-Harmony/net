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
/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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


