/*******************************************************************************
  Application to Demo SNMP Server

  Summary:
    Support for SNMP module in Microchip TCP/IP Stack

  Description:
    - Implements the SNMP application
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

#include <errno.h>

#include "system_config.h"
#include "system_definitions.h"

#if defined(TCPIP_STACK_USE_SNMP_SERVER)
#include "tcpip/tcpip.h"
#include "mib.h"
#include "tcpip/src/common/helpers.h"

#ifndef APP_SWITCH_1StateGet
#define APP_SWITCH_1StateGet() 0
#endif

#ifndef APP_SWITCH_2StateGet
#define APP_SWITCH_2StateGet() 0
#endif

#ifndef APP_SWITCH_3StateGet
#define APP_SWITCH_3StateGet() 0
#endif

#ifndef APP_LED_1StateGet
#define APP_LED_1StateGet() 0
#endif
#ifndef APP_LED_2StateGet
#define APP_LED_2StateGet() 0
#endif
#ifndef APP_LED_3StateGet
#define APP_LED_3StateGet() 0
#endif

#ifndef APP_LED_1StateSet
#define APP_LED_1StateSet()
#endif
#ifndef APP_LED_2StateSet
#define APP_LED_2StateSet()
#endif
#ifndef APP_LED_3StateSet
#define APP_LED_3StateSet()
#endif

#ifndef APP_LED_1StateClear
#define APP_LED_1StateClear()
#endif
#ifndef APP_LED_2StateClear
#define APP_LED_2StateClear()
#endif
#ifndef APP_LED_3StateClear
#define APP_LED_3StateClear()
#endif
/****************************************************************************
  Section:
    Global Variables
  ******* */

#define TCPIP_SNMP_TRAP_PARAM_SIZE      5

typedef int (*trap_param_fn)(SNMP_ID var, SNMP_VAL *val, void *data);

typedef struct tSNMP_TRAP_PARAM_INFO
{
   uint8_t Size;
    uint32_t        trap_id;
   struct
   {
       SNMP_ID var;
        uint8_t specTrap;
       trap_param_fn    snmp_trap_param_fn;
   } table[TCPIP_SNMP_TRAP_PARAM_SIZE];
} SNMP_TRAP_PARAM_INFO;

typedef struct tSNMP_TRAP_INFO
{
   uint8_t Size;
   struct
   {
       uint8_t communityLen;    //Community name length
       char community[TCPIP_SNMP_COMMUNITY_MAX_LEN];         //TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN   Community name array
       IPV4_ADDR IPAddress;     //IP address to which trap to be sent
       struct
       {
           unsigned int bEnabled : 1;   //Trap enabled flag
       } Flags;
    } table[TCPIP_SNMP_TRAP_TABLE_SIZE];
    SNMP_TRAP_PARAM_INFO *TrapParamsInfo;
} SNMP_TRAP_INFO;

typedef struct tIPV6_SNMP_TRAP_INFO
{
   uint8_t Size;
   struct
   {
       uint8_t communityLen;    //Community name length
       char community[TCPIP_SNMP_COMMUNITY_MAX_LEN];         //TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN   Community name array
       IPV6_ADDR IPv6Address;   //IPv6 address to which trap to be sent
       struct
       {
           unsigned int bEnabled : 1;   //Trap enabled flag
       } Flags;
   } table[TCPIP_SNMP_TRAP_TABLE_SIZE];
    SNMP_TRAP_PARAM_INFO *TrapParamsInfo;
} IPV6_SNMP_TRAP_INFO;

#define SNMP_MIB_II_INDX        0
#define SNMP_LLDP_MIB_INDX      1
#define SNMP_MAX_MIBS_SUPPORTED 2

#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
static char gSnmpv3UserSecurityName[TCPIP_SNMPV3_USER_SECURITY_NAME_LEN_MEM_USE];
static char gSnmpv3UserPrivPswdLoclizdKey[TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN_MEM_USE];
static char gSnmpv3UserAuthPswdLoclizdKey[TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN_MEM_USE];
#endif

static int SNMPv2_MIB_II_TrapParamGet(SNMP_ID var, SNMP_VAL *val, void *data);
static int SNMPv2_LLDP_TrapParamGet(SNMP_ID var, SNMP_VAL *val, void *data);

SNMP_TRAP_PARAM_INFO    g_mibIITrapParamInfo = {4, SNMP_DEMO_TRAP, 
                                                {{ANALOG_POT0, POT_READING_MORE_512, SNMPv2_MIB_II_TrapParamGet},
                                                    {PUSH_BUTTON, BUTTON_PUSH_EVENT, SNMPv2_MIB_II_TrapParamGet}, 
                                                    {LED_D5, POT_READING_MORE_512, SNMPv2_MIB_II_TrapParamGet}, 
                                                    {TRAP_COMMUNITY, POT_READING_MORE_512, SNMPv2_MIB_II_TrapParamGet}}};

SNMP_TRAP_PARAM_INFO    g_lldpTrapParamInfo = {1, LLDP_REM_TBL_CHNG,
                                                {{LLDP_MSG_INTRVL, 1U, SNMPv2_LLDP_TrapParamGet}}};

SNMP_TRAP_PARAM_INFO    *g_mibTrapParamInfo[SNMP_MAX_MIBS_SUPPORTED] = {&g_mibIITrapParamInfo, &g_lldpTrapParamInfo};

/*
*  Default STACK_USE_SMIV2 is enabled . For Stack V5.31,  STACK_USE_SMIV2 should be disabled.
*/
#define STACK_USE_SMIV2


/*
* NOTE -
* gSnmpNonMibRecInfo[] is the list of static variable Parent OIDs which are not part of MIB.h file.
* This structure is used to restrict access to static variables of SNMPv3 OIDs from SNMPv2c and SNMPv1 version.
* With SNMPv3 all the OIDs accessible but when we are using SNMPv2c version , static variables of the SNMPv3
* cannot be accessible with SNMPversion v2c.
* With V5.31 there was no MODULE-IDENTITY number in the SNMP.mib file. Now onwards we have supported SMIv2
* standard and SNMP.mib has been updated with respect to SMIV2 standard and it also includes
* MODULE-IDENTITY ( number 1)after ENTERPRISE-ID.
*/

/*
* This structure has been moved from snmp.c file to here.
*/
#ifdef STACK_USE_SMIV2
/*
* With SMIv2 standard which includes MODULE-IDENTITY number with the existing OID string.
* For New snmp.mib file with SMIv2 standard
*/
/*
* ENTERPRISEID - 17095(Microchip) as per BER encoding standard 0x81,0x85,0x47
* Need to be modified with respect to customer enterprise ID
*/


static SNMP_NON_MIB_RECD_INFO gSnmpNonMibRecInfo[] =
{
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
    /* To restrict SNMPv3 Static Variable OID string which is not part of mib.h file, User is required to include here.*More details in the above NOTE.*/
#endif
    {{43,6,1,2,1,1,0,0,0,0,0,0,0,0,0,0},SNMP_V2C}, /* Max matching Subids of the iso+org (43),dod(6),internet(1),mgmt(2),MIB2(1),system(1) tree*/
    {{43,6,1,4,1,0x81,0x85,0x47,0x1,1,0,0,0,0,0,0},SNMP_V2C},
    /*Max matching Subids of the iso+org (43),dod(6),internet(1),private(4),ENTERPRISE(17095),MODULE-IDENTITY(1),product tree*/
    {{40,196,98,1,1,2,0,0,0,0,0,0,0,0,0,0},SNMP_V2C}, /* Max matching Subids of the iso+org (43),dod(6),internet(1),mgmt(2),MIB2(1),system(1) tree*/

};
/*
 * if snmp.mib file doesnot have MODULE-IDENTITY number then this is the following structure should be used
 */

#else

/*
* OLD snmp.mib file with SMIv1 standard
*/

static SNMP_NON_MIB_RECD_INFO gSnmpNonMibRecInfo[] =
{
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
    {{43,6,1,4,1,0x81,0x85,0x47,6},SNMP_V3},  /* SNMPv3 PVT test MIB OID is not part of mib.h file */
#endif
    {{43,6,1,2,1,1},SNMP_V2C}, /* Max matching Subids of the iso+org (43),dod(6),internet(1),mgmt(2),MIB2(1),system(1) tree*/
    {{43,6,1,4,1,0x81,0x85,0x47,0x1},SNMP_V2C},
    /*Max matching Subids of the iso+org (43),dod(6),internet(1),private(4),ENTERPRISE(17095),product tree*/

};

#endif /* STACK_USE_SMIV2 */

// local prototypes
bool TCPIP_SNMP_SendTrapToSelctedInterface(size_t *netIx,TCPIP_NET_HANDLE netIf);
void SNMPV2TrapDemo(void);
void SNMPTrapDemo(void);

/*Initialize trap table with no entries.*/
static SNMP_TRAP_INFO trapInfo[SNMP_MAX_MIBS_SUPPORTED]={TCPIP_SNMP_TRAP_TABLE_SIZE};
#ifdef TCPIP_STACK_USE_IPV6
static IPV6_SNMP_TRAP_INFO ipv6TrapInfo[SNMP_MAX_MIBS_SUPPORTED]={TCPIP_SNMP_TRAP_TABLE_SIZE};
static bool TCPIP_SNMP_IPv6_NotificationSend(uint8_t mibIndex, uint8_t receiverIndex, SNMP_ID var, SNMP_VAL val,uint8_t targetIndex);
#endif
static uint32_t SNMPGetTimeStamp(void);

static bool SNMPSendNotification(uint8_t mibIndex, uint8_t receiverIndex, SNMP_ID var, SNMP_VAL val,uint8_t targetIndex);

static uint8_t  gSendTrapSMstate=0U;
static bool     gtrapSMStateUpdate=false;
static uint32_t g_lldpMsgIntrvl = 1234;
static uint32_t g_lldpMsgTxHoldMulti = 5;

#define MAX_TRY_TO_SEND_TRAP (30)

#if defined(SYS_OUT_ENABLE)
static uint8_t lcdMessage[16*2+1]="";
#endif
/****************************************************************************
  ===========================================================================
  Section:
    SNMP Routines
  ===========================================================================
  ******* */
static int SNMPv2_MIB_II_TrapParamGet(SNMP_ID var, SNMP_VAL *val, void *data)
{
    int *snmp_version = 0;
    if(data)
    {
        snmp_version = (int *)data;
    }
    switch(var){
        case ANALOG_POT0:
            val->word = (uint16_t)SYS_RANDOM_PseudoGet();
            if(val->word <= 12u)
                return 1;
            
            break;
            
        case PUSH_BUTTON:
            if(snmp_version == 0)
            {               
            val->byte = (uint8_t)APP_SWITCH_1StateGet();
            }
            else
            {
                if(*snmp_version == 1)
                {
                    if(APP_SWITCH_3StateGet() == 0u)
                    {
                        val->byte = 0;
                    }
                    else
                    {
                        return 1;
                    }
                }
            }
            break;
            
        case LED_D5:
            if(snmp_version == 0)
            {   
            val->byte = (uint8_t)APP_LED_1StateGet();//BSP_LEDStateGet(APP_LED_1);
            }
            else
            {
                if(*snmp_version == 1)
                    return 1;
            }
            break;
            
        case TRAP_COMMUNITY:
        {
            uint8_t *asciiStr = malloc(20);
            val->dword = (uint32_t)asciiStr;
            memset(asciiStr, 0, 20);
            strcpy(asciiStr, "ascii_str_trap");

            if((snmp_version != 0) && (*snmp_version == 1)) 
            {   
                uint32_t led1 = APP_LED_1StateGet();
                uint32_t led3 = APP_LED_3StateGet();

                if((led1 == 0U) || (led3 == 0U)) 
                {
                    return 1;
                }
            }            
            
            break;
        }
        
        default:
            return 1;
    };
    
    return 0;
}

static int SNMPv2_LLDP_TrapParamGet(SNMP_ID var, SNMP_VAL *val, void *data)
{
    if(var == LLDP_MSG_INTRVL)
    {
        val->dword = g_lldpMsgIntrvl;
    }
    
    return 0;
}

bool TCPIP_SNMP_SendTrapToSelctedInterface(size_t *netIx,TCPIP_NET_HANDLE netIf)
{
    netIf = TCPIP_SNMP_ClientGetNet(netIx,netIf);
    if(netIf == NULL)
    {
        netIx = NULL;
        return false;
    }
    if(!TCPIP_SNMP_ValidateTrapIntf(netIf))
    {
        return false;
    }
    TCPIP_SNMP_TrapInterFaceSet(netIf);
    return true;
}

