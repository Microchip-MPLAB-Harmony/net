/*******************************************************************
* FileName: mib.h
* This file was automatically generated on Fri Dec 14 2018 05:58:28 
* by mib2bib utility.
* This file contains 'C' defines for dynamic OIDs and AgentID only.
* Do not modify this file manually.
* Include this file in your application source file that handles SNMP callbacks and TRAP.

*******************************************************************/

/*
Copyright (C) 2012-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#define MICROCHIP      255		// This is an Agent ID for use in SNMPNotify() only.
#define SYS_UP_TIME      250			//43.6.1.2.1.1.3 :  READONLY TIME_TICKS.
#define SNMP_DEMO_TRAP      500			//43.6.1.4.1.17095.1.0.1 :  READONLY BYTE.
#define TRAP_RECEIVER_ID      1			//43.6.1.4.1.17095.1.2.1.1.1 :  READWRITE BYTE.
#define TRAP_RECEIVER_ENABLED      2			//43.6.1.4.1.17095.1.2.1.1.2 :  READWRITE BYTE.
#define TRAP_RECEIVER_IP      3			//43.6.1.4.1.17095.1.2.1.1.3 :  READWRITE IP_ADDRESS.
#define TRAP_COMMUNITY      4			//43.6.1.4.1.17095.1.2.1.1.4 :  READWRITE ASCII_STRING.
#define IPV6_TRAP_RECEIVER_ID      31			//43.6.1.4.1.17095.1.2.2.1.1 :  READWRITE BYTE.
#define IPV6_TRAP_ENABLED      32			//43.6.1.4.1.17095.1.2.2.1.2 :  READWRITE BYTE.
#define IPV6_TRAP_RECEIVER_IP      33			//43.6.1.4.1.17095.1.2.2.1.3 :  READWRITE OCTET_STRING.
#define IPV6_TRAP_COMMUNITY      34			//43.6.1.4.1.17095.1.2.2.1.4 :  READWRITE ASCII_STRING.
#define LED_D5      5			//43.6.1.4.1.17095.1.3.1 :  READWRITE BYTE.
#define LED_D6      6			//43.6.1.4.1.17095.1.3.2 :  READWRITE BYTE.
#define PUSH_BUTTON      7			//43.6.1.4.1.17095.1.3.3 :  READONLY BYTE.
#define ANALOG_POT0      8			//43.6.1.4.1.17095.1.3.4 :  READONLY WORD.
#define LCD_DISPLAY      10			//43.6.1.4.1.17095.1.3.6 :  READWRITE ASCII_STRING.
#define USM_INDEX_ID      11			//43.6.1.4.1.17095.1.4.1.1.1 :  READONLY BYTE.
#define USER_SECURITY_NAME      12			//43.6.1.4.1.17095.1.4.1.1.2 :  READWRITE ASCII_STRING.
#define USM_AUTH_PROT      13			//43.6.1.4.1.17095.1.4.1.1.3 :  READWRITE BYTE.
#define USM_AUTH_KEY      14			//43.6.1.4.1.17095.1.4.1.1.4 :  READWRITE ASCII_STRING.
#define USM_PRIV_PROT      15			//43.6.1.4.1.17095.1.4.1.1.5 :  READWRITE BYTE.
#define USM_PRIV_KEY      16			//43.6.1.4.1.17095.1.4.1.1.6 :  READWRITE ASCII_STRING.
#define SNMP_TARGET_INDEX_ID      17			//43.6.1.4.1.17095.1.5.1.1.1 :  READONLY BYTE.
#define SNMP_TARGET_MP_MODEL      18			//43.6.1.4.1.17095.1.5.1.1.2 :  READWRITE BYTE.
#define SNMP_TARGET_SECURITY_MODEL      19			//43.6.1.4.1.17095.1.5.1.1.3 :  READWRITE BYTE.
#define SNMP_TARGET_SECURITY_NAME      20			//43.6.1.4.1.17095.1.5.1.1.4 :  READWRITE ASCII_STRING.
#define SNMP_TARGET_SECURITY_LEVEL      21			//43.6.1.4.1.17095.1.5.1.1.5 :  READWRITE BYTE.
#define ENGINE_ID      249			//43.6.1.6.3.10.2.1.1 :  READONLY ASCII_STRING.
#define ENGINE_BOOT      248			//43.6.1.6.3.10.2.1.2 :  READONLY DWORD.
#define ENGINE_TIME      247			//43.6.1.6.3.10.2.1.3 :  READONLY DWORD.
#define ENGINE_MAX_MSG      246			//43.6.1.6.3.10.2.1.4 :  READONLY WORD.
