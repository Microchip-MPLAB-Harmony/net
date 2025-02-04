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
Copyright (C) 2012-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef H_TCPIP_TYPES_H_
#define H_TCPIP_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> 



typedef union
{
    uint8_t Val;
    struct __attribute__((packed))
    {
         unsigned b0:1;
         unsigned b1:1;
         unsigned b2:1;
         unsigned b3:1;
         unsigned b4:1;
         unsigned b5:1;
         unsigned b6:1;
         unsigned b7:1;
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
    } vByte;
    struct __attribute__((packed))
    {
         unsigned b0:1;
         unsigned b1:1;
         unsigned b2:1;
         unsigned b3:1;
         unsigned b4:1;
         unsigned b5:1;
         unsigned b6:1;
         unsigned b7:1;
         unsigned b8:1;
         unsigned b9:1;
         unsigned b10:1;
         unsigned b11:1;
         unsigned b12:1;
         unsigned b13:1;
         unsigned b14:1;
         unsigned b15:1;
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
    } vByte;
    struct __attribute__((packed))
    {
        TCPIP_UINT16_VAL low;
        TCPIP_UINT16_VAL high;
    }wordUnion;
    struct __attribute__((packed))
    {
         unsigned b0:1;
         unsigned b1:1;
         unsigned b2:1;
         unsigned b3:1;
         unsigned b4:1;
         unsigned b5:1;
         unsigned b6:1;
         unsigned b7:1;
         unsigned b8:1;
         unsigned b9:1;
         unsigned b10:1;
         unsigned b11:1;
         unsigned b12:1;
         unsigned b13:1;
         unsigned b14:1;
         unsigned b15:1;
         unsigned b16:1;
         unsigned b17:1;
         unsigned b18:1;
         unsigned b19:1;
         unsigned b20:1;
         unsigned b21:1;
         unsigned b22:1;
         unsigned b23:1;
         unsigned b24:1;
         unsigned b25:1;
         unsigned b26:1;
         unsigned b27:1;
         unsigned b28:1;
         unsigned b29:1;
         unsigned b30:1;
         unsigned b31:1;
    } bits;
} TCPIP_UINT32_VAL;

typedef union
{
    uint64_t Val;
    uint32_t v32[2] __attribute__((packed));
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
         unsigned b0:1;
         unsigned b1:1;
         unsigned b2:1;
         unsigned b3:1;
         unsigned b4:1;
         unsigned b5:1;
         unsigned b6:1;
         unsigned b7:1;
         unsigned b8:1;
         unsigned b9:1;
         unsigned b10:1;
         unsigned b11:1;
         unsigned b12:1;
         unsigned b13:1;
         unsigned b14:1;
         unsigned b15:1;
         unsigned b16:1;
         unsigned b17:1;
         unsigned b18:1;
         unsigned b19:1;
         unsigned b20:1;
         unsigned b21:1;
         unsigned b22:1;
         unsigned b23:1;
         unsigned b24:1;
         unsigned b25:1;
         unsigned b26:1;
         unsigned b27:1;
         unsigned b28:1;
         unsigned b29:1;
         unsigned b30:1;
         unsigned b31:1;
         unsigned b32:1;
         unsigned b33:1;
         unsigned b34:1;
         unsigned b35:1;
         unsigned b36:1;
         unsigned b37:1;
         unsigned b38:1;
         unsigned b39:1;
         unsigned b40:1;
         unsigned b41:1;
         unsigned b42:1;
         unsigned b43:1;
         unsigned b44:1;
         unsigned b45:1;
         unsigned b46:1;
         unsigned b47:1;
         unsigned b48:1;
         unsigned b49:1;
         unsigned b50:1;
         unsigned b51:1;
         unsigned b52:1;
         unsigned b53:1;
         unsigned b54:1;
         unsigned b55:1;
         unsigned b56:1;
         unsigned b57:1;
         unsigned b58:1;
         unsigned b59:1;
         unsigned b60:1;
         unsigned b61:1;
         unsigned b62:1;
         unsigned b63:1;
    } bits;
} TCPIP_UINT64_VAL;


#endif // H_TCPIP_TYPES_H_ 


