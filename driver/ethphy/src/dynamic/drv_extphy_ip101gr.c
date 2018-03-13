/*******************************************************************************
  IP101GR API for Microchip TCP/IP Stack
*******************************************************************************/

/*******************************************************************************
File Name:  drv_extphy_ip101gr.c
Copyright © 2012 released Microchip Technology Inc.  All rights
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

#include "driver/ethphy/src/drv_ethphy_local.h"

#include "driver/ethphy/src/dynamic/drv_extphy_ip101gr.h"

/****************************************************************************
 *                 interface functions
 ****************************************************************************/
#ifdef PHY_WOL_ENABLE
static void set_wol_dnspd_en(DRV_HANDLE hClientObj,int en);
static void set_wol(DRV_HANDLE hClientObj,int en, int master, int timer);
static void set_magic_mac(DRV_HANDLE hClientObj,uint8_t *dat);
static bool set_wol_intr(DRV_HANDLE hClientObj,int en);
static bool wol_enabled(DRV_HANDLE hClientObj);
static void manual_set_wol(DRV_HANDLE hClientObj);
static void set_sense_any_pkt(DRV_HANDLE hClientObj,int anypkt);
static bool read_sense_any_pkt(DRV_HANDLE hClientObj);
static void set_sense_magic_pkt(DRV_HANDLE hClientObj,int magic);
static bool read_sense_magic_pkt(DRV_HANDLE hClientObj);
static eWOL_STATE check_wol_status(DRV_HANDLE hClientObj);
static WOL_OPERATION_MODE read_wol_mode(DRV_HANDLE hClientObj);
static bool wol_intr32_pin_enabled(DRV_HANDLE hClientObj);
static bool wol_intr_enabled(DRV_HANDLE hClientObj);
static void IP101GRWOLIsr(void *p);

/*
* This LEDstate is to check if Interrupt has occured or not
*/
int32_t LEDstate = SYS_USERIO_LED_DEASSERTED;
#endif /* PHY_WOL_ENABLE */

/****************************************************************************
 * Function:        DRV_EXTPHY_MIIConfigure
 *
 * PreCondition:    - Communication to the PHY should have been established.
 *
 * Input:   		handle - A valid open-instance handle, returned from the driver's open routine   
 *				cFlags - the requested configuration flags: DRV_ETHPHY_CFG_RMII/DRV_ETHPHY_CFG_MII
 *
 * Output:          DRV_ETHPHY_RES_OK - operation completed successfully
 *
 *                  DRV_ETHPHY_RES_PENDING - operation pending; the call needs to be re-issued until
 *                                    DRV_ETHPHY_RES_OK or an error reported
 *
 *
 *                  < 0         - an error occurred and the operation was aborted
 *
 * Side Effects:    None
 *
 * Overview:        This function configures the PHY in one of MII/RMII operation modes.
 *
 *****************************************************************************/
static DRV_ETHPHY_RESULT DRV_EXTPHY_MIIConfigure(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj,DRV_ETHPHY_CONFIG_FLAGS cFlags)
{
    union
    {
        struct
        {
            uint16_t low;
            uint16_t high;
        };
        uint32_t    w;
    }vendorData = {};

    uint16_t    phyReg = 0;
    uint16_t    miiConfPhase = 0;
    int         phyAddress = 0;

    DRV_ETHPHY_RESULT res = pBaseObj->DRV_ETHPHY_VendorDataGet(hClientObj, &vendorData.w);

    if(res < 0)
    {   // some error occurred
        return res;
    }

    miiConfPhase = vendorData.low;

    pBaseObj->DRV_ETHPHY_PhyAddressGet(hClientObj, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &phyAddress);

    switch(miiConfPhase)
    {
        case 0:

            // RMII12  is in page 16 Reg16
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_PAGE_SEL, PAGENUM_16, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // advance to the next phase
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++miiConfPhase);
            return DRV_ETHPHY_RES_PENDING;

        case 1:

            res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_REG_UTP_CONTROL_MODE, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // advance to the next phase
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++miiConfPhase);
            return DRV_ETHPHY_RES_PENDING;

        case 2:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // got PHY_REG_UTP_CONTROL_MODE result
            if(cFlags & DRV_ETHPHY_CFG_RMII)
            {
                // use RMII 1.2 and set the repeater
                phyReg |= _UTP_MODECTRL_RMII_V12_MASK | _UTP_MODECTRL_REPEATER_MODE_MASK;
            }

            // save the phyReg for the next state
            vendorData.high = phyReg;
            vendorData.low = miiConfPhase + 1;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;

        case 3:
            phyReg = vendorData.high;
            // update the RMII and Bypass Register Register
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_UTP_CONTROL_MODE, phyReg, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++miiConfPhase);
            return DRV_ETHPHY_RES_PENDING;


        case 4:
            // go back to Page 0
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_PAGE_SEL, PAGENUM_0, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // done
            return DRV_ETHPHY_RES_OK;

        default:
            // shouldn't happen
            return DRV_ETHPHY_RES_OPERATION_ERR; 
    }	
}


