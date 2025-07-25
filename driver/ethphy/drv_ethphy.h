/**************************************************************************
  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_ethphy.h

  Summary:
    Ethernet ETHPHY Device Driver Interface File

  Description:
    Ethernet ETHPHY Device Driver Interface
    
    The Ethernet ETHPHY device driver provides a simple interface to manage
    an Ethernet ETHPHY peripheral using MIIM (or SMI) interface. This file
    defines the interface definitions and prototypes for the Ethernet
    ETHPHY driver.                                                         
  **************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2013-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef H_DRV_ETHPHY_H
#define H_DRV_ETHPHY_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "system/system_common.h"
#include "system/system_module.h"

#include "driver/driver_common.h"


#include "tcpip/tcpip_ethernet.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END  


// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Ethernet PHY Driver Operation Result
 *
  Summary:
    Defines the possible results of Ethernet operations that can succeed or
    fail

  Description:
    PHY Driver Operation Result Codes
    
    This enumeration defines the possible results of any of the PHY
    driver operations that have the possibility of failing. This result
    should be checked to ensure that the operation achieved the desired
    result.                                                                
*/

typedef enum
{
    /* Everything ok */
    DRV_ETHPHY_RES_OK                      /*DOM-IGNORE-BEGIN*/ =  0, /*DOM-IGNORE-END*/

    /* Operation pending, in progress */
    DRV_ETHPHY_RES_PENDING                 /*DOM-IGNORE-BEGIN*/ =  1, /*DOM-IGNORE-END*/

    /* Device/transaction status is busy */
    /* Driver busy with a previous operation and operation needs to be retried */
    DRV_ETHPHY_RES_BUSY                    /*DOM-IGNORE-BEGIN*/ =  2, /*DOM-IGNORE-END*/


    /* Errors */

    /* No PHY was detected or it failed to respond to reset command */
    DRV_ETHPHY_RES_DTCT_ERR               /*DOM-IGNORE-BEGIN*/ =  -1, /*DOM-IGNORE-END*/

    /* Failed to clear the BMCON detect bits */
    DRV_ETHPHY_RES_DTCT_CLR_ERR             = -2,

    /* Failed to clear the BMCON reset bits */
    DRV_ETHPHY_RES_DTCT_RST_CLR_ERR         = -3,

    /* Timeout in Reset read step */
    DRV_ETHPHY_RES_DTCT_RST_TMO_ERR         = -4,

    /* Timeout in the detection procedure */
    DRV_ETHPHY_RES_DTCT_TMO               /*DOM-IGNORE-BEGIN*/ =  -5, /*DOM-IGNORE-END*/

    /* No match between the capabilities: the PHY supported and the open
      requested ones */
    DRV_ETHPHY_RES_CPBL_ERR               /*DOM-IGNORE-BEGIN*/ =  -6, /*DOM-IGNORE-END*/

    /* Hardware configuration doesn't match the requested open mode */
    DRV_ETHPHY_RES_CFG_ERR                /*DOM-IGNORE-BEGIN*/ =  -7, /*DOM-IGNORE-END*/

    /* No negotiation active */
    DRV_ETHPHY_RES_NEGOTIATION_INACTIVE   /*DOM-IGNORE-BEGIN*/ =  -8, /*DOM-IGNORE-END*/

    /* No negotiation support */
    DRV_ETHPHY_RES_NEGOTIATION_UNABLE     /*DOM-IGNORE-BEGIN*/ =  -9, /*DOM-IGNORE-END*/

    /* Negotiation not started yet */
    DRV_ETHPHY_RES_NEGOTIATION_NOT_STARTED/*DOM-IGNORE-BEGIN*/ =  -10, /*DOM-IGNORE-END*/

    /* Negotiation active */
    DRV_ETHPHY_RES_NEGOTIATION_ACTIVE     /*DOM-IGNORE-BEGIN*/ =  -11, /*DOM-IGNORE-END*/

    /* Unsupported or operation error */
    DRV_ETHPHY_RES_OPERATION_ERR          /*DOM-IGNORE-BEGIN*/ =  -12, /*DOM-IGNORE-END*/

    /* Driver busy with a previous operation */
    DRV_ETHPHY_RES_NOT_READY_ERR          /*DOM-IGNORE-BEGIN*/ =  -13, /*DOM-IGNORE-END*/

    /* Passed handle is invalid */
    DRV_ETHPHY_RES_HANDLE_ERR             /*DOM-IGNORE-BEGIN*/ =  -14, /*DOM-IGNORE-END*/

    /* Operation aborted */
    DRV_ETHPHY_RES_ABORTED                /*DOM-IGNORE-BEGIN*/ =  -15, /*DOM-IGNORE-END*/

    /* MIIM Driver Operation Error */
    DRV_ETHPHY_RES_MIIM_ERR              /*DOM-IGNORE-BEGIN*/ =   -16, /*DOM-IGNORE-END*/

} DRV_ETHPHY_RESULT; 

// *****************************************************************************
/* Ethernet PHY Configuration Flags

  Summary:
    Defines configuration options for the Ethernet PHY.

  Description:
    This enumeration defines configuration options for the Ethernet PHY.
    Used by: DRV_ETHPHY_MIIConfigure, DRV_ETHPHY_INIT structure, DRV_ETHPHY_Setup,
    Returned by: DRV_ETHPHY_HWConfigFlagsGet
*/

typedef enum
{
    /*  RMII data interface in configuration fuses. */
    DRV_ETHPHY_CFG_RMII        /*DOM-IGNORE-BEGIN*/ = 0x01 /*DOM-IGNORE-END*/ ,
    /*  MII data interface in configuration fuses. */
    DRV_ETHPHY_CFG_MII         /*DOM-IGNORE-BEGIN*/ = 0x02 /*DOM-IGNORE-END*/ ,
    /*  GMII data interface in configuration fuses. */
    DRV_ETHPHY_CFG_RGMII       /*DOM-IGNORE-BEGIN*/ = 0x04 /*DOM-IGNORE-END*/ ,
    /*  GMII data interface in configuration fuses. */
    DRV_ETHPHY_CFG_GMII        /*DOM-IGNORE-BEGIN*/ = 0x08 /*DOM-IGNORE-END*/ ,
    /*  I/O Configuration fuses is ALT */
    DRV_ETHPHY_CFG_ALTERNATE   /*DOM-IGNORE-BEGIN*/ = 0x10 /*DOM-IGNORE-END*/ ,

    /*  I/O Configuration fuses is DEFAULT */
    DRV_ETHPHY_CFG_DEFAULT     /*DOM-IGNORE-BEGIN*/ = 0x00 /*DOM-IGNORE-END*/ ,

    /*  Use the fuses configuration to detect if you are RMII/MII and ALT/DEFAULT configuration */
    /*  NOTE: - this option does not check the consistency btw the software call and the way the */
    /*          fuses are configured. If just assumes that the fuses are properly configured. */
    /*        - option is valid for DRV_ETHPHY_Setup() call only! */
    DRV_ETHPHY_CFG_AUTO        /*DOM-IGNORE-BEGIN*/ = 0x40 /*DOM-IGNORE-END*/

} DRV_ETHPHY_CONFIG_FLAGS;

// *****************************************************************************
/* Ethernet PHY Interface Type

  Summary:
    Defines the type of interface a PHY supports.

  Description:
    This enumeration defines the type of interface supported by the PHY
    Returned by: DRV_ETHPHY_GetInterfaceType
*/

typedef enum
{
    /*  External Interface */
    DRV_ETHPHY_INF_TYPE_EXTERNAL,

     /*  Internal Interface */
    DRV_ETHPHY_INF_TYPE_INTERNAL,

    /*  Not Supported */
    DRV_ETHPHY_INF_TYPE_NOT_SUPPORTED
} DRV_ETHPHY_INTERFACE_TYPE;

// *****************************************************************************
/* Ethernet PHY Interface Index

  Summary:
    Defines the index type for a PHY interface.

  Description:
    This enumeration defines the index type supported by the PHY
    Used by: DRV_ETHPHY_PhyAddressGet, DRV_ETHPHY_RestartNegotiation, 
             DRV_ETHPHY_NegotiationIsComplete, DRV_ETHPHY_LinkStatusGet
*/

typedef enum
{
    /*  All External Interfaces */
    DRV_ETHPHY_INF_IDX_ALL_EXTERNAL,

     /*  Port 0 interface */
    DRV_ETHPHY_INF_IDX_PORT_0,

     /*  Port 1 interface */
    DRV_ETHPHY_INF_IDX_PORT_1,
            
     /*  Port 2 interface */
    DRV_ETHPHY_INF_IDX_PORT_2,

     /*  Port 3 interface */
    DRV_ETHPHY_INF_IDX_PORT_3,

     /*  Port 4 interface */
    DRV_ETHPHY_INF_IDX_PORT_4,
            
     /*  Port 5 interface */
    DRV_ETHPHY_INF_IDX_PORT_5,
            
} DRV_ETHPHY_INTERFACE_INDEX;


