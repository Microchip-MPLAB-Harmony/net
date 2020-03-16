/*******************************************************************************
  ENCx24J600 Initialization State Machine
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_initialization_state.h
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2014-2018 Microchip Technology Inc. and its subsidiaries.

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

// DOM-IGNORE-END
#ifndef _DRV_ENCX24J600_INITIALIZATION_STATE_H_
#define _DRV_ENCX24J600_INITIALIZATION_STATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "system_config.h"
#include "drv_encx24j600_detect_state.h"
#include "drv_encx24j600_reset_state.h"
#include "drv_encx24j600_configure_state.h"

struct _DRV_ENCX24J600_DriverInfo;

typedef enum
{
    DRV_ENCX24J600_IS_OPEN_BUS,
    DRV_ENCX24J600_IS_DETECT,
    DRV_ENCX24J600_IS_RESET,
    DRV_ENCX24J600_IS_CONFIG,
}DRV_ENCX24J600_INITIALIZATION_STATES;

typedef struct _DRV_ENCX24J600_INITIALIZATION_STATE_INFO
{
    DRV_ENCX24J600_INITIALIZATION_STATES state;
    DRV_ENCX24J600_DETECT_STATE_INFO detectStateInfo;
    DRV_ENCX24J600_RESET_STATE_INFO resetStateInfo;
    DRV_ENCX24J600_CONFIGURE_STATE_INFO configStateInfo;
}DRV_ENCX24J600_INITIALIZATION_STATE_INFO;

int32_t DRV_ENCX24J600_InitStateTask(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);
int32_t DRV_ENCX24J600_InitStateEnter(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);
int32_t DRV_ENCX24J600_InitStateExit(struct _DRV_ENCX24J600_DriverInfo * pDrvInst);



#ifdef __cplusplus
}
#endif


#endif