/****************************************************************************
 * Function:        DRV_EXTPHY_MDIXConfigure
 *
 * PreCondition:    - Communication to the PHY should have been established.
 *
 * Input:           handle - A valid open-instance handle, returned from the driver's open routine
 *                  oFlags - the requested open flags: TCPIP_ETH_OPEN_MDIX_AUTO, TCPIP_ETH_OPEN_MDIX_NORM/TCPIP_ETH_OPEN_MDIX_SWAP
 *
 * Output:          DRV_ETHPHY_RES_OK - operation completed successfully
 *
 *                  DRV_ETHPHY_RES_PENDING - operation pending; the call needs to be re-issued until
 *                                    DRV_ETHPHY_RES_OK or an error reported
 *
 *
 *                  < 0         - an error occurred and the operation was aborted
 *
 * Side Effects:    None
 *
 * Overview:        This function configures the MDIX mode for the PHY.
 *
 * Note:            None
 *****************************************************************************/
static DRV_ETHPHY_RESULT DRV_EXTPHY_MDIXConfigure(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, TCPIP_ETH_OPEN_FLAGS oFlags)
{
    union
    {
        struct
        {
            uint16_t low;
            uint16_t high;
        };
        uint32_t    w;
    }vendorData = {};

    uint16_t    phyReg = 0;
    uint16_t    mdixConfPhase = 0;
    int         phyAddress = 0;

    DRV_ETHPHY_RESULT res = pBaseObj->DRV_ETHPHY_VendorDataGet(hClientObj, &vendorData.w);

    if(res < 0)
    {   // some error occurred
        return res;
    }

    mdixConfPhase = vendorData.low;

    pBaseObj->DRV_ETHPHY_PhyAddressGet(hClientObj, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &phyAddress);

    switch(mdixConfPhase)
    {
        case 0:
            // Auto MDIX  is in page 16 ,Reg 16
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_PAGE_SEL, PAGENUM_16, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++mdixConfPhase);
            return DRV_ETHPHY_RES_PENDING;

        case 1:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_REG_UTP_CONTROL_MODE, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // advance to the next phase
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++mdixConfPhase);
            return DRV_ETHPHY_RES_PENDING;


        case 2:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // got PHY_REG_UTP_CONTROL_MODE result

            vendorData.low = mdixConfPhase + 1;
            vendorData.high = phyReg;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;
 
        case 3:
            phyReg = vendorData.high;
            // update the PHY_REG_UTP_CONTROL_MODE Register
            if(oFlags & TCPIP_ETH_OPEN_MDIX_AUTO)
            {   // enable Auto-MDIX
                phyReg |= _UTP_MODECTRL_AUTO_MDIX_MASK;
            }
            else
            {	// no Auto-MDIX It is a Forced MDIX
                phyReg &= ~(_UTP_MODECTRL_AUTO_MDIX_MASK);	// disable Auto-MDIX
            }

            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_UTP_CONTROL_MODE, phyReg, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // done    
            if(oFlags & TCPIP_ETH_OPEN_MDIX_AUTO)
            {   // done for Auto-MDIX
                mdixConfPhase = 8;
            }
            else
            {
                mdixConfPhase = 4;
            }
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, mdixConfPhase);
            return DRV_ETHPHY_RES_PENDING;

        case 4:
            // Set the Page16 and reg 30 for Forced MDIX
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_PAGE_SEL, PAGENUM_16, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // done
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++mdixConfPhase);
            return DRV_ETHPHY_RES_PENDING;

        case 5:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_REG_MDIX_CNTL_SPFC_STATUS, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // advance to the next phase
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++mdixConfPhase);
            return DRV_ETHPHY_RES_PENDING;

        case 6:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // got PHY_REG_MDIX_CNTL_SPFC_STATUS result
            if(oFlags & TCPIP_ETH_OPEN_MDIX_SWAP)
            {
                phyReg |= _MDIX_CONTR_STATUS_FORCEMDIX_MASK;	// swap
            }
            else
            {
                phyReg &= ~(_MDIX_CONTR_STATUS_FORCEMDIX_MASK);	// normal
            }

            vendorData.low = mdixConfPhase + 1;
            vendorData.high = phyReg;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;

        case 7:
            phyReg = vendorData.high;
            // Set the PHY_REG_MDIX_CNTL_SPFC_STATUS
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_MDIX_CNTL_SPFC_STATUS, phyReg, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // done
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++mdixConfPhase);
            return DRV_ETHPHY_RES_PENDING;

        case 8:
           // reset the page number
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_PAGE_SEL, PAGENUM_0, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // done
            return DRV_ETHPHY_RES_OK;


        default:
            // shouldn't happen
            return DRV_ETHPHY_RES_OPERATION_ERR; 
    }
}


