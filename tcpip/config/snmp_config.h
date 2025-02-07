/*******************************************************************************
  Simple Network Management Protocol (SNMP) Configuration file

  Company:
    Microchip Technology Inc.
    
  File Name:
    snmp_config.h

  Summary:
    SNMP configuration file

  Description:
    This file contains the SNMP module configuration options

    This file is not part of the project, it is a configuration template file only. 
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2011-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#ifndef _SNMP_CONFIG_H_
#define _SNMP_CONFIG_H_

// SNMP agent information

//Trap information.
//This macro will be used to avoid SNMP OID memory buffer corruption 
#define TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN_MEM_USE   (8)

//This table maintains list of interested receivers
//who should receive notifications when some interesting
//event occurs.
#define TCPIP_SNMP_TRAP_TABLE_SIZE         (2)

//The maximum size of TRAP community string length
#define TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN       (TCPIP_SNMP_TRAP_COMMUNITY_MAX_LEN_MEM_USE+1)

// SNMP task processing rate, in milli-seconds.
// The SNMP module will process a timer event with this rate
// for processing its own state machine, etc.
// The default value is 200 milliseconds.
// The lower the rate (higher the frequency) the higher the module priority
// and higher module performance can be obtained
// The value cannot be lower than the TCPIP_STACK_TICK_RATE.
#define TCPIP_SNMP_TASK_PROCESS_RATE    (200)

// The Microchip mib2bib.jar compiler is used to convert the Microchip MIB script to binary format 
// and it is compatible with the Microchip SNMP agent. which is written in ASCII format.
// Name of the bib file for SNMP is snmp.bib.
#define TCPIP_SNMP_BIB_FILE_NAME        "snmp.bib"

// Maximum length for the OID String. Change this to match your OID string length.
#define TCPIP_SNMP_OID_MAX_LEN          (18)

// The maximum length in octets of an SNMP message which this SNMP agent able to process.
// As per RFC 3411 snmpEngineMaxMessageSize and RFC 1157 ( section 4- protocol specification )
// and implementation  supports more than 480 whenever feasible.
//It should be divisible by 16
#define TCPIP_SNMP_MAX_MSG_SIZE     480

// Update the Non record id OID value which is part of CustomSnmpDemoApp.c file.
// This is the maximum size for gSnmpNonMibRecInfo[] which is the list of static variable Parent 
// OIDs which are not part of mib.h file. This structure is used to restrict access to static 
// variables of SNMPv3 OIDs from SNMPv2c and SNMPv1 version.
// With SNMPv3 all the OIDs accessible but when we are using SNMPv2c version , static variables of the SNMPv3
// cannot be accessible with SNMP version v2c.
// SNMP agent supports both SMIv1 and SMIv2 standard and 
// snmp.mib has been updated with respect to SMIV2 standard and it also includes
// MODULE-IDENTITY ( number 1)after ENTERPRISE-ID.
#define TCPIP_SNMP_MAX_NON_REC_ID_OID  3

// This is the maximum length for community string.
// Application must ensure that this length is observed.
// SNMP module adds one byte extra after TCPIP_SNMP_COMMUNITY_MAX_LEN
// for adding '\0' NULL character.
#define TCPIP_SNMP_COMMUNITY_MAX_LEN    (8u)

// Specifying more strings than TCPIP_SNMP_MAX_COMMUNITY_SUPPORT will result in the
// later strings being ignored (but still wasting program memory).  Specifying
// fewer strings is legal, as long as at least one is present.
#define TCPIP_SNMP_MAX_COMMUNITY_SUPPORT    (3u)

// Maximum length for SNMP Trap community name 
#define TCPIP_SNMP_NOTIFY_COMMUNITY_LEN (TCPIP_SNMP_COMMUNITY_MAX_LEN)


#endif  // _SNMP_CONFIG_H_
