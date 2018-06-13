<#--
/*******************************************************************************
  TCPIP MAC Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_extphy_ksz8061.h.ftl

  Summary:
    TCPIP MAC Freemarker Template File

  Description:

*******************************************************************************/

/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
-->


#define TCPIP_INTMAC_PHY_CONFIG_FLAGS     			\
<#if TCPIP_INTMAC_PHY_CONFIG_RMII>
                                                    DRV_ETHPHY_CFG_RMII | \
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_ALTERNATE>
                                                    DRV_ETHPHY_CFG_ALTERNATE | \
</#if>
<#if TCPIP_INTMAC_PHY_CONFIG_AUTO>
                                                    DRV_ETHPHY_CFG_AUTO | \
</#if>
                                                    0                                                    

#define TCPIP_INTMAC_PHY_LINK_INIT_DELAY  			${TCPIP_INTMAC_PHY_LINK_INIT_DELAY}
#define TCPIP_INTMAC_PHY_ADDRESS		    			${TCPIP_INTMAC_PHY_ADDRESS}
#define DRV_ETHPHY_INSTANCES_NUMBER					${DRV_ETHPHY_INSTANCES_NUMBER}
#define DRV_ETHPHY_CLIENTS_NUMBER					${DRV_ETHPHY_CLIENTS_NUMBER}
#define DRV_ETHPHY_INDEX		        			${DRV_ETHPHY_INDEX}
#define DRV_ETHPHY_PERIPHERAL_ID					${DRV_ETHPHY_PERIPHERAL_ID}
#define DRV_ETHPHY_NEG_INIT_TMO		    			${DRV_ETHPHY_NEG_INIT_TMO}
#define DRV_ETHPHY_NEG_DONE_TMO		    			${DRV_ETHPHY_NEG_DONE_TMO}
#define DRV_ETHPHY_RESET_CLR_TMO					${DRV_ETHPHY_RESET_CLR_TMO}
<#if drvMiim.DRV_MIIM_USE_DRIVER?has_content && drvMiim.DRV_MIIM_USE_DRIVER == true >
#define DRV_ETHPHY_USE_DRV_MIIM                     true
<#else>
#define DRV_ETHPHY_USE_DRV_MIIM                     false
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
