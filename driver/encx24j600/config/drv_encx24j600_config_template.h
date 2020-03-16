/*******************************************************************************
  ENCX24J600 Driver Configuration Definitions for the Template Version

  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_config_template.h

  Summary:
    ENCX24J600 Driver configuration definitions template.

  Description:
    These definitions statically define the driver's mode of operation.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2013-2018 Microchip Technology Inc. and its subsidiaries.

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
*****************************************************************************/

//DOM-IGNORE-END

#ifndef _DRV_ENCX24J6600_CONFIG_TEMPLATE_H
#define _DRV_ENCX24J600_CONFIG_TEMPLATE_H


#error "This is a configuration template file.  Do not include it directly."


// *****************************************************************************
/* ENCX24J600 hardware instance configuration

  Summary:
    Selects the maximum number of hardware instances that can be supported by
    the dynamic driver.
  Description:
    This definition selects the maximum number of hardware instances that can be 
    supported by the dynamic driver.

  Remarks:
    Mandatory definition.
*/

#define DRV_ENCX24J600_INSTANCES_NUMBER                        1


/*********************************************************************
  Summary:
    Selects the maximum number of clients.
	
  Description:
    ENCX24J600 maximum number of clients
    
    This definition selects the maximum number of clients that the ENCX24J600
    driver can support at run-time.
	
  Remarks:
    Mandatory definition.
  *********************************************************************/

#define DRV_ENCX24J600_CLIENT_INSTANCES                          1


#endif // #ifndef _DRV_ENCX24J6600_CONFIG_TEMPLATE_H

/*******************************************************************************
 End of File
*/

