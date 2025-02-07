/*******************************************************************************
  NetBIOS Name Service (NBNS) Server

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    -Responds to NBNS name requests to allow human name assignment 
     to the board.  i.e. allows nodes on the same IP subnet to use a 
     hostname to access the board instead of an IP address.
    -Reference: RFC 1002
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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_NBNS

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_NBNS)


// NBNS Header structure
typedef struct __attribute__((packed)) S_NBNS_HEADER
{
    TCPIP_UINT16_VAL TransactionID;
    TCPIP_UINT16_VAL Flags;
    TCPIP_UINT16_VAL Questions;
    TCPIP_UINT16_VAL Answers;
    TCPIP_UINT16_VAL AuthoritativeRecords;
    TCPIP_UINT16_VAL AdditionalRecords;
} NBNS_HEADER;

typedef struct __attribute__((packed)) S_NBNS_QUESTION
{
    uint8_t StringTerminator;
        TCPIP_UINT16_VAL Type;
        TCPIP_UINT16_VAL Class;

} NBNS_QUESTION;

static void TCPIP_NBNS_NameGet(UDP_SOCKET s, uint8_t *String);

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void TCPIP_NBNS_Cleanup(void);
#else
#define TCPIP_NBNS_Cleanup()
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void TCPIP_NBNS_Process(void);

static void     NBNSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);

typedef struct
{
    UDP_SOCKET          uSkt;
}TCPIP_NBNS_DCPT;


static TCPIP_NBNS_DCPT    nbnsDcpt;
static int                nbnsInitCount = 0;

static TCPIP_SIGNAL_HANDLE   nbnsSignalHandle = NULL;

/*********************************************************************
 * Function:        void TCPIP_NBNS_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_NBNS_MODULE_CONFIG* pNbnsInit)
 *
 * PreCondition:    None
 *
 * Input:           stackCtrl - Interface and stack module data.
 *                  pNbnsInit - Module-specific information for NBNS.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initializes state machine
 *
 * Note:            None
 ********************************************************************/
bool TCPIP_NBNS_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{
    TCPIP_UDP_SIGNAL_HANDLE sigHandle;
    bool    initFail = false;

    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {  // interface restart
        return true;
    }
    
    // stack init
    while(nbnsInitCount == 0)
    {   // first time we run
        UDP_SOCKET s;

        nbnsDcpt.uSkt = INVALID_UDP_SOCKET;

        s = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, TCPIP_NBNS_SERVER_PORT, NULL);
        if ( s == INVALID_UDP_SOCKET)
        {
            initFail = true;
            break;
        }
        
        sigHandle = TCPIP_UDP_SignalHandlerRegister(s, TCPIP_UDP_SIGNAL_RX_DATA, &NBNSSocketRxSignalHandler, NULL);
        if(sigHandle == NULL)
        {
            initFail = true;
            break;
        }

        if (!TCPIP_UDP_RemoteBind(s, IP_ADDRESS_TYPE_IPV4, TCPIP_NBNS_SERVER_PORT, NULL))
        {
            initFail = true;
            break;
        }

        nbnsDcpt.uSkt = s;

        // create the NBNS timer
        nbnsSignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_NBNS_Task, TCPIP_NBNS_TASK_TICK_RATE); 
        if(nbnsSignalHandle == NULL)
        {   // cannot create the NBNS timer
            initFail = true;
        }

        break;
    }

    if(initFail)
    {
        TCPIP_NBNS_Cleanup();
        return false;
    }

    
    // Reset per interface state machine and flags to default values
    
    nbnsInitCount++;
    return true;
}

/*********************************************************************
 * Function:        void TCPIP_NBNS_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
 *
 * PreCondition:    None
 *
 * Input:           stackCtrl - Interface and stack module data.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        DeInitializes state machine
 *
 * Note:            None
 ********************************************************************/