// *****************************************************************************
/* Ethernet PHY Driver Base Object

  Summary:
    Forward declaration of a PHY base object.

  Description:
    This data structure identifies the required basic interface of the Ethernet PHY driver.
    Any dynamic PHY driver has to export this interface.

  Remarks:
    The PHY driver consists of 2 modules:
        - the main/base PHY driver which uses standard IEEE PHY registers
        - the vendor specific functionality
    This object provides the base functionality.
    Every dynamic PHY driver has to expose this basic functionality as part of its interface.

    See the following structure definition for details of its memebers.
*/

struct DRV_ETHPHY_OBJECT_BASE_TYPE;

// *****************************************************************************
/* Pointer To Function:
    typedef DRV_ETHPHY_RESULT  (* DRV_ETHPHY_VENDOR_MII_CONFIGURE) (const struct DRV_ETHPHY_OBJECT_BASE_TYPE* pBaseObj, DRV_HANDLE handle, DRV_ETHPHY_CONFIG_FLAGS cFlags );

  Summary:
    Pointer to function to configure the Ethernet PHY in one of the MII/RMII operation modes.

  Description:
    This type describes a pointer to a function that configures the Ethernet PHY
    in one of the MII/RMII operation modes.
    This configuration function is PHY specific and every PHY driver has to provide their own implementation. 

  Precondition:
    Communication to the PHY should have been established.

  Parameters:
    - pBaseObj- pointer to a PHY Base object
    - handle  - Client's driver handle (returned from DRV_ETHPHY_Open)
    - cFlags  - Requested configuration flags: DRV_ETHPHY_CFG_RMII or DRV_ETHPHY_CFG_MII

  Returns:
    - DRV_ETHPHY_RES_OK      - if success, operation complete
    - DRV_ETHPHY_RES_PENDING - if function needs to be called again
    < 0               - on failure: configuration not supported or some other error

  Remarks:
    The PHY driver consists of 2 modules:
        - the main/base PHY driver which uses standard IEEE PHY registers
        - the vendor specific functionality
    This function provides vendor specific functionality.
    Every PHY driver has to expose this vendor specific function as part of its interface.
    
    Traditionally the name used for this function is DRV_EXTPHY_MIIConfigure but any name can be used.

    The PHY driver will call the vendor set up functions after the communication to the PHY has been established.

    The function can use all the vendor specific functions to store/retrieve specific data
    or start SMI transactions (see Vendor Interface Routines). 

    The function should not block but return DRV_ETHPHY_RES_PENDING if waiting for SMI transactions.
*/

typedef DRV_ETHPHY_RESULT  (* DRV_ETHPHY_VENDOR_MII_CONFIGURE) ( const struct DRV_ETHPHY_OBJECT_BASE_TYPE* pBaseObj, DRV_HANDLE handle, DRV_ETHPHY_CONFIG_FLAGS cFlags );


// *****************************************************************************
/* Pointer To Function:
    typedef DRV_ETHPHY_RESULT  (* DRV_ETHPHY_VENDOR_MDIX_CONFIGURE) ( const struct DRV_ETHPHY_OBJECT_BASE_TYPE* pBaseObj, DRV_HANDLE handle, TCPIP_ETH_OPEN_FLAGS oFlags );

  Summary:
    Pointer to function that configures the MDIX mode for the Ethernet PHY.

  Description:
    This type describes a pointer to a function that configures the MDIX mode for the Ethernet PHY.
    This configuration function is PHY specific and every PHY driver has to provide their own implementation. 

  Precondition:
    Communication to the PHY should have been established.

  Parameters:
    - pBaseObj- pointer to a PHY Base object
    - handle  - Client's driver handle (returned from DRV_ETHPHY_Open)
    - oFlags  - Requested open flags: TCPIP_ETH_OPEN_MDIX_AUTO, TCPIP_ETH_OPEN_MDIX_NORM,
                or TCPIP_ETH_OPEN_MDIX_NORM | TCPIP_ETH_OPEN_MDIX_SWAP

  Returns:
    - DRV_ETHPHY_RES_OK      - if success, operation complete
    - DRV_ETHPHY_RES_PENDING - if function needs to be called again
    < 0               - on failure: configuration not supported or some other error

  Remarks:
    The PHY driver consists of 2 modules:
        - the main/base PHY driver which uses standard IEEE PHY registers
        - the vendor specific functionality
    This function provides vendor specific functionality.
    Every PHY driver has to expose this vendor specific function as part of its interface.
    
    Traditionally the name used for this function is DRV_EXTPHY_MDIXConfigure but any name can be used.

    The function can use all the vendor specific functions to store/retrieve specific data
    or start SMI transactions (see Vendor Interface Routines). 

    The function should not block but return DRV_ETHPHY_RES_PENDING if waiting for SMI transactions.
*/

typedef DRV_ETHPHY_RESULT  (* DRV_ETHPHY_VENDOR_MDIX_CONFIGURE) ( const struct DRV_ETHPHY_OBJECT_BASE_TYPE* pBaseObj, DRV_HANDLE handle, TCPIP_ETH_OPEN_FLAGS oFlags );


// *****************************************************************************
/* Pointer to Function:
    typedef unsigned int  (* DRV_ETHPHY_VENDOR_SMI_CLOCK_GET) ( const struct DRV_ETHPHY_OBJECT_BASE_TYPE* pBaseObj, DRV_HANDLE handle );

  Summary:
    Pointer to a function to return the SMI/MIIM maximum clock speed in Hz of the Ethernet PHY.

  Description:
    This type describes a pointer to a function that returns the SMI/MIIM
    maximum clock speed in Hz of the Ethernet PHY.
    This configuration function is PHY specific and every PHY driver has to provide their own implementation. 

  Precondition:
    Communication to the PHY should have been established.

  Parameters:
    - pBaseObj- pointer to a PHY Base object
    - handle  - Client's driver handle (returned from DRV_ETHPHY_Open)

  Returns:
    The maximum SMI/MIIM clock speed as an unsigned integer.


  Remarks:
    The PHY driver consists of 2 modules:
        - the main/base PHY driver which uses standard IEEE PHY registers
        - the vendor specific functionality
    This function provides vendor specific functionality.
    Every PHY driver has to expose this vendor specific function as part of its interface.
    
    This value is PHY specific. All PHYs are requested to support 2.5 MHz.

    Traditionally the name used for this function is DRV_EXTPHY_SMIClockGet but any name can be used.

    The PHY driver will call the vendor set up functions after the communication to the PHY has been established.

    The function should not block but return immediately.
    The function cannot start SMI transactions and cannot use the vendor specific functions
    to store/retrieve specific data (see Vendor Interface Routines). 
*/

typedef unsigned int  (* DRV_ETHPHY_VENDOR_SMI_CLOCK_GET) ( const struct DRV_ETHPHY_OBJECT_BASE_TYPE* pBaseObj, DRV_HANDLE handle );



// *****************************************************************************
/* Pointer to Function:
    typedef void (* DRV_ETHPHY_VENDOR_WOL_CONFIGURE) ( const struct DRV_ETHPHY_OBJECT_BASE_TYPE* pBaseObj, DRV_HANDLE handle, unsigned char bAddr[]);

  Summary:
    Pointer to a function to configure the PHY WOL functionality
   
  Description:
    This type describes a pointer to a function that configures
    the PHY WOL functionality of the Ethernet PHY.
    Configures the WOL of the PHY with a Source MAC address
    or a 6 byte magic packet mac address.

    This configuration function is PHY specific and every PHY driver has to provide their own implementation. 

  Precondition:
    Communication to the PHY should have been established.

  Parameters:
    - pBaseObj- pointer to a PHY Base object
    - handle  - Client's driver handle (returned from DRV_ETHPHY_Open)
    - bAddr[] - Source Mac Address, or a Magic Packet MAC address

  Returns:
    None


  Remarks:
    The PHY driver consists of 2 modules:
        - the main/base PHY driver which uses standard IEEE PHY registers
        - the vendor specific functionality
    This function provides vendor specific functionality.
    Every PHY driver has to expose this vendor specific function as part of its interface.
    
    Traditionally the name used for this function is DRV_EXTPHY_WOLConfiguration but any name can be used.

    The PHY driver will call the vendor set up functions after the communication to the PHY has been established.

    The function can use all the vendor specific functions to store/retrieve specific data
    or start SMI transactions (see Vendor Interface Routines). 

    The function should not block but return DRV_ETHPHY_RES_PENDING if waiting for SMI transactions.

    This feature is not currently supported for all PHYs.
*/

typedef void (* DRV_ETHPHY_VENDOR_WOL_CONFIGURE) ( const struct DRV_ETHPHY_OBJECT_BASE_TYPE* pBaseObj, DRV_HANDLE handle, unsigned char bAddr[]);

