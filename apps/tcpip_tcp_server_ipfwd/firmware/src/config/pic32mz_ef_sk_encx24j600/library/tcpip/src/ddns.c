/*******************************************************************************
  Dynamic DNS Client Module

  Summary:
    Reference: DNS Update API Version 2.0.3 (www.dyndns.com)
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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_DYNDNS_CLIENT

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)


// Response codes from DynDNS Update Server
static const char* _updateIpSrvrResponse[] =
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

static TCPIP_DDNS_STATE smDDNS = SM_IDLE;

static IPV4_ADDR lastKnownIP;       // Last known IP address of this device
static DDNS_STATUS lastStatus;  // Status response from last update

DDNS_POINTERS DDNSClient;       // Configuration parameters for the module

static uint32_t dwUpdateAt = 0; // Indicates when the next CheckIP should be done
static bool bForceUpdate;       // Indicates that the update should be done regardless
                                // of whether or not the IP changed.  Use this flag
                                // when the user/pass/hostname have changed.

// Indicates how many interfaces are up.
static int ddnsInitCount = 0;
static tcpipSignalHandle   ddnsSignalHandle = 0;


static uint32_t     DDnsTimer;
static TCP_SOCKET   MySocket = INVALID_SOCKET;
static char const *     ROMStrPtr;
static char *       RAMStrPtr;
static uint8_t vBuffer[16];
static IPV4_ADDR ipParsed;
// the server address
static IPV4_ADDR  ddnsServerIP;
static IPV4_ADDR  ddnsUpdateIP;


static void TCPIP_DDNS_Process(void);

static void _DDNSSocketRxSignalHandler(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, TCPIP_TCP_SIGNAL_TYPE sigType, const void* param);


// ddns_manager.h
bool TCPIP_DDNS_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackData,
              const DDNS_MODULE_CONFIG* ddnsData)
{
    if (stackData->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    if (!ddnsInitCount)
    {
        // create the NBNS timer
        ddnsSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_DDNS_Task, TCPIP_DDNS_TASK_TICK_RATE);
        if(ddnsSignalHandle == 0)
        {   // cannot create the NBNS timer
            return false;
        }

        // Clear the Dynamic DNS Client to start
        memset((void*)&DDNSClient, 0x00, sizeof(DDNSClient));

        // Use the default Check IP server
        DDNSClient.ROMPointers.CheckIPServer = 1;
        DDNSClient.CheckIPServer.szROM = (const uint8_t*)TCPIP_DDNS_CHECKIP_SERVER;
        DDNSClient.CheckIPPort = TCPIP_DDNS_SERVER_REMOTE_PORT;

        dwUpdateAt = 0;
        bForceUpdate = true;
        lastStatus = DDNS_STATUS_UNKNOWN;
        smDDNS = SM_IDLE;
    }

    ddnsInitCount++;

    return true;
}


// ddns_manager.h
#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_DDNS_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData)
{
    if (ddnsInitCount)
    {
        if (stackData->stackAction == TCPIP_STACK_ACTION_DEINIT)
        {
            if (!--ddnsInitCount)
            {
                // All interfaces are down, clean up.
                memset((void*)&DDNSClient, 0x00, sizeof(DDNSClient));
                bForceUpdate = false;
                lastStatus = DDNS_STATUS_UNKNOWN;
                if(ddnsSignalHandle)
                {
                    _TCPIPStackSignalHandlerDeregister(ddnsSignalHandle);
                    ddnsSignalHandle = 0;
                }
            }
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_DDNS_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != 0)
    { // TMO or RX signals occurred
        TCPIP_DDNS_Process();
    }

}


// send a signal to the DDNS module that data is available
// no manager alert needed since this normally results as a higher layer (TCP) signal
static void _DDNSSocketRxSignalHandler(TCP_SOCKET hTCP, TCPIP_NET_HANDLE hNet, TCPIP_TCP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_TCP_SIGNAL_RX_DATA)
    {
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}



static void TCPIP_DDNS_Process(void)
{
    uint8_t                 i;
    uint16_t wPos;
    TCPIP_DNS_RESULT      dnsRes;

    switch(smDDNS)
    {
        case SM_IDLE:

            if(dwUpdateAt == 0)
            {   // First update is 15 seconds after boot, allowing DHCP to stabilize
                dwUpdateAt = SYS_TMR_TickCountGet() + 15 * SYS_TMR_TickCounterFrequencyGet();
                break;
            }

            // Wait for timeout to begin IP check
            if((int32_t)(SYS_TMR_TickCountGet() - dwUpdateAt) < 0)
            {
                break;
            }

            // Otherwise, continue to next state
            smDDNS = SM_BEGIN_CHECKIP;

        case SM_BEGIN_CHECKIP:

            // If a fatal error has occurred, abort to the SM_DONE state and keep
            // the error message.
            if(lastStatus >= DDNS_STATUS_ABUSE && lastStatus <= DDNS_STATUS_911)
            {
                smDDNS = SM_DONE;
                break;
            }

            // If DDNSClient is not properly configured, abort
            if(
                // Verify that each pointer is not null, and is not empty
                (DDNSClient.ROMPointers.Host && (!DDNSClient.Host.szROM || *DDNSClient.Host.szROM == '\0') ) ||
                (!DDNSClient.ROMPointers.Host && (!DDNSClient.Host.szRAM || *DDNSClient.Host.szRAM == '\0') ) ||
                (DDNSClient.ROMPointers.Username && (!DDNSClient.Username.szROM || *DDNSClient.Username.szROM == '\0') ) ||
                (!DDNSClient.ROMPointers.Username && (!DDNSClient.Username.szRAM || *DDNSClient.Username.szRAM == '\0') ) ||
                (DDNSClient.ROMPointers.Password && (!DDNSClient.Password.szROM || *DDNSClient.Password.szROM == '\0') ) ||
                (!DDNSClient.ROMPointers.Password && (!DDNSClient.Password.szRAM || *DDNSClient.Password.szRAM == '\0') ) ||
                (DDNSClient.ROMPointers.CheckIPServer && (!DDNSClient.CheckIPServer.szROM || *DDNSClient.CheckIPServer.szROM == '\0') ) ||
                (!DDNSClient.ROMPointers.CheckIPServer && (!DDNSClient.CheckIPServer.szRAM || *DDNSClient.CheckIPServer.szRAM == '\0') ) ||
                (DDNSClient.ROMPointers.UpdateServer && (!DDNSClient.UpdateServer.szROM || *DDNSClient.UpdateServer.szROM == '\0') ) ||
                (!DDNSClient.ROMPointers.UpdateServer && (!DDNSClient.UpdateServer.szRAM || *DDNSClient.UpdateServer.szRAM == '\0') )
            )
            {
                smDDNS = SM_SOFT_ERROR;
                lastStatus = DDNS_STATUS_INVALID;
                break;
            }

            // Start with an invalidated IP String
            vBuffer[0] = '\0';

            smDDNS++;
            break;

        case SM_DNS_START_RESOLVE:

            // resolve the remote server
            if(DDNSClient.ROMPointers.CheckIPServer)
            {
                TCPIP_DNS_Resolve((const char*)DDNSClient.CheckIPServer.szROM, TCPIP_DNS_TYPE_A);
            }
            else
            {
                TCPIP_DNS_Resolve((const char*)DDNSClient.CheckIPServer.szRAM, TCPIP_DNS_TYPE_A);
            }

            smDDNS++;
            break;

        case SM_DNS_WAIT_RESOLVE:

            if(DDNSClient.ROMPointers.CheckIPServer)
            {
                dnsRes = TCPIP_DNS_IsNameResolved((const char*)DDNSClient.CheckIPServer.szROM, &ddnsServerIP, 0);
            }
            else
            {
                dnsRes = TCPIP_DNS_IsNameResolved((const char*)DDNSClient.CheckIPServer.szRAM, &ddnsServerIP, 0);
            }

            if(dnsRes == TCPIP_DNS_RES_PENDING)
            {   // ongoing operation;
                break;
            }

            if(dnsRes < 0)
            {   // some DNS error occurred; retry later
                lastStatus = DDNS_STATUS_DNS_ERROR;
                smDDNS = SM_DNS_ERROR;
                break;
            }

            // server IP solved
            // open the client socket
            MySocket = TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE_IPV4, DDNSClient.CheckIPPort, (IP_MULTI_ADDRESS*)&ddnsServerIP);

            // If no socket available, try again later
            if(MySocket == INVALID_SOCKET)
            {
                lastStatus = DDNS_STATUS_SKT_ERROR;
                smDDNS = SM_SKT_ERROR;
                break;
            }
            TCPIP_TCP_SignalHandlerRegister(MySocket, TCPIP_TCP_SIGNAL_RX_DATA, _DDNSSocketRxSignalHandler, 0);

            // socket opened OK
            smDDNS++;
            DDnsTimer = SYS_TMR_TickCountGet();
            break;

        case SM_CHECKIP_SKT_OBTAINED:

            // Wait for the remote server to accept our connection request
            if(!TCPIP_TCP_IsConnected(MySocket))
            {
                // Time out if too much time is spent in this state
                if(SYS_TMR_TickCountGet()-DDnsTimer > 6*SYS_TMR_TickCounterFrequencyGet())
                {
                    // Close the socket so it can be used by other modules
                    // We will retry soon
                    TCPIP_TCP_Close(MySocket);
                    MySocket = INVALID_SOCKET;
                    lastStatus = DDNS_STATUS_CHECKIP_ERROR;
                    smDDNS = SM_SOFT_ERROR;
                }
                break;
            }

            DDnsTimer = SYS_TMR_TickCountGet();

            // Make certain the socket can be written to
            if(TCPIP_TCP_PutIsReady(MySocket) < 125u)//125 = size of TCP Tx buffer
                break;

            // Transmit the request to the server
            TCPIP_TCP_StringPut(MySocket, (const uint8_t*)"GET / HTTP/1.0\r\nHost: ");

            if(DDNSClient.ROMPointers.CheckIPServer)
            {
                TCPIP_TCP_StringPut(MySocket, DDNSClient.CheckIPServer.szROM);
            }
            else
            {
                TCPIP_TCP_StringPut(MySocket, DDNSClient.CheckIPServer.szRAM);
            }

            TCPIP_TCP_StringPut(MySocket, (const uint8_t*)"\r\nConnection: close\r\n\r\n");

            // Send the packet
            TCPIP_TCP_Flush(MySocket);
            smDDNS++;
            break;

        case SM_CHECKIP_FIND_DELIMITER:

            // Check if remote node is still connected.  If not, force to the disconnect state,
            // but don't break because data may still be waiting.
            if(!TCPIP_TCP_IsConnected(MySocket) || SYS_TMR_TickCountGet() - DDnsTimer > 6*SYS_TMR_TickCounterFrequencyGet())
                smDDNS = SM_CHECKIP_DISCONNECT;

            // Search out the "Address: " delimiter in the response
            wPos = TCPIP_TCP_ArrayFind(MySocket, (const uint8_t*)"Address: ", 9, 0, 0, false);

            // If not yet found, clear as much as possible and break
            if(wPos == 0xffff)
            {
                wPos = TCPIP_TCP_GetIsReady(MySocket);
                if(wPos > 9u)
                    TCPIP_TCP_ArrayGet(MySocket, NULL, wPos - 9);
                break;
            }

            // Clear up to and past that string
            TCPIP_TCP_ArrayGet(MySocket, NULL, wPos + 9);

            // Continue on to read the IP
            DDnsTimer = SYS_TMR_TickCountGet();
            smDDNS++;

        case SM_CHECKIP_FIND_ADDRESS:

            // Check if remote node is still connected.  If not, force to the disconnect state,
            // but don't break because data may still be waiting.
            if(!TCPIP_TCP_IsConnected(MySocket) || SYS_TMR_TickCountGet() - DDnsTimer > 6*SYS_TMR_TickCounterFrequencyGet())
                smDDNS = SM_CHECKIP_DISCONNECT;

            // Search out the "</body>" delimiter in the response
            wPos = TCPIP_TCP_ArrayFind(MySocket, (const uint8_t*)"</body>", 7, 0, 0, false);

            // If not yet found, break
            if(wPos == 0xffff)
                break;

            // Read and terminate that string as the IP address (preventing buffer overflows)
            if(wPos > 15u)
                wPos = 15;
            TCPIP_TCP_ArrayGet(MySocket, vBuffer, wPos);
            vBuffer[wPos] = '\0';

            // Parse the IP address that was read, invalidating on failure
            if(!TCPIP_Helper_StringToIPAddress((char*)vBuffer, &ipParsed))
                vBuffer[0] = '\0';

            // Continue on to close the socket
            smDDNS++;

        case SM_CHECKIP_DISCONNECT:

            // Close the socket
            TCPIP_TCP_Close(MySocket);
            MySocket = INVALID_SOCKET;

            // Determine if an update is necessary
            if(vBuffer[0] == '\0')
            {// CheckIP Failed
                lastStatus = DDNS_STATUS_CHECKIP_ERROR;
                smDDNS = SM_SOFT_ERROR;
                break;
            }

            if( (ipParsed.Val ==lastKnownIP.Val) && (!bForceUpdate))
            {
                // IP address has not changed and no update is forced
                lastStatus = DDNS_STATUS_UNCHANGED;
                smDDNS = SM_DONE;
                break;
            }

            // Need to perform an update
            lastKnownIP = ipParsed;
            bForceUpdate = false;
            smDDNS++;
            break;

        case SM_IP_UPDATE_HOME:

            // resolve the remote update server
            if(DDNSClient.ROMPointers.UpdateServer)
            {
                TCPIP_DNS_Resolve((const char*)DDNSClient.UpdateServer.szROM, TCPIP_DNS_TYPE_A);
            }
            else
            {
                TCPIP_DNS_Resolve((const char*)DDNSClient.UpdateServer.szRAM, TCPIP_DNS_TYPE_A);
            }

            smDDNS++;
            break;

        case SM_IP_UPDATE_WAIT_DNS:

            if(DDNSClient.ROMPointers.UpdateServer)
            {
                dnsRes = TCPIP_DNS_IsNameResolved((const char*)DDNSClient.UpdateServer.szROM, &ddnsUpdateIP, 0);
            }
            else
            {
                dnsRes = TCPIP_DNS_IsNameResolved((const char*)DDNSClient.UpdateServer.szRAM, &ddnsUpdateIP, 0);
            }

            if(dnsRes == TCPIP_DNS_RES_PENDING)
            {   // ongoing operation;
                break;
            }

            // update server IP solved
            // open the client socket to the update server
            MySocket = TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE_IPV4, DDNSClient.UpdatePort, (IP_MULTI_ADDRESS*)&ddnsUpdateIP);

            // If no socket available, try again later
            if(MySocket == INVALID_SOCKET)
            {
                lastStatus = DDNS_STATUS_SKT_ERROR;
                smDDNS = SM_SKT_ERROR;
                break;
            }
            TCPIP_TCP_SignalHandlerRegister(MySocket, TCPIP_TCP_SIGNAL_RX_DATA, _DDNSSocketRxSignalHandler, 0);

            // socket opened OK
            // Move on to the next state
            smDDNS++;
            DDnsTimer = SYS_TMR_TickCountGet();
            break;

        case SM_IP_UPDATE_SKT_OBTAINED:

            // Wait for the remote server to accept our connection request
            if(!TCPIP_TCP_IsConnected(MySocket))
            {
                // Time out if too much time is spent in this state
                if(SYS_TMR_TickCountGet() - DDnsTimer > 6*SYS_TMR_TickCounterFrequencyGet())
                {
                    // Close the socket so it can be used by other modules
                    // We will try again immediately
                    TCPIP_TCP_Close(MySocket);
                    MySocket = INVALID_SOCKET;
                    lastStatus = DDNS_STATUS_UPDATE_ERROR;
                    smDDNS--;
                }
                break;
            }


            // Reset timer and begin sending the request
            DDnsTimer = SYS_TMR_TickCountGet();
            smDDNS++;
            // No break needed...try to send first bit immediately.

        case SM_IP_UPDATE_REQ_A:

            // Check for lost connections or timeouts
            if(!TCPIP_TCP_IsConnected(MySocket) || (SYS_TMR_TickCountGet() - DDnsTimer > 10*SYS_TMR_TickCounterFrequencyGet()))
            {
                lastStatus = DDNS_STATUS_UPDATE_ERROR;
                smDDNS = SM_IPUDATE_DISCONNECT;
                break;
            }

            if(TCPIP_TCP_PutIsReady(MySocket) < 25u)  // 25 =~ 16+9
                break;

            TCPIP_TCP_StringPut(MySocket, (const uint8_t*)"GET /nic/update?hostname=");
            smDDNS++;
            // No break needed...try to send next bit immediately.

        case SM_IP_UPDATE_REQ_B:

            // Check for lost connections or timeouts
            if(!TCPIP_TCP_IsConnected(MySocket) || (SYS_TMR_TickCountGet() - DDnsTimer > 10*SYS_TMR_TickCountGet()))
            {
                lastStatus = DDNS_STATUS_UPDATE_ERROR;
                smDDNS = SM_IPUDATE_DISCONNECT;
                break;
            }

            // Try to write, verifying that space is available first
            if(DDNSClient.ROMPointers.Host)
            {
                if(TCPIP_TCP_PutIsReady(MySocket) < strlen((const char*)DDNSClient.Host.szROM))
                    break;
                TCPIP_TCP_StringPut(MySocket,DDNSClient.Host.szROM);
            }
            else
            {
                if(TCPIP_TCP_PutIsReady(MySocket) < strlen((char*)DDNSClient.Host.szRAM))
                    break;
                TCPIP_TCP_StringPut(MySocket,DDNSClient.Host.szRAM);
            }

            smDDNS++;
            // No break needed...try to send next bit immediately.

        case SM_IP_UPDATE_REQ_C:

            // Check for lost connections or timeouts
            if(!TCPIP_TCP_IsConnected(MySocket) || SYS_TMR_TickCountGet() - DDnsTimer > 10*SYS_TMR_TickCounterFrequencyGet())
            {
                lastStatus = DDNS_STATUS_UPDATE_ERROR;
                smDDNS = SM_IPUDATE_DISCONNECT;
                break;
            }

            if(TCPIP_TCP_PutIsReady(MySocket) < 70u)
                break;

            TCPIP_TCP_StringPut(MySocket, (const uint8_t*)"&myip=");
            TCPIP_TCP_StringPut(MySocket, vBuffer);
            TCPIP_TCP_StringPut(MySocket, (const uint8_t*)"&wildcard=NOCHG&mx=NOCHG&backmx=NOCHG HTTP/1.0");

            TCPIP_TCP_Flush(MySocket);
            smDDNS++;
            // No break needed...try to send next bit immediately.

        case SM_IP_UPDATE_REQ_D:

            // Check for lost connections or timeouts
            if(!TCPIP_TCP_IsConnected(MySocket) || SYS_TMR_TickCountGet() - DDnsTimer > 10*SYS_TMR_TickCounterFrequencyGet())
            {
                lastStatus = DDNS_STATUS_UPDATE_ERROR;
                smDDNS = SM_IPUDATE_DISCONNECT;
                break;
            }

            if(TCPIP_TCP_PutIsReady(MySocket) < 131u) // 131 =~ 8+23 + dynamic dns server hostname
                break;

            TCPIP_TCP_StringPut(MySocket, (const uint8_t*)"\r\nHost: ");//8

            if(DDNSClient.ROMPointers.UpdateServer)
                TCPIP_TCP_StringPut(MySocket,DDNSClient.UpdateServer.szROM);
            else
                TCPIP_TCP_StringPut(MySocket,DDNSClient.UpdateServer.szRAM);

            TCPIP_TCP_StringPut(MySocket, (const uint8_t*)"\r\nAuthorization: Basic ");//23

            TCPIP_TCP_Flush(MySocket);
            smDDNS++;
            // No break needed...try to send the next bit immediately.

        case SM_IP_UPDATE_REQ_E:

            // Check for lost connections or timeouts
            if(!TCPIP_TCP_IsConnected(MySocket) || SYS_TMR_TickCountGet() - DDnsTimer > 6*SYS_TMR_TickCounterFrequencyGet())
            {
                lastStatus = DDNS_STATUS_UPDATE_ERROR;
                smDDNS = SM_IPUDATE_DISCONNECT;
                break;
            }

            // User name and passwords for DynDNS.org can each be up to 24 characters
            // Base64 encoded data is always at least 25% bigger than the original
            if(TCPIP_TCP_PutIsReady(MySocket) < 100u)
                break;

            if(DDNSClient.ROMPointers.Username)
            {
                ROMStrPtr = (const char*)DDNSClient.Username.szROM;
                wPos = strlen(ROMStrPtr);
            }
            else
            {
                RAMStrPtr = (char*)DDNSClient.Username.szRAM;
                wPos = strlen((char*)RAMStrPtr);
            }

            i = 0;
            while(wPos)
            {
                while(i < wPos && i < 3u)
                {
                    if(DDNSClient.ROMPointers.Username)
                        vBuffer[i] = *ROMStrPtr++;
                    else
                        vBuffer[i] = *RAMStrPtr++;
                    i++;
                }
                wPos -= i;

                if(i == 3u)
                {
                    TCPIP_Helper_Base64Encode(vBuffer, i, vBuffer, 4);
                    TCPIP_TCP_ArrayPut(MySocket, vBuffer, 4);
                    i = 0;
                }
            }

            if(DDNSClient.ROMPointers.Password)
            {
                ROMStrPtr = (const char*)DDNSClient.Password.szROM;
                wPos = strlen(ROMStrPtr);
            }
            else
            {
                RAMStrPtr = (char*)DDNSClient.Password.szRAM;
                wPos = strlen((char*)RAMStrPtr);
            }

            // Increment for the ':' separator and i for bytes left in username
            wPos += i + 1;

            vBuffer[i++] = ':';

            while(wPos)
            {
                while(i < wPos && i < 3u)
                {
                    if(DDNSClient.ROMPointers.Password)
                        vBuffer[i] = *ROMStrPtr++;
                    else
                        vBuffer[i] = *RAMStrPtr++;
                    i++;
                }
                wPos -= i;
                TCPIP_Helper_Base64Encode(vBuffer, i, vBuffer, 4);
                TCPIP_TCP_ArrayPut(MySocket, vBuffer, 4);
                i = 0;
            }

            TCPIP_TCP_Flush(MySocket);
            smDDNS++;
            break;


        case SM_IP_UPDATE_REQ_F:

            // Check for lost connections or timeouts
            if(!TCPIP_TCP_IsConnected(MySocket) || SYS_TMR_TickCountGet() - DDnsTimer > 10*SYS_TMR_TickCounterFrequencyGet())
            {
                lastStatus = DDNS_STATUS_UPDATE_ERROR;
                smDDNS = SM_IPUDATE_DISCONNECT;
                break;
            }

            if(TCPIP_TCP_PutIsReady(MySocket) < 50u)
                break;

            TCPIP_TCP_StringPut(MySocket, (const uint8_t*)"\r\nUser-Agent: Microchip - TCPIPSTACK - "TCPIP_STACK_VERSION_STR"\r\n\r\n");
            TCPIP_TCP_Flush(MySocket);
            smDDNS++;

            // Reset the timer to wait for a response
            DDnsTimer = SYS_TMR_TickCountGet();
            break;

        case SM_IPUPDATE_FIND_RESPONSE:
            // Locate the response string

            // Wait up to 10 seconds for a response
            if(SYS_TMR_TickCountGet() - DDnsTimer > 10*SYS_TMR_TickCounterFrequencyGet())
            {
                lastStatus = DDNS_STATUS_UPDATE_ERROR;
                smDDNS = SM_IPUDATE_DISCONNECT;
                break;
            }

            // According to HTTP, the response will start after the two CRLFs
            wPos = TCPIP_TCP_ArrayFind(MySocket, (const uint8_t*)"\r\n\r\n", 4, 0, 0, false);

            // If not yet found, eliminate everything up to
            if(wPos == 0xffff)
            {
                wPos = TCPIP_TCP_GetIsReady(MySocket);
                if(wPos > 4u)
                    TCPIP_TCP_ArrayGet(MySocket, NULL, wPos - 4);
                break;
            }

            TCPIP_TCP_ArrayGet(MySocket, NULL, wPos+4);
            smDDNS++;
            // No break...continue to next state immediately

        case SM_IPUPDATE_PARSE_RESPONSE:
            // Try to parse the response text

            // Wait up to 10 seconds for the remote server to disconnect
            // so we know all data has been received
            if(TCPIP_TCP_IsConnected(MySocket) && SYS_TMR_TickCountGet() - DDnsTimer < 10*SYS_TMR_TickCounterFrequencyGet())
                break;

            // Read the response code
            wPos = TCPIP_TCP_GetIsReady(MySocket);
            if(wPos > sizeof(vBuffer) - 1)
                wPos = sizeof(vBuffer) - 1;

            wPos = TCPIP_TCP_ArrayGet(MySocket, vBuffer, wPos);
            vBuffer[wPos] = '\0';
            for(i = 0; i < sizeof(vBuffer); i++)
                if(vBuffer[i] == ' ')
                    vBuffer[i] = '\0';

            for(lastStatus = 0; lastStatus < DDNS_STATUS_UPDATE_ERROR; lastStatus++)
                if(!strcmp((char*)vBuffer, (const char*)_updateIpSrvrResponse[lastStatus]))
                    break;

            smDDNS++;
            // No break...continue to finalization

        case SM_IPUDATE_DISCONNECT:
            // Close the socket so it can be used by other modules.
            if(MySocket != INVALID_SOCKET)
            {
                TCPIP_TCP_Close(MySocket);
                MySocket = INVALID_SOCKET;
            }

            // Determine what to do based on status
            if(lastStatus <= DDNS_STATUS_NUMHOST || lastStatus == DDNS_STATUS_UNCHANGED)
                smDDNS = SM_DONE;
            else if(lastStatus == DDNS_STATUS_911 || lastStatus == DDNS_STATUS_DNSERR)
                smDDNS = SM_SYSTEM_ERROR;
            else
                smDDNS = SM_SOFT_ERROR;

            smDDNS++;
            break;

        case SM_DONE:
            dwUpdateAt = SYS_TMR_TickCountGet() + 10*60*SYS_TMR_TickCounterFrequencyGet();   // 10 minutes
            smDDNS = SM_IDLE;
            break;

        case SM_SOFT_ERROR:
        case SM_DNS_ERROR:
        case SM_SKT_ERROR:
            dwUpdateAt = SYS_TMR_TickCountGet() + 30*SYS_TMR_TickCounterFrequencyGet();      // 30 seconds
            smDDNS = SM_IDLE;
            break;

        case SM_SYSTEM_ERROR:
            dwUpdateAt = SYS_TMR_TickCountGet() + 30*60*SYS_TMR_TickCounterFrequencyGet();       // 30 minutes
            smDDNS = SM_IDLE;
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
    DDNSClient.ROMPointers.UpdateServer = 1;
    DDNSClient.UpdateServer.szROM = (const uint8_t*)ddnsServiceHosts[svc];
    DDNSClient.UpdatePort = ddnsServicePorts[svc];
}


// ddns.h
IPV4_ADDR TCPIP_DDNS_LastIPGet(void)
{
    return lastKnownIP;
}


// ddns.h
DDNS_STATUS TCPIP_DDNS_LastStatusGet(void)
{
    return lastStatus;
}

#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_DYNAMICDNS_CLIENT)

