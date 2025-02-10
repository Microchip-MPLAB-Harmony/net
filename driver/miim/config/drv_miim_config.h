/*******************************************************************************
  MIIM Driver Configuration Definitions for the Template Version

  Company:
    Microchip Technology Inc.

  File Name:
    drv_miim_config.h

  Summary:
    MIIM driver configuration definitions template.

  Description:
    These definitions statically define the driver's mode of operation.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2016-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _DRV_MIIM_CONFIG_H
#define _DRV_MIIM_CONFIG_H


// *****************************************************************************
// *****************************************************************************
// Section: MIIM Configuration
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MIIM hardware instance configuration

  Summary:
    Selects the maximum number of hardware instances that can be supported by
    the dynamic driver.

  Description:
    This definition selects the maximum number of hardware instances that can be
    supported by the dynamic driver.
    Usually set to 1.

  Remarks:
    None.
*/

#define DRV_MIIM_INSTANCES_NUMBER                1

// *****************************************************************************
/* MIIM instance operations

  Summary:
    Selects the maximum number of simultaneous operations for an instance.

  Description:
    This definition selects the maximum number of simultaneous operations
    that can be supported by this driver.
    Note that this represents operations for all clients

  Remarks:
    None.
*/

#define DRV_MIIM_INSTANCE_OPERATIONS        4


// *****************************************************************************
/* MIIM number of clients

  Summary:
    Selects the maximum number of clients.

  Description:
    This definition select the MIIM Maximum Number of Clients per instance.

  Remarks:
    By default the 1st MIIM client is the DRV_ETHPHY.
    An extra client is allowed.
  *************************************************************************/

#define DRV_MIIM_INSTANCE_CLIENTS                2

// *****************************************************************************
/* MIIM client Operation Protection

  Summary:
    Enables/Disables Client Operation Protection feature.

  Description:
    Because of the recirculation of the operation handles and client handles
    the possibility exists that a misbehaved client inadvertently gets the results
    of a previous completed operations that now belongs to a different client.
    When this feature is enabled, extra protection is added for an operation handle
    to uniquely identify a client that has started the operation and extra check is done
    that operation belongs to the client that asks for the result.


  Remarks:
    Set the value to 1 to enable, 0 to disable the feature.

    Enabling this feature requires a small overhead in code and data size.

  *************************************************************************/

#define DRV_MIIM_CLIENT_OP_PROTECTION       0

// *****************************************************************************
/* MIIM PHY Commands

  Summary:
    Enables/Disables MIIM commands feature.

  Description:
    Adds a MIIM command to the TCP/IP command menu allowing to read/write
    a PHY register.

  Remarks:
    Set the value to 1 to enable, 0 to disable the feature.

    Currently the MIIM commands are integrated in the TCP/IP commands.
    To have the MIIM commands available the TCP/IP commands need to be enabled.

  *************************************************************************/

#define DRV_MIIM_COMMANDS       0


#endif // #ifndef _DRV_MIIM_CONFIG_H

/*******************************************************************************
 End of File
*/

