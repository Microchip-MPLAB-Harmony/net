<#--
/*******************************************************************************
  MIIM Driver Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_miim.h.ftl

  Summary:
    MIIM Driver Freemarker Template File

  Description:

*******************************************************************************/
-->

<#----------------------------------------------------------------------------
 Copyright (C) 2016-2018 Microchip Technology Inc. and its subsidiaries.

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
----------------------------------------------------------------------------->


<#if DRV_MIIM_USE_DRIVER>
/*** MIIM Driver Configuration ***/
#define DRV_MIIM_ETH_MODULE_ID              ${DRV_MIIM_ETH_MODULE_ID}
#define DRV_MIIM_INSTANCES_NUMBER           ${DRV_MIIM_INSTANCES_NUMBER}
#define DRV_MIIM_INSTANCE_OPERATIONS        ${DRV_MIIM_INSTANCE_OPERATIONS}
#define DRV_MIIM_INSTANCE_CLIENTS           ${DRV_MIIM_INSTANCE_CLIENTS}
<#if DRV_MIIM_CLIENT_OP_PROTECTION == true>
#define DRV_MIIM_CLIENT_OP_PROTECTION   true
<#else>
#define DRV_MIIM_CLIENT_OP_PROTECTION   false
</#if>
<#if DRV_MIIM_COMMANDS == true>
#define DRV_MIIM_COMMANDS   true
<#else>
#define DRV_MIIM_COMMANDS   false
</#if>
#define DRV_MIIM_DRIVER_OBJECT              ${DRV_MIIM_DRIVER_OBJECT}
#define DRV_MIIM_DRIVER_INDEX               DRV_MIIM_INDEX_${DRV_MIIM_DRIVER_INDEX}              
</#if>
