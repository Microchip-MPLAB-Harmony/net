/**
 * \brief Header file for SAME/SAME70 RSWDT
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
#ifndef SAME_SAME70_RSWDT_MODULE_H
#define SAME_SAME70_RSWDT_MODULE_H

/** \addtogroup SAME_SAME70 Reinforced Safety Watchdog Timer
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_RSWDT                                 */
/* ========================================================================== */

/* -------- RSWDT_CR : (RSWDT Offset: 0x00) ( /W 32) Control Register -------- */
#define RSWDT_CR_WDRSTT_Pos                   (0U)                                           /**< (RSWDT_CR) Watchdog Restart Position */
#define RSWDT_CR_WDRSTT_Msk                   (0x1U << RSWDT_CR_WDRSTT_Pos)                  /**< (RSWDT_CR) Watchdog Restart Mask */
#define RSWDT_CR_WDRSTT(value)                (RSWDT_CR_WDRSTT_Msk & ((value) << RSWDT_CR_WDRSTT_Pos))
#define RSWDT_CR_KEY_Pos                      (24U)                                          /**< (RSWDT_CR) Password Position */
#define RSWDT_CR_KEY_Msk                      (0xFFU << RSWDT_CR_KEY_Pos)                    /**< (RSWDT_CR) Password Mask */
#define RSWDT_CR_KEY(value)                   (RSWDT_CR_KEY_Msk & ((value) << RSWDT_CR_KEY_Pos))
#define   RSWDT_CR_KEY_PASSWD_Val             (196U)                                         /**< (RSWDT_CR) Writing any other value in this field aborts the write operation.  */
#define RSWDT_CR_KEY_PASSWD                   (RSWDT_CR_KEY_PASSWD_Val << RSWDT_CR_KEY_Pos)  /**< (RSWDT_CR) Writing any other value in this field aborts the write operation. Position  */
#define RSWDT_CR_Msk                          (0xFF000001UL)                                 /**< (RSWDT_CR) Register Mask  */

/* -------- RSWDT_MR : (RSWDT Offset: 0x04) (R/W 32) Mode Register -------- */
#define RSWDT_MR_WDV_Pos                      (0U)                                           /**< (RSWDT_MR) Watchdog Counter Value Position */
#define RSWDT_MR_WDV_Msk                      (0xFFFU << RSWDT_MR_WDV_Pos)                   /**< (RSWDT_MR) Watchdog Counter Value Mask */
#define RSWDT_MR_WDV(value)                   (RSWDT_MR_WDV_Msk & ((value) << RSWDT_MR_WDV_Pos))
#define RSWDT_MR_WDFIEN_Pos                   (12U)                                          /**< (RSWDT_MR) Watchdog Fault Interrupt Enable Position */
#define RSWDT_MR_WDFIEN_Msk                   (0x1U << RSWDT_MR_WDFIEN_Pos)                  /**< (RSWDT_MR) Watchdog Fault Interrupt Enable Mask */
#define RSWDT_MR_WDFIEN(value)                (RSWDT_MR_WDFIEN_Msk & ((value) << RSWDT_MR_WDFIEN_Pos))
#define RSWDT_MR_WDRSTEN_Pos                  (13U)                                          /**< (RSWDT_MR) Watchdog Reset Enable Position */
#define RSWDT_MR_WDRSTEN_Msk                  (0x1U << RSWDT_MR_WDRSTEN_Pos)                 /**< (RSWDT_MR) Watchdog Reset Enable Mask */
#define RSWDT_MR_WDRSTEN(value)               (RSWDT_MR_WDRSTEN_Msk & ((value) << RSWDT_MR_WDRSTEN_Pos))
#define RSWDT_MR_WDDIS_Pos                    (15U)                                          /**< (RSWDT_MR) Watchdog Disable Position */
#define RSWDT_MR_WDDIS_Msk                    (0x1U << RSWDT_MR_WDDIS_Pos)                   /**< (RSWDT_MR) Watchdog Disable Mask */
#define RSWDT_MR_WDDIS(value)                 (RSWDT_MR_WDDIS_Msk & ((value) << RSWDT_MR_WDDIS_Pos))
#define RSWDT_MR_ALLONES_Pos                  (16U)                                          /**< (RSWDT_MR) Must Always Be Written with 0xFFF Position */
#define RSWDT_MR_ALLONES_Msk                  (0xFFFU << RSWDT_MR_ALLONES_Pos)               /**< (RSWDT_MR) Must Always Be Written with 0xFFF Mask */
#define RSWDT_MR_ALLONES(value)               (RSWDT_MR_ALLONES_Msk & ((value) << RSWDT_MR_ALLONES_Pos))
#define RSWDT_MR_WDDBGHLT_Pos                 (28U)                                          /**< (RSWDT_MR) Watchdog Debug Halt Position */
#define RSWDT_MR_WDDBGHLT_Msk                 (0x1U << RSWDT_MR_WDDBGHLT_Pos)                /**< (RSWDT_MR) Watchdog Debug Halt Mask */
#define RSWDT_MR_WDDBGHLT(value)              (RSWDT_MR_WDDBGHLT_Msk & ((value) << RSWDT_MR_WDDBGHLT_Pos))
#define RSWDT_MR_WDIDLEHLT_Pos                (29U)                                          /**< (RSWDT_MR) Watchdog Idle Halt Position */
#define RSWDT_MR_WDIDLEHLT_Msk                (0x1U << RSWDT_MR_WDIDLEHLT_Pos)               /**< (RSWDT_MR) Watchdog Idle Halt Mask */
#define RSWDT_MR_WDIDLEHLT(value)             (RSWDT_MR_WDIDLEHLT_Msk & ((value) << RSWDT_MR_WDIDLEHLT_Pos))
#define RSWDT_MR_Msk                          (0x3FFFBFFFUL)                                 /**< (RSWDT_MR) Register Mask  */

/* -------- RSWDT_SR : (RSWDT Offset: 0x08) (R/  32) Status Register -------- */
#define RSWDT_SR_WDUNF_Pos                    (0U)                                           /**< (RSWDT_SR) Watchdog Underflow Position */
#define RSWDT_SR_WDUNF_Msk                    (0x1U << RSWDT_SR_WDUNF_Pos)                   /**< (RSWDT_SR) Watchdog Underflow Mask */
#define RSWDT_SR_WDUNF(value)                 (RSWDT_SR_WDUNF_Msk & ((value) << RSWDT_SR_WDUNF_Pos))
#define RSWDT_SR_Msk                          (0x00000001UL)                                 /**< (RSWDT_SR) Register Mask  */

/** \brief RSWDT register offsets definitions */
#define RSWDT_CR_OFFSET                (0x00)         /**< (RSWDT_CR) Control Register Offset */
#define RSWDT_MR_OFFSET                (0x04)         /**< (RSWDT_MR) Mode Register Offset */
#define RSWDT_SR_OFFSET                (0x08)         /**< (RSWDT_SR) Status Register Offset */

/** \brief RSWDT register API structure */
typedef struct
{
  __O   uint32_t                       RSWDT_CR;        /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       RSWDT_MR;        /**< Offset: 0x04 (R/W  32) Mode Register */
  __I   uint32_t                       RSWDT_SR;        /**< Offset: 0x08 (R/   32) Status Register */
} rswdt_registers_t;
/** @}  end of Reinforced Safety Watchdog Timer */

#endif /* SAME_SAME70_RSWDT_MODULE_H */