/****************************************************************************
 * Function:        DRV_EXTPHY_SMIClockGet
 *
 * PreCondition:    None
 *
 * Input:           handle - A valid open-instance handle, returned from the driver's open routine
 *
 * Output:          PHY MIIM clock, Hz
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the maximum clock frequency that the PHY can use for the MIIM transactions
 *
 * Note:            None
 *****************************************************************************/
static unsigned int DRV_EXTPHY_SMIClockGet(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE handle)
{
	return 2500000;		//  2.5 MHz max clock supported
}


// the DRV_ETHPHY_OBJECT

const DRV_ETHPHY_OBJECT  DRV_ETHPHY_OBJECT_IP_IP101GR = 
{
    DRV_EXTPHY_MIIConfigure,
    DRV_EXTPHY_MDIXConfigure,
    DRV_EXTPHY_SMIClockGet,
    0,                          // no WOL functionality yet
};




#ifdef PHY_WOL_ENABLE
/****************************************************************************
 * Function:        set_wol
 *
 * PreCondition:    Ethernet Initiazation should be completed.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *                   en -  Wol enable or disable
 *                   master - Master or slave mode
 *                   timer - timer value
 * Output:         none
 *
 *
 * Side Effects:    None
 *
 * Overview:       This api is used to configure the WOL  and is only used for IP101GR PHY driver . 
 *                      
 *
 * Note:            None
 *****************************************************************************/