#ifdef TCPIP_STACK_USE_IPV6
/****************************************************************************
Function:
     bool TCPIP_SNMP_IPv6_NotificationSend(uint8_t receiverIndex, SNMP_ID var, SNMP_VAL val,uint8_t targetIndex)

Summary:
    Prepare, validate remote node which will receive trap and send trap pdu.

Description:
    This routine prepares the trap notification pdu, sends ARP and get
    remote device MAC address to which notification to sent, sends
    the notification.
    Notofication state machine is getting updated if there is any ARP resolution failure for
    a perticular trap destination address.

PreCondition:
    SNMPTrapDemo() is called.

parameters:
     receiverIndex - The index to array where remote ip address is stored.
     var           - SNMP var ID that is to be used in notification
     val           - Value of var. Only value of uint8_t, uint16_t or uint32_t can be sent.

Return Values:
     true   -   If notification send is successful.
     false  -   If send notification failed.

Remarks:
     None.
 *************************************************************************/
static bool TCPIP_SNMP_IPv6_NotificationSend(uint8_t mibIndex, uint8_t receiverIndex, SNMP_ID var, SNMP_VAL val,uint8_t targetIndex)
{
    static enum { SM_PREPARE, SM_NOTIFY_WAIT } smState = SM_PREPARE;
    IP_MULTI_ADDRESS ipv6Address;
    static uint8_t tempRxIndex;
    static IPV6_ADDR tempIpv6Address;
    bool        retVal;
    uint8_t     specTrap = 0U;
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
    uint8_t     snmpv3MsgModelType;
    uint8_t     snmpv3SecModelType;
#endif

    ipv6TrapInfo[mibIndex].Size = TCPIP_SNMP_TRAP_TABLE_SIZE;

    // Convert local to network order.
    (void)memcpy(&ipv6Address.v6Add,&ipv6TrapInfo[mibIndex].table[receiverIndex].IPv6Address,sizeof(IPV6_ADDR));

    if(gtrapSMStateUpdate == true)
    {
        smState = SM_PREPARE;
        gtrapSMStateUpdate = false;
    }

    retVal = false;
    switch(smState)
    {
        case SM_PREPARE:
            gSendTrapSMstate = (uint8_t)smState;
            tempRxIndex=receiverIndex;
            // Convert local to network order.
            (void)memcpy(&tempIpv6Address,&ipv6TrapInfo[mibIndex].table[receiverIndex].IPv6Address,16);
            // get the specific SNMP trap notification type
            TCPIP_SNMP_TrapSpecificNotificationGet(&specTrap);
            TCPIP_SNMP_NotifyPrepare(NULL,
                  ipv6TrapInfo[mibIndex].table[receiverIndex].community,
                  ipv6TrapInfo[mibIndex].table[receiverIndex].communityLen,
                  MICROCHIP,  // Agent ID Var
                  specTrap,   // Specifc Trap notification code
                  SNMPGetTimeStamp());

            smState = SM_NOTIFY_WAIT;

            break;

        case SM_NOTIFY_WAIT:
            gSendTrapSMstate = (uint8_t)smState;
            if ( TCPIP_SNMP_NotifyIsReady(&ipv6Address,IPV6_SNMP_TRAP) )
            {
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
                if(TCPIP_SNMPV3_TrapTypeGet())
                {
                    if(targetIndex == (TCPIP_SNMPV3_USM_MAX_USER-1))
                    {
                        smState = SM_PREPARE;
                    }
                    // Get SNMPv3 messsage model type and security model type
                    TCPIP_SNMPv3_TrapConfigDataGet(targetIndex,&snmpv3MsgModelType,&snmpv3SecModelType);
                    if((snmpv3MsgModelType == SNMPV3_MSG_PROCESSING_MODEL) && (snmpv3SecModelType == SNMPV3_USM_SECURITY_MODEL))
                    {
                        retVal = TCPIP_SNMPv3_Notify(var, val, 0,targetIndex,IPV6_SNMP_TRAP);
                    }
                    else if((snmpv3MsgModelType == SNMPV2C_MSG_PROCESSING_MODEL) && (snmpv3SecModelType == SNMPV2C_SECURITY_MODEL))
                    {
                        retVal = TCPIP_SNMP_TRAPv2Notify(var, val, 0,IPV6_SNMP_TRAP);
                    }
                    else if((snmpv3MsgModelType == SNMPV1_MSG_PROCESSING_MODEL) && (snmpv3SecModelType == SNMPV1_SECURITY_MODEL))
                    {
                        retVal = TCPIP_SNMP_TRAPv1Notify(var, val, 0,IPV6_SNMP_TRAP);
                    }
                    else
                    {
                        retVal = true;
                        break;
                    }
                }
                else
#endif
                if(TCPIP_SNMP_TRAPTypeGet())
                {
                    smState = SM_PREPARE;
                    retVal = TCPIP_SNMP_TRAPv2Notify(var, val, 0,IPV6_SNMP_TRAP);
                }
                else
                {
                    smState = SM_PREPARE;
                    retVal = TCPIP_SNMP_TRAPv1Notify(var, val, 0,IPV6_SNMP_TRAP);
                }
                break;
            }
            /* if trapInfo table address for a particular index is different comparing to the SM_PREPARE IP address
                    then change the state to SM_PREPARE*/
            if((memcmp(tempIpv6Address.v, ipv6Address.v6Add.v,16) != 0) && (tempRxIndex == receiverIndex))
            {
                smState = SM_PREPARE;
            }
            /* Change state machine from SM_NOTIFY_WAIT to SM_PREPARE if incoming trap destination
            index is different from the SM_PREPARE  trap destination index*/
            if(tempRxIndex != receiverIndex)
            {
                smState=SM_PREPARE;
            }
            break;

        default:
            // do nothing
            break;
    }

    return retVal;
}
#endif

/****************************************************************************
  Function:
     bool SNMPSendNotification(uint8_t mibIndex, uint8_t receiverIndex, SNMP_ID var, SNMP_VAL val,uint8_t targetIndex)

  Summary:
    Prepare, validate remote node which will receive trap and send trap pdu.

  Description:
    This routine prepares the trap notification pdu, sends ARP and get
    remote device MAC address to which notification to sent, sends
    the notification.
    Notofication state machine is getting updated if there is any ARP resolution failure for
    a perticular trap destination address.

  PreCondition:
    SNMPTrapDemo() is called.

  parameters:
     receiverIndex - The index to array where remote ip address is stored.
     var           - SNMP var ID that is to be used in notification
     val           - Value of var. Only value of uint8_t, uint16_t or uint32_t can be sent.

  Return Values:
     true   -   If notification send is successful.
     false  -   If send notification failed.

  Remarks:
     None.
 ***** */
static bool SNMPSendNotification(uint8_t mibIndex, uint8_t receiverIndex, SNMP_ID var, SNMP_VAL val,uint8_t targetIndex)
{
    static enum { SM_PREPARE, SM_NOTIFY_WAIT } smState = SM_PREPARE;
    static IP_MULTI_ADDRESS IPAddress;//IPV4_ADDR IPAddress;
    static uint8_t tempRxIndex;
    static IPV4_ADDR tempIpAddress;
    bool        retVal;
    uint8_t     specTrap = 0U;
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
    uint8_t     snmpv3MsgModelType;
    uint8_t     snmpv3SecModelType;
#endif

    trapInfo[mibIndex].Size = TCPIP_SNMP_TRAP_TABLE_SIZE;

    // Convert local to network order.
    IPAddress.v4Add.v[0] = trapInfo[mibIndex].table[receiverIndex].IPAddress.v[3];
    IPAddress.v4Add.v[1] = trapInfo[mibIndex].table[receiverIndex].IPAddress.v[2];
    IPAddress.v4Add.v[2] = trapInfo[mibIndex].table[receiverIndex].IPAddress.v[1];
    IPAddress.v4Add.v[3] = trapInfo[mibIndex].table[receiverIndex].IPAddress.v[0];

    if(gtrapSMStateUpdate == true)
    {
        smState = SM_PREPARE;
        gtrapSMStateUpdate = false;
    }

    retVal = false;
    switch(smState)
    {
        case SM_PREPARE:
            gSendTrapSMstate = (uint8_t)smState;
            tempRxIndex=receiverIndex;
            // Convert local to network order.
            tempIpAddress.v[0] = trapInfo[mibIndex].table[receiverIndex].IPAddress.v[3];
            tempIpAddress.v[1] = trapInfo[mibIndex].table[receiverIndex].IPAddress.v[2];
            tempIpAddress.v[2] = trapInfo[mibIndex].table[receiverIndex].IPAddress.v[1];
            tempIpAddress.v[3] = trapInfo[mibIndex].table[receiverIndex].IPAddress.v[0];
            // get the specific SNMP trap notification type
            TCPIP_SNMP_TrapSpecificNotificationGet(&specTrap);
            TCPIP_SNMP_NotifyPrepare(&IPAddress,
                    trapInfo[mibIndex].table[receiverIndex].community,
                    trapInfo[mibIndex].table[receiverIndex].communityLen,
                    MICROCHIP,                   // Agent ID Var
                    specTrap,   // Specifc Trap notification code
                    SNMPGetTimeStamp());

            smState = SM_NOTIFY_WAIT;

            break;

        case SM_NOTIFY_WAIT:
            gSendTrapSMstate = (uint8_t)smState;
            if ( TCPIP_SNMP_NotifyIsReady(&IPAddress,IPV4_SNMP_TRAP) )
            {
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
                if(TCPIP_SNMPV3_TrapTypeGet())
                {
                    if(targetIndex == (TCPIP_SNMPV3_USM_MAX_USER-1))
                    {
                        smState = SM_PREPARE;
                    }
                    // Get SNMPv3 messsage model type and security model type
                    TCPIP_SNMPv3_TrapConfigDataGet(targetIndex,&snmpv3MsgModelType,&snmpv3SecModelType);
                    if((snmpv3MsgModelType == SNMPV3_MSG_PROCESSING_MODEL) && (snmpv3SecModelType == SNMPV3_USM_SECURITY_MODEL))
                    {
                        retVal = TCPIP_SNMPv3_Notify(var, val, 0,targetIndex,IPV4_SNMP_TRAP);
                    }
                    else if((snmpv3MsgModelType == SNMPV2C_MSG_PROCESSING_MODEL) && (snmpv3SecModelType == SNMPV2C_SECURITY_MODEL))
                    {
                        retVal = TCPIP_SNMP_TRAPv2Notify(var, val, 0,IPV4_SNMP_TRAP);
                    }
                    else if((snmpv3MsgModelType == SNMPV1_MSG_PROCESSING_MODEL) && (snmpv3SecModelType == SNMPV1_SECURITY_MODEL))
                    {
                        retVal = TCPIP_SNMP_TRAPv1Notify(var, val, 0,IPV4_SNMP_TRAP);
                    }
                    else
                    {
                        retVal = true;
                        break;

                    }
                }
                else
#endif
                    if(TCPIP_SNMP_TRAPTypeGet())
                    {
                        smState = SM_PREPARE;
                        retVal = TCPIP_SNMP_TRAPv2Notify(var, val, 0,IPV4_SNMP_TRAP);
                    }
                    else
                    {
                        smState = SM_PREPARE;
                        retVal = TCPIP_SNMP_TRAPv1Notify(var, val, 0,IPV4_SNMP_TRAP);
                    }
                break;
            }
            /* if trapInfo table address for a particular index is different comparing to the SM_PREPARE IP address
               then change the state to SM_PREPARE*/
            if((tempIpAddress.Val != IPAddress.v4Add.Val) &&
                    (tempRxIndex == receiverIndex))
            {
                smState = SM_PREPARE;
            }
            /* Change state machine from SM_NOTIFY_WAIT to SM_PREPARE if incoming trap destination
               index is different from the SM_PREPARE  trap destination index*/
            if(tempRxIndex != receiverIndex)
            {
                smState=SM_PREPARE;
            }
            break;

        default:
            // do nothing
            break;
    }

    return retVal;
}