// *****************************************************************************
/* Pointer to Function:
    typedef DRV_ETHPHY_RESULT (* DRV_ETHPHY_VENDOR_DETECT) ( const struct DRV_ETHPHY_OBJECT_BASE_TYPE* pBaseObj, DRV_HANDLE handle);

  Summary:
    Pointer to a function to perform the detection of a PHY
   
  Description:
    This type describes a pointer to a function that performs
    the PHY detection procedure.
    The function should decide if the communication with the PHY is working correctly
    and the PHY responds properly to the read/write attempts done by the driver.

    If this function is not provided (0) a default detection function is used
         - The default detection function uses the BMCON register:
            - reads the register to check that the RESET bit is cleared
            - sets the LOOPBACK and DUPLEX bits and writes them to BMCON
            - reads back the register and checks that LOOPBACK and DUPLEX bits were set
            - resets the LOOPBACK and DUPLEX bits, write back and checks that they were cleared
            - if all operations were completed successfully, the detection procedure has passed
              otherwise a failure will be returned

    Otherwise the PHY driver can provide their own implementation

  Precondition:
    The PHY driver has been initialized

  Parameters:
    - pBaseObj- pointer to a PHY Base object
    - handle  - Client's driver handle (returned from DRV_ETHPHY_Open)

  Returns:
    - DRV_ETHPHY_RES_OK      - if success, operation complete
    - DRV_ETHPHY_RES_PENDING - if function needs to be called again
    < 0               - on failure: communication with the PHY failed or some other error

  Remarks:
    The PHY driver consists of 2 modules:
        - the main/base PHY driver which uses standard IEEE PHY registers
        - the vendor specific functionality
    This function provides vendor specific functionality.
    Every PHY driver has to expose this vendor specific function as part of its interface.
    
    Traditionally the name used for this function is DRV_EXTPHY_Detect but any name can be used.

    The PHY driver will call the vendor detect function as part of the PHY initialization

    The function can use all the vendor specific functions to store/retrieve specific data
    or start SMI transactions (see Vendor Interface Routines). 

    The function should not block but return DRV_ETHPHY_RES_PENDING if waiting for SMI transactions.

    The function should complete within a specified time, otherwise a timeout will be generated.
    Currently this timeout is given by the DRV_ETHPHY_RESET_CLR_TMO parameter.

*/

typedef DRV_ETHPHY_RESULT (* DRV_ETHPHY_VENDOR_DETECT) ( const struct DRV_ETHPHY_OBJECT_BASE_TYPE* pBaseObj, DRV_HANDLE handle);


// *****************************************************************************
/* Pointer to Function:
    typedef void (* DRV_ETHPHY_RESET_FUNCTION) (const struct DRV_ETHPHY_OBJECT_BASE_TYPE*, DRV_HANDLE handle);

  Summary:
    Pointer to a function to perform an additional PHY reset
   
  Description:
    This type describes a pointer to a function that is called by the driver
    before starting the detection and initialization process to the PHY - 
    as a result of the DRV_ETHPHY_Setup call.


  Precondition:
    None

  Parameters:
    - pBaseObj- pointer to the PHY Base object that calls this function as a result of 
      performing its initialization procedure.
    - handle  - Client's driver handle (returned from DRV_ETHPHY_Open)

  Returns:
    None

  Remarks:

    The PHY driver will call this function as part of its detection and initialization procedure.
    It can be used for implementing extra steps that the user needs, before the driver starts
    talking to the PHY.
    For example, if a hard reset needs to be applied to the PHY.

    The function should be short and not block.
    It is meant just for short I/O operations, not for lengthy processing.
*/

typedef void (* DRV_ETHPHY_RESET_FUNCTION) (const struct DRV_ETHPHY_OBJECT_BASE_TYPE* basetype, DRV_HANDLE handle);


// *****************************************************************************
/* Ethernet PHY Driver Vendor Object

  Summary:
    Identifies the interface of a Ethernet PHY vendor driver.

  Description:
    This data structure identifies the required interface of the Ethernet PHY driver.
    Any PHY vendor driver has to export this interface.

  Remarks:
    The PHY driver consists of 2 modules:
        - the main/base PHY driver which uses standard IEEE PHY registers
        - the vendor specific functionality
    This object provides vendor specific functionality.
    Every PHY driver has to expose this vendor specific functionality as part of its interface.
*/

typedef struct
{
    /* PHY driver function to configure the operation mode: MII/RMII */
    DRV_ETHPHY_VENDOR_MII_CONFIGURE     miiConfigure;

    /* PHY driver function to configure the MDIX mode */
    DRV_ETHPHY_VENDOR_MDIX_CONFIGURE    mdixConfigure;

    /* PHY driver function to get the SMI clock rate */
    DRV_ETHPHY_VENDOR_SMI_CLOCK_GET     smiClockGet;    

    /* PHY driver function to configure the WOL functionality */
    DRV_ETHPHY_VENDOR_WOL_CONFIGURE     wolConfigure;

    /* PHY driver function to detect the PHY is present and working correctly.
     * If == 0, a default detection function will be used
     * see the DRV_ETHPHY_VENDOR_DETECT definition */
    DRV_ETHPHY_VENDOR_DETECT            phyDetect;

    /* Detect mask to be used by the detection procedure.
     * This mask represents read/write bits in the BMCON that can be manipulated
     * to detect that the SMI communication with the PHY works correctly.
     * If 0, then a default mask of BMCON_LOOPBACK_MASK | BMCON_DUPLEX_MASK BMCON will be used.
     * Drivers that need special processing can use their own detection mask for the default detection procedure
     * or use a specific detect procedure */
    uint16_t                            bmconDetectMask;

    /* Extra capability mask to be used by the PHY setup procedure.
     * Some PHYs need to be able to use extra bits set if their
     * BMSTAT register doesn't advertise the standard capabilities
     * (for example there is no 100Base-T1 FD support in the BMSTAT for LAN8770).
     * By default this value should be 0.
     * The BMSTAT OR-ed with this mask will be used for checking the PHY capabilities.
     * Note: this should be limited to 10/100 FD/HD capabilities mask! */
    uint16_t                            bmstatCpblMask;
}DRV_ETHPHY_OBJECT;



// *****************************************************************************
/* Ethernet PHY Driver Client Status

  Summary:
    Identifies the client-specific status of the Ethernet PHY driver.

  Description:
    This enumeration identifies the client-specific status of the Ethernet PHY driver.

  Remarks:
    None.
*/

typedef enum
{
    /* Unspecified error condition */
    DRV_ETHPHY_CLIENT_STATUS_ERROR   /*DOM-IGNORE-BEGIN*/ = DRV_CLIENT_STATUS_ERROR /*DOM-IGNORE-END*/,

   /* Client is not open */
    DRV_ETHPHY_CLIENT_STATUS_CLOSED  /*DOM-IGNORE-BEGIN*/ = DRV_CLIENT_STATUS_CLOSED /*DOM-IGNORE-END*/,

    /* An operation is currently in progress */
    DRV_ETHPHY_CLIENT_STATUS_BUSY    /*DOM-IGNORE-BEGIN*/ = DRV_CLIENT_STATUS_BUSY /*DOM-IGNORE-END*/,

    /* Up and running, no operations running */
    DRV_ETHPHY_CLIENT_STATUS_READY   /*DOM-IGNORE-BEGIN*/ = DRV_CLIENT_STATUS_READY /*DOM-IGNORE-END*/

} DRV_ETHPHY_CLIENT_STATUS;

// *****************************************************************************
/* Ethernet PHY Device Driver Time-Out Initialization Data

  Summary:
    Contains all the data necessary to initialize the PHY Device Driver Time-Outs.

  Description:
    This data structure contains all the data necessary to initialize the Ethernet PHY
    device.

  Remarks:
    A pointer to a structure of this format containing the desired
    initialization data must be passed into the DRV_ETHPHY_Initialize routine.
*/
typedef struct S_DRV_ETHPHY_TMO
{
    //ETH PHY Reset Clear Time-out (mSec)
    uint32_t                    resetTmo;
    //ETH PHY Auto-Negotiation Done Time-out (mSec)
    uint32_t                    aNegDoneTmo;
    //ETH PHY Auto-Negotiation Time-out (mSec)
    uint32_t                    aNegInitTmo;
}DRV_ETHPHY_TMO;
// *****************************************************************************
/* Ethernet PHY Device Driver Initialization Data

  Summary:
    Contains all the data necessary to initialize the Ethernet PHY device.

  Description:
    This data structure contains all the data necessary to initialize the Ethernet PHY
    device.

  Remarks:
    A pointer to a structure of this format containing the desired
    initialization data must be passed into the DRV_ETHPHY_Initialize routine.
*/

typedef struct DRV_ETHPHY_INIT
{
    /* System module initialization */
    SYS_MODULE_INIT             moduleInit;

    /* Identifies peripheral (PLIB-level) ID */
    uintptr_t                  ethphyId;

    /* PHY address, as configured on the board. */
    /* All PHYs respond to address 0 */
    uint16_t                    phyAddress;

    /*  PHY configuration  */
    DRV_ETHPHY_CONFIG_FLAGS     phyFlags;

    /* Non-volatile pointer to the PHY object providing vendor functions for this PHY */
    const DRV_ETHPHY_OBJECT*    pPhyObject;   

    /* Function to be called when the PHY is reset/initialized. */
    /* Could be NULL if no special reset functionality needed - default */
    DRV_ETHPHY_RESET_FUNCTION   resetFunction;

    /* Non-volatile pointer to the DRV_MIIM object providing MIIM access for this PHY */
    /* Could be NULL if the MIIM driver is not used */
    const struct S_DRV_MIIM_OBJECT_BASE* pMiimObject; 

    /* Non-volatile pointer to the DRV_MIIM initialization data */
    /* Could be NULL if the MIIM driver is not used */
    const struct S_DRV_MIIM_INIT* pMiimInit;

    /* MIIM module index to be used */
    /* Not needed if the MIIM driver is not used */
    SYS_MODULE_INDEX            miimIndex; 
            
    /* PHY Initialization Time-outs */
    DRV_ETHPHY_TMO *            ethphyTmo; 

} DRV_ETHPHY_INIT;


