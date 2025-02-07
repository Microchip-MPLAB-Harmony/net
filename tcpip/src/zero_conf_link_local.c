/*******************************************************************************
  Zero Configuration (Zeroconf) IPV4 Link Local Addressing
  Module for Microchip TCP/IP Stack

  Summary:

  Description:
    Zeroconf implementation: RFC 3927:
    Dynamic Configuration of IPv4 Link-Local Addresses
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








#define TCPIP_THIS_MODULE_ID    TCPIP_MODULE_ZCLL

#include "tcpip_private.h"
#include "zero_conf_link_local_private.h"

#if defined(TCPIP_STACK_USE_IPV4)
#if !defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)

bool TCPIP_ZCLL_Enable(TCPIP_NET_HANDLE hNet)
{
    return false;
}

bool TCPIP_ZCLL_Disable(TCPIP_NET_HANDLE hNet)
{
    return false;
}

bool TCPIP_ZCLL_IsEnabled(TCPIP_NET_HANDLE hNet)
{
    return false;
}


#else   // defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)


/**************** Global Declarations ***************/
static ZCLL_NET_HANDLE* phZCLL = NULL;

static int              zcllInitCount = 0;          // ZCLL module initialization count
static TCPIP_SIGNAL_HANDLE zcllSignalHandle = NULL; // ZCLL timer handle

/***************** Forward Declarations **************/

#if (TCPIP_STACK_DOWN_OPERATION != 0)
static void     F_ZCLLCleanup(const TCPIP_STACK_MODULE_CTRL* const stackCtrl);
#else
#define F_ZCLLCleanup(stackCtrl)
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)

static void     F_ZCLLDisable(TCPIP_NET_IF* pNetIf);

static bool     F_ZCLLEnable(TCPIP_NET_IF* pNetIf);



static void F_ZeroconfARPPktNotify (const TCPIP_NET_IF*  pNetIf, uint32_t SenderIPAddr, uint32_t TargetIPAddr,
                                   TCPIP_MAC_ADDR* SenderMACAddr, TCPIP_MAC_ADDR* TargetMACAddr, uint8_t op_req);

static ARP_PKT_TYPE F_FindARPPktType(uint32_t SrcIPAddr, uint32_t DestIPAddr, uint8_t op_req);


static void TCPIP_ZCLL_Process(void);

static void TCPIP_ZCLL_ARPAction(TCPIP_NET_HANDLE hNet, const IPV4_ADDR* SrcIPAddr, const IPV4_ADDR* DestIPAddr,
              uint16_t arpOpType, ZCLL_ARP_STATE arp_action);

static struct arp_app_callbacks callbacks =
{
   .TCPIP_ARP_PacketNotify  = &F_ZeroconfARPPktNotify,
};


static void F_ZCLL_RandInit(ZCLL_NET_HANDLE *hZcll, const TCPIP_NET_IF* pNetIf)
{
    if(hZcll->rand_state == 0UL)
    {   // Seed the pseudo-rng with the interface's MAC address as recommended by RFC3927, Section 2.1.
        // Different hosts will generate unique values
        const uint8_t *mac_addr = TCPIP_STACK_NetUpMACAddressGet(pNetIf);
        uint32_t rand_val = 0;
        if(mac_addr != NULL)
        {
            rand_val =  (((uint32_t)mac_addr[2] << 24)  | ((uint32_t)mac_addr[3] << 16) |
                        ((uint32_t)mac_addr[4] << 8)    | ((uint32_t)mac_addr[5]));
        }

        // The MAC address should never be zero.
        // But here's a way to recover just in case.
        if(rand_val == 0UL)
        {
            rand_val = SYS_RANDOM_PseudoGet();
        }

        hZcll->rand_state = rand_val;
    }
}


// returns a pseudo-random number in the range [min, max].
// Use a LCG modulo p, with p=2^32
// simplest case to implement, since the modulo arithmetic is made automatically
#define ZCLL_LCG32_A    (2147001325UL)
#define ZCLL_LCG32_B    (715136305UL)
static uint32_t F_zcll_rand(ZCLL_NET_HANDLE* hZcll, uint32_t min, uint32_t max)
{
    hZcll->rand_state = ZCLL_LCG32_A * hZcll->rand_state + ZCLL_LCG32_B;

   return min + hZcll->rand_state % (max - min + 1UL);
}

