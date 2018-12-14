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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_NBNS

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_NBNS)


// NBNS Header structure
typedef struct __attribute__((packed)) _NBNS_HEADER
{
	TCPIP_UINT16_VAL TransactionID;
	TCPIP_UINT16_VAL Flags;
	TCPIP_UINT16_VAL Questions;
	TCPIP_UINT16_VAL Answers;
	TCPIP_UINT16_VAL AuthoritativeRecords;
	TCPIP_UINT16_VAL AdditionalRecords;
} NBNS_HEADER;

typedef struct __attribute__((packed)) _NBNS_QUESTION
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

static tcpipSignalHandle   nbnsSignalHandle = 0;

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
bool TCPIP_NBNS_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const TCPIP_NBNS_MODULE_CONFIG* pNbnsInit)
{
    TCPIP_UDP_SIGNAL_HANDLE sigHandle;
    bool    initFail = false;

    if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_UP)
    {  // interface restart
        return true;
    }
    
    // stack init
    while(nbnsInitCount == 0)
    {   // first time we run
        UDP_SOCKET s;

        nbnsDcpt.uSkt = INVALID_UDP_SOCKET;

        s = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, TCPIP_NBNS_SERVER_PORT, 0);
        if ( s == INVALID_UDP_SOCKET)
        {
            initFail = true;
            break;
        }
        
        sigHandle = TCPIP_UDP_SignalHandlerRegister(s, TCPIP_UDP_SIGNAL_RX_DATA, NBNSSocketRxSignalHandler, 0);
        if(!sigHandle)
        {
            initFail = true;
            break;
        }

        if (!TCPIP_UDP_RemoteBind(s, IP_ADDRESS_TYPE_IPV4, TCPIP_NBNS_SERVER_PORT, 0))
        {
            initFail = true;
            break;
        }

        nbnsDcpt.uSkt = s;

        // create the NBNS timer
        nbnsSignalHandle =_TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, TCPIP_NBNS_Task, TCPIP_NBNS_TASK_TICK_RATE); 
        if(nbnsSignalHandle == 0)
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
        if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT)
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
        TCPIP_UDP_Close(nbnsDcpt.uSkt);
        nbnsDcpt.uSkt = INVALID_UDP_SOCKET;
    }

    if(nbnsSignalHandle)
    {
        _TCPIPStackSignalHandlerDeregister(nbnsSignalHandle);
        nbnsSignalHandle = 0;
    }

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_NBNS_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = _TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(sigPend != 0)
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
        _TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}


static void TCPIP_NBNS_Process(void)
{
    NBNS_HEADER NBNSHeader;
    uint8_t NameString[16];
    UDP_SOCKET s;
    int nBytes;
    int nbnsTxSize;
    UDP_SOCKET_INFO sktInfo;

    s = nbnsDcpt.uSkt;

    while(true)
    {
        nBytes = TCPIP_UDP_GetIsReady(s);
        if (nBytes == 0)
        {
            break;
        }

        // Respond only to name requests sent to us from nodes on the same subnet
        // This prevents us from sending out the wrong IP address information if 
        // we haven't gotten a DHCP lease yet.
        TCPIP_UDP_SocketInfoGet(s, &sktInfo);
        if (sktInfo.addressType != IP_ADDRESS_TYPE_IPV4 ||
                _TCPIPStackIpAddFromAnyNet(0, &sktInfo.remoteIPaddress.v4Add) == 0)
        {
            TCPIP_UDP_Discard(s);
            continue;
        }

        // Retrieve the NBNS header and de-big-endian it
        nBytes = TCPIP_UDP_ArrayGet(s, (uint8_t*) & NBNSHeader, sizeof (NBNS_HEADER));
        if(nBytes < sizeof(NBNS_HEADER))
        {
            TCPIP_UDP_Discard(s);
            continue;
        }
        //NBNSHeader.TransactionID.Val = NBNSHeader.TransactionID.v[0] << 8 | NBNSHeader.TransactionID.v[1];
        NBNSHeader.Questions.Val = NBNSHeader.Questions.v[0] << 8 | NBNSHeader.Questions.v[1];
        // Remove all questions
        while (NBNSHeader.Questions.Val--)
        {
            NBNS_QUESTION question;
            TCPIP_NBNS_NameGet(s, NameString);

            nBytes = TCPIP_UDP_ArrayGet(s, (uint8_t*) & question, sizeof (NBNS_QUESTION));
            if(nBytes < sizeof(NBNS_QUESTION))
            {
                break;
            }

            question.Class.Val = question.Class.v[0] << 8 | question.Class.v[1];
            question.Type.Val = question.Type.v[0] << 8 | question.Type.v[1];
            if (question.Type.Val == 0x0020u && question.Class.Val == 0x0001u)
            {
                int nIfs, nIx;
                TCPIP_NET_IF* pIf;
                const char* netbName;

                nIfs = TCPIP_STACK_NumberOfNetworksGet();
                for (nIx = 0; nIx < nIfs; nIx++)
                {
                    pIf = _TCPIPStackHandleToNetUp(TCPIP_STACK_IndexToNet(nIx));
                    if(pIf == 0)
                    {   // interface down
                        continue;
                    }

                    netbName = TCPIP_STACK_NetBIOSName(pIf);

                    if (netbName != 0 && memcmp((void*) NameString, netbName, sizeof (pIf->NetBIOSName)) == 0)
                    { // one of our interfaces has this name
                        nbnsTxSize = TCPIP_UDP_TxPutIsReady(s, 64);
                        if (nbnsTxSize)
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
                            uint16_t* idPtr = (uint16_t*)nbnsMessage;
                            *idPtr = NBNSHeader.TransactionID.Val;

                            const uint8_t * pNetName = (uint8_t*) netbName;
                            uint8_t i;
                            for (i = 13; i < 13 + 32; i += 2)
                            {
                                uint8_t j = *pNetName++;

                                nbnsMessage[i] = (j >> 4) + 'A';
                                nbnsMessage[i + 1] = (j & 0x0F) + 'A';

                            }

                            TCPIP_UDP_ArrayPut(s, nbnsMessage, sizeof (nbnsMessage));

                            // Change the destination address to the unicast address of the last received packet
                            TCPIP_UDP_SocketInfoGet(s, &sktInfo);
                            TCPIP_UDP_DestinationIPAddressSet(s, sktInfo.addressType, &sktInfo.sourceIPaddress);
                            TCPIP_UDP_Flush(s);
                        }
                        break;
                    }
                }
            }
        }

        TCPIP_UDP_Discard(s);
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

    uint8_t encodedString[33]; // NetBIOS strings are 16 characters long, encoded 32 bytes, 1 byte for length
    TCPIP_UDP_ArrayGet(s, encodedString, sizeof (encodedString));

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
        for (i = 1; i < 33; i+=2)
        {
            j = encodedString[i];
            j -= 'A';
            k = j << 4;
            j = encodedString[i+1];
            j -= 'A';
            *String++ = k | j;
        }
    }
}


#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_NBNS)