int SNMPv3_SendTrap(uint8_t mibIndex, SNMP_TRAP_INFO *pTrapInfo, 
                    SNMP_TRAP_IP_ADDRESS_TYPE ipAddresstype, uint8_t trapIndex, uint8_t targetIndex)
{
    SNMP_TRAP_PARAM_INFO    *pTrapParamInfo = pTrapInfo->TrapParamsInfo;
    uint8_t     snmpv3MsgModelType;
    uint8_t     snmpv3SecModelType;
    bool        retVal = false;

    TCPIP_SNMP_TrapSendFlagSet(true);

    int trap_param_cnt = pTrapParamInfo->Size;
    int i = 0;
    while(i < trap_param_cnt)
    {
        SNMP_VAL    val;
        SNMP_ID     var;
        uint32_t    ret_val = 0; 

        if( i == (trap_param_cnt - 1))
        {
            TCPIP_SNMP_TrapSendFlagSet(false);
        }

        memset(&val, 0, sizeof(val));                        
        var = pTrapParamInfo->table[i].var;
        ret_val = pTrapParamInfo->table[i].snmp_trap_param_fn(var, &val, NULL);
        if(ret_val != 0)
        {
            i++;
            continue;
        }

        if( i == 0)
        {
            if(ipAddresstype == IPV4_SNMP_TRAP)
            {
                retVal =SNMPSendNotification(mibIndex, trapIndex, var, val, targetIndex);
                if((gSendTrapSMstate == 0x0U) && (retVal == false)) // gSendTrapSMstate == SM_PREPARE
                {
                    retVal = SNMPSendNotification(mibIndex, trapIndex, var, val,targetIndex);
                }
            }
#ifdef TCPIP_STACK_USE_IPV6
            else
            {
                retVal =TCPIP_SNMP_IPv6_NotificationSend(mibIndex, trapIndex,var,val,targetIndex);
                if((gSendTrapSMstate == 0x0U) && (retVal == false)) // gSendTrapSMstate == SM_PREPARE
                {
                    retVal = TCPIP_SNMP_IPv6_NotificationSend(mibIndex, trapIndex, var, val,targetIndex);
                }
            }
#endif      /* #ifdef TCPIP_STACK_USE_IPV6 */
            if(retVal == false)
            {
                return 1;
            }                                
        }
        else
        {
            // Not the first element of the list
            // Get SNMPv3 messsage model type and security model type
            TCPIP_SNMPv3_TrapConfigDataGet(targetIndex,&snmpv3MsgModelType,&snmpv3SecModelType);
            if((snmpv3MsgModelType == SNMPV3_MSG_PROCESSING_MODEL)
                && (snmpv3SecModelType == SNMPV3_USM_SECURITY_MODEL))
            {
                (void)TCPIP_SNMPv3_Notify(var, val, 0, targetIndex, ipAddresstype);
            }
            else if((snmpv3MsgModelType == SNMPV2C_MSG_PROCESSING_MODEL)
                && (snmpv3SecModelType == SNMPV2C_SECURITY_MODEL))
            {
                (void)TCPIP_SNMP_TRAPv2Notify(var, val, 0, ipAddresstype);
            }
            else if((snmpv3MsgModelType == SNMPV1_MSG_PROCESSING_MODEL)
                && (snmpv3SecModelType == SNMPV1_SECURITY_MODEL))
            {
                (void)TCPIP_SNMP_TRAPv1Notify(var, val, 0, ipAddresstype);
            }
            else
            {
                //do nothing
            }
        }
        i++;
    };     
    
    return 0;
}

int SNMPv2_SendTrap(uint8_t mibIndex, SNMP_TRAP_INFO *pTrapInfo, 
                    SNMP_TRAP_IP_ADDRESS_TYPE ipAddresstype, uint8_t trapIndex, uint8_t targetIndex)
{
    SNMP_TRAP_PARAM_INFO    *pTrapParamInfo = pTrapInfo->TrapParamsInfo;
    bool        retVal = false;
    int trap_param_cnt = pTrapParamInfo->Size;
    int i = 0;

    TCPIP_SNMP_TrapSendFlagSet(true);

    while(i < trap_param_cnt)
    {
        SNMP_VAL    val;
        SNMP_ID     var;
        uint32_t    ret_val = 0; 

        if( i == (trap_param_cnt - 1))
        {
            TCPIP_SNMP_TrapSendFlagSet(false);
        }

        memset(&val, 0, sizeof(val));                        
        var = pTrapParamInfo->table[i].var;
        ret_val = pTrapParamInfo->table[i].snmp_trap_param_fn(var, &val, NULL);
        if(ret_val != 0)
        {
            i++;
            continue;
        }

        if( i == 0)
        {
            if(ipAddresstype == IPV4_SNMP_TRAP)
            {
                retVal =SNMPSendNotification(mibIndex, trapIndex, var, val, targetIndex);
                if((gSendTrapSMstate == 0x0U) && (retVal == false)) // gSendTrapSMstate == SM_PREPARE
                {
                    retVal = SNMPSendNotification(mibIndex, trapIndex, var, val,targetIndex);
                }
            }
#ifdef TCPIP_STACK_USE_IPV6
            else
            {
                retVal =TCPIP_SNMP_IPv6_NotificationSend(mibIndex, trapIndex,var,val,targetIndex);
                if((gSendTrapSMstate == 0x0U) && (retVal == false)) // gSendTrapSMstate == SM_PREPARE
                {
                    retVal = TCPIP_SNMP_IPv6_NotificationSend(mibIndex, trapIndex, var, val,targetIndex);
                }
            }
#endif      /* #ifdef TCPIP_STACK_USE_IPV6 */
            if(retVal == false)
            {
                return 1;
            }                                
        }
        else
        {
            // Not the first element of the list
            (void)TCPIP_SNMP_TRAPv2Notify(var, val, 0, ipAddresstype);
        }
        i++;
    };     
    
    return 0;
}

int SNMPv1_SendTrap(uint8_t mibIndex, SNMP_TRAP_INFO *pTrapInfo, 
                    SNMP_TRAP_IP_ADDRESS_TYPE ipAddresstype, uint8_t trapIndex, uint8_t targetIndex)
{
    SNMP_TRAP_PARAM_INFO    *pTrapParamInfo = pTrapInfo->TrapParamsInfo;
    bool        retVal = false;
    int trap_param_cnt = pTrapParamInfo->Size;
    int i = 0;
    int snmp_version = 1;

    while(i < trap_param_cnt)
    {
        SNMP_VAL    val;
        SNMP_ID     var;
        uint32_t    ret_val = 0; 

        memset(&val, 0, sizeof(val));                        
        var = pTrapParamInfo->table[i].var;
        ret_val = pTrapParamInfo->table[i].snmp_trap_param_fn(var, &val, &snmp_version);
        if(ret_val != 0)
        {
            i++;
            continue;
        }

        TCPIP_SNMP_TrapSpecificNotificationSet((uint8_t)pTrapParamInfo->table[i].specTrap,(uint8_t)ENTERPRISE_SPECIFIC,SNMP_DEMO_TRAP);	
        TCPIP_SNMP_TrapSendFlagSet(false);

        if(ipAddresstype == IPV4_SNMP_TRAP)
        {
            retVal =SNMPSendNotification(mibIndex, trapIndex, var, val, targetIndex);
            if((gSendTrapSMstate == 0x0U) && (retVal == false)) // gSendTrapSMstate == SM_PREPARE
            {
                retVal = SNMPSendNotification(mibIndex, trapIndex, var, val,targetIndex);
            }
        }
#ifdef TCPIP_STACK_USE_IPV6
        else
        {
            retVal =TCPIP_SNMP_IPv6_NotificationSend(mibIndex, trapIndex,var,val,targetIndex);
            if((gSendTrapSMstate == 0x0U) && (retVal == false)) // gSendTrapSMstate == SM_PREPARE
            {
                retVal = TCPIP_SNMP_IPv6_NotificationSend(mibIndex, trapIndex, var, val,targetIndex);
            }
        }
#endif      /* #ifdef TCPIP_STACK_USE_IPV6 */
        if(retVal == false)
        {
            return 1;
        }                                

        i++;
    };     
    
    return 0;
}

/**************************************************************************
  Function:
    void SNMPV2TrapDemo(void)

  Summary:
    Send SNMP V2 notification with multiple varbinds.

  Description:
    This routine sends a trap v2 pdu with multiple varbind variables
    for the predefined ip addresses with the agent. And as per RFC1905
    the first two variable bindings in the varbind pdu list of an
    SNMPv2-Trap-PDU are sysUpTime.0 and snmpTrapOID.0 respectively.
    To support multiple varbind, user need to call SNMPSendNotification()
    for the first varbind variable and SNMPSendNotification() will do the
    arp resolve and adds sysUpTime.0 and snmpTrapOID.0 variable to
    the pdu. For the second varbind variable onwards user need to
    call only TCPIP_SNMP_Notify().
    In this demo , snmpv2 trap includes ANALOG_POT0,PUSH_BUTTON and LED_D5
    variable bindings and TrapCommunity variable is being used as part of the fourth varbind of
    the TRAP PDU which is ASCII string format.
    and this trap can be generated by using Analog portmeter value.
    and SNMPv2-Trap-PDU will be generated only when pot meter reading exceeds 12.

    gSetTrapSendFlag Should be set to true when user is trying to send first
    variable binding and gSetTrapSendFlag should be set to false before
    sending the last variable binding.

    * if user is sending only one variable binding then
    * gSetTrapSendFlag should be set to False.
    * user can add more variable bindings.
    * For ASCII STR trap , argument VAL contains the pointer address of the string variable.
  PreCondition:
    Application defined event occurs to send the trap.

  parameters:
     None.

  Returns:
     None.

  Remarks:
    This routine guides how to build a event generated trap notification.
 ***** */
