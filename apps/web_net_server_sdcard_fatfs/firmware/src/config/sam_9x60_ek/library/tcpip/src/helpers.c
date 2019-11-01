/*******************************************************************************
  Helper Functions for Microchip tcpip

  Summary:
    TCP/IP Stack Library Helpers
    
  Description:
    Common Microchip helper functions
*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "helpers.h"
#include "system/console/sys_console.h"

typedef union 
{
    uint16_t Val;
    uint8_t v[2];
} MCHP_UINT16_VAL;

typedef union
{
    uint32_t Val;
    uint16_t w[2] __attribute__((packed));
    uint8_t  v[4];
} MCHP_UINT32_VAL;

/*****************************************************************************
  Function:
    void uitoa(uint16_t Value, uint8_t* Buffer)

  Summary:
    Converts an unsigned integer to a decimal string.

  Description:
    Converts a 16-bit unsigned integer to a null-terminated decimal string.

  Precondition:
    None

  Parameters:
    Value   - The number to be converted
    Buffer  - Pointer in which to store the converted string

  Returns:
    None
  ***************************************************************************/
void uitoa(uint16_t Value, uint8_t* Buffer)
{
    uint8_t i;
    uint16_t Digit;
    uint16_t Divisor;
    bool Printed = false;

    if(Value)
    {
        for(i = 0, Divisor = 10000; i < 5u; i++)
        {
            Digit = Value/Divisor;
            if(Digit || Printed)
            {
                *Buffer++ = '0' + Digit;
                Value -= Digit*Divisor;
                Printed = true;
            }
            Divisor /= 10;
        }
    }
    else
    {
        *Buffer++ = '0';
    }

    *Buffer = '\0';
}


/*****************************************************************************
  Function:
    uint8_t hexatob(uint16_t AsciiVal)

  Summary:
    Converts a hex string to a single byte.

  Description:
    Converts a two-character ASCII hex string to a single packed byte.

  Precondition:
    None

  Parameters:
    AsciiVal - uint16_t where the LSB is the ASCII value for the lower nibble
                    and MSB is the ASCII value for the upper nibble.  Each
                    must range from '0'-'9', 'A'-'F', or 'a'-'f'.

  Returns:
    Resulting packed byte 0x00 - 0xFF.
  ***************************************************************************/
uint8_t hexatob(uint16_t AsciiVal)
{
    MCHP_UINT16_VAL AsciiChars;
    AsciiChars.Val = AsciiVal;

    // Convert lowercase to uppercase
    if(AsciiChars.v[1] > 'F')
        AsciiChars.v[1] -= 'a'-'A';
    if(AsciiChars.v[0] > 'F')
        AsciiChars.v[0] -= 'a'-'A';

    // Convert 0-9, A-F to 0x0-0xF
    if(AsciiChars.v[1] > '9')
        AsciiChars.v[1] -= 'A' - 10;
    else
        AsciiChars.v[1] -= '0';

    if(AsciiChars.v[0] > '9')
        AsciiChars.v[0] -= 'A' - 10;
    else
        AsciiChars.v[0] -= '0';

    // Concatenate
    return (AsciiChars.v[1]<<4) |  AsciiChars.v[0];
}

/*****************************************************************************
  Function:
    uint8_t btohexa_high(uint8_t b)

  Summary:
    Converts the upper nibble of a binary value to a hexadecimal ASCII byte.

  Description:
    Converts the upper nibble of a binary value to a hexadecimal ASCII byte.
    For example, btohexa_high(0xAE) will return 'a'.

  Precondition:
    None

  Parameters:
    b - the byte to convert

  Returns:
    The upper hexadecimal ASCII byte '0'-'9' or 'a'-'f'.
  ***************************************************************************/
uint8_t btohexa_high(uint8_t b)
{
    b >>= 4;
    return (b>0x9u) ? b+'a'-10:b+'0';
}

/*****************************************************************************
  Function:
    uint8_t btohexa_high(uint8_t b)

  Summary:
    Converts the lower nibble of a binary value to a hexadecimal ASCII byte.

  Description:
    Converts the lower nibble of a binary value to a hexadecimal ASCII byte.
    For example, btohexa_high(0xAE) will return 'e'.

  Precondition:
    None

  Parameters:
    b - the byte to convert

  Returns:
    The lower hexadecimal ASCII byte '0'-'9' or 'a'-'f'.
  ***************************************************************************/
uint8_t btohexa_low(uint8_t b)
{
    b &= 0x0F;
    return (b>9u) ? b+'a'-10:b+'0';
}

