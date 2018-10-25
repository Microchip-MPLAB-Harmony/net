/*******************************************************************************
  Link Layer Discovery Protocol (LLDP)

  Summary:
    Module for Microchip TCP/IP Stack

  Description:
    - LLDP implementation for Microchip TCP/IP stack
*******************************************************************************/

/*******************************************************************************
File Name:  lldp_tlv.c
Copyright © 2014 released Microchip Technology Inc.  All rights
reserved.

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

#include "tcpip/src/tcpip_private.h"
#include "lldp_private.h"
#include "lldp_tlv.h"


// proto
//

// TLVs creation

// creation TLVs flags
// multiple flags could be or-ed
// 8 bit only supported for now
typedef enum
{
    TCPIP_LLDP_CREATE_NO_SUBTYPE    = 0x01,     // some TLVs do not take a subtype filed (TTL for example)

}TCPIP_LLDP_CREATE_FLAGS;
// structure for the creation of a TLV frame
//
typedef struct _tag_createTLVFrame_t
{
    uint8_t                 flags;          // create flags
    uint8_t                 type;           // TCPIP_LLDP_TLV_TYPE value
    uint8_t                 subType;        // specific sub type
    uint8_t                 ouiDataSize;    // length of the OUI type, normally 3 bytes
    const uint8_t*          pOuiData;       // OUI data, inserted at the beg of the TLV, just before the subType
    const uint8_t*          pTlvData;       // pointer to the TLV data to be inserted after the OUI field 
    size_t                  tlvDataSize;    // size of the TLV specific data ; 
    // TLV creation function
    // returns the size needed for TLV or -1 if TLV cannot be created
    // pDestBuff could be 0, if only size needed;
    // if pDestBuff is not zero, it will write the LV at pDestBuff
    // Notes:
    //  1. The TLV is created as follows:
    //      - the standard type + length (2 bytes)
    //      - OUI, if ouiDataSize != 0 (ouiDataSize bytes)
    //      - subType, if TCPIP_LLDP_CREATE_NO_SUBTYPE flag is not set - 1 byte
    //      - TLV data - supplied as parameter, if tlvDataSize != 0 (tlvDataSize bytes)
    //
    //
    //      - the TLV length is calculated from the parameter data
    //
    //  2. the function checks for enough space in the destination buffer
    //  3. The ouiDataSize and pOuiData should be consistent: either both 0 or both valid values.
    //     No check is done for combinations like pOuiData == 0 but ouiDataSize != 0 and the like.
    //  4. The tlvDataSize and pTlvData should be consistent: either both 0 or both valid values.
    //     No check is done for combinations like pTlvData == 0 but tlvDataSize != 0 and the like.
    //
    //
    //
    size_t                  (*createFnc)(const struct _tag_createTLVFrame_t*, uint8_t* pDestBuff, size_t destBuffSize);
}createTLVFrame_t;

static uint8_t* tl_tlvConstruct(TCPIP_LLDP_TLV_TYPE type, uint16_t tlvLen, uint8_t* pBuff);
static size_t createTLVFrame(const createTLVFrame_t* pFrame, uint8_t* pDestBuff, size_t destBuffSize);
static void prepCreateTLVFrame(void);


// TLV processing
static TCPIP_LLDP_TLV_PROCESS_RES    processEndTlv(TCPIP_LLDP_TLV* pTlv, uint8_t tlvType, uint16_t tlvLen, TCPIP_LLDP_TLV_TYPE lastType);
static TCPIP_LLDP_TLV_PROCESS_RES    processChassisIdTlv(TCPIP_LLDP_TLV* pTlv, uint8_t tlvType, uint16_t tlvLen, TCPIP_LLDP_TLV_TYPE lastType);
static TCPIP_LLDP_TLV_PROCESS_RES    processPortIdTlv(TCPIP_LLDP_TLV* pTlv, uint8_t tlvType, uint16_t tlvLen, TCPIP_LLDP_TLV_TYPE lastType);
static TCPIP_LLDP_TLV_PROCESS_RES    processTTLTlv(TCPIP_LLDP_TLV* pTlv, uint8_t tlvType, uint16_t tlvLen, TCPIP_LLDP_TLV_TYPE lastType);


static TCPIP_LLDP_TLV_PROCESS_RES    processOptionalTlv(TCPIP_LLDP_TLV* pTlv, uint8_t tlvType, uint16_t tlvLen, TCPIP_LLDP_TLV_TYPE lastType);

static TCPIP_LLDP_TLV_PROCESS_RES    processOrgSpecificTlv(TCPIP_LLDP_TLV* pTlv, uint8_t tlvType, uint16_t tlvLen, TCPIP_LLDP_TLV_TYPE lastType);

static TCPIP_LLDP_TLV_PROCESS_RES processCiscoPowerTlv(uint8_t* pData, uint16_t tlvLen, uint32_t oui, uint8_t orgSubType);
static TCPIP_LLDP_TLV_PROCESS_RES processIEEE3PowerTlv(uint8_t* pData, uint16_t tlvLen, uint32_t oui, uint8_t orgSubType);


// Helpers
static void swapEvenBytes(uint8_t *buff, uint8_t len);

// Data
// TLV construct
//
// this chassis MAC address
static uint8_t chassisMacAddr[sizeof(TCPIP_MAC_ADDR)]; 

// this port TTL
static union
{
    uint8_t     ttl[2];
    uint16_t    txTTL;
}portTTL;


// Cisco OUI and data
static uint8_t cisco_Oui[ORG_SPECIFIC_OUI_LENGTH] = { (uint8_t)(CISCO_OUI >> 16), (uint8_t)(CISCO_OUI >> 8), (uint8_t)(CISCO_OUI)};
static uint8_t cisco_Mdi[1] = {0x05};

// IEEE3 OUI and data
static uint8_t ieee3_Oui[ORG_SPECIFIC_OUI_LENGTH] = { (uint8_t)(IEEE_3_OUI >> 16), (uint8_t)(IEEE_3_OUI >> 8), (uint8_t)(IEEE_3_OUI)};
                            // class pwrP  pwrC  type  reqP  reqP  allP  allP
static uint8_t ieee3_Mdi[8] = {0x0f, 0x01, 0x05, 0x53, 0x00, 0x82, 0x00, 0xff};


// TIA OUI and data
static uint8_t tia_Oui[ORG_SPECIFIC_OUI_LENGTH] = { (uint8_t)(TIA_OUI >> 16), (uint8_t)(TIA_OUI >> 8), (uint8_t)(TIA_OUI)};
static uint8_t tia_MedCap[3] = {0x00, 0x11, 0x04};
static uint8_t tia_Med[3] = {0x51, 0x00, 0xff};


// IEEE1 OUI - not used 
// static uint8_t ieee1_Oui[ORG_SPECIFIC_OUI_LENGTH] = { (uint8_t)(IEEE_1_OUI >> 16), (uint8_t)(IEEE_1_OUI >> 8), (uint8_t)(IEEE_1_OUI)};

// this port interface - MAC address
static const TCPIP_MAC_ADDR portInterface = { {0x50, 0x4f, 0x45, 0x20, 0x50, 0x44} };

// the mib storing the info 
static TCPIP_LLDP_MIB_t           mib;

// processing flags
static TCPIP_LLDP_ORG_FLAGS    orgProcFlags;

// table containing the entries for creating all the TLV frames
static const createTLVFrame_t lldpCreateTLVFrameTbl[] = 
{
    // Mandatory/Fixed TLVs at the beginning of any LLDPDU
    // The order of these TLVs cannot be changed!
    // 
    // flags    type                    subType                 ouiDataSize                 pOuiData        pTlvData        tlvDataSize             createFnc
    {0,         CHASSIS_ID_TLV,         CHASSIS_MAC_ADDRESS,    0,                          0,              chassisMacAddr, sizeof(chassisMacAddr), createTLVFrame},
    {0,         PORT_ID_TLV,            PORT_INTERFACE_NAME,    0,                          0,              portInterface.v,sizeof(portInterface),  createTLVFrame},
    {0x01,      TIME_TO_LIVE_TLV,       0,                      0,                          0,              portTTL.ttl,    sizeof(portTTL),        createTLVFrame},


    // Fixed Org Specific TLV creation table.
    // Order of these TLVs is maintained manually for now!
    //
    // flags    type                    subType                 ouiDataSize                 pOuiData        pTlvData        tlvDataSize             createFnc
    {0,         ORG_SPECIFIC_TLV,       CISCO_POWER_VIA_MDI,    ORG_SPECIFIC_OUI_LENGTH,    cisco_Oui,      cisco_Mdi,      sizeof(cisco_Mdi),      createTLVFrame},
    {0,         ORG_SPECIFIC_TLV,       IEEE_3_POWER_VIA_MDI,   ORG_SPECIFIC_OUI_LENGTH,    ieee3_Oui,      ieee3_Mdi,      sizeof(ieee3_Mdi),      createTLVFrame},
    {0,         ORG_SPECIFIC_TLV,       TIA_MED_CAP,            ORG_SPECIFIC_OUI_LENGTH,    tia_Oui,        tia_MedCap,     sizeof(tia_MedCap),     createTLVFrame},
    {0,         ORG_SPECIFIC_TLV,       TIA_MED,                ORG_SPECIFIC_OUI_LENGTH,    tia_Oui,        tia_Med,        sizeof(tia_Med),        createTLVFrame},
    
    // Mandatory End TLV at the end of any LLDPDU
    // Always on last position in this table!
    // flags    type                    subType                 ouiDataSize                 pOuiData        pTlvData        tlvDataSize             createFnc
    {0x01,      END_LLDPDU_TLV,         0,                      0,                          0,              0,              0,                      createTLVFrame},
};


// TLV processing
//

// TLV processing function
typedef TCPIP_LLDP_TLV_PROCESS_RES(*tlvProcessFuncPtr)(TCPIP_LLDP_TLV* pTlv, uint8_t tlvType, uint16_t tlvLen, TCPIP_LLDP_TLV_TYPE lastType);

// IEEE Org Specific TLV processing function
typedef TCPIP_LLDP_TLV_PROCESS_RES (*IEEOrgFuncPtr)(uint8_t* pData, uint16_t tlvLen, uint32_t oui, uint8_t orgSubType);

typedef struct
{
    uint32_t oui;
    uint8_t subtype;
    IEEOrgFuncPtr processOrgTlvs;
} orgSpecificTLVs_t;

// tlv processing functions table
static const tlvProcessFuncPtr tlvProcessTbl[TCPIP_LLDP_TLV_TYPES] =
{
    processEndTlv,          // END_LLDPDU_TLV
    processChassisIdTlv,    // CHASSIS_ID_TLV
    processPortIdTlv,       // PORT_ID_TLV
    processTTLTlv,          // TIME_TO_LIVE_TLV
    processOptionalTlv,     // PORT_DESCRIPTION_TLV
    processOptionalTlv,     // SYSTEM_NAME_TLV
    processOptionalTlv,     // SYSTEM_DESCRIPTION_TLV
    processOptionalTlv,     // SYSTEM_CAPABILITIES_TLV
    processOptionalTlv,     // MANAGEMENT_ADDRESS_TLV
};



// A table to keep up with any newly added org specific TLVs
static const orgSpecificTLVs_t lldp_org_tlvTable[] = 
{
  {CISCO_OUI, (ORG_CISCO_SUBTYPE)CISCO_POWER_VIA_MDI, &processCiscoPowerTlv},
  {IEEE_3_OUI, (ORG_IEEE_3_SUBTYPE)IEEE_3_POWER_VIA_MDI, &processIEEE3PowerTlv}
};


// TLV initialization

void TCPIP_LLDP_TlvInit(void)
{
    orgProcFlags.val = 0;
}


// TLV creation

// constructor of the TLV frames in a LLPDU
// lldpCreateTLVFrameTbl indicates what frames need to be built
//
size_t TCPIP_LLDP_ConstructMibLDPDU(uint8_t* pBuff, size_t buffSize)
{

    const createTLVFrame_t*  ctorFrame;
    int   ix;
    size_t currSize;
    size_t totalSize = 0;

    if(pBuff == 0 || buffSize == 0)
    {   // size info only
        pBuff = 0;
    }
    else
    {
        prepCreateTLVFrame();
    }

    ctorFrame = lldpCreateTLVFrameTbl;
    for(ix = 0; ix < sizeof(lldpCreateTLVFrameTbl) / sizeof(*lldpCreateTLVFrameTbl); ix++, ctorFrame++)
    {
        currSize = (*ctorFrame->createFnc)(ctorFrame, pBuff, buffSize);

        if(currSize == -1)
        {   // error/overflow
            return -1;
        }
        totalSize += currSize;
        if(pBuff)
        {
            buffSize -= currSize;
            pBuff += currSize;
        }
    }

    return totalSize;
}

// function that updates the dynamic data for the TLV creation
static void prepCreateTLVFrame(void)
{
    const lldp_per_port_t* pLldp_port = TCPIP_LLDP_PortGet();

    // mandatory TLVs data update
    memcpy(chassisMacAddr, TCPIP_LLDP_LocalIfAddressGet(), sizeof(chassisMacAddr));

    portTTL.txTTL = TCPIP_Helper_htons(pLldp_port->tx.txTTL);
    
    if(orgProcFlags.poeEnabledPair == 1)
    {
        cisco_Mdi[0] = 0x0D;  // 4-wire, PD spare arch shared, PD poe on, PSE poe on
    }
    else
    {
        cisco_Mdi[0] = 0x05;
    }


    ieee3_Mdi[6] = pLldp_port->allocatedPower >> 8;
    ieee3_Mdi[7] = pLldp_port->allocatedPower;

    if(pLldp_port->allocatedPower != pLldp_port->desiredPower)
    {
        ieee3_Mdi[4] = pLldp_port->desiredPower >> 8;
        ieee3_Mdi[5] = pLldp_port->desiredPower;
        orgProcFlags.powerAllocated = false;
    }
    else
    {
        ieee3_Mdi[4] = pLldp_port->allocatedPower >> 8;
        ieee3_Mdi[5] = pLldp_port->allocatedPower;
        orgProcFlags.powerAllocated = true;
    }
}
// simple TLV constructor, type and length only
static uint8_t* tl_tlvConstruct(TCPIP_LLDP_TLV_TYPE type, uint16_t tlvLen, uint8_t* pBuff)
{
    if(pBuff)
    {
        TCPIP_UINT16_VAL tl;

        tl.Val = type;
        tl.Val <<= 9;
        tl.Val |= tlvLen;

        *pBuff++ = tl.v[1];
        *pBuff++ = tl.v[0];
    }

    return pBuff;
}

// allows the generic creation of any TLV with the supplied parameters
static size_t createTLVFrame(const createTLVFrame_t* pFrame, uint8_t* pDestBuff, size_t destBuffSize)
{
    uint16_t tlvLen = pFrame->ouiDataSize;
    if((pFrame->flags & TCPIP_LLDP_CREATE_NO_SUBTYPE) == 0)
    {   // add the subType
        tlvLen += sizeof(pFrame->subType);
    }
    tlvLen += pFrame->tlvDataSize;

    if(pDestBuff)
    {
        if(destBuffSize < sizeof(TCPIP_LLDP_TLV) + tlvLen)
        {   // not enough room
            return -1;
        }
        pDestBuff = tl_tlvConstruct(pFrame->type, tlvLen, pDestBuff);
        // add the OUI
        if(pFrame->pOuiData)
        {
            memcpy(pDestBuff, pFrame->pOuiData, pFrame->ouiDataSize);
            pDestBuff += pFrame->ouiDataSize;
        }
        //
        if((pFrame->flags & TCPIP_LLDP_CREATE_NO_SUBTYPE) == 0)
        {   // add the subType
            *pDestBuff++ = pFrame->subType;
        }
        // add the data buffer
        if(pFrame->pTlvData != 0)
        {
            memcpy(pDestBuff, pFrame->pTlvData, pFrame->tlvDataSize);
        }
    }

    return sizeof(TCPIP_LLDP_TLV) + tlvLen;

} 

// TLV processing


TCPIP_LLDP_TLV_PROCESS_RES TCPIP_LLDP_TlvProcess(TCPIP_LLDP_TLV* pTlv, TCPIP_LLDP_TLV_TYPE lastType)
{
    TCPIP_LLDP_TLV_PROCESS_RES tlvRes;
    uint16_t tlvLen = pTlv->length;
    uint8_t  tlvType = pTlv->type;

    if(tlvType < TCPIP_LLDP_TLV_TYPES)
    {   // standard type
        tlvRes = (*tlvProcessTbl[tlvType])(pTlv, tlvType, tlvLen, lastType);
    }
    else if(tlvType == ORG_SPECIFIC_TLV)
    {
        tlvRes = processOrgSpecificTlv(pTlv, tlvType, tlvLen, lastType);
    } 
    else
    {
        // unhandled TLV type
        tlvRes = TLV_PROCESS_ERROR;
    }

    return tlvRes;

}




static TCPIP_LLDP_TLV_PROCESS_RES    processEndTlv(TCPIP_LLDP_TLV* pTlv, uint8_t tlvType, uint16_t tlvLen, TCPIP_LLDP_TLV_TYPE lastType)
{
    if(lastType >=  TIME_TO_LIVE_TLV && tlvLen == 0)
    {   // end of LLDPU
        return TLV_PROCESS_END;
    }

    return TLV_PROCESS_ERROR;
}

static TCPIP_LLDP_TLV_PROCESS_RES    processChassisIdTlv(TCPIP_LLDP_TLV* pTlv, uint8_t tlvType, uint16_t tlvLen, TCPIP_LLDP_TLV_TYPE lastType)
{

    if(lastType == TCPIP_LLDP_TLV_TYPE_FRAME_START && tlvLen >= 1)
    {   // valid frame
        mib.lldpdu.chassis.type = tlvType;
        mib.lldpdu.chassis.length = tlvLen;
        memcpy(mib.lldpdu.chassis.id.field, pTlv->data, tlvLen);
        swapEvenBytes((uint8_t*)&mib.lldpdu.chassis.id.ID, 6);
        
        return TLV_PROCESS_CONTINUE;
    }

    return TLV_PROCESS_ERROR;
}

static TCPIP_LLDP_TLV_PROCESS_RES    processPortIdTlv(TCPIP_LLDP_TLV* pTlv, uint8_t tlvType, uint16_t tlvLen, TCPIP_LLDP_TLV_TYPE lastType)
{

    if(lastType == CHASSIS_ID_TLV && tlvLen >= 1)
    {   // valid frame
        mib.lldpdu.port.type = tlvType;
        mib.lldpdu.port.length = tlvLen;
        memcpy(mib.lldpdu.port.id.field, pTlv->data, tlvLen);
        swapEvenBytes((uint8_t*)&mib.lldpdu.port.id.ID, 6);

        return TLV_PROCESS_CONTINUE;
    }

    return TLV_PROCESS_ERROR;
}

static TCPIP_LLDP_TLV_PROCESS_RES    processTTLTlv(TCPIP_LLDP_TLV* pTlv, uint8_t tlvType, uint16_t tlvLen, TCPIP_LLDP_TLV_TYPE lastType)
{
    if(lastType == PORT_ID_TLV && tlvLen == 2)
    {   // valid frame
        mib.lldpdu.ttl.type = tlvType;
        mib.lldpdu.ttl.length = tlvLen;
        memcpy(mib.lldpdu.ttl.t.field, pTlv->data, tlvLen);
        mib.lldpdu.ttl.t.ttlField = TCPIP_Helper_ntohs(mib.lldpdu.ttl.t.ttlField);
        mib.rxInfoTTL = (uint16_t)mib.lldpdu.ttl.t.ttlField;    // stored in reverse order

        return TLV_PROCESS_CONTINUE;
    }

    return TLV_PROCESS_ERROR;
}

static TCPIP_LLDP_TLV_PROCESS_RES    processOptionalTlv(TCPIP_LLDP_TLV* pTlv, uint8_t tlvType, uint16_t tlvLen, TCPIP_LLDP_TLV_TYPE lastType)
{
    // We are not processing any optional TLVs right now.
    // Code should be added for the ones that need to be processed
    // And the rest should be discarded

    if(lastType >= TIME_TO_LIVE_TLV )
    {   // valid frame
        return TLV_PROCESS_CONTINUE;
    }

    return TLV_PROCESS_ERROR;
}

static TCPIP_LLDP_TLV_PROCESS_RES processOrgSpecificTlv(TCPIP_LLDP_TLV* pTlv, uint8_t tlvType, uint16_t tlvLen, TCPIP_LLDP_TLV_TYPE lastType)
{
    while(lastType >= TIME_TO_LIVE_TLV && tlvLen >= 4)
    {   // valid frame
        uint32_t orgOui;
        uint8_t orgSubtype;
        int ix;
        const orgSpecificTLVs_t *hptr;
        TCPIP_LLDP_TLV_PROCESS_RES res = TLV_PROCESS_CONTINUE;

        memcpy((uint8_t*)&orgOui, pTlv->data, 3); // orgOui is 3 bytes long
        orgOui <<= 8;
        orgOui = TCPIP_Helper_ntohl(orgOui);
        orgSubtype = pTlv->data[3];

        hptr = lldp_org_tlvTable;
        for (ix = 0; ix <= sizeof(lldp_org_tlvTable) / sizeof(*lldp_org_tlvTable); ix++, hptr++)
        {
            if (hptr->oui == orgOui && hptr->subtype == orgSubtype)
            {   // found entry
                res = hptr->processOrgTlvs(pTlv->data + 4, tlvLen - 4, orgOui, orgSubtype);
                break;
            }
        }

        if(res == TLV_PROCESS_ERROR)
        {   // invalid TLV ?
            break;
        }

        return TLV_PROCESS_CONTINUE;
    }


    return TLV_PROCESS_ERROR;
}


static TCPIP_LLDP_TLV_PROCESS_RES processCiscoPowerTlv(uint8_t* pData, uint16_t len, uint32_t oui, uint8_t orgSubType)
{
    if(len == 1)
    {   // valid frame
        mib.lldpdu.cisco_4wire_poe.OUI = oui;
        mib.lldpdu.cisco_4wire_poe.type = ORG_SPECIFIC_TLV;
        mib.lldpdu.cisco_4wire_poe.length = len + 4;     // whole TLV length stored!
        mib.lldpdu.cisco_4wire_poe.subtype = orgSubType;
        mib.lldpdu.cisco_4wire_poe.capability.capabilities = *pData;
        if((mib.lldpdu.cisco_4wire_poe.capability.capabilities & 0x08) != 0)
        {
           orgProcFlags.poeEnabledPair=1;
        }
        else
        {
           orgProcFlags.poeEnabledPair=0;
        }

        return TLV_PROCESS_CONTINUE;
    }

    return TLV_PROCESS_ERROR;
}

static TCPIP_LLDP_TLV_PROCESS_RES processIEEE3PowerTlv(uint8_t* pData, uint16_t len, uint32_t oui, uint8_t orgSubType)
{
    if(len == 8)
    {   // valid frame
        uint16_t reqdPower, allocPower;

        mib.lldpdu.ieee_mdi_poe.OUI = oui;
        mib.lldpdu.ieee_mdi_poe.type = ORG_SPECIFIC_TLV;
        mib.lldpdu.ieee_mdi_poe.length = len + 4;       // whole TLV length stored!
        mib.lldpdu.ieee_mdi_poe.subtype = orgSubType;
        mib.lldpdu.ieee_mdi_poe.capability.capabilities = *pData++;
        mib.lldpdu.ieee_mdi_poe.PSE_power_pair          = *pData++;
        mib.lldpdu.ieee_mdi_poe.power_class             = *pData++;
        mib.lldpdu.ieee_mdi_poe.type_src_prior.TSPval   = *pData++;

        memcpy((uint8_t*)&reqdPower, pData, 2);
        memcpy((uint8_t*)&allocPower, pData + 2, 2);
        mib.lldpdu.ieee_mdi_poe.PD_reqd_power = TCPIP_Helper_ntohs(reqdPower);
        mib.lldpdu.ieee_mdi_poe.PSE_allocated_power = TCPIP_Helper_ntohs(allocPower);

        // store the allocated power
        TCPIP_LLDP_AllocatedPowerSet(mib.lldpdu.ieee_mdi_poe.PSE_allocated_power);

        if(mib.lldpdu.ieee_mdi_poe.PSE_allocated_power > 300)
        {   // 30.0 W threshold
            orgProcFlags.val &= 0x01;           // clear all mutually exclusive flags
            orgProcFlags.uPoeEnabledPower = 1;  // full UPOE
        }
        else if (mib.lldpdu.ieee_mdi_poe.PSE_allocated_power > 150 )
        {   // 15.0 W threshold
            orgProcFlags.val &= 0x01;               // clear all mutually exclusive flags
            orgProcFlags.poePlusEnabledPower = 1;   // upto 15 watts
        }
        else
        {   // < 15.0 W
            orgProcFlags.val &= 0x01;               // clear all mutually exclusive flags
            orgProcFlags.poeEnabledMinPower = 1;    // lower than 15 watts
        }

        return TLV_PROCESS_CONTINUE;
    }

    return TLV_PROCESS_ERROR;
}


bool TCPIP_LLDP_OrgProcessFlagsGet(TCPIP_LLDP_ORG_FLAGS* pFlags)
{
    if(pFlags)
    {
        pFlags->val = orgProcFlags.val;
        return true;
    }

    return false;
}

// Helpers

// length has to be even
static void swapEvenBytes(uint8_t *buff, uint8_t len)
{
    uint8_t spare, x, times;
    times = len >> 1;
    for (x = 0; x < times; x++)
    {
        spare = buff[x];
        buff[x]= buff[len-1-x];
        buff[len-1-x]=spare;
    }
}


