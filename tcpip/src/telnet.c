/*******************************************************************************
  Telnet Server

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    -Provides Telnet services on TCP port 23
    -Reference: RFC 854
*******************************************************************************/

/*
Copyright (C) 2016-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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


#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_TELNET_SERVER

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_TELNET_SERVER)
#include "net_pres/pres/net_pres_socketapi.h"
#include "system/command/sys_command.h"


#define TELNET_LINE_RETURN  "\r"
#define TELNET_LINE_FEED    "\n"
#define TELNET_LINE_TERM    TELNET_LINE_RETURN TELNET_LINE_FEED  

// limited set of supported telnet commands 
#define TELNET_CMD_IAC          "\xff"
#define TELNET_CMD_DONT         "\xfe"
#define TELNET_CMD_DO           "\xfd"
#define TELNET_CMD_WONT         "\xfc"
#define TELNET_CMD_WILL         "\xfb"

#define TELNET_CMD_IAC_CODE     '\xff'

// command option codes
#define TELNET_CMD_OPT_CODE_BEG   '\xfb'
#define TELNET_CMD_OPT_CODE_END   '\xfe'

#define TELNET_CMD_WILL_CODE    '\xfb'
#define TELNET_CMD_WONT_CODE    '\xfc'
#define TELNET_CMD_DO_CODE      '\xfd'
#define TELNET_CMD_DONT_CODE    '\xfe'

// other telnet commands codes (not negotiated options)
#define TELNET_CMD_NOPT_CODE_BEG   '\xf0'
#define TELNET_CMD_NOPT_CODE_END   '\xf9'

#define TELNET_CMD_SE_CODE      '\xf0'
#define TELNET_CMD_NOP_CODE     '\xf1'
#define TELNET_CMD_DM_CODE      '\xf2'
#define TELNET_CMD_BREAK_CODE   '\xf3'
#define TELNET_CMD_IP_CODE      '\xf4'
#define TELNET_CMD_AO_CODE      '\xf5'
#define TELNET_CMD_AYT_CODE     '\xf6'
#define TELNET_CMD_EC_CODE      '\xf7'
#define TELNET_CMD_EL_CODE      '\xf8'
#define TELNET_CMD_GA_CODE      '\xf9'

// subnegotiation start command
#define TELNET_CMD_SB_CODE    '\xfa'


// limited set of supported telnet options
#define TELNET_OPT_SUPP_LOCAL_ECHO  "\x2d"      // suppress local echo


// connection display strings
//

// start up message
// 2J is clear screen, 31m is red, 1m is bold
// 0m is clear all attributes

/* MISRA C-2012 Rule 4.1 deviated:1 Deviation record ID -  H3_MISRAC_2012_R_4_1_NET_DR_1 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:1 "MISRA C-2012 Rule 4.1" "H3_MISRAC_2012_R_4_1_NET_DR_1" 
static const char TELNET_START_MSG[] = "\x1b[2J\x1b[31m\x1b[1m Microchip Telnet Server 1.1\x1b[0m\r\n Login: ";
#pragma coverity compliance end_block "MISRA C-2012 Rule 4.1"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */

//
// ask password message
#define TELNET_ASK_PASSWORD_MSG     "Password: " TELNET_CMD_IAC TELNET_CMD_DO TELNET_OPT_SUPP_LOCAL_ECHO        // ask Suppress Local Echo

// Access denied message/ failed logon
#define TELNET_FAIL_LOGON_MSG       TELNET_LINE_TERM "Access denied" TELNET_LINE_TERM TELNET_LINE_TERM      

// internal buffer overflow message
#define TELNET_BUFFER_OVFLOW_MSG    "Too much data. Aborted" TELNET_LINE_TERM

// Successful authentication message/log on OK
#define TELNET_LOGON_OK             TELNET_LINE_TERM "Logged in successfully" TELNET_LINE_TERM TELNET_LINE_TERM 

// welcome message
#define TELNET_WELCOME_MSG          TELNET_LINE_TERM "--- Telnet Console ---" TELNET_LINE_TERM \
                                                     "Type help for commands" TELNET_LINE_TERM ">"

// disconnect message
//#define TELNET_BYE_MSG            TELNET_LINE_TERM TELNET_LINE_TERM "Goodbye!" TELNET_LINE_TERM TELNET_LINE_TERM

// failure to register with the command processor
#define TELNET_FAIL_CMD_REGISTER    "Failed to connect to the command processor. Aborting!" TELNET_LINE_TERM