/*****************************************************************************
  Function:
    char* strncpy_m(char* destStr, size_t destSize, int nStrings, ...)

  Summary:
    Copies multiple strings to a destination

  Description:
    Copies multiple strings to a destination
    but doesn't copy more than destSize characters.
    Useful where the destination is actually an array and an extra \0
    won't be appended to overflow the buffer
    
  Precondition:
    - valid string pointers
    - destSize should be > 0

  Parameters:
    destStr - Pointer to a string to be initialized with the multiple strings provided as arguments.

    destSize    - the maximum size of the destStr field, that cannot be exceeded.
                  An \0 won't be appended if the resulting size is > destSize

    nStrings    - number of string parameters to be copied into destStr

    ...         - variable number of arguments
    

  Returns:
    Length of the destination string, terminating \0 (if exists) not included
  ***************************************************************************/
size_t strncpy_m(char* destStr, size_t destSize, int nStrings, ...)
{
    va_list     args = {0};
    const char* str;
    char*       end;
    size_t      len;

    destStr[0] = '\0';
    end = destStr + destSize - 1;
    *end = '\0';
    len = 0;
    
    va_start( args, nStrings );
    
    while(nStrings--)
    {
        if(*end)
        {   // if already full don't calculate strlen outside the string area
            len = destSize;
            break;
        }
        
        str = va_arg(args, const char*);
        strncpy(destStr + len, str, destSize - len);
        len += strlen(str);
    }

    va_end( args );
    
    return len;
}

int stricmp(const char *s1, const char *s2)
{
    if (s1 == 0 || s2 == 0)
    {
        return s1 == 0 ? s2 == 0 ? 0 : -(*s2) : *s1;
    }

    uint8_t c1, c2;
    do
    {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
    }while(c1 != 0 && c1 == c2);

    return (int)(c1 - c2); 
}

////////////////////////////////////////////////////////////////////////////////
#if PROFILE_HOOKS_DESIRED
#include "system/sys_time_h2_adapter.h"
#include "system/command/sys_command.h"

static ProfileObject profileObject;

void profilingInit( uint32_t aPrintInterval )
{
    profileObject.state = ProfileObjReady;
    profileObject.activeCount = 0;
    profileObject.useCount = 0;
    profileObject.printInterval = aPrintInterval;
    profileObject.printTime = SYS_TMR_TickCountGet() + profileObject.printInterval;
    memset( (char *) profileObject.entry, 0, NUM_PROFILE_ENTRIES * sizeof( ProfileData ) );
}

void profilingPause( void )
{
    if( ProfileObjInvalid != profileObject.state )
    {
        profileObject.state = ProfileObjPaused;
    }
}

void profilingContinue( void )
{
    if( ProfileObjInvalid != profileObject.state )
    {
        profileObject.state = ProfileObjReady;
    }
}

static void profileInitHandle( uint32_t handle, ProfileInitType aProfileInitType )
{
    if( handle < NUM_PROFILE_ENTRIES )
    {
        ProfileData * pProfileData = &profileObject.entry[ handle ];
        if( aProfileInitType != ProfileInitForReset )
        {
            memset( pProfileData->titleStr, 0, PROFILE_TITLE_STR_SZ + 1 );
            pProfileData->bInUse = false;
        }
        pProfileData->bActive = false;
        pProfileData->sum = 0;
        pProfileData->nValue = 0;
        pProfileData->ema = 0;
        pProfileData->min = PROFILE_MAX_DELTA;
        pProfileData->max = 0;
        pProfileData->sNValue = 0;
        pProfileData->sMax = 0;
        pProfileData->start = 0;
    }
}

uint32_t profileGetHandle( char * const pTitleStr )
{
    uint32_t handle = 0;
    if( ProfileObjInvalid != profileObject.state )
    {
        ProfileData * pProfileData;
        while( handle < NUM_PROFILE_ENTRIES )
        {
            pProfileData = &profileObject.entry[ handle ];
            if( !pProfileData->bInUse )
            {
                profileInitHandle( handle, ProfileInitForUse );
                strncpy( pProfileData->titleStr, pTitleStr, PROFILE_TITLE_STR_SZ );
                pProfileData->bInUse = true;
                profileObject.useCount++;
                break;
            }
            ++handle;
        }
    }
    return( handle );
}

