/**
 * \brief Header file for SAME/SAME70 GPBR
 *
 * Copyright (c) 2017-2018 Microchip Technology Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/* file generated from device description version 2018-09-19T14:04:45Z */
#ifndef SAME_SAME70_GPBR_MODULE_H
#define SAME_SAME70_GPBR_MODULE_H

/** \addtogroup SAME_SAME70 General Purpose Backup Registers
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_GPBR                                  */
/* ========================================================================== */

/* -------- SYS_GPBR : (GPBR Offset: 0x00) (R/W 32) General Purpose Backup Register 0 -------- */
#define SYS_GPBR_GPBR_VALUE_Pos               (0U)                                           /**< (SYS_GPBR) Value of GPBR x Position */
#define SYS_GPBR_GPBR_VALUE_Msk               (0xFFFFFFFFU << SYS_GPBR_GPBR_VALUE_Pos)       /**< (SYS_GPBR) Value of GPBR x Mask */
#define SYS_GPBR_GPBR_VALUE(value)            (SYS_GPBR_GPBR_VALUE_Msk & ((value) << SYS_GPBR_GPBR_VALUE_Pos))
#define SYS_GPBR_Msk                          (0xFFFFFFFFUL)                                 /**< (SYS_GPBR) Register Mask  */

/** \brief GPBR register offsets definitions */
#define SYS_GPBR_OFFSET                (0x00)         /**< (SYS_GPBR) General Purpose Backup Register 0 Offset */

/** \brief GPBR register API structure */
typedef struct
{
  __IO  uint32_t                       SYS_GPBR[8];     /**< Offset: 0x00 (R/W  32) General Purpose Backup Register 0 */
} gpbr_registers_t;
/** @}  end of General Purpose Backup Registers */

#endif /* SAME_SAME70_GPBR_MODULE_H */