// buffering defines
#define M_TCPIP_TELNET_LINE_BUFF_SIZE    (TCPIP_TELNET_LINE_BUFF_SIZE + 3) // + extra room for \r\n 

#define TELNET_SKT_MESSAGE_SPACE    80U      // min space needed in the socket buffer for displaying messages

// authentication defines
#if !defined(TCPIP_TELNET_OBSOLETE_AUTHENTICATION) || (TCPIP_TELNET_OBSOLETE_AUTHENTICATION == 0)
#define M_TELNET_USE_AUTHENTICATION_CALLBACK     1
#else
#define M_TELNET_USE_AUTHENTICATION_CALLBACK     0
#endif  // !defined(TCPIP_TELNET_OBSOLETE_AUTHENTICATION) || (TCPIP_TELNET_OBSOLETE_AUTHENTICATION == 0)

typedef enum
{
    TELNET_MSG_LINE_PENDING,        // no line assembled yet
    TELNET_MSG_LINE_DONE,           // line assembled
    TELNET_MSG_LINE_OVFL            // line buffer capacity exceeded
}TELNET_MSG_LINE_RES;   // message line result
    
typedef struct
{
    NET_PRES_SKT_HANDLE_T   telnetSkt;
    TCPIP_TELNET_STATE      telnetState;
    SYS_CMD_DEVICE_NODE*    telnetIO;
    char                    username[TCPIP_TELNET_USERNAME_SIZE + 1U];   // username for the connection login
}TELNET_DCPT;

static const void*      telnetHeapH = NULL;                    // memory allocation handle
static TELNET_DCPT*     telnetDcpt = NULL; // array of descriptors, each for a connection

static int              telnetInitCount = 0;      // TELNET module initialization count

static TCPIP_SIGNAL_HANDLE telnetSignalHandle = NULL;

static TCPIP_TELNET_MODULE_CONFIG telnetConfigData;   // initialization data

#if (M_TELNET_USE_AUTHENTICATION_CALLBACK != 0)
static TCPIP_TELNET_AUTH_HANDLER telnetAuthHandler;
static const void* telnetAuthHParam;
#endif  // (M_TELNET_USE_AUTHENTICATION_CALLBACK != 0)

// prototypes
static void F_Telnet_MSG(const void* cmdIoParam, const char* str);
static void F_Telnet_PRINT(const void* cmdIoParam, const char* format, ...) FORMAT_ATTRIBUTE(printf, 2,  3);
static void F_Telnet_PUTC(const void* cmdIoParam, char c);
static int  F_Telnet_DATA_RDY(const void* cmdIoParam);
static char F_Telnet_GETC(const void* cmdIoParam);

static void F_Telnet_Deregister(TELNET_DCPT* pDcpt);
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void F_Telnet_Cleanup(void);
#else
#define F_Telnet_Cleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static TCPIP_TELNET_STATE F_Telnet_UserCheck(TELNET_DCPT* pDcpt);
static TCPIP_TELNET_STATE F_Telnet_LogonCheck(TELNET_DCPT* pDcpt);
static TELNET_MSG_LINE_RES F_Telnet_MessageLineCheck(NET_PRES_SKT_HANDLE_T tSkt, char** pLineBuffer, uint16_t bufferSize, uint16_t* readBytes);
static const char* F_Telnet_Process_CmdOptions(const char* strMsg, uint16_t avlblBytes);

static void TCPIP_TELNET_Process(void);

static void F_TelnetSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param);

static const SYS_CMD_API telnetIOApi =
{
    &F_Telnet_MSG,
    (SYS_CMD_PRINT_FNC)&F_Telnet_PRINT,
    &F_Telnet_PUTC,
    &F_Telnet_DATA_RDY,
    &F_Telnet_GETC
};


// conversion functions/helpers
//
static __inline__ TCPIP_TELNET_HANDLE __attribute__((always_inline)) FC_AuthHndl2TelHndl(TCPIP_TELNET_AUTH_HANDLER authHandler)
{
    union
    {
        TCPIP_TELNET_AUTH_HANDLER authHandler;
        TCPIP_TELNET_HANDLE tHndl;
    }U_AUTH_HNDL_T_HNDL;

    U_AUTH_HNDL_T_HNDL.authHandler = authHandler;
    return U_AUTH_HNDL_T_HNDL.tHndl;
}

