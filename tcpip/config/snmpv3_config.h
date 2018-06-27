/*******************************************************************************
  Simple Network Management Protocol (SNMPv3) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    snmpv3_config.h

  Summary:
    SNMPv3 configuration file
    
  Description:
    This file contains the SNMPv3 module configuration options    
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2011 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END
#ifndef _SNMPv3_CONFIG_H_
#define _SNMPv3_CONFIG_H_

// Maximum number of SNMPv3 users.
// User Security Model should have at least 1 user. Default is 3.
#define TCPIP_SNMPV3_USM_MAX_USER	3

// Maximum size for SNMPv3 User Security Name length.
#define TCPIP_SNMPV3_USER_SECURITY_NAME_LEN (16)

// User security name length for memory validation
#define TCPIP_SNMPV3_USER_SECURITY_NAME_LEN_MEM_USE (TCPIP_SNMPV3_USER_SECURITY_NAME_LEN+1)

// SNMPv3 Authentication Localized password key length size
#define TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN	(20)

// SNMPv3 authentication localized Key length for memory validation
#define TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN_MEM_USE (TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN+1)

// SNMPv3 Privacy Password key length size
#define TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN	(20)

// SNMPv3 privacy key length size for memory validation
#define TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN_MEM_USE (TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN+1)

#endif  // _SNMPv3_CONFIG_H_
