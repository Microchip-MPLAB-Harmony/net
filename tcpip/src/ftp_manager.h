/*******************************************************************************
  FTP Server Internal Stack API

  Company:
    Microchip Technology Inc.
    
  File Name:
    ftp_manager.h

  Summary:

  Description:

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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








// DOM-IGNORE-END

#ifndef __FTP_MANAGER_H_
#define __FTP_MANAGER_H_

/*********************************************************************
  Function:  bool TCPIP_FTP_ServerInitialize(const TCPIP_STACK_MODULE_CTRL* const stackData, const TCPIP_FTP_MODULE_CONFIG* ftpData)

  PreCondition:    TCP module is already initialized.

  Input:           None

  Output:          true is success
                   false otherwise

  Side Effects:    None

  Overview:        Initializes internal variables of FTP

  Note:
 */
bool TCPIP_FTP_ServerInitialize(const TCPIP_STACK_MODULE_CTRL* const stackData,
                          const TCPIP_FTP_MODULE_CONFIG* ftpData);


/*********************************************************************
  Function:        void TCPIP_FTP_ServerDeinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData)

  PreCondition:    None

  Input:           None

  Output:          None

  Side Effects:    None

  Overview:        DeInitializes internal variables of FTP

  Note:
 */
void TCPIP_FTP_ServerDeinitialize(const TCPIP_STACK_MODULE_CTRL* const stackData);



#endif  // __FTP_MANAGER_H_

