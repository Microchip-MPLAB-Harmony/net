/*******************************************************************************
  ETHMAC Driver Configuration Definitions for the template version

  Company:
    Microchip Technology Inc.

  File Name:
    drv_ethmac_config.h

  Summary:
    Ethernet MAC driver configuration definitions template.

  Description:
    These definitions statically define the driver's mode of operation.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2013-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _DRV_ETHMAC_CONFIG_H
#define _DRV_ETHMAC_CONFIG_H


// *****************************************************************************
/* Ethernet MAC hardware instance configuration

  Summary:
    Selects the maximum number of hardware instances that can be supported by
    the dynamic driver.

  Description:
    This definition selects the maximum number of hardware instances that can be
    supported by the dynamic driver. Not defining it means using a static driver.

  Remarks:
    None.
*/

#define DRV_ETHMAC_INSTANCES_NUMBER  1


// *****************************************************************************
/* Ethernet MAC Maximum Number of Clients

  Summary:
    Selects the maximum number of clients.

  Description:
    This definition select the maximum number of clients that the Ethernet MAC
    driver can support at run time.


  Remarks:
    The MAC driver is not a true multi-client driver.  
    Under normal usage, the only client of the MAC driver is the TCP/IP stack.
    After the MAC driver provided an DRV_HANDLE as a result of an Open operation,
    any other attempt to call Open will return a invalid handle.
    Default value should be 1.
    
    However, for allowing other modules to interface directly with the MAC driver
    while the TCP/IP stack currently uses the the MAC driver this symbol can have
    a value greater than 1.
    But the returned handle is the same one as the TCP/IP stack uses.
    
*/

#define DRV_ETHMAC_CLIENTS_NUMBER  1


// *****************************************************************************
/* Ethernet MAC Static Index Selection

  Summary:
    Ethernet MAC static index selection.

  Description:
    This definition selects the Ethernet MAC static index for the driver object 
    reference

  Remarks:
    This index is required to make a reference to the driver object.
*/

#define DRV_ETHMAC_INDEX  DRV_ETHMAC_INDEX_1


// *****************************************************************************
/* Ethernet MAC Interrupt And Polled Mode Operation Control

  Summary:
    Controls operation of the driver in the interrupt or polled mode.

  Description:
    This macro controls the operation of the driver in the interrupt
    mode of operation. The possible values of this macro are:

    - true  - Select if interrupt mode of timer operation is desired

    - false - Select if polling mode of timer operation is desired

    Not defining this option to true or false will result in a build error.

  Remarks:
    None.
*/

#define DRV_ETHMAC_INTERRUPT_MODE  true


// *****************************************************************************
// *****************************************************************************
// Section: Initialization Overrides
// *****************************************************************************
// *****************************************************************************
/* This section defines the initialization overrides */

// *****************************************************************************
/* Ethernet MAC Peripheral ID Selection

  Summary:
    Defines an override of the peripheral ID.

  Description:
    Defines an override of the peripheral ID, using macros.

  Remarks:

    Note: Some devices also support ETHMAC_ID_0
*/

#define DRV_ETHMAC_PERIPHERAL_ID  ETHMAC_ID_1


// *****************************************************************************
/* Ethernet MAC Interrupt Source

  Summary:
    Defines an override of the interrupt source in case of static driver.

  Description:
    Defines an override of the interrupt source in case of static driver.

  Remarks:
    Refer to the INT PLIB document for more information on INT_SOURCE
    enumeration.

*/

#define DRV_ETHMAC_INTERRUPT_SOURCE  INT_SOURCE_ETH_1


// *****************************************************************************
/* Ethernet MAC power state configuration

  Summary:
    Defines an override of the power state of the Ethernet MAC driver.

  Description:
    Defines an override of the power state of the Ethernet MAC driver.

  Remarks:
    Note: This feature may not be available in the device or the Ethernet MAC module
    selected.
*/

#define DRV_ETHMAC_POWER_STATE  SYS_MODULE_POWER_IDLE_STOP


#endif // #ifndef _DRV_ETHMAC_CONFIG_H

/*******************************************************************************
 End of File
*/