// *****************************************************************************
/* Ethernet PHY Device Driver Set up Data

  Summary:
    Contains all the data necessary to set up the Ethernet PHY device.

  Description:
    This data structure contains all the data necessary to configure the Ethernet PHY
    device.

  Remarks:
    A pointer to a structure of this format containing the desired
    initialization data must be passed into the DRV_ETHPHY_Setup routine.
*/

typedef struct
{
    int                         phyAddress;     // the address the PHY is configured for
    TCPIP_ETH_OPEN_FLAGS        openFlags;      // the capability flags: FD/HD, 100/100Mbps, etc.
    DRV_ETHPHY_CONFIG_FLAGS     configFlags;    // configuration flags: MII/RMII, I/O setup 
    TCPIP_ETH_PAUSE_TYPE        macPauseType;   // MAC requested pause type
    DRV_ETHPHY_RESET_FUNCTION   resetFunction;  // If ! NULL, function to be called when the PHY is reset/initialized
} DRV_ETHPHY_SETUP;

// *****************************************************************************
/* Ethernet PHY Device Link Status Codes

  Summary:
    Defines the possible status flags of PHY Ethernet link.

  Description:
    This enumeration defines the flags describing the status of the PHY Ethernet
    link.

  Remarks:
    Multiple flags can be set.
*/

typedef enum
{
    /*  No connection to the LinkPartner */
    DRV_ETHPHY_LINK_ST_DOWN           /*DOM-IGNORE-BEGIN*/ = 0x0 /*DOM-IGNORE-END*/,

    /*  Link is up */
    DRV_ETHPHY_LINK_ST_UP             /*DOM-IGNORE-BEGIN*/ = 0x1 /*DOM-IGNORE-END*/,

    /*  LP non negotiation able */
    DRV_ETHPHY_LINK_ST_LP_NEG_UNABLE  /*DOM-IGNORE-BEGIN*/ = 0x2 /*DOM-IGNORE-END*/,

    /*  LP fault during negotiation */
    DRV_ETHPHY_LINK_ST_REMOTE_FAULT   /*DOM-IGNORE-BEGIN*/ = 0x4 /*DOM-IGNORE-END*/,

    /*  Parallel Detection Fault encountered (when DRV_ETHPHY_LINK_ST_LP_NEG_UNABLE) */
    DRV_ETHPHY_LINK_ST_PDF            /*DOM-IGNORE-BEGIN*/ = 0x8 /*DOM-IGNORE-END*/,

    /*  LP supports symmetric pause */
    DRV_ETHPHY_LINK_ST_LP_PAUSE       /*DOM-IGNORE-BEGIN*/ = 0x10 /*DOM-IGNORE-END*/,

    /*  LP supports asymmetric TX/RX pause operation */
    DRV_ETHPHY_LINK_ST_LP_ASM_DIR     /*DOM-IGNORE-BEGIN*/ = 0x20 /*DOM-IGNORE-END*/,

    /*  LP not there */
    DRV_ETHPHY_LINK_ST_NEG_TMO        /*DOM-IGNORE-BEGIN*/ = 0x1000 /*DOM-IGNORE-END*/,

    /*  An unexpected fatal error occurred during the negotiation */
    DRV_ETHPHY_LINK_ST_NEG_FATAL_ERR  /*DOM-IGNORE-BEGIN*/ = 0x2000 /*DOM-IGNORE-END*/

} DRV_ETHPHY_LINK_STATUS; 

// *****************************************************************************
/* Ethernet PHY Device Driver Negotiation result Data

  Summary:
    Contains all the data necessary to get the Ethernet PHY negotiation result

  Description:
    Contains all the data necessary to get the Ethernet PHY negotiation result

  Remarks:
    A pointer to a structure of this format must be passed into the
    DRV_ETHPHY_NegotiationResultGet routine.
*/

typedef struct
{
    DRV_ETHPHY_LINK_STATUS  linkStatus;     // link status after a completed negotiation
    TCPIP_ETH_OPEN_FLAGS    linkFlags;      // the negotiation result flags
    TCPIP_ETH_PAUSE_TYPE    pauseType;      // pause type supported by the link partner

} DRV_ETHPHY_NEGOTIATION_RESULT;


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - System Level
// *****************************************************************************
// *****************************************************************************


/*****************************************************************************************
  Function:
       SYS_MODULE_OBJ DRV_ETHPHY_Initialize( const SYS_MODULE_INDEX        iModule,
                                             const SYS_MODULE_INIT * const init )
    
  Summary:
    Initializes the Ethernet PHY driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function initializes the Ethernet PHY driver, making it ready for
    clients to open and use it.

  Precondition:
    None.

  Parameters:
    - iModule  - Index for the driver instance to be initialized
    - init      - Pointer to a data structure containing any data necessary to
                  initialize the driver. This pointer may be null if no data
                  is required because static overrides have been provided.

  Returns:
    - a valid handle to a driver object, if successful.
    - SYS_MODULE_OBJ_INVALID if initialization failed.
    
  Example:
    <code>
    DRV_ETHPHY_INIT    init;
    SYS_MODULE_OBJ  objectHandle;
    
    # Populate the Ethernet PHY initialization structure
    init.phyId  = ETHPHY_ID_0;
    
    # Populate the Ethernet PHY initialization structure
    init.phyId  = ETHPHY_ID_2;
    init.pPhyObject  = &DRV_ETHPHY_OBJECT_LAN8720;
    
    # Do something
    
    objectHandle = DRV_ETHPHY_Initialize(0, (SYS_MODULE_INIT*)&init);
    if (SYS_MODULE_OBJ_INVALID == objectHandle)
    {
        # Handle error
    }
    </code>

  Remarks:
     - This function must be called before any other Ethernet PHY routine is
       called.
    -  This function should only be called once during system initialization
       unless DRV_ETHPHY_Deinitialize is called to deinitialize the driver
       instance.                                                                             
    - The returned object must be passed as argument to DRV_ETHPHY_Reinitialize,
      DRV_ETHPHY_Deinitialize, DRV_ETHPHY_Tasks and DRV_ETHPHY_Status routines.

  *****************************************************************************************/

SYS_MODULE_OBJ DRV_ETHPHY_Initialize ( const SYS_MODULE_INDEX        iModule,
                                       const SYS_MODULE_INIT * const init );


// *****************************************************************************
/* Function:
    void DRV_ETHPHY_Reinitialize( SYS_MODULE_OBJ                object,
                                  const SYS_MODULE_INIT * const init )

  Summary:
    Reinitializes the driver and refreshes any associated hardware settings.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function reinitializes the driver and refreshes any associated hardware
    settings using the initialization data given, but it will not interrupt any
    ongoing operations.

  Precondition:
    The DRV_ETHPHY_Initialize function must have been called before calling this
    routine and a valid SYS_MODULE_OBJ must have been returned.

  Parameters:
    - object          - Driver object handle, returned from the DRV_ETHPHY_Initialize
                        routine
    - init            - Pointer to the initialization data structure

  Returns:
    None.

  Example:
    <code>
    DRV_ETHPHY_INIT    init;
    SYS_MODULE_OBJ  objectHandle;

    # Populate the Ethernet PHY initialization structure
    init.phyId  = ETHPHY_ID_2;
    init.pPhyObject  = &DRV_ETHPHY_OBJECT_LAN8720;

    DRV_ETHPHY_Reinitialize(objectHandle, (SYS_MODULE_INIT*)&init);

    phyStatus = DRV_ETHPHY_Status(objectHandle);
    if (SYS_STATUS_BUSY == phyStatus)
    {
        # Check again later to ensure the driver is ready
    }
    else if (SYS_STATUS_ERROR >= phyStatus)
    {
        # Handle error
    }
    </code>

  Remarks:
    - This function can be called multiple times to reinitialize the module.
    - This operation can be used to refresh any supported hardware registers as
      specified by the initialization data or to change the power state of the
      module.
*/

void DRV_ETHPHY_Reinitialize ( SYS_MODULE_OBJ               object,
                               const SYS_MODULE_INIT * const init );


// *****************************************************************************
/* Function:
    void DRV_ETHPHY_Deinitialize ( SYS_MODULE_OBJ object )

  Summary:
    Deinitializes the specified instance of the Ethernet PHY driver module.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function deinitializes the specified instance of the Ethernet PHY driver
    module, disabling its operation (and any hardware) and invalidates all of the
    internal data.

  Precondition:
    The DRV_ETHPHY_Initialize function must have been called before calling this
    routine and a valid SYS_MODULE_OBJ must have been returned.

  Parameters:
    - object    - Driver object handle, returned from DRV_ETHPHY_Initialize

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     #  Returned from DRV_ETHPHY_Initialize
    SYS_STATUS          status;

    DRV_ETHPHY_Deinitialize(object);

    status = DRV_ETHPHY_Status(object);
    if (SYS_MODULE_DEINITIALIZED != status)
    {
        # Check again later if you need to know
        # when the driver is deinitialized.
    }
    </code>

  Remarks:
    - Once the Initialize operation has been called, the Deinitialize operation
      must be called before the Initialize operation can be called again.
*/