static __inline__ TCPIP_TELNET_STATE __attribute__((always_inline)) FC_TelStateInc(TCPIP_TELNET_STATE tState, int inc)
{
    union
    {
        TCPIP_TELNET_STATE tState;
        int                iState;
    }U_T_STATE_INT;

    U_T_STATE_INT.tState = tState;
    U_T_STATE_INT.iState += inc;
    return U_T_STATE_INT.tState;
}


// implementation
//


bool TCPIP_TELNET_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{

    size_t tIx;
    TELNET_DCPT* pDcpt;
    NET_PRES_SKT_HANDLE_T tSkt;
    TCPIP_TCP_SIGNAL_HANDLE sigHandle;
    bool    initFail = false;

    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    // interface restart


    // stack init

    while(telnetInitCount == 0)
    {   // first time we're run

        const TCPIP_TELNET_MODULE_CONFIG* pTelConfig = (const TCPIP_TELNET_MODULE_CONFIG*)initData;
        // check configuration data is not missing
        if(stackCtrl->memH == NULL || pTelConfig == NULL || pTelConfig->nConnections == 0U)
        {
            SYS_ERROR(SYS_ERROR_ERROR, "telnet NULL dynamic allocation handle/init data");
            return false;
        }

        telnetHeapH = stackCtrl->memH;
        telnetConfigData = *pTelConfig;
        // allocate data
        telnetDcpt = (TELNET_DCPT*)TCPIP_HEAP_Calloc(telnetHeapH, telnetConfigData.nConnections, sizeof(*telnetDcpt)); 
        if(telnetDcpt == NULL)
        {
            SYS_ERROR(SYS_ERROR_ERROR, " telnet Dynamic allocation failed");
            return false;
        }

        // initialize telnetDcpt and open the server sockets
        pDcpt = telnetDcpt;
        for(tIx = 0; tIx < (size_t)telnetConfigData.nConnections; tIx++)
        {
            pDcpt->telnetSkt = (tSkt = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_SERVER, (NET_PRES_SKT_ADDR_T)IP_ADDRESS_TYPE_ANY, telnetConfigData.listenPort, NULL, NULL));
            sigHandle = NET_PRES_SocketSignalHandlerRegister(tSkt, (uint16_t)TCPIP_TCP_SIGNAL_RX_DATA, &F_TelnetSocketRxSignalHandler, NULL);
            if(tSkt == INVALID_SOCKET || sigHandle == NULL)
            {
                initFail = true;
                break;
            }
            // set socket options
            if((telnetConfigData.configFlags & (uint16_t)TCPIP_TELNET_FLAG_NO_DELAY) != 0U)
            {
                void* tcpForceFlush = FC_Uint2VPtr(1UL);
                (void)NET_PRES_SocketOptionsSet(tSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_NODELAY, tcpForceFlush);
            }
            if(telnetConfigData.sktTxBuffSize != 0U)
            {
                void* tcpBuffSize = FC_Uint2VPtr((uint32_t)telnetConfigData.sktTxBuffSize);
                if(!NET_PRES_SocketOptionsSet(tSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_TX_BUFF, tcpBuffSize))
                {
                    SYS_ERROR(SYS_ERROR_WARNING, " telnet: Setting TX Buffer failed: %d\r\n", tIx);
                }
            }
            if(telnetConfigData.sktRxBuffSize != 0U)
            {
                void* tcpBuffSize = FC_Uint2VPtr((uint32_t)telnetConfigData.sktRxBuffSize);
                if(!NET_PRES_SocketOptionsSet(tSkt, (NET_PRES_SKT_OPTION_TYPE)TCP_OPTION_RX_BUFF, tcpBuffSize))
                {
                    SYS_ERROR(SYS_ERROR_WARNING, " telnet: Setting RX Buffer failed: %d\r\n", tIx);
                }
            }
            pDcpt++;
        }

        if(!initFail)
        {   // create the telnet timer
            telnetSignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_TELNET_Task, TCPIP_TELNET_TASK_TICK_RATE);
            if(telnetSignalHandle == NULL)
            {   // cannot create the telnet timer
                initFail = true;
            }
        }
#if (M_TELNET_USE_AUTHENTICATION_CALLBACK != 0)
        telnetAuthHandler = NULL;
#endif  // (M_TELNET_USE_AUTHENTICATION_CALLBACK != 0)

        break;

    }

    if(initFail)
    {
        F_Telnet_Cleanup();
        return false;
    }

    telnetInitCount++;
    return true;
}


#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_TELNET_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{

    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    if(telnetInitCount > 0)
    {   // we're up and running
        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--telnetInitCount == 0)
            {   // all closed
                F_Telnet_Cleanup();
            }
        }
    }

}