static void set_wol(DRV_HANDLE hClientObj,int en, int master, int timer)
{   
    unsigned short wol; 
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_4);
    
    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_WOL_CNTRL);
    wol=DRV_ETHPHY_SMIReadResultGet(hClientObj);
    if (en)     
        wol |= _WOL_EN_MASK;   
    else        
        wol &= ~_WOL_EN_MASK;  
    
    if (master) 
    {
        wol |= _WOL_PLUS_MASTER_SLAVE_MASK;  // Master mode
        // set the INTR level to active HIGH
        wol |= _WOL_INTR_ACTIVE_HIGH_MASK; 
    }
    else
    {
        wol &= ~_WOL_PLUS_MASTER_SLAVE_MASK; // slave mode
        // set the INTR level to active HIGH
        wol &= ~_WOL_INTR_ACTIVE_HIGH_MASK; 
    }
    switch(timer)
    {
        case WOL_TIMER_30SEC:
            wol &= ~(_WOL_PLUS_TIMER_3MIN_SEL_MASK|_WOL_PLUS_TIMER_10MIN_SEL_MASK);
            wol |= _WOL_PLUS_TIMER_30SEC_SEL_MASK;  // timer set to 30SEC
            break;
        case WOL_TIMER_3MIN:
            wol &= ~(_WOL_PLUS_TIMER_30SEC_SEL_MASK|_WOL_PLUS_TIMER_10MIN_SEL_MASK);
            wol |= _WOL_PLUS_TIMER_3MIN_SEL_MASK;  // timer set to 3MIN
            break;
        case WOL_TIMER_10MIN:
            wol &= ~(_WOL_PLUS_TIMER_30SEC_SEL_MASK|_WOL_PLUS_TIMER_3MIN_SEL_MASK);
            wol |= _WOL_PLUS_TIMER_10MIN_SEL_MASK;  // timer set to 10MIN
            break;            
    }


    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_WOL_CNTRL, wol);

    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_WOL_CNTRL);
    wol=DRV_ETHPHY_SMIReadResultGet(hClientObj);

    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_0);
    
}

/****************************************************************************
 * Function:        set_magic_mac
 *
 * PreCondition:    Ethernet Initiazation should be completed.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *                   bAddr -  magic packet address or Unicast Mac address
 * Output:         none
 *
 *
 * Side Effects:    None
 *
 * Overview:       This API is used to set magic packet Mac address or unicast Mac address and is only used 
 *                       for IP101GR PHY driver . 
 *                      
 *
 * Note:            None
 *****************************************************************************/
static void set_magic_mac(DRV_HANDLE hClientObj,uint8_t *bAddr)
{
    union
    {
        double          align;          // 8 bytes aligned
        unsigned char   addr[6];        // MAC address, network order
    }uA;       // aligned MAC address
    
    memcpy(uA.addr, bAddr, sizeof(uA.addr)); // align properly
    unsigned short* pS=(unsigned short*)uA.addr;
    unsigned short macAddr01Byte = *pS++;
    unsigned short macAddr23Byte = *pS++;    
    unsigned short macAddr45Byte = *pS;
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_5);

    // due to little endian we don't need to convert this again.
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_WOL_MAC_ADDR, macAddr45Byte);    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_WOL_MAC_ADDR, macAddr23Byte);    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_WOL_MAC_ADDR, macAddr01Byte);

    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_0);
}

/****************************************************************************
 * Function:        set_wol_intr
 *
 * PreCondition:    Ethernet Initiazation should be completed.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *                   en - enable or disable intr state 
 * 
 * Output:         true or false
 *
 *
 * Side Effects:    None
 *
 * Overview:       This API is used to set the if WOL inerrupt to receive the WOL event packet.
 *                       and is onlyused for IP101GR PHY driver . 
 *                      
 *
 * Note:            None
 *****************************************************************************/

static bool set_wol_intr(DRV_HANDLE hClientObj,int en)
{
    int wol;
    
    if(!wol_intr32_pin_enabled(hClientObj))
        return false;
    if (!wol_enabled(hClientObj))
        return false;
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_17);
    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_WOL_STATUS);
    wol=DRV_ETHPHY_SMIReadResultGet(hClientObj);
    if (en)
    {
        wol = (wol & ~_WOL_PLUS_INTR_DIS_MASK);
    }
    else
    {
        wol = (wol |_WOL_PLUS_INTR_DIS_MASK);
    }
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_WOL_STATUS, wol);

    
    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_WOL_STATUS);
    wol=DRV_ETHPHY_SMIReadResultGet(hClientObj);
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_0);
    return true;
}