void DRV_ETHPHY_Deinitialize ( SYS_MODULE_OBJ object );


/**************************************************************************
  Function:
       SYS_STATUS DRV_ETHPHY_Status ( SYS_MODULE_OBJ object )
    
  Summary:
    Provides the current status of the Ethernet PHY driver module.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function provides the current status of the Ethernet PHY driver
    module.

  Precondition:
    The DRV_ETHPHY_Initialize function must have been called before calling
    this function.

  Parameters:
    object -  Driver object handle, returned from DRV_ETHPHY_Initialize

  Returns:
    - SYS_STATUS_READY  - Indicates that any previous module operation for the
                          specified module has completed
    - SYS_STATUS_BUSY   - Indicates that a previous module operation for the
                          specified module has not yet completed
    - SYS_STATUS_ERROR  - Indicates that the specified module is in an error state

  Example:
    <code>
    SYS_MODULE_OBJ      object;     # Returned from DRV_ETHPHY_Initialize
    SYS_STATUS          status;
    
    status = DRV_ETHPHY_Status(object);
    if (SYS_STATUS_ERROR >= status)
    {
        # Handle error
    }
    </code>

  Remarks:
    - Any value greater than SYS_STATUS_READY is also a normal running state
      in which the driver is ready to accept new operations.
    - SYS_STATUS_BUSY - Indicates that the driver is busy with a previous
      system level operation and cannot start another
    - SYS_STATUS_ERROR - Indicates that the driver is in an error state
    - Any value less than SYS_STATUS_ERROR is also an error state.
    - SYS_MODULE_DEINITIALIZED - Indicates that the driver has been
      deinitialized
    - The this operation can be used to determine when any of the driver's
      module level operations has completed.
    - If the status operation returns SYS_STATUS_BUSY, the a previous
      operation has not yet completed. Once the status operation returns
      SYS_STATUS_READY, any previous operations have completed.
    - The value of SYS_STATUS_ERROR is negative (-1). Any value less than
      that is also an error state.
    - This function will NEVER block waiting for hardware.
    - If the Status operation returns an error value, the error may be
      cleared by calling the reinitialize operation. If that fails, the
      deinitialize operation will need to be called, followed by the
      initialize operation to return to normal operations.                   
  **************************************************************************/

SYS_STATUS DRV_ETHPHY_Status ( SYS_MODULE_OBJ object );


/***************************************************************************
  Function:
       void DRV_ETHPHY_Tasks( SYS_MODULE_OBJ object )
    
  Summary:
    Maintains the driver's state machine and implements its ISR.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function is used to maintain the driver's internal state machine
    and implement its ISR for interrupt-driven implementations.

  Precondition:
    The DRV_ETHPHY_Initialize routine must have been called for the
    specified Ethernet PHY driver instance.

  Parameters:
    - object -  Object handle for the specified driver instance (returned from
                DRV_ETHPHY_Initialize)
  Returns:
    None
  Example:
    <code>
    SYS_MODULE_OBJ      object;     # Returned from DRV_ETHPHY_Initialize
    
    while (true)
    {
        DRV_ETHPHY_Tasks (object);
    
        # Do other tasks
    }
    </code>

  Remarks:
    - This function is normally not called directly by an application. It is
      called by the system's Tasks routine (SYS_Tasks)
    - This function will never block or access any resources that may cause
      it to block.                        
  ***************************************************************************/

void DRV_ETHPHY_Tasks( SYS_MODULE_OBJ object );


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Client Level
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_ETHPHY_Open( const SYS_MODULE_INDEX iModule,
                                const DRV_IO_INTENT    ioIntent )

  Summary:
    Opens the specified Ethernet PHY driver instance and returns a handle to it.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function opens the specified Ethernet PHY driver instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver.

  Precondition:
    The DRV_ETHPHY_Initialize function must have been called before calling this
    function.

  Parameters:
    - drvIndex    - Identifier for the object instance to be opened
    - intent      - Zero or more of the values from the enumeration
                    DRV_IO_INTENT ORed together to indicate the intended use
                    of the driver

  Returns:
    - valid open-instance handle if successful (a number
      identifying both the caller and the module instance).
    - DRV_HANDLE_INVALID if an error occurs

  Example:
    <code>
    DRV_HANDLE  handle;

    handle = DRV_ETHPHY_Open(0, 0);
    if (DRV_HANDLE_INVALID == handle)
    {
        # Unable to open the driver
    }
    </code>

  Remarks:
    The handle returned is valid until the DRV_ETHPHY_Close routine is called.

    This function will NEVER block waiting for hardware.

    The intent parameter is not used. The PHY driver implements a non-blocking
    behavior.
*/

DRV_HANDLE DRV_ETHPHY_Open( const SYS_MODULE_INDEX iModule,
                            const DRV_IO_INTENT    ioIntent  );


// *****************************************************************************
/* Function:
    void DRV_ETHPHY_Close( DRV_HANDLE handle )

  Summary:
    Closes an opened instance of the Ethernet PHY driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function closes an opened instance of the Ethernet PHY driver, invalidating
    the handle.

  Precondition:
    The DRV_ETHPHY_Initialize routine must have been called for the specified
    Ethernet PHY driver instance.

    DRV_ETHPHY_Open must have been called to obtain a valid opened device handle.

  Parameters:
    - handle     - A valid open instance handle, returned from the driver's
                   open routine

  Returns:
    None

  Example:
    <code>
    DRV_HANDLE handle;  # Returned from DRV_ETHPHY_Open

    DRV_ETHPHY_Close(handle);
    </code>

  Remarks:
    - After calling this routine, the handle passed in "handle" must not be used
      with any of the remaining driver routines.  A new handle must be obtained by
      calling DRV_ETHPHY_Open before the caller may use the driver again.
    - Usually there is no need for the driver client to verify that the Close
      operation has completed.
*/

void DRV_ETHPHY_Close( DRV_HANDLE handle );


/***************************************************************************
  Function:
       DRV_ETHPHY_CLIENT_STATUS DRV_ETHPHY_ClientStatus( DRV_HANDLE handle )
    
  Summary:
    Gets the current client-specific status the Ethernet PHY driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function gets the client-specific status of the Ethernet PHY
    driver associated with the given handle.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid opened device
      handle.

  Parameters:
    - handle -  A valid instance handle, returned from the driver's open
                routine
  Returns:
    - DRV_ETHPHY_CLIENT_STATUS value describing the current status of the
      driver.

  Example:
    <code>
    DRV_HANDLE phyHandle;  # Returned from DRV_ETHPHY_Open
    DRV_ETHPHY_CLIENT_STATUS phyClientStatus;
    
    phyClientStatus = DRV_ETHPHY_ClientStatus(phyHandle);
    if(DRV_ETHPHY_CLIENT_STATUS_ERROR >= phyClientStatus)
    {
        # Handle the error
    }
    </code>

  Remarks:
    This function will not block for hardware access and will immediately
    return the current status.                                             

    This function has to be used to check that a driver operation
    has completed.
    It will return DRV_ETHPHY_CLIENT_STATUS_BUSY when an operation is in progress.
    It will return DRV_ETHPHY_CLIENT_STATUS_READY when the operation has completed.

  ***************************************************************************/

DRV_ETHPHY_CLIENT_STATUS DRV_ETHPHY_ClientStatus( DRV_HANDLE handle );


/***************************************************************************
  Function:
       DRV_ETHPHY_RESULT DRV_ETHPHY_ClientOperationResult( DRV_HANDLE handle)
    
  Summary:
    Gets the result of a client operation  initiated by the Ethernet PHY driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    Returns the result of a client operation  initiated by the Ethernet PHY driver.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid opened device
      handle.
    - A driver operation was started and completed

  Parameters:
    - handle -  A valid instance handle, returned from the driver's open
                routine
  Returns:
    - DRV_ETHPHY_RESULT value describing the current operation result:
        DRV_ETHPHY_RES_OK for success; operation has been completed successfully
        DRV_ETHPHY_RES_PENDING operation is in progress
        an DRV_ETHPHY_RESULT error code if the operation failed.

  Example:
    <code>
    </code>

  Remarks:
    This function will not block for hardware access and will immediately
    return the current status.                                             

    This function returns the result of the last driver operation.
    It will return DRV_ETHPHY_RES_PENDING if an operation is still in progress.
    Otherwise a DRV_ETHPHY_RESULT describing the operation outcome.
    
  ***************************************************************************/

DRV_ETHPHY_RESULT DRV_ETHPHY_ClientOperationResult( DRV_HANDLE handle);  

/***************************************************************************
  Function:
       DRV_ETHPHY_RESULT DRV_ETHPHY_ClientOperationAbort( DRV_HANDLE handle)
    
  Summary:
    Aborts a current client operation  initiated by the Ethernet PHY driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    Aborts a current client operation  initiated by the Ethernet PHY driver.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid opened device
      handle.
    - A driver operation was started

  Parameters:
    - handle -  A valid instance handle, returned from the driver's open
                routine
  Returns:
    - DRV_ETHPHY_RESULT value describing the current operation result:
        DRV_ETHPHY_RES_OK for success; operation has been aborted
        an DRV_ETHPHY_RESULT error code if the operation failed.

  Example:
    <code>
    </code>

  Remarks:
    None
  ***************************************************************************/