static void F_Telnet_Cleanup(void)
{
    size_t tIx;
    TELNET_DCPT* pDcpt;

    pDcpt = telnetDcpt;
    for(tIx = 0; tIx < (size_t)telnetConfigData.nConnections; tIx++)
    {
        F_Telnet_Deregister(pDcpt);

        if( pDcpt->telnetSkt != INVALID_SOCKET)
        {
            NET_PRES_SocketClose(pDcpt->telnetSkt);
        }
        pDcpt++;
    }

    (void)TCPIP_HEAP_Free(telnetHeapH, telnetDcpt);
    telnetDcpt = NULL;
    telnetConfigData.nConnections = 0U;

    if(telnetSignalHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(telnetSignalHandle);
        telnetSignalHandle = NULL;
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)


static void F_Telnet_Deregister(TELNET_DCPT* pDcpt)
{
    if (pDcpt->telnetIO != NULL)
    {
        (void)SYS_CMD_DELETE(pDcpt->telnetIO);
        pDcpt->telnetIO = NULL;
    }

}

// Telnet's PUTC
static void F_Telnet_PUTC(const void* cmdIoParam, char c)
{
    NET_PRES_SKT_HANDLE_T tSkt = (NET_PRES_SKT_HANDLE_T)FC_CvPtr2U32(cmdIoParam);
    if(tSkt != INVALID_SOCKET)
    {
        (void)NET_PRES_SocketWrite(tSkt, (void*)&c, 1U);
    }
}

// Telnet's message 
static void F_Telnet_MSG(const void* cmdIoParam, const char* str)
{
    NET_PRES_SKT_HANDLE_T tSkt = (NET_PRES_SKT_HANDLE_T)FC_CvPtr2U32(cmdIoParam);
    if(tSkt != INVALID_SOCKET)
    {
        (void)NET_PRES_SocketWrite(tSkt, FC_CStr2CVPtr(str), (uint16_t)strlen(str));
    }
}


/* MISRA C-2012 Rule 17.1 deviated:3 Deviation record ID -  H3_MISRAC_2012_R_17_1_NET_DR_2 */
/* MISRA C-2012 Rule 21.6 deviated:1 Deviation record ID -  H3_MISRAC_2012_R_21_6_NET_DR_3 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:3 "MISRA C-2012 Rule 17.1" "H3_MISRAC_2012_R_17_1_NET_DR_2" 
#pragma coverity compliance block deviate:1 "MISRA C-2012 Rule 21.6" "H3_MISRAC_2012_R_21_6_NET_DR_3" 
// Telnet's print
static void F_Telnet_PRINT(const void* cmdIoParam, const char* format, ...)
{
    va_list arg_list = {NULL};
    char buff[TCPIP_TELNET_PRINT_BUFF_SIZE];

    va_start(arg_list, format);
    (void)vsnprintf(buff, TCPIP_TELNET_PRINT_BUFF_SIZE, format, arg_list); 
    va_end(arg_list);

    F_Telnet_MSG(cmdIoParam, buff);
}
#pragma coverity compliance end_block "MISRA C-2012 Rule 17.1"
#pragma coverity compliance end_block "MISRA C-2012 Rule 21.6"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */



// Telnet's data ready
static int F_Telnet_DATA_RDY(const void* cmdIoParam)
{
    NET_PRES_SKT_HANDLE_T tSkt = (NET_PRES_SKT_HANDLE_T)FC_CvPtr2U32(cmdIoParam);
    if(tSkt != INVALID_SOCKET)
    {
        return (int)NET_PRES_SocketReadIsReady(tSkt);
    }

    return 0;
}

// Telnet's getc
// it removes the control characters from the stream
static char F_Telnet_GETC(const void* cmdIoParam)
{
    char bData;

    NET_PRES_SKT_HANDLE_T tSkt = (NET_PRES_SKT_HANDLE_T)FC_CvPtr2U32(cmdIoParam);
    if(tSkt == INVALID_SOCKET)
    {
        return '\0';
    }

    if((telnetConfigData.configFlags & (uint16_t)TCPIP_TELNET_FLAG_PASS_CONTROL_CHARS) != 0U)
    {
        if (NET_PRES_SocketRead(tSkt, (uint8_t*)&bData, 1) != 0U)
        {
            return (char)bData;
        }
        return '\0';
    }

    // remove the control chars from the console stream
    char    lineBuffer[M_TCPIP_TELNET_LINE_BUFF_SIZE];    // telnet line buffer

    uint16_t avlblBytes = NET_PRES_SocketPeek(tSkt, (uint8_t*)lineBuffer, sizeof(lineBuffer) - 1U);

    if(avlblBytes != 0U)
    {
        char* lineEnd = lineBuffer + avlblBytes;
        *lineEnd = '\0';
        
        const char* linePtr = F_Telnet_Process_CmdOptions(lineBuffer, avlblBytes);
    
        avlblBytes = FC_ChPtrDiff2UI16(lineEnd, linePtr);

        if(avlblBytes != 0U)
        {
            bData = *linePtr++;
        }
        else
        {
            bData = '\0';
        }

        // discard consumed and unused
        uint16_t discardBytes = FC_ChPtrDiff2UI16(linePtr, lineBuffer);
        if(discardBytes != 0U)
        {
            (void)NET_PRES_SocketRead(tSkt, NULL, discardBytes);
        }
        return bData;
    }

    return '\0';

}

void TCPIP_TELNET_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;
    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != TCPIP_MODULE_SIGNAL_NONE)
    { // TMO or RX signal occurred
        TCPIP_TELNET_Process();
    }

}

