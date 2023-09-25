/*******************************************************************************
 LLDP TLV types Header File
 
  Company:
    Microchip Technology Inc.
    
  File Name:
    lldp_tlv.h
    
  Summary:

  Description:

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

#ifndef _LLDP_TLVS_H
#define _LLDP_TLVS_H


typedef struct __attribute__((packed))
{
    union
    {
        struct
        {
            uint16_t    length      : 9;    // TLV string length
            uint16_t    type        : 7;    // TLV type
        };
        uint16_t    type_length;        // compressed format
    };
    uint8_t        data[];             // variable data
}TCPIP_LLDP_TLV;


/******************* LLDP TLV TYPES **************************/

/*Mandatory & Optional TLV types from IEEE 802.1AB*/
typedef enum
{
    TCPIP_LLDP_TLV_TYPE_FRAME_START = -1,   // non existent type

    END_LLDPDU_TLV = 0,                 // Mandatory
    CHASSIS_ID_TLV,                     // Mandatory
    PORT_ID_TLV,                        // Mandatory
    TIME_TO_LIVE_TLV,                   // Mandatory
    PORT_DESCRIPTION_TLV,               // OPTIONAL
    SYSTEM_NAME_TLV,                    // OPTIONAL
    SYSTEM_DESCRIPTION_TLV,             // OPTIONAL
    SYSTEM_CAPABILITIES_TLV,            // OPTIONAL
    MANAGEMENT_ADDRESS_TLV,             // OPTIONAL

    TCPIP_LLDP_TLV_TYPES,          // number of standard TLV types

    /* 9 - 126 are reserved */
    ORG_SPECIFIC_TLV=127                //OPTIONAL

} TCPIP_LLDP_TLV_TYPE;

/* TLV Subtypes from section 9 of IEEE 802.1AB */
typedef enum
{
  CHASSIS_COMPONENT=1,
  CHASSIS_INTERFACE_ALIAS,
  CHASSIS_PORT_COMPONENT,
  CHASSIS_MAC_ADDRESS,
  CHASSIS_NETWORK_ADDRESS,
  CHASSIS_INTERFACE_NAME,
  CHASSIS_LOCALLY_ASSIGNED
  /* 8-255 - reserved */
} CHASSIS_SUBTYPE;

/* TLV Subtypes from section 9 of IEEE 802.1AB */
typedef enum
{
  /* 0 - reserved */
  PORT_INTERFACE_ALIAS=1,
  PORT_COMPONENT,
  PORT_MAC_ADDRESS,
  PORT_NETWORK_ADDRESS,
  PORT_INTERFACE_NAME,
  PORT_AGENT_CIRCUIT_ID,
  PORT_LOCALLY_ASSIGNED
  /* 8-255 - reserved */
} PORT_SUBTYPE;

/* organizationally Specific TLVS IEEE 802.3 sec 79.3*/
typedef enum
{
  /* 0 - reserved */
  IEEE_3_MAC_PHY_CONFIG=1,
  IEEE_3_POWER_VIA_MDI,
  IEEE_3_LINK_AGGREGATION,//Deprecated
  IEEE_3_MAX_FRAME_SIZE,
  IEEE_3_ENERGY_EFFICIENT_ETHERNET,
  /* 6-255 - reserved */
} ORG_IEEE_3_SUBTYPE;

/* organizationally Specific TLVS IEEE 802.3 sec 79.3*/
typedef enum
{
  /* 0 - reserved */
  CISCO_POWER_VIA_MDI = 1,
} ORG_CISCO_SUBTYPE;


/* TIA MED specific TLV subtypes */
typedef enum
{
    TIA_MED_CAP     = 1,
    TIA_MED         = 4,
}TIA_MED_SUBTYPE;


/*************************LLDP TLVs*********************/

