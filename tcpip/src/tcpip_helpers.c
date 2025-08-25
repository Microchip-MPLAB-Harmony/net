/*******************************************************************************
  Helper Functions for Microchip tcpip

  Summary:
    TCP/IP Stack Library Helpers
    
  Description:
    Helpers library for Microchip TCP/IP Stack
*******************************************************************************/

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









#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_MANAGER

#include "tcpip/src/tcpip_private.h"

#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>


static bool CheckIpStrEnd(uint8_t ch);

static __inline__ bool __attribute__((always_inline)) IsHexDigit(uint8_t ch)
{
    signed char sch = (signed char)ch;

    return (isxdigit(sch) != 0);
}


// table with private IPv4 addresses, BE
typedef struct
{
    uint32_t    mask;
    uint32_t    address;
}S_TCPIP_Helper_PrivateAddEntry;

static const S_TCPIP_Helper_PrivateAddEntry T_TCPIP_Helper_PrivateAddTbl[] = 
{
    {0x000000ff, 0x0000000a},     // 10.0.0.0/8
    {0x0000f0ff, 0x000010ac},     // 172.16.0.0/12
    {0x0000ffff, 0x0000a8c0},     // 192.168.0.0/16
};

// secure port entries

#define M_TCPIP_STACK_SECURE_PORT_MIN_ENTRIES 10

#if TCPIP_STACK_SECURE_PORT_ENTRIES == 0
#define M_TCPIP_STACK_SECURE_PORT_ENTRIES    0
#elif TCPIP_STACK_SECURE_PORT_ENTRIES < M_TCPIP_STACK_SECURE_PORT_MIN_ENTRIES
#define M_TCPIP_STACK_SECURE_PORT_ENTRIES M_TCPIP_STACK_SECURE_PORT_MIN_ENTRIES
#else
#define M_TCPIP_STACK_SECURE_PORT_ENTRIES TCPIP_STACK_SECURE_PORT_ENTRIES
#endif

typedef enum
{
    TCPIP_HELPER_PORT_FLAG_STREAM   = 0x01,     // stream socket port
    TCPIP_HELPER_PORT_FLAG_DGRAM    = 0x02,     // datagram socket port

}TCPIP_HELPER_PORT_FLAGS;

typedef struct
{
    uint16_t    port;
    uint16_t    flags;  // TCPIP_HELPER_PORT_FLAGS value
}TCPIP_HELPER_PORT_ENTRY;

#if M_TCPIP_STACK_SECURE_PORT_ENTRIES != 0
static TCPIP_HELPER_PORT_ENTRY tcpSecurePortTbl[M_TCPIP_STACK_SECURE_PORT_ENTRIES] = 
{
    // port         flags                                                                           // module
    {443,           (uint16_t)(TCPIP_HELPER_PORT_FLAG_STREAM | TCPIP_HELPER_PORT_FLAG_DGRAM)},      // HTTPS
    {465,           (uint16_t)TCPIP_HELPER_PORT_FLAG_STREAM },                                      // SMTPS
    {587,           (uint16_t)TCPIP_HELPER_PORT_FLAG_STREAM },                                      // SMTPS
    {563,           (uint16_t)(TCPIP_HELPER_PORT_FLAG_STREAM | TCPIP_HELPER_PORT_FLAG_DGRAM)},      // NNTPS
    {636,           (uint16_t)(TCPIP_HELPER_PORT_FLAG_STREAM | TCPIP_HELPER_PORT_FLAG_DGRAM)},      // LDAPS
    {989,           (uint16_t)(TCPIP_HELPER_PORT_FLAG_STREAM | TCPIP_HELPER_PORT_FLAG_DGRAM)},      // FTPS
    {990,           (uint16_t)(TCPIP_HELPER_PORT_FLAG_STREAM | TCPIP_HELPER_PORT_FLAG_DGRAM)},      // FTPS
    {992,           (uint16_t)(TCPIP_HELPER_PORT_FLAG_STREAM | TCPIP_HELPER_PORT_FLAG_DGRAM)},      // telnet
    {993,           (uint16_t)TCPIP_HELPER_PORT_FLAG_STREAM },                                      // IMAPS
    {8883,          (uint16_t)TCPIP_HELPER_PORT_FLAG_STREAM },                                      // MQTT

    // extra slots could be added
};

static TCPIP_HELPER_PORT_ENTRY*    F_TCPIP_Helper_SecurePortEntry(uint16_t port, TCPIP_HELPER_PORT_ENTRY** pFreeEntry);
#endif  // M_TCPIP_STACK_SECURE_PORT_ENTRIES != 0


/*****************************************************************************
  Function:
    bool TCPIP_Helper_StringToIPAddress(uint8_t* str, IPV4_ADDR* IPAddress)

  Summary:
    Converts a string to an IP address

  Description:
    This function parses a dotted-quad decimal IP address string into an 
    IPV4_ADDR struct.  The output result is big-endian.
    
  Precondition:
    None

  Parameters:
    str - Pointer to a dotted-quad IP address string
    IPAddress - Pointer to IPV4_ADDR in which to store the result

  Return Values:
    true - an IP address was successfully decoded
    false - no IP address could be found, or the format was incorrect
  ***************************************************************************/
bool TCPIP_Helper_StringToIPAddress(const char* str, IPV4_ADDR* IPAddress)
{
    TCPIP_UINT32_VAL dwVal;
    IPV4_ADDR   convAddr;
    uint8_t cStr, charLen, currentOctet;

    if(IPAddress != NULL)
    {
        IPAddress->Val = 0;
    }

    if(str == NULL || (strlen(str) == 0U))
    {
        return false;
    }

    charLen = 0;
    currentOctet = 0;
    dwVal.Val = 0;

    while((cStr = (uint8_t)*str++) != 0U)
    {
        if(currentOctet > 3u)
        {
            break;
        }

        uint8_t cAdj = cStr -(uint8_t)'0';  // subtract the ascii 0
        

        // Validate the character is a numerical digit or dot, depending on location
        if(charLen == 0u)
        {
            if(cAdj > 9u)
            {
                return false;
            }
        }
        else if(charLen == 3u)
        {
            if((int)cStr != (int)'.')
            {
                return false;
            }

            if(dwVal.Val > 0x00020505UL)
            {
                return false;
            }

            convAddr.v[currentOctet] = dwVal.v[2]*((uint8_t)100) + (dwVal.v[1]*((uint8_t)10)) + dwVal.v[0];
            currentOctet++;
            charLen = 0;
            dwVal.Val = 0;
            continue;
        }
        else
        {
            if((int)cStr == (int)'.')
            {
                if(dwVal.Val > 0x00020505UL)
                {
                    return false;
                }

                convAddr.v[currentOctet] = dwVal.v[2]*((uint8_t)100) + (dwVal.v[1]*((uint8_t)10)) + dwVal.v[0];
                currentOctet++;
                charLen = 0;
                dwVal.Val = 0;
                continue;
            }
            if(cAdj > 9u)
            {
                return false;
            }
        }

        charLen++;
        dwVal.Val <<= 8;
        dwVal.v[0] = cAdj;
    }

    // Make sure the very last character is a valid termination character 
    // (i.e., not more hostname, which could be legal and not an IP 
    // address as in "10.5.13.233.picsaregood.com"
    if(currentOctet != 3u)
    {
        return false;
    }

    if(!CheckIpStrEnd(cStr))
    {
        return false;
    }

    // Verify and convert the last octet and return the result
    if(dwVal.Val > 0x00020505UL)
    {
        return false;
    }

    convAddr.v[3] = dwVal.v[2]*((uint8_t)100) + (dwVal.v[1]*((uint8_t)10)) + dwVal.v[0];
    
    if(IPAddress != NULL)
    {
        IPAddress->Val = convAddr.Val;
    }

    return true;
}

static bool CheckIpStrEnd(uint8_t ch)
{
    bool checkEnd;

    if((ch == 0u) || (ch == (uint8_t)'/') || (ch == (uint8_t)'\r') || (ch == (uint8_t)'\n'))
    {
        checkEnd = true;
    }
    else if((ch == (uint8_t)' ') || (ch == (uint8_t)'\t') || (ch == (uint8_t)':'))
    {
        checkEnd = true;
    }
    else
    {
        checkEnd = false;
    }

    return checkEnd;
}