// send a signal to the telnet module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void F_TelnetSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param)
{
    if(sigType == (uint16_t)TCPIP_TCP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}


static void TCPIP_TELNET_Process(void)
{
    size_t tIx;
    TELNET_DCPT* pDcpt;
    NET_PRES_SKT_HANDLE_T   tSocket;
    TCPIP_TELNET_STATE tState;


    // Loop through each telnet session and process state changes and TX/RX data
    pDcpt = telnetDcpt;
    for(tIx = 0; tIx < (size_t)telnetConfigData.nConnections; tIx++)
    {
        // Load up static state information for this session
        tSocket = pDcpt->telnetSkt;
        tState = pDcpt->telnetState;

        // Reset our state if the remote client disconnected from us
        if(NET_PRES_SocketWasReset(tSocket) || NET_PRES_SocketWasDisconnected(tSocket))
        {
            (void)NET_PRES_SocketDisconnect(tSocket);
            // Deregister IO and free its space
            F_Telnet_Deregister(pDcpt);
            tState = TCPIP_TELNET_IDLE;
        }

        // Handle session state
        switch(tState)
        {
            case TCPIP_TELNET_IDLE:
                // Make certain the socket can be written to
                if(NET_PRES_SocketWriteIsReady(tSocket, TELNET_SKT_MESSAGE_SPACE, 0U) < TELNET_SKT_MESSAGE_SPACE)
                {
                    break;
                }

                // Place the application protocol data into the transmit buffer.
                (void)NET_PRES_SocketWrite(tSocket, FC_CStr2CVPtr(TELNET_START_MSG), (uint16_t)strlen(TELNET_START_MSG));
                
                // Send the packet
                (void)NET_PRES_SocketFlush(tSocket);
                tState = FC_TelStateInc(tState, 1);
                break;

            case TCPIP_TELNET_GET_LOGIN:
                tState = F_Telnet_UserCheck(pDcpt);
                break;

            case TCPIP_TELNET_GET_PASSWORD:
                tState = F_Telnet_LogonCheck(pDcpt);
                break;

            case TCPIP_TELNET_AUTHENTICATED:
                if(NET_PRES_SocketWriteIsReady(tSocket, TELNET_SKT_MESSAGE_SPACE, 0U) < TELNET_SKT_MESSAGE_SPACE)
                {
                    break;
                }

                (void)NET_PRES_SocketWrite(tSocket, FC_CStr2CVPtr(TELNET_WELCOME_MSG), (uint16_t)strlen(TELNET_WELCOME_MSG));
                tState = FC_TelStateInc(tState, 1);

                (void)NET_PRES_SocketFlush(tSocket);

                // Register telnet as cmd IO device
                pDcpt->telnetIO = SYS_CMDIO_ADD(&telnetIOApi, FC_U322CvPtr((uint32_t)tSocket), 0);
                if (pDcpt->telnetIO == NULL)
                {
                    (void)NET_PRES_SocketWrite(tSocket, FC_CStr2CVPtr(TELNET_FAIL_CMD_REGISTER), (uint16_t)strlen(TELNET_FAIL_CMD_REGISTER));
                    (void)NET_PRES_SocketDisconnect(tSocket);
                    tState = TCPIP_TELNET_IDLE;
                }   
                break;

            case TCPIP_TELNET_CONNECTED:
                // Check if you're disconnected and de-register from the command processor
                break;

            default:
                // should not happen
                TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
                break;
        }


        // Save session state back into the static array
        pDcpt->telnetState = tState;
        pDcpt++;
    }

}

static TCPIP_TELNET_STATE F_Telnet_UserCheck(TELNET_DCPT* pDcpt)
{
    uint16_t avlblBytes;
    char        *lineStr;
    NET_PRES_SKT_HANDLE_T tSkt;
    TELNET_MSG_LINE_RES lineRes;

    char    userMessage[M_TCPIP_TELNET_LINE_BUFF_SIZE];    // telnet confirmation message

    tSkt = pDcpt->telnetSkt;
    if(NET_PRES_SocketWriteIsReady(tSkt, TELNET_SKT_MESSAGE_SPACE, 0U) < TELNET_SKT_MESSAGE_SPACE)
    {   // wait some more
        return TCPIP_TELNET_GET_LOGIN;
    }

    lineStr = userMessage;
    lineRes = F_Telnet_MessageLineCheck(tSkt, &lineStr, (uint16_t)sizeof(userMessage), &avlblBytes);

    if(lineRes == TELNET_MSG_LINE_PENDING)
    {   // wait some more
        return TCPIP_TELNET_GET_LOGIN;
    }
    else if(lineRes == TELNET_MSG_LINE_OVFL)
    {
        (void)NET_PRES_SocketWrite(tSkt, FC_CStr2CVPtr(TELNET_BUFFER_OVFLOW_MSG), (uint16_t)strlen(TELNET_BUFFER_OVFLOW_MSG));
        (void)NET_PRES_SocketWrite(tSkt, FC_CStr2CVPtr(TELNET_FAIL_LOGON_MSG), (uint16_t)strlen(TELNET_FAIL_LOGON_MSG));
        (void)NET_PRES_SocketDisconnect(tSkt);
        return TCPIP_TELNET_IDLE;   
    }
    else
    {
        // do nothing
    }

    // TELNET_MSG_LINE_DONE
    // remove the line termination 
    lineStr = strtok(lineStr, TELNET_LINE_TERM);
    // save the user name
    if(lineStr != NULL)
    {
        (void)strncpy(pDcpt->username, lineStr, sizeof(pDcpt->username) - 1U);
        pDcpt->username[sizeof(pDcpt->username) - 1U] = '\0';
    }
    else
    {
        pDcpt->username[0] = '\0';
    }

    (void)NET_PRES_SocketRead(tSkt, NULL, avlblBytes);
    // Print the password prompt
    (void)NET_PRES_SocketWrite(tSkt, FC_CStr2CVPtr(TELNET_ASK_PASSWORD_MSG), (uint16_t)strlen(TELNET_ASK_PASSWORD_MSG));
    return TCPIP_TELNET_GET_PASSWORD;

}

static TCPIP_TELNET_STATE F_Telnet_LogonCheck(TELNET_DCPT* pDcpt)
{
    uint16_t avlblBytes;
    bool    sktDisconnect, sktOverflow, authRes;
    NET_PRES_SKT_HANDLE_T tSkt;
    char*   lineStr;
    TELNET_MSG_LINE_RES lineRes;

    char    passMessage[M_TCPIP_TELNET_LINE_BUFF_SIZE];    // telnet confirmation message

    tSkt = pDcpt->telnetSkt;

    if(NET_PRES_SocketWriteIsReady(tSkt, TELNET_SKT_MESSAGE_SPACE, 0U) < TELNET_SKT_MESSAGE_SPACE)
    {   // wait some more
        return TCPIP_TELNET_GET_PASSWORD;
    }

    lineStr = passMessage;
    lineRes = F_Telnet_MessageLineCheck(tSkt, &lineStr, (uint16_t)sizeof(passMessage), &avlblBytes);

    if(lineRes == TELNET_MSG_LINE_PENDING)
    {   // wait some more
        return TCPIP_TELNET_GET_PASSWORD;
    }

    sktDisconnect = sktOverflow = false;

    if(lineRes == TELNET_MSG_LINE_OVFL)
    {
        sktOverflow = true;
    }
    else
    {   // TELNET_MSG_LINE_DONE
        // remove the line termination 
        lineStr = strtok(lineStr, TELNET_LINE_TERM);

        authRes = false;
        if(lineStr != NULL)
        {
#if (M_TELNET_USE_AUTHENTICATION_CALLBACK != 0)
            if(telnetAuthHandler != NULL)
            {
#if !defined(TCPIP_TELNET_AUTHENTICATION_CONN_INFO) || (TCPIP_TELNET_AUTHENTICATION_CONN_INFO != 0)
                // we need connection info
                TCPIP_TELNET_CONN_INFO connInfo;
                connInfo.connIx = FC_PtrDiff2UI16(pDcpt - telnetDcpt);
                connInfo.state = (uint16_t)TCPIP_TELNET_GET_PASSWORD;
                connInfo.presSkt = tSkt;
                connInfo.tcpSkt = NET_PRES_SocketGetTransportHandle(tSkt);
                (void)NET_PRES_SocketInfoGet(connInfo.tcpSkt, &connInfo.tcpInfo);
                authRes = (*telnetAuthHandler)(pDcpt->username, lineStr, &connInfo, telnetAuthHParam);
#else
                authRes = (*telnetAuthHandler)(pDcpt->username, lineStr, NULL, telnetAuthHParam);

#endif  // !defined(TCPIP_TELNET_AUTHENTICATION_CONN_INFO) || (TCPIP_TELNET_AUTHENTICATION_CONN_INFO != 0)
            }
#else
            // OBSOLETE authentication method
            // use the build time defaults
            authRes = strcmp(pDcpt->username, TCPIP_TELNET_USERNAME) == 0 && strcmp(lineStr, TCPIP_TELNET_PASSWORD) == 0;
#endif  // (M_TELNET_USE_AUTHENTICATION_CALLBACK != 0)
            // done with password
            (void)memset(passMessage, 0, sizeof(passMessage));
        }

        if(authRes == false)
        {   // failed
            sktDisconnect = true;
        }
    }

    // done with the user name
    (void)memset(pDcpt->username, 0, sizeof(pDcpt->username));

    if(sktOverflow)
    {
        (void)NET_PRES_SocketWrite(tSkt, FC_CStr2CVPtr(TELNET_BUFFER_OVFLOW_MSG), (uint16_t)strlen(TELNET_BUFFER_OVFLOW_MSG));
    }

    if(sktOverflow || sktDisconnect)
    {
        (void)NET_PRES_SocketWrite(tSkt, FC_CStr2CVPtr(TELNET_FAIL_LOGON_MSG), (uint16_t)strlen(TELNET_FAIL_LOGON_MSG));
        (void)NET_PRES_SocketDisconnect(tSkt);
        return TCPIP_TELNET_IDLE;   
    }

    // success
    (void)NET_PRES_SocketRead(tSkt, NULL, avlblBytes);  //  throw this line of data away
    // Print the authenticated prompt
    (void)NET_PRES_SocketWrite(tSkt, FC_CStr2CVPtr(TELNET_LOGON_OK), (uint16_t)strlen(TELNET_LOGON_OK));
    return TCPIP_TELNET_AUTHENTICATED;

}

// checks if a complete line is assembled
static TELNET_MSG_LINE_RES F_Telnet_MessageLineCheck(NET_PRES_SKT_HANDLE_T tSkt, char** pLineBuffer, uint16_t bufferSize, uint16_t* readBytes)
{
    uint16_t     avlblBytes, skipBytes;
    char    *lineBuffer, *lineTerm;
    const char    *cmdSkip;

    lineBuffer = *pLineBuffer;
    avlblBytes = NET_PRES_SocketPeek(tSkt, (uint8_t*)lineBuffer, bufferSize - 1U);
    
    skipBytes = 0U;
    if(avlblBytes != 0U)
    {   // we need at least one terminator character
        // make sure we have a complete line
        lineBuffer[avlblBytes] = '\0';

        // check for (unsupported) commands/options
        cmdSkip = F_Telnet_Process_CmdOptions(lineBuffer, avlblBytes);
        skipBytes = FC_ChPtrDiff2UI16(cmdSkip, lineBuffer);
        avlblBytes -= skipBytes;
    }

    if(skipBytes != 0U)
    {   // discard
        (void)NET_PRES_SocketRead(tSkt, NULL, skipBytes);
        lineBuffer += skipBytes;
    }

    if(avlblBytes != 0U)
    {
        lineTerm = strstr(lineBuffer, TELNET_LINE_RETURN);
        if(lineTerm == NULL)
        {
            lineTerm = strstr(lineBuffer, TELNET_LINE_FEED);
        }

        if(lineTerm != NULL)
        {
            *pLineBuffer = lineBuffer;
            *readBytes = avlblBytes;
            return TELNET_MSG_LINE_DONE;
        }

        // no end of line pressed yet
        if(avlblBytes == bufferSize - 1U)
        {   // our buffer is full: overflowed
            return TELNET_MSG_LINE_OVFL;
        }
        // else wait some more
    }

    return TELNET_MSG_LINE_PENDING;

}

// process telnet commands and options
// returns a pointer to the message after the options were processed
// Note: for now no telnet commands or options are supported
// The options are simply removed from the telnet message
static const char* F_Telnet_Process_CmdOptions(const char* strMsg, uint16_t avlblBytes)
{
    char c;

    while(avlblBytes != 0U)
    {
        if(*strMsg != TELNET_CMD_IAC_CODE)
        {
            break;
        }
        // start of command
        c = *++strMsg;
        if(c == TELNET_CMD_IAC_CODE)
        {   // this is data, not command
            break;
        }
        // valid command sequence
        if(c >= TELNET_CMD_OPT_CODE_BEG && c <= TELNET_CMD_OPT_CODE_END)
        {   // 3 Bytes option codes: skip option character that follows
            strMsg += 2;
        }
        else if(c >= TELNET_CMD_NOPT_CODE_BEG && c <= TELNET_CMD_NOPT_CODE_END)
        {   // 2 Bytes option codes;
            strMsg++;
        }
        else if(c == TELNET_CMD_SB_CODE)
        {   // eat up chars until you get to the end of option: TELNET_CMD_IAC_CODE + TELNET_CMD_SE_CODE
            const char* endSubOpt = NULL;
            if(avlblBytes > 2U)
            {   // at least "command + subnegotiation" bytes needed
                const char* srchEnd = strMsg + 1;
                avlblBytes -= 2U;
                while(avlblBytes > 1U)
                {
                    if(*srchEnd == TELNET_CMD_IAC_CODE && *(srchEnd + 1) == TELNET_CMD_SE_CODE)
                    {   // found the option end
                        endSubOpt = srchEnd + 2;
                        avlblBytes -= 2U;
                        break;
                    }
                    // continue
                    srchEnd++;
                    avlblBytes--;
                }
            }

            if(endSubOpt == NULL)
            {   // not found
                --strMsg;
                break;
            }
            else
            {
                strMsg = endSubOpt;
            }
        }
        else 
        {   // we don't support other commands for now
            --strMsg;
            break;
        }
    }

    return strMsg;

}

uint16_t TCPIP_TELNET_ConnectionsGet(void)
{
    if(telnetDcpt != NULL)
    {
        return telnetConfigData.nConnections;
    }

    return 0U;
}

bool TCPIP_TELNET_ConnectionInfoGet(uint16_t connIx, TCPIP_TELNET_CONN_INFO* pInfo)
{
    if(telnetDcpt != NULL && connIx < telnetConfigData.nConnections)
    {
        if(pInfo != NULL)
        {
            TELNET_DCPT* pDcpt = telnetDcpt + connIx;
            pInfo->connIx = connIx;
            pInfo->state = (uint16_t)pDcpt->telnetState;
            pInfo->presSkt = pDcpt->telnetSkt;
            pInfo->tcpSkt = NET_PRES_SocketGetTransportHandle(pDcpt->telnetSkt);
            (void)NET_PRES_SocketInfoGet(pInfo->tcpSkt, &pInfo->tcpInfo);
        }
        return true;
    }

    return false;
}


#if (M_TELNET_USE_AUTHENTICATION_CALLBACK != 0)
TCPIP_TELNET_HANDLE TCPIP_TELNET_AuthenticationRegister(TCPIP_TELNET_AUTH_HANDLER authHandler, const void* handlerParam)
{
    TCPIP_TELNET_HANDLE tHandle = NULL;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(telnetAuthHandler == NULL)
    {
        telnetAuthHParam = handlerParam;
        telnetAuthHandler = authHandler;
        tHandle = FC_AuthHndl2TelHndl(authHandler);
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return tHandle;
}

bool TCPIP_TELNET_AuthenticationDeregister(TCPIP_TELNET_HANDLE authHandle)
{
    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(FC_AuthHndl2TelHndl(telnetAuthHandler) == authHandle)
    {
        telnetAuthHandler = NULL;
        res = true;
    } 

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}

#endif  // (M_TELNET_USE_AUTHENTICATION_CALLBACK != 0)

#endif  //#if defined(TCPIP_STACK_USE_TELNET_SERVER)