typedef struct
{
    struct
    {
        uint8_t  type;
        uint16_t length;
        union
        {
            uint8_t field[255];
            struct
            {
                uint8_t  subtype;
                uint8_t  ID[254];
            };
        }id;
    }chassis;

    struct
    {
        uint8_t  type;
        uint16_t length;
        union
        {
            uint8_t field[255];
            struct
            {
                uint8_t subtype;
                uint8_t  ID[254];
            };
        }id;
    }port;

    struct
    {
        uint8_t  type;
        uint16_t length;
        union
        {
            uint16_t ttlField;
            struct
            {
                uint8_t  field[2];
            };
        }t;
    }ttl;

    struct
    {
        uint8_t  type;
        uint16_t length;
        uint32_t OUI;
        uint8_t  subtype;
        union
        {
            uint8_t     capabilities;
            struct
            {
                unsigned reserved                       :4;     // Reserved
                unsigned PSE_sparePairPOE_ON            :1;     // 1=ON, 0=OFF
                unsigned PD_req_sparePairPOE_ON         :1;     // 1=ON
                unsigned PD_sparePair_Arch_shared       :1;     // 1=Desired
                unsigned PSE_4wirePOE_supported         :1;     // 1=supported
            }bits;
        }capability;
        //uint8_t    capabilities;
    }cisco_4wire_poe;
    
    struct
    {
        uint8_t  type;
        uint16_t length;
        uint32_t OUI;
        uint8_t  subtype;
        union
        {
            uint8_t     capabilities;
            struct
            {
                unsigned reserved                       :4;     // Reserved
                unsigned PSE_pairsControl               :1;     // 1=can be controlled
                unsigned PSE_MDI_powerState             :1;     // 1=enabled
                unsigned PSE_MDI_powerSupport           :1;     // 1=supported
                unsigned portClass                      :1;     // 1=PSE
            }bits;
        }capability;
        //uint8_t     capabilities;
        uint8_t     PSE_power_pair;
        uint8_t     power_class;
        union
        {
            uint8_t     TSPval;
            struct
            {
                unsigned power_type     :2; // 11=Type1 PD, 10=Type1 PSE, 01=Type2 PD,00=Type2 PSE
                unsigned power_source   :2; // if power_type = PD:11=PSE&local, 10=Reserved, 01=PSE, 00=unkown
                                            // if power_type = PSE:11=Reserved, 10=Backup Src, 01=Primary pwr src,00=unkown
                unsigned reserved       :2;
                unsigned power_priority :2; // 11=low, 10=high,01=critical,00=unknown
            }bits;
        }type_src_prior;
        //uint8_t     TSPval;
        uint16_t    PD_reqd_power;
        uint16_t    PSE_allocated_power;
    }ieee_mdi_poe;

    struct
    {
        uint8_t  type;
        uint16_t length;
    }end;

}TCPIP_LLDP_PDU_t;


typedef struct
{
    uint16_t            rxInfoTTL;
    TCPIP_LLDP_PDU_t    lldpdu;
}TCPIP_LLDP_MIB_t;


// TLV packets
//


// LLDP PACKET
typedef struct
{
    //CHASSIS
    uint16_t          chassisTypeLen;
    struct
    {
        uint8_t           subtype;
        TCPIP_MAC_ADDR    id;
    }chassisField;
    //PORT
    uint16_t          portTypeLen;
    struct
    {
        uint8_t          subtype;
        TCPIP_MAC_ADDR   id;
    }portField;
    //TTL
    uint16_t          ttlTypeLen;
    uint16_t          ttlField;
    //UPOE
    uint16_t          upoeTypeLen;
    struct
    {
        uint8_t           OUI[3];
        uint8_t           subtype;
        uint8_t           capability;
#ifdef EXTEND_POWER
        uint8_t           PSEPowerPair;
        uint8_t           powerClass;
        uint8_t           typeSrcPrior;
        uint16_t          PDReqdPower;
        uint16_t          PSEallocatedPower;
#endif
    }upoeField;
    //END
    uint16_t          endTypeLen;
}TCPIP_LLDP_PACKET;

// TLV creation






// fills a LLDPDU with MIB data
// returns the size taken/needed or -1 if there was an error
// pBuff could be 0 if just info size needed
size_t TCPIP_LLDP_ConstructMibLDPDU(uint8_t* pBuff, size_t buffSize);


void TCPIP_LLDP_TlvInit(void);

// standard TLVs receive/processing functions
//

// TLV process result
typedef enum
{
    TLV_PROCESS_ERROR = 0,          // some error has ocurred
    TLV_PROCESS_CONTINUE,           // OK, keep processing
    TLV_PROCESS_END,                // done, end of frame
    TLV_PROCESS_UNKNOWN,            // not known option TLVs
}TCPIP_LLDP_TLV_PROCESS_RES;


TCPIP_LLDP_TLV_PROCESS_RES      TCPIP_LLDP_TlvProcess(TCPIP_LLDP_TLV* pTlv, TCPIP_LLDP_TLV_TYPE lastType);




#endif  /* _LLDP_TLVS_H */