bool TCPIP_Helper_IPAddressToString(const IPV4_ADDR* IPAddress, char* buff, size_t buffSize)
{
    if((IPAddress != NULL) && (buff != NULL))
    {
        size_t len;
        char tempBuff[20];  // enough to hold largest IPv4 address string

        (void) FC_sprintf(tempBuff, sizeof(tempBuff), "%d.%d.%d.%d", IPAddress->v[0], IPAddress->v[1], IPAddress->v[2], IPAddress->v[3]);
        len = strlen(tempBuff) + 1U;
        if(buffSize >= len)
        {
            (void) strcpy(buff, tempBuff);
            return true;
        }
    }

    return false; 
}

//#if defined TCPIP_STACK_USE_IPV6
/*****************************************************************************
  Function:
    bool TCPIP_Helper_StringToIPv6Address(const char * addStr, IPV6_ADDR * addr)

  Summary:
    Converts a string to an IPv6 address

  Description:
    This function parses the text representation of an IPv6 address
    IPV6_ADDR struct.  The output result is big-endian.
    
  Precondition:
    None

  Parameters:
    addr - Pointer to IPV6_ADDR in which to store the result
    addStr - Pointer to an RFC3513, Section 2.2 text representation of
        an IPv6 address
        Example:    1111:2222:3333:4444:5555:6666:AAAA:FFFF
                    1111:2222::FFFF
                    1111:2222:3333:4444:5555:6666:192.168.1.20
                    [::1234],
                    etc.

  Return Values:
    true - an IP address was successfully decoded
    false - no IP address could be found, or the format was incorrect
  ***************************************************************************/
bool TCPIP_Helper_StringToIPv6Address(const char * addStr, IPV6_ADDR * addr)
{
    uint8_t shiftIndex = 0xFF;
    uint8_t subString[5];
    uint16_t convertedValue;
    int32_t len;
    int32_t conv_base = 16;
    uint8_t i, j;
    uint8_t currentWord;
    char * endPtr;
    char*  str;
    IPV6_ADDR   convAddr;
    char   str_buff[64 + 1];     // enough space for longest address: 1111:2222:3333:4444:5555:6666:192.250.250.250

    if(addr != NULL)
    {
        (void) memset(addr, 0, sizeof(*addr));
    }

    if(addStr == NULL)
    {
        return false;
    }
    len = (int32_t)strlen(addStr);
    if(len == 0)
    {
        return false;
    }

    (void) memset(convAddr.v, 0, sizeof(convAddr));

    while(isspace((int32_t)*addStr) != 0)
    {   // skip leading space
        addStr++;
        len--;
    }
    while(isspace((int32_t)*(addStr + len - 1)) != 0)
    {   // skip trailing space
        len--;
    }

    if((uint32_t)len > (sizeof(str_buff) - 1U))
    {   // not enough room to store
        return false;
    }

    (void) strncpy(str_buff, addStr, (uint32_t)len);
    str_buff[len] = (char)0;
    str = str_buff;

    if (*str == '[')
    {   // match the end ]
        if(str[len - 1] != ']')
        {   // bracket mismatch
            return false;
        }
        str[len - 1] = (char)0;   // delete trailing ]
        len--;
        str++;  // skip leading [
        len--;
    }

    currentWord = 0;
    while(isspace((int32_t)*str) != 0)
    {   // skip leading space
        str++;
        len--;
    }
    endPtr = str + len;
    while(isspace((int32_t)*(endPtr - 1)) != 0)
    {   // skip trailing space
        endPtr--;
    }
    *endPtr = (char)0;

    if(*str == ':')
    {
        if(*++str != ':')
        {   // cannot start with stray :
            return false;
        }
        str++;
        shiftIndex = 0;
    }

    i = (uint8_t)*str;

    if(!IsHexDigit(i))
    {
        return false;
    }

    str++;
    while ((i != (uint8_t)':') && (i != 0u) && (i != (uint8_t)'.') && (i != (uint8_t)'/') && (i != (uint8_t)'\r') && (i != (uint8_t)'\n') && (i != (uint8_t)' ') && (i != (uint8_t)'\t'))
    {
        j = 0;
        while ((i != (uint8_t)':') && (i != 0u) && (i != (uint8_t)'.') && (i != (uint8_t)'/') && (i != (uint8_t)'\r') && (i != (uint8_t)'\n') && (i != (uint8_t)' ') && (i != (uint8_t)'\t'))
        {
            if (j == 4U)
            {
                return false;
            }
                
            subString[j] = i;
            j++;
            i = (uint8_t)*str;
            str++;
        }
        subString[j] = 0;
        
        if(i == (uint8_t)'.')
        {
            conv_base = 10;
        }
        else if((i == (uint8_t)':') && (conv_base == 10))
        {
            return false;
        }
        else
        {
            /* Do Nothing */
        }

        errno = 0;
        convertedValue = (uint16_t)strtol((const char *)subString, &endPtr, conv_base);
        if(errno != 0)
        {
            return false;
        }

        if((convertedValue == 0U) && (endPtr != ((char*)subString + j)))
        {   // could not convert all data in there
            return false;
        }
        
        convAddr.w[currentWord] = TCPIP_Helper_htons(convertedValue);
        currentWord++;
        
        if(i == 0U)
        {   // end of stream
            break;
        }

        if (i == (uint8_t)':')
        {
            if (*str == ':')
            {
                // Double colon - pad with zeros here
                if (shiftIndex == 0xFFU)
                {
                    shiftIndex = currentWord;
                }
                else
                {
                    // Can't have two double colons
                    return false;
                }
                i = (uint8_t)*str;
                str++;
            }
        }
        
        if (i == (uint8_t)',')
        {
            return false;
        }
        
        i = (uint8_t)*str;
        str++;
    }

    if((currentWord > 8U) || ((currentWord < 8U) && (shiftIndex == 0xffU)))
    {   // more than 8 words entered or less, but no ::
        return false;
    }

    if (shiftIndex != 0xFFU)
    {
        i = 7;
        for(j = currentWord - 1U; (int8_t)j >= (int8_t)shiftIndex; j--)
        {
            convAddr.w[i] = convAddr.w[j];
            i--;
        }

        for (i = shiftIndex; i <= (7U - (currentWord - shiftIndex)); i++)
        {
            convAddr.w[i] = 0x0000;
        }
    }

    if(addr != NULL)
    {
        (void) memcpy(addr->v, convAddr.v, sizeof(*addr));
    }

    return true;
}

bool TCPIP_Helper_IPv6AddressToString (const IPV6_ADDR * addr, char* buff, size_t buffSize)
{
    if((addr != NULL) && (buff != NULL) && (buffSize >= 41U))
    {
        uint8_t i, j;
        char k;
        char* str = buff;

        for (i = 0; i < 8U; i++)
        {
            j = 0;
            k = (char)btohexa_high(addr->v[(i<<1)]);
            if (k != '0')
            {
                j = 1;
                *str = k;
                str++;
            }
            k = (char)btohexa_low(addr->v[(i<<1)]);
            if ((k != '0') || (j == 1U))
            {
                j = 1;
                *str = k;
                str++;
            }
            k = (char)btohexa_high(addr->v[1U + (i<<1)]);
            if ((k != '0') || (j == 1U))
            {
                *str = k;
                str++;
            }
            k = (char)btohexa_low(addr->v[1U + (i<<1)]);
            *str++ = k;
            if (i != 7U)
            {
                *str = ':';
                str++;
            }
        }
        *str = (char)0;

        return true;
    }

    return false;
}
//#endif

/*****************************************************************************
  Function:
    bool TCPIP_Helper_IsPrivateAddress(uint32_t ipv4Address)

  Summary:
    Detects a private (non-routable) address

  Description:
    This function checks if the passed in IPv4 address
    is a private or a routable address

  Precondition:
    None

  Parameters:
    ipv4Address - IPv4 address to check, network order

  Return Values:
    true - the IPv4 address is a private address
    false - the IPv4 address is a routable address
  ***************************************************************************/
