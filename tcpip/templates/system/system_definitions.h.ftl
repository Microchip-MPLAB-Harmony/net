<#----------------------------------------------------------------------------
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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

#include "library/tcpip/tcpip.h"
<#if (drvGmac.TCPIP_USE_ETH_MAC)?has_content && (drvGmac.TCPIP_USE_ETH_MAC) == true>
#include "driver/gmac/drv_gmac.h"
</#if>
<#if (drvPic32mEthmac.TCPIP_USE_ETH_MAC)?has_content && (drvPic32mEthmac.TCPIP_USE_ETH_MAC) == true>
#include "driver/ethmac/drv_ethmac.h"
</#if>

<#if ((drvEmac0.TCPIP_USE_EMAC0)?has_content && (drvEmac0.TCPIP_USE_EMAC0 == true)) || ((drvEmac1.TCPIP_USE_EMAC1)?has_content && (drvEmac1.TCPIP_USE_EMAC1 == true))>
#include "driver/emac/drv_emac.h"
</#if>

<#if (drvMiim.DRV_MIIM_USE_DRIVER)?has_content && (drvMiim.DRV_MIIM_USE_DRIVER) == true>
#include "driver/miim/drv_miim.h"
</#if>