void SNMPV2TrapDemo(void)
{
    static uint32_t tempTimerRead=0;
    static uint8_t  trapIndex[SNMP_MAX_MIBS_SUPPORTED] = {0};
    static SNMP_VAL     analogPotVal;
    static bool potReadLock = false;
    static bool timeLock[SNMP_MAX_MIBS_SUPPORTED] = {false};
    uint8_t     targetIndex[SNMP_MAX_MIBS_SUPPORTED] = {0};
    bool        retVal = false;
    static bool netStartForTrap[SNMP_MAX_MIBS_SUPPORTED] = {true};
    static SNMP_TRAP_IP_ADDRESS_TYPE snmpTrapIpAddresstype = IPV4_SNMP_TRAP;
    //UDP_SOCKET socket = INVALID_UDP_SOCKET;
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
    static uint8_t userIndex=0;
    uint8_t     snmpv3MsgModelType;
    uint8_t     snmpv3SecModelType;
#endif
    static size_t     netIx=0;
    static TCPIP_NET_HANDLE netIf=NULL;

    for (int mibCnt = 0; mibCnt < SNMP_MAX_MIBS_SUPPORTED; mibCnt++)
    {
        SNMP_TRAP_INFO  *pTrapInfo = &trapInfo[mibCnt]; 
        pTrapInfo->Size = TCPIP_SNMP_TRAP_TABLE_SIZE;
#ifdef TCPIP_STACK_USE_IPV6
        ipv6TrapInfo[mibCnt].Size = TCPIP_SNMP_TRAP_TABLE_SIZE;
#endif
    
        pTrapInfo->TrapParamsInfo = g_mibTrapParamInfo[mibCnt];
        if(timeLock[mibCnt]==false)
    {
        tempTimerRead = SYS_TMR_TickCountGet();
            timeLock[mibCnt]=true;
    }
	if(!TCPIP_SNMP_IsTrapEnabled())
    {
        return;
    }

    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet();

        while(trapIndex[mibCnt] < TCPIP_SNMP_TRAP_TABLE_SIZE)
    {
        if(snmpTrapIpAddresstype == IPV4_SNMP_TRAP)
        {
                if(pTrapInfo->table[trapIndex[mibCnt]].Flags.bEnabled == 0U)
            {
                    trapIndex[mibCnt]++;
                continue;
            }
        }
#ifdef TCPIP_STACK_USE_IPV6
        else
        {
                if(ipv6TrapInfo[mibCnt].table[trapIndex[mibCnt]].Flags.bEnabled == 0U)
            {
                    trapIndex[mibCnt]++;
                continue;
            }
        }
#endif
            if(netStartForTrap[mibCnt])
        {
            // select and set the trap interface
            if(!TCPIP_SNMP_SendTrapToSelctedInterface(&netIx,netIf))
            {
                    #if (SNMP_DEBUG != 0)
                    SYS_CONSOLE_PRINT("\r\n %s(%d): mibCnt = %d ; trapIndex= %d", __FUNCTION__, __LINE__, mibCnt, trapIndex[mibCnt]);
                    #endif
                    trapIndex[mibCnt]++;
                continue;
            }
                /* Specify SNMPV2 specific trap ID Here. Which will help Ireasoning and other SNMP manager tools to
recognise the trap information and it will help the SNMP manager tool to decrypt the
trap information.
                This ID is only related to trap ID. and this implementaion is only for TRAPv2 specific. */
                TCPIP_SNMP_TrapSpecificNotificationSet(1U,(uint8_t)ENTERPRISE_SPECIFIC,pTrapInfo->TrapParamsInfo->trap_id);
                netStartForTrap[mibCnt] = false;
        }

            /*
             * prepare  and send multivarbind pdu using pot meter value.
             * for SNMP v2 trap sysUpTime.0 and SNMPv2TrapOID.0 are mandatory
             * apart from these varbinds, push button and potmeter OID are included
             * to this pdu.
            */
            // insert ANALOG_POT0 OID value and OID to the varbind pdu
            //set global flag gSetTrapSendFlag to true , it signifies that there are more than one
            // variable need to be the part of SNMP v2 TRAP.
            // if there is  only varbind variable to be the part of SNMP v2 trap,
            // then user should set gSetTrapSendFlag to false.
            //snmpStkDcptMemStubPtr->gSetTrapSendFlag = false;
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
            if(TCPIP_SNMPV3_TrapTypeGet())
            {
                for(targetIndex[mibCnt]=userIndex;targetIndex[mibCnt]<TCPIP_SNMPV3_USM_MAX_USER;targetIndex[mibCnt]++)
                {
                    int ret; 
                    ret = SNMPv3_SendTrap(mibCnt, pTrapInfo, snmpTrapIpAddresstype, 
                            trapIndex[mibCnt], targetIndex[mibCnt]);
                    if(ret != 0)
                    {
                                userIndex = targetIndex[mibCnt];
                        if((SYS_TMR_TickCountGet() - TCPIP_SNMP_TrapTimeGet()) >= (1U * sysFreq))
                        {
                                    trapIndex[mibCnt]++;
                            gtrapSMStateUpdate = true;
                        }
                        return;
                    }
                        }
                if(userIndex == TCPIP_SNMPV3_USM_MAX_USER-1)
                {
                    userIndex = 0;
                }
            }
			else
#endif
            if(TCPIP_SNMP_TRAPTypeGet())
            {
                int ret; 
                ret = SNMPv2_SendTrap(mibCnt, pTrapInfo, snmpTrapIpAddresstype, 
                        trapIndex[mibCnt], targetIndex[mibCnt]);
                if(ret != 0)
                {
                            userIndex = targetIndex[mibCnt];
                    if((SYS_TMR_TickCountGet() - TCPIP_SNMP_TrapTimeGet()) >= (1U * sysFreq))
                    {
                                trapIndex[mibCnt]++;
                        gtrapSMStateUpdate = true;
                    }
                    return;
                }
                    }
                    else
                {
                // do nothing
            }
            
            trapIndex[mibCnt]++;
    }

        //Try for max 10 seconds to send TRAP, do not get block in while()
        if((SYS_TMR_TickCountGet() - tempTimerRead) >= (10U * sysFreq))
    {
            timeLock[mibCnt] = false;
#ifdef TCPIP_STACK_USE_IPV6
        if(snmpTrapIpAddresstype == IPV4_SNMP_TRAP)
        {
            snmpTrapIpAddresstype=IPV6_SNMP_TRAP;
        }
        else if(snmpTrapIpAddresstype == IPV6_SNMP_TRAP)
        {
            snmpTrapIpAddresstype=IPV4_SNMP_TRAP;
        }
        else
        {
            // do nothing
        }
#endif
            netStartForTrap[mibCnt] = true;
            trapIndex[mibCnt] = 0;
        analogPotVal.word = 0;
        gSendTrapSMstate = 0U;
    }
}
}

/**************************************************************************
  Function:
    void SNMPTrapDemo(void)

  Summary:
    Send trap pdu demo application.

  Description:
    This routine is used to send various trap events for the predefined ip addresses with the
    agent. Events like ANALOG POT value, PUSH_BUTTON and ASCII Str notification.
    ANALOG POT event -  When there is a POT value greater than 12 and for this ANALOG_POT0
    is used as a TRAP PDU variable for this event.
    PUSH BUTTON event -  BUTTON2_IO is the varaible used for this trap event.
    ASCII STR event - TRAP_COMMUNITY variable is used for this event and this event will occur when
    both LED1 and LED2 are blinking.
  PreCondition:
    Application defined event occurs to send the trap.

  parameters:
     None.

  Returns:
     None.

  Remarks:
    This routine guides how to build a event generated trap notification.
    The application should make use of TCPIP_SNMP_SendFailureTrap() routine to generate
    and send trap.
 ***** */
void SNMPTrapDemo(void)
{
    static uint32_t tempTimerRead=0;
    static bool timeLock[SNMP_MAX_MIBS_SUPPORTED] = {false};
    uint8_t     targetIndex[SNMP_MAX_MIBS_SUPPORTED] = {0};
    static uint8_t  trapIndex[SNMP_MAX_MIBS_SUPPORTED]={0};
    static size_t     netIx=0;
    static TCPIP_NET_HANDLE netIf;
    static bool netStartForTrap[SNMP_MAX_MIBS_SUPPORTED] = {true};
    uint32_t sysFreq;

    for (int mibCnt = 0; mibCnt < SNMP_MAX_MIBS_SUPPORTED; mibCnt++)
    {
        SNMP_TRAP_INFO  *pTrapInfo = &trapInfo[mibCnt]; 

        pTrapInfo->Size = TCPIP_SNMP_TRAP_TABLE_SIZE;
        pTrapInfo->TrapParamsInfo = g_mibTrapParamInfo[mibCnt];

        if(timeLock[mibCnt]==false)
    {
            tempTimerRead = SYS_TMR_TickCountGet();
            timeLock[mibCnt]=true;
    }

	if(!TCPIP_SNMP_IsTrapEnabled())
    {
        return;
    }

    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet();

        while(trapIndex[mibCnt] < TCPIP_SNMP_TRAP_TABLE_SIZE)
    {
            if(pTrapInfo->table[trapIndex[mibCnt]].Flags.bEnabled == 0U)
    {
                #if (SNMP_DEBUG != 0)
                SYS_CONSOLE_PRINT("\r\n %s(%d): mibCnt = %d ; trapIndex= %d ; Flags.bEnabled == 0", __FUNCTION__, __LINE__, mibCnt, trapIndex[mibCnt]);
                #endif
                trapIndex[mibCnt]++;
                continue;
        }
            if(netStartForTrap[mibCnt])
            {
                // select and set the trap interface
                if(!TCPIP_SNMP_SendTrapToSelctedInterface(&netIx,netIf))
                {
                    #if (SNMP_DEBUG != 0)
                    SYS_CONSOLE_PRINT("\r\n %s(%d): mibCnt = %d ; trapIndex= %d", __FUNCTION__, __LINE__, mibCnt, trapIndex[mibCnt]);
                    #endif
                    trapIndex[mibCnt]++;
                    continue;
                }
                /* Specify SNMPV2 specific trap ID Here. Which will help Ireasoning and other SNMP manager tools to
recognise the trap information and it will help the SNMP manager tool to decrypt the
trap information.
                This ID is only related to trap ID. and this implementaion is only for TRAPv2 specific. */
                TCPIP_SNMP_TrapSpecificNotificationSet(1U,(uint8_t)ENTERPRISE_SPECIFIC,pTrapInfo->TrapParamsInfo->trap_id);
                netStartForTrap[mibCnt] = false;
            }

                /*
             * prepare  and send multivarbind pdu using pot meter value.
             * for SNMP v2 trap sysUpTime.0 and SNMPv2TrapOID.0 are mandatory
             * apart from these varbinds, push button and potmeter OID are included
             * to this pdu.
    */
            // insert ANALOG_POT0 OID value and OID to the varbind pdu
            //set global flag gSetTrapSendFlag to true , it signifies that there are more than one
            // variable need to be the part of SNMP v2 TRAP.
            // if there is  only varbind variable to be the part of SNMP v2 trap,
            // then user should set gSetTrapSendFlag to false.
            //snmpStkDcptMemStubPtr->gSetTrapSendFlag = false;
            int ret; 
            ret = SNMPv1_SendTrap(mibCnt, pTrapInfo, IPV4_SNMP_TRAP, 
                    trapIndex[mibCnt], targetIndex[mibCnt]);
            if(ret != 0)
                    {
                        if((SYS_TMR_TickCountGet() - TCPIP_SNMP_TrapTimeGet()) >= (2U * sysFreq))
                        {
                    trapIndex[mibCnt]++;
                            gtrapSMStateUpdate = true;
                        }
                        return ;
                    }
			
            trapIndex[mibCnt]++;
    }

        //Try for max 10 seconds to send TRAP, do not get block in while()
        if((SYS_TMR_TickCountGet() - tempTimerRead) >= (5U * sysFreq))
    {
            timeLock[mibCnt] = false;
            netStartForTrap[mibCnt] = true;
            trapIndex[mibCnt] = 0;
        gSendTrapSMstate = 0U;
    }
}
}

void TCPIP_SNMP_SendFailureTrap(void)
{
    static bool timeLock=false;
    static uint8_t receiverIndex=0; ///is application specific
    static IP_MULTI_ADDRESS remHostIPAddress;
    IP_MULTI_ADDRESS* remHostIpAddrPtr;
    SNMP_VAL val;
    static uint32_t TimerRead;
    uint8_t   specTrap = 0U;
    uint32_t    mibID=0;
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
    static uint8_t userIndex=0;
    uint8_t     snmpv3MsgModelType;
    uint8_t     snmpv3SecModelType;
#endif
    static enum
    {
        SM_PREPARE,
        SM_NOTIFY_WAIT
    } smState = SM_PREPARE;
    static size_t     netIx=0;
    static TCPIP_NET_HANDLE netIf;
    static bool netStartForTrap = true;

    trapInfo[SNMP_MIB_II_INDX].Size = TCPIP_SNMP_TRAP_TABLE_SIZE;
    if(trapInfo[SNMP_MIB_II_INDX].table[receiverIndex].Flags.bEnabled != 0U)
    {
        remHostIPAddress.v4Add.v[0] = trapInfo[SNMP_MIB_II_INDX].table[receiverIndex].IPAddress.v[3];
        remHostIPAddress.v4Add.v[1] = trapInfo[SNMP_MIB_II_INDX].table[receiverIndex].IPAddress.v[2];
        remHostIPAddress.v4Add.v[2] = trapInfo[SNMP_MIB_II_INDX].table[receiverIndex].IPAddress.v[1];
        remHostIPAddress.v4Add.v[3] = trapInfo[SNMP_MIB_II_INDX].table[receiverIndex].IPAddress.v[0];
        remHostIpAddrPtr = &remHostIPAddress;
        if(timeLock==false)
        {
            TimerRead= SYS_TMR_TickCountGet();
            timeLock=true;
        }
    }
    else
    {
        receiverIndex++;
        if((receiverIndex == (uint8_t)TCPIP_SNMP_TRAP_TABLE_SIZE))
        {
            receiverIndex=0;
            timeLock=false;
            TCPIP_SNMP_AuthTrapFlagSet(false);
        }
        return;
    }

    if(netStartForTrap)
    {
        // select and set the trap interface
        if(!TCPIP_SNMP_SendTrapToSelctedInterface(&netIx,netIf))
        {
            return;
        }
        /*
Specify SNMPV2 specific trap ID Here. Which will help Ireasoning and other SNMP manager tools to
recognise the trap information and it will help the SNMP manager tool to decrypt the
trap information.

This ID is only related to trap ID. and this implementaion is only for TRAPv2 specific.
*/
        TCPIP_SNMP_TrapSpecificNotificationSet(1U,(uint8_t)AUTH_FAILURE,SNMP_DEMO_TRAP);
        netStartForTrap = false;
    }

    switch(smState)
    {
        case SM_PREPARE:
            TCPIP_SNMP_TrapSpecificNotificationGet(&specTrap);
            TCPIP_SNMP_NotifyPrepare(remHostIpAddrPtr,trapInfo[SNMP_MIB_II_INDX].table[receiverIndex].community,
                    trapInfo[SNMP_MIB_II_INDX].table[receiverIndex].communityLen,
                    MICROCHIP,            // Agent ID Var
                    specTrap,  // Notification code.
                    SNMPGetTimeStamp());
            smState++;
            break;

        case SM_NOTIFY_WAIT:
            if(TCPIP_SNMP_NotifyIsReady(remHostIpAddrPtr,IPV4_SNMP_TRAP))
            {
                smState = SM_PREPARE;
                val.byte = 0;
                receiverIndex++;
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
                if(TCPIP_SNMPV3_TrapTypeGet())
                {
                    uint8_t  targetIndex = 0;
                    for(targetIndex=userIndex;targetIndex<TCPIP_SNMPV3_USM_MAX_USER;targetIndex++)
                    {
                        // Get SNMPv3 messsage model type and security model type
                        TCPIP_SNMPv3_TrapConfigDataGet(targetIndex,&snmpv3MsgModelType,&snmpv3SecModelType);
                        TCPIP_SNMP_TRAPMibIDGet(&mibID);
                        if((snmpv3MsgModelType == SNMPV3_MSG_PROCESSING_MODEL) && (snmpv3SecModelType == SNMPV3_USM_SECURITY_MODEL))
                        {
                            (void)TCPIP_SNMPv3_Notify(mibID, val, 0,targetIndex,IPV4_SNMP_TRAP);
                        }
                        else if((snmpv3MsgModelType == SNMPV2C_MSG_PROCESSING_MODEL) && (snmpv3SecModelType == SNMPV2C_SECURITY_MODEL))
                        {
                            (void)TCPIP_SNMP_TRAPv2Notify(mibID, val, 0,IPV4_SNMP_TRAP);
                        }
                        else if((snmpv3MsgModelType == SNMPV1_MSG_PROCESSING_MODEL) && (snmpv3SecModelType == SNMPV1_SECURITY_MODEL))
                        {
                            (void)TCPIP_SNMP_TRAPv1Notify(mibID, val, 0,IPV4_SNMP_TRAP);
                        }
                        else
                        {
                            // continue
                        }
                    }
                }
                else
#endif
                {
                    if(TCPIP_SNMP_TRAPTypeGet())
                    {
                        TCPIP_SNMP_TRAPMibIDGet(&mibID);
                        (void)TCPIP_SNMP_TRAPv2Notify(mibID, val, 0,IPV4_SNMP_TRAP);
                    }
                    else
                    {
                        TCPIP_SNMP_TRAPMibIDGet(&mibID);
                        (void)TCPIP_SNMP_TRAPv1Notify(mibID, val, 0,IPV4_SNMP_TRAP);
                    }
                }
                //application has to decide on which SNMP var OID to send. Ex. PUSH_BUTTON
                smState = SM_PREPARE;
                break;
            }
            break;

        default:
            // do nothing
            break;
    }

    //Try for max 5 seconds to send TRAP, do not get block in while()
    uint32_t sysFreq = SYS_TMR_TickCounterFrequencyGet();
    if(((SYS_TMR_TickCountGet() - TimerRead) >= (5U * sysFreq)) || (receiverIndex == (uint8_t)TCPIP_SNMP_TRAP_TABLE_SIZE))
    {
        smState = SM_PREPARE;
        receiverIndex=0;
        timeLock=false;
        TCPIP_SNMP_AuthTrapFlagSet(false);
        netStartForTrap = true;
    }
}