bool TCPIP_Helper_IsPrivateAddress(uint32_t ipv4Address)
{
    size_t ix;
    const S_TCPIP_Helper_PrivateAddEntry* pEntry;

    pEntry = T_TCPIP_Helper_PrivateAddTbl;
    for(ix = 0; ix < sizeof(T_TCPIP_Helper_PrivateAddTbl) / sizeof(*T_TCPIP_Helper_PrivateAddTbl); ix++)
    {
        if((ipv4Address & pEntry->mask) == pEntry->address)
        {
            return true;
        }
        pEntry++;
    }


    return false;
}


/*****************************************************************************
  Function:
    bool TCPIP_Helper_StringToMACAddress(const char* str, uint8_t macAddr[6])

  Summary:
    Converts a string to an MAC address

  Description:
    This function parses a MAC address string "aa:bb:cc:dd:ee:ff"
    or "aa-bb-cc-dd-ee-ff" into an hex MAC address.

  Precondition:
    None

  Parameters:
    str - Pointer to a colon separated MAC address string
    macAddr - Pointer to buffer to store the result

  Return Values:
    true - a MAC address was successfully decoded
    false - no MAC address could be found, or the format was incorrect
  ***************************************************************************/
bool TCPIP_Helper_StringToMACAddress(const char* str, uint8_t macAddr[6])
{
    const char  *beg;
    TCPIP_UINT16_VAL    hexDigit;
    uint8_t     convAddr[6];
    uint8_t*    pAdd;
    int         ix;
    
    if(macAddr != NULL)
    {
        (void) memset(macAddr, 0, sizeof(convAddr));
    }

    if((str == NULL) || (strlen(str) == 0U))
    {
        return true;
    }

    beg = str;
    pAdd = convAddr;
    for(ix=0; ix<6; ix++)
    {
        if((!IsHexDigit((uint8_t)beg[0])) || (!IsHexDigit((uint8_t)beg[1])))
        {
            return false;
        }

        // found valid byte
        hexDigit.v[0] = (uint8_t)beg[1];
        hexDigit.v[1] = (uint8_t)beg[0];
        *pAdd = hexatob(hexDigit.Val);
        pAdd++;

        // next colon number
        beg += 2;
        if(beg[0] == '\0')
        {
            break;  // done
        }
        else if((beg[0] != ':') && (beg[0] != '-'))
        {
            return false;   // invalid delimiter
        }
        else
        {
            /* Do Nothing */
        }
        beg++; // next digit
    }

    if(macAddr != NULL)
    {
        (void) memcpy(macAddr, convAddr, sizeof(convAddr));
    }
    
    return (ix == 5) ? true : false;    // false if not enough digits    
    
}