DRV_ETHPHY_RESULT DRV_ETHPHY_ClientOperationAbort( DRV_HANDLE handle);  


/**************************************************************************
  Function:
       DRV_ETHPHY_RESULT DRV_ETHPHY_PhyAddressGet( DRV_HANDLE handle, int* pPhyAddress);
    
  Summary:
    Returns the PHY address.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the current PHY address
    as set by the DRV_ETHPHY_Setup procedure.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid device
      handle.
    - DRV_ETHPHY_Setup must have been called to properly configure the PHY

  Parameters:
    - handle      -  Client's driver handle (returned from DRV_ETHPHY_Open)
    - pPhyAddress - address to store the PHY address

  Returns:
    DRV_ETHPHY_RES_OK  - operation successful and the PHY address stored at 

    DRV_ETHPHY_RES_HANDLE_ERR  - passed in handle was invalid pPhyAddress

  Example:
    <code>
    
    </code>
  Remarks:
    None.                                                                           
  **************************************************************************/

DRV_ETHPHY_RESULT DRV_ETHPHY_PhyAddressGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, int* pPhyAddress);


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Client & Module Level
// *****************************************************************************
// *****************************************************************************

/**************************************************************************
  Function:
       DRV_ETHPHY_RESULT DRV_ETHPHY_Setup( DRV_HANDLE  handle, DRV_ETHPHY_SETUP* pSetUp, TCPIP_ETH_OPEN_FLAGS* pSetupFlags)
    
  Summary:
    Initializes Ethernet PHY configuration and set up procedure.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function initializes the Ethernet PHY communication. It tries to
    detect the external Ethernet PHY, to read the capabilities and find a
    match with the requested features. Then, it programs the Ethernet PHY
    accordingly.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid device
      handle.

  Parameters:
    - handle -     A valid open instance handle, returned from the driver's
                   open routine
    - pSetUp -     A pointer to an initialization structure containing:
                    - phyAddress: the PHY SMI address
                    - openFlags: the requested open flags
                    - configFlags: Ethernet PHY MII/RMII configuration flags
    - pSetupFlags:  address to store the set up procedure result
                    once the set up procedure is completed successfully
                    the resulting flags will be stored here

  Returns:
    - DRV_ETHPHY_RES_PENDING operation has been scheduled successfully
    - an DRV_ETHPHY_RESULT error code if the set up procedure failed.

  Example:
    <code>
    
    </code>

  Remarks:
    PHY configuration may be a lengthy operation due to active negotiation that the PHY
    has to perform with the link party.
    The DRV_ETHPHY_ClientStatus will repeatedly return DRV_ETHPHY_CLIENT_STATUS_BUSY
    until the set up procedure is complete
    (unless an error detected at which an error code will be returned immediately).

    Use DRV_ETHPHY_ClientStatus() and DRV_ETHPHY_ClientOperationResult() to check
    when the operation was completed and its outcome. 

  **************************************************************************/

DRV_ETHPHY_RESULT DRV_ETHPHY_Setup( DRV_HANDLE  handle, DRV_ETHPHY_SETUP* pSetUp, TCPIP_ETH_OPEN_FLAGS* pSetupFlags);


// *****************************************************************************
/* Function:
    DRV_ETHPHY_RESULT DRV_ETHPHY_RestartNegotiation( DRV_HANDLE hClient )

  Summary:
    Restarts auto-negotiation of the Ethernet PHY link.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function restarts auto-negotiation of the Ethernet PHY link.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid device
      handle.
    - DRV_ETHPHY_Setup must have been called to properly configure the PHY

  Parameters:
    - hClient  - Client's driver handle (returned from DRV_ETHPHY_Open)

  Returns:
    - DRV_ETHPHY_RES_PENDING operation has been scheduled successfully
    - an DRV_ETHPHY_RESULT error code if the procedure failed.

  Example:
    <code>
    </code>

  Remarks:
    Use DRV_ETHPHY_ClientStatus() and DRV_ETHPHY_ClientOperationResult() to check
    when the operation was completed and its outcome. 
*/

DRV_ETHPHY_RESULT DRV_ETHPHY_RestartNegotiation( DRV_HANDLE hClient, DRV_ETHPHY_INTERFACE_INDEX portIndex );


// *****************************************************************************
/* Function:
    DRV_ETHPHY_RESULT DRV_ETHPHY_HWConfigFlagsGet( DRV_HANDLE handle, DRV_ETHPHY_CONFIG_FLAGS* pFlags )

  Summary:
    Returns the current Ethernet PHY hardware MII/RMII and ALTERNATE/DEFAULT
    configuration flags.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the current Ethernet PHY hardware MII/RMII and
    ALTERNATE/DEFAULT configuration flags from the Device Configuration Fuse bits.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid device
      handle.
    - DRV_ETHPHY_Setup must have been called to properly configure the PHY

  Parameters:
    handle  - Client's driver handle (returned from DRV_ETHPHY_Open)
    pFlags  - address to store the hardware configuration

  Returns:
    DRV_ETHPHY_RES_OK  - if the configuration flags successfully stored at pFlags
    DRV_ETHPHY_RESULT error code otherwise

  Example:
    <code>
    </code>

  Remarks:
    None.
*/

DRV_ETHPHY_RESULT DRV_ETHPHY_HWConfigFlagsGet( DRV_HANDLE handle, DRV_ETHPHY_CONFIG_FLAGS* pFlags );


// *****************************************************************************
/* Function:
    DRV_ETHPHY_RESULT DRV_ETHPHY_NegotiationIsComplete( DRV_HANDLE hClient, bool waitComplete )

  Summary:
    Returns the results of a previously initiated Ethernet PHY negotiation.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
   This function returns the results of a previously initiated Ethernet PHY
   negotiation.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid device
      handle.
    - DRV_ETHPHY_Setup must have been called to properly configure the PHY
    - DRV_ETHPHY_RestartNegotiation should have been called.

  Parameters:
    - hClient  - Client's driver handle (returned from DRV_ETHPHY_Open)
    - waitComplete - boolean: wait for negotiation to complete or return immediately

  Returns:
    - DRV_ETHPHY_RES_PENDING operation is ongoing
    - an DRV_ETHPHY_RESULT error code if the procedure failed.


  Example:
    <code>
    </code>

  Remarks:
    Use DRV_ETHPHY_ClientStatus() and DRV_ETHPHY_ClientOperationResult() to check
    when the operation was completed and its outcome. 

    When operation is completed but negotiation has failed,
    DRV_ETHPHY_ClientOperationResult will return:

    - DRV_ETHPHY_RES_NEGOTIATION_INACTIVE if no negotiation in progress
    - DRV_ETHPHY_RES_NEGOTIATION_NOT_STARTED if negotiation not yet started yet
      (means time out if waitComplete was requested)
    - DRV_ETHPHY_RES_NEGOTIATION_ACTIVE if negotiation ongoing
     (means time out if waitComplete was requested).

    See also DRV_ETHPHY_NegotiationResultGet.
*/

DRV_ETHPHY_RESULT DRV_ETHPHY_NegotiationIsComplete( DRV_HANDLE hClient, DRV_ETHPHY_INTERFACE_INDEX portIndex, bool waitComplete );


// *****************************************************************************
/* Function:
    DRV_ETHPHY_RESULT DRV_ETHPHY_NegotiationResultGet( DRV_HANDLE handle, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult)
  
  Summary:
    Returns the result of a completed negotiation.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the PHY negotiation data gathered after a completed negotiation.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid device
      handle.
    - DRV_ETHPHY_Setup must have been called to properly configure the PHY
    - DRV_ETHPHY_RestartNegotiation, and DRV_ETHPHY_NegotiationIsComplete
      should have been called.

  Parameters:
    - handle     - Client's driver handle (returned from DRV_ETHPHY_Open)
    - pNegResult - address to store the negotiation result:
                    - link status after the (completed) negotiation
                    - capability flags
                    - supported pause type

  Returns:
    - DRV_ETHPHY_RES_PENDING operation is ongoing
    - an DRV_ETHPHY_RESULT error code if the procedure failed.
  

  Example:
    <code>
    </code>

  Remarks:
    Use DRV_ETHPHY_ClientStatus() and DRV_ETHPHY_ClientOperationResult() to check
    when the operation was completed and its outcome. 

    When operation is completed but negotiation has failed,
    DRV_ETHPHY_ClientOperationResult will return:

    - DRV_ETHPHY_RES_NEGOTIATION_INACTIVE if no negotiation in progress
    - DRV_ETHPHY_RES_NEGOTIATION_NOT_STARTED if negotiation not yet started yet
      (means time out if waitComplete was requested)
    - DRV_ETHPHY_RES_NEGOTIATION_ACTIVE if negotiation ongoing

    The returned value for the negotiation flags is valid only if the
    negotiation was completed successfully.
*/

DRV_ETHPHY_RESULT DRV_ETHPHY_NegotiationResultGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult);