/*********************************************************************
  Function:
  bool TCPIP_SNMP_IsValidLength(SNMP_ID var, uint8_t len,uint8_t index)

  Summary:
    Validates the set variable data length to data type.

  Description:
    This routine is used to validate the dynamic variable data length
    to the variable data type. It is used when SET request is processed.
    This is a callback function called by module. User application
    must implement this function.

  PreCondition:
    TCPIP_SNMP_ProcessSetVar() is called.

  Parameters:
    var -   Variable id whose value is to be set
    len -   Length value that is to be validated.

  Return Values:
    true  - if given var can be set to given len
    false - if otherwise.

  Remarks:
    This function will be called for only dynamic variables that are
    defined as ASCII_STRING and OCTET_STRING (i.e. data length greater
    than 4 bytes)
 */
bool TCPIP_SNMP_IsValidLength(SNMP_ID var, uint8_t len,uint8_t index)
{
    bool retVal = false;
    switch(var)
    {
        case TRAP_COMMUNITY:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                retVal = true;
            }
            else if ( len < (uint8_t)TCPIP_SNMP_COMMUNITY_MAX_LEN)
            {
                retVal = true;
            }
            else
            {
                // do nothing
            }

            break;
#ifdef TCPIP_STACK_USE_IPV6
        case IPV6_TRAP_COMMUNITY:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                retVal = true;
            }
            else if ( len < (uint8_t)TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN_MEM_USE + 1U)
            {
                retVal = true;
            }
            else
            {
                // do nothing
            }
            break;
        case IPV6_TRAP_RECEIVER_IP:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                retVal = true;
            }
            else if(len == sizeof(IPV6_ADDR))
            {
                retVal = true;
            }
            else
            {
                // do nothing
            }
            break;
#endif
#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
        case USER_SECURITY_NAME:
            if(len <= TCPIP_SNMPV3_USER_SECURITY_NAME_LEN)
            {
                (void)memset(gSnmpv3UserSecurityName, 0,TCPIP_SNMPV3_USER_SECURITY_NAME_LEN);
                retVal = true;
            }
            break;

        case USM_AUTH_KEY:
            if(len == TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN)
            {
                (void)memset(gSnmpv3UserPrivPswdLoclizdKey, 0,TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN_MEM_USE);
                retVal = true;
            }
            break;

        case USM_PRIV_KEY:
            if(len == TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN)
            {
                (void)memset(gSnmpv3UserPrivPswdLoclizdKey, 0,TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN_MEM_USE);
                retVal = true;
            }
            break;

        case SNMP_TARGET_SECURITY_NAME :            // 43.6.1.4.1.17095.5.1.1.4: READWRITE ASCII_STRING.
            if(!TCPIP_SNMPV3_TrapTypeGet())
            {
                retVal = true;
            }
            else if(len <= TCPIP_SNMPV3_USER_SECURITY_NAME_LEN)
            {
                (void)memset(gSnmpv3UserSecurityName, 0,TCPIP_SNMPV3_USER_SECURITY_NAME_LEN);
                retVal = true;
            }
            else
            {
                // do nothing
            }
            break;


#endif /* TCPIP_STACK_USE_SNMPV3_SERVER */

#if defined(SYS_OUT_ENABLE)
        case LCD_DISPLAY:
            if ( len < SYS_OUT_MESSAGE_LINE_COUNT() * SYS_OUT_MESSAGE_LINE_LENGTH() + 1 )
            {
                retVal = true;
            }
            break;
#endif /* defined(SYS_OUT_ENABLE) */

        default:
            // do nothing
            break;
    }
    return retVal;
}

/*********************************************************************
  Function:
    bool TCPIP_SNMP_VarbindSet(SNMP_ID var, SNMP_INDEX index,
                                   uint8_t ref, SNMP_VAL val)

  Summary:
    This routine Set the mib variable with the requested value.

  Description:
    This is a callback function called by module for the snmp
    SET request.User application must modify this function
    for the new variables address.

  Precondition:
    TCPIP_SNMP_ProcessVariables() is called.

  Parameters:
    var -   Variable id whose value is to be set

    ref -   Variable reference used to transfer multi-byte data
            0 if first byte is set otherwise nonzero value to indicate
            corresponding byte being set.

    val -   Up to 4 byte data value.
            If var data type is uint8_t, variable
               value is in val->byte
            If var data type is uint16_t, variable
               value is in val->word
            If var data type is uint32_t, variable
               value is in val->dword.
            If var data type is IP_ADDRESS, COUNTER32,
               or GAUGE32, value is in val->dword
            If var data type is OCTET_STRING, ASCII_STRING
               value is in val->byte; multi-byte transfer
               will be performed to transfer remaining
               bytes of data.

  Return Values:
    true    -   if it is OK to set more byte(s).
    false   -   if otherwise.

  Remarks:
    This function may get called more than once depending on number
    of bytes in a specific set request for given variable.
    only dynamic read-write variables needs to be handled.
 */
bool TCPIP_SNMP_VarbindSet(SNMP_ID var, SNMP_INDEX index, uint8_t ref, SNMP_VAL val)
{
    bool retVal = false;
    switch(var)
    {
        case LLDP_MSG_INTRVL:
            g_lldpMsgIntrvl = val.dword;
            retVal = true;
            break;

        case LLDP_MSG_TX_HOLD_MULTIPLIER:
            g_lldpMsgTxHoldMulti = val.dword;
            retVal = true;
            break;

        case LED_D5:
            if(val.byte != 0U)          
            {
                APP_LED_3StateSet();
            }
            else
            {
                APP_LED_3StateClear();
            }
            //BSP_LEDStateSet(APP_LED_3, val.byte);
            retVal = true;
            break;

        case LED_D6:
            if(val.byte != 0U)          
            {
                APP_LED_2StateSet();
            }
            else
            {
                APP_LED_2StateClear();
            }
            //BSP_LEDStateSet(APP_LED_2, val.byte);
            retVal = true;
            break;

        case TRAP_RECEIVER_IP:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                retVal = true;
                break;
            }

            for(int mibcnt = 0; mibcnt < SNMP_MAX_MIBS_SUPPORTED; mibcnt++)
            {
            // Make sure that index is within our range.
                if ( index < trapInfo[mibcnt].Size )
            {
                // This is just an update to an existing entry.
                    trapInfo[mibcnt].table[index].IPAddress.Val = val.dword;
                retVal = true;
            }
            else if ( index < (uint8_t)TCPIP_SNMP_TRAP_TABLE_SIZE )
            {
                // This is an addition to table.
                    trapInfo[mibcnt].table[index].IPAddress.Val = val.dword;
                    trapInfo[mibcnt].table[index].communityLen = 0;
                    trapInfo[mibcnt].Size++;
                retVal = true;
            }
            else
            {
                // do nothing
            }
            }
            break;

        case TRAP_RECEIVER_ENABLED:
            for(int mibcnt = 0; mibcnt < SNMP_MAX_MIBS_SUPPORTED; mibcnt++)
            {
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                retVal = true;
                break;
            }
            // Make sure that index is within our range.
                if ( index < trapInfo[mibcnt].Size )
            {
                // Value of '1' means Enabled".
                if ( val.byte == 1u )
                {
                        #if (SNMP_DEBUG != 0)
                        SYS_CONSOLE_PRINT("\r\n %s(%d): mibCnt = %d ; index= %d; Flags.bEnabled = 1U", __FUNCTION__, __LINE__, mibcnt, index);
                        #endif
                        trapInfo[mibcnt].table[index].Flags.bEnabled = 1U;
                }
                // Value of '0' means "Disabled.
                else if ( val.byte == 0u )
                {
                        #if (SNMP_DEBUG != 0)
                        SYS_CONSOLE_PRINT("\r\n %s(%d): mibCnt = %d ; index= %d; Flags.bEnabled = 0U", __FUNCTION__, __LINE__, mibcnt, index);
                        #endif
                        trapInfo[mibcnt].table[index].Flags.bEnabled = 0U;
                }
                else
                {   // This is unknown value.
                    break;
                }
                retVal = true;
                    continue;
            }
            // Given index is more than our current table size.
            // If it is within our range, treat it as an addition to table.
            else if ( index < (uint8_t)TCPIP_SNMP_TRAP_TABLE_SIZE )
            {
                // Treat this as an addition to table.
                    trapInfo[mibcnt].Size++;
                    trapInfo[mibcnt].table[index].communityLen = 0;
            }
            else
            {
                // do nothing
            }
            }
            break;

        case TRAP_COMMUNITY:
            for(int mibcnt = 0; mibcnt < SNMP_MAX_MIBS_SUPPORTED; mibcnt++)
            {
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                retVal = true;
                break;
            }
            // Since this is a ASCII_STRING data type, SNMP will call with
            // SNMP_END_OF_VAR to indicate no more bytes.
            // Use this information to determine if we just added new row
            // or updated an existing one.
            if ( ref ==  SNMP_END_OF_VAR )
            {
                // Index equal to table size means that we have new row.
                    if ( index == trapInfo[mibcnt].Size )
                {
                        trapInfo[mibcnt].Size++;
                }

                // Length of string is one more than index.
                    trapInfo[mibcnt].table[index].communityLen++;

                retVal = true;
                    continue;
            }

            // Make sure that index is within our range.
                if ( index < trapInfo[mibcnt].Size )
            {
                // Copy given value into local buffer.
                    trapInfo[mibcnt].table[index].community[ref] = (char)val.byte;
                // Keep track of length too.
                // This may not be NULL terminate string.
                    trapInfo[mibcnt].table[index].communityLen = (uint8_t)ref;
                retVal = true;
            }
            }
            break;