bool TCPIP_Helper_MACAddressToString(const TCPIP_MAC_ADDR* macAddr, char* buff, size_t buffSize)
{
    if((macAddr != NULL) && (buff != NULL) && (buffSize >= 18U))
    {
        const uint8_t *pAdd = (const uint8_t*)macAddr;

        (void) FC_sprintf(buff, buffSize, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", *pAdd, *(pAdd+1), *(pAdd+2), *(pAdd+3), *(pAdd+4), *(pAdd+5));
        return true;
    }
    return false;
}

typedef struct
{
    TCPIP_MAC_POWER_MODE    pwrMode;    // the power mode
    const char*             pwrName;    // corresponding name
}TCPIP_MAC_POWER_ENTRY;

static const TCPIP_MAC_POWER_ENTRY TCPIP_MAC_POWER_TBL[] = 
{
    {TCPIP_MAC_POWER_NONE, TCPIP_STACK_IF_POWER_NONE},
    {TCPIP_MAC_POWER_FULL, TCPIP_STACK_IF_POWER_FULL},
    {TCPIP_MAC_POWER_LOW,  TCPIP_STACK_IF_POWER_LOW},
    {TCPIP_MAC_POWER_DOWN, TCPIP_STACK_IF_POWER_DOWN},
};



/*****************************************************************************
  Function:
    TCPIP_MAC_POWER_MODE TCPIP_Helper_StringToPowerMode(const char* str)

  Summary:
    Converts a string to a TCPIP stack power mode

  Description:
    This function parses a power mode string ("full", "low", "down", etc)
    to an internal TCPIP_MAC_POWER_MODE. 

  Precondition:
    None

  Parameters:
    str - Pointer to a power mode string

  Return Values:
    a valid TCPIP_MAC_POWER_MODE - a power mode was successfully decoded
    TCPIP_MAC_POWER_NONE - no power mode could be found
  ***************************************************************************/
TCPIP_MAC_POWER_MODE TCPIP_Helper_StringToPowerMode(const char* str)
{
    if(str != NULL)
    {
        size_t pwrIx;
        const TCPIP_MAC_POWER_ENTRY* pEntry;

        pEntry = TCPIP_MAC_POWER_TBL + 0;
        for(pwrIx = 0; pwrIx < (sizeof(TCPIP_MAC_POWER_TBL) / sizeof(*TCPIP_MAC_POWER_TBL)); pwrIx++)
        {
            if(pEntry->pwrName != NULL)
            {
                if(strcmp(str, pEntry->pwrName) == 0)
                {
                    return pEntry->pwrMode;
                }
            }
            pEntry++;
        }
    }

    return TCPIP_MAC_POWER_NONE;
}

const char* TCPIP_Helper_PowerModeToString(TCPIP_MAC_POWER_MODE mode)
{
    if((int)mode >= 0 && ((size_t)mode < (sizeof(TCPIP_MAC_POWER_TBL) / sizeof(*TCPIP_MAC_POWER_TBL))))
    {
        return (TCPIP_MAC_POWER_TBL + (size_t)mode)->pwrName;
    }

    return NULL;
}

/*****************************************************************************
  Function:
    uint16_t TCPIP_Helper_Base64Decode(const uint8_t* sourceData, uint16_t sourceLen, 
                        uint8_t* destData, uint16_t destLen)

  Summary:
    Helper function to decode a Base-64 encoded array.

  Description:
    Decodes a Base-64 array to its literal representation.
    
  Precondition:
    None

  Parameters:
    sourceData  - Pointer to a string of Base-64 encoded data
    sourceLen   - Length of the Base-64 source data
    destData    - Pointer to write the decoded data
    sourceLen   - Maximum length that can be written to destData

  Returns:
    Number of decoded bytes written to destData.
  
  Remarks:
    This function will ignore invalid Base-64 characters (CR, LF, etc).
    If sourceData is equal to destData, the data will be converted
    in-place.
    If sourceData is not equal to destData, but the regions 
    overlap, the behavior is undefined.
    
    Decoded data size is 3 / 4 the size of the encoded source data.
  ***************************************************************************/
uint16_t TCPIP_Helper_Base64Decode(const uint8_t* sourceData, uint16_t sourceLen, uint8_t* destData, uint16_t destLen)
{
    uint8_t i;
    uint8_t vByteNumber;
    uint16_t wBytesOutput;

    if((sourceData == NULL) || (destData == NULL))
    {
        return 0;
    }

    vByteNumber = 0;
    wBytesOutput = 0;

    // Loop over all provided bytes
    while(sourceLen-- != 0U)
    {
        // Fetch a Base64 byte and decode it to the original 6 bits
        i = *sourceData;
        sourceData++;
        if((i >= (uint8_t)'A') && (i <= (uint8_t)'Z'))  // Regular data
        {
            i -= (uint8_t)'A' - 0U;
        }
        else if((i >= (uint8_t)'a') && (i <= (uint8_t)'z'))
        {
            i -= (uint8_t)'a' - 26U;
        }
        else if((i >= (uint8_t)'0') && (i <= (uint8_t)'9'))
        {
            i -= (uint8_t)'0' - (uint8_t)52U;
        }
        else if((i == (uint8_t)'+') || (i == (uint8_t)'-'))
        {
            i = 62;
        }
        else if((i == (uint8_t)'/') || (i == (uint8_t)'_'))
        {
            i = 63;
        }
        else                        // Skip all padding (=) and non-Base64 characters
        {
            continue;
        }


        // Write the 6 bits to the correct destination location(s)
        if(vByteNumber == 0u)
        {
            vByteNumber++;
            if(wBytesOutput >= destLen)
            {
                break;
            }
            wBytesOutput++;
            *destData = i << 2;
        }
        else if(vByteNumber == 1u)
        {
            vByteNumber++;
            *destData |= i >> 4;
            destData++;
            if(wBytesOutput >= destLen)
            {
                break;
            }
            wBytesOutput++;
            *destData = i << 4;
        }
        else if(vByteNumber == 2u)
        {
            vByteNumber++;
            *destData |= i >> 2;
            destData++;
            if(wBytesOutput >= destLen)
            {
                break;
            }
            wBytesOutput++;
            *destData = i << 6;
        }
        else if(vByteNumber == 3u)
        {
            vByteNumber = 0;
            *destData    |= i;
            destData++;
        }
        else
        {
            /* Do Nothing */
        }
    }

    return wBytesOutput;
}

/*****************************************************************************
  Function:
    uint16_t TCPIP_Helper_Base64Encode(const uint8_t* sourceData, uint16_t sourceLen,
                        uint8_t* destData, uint16_t destLen)
  Summary:
    Helper function to encode to Base-64.

  Description:
    This function encodes a binary array to Base-64.
    
  Precondition:
    None

  Parameters:
    sourceData - Pointer to a string of binary data
    sourceLen   - Length of the binary source data
    destData    - Pointer to write the Base-64 encoded data
    destLen     - Maximum length that can be written to destData

  Returns:
    Number of encoded bytes written to destData.  This will always be
    a multiple of 4.
  
  Remarks:
    Encoding cannot be performed in-place.
    If surceData overlaps with  destData, the behavior is undefined.
    
    The source data is padded with 1 or 2 bytes, if needed, to make the source size a multiple
    of 3 bytes.
    Then for each 3 bytes tuple in the source 4 output bytes are generated.
    The output size needed is pad(sourceLen) * 4 / 3 bytes.

  ***************************************************************************/
uint16_t TCPIP_Helper_Base64Encode(const uint8_t* sourceData, uint16_t sourceLen, uint8_t* destData, uint16_t destLen)
{
    uint8_t i, j;
    uint8_t vOutput[4];
    uint16_t wOutputLen;

    if((sourceData == NULL) || (destData == NULL))
    {
        return 0;
    }

    wOutputLen = 0;
    while(destLen >= 4u)
    {
        // Start out treating the output as all padding
        vOutput[0] = 0xFF;
        vOutput[1] = 0xFF;
        vOutput[2] = 0xFF;
        vOutput[3] = 0xFF;

        // Get 3 input octets and split them into 4 output hextets (6-bits each) 
        if(sourceLen == 0u)
        {
            break;
        }
        i = *sourceData;
        sourceData++;
        sourceLen--;
        vOutput[0] = (i & 0xFCU)>>2U;
        vOutput[1] = (i & 0x03U)<<4U;
        if(sourceLen != 0U)
        {
            i = *sourceData;
            sourceData++;
            sourceLen--;
            vOutput[1] |= (i & 0xF0U)>>4U;
            vOutput[2] = (i & 0x0FU)<<2U;
            if(sourceLen != 0U)
            {
                i = *sourceData;
                sourceData++;
                sourceLen--;
                vOutput[2] |= (i & 0xC0U)>>6U;
                vOutput[3] = i & 0x3FU;
            }
        }
    
        // Convert hextets into Base 64 alphabet and store result
        for(i = 0; i < 4u; i++)
        {
            j = vOutput[i];

            if(j <= 25u)
            {
                j += (uint8_t)'A' - (uint8_t)0;
            }
            else if(j <= 51u)
            {
                j += (uint8_t)'a' - (uint8_t)26;
            }
            else if(j <= 61u)
            {
                j += (uint8_t)'0' - (uint8_t)52;
            }
            else if(j == 62u)
            {
                j = (uint8_t)'+';
            }
            else if(j == 63u)
            {
                j = (uint8_t)'/';
            }
            else                // Padding
            {
                j = (uint8_t)'=';
            }

            *destData = j;
            destData++;
        }

        // Update counters
        destLen -= 4U;
        wOutputLen += 4U;
    }

    return wOutputLen;
}


/*****************************************************************************
  Function:
    uint16_t TCPIP_Helper_CalcIPChecksum(const uint8_t* dataBuffer, uint16_t len, uint16_t seed)

  Summary:
    Calculates an IP checksum value.

  Description:
    This function calculates an IP checksum over an array of input data.  The
    checksum is the 16-bit one's complement of one's complement sum of all 
    words in the data (with zero-padding if an odd number of bytes are 
    summed).  This checksum is defined in RFC 793.

  Precondition:
    None

  Parameters:
    dataBuffer - pointer to the data to be checksummed
    len  - number of bytes to be checksummed
    seed   - start seed

  Returns:
    The calculated checksum.
    
  Note:
    The checksum is implemented as a fast assembly function on PIC32M platforms.
  ***************************************************************************/
#if !defined(__mips__)
uint16_t TCPIP_Helper_CalcIPChecksum(const uint8_t* dataBuffer, uint16_t len, uint16_t seed)
{
    uint16_t i;
    uint16_t *pVal;
    union
    {
        uint8_t  b[4];
        uint16_t w[2];
        uint32_t dw;
    } sum;
    
    if(dataBuffer == NULL)
    {
        return 0;
    }
    
    // Calculate the sum of all words
    sum.dw = (uint32_t)seed;
    if (((uint32_t)dataBuffer % 2U) != 0U)
    {
        uint16_t tSum = *dataBuffer ;
        sum.w[0] += tSum << 8;
        pVal = FC_Cptr82Ptr16(dataBuffer + 1U);
        len--;
    }
    else
    {
        pVal = FC_Cptr82Ptr16(dataBuffer);
    }

    i = len >> 1;

    while(i-- != 0U)
    {
        sum.dw += (uint32_t)*pVal;
        pVal++;
    }

    // Add in the sum of the remaining byte, if present
    if((len & 0x1U) != 0U)
    {
        sum.dw += (uint32_t)*(uint8_t*)pVal;
    }

    // Do an end-around carry (one's complement arrithmatic)
    sum.dw = (uint32_t)sum.w[0] + (uint32_t)sum.w[1];

    // Do another end-around carry in case if the prior add 
    // caused a carry out
    sum.w[0] += sum.w[1];

    if (((uint32_t)dataBuffer % 2U) != 0U)
    {
        sum.w[0] = ((uint16_t)sum.b[0] << 8 ) | (uint16_t)sum.b[1];
    }

    // Return the resulting checksum
    return ~sum.w[0];
}

// This version of  TCPIP_Helper_Memcpy (without standard library memcpy) 
// is tested on Cortex-A7, Cortex-A5, Cortex-M4, Cortex-M7, Cortex-M33.
// This is a lightweight routine with higher performance.But devices that do not
// support/disabled unaligned memory access must not use this version.
#if (!defined(UNALIGNED_SUPPORT_DISABLE))  &&  \
    ((defined(__CORTEX_A) && ((__CORTEX_A == 5U) || (__CORTEX_A == 7U))) ||  \
     (defined(__CORTEX_M) && ((__CORTEX_M == 4U) || (__CORTEX_M == 7U) || \
     (__CORTEX_M == 33U))))
void TCPIP_Helper_Memcpy (void *dst, const void *src, size_t len)
{
#define WORD_ALIGN_MASK 0x00000003U
    uint32_t* dst_32;
    const uint32_t* src_32;
    uint8_t* dst_8;
    const uint8_t* src_8;
    uint32_t count = 0;
    uint32_t remaining_bytes = 0;

    dst_32 = (uint32_t*)dst;
    src_32 = (const uint32_t*)src;

    count = ((uint32_t)len) >> 2;
    remaining_bytes = ((uint32_t)len) & WORD_ALIGN_MASK;  
    while (count-- != 0U)
    {
        *dst_32++ = *src_32++;
    } 
    if(remaining_bytes != 0U)
    {
        dst_8 = (uint8_t*)dst_32;
        src_8 = (const uint8_t*)src_32; 
        while (remaining_bytes-- != 0U)
        {
            *dst_8++ = *src_8++;
        }
    }    
}   
#else
void TCPIP_Helper_Memcpy (void *dst, const void *src, size_t len)
{
#define WORD_ALIGN_MASK 0x00000003U
    uint32_t* dst_32;
    const uint32_t* src_32;
    uint8_t* dst_8;
    const uint8_t* src_8;
    uint32_t count = 0;
    uint32_t remaining_bytes = 0;

    if(((uint32_t)dst & WORD_ALIGN_MASK) || ((uint32_t)src & WORD_ALIGN_MASK))
    {
        memcpy(dst, src, len);
    }
    else
    {       
        dst_32 = (uint32_t*)dst;
        src_32 = (const uint32_t*)src;

        count = ((uint32_t)len) >> 2;
        remaining_bytes = ((uint32_t)len) & WORD_ALIGN_MASK;  
        while (count-- != 0U)
        {
            *dst_32++ = *src_32++;
        } 
        if(remaining_bytes != 0U)
        {
            dst_8 = (uint8_t*)dst_32;
            src_8 = (const uint8_t*)src_32; 
            while (remaining_bytes-- != 0U)
            {
                *dst_8++ = *src_8++;
            }
        }

    }    
}  
#endif  // DEVICE_ARCH 
#endif  // !defined(__mips__)

// calculates the IP checksum for a packet with multiple segments
uint16_t TCPIP_Helper_PacketChecksum(TCPIP_MAC_PACKET* pPkt, uint8_t* startAdd, uint16_t len, uint16_t seed)
{
    TCPIP_MAC_DATA_SEGMENT  *pSeg;
    uint8_t* pChkBuff;
    uint16_t checkLength, chkBytes, nBytes;
    uint16_t segChkSum;
    uint32_t calcChkSum;

    if(len == 0U)
    {
        return seed;
    }

    calcChkSum = seed;
    checkLength = len;
    nBytes = 0;
    pChkBuff = startAdd; 
    pSeg = TCPIP_PKT_DataSegmentGet(pPkt, startAdd, true);

    while((pSeg != NULL) && (checkLength != 0U))
    {
        chkBytes = FC_Int322Uint16((pSeg->segLoad + pSeg->segSize) - pChkBuff);

        if((pSeg->segLen != 0U) && (chkBytes > pSeg->segLen))
        {   // segLen must be non-zero to avoid an infinite loop
            chkBytes = pSeg->segLen;
        } 

        if(chkBytes > checkLength)
        {
            chkBytes = checkLength;
        } 

        if(chkBytes != 0U)
        {
            segChkSum = ~TCPIP_Helper_CalcIPChecksum(pChkBuff, chkBytes, 0);
            if((nBytes & 0x1U) != 0U)
            {
                segChkSum = TCPIP_Helper_htons(segChkSum);
            }

            checkLength -= chkBytes;
            nBytes += chkBytes;
            calcChkSum += segChkSum;
        }
        
        pSeg = pSeg->next;
        
        if(pSeg != NULL)
        {
            pChkBuff = pSeg->segLoad;
        }
#if defined(TCPIP_IPV4_FRAGMENTATION) && (TCPIP_IPV4_FRAGMENTATION != 0)
        else if((pPkt = pPkt->pkt_next) != NULL)
        {
            pSeg = pPkt->pDSeg;
            pChkBuff = pPkt->pNetLayer;
        }
#endif  // defined(TCPIP_IPV4_FRAGMENTATION) && (TCPIP_IPV4_FRAGMENTATION != 0)
        else
        {
            // Do nothing
        }
    }

    return ~TCPIP_Helper_ChecksumFold(calcChkSum);
}

uint16_t TCPIP_Helper_ChecksumFold(uint32_t rawChksum)
{
    TCPIP_UINT32_VAL checksum1, checksum2;

    checksum1.Val = rawChksum;   // init checksum

    checksum2.Val = (uint32_t)checksum1.w[0] + (uint32_t)checksum1.w[1];   // checksum = low 16 bit + high 16 bit
    checksum2.w[0] += checksum2.w[1];
    return checksum2.w[0];
    
}

// copies packet segment data to a linear destination buffer
// updates the pointer to the current location in the packet segment for further copy
// returns the number of total bytes copied
uint16_t TCPIP_Helper_PacketCopy(TCPIP_MAC_PACKET* pSrcPkt, uint8_t* pDest, uint8_t** pStartAdd, uint16_t len, bool srchTransport)
{
    TCPIP_MAC_DATA_SEGMENT* pSeg;
    uint16_t copyLen, copyBytes;
    uint8_t  *pCopyBuff, *pSrcBuff;
    uint16_t totCopyBytes = 0;

    copyLen = len;
    pCopyBuff = *pStartAdd; 
    pSrcBuff = *pStartAdd; 
    pSeg = TCPIP_PKT_DataSegmentGet(pSrcPkt, pSrcBuff, srchTransport);

    while((pSeg != NULL) && (copyLen != 0U))
    {
        copyBytes = FC_U8PtrDiff2UI16((pSeg->segLoad + pSeg->segSize), pCopyBuff);

        if(copyBytes > pSeg->segLen)
        {
            copyBytes = pSeg->segLen;
        } 

        if(copyBytes > copyLen)
        {
            copyBytes = copyLen;
        } 

        if(copyBytes != 0U)
        {
            (void) memcpy(pDest, pCopyBuff, copyBytes);
            pDest += copyBytes;
            copyLen -= copyBytes;
            pSrcBuff = pCopyBuff + copyBytes;
            totCopyBytes += copyBytes;
        }

        pSeg = pSeg->next;
        if(pSeg != NULL)
        {
            pCopyBuff = pSeg->segLoad;
        }
    }
    
    *pStartAdd = pSrcBuff;

    return totCopyBytes;
}
  

/*****************************************************************************
  Function:
    void TCPIP_Helper_FormatNetBIOSName(uint8_t Name[])

  Summary:
    Formats a string to a valid NetBIOS name.

  Description:
    This function formats a string to a valid NetBIOS name.  Names will be
    exactly 16 characters, as defined by the NetBIOS spec.  The 16th 
    character will be a 0x00 byte, while the other 15 will be the 
    provided string, padded with spaces as necessary.

  Precondition:
    None

  Parameters:
    Name - the string to format as a NetBIOS name.  This parameter must have
      at least 16 bytes allocated.

  Returns:
    None
  ***************************************************************************/
void TCPIP_Helper_FormatNetBIOSName(uint8_t Name[])
{
    uint8_t i;

    Name[15] = (uint8_t)'\0';
    i = 0;
    while(i < 15u)
    {
        Name[i] = (uint8_t)toupper((int32_t)Name[i]);
        if(Name[i] == (uint8_t)'\0')
        {
            while(i < 15u)
            {
                Name[i] = (uint8_t)' ';
                i++;
            }
            break;
        }
        i++;
    }
}

uint8_t TCPIP_Helper_FindCommonPrefix (const uint8_t* addr1, const uint8_t* addr2, uint8_t bytes)
{
    uint8_t i = 0;
    uint8_t matchLen = 0;
    uint8_t mask = 0x80;
    uint8_t j, k;

    while (i < bytes)
    {
        j = *addr1;
        k = *addr2;
        if ((j ^ k) == 0U)
        {
            matchLen += 8U;
        }
        else
        {
            while ((mask & ~(j ^ k)) != 0U)
            {
                matchLen++;
                mask >>=1;
            }
            break;
        }
        i++;
        addr1++;
        addr2++;
    }

    return matchLen;
}


// lists implementation: single and double linked

void  TCPIP_Helper_SingleListInitialize(SINGLE_LIST* pL)
{
    pL->head = NULL;
    pL->tail = NULL;
    pL->nNodes = 0U;
}



void  TCPIP_Helper_SingleListHeadAdd(SINGLE_LIST* pL, SGL_LIST_NODE* pN)
{
    pN->next = pL->head;
    pL->head = pN;
    if(pL->tail == NULL)
    {  // empty list
        pL->tail = pN;
    }
    pL->nNodes++;
}

void  TCPIP_Helper_SingleListTailAdd(SINGLE_LIST* pL, SGL_LIST_NODE* pN)
{
    pN->next = NULL;
    if(pL->tail == NULL)
    {
        pL->head = pN;
        pL->tail = pN;
    }
    else
    {
        pL->tail->next = pN;
        pL->tail = pN;
    }
    pL->nNodes++;
}


// insertion in the middle, not head or tail
void  TCPIP_Helper_SingleListMidAdd(SINGLE_LIST* pL, SGL_LIST_NODE* pN, SGL_LIST_NODE* after)
{
    pN->next = after->next;
    after->next = pN;
    pL->nNodes++; 
}

// insertion at head, at tail or in the middle
void  TCPIP_Helper_SingleListAdd(SINGLE_LIST* pL, SGL_LIST_NODE* pN, SGL_LIST_NODE* after)
{
    if(after == NULL)
    {
        TCPIP_Helper_SingleListHeadAdd(pL, pN);
    }
    else if(after == pL->tail)
    {
        TCPIP_Helper_SingleListTailAdd(pL, pN);
    }
    else
    {   // node somewhere in the middle; search
        TCPIP_Helper_SingleListMidAdd(pL, pN, after);
    }
}

SGL_LIST_NODE*  TCPIP_Helper_SingleListHeadRemove(SINGLE_LIST* pL)
{
    SGL_LIST_NODE* pN = pL->head;
    if(pN != NULL)
    {
        if(pL->head == pL->tail)
        {
            pL->head = NULL;
            pL->tail = NULL;
        }
        else
        {
            pL->head = pN->next;
        }
        pL->nNodes--;
    }

    return pN;
}

// removes the next node (following prev) in the list
// if prev == 0 removed the head
SGL_LIST_NODE*  TCPIP_Helper_SingleListNextRemove(SINGLE_LIST* pL, SGL_LIST_NODE* prev)
{
    SGL_LIST_NODE*  pN;

    if(prev == NULL)
    {
        return TCPIP_Helper_SingleListHeadRemove(pL);
    }

    pN = prev->next;
    if(pN != NULL)
    {
        prev->next = pN->next;
        if(pN == pL->tail)
        {
            pL->tail = prev;
        }
        pL->nNodes--;
    }

    return pN;


}


// removes a node somewhere in the middle
// Note: this is lengthy!
// Use a double linked list if faster operation needed!
SGL_LIST_NODE*  TCPIP_Helper_SingleListNodeRemove(SINGLE_LIST* pL, SGL_LIST_NODE* pN)
{
    if(pN == pL->head)
    {
        (void) TCPIP_Helper_SingleListHeadRemove(pL);
    }
    else
    {
        SGL_LIST_NODE* prev;
        for(prev = pL->head; (prev != NULL) && (prev->next != pN); prev = prev->next)
        {
            /* Do Nothing */
        }
        if(prev == NULL)
        {   // no such node
            return NULL;
        }
        // found it
        prev->next = pN->next;
        if(pN == pL->tail)
        {
            pL->tail = prev;
        }
        pL->nNodes--;
    }

    return pN;
}

bool TCPIP_Helper_SingleListFind(SINGLE_LIST* pL, SGL_LIST_NODE* pN)
{
    SGL_LIST_NODE* node;
    for(node = pL->head; node != NULL ; node = node->next)
    {
        if(node == pN)
        {
            return true;
        }
    }

    return false;
}

void  TCPIP_Helper_SingleListAppend(SINGLE_LIST* pDstL, SINGLE_LIST* pAList)
{
    SGL_LIST_NODE* pN;
    while(true)
    {
        pN = TCPIP_Helper_SingleListHeadRemove(pAList);
        if(pN == NULL)
        {
            break;
        }

        TCPIP_Helper_SingleListTailAdd(pDstL, pN);
    }
}


// Protected Single linked list manipulation

bool  TCPIP_Helper_ProtSglListInitialize(PROTECTED_SINGLE_LIST* pL)
{
    TCPIP_Helper_SingleListInitialize(&pL->list);
    pL->semValid = (OSAL_SEM_Create(&pL->semaphore, OSAL_SEM_TYPE_BINARY, 1, 1) == OSAL_RESULT_SUCCESS);
    return pL->semValid;
}

void  TCPIP_Helper_ProtSglListDeinitialize(PROTECTED_SINGLE_LIST* pL)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        TCPIP_Helper_SingleListRemoveAll(&pL->list);
        (void) OSAL_SEM_Delete(&pL->semaphore);
        pL->semValid = false;
    }
}

