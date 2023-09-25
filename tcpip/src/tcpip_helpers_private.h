/*******************************************************************************
  Header file for tcpip_helpers_private

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_helpers_private.h

  Summary:
    
  Description:
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

#ifndef __TCPIP_HELPERS_PRIVATE_H_
#define __TCPIP_HELPERS_PRIVATE_H_



unsigned char   TCPIP_Helper_FindCommonPrefix (const unsigned char * addr1, const unsigned char * addr2, unsigned char bytes);

TCPIP_MAC_POWER_MODE TCPIP_Helper_StringToPowerMode(const char* str);

const char*     TCPIP_Helper_PowerModeToString(TCPIP_MAC_POWER_MODE mode);

uint16_t        TCPIP_Helper_CalcIPChecksum(const uint8_t* buffer, uint16_t len, uint16_t seed);
#if defined(__mips__)
#define TCPIP_Helper_Memcpy(dst, src, len)     memcpy(dst, src, len)
#else
void            TCPIP_Helper_Memcpy(void *dst, const void *src, size_t len);
#endif
uint16_t        TCPIP_Helper_PacketChecksum(TCPIP_MAC_PACKET* pPkt, uint8_t* startAdd, uint16_t len, uint16_t seed);

uint16_t        TCPIP_Helper_ChecksumFold(uint32_t checksum);

uint16_t        TCPIP_Helper_PacketCopy(TCPIP_MAC_PACKET* pSrcPkt, uint8_t* pDest, uint8_t** pStartAdd, uint16_t len, bool srchTransport);


// Protocols understood by the TCPIP_Helper_ExtractURLFields() function.  IMPORTANT: If you 
// need to reorder these (change their constant values), you must also reorder 
// the constant arrays in TCPIP_Helper_ExtractURLFields().
typedef enum
{
    PROTOCOL_HTTP = 0u,
    PROTOCOL_HTTPS,
    PROTOCOL_MMS,
    PROTOCOL_RTSP
} PROTOCOLS;

uint8_t         TCPIP_Helper_ExtractURLFields(uint8_t *vURL, PROTOCOLS *protocol,
                                 uint8_t *vUsername, uint16_t *wUsernameLen,
                                 uint8_t *vPassword, uint16_t *wPasswordLen,
                                 uint8_t *vHostname, uint16_t *wHostnameLen,
                                 uint16_t *wPort, uint8_t *vFilePath, uint16_t *wFilePathLen);


#endif  // __TCPIP_HELPERS_PRIVATE_H_
