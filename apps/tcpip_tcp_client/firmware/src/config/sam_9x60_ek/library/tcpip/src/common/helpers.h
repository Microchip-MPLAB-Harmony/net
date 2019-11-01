/*******************************************************************************
  Header file for common MCHP helpers

  Summary:
    SUMMARY
    
  Description:
    DESCRIPTION
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
#ifndef __HELPERS_H_
#define __HELPERS_H_

#include <stdint.h>
#include <stdbool.h>

void        uitoa(uint16_t Value, uint8_t* Buffer);

uint8_t     hexatob(uint16_t AsciiVal);
uint8_t     btohexa_high(uint8_t b);
uint8_t     btohexa_low(uint8_t b);

size_t      strncpy_m(char* destStr, size_t destSize, int nStrings, ...);
int         stricmp ( const char * s1, const char * s2 );

////////////////////////////////////////////////////////////////////////////////
#define PROFILE_HOOKS_DESIRED   true
#define NUM_PROFILE_ENTRIES     150
#define PROFILE_TITLE_STR_SZ    32
#define PROFILE_MAX_DELTA       0xFFFFFFFF
#define PROFILE_EMA_ALPHA       10LL
#define PROFILE_EMA_SCALE       100LL
#define PROFILE_EMA_RESIDUAL    (PROFILE_EMA_SCALE - PROFILE_EMA_ALPHA)

typedef enum
{
    ProfileObjInvalid = 0,
    ProfileObjReady,
    ProfileObjPaused,
} ProfileObjState;

typedef enum
{
    ProfileInitForRelease,
    ProfileInitForReset,
    ProfileInitForUse,
} ProfileInitType;

typedef enum
{
    ProfileListRelease,
    ProfileListReset,
    ProfileListStart,
    ProfileListStop,
} ProfileListAction;

typedef enum
{
    ProfileViewCumulative,
    ProfileViewSample,
    ProfileViewA,                   // hybrid of Cumulative & Sample
    ProfileViewAChanges,            // ViewA but only if sNValue changes
    ProfileViewAll,
} ProfileView;

typedef struct {
    bool            bInUse;         // handle claimed and may be used
    bool            bActive;        // actively timing: start has been called, stop has not
    uint64_t        start;          // the start time -- only valid if bInUse && bActive
    // accumlation values
    uint64_t        sum;            // sum of elapsed stop-start times
    uint32_t        nValue;         // number of elements in the sum
    uint32_t        ema;
    uint32_t        min;            // overall min
    uint32_t        max;            // overall max
    // periodic sample values
    uint32_t        sSum;           // sample sum, sum since last profileInfo print
    uint32_t        sNValue;        // sample nValue, change in measurement count since last profileInfo print
    uint32_t        sMax;           // sample max -- max since last profileInfo print
    char            titleStr[ PROFILE_TITLE_STR_SZ + 1 ];
} ProfileData;

typedef struct
{   // container for profile elements
    ProfileObjState state;
    uint32_t        printTime;
    uint32_t        printInterval;
    uint32_t        useCount;
    uint32_t        activeCount;
    ProfileData     entry[ NUM_PROFILE_ENTRIES ];
} ProfileObject;

#if PROFILE_HOOKS_DESIRED
void        profilingInit(          uint32_t    aProfilePrintInterval );
void        profilingPause(         void );
void        profilingContinue(      void );

uint32_t    profileGetHandle(       char * const pTitleStr );
void        profileReleaseHandle(   uint32_t    handle );
void        profileReset(           uint32_t    handle );
void        profileStart(           uint32_t    handle );
void        profileStop(            uint32_t    handle );
void        profileInfo(            ProfileView aProfileView );
void        profilePeriodicDisplay( ProfileView aProfileView );
//
void        profileListAction(      uint32_t *          pHandleList,        // 0 implies all
                                    uint32_t            numHandles,
                                    ProfileListAction   aProfileListAction
                                    );
#define     profileReleaseList(     argA, argB )    profileListAction( argA, argB, ProfileListRelease )
#define     profileResetList(       argA, argB )    profileListAction( argA, argB, ProfileListReset )
#define     profileStartList(       argA, argB )    profileListAction( argA, argB, ProfileListStart )
#define     profileStopList(        argA, argB )    profileListAction( argA, argB, ProfileListStop )
#else
#define     profilingInit(          arg )
#define     profilingHalt(          )
#define     profilingContinue(      )
#define     profileGetHandle(       arg )         0
#define     profileReleaseHandle(   arg )
#define     profileReset(           arg )
#define     profileStart(           arg )
#define     profileStop(            arg )
#define     profileInfo(            arg )
#define     profilePeriodicDisplay( arg )
#define     profileListAction(      argA, argB, argC )
#define     profileReleaseList(     argA, argB )
#define     profileResetList(       argA, argB )
#define     profileStartList(       argA, argB )
#define     profileStopList(        argA, argB )
#endif
////////////////////////////////////////////////////////////////////////////////

#endif  // end of header