void  TCPIP_Helper_ProtSglListHeadAdd(PROTECTED_SINGLE_LIST* pL, SGL_LIST_NODE* pN)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        TCPIP_Helper_SingleListHeadAdd(&pL->list, pN);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
    }
}

void  TCPIP_Helper_ProtSglListTailAdd(PROTECTED_SINGLE_LIST* pL, SGL_LIST_NODE* pN)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        TCPIP_Helper_SingleListTailAdd(&pL->list, pN);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
    }

}


// insertion in the middle, not head or tail
void  TCPIP_Helper_ProtSglListMidAdd(PROTECTED_SINGLE_LIST* pL, SGL_LIST_NODE* pN, SGL_LIST_NODE* after)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        TCPIP_Helper_SingleListMidAdd(&pL->list, pN, after);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
    }
}

void TCPIP_Helper_ProtSglListAdd(PROTECTED_SINGLE_LIST* pL, SGL_LIST_NODE* pN, SGL_LIST_NODE* after)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        TCPIP_Helper_SingleListAdd(&pL->list, pN, after);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
    }
}

// removes the head node
SGL_LIST_NODE*  TCPIP_Helper_ProtSglListHeadRemove(PROTECTED_SINGLE_LIST* pL)
{

    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        SGL_LIST_NODE * ret = TCPIP_Helper_SingleListHeadRemove(&pL->list);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        return ret;
    }

    return NULL;

}