#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_NBNS_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    if(nbnsInitCount > 0)
    {   // we're up and running
        // one way or another this interface is going down
        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--nbnsInitCount == 0)
            {   // all closed
                // release resources
                if(nbnsDcpt.uSkt != INVALID_UDP_SOCKET)
                {
                    TCPIP_NBNS_Cleanup();
                }
            }
        }
    }

}

static void TCPIP_NBNS_Cleanup(void)
{

    if(nbnsDcpt.uSkt != INVALID_UDP_SOCKET)
    {
        (void)TCPIP_UDP_Close(nbnsDcpt.uSkt);
        nbnsDcpt.uSkt = INVALID_UDP_SOCKET;
    }

    if(nbnsSignalHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(nbnsSignalHandle);
        nbnsSignalHandle = NULL;
    }

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_NBNS_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != TCPIP_MODULE_SIGNAL_NONE)
    { // TMO or RX signal occurred
        TCPIP_NBNS_Process();
    }

}

// send a signal to the NBNS module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void NBNSSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}


static void TCPIP_NBNS_Process(void)
{
    NBNS_HEADER NBNSHeader;
    uint8_t NameString[16];
    UDP_SOCKET s;
    uint16_t nBytes;
    uint16_t nbnsTxSize;
    UDP_SOCKET_INFO sktInfo;
    const uint8_t * pNetName;

    s = nbnsDcpt.uSkt;
    (void)memset(&sktInfo, 0x0, sizeof(sktInfo));

    while(true)
    {
        nBytes = TCPIP_UDP_GetIsReady(s);
        if (nBytes == 0U)
        {
            break;
        }

        // Respond only to name requests sent to us from nodes on the same subnet
        // This prevents us from sending out the wrong IP address information if 
        // we haven't gotten a DHCP lease yet.
        (void)TCPIP_UDP_SocketInfoGet(s, &sktInfo);
        if (sktInfo.addressType != IP_ADDRESS_TYPE_IPV4 ||
                TCPIPStackIpAddFromAnyNet(NULL, &sktInfo.remoteIPaddress.v4Add) == NULL)
        {
            (void)TCPIP_UDP_Discard(s);
            continue;
        }

        // Retrieve the NBNS header and de-big-endian it
        nBytes = TCPIP_UDP_ArrayGet(s, (uint8_t*) & NBNSHeader, (uint16_t)sizeof (NBNS_HEADER));
        if(nBytes < (uint16_t)sizeof(NBNS_HEADER))
        {
            (void)TCPIP_UDP_Discard(s);
            continue;
        }
        // NBNSHeader.TransactionID.Val = TCPIP_Helper_ntohs(NBNSHeader.TransactionID.Val);
        NBNSHeader.Questions.Val = TCPIP_Helper_ntohs(NBNSHeader.Questions.Val);
        // Remove all questions
        while (NBNSHeader.Questions.Val-- != 0U)
        {
            NBNS_QUESTION nbQuestion;
            TCPIP_NBNS_NameGet(s, NameString);

            nBytes = TCPIP_UDP_ArrayGet(s, (uint8_t*) & nbQuestion, (uint16_t)sizeof (NBNS_QUESTION));
            if(nBytes < (uint16_t)sizeof(NBNS_QUESTION))
            {
                break;
            }

            nbQuestion.Class.Val = TCPIP_Helper_ntohs(nbQuestion.Class.Val);
            nbQuestion.Type.Val = TCPIP_Helper_ntohs(nbQuestion.Type.Val);
            if (nbQuestion.Type.Val == 0x0020u && nbQuestion.Class.Val == 0x0001u)
            {
                size_t nIfs, nIx;
                TCPIP_NET_IF* pIf;
                const char* netbName;

                nIfs = TCPIP_STACK_NumberOfNetworksGet();
                for (nIx = 0; nIx < nIfs; nIx++)
                {
                    pIf = TCPIPStackHandleToNetUp(TCPIP_STACK_IndexToNet(nIx));
                    if(pIf == NULL)
                    {   // interface down
                        continue;
                    }

                    netbName = TCPIP_STACK_NetBIOSName(pIf);

                    pNetName = (const uint8_t*) netbName;
                    if (netbName != NULL && memcmp(NameString, pNetName, sizeof (pIf->NetBIOSName)) == 0)
                    { // one of our interfaces has this name
                        nbnsTxSize = TCPIP_UDP_TxPutIsReady(s, 64U);
                        if (nbnsTxSize != 0U)
                        {
                            uint8_t nbnsMessage[] = {
                                0, 0,
                                0x84, //0x8400 Flags
                                0x00,
                                0x00, 0x00, // 0x0000 Questions
                                0x00, 0x01, // 0x0001 Answers
                                0x00, 0x00, // 0x0000 Athoritative records
                                0x00, 0x00, // 0x0000 Additional records

                                32, // NetBIOS names are always 32 bytes long (16 decoded bytes)
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Name
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x00, //String Termination

                                0x00, 0x20, // 0x0020 Type: NetBIOS
                                0x00, 0x01, // 0x0001 Class: Internet
                                0x00, 0x00, 0x00, 0x00, // 0x00000000 Time To Live
                                0x00, 0x06, // 0x0006 Data length
                                0x60, 0x00, // 0x6000 Flags: H-node, Unique

                                pIf->netIPAddr.v[0], // This has to go out as little endian,
                                pIf->netIPAddr.v[1], // This is also not bus aligned so it has to be set this way
                                pIf->netIPAddr.v[2],
                                pIf->netIPAddr.v[3],

                            };
                            // set the transaction ID
                            nbnsMessage[0] = NBNSHeader.TransactionID.v[0];
                            nbnsMessage[1] = NBNSHeader.TransactionID.v[1];

                            uint8_t i;
                            for (i = 13U; i < 13U + 32U; i += 2U)
                            {
                                uint8_t j = *pNetName++;

                                nbnsMessage[i] = (j >> 4U) + (uint8_t)'A';
                                nbnsMessage[i + 1U] = (j & 0x0FU) + (uint8_t)'A';

                            }

                            (void)TCPIP_UDP_ArrayPut(s, nbnsMessage, (uint16_t)sizeof (nbnsMessage));

                            // Change the destination address to the unicast address of the last received packet
                            (void)TCPIP_UDP_SocketInfoGet(s, &sktInfo);
                            (void)TCPIP_UDP_DestinationIPAddressSet(s, sktInfo.addressType, &sktInfo.sourceIPaddress);
                            (void)TCPIP_UDP_Flush(s);
                        }
                        break;
                    }
                }
            }
        }

        (void)TCPIP_UDP_Discard(s);
    }

}

/*********************************************************************
 * Function:        static void TCPIP_NBNS_NameGet (UDP_SOCKET s, uint8_t *String)
 *
 * PreCondition:    None
 *
 * Input:           String: Pointer to an array into which
 *                  a received NetBIOS name should be copied.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Reads the NetBIOS name from a UDP socket and
 *                  copies it into a user-specified buffer.
 *
 * Note:            None
 ********************************************************************/
static void TCPIP_NBNS_NameGet(UDP_SOCKET s, uint8_t *String)
{
    uint8_t i, j, k;

    uint8_t encodedString[33] = {0}; // NetBIOS strings are 16 characters long, encoded 32 bytes, 1 byte for length
    (void)TCPIP_UDP_ArrayGet(s, encodedString, (uint16_t)sizeof (encodedString));

    if (String == NULL)
    {
        return;
    }
    else
    {
        if (encodedString[0] != 32u)
        {
            *String = 0;
            return;
        }
        for (i = 1U; i < 33U; i+=2U)
        {
            j = encodedString[i];
            j -= (uint8_t)'A';
            k = j << 4U;
            j = encodedString[i+1U];
            j -= (uint8_t)'A';
            *String++ = k | j;
        }
    }
}


#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_NBNS)

