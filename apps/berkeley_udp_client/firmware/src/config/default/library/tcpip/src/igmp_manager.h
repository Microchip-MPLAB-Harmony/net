/*******************************************************************************
  IGMP Module Manager internal stack API

  Company:
    Microchip Technology Inc.
    
  File Name:
    igmp_manager.h

  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2016 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef __IGMP_MANAGER_H_
#define __IGMP_MANAGER_H_

/*****************************************************************************
  Function:
    bool TCPIP_IGMP_Initialize(const TCPIP_STACK_MODULE_CTRL* const stackInit, const TCPIP_IGMP_MODULE_CONFIG* pIgmpCfg)

  Summary:
    Initializes the IGMP module.

  Description:
    Initializes the IGMP module. 

  Precondition:
    None

  Parameters:
    stackInit   - pointer to stack initialization data; contains heap, interfaces, etc

    pIgmpCfg    - pointer to a IGMP initialization structure


  Returns:
    true if initialization succeeded
    false otherwise

  Remarks:
   None
 */
bool TCPIP_IGMP_Initialize(const TCPIP_STACK_MODULE_CTRL *const stackCtrl, const TCPIP_IGMP_MODULE_CONFIG* pIgmpCfg);

/*****************************************************************************
  Function:
    void TCPIP_IGMP_Deinitialize(const TCPIP_STACK_MODULE_CTRL* const stackInit)

  Summary:
    De-Initializes the IGMP module.

  Description:
    De-Initializes the IGMP module and performs module clean up
    including dynamic memory deallocation, etc.

  Precondition:
    TCPIP_IGMP_Initialize() should have been called

  Parameters:
    stackInit   - pointer to stack initialization data; contains heap, interfaces, etc
                  and interface that's going down

  Returns:
    None

  Remarks:
   None
 */


void TCPIP_IGMP_Deinitialize(const TCPIP_STACK_MODULE_CTRL *const stackCtrl);

/*****************************************************************************
  Function:
    bool TCPIP_IGMP_IsMcastEnabled(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, IPV4_ADDR sourceAddress);

  Summary:
    Checks that multicast traffic towards the specified socket is enabled.

  Description:
    This function will perform a check if the multicast traffic arriving for the UDP socket
    to the mcast group address from the source address is enabled/valid.

  Precondition:
    TCPIP_IGMP_Initialize() should have been called

  Parameters:
    socket       - the UDP multicast socket
    hNet         - Interface handle.
    mcastAddress - the multicast group addressthat's the destination of this traffic
    sourceAddress   - the source of the multicast traffic

  Returns:
    true if multicast traffic is enabled
    false otherwise

  Remarks:
   This function checks SSM traffic only.
   Any other multicast traffic is unchecked and the return value will be true.
 */
bool TCPIP_IGMP_IsMcastEnabled(UDP_SOCKET socket, TCPIP_NET_HANDLE hNet, IPV4_ADDR mcastAddress, IPV4_ADDR sourceAddress);


// debugging helpers
//

typedef enum
{
    TCPIP_IGMP_REPORT_LIST_SC           = 0x0001,   // a source change list: Source list change + filter change
    TCPIP_IGMP_REPORT_LIST_GEN_QUERY    = 0x0002,   // a general query list
    TCPIP_IGMP_REPORT_LIST_GROUP_QUERY  = 0x0004,   // a group query list
    // other lists here


    // debugging flags 
    TCPIP_IGMP_REPORT_LIST_FLAG_FREE    = 0x4000,   // display the free list
    TCPIP_IGMP_REPORT_LIST_FLAG_BUSY    = 0x8000,   // display the busy/scheduled list

    //
    TCPIP_IGMP_REPORT_LIST_MASK_ALL     = 0xc007,   // all options set
}TCPIP_IGMP_REPORT_LIST_TYPE;

void TCPIP_IGMP_ReportListPrint(TCPIP_IGMP_REPORT_LIST_TYPE rListType);

typedef enum
{
    TCPIP_IGMP_ROUTE_INTERNAL   = 0x01,     // route the query request internally
    TCPIP_IGMP_ROUTE_EXTERNAL   = 0x02,     // route the query request externally

}TCPIP_IGMP_ROUTE_FLAG;

// respTime in seconds
// pGroupAdd == 0 or *pGroupAdd == 0 -> General Query
// nSources == 0 -> Group Specific Query
// nSources != 0, sourceList != 0 -> Group Source Specific Query
TCPIP_IGMP_RESULT TCPIP_IGMP_SendQuery(uint32_t* pGroupAdd, uint32_t* sourceList, uint16_t nSources, uint16_t respTime, int ifIx, TCPIP_IGMP_ROUTE_FLAG rFlag);


#endif  // __IGMP_MANAGER_H_