// removes the next node (following prev) in the list
// if prev == 0 removed the head
SGL_LIST_NODE*  TCPIP_Helper_ProtSglListNextRemove(PROTECTED_SINGLE_LIST* pL, SGL_LIST_NODE* prev)
{

    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        SGL_LIST_NODE * ret = TCPIP_Helper_SingleListNextRemove(&pL->list, prev);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        return ret;
    }
    return NULL;
}



// removes a node anywhere in the list
// Note: this is lengthy!
// Use a double linked list if faster operation needed!
SGL_LIST_NODE*  TCPIP_Helper_ProtSglListNodeRemove(PROTECTED_SINGLE_LIST* pL, SGL_LIST_NODE* pN)
{

    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        SGL_LIST_NODE * ret = TCPIP_Helper_SingleListNodeRemove(&pL->list, pN);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        return ret;
    }

    return NULL;
}



void  TCPIP_Helper_ProtSglListAppend(PROTECTED_SINGLE_LIST* pDstL, SINGLE_LIST* pAList)
{
    if(pDstL->semValid)
    {
        if (OSAL_SEM_Pend(&pDstL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        TCPIP_Helper_SingleListAppend(&pDstL->list, pAList);
        if (OSAL_SEM_Post(&pDstL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
    }
}



void TCPIP_Helper_ProtSglListRemoveAll(PROTECTED_SINGLE_LIST* pL)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        TCPIP_Helper_SingleListRemoveAll(&pL->list);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
    }
}

bool TCPIP_Helper_ProtSglListLock(PROTECTED_SINGLE_LIST* pL)
{
    if(pL->semValid)
    {
        return (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) == OSAL_RESULT_SUCCESS);
    }

    return false;
}

bool TCPIP_Helper_ProtSglListUnlock(PROTECTED_SINGLE_LIST* pL)
{
    if(pL->semValid)
    {
        return (OSAL_SEM_Post(&pL->semaphore) == OSAL_RESULT_SUCCESS);
    }

    return false;
}


/////  double linked lists manipulation ///////////
//


void  TCPIP_Helper_DoubleListInitialize(DOUBLE_LIST* pL)
{
    pL->head = NULL;
    pL->tail = NULL;
    pL->nNodes = 0U;
}


void  TCPIP_Helper_DoubleListHeadAdd(DOUBLE_LIST* pL, DBL_LIST_NODE* pN)
{
    if(pL->head == NULL)
    { // empty list, first node
        pL->head = pN;
        pL->tail = pN;
        pN->next = NULL;
        pN->prev = NULL;
    }
    else
    {
        pN->next = pL->head;
        pN->prev = NULL;
        pL->head->prev = pN;
        pL->head = pN;
    }       
    pL->nNodes++;
}

void  TCPIP_Helper_DoubleListTailAdd(DOUBLE_LIST* pL, DBL_LIST_NODE* pN)
{
    if(pL->head == NULL)
    { // empty list, first node
        pL->head = pN;
        pL->tail = pN;
        pN->next = NULL;
        pN->prev = NULL;
    }
    else
    {
        pN->next = NULL;
        pN->prev = pL->tail;
        pL->tail->next = pN;
        pL->tail = pN;
    }       
    pL->nNodes++;
}

// add node pN in the middle, after existing node "after"
void  TCPIP_Helper_DoubleListMidAdd(DOUBLE_LIST* pL, DBL_LIST_NODE* pN, DBL_LIST_NODE* after)
{
    pN->next = after->next;
    pN->prev = after;
    after->next->prev = pN;
    after->next = pN;
    pL->nNodes++;
}

void  TCPIP_Helper_DoubleListAdd(DOUBLE_LIST* pL, DBL_LIST_NODE* pN, DBL_LIST_NODE* after)
{
    if(after == NULL)
    {
        TCPIP_Helper_DoubleListHeadAdd(pL, pN);
    }
    else if(after == pL->tail)
    {
        TCPIP_Helper_DoubleListTailAdd(pL, pN);
    }
    else
    {   // node somewhere in the middle; search
        TCPIP_Helper_DoubleListMidAdd(pL, pN, after);
    }
}

DBL_LIST_NODE*  TCPIP_Helper_DoubleListHeadRemove(DOUBLE_LIST* pL)
{
    DBL_LIST_NODE* pN = pL->head;
    if(pN != NULL)
    {
        if(pL->head == pL->tail)
        {
            pL->head = NULL;
            pL->tail = NULL;
        }
        else
        {
            pL->head = pN->next;
            pL->head->prev = NULL;
        }
        pL->nNodes--;
    }
    return pN;
}

DBL_LIST_NODE*  TCPIP_Helper_DoubleListTailRemove(DOUBLE_LIST* pL)
{
    DBL_LIST_NODE* pN = pL->tail;
    if(pN != NULL)
    {
        if(pL->head == pL->tail)
        {
            pL->head = NULL;
            pL->tail = NULL;
        }
        else
        {
            pL->tail = pN->prev;
            pL->tail->next = NULL;
        }
        pL->nNodes--;
    }
    return pN;
}

// remove existing node, neither head, nor tail
void  TCPIP_Helper_DoubleListMidRemove(DOUBLE_LIST* pL, DBL_LIST_NODE* pN)
{
    (pN)->prev->next = (pN)->next;
    (pN)->next->prev = (pN)->prev;
    pL->nNodes--;
}

void  TCPIP_Helper_DoubleListNodeRemove(DOUBLE_LIST* pL, DBL_LIST_NODE* pN)
{
    if(pN == pL->head)
    {
        (void) TCPIP_Helper_DoubleListHeadRemove(pL);
    }
    else if(pN == pL->tail)
    {
        (void) TCPIP_Helper_DoubleListTailRemove(pL);
    }
    else
    {
        TCPIP_Helper_DoubleListMidRemove(pL, pN);
    }
}

bool TCPIP_Helper_DoubleListFind(DOUBLE_LIST* pL, DBL_LIST_NODE* pN)
{
    DBL_LIST_NODE* node;
    for(node = pL->head; node != NULL ; node = node->next)
    {
        if(node == pN)
        {
            return true;
        }
    }

    return false;
}



bool  TCPIP_Helper_ProtDblListInitialize(PROTECTED_DOUBLE_LIST* pL)
{
    TCPIP_Helper_DoubleListInitialize(&pL->list);
    pL->semValid = (OSAL_SEM_Create(&pL->semaphore, OSAL_SEM_TYPE_BINARY, 1, 1) == OSAL_RESULT_SUCCESS);
    return pL->semValid;
}

void  TCPIP_Helper_ProtDblListDeinitialize(PROTECTED_DOUBLE_LIST* pL)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        TCPIP_Helper_DoubleListRemoveAll(&pL->list);
        (void) OSAL_SEM_Delete(&pL->semaphore);
        pL->semValid = false;
    }
}