/***************************************************************
  Function:
   void TCPIP_ZCLL_ARPAction(TCPIP_NET_HANDLE hNet, IPV4_ADDR* SrcIPAddr, IPV4_ADDR* DestIPAddr, uint16_t arpOpType, ZCLL_ARP_STATE arp_action)

  Summary:
     a).ZCLL_ARP_PROBE:
     Sends out the ARP-probe packet.
     b).ZCLL_ARP_CLAIM:
     Sends out the ARP-Claim packet.
     c).ZCLL_ARP_DEFEND:
         Sends out the ARP-Defend packet, when a address-conflict is detected.

  Description:


    a).ZCLL_ARP_PROBE:
   This function is used to send out the ARP-Probe packet to check
    the uniquness of selected IP-address in private space(169.254.x.x)

    This function makes use of ARPSendPkt User-API exposed by ARP
    module.

    ARP-Probe Packet:
     ARP-Request
     sender IP address: 0.0.0.0
     sender HW address: Self MAC address
     target IP address: <probe addr> (Chosen IP-address 169.254.x.x)
     target HW address: FF:FF:FF:FF:FF:FF

    b).ZCLL_ARP_CLAIM:
    This function is used to send out the ARP-Claim packet to finalize
    the uniquness of selected IP-address in private space(169.254.x.x).
    This claim packet is final-step in decision making of selected IP-
    address.

    This function makes use of ARPSendPkt User-API exposed by ARP
    module.

    ARP-Probe Packet:
     ARP-Request
     sender IP address: <claim addr> (Chosen IP-address 169.254.x.x)
     sender HW address: Self MAC address
     target IP address: <claim addr> (Chosen IP-address 169.254.x.x)
     target HW address: FF:FF:FF:FF:FF:FF

    c).ZCLL_ARP_DEFEND:
       This function is used to send out the ARP-Defend packet to defend
    the selected IP-address. When a conflicting ARP-packet (Probe or
    Claim) is observed on local network ARP-defend packet will be sent
    out to announe its authority of owning IP-address.

    This function makes use of ARPSendPkt User-API exposed by ARP
    module.

    ARP-Probe Packet:
     ARP-Response
     sender IP address: <claim addr> (Chosen IP-address 169.254.x.x)
     sender HW address: Self MAC address
     target IP address: <claim addr> (Chosen IP-address 169.254.x.x)
     target HW address: FF:FF:FF:FF:FF:FF

  Parameters:
   arpOpType - a TCPIP_ARP_OPERATION_TYPE value

  Returns:
     None
  ***************************************************************/
 static void TCPIP_ZCLL_ARPAction(TCPIP_NET_HANDLE hNet, const IPV4_ADDR* SrcIPAddr, const IPV4_ADDR* DestIPAddr,
              uint16_t arpOpType, ZCLL_ARP_STATE arp_action)
{
    TCPIP_ARP_RESULT probeRes;

    probeRes = TCPIP_ARP_Probe(hNet, DestIPAddr, SrcIPAddr, (TCPIP_ARP_OPERATION_TYPE)arpOpType);

    if((int)probeRes < 0)
    {
        switch (arp_action)
        {
            case ZCLL_ARP_PROBE:
                INFO_ZCLL_PRINT("ZCLL_ARP_PROBE: Error in sending out ARP-Probe pkt \n");
                break;
            case ZCLL_ARP_CLAIM:
                INFO_ZCLL_PRINT("ZCLL_ARP_CLAIM: Error in sending out ARP-Claim pkt \n");
                break;
            case ZCLL_ARP_DEFEND:
                INFO_ZCLL_PRINT("ZCLL_ARP_DEFEND: Error in sending out ARP-Defend pkt \n");
                break;

            default:
                // should not happen
                TCPIPStack_Assert(false, __FILE__, __func__, __LINE__);
                break;
        }
    }

}

/***************************************************************
  Function:
   ARP_PKT_TYPE F_FindARPPktType (uint32_t SrcIPAddr, uint32_t DestIPAddr,
                                 uint8_t op_req)

  Summary:
   Finds the Type of ARP-Packet based on the Source IP-address,
    Destination IP-address and operation-request.

  Description:
   This function is used to find out the ARP-packet type. When ARP
    module passes up a ARP-packet to Zeroconf Link-Local module, it
    parses contents and finds the packet-type (like ARP-Probe, ARP-
    Claim, ARP-Defend, or generic ARP-request/response)

  Parameters:
   SrcIPAddr    - Source IP-Address
    DestIPAddr   - Destination IP-Address
    op_req       - Operation-Request (ARP-Request/Response)

  Returns:
     ARP_PKT_TYPE - Type of ARP-Packet (Probe, Claim, Defend or
                    generic ARP-request/response)
  ***************************************************************/
static ARP_PKT_TYPE F_FindARPPktType (uint32_t SrcIPAddr, uint32_t DestIPAddr, uint8_t op_req)
{
    if(op_req == (uint8_t)ARP_OPERATION_REQ)
    {
        if(SrcIPAddr == 0UL)
        {
            return ARP_PROBE_TYPE;
        }
        else if (SrcIPAddr == DestIPAddr)
        {
            return ARP_CLAIM_TYPE;
        }
        else
        {
            return ARP_REQUEST_TYPE;
        }

    }

    else if(op_req == (uint8_t)ARP_OPERATION_RESP)
    {
        if(SrcIPAddr == DestIPAddr)
        {
            return ARP_DEFEND_TYPE;
        }
        else
        {
            return ARP_RESPONSE_TYPE;
        }
    }

    else
    {
        return UNKNOWN_TYPE;
    }
}