/*********************************************************************************
  Function:
       DRV_ETHPHY_RESULT DRV_ETHPHY_LinkStatusGet( DRV_HANDLE handle, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh )
    
  Summary:
    Returns the current link status.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the current link status.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid device
      handle.
    - DRV_ETHPHY_Setup must have been called to properly configure the PHY
    
  Parameters:
    - handle -   Client's driver handle (returned from DRV_ETHPHY_Open)
    - pLinkStat - valid address to deposit the operation result
    - refresh -  Boolean flag, true to specify that a double read is needed

  Returns:
    - DRV_ETHPHY_RES_PENDING for ongoing, in progress operation
    - an DRV_ETHPHY_RESULT error code if the link status get procedure failed.

  Example:
    <code>
    
    </code>

  Remarks:
    This function reads the Ethernet PHY to get current link status.
    If refresh is specified then, if the link is down a second read will be
    performed to return the current link status.

    Use DRV_ETHPHY_ClientStatus() and DRV_ETHPHY_ClientOperationResult() to check
    when the operation was completed and its outcome. 

  *********************************************************************************/

DRV_ETHPHY_RESULT DRV_ETHPHY_LinkStatusGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh );


/**************************************************************************
  Function:
       DRV_ETHPHY_RESULT DRV_ETHPHY_Reset( DRV_HANDLE handle, bool waitComplete )
    
  Summary:
    Immediately resets the Ethernet PHY.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function immediately resets the Ethernet PHY, optionally waiting
    for a reset to complete.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid device
      handle.
    - DRV_ETHPHY_Setup must have been called to properly configure the PHY
    
  Parameters:
    - handle       - Client's driver handle (returned from DRV_ETHPHY_Open)
    - waitComplete - Boolean flag, if true the procedure will wait for reset
                     to complete
  Returns:
    - DRV_ETHPHY_RES_PENDING for ongoing, in progress operation 
    - DRV_ETHPHY_RES_OPERATION_ERR - invalid parameter or operation in the current context

  Example:
    <code>
    
    </code>

  Remarks:
    Use DRV_ETHPHY_ClientStatus() and DRV_ETHPHY_ClientOperationResult() to check
    when the operation was completed and its outcome. 

    When operation is completed but failed,
    DRV_ETHPHY_ClientOperationResult will return:
    - DRV_ETHPHY_RES_DTCT_ERR if the PHY failed to respond
  **************************************************************************/

DRV_ETHPHY_RESULT DRV_ETHPHY_Reset( DRV_HANDLE handle, bool waitComplete );

// *****************************************************************************
// *****************************************************************************
// Section: Vendor Interface Routines - Vendor access to DRV_ETHPHY resources
// *****************************************************************************
// *****************************************************************************
/* These functions allow the implementation of vendor specific functionality
 * They are meant for supporting the PHY MII and MDIX Configuration
 * */

// *****************************************************************************
/* Function:
    DRV_ETHPHY_RESULT DRV_ETHPHY_VendorDataGet( DRV_HANDLE handle, uint32_t* pVendorData )

  Summary:
    Returns the current value of the vendor data.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the current value of the vendor data.
    Each DRV_ETHPHY client object maintains data that could be used
    for vendor specific operations.
    This routine allows retrieving of the vendor specific data.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid device
      handle.
    - DRV_ETHPHY_Setup must have been called to properly configure the PHY

  Parameters:
    - handle  - Client's driver handle (returned from DRV_ETHPHY_Open)
    - pVendorData   - address to store a vendor specific 32 bit data

  Returns:
    DRV_ETHPHY_RES_OK  - if the vendor data is stored at the pVendorData address
    
    DRV_ETHPHY_RES_HANDLE_ERR  - handle error


  Example:
    <code>
    </code>

  Remarks:
    The PHY driver will clear the vendor specific data before any call
    to a vendor specific routine.
    Otherwise the PHY driver functions do not touch this value.

    The DRV_ETHPHY_VendorDataSet can be used for writing data into this field.

    Currently only a 32 bit value is supported.

    The function is intended for implementing vendor specific functions,
    like DRV_EXTPHY_MIIConfigure and DRV_EXTPHY_MDIXConfigure,
    that need a way of maintaining their own data and state machine.

*/

DRV_ETHPHY_RESULT DRV_ETHPHY_VendorDataGet( DRV_HANDLE handle, uint32_t* pVendorData );

// *****************************************************************************
/* Function:
    DRV_ETHPHY_RESULT DRV_ETHPHY_VendorDataSet( DRV_HANDLE handle, uint32_t vendorData )

  Summary:
    Returns the current value of the vendor data.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the current value of the vendor data.
    Each DRV_ETHPHY client object maintains data that could be used
    for vendor specific operations.
    This routine allows setting of the vendor specific data.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid device
      handle.
    - DRV_ETHPHY_Setup must have been called to properly configure the PHY

  Parameters:
    - handle        - Client's driver handle (returned from DRV_ETHPHY_Open)
    - vendorData    - vendor specific data to be set

  Returns:
    DRV_ETHPHY_RES_OK  - if the vendor data is stored in the client object
    
    DRV_ETHPHY_RES_HANDLE_ERR  - handle error

  Example:
    <code>
    </code>

  Remarks:
    The PHY driver will clear the vendor specific data before any call
    to a vendor specific routine.
    Otherwise the PHY driver functions do not touch this value.

    The DRV_ETHPHY_VendorDataGet can be used for reading data into this field.

    Currently only a 32 bit value is supported.

    The function is intended for implementing vendor specific functions,
    like DRV_EXTPHY_MIIConfigure and DRV_EXTPHY_MDIXConfigure,
    that need a way of maintaining their own data and state machine.

*/

DRV_ETHPHY_RESULT DRV_ETHPHY_VendorDataSet( DRV_HANDLE handle, uint32_t vendorData );

// *****************************************************************************
/* Function:
    DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIReadStart( DRV_HANDLE handle, uint16_t rIx,  int phyAddress )

  Summary:
    Starts a vendor SMI read transfer.
    Data will be available with DRV_ETHPHY_VendorSMIReadResultGet.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will start a SMI read transfer.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid device
      handle.
    - DRV_ETHPHY_Setup is in progress and configures the PHY
    - The vendor implementation of the DRV_EXTPHY_MIIConfigure/DRV_EXTPHY_MDIXConfigure
      is running and a SMI transfer is needed

  Parameters:
    - handle  - driver handle as passed by the DRV_EXTPHY_MIIConfigure/DRV_EXTPHY_MDIXConfigure call
    - rIx    - vendor transaction SMI register to read
    - phyAddress - PHY address to use for transaction

  Returns:
    DRV_ETHPHY_RES_OK       - the vendor transaction is started
                       DRV_ETHPHY_VendorSMIReadResultGet() needs to be called
                       for the transaction to complete and to retrieve the result

    DRV_ETHPHY_RES_PENDING  - the SMI bus is busy and the call needs to be retried
         
    
    < 0 - some error and the DRV_EXTPHY_MIIConfigure/DRV_EXTPHY_MDIXConfigure
          has to return error to be aborted by the DRV_ETHPHY_Setup

  Example:
    <code>
    </code>

  Remarks:
    The function is intended for implementing vendor SMI transfers
    within DRV_EXTPHY_MIIConfigure and DRV_EXTPHY_MDIXConfigure.

    It has to be called from within the DRV_EXTPHY_MIIConfigure or DRV_EXTPHY_MDIXConfigure
    functions (which are called, in turn, by the DRV_ETHPHY_Setup procedure)
    otherwise the call will fail.

    The DRV_ETHPHY_RES_OK and DRV_ETHPHY_RES_PENDING significance is changed from the general driver API.


*/

DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIReadStart( DRV_HANDLE handle, uint16_t rIx,  int phyAddress );

// *****************************************************************************
/* Function:
    DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIReadResultGet( DRV_HANDLE handle, uint16_t* pSmiRes)

  Summary:
    Reads the result of a previous vendor initiated SMI read transfer
    with DRV_ETHPHY_VendorSMIReadStart.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will return the data of a SMI read transfer.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid device
      handle.
    - DRV_ETHPHY_Setup is in progress and configures the PHY
    - The vendor implementation of the DRV_EXTPHY_MIIConfigure/DRV_EXTPHY_MDIXConfigure
      is running and a SMI transfer is needed
    - DRV_ETHPHY_VendorSMIReadStart should have been called to initiate a transfer

  Parameters:
    - handle  - driver handle as passed by the DRV_EXTPHY_MIIConfigure/DRV_EXTPHY_MDIXConfigure call
    - pSmiRes - address to store the transaction data

  Returns:

    DRV_ETHPHY_RES_OK      - transaction complete and result deposited at pSmiRes.     

    DRV_ETHPHY_RES_PENDING  - if the vendor transaction is still ongoing
                       The call needs to be retried.
    
    < 0 - some error and the DRV_EXTPHY_MIIConfigure/DRV_EXTPHY_MDIXConfigure
          has to return error to be aborted by the DRV_ETHPHY_Setup


  Example:
    <code>
    </code>

  Remarks:
    The function is intended for implementing vendor SMI transfers
    within DRV_EXTPHY_MIIConfigure and DRV_EXTPHY_MDIXConfigure.

    It has to be called from within the DRV_EXTPHY_MIIConfigure or DRV_EXTPHY_MDIXConfigure
    functions (which are called, in turn, by the DRV_ETHPHY_Setup procedure)
    otherwise the call will fail.

    The DRV_ETHPHY_RES_OK and DRV_ETHPHY_RES_PENDING significance is changed from the general driver API.
*/

DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIReadResultGet( DRV_HANDLE handle, uint16_t* pSmiRes);

// *****************************************************************************
/* Function:
    DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIWriteStart( DRV_HANDLE handle, uint16_t rIx,  uint16_t wData, int phyAddress )

  Summary:
    Starts a vendor SMI write transfer.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will start a SMI write transfer.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid device
      handle.
    - DRV_ETHPHY_Setup is in progress and configures the PHY
    - The vendor implementation of the DRV_EXTPHY_MIIConfigure/DRV_EXTPHY_MDIXConfigure
      is running and a SMI transfer is needed

  Parameters:
    - handle  - driver handle as passed by the DRV_EXTPHY_MIIConfigure/DRV_EXTPHY_MDIXConfigure call
    - rIx    - vendor transaction SMI register to write
    - wData  - data to be written
    - phyAddress - PHY address to use for transaction

  Returns:
    DRV_ETHPHY_RES_OK  - if the vendor SMI write transfer is started

    DRV_ETHPHY_RES_PENDING - the SMI bus was busy and the call needs to be retried

    
    < 0 - some error and the DRV_EXTPHY_MIIConfigure/DRV_EXTPHY_MDIXConfigure
          has to return error to be aborted by the DRV_ETHPHY_Setup


  Example:
    <code>
    </code>

  Remarks:
    The function is intended for implementing vendor SMI transfers
    within DRV_EXTPHY_MIIConfigure and DRV_EXTPHY_MDIXConfigure.

    It has to be called from within the DRV_EXTPHY_MIIConfigure or DRV_EXTPHY_MDIXConfigure
    functions (which are called, in turn, by the DRV_ETHPHY_Setup procedure)
    otherwise the call will fail.

    The DRV_ETHPHY_RES_OK and DRV_ETHPHY_RES_PENDING significance is changed from the general driver API.
*/

DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIWriteStart( DRV_HANDLE handle, uint16_t rIx,  uint16_t wData, int phyAddress );

// *****************************************************************************
/* Function:
    DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIWriteWaitComplete( DRV_HANDLE handle, uint16_t rIx,  uint16_t wData, int phyAddress )

  Summary:
    Starts a vendor SMI write transfer. After this function call, check the completion of 
    write operation with DRV_ETHPHY_VendorSMIOperationIsComplete.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will start a SMI write transfer.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid device
      handle.
    - DRV_ETHPHY_Setup is in progress and configures the PHY
    - The vendor implementation of the DRV_EXTPHY_MIIConfigure/DRV_EXTPHY_MDIXConfigure
      is running and a SMI transfer is needed

  Parameters:
    - handle  - driver handle as passed by the DRV_EXTPHY_MIIConfigure/DRV_EXTPHY_MDIXConfigure call
    - rIx    - vendor transaction SMI register to write
    - wData  - data to be written
    - phyAddress - PHY address to use for transaction

  Returns:
    DRV_ETHPHY_RES_OK  - if the vendor SMI write transfer is started

    DRV_ETHPHY_RES_PENDING - the SMI bus was busy and the call needs to be retried

    
    < 0 - some error and the DRV_EXTPHY_MIIConfigure/DRV_EXTPHY_MDIXConfigure
          has to return error to be aborted by the DRV_ETHPHY_Setup


  Example:
    <code>
    </code>

  Remarks:
    The function is intended for implementing vendor SMI transfers
    within DRV_EXTPHY_MIIConfigure and DRV_EXTPHY_MDIXConfigure.

    It has to be called from within the DRV_EXTPHY_MIIConfigure or DRV_EXTPHY_MDIXConfigure
    functions (which are called, in turn, by the DRV_ETHPHY_Setup procedure)
    otherwise the call will fail.

    The DRV_ETHPHY_RES_OK and DRV_ETHPHY_RES_PENDING significance is changed from the general driver API.
*/
DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIWriteWaitComplete( DRV_HANDLE handle, uint16_t rIx,  uint16_t wData, int phyAddress );
// *****************************************************************************
/* Function:
    DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIOperationIsComplete(DRV_HANDLE handle)

  Summary:
    Check the SMI Operation is complete
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function will return the status of SMI transfer.

  Precondition:
    - The DRV_ETHPHY_Initialize routine must have been called.
    - DRV_ETHPHY_Open must have been called to obtain a valid device
      handle.
    - DRV_ETHPHY_Setup is in progress and configures the PHY
    - The vendor implementation of the DRV_EXTPHY_MIIConfigure/DRV_EXTPHY_MDIXConfigure
      is running and a SMI transfer is needed

  Parameters:
    - handle  - driver handle as passed by the DRV_EXTPHY_MIIConfigure/DRV_EXTPHY_MDIXConfigure call

  Returns:

    DRV_ETHPHY_RES_OK      -   transaction complete.   

    DRV_ETHPHY_RES_PENDING  - if the vendor transaction is still ongoing
                              The call needs to be retried.
    DRV_ETHPHY_RES_OPERATION_ERR - error happened during write transaction


  Example:
    <code>
    </code>

  Remarks:
    None
*/

DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIOperationIsComplete(DRV_HANDLE handle);

// *****************************************************************************
/* Ethernet PHY Driver Base Object

  Summary:
    Identifies the base interface of a Ethernet PHY driver.

  Description:
    This data structure identifies the required interface of the Ethernet PHY driver.
    Any dynamic PHY driver has to export this interface.

  Remarks:
    The PHY driver consists of 2 modules:
        - the main/base PHY driver which uses standard IEEE PHY registers
        - the vendor specific functionality
    This object provides the base functionality.
    Every dynamic PHY driver has to expose this basic functionality as part of its interface.

    See above the description of each function that's part of the base PHY driver.
*/

typedef struct DRV_ETHPHY_OBJECT_BASE_TYPE
{
    SYS_MODULE_OBJ           (*phy_Initialize) ( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init );

    void                     (*phy_Reinitialize) ( SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init );

    void                     (*phy_Deinitialize) ( SYS_MODULE_OBJ object );

    SYS_STATUS               (*phy_Status) ( SYS_MODULE_OBJ object );

    void                     (*phy_Tasks)( SYS_MODULE_OBJ object );

    DRV_HANDLE               (*phy_Open)( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT    intent  );

    void                     (*phy_Close)( DRV_HANDLE handle );

    DRV_ETHPHY_CLIENT_STATUS (*phy_ClientStatus)( DRV_HANDLE handle );

    DRV_ETHPHY_RESULT        (*phy_ClientOperationResult)( DRV_HANDLE handle);  

    DRV_ETHPHY_RESULT        (*phy_ClientOperationAbort)( DRV_HANDLE handle);  

    DRV_ETHPHY_RESULT        (*phy_PhyAddressGet)( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, int* pPhyAddress);

    DRV_ETHPHY_RESULT        (*phy_Setup)( DRV_HANDLE  handle, DRV_ETHPHY_SETUP* pSetUp, TCPIP_ETH_OPEN_FLAGS* pSetupFlags);

    DRV_ETHPHY_RESULT        (*phy_RestartNegotiation)( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex );

    DRV_ETHPHY_RESULT        (*phy_HWConfigFlagsGet)( DRV_HANDLE handle, DRV_ETHPHY_CONFIG_FLAGS* pFlags );

    DRV_ETHPHY_RESULT        (*phy_NegotiationIsComplete)( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, bool waitComplete );

    DRV_ETHPHY_RESULT        (*phy_NegotiationResultGet)( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult);

    DRV_ETHPHY_RESULT        (*phy_LinkStatusGet)( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh );

    DRV_ETHPHY_RESULT        (*phy_Reset)( DRV_HANDLE handle, bool waitComplete );

    DRV_ETHPHY_RESULT        (*phy_VendorDataGet)( DRV_HANDLE handle, uint32_t* pVendorData );

    DRV_ETHPHY_RESULT        (*phy_VendorDataSet)( DRV_HANDLE handle, uint32_t vendorData );

    DRV_ETHPHY_RESULT        (*phy_VendorSMIReadStart)( DRV_HANDLE handle, uint16_t rIx,  int phyAddress );

    DRV_ETHPHY_RESULT        (*phy_VendorSMIReadResultGet)( DRV_HANDLE handle, uint16_t* pSmiRes);

    DRV_ETHPHY_RESULT        (*phy_VendorSMIWriteStart)( DRV_HANDLE handle, uint16_t rIx,  uint16_t wData, int phyAddress );
    
    DRV_ETHPHY_RESULT        (*phy_VendorSMIWriteWaitComplete)( DRV_HANDLE handle, uint16_t rIx,  uint16_t wData, int phyAddress );
    
    DRV_ETHPHY_RESULT        (*phy_VendorSMIOperationIsComplete)( DRV_HANDLE handle);
    

}DRV_ETHPHY_OBJECT_BASE;


// *****************************************************************************
/* The supported basic PHY driver (DRV_ETHPHY_OBJECT_BASE).
   This object is implemented by default as part of PHY driver.
   It can be overwritten dynamically when needed.

*/
extern const DRV_ETHPHY_OBJECT_BASE  DRV_ETHPHY_OBJECT_BASE_Default;

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif // #ifndef H_DRV_ETHPHY_H

/*******************************************************************************
 End of File
*/

