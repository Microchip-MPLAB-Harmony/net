/*******************************************************************************
  Telnet Server

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    -Provides Telnet services on TCP port 23
    -Reference: RFC 854
*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2016-2018 Microchip Technology Inc. and its subsidiaries.

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
#define TELNET_START_MSG                "\x1b[2J\x1b[31m\x1b[1m" \
                                "Microchip Telnet Server 1.1\x1b[0m\r\n" \
								"Login: "
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
#define _TCPIP_TELNET_LINE_BUFF_SIZE    (TCPIP_TELNET_LINE_BUFF_SIZE + 3) // + extra room for \r\n 

#define TELNET_SKT_MESSAGE_SPACE    80      // min space needed in the socket buffer for displaying messages

// authentication defines
#if (TCPIP_TELNET_OBSOLETE_AUTHENTICATION == 0)
#define _TELNET_USE_AUTHENTICATION_CALLBACK     1
#else
#define _TELNET_USE_AUTHENTICATION_CALLBACK     0
#endif  // (TCPIP_TELNET_OBSOLETE_AUTHENTICATION == 0)

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
    char                    username[TCPIP_TELNET_USERNAME_SIZE + 1];   // username for the connection login
}TELNET_DCPT;

static const void*      telnetHeapH = 0;                    // memory allocation handle
static TELNET_DCPT*     telnetDcpt = 0; // array of descriptors, each for a connection

static int              telnetInitCount = 0;      // TELNET module initialization count

static tcpipSignalHandle telnetSignalHandle = 0;

static TCPIP_TELNET_MODULE_CONFIG telnetConfigData;   // initialization data

#if (_TELNET_USE_AUTHENTICATION_CALLBACK != 0)
static TCPIP_TELNET_AUTH_HANDLER telnetAuthHandler;
static const void* telnetAuthHParam;
#endif  // (_TELNET_USE_AUTHENTICATION_CALLBACK != 0)

// prototypes
static void _Telnet_MSG(const void* cmdIoParam, const char* str);
static void _Telnet_PRINT(const void* cmdIoParam, const char* format, ...);
static void _Telnet_PUTC(const void* cmdIoParam, char c);
static int  _Telnet_DATA_RDY(const void* cmdIoParam);
static char _Telnet_GETC(const void* cmdIoParam);

static void _Telnet_Deregister(TELNET_DCPT* pDcpt);
#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void _Telnet_Cleanup(void);
#else
#define _Telnet_Cleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static TCPIP_TELNET_STATE _Telnet_UserCheck(TELNET_DCPT* pDcpt);
static TCPIP_TELNET_STATE _Telnet_LogonCheck(TELNET_DCPT* pDcpt);
static TELNET_MSG_LINE_RES _Telnet_MessageLineCheck(NET_PRES_SKT_HANDLE_T tSkt, char** pLineBuffer, int bufferSize, int* readBytes);
static char* _Telnet_Process_CmdOptions(const char* strMsg, int avlblBytes);

static void TCPIP_TELNET_Process(void);

static void _TelnetSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param);

static const SYS_CMD_API telnetIOApi =
{
    _Telnet_MSG,
    _Telnet_PRINT,
    _Telnet_PUTC,
    _Telnet_DATA_RDY,
    _Telnet_GETC
};



// implementation
//


bool TCPIP_TELNET_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_TELNET_MODULE_CONFIG* pTelConfig)
{

    int tIx;
    TELNET_DCPT* pDcpt;
    NET_PRES_SKT_HANDLE_T tSkt;
    TCPIP_TCP_SIGNAL_HANDLE sigHandle;
    bool    initFail = false;

    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    // interface restart


    // stack init

    while(telnetInitCount == 0)
    {   // first time we're run

        // check configuration data is not missing
        if(stackCtrl->memH == 0 || pTelConfig == 0 || pTelConfig->nConnections == 0)
        {
            SYS_ERROR(SYS_ERROR_ERROR, "telnet NULL dynamic allocation handle/init data");
            return false;
        }

        telnetHeapH = stackCtrl->memH;
        telnetConfigData = *pTelConfig;
        // allocate data
        telnetDcpt = (TELNET_DCPT*)TCPIP_HEAP_Calloc(telnetHeapH, telnetConfigData.nConnections, sizeof(*telnetDcpt)); 
        if(telnetDcpt == 0)
        {
            SYS_ERROR(SYS_ERROR_ERROR, " telnet Dynamic allocation failed");
            return false;
        }

        // initialize telnetDcpt and open the server sockets
        pDcpt = telnetDcpt;
        for(tIx = 0; tIx < telnetConfigData.nConnections; tIx++, pDcpt++)
        {
            pDcpt->telnetSkt = (tSkt = NET_PRES_SocketOpen(0, NET_PRES_SKT_DEFAULT_STREAM_SERVER, IP_ADDRESS_TYPE_ANY, telnetConfigData.listenPort, 0, 0));
            sigHandle = NET_PRES_SocketSignalHandlerRegister(tSkt, TCPIP_TCP_SIGNAL_RX_DATA, _TelnetSocketRxSignalHandler, 0);
            if(tSkt == INVALID_SOCKET || sigHandle == 0)
            {
                initFail = true;
                break;
            }
            // set socket options
            if((telnetConfigData.configFlags & TCPIP_TELNET_FLAG_NO_DELAY) != 0)
            {
                void* tcpForceFlush = (void*)1;
                NET_PRES_SocketOptionsSet(tSkt, TCP_OPTION_NODELAY, tcpForceFlush);
            }
            if(telnetConfigData.sktTxBuffSize != 0)
            {
                void* tcpBuffSize = (void*)(unsigned int)telnetConfigData.sktTxBuffSize;
                if(!NET_PRES_SocketOptionsSet(tSkt, TCP_OPTION_TX_BUFF, tcpBuffSize))
                {
                    SYS_ERROR(SYS_ERROR_WARNING, " telnet: Setting TX Buffer failed: %d\r\n", tIx);
                }
            }
            if(telnetConfigData.sktRxBuffSize != 0)
            {
                void* tcpBuffSize = (void*)(unsigned int)telnetConfigData.sktRxBuffSize;
                if(!NET_PRES_SocketOptionsSet(tSkt, TCP_OPTION_RX_BUFF, tcpBuffSize))
                {
                    SYS_ERROR(SYS_ERROR_WARNING, " telnet: Setting RX Buffer failed: %d\r\n", tIx);
                }
            }
        }

        if(!initFail)
        {   // create the telnet timer
            telnetSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_TELNET_Task, TCPIP_TELNET_TASK_TICK_RATE);
            if(telnetSignalHandle == 0)
            {   // cannot create the telnet timer
                initFail = true;
            }
        }
#if (_TELNET_USE_AUTHENTICATION_CALLBACK != 0)
        telnetAuthHandler = 0;
#endif  // (_TELNET_USE_AUTHENTICATION_CALLBACK != 0)

        break;

    }

    if(initFail)
    {
        _Telnet_Cleanup();
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
        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--telnetInitCount == 0)
            {   // all closed
                _Telnet_Cleanup();
            }
        }
    }

}

static void _Telnet_Cleanup(void)
{
    int tIx;
    TELNET_DCPT* pDcpt;

    pDcpt = telnetDcpt;
    for(tIx = 0; tIx < telnetConfigData.nConnections; tIx++, pDcpt++)
    {
        _Telnet_Deregister(pDcpt);

        if( pDcpt->telnetSkt != INVALID_SOCKET)
        {
            NET_PRES_SocketClose(pDcpt->telnetSkt);
        }
    }

    TCPIP_HEAP_Free(telnetHeapH, telnetDcpt);
    telnetDcpt = 0;
    telnetConfigData.nConnections = 0;

    if(telnetSignalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(telnetSignalHandle);
        telnetSignalHandle = 0;
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)


static void _Telnet_Deregister(TELNET_DCPT* pDcpt)
{
    if (pDcpt->telnetIO != 0)
    {
        SYS_CMD_DELETE(pDcpt->telnetIO);
        pDcpt->telnetIO = 0;
    }

}

// Telnet's PUTC
static void _Telnet_PUTC(const void* cmdIoParam, char c)
{
    NET_PRES_SKT_HANDLE_T tSkt = (NET_PRES_SKT_HANDLE_T)(int)cmdIoParam;
    if(tSkt != INVALID_SOCKET)
    {
        NET_PRES_SocketWrite(tSkt, &c, 1);
    }
}

// Telnet's message	
static void _Telnet_MSG(const void* cmdIoParam, const char* str)
{
    NET_PRES_SKT_HANDLE_T tSkt = (NET_PRES_SKT_HANDLE_T)(int)cmdIoParam;
    if(tSkt != INVALID_SOCKET)
    {
        NET_PRES_SocketWrite(tSkt, (const uint8_t*)str, strlen(str));
    }
}


// Telnet's print
static void _Telnet_PRINT(const void* cmdIoParam, const char* format, ...)
{
    va_list arg_list = {0};
    char buff[TCPIP_TELNET_PRINT_BUFF_SIZE];

    va_start(arg_list, format);
    vsnprintf(buff, TCPIP_TELNET_PRINT_BUFF_SIZE, format, arg_list); 
    va_end(arg_list);

    _Telnet_MSG(cmdIoParam, buff);
}


// Telnet's data ready
static int _Telnet_DATA_RDY(const void* cmdIoParam)
{
    NET_PRES_SKT_HANDLE_T tSkt = (NET_PRES_SKT_HANDLE_T)(int)cmdIoParam;
    if(tSkt != INVALID_SOCKET)
    {
        return NET_PRES_SocketReadIsReady(tSkt);
    }

    return 0;
}

// Telnet's getc
// it removes the control characters from the stream
static char _Telnet_GETC(const void* cmdIoParam)
{
    uint8_t bData;

    NET_PRES_SKT_HANDLE_T tSkt = (NET_PRES_SKT_HANDLE_T)(int)cmdIoParam;
    if(tSkt == INVALID_SOCKET)
    {
        return 0;
    }

    if((telnetConfigData.configFlags & TCPIP_TELNET_FLAG_PASS_CONTROL_CHARS) != 0)
    {
        if (NET_PRES_SocketRead(tSkt, &bData, 1))
        {
            return (char)bData;
        }
        return 0;
    }

    // remove the control chars from the console stream
    char    lineBuffer[_TCPIP_TELNET_LINE_BUFF_SIZE];    // telnet line buffer

    int avlblBytes = NET_PRES_SocketPeek(tSkt, (uint8_t*)lineBuffer, sizeof(lineBuffer) - 1);

    if(avlblBytes)
    {
        char* lineEnd = lineBuffer + avlblBytes;
        *lineEnd = 0;
        
        char* linePtr = _Telnet_Process_CmdOptions(lineBuffer, avlblBytes);
    
        avlblBytes = lineEnd - linePtr;

        if(avlblBytes)
        {
            bData = *linePtr++;
        }
        else
        {
            bData = 0;
        }

        // discard consumed and unused
        int discardBytes = linePtr - lineBuffer;
        if(discardBytes != 0)
        {
            NET_PRES_SocketRead(tSkt, 0, discardBytes);
        }
        return (char)bData;
    }

    return 0;

}

void TCPIP_TELNET_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;
    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != 0)
    { // TMO or RX signal occurred
        TCPIP_TELNET_Process();
    }

}

// send a signal to the telnet module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void _TelnetSocketRxSignalHandler(NET_PRES_SKT_HANDLE_T hTCP, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param)
{
    if(sigType == TCPIP_TCP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}


static void TCPIP_TELNET_Process(void)
{
    int         tIx;
    TELNET_DCPT* pDcpt;
    NET_PRES_SKT_HANDLE_T	tSocket;
    TCPIP_TELNET_STATE tState;


    // Loop through each telnet session and process state changes and TX/RX data
    pDcpt = telnetDcpt;
    for(tIx = 0; tIx < telnetConfigData.nConnections; tIx++, pDcpt++)
    {
        // Load up static state information for this session
        tSocket = pDcpt->telnetSkt;
        tState = pDcpt->telnetState;

        // Reset our state if the remote client disconnected from us
        if(NET_PRES_SocketWasReset(tSocket) || NET_PRES_SocketWasDisconnected(tSocket))
        {
            NET_PRES_SocketDisconnect(tSocket);
            // Deregister IO and free its space
            _Telnet_Deregister(pDcpt);
            tState = TCPIP_TELNET_IDLE;
        }

        // Handle session state
        switch(tState)
        {
            case TCPIP_TELNET_IDLE:
                // Make certain the socket can be written to
                if(NET_PRES_SocketWriteIsReady(tSocket, TELNET_SKT_MESSAGE_SPACE, 0) < TELNET_SKT_MESSAGE_SPACE)
                    break;

                // Place the application protocol data into the transmit buffer.
                NET_PRES_SocketWrite(tSocket, (const uint8_t*)TELNET_START_MSG, strlen(TELNET_START_MSG));
                
                // Send the packet
                NET_PRES_SocketFlush(tSocket);
                tState++;

            case TCPIP_TELNET_GET_LOGIN:
                tState = _Telnet_UserCheck(pDcpt);
                break;

            case TCPIP_TELNET_GET_PASSWORD:
                tState = _Telnet_LogonCheck(pDcpt);
                break;

            case TCPIP_TELNET_AUTHENTICATED:
                if(NET_PRES_SocketWriteIsReady(tSocket, TELNET_SKT_MESSAGE_SPACE, 0) < TELNET_SKT_MESSAGE_SPACE)
                    break;

                NET_PRES_SocketWrite(tSocket, (const uint8_t*)TELNET_WELCOME_MSG, strlen(TELNET_WELCOME_MSG));
                tState++;

                NET_PRES_SocketFlush(tSocket);

                // Register telnet as cmd IO device
                pDcpt->telnetIO = SYS_CMDIO_ADD(&telnetIOApi, (const void*)(int)tSocket, 0);
                if (pDcpt->telnetIO == 0)
                {
                    NET_PRES_SocketWrite(tSocket, (const uint8_t*)TELNET_FAIL_CMD_REGISTER, strlen(TELNET_FAIL_CMD_REGISTER));
                    NET_PRES_SocketDisconnect(tSocket);
                    tState = TCPIP_TELNET_IDLE;
                    break;
                }	

            case TCPIP_TELNET_CONNECTED:
                // Check if you're disconnected and de-register from the command processor

                break;
        }


        // Save session state back into the static array
        pDcpt->telnetState = tState;
    }

}

static TCPIP_TELNET_STATE _Telnet_UserCheck(TELNET_DCPT* pDcpt)
{
    int         avlblBytes;
    char        *lineStr;
    NET_PRES_SKT_HANDLE_T tSkt;
    TELNET_MSG_LINE_RES lineRes;

    char    userMessage[_TCPIP_TELNET_LINE_BUFF_SIZE];    // telnet confirmation message

    tSkt = pDcpt->telnetSkt;
    if(NET_PRES_SocketWriteIsReady(tSkt, TELNET_SKT_MESSAGE_SPACE, 0) < TELNET_SKT_MESSAGE_SPACE)
    {   // wait some more
        return TCPIP_TELNET_GET_LOGIN;
    }

    lineStr = userMessage;
    lineRes = _Telnet_MessageLineCheck(tSkt, &lineStr, sizeof(userMessage), &avlblBytes);

    if(lineRes == TELNET_MSG_LINE_PENDING)
    {   // wait some more
        return TCPIP_TELNET_GET_LOGIN;
    }
    else if(lineRes == TELNET_MSG_LINE_OVFL)
    {
        NET_PRES_SocketWrite(tSkt, (const uint8_t*)TELNET_BUFFER_OVFLOW_MSG, strlen(TELNET_BUFFER_OVFLOW_MSG));
        NET_PRES_SocketWrite(tSkt, (const uint8_t*)TELNET_FAIL_LOGON_MSG, strlen(TELNET_FAIL_LOGON_MSG));
        NET_PRES_SocketDisconnect(tSkt);
        return TCPIP_TELNET_IDLE;	
    }

    // TELNET_MSG_LINE_DONE
    // remove the line termination 
    lineStr = strtok(lineStr, TELNET_LINE_TERM);
    // save the user name
    if(lineStr)
    {
        strncpy(pDcpt->username, lineStr, sizeof(pDcpt->username) - 1);
        pDcpt->username[sizeof(pDcpt->username) - 1] = 0;
    }
    else
    {
        pDcpt->username[0] = 0;
    }

    NET_PRES_SocketRead(tSkt, 0, avlblBytes);
    // Print the password prompt
    NET_PRES_SocketWrite(tSkt, (const uint8_t*)TELNET_ASK_PASSWORD_MSG, strlen(TELNET_ASK_PASSWORD_MSG));
    return TCPIP_TELNET_GET_PASSWORD;

}

static TCPIP_TELNET_STATE _Telnet_LogonCheck(TELNET_DCPT* pDcpt)
{
    int     avlblBytes;
    bool    sktDisconnect, sktOverflow, authRes;
    NET_PRES_SKT_HANDLE_T tSkt;
    char*   lineStr;
    TELNET_MSG_LINE_RES lineRes;

    char    passMessage[_TCPIP_TELNET_LINE_BUFF_SIZE];    // telnet confirmation message

    tSkt = pDcpt->telnetSkt;

    if(NET_PRES_SocketWriteIsReady(tSkt, TELNET_SKT_MESSAGE_SPACE, 0) < TELNET_SKT_MESSAGE_SPACE)
    {   // wait some more
        return TCPIP_TELNET_GET_PASSWORD;
    }

    lineStr = passMessage;
    lineRes = _Telnet_MessageLineCheck(tSkt, &lineStr, sizeof(passMessage), &avlblBytes);

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
        if(lineStr)
        {
#if (_TELNET_USE_AUTHENTICATION_CALLBACK != 0)
            if(telnetAuthHandler != 0)
            {
#if (TCPIP_TELNET_AUTHENTICATION_CONN_INFO != 0)
                // we need connection info
                TCPIP_TELNET_CONN_INFO connInfo;
                connInfo.connIx = pDcpt - telnetDcpt;
                connInfo.state = TCPIP_TELNET_GET_PASSWORD;
                connInfo.presSkt = tSkt;
                connInfo.tcpSkt = NET_PRES_SocketGetTransportHandle(tSkt);
                TCPIP_TCP_SocketInfoGet(connInfo.tcpSkt, &connInfo.tcpInfo);
                authRes = (*telnetAuthHandler)(pDcpt->username, lineStr, &connInfo, telnetAuthHParam);
#else
                authRes = (*telnetAuthHandler)(pDcpt->username, lineStr, 0, telnetAuthHParam);

#endif  // (TCPIP_TELNET_AUTHENTICATION_CONN_INFO != 0)
            }
#else
            // OBSOLETE authentication method
            // use the build time defaults
            authRes = strcmp(pDcpt->username, TCPIP_TELNET_USERNAME) == 0 && strcmp(lineStr, TCPIP_TELNET_PASSWORD) == 0;
#endif  // (_TELNET_USE_AUTHENTICATION_CALLBACK != 0)
            // done with password
            memset(passMessage, 0, sizeof(passMessage));
        }

        if(authRes == false)
        {   // failed
            sktDisconnect = true;
        }
    }

    // done with the user name
    memset(pDcpt->username, 0, sizeof(pDcpt->username));

    if(sktOverflow)
    {
        TCPIP_TCP_StringPut(tSkt, (const uint8_t*)TELNET_BUFFER_OVFLOW_MSG);
    }

    if(sktOverflow || sktDisconnect)
    {
        NET_PRES_SocketWrite(tSkt, (const uint8_t*)TELNET_FAIL_LOGON_MSG, strlen(TELNET_FAIL_LOGON_MSG));
        NET_PRES_SocketDisconnect(tSkt);
        return TCPIP_TELNET_IDLE;	
    }

    // success
    NET_PRES_SocketRead(tSkt, 0, avlblBytes);  //  throw this line of data away
    // Print the authenticated prompt
    NET_PRES_SocketWrite(tSkt, (const uint8_t*)TELNET_LOGON_OK, strlen(TELNET_LOGON_OK));
    return TCPIP_TELNET_AUTHENTICATED;

}

// checks if a complete line is assembled
static TELNET_MSG_LINE_RES _Telnet_MessageLineCheck(NET_PRES_SKT_HANDLE_T tSkt, char** pLineBuffer, int bufferSize, int* readBytes)
{
    int     avlblBytes, skipBytes;
    char    *lineBuffer, *lineTerm, *cmdSkip;

    lineBuffer = *pLineBuffer;
    avlblBytes = NET_PRES_SocketPeek(tSkt, (uint8_t*)lineBuffer, bufferSize - 1);
    
    skipBytes = 0;
    if(avlblBytes)
    {   // we need at least one terminator character
        // make sure we have a complete line
        lineBuffer[avlblBytes] = 0;

        // check for (unsupported) commands/options
        cmdSkip = _Telnet_Process_CmdOptions(lineBuffer, avlblBytes);
        skipBytes = cmdSkip - lineBuffer;
        avlblBytes -= skipBytes;
    }

    if(skipBytes)
    {   // discard
        NET_PRES_SocketRead(tSkt, 0, skipBytes);
        lineBuffer += skipBytes;
    }

    if(avlblBytes)
    {
        lineTerm = strstr(lineBuffer, TELNET_LINE_RETURN);
        if(lineTerm == 0)
        {
            lineTerm = strstr(lineBuffer, TELNET_LINE_FEED);
        }

        if(lineTerm != 0)
        {
            *pLineBuffer = lineBuffer;
            *readBytes = avlblBytes;
            return TELNET_MSG_LINE_DONE;
        }

        // no end of line pressed yet
        if(avlblBytes == bufferSize - 1)
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
static char* _Telnet_Process_CmdOptions(const char* strMsg, int avlblBytes)
{
    char c;

    while(avlblBytes)
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
            const char* endSubOpt = 0;
            if(avlblBytes > 2)
            {   // at least "command + subnegotiation" bytes needed
                const char* srchEnd = strMsg + 1;
                avlblBytes -= 2;
                while(avlblBytes > 1)
                {
                    if(*srchEnd == TELNET_CMD_IAC_CODE && *(srchEnd + 1) == TELNET_CMD_SE_CODE)
                    {   // found the option end
                        endSubOpt = srchEnd + 2;
                        avlblBytes -= 2;
                        break;
                    }
                    // continue
                    srchEnd++;
                    avlblBytes--;
                }
            }

            if(endSubOpt == 0)
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

    return (char*)strMsg;

}

uint16_t TCPIP_TELNET_ConnectionsGet(void)
{
    if(telnetDcpt != 0)
    {
        return telnetConfigData.nConnections;
    }

    return 0;
}

bool TCPIP_TELNET_ConnectionInfoGet(uint16_t connIx, TCPIP_TELNET_CONN_INFO* pInfo)
{
    if(telnetDcpt != 0 && connIx < telnetConfigData.nConnections)
    {
        if(pInfo)
        {
            TELNET_DCPT* pDcpt = telnetDcpt + connIx;
            pInfo->connIx = connIx;
            pInfo->state = pDcpt->telnetState;
            pInfo->presSkt = pDcpt->telnetSkt;
            pInfo->tcpSkt = NET_PRES_SocketGetTransportHandle(pDcpt->telnetSkt);
            TCPIP_TCP_SocketInfoGet(pInfo->tcpSkt, &pInfo->tcpInfo);
        }
        return true;
    }

    return false;
}


#if (_TELNET_USE_AUTHENTICATION_CALLBACK != 0)
TCPIP_TELNET_HANDLE TCPIP_TELNET_AuthenticationRegister(TCPIP_TELNET_AUTH_HANDLER authHandler, const void* handlerParam)
{
    TCPIP_TELNET_HANDLE tHandle = 0;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(telnetAuthHandler == 0)
    {
        telnetAuthHParam = handlerParam;
        telnetAuthHandler = authHandler;
        tHandle = (TCPIP_TELNET_HANDLE) authHandler;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return tHandle;
}

bool TCPIP_TELNET_AuthenticationDeregister(TCPIP_TELNET_HANDLE authHandle)
{
    bool res = false;
    OSAL_CRITSECT_DATA_TYPE critSect =  OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if(telnetAuthHandler == (TCPIP_TELNET_AUTH_HANDLER) authHandle)
    {
        telnetAuthHandler = 0;
        res = true;
    } 

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);
    return res;
}

#endif  // (_TELNET_USE_AUTHENTICATION_CALLBACK != 0)

#endif	//#if defined(TCPIP_STACK_USE_TELNET_SERVER)