/***************************************************************
  Function:
static void F_ZeroconfARPPktNotify ( TCPIP_NET_IF*  pNetIf
                                 ,uint32_t SenderIPAddr
                                 ,uint32_t TargetIPAddr
                                 ,TCPIP_MAC_ADDR* SenderMACAddr
                                 ,TCPIP_MAC_ADDR* TargetMACAddr
                                 ,uint8_t op_req)

  Summary:
   Callback registered with ARP-Module. This gets invoked from ARP-
    module and runs in the same context.

  Description:
   This function is registered as a callback with ARP-module to get
    notified about incoming Packet-events. Based on the type of packet
    received and Link-Local current state, appropriate action will be
    taken. To find the type of ARP-Packet this function makes use of
    F_FindARPPktType routine.

    Primary purpose of this function is to decipher the ARP-Packet rxed
    and check whether its leading to a conflict with the selected IP-
    address.

    Two types of conflicts are defined: Probe-Conflict and Late-Conflict
    If the current state of Link-Local is Probe/Claim and a conflict is
    detected its called "Probe-Conflict"
    If the current state of Link-Local is Defend-state and a conflict is
    detected its called "Late-Conflict"

  Parameters:
   SenderIPAddr    - Sender IP-Address
    TargetIPAddr    - Target IP-Address
    SenderMACAddr   - Sender MAC-Address
    TargetMACAddr   - Target MAC-Address
    op_req          - Operation-Request (ARP-Request/Response)

  Returns:
     None
  ***************************************************************/
static void F_ZeroconfARPPktNotify ( const TCPIP_NET_IF*  pNetIf
                                 ,uint32_t SenderIPAddr
                                 ,uint32_t TargetIPAddr
                                 ,TCPIP_MAC_ADDR* SenderMACAddr
                                 ,TCPIP_MAC_ADDR* TargetMACAddr
                                 ,uint8_t op_req)
{
    ARP_PKT_TYPE pkt_type;
    ZCLL_NET_HANDLE *hZcll;

    int i;

    pkt_type = F_FindARPPktType (SenderIPAddr, TargetIPAddr, op_req);

    if(pkt_type == UNKNOWN_TYPE)
    {
        return; // Can't hit this
    }

    i = TCPIP_STACK_NetIxGet(pNetIf);
    hZcll =(phZCLL + i);

    switch (hZcll->zcll_state)
    {
        case SM_ADDR_PROBE:
        case SM_ADDR_CLAIM:
            {
                switch(pkt_type)
                {
                    case ARP_PROBE_TYPE:
                    case ARP_CLAIM_TYPE:
                    case ARP_DEFEND_TYPE:
                        if(hZcll->temp_IP_addr.Val == TargetIPAddr ) // Probe-Conflict
                        {
                            if(memcmp(SenderMACAddr->v, pNetIf->netMACAddr.v, 6) != 0)
                            {
                                hZcll->zcll_flags.probe_conflict = 1U;
                            }
                        }
                        break;

                    case ARP_RESPONSE_TYPE:
                        /* Some-body has been using probed addr
                         * We need to choose different Address */
                        if(hZcll->temp_IP_addr.Val == SenderIPAddr)
                        {
                            INFO_ZCLL_PRINT("IP address conflict. Please wait...\r\n");
                            hZcll->zcll_flags.probe_conflict = 1U;
                        }
                        break;

                    default:
                        // do nothing
                        break;
                }
            }
            break;

        case SM_ADDR_DEFEND:
            if(pNetIf->netIPAddr.Val == SenderIPAddr)
            {
                if(memcmp(SenderMACAddr->v, pNetIf->netMACAddr.v, 6) != 0)
                {
                    INFO_ZCLL_PRINT("Zero Conf is defending the IP address\r\n");
                    hZcll->zcll_flags.late_conflict = 1U;
                }
            }
            break;

        default:
            // Nothing to do in other states
            break;
    }

}

static void F_ZCLLDisable(TCPIP_NET_IF* pNetIf)
{
    ZCLL_NET_HANDLE *hZcll;

    hZcll = (phZCLL + TCPIP_STACK_NetIxGet(pNetIf));
    (void)TCPIP_ARP_CallbacksDeregister((size_t)hZcll->arpRegId);
    hZcll->zcll_state = SM_INIT;   //  stop the ZCLL state machine
    pNetIf->Flags.bIsZcllEnabled = 0U;
}

static bool F_ZCLLEnable(TCPIP_NET_IF* pNetIf)
{
    ZCLL_NET_HANDLE* hZcll;

    hZcll = (phZCLL + TCPIP_STACK_NetIxGet(pNetIf));
    hZcll->arpRegId = (int8_t)TCPIP_ARP_CallbacksRegister(&callbacks);
    if(hZcll->arpRegId <0)
    {
        hZcll->zcll_state = SM_INIT;   //  should already be in idle state
        SYS_ERROR(SYS_ERROR_ERROR, "F_ZCLLEnable: ARP registration Failed!!! \r\n");
        return false;
    }

    pNetIf->Flags.bIsZcllEnabled = 1U;
    hZcll->zcll_state = SM_ADDR_INIT;
    return true;
}

