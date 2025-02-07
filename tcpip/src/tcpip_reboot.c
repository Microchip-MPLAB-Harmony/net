/*******************************************************************************
  Reboot Module

  Summary:
    Module for Microchip TCP/IP Stack
    
  Description:
    -Remotely resets the PIC
    -Reference: Internet Bootloader documentation
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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_REBOOT_SERVER

#include "tcpip/src/tcpip_private.h"

#if defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_REBOOT_SERVER)

#include "system/reset/sys_reset.h"


static UDP_SOCKET       rebootSocket = INVALID_UDP_SOCKET;
static int              rebootInitCount = 0;
static TCPIP_SIGNAL_HANDLE rebootSignalHandle = NULL;

static void TCPIP_REBOOT_Process(void);
static void RebootSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param);


/*****************************************************************************
  Function:
    bool TCPIP_REBOOT_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData);

  Summary:
    Resets the reboot server module for the specified interface.

  Description:
    Resets the reboot server module for the specified interface.

  Precondition:
    None

  Parameters:
    stackCtrl - pointer to stack structure specifying the interface to initialize

  Returns:
    None

  Remarks:
    This function should be called internally just once per interface 
    by the stack manager.
***************************************************************************/
bool TCPIP_REBOOT_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl, const void* initData)
{
    if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // interface restart
        return true;
    }

    // stack init
    
    if(rebootInitCount == 0)
    {   // first time we're run

        rebootSocket = TCPIP_UDP_ServerOpen(IP_ADDRESS_TYPE_IPV4, TCPIP_REBOOT_SERVER_PORT, NULL);

        if(rebootSocket == INVALID_UDP_SOCKET)
        {   // failed
            return false;
        }

        (void)TCPIP_UDP_SignalHandlerRegister(rebootSocket, TCPIP_UDP_SIGNAL_RX_DATA, &RebootSocketRxSignalHandler, NULL);

        // create the reboot timer
        rebootSignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_REBOOT_Task, 0);
        if(rebootSignalHandle == NULL)
        {   // cannot create the reboot timer
            (void)TCPIP_UDP_Close(rebootSocket);
            rebootSocket = INVALID_UDP_SOCKET;
            return false;
        }

    }
            
    rebootInitCount++;
    return true;
}

/*****************************************************************************
  Function:
    bool TCPIP_REBOOT_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl);

  Summary:
    Turns off the reboot server module for the specified interface.

  Description:
    Closes the reboot server.

  Precondition:
    None

  Parameters:
    stackData - pointer to stack structure specifying the interface to deinitialize

  Returns:
    None

  Remarks:
    This function should be called internally just once per interface 
    by the stack manager.
***************************************************************************/
#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_REBOOT_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{

    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    if(rebootInitCount > 0)
    {   // we're up and running
        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--rebootInitCount == 0)
            {   // all closed
                // release resources
                (void)TCPIP_UDP_Close(rebootSocket);
                rebootSocket = INVALID_UDP_SOCKET;
                if(rebootSignalHandle != NULL)
                {
                    TCPIPStackSignalHandlerDeregister(rebootSignalHandle);
                    rebootSignalHandle = NULL;
                }
            }
        }
    }
}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

void TCPIP_REBOOT_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_RX_PENDING) != 0U)
    { //  RX signal occurred
        TCPIP_REBOOT_Process();
    }

}


// send a signal to the Reboot module that data is available
// no manager alert needed since this normally results as a higher layer (UDP) signal
static void RebootSocketRxSignalHandler(UDP_SOCKET hUDP, TCPIP_NET_HANDLE hNet, TCPIP_UDP_SIGNAL_TYPE sigType, const void* param)
{
    if(sigType == TCPIP_UDP_SIGNAL_RX_DATA)
    {
        (void)TCPIPStackModuleSignalRequest(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_RX_PENDING, true); 
    }
}



static void TCPIP_REBOOT_Process(void)
{
    uint16_t nBytes, rebootMsgSize;
    bool    msgFail, needReboot;
    uint8_t rebootBuffer[sizeof(TCPIP_REBOOT_MESSAGE)];

#if defined(TCPIP_REBOOT_SAME_SUBNET_ONLY)
    UDP_SOCKET_INFO     sktInfo;
#endif  // defined(TCPIP_REBOOT_SAME_SUBNET_ONLY)

    rebootMsgSize = (uint16_t)strlen(TCPIP_REBOOT_MESSAGE);

    while(true)
    {
        // Do nothing if no data is waiting
        nBytes = TCPIP_UDP_GetIsReady(rebootSocket);

        if(nBytes == 0U)
        {   // no more data pending
            return;
        }

        msgFail = needReboot = false;
        if(nBytes < rebootMsgSize)
        {   // wrong message received
            msgFail = true;
        }
#if defined(TCPIP_REBOOT_SAME_SUBNET_ONLY)
        else
        {   // Respond only to name requests sent to us from nodes on the same subnet
            (void)memset(&sktInfo, 0, sizeof(sktInfo));
            (void)TCPIP_UDP_SocketInfoGet(rebootSocket, &sktInfo);
            if(TCPIPStackIpAddFromAnyNet(NULL, &sktInfo.remoteIPaddress.v4Add) == NULL)
            {
                msgFail = true;
            }
        }
#endif  // defined(TCPIP_REBOOT_SAME_SUBNET_ONLY)

        if(!msgFail)
        {   // check that we got the reset message
            (void)TCPIP_UDP_ArrayGet(rebootSocket, rebootBuffer, rebootMsgSize);
            rebootBuffer[rebootMsgSize] = 0;

            if(strcmp((char*)rebootBuffer, TCPIP_REBOOT_MESSAGE) == 0)
            {   // got the reset message
                needReboot = true;
            }
        }

        (void)TCPIP_UDP_Discard(rebootSocket);
        if(needReboot)
        {
            SYS_ERROR(SYS_ERROR_WARNING, "System remote reset requested\r\n");
            SYS_RESET_SoftwareReset();
        }
    }

}

#endif  // defined(TCPIP_STACK_USE_IPV4) && defined(TCPIP_STACK_USE_REBOOT_SERVER)