#ifdef TCPIP_STACK_USE_IPV6
        case IPV6_TRAP_RECEIVER_IP:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                retVal = true;
                break;
            }
            // Make sure that index is within our range.
            if ( index < ipv6TrapInfo[SNMP_MIB_II_INDX].Size )
            {
                // This is just an update to an existing entry.
                ipv6TrapInfo[SNMP_MIB_II_INDX].table[index].IPv6Address.v[ref] = val.byte;
                retVal = true;
            }
            break;

        case IPV6_TRAP_ENABLED:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                retVal = true;
                break;
            }
            // Make sure that index is within our range.
            if ( index < ipv6TrapInfo[SNMP_MIB_II_INDX].Size )
            {
                // Value of '1' means Enabled".
                if ( val.byte == 1u )
                {
                    ipv6TrapInfo[SNMP_MIB_II_INDX].table[index].Flags.bEnabled = 1U;
                }
                // Value of '0' means "Disabled.
                else if ( val.byte == 0u )
                {
                    ipv6TrapInfo[SNMP_MIB_II_INDX].table[index].Flags.bEnabled = 0U;
                }
                else
                {   // This is unknown value.
                    break;
                }
                retVal = true;
                break;
            }
            // Given index is more than our current table size.
            // If it is within our range, treat it as an addition to table.
            else if ( index < (uint8_t)TCPIP_SNMP_TRAP_TABLE_SIZE )
            {
                // Treat this as an addition to table.
                ipv6TrapInfo[SNMP_MIB_II_INDX].Size++;
                ipv6TrapInfo[SNMP_MIB_II_INDX].table[index].communityLen = 0;
            }
            else
            {
                // do nothing
            }

            break;

        case IPV6_TRAP_COMMUNITY:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                retVal = true;
                break;
            }
            // Since this is a ASCII_STRING data type, SNMP will call with
            // SNMP_END_OF_VAR to indicate no more bytes.
            // Use this information to determine if we just added new row
            // or updated an existing one.
            if ( ref ==  SNMP_END_OF_VAR )
            {
                // Index equal to table size means that we have new row.
                if ( index == ipv6TrapInfo[SNMP_MIB_II_INDX].Size )
                {
                    ipv6TrapInfo[SNMP_MIB_II_INDX].Size++;
                }

                // Length of string is one more than index.
                ipv6TrapInfo[SNMP_MIB_II_INDX].table[index].communityLen++;

                retVal = true;
                break;
            }

            // Make sure that index is within our range.
            if ( index < ipv6TrapInfo[SNMP_MIB_II_INDX].Size )
            {
                // Copy given value into local buffer.
                ipv6TrapInfo[SNMP_MIB_II_INDX].table[index].community[ref] = (char)val.byte;
                // Keep track of length too.
                // This may not be NULL terminate string.
                ipv6TrapInfo[SNMP_MIB_II_INDX].table[index].communityLen = (uint8_t)ref;
                retVal = true;
            }
            break;
#endif /* TCPIP_STACK_USE_IPV6 */

#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
        case USM_AUTH_PROT:
            if(index>TCPIP_SNMPV3_USM_MAX_USER)
            {
                break;
            }
            if(TCPIP_SNMPV3_EngineUserDataBaseSet(SNMPV3_HASHTYPE_CONFIG_TYPE, 0U,index,&val.byte) == true)
            {
                retVal = false;
            }
            break;

        case USER_SECURITY_NAME:
            /* validate user security length*/
            // Since this is a ASCII_STRING data type, SNMP will call with
            // SNMP_END_OF_VAR to indicate no more bytes.
            // Use this information to determine if we just added new row
            // or updated an existing one.
            if ( ref ==  SNMP_END_OF_VAR )
            {
                if(TCPIP_SNMPV3_EngineUserDataBaseSet(SNMPV3_USERNAME_CONFIG_TYPE, (uint8_t)strlen(gSnmpv3UserSecurityName),index,gSnmpv3UserSecurityName) != true)
                {
                    break;
                }
            }
            // Make sure that index is within our range.
            if ( index < TCPIP_SNMPV3_USM_MAX_USER )
            {
                if(ref > TCPIP_SNMPV3_USER_SECURITY_NAME_LEN)
                {
                    break;
                }
                // Copy given value into local buffer.
                gSnmpv3UserSecurityName[ref]=(char)val.byte;
                retVal = true;
            }
            break;

        case USM_AUTH_KEY:
            if ( ref ==  SNMP_END_OF_VAR )
            {
                if(TCPIP_SNMPV3_EngineUserDataBaseSet(SNMPV3_AUTHPASSWDLOCALIZEDKEY_CONFIG_TYPE, (uint8_t)strlen(gSnmpv3UserAuthPswdLoclizdKey),index,gSnmpv3UserAuthPswdLoclizdKey) == true)
                {
                    retVal = true;
                }
                break;
            }
            // Make sure that index is within our range.
            if ( index < TCPIP_SNMPV3_USM_MAX_USER )
            {
                if(ref > TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN)
                {
                    break;
                }
                // Copy given value into local buffer.
                gSnmpv3UserAuthPswdLoclizdKey[ref]=(char)val.byte;
                retVal = true;
            }
            break;

        case USM_PRIV_PROT:
            if(index>TCPIP_SNMPV3_USM_MAX_USER)
            {
                break;
            }
            if(TCPIP_SNMPV3_EngineUserDataBaseSet(SNMPV3_PRIVTYPE_CONFIG_TYPE,0,index,&val.byte) == true)
            {
                retVal = true;
            }
            break;

        case USM_PRIV_KEY:
            if ( ref ==  SNMP_END_OF_VAR )
            {
                if(TCPIP_SNMPV3_EngineUserDataBaseSet(SNMPV3_PRIVPASSWWDLOCALIZEDKEY_CONFIG_TYPE, (uint8_t)strlen(gSnmpv3UserPrivPswdLoclizdKey),index,gSnmpv3UserPrivPswdLoclizdKey) == true)
                {
                    retVal = true;
                }
                break;
            }
            // Make sure that index is within our range.
            if ( index < TCPIP_SNMPV3_USM_MAX_USER )
            {
                if(ref > TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN)
                {
                    break;
                }
                // Copy given value into local buffer.
                gSnmpv3UserPrivPswdLoclizdKey[ref]=(char)val.byte;
                retVal = true;
            }
            break;

        case SNMP_TARGET_INDEX_ID :         // 43.6.1.4.1.17095.5.1.1.1: READONLY  uint8_t.
            // do nothing
            break;

        case SNMP_TARGET_MP_MODEL :         // 43.6.1.4.1.17095.5.1.1.2: READWRITE  uint8_t.
            if(!TCPIP_SNMPV3_TrapTypeGet())
            {
                retVal = true;
                break;
            }
            if(index < TCPIP_SNMPV3_USM_MAX_USER)
            {
                if(TCPIP_SNMPV3_EngineUserDataBaseSet(SNMPV3_TARGET_MP_MODEL_TYPE,0,index,&val.byte) == true)
                {
                    retVal = true;
                }
            }
            break;

        case  SNMP_TARGET_SECURITY_MODEL :          // 43.6.1.4.1.17095.5.1.1.3: READWRITE  uint8_t.
            if(!TCPIP_SNMPV3_TrapTypeGet())
            {
                retVal = true;
                break;
            }

            if(TCPIP_SNMPV3_EngineUserDataBaseSet(SNMPV3_TARGET_SECURITY_MODEL_TYPE,0,index,&val.byte) == true)
            {
                retVal = true;
            }
            break;

        case SNMP_TARGET_SECURITY_NAME :            // 43.6.1.4.1.17095.5.1.1.4: READWRITE ASCII_STRING.
            if(!TCPIP_SNMPV3_TrapTypeGet())
            {
                retVal = true;
                break;
            }
            if ( ref ==  SNMP_END_OF_VAR )
            {
                if(TCPIP_SNMPV3_EngineUserDataBaseSet(SNMPV3_TARGET_SECURITY_NAME_TYPE, (uint8_t)strlen(gSnmpv3UserSecurityName),index,gSnmpv3UserSecurityName) == true)
                {
                    retVal = true;
                }
                break;
            }
            // Make sure that index is within our range.
            if ( index < TCPIP_SNMPV3_USM_MAX_USER )
            {
                if(ref > TCPIP_SNMPV3_USER_SECURITY_NAME_LEN)
                {
                    break;
                }
                // Copy given value into local buffer.
                gSnmpv3UserSecurityName[ref]=(char)val.byte;
                retVal = true;
            }
            break;

        case SNMP_TARGET_SECURITY_LEVEL :           // 43.6.1.4.1.17095.5.1.1.5: READWRITE  uint8_t.
            if(!TCPIP_SNMPV3_TrapTypeGet())
            {
                retVal = true;
                break;
            }
            if(TCPIP_SNMPV3_EngineUserDataBaseSet(SNMPV3_TARGET_SECURITY_LEVEL_TYPE,0,index,&val.byte) == true)
            {
                retVal = true;
            }
            break;

#endif  /* TCPIP_STACK_USE_SNMPV3_SERVER */
#if defined(SYS_OUT_ENABLE)
        case LCD_DISPLAY:
            // Copy all bytes until all bytes are transferred
            if ( ref != SNMP_END_OF_VAR)
            {
                lcdMessage[ref] = val.byte;
                lcdMessage[ref+1] = 0;
            }
            else
            {
                SYS_OUT_MESSAGE_LINE((char*)lcdMessage,2);
            }

            retVal = true;
            break;
#endif

        default:
            // do nothing
            break;
    }

    return retVal;
}


/*********************************************************************
  Function:
    bool TCPIP_SNMP_ExactIndexGet(SNMP_ID var,SNMP_INDEX *index)

  Summary:
    To search for exact index node in case of a Sequence variable.

  Description:
    This is a callback function called by SNMP module.
    SNMP user must implement this function in user application and
    provide appropriate data when called.  This function will only
    be called for OID variable of type sequence.

  PreCondition:
    None

  Parameters:
    var     -   Variable id as per mib.h (input)
    index      -     Index of variable (input)

  Return Values:
    true    -    If the exact index value exists for given variable at given
                 index.
    false   -    Otherwise.

  Remarks:
      Only sequence index needs to be handled in this function.
 */
bool TCPIP_SNMP_ExactIndexGet(SNMP_ID var, SNMP_INDEX *index)
{
    bool retVal = false;
    switch(var)
    {
        case TRAP_RECEIVER_ID:
        case TRAP_RECEIVER_ENABLED:
        case TRAP_RECEIVER_IP:
        case TRAP_COMMUNITY:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                retVal = true;
                break;
            }
            // There is no next possible index if table itself is empty.
            if ( trapInfo[SNMP_MIB_II_INDX].Size == 0u )
            {
                break;
            }
            if(*index == SNMP_INDEX_INVALID)
            {
                *index = 0;
            }

            if(*index < trapInfo[SNMP_MIB_II_INDX].Size)
            {
                retVal = true;
            }
        break;

#ifdef TCPIP_STACK_USE_IPV6
        case IPV6_TRAP_RECEIVER_ID:
        case IPV6_TRAP_ENABLED:
    case IPV6_TRAP_RECEIVER_IP:
    case IPV6_TRAP_COMMUNITY:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                retVal = true;
                break;
            }
            // There is no next possible index if table itself is empty.
            if ( ipv6TrapInfo[SNMP_MIB_II_INDX].Size == 0u )
            {
                break;
            }
            if(*index == SNMP_INDEX_INVALID)
            {
                *index = 0;
            }
            if(*index < ipv6TrapInfo[SNMP_MIB_II_INDX].Size)
            {
                retVal = true;
            }
            break;
#endif /* TCPIP_STACK_USE_IPV6*/