bool TCPIP_ZCLL_Enable(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);

    if(pNetIf == NULL)
    {
        return false;
    }

    if(pNetIf->Flags.bIsZcllEnabled != 0U)
    {   // already started
        return true;
    }

    if(TCPIP_STACK_AddressServiceCanStart(pNetIf, TCPIP_STACK_ADDR_SRVC_ZCLL))
    { 
        return F_ZCLLEnable(pNetIf);
    }

    return false;
}

bool TCPIP_ZCLL_Disable(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    if(pNetIf != NULL)
    {
        if(pNetIf->Flags.bIsZcllEnabled != 0U)
        {
            F_ZCLLDisable(pNetIf);
            TCPIP_STACK_AddressServiceEvent(pNetIf, TCPIP_STACK_ADDR_SRVC_ZCLL, TCPIP_ADDR_SRVC_EV_USER_STOP);
        }
        return true;
    }

    return false;
}

bool TCPIP_ZCLL_IsEnabled(TCPIP_NET_HANDLE hNet)
{
    TCPIP_NET_IF* pNetIf = TCPIPStackHandleToNetUp(hNet);
    if(pNetIf != NULL)
    {
        return pNetIf->Flags.bIsZcllEnabled != 0U;
    }

    return false;
}

// Enables/disables the ZCLL service as requested by the stack manager
// No callback into the stack manager is done
// No other checking is done
bool TCPIP_ZCLL_ServiceEnable(TCPIP_NET_IF* pNetIf, bool enable)
{
    bool res = false;

    if(enable == true && pNetIf->Flags.bIsZcllEnabled == 0U)
    {
        res = F_ZCLLEnable(pNetIf);
    }
    else if(enable == false && pNetIf->Flags.bIsZcllEnabled != 0U)
    {
        F_ZCLLDisable(pNetIf);
        res = true;
    }
    else
    {
        // do nothing
    }

    return res;
}

/***************************************************************
  Function:
   void TCPIP_ZCLL_Deinitialize(void)

  Summary:
   Deinitialization routine for Zeroconf Link-Local state-machine.

  Description:
    This is deinitialization function for Zeroconf Link-Local and
    invoked from deinitialization portion of Main-function.

  Parameters:
   None

  Returns:
     None
  ***************************************************************/
#if (TCPIP_STACK_DOWN_OPERATION != 0)
void TCPIP_ZCLL_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_DEINIT) // stack shut down
    // if(stackCtrl->stackAction == TCPIP_STACK_ACTION_IF_DOWN) // interface down

    if(zcllInitCount > 0)
    {   // we're up and running
        // one way or another this interface is going down
        F_ZCLLDisable(stackCtrl->pNetIf);

        if(stackCtrl->stackAction == (uint8_t)TCPIP_STACK_ACTION_DEINIT)
        {   // whole stack is going down
            if(--zcllInitCount == 0)
            {   // all closed
                // release resources
                F_ZCLLCleanup(stackCtrl);
            }
        }
    }
}

static void F_ZCLLCleanup(const TCPIP_STACK_MODULE_CTRL* const stackCtrl)
{
    (void)TCPIP_HEAP_Free(stackCtrl->memH, phZCLL);  // free the allocated memory
    phZCLL = NULL;
    if(zcllSignalHandle != NULL)
    {
        TCPIPStackSignalHandlerDeregister(zcllSignalHandle);
        zcllSignalHandle = NULL;
    }

}
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)


/***************************************************************
  Function:
   bool TCPIP_ZCLL_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackData, const void* initData);

  Summary:
   Initialization routine for Zeroconf Link-Local state-machine.

  Description:
    This is initialization function for Zeroconf Link-Local and
    invoked from initialization portion of Main-function.

    This function registers with ARP-module to get notifications
    about the incoming packets. Checks whether the WiFi MAC is
    connected to an Access-Point or not.

  Parameters:
   None

  Returns:
     None
  ***************************************************************/
bool TCPIP_ZCLL_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackData, const void* initData)
{
    if(stackData->stackAction != (uint8_t)TCPIP_STACK_ACTION_IF_UP)
    {   // stack init/restart
        if(zcllInitCount == 0)
        {   // 1st time we run

            phZCLL = (ZCLL_NET_HANDLE*)TCPIP_HEAP_Calloc(stackData->memH, stackData->nIfs, sizeof(ZCLL_NET_HANDLE));
            if(phZCLL == NULL)
            {
                SYS_ERROR(SYS_ERROR_ERROR, "TCPIP_ZCLL_Initialize: Failed to allocate memory\r\n");
                return false;
            }
            zcllSignalHandle =TCPIPStackSignalHandlerRegister(TCPIP_THIS_MODULE_ID, &TCPIP_ZCLL_Task, ZCLL_TASK_TICK_RATE); 
            if(zcllSignalHandle == NULL)
            {   // cannot create the ZCLL timer
                F_ZCLLCleanup(stackData);
                return false;
            }
        }
        zcllInitCount++;
    }

    if(stackData->pNetIf->Flags.bIsZcllEnabled != 0U)
    {
        (void)F_ZCLLEnable(stackData->pNetIf);
    }

    return true;
}

