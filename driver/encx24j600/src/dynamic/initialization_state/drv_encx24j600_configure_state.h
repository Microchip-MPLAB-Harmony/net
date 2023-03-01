/*******************************************************************************
  ENCx24J600 Configure State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_configure_state.h
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2014-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#ifndef _DRV_ENCX24J600_CONFIGURE_STATE_H_
#define _DRV_ENCX24J600_CONFIGURE_STATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"


struct _DRV_ENCX24J600_DriverInfo;

typedef enum
{
    DRV_ENCX24J600_CS_SET_EGPRDPT,
    DRV_ENCX24J600_CS_SET_ERXRDPT,
    DRV_ENCX24J600_CS_SET_EUDARDPT,
    DRV_ENCX24J600_CS_SET_EGPWRPT,
    DRV_ENCX24J600_CS_SET_ERXWRPT,
    DRV_ENCX24J600_CS_SET_EUDAWRPT,
    DRV_ENCX24J600_CS_SET_ERXST,
    DRV_ENCX24J600_CS_SET_ERXTAIL,
    DRV_ENCX24J600_CS_SET_PHCON1,
    DRV_ENCX24J600_CS_SET_EUDAST,
    DRV_ENCX24J600_CS_SET_EUDAND,
    DRV_ENCX24J600_CS_SET_ERXFCON,
    DRV_ENCX24J600_CS_READ_MACON2,
    DRV_ENCX24J600_CS_WRITE_MAADR1,
    DRV_ENCX24J600_CS_WRITE_MAADR2,
    DRV_ENCX24J600_CS_WRITE_MAADR3,
    DRV_ENCX24J600_CS_READ_MAADR1,
    DRV_ENCX24J600_CS_READ_MAADR2,
    DRV_ENCX24J600_CS_READ_MAADR3,
    DRV_ENCX24J600_CS_WAIT_FOR_MACON2,
    DRV_ENCX24J600_CS_SET_MACON2,
    DRV_ENCX24J600_CS_SET_PHANA,
    DRV_ENCX24J600_CS_SET_EIE,
    DRV_ENCX24J600_CS_WAIT_FOR_MAADR3,
}DRV_ENCX24J600_CONFIGURE_STATES;

typedef enum
{
    DRV_ENCX24J600_CS_OP_SET_EGPRDPT,
    DRV_ENCX24J600_CS_OP_SET_MACON2 = DRV_ENCX24J600_CS_OP_SET_EGPRDPT,
    DRV_ENCX24J600_CS_OP_SET_ERXRDPT,
    DRV_ENCX24J600_CS_OP_SET_PHANA_1 = DRV_ENCX24J600_CS_OP_SET_ERXRDPT,
    DRV_ENCX24J600_CS_OP_SET_EUDARDPT,
    DRV_ENCX24J600_CS_OP_SET_PHANA_2 = DRV_ENCX24J600_CS_OP_SET_EUDARDPT,
    DRV_ENCX24J600_CS_OP_SET_EGPWRPT,
    DRV_ENCX24J600_CS_OP_SET_EIE = DRV_ENCX24J600_CS_OP_SET_EGPWRPT,
    DRV_ENCX24J600_CS_OP_SET_ERXWRPT,
    DRV_ENCX24J600_CS_OP_SET_EUDAWRPT,
    DRV_ENCX24J600_CS_OP_SET_ERXST,
    DRV_ENCX24J600_CS_OP_SET_ERXTAIL,
    DRV_ENCX24J600_CS_OP_SET_PHCON1,
    DRV_ENCX24J600_CS_OP_SET_EUDAST,
    DRV_ENCX24J600_CS_OP_SET_EUDAND,
    DRV_ENCX24J600_CS_OP_SET_ERXFCON,
    DRV_ENCX24J600_CS_OP_READ_MACON2,
    DRV_ENCX24J600_CS_OP_WRITE_MAADR1,
    DRV_ENCX24J600_CS_OP_WRITE_MAADR2,
    DRV_ENCX24J600_CS_OP_WRITE_MAADR3,
    DRV_ENCX24J600_CS_OP_READ_MAADR1,
    DRV_ENCX24J600_CS_OP_READ_MAADR2,
    DRV_ENCX24J600_CS_OP_READ_MAADR3,

}DRV_ENCX24J600_CONFIG_STATE_OPS;

typedef struct _DRV_ENCX24J600_CONFIGURE_STATE_INFO
{
    DRV_ENCX24J600_CONFIGURE_STATES state;
    uintptr_t waitForMacon2Op;
    uintptr_t waitForMaddr1Op;
    uintptr_t waitForMaddr2Op;
    uintptr_t waitForMaddr3Op;
}DRV_ENCX24J600_CONFIGURE_STATE_INFO;

int32_t DRV_ENCX24J600_ConfigStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);
int32_t DRV_ENCX24J600_ConfigStateEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);
int32_t DRV_ENCX24J600_ConfigStateExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);



#ifdef __cplusplus
}
#endif


#endif