#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
        case USM_INDEX_ID:
        case USM_AUTH_KEY:
        case USM_AUTH_PROT:
        case USER_SECURITY_NAME:
        case USM_PRIV_KEY:
        case USM_PRIV_PROT:
            if(*index == SNMP_INDEX_INVALID)
            {
                *index = 0;
            }
            if ( *index < TCPIP_SNMPV3_USM_MAX_USER)
            {
                retVal = true;
            }
        break;

        case SNMP_TARGET_INDEX_ID :         // 43.6.1.4.1.17095.5.1.1.1: READONLY uint8_t.
        case SNMP_TARGET_MP_MODEL :         // 43.6.1.4.1.17095.5.1.1.2: READWRITE uint8_t.
        case  SNMP_TARGET_SECURITY_MODEL :          // 43.6.1.4.1.17095.5.1.1.3: READWRITE uint8_t.
        case SNMP_TARGET_SECURITY_NAME :            // 43.6.1.4.1.17095.5.1.1.4: READWRITE ASCII_STRING.
        case SNMP_TARGET_SECURITY_LEVEL :           // 43.6.1.4.1.17095.5.1.1.5: READWRITE uint8_t.
            if(!TCPIP_SNMPV3_TrapTypeGet())
            {
                retVal = true;
                break;
            }
            if(*index == SNMP_INDEX_INVALID)
            {
                *index = 0;
            }
            if ( *index < TCPIP_SNMPV3_USM_MAX_USER)
            {
                retVal = true;
            }
        break;

#endif /* TCPIP_STACK_USE_SNMPV3_SERVER */
        
        default:
        // do nothing
        break;
    }
    return retVal;
}


/*********************************************************************
  Function:
    bool TCPIP_SNMP_NextIndexGet(SNMP_ID var,SNMP_INDEX* index)

  Summary:
    To search for next index node in case of a Sequence variable.

  Description:
    This is a callback function called by SNMP module.
    SNMP user must implement this function in user application and
    provide appropriate data when called.  This function will only
    be called for OID variable of type sequence.

  PreCondition:
    None

  Parameters:
    var     -   Variable id whose value is to be returned
    index   -   Next Index of variable that should be transferred

  Return Values:
    true    -    If a next index value exists for given variable at given
                 index and index parameter contains next valid index.
    false   -    Otherwise.

  Remarks:
      Only sequence index needs to be handled in this function.
 */
bool TCPIP_SNMP_NextIndexGet(SNMP_ID var, SNMP_INDEX* index)
{
    SNMP_INDEX tempIndex;

    tempIndex = *index;

    bool retVal = false;
    switch(var)
    {

        case TRAP_RECEIVER_ID:
        case TRAP_RECEIVER_ENABLED:
        case TRAP_RECEIVER_IP:
        case TRAP_COMMUNITY:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                break;
            }
            // There is no next possible index if table itself is empty.
            if ( trapInfo[SNMP_MIB_II_INDX].Size == 0u )
            {
                break;
            }
            // INDEX_INVALID means start with first index.
            if ( tempIndex == (uint8_t)SNMP_INDEX_INVALID )
            {
                *index = 0;
                retVal = true;
            }
            else if ( tempIndex < (trapInfo[SNMP_MIB_II_INDX].Size-1) )
            {
                *index = tempIndex + 1U;
                retVal = true;
            }
            else
            {
                // do nothing
            }
            break;

#ifdef TCPIP_STACK_USE_IPV6
        case IPV6_TRAP_RECEIVER_ID:
        case IPV6_TRAP_ENABLED:
        case IPV6_TRAP_RECEIVER_IP:
        case IPV6_TRAP_COMMUNITY:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                break;
            }
            // There is no next possible index if table itself is empty.
            if ( ipv6TrapInfo[SNMP_MIB_II_INDX].Size == 0u )
            {
                break;
            }
            // INDEX_INVALID means start with first index.
            if ( tempIndex == (uint8_t)SNMP_INDEX_INVALID )
            {
                *index = 0;
                retVal = true;
            }
            else if ( tempIndex < (ipv6TrapInfo[SNMP_MIB_II_INDX].Size-1) )
            {
                *index = tempIndex+1U;
                retVal = true;
            }
            else
            {
                // do nothing
            }
            break;
#endif /* TCPIP_STACK_USE_IPV6 */

#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
        case USM_INDEX_ID:
        case USM_AUTH_KEY:
        case USM_AUTH_PROT:
        case USER_SECURITY_NAME:
        case USM_PRIV_KEY:
        case USM_PRIV_PROT:
            if ( tempIndex == (uint8_t)SNMP_INDEX_INVALID )
            {
                *index = 0;
                retVal = true;
            }
            else if ( tempIndex < (TCPIP_SNMPV3_USM_MAX_USER-1) )
            {
                *index = tempIndex+1U;
                retVal = true;
            }
            else
            {
                // do nothing
            }
            break;

        case SNMP_TARGET_INDEX_ID :         // 43.6.1.4.1.17095.5.1.1.1: READONLY uint8_t.
        case SNMP_TARGET_MP_MODEL :         // 43.6.1.4.1.17095.5.1.1.2: READWRITE uint8_t.
        case SNMP_TARGET_SECURITY_MODEL :           // 43.6.1.4.1.17095.5.1.1.3: READWRITE uint8_t.
        case SNMP_TARGET_SECURITY_NAME :            // 43.6.1.4.1.17095.5.1.1.4: READWRITE ASCII_STRING.
        case SNMP_TARGET_SECURITY_LEVEL :           // 43.6.1.4.1.17095.5.1.1.5: READWRITE uint8_t.
            if(!TCPIP_SNMPV3_TrapTypeGet())
            {
                break;
            }
            if ( tempIndex == (uint8_t)SNMP_INDEX_INVALID )
            {
                *index = 0;
                retVal = true;
            }
            else if ( tempIndex < (TCPIP_SNMPV3_USM_MAX_USER-1) )
            {
                *index = tempIndex+1U;
                retVal = true;
            }
            else
            {
                // do nothing
            }
            break;
#endif /* TCPIP_STACK_USE_SNMPV3_SERVER */

        default:
            // do nothing
            break;
    }

    return retVal;
}


/*********************************************************************
  Function:
    bool TCPIP_SNMP_VarbindGet(SNMP_ID var, SNMP_INDEX index,uint8_t* ref, SNMP_VAL* val)

  Summary:
    Used to Get/collect OID variable information.

  Description:
    This is a callback function called by SNMP module. SNMP user must
    implement this function in user application and provide appropriate
    data when called.

  PreCondition:
    None

  parameters:
    var     -   Variable id whose value is to be returned
    index   -   Index of variable that should be transferred
    ref     -   Variable reference used to transfer
                multi-byte data
                It is always SNMP_START_OF_VAR when very
                first byte is requested.
                Otherwise, use this as a reference to
                keep track of multi-byte transfers.
    val     -   Pointer to up to 4 byte buffer.
                If var data type is uint8_t, transfer data
                  in val->byte
                If var data type is uint16_t, transfer data in
                  val->word
                If var data type is uint32_t, transfer data in
                  val->dword
                If var data type is IP_ADDRESS, transfer data
                  in val->v[] or val->dword
                If var data type is COUNTER32, TIME_TICKS or
                  GAUGE32, transfer data in val->dword
                If var data type is ASCII_STRING or OCTET_STRING
                  transfer data in val->byte using multi-byte
                  transfer mechanism.

  Return Values:
    true    -   If a value exists for given variable at given index.
    false   -   Otherwise.

  Remarks:
    None.
 */
bool TCPIP_SNMP_VarbindGet(SNMP_ID var, SNMP_INDEX index, uint8_t* ref, SNMP_VAL* val)
{
	uint8_t myRef;
	static char AN0String[8];
    char*    endPtr = NULL;
<#if ((tcpipStack.TCPIP_DEVICE_FAMILY?has_content) && ((tcpipStack.TCPIP_DEVICE_FAMILY  == "SAME70") || (tcpipStack.TCPIP_DEVICE_FAMILY  == "SAMV71") || (tcpipStack.TCPIP_DEVICE_FAMILY  == "PIC32CZCA7") || (tcpipStack.TCPIP_DEVICE_FAMILY  == "SAME54") || (tcpipStack.TCPIP_DEVICE_FAMILY  == "PIC32M"))) >
	uint16_t     randPotVal=0;
</#if>

    // Convert potentiometer result into ASCII string
	randPotVal= (uint16_t)SYS_RANDOM_PseudoGet();
    uitoa(randPotVal, AN0String);


    myRef = *ref;

    bool retVal = false;
    switch(var)
    {
        case LLDP_MSG_INTRVL:
            val->dword = g_lldpMsgIntrvl;
            retVal = true;
            break;

        case LLDP_MSG_TX_HOLD_MULTIPLIER:
            val->dword = g_lldpMsgTxHoldMulti;
            retVal = true;
            break;

        case SYS_UP_TIME:
            val->dword = SNMPGetTimeStamp();
            retVal = true;
            break;

        case LED_D5:
            val->byte = (uint8_t)APP_LED_3StateGet();//BSP_LEDStateGet(APP_LED_3);
            retVal = true;
            break;

        case LED_D6:
            val->byte = (uint8_t)APP_LED_2StateGet();//BSP_LEDStateGet(APP_LED_2);
            retVal = true;
            break;

        case PUSH_BUTTON:
            // There is only one button - meaning only index of 0 is allowed.
            val->byte = (uint8_t)APP_SWITCH_1StateGet();
            retVal = true;
            break;

        case ANALOG_POT0:
            errno = 0;
            val->word = (uint16_t)strtoul(AN0String, &endPtr, 10);
            if(errno != 0 || (endPtr - AN0String) != (ssize_t)strlen(AN0String))
            {   // range error or not all was converted
                retVal = false;
            }
            else
            {
                retVal = true;
            }
            break;

        case TRAP_RECEIVER_ID:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                break;
            }
            if ( index < trapInfo[SNMP_MIB_II_INDX].Size )
            {
                val->byte = index;
                retVal = true;
            }
            break;

        case TRAP_RECEIVER_ENABLED:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                break;
            }

            if ( index < trapInfo[SNMP_MIB_II_INDX].Size )
            {
                val->byte = trapInfo[SNMP_MIB_II_INDX].table[index].Flags.bEnabled;
                retVal = true;
            }
            break;

        case TRAP_RECEIVER_IP:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                break;
            }

            if ( index < trapInfo[SNMP_MIB_II_INDX].Size )
            {
                val->dword = trapInfo[SNMP_MIB_II_INDX].table[index].IPAddress.Val;
                retVal = true;
            }
            break;

        case TRAP_COMMUNITY:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                break;
            }
            
            if ( index < trapInfo[SNMP_MIB_II_INDX].Size )
            {
                if ( myRef == trapInfo[SNMP_MIB_II_INDX].table[index].communityLen )
                {
                    *ref = SNMP_END_OF_VAR;
                    retVal = true;
                    break;
                }
                if ( trapInfo[SNMP_MIB_II_INDX].table[index].communityLen == 0u )
                {
                    *ref = SNMP_END_OF_VAR;
                }
                else
                {
                    val->byte = (uint8_t)trapInfo[SNMP_MIB_II_INDX].table[index].community[myRef];
                    myRef++;
                    *ref = myRef;
                }
                retVal = true;
            }
            break;
#ifdef  TCPIP_STACK_USE_IPV6
        case IPV6_TRAP_RECEIVER_ID:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                break;
            }
            if ( index < ipv6TrapInfo[SNMP_MIB_II_INDX].Size )
            {
                val->byte = index;
                retVal = true;
            }
            break;

        case IPV6_TRAP_ENABLED:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                break;
            }
            if ( index < ipv6TrapInfo[SNMP_MIB_II_INDX].Size )
            {
                val->byte = ipv6TrapInfo[SNMP_MIB_II_INDX].table[index].Flags.bEnabled;
                retVal = true;
            }
            break;

        case IPV6_TRAP_RECEIVER_IP:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                break;
            }
            if ( index < ipv6TrapInfo[SNMP_MIB_II_INDX].Size )
            {
                if ( myRef == sizeof(IPV6_ADDR) )
                {
                    *ref = SNMP_END_OF_VAR;
                    retVal = true;
                    break;
                }
                val->byte = ipv6TrapInfo[SNMP_MIB_II_INDX].table[index].IPv6Address.v[myRef];

                myRef++;
                *ref = myRef;
                retVal = true;
            }
            break;

    case IPV6_TRAP_COMMUNITY:
            if(!TCPIP_SNMP_IsTrapEnabled())
            {
                break;
            }
            if ( index < ipv6TrapInfo[SNMP_MIB_II_INDX].Size )
            {
                if ( myRef == ipv6TrapInfo[SNMP_MIB_II_INDX].table[index].communityLen )
                {
                    *ref = SNMP_END_OF_VAR;
                    retVal = true;
                    break;
                }
                if ( ipv6TrapInfo[SNMP_MIB_II_INDX].table[index].communityLen == 0u )
                {
                    *ref = SNMP_END_OF_VAR;
                }
                else
                {
                    val->byte = (uint8_t)ipv6TrapInfo[SNMP_MIB_II_INDX].table[index].community[myRef];
                    myRef++;
                    *ref = myRef;
                }
                retVal = true;
            }
            break;