/****************************************************************************
 * Function:        wol_enabled
 *
 * PreCondition:    Ethernet Initiazation should be completed.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *
 * Output:         true or false
 *
 *
 * Side Effects:    None
 *
 * Overview:       This API is used to get the if WOL is enabled or disabled.
 *                       and is only used for IP101GR PHY driver . 
 *                      
 *
 * Note:            None
 *****************************************************************************/

static bool wol_enabled(DRV_HANDLE hClientObj)
{
    unsigned short wol;
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_4);
    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_WOL_CNTRL);
    wol=DRV_ETHPHY_SMIReadResultGet(hClientObj);
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_0);
    return (wol & _WOL_EN_MASK) ? true : false;
}
#if 0
// TODO : This API can be used later
/****************************************************************************
 * Function:        wol_intr_enabled
 *
 * PreCondition:    Ethernet Initiazation should be completed.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *
 * Output:         true or false
 *
 *
 * Side Effects:    None
 *
 * Overview:       This api is used to get the interupt status of WOL event.
 *                       and is only used for IP101GR PHY driver . 
 *                      
 *
 * Note:            None
 *****************************************************************************/

static bool wol_intr_enabled(DRV_HANDLE hClientObj)
{
    unsigned short intr;

    if(!wol_intr32_pin_enabled(hClientObj))
        return false;
    
    if (!wol_enabled(hClientObj))      
        return false; 

    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_17);
    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_WOL_STATUS);
    intr=DRV_ETHPHY_SMIReadResultGet(hClientObj);

    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_0);
    return (intr & _WOL_PLUS_INTR_DIS_MASK) ? true : false;
}
#endif
/****************************************************************************
 * Function:        wol_intr32_pin_enabled
 *
 * PreCondition:    Ethernet Initiazation should be completed.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *
 * Output:         true or false
 *
 *
 * Side Effects:    None
 *
 * Overview:       This api is used to get status of WOL INTR32 pin.
 *                       and is only used for IP101GR PHY driver . 
 *                      
 *
 * Note:            None
 *****************************************************************************/
static bool wol_intr32_pin_enabled(DRV_HANDLE hClientObj)
{
    unsigned short intr;

    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_16);
    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_INTR_STATUS);
    intr=DRV_ETHPHY_SMIReadResultGet(hClientObj);

    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_0);
    return (intr & _INTERRUPTSTATUS_INTR_MASK) ? true : false;
}

/****************************************************************************
 * Function:        set_wol_intr32_pin
 *
 * PreCondition:    Ethernet Initiazation should be completed.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *
 * Output:         none
 *
 *
 * Side Effects:    None
 *
 * Overview:       This api is used set WOL intr32 pin to receive WOL event 
 *                       and is only used for IP101GR PHY driver . 
 *                      
 *
 * Note:            None
 *****************************************************************************/

static void set_wol_intr32_pin(DRV_HANDLE hClientObj,int en)
{
    unsigned short intr;

    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_16);
    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_INTR_STATUS);
    intr=DRV_ETHPHY_SMIReadResultGet(hClientObj);
    if(en)
    {
        intr |= _INTERRUPTSTATUS_INTR_MASK;
    }
    else
    {
        intr &= ~_INTERRUPTSTATUS_INTR_MASK;
    }
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_INTR_STATUS, intr);

    
    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_INTR_STATUS);
    intr=DRV_ETHPHY_SMIReadResultGet(hClientObj);
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_0);
}

/****************************************************************************
 * Function:        check_wol_status
 *
 * PreCondition:    Ethernet Initiazation should be completed.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *
 * Output:         WOL state
 *
 *
 * Side Effects:    None
 *
 * Overview:       This api is used get WOL state 
 *                       and is only used for IP101GR PHY driver . 
 *                      
 *
 * Note:            None
 *****************************************************************************/

