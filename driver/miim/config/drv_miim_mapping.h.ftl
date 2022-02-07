/*******************************************************************************
  MIIM Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_miim_local.h

  Summary:
    MIIM driver local declarations and definitions.

  Description:
    This file contains the MIIM driver's local declarations and definitions.
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

#ifndef _DRV_MIIM_MAPPING_H
#define _DRV_MIIM_MAPPING_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "device.h"
// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************
<#if (drvPic32mEthmac.TCPIP_USE_ETH_MAC)?has_content && (drvPic32mEthmac.TCPIP_USE_ETH_MAC == true)>
#include "driver/ethmac/src/dynamic/drv_eth_pic32_lib.h"
</#if>

<#if ((drvEmac0.TCPIP_USE_EMAC0)?has_content && (drvEmac0.TCPIP_USE_EMAC0 == true)) || ((drvEmac1.TCPIP_USE_EMAC1)?has_content && (drvEmac1.TCPIP_USE_EMAC1 == true))>
static  __inline__ DRV_MIIM_RESULT
__attribute__((always_inline)) _DRV_MIIM_ETH_ENABLE( uintptr_t ethPhyId )
{
    return DRV_MIIM_RES_OK;
}

static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_MII_RELEASE_RESET( uintptr_t ethPhyId )
{
}

static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_SETUP_PREAMBLE(
    uintptr_t               ethPhyId,
    const DRV_MIIM_SETUP *  pSetUp
    )
{
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_SCAN_INCREMENT(
    uintptr_t               ethPhyId,
    const DRV_MIIM_SETUP *  pSetUp
    )
{
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_MNGMNT_PORT_ENABLE( uintptr_t ethPhyId )
{
    emac_registers_t * macRegs = (emac_registers_t *) ethPhyId; // EMAC0_REGS or EMAC1_REGS
    if( EMAC_NCR_MPE_Msk != (macRegs->EMAC_NCR & EMAC_NCR_MPE_Msk) )
    {   // once enabled should not toggle RE or TE because that will
        uint32_t tenRenSettings = macRegs->EMAC_NCR & (EMAC_NCR_RE_Msk | EMAC_NCR_TE_Msk);
        if( !tenRenSettings )
        {   // REN and TEN are both disabled
            macRegs->EMAC_NCR |=  EMAC_NCR_MPE_Msk;
        }
        else
        {   // changing TEN will cause emac DMA to reset to first queue
            macRegs->EMAC_NCR &= ~tenRenSettings;
            macRegs->EMAC_NCR |= EMAC_NCR_MPE_Msk;
            macRegs->EMAC_NCR |= tenRenSettings;
        }
    }
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_MNGMNT_PORT_DISABLE( uintptr_t ethPhyId )
{
}
static  __inline__ bool
__attribute__((always_inline)) _DRV_MIIM_IS_BUSY( uintptr_t ethPhyId )
{
    emac_registers_t * macRegs = (emac_registers_t *) ethPhyId; // EMAC0_REGS or EMAC1_REGS
   if ((macRegs->EMAC_NSR & EMAC_NSR_IDLE_Msk) != EMAC_NSR_IDLE_Msk)
       return true;
   else
       return false;
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_PHYADDR_SET(
    uintptr_t           ethPhyId,
    DRV_MIIM_OP_DCPT *  pOpDcpt
    )
{
}
static  __inline__ DRV_MIIM_TXFER_STAT
__attribute__((always_inline)) _DRV_MIIM_OP_SCAN_ENABLE( uintptr_t ethPhyId )
{
     return DRV_MIIM_TXFER_SCAN_STALE;
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_OP_WRITE_DATA(
    uintptr_t           ethPhyId,
    DRV_MIIM_OP_DCPT *  pOpDcpt
    )
{
    emac_registers_t * macRegs = (emac_registers_t *) ethPhyId; // EMAC0_REGS or EMAC1_REGS
    macRegs->EMAC_MAN = EMAC_MAN_SOF( 1 )       // clause 22 start of frame
                        | EMAC_MAN_RW( 1 )      // write
                        | EMAC_MAN_PHYA( pOpDcpt->phyAdd )
                        | EMAC_MAN_REGA( pOpDcpt->regIx )
                        | EMAC_MAN_CODE( 2 )
                        | EMAC_MAN_DATA( pOpDcpt->opData )
                        ;
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_WRITE_START( uintptr_t ethPhyId )
{
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_OP_READ_START(
    uintptr_t           ethPhyId,
    DRV_MIIM_OP_DCPT *  pOpDcpt
    )
{
    emac_registers_t * macRegs = (emac_registers_t *) ethPhyId; // EMAC0_REGS or EMAC1_REGS
    macRegs->EMAC_MAN = EMAC_MAN_SOF( 1 )       // clause 22 start of frame
                        | EMAC_MAN_RW( 2 )      // read
                        | EMAC_MAN_PHYA( pOpDcpt->phyAdd )
                        | EMAC_MAN_REGA( pOpDcpt->regIx )
                        | EMAC_MAN_CODE( 2 )
                        | EMAC_MAN_DATA( pOpDcpt->opData )
                        ;
}
static  __inline__ uint16_t
__attribute__((always_inline)) _DRV_MIIM_OP_READ_DATA_GET( uintptr_t ethPhyId )
{
    emac_registers_t * macRegs = (emac_registers_t *) ethPhyId; // EMAC0_REGS or EMAC1_REGS
    return (uint16_t)(macRegs->EMAC_MAN & EMAC_MAN_DATA_Msk);
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_CLEAR_DATA_VALID( uintptr_t ethPhyId )
{
}
static  __inline__ bool
__attribute__((always_inline)) _DRV_MIIM_IS_DATA_VALID( uintptr_t ethPhyId )
{
    return false;
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_SCAN_DISABLE( uintptr_t ethPhyId )
{
}
static  __inline__ void __attribute__((always_inline)) _DRV_MIIM_SMI_CLOCK_SET(uintptr_t ethPhyId, uint32_t hostClock, uint32_t maxMIIMClock )
{ 
	emac_registers_t *  macRegs = (emac_registers_t *) ethPhyId; // EMAC0_REGS or EMAC1_REGS
    uint32_t            tenRenSettings = macRegs->EMAC_NCR & (EMAC_NCR_RE_Msk | EMAC_NCR_TE_Msk);
    uint32_t            clockDivider;
    uint32_t            mdcDiv;
    mdcDiv = hostClock / maxMIIMClock;
    if( mdcDiv <= 8 )
    {
        clockDivider = EMAC_NCFGR_CLK_MCK_8;
    }
    else if( mdcDiv <= 16 )
    {
        clockDivider = EMAC_NCFGR_CLK_MCK_16;
    }
    else if( mdcDiv <= 32 )
    {
        clockDivider = EMAC_NCFGR_CLK_MCK_32;
    }
    else
    {
        clockDivider = EMAC_NCFGR_CLK_MCK_64;
    }
    if( !tenRenSettings )
    {   // REN and TEN are both disabled
        macRegs->EMAC_NCFGR |= clockDivider;
    }
    else
    {   // changing TEN will cause emac DMA to reset to first queue
        macRegs->EMAC_NCR &= ~tenRenSettings;
        macRegs->EMAC_NCFGR |= clockDivider;
        macRegs->EMAC_NCR |= tenRenSettings;
    }	
} 
</#if>

<#if (drvGmac.TCPIP_USE_ETH_MAC)?has_content && (drvGmac.TCPIP_USE_ETH_MAC == true)>
    //*****************************************************************************
    /* MII Clock Selection

      Summary:
        Lists the possible speed selection for the Reduced Media Independent 
        Interface (RMII).

      Description:
        This enumeration lists the possible speed selection for RMII.
        The body contains only two states: RMII_10Mbps or RMII_100Mbps.

      Remarks:
        This enumeration is processor specific and is defined in the processor-
        specific header files (see processor.h).
    */
    typedef enum {

        GMAC_MIIM_SYSCLK_DIV_BY_8   /*DOM-IGNORE-BEGIN*/ = 0x00 /*DOM-IGNORE-END*/ ,
        GMAC_MIIM_SYSCLK_DIV_BY_16  /*DOM-IGNORE-BEGIN*/ = 0x01 /*DOM-IGNORE-END*/ ,
        GMAC_MIIM_SYSCLK_DIV_BY_32  /*DOM-IGNORE-BEGIN*/ = 0x02 /*DOM-IGNORE-END*/ ,
        GMAC_MIIM_SYSCLK_DIV_BY_48  /*DOM-IGNORE-BEGIN*/ = 0x03 /*DOM-IGNORE-END*/ ,
        GMAC_MIIM_SYSCLK_DIV_BY_64  /*DOM-IGNORE-BEGIN*/ = 0x04 /*DOM-IGNORE-END*/ ,
        GMAC_MIIM_SYSCLK_DIV_BY_96  /*DOM-IGNORE-BEGIN*/ = 0x05 /*DOM-IGNORE-END*/ ,

    } GMAC_MIIM_CLK;
    
    static  __inline__ DRV_MIIM_RESULT __attribute__((always_inline))_DRV_MIIM_ETH_ENABLE(uintptr_t ethPhyId)
    {
        return DRV_MIIM_RES_OK;
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_MII_RELEASE_RESET(uintptr_t ethPhyId)
    {
    }

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_SETUP_PREAMBLE(uintptr_t ethPhyId,const DRV_MIIM_SETUP* pSetUp)
    {
    }

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_SCAN_INCREMENT(uintptr_t ethPhyId,const DRV_MIIM_SETUP* pSetUp)
    {
    }

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_MNGMNT_PORT_ENABLE(uintptr_t ethPhyId)
    {
        <#if (drvGmac.TCPIP_INTMAC_DEVICE)?has_content && (drvGmac.TCPIP_INTMAC_DEVICE == "PIC32CZ")>
        ETH_REGS->ETH_NCR |=	ETH_NCR_MPE_Msk;
        <#else>
        GMAC_REGS->GMAC_NCR |=	GMAC_NCR_MPE_Msk;
        </#if>
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_MNGMNT_PORT_DISABLE(uintptr_t ethPhyId)
    {
        <#if (drvGmac.TCPIP_INTMAC_DEVICE)?has_content && (drvGmac.TCPIP_INTMAC_DEVICE == "PIC32CZ")>
        ETH_REGS->ETH_NCR &= ~ETH_NCR_MPE_Msk;
        <#else>
        GMAC_REGS->GMAC_NCR &= ~GMAC_NCR_MPE_Msk;
        </#if>       
       
    }
    
    static  __inline__ bool __attribute__((always_inline))_DRV_MIIM_IS_BUSY(uintptr_t ethPhyId)
    {
        <#if (drvGmac.TCPIP_INTMAC_DEVICE)?has_content && (drvGmac.TCPIP_INTMAC_DEVICE == "PIC32CZ")>
        bool    phyBusy = (ETH_REGS->ETH_NSR & ETH_NSR_IDLE_Msk) != ETH_NSR_IDLE_Msk;
        <#else>
        bool    phyBusy = (GMAC_REGS->GMAC_NSR & GMAC_NSR_IDLE_Msk) != GMAC_NSR_IDLE_Msk;
        </#if>    
        
        return phyBusy;        
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_PHYADDR_SET(uintptr_t ethPhyId,DRV_MIIM_OP_DCPT* pOpDcpt)
    {
      
    }

    static  __inline__ DRV_MIIM_TXFER_STAT __attribute__((always_inline))_DRV_MIIM_OP_SCAN_ENABLE(uintptr_t ethPhyId)
    {        
         return DRV_MIIM_TXFER_SCAN_STALE;
    }

    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_OP_WRITE_DATA(uintptr_t ethPhyId,DRV_MIIM_OP_DCPT* pOpDcpt)
    {
        <#if (drvGmac.TCPIP_INTMAC_DEVICE)?has_content && (drvGmac.TCPIP_INTMAC_DEVICE == "PIC32CZ")>
        ETH_REGS->ETH_MAN = 
                                (~ETH_MAN_WZO_Msk & ETH_MAN_CLTTO_Msk)
                                 | (ETH_MAN_OP(0x1)) 
                                 | ETH_MAN_WTN(0x02) 
                                 | ETH_MAN_PHYA(pOpDcpt->phyAdd) 
                                 | ETH_MAN_REGA(pOpDcpt->regIx) 
                                 | ETH_MAN_DATA(pOpDcpt->opData);
        <#else>
        GMAC_REGS->GMAC_MAN = 
                                (~GMAC_MAN_WZO_Msk & GMAC_MAN_CLTTO_Msk)
                                 | (GMAC_MAN_OP(0x1)) 
                                 | GMAC_MAN_WTN(0x02) 
                                 | GMAC_MAN_PHYA(pOpDcpt->phyAdd) 
                                 | GMAC_MAN_REGA(pOpDcpt->regIx) 
                                 | GMAC_MAN_DATA(pOpDcpt->opData);
        </#if>   
        
    }

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_WRITE_START(uintptr_t ethPhyId)
    {
    }
    
   
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_OP_READ_START(uintptr_t ethPhyId, DRV_MIIM_OP_DCPT* pOpDcpt)
    {
        <#if (drvGmac.TCPIP_INTMAC_DEVICE)?has_content && (drvGmac.TCPIP_INTMAC_DEVICE == "PIC32CZ")>
        ETH_REGS->ETH_MAN = 
                                (~ETH_MAN_WZO_Msk & ETH_MAN_CLTTO_Msk)
                                 | (ETH_MAN_OP(0x2)) 
                                 | ETH_MAN_WTN(0x02) 
                                 | ETH_MAN_PHYA(pOpDcpt->phyAdd) 
                                 | ETH_MAN_REGA(pOpDcpt->regIx) 
                                 | ETH_MAN_DATA(0);
        <#else>
        GMAC_REGS->GMAC_MAN =    (~GMAC_MAN_WZO_Msk & GMAC_MAN_CLTTO_Msk) 
                                    | (GMAC_MAN_OP(0x2)) 
                                    | GMAC_MAN_WTN(0x02) 
                                    | GMAC_MAN_PHYA(pOpDcpt->phyAdd) 
                                    | GMAC_MAN_REGA(pOpDcpt->regIx) 
                                    | GMAC_MAN_DATA(0);
        </#if>      
     
    }
    
                                 
    static  __inline__ uint16_t __attribute__((always_inline))_DRV_MIIM_OP_READ_DATA_GET(uintptr_t ethPhyId)
    {
       <#if (drvGmac.TCPIP_INTMAC_DEVICE)?has_content && (drvGmac.TCPIP_INTMAC_DEVICE == "PIC32CZ")>
        return (uint16_t)(ETH_REGS->ETH_MAN & ETH_MAN_DATA_Msk) ;
        <#else>
        return (uint16_t)(GMAC_REGS->GMAC_MAN & GMAC_MAN_DATA_Msk) ;
        </#if>   
       
    }

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_CLEAR_DATA_VALID(uintptr_t ethPhyId)
    {
    }
  
    static  __inline__ bool __attribute__((always_inline))_DRV_MIIM_IS_DATA_VALID(uintptr_t ethPhyId)
    {
        return false;
    }  

 
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_SCAN_DISABLE(uintptr_t ethPhyId)
    {
    } 
    
    static  __inline__ void __attribute__((always_inline)) _DRV_MIIM_SMI_CLOCK_SET(uintptr_t ethPhyId, uint32_t hostClock, uint32_t maxMIIMClock )
    { 
    	uint32_t mdc_div; 
        GMAC_MIIM_CLK clock_dividor ; 
        mdc_div = hostClock/maxMIIMClock; 
        if (mdc_div <= 8 ) 
        { 
            clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_8; 
        } 
        else if (mdc_div <= 16 ) 
        { 
            clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_16; 
        } 
        else if (mdc_div <= 32 ) 
        { 
            clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_32; 
        } 
        else if (mdc_div <= 48 ) 
        { 
            clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_48; 
        } 
        else if (mdc_div <= 64 ) 
        { 
            clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_64; 
        } 
        else if (mdc_div <= 96 ) 
        { 
            clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_96; 
        } 
        else 
        { 
            clock_dividor = 0; 
        } 
        <#if (drvGmac.TCPIP_INTMAC_DEVICE)?has_content && (drvGmac.TCPIP_INTMAC_DEVICE == "PIC32CZ")>
        ETH_REGS->ETH_NCR &= ~ETH_NCR_TXEN_Msk; 
        ETH_REGS->ETH_NCR &= ~ETH_NCR_RXEN_Msk;	
        ETH_REGS->ETH_NCFGR =   (ETH_REGS->ETH_NCFGR & 
                                (~ETH_NCFGR_CLK_Msk)) | 
                                (clock_dividor << ETH_NCFGR_CLK_Pos); 
        ETH_REGS->ETH_NCR |= ETH_NCR_TXEN_Msk; 
        ETH_REGS->ETH_NCR |= ETH_NCR_RXEN_Msk;
        <#else>
        GMAC_REGS->GMAC_NCR &= ~GMAC_NCR_TXEN_Msk; 
        GMAC_REGS->GMAC_NCR &= ~GMAC_NCR_RXEN_Msk;	
        GMAC_REGS->GMAC_NCFGR = (GMAC_REGS->GMAC_NCFGR & 
                                (~GMAC_NCFGR_CLK_Msk)) | 
                                (clock_dividor << GMAC_NCFGR_CLK_Pos); 
        GMAC_REGS->GMAC_NCR |= GMAC_NCR_TXEN_Msk; 
        GMAC_REGS->GMAC_NCR |= GMAC_NCR_RXEN_Msk;
        </#if>  
        
        	
    } 
</#if>    

<#if (drvPic32mEthmac.TCPIP_USE_ETH_MAC)?has_content && (drvPic32mEthmac.TCPIP_USE_ETH_MAC == true)>
   static const short _MIIMClockDivisorTable[]=
    {
        4, 6, 8, 10, 14, 20, 28, 40, 
#if defined (__PIC32MZ__)
        48, 50,
#endif  // defined (__PIC32MZ__)
    };  // divider values for the Host clock
   
    static  __inline__ DRV_MIIM_RESULT __attribute__((always_inline))_DRV_MIIM_ETH_ENABLE(uintptr_t ethPhyId)
    {        
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_MIIM_RESULT res = DRV_MIIM_RES_OK;

        if(!DRV_ETH_IsEnabled(ethId)) 
        { 
            DRV_ETH_Enable(ethId);  
            res = DRV_MIIM_RES_INIT_WARNING;
        } 
        return res;
    }
    

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_MII_RELEASE_RESET(uintptr_t ethPhyId)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_ETH_MIIResetDisable(ethId);

    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_SETUP_PREAMBLE(uintptr_t ethPhyId, const DRV_MIIM_SETUP* pSetUp)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        if((pSetUp->setupFlags & DRV_MIIM_SETUP_FLAG_PREAMBLE_SUPPRESSED) != 0)
        {
            DRV_ETH_MIIMNoPreEnable(ethId);            
        }
        else
        {
            DRV_ETH_MIIMNoPreDisable(ethId);
        }

    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_SCAN_INCREMENT(uintptr_t ethPhyId, const DRV_MIIM_SETUP* pSetUp)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        if((pSetUp->setupFlags & DRV_MIIM_SETUP_FLAG_SCAN_ADDRESS_INCREMENT) != 0)
        {
            DRV_ETH_MIIMScanIncrEnable(ethId);
        }
        else
        {
            DRV_ETH_MIIMScanIncrDisable(ethId);
        }

    }

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_MNGMNT_PORT_ENABLE(uintptr_t ethPhyId)
    {
       
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_MNGMNT_PORT_DISABLE(uintptr_t ethPhyId)
    {
       
    }
    
    static  __inline__ bool __attribute__((always_inline))_DRV_MIIM_IS_BUSY(uintptr_t ethPhyId)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        return DRV_ETH_MIIMIsBusy(ethId);
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_PHYADDR_SET(uintptr_t ethPhyId,DRV_MIIM_OP_DCPT* pOpDcpt)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_ETH_PHYAddressSet(ethId, pOpDcpt->phyAdd);
        DRV_ETH_RegisterAddressSet(ethId,pOpDcpt->regIx);
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_CLEAR_DATA_VALID(uintptr_t ethPhyId)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_ETH_ClearDataValid(ethId);
    }
   
    static  __inline__ DRV_MIIM_TXFER_STAT __attribute__((always_inline))_DRV_MIIM_OP_SCAN_ENABLE(uintptr_t ethPhyId)
    {   
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_ETH_ClearDataValid(ethId);
        DRV_ETH_MIIMScanModeEnable(ethId);
        return DRV_MIIM_TXFER_SCAN_STALE;
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_OP_WRITE_DATA(uintptr_t ethPhyId,DRV_MIIM_OP_DCPT* pOpDcpt)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_ETH_MIIMWriteDataSet(ethId, pOpDcpt->opData);
    }
    

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_WRITE_START(uintptr_t ethPhyId)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_ETH_MIIMWriteStart(ethId);
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_OP_READ_START(uintptr_t ethPhyId,DRV_MIIM_OP_DCPT* pOpDcpt)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_ETH_MIIMReadStart(ethId);
    }

    static  __inline__ uint16_t __attribute__((always_inline))_DRV_MIIM_OP_READ_DATA_GET(uintptr_t ethPhyId)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        return DRV_ETH_MIIMReadDataGet(ethId);
    }
    
    static  __inline__ bool __attribute__((always_inline))_DRV_MIIM_IS_DATA_VALID(uintptr_t ethPhyId)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        return !DRV_ETH_DataNotValid(ethId);
    }
      
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_SCAN_DISABLE(uintptr_t ethPhyId)
    { 
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_ETH_MIIMScanModeDisable(ethId);
    }
    
    static  __inline__ void __attribute__((always_inline)) _DRV_MIIM_SMI_CLOCK_SET(uintptr_t ethPhyId, uint32_t hostClock, uint32_t maxMIIMClock )
    {

        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        int  ix;
        DRV_ETH_MIIMResetEnable(ethId); // Issue MIIM reset
        DRV_ETH_MIIMResetDisable(ethId); // Clear MIIM reset

        for(ix = 0; ix < sizeof(_MIIMClockDivisorTable) / sizeof(*_MIIMClockDivisorTable); ix++)
        {
            if(hostClock / _MIIMClockDivisorTable[ix] <= maxMIIMClock)
            {   // found it
                break;
            }
        }

        if(ix == sizeof(_MIIMClockDivisorTable) / sizeof(*_MIIMClockDivisorTable))
        {
            ix--;   // max divider; best we can do
        }

        DRV_ETH_MIIMClockSet(ethId, ix + 1);  // program the clock*/

    }
</#if>

#endif //#ifndef _DRV_MIIM_MAPPING_H

/*******************************************************************************
 End of File
*/

