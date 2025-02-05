/*******************************************************************************
  Dynamic DNS Client Module

  Summary:
    Reference: DNS Update API Version 2.0.3 (www.dyndns.com)
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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_DYNDNS_CLIENT

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)


// Response codes from DynDNS Update Server
static const char* updateIpSrvrResponse[] =
{
    /* 0 */  "good",        // Update was successful
    /* 1 */  "nochg",       // No change was made; request is considered abusive
    /* 2 */  "abuse",       // Account has been blocked for abuse
    /* 3 */  "badsys",      // System is not supported
    /* 4 */  "badagent",    // Agent has been blocked for abuse
    /* 5 */  "badauth",     // Authentication failed
    /* 6 */  "!donator",    // A paid account feature was requested on a free account
    /* 7 */  "notfqdn",     // Hostname was not a fully-qualified domain name
    /* 8 */  "nohost",      // Host name was not found in the system
    /* 9 */  "!yours",      // The specified hostname does not belong to this account
    /* 10 */ "numhost",     // Number of hosts does not match / serious error
    /* 11 */ "dnserr",      // System error was encountered, try again soon
    /* 12 */ "911",         // System error was encountered, try again later
};

/****************************************************************************
  Section:
    Dynamic DNS Services
    These services must support the DynDNS API, and correspond to
    DDNS_SERVICES enumeration
  ***************************************************************************/

    // Host names for various Dynamic DNS services
const char * const ddnsServiceHosts[] =
{
    "members.dyndns.org",       // DYNDNS_ORG
    "dynupdate.no-ip.com",      // NO_IP_COM
    "updates.dnsomatic.com",    // DNSOMATIC_COM
};

    // Port numbers for various Dynamic DNS services
static const uint16_t ddnsServicePorts[] =
{
    80,                         // DYNDNS_ORG
    80,                         // NO_IP_COM
    80,                         // DNSOMATIC_COM
};

/****************************************************************************
Section:
Global Variables
 ***************************************************************************/

typedef enum
{
    SM_IDLE = 0u,
    SM_BEGIN_CHECKIP,
    SM_DNS_START_RESOLVE,
    SM_DNS_WAIT_RESOLVE,
    SM_CHECKIP_SKT_OBTAINED,
    SM_CHECKIP_FIND_DELIMITER,
    SM_CHECKIP_FIND_ADDRESS,
    SM_CHECKIP_DISCONNECT,
    SM_IP_UPDATE_HOME,
    SM_IP_UPDATE_WAIT_DNS,
    SM_IP_UPDATE_SKT_OBTAINED,

    /*
       HTTP request msg is divided into 6 parts
       SM_IP_UPDATE_REQ_A,B,C,D,E,F as the tcp ip tx
       buffer is only able to carry 200 bytes at a time.
       */

    SM_IP_UPDATE_REQ_A,             //0x8
    SM_IP_UPDATE_REQ_B,             //0x9
    SM_IP_UPDATE_REQ_C,             //0xa
    SM_IP_UPDATE_REQ_D,             //0xb
    SM_IP_UPDATE_REQ_E,             //0xc
    SM_IP_UPDATE_REQ_F,             //0xd

    SM_IPUPDATE_FIND_RESPONSE,      //0xe
    SM_IPUPDATE_PARSE_RESPONSE,     //0xf
    SM_IPUDATE_DISCONNECT,          //0x10
    SM_DONE,                        // Done, try again in 10 minutes
    SM_DNS_ERROR,                   // DNS resolver error, try again in 30 seconds
    SM_SKT_ERROR,                   // socket open error, try again in 30 seconds
    SM_SOFT_ERROR,                  // Soft error, try again in 30 seconds
    SM_SYSTEM_ERROR                 // System error, try again in 30 minutes
}TCPIP_DDNS_STATE;

static union
{
    int                 val;
    TCPIP_DDNS_STATE    state;
}smDDNS = {0};

static IPV4_ADDR lastKnownIP;       // Last known IP address of this device
static DDNS_STATUS lastStatus;  // Status response from last update

DDNS_POINTERS DDNSClient;       // Configuration parameters for the module

