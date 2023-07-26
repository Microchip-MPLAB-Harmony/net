<#--
/*******************************************************************************
  MIIM Driver Initialization File

  File Name:
    drv_miim_init.c.ftl

  Summary:
    This file contains source code necessary to initialize the MIIM driver.

  Description:
    This file contains source code necessary to initialize the system.  It
    implements the "SYS_Initialize" function, configuration bits, and allocates
    any necessary global system resources, such as the systemObjects structure
    that contains the object handles to all the MPLAB Harmony module objects in
    the system.
 *******************************************************************************/
-->

<#--
Copyright (C) 2016-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

<#if DRV_MIIM_USE_DRIVER == true>
    <#if DRV_MIIM_INSTANCES_NUMBER gte 1>
        <#lt><#list 0..(DRV_MIIM_INSTANCES_NUMBER - 1) as i >
        <#lt>   /* Initialize the MIIM Driver Instance ${i}*/
        <#lt>   sysObj.drvMiim_${i} = ${DRV_MIIM_DRIVER_OBJECT}.DRV_MIIM_Initialize(DRV_MIIM_DRIVER_INDEX_${i}, (const SYS_MODULE_INIT *) &drvMiimInitData_${i}); 
        <#lt></#list>
    </#if>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