void  TCPIP_Helper_ProtDblListHeadAdd(PROTECTED_DOUBLE_LIST* pL, DBL_LIST_NODE* pN)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        TCPIP_Helper_DoubleListHeadAdd(&pL->list, pN);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
    }
}

void  TCPIP_Helper_ProtDblListTailAdd(PROTECTED_DOUBLE_LIST* pL, DBL_LIST_NODE* pN)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        TCPIP_Helper_DoubleListTailAdd(&pL->list, pN);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
    }
}


// insertion in the middle, not head or tail
void  TCPIP_Helper_ProtDblListMidAdd(PROTECTED_DOUBLE_LIST* pL, DBL_LIST_NODE* pN, DBL_LIST_NODE* after)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        TCPIP_Helper_DoubleListMidAdd(&pL->list, pN, after);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
    }
}

void  TCPIP_Helper_ProtDblListAdd(PROTECTED_DOUBLE_LIST* pL, DBL_LIST_NODE* pN, DBL_LIST_NODE* after)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        TCPIP_Helper_DoubleListAdd(&pL->list, pN, after);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
    }
}

// removes the head node
DBL_LIST_NODE*  TCPIP_Helper_ProtDblListHeadRemove(PROTECTED_DOUBLE_LIST* pL)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        DBL_LIST_NODE * ret = TCPIP_Helper_DoubleListHeadRemove(&pL->list);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        return ret;
    }

    return NULL;
}

// removes the next node (following prev) in the list
// if prev == 0 removed the head
DBL_LIST_NODE*  TCPIP_Helper_ProtDblListTailRemove(PROTECTED_DOUBLE_LIST* pL)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        DBL_LIST_NODE * ret = TCPIP_Helper_DoubleListTailRemove(&pL->list);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        return ret;
    }

    return NULL;
}

void  TCPIP_Helper_ProtDblListMidRemove(PROTECTED_DOUBLE_LIST* pL, DBL_LIST_NODE* pN)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        TCPIP_Helper_DoubleListMidRemove(&pL->list, pN);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
    }
}


void  TCPIP_Helper_ProtDblListNodeRemove(PROTECTED_DOUBLE_LIST* pL, DBL_LIST_NODE* pN)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        TCPIP_Helper_DoubleListNodeRemove(&pL->list, pN);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
    }
}

void TCPIP_Helper_ProtDblListRemoveAll(PROTECTED_DOUBLE_LIST* pL)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
        TCPIP_Helper_DoubleListRemoveAll(&pL->list);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
    }
}

void TCPIP_Helper_ProtDblListLock(PROTECTED_DOUBLE_LIST* pL)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
    }
}

void TCPIP_Helper_ProtDblListUnlock(PROTECTED_DOUBLE_LIST* pL)
{
    if(pL->semValid)
    {
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_SUCCESS)
        {
            //SYS_DEBUG LOG
        }
    }
}

#if M_TCPIP_STACK_SECURE_PORT_ENTRIES != 0