static eWOL_STATE check_wol_status(DRV_HANDLE hClientObj)
{
    WOL_OPERATION_MODE mode;
    unsigned short  status; 
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_17);
    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_WOL_STATUS);
    status=DRV_ETHPHY_SMIReadResultGet(hClientObj);

    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_0);

    mode = read_wol_mode(hClientObj);
    if (mode == WOL_MODE_MASTER)
    {
        if (status & _WOL_PLUS_SLEEPING_STATUS_MASK) 
            return  WOL_SLEEPING;
        else if(status & _WOL_PLUS_WAKE_STATUS_MASK)
            return WOL_WAKEUP;
        else
            return  WOL_NORMAL;
    }
    else
    { 
        status &= (_WOL_PLUS_SLEEPING_STATUS_MASK | _WOL_PLUS_SLEEP_MASK | _WOL_PLUS_WAKE_STATUS_MASK);     
        if (status == _WOL_PLUS_SLEEP_MASK) 
            return  WOL_RDY4SLP;
        else if (status == (_WOL_PLUS_WAKE_STATUS_MASK |_WOL_PLUS_SLEEPING_STATUS_MASK))
            return  WOL_RDY4WAKE;
        else if(status & _WOL_PLUS_WAKE_STATUS_MASK)
            return WOL_RDY4WAKE;
       /* else if (status & _WOL_PLUS_SLEEPING_STATUS_MASK)
            return  WOL_SLEEPING;
            */
        else
            return  WOL_NORMAL; 
    }
}

/****************************************************************************
 * Function:        read_wol_mode
 *
 * PreCondition:    Ethernet Initiazation should be completed.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *
 * Output:         Master or slave WOL mode
 *
 *
 * Side Effects:    None
 *
 * Overview:       This api is used get WOL event operation mode whether it is operated on Master or Slave mode
 *                       and is only used for IP101GR PHY driver . 
 *                      
 *
 * Note:            None
 *****************************************************************************/
static WOL_OPERATION_MODE read_wol_mode(DRV_HANDLE hClientObj)
{  
    unsigned short mode; 
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_4);
    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_WOL_CNTRL);
    mode=DRV_ETHPHY_SMIReadResultGet(hClientObj);
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_0);
    return (mode & _WOL_PLUS_MASTER_SLAVE_MASK) ? WOL_MODE_MASTER : WOL_MODE_SLAVE;
}

/****************************************************************************
 * Function:        set_sense_magic_pkt
 *
 * PreCondition:    Ethernet Initiazation should be completed.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *
 * Output:         none
 *
 *
 * Side Effects:    None
 *
 * Overview:       This api is used set magic packet option for a WOL event
 *                       and is only used for IP101GR PHY driver . 
 *                      
 *
 * Note:            None
 *****************************************************************************/

static void set_sense_magic_pkt(DRV_HANDLE hClientObj,int magic)
{   
    unsigned short wol;
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_4);
    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_WOL_CNTRL);
    wol = DRV_ETHPHY_SMIReadResultGet(hClientObj);

    if (magic)
    {
        wol =  wol | _WOL_SENSE_MAGIC_PKT_MASK;  
    }
    else
    {
        wol =  wol & ~_WOL_SENSE_MAGIC_PKT_MASK;  
    }
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_WOL_CNTRL, wol);
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_0);
}
#if 0
// TODO : This API can be used later
/****************************************************************************
 * Function:        read_sense_magic_pkt
 *
 * PreCondition:    Ethernet Initiazation should be completed.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *
 * Output:         true or false
 *
 *
 * Side Effects:    None
 *
 * Overview:       This api is used get result if driver has sensed a magic packet for a WOL event
 *                       and is only used for IP101GR PHY driver . 
 *                      
 *
 * Note:            None
 *****************************************************************************/

static bool read_sense_magic_pkt(DRV_HANDLE hClientObj)
{
    unsigned short magic;
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_4);
    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_WOL_CNTRL);
    magic = DRV_ETHPHY_SMIReadResultGet(hClientObj);
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_0);
    return (magic & _WOL_SENSE_MAGIC_PKT_MASK) ? true : false;
}
#endif
/****************************************************************************
 * Function:        set_sense_any_pkt
 *
 * PreCondition:    Ethernet Initiazation should be completed.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *
 * Output:        none
 *
 *
 * Side Effects:    None
 *
 * Overview:       This api is used set any packet  for WOL event
 *                       and is only used for IP101GR PHY driver . 
 *                      
 *
 * Note:            None
 *****************************************************************************/