void profileReleaseHandle( uint32_t handle )
{
    if(     (ProfileObjInvalid != profileObject.state)
        &&  (handle < NUM_PROFILE_ENTRIES)
        &&  profileObject.entry[ handle ].bInUse
    )
    {
        profileInitHandle( handle, ProfileInitForRelease );
        profileObject.useCount--;
    }
}

void profileReset( uint32_t handle )
{
    if(     (ProfileObjInvalid != profileObject.state)
        &&  (handle < NUM_PROFILE_ENTRIES)
    )
    {
        profileInitHandle( handle, ProfileInitForReset );
    }
}

void profileStart( uint32_t handle )
{
    if(     (ProfileObjReady == profileObject.state)
        &&  (handle < NUM_PROFILE_ENTRIES)
    )
    {
        ProfileData * pProfileData = &profileObject.entry[ handle ];
        if( pProfileData->bInUse )
        {
            pProfileData->start = SYS_TIME_Counter64Get();
            pProfileData->bActive = true;
            profileObject.activeCount++;
        }
    }
}

void profileStop( uint32_t handle )
{
    if(     (ProfileObjInvalid != profileObject.state)
        &&  (handle < NUM_PROFILE_ENTRIES)
    )
    {
        ProfileData * pProfileData = &profileObject.entry[ handle ];
        if( pProfileData->bActive )
        {
            uint32_t delta;
            uint32_t stop = SYS_TIME_Counter64Get();

            pProfileData->bActive = false;
            if( stop >= pProfileData->start )
            {
                delta = stop - pProfileData->start;
            }
            else
            {
                delta = PROFILE_MAX_DELTA - pProfileData->start;
                delta += stop + 1;
            }

            if( delta > pProfileData->max )
            {
                pProfileData->max = delta;
            }
            if( delta < pProfileData->min )
            {
                pProfileData->min = delta;
            }
            pProfileData->ema = (   (PROFILE_EMA_ALPHA * delta)
                                    + (PROFILE_EMA_RESIDUAL * pProfileData->ema)
                                    ) / PROFILE_EMA_SCALE;
            pProfileData->sum += delta;
            pProfileData->nValue++;
            // sample, print, values
            pProfileData->sSum += delta;
            pProfileData->sNValue++;
            if( delta > pProfileData->sMax )
            {
                pProfileData->sMax = delta;
            }
            //
            profileObject.activeCount--;
        }
    }
}

void profileListAction(
    uint32_t *          pHandleList,
    uint32_t            numHandles,
    ProfileListAction   aProfileListAction
    )
{
    if( ProfileObjInvalid != profileObject.state )
    {
        void (*pla)( uint32_t );

        switch( aProfileListAction )
        {
            case ProfileListRelease:    pla = profileReleaseHandle; break;
            case ProfileListReset:      pla = profileReset;         break;
            case ProfileListStart:      pla = profileStart;         break;
            default:
            case ProfileListStop:       pla = profileStop;          break;
        }
        if( !pHandleList )
        {
            numHandles = NUM_PROFILE_ENTRIES;
        }
        for( uint32_t ii = 0; profileObject.activeCount && (ii < numHandles); ++ii )
        {
            pla( pHandleList ? pHandleList[ ii ] : ii );
        }
    }
}