void TCPIP_ZCLL_Task(void)
{
    TCPIP_MODULE_SIGNAL sigPend;

    sigPend = TCPIPStackModuleSignalGet(TCPIP_THIS_MODULE_ID, TCPIP_MODULE_SIGNAL_MASK_ALL);

    if(((uint16_t)sigPend & (uint16_t)TCPIP_MODULE_SIGNAL_TMO) != 0U)
    { // regular TMO occurred
        TCPIP_ZCLL_Process();
    }

}


static void TCPIP_ZCLL_Process(void)
{
    size_t netIx;
    ZCLL_NET_HANDLE *hZcll;
    const TCPIP_NET_IF* cNetIf;
    TCPIP_NET_IF* pNetIf;
    uint8_t     zgzc_action;
    IPV4_ADDR zeroAdd = {0};
#if defined(TCPIP_ZC_INFO_ZCLL) || defined(TCPIP_ZC_DEBUG_ZCLL)
    char zeroconf_dbg_msg[256];
#endif

    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet(); 
    for(netIx = 0; netIx < TCPIP_STACK_NumberOfNetworksGet(); netIx++)
    {

        hZcll = phZCLL + netIx;
        cNetIf = (const TCPIP_NET_IF*)TCPIP_STACK_IndexToNet(netIx);
        pNetIf = FC_CNetIf2NetIf(cNetIf);


        if(hZcll->zcll_state == SM_INIT)
        {   // nothing to do in this state
            continue;
        }

        if(!TCPIP_STACK_NetworkIsLinked(cNetIf))
        {   // lost connection; re-start
            hZcll->zcll_state = SM_ADDR_INIT;
            TCPIP_STACK_AddressServiceEvent(pNetIf, TCPIP_STACK_ADDR_SRVC_ZCLL, TCPIP_ADDR_SRVC_EV_CONN_LOST);
            return;
        }

        switch(hZcll->zcll_state)
        {
            case SM_ADDR_INIT:
                // Setup random number generator
                F_ZCLL_RandInit(hZcll, cNetIf);

                hZcll->conflict_count = 0U;
                TCPIPStackSetConfigAddress(pNetIf, &zeroAdd, &zeroAdd, NULL, true);
                hZcll->probe_count = 0U;

                hZcll->zcll_state = SM_ADDR_PROBE;
                INFO_ZCLL_PRINT("ADDR_INIT --> ADDR_PROBE \r\n");

                break;

            case SM_ADDR_PROBE:

                zgzc_action = zgzc_wait_for(&hZcll->random_delay, &hZcll->event_time, &hZcll->time_recorded);

                if(zgzc_action == ZGZC_STARTED_WAITING)
                {
                    if (hZcll->probe_count == 0U)
                    {
                        // First probe. Wait for [0 ~ PROBE_WAIT] seconds before sending the probe.

                        hZcll->random_delay = F_zcll_rand(hZcll, 0L, (uint32_t)PROBE_WAIT * sysFreq);
#if defined(TCPIP_ZC_DEBUG_ZCLL)
                        (void)FC_sprintf(zeroconf_dbg_msg, sizeof(zeroconf_dbg_msg),"PROBE_WAIT Random Delay [%d]: %lu secs \r\n", hZcll->probe_count, (unsigned long)hZcll->random_delay);
#endif  // defined(TCPIP_ZC_DEBUG_ZCLL)
                    }
                    else if (hZcll->probe_count < (uint8_t)PROBE_NUM)
                    {
                        // Subsequent probes. Wait for [PROBE_MIN ~ PROBE_MAX] seconds before sending the probe.

                        hZcll->random_delay = F_zcll_rand(hZcll, (uint32_t)PROBE_MIN * sysFreq,
                                                          (uint32_t)PROBE_MAX * sysFreq);

#if defined(TCPIP_ZC_DEBUG_ZCLL)
                        (void)FC_sprintf(zeroconf_dbg_msg, sizeof(zeroconf_dbg_msg),"PROBE Random Delay [%d]: %lu ticks \r\n", hZcll->probe_count, (unsigned long)hZcll->random_delay);
#endif  // defined(TCPIP_ZC_DEBUG_ZCLL)
                    }
                    else
                    {
                        // Completed PROBE_NUM of probes. Now wait for ANNOUNCE_WAIT seconds to determine if
                        // we can claim it.

                        hZcll->random_delay = ((uint32_t)ANNOUNCE_WAIT * sysFreq);
#if defined(TCPIP_ZC_DEBUG_ZCLL)
                        (void)FC_sprintf(zeroconf_dbg_msg, sizeof(zeroconf_dbg_msg),"ANNOUNCE_WAIT delay [%d]: %lu ticks\r\n", hZcll->probe_count, (unsigned long)hZcll->random_delay);
#endif  // defined(TCPIP_ZC_DEBUG_ZCLL)
                    }

                    DEBUG_ZCLL_PRINT((char*)zeroconf_dbg_msg);
                    break;
                }
                else if(zgzc_action == ZGZC_KEEP_WAITING)
                {   // Not Completed the delay proposed
                    break;
                }
                else
                {
                    // do nothing
                }

                // Completed the delay required

#if defined(TCPIP_ZC_DEBUG_ZCLL)
                (void)FC_sprintf(zeroconf_dbg_msg, sizeof(zeroconf_dbg_msg),"   delay: %lu ticks completed \r\n", (unsigned long)hZcll->random_delay);
#endif  // defined(TCPIP_ZC_DEBUG_ZCLL)
                DEBUG_ZCLL_PRINT((char *)zeroconf_dbg_msg);

                if(hZcll->zcll_flags.probe_conflict != 0U)
                {
                    /* Conflict with selected address */
                    INFO_ZCLL_PRINT("Probe Conflict-1 Detected. Need to select diff addr \r\n");
                    hZcll->temp_IP_addr.Val = 0x0;

                    hZcll->conflict_count++;
                    TCPIPStackSetConfigAddress(pNetIf, &zeroAdd, &zeroAdd, NULL, true);
                }
                else if((hZcll->conflict_count == 0U) &&
                        hZcll->temp_IP_addr.Val != 0UL &&
                        pNetIf->netIPAddr.Val != 0UL &&
                        (TCPIP_ARP_IsResolved(cNetIf,&hZcll->temp_IP_addr, &hZcll->temp_MAC_addr)) )
                {
                    if(memcmp (hZcll->temp_MAC_addr.v, cNetIf->netMACAddr.v, 6) == 0)
                    {
                        DEBUG_ZCLL_PRINT("SM_ADDR_PROBE: Resolved with our address only. " \
                                "Rare Case !!!! \r\n");
                    }
                    else
                    {
                        /* Conflict with selected address */
                        INFO_ZCLL_PRINT("Probe Conflict-2 Detected. Need to select diff addr \r\n");
                        hZcll->temp_IP_addr.Val = 0x0;

                        hZcll->conflict_count++;
                        TCPIPStackSetConfigAddress(pNetIf, &zeroAdd, &zeroAdd, NULL, true);
                    }
                }
                else
                {
                    // do nothing
                }

                if ((hZcll->zcll_flags.probe_conflict == 1U) ||
                        (hZcll->bDefaultIPTried == 0U))
                {
                    /*
                     * Pick random IP address in IPv4 link-local range
                     * 169.254.1.0/16 is the allowed address range however
                     * 169.254.0.0/24 and 169.254.255.0/24 must be excluded,
                     * which removes 512 address from our 65535 candidates.
                     * That leaves us with 65023 (0xfdff).
                     * The link-local address must start with 169.254.#.#
                     * If it does not then pick a random link-local address.
                     */
                    hZcll->probe_count = 0U;

                    if(hZcll->bDefaultIPTried == 0U)
                    {
                        // First probe, and the default IP is a valid IPV4_SOFTAP_LLBASE address.
                        if(cNetIf->DefaultIPAddr.v[0] != 169U ||
                           cNetIf->DefaultIPAddr.v[1] != 254U ||
                           cNetIf->DefaultIPAddr.v[2] == 0U   ||
                           cNetIf->DefaultIPAddr.v[2] == 255U)
                        {

#if defined(CONFIG_TCPIP_ZC_WARN_ZCLL)
                            (void)FC_sprintf(zeroconf_dbg_msg, sizeof(zeroconf_dbg_msg),"\r\n%d.%d.%d.%d not a valid link local addess. "
                                    "Autogenerating address.\r\n"
                                    ,cNetIf->DefaultIPAddr.v[0],cNetIf->DefaultIPAddr.v[1]
                                    ,cNetIf->DefaultIPAddr.v[2],cNetIf->DefaultIPAddr.v[3]);
#endif  // CONFIG_TCPIP_ZC_WARN_ZCLL(TCPIP_ZC_INFO_ZCLL)
                            WARN_ZCLL_PRINT((char *)zeroconf_dbg_msg);
                            // First probe, if the default IP is a valid IPv4 LL then use it.
                            hZcll->temp_IP_addr.Val = F_zcll_rand(hZcll, (uint32_t)IPV4_LLBASE, (uint32_t)IPV4_LLBASE + 0xfdffUL);
                        }
                        else
                        {
                            hZcll->temp_IP_addr.Val = TCPIP_Helper_ntohl(cNetIf->DefaultIPAddr.Val);
                        }

                        hZcll->bDefaultIPTried = 1U;
                    }
                    else
                    {
                        hZcll->temp_IP_addr.Val = F_zcll_rand(hZcll, (uint32_t)IPV4_LLBASE, (uint32_t)IPV4_LLBASE + 0xfdffUL);
                    }

#if defined(TCPIP_ZC_INFO_ZCLL)
                    (void)FC_sprintf(zeroconf_dbg_msg, sizeof(zeroconf_dbg_msg), "Picked IP-Addr [%d]: %d.%d.%d.%d \r\n",
                            hZcll->probe_count,
                            hZcll->temp_IP_addr.v[3],hZcll->temp_IP_addr.v[2],
                            hZcll->temp_IP_addr.v[1],hZcll->temp_IP_addr.v[0]);
#endif  // defined(TCPIP_ZC_INFO_ZCLL)
                    INFO_ZCLL_PRINT((char *)zeroconf_dbg_msg);

                    hZcll->temp_IP_addr.Val = TCPIP_Helper_ntohl((uint32_t) hZcll->temp_IP_addr.Val);
                }

                if((hZcll->zcll_flags.probe_conflict == 1U) || (hZcll->probe_count < (uint8_t)PROBE_NUM))
                {

                    hZcll->zcll_flags.probe_conflict = 0U;

                    TCPIP_ZCLL_ARPAction( cNetIf
                            , &cNetIf->netIPAddr
                            , &hZcll->temp_IP_addr
                            , (uint16_t)ARP_OPERATION_REQ | (uint16_t)ARP_OPERATION_CONFIGURE | (uint16_t)ARP_OPERATION_PROBE_ONLY
                            , ZCLL_ARP_PROBE);
                    hZcll->probe_count++;

#if defined(TCPIP_ZC_DEBUG_ZCLL)
                    (void)FC_sprintf(zeroconf_dbg_msg, sizeof(zeroconf_dbg_msg), "Sending ARP [%d]\r\n", hZcll->probe_count);
#endif  // defined(TCPIP_ZC_DEBUG_ZCLL)
                    DEBUG_ZCLL_PRINT((char *)zeroconf_dbg_msg);

                    break;
                }

                // No conflict detected ...

                if(hZcll->probe_count >= (uint8_t)PROBE_NUM)
                {
                    hZcll->zcll_state = SM_ADDR_CLAIM;
                    hZcll->bDefaultIPTried = 0U;

                    INFO_ZCLL_PRINT("ADDR_PROBE --> ADDR_CLAIM \r\n");
                }

                break;

            case SM_ADDR_CLAIM:
                zgzc_action = zgzc_wait_for( &hZcll->random_delay, &hZcll->event_time, &hZcll->time_recorded);

                if(zgzc_action == ZGZC_STARTED_WAITING)
                {
                    if (hZcll->bDefaultIPTried == 0U)
                    {
                        // First announcement is immediate. We have passed the ANNOUNCE_WAIT in
                        // PROBE state already.

                        hZcll->random_delay = 0UL;
                    }
                    else
                    {
                        // Subsequent announcements need to wait ANNOUNCE_INTERVAL seconds
                        // before sending the announcement.

                        hZcll->random_delay = ((uint32_t)ANNOUNCE_INTERVAL * SYS_TMR_TickCounterFrequencyGet());
                    }
                    break;
                }
                else if(zgzc_action == ZGZC_KEEP_WAITING)
                {   // Not Completed the delay proposed
                    break;
                }
                else
                {
                    // do nothing
                }

                // Completed the delay required

#if defined(TCPIP_ZC_DEBUG_ZCLL)
                (void)FC_sprintf(zeroconf_dbg_msg, sizeof(zeroconf_dbg_msg),"ANNOUNCE delay: %lu ticks completed \r\n", hZcll->random_delay);
#endif  // defined(TCPIP_ZC_DEBUG_ZCLL)
                DEBUG_ZCLL_PRINT((char *)zeroconf_dbg_msg);

                if ( hZcll->bDefaultIPTried < (uint8_t)ANNOUNCE_NUM )
                {
                    TCPIP_ZCLL_ARPAction(cNetIf,&hZcll->temp_IP_addr,&hZcll->temp_IP_addr, (uint16_t)ARP_OPERATION_REQ | (uint16_t)ARP_OPERATION_CONFIGURE | (uint16_t)ARP_OPERATION_PROBE_ONLY, ZCLL_ARP_CLAIM);
                    (hZcll->bDefaultIPTried)++;

#if defined(TCPIP_ZC_DEBUG_ZCLL)
                    (void)FC_sprintf(zeroconf_dbg_msg, sizeof(zeroconf_dbg_msg), "Sending ANNOUNCEMENT [%d]\r\n", hZcll->bDefaultIPTried);
#endif  // defined(TCPIP_ZC_DEBUG_ZCLL)
                    DEBUG_ZCLL_PRINT((char *)zeroconf_dbg_msg);
                }
                else
                {
                    // Claim it. Goto DEFEND state
                    IPV4_ADDR   zcllMask;
                    zcllMask.Val = IPV4_LLBASE_MASK;
                    TCPIPStackSetConfigAddress(pNetIf, &hZcll->temp_IP_addr, &zcllMask, NULL, false);
                    hZcll->zcll_state = SM_ADDR_DEFEND;
#if defined(TCPIP_ZC_INFO_ZCLL)
                    (void)FC_sprintf(zeroconf_dbg_msg, sizeof(zeroconf_dbg_msg), "\r\n******** Taken IP-Addr: %d.%d.%d.%d ******** \r\n",
                            cNetIf->netIPAddr.v[0],cNetIf->netIPAddr.v[1],
                            cNetIf->netIPAddr.v[2],cNetIf->netIPAddr.v[3]);
#endif  // defined(TCPIP_ZC_INFO_ZCLL)
                    INFO_ZCLL_PRINT((char *)zeroconf_dbg_msg);
                    INFO_ZCLL_PRINT("ADDR_CLAIM --> ADDR_DEFEND \r\n");
                }

                break;

            case SM_ADDR_DEFEND:

                if( hZcll->zcll_flags.late_conflict != 0U)
                {
                    if (hZcll->zcll_flags.defended == 0U)
                    {
                        hZcll->zcll_flags.late_conflict = 0U;
                        INFO_ZCLL_PRINT("CONFLICT DETECTED !!! \r\n");

                        INFO_ZCLL_PRINT("Defending the Self Address once \r\n");
                        TCPIP_ZCLL_ARPAction( cNetIf
                                ,&cNetIf->netIPAddr
                                ,&cNetIf->netIPAddr
                                ,(uint16_t)ARP_OPERATION_RESP | (uint16_t)ARP_OPERATION_CONFIGURE | (uint16_t)ARP_OPERATION_PROBE_ONLY
                                ,ZCLL_ARP_DEFEND);

                        hZcll->zcll_flags.defended = 1U;
                    }
                    else
                    {
                        // We are not allowed to defend another conflict during an active defended period

                        INFO_ZCLL_PRINT("Releasing the IP-Address because of multiple Conflicts \r\n");

                        hZcll->zcll_state = SM_ADDR_RELEASE;

                        hZcll->zcll_flags.defended = 0U;
                        hZcll->event_time = 0UL;
                        hZcll->random_delay = 0UL;

                        INFO_ZCLL_PRINT("ADDR_DEFEND --> ADDR_RELEASE \r\n");
                        break;
                    }
                }

                if (hZcll->zcll_flags.defended != 0U)
                {
                    zgzc_action = zgzc_wait_for(&hZcll->random_delay, &hZcll->event_time, &hZcll->time_recorded);

                    if(zgzc_action == ZGZC_STARTED_WAITING)
                    {
                        hZcll->random_delay = ((uint32_t)DEFEND_INTERVAL * SYS_TMR_TickCounterFrequencyGet());
#if defined(TCPIP_ZC_DEBUG_ZCLL)
                        (void)FC_sprintf(zeroconf_dbg_msg, sizeof(zeroconf_dbg_msg),"DEFEND_INTERVAL Delay : %lu ticks\r\n", hZcll->random_delay);
#endif  // defined(TCPIP_ZC_DEBUG_ZCLL)
                        DEBUG_ZCLL_PRINT((char *)zeroconf_dbg_msg);

                        break; 
                    }
                    else if(zgzc_action == ZGZC_KEEP_WAITING)
                    {   // Not Completed the delay proposed
                        break;
                    }
                    else
                    {
                        // do nothing
                    }

                    // Completed the delay required

#if defined(TCPIP_ZC_DEBUG_ZCLL)
                    (void)FC_sprintf(zeroconf_dbg_msg, sizeof(zeroconf_dbg_msg),"ANNOUNCE delay: %lu ticks completed \r\n", hZcll->random_delay);
#endif  // defined(TCPIP_ZC_DEBUG_ZCLL)
                    DEBUG_ZCLL_PRINT((char *)zeroconf_dbg_msg);

                    hZcll->zcll_flags.defended = 0U;
                }

                break;

            case SM_ADDR_RELEASE:

                INFO_ZCLL_PRINT("ADDR_RELEASE --> ADDR_INIT\r\n");

                TCPIPStackSetConfigAddress(pNetIf, &zeroAdd, &zeroAdd, NULL, true);

                // Need New Addr
                hZcll->temp_IP_addr.Val = F_zcll_rand(hZcll, (uint32_t)IPV4_LLBASE, (uint32_t)IPV4_LLBASE + 0xfdffUL);
                hZcll->temp_IP_addr.Val = TCPIP_Helper_ntohl((uint32_t) hZcll->temp_IP_addr.Val);

                hZcll->zcll_state = SM_ADDR_INIT;
                hZcll->time_recorded = 0U;
                hZcll->zcll_flags.defended = 0U;
                hZcll->event_time = 0UL;
                break;

            default:
                // do nothing
                break;
        }

    }

}

#endif  // defined(TCPIP_STACK_USE_ZEROCONF_LINK_LOCAL)
#endif  // defined(TCPIP_STACK_USE_IPV4)

