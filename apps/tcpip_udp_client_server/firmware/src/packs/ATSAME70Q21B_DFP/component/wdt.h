/**
 * \brief Header file for SAME/SAME70 WDT
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
#ifndef SAME_SAME70_WDT_MODULE_H
#define SAME_SAME70_WDT_MODULE_H

/** \addtogroup SAME_SAME70 Watchdog Timer
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_WDT                                   */
/* ========================================================================== */

/* -------- WDT_CR : (WDT Offset: 0x00) ( /W 32) Control Register -------- */
#define WDT_CR_WDRSTT_Pos                     (0U)                                           /**< (WDT_CR) Watchdog Restart Position */
#define WDT_CR_WDRSTT_Msk                     (0x1U << WDT_CR_WDRSTT_Pos)                    /**< (WDT_CR) Watchdog Restart Mask */
#define WDT_CR_WDRSTT(value)                  (WDT_CR_WDRSTT_Msk & ((value) << WDT_CR_WDRSTT_Pos))
#define WDT_CR_KEY_Pos                        (24U)                                          /**< (WDT_CR) Password Position */
#define WDT_CR_KEY_Msk                        (0xFFU << WDT_CR_KEY_Pos)                      /**< (WDT_CR) Password Mask */
#define WDT_CR_KEY(value)                     (WDT_CR_KEY_Msk & ((value) << WDT_CR_KEY_Pos))
#define   WDT_CR_KEY_PASSWD_Val               (165U)                                         /**< (WDT_CR) Writing any other value in this field aborts the write operation.  */
#define WDT_CR_KEY_PASSWD                     (WDT_CR_KEY_PASSWD_Val << WDT_CR_KEY_Pos)      /**< (WDT_CR) Writing any other value in this field aborts the write operation. Position  */
#define WDT_CR_Msk                            (0xFF000001UL)                                 /**< (WDT_CR) Register Mask  */

/* -------- WDT_MR : (WDT Offset: 0x04) (R/W 32) Mode Register -------- */
#define WDT_MR_WDV_Pos                        (0U)                                           /**< (WDT_MR) Watchdog Counter Value Position */
#define WDT_MR_WDV_Msk                        (0xFFFU << WDT_MR_WDV_Pos)                     /**< (WDT_MR) Watchdog Counter Value Mask */
#define WDT_MR_WDV(value)                     (WDT_MR_WDV_Msk & ((value) << WDT_MR_WDV_Pos))
#define WDT_MR_WDFIEN_Pos                     (12U)                                          /**< (WDT_MR) Watchdog Fault Interrupt Enable Position */
#define WDT_MR_WDFIEN_Msk                     (0x1U << WDT_MR_WDFIEN_Pos)                    /**< (WDT_MR) Watchdog Fault Interrupt Enable Mask */
#define WDT_MR_WDFIEN(value)                  (WDT_MR_WDFIEN_Msk & ((value) << WDT_MR_WDFIEN_Pos))
#define WDT_MR_WDRSTEN_Pos                    (13U)                                          /**< (WDT_MR) Watchdog Reset Enable Position */
#define WDT_MR_WDRSTEN_Msk                    (0x1U << WDT_MR_WDRSTEN_Pos)                   /**< (WDT_MR) Watchdog Reset Enable Mask */
#define WDT_MR_WDRSTEN(value)                 (WDT_MR_WDRSTEN_Msk & ((value) << WDT_MR_WDRSTEN_Pos))
#define WDT_MR_WDDIS_Pos                      (15U)                                          /**< (WDT_MR) Watchdog Disable Position */
#define WDT_MR_WDDIS_Msk                      (0x1U << WDT_MR_WDDIS_Pos)                     /**< (WDT_MR) Watchdog Disable Mask */
#define WDT_MR_WDDIS(value)                   (WDT_MR_WDDIS_Msk & ((value) << WDT_MR_WDDIS_Pos))
#define WDT_MR_WDD_Pos                        (16U)                                          /**< (WDT_MR) Watchdog Delta Value Position */
#define WDT_MR_WDD_Msk                        (0xFFFU << WDT_MR_WDD_Pos)                     /**< (WDT_MR) Watchdog Delta Value Mask */
#define WDT_MR_WDD(value)                     (WDT_MR_WDD_Msk & ((value) << WDT_MR_WDD_Pos))
#define WDT_MR_WDDBGHLT_Pos                   (28U)                                          /**< (WDT_MR) Watchdog Debug Halt Position */
#define WDT_MR_WDDBGHLT_Msk                   (0x1U << WDT_MR_WDDBGHLT_Pos)                  /**< (WDT_MR) Watchdog Debug Halt Mask */
#define WDT_MR_WDDBGHLT(value)                (WDT_MR_WDDBGHLT_Msk & ((value) << WDT_MR_WDDBGHLT_Pos))
#define WDT_MR_WDIDLEHLT_Pos                  (29U)                                          /**< (WDT_MR) Watchdog Idle Halt Position */
#define WDT_MR_WDIDLEHLT_Msk                  (0x1U << WDT_MR_WDIDLEHLT_Pos)                 /**< (WDT_MR) Watchdog Idle Halt Mask */
#define WDT_MR_WDIDLEHLT(value)               (WDT_MR_WDIDLEHLT_Msk & ((value) << WDT_MR_WDIDLEHLT_Pos))
#define WDT_MR_Msk                            (0x3FFFBFFFUL)                                 /**< (WDT_MR) Register Mask  */

/* -------- WDT_SR : (WDT Offset: 0x08) (R/  32) Status Register -------- */
#define WDT_SR_WDUNF_Pos                      (0U)                                           /**< (WDT_SR) Watchdog Underflow (cleared on read) Position */
#define WDT_SR_WDUNF_Msk                      (0x1U << WDT_SR_WDUNF_Pos)                     /**< (WDT_SR) Watchdog Underflow (cleared on read) Mask */
#define WDT_SR_WDUNF(value)                   (WDT_SR_WDUNF_Msk & ((value) << WDT_SR_WDUNF_Pos))
#define WDT_SR_WDERR_Pos                      (1U)                                           /**< (WDT_SR) Watchdog Error (cleared on read) Position */
#define WDT_SR_WDERR_Msk                      (0x1U << WDT_SR_WDERR_Pos)                     /**< (WDT_SR) Watchdog Error (cleared on read) Mask */
#define WDT_SR_WDERR(value)                   (WDT_SR_WDERR_Msk & ((value) << WDT_SR_WDERR_Pos))
#define WDT_SR_Msk                            (0x00000003UL)                                 /**< (WDT_SR) Register Mask  */

/** \brief WDT register offsets definitions */
#define WDT_CR_OFFSET                  (0x00)         /**< (WDT_CR) Control Register Offset */
#define WDT_MR_OFFSET                  (0x04)         /**< (WDT_MR) Mode Register Offset */
#define WDT_SR_OFFSET                  (0x08)         /**< (WDT_SR) Status Register Offset */

/** \brief WDT register API structure */
typedef struct
{
  __O   uint32_t                       WDT_CR;          /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       WDT_MR;          /**< Offset: 0x04 (R/W  32) Mode Register */
  __I   uint32_t                       WDT_SR;          /**< Offset: 0x08 (R/   32) Status Register */
} wdt_registers_t;
/** @}  end of Watchdog Timer */

#endif /* SAME_SAME70_WDT_MODULE_H */

