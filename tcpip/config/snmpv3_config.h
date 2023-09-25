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

    This file is not part of the project, it is a configuration template file only. 
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2011-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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








// DOM-IGNORE-END
#ifndef _SNMPv3_CONFIG_H_
#define _SNMPv3_CONFIG_H_

// Maximum number of SNMPv3 users.
// User Security Model should have at least 1 user. Default is 3.
#define TCPIP_SNMPV3_USM_MAX_USER   3

// Maximum size for SNMPv3 User Security Name length.
#define TCPIP_SNMPV3_USER_SECURITY_NAME_LEN (16)

// User security name length for memory validation
#define TCPIP_SNMPV3_USER_SECURITY_NAME_LEN_MEM_USE (TCPIP_SNMPV3_USER_SECURITY_NAME_LEN+1)

// SNMPv3 Authentication Localized password key length size
#define TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN    (20)

// SNMPv3 authentication localized Key length for memory validation
#define TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN_MEM_USE (TCPIP_SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN+1)

// SNMPv3 Privacy Password key length size
#define TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN    (20)

// SNMPv3 privacy key length size for memory validation
#define TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN_MEM_USE (TCPIP_SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN+1)

#endif  // _SNMPv3_CONFIG_H_
