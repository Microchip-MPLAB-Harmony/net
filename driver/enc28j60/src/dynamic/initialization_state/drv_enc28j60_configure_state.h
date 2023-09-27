/*******************************************************************************
  ENC28J60 Configure State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_configure_state.h
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2015-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#ifndef _DRV_ENC28J60_CONFIGURE_STATE_H_
#define _DRV_ENC28J60_CONFIGURE_STATE_H_

#include "system_config.h"


struct _DRV_ENC28J60_DriverInfo;

typedef enum
{
    DRV_ENC28J60_CS_SET_ERXST,
    DRV_ENC28J60_CS_WAIT_ERXST,
    DRV_ENC28J60_CS_SET_ERXRDPT,
    DRV_ENC28J60_CS_WAIT_ERXRDPT,
    DRV_ENC28J60_CS_SET_ERXND,
    DRV_ENC28J60_CS_WAIT_ERXND,

    DRV_ENC28J60_CS_SET_ETXST,
    DRV_ENC28J60_CS_WAIT_ETXST,
    DRV_ENC28J60_CS_SET_EWRPT,
    DRV_ENC28J60_CS_WAIT_EWRPT,

    DRV_ENC28J60_CS_SET_MACON1,
    DRV_ENC28J60_CS_WAIT_MACON1,
    DRV_ENC28J60_CS_SET_MACON3,
    DRV_ENC28J60_CS_WAIT_MACON3,
    DRV_ENC28J60_CS_SET_MABBIPG,
    DRV_ENC28J60_CS_WAIT_MABBIPG,
    DRV_ENC28J60_CS_SET_MACON4,
    DRV_ENC28J60_CS_WAIT_MACON4,

    DRV_ENC28J60_CS_SET_MACLCON2,
    DRV_ENC28J60_CS_WAIT_MACLCON2,
    DRV_ENC28J60_CS_SET_MAIPG,
    DRV_ENC28J60_CS_WAIT_MAIPG,
    DRV_ENC28J60_CS_SET_MAMXFL, 
    DRV_ENC28J60_CS_WAIT_MAMXFL,

    DRV_ENC28J60_CS_WRITE_MAADR5,
    DRV_ENC28J60_CS_WAIT_MAADR5,
    DRV_ENC28J60_CS_WRITE_MAADR3,
    DRV_ENC28J60_CS_WAIT_MAADR3,
    DRV_ENC28J60_CS_WRITE_MAADR1,
    DRV_ENC28J60_CS_WAIT_MAADR1,

    DRV_ENC28J60_CS_SET_ECOCON,
    DRV_ENC28J60_CS_WAIT_ECOCON,

    DRV_ENC28J60_CS_SET_PHCON2,
    DRV_ENC28J60_CS_WAIT_PHCON2,
    DRV_ENC28J60_CS_SET_PHLCON,
    DRV_ENC28J60_CS_WAIT_PHLCON,
    DRV_ENC28J60_CS_SET_PHCON1,
    DRV_ENC28J60_CS_WAIT_PHCON1,
    DRV_ENC28J60_CS_SET_PHIE,
    DRV_ENC28J60_CS_WAIT_PHIE,

    DRV_ENC28J60_CS_SET_EIE,
    DRV_ENC28J60_CS_WAIT_EIE,
    
    DRV_ENC28J60_CS_SET_ECON2,
    DRV_ENC28J60_CS_WAIT_ECON2,
    DRV_ENC28J60_CS_SET_ECON1,
    DRV_ENC28J60_CS_WAIT_ECON1,

    DRV_ENC28J60_CS_DONE,

}DRV_ENC28J60_CONFIGURE_STATES;


typedef struct _DRV_ENC28J60_CONFIGURE_STATE_INFO
{
    DRV_ENC28J60_CONFIGURE_STATES state;
    uintptr_t   op;

}DRV_ENC28J60_CONFIGURE_STATE_INFO;

int32_t DRV_ENC28J60_ConfigStateTask(struct _DRV_ENC28J60_DriverInfo * pDrvInst);
int32_t DRV_ENC28J60_ConfigStateEnter(struct _DRV_ENC28J60_DriverInfo * pDrvInst);
int32_t DRV_ENC28J60_ConfigStateExit(struct _DRV_ENC28J60_DriverInfo * pDrvInst);


#endif