static void set_sense_any_pkt(DRV_HANDLE hClientObj,int anypkt)
{
    unsigned short wol;
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_4);
    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_WOL_CNTRL);
    wol = DRV_ETHPHY_SMIReadResultGet(hClientObj);

    if (anypkt)
    {
        wol =  wol | _WOL_SENSE_ANY_PKT_MASK;  
    }
    else
    {
        wol =  wol & ~_WOL_SENSE_ANY_PKT_MASK;  
    }

    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_WOL_CNTRL, wol);
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_0);
}
#if 0
// TODO : This API can be used later
/****************************************************************************
 * Function:        read_sense_any_pkt
 *
 * PreCondition:    Ethernet Initiazation should be completed.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *
 * Output:         true or false
 *
 *
 * Side Effects:    None
 *
 * Overview:       This api is used get the result if Any packet is sensed when for WOL event
 *                       and is only used for IP101GR PHY driver . 
 *                      
 *
 * Note:            None
 *****************************************************************************/
static bool read_sense_any_pkt(DRV_HANDLE hClientObj)
{
    unsigned short anypkt;
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_4);
    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_WOL_CNTRL);
    anypkt = DRV_ETHPHY_SMIReadResultGet(hClientObj);
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_0);
    return (anypkt & _WOL_SENSE_ANY_PKT_MASK) ? true : false;
}
#endif
/****************************************************************************
 * Function:        manual_set_wol
 *
 * PreCondition:    Ethernet Initiazation should be completed.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *
 * Output:         none
 *
 *
 * Side Effects:    None
 *
 * Overview:       This api is used for manual configuration of WOL and is only used for IP101GR PHY driver . 
 *                      
 *
 * Note:            None
 *****************************************************************************/

static void manual_set_wol(DRV_HANDLE hClientObj)
{   
    int wol; 
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_4);
    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_WOL_CNTRL);
    wol = DRV_ETHPHY_SMIReadResultGet(hClientObj);
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_WOL_CNTRL, (wol|_WOL_PLUS_MANUAL_SET_MASK));

    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_0);
}
#if 0
// TODO : This API can be used later
/****************************************************************************
 * Function:        set_wol_dnspd_en
 *
 * PreCondition:    Ethernet Initiazation should be completed.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *                  en - down speed enable and disable
 *
 * Output:         none
 *
 *
 * Side Effects:    None
 *
 * Overview:       This api is used for WOL down soeed enable and disable and is only used for IP101GR PHY driver . 
 *                      
 *
 * Note:            None
 *****************************************************************************/
static void set_wol_dnspd_en(DRV_HANDLE hClientObj,int en)
{
    unsigned short wol;
    
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_4);
    DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_WOL_CNTRL);
    wol = DRV_ETHPHY_SMIReadResultGet(hClientObj);

    if (en)
    {
        wol =  wol | _WOL_PLUS_DOWN_SPEED_MASK;  
    }
    else
    {
        wol =  wol & ~_WOL_PLUS_DOWN_SPEED_MASK;  
    }
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_WOL_CNTRL, wol);
    DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_PAGE_SEL, PAGENUM_0);
}

#endif
/****************************************************************************
 * Function:        DRV_EXTPHY_IntInit
 *
 * PreCondition:    EthInit and EthPhyInit should have been called.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *                  src - interrupt source
 *                  intPri - intrrupt priority
 *                  intSubPri - Interuupt Sub Priority
 *
 * Output:         none
 *
 *
 * Side Effects:    None
 *
 * Overview:       Initialize External Interrupt 3.	One can take this as reference for their WOL 
 *                      Interrupt.
 *
 * Note:            None
 *****************************************************************************/