static uint32_t dwUpdateAt = 0U;// Indicates when the next CheckIP should be done
static bool bForceUpdate;       // Indicates that the update should be done regardless
                                // of whether or not the IP changed.  Use this flag
                                // when the user/pass/hostname have changed.

// Indicates how many interfaces are up.
static int ddnsInitCount = 0;
static TCPIP_SIGNAL_HANDLE   ddnsSignalHandle = NULL;


static uint32_t     DDnsTimer;
static TCP_SOCKET   MySocket = INVALID_SOCKET;
static char const*  ROMStrPtr;
static char *       RAMStrPtr;
static IPV4_ADDR    ipParsed;
// the server address
static IPV4_ADDR    ddnsServerIP;
static IPV4_ADDR    ddnsUpdateIP;

static uint8_t      vBuffer[16];
static union
{
    uint8_t*    uPtr;
    char*       cPtr;
}vBufferPtr;


static uint32_t  tickFreq;

static void TCPIP_DDNS_Process(void);

static void DDNSSocketRxSignalHandler(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, TCPIP_TCP_SIGNAL_TYPE sigType, const void* param);

// ddns_manager.h
bool TCPIP_DDNS_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackData, const void* initData)
{
    if (stackData->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    if (ddnsInitCount == 0)
    {
        // create the NBNS timer
        ddnsSignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_DDNS_Task, TCPIP_DDNS_TASK_TICK_RATE);
        if(ddnsSignalHandle == NULL)
        {   // cannot create the NBNS timer
            return false;
        }

        // Clear the Dynamic DNS Client to start
        (void)memset((void*)&DDNSClient, 0x00, sizeof(DDNSClient));

        // Use the default Check IP server
        DDNSClient.ROMPointers.CheckIPServer = 1U;
        DDNSClient.CheckIPServer.czROM = TCPIP_DDNS_CHECKIP_SERVER;
        DDNSClient.CheckIPPort = TCPIP_DDNS_SERVER_REMOTE_PORT;

        dwUpdateAt = 0;
        bForceUpdate = true;
        lastStatus = DDNS_STATUS_UNKNOWN;
        smDDNS.state = SM_IDLE;

        vBufferPtr.uPtr = vBuffer; 
        tickFreq = SYS_TMR_TickCounterFrequencyGet(); 
    }

    ddnsInitCount++;

    return true;
}