#endif /* TCPIP_STACK_USE_IPV6 */


#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
        case ENGINE_ID:
            if(TCPIP_SNMPV3_EngineUserDataBaseGet(SNMPV3_ENGINE_ID_TYPE,myRef,0,&val->byte) == false)
            {
                *ref = SNMP_END_OF_VAR;
            }
            else
            {
                myRef++;
                *ref = myRef;
            }
            retVal = true;
            break;

        case ENGINE_BOOT:
            if(TCPIP_SNMPV3_EngineUserDataBaseGet(SNMPV3_ENGINE_BOOT_TYPE,myRef,0,&val->dword) == true)
            {
                retVal = true;
            }
            break;

        case ENGINE_TIME:
           if(TCPIP_SNMPV3_EngineUserDataBaseGet(SNMPV3_ENGINE_TIME_TYPE,myRef,0,&val->dword) == true)
           {
               retVal = true;
           }
           break;

        case ENGINE_MAX_MSG:
            if(TCPIP_SNMPV3_EngineUserDataBaseGet(SNMPV3_ENGINE_MAX_MSG_TYPE,myRef,0,&val->dword) == true)
            {
                retVal = true;
            }
            break;

        case USM_INDEX_ID:
            if ( index < TCPIP_SNMPV3_USM_MAX_USER)
            {
                val->byte = index;
                retVal = true;
            }
            break;

        case USM_AUTH_PROT:
            if(index < TCPIP_SNMPV3_USM_MAX_USER)
            {
                if(TCPIP_SNMPV3_EngineUserDataBaseGet(SNMPV3_HASHTYPE_CONFIG_TYPE,0,index,&val->byte) == false)
                {
                    break;
                }
            }
            else
            {
                break;
            }
            retVal = true;
            break;

        case USM_PRIV_PROT:
            // code change is required
            if(index < TCPIP_SNMPV3_USM_MAX_USER)
            {
                if(TCPIP_SNMPV3_EngineUserDataBaseGet(SNMPV3_PRIVTYPE_CONFIG_TYPE,0,index,&val->byte) == false)
                {
                    break;
                }
            }
            else
            {
                break;
            }
            retVal = true;
            break;

        case USER_SECURITY_NAME:
            if(index < TCPIP_SNMPV3_USM_MAX_USER)
            {
                if(TCPIP_SNMPV3_EngineUserDataBaseGet(SNMPV3_USERNAME_CONFIG_TYPE,myRef,index,&val->byte) == false)
                {
                    *ref = SNMP_END_OF_VAR;
                }
                else
                {
                    myRef++;
                    *ref = myRef;
                }
                retVal = true;
            }
            break;

        case USM_AUTH_KEY:
            if(index < TCPIP_SNMPV3_USM_MAX_USER)
            {
                if(TCPIP_SNMPV3_EngineUserDataBaseGet(SNMPV3_AUTHPASSWDLOCALIZEDKEY_CONFIG_TYPE,myRef,index,&val->byte) == false)
                {
                    *ref = SNMP_END_OF_VAR;
                }
                else
                {
                    myRef++;
                    *ref = myRef;
                }
                retVal = true;
            }
            break;

        case USM_PRIV_KEY:
            // code change is required / for temp- same auth passwd string is returned.
            if(index < TCPIP_SNMPV3_USM_MAX_USER)
            {
                if(TCPIP_SNMPV3_EngineUserDataBaseGet(SNMPV3_PRIVPASSWWDLOCALIZEDKEY_CONFIG_TYPE,myRef,index,&val->byte) == false)
                {
                    *ref = SNMP_END_OF_VAR;
                }
                else
                {
                    myRef++;
                    *ref = myRef;
                }
                retVal = true;
            }
            break;

        case SNMP_TARGET_INDEX_ID :         // 43.6.1.4.1.17095.5.1.1.1: READONLY uint8_t.
            if(!TCPIP_SNMPV3_TrapTypeGet())
            {
                break;
            }
            if(index < TCPIP_SNMPV3_USM_MAX_USER)
            {
                val->byte = index;
                retVal = true;
            }
            break;

        case SNMP_TARGET_MP_MODEL :         // 43.6.1.4.1.17095.5.1.1.2: READWRITE uint8_t.
            if(!TCPIP_SNMPV3_TrapTypeGet())
            {
                break;
            }
            if(index < TCPIP_SNMPV3_USM_MAX_USER)
            {
                if(TCPIP_SNMPV3_EngineUserDataBaseGet(SNMPV3_TARGET_MP_MODEL_TYPE,0,index,&val->byte) == true)
                {
                    retVal = true;
                }
            }
            break;

        case  SNMP_TARGET_SECURITY_MODEL :          // 43.6.1.4.1.17095.5.1.1.3: READWRITE uint8_t.
            if(!TCPIP_SNMPV3_TrapTypeGet())
            {
                break;
            }
            if(index < TCPIP_SNMPV3_USM_MAX_USER)
            {
                if(TCPIP_SNMPV3_EngineUserDataBaseGet(SNMPV3_TARGET_SECURITY_MODEL_TYPE,0,index,&val->byte) ==  true)
                {
                    retVal = true;
                }
            }
            break;

        case SNMP_TARGET_SECURITY_NAME :            // 43.6.1.4.1.17095.5.1.1.4: READWRITE ASCII_STRING.
            if(!TCPIP_SNMPV3_TrapTypeGet())
            {
                break;
            }
            if(index < TCPIP_SNMPV3_USM_MAX_USER)
            {
                if(TCPIP_SNMPV3_EngineUserDataBaseGet(SNMPV3_TARGET_SECURITY_NAME_TYPE,myRef,index,&val->byte) == false)
                {
                    *ref = SNMP_END_OF_VAR;
                }
                else
                {
                    myRef++;
                    *ref = myRef;
                }
                retVal = true;
            }
            break;
        case SNMP_TARGET_SECURITY_LEVEL :           // 43.6.1.4.1.17095.5.1.1.5: READWRITE uint8_t.
            if(!TCPIP_SNMPV3_TrapTypeGet())
            {
                break;
            }
            if(index < TCPIP_SNMPV3_USM_MAX_USER)
            {
                if(TCPIP_SNMPV3_EngineUserDataBaseGet(SNMPV3_TARGET_SECURITY_LEVEL_TYPE,0,index,&val->byte) == true)
                {
                    retVal = true;
                }
            }
            break;

#endif /* TCPIP_STACK_USE_SNMPV3_SERVER */
#if defined(SYS_OUT_ENABLE)
        case LCD_DISPLAY:
            strncpy((char*)lcdMessage, (char*)SYS_OUT_GET_LCD_MESSAGE(), sizeof(lcdMessage)-1);
            if ( lcdMessage[0] == 0u )
            {
                myRef = SNMP_END_OF_VAR;
            }
            else
            {
                val->byte = lcdMessage[myRef++];
                if ( lcdMessage[myRef] == 0u )
                {
                    myRef = SNMP_END_OF_VAR;
                }
            }

            *ref = myRef;
            retVal = true;
            break;
#endif /* SYS_OUT_ENABLE */

        default:
            // do nothing
            break;
    }

    return retVal;
}

bool TCPIP_SNMP_RecordIDValidation(uint8_t snmpVersion,bool idPresent,uint16_t varId,uint8_t * oidValuePtr,uint8_t oidLen)
{
    size_t i=0,j=0;
    uint8_t len=0;
    bool flag=false;
    uint8_t size=0;

    if(!idPresent)
    {
        if(oidValuePtr == NULL)
        {
            #if (SNMP_DEBUG != 0)
            SYS_CONSOLE_PRINT("\r\n%s(%d): Exit", __FUNCTION__, __LINE__);
            #endif
            return false;
        }

        for(i=0; i< sizeof(gSnmpNonMibRecInfo) / sizeof(*gSnmpNonMibRecInfo); i++)
        {
            if((snmpVersion != SNMP_V3) && (gSnmpNonMibRecInfo[i].version == SNMP_V3))
            {
                continue;
            }

            size = (uint8_t)strlen((char*)gSnmpNonMibRecInfo[i].oidstr);
            if(size == 0U)
            {
                continue;
            }
            if( size <= oidLen)
            {
                len = size;
            }
            else
            {
                continue;
            }

            // find the first unmatching byte
            while(len-- != 0)
            {
                if(gSnmpNonMibRecInfo[i].oidstr[j] != oidValuePtr[j])
                {
                    flag = false;
                    #if (SNMP_DEBUG != 0)
                    SYS_CONSOLE_PRINT("\r\n%s(%d): i = %d j = %d Exit", __FUNCTION__, __LINE__, i, j);
                    #endif
                    j=0;
                    break;
                }
                else
                {
                    flag = true;
                    j++;
                }
            }
            if(flag == true)
            {
                return true;
            }
        }
        return false;
    }
    if(varId == MICROCHIP || varId == SYS_UP_TIME || varId == LED_D5 || 
            varId == LED_D6 || varId == PUSH_BUTTON || varId == ANALOG_POT0 || 
            varId == LLDP_MSG_INTRVL || varId == LLDP_MSG_TX_HOLD_MULTIPLIER)
    {
        return true;
    }

#if defined(SYS_OUT_ENABLE)
    if(varId == LCD_DISPLAY)
    {
        return true;
    }
#endif /* SYS_OUT_ENABLE */

    if(TCPIP_SNMP_IsTrapEnabled())
    {
        if(varId == TRAP_RECEIVER_ID || varId == TRAP_RECEIVER_ENABLED || varId == TRAP_RECEIVER_IP|| varId == TRAP_COMMUNITY)
        {
            return true;
        }

#ifdef TCPIP_STACK_USE_IPV6
        if( varId == IPV6_TRAP_RECEIVER_ID || varId == IPV6_TRAP_ENABLED || varId == IPV6_TRAP_RECEIVER_IP || varId == IPV6_TRAP_COMMUNITY)
        {
            return true;
        }
#endif
    }

#ifdef TCPIP_STACK_USE_SNMPV3_SERVER
    if(snmpVersion == SNMP_V3)
    {
        if(varId == ENGINE_ID || varId == ENGINE_BOOT || varId == ENGINE_TIME || varId == ENGINE_MAX_MSG || varId == USM_INDEX_ID || varId == USM_AUTH_PROT
                || varId == USM_PRIV_PROT || varId == USER_SECURITY_NAME || varId == USM_AUTH_KEY || varId == USM_PRIV_KEY)
        {
            return true;
        }

        if(TCPIP_SNMPV3_TrapTypeGet())
        {
            if(varId ==  SNMP_TARGET_INDEX_ID                   // 43.6.1.4.1.17095.5.1.1.1: READONLY uint8_t.
                    || varId ==  SNMP_TARGET_MP_MODEL                 // 43.6.1.4.1.17095.5.1.1.2: READWRITE uint8_t.
                    || varId ==   SNMP_TARGET_SECURITY_MODEL          // 43.6.1.4.1.17095.5.1.1.3: READWRITE uint8_t.
                    || varId ==  SNMP_TARGET_SECURITY_NAME            // 43.6.1.4.1.17095.5.1.1.4: READWRITE ASCII_STRING.
                    || varId ==  SNMP_TARGET_SECURITY_LEVEL)          // 43.6.1.4.1.17095.5.1.1.5: READWRITE uint8_t.
            {
                return true;
            }
        }
    }
#endif /* TCPIP_STACK_USE_SNMPV3_SERVER */

    #if (SNMP_DEBUG != 0)
    SYS_CONSOLE_PRINT("\r\n%s(%d): Exit", __FUNCTION__, __LINE__);
    #endif
    return false;
}

/*********************************************************************
  Function:
    static uint32_t SNMPGetTimeStamp(void)

  Summary:
    Obtains the current Tick value for the SNMP time stamp.

  Description:
    This function retrieves the absolute time measurements for
    SNMP time stamp in tens of milliseconds.

  PreCondition:
    None

  parameters:
    None

  Return Values:
    timeStamp - uint32_t timevalue

  Remarks:
    None.
 */
static uint32_t SNMPGetTimeStamp(void)
{
    uint32_t tickFreq = SYS_TMR_TickCounterFrequencyGet();
    uint32_t tickCount = SYS_TMR_TickCountGet(); 
    uint32_t timeStamp = (uint32_t)(((uint64_t)tickCount * 100ULL) / tickFreq);
    return timeStamp;

}

#endif  //#if defined(TCPIP_STACK_USE_SNMP_SERVER)

