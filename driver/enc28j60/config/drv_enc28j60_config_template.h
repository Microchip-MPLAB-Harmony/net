/*******************************************************************************
  enc28j60 Driver Configuration Definitions for the Template Version

  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_config_template.h

  Summary:
    enc28j60 Driver configuration definitions template.

  Description:
    These definitions statically define the driver's mode of operation.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2015-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

//DOM-IGNORE-END

#ifndef _DRV_ENC28J60_CONFIG_TEMPLATE_H
#define _DRV_ENC28J60_CONFIG_TEMPLATE_H


#error "This is a configuration template file.  Do not include it directly."


// *****************************************************************************
/* enc28j60 hardware instance configuration

  Summary:
    Selects the maximum number of hardware instances that can be supported by
    the dynamic driver.
  Description:
    This definition selects the maximum number of hardware instances that can be 
    supported by the dynamic driver.

  Remarks:
    Mandatory definition.
*/

#define DRV_ENC28J60_INSTANCES_NUMBER                        1

/*************************************************************************
  Summary:
    Selects the maximum number of clients with each instance
  Description:
    ENC28J60 maximum number of clients
    
    For example, DRV_ENC28J60_CLIENT_INSTANCES_IDX0 selects the maximum
    number of clients the ENC28J60 driver can support at run-time for
    instance 0
  Remarks:
    The instance number is suffixed with DRV_ENC28J60_CLIENT_INSTANCES_IDX
  *************************************************************************/

#define DRV_ENC28J60_CLIENT_INSTANCES_IDXx                          1


#endif // #ifndef _DRV_ENC28J60_CONFIG_TEMPLATE_H

/*******************************************************************************
 End of File
*/