// ddns_manager.h
#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_DDNS_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData)
{
    if (ddnsInitCount != 0)
    {
        if (stackData->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {
            if (--ddnsInitCount == 0)
            {
                // All interfaces are down, clean up.
                (void)memset((void*)&DDNSClient, 0x00, sizeof(DDNSClient));
                bForceUpdate = false;
                lastStatus = DDNS_STATUS_UNKNOWN;
                if(ddnsSignalHandle != NULL)
                {
                    TCPIPStackSignalHandlerDeregister(ddnsSignalHandle);
                    ddnsSignalHandle = NULL;
                }
            }
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_DDNS_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if((uint32_t)sigPend != 0U)
    { // TMO or RX signals occurred
        TCPIP_DDNS_Process();
    }

}


// send a signal to the DDNS module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void DDNSSocketRxSignalHandler(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, TCPIP_TCP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_TCP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}



static void TCPIP_DDNS_Process(void)
{
    uint8_t                 i;
    uint16_t wPos;
    TCPIP_DNS_RESULT      dnsRes;

    uint32_t currTick = SYS_TMR_TickCountGet();
    switch(smDDNS.state)
    {
        case SM_IDLE:

            if(dwUpdateAt == 0U)
            {   // First update is 15 seconds after boot, allowing DHCP to stabilize
                dwUpdateAt = currTick + 15U * tickFreq;
                break;
            }

            // Wait for timeout to begin IP check
            if(((int32_t)currTick - (int32_t)dwUpdateAt) < 0)
            {
                break;
            }

            // Otherwise, continue to next state
            smDDNS.state = SM_BEGIN_CHECKIP;
            break;

        case SM_BEGIN_CHECKIP:

            // If a fatal error has occurred, abort to the SM_DONE state and keep
            // the error message.
            if(lastStatus >= DDNS_STATUS_ABUSE && lastStatus <= DDNS_STATUS_911)
            {
                smDDNS.state = SM_DONE;
                break;
            }

            // If DDNSClient is not properly configured, abort
            if(
                // Verify that each pointer is not null, and is not empty
                (DDNSClient.ROMPointers.Host != 0U && (DDNSClient.Host.szROM == NULL || *DDNSClient.Host.czROM == '\0') ) ||
                (DDNSClient.ROMPointers.Host == 0U && (DDNSClient.Host.szRAM == NULL || *DDNSClient.Host.czRAM == '\0') ) ||
                (DDNSClient.ROMPointers.Username != 0U && (DDNSClient.Username.szROM ==NULL || *DDNSClient.Username.czROM == '\0') ) ||
                (DDNSClient.ROMPointers.Username == 0U && (DDNSClient.Username.szRAM == NULL || *DDNSClient.Username.czRAM == '\0') ) ||
                (DDNSClient.ROMPointers.Password != 0U && (DDNSClient.Password.szROM == NULL|| *DDNSClient.Password.czROM == '\0') ) ||
                (DDNSClient.ROMPointers.Password == 0U && (DDNSClient.Password.szRAM == NULL || *DDNSClient.Password.czRAM == '\0') ) ||
                (DDNSClient.ROMPointers.CheckIPServer != 0U && (DDNSClient.CheckIPServer.szROM == NULL || *DDNSClient.CheckIPServer.czROM == '\0') ) ||
                (DDNSClient.ROMPointers.CheckIPServer == 0U && (DDNSClient.CheckIPServer.szRAM == NULL || *DDNSClient.CheckIPServer.czRAM == '\0') ) ||
                (DDNSClient.ROMPointers.UpdateServer != 0U && (DDNSClient.UpdateServer.szROM == NULL || *DDNSClient.UpdateServer.czROM == '\0') ) ||
                (DDNSClient.ROMPointers.UpdateServer == 0U && (DDNSClient.UpdateServer.szRAM == NULL || *DDNSClient.UpdateServer.czRAM == '\0') )
            )
            {
                smDDNS.state = SM_SOFT_ERROR;
                lastStatus = DDNS_STATUS_INVALID;
                break;
            }

            // Start with an invalidated IP String
            vBufferPtr.cPtr[0] = '\0';

            smDDNS.val++;
            break;

        case SM_DNS_START_RESOLVE:

            // resolve the remote server
            if(DDNSClient.ROMPointers.CheckIPServer != 0U)
            {
                (void)TCPIP_DNS_Resolve(DDNSClient.CheckIPServer.czROM, TCPIP_DNS_TYPE_A);
            }
            else
            {
                (void)TCPIP_DNS_Resolve(DDNSClient.CheckIPServer.czRAM, TCPIP_DNS_TYPE_A);
            }

            smDDNS.val++;
            break;

        case SM_DNS_WAIT_RESOLVE:

            if(DDNSClient.ROMPointers.CheckIPServer != 0U)
            {
                dnsRes = TCPIP_DNS_IsNameResolved(DDNSClient.CheckIPServer.czROM, &ddnsServerIP, NULL);
            }
            else
            {
                dnsRes = TCPIP_DNS_IsNameResolved(DDNSClient.CheckIPServer.czRAM, &ddnsServerIP, NULL);
            }

            if(dnsRes == TCPIP_DNS_RES_PENDING)
            {   // ongoing operation;
                break;
            }

            if((ssize_t)dnsRes < 0)
            {   // some DNS error occurred; retry later
                lastStatus = DDNS_STATUS_DNS_ERROR;
                smDDNS.state = SM_DNS_ERROR;
                break;
            }

            // server IP solved
            // open the client socket
            MySocket = TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE_IPV4, DDNSClient.CheckIPPort, FC_Ip4Add2Multi(&ddnsServerIP));

            // If no socket available, try again later
            if(MySocket == INVALID_SOCKET)
            {
                lastStatus = DDNS_STATUS_SKT_ERROR;
                smDDNS.state = SM_SKT_ERROR;
                break;
            }
            (void)TCPIP_TCP_SignalHandlerRegister(MySocket, TCPIP_TCP_SIGNAL_RX_DATA, &DDNSSocketRxSignalHandler, NULL);

            // socket opened OK
            smDDNS.val++;
            DDnsTimer = currTick;
            break;

        case SM_CHECKIP_SKT_OBTAINED:

            // Wait for the remote server to accept our connection request
            if(!TCPIP_TCP_IsConnected(MySocket))
            {
                // Time out if too much time is spent in this state
                if(currTick-DDnsTimer > 6U * tickFreq)
                {
                    // Close the socket so it can be used by other modules
                    // We will retry soon
                    (void)TCPIP_TCP_Close(MySocket);
                    MySocket = INVALID_SOCKET;
                    lastStatus = DDNS_STATUS_CHECKIP_ERROR;
                    smDDNS.state = SM_SOFT_ERROR;
                }
                break;
            }

            DDnsTimer = currTick;

            // Make certain the socket can be written to
            if(TCPIP_TCP_PutIsReady(MySocket) < 125u)//125 = size of TCP Tx buffer
            {
                break;
            }

            // Transmit the request to the server
            (void)TCPIP_TCP_StringPut(MySocket, FC_CStr2CUPtr("GET / HTTP/1.0\r\nHost: "));

            if(DDNSClient.ROMPointers.CheckIPServer != 0U)
            {
                (void)TCPIP_TCP_StringPut(MySocket, DDNSClient.CheckIPServer.szROM);
            }
            else
            {
                (void)TCPIP_TCP_StringPut(MySocket, DDNSClient.CheckIPServer.szRAM);
            }

            (void)TCPIP_TCP_StringPut(MySocket, FC_CStr2CUPtr("\r\nConnection: close\r\n\r\n"));

            // Send the packet
            (void)TCPIP_TCP_Flush(MySocket);
            smDDNS.val++;
            break;

        case SM_CHECKIP_FIND_DELIMITER:

            // Check if remote node is still connected.  If not, force to the disconnect state,
            // but don't break because data may still be waiting.
            if(!TCPIP_TCP_IsConnected(MySocket) || currTick - DDnsTimer > 6U * tickFreq)
            {
                smDDNS.state = SM_CHECKIP_DISCONNECT;
            }

            // Search out the "Address: " delimiter in the response
            wPos = TCPIP_TCP_ArrayFind(MySocket, FC_CStr2CUPtr("Address: "), 9, 0, 0, false);

            // If not yet found, clear as much as possible and break
            if(wPos == 0xffffU)
            {
                wPos = TCPIP_TCP_GetIsReady(MySocket);
                if(wPos > 9u)
                {
                    (void)TCPIP_TCP_ArrayGet(MySocket, NULL, wPos - 9U);
                }
                break;
            }

            // Clear up to and past that string
            (void)TCPIP_TCP_ArrayGet(MySocket, NULL, wPos + 9U);

            // Continue on to read the IP
            DDnsTimer = currTick;
            smDDNS.val++;
            break;

        case SM_CHECKIP_FIND_ADDRESS:

            // Check if remote node is still connected.  If not, force to the disconnect state,
            // but don't break because data may still be waiting.
            if(!TCPIP_TCP_IsConnected(MySocket) || currTick - DDnsTimer > 6U * tickFreq)
            {
                smDDNS.state = SM_CHECKIP_DISCONNECT;
            }

            // Search out the "</body>" delimiter in the response
            wPos = TCPIP_TCP_ArrayFind(MySocket, FC_CStr2CUPtr("</body>"), 7, 0, 0, false);

            // If not yet found, break
            if(wPos == 0xffffU)
            {
                break;
            }

            // Read and terminate that string as the IP address (preventing buffer overflows)
            if(wPos > 15U)
            {
                wPos = 15U;
            }
            (void)TCPIP_TCP_ArrayGet(MySocket, vBufferPtr.uPtr, wPos);
            vBufferPtr.cPtr[wPos] = '\0';

            // Parse the IP address that was read, invalidating on failure
            if(!TCPIP_Helper_StringToIPAddress(vBufferPtr.cPtr, &ipParsed))
            {
                vBufferPtr.cPtr[0] = '\0';
            }

            // Continue on to close the socket
            smDDNS.val++;
            break;

        case SM_CHECKIP_DISCONNECT:

            // Close the socket
            (void)TCPIP_TCP_Close(MySocket);
            MySocket = INVALID_SOCKET;

            // Determine if an update is necessary
            if(vBufferPtr.cPtr[0] == '\0')
            {// CheckIP Failed
                lastStatus = DDNS_STATUS_CHECKIP_ERROR;
                smDDNS.state = SM_SOFT_ERROR;
                break;
            }

            if( (ipParsed.Val ==lastKnownIP.Val) && (!bForceUpdate))
            {
                // IP address has not changed and no update is forced
                lastStatus = DDNS_STATUS_UNCHANGED;
                smDDNS.state = SM_DONE;
                break;
            }

            // Need to perform an update
            lastKnownIP = ipParsed;
            bForceUpdate = false;
            smDDNS.val++;
            break;

        case SM_IP_UPDATE_HOME:

            // resolve the remote update server
            if(DDNSClient.ROMPointers.UpdateServer != 0U)
            {
                (void)TCPIP_DNS_Resolve(DDNSClient.UpdateServer.czROM, TCPIP_DNS_TYPE_A);
            }
            else
            {
                (void)TCPIP_DNS_Resolve(DDNSClient.UpdateServer.czRAM, TCPIP_DNS_TYPE_A);
            }

            smDDNS.val++;
            break;

        case SM_IP_UPDATE_WAIT_DNS:

            if(DDNSClient.ROMPointers.UpdateServer != 0U)
            {
                dnsRes = TCPIP_DNS_IsNameResolved(DDNSClient.UpdateServer.czROM, &ddnsUpdateIP, NULL);
            }
            else
            {
                dnsRes = TCPIP_DNS_IsNameResolved(DDNSClient.UpdateServer.czRAM, &ddnsUpdateIP, NULL);
            }

            if(dnsRes == TCPIP_DNS_RES_PENDING)
            {   // ongoing operation;
                break;
            }

            // update server IP solved
            // open the client socket to the update server
            MySocket = TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE_IPV4, DDNSClient.UpdatePort, FC_Ip4Add2Multi(&ddnsUpdateIP));

            // If no socket available, try again later
            if(MySocket == INVALID_SOCKET)
            {
                lastStatus = DDNS_STATUS_SKT_ERROR;
                smDDNS.state = SM_SKT_ERROR;
                break;
            }
            (void)TCPIP_TCP_SignalHandlerRegister(MySocket, TCPIP_TCP_SIGNAL_RX_DATA, &DDNSSocketRxSignalHandler, NULL);

            // socket opened OK
            // Move on to the next state
            smDDNS.val++;
            DDnsTimer = currTick;
            break;

        case SM_IP_UPDATE_SKT_OBTAINED:

            // Wait for the remote server to accept our connection request
            if(!TCPIP_TCP_IsConnected(MySocket))
            {
                // Time out if too much time is spent in this state
                if(currTick - DDnsTimer > 6U * tickFreq)
                {
                    // Close the socket so it can be used by other modules
                    // We will try again immediately
                    (void)TCPIP_TCP_Close(MySocket);
                    MySocket = INVALID_SOCKET;
                    lastStatus = DDNS_STATUS_UPDATE_ERROR;
                    smDDNS.val--;
                }
                break;
            }


            // Reset timer and begin sending the request
            DDnsTimer = currTick;
            smDDNS.val++;
            break;

        case SM_IP_UPDATE_REQ_A:

            // Check for lost connections or timeouts
            if(!TCPIP_TCP_IsConnected(MySocket) || (currTick - DDnsTimer > 10U * tickFreq))
            {
                lastStatus = DDNS_STATUS_UPDATE_ERROR;
                smDDNS.state = SM_IPUDATE_DISCONNECT;
                break;
            }

            if(TCPIP_TCP_PutIsReady(MySocket) < 25u)  // 25 =~ 16+9
            {
                break;
            }

            (void)TCPIP_TCP_StringPut(MySocket, FC_CStr2CUPtr("GET /nic/update?hostname="));
            smDDNS.val++;
            break;

        case SM_IP_UPDATE_REQ_B:

            // Check for lost connections or timeouts
            if(!TCPIP_TCP_IsConnected(MySocket) || (currTick - DDnsTimer > 10U * tickFreq))
            {
                lastStatus = DDNS_STATUS_UPDATE_ERROR;
                smDDNS.state = SM_IPUDATE_DISCONNECT;
                break;
            }

            // Try to write, verifying that space is available first
            if(DDNSClient.ROMPointers.Host != 0U)
            {
                if(TCPIP_TCP_PutIsReady(MySocket) < strlen(DDNSClient.Host.czROM))
                {
                    break;
                }
                (void)TCPIP_TCP_StringPut(MySocket,DDNSClient.Host.szROM);
            }
            else
            {
                if(TCPIP_TCP_PutIsReady(MySocket) < strlen(DDNSClient.Host.czRAM))
                {
                    break;
                }
                (void)TCPIP_TCP_StringPut(MySocket, DDNSClient.Host.szRAM);
            }

            smDDNS.val++;
            break;

        case SM_IP_UPDATE_REQ_C:

            // Check for lost connections or timeouts
            if(!TCPIP_TCP_IsConnected(MySocket) || currTick - DDnsTimer > 10U * tickFreq)
            {
                lastStatus = DDNS_STATUS_UPDATE_ERROR;
                smDDNS.state = SM_IPUDATE_DISCONNECT;
                break;
            }

            if(TCPIP_TCP_PutIsReady(MySocket) < 70u)
            {
                break;
            }

            (void)TCPIP_TCP_StringPut(MySocket, FC_CStr2CUPtr("&myip="));
            (void)TCPIP_TCP_StringPut(MySocket, vBufferPtr.uPtr);
            (void)TCPIP_TCP_StringPut(MySocket, FC_CStr2CUPtr("&wildcard=NOCHG&mx=NOCHG&backmx=NOCHG HTTP/1.0"));

            (void)TCPIP_TCP_Flush(MySocket);
            smDDNS.val++;
            break;

        case SM_IP_UPDATE_REQ_D:

            // Check for lost connections or timeouts
            if(!TCPIP_TCP_IsConnected(MySocket) || currTick - DDnsTimer > 10U * tickFreq)
            {
                lastStatus = DDNS_STATUS_UPDATE_ERROR;
                smDDNS.state = SM_IPUDATE_DISCONNECT;
                break;
            }

            if(TCPIP_TCP_PutIsReady(MySocket) < 131u) // 131 =~ 8+23 + dynamic dns server hostname
            {
                break;
            }

            (void)TCPIP_TCP_StringPut(MySocket, FC_CStr2CUPtr("\r\nHost: "));//8

            if(DDNSClient.ROMPointers.UpdateServer != 0U)
            {
                (void)TCPIP_TCP_StringPut(MySocket,DDNSClient.UpdateServer.szROM);
            }
            else
            {
                (void)TCPIP_TCP_StringPut(MySocket, DDNSClient.UpdateServer.szRAM);
            }

            (void)TCPIP_TCP_StringPut(MySocket, FC_CStr2CUPtr("\r\nAuthorization: Basic "));//23

            (void)TCPIP_TCP_Flush(MySocket);
            smDDNS.val++;
            break;

        case SM_IP_UPDATE_REQ_E:

            // Check for lost connections or timeouts
            if(!TCPIP_TCP_IsConnected(MySocket) || currTick - DDnsTimer > 6U * tickFreq)
            {
                lastStatus = DDNS_STATUS_UPDATE_ERROR;
                smDDNS.state = SM_IPUDATE_DISCONNECT;
                break;
            }

            // User name and passwords for DynDNS.org can each be up to 24 characters
            // Base64 encoded data is always at least 25% bigger than the original
            if(TCPIP_TCP_PutIsReady(MySocket) < 100u)
            {
                break;
            }

            if(DDNSClient.ROMPointers.Username != 0U)
            {
                ROMStrPtr = DDNSClient.Username.czROM;
                wPos = (uint16_t)strlen(ROMStrPtr);
            }
            else
            {
                RAMStrPtr = DDNSClient.Username.czRAM;
                wPos = (uint16_t)strlen(RAMStrPtr);
            }

            i = 0;
            while(wPos != 0U)
            {
                while(i < wPos && i < 3u)
                {
                    if(DDNSClient.ROMPointers.Username != 0U)
                    {
                        vBufferPtr.cPtr[i] = *ROMStrPtr++;
                    }
                    else
                    {
                        vBufferPtr.cPtr[i] = *RAMStrPtr++;
                    }
                    i++;
                }
                wPos -= i;

                if(i == 3u)
                {
                    (void)TCPIP_Helper_Base64Encode(vBufferPtr.uPtr, i, vBufferPtr.uPtr, 4);
                    (void)TCPIP_TCP_ArrayPut(MySocket, vBufferPtr.uPtr, 4);
                    i = 0;
                }
            }

            if(DDNSClient.ROMPointers.Password != 0U)
            {
                ROMStrPtr = DDNSClient.Password.czROM;
                wPos = (uint16_t)strlen(ROMStrPtr);
            }
            else
            {
                RAMStrPtr = DDNSClient.Password.czRAM;
                wPos = (uint16_t)strlen(RAMStrPtr);
            }

            // Increment for the ':' separator and i for bytes left in username
            wPos += (uint16_t)i + 1U;

            vBufferPtr.cPtr[i++] = ':';

            while(wPos != 0U)
            {
                while(i < wPos && i < 3u)
                {
                    if(DDNSClient.ROMPointers.Password != 0U)
                    {
                        vBufferPtr.cPtr[i] = *ROMStrPtr++;
                    }
                    else
                    {
                        vBufferPtr.cPtr[i] = *RAMStrPtr++;
                    }
                    i++;
                }
                wPos -= i;
                (void)TCPIP_Helper_Base64Encode(vBufferPtr.uPtr, i, vBufferPtr.uPtr, 4);
                (void)TCPIP_TCP_ArrayPut(MySocket, vBufferPtr.uPtr, 4);
                i = 0;
            }

            (void)TCPIP_TCP_Flush(MySocket);
            smDDNS.val++;
            break;


        case SM_IP_UPDATE_REQ_F:

            // Check for lost connections or timeouts
            if(!TCPIP_TCP_IsConnected(MySocket) || currTick - DDnsTimer > 10U * tickFreq)
            {
                lastStatus = DDNS_STATUS_UPDATE_ERROR;
                smDDNS.state = SM_IPUDATE_DISCONNECT;
                break;
            }

            if(TCPIP_TCP_PutIsReady(MySocket) < 50u)
            {
                break;
            }

            (void)TCPIP_TCP_StringPut(MySocket, FC_CStr2CUPtr("\r\nUser-Agent: Microchip - TCPIPSTACK - "TCPIP_STACK_VERSION_STR"\r\n\r\n"));
            (void)TCPIP_TCP_Flush(MySocket);
            smDDNS.val++;

            // Reset the timer to wait for a response
            DDnsTimer = currTick;
            break;

        case SM_IPUPDATE_FIND_RESPONSE:
            // Locate the response string

            // Wait up to 10 seconds for a response
            if(currTick - DDnsTimer > 10U * tickFreq)
            {
                lastStatus = DDNS_STATUS_UPDATE_ERROR;
                smDDNS.state = SM_IPUDATE_DISCONNECT;
                break;
            }

            // According to HTTP, the response will start after the two CRLFs
            wPos = TCPIP_TCP_ArrayFind(MySocket, FC_CStr2CUPtr("\r\n\r\n"), 4, 0, 0, false);

            // If not yet found, eliminate everything up to
            if(wPos == 0xffffU)
            {
                wPos = TCPIP_TCP_GetIsReady(MySocket);
                if(wPos > 4U)
                {
                    (void)TCPIP_TCP_ArrayGet(MySocket, NULL, wPos - 4U);
                }
                break;
            }

            (void)TCPIP_TCP_ArrayGet(MySocket, NULL, wPos + 4U);
            smDDNS.val++;
            break;

        case SM_IPUPDATE_PARSE_RESPONSE:
            // Try to parse the response text

            // Wait up to 10 seconds for the remote server to disconnect
            // so we know all data has been received
            if(TCPIP_TCP_IsConnected(MySocket) && currTick - DDnsTimer < 10U * tickFreq)
            {
                break;
            }

            // Read the response code
            wPos = TCPIP_TCP_GetIsReady(MySocket);
            if(wPos > sizeof(vBuffer) - 1U)
            {
                wPos = sizeof(vBuffer) - 1U;
            }

            wPos = TCPIP_TCP_ArrayGet(MySocket, vBufferPtr.uPtr, wPos);
            vBufferPtr.cPtr[wPos] = '\0';
            for(i = 0; i < sizeof(vBuffer); i++)
            {
                if(vBufferPtr.cPtr[i] == ' ')
                {
                    vBufferPtr.cPtr[i] = '\0';
                }
            }

            for(i = (uint8_t)DDNS_STATUS_GOOD; i < (uint8_t)DDNS_STATUS_UPDATE_ERROR; i++)
            {
                if(strcmp(vBufferPtr.cPtr, (const char*)updateIpSrvrResponse[i]) == 0)
                {
                    break;
                }
            }
            lastStatus = (DDNS_STATUS)i;

            smDDNS.val++;
            break;

        case SM_IPUDATE_DISCONNECT:
            // Close the socket so it can be used by other modules.
            if(MySocket != INVALID_SOCKET)
            {
                (void)TCPIP_TCP_Close(MySocket);
                MySocket = INVALID_SOCKET;
            }

            // Determine what to do based on status
            if(lastStatus <= DDNS_STATUS_NUMHOST || lastStatus == DDNS_STATUS_UNCHANGED)
            {
                smDDNS.state = SM_DONE;
            }
            else if(lastStatus == DDNS_STATUS_911 || lastStatus == DDNS_STATUS_DNSERR)
            {
                smDDNS.state = SM_SYSTEM_ERROR;
            }
            else
            {
                smDDNS.state = SM_SOFT_ERROR;
            }

            smDDNS.val++;
            break;

        case SM_DONE:
            dwUpdateAt = currTick + 10U * 60U * tickFreq;   // 10 minutes
            smDDNS.state = SM_IDLE;
            break;

        case SM_SOFT_ERROR:
        case SM_DNS_ERROR:
        case SM_SKT_ERROR:
            dwUpdateAt = currTick + 30U * tickFreq;      // 30 seconds
            smDDNS.state = SM_IDLE;
            break;

        case SM_SYSTEM_ERROR:
        default:
            dwUpdateAt = currTick + 30U * 60U * tickFreq;       // 30 minutes
            smDDNS.state = SM_IDLE;
            break;
    }
}


// ddns.h
void TCPIP_DDNS_UpdateForce(void)
{
    // Force update on next DDNSClient call
    dwUpdateAt = SYS_TMR_TickCountGet();
    bForceUpdate = true;
    lastStatus = DDNS_STATUS_UNKNOWN;
}


// ddns.h
void TCPIP_DDNS_ServiceSet(DDNS_SERVICES svc)
{
    DDNSClient.ROMPointers.UpdateServer = 1U;
    DDNSClient.UpdateServer.czROM = ddnsServiceHosts[svc];
    DDNSClient.UpdatePort = ddnsServicePorts[svc];
}


// ddns.h
uint32_t TCPIP_DDNS_LastIPGet(void)
{
    return lastKnownIP.Val;
}


// ddns.h
DDNS_STATUS TCPIP_DDNS_LastStatusGet(void)
{
    return lastStatus;
}

#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)

