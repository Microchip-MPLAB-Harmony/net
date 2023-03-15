/***********************************************************************
TCP/IP Stack Ethernet PHY and MAC Driver Definitions Header File

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_ethernet.h

  Summary:
    Common Ethernet definitions file

  Description:
    This file provides the definition of commonly-used enumerations
    for use with the Ethernet PHY and Ethernet MAC Drivers.
  ***********************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2013-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _TCPIP_ETHERNET_H_
#define _TCPIP_ETHERNET_H_

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  


// *****************************************************************************
/* Ethernet Open Configuration Settings

  Summary:
    Supported open configuration flags for the Ethernet module (EthMACOpen).

  Description:
    This enumeration defines the various configuration options for the Ethernet
    module.  These values can be ORed together to create a configuration mask
    passed to the EthMACOpen routine.

  Remarks:
    When Auto-negotiation is specified:
        - If multiple capability flags are set (TCPIP_ETH_OPEN_FDUPLEX,
          TCPIP_ETH_OPEN_HDUPLEX, TCPIP_ETH_OPEN_100, TCPIP_ETH_OPEN_10 ) they are all advertised
          as this link side capabilities.
        - If no setting is passed, the lowest one is taken, i.e.,
          TCPIP_ETH_OPEN_HDUPLEX and TCPIP_ETH_OPEN_10.
        - Auto-MDIX requires Auto-Negotiation; TCPIP_ETH_OPEN_MDIX_NORM or
          TCPIP_ETH_OPEN_MDIX_SWAP setting irrelevant.

    When No Auto-negotiation is specified:
        - If multiple settings, the highest priority setting is taken, i.e.
          TCPIP_ETH_OPEN_FDUPLEX over TCPIP_ETH_OPEN_HDUPLEX and TCPIP_ETH_OPEN_100 over
          TCPIP_ETH_OPEN_10.
        - If no setting, the lowest setting is taken, i.e., TCPIP_ETH_OPEN_HDUPLEX and
          TCPIP_ETH_OPEN_10.
        - The MDIX is set based on the TCPIP_ETH_OPEN_MDIX_NORM/TCPIP_ETH_OPEN_MDIX_SWAP
          setting.
*/