void profileInfo( ProfileView aProfileView )
{
    if(     (ProfileObjInvalid != profileObject.state)
        &&  profileObject.useCount
    )
    {
        char *      formatStr;
        uint32_t    ave, handle, useCount, entryCount;

        switch( aProfileView )
        {
            case ProfileViewAll:
                SYS_CONSOLE_MESSAGE( "CUMMULATIVE,                                                            SAMPLE\r\n" );
                SYS_CONSOLE_MESSAGE( "SUM,            N,          AVE,        EMA,        MIN,        MAX,        sSum,       sN,         sMAX,       TITLE\r\n" );
                //                    0xABCDabcd1234, 0xabcd1234, 0xabcd1234, 0xabcd1234, 0xabcd1234, 0xabcd1234, 0xabcd1234, 0xabcd1234, 0xabcd1234, string
                formatStr = "0x%-12llx, 0x%-8x, 0x%-8x, 0x%-8x, 0x%-8x, 0x%-8x, 0x%-8x, 0x%-8x, %s\r\n";
                break;
            case ProfileViewCumulative:
                SYS_CONSOLE_MESSAGE( "CUMMULATIVE\r\n" );
                SYS_CONSOLE_MESSAGE( "SUM,            N,          AVE,        EMA,        MIN,        MAX,        TITLE\r\n" );
                //                    0xABCDabcd1234, 0xabcd1234, 0xabcd1234, 0xabcd1234, 0xabcd1234, 0xabcd1234, string
                formatStr = "0x%-12llx, 0x%-8x, 0x%-8x, 0x%-8x, 0x%-8x, 0x%-8x, %s\r\n";
                break;
            case ProfileViewSample:
                SYS_CONSOLE_MESSAGE( "SAMPLE\r\n" );
                SYS_CONSOLE_MESSAGE( "sSum,       sN,         sMAX,       TITLE\r\n" );
                //                    0xabcd1234, 0xabcd1234, 0xabcd1234, string
                formatStr = "0x%-8x, 0x%-8x, 0x%-8x, %s\r\n";
                break;
            default:
            case ProfileViewA:
            case ProfileViewAChanges:
                SYS_CONSOLE_MESSAGE( "CUMMULATIVE,                                    SAMPLE\r\n" );
                SYS_CONSOLE_MESSAGE( "N,          AVE,        EMA,        MAX,        sSum,       sN,         sMAX,       TITLE\r\n" );
                //                    0xabcd1234, 0xabcd1234, 0xabcd1234, 0xabcd1234, 0xabcd1234, 0xabcd1234, 0xabcd1234, string
                formatStr = "0x%-8x, 0x%-8x, 0x%-8x, 0x%-8x, 0x%-8x, 0x%-8x, 0x%-8x, %s\r\n";
                break;
        }
        entryCount = 0;
        for( useCount = handle = 0; (handle < NUM_PROFILE_ENTRIES) && (useCount < profileObject.useCount); ++handle )
        {
            ProfileData * pProfileData = &profileObject.entry[ handle ];
            if( pProfileData->bInUse )
            {
                ++useCount;
                if(     pProfileData->nValue
                    &&  ((ProfileViewAChanges == aProfileView) ? pProfileData->sNValue : true)
                )
                {
                    ++entryCount;
                    ave = (uint32_t) (pProfileData->sum / pProfileData->nValue);
                    if( pProfileData->nValue > PROFILE_EMA_SCALE )
                    {
                        pProfileData->ema = ave;
                    }
                    switch( aProfileView )
                    {
                        case ProfileViewAll:
                            SYS_CONSOLE_PRINT( formatStr,
                                pProfileData->sum,
                                pProfileData->nValue,
                                ave,
                                pProfileData->ema,
                                pProfileData->min,
                                pProfileData->max,
                                //
                                pProfileData->sSum,
                                pProfileData->sNValue,
                                pProfileData->sMax,
                                pProfileData->titleStr
                                );
                            break;
                        case ProfileViewCumulative:
                            SYS_CONSOLE_PRINT( formatStr,
                                pProfileData->sum,
                                pProfileData->nValue,
                                ave,
                                pProfileData->ema,
                                pProfileData->max
                                );
                            break;
                        case ProfileViewSample:
                            SYS_CONSOLE_PRINT( formatStr,
                                pProfileData->sSum,
                                pProfileData->sNValue,
                                pProfileData->sMax,
                                pProfileData->titleStr
                                );
                            break;
                        default:
                        case ProfileViewAChanges:
                        case ProfileViewA:
                            SYS_CONSOLE_PRINT( formatStr,
                                pProfileData->nValue,
                                ave,
                                pProfileData->ema,
                                pProfileData->max,
                                //
                                pProfileData->sSum,
                                pProfileData->sNValue,
                                pProfileData->sMax,
                                pProfileData->titleStr
                                );
                            break;
                    }
                    pProfileData->sSum = 0;
                    pProfileData->sNValue = 0;
                    pProfileData->sMax = 0;
                }
            }
        }
        SYS_CONSOLE_PRINT( "%d entries\r\n\r\n", entryCount );
        (void) formatStr;   // eliminate unused warning when SYS_CONSOLE_PRINT is not defined
        // perform print time update here; when profileInfo is called independently,
        // then periodicDisplay will be rescheduled
        profileObject.printTime = SYS_TMR_TickCountGet() + profileObject.printInterval;
    }
}

void profilePeriodicDisplay( ProfileView aProfileView )
{   // user can call profileInfo when state is halted; but, don't auto print
    if(     (ProfileObjReady == profileObject.state)
        &&  profileObject.useCount
        &&  (SYS_TMR_TickCountGet() >= profileObject.printTime)
    )
    {
        profileInfo( aProfileView );
    }
}
#endif
////////////////////////////////////////////////////////////////////////////////