// returns the corresponding port entry in the tcpSecurePortTbl
// if not found, could store a free entry location in the table if needed
static TCPIP_HELPER_PORT_ENTRY* F_TCPIP_Helper_SecurePortEntry(uint16_t port, TCPIP_HELPER_PORT_ENTRY** pFreeEntry)
{
    TCPIP_HELPER_PORT_ENTRY* pEntry, *pFree;
    size_t ix;

    pEntry = tcpSecurePortTbl;
    pFree = NULL;
    for (ix = 0; ix < (sizeof(tcpSecurePortTbl) / sizeof(*tcpSecurePortTbl)); ix++)
    {
        if(pEntry->port == 0U)
        {
            if(pFree == NULL)
            {
                pFree = pEntry;
            }
        }
        else if(pEntry->port == port)
        {
            return pEntry;
        }
        else
        {
            /* Do Nothing */
        }
        pEntry++;
    }

    if(pFreeEntry != NULL)
    {
        *pFreeEntry = pFree;
    }

    return NULL;

}

bool TCPIP_Helper_TCPSecurePortGet(uint16_t tcpPort)
{
    TCPIP_HELPER_PORT_ENTRY* pEntry = F_TCPIP_Helper_SecurePortEntry(tcpPort, NULL);

    if(pEntry != NULL)
    {   // found port
        return (pEntry->flags & (uint16_t)TCPIP_HELPER_PORT_FLAG_STREAM) != 0U;
    }

    return false;
}

bool TCPIP_Helper_UDPSecurePortGet(uint16_t udpPort)
{
    TCPIP_HELPER_PORT_ENTRY* pEntry = F_TCPIP_Helper_SecurePortEntry(udpPort, NULL);

    if(pEntry != NULL)
    {   // found port
        return (pEntry->flags & (uint16_t)TCPIP_HELPER_PORT_FLAG_DGRAM) != 0U;
    }

    return false;
}

uint16_t TCPIP_Helper_SecurePortGetByIndex(size_t index, bool streamSocket, size_t* pnIndexes)
{
    TCPIP_HELPER_PORT_FLAGS entryFlags = streamSocket ? TCPIP_HELPER_PORT_FLAG_STREAM : TCPIP_HELPER_PORT_FLAG_DGRAM;

    if(pnIndexes != NULL)
    {
        *pnIndexes = sizeof(tcpSecurePortTbl) / sizeof(*tcpSecurePortTbl);
    }

    if(index < sizeof(tcpSecurePortTbl) / sizeof(*tcpSecurePortTbl))
    {
        TCPIP_HELPER_PORT_ENTRY* pEntry = tcpSecurePortTbl + index;
        if(pEntry->port != 0U)
        {
            if((pEntry->flags & (uint16_t)entryFlags) == (uint16_t)entryFlags)
            {
                return pEntry->port;
            }
        }
    }

    return 0;
}



bool TCPIP_Helper_SecurePortSet(uint16_t port, bool streamSocket, bool isSecure)
{
    TCPIP_HELPER_PORT_ENTRY* pEntry, *pFree = NULL;

    if(port == 0U)
    {   // invalid port
        return false;
    }

    TCPIP_HELPER_PORT_FLAGS entryFlags = streamSocket ? TCPIP_HELPER_PORT_FLAG_STREAM : TCPIP_HELPER_PORT_FLAG_DGRAM;

    pEntry = F_TCPIP_Helper_SecurePortEntry(port, &pFree);

    if(isSecure)
    {
        if(pEntry == NULL)
        {   // not in there already
            if(pFree == NULL)
            {   // no more room
                return false;
            }
            pFree->port = port;
            pFree->flags = (uint16_t)entryFlags;
        }
        else
        {   // make sure is flagged properly
            pEntry->flags |= (uint16_t)entryFlags;
        }
        return true;
    }

    // non secure port: need to remove
    if(pEntry != NULL)
    {
        pEntry->flags &= ~(uint16_t)entryFlags;
        if(pEntry->flags == 0U)
        {   // done, free it
            pEntry->port = 0;
        }
    }
    // else not even in there

    return true;
}

#else
bool TCPIP_Helper_TCPSecurePortGet(uint16_t tcpPort)
{
    return false;
}

bool TCPIP_Helper_UDPSecurePortGet(uint16_t udpPort)
{
    return false;
}

uint16_t TCPIP_Helper_SecurePortGetByIndex(size_t index, bool streamSocket, size_t* pnIndexes)
{
    if(pnIndexes)
    {
        *pnIndexes = 0;
    }

    return 0;
}

bool TCPIP_Helper_SecurePortSet(uint16_t port, bool streamSocket, bool isSecure)
{
    return false;
}

#endif  // M_TCPIP_STACK_SECURE_PORT_ENTRIES != 0

// conversion functions/helpers

/* MISRA C-2012 Rule 17.1 deviated:3 Deviation record ID -  H3_MISRAC_2012_R_17_1_NET_DR_2 */
/* MISRA C-2012 Rule 21.6 deviated:1 Deviation record ID -  H3_MISRAC_2012_R_21_6_NET_DR_3 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:3 "MISRA C-2012 Rule 17.1" "H3_MISRAC_2012_R_17_1_NET_DR_2" 
#pragma coverity compliance block deviate:1 "MISRA C-2012 Rule 21.6" "H3_MISRAC_2012_R_21_6_NET_DR_3" 
// safe sprintf function
int FC_sprintf(char* buff, size_t buffSize, const char* fmt, ...)
{
    int needBytes;
    
    va_list args;
    va_start( args, fmt );

    needBytes = vsnprintf(buff, buffSize, fmt, args);
    TCPIPStack_Assert(needBytes >= 0, __FILE__, __func__, __LINE__);

    va_end( args );

    TCPIPStack_Assert((size_t)needBytes <= buffSize, __FILE__, __func__, __LINE__);
    return needBytes;
}

#pragma coverity compliance end_block "MISRA C-2012 Rule 17.1"
#pragma coverity compliance end_block "MISRA C-2012 Rule 21.6"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */

/* MISRA C-2012 Rule 21.9 deviated:1 Deviation record ID -  H3_MISRAC_2012_R_21_9_NET_DR_4 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:1 "MISRA C-2012 Rule 21.9" "H3_MISRAC_2012_R_21_9_NET_DR_4" 
// sort function helper
void FC_Sort (void * base, size_t numElem, size_t elemSize, int (*comparF)(const void* p1, const void* p2))
{
    qsort(base, numElem, elemSize, comparF);
}
#pragma coverity compliance end_block "MISRA C-2012 Rule 21.9"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */


// string to int conversion
// puRes != NULL or psRes != NULL decide which conversion is made: unsigned/signed
static int FC_Str2Int(const char* str, int base, uint32_t* puRes, int32_t*  psRes)
{
    uint32_t ulVal;
    int32_t  lVal;
    char*    endPtr;
    bool     isUnsigned;

    if(str == NULL || (puRes == NULL && psRes == NULL))
    {
        return -1;
    }

    size_t len = strlen(str);
    if(len == 0U)
    {
        return -1;
    }

    isUnsigned = puRes != NULL ? true : false;

    errno = 0;
    if(isUnsigned)
    {
        ulVal = strtoul(str, &endPtr, base);
    }
    else
    {
        lVal = strtol(str, &endPtr, base);
    }

    if(isUnsigned)
    {
        *puRes = ulVal;
    }
    else
    {
        *psRes = lVal;
    }

    if(errno != 0 || (endPtr - str) != (ssize_t)len)
    {   // range error or not all was converted
        return -1;
    }

    // success
    return 0;
}

// string to unsigned long conversion
int FC_Str2UL(const char* str, int base, uint32_t* pRes)
{
    uint32_t u32Res = 0UL;
    int convRes = FC_Str2Int(str, base, &u32Res, NULL);
    if(pRes != NULL)
    {
        *pRes = u32Res;
    }
    return convRes;
}

// string to long conversion
int FC_Str2L(const char* str, int base, int32_t* pRes)
{
    int32_t i32Res = 0;
    int convRes = FC_Str2Int(str, base, NULL, &i32Res);
    if(pRes != NULL)
    {
        *pRes = i32Res;
    }
    return convRes;
}


