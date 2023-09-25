/*******************************************************************************
  Multiple MAC Module implementation for Microchip Stack

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_mac_object.h

  Summary:
   Multiple MAC module API definitions.
    
  Description:
   This file provides the API definitions for Multiple MAC module implementation.
*******************************************************************************/
// DOM-IGNORE-BEGIN
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








// DOM-IGNORE-END

#ifndef _TCPIP_MAC_OBJECT_H_ 
#define _TCPIP_MAC_OBJECT_H_ 

#include "tcpip/tcpip_mac.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  

// *****************************************************************************
// *****************************************************************************
// Section: MPLAB Harmony MAC Parameterized Interface Implementation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************

typedef struct TCPIP_MAC_OBJECT_TYPE
{
    uint16_t            macId;          // one of the TCPIP_MODULE_MAC_ID IDs
    uint8_t             macType;        // a TCPIP_MAC_TYPE value 
    uint8_t             reserved;       // reserved for future use
    const char*         macName;        // PIC32INT, MRF24WN, etc
    SYS_MODULE_OBJ      (*TCPIP_MAC_Initialize)(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);
    void                (*TCPIP_MAC_Deinitialize)(SYS_MODULE_OBJ object);
    void                (*TCPIP_MAC_Reinitialize)(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init);
    SYS_STATUS          (*TCPIP_MAC_Status)( SYS_MODULE_OBJ object );
    void                (*TCPIP_MAC_Tasks)( SYS_MODULE_OBJ object );
    DRV_HANDLE          (*TCPIP_MAC_Open)(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);
    void                (*TCPIP_MAC_Close)(DRV_HANDLE hMac);
    bool                (*TCPIP_MAC_LinkCheck)(DRV_HANDLE hMac);
    TCPIP_MAC_RES       (*TCPIP_MAC_RxFilterHashTableEntrySet)(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr);
    bool                (*TCPIP_MAC_PowerMode)(DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode);
    TCPIP_MAC_RES       (*TCPIP_MAC_PacketTx)(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket);
    TCPIP_MAC_PACKET*   (*TCPIP_MAC_PacketRx)(DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, TCPIP_MAC_PACKET_RX_STAT* pPktStat);
    TCPIP_MAC_RES       (*TCPIP_MAC_Process)(DRV_HANDLE hMac);
    TCPIP_MAC_RES       (*TCPIP_MAC_StatisticsGet)(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics);
    TCPIP_MAC_RES       (*TCPIP_MAC_ParametersGet)(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams);
    TCPIP_MAC_RES       (*TCPIP_MAC_RegisterStatisticsGet)(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries);
    size_t              (*TCPIP_MAC_ConfigGet)(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize);

    bool                (*TCPIP_MAC_EventMaskSet)(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable);
    bool                (*TCPIP_MAC_EventAcknowledge)(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents);
    TCPIP_MAC_EVENT     (*TCPIP_MAC_EventPendingGet)(DRV_HANDLE hMac);

}TCPIP_MAC_OBJECT;        // TCPIP MAC object descriptor

typedef struct
{
    const TCPIP_MAC_OBJECT* pObj;       // associated object
                                        // pointer to the object here is intended to allow
                                        // multiple MAC objects of the same type
                                        // to share an unique const object table
    void*               mac_data[];     // specific MAC object data
}TCPIP_MAC_DCPT; 
    
// supported MAC objects
extern const TCPIP_MAC_OBJECT DRV_ETHMAC_PIC32MACObject;
extern const TCPIP_MAC_OBJECT DRV_GMAC_Object;
extern const TCPIP_MAC_OBJECT DRV_GMAC0_Object;
extern const TCPIP_MAC_OBJECT DRV_GMAC1_Object;
extern const TCPIP_MAC_OBJECT DRV_EMAC0_Object;
extern const TCPIP_MAC_OBJECT DRV_EMAC1_Object;
extern const TCPIP_MAC_OBJECT DRV_ENCX24J600_MACObject;
extern const TCPIP_MAC_OBJECT DRV_ENC28J60_MACObject;
extern const TCPIP_MAC_OBJECT DRV_LAN865X_MACObject;
extern const TCPIP_MAC_OBJECT WDRV_MRF24WN_MACObject;
extern const TCPIP_MAC_OBJECT WDRV_WINC_MACObject;
extern const TCPIP_MAC_OBJECT WDRV_WILC1000_MACObject;
extern const TCPIP_MAC_OBJECT DRV_PIC32WK_MACObject;
extern const TCPIP_MAC_OBJECT WDRV_PIC32MZW1_MACObject;
extern const TCPIP_MAC_OBJECT DRV_PPP_MACObject;
extern const TCPIP_MAC_OBJECT DRV_G3ADP_MACObject;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  // _TCPIP_MAC_OBJECT_H_ 