void DRV_EXTPHY_IntInit(DRV_HANDLE hClientObj,INT_SOURCE src,int intPri, int intSubPri)
{
	// set up the External Interrupt 3 with a prioirty of 5 and 1 sub-priority    

    SYS_INT_SourceDisable(src);      // stop Eth ints
    SYS_INT_SourceStatusClear(src);
    SYS_INT_VectorPrioritySet(src, intPri);
    SYS_INT_VectorSubprioritySet(src, intSubPri);

    SYS_INT_SourceEnable(src);

}

static void IP101GRWOLIsr(void *p)
{
   DRV_HANDLE hClientObj = (DRV_HANDLE)p;
   eWOL_STATE wol_status;
   wol_status = check_wol_status(hClientObj);
   
   INT_SOURCE src = DRV_ETHPHY_INTERRUPT_SOURCE;
   
   LEDstate ^= SYS_USERIO_LED_ASSERTED;
   SYS_USERIO_SetLED(SYS_USERIO_LED_1, LEDstate);

   switch(wol_status)
   {
        case WOL_SLEEPING:
        case WOL_RDY4SLP:
        {
            manual_set_wol(hClientObj); // change the state of PHY from Normal mode to sleep mode
            break;
        }
        case WOL_WAKEUP: 
        {
            set_wol(hClientObj,IP101GR_WOL_DISABLE,WOL_MODE_MASTER,WOL_TIMER_30SEC);
            break;
        }
        case WOL_RDY4WAKE:
        {
            manual_set_wol(hClientObj); // change the state of PHY from Sleep mode to Wakeup mode
            set_wol(hClientObj,IP101GR_WOL_DISABLE,WOL_MODE_SLAVE,WOL_TIMER_30SEC);
            break;
        }
        case WOL_NORMAL:

        break;        
   }
   SYS_INT_SourceDisable(src);
}



/****************************************************************************
 * Function:        DRV_EXTPHY_WOLConfiguration
 *
 * PreCondition:    EthInit and EthPhyInit should have been called.
 *
 * Input:          hClientObj - A valid open-instance handle, returned from the driver's open routine
 *                    bAddr[] -  Source Mac Address , or a Magic Packet MAC address
 *
 * Output:         none
 *
 *
 * Side Effects:    None
 *
 * Overview:       Configure WOL for IP101GR with a Source MAC address or a 6 byte magic packet mac address.
 *
 * Note:            TODO: This function needs to be converted to the non-blocking Harmony model!
 *****************************************************************************/
void  DRV_EXTPHY_WOLConfiguration(DRV_HANDLE hClientObj,unsigned char bAddr[])
{
    unsigned char bAddr1[6] = {0x11,0x22,0x33,0x44,0x55,0x66};
    //unsigned char bAddr1[6] = {0x66,0x55,0x44,0x33,0x22,0x11};
    WOL_FUNCTION wol_en=IP101GR_WOL_ENBALE;
    WOL_OPERATION_MODE wol_mode=WOL_MODE_SLAVE;
    IP101GR_WOL_TIMER wol_timer=WOL_TIMER_30SEC;
    
    INT_SOURCE src = DRV_ETHPHY_INTERRUPT_SOURCE;

    // Set MAC address
    set_magic_mac(hClientObj,bAddr1);

    // enable interrupt pin
    set_wol_intr32_pin(hClientObj,IP101GR_INTR_PIN_32_ENABLE);

    //wake up when receive Magic packet
    set_sense_magic_pkt(hClientObj,IP101GR_WOL_ENBALE);
    set_sense_any_pkt(hClientObj,IP101GR_WOL_DISABLE);
    // enable WOL , Slave mode, Time to default 30 sec, INTR active LOw in slave mode
    set_wol(hClientObj,wol_en,wol_mode,wol_timer);

    // Set interrupt pin disable
    set_wol_intr(hClientObj,wol_en);

    // forced to sleep , manual set to sleep    
    manual_set_wol(hClientObj); // change the state of PHY from Normal mode to sleep mode
    
	DRV_EXTPHY_IntInit(hClientObj,src,3,0);
}
#endif /* PHY_WOL_ENABLE */



