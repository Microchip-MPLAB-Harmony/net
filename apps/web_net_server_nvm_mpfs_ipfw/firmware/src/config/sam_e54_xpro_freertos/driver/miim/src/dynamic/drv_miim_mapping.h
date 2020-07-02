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
        GMAC_REGS->GMAC_NCR |=	GMAC_NCR_MPE_Msk;
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_MNGMNT_PORT_DISABLE(uintptr_t ethPhyId)
    {
       GMAC_REGS->GMAC_NCR &= ~GMAC_NCR_MPE_Msk;
    }
    
    static  __inline__ bool __attribute__((always_inline))_DRV_MIIM_IS_BUSY(uintptr_t ethPhyId)
    {
       if ((GMAC_REGS->GMAC_NSR & GMAC_NSR_IDLE_Msk) != GMAC_NSR_IDLE_Msk)
           return true;
       else
           return false;          
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
        GMAC_REGS->GMAC_MAN = 
                                (~GMAC_MAN_WZO_Msk & GMAC_MAN_CLTTO_Msk)
                                 | (GMAC_MAN_OP(0x1)) 
                                 | GMAC_MAN_WTN(0x02) 
                                 | GMAC_MAN_PHYA(pOpDcpt->phyAdd) 
                                 | GMAC_MAN_REGA(pOpDcpt->regIx) 
                                 | GMAC_MAN_DATA(pOpDcpt->opData);
    }

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_WRITE_START(uintptr_t ethPhyId)
    {
    }
    
   
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_OP_READ_START(uintptr_t ethPhyId, DRV_MIIM_OP_DCPT* pOpDcpt)
    {
        GMAC_REGS->GMAC_MAN =    (~GMAC_MAN_WZO_Msk & GMAC_MAN_CLTTO_Msk) 
                                    | (GMAC_MAN_OP(0x2)) 
                                    | GMAC_MAN_WTN(0x02) 
                                    | GMAC_MAN_PHYA(pOpDcpt->phyAdd) 
                                    | GMAC_MAN_REGA(pOpDcpt->regIx) 
                                    | GMAC_MAN_DATA(0);
    }
    
                                 
    static  __inline__ uint16_t __attribute__((always_inline))_DRV_MIIM_OP_READ_DATA_GET(uintptr_t ethPhyId)
    {
       return (uint16_t)(GMAC_REGS->GMAC_MAN & GMAC_MAN_DATA_Msk) ;
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
        GMAC_REGS->GMAC_NCR &= ~GMAC_NCR_TXEN_Msk; 
        GMAC_REGS->GMAC_NCR &= ~GMAC_NCR_RXEN_Msk;	
        GMAC_REGS->GMAC_NCFGR = 
                   (GMAC_REGS->GMAC_NCFGR & 
                        (~GMAC_NCFGR_CLK_Msk)) | (clock_dividor << GMAC_NCFGR_CLK_Pos); 
        GMAC_REGS->GMAC_NCR |= GMAC_NCR_TXEN_Msk; 
        GMAC_REGS->GMAC_NCR |= GMAC_NCR_RXEN_Msk;	
    } 


#endif //#ifndef _DRV_MIIM_MAPPING_H

/*******************************************************************************
 End of File
*/

