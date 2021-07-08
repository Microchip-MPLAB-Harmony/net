<#----------------------------------------------------------------------------
* Copyright (C) 2021 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
---------------------------------------------------------------------------->

<#if ((drvGmac.DRV_INTMAC_PHY_TYPE)?has_content) || ((drvPic32mEthmac.DRV_INTMAC_PHY_TYPE)?has_content) || ((drvEmac0.DRV_MAC_PHY_TYPE)?has_content) || ((drvEmac1.DRV_MAC_PHY_TYPE)?has_content)>
    
    <#assign emac_phy_type = TCPIP_EMAC_PHY_TYPE>
    <#assign use_phy_reset_callback = DRV_ETHPHY_USE_RESET_CALLBACK>
    <#assign phy_reset_callback = DRV_ETHPHY_RESET_CALLBACK>
    

    <#lt>/*** ETH PHY Initialization Data ***/
    <#if use_phy_reset_callback == true>
        <#if phy_reset_callback?has_content>
            <#lt>extern void ${phy_reset_callback}( const struct DRV_ETHPHY_OBJECT_BASE_TYPE* pBaseObj);
        </#if>
    </#if>
    <#lt>const DRV_ETHPHY_INIT tcpipPhyInitData_${emac_phy_type} =
    <#lt>{    
    <#lt>    .ethphyId               = TCPIP_INTMAC_MODULE_ID,
    <#lt>    .phyAddress             = TCPIP_INTMAC_PHY_ADDRESS,
    <#lt>    .phyFlags               = TCPIP_INTMAC_PHY_CONFIG_FLAGS,
    <#lt>    .pPhyObject             = &DRV_ETHPHY_OBJECT_${emac_phy_type},
    <#if use_phy_reset_callback == true && phy_reset_callback?has_content>
        <#lt>    .resetFunction          = ${phy_reset_callback},
    <#else>
        <#lt>    .resetFunction          = 0,
    </#if>
    <#if drvMiim.DRV_MIIM_USE_DRIVER?has_content && drvMiim.DRV_MIIM_USE_DRIVER == true >
        <#lt>    .pMiimObject            = &${drvMiim.DRV_MIIM_DRIVER_OBJECT},
        <#lt>    .pMiimInit              = &drvMiimInitData,
        <#lt>    .miimIndex              = DRV_MIIM_DRIVER_INDEX,
    <#else>
        <#lt>    .pMiimObject            = 0,
        <#lt>    .pMiimInit              = 0,
        <#lt>    .miimIndex              = 0,
    </#if>
    <#lt>};
</#if>