typedef enum
{
    // Link capabilities flags:

    // Use auto negotiation. set the following flags to specify your choices
    TCPIP_ETH_OPEN_AUTO
        /*DOM-IGNORE-BEGIN*/ = 0x00000001 /*DOM-IGNORE-END*/,

    // Use full duplex or full duplex negotiation capability needed
    TCPIP_ETH_OPEN_FDUPLEX
        /*DOM-IGNORE-BEGIN*/ = 0x00000002 /*DOM-IGNORE-END*/,

    // Use half duplex or half duplex negotiation capability needed
    TCPIP_ETH_OPEN_HDUPLEX
        /*DOM-IGNORE-BEGIN*/ = 0x00000004 /*DOM-IGNORE-END*/,
            
    // Use 1000MBps or 1000MBps negotiation capability needed
    TCPIP_ETH_OPEN_1000
        /*DOM-IGNORE-BEGIN*/ = 0x00000008 /*DOM-IGNORE-END*/,
            
    // Use 100MBps or 100MBps negotiation capability needed
    TCPIP_ETH_OPEN_100
        /*DOM-IGNORE-BEGIN*/ = 0x00000010 /*DOM-IGNORE-END*/,

    // Use 10MBps or 10MBps negotiation capability needed
    TCPIP_ETH_OPEN_10
        /*DOM-IGNORE-BEGIN*/ = 0x00000020 /*DOM-IGNORE-END*/,

    // Allow huge packets RX/TX
    TCPIP_ETH_OPEN_HUGE_PKTS
        /*DOM-IGNORE-BEGIN*/ = 0x00000040 /*DOM-IGNORE-END*/,

    // Loopback at the MAC level
    TCPIP_ETH_OPEN_MAC_LOOPBACK
        /*DOM-IGNORE-BEGIN*/ = 0x00000080 /*DOM-IGNORE-END*/,

    /*When PHY is loopback, negotiation will be disabled!*/
    TCPIP_ETH_OPEN_PHY_LOOPBACK
        /*DOM-IGNORE-BEGIN*/ = 0x00000100 /*DOM-IGNORE-END*/,

    // Use Auto MDIX
    TCPIP_ETH_OPEN_MDIX_AUTO
        /*DOM-IGNORE-BEGIN*/ = 0x00000200 /*DOM-IGNORE-END*/,

    // Use normal MDIX when Auto MDIX disabled
    TCPIP_ETH_OPEN_MDIX_NORM
        /*DOM-IGNORE-BEGIN*/ = 0x00000400 /*DOM-IGNORE-END*/,

    // Use swapped MDIX when Auto MDIX disabled
    TCPIP_ETH_OPEN_MDIX_SWAP
        /*DOM-IGNORE-BEGIN*/ = 0x00000800 /*DOM-IGNORE-END*/,

    // RGMII/GMII/MII/RMII flags:
    // RGMII connection
    TCPIP_ETH_OPEN_RGMII
        /*DOM-IGNORE-BEGIN*/ = 0x00001000 /*DOM-IGNORE-END*/,
    // GMII connection
    TCPIP_ETH_OPEN_GMII
        /*DOM-IGNORE-BEGIN*/ = 0x00002000 /*DOM-IGNORE-END*/,
    // RMII connection
    TCPIP_ETH_OPEN_RMII
        /*DOM-IGNORE-BEGIN*/ = 0x00004000 /*DOM-IGNORE-END*/,

    // MII connection
    TCPIP_ETH_OPEN_MII
        /*DOM-IGNORE-BEGIN*/ = 0x00008000 /*DOM-IGNORE-END*/,

    // All capabilities default
    TCPIP_ETH_OPEN_DEFAULT = (TCPIP_ETH_OPEN_AUTO|TCPIP_ETH_OPEN_FDUPLEX|TCPIP_ETH_OPEN_HDUPLEX|
                        TCPIP_ETH_OPEN_100|TCPIP_ETH_OPEN_10|TCPIP_ETH_OPEN_MDIX_AUTO)

} TCPIP_ETH_OPEN_FLAGS; 




// *****************************************************************************
/* TCP/IP Ethernet Pause Types

  Summary:
    Defines the possible TCP/IP Ethernet pause types.

  Description:
    This enumeration defines the pause capabilities of the TCP/IP Ethernet.

  Remarks:
    The pause type is mainly supported over Ethernet links and takes part
    in the PHY link negotiation process.

    8 bits mask supported only!
*/

typedef enum
{
    // No PAUSE capabilities
    TCPIP_ETH_PAUSE_TYPE_NONE     /*DOM-IGNORE-BEGIN*/ = 0x0 /*DOM-IGNORE-END*/,

    // Supports symmetric PAUSE
    TCPIP_ETH_PAUSE_TYPE_PAUSE    /*DOM-IGNORE-BEGIN*/ = 0x1 /*DOM-IGNORE-END*/,

    // Supports ASM_DIR
    TCPIP_ETH_PAUSE_TYPE_ASM_DIR  /*DOM-IGNORE-BEGIN*/ = 0x2 /*DOM-IGNORE-END*/,

    // The previous two values converted to TX/RX capabilities:

    // Enable MAC TX pause support
    TCPIP_ETH_PAUSE_TYPE_EN_TX    /*DOM-IGNORE-BEGIN*/ = 0x4 /*DOM-IGNORE-END*/,

    // Enable MAC RX pause support
    TCPIP_ETH_PAUSE_TYPE_EN_RX    /*DOM-IGNORE-BEGIN*/ = 0x8 /*DOM-IGNORE-END*/,

    // All types of pause
    TCPIP_ETH_PAUSE_TYPE_ALL    = (TCPIP_ETH_PAUSE_TYPE_PAUSE|TCPIP_ETH_PAUSE_TYPE_ASM_DIR|
                           TCPIP_ETH_PAUSE_TYPE_EN_TX|TCPIP_ETH_PAUSE_TYPE_EN_RX),

} TCPIP_ETH_PAUSE_TYPE; 


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif//_TCPIP_ETHERNET_H_