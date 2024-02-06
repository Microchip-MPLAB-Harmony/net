<#--
Copyright (C) 2021-2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
-->
<#assign emac_phy_type = TCPIP_EMAC_PHY_TYPE>
<#assign use_phy_reset_callback = DRV_ETHPHY_USE_RESET_CALLBACK>
<#assign phy_reset_callback = DRV_ETHPHY_RESET_CALLBACK>
<#lt><#assign count = 0 >
<#if (tcpipStack??) >    
    <#lt><#list 0 ..(tcpipStack.TCPIP_STACK_NET_INTERFACE_NUM -1) as i >
        <#lt><#assign isIntMac = "tcpipStack.TCPIP_STACK_INT_MAC_IDX${i}"?eval> 
        <#lt><#if (isIntMac == true)>   
            <#if ((tcpipStack.TCPIP_DEVICE_FAMILY?has_content) && (tcpipStack.TCPIP_DEVICE_FAMILY  == "PIC32M"))>
                <#lt><#assign intMacName = "_ETH">
            <#else>
                <#lt><#assign intMacName = "tcpipStack.TCPIP_STACK_NET_INTERFACE_NAME_IDX${i}"?eval>
            </#if>  
            <#lt><#if intMacName + "_BASE_ADDRESS" == "${DRV_ETHPHY_PERIPHERAL_ID}">
                <#lt><#assign miimIdx = count>
            <#lt></#if>             
            <#lt><#assign count++ >
        <#lt></#if> 
    <#lt></#list>
<#else>
    <#lt><#assign miimIdx = count>
</#if>    
<#if (emac_phy_type  != "Dummy")>
<#lt>/*** ${emac_phy_type} PHY Driver Time-Out Initialization Data ***/
<#lt>DRV_ETHPHY_TMO drv${emac_phy_type?lower_case}Tmo = 
<#lt>{
<#lt>    .resetTmo = DRV_ETHPHY_${emac_phy_type}_RESET_CLR_TMO,
<#lt>    .aNegDoneTmo = DRV_ETHPHY_${emac_phy_type}_NEG_DONE_TMO,
<#lt>    .aNegInitTmo = DRV_ETHPHY_${emac_phy_type}_NEG_INIT_TMO,    
<#lt>};
</#if>    

<#lt>/*** ETH PHY Initialization Data ***/
<#if use_phy_reset_callback == true>
    <#if phy_reset_callback?has_content>
        <#lt>extern void ${phy_reset_callback}(const struct DRV_ETHPHY_OBJECT_BASE_TYPE* pBaseObj, DRV_HANDLE handle);
    </#if>
</#if>
<#lt>const DRV_ETHPHY_INIT tcpipPhyInitData_${emac_phy_type} =
<#lt>{    
<#if (emac_phy_type  != "Dummy")>
    <#lt>    .ethphyId               = DRV_${emac_phy_type}_PHY_PERIPHERAL_ID,
    <#lt>    .phyAddress             = DRV_${emac_phy_type}_PHY_ADDRESS,
    <#lt>    .phyFlags               = DRV_${emac_phy_type}_PHY_CONFIG_FLAGS,
    <#lt>    .pPhyObject             = &DRV_ETHPHY_OBJECT_${emac_phy_type},
    <#lt>    .ethphyTmo              = &drv${emac_phy_type?lower_case}Tmo,
    <#if drvMiim.DRV_MIIM_USE_DRIVER?has_content && drvMiim.DRV_MIIM_USE_DRIVER == true >
        <#lt>    .pMiimObject            = &${drvMiim.DRV_MIIM_DRIVER_OBJECT},
        <#lt>    .pMiimInit              = &drvMiimInitData_${miimIdx},
        <#lt>    .miimIndex              = ${miimIdx},
    <#else>
        <#lt>    .pMiimObject            = 0,
        <#lt>    .pMiimInit              = 0,
        <#lt>    .miimIndex              = 0,
    </#if>
<#else>
    <#lt>    .ethphyId               = 0,
    <#lt>    .phyAddress             = 0,
    <#lt>    .pPhyObject             = 0,
    <#lt>    .phyFlags               = 0,
    <#lt>    .pMiimObject            = 0,
    <#lt>    .pMiimInit              = 0,
    <#lt>    .miimIndex              = 0,
</#if>


<#if use_phy_reset_callback == true && phy_reset_callback?has_content>
    <#lt>    .resetFunction          = ${phy_reset_callback},
<#else>
    <#lt>    .resetFunction          = 0,
</#if>
<#lt>};

