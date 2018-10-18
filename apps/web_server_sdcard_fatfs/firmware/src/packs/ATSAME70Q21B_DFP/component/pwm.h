/**
 * \brief Header file for SAME/SAME70 PWM
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
#ifndef SAME_SAME70_PWM_MODULE_H
#define SAME_SAME70_PWM_MODULE_H

/** \addtogroup SAME_SAME70 Pulse Width Modulation Controller
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_PWM                                   */
/* ========================================================================== */

/* -------- PWM_CLK : (PWM Offset: 0x00) (R/W 32) PWM Clock Register -------- */
#define PWM_CLK_DIVA_Pos                      (0U)                                           /**< (PWM_CLK) CLKA Divide Factor Position */
#define PWM_CLK_DIVA_Msk                      (0xFFU << PWM_CLK_DIVA_Pos)                    /**< (PWM_CLK) CLKA Divide Factor Mask */
#define PWM_CLK_DIVA(value)                   (PWM_CLK_DIVA_Msk & ((value) << PWM_CLK_DIVA_Pos))
#define   PWM_CLK_DIVA_CLKA_POFF_Val          (0U)                                           /**< (PWM_CLK) CLKA clock is turned off  */
#define   PWM_CLK_DIVA_PREA_Val               (1U)                                           /**< (PWM_CLK) CLKA clock is clock selected by PREA  */
#define PWM_CLK_DIVA_CLKA_POFF                (PWM_CLK_DIVA_CLKA_POFF_Val << PWM_CLK_DIVA_Pos) /**< (PWM_CLK) CLKA clock is turned off Position  */
#define PWM_CLK_DIVA_PREA                     (PWM_CLK_DIVA_PREA_Val << PWM_CLK_DIVA_Pos)    /**< (PWM_CLK) CLKA clock is clock selected by PREA Position  */
#define PWM_CLK_PREA_Pos                      (8U)                                           /**< (PWM_CLK) CLKA Source Clock Selection Position */
#define PWM_CLK_PREA_Msk                      (0xFU << PWM_CLK_PREA_Pos)                     /**< (PWM_CLK) CLKA Source Clock Selection Mask */
#define PWM_CLK_PREA(value)                   (PWM_CLK_PREA_Msk & ((value) << PWM_CLK_PREA_Pos))
#define   PWM_CLK_PREA_CLK_Val                (0U)                                           /**< (PWM_CLK) Peripheral clock  */
#define   PWM_CLK_PREA_CLK_DIV2_Val           (1U)                                           /**< (PWM_CLK) Peripheral clock/2  */
#define   PWM_CLK_PREA_CLK_DIV4_Val           (2U)                                           /**< (PWM_CLK) Peripheral clock/4  */
#define   PWM_CLK_PREA_CLK_DIV8_Val           (3U)                                           /**< (PWM_CLK) Peripheral clock/8  */
#define   PWM_CLK_PREA_CLK_DIV16_Val          (4U)                                           /**< (PWM_CLK) Peripheral clock/16  */
#define   PWM_CLK_PREA_CLK_DIV32_Val          (5U)                                           /**< (PWM_CLK) Peripheral clock/32  */
#define   PWM_CLK_PREA_CLK_DIV64_Val          (6U)                                           /**< (PWM_CLK) Peripheral clock/64  */
#define   PWM_CLK_PREA_CLK_DIV128_Val         (7U)                                           /**< (PWM_CLK) Peripheral clock/128  */
#define   PWM_CLK_PREA_CLK_DIV256_Val         (8U)                                           /**< (PWM_CLK) Peripheral clock/256  */
#define   PWM_CLK_PREA_CLK_DIV512_Val         (9U)                                           /**< (PWM_CLK) Peripheral clock/512  */
#define   PWM_CLK_PREA_CLK_DIV1024_Val        (10U)                                          /**< (PWM_CLK) Peripheral clock/1024  */
#define PWM_CLK_PREA_CLK                      (PWM_CLK_PREA_CLK_Val << PWM_CLK_PREA_Pos)     /**< (PWM_CLK) Peripheral clock Position  */
#define PWM_CLK_PREA_CLK_DIV2                 (PWM_CLK_PREA_CLK_DIV2_Val << PWM_CLK_PREA_Pos) /**< (PWM_CLK) Peripheral clock/2 Position  */
#define PWM_CLK_PREA_CLK_DIV4                 (PWM_CLK_PREA_CLK_DIV4_Val << PWM_CLK_PREA_Pos) /**< (PWM_CLK) Peripheral clock/4 Position  */
#define PWM_CLK_PREA_CLK_DIV8                 (PWM_CLK_PREA_CLK_DIV8_Val << PWM_CLK_PREA_Pos) /**< (PWM_CLK) Peripheral clock/8 Position  */
#define PWM_CLK_PREA_CLK_DIV16                (PWM_CLK_PREA_CLK_DIV16_Val << PWM_CLK_PREA_Pos) /**< (PWM_CLK) Peripheral clock/16 Position  */
#define PWM_CLK_PREA_CLK_DIV32                (PWM_CLK_PREA_CLK_DIV32_Val << PWM_CLK_PREA_Pos) /**< (PWM_CLK) Peripheral clock/32 Position  */
#define PWM_CLK_PREA_CLK_DIV64                (PWM_CLK_PREA_CLK_DIV64_Val << PWM_CLK_PREA_Pos) /**< (PWM_CLK) Peripheral clock/64 Position  */
#define PWM_CLK_PREA_CLK_DIV128               (PWM_CLK_PREA_CLK_DIV128_Val << PWM_CLK_PREA_Pos) /**< (PWM_CLK) Peripheral clock/128 Position  */
#define PWM_CLK_PREA_CLK_DIV256               (PWM_CLK_PREA_CLK_DIV256_Val << PWM_CLK_PREA_Pos) /**< (PWM_CLK) Peripheral clock/256 Position  */
#define PWM_CLK_PREA_CLK_DIV512               (PWM_CLK_PREA_CLK_DIV512_Val << PWM_CLK_PREA_Pos) /**< (PWM_CLK) Peripheral clock/512 Position  */
#define PWM_CLK_PREA_CLK_DIV1024              (PWM_CLK_PREA_CLK_DIV1024_Val << PWM_CLK_PREA_Pos) /**< (PWM_CLK) Peripheral clock/1024 Position  */
#define PWM_CLK_DIVB_Pos                      (16U)                                          /**< (PWM_CLK) CLKB Divide Factor Position */
#define PWM_CLK_DIVB_Msk                      (0xFFU << PWM_CLK_DIVB_Pos)                    /**< (PWM_CLK) CLKB Divide Factor Mask */
#define PWM_CLK_DIVB(value)                   (PWM_CLK_DIVB_Msk & ((value) << PWM_CLK_DIVB_Pos))
#define   PWM_CLK_DIVB_CLKB_POFF_Val          (0U)                                           /**< (PWM_CLK) CLKB clock is turned off  */
#define   PWM_CLK_DIVB_PREB_Val               (1U)                                           /**< (PWM_CLK) CLKB clock is clock selected by PREB  */
#define PWM_CLK_DIVB_CLKB_POFF                (PWM_CLK_DIVB_CLKB_POFF_Val << PWM_CLK_DIVB_Pos) /**< (PWM_CLK) CLKB clock is turned off Position  */
#define PWM_CLK_DIVB_PREB                     (PWM_CLK_DIVB_PREB_Val << PWM_CLK_DIVB_Pos)    /**< (PWM_CLK) CLKB clock is clock selected by PREB Position  */
#define PWM_CLK_PREB_Pos                      (24U)                                          /**< (PWM_CLK) CLKB Source Clock Selection Position */
#define PWM_CLK_PREB_Msk                      (0xFU << PWM_CLK_PREB_Pos)                     /**< (PWM_CLK) CLKB Source Clock Selection Mask */
#define PWM_CLK_PREB(value)                   (PWM_CLK_PREB_Msk & ((value) << PWM_CLK_PREB_Pos))
#define   PWM_CLK_PREB_CLK_Val                (0U)                                           /**< (PWM_CLK) Peripheral clock  */
#define   PWM_CLK_PREB_CLK_DIV2_Val           (1U)                                           /**< (PWM_CLK) Peripheral clock/2  */
#define   PWM_CLK_PREB_CLK_DIV4_Val           (2U)                                           /**< (PWM_CLK) Peripheral clock/4  */
#define   PWM_CLK_PREB_CLK_DIV8_Val           (3U)                                           /**< (PWM_CLK) Peripheral clock/8  */
#define   PWM_CLK_PREB_CLK_DIV16_Val          (4U)                                           /**< (PWM_CLK) Peripheral clock/16  */
#define   PWM_CLK_PREB_CLK_DIV32_Val          (5U)                                           /**< (PWM_CLK) Peripheral clock/32  */
#define   PWM_CLK_PREB_CLK_DIV64_Val          (6U)                                           /**< (PWM_CLK) Peripheral clock/64  */
#define   PWM_CLK_PREB_CLK_DIV128_Val         (7U)                                           /**< (PWM_CLK) Peripheral clock/128  */
#define   PWM_CLK_PREB_CLK_DIV256_Val         (8U)                                           /**< (PWM_CLK) Peripheral clock/256  */
#define   PWM_CLK_PREB_CLK_DIV512_Val         (9U)                                           /**< (PWM_CLK) Peripheral clock/512  */
#define   PWM_CLK_PREB_CLK_DIV1024_Val        (10U)                                          /**< (PWM_CLK) Peripheral clock/1024  */
#define PWM_CLK_PREB_CLK                      (PWM_CLK_PREB_CLK_Val << PWM_CLK_PREB_Pos)     /**< (PWM_CLK) Peripheral clock Position  */
#define PWM_CLK_PREB_CLK_DIV2                 (PWM_CLK_PREB_CLK_DIV2_Val << PWM_CLK_PREB_Pos) /**< (PWM_CLK) Peripheral clock/2 Position  */
#define PWM_CLK_PREB_CLK_DIV4                 (PWM_CLK_PREB_CLK_DIV4_Val << PWM_CLK_PREB_Pos) /**< (PWM_CLK) Peripheral clock/4 Position  */
#define PWM_CLK_PREB_CLK_DIV8                 (PWM_CLK_PREB_CLK_DIV8_Val << PWM_CLK_PREB_Pos) /**< (PWM_CLK) Peripheral clock/8 Position  */
#define PWM_CLK_PREB_CLK_DIV16                (PWM_CLK_PREB_CLK_DIV16_Val << PWM_CLK_PREB_Pos) /**< (PWM_CLK) Peripheral clock/16 Position  */
#define PWM_CLK_PREB_CLK_DIV32                (PWM_CLK_PREB_CLK_DIV32_Val << PWM_CLK_PREB_Pos) /**< (PWM_CLK) Peripheral clock/32 Position  */
#define PWM_CLK_PREB_CLK_DIV64                (PWM_CLK_PREB_CLK_DIV64_Val << PWM_CLK_PREB_Pos) /**< (PWM_CLK) Peripheral clock/64 Position  */
#define PWM_CLK_PREB_CLK_DIV128               (PWM_CLK_PREB_CLK_DIV128_Val << PWM_CLK_PREB_Pos) /**< (PWM_CLK) Peripheral clock/128 Position  */
#define PWM_CLK_PREB_CLK_DIV256               (PWM_CLK_PREB_CLK_DIV256_Val << PWM_CLK_PREB_Pos) /**< (PWM_CLK) Peripheral clock/256 Position  */
#define PWM_CLK_PREB_CLK_DIV512               (PWM_CLK_PREB_CLK_DIV512_Val << PWM_CLK_PREB_Pos) /**< (PWM_CLK) Peripheral clock/512 Position  */
#define PWM_CLK_PREB_CLK_DIV1024              (PWM_CLK_PREB_CLK_DIV1024_Val << PWM_CLK_PREB_Pos) /**< (PWM_CLK) Peripheral clock/1024 Position  */
#define PWM_CLK_Msk                           (0x0FFF0FFFUL)                                 /**< (PWM_CLK) Register Mask  */

/* -------- PWM_ENA : (PWM Offset: 0x04) ( /W 32) PWM Enable Register -------- */
#define PWM_ENA_CHID0_Pos                     (0U)                                           /**< (PWM_ENA) Channel ID Position */
#define PWM_ENA_CHID0_Msk                     (0x1U << PWM_ENA_CHID0_Pos)                    /**< (PWM_ENA) Channel ID Mask */
#define PWM_ENA_CHID0(value)                  (PWM_ENA_CHID0_Msk & ((value) << PWM_ENA_CHID0_Pos))
#define PWM_ENA_CHID1_Pos                     (1U)                                           /**< (PWM_ENA) Channel ID Position */
#define PWM_ENA_CHID1_Msk                     (0x1U << PWM_ENA_CHID1_Pos)                    /**< (PWM_ENA) Channel ID Mask */
#define PWM_ENA_CHID1(value)                  (PWM_ENA_CHID1_Msk & ((value) << PWM_ENA_CHID1_Pos))
#define PWM_ENA_CHID2_Pos                     (2U)                                           /**< (PWM_ENA) Channel ID Position */
#define PWM_ENA_CHID2_Msk                     (0x1U << PWM_ENA_CHID2_Pos)                    /**< (PWM_ENA) Channel ID Mask */
#define PWM_ENA_CHID2(value)                  (PWM_ENA_CHID2_Msk & ((value) << PWM_ENA_CHID2_Pos))
#define PWM_ENA_CHID3_Pos                     (3U)                                           /**< (PWM_ENA) Channel ID Position */
#define PWM_ENA_CHID3_Msk                     (0x1U << PWM_ENA_CHID3_Pos)                    /**< (PWM_ENA) Channel ID Mask */
#define PWM_ENA_CHID3(value)                  (PWM_ENA_CHID3_Msk & ((value) << PWM_ENA_CHID3_Pos))
#define PWM_ENA_Msk                           (0x0000000FUL)                                 /**< (PWM_ENA) Register Mask  */

/* -------- PWM_DIS : (PWM Offset: 0x08) ( /W 32) PWM Disable Register -------- */
#define PWM_DIS_CHID0_Pos                     (0U)                                           /**< (PWM_DIS) Channel ID Position */
#define PWM_DIS_CHID0_Msk                     (0x1U << PWM_DIS_CHID0_Pos)                    /**< (PWM_DIS) Channel ID Mask */
#define PWM_DIS_CHID0(value)                  (PWM_DIS_CHID0_Msk & ((value) << PWM_DIS_CHID0_Pos))
#define PWM_DIS_CHID1_Pos                     (1U)                                           /**< (PWM_DIS) Channel ID Position */
#define PWM_DIS_CHID1_Msk                     (0x1U << PWM_DIS_CHID1_Pos)                    /**< (PWM_DIS) Channel ID Mask */
#define PWM_DIS_CHID1(value)                  (PWM_DIS_CHID1_Msk & ((value) << PWM_DIS_CHID1_Pos))
#define PWM_DIS_CHID2_Pos                     (2U)                                           /**< (PWM_DIS) Channel ID Position */
#define PWM_DIS_CHID2_Msk                     (0x1U << PWM_DIS_CHID2_Pos)                    /**< (PWM_DIS) Channel ID Mask */
#define PWM_DIS_CHID2(value)                  (PWM_DIS_CHID2_Msk & ((value) << PWM_DIS_CHID2_Pos))
#define PWM_DIS_CHID3_Pos                     (3U)                                           /**< (PWM_DIS) Channel ID Position */
#define PWM_DIS_CHID3_Msk                     (0x1U << PWM_DIS_CHID3_Pos)                    /**< (PWM_DIS) Channel ID Mask */
#define PWM_DIS_CHID3(value)                  (PWM_DIS_CHID3_Msk & ((value) << PWM_DIS_CHID3_Pos))
#define PWM_DIS_Msk                           (0x0000000FUL)                                 /**< (PWM_DIS) Register Mask  */

/* -------- PWM_SR : (PWM Offset: 0x0C) (R/  32) PWM Status Register -------- */
#define PWM_SR_CHID0_Pos                      (0U)                                           /**< (PWM_SR) Channel ID Position */
#define PWM_SR_CHID0_Msk                      (0x1U << PWM_SR_CHID0_Pos)                     /**< (PWM_SR) Channel ID Mask */
#define PWM_SR_CHID0(value)                   (PWM_SR_CHID0_Msk & ((value) << PWM_SR_CHID0_Pos))
#define PWM_SR_CHID1_Pos                      (1U)                                           /**< (PWM_SR) Channel ID Position */
#define PWM_SR_CHID1_Msk                      (0x1U << PWM_SR_CHID1_Pos)                     /**< (PWM_SR) Channel ID Mask */
#define PWM_SR_CHID1(value)                   (PWM_SR_CHID1_Msk & ((value) << PWM_SR_CHID1_Pos))
#define PWM_SR_CHID2_Pos                      (2U)                                           /**< (PWM_SR) Channel ID Position */
#define PWM_SR_CHID2_Msk                      (0x1U << PWM_SR_CHID2_Pos)                     /**< (PWM_SR) Channel ID Mask */
#define PWM_SR_CHID2(value)                   (PWM_SR_CHID2_Msk & ((value) << PWM_SR_CHID2_Pos))
#define PWM_SR_CHID3_Pos                      (3U)                                           /**< (PWM_SR) Channel ID Position */
#define PWM_SR_CHID3_Msk                      (0x1U << PWM_SR_CHID3_Pos)                     /**< (PWM_SR) Channel ID Mask */
#define PWM_SR_CHID3(value)                   (PWM_SR_CHID3_Msk & ((value) << PWM_SR_CHID3_Pos))
#define PWM_SR_Msk                            (0x0000000FUL)                                 /**< (PWM_SR) Register Mask  */

/* -------- PWM_IER1 : (PWM Offset: 0x10) ( /W 32) PWM Interrupt Enable Register 1 -------- */
#define PWM_IER1_CHID0_Pos                    (0U)                                           /**< (PWM_IER1) Counter Event on Channel 0 Interrupt Enable Position */
#define PWM_IER1_CHID0_Msk                    (0x1U << PWM_IER1_CHID0_Pos)                   /**< (PWM_IER1) Counter Event on Channel 0 Interrupt Enable Mask */
#define PWM_IER1_CHID0(value)                 (PWM_IER1_CHID0_Msk & ((value) << PWM_IER1_CHID0_Pos))
#define PWM_IER1_CHID1_Pos                    (1U)                                           /**< (PWM_IER1) Counter Event on Channel 1 Interrupt Enable Position */
#define PWM_IER1_CHID1_Msk                    (0x1U << PWM_IER1_CHID1_Pos)                   /**< (PWM_IER1) Counter Event on Channel 1 Interrupt Enable Mask */
#define PWM_IER1_CHID1(value)                 (PWM_IER1_CHID1_Msk & ((value) << PWM_IER1_CHID1_Pos))
#define PWM_IER1_CHID2_Pos                    (2U)                                           /**< (PWM_IER1) Counter Event on Channel 2 Interrupt Enable Position */
#define PWM_IER1_CHID2_Msk                    (0x1U << PWM_IER1_CHID2_Pos)                   /**< (PWM_IER1) Counter Event on Channel 2 Interrupt Enable Mask */
#define PWM_IER1_CHID2(value)                 (PWM_IER1_CHID2_Msk & ((value) << PWM_IER1_CHID2_Pos))
#define PWM_IER1_CHID3_Pos                    (3U)                                           /**< (PWM_IER1) Counter Event on Channel 3 Interrupt Enable Position */
#define PWM_IER1_CHID3_Msk                    (0x1U << PWM_IER1_CHID3_Pos)                   /**< (PWM_IER1) Counter Event on Channel 3 Interrupt Enable Mask */
#define PWM_IER1_CHID3(value)                 (PWM_IER1_CHID3_Msk & ((value) << PWM_IER1_CHID3_Pos))
#define PWM_IER1_FCHID0_Pos                   (16U)                                          /**< (PWM_IER1) Fault Protection Trigger on Channel 0 Interrupt Enable Position */
#define PWM_IER1_FCHID0_Msk                   (0x1U << PWM_IER1_FCHID0_Pos)                  /**< (PWM_IER1) Fault Protection Trigger on Channel 0 Interrupt Enable Mask */
#define PWM_IER1_FCHID0(value)                (PWM_IER1_FCHID0_Msk & ((value) << PWM_IER1_FCHID0_Pos))
#define PWM_IER1_FCHID1_Pos                   (17U)                                          /**< (PWM_IER1) Fault Protection Trigger on Channel 1 Interrupt Enable Position */
#define PWM_IER1_FCHID1_Msk                   (0x1U << PWM_IER1_FCHID1_Pos)                  /**< (PWM_IER1) Fault Protection Trigger on Channel 1 Interrupt Enable Mask */
#define PWM_IER1_FCHID1(value)                (PWM_IER1_FCHID1_Msk & ((value) << PWM_IER1_FCHID1_Pos))
#define PWM_IER1_FCHID2_Pos                   (18U)                                          /**< (PWM_IER1) Fault Protection Trigger on Channel 2 Interrupt Enable Position */
#define PWM_IER1_FCHID2_Msk                   (0x1U << PWM_IER1_FCHID2_Pos)                  /**< (PWM_IER1) Fault Protection Trigger on Channel 2 Interrupt Enable Mask */
#define PWM_IER1_FCHID2(value)                (PWM_IER1_FCHID2_Msk & ((value) << PWM_IER1_FCHID2_Pos))
#define PWM_IER1_FCHID3_Pos                   (19U)                                          /**< (PWM_IER1) Fault Protection Trigger on Channel 3 Interrupt Enable Position */
#define PWM_IER1_FCHID3_Msk                   (0x1U << PWM_IER1_FCHID3_Pos)                  /**< (PWM_IER1) Fault Protection Trigger on Channel 3 Interrupt Enable Mask */
#define PWM_IER1_FCHID3(value)                (PWM_IER1_FCHID3_Msk & ((value) << PWM_IER1_FCHID3_Pos))
#define PWM_IER1_Msk                          (0x000F000FUL)                                 /**< (PWM_IER1) Register Mask  */

/* -------- PWM_IDR1 : (PWM Offset: 0x14) ( /W 32) PWM Interrupt Disable Register 1 -------- */
#define PWM_IDR1_CHID0_Pos                    (0U)                                           /**< (PWM_IDR1) Counter Event on Channel 0 Interrupt Disable Position */
#define PWM_IDR1_CHID0_Msk                    (0x1U << PWM_IDR1_CHID0_Pos)                   /**< (PWM_IDR1) Counter Event on Channel 0 Interrupt Disable Mask */
#define PWM_IDR1_CHID0(value)                 (PWM_IDR1_CHID0_Msk & ((value) << PWM_IDR1_CHID0_Pos))
#define PWM_IDR1_CHID1_Pos                    (1U)                                           /**< (PWM_IDR1) Counter Event on Channel 1 Interrupt Disable Position */
#define PWM_IDR1_CHID1_Msk                    (0x1U << PWM_IDR1_CHID1_Pos)                   /**< (PWM_IDR1) Counter Event on Channel 1 Interrupt Disable Mask */
#define PWM_IDR1_CHID1(value)                 (PWM_IDR1_CHID1_Msk & ((value) << PWM_IDR1_CHID1_Pos))
#define PWM_IDR1_CHID2_Pos                    (2U)                                           /**< (PWM_IDR1) Counter Event on Channel 2 Interrupt Disable Position */
#define PWM_IDR1_CHID2_Msk                    (0x1U << PWM_IDR1_CHID2_Pos)                   /**< (PWM_IDR1) Counter Event on Channel 2 Interrupt Disable Mask */
#define PWM_IDR1_CHID2(value)                 (PWM_IDR1_CHID2_Msk & ((value) << PWM_IDR1_CHID2_Pos))
#define PWM_IDR1_CHID3_Pos                    (3U)                                           /**< (PWM_IDR1) Counter Event on Channel 3 Interrupt Disable Position */
#define PWM_IDR1_CHID3_Msk                    (0x1U << PWM_IDR1_CHID3_Pos)                   /**< (PWM_IDR1) Counter Event on Channel 3 Interrupt Disable Mask */
#define PWM_IDR1_CHID3(value)                 (PWM_IDR1_CHID3_Msk & ((value) << PWM_IDR1_CHID3_Pos))
#define PWM_IDR1_FCHID0_Pos                   (16U)                                          /**< (PWM_IDR1) Fault Protection Trigger on Channel 0 Interrupt Disable Position */
#define PWM_IDR1_FCHID0_Msk                   (0x1U << PWM_IDR1_FCHID0_Pos)                  /**< (PWM_IDR1) Fault Protection Trigger on Channel 0 Interrupt Disable Mask */
#define PWM_IDR1_FCHID0(value)                (PWM_IDR1_FCHID0_Msk & ((value) << PWM_IDR1_FCHID0_Pos))
#define PWM_IDR1_FCHID1_Pos                   (17U)                                          /**< (PWM_IDR1) Fault Protection Trigger on Channel 1 Interrupt Disable Position */
#define PWM_IDR1_FCHID1_Msk                   (0x1U << PWM_IDR1_FCHID1_Pos)                  /**< (PWM_IDR1) Fault Protection Trigger on Channel 1 Interrupt Disable Mask */
#define PWM_IDR1_FCHID1(value)                (PWM_IDR1_FCHID1_Msk & ((value) << PWM_IDR1_FCHID1_Pos))
#define PWM_IDR1_FCHID2_Pos                   (18U)                                          /**< (PWM_IDR1) Fault Protection Trigger on Channel 2 Interrupt Disable Position */
#define PWM_IDR1_FCHID2_Msk                   (0x1U << PWM_IDR1_FCHID2_Pos)                  /**< (PWM_IDR1) Fault Protection Trigger on Channel 2 Interrupt Disable Mask */
#define PWM_IDR1_FCHID2(value)                (PWM_IDR1_FCHID2_Msk & ((value) << PWM_IDR1_FCHID2_Pos))
#define PWM_IDR1_FCHID3_Pos                   (19U)                                          /**< (PWM_IDR1) Fault Protection Trigger on Channel 3 Interrupt Disable Position */
#define PWM_IDR1_FCHID3_Msk                   (0x1U << PWM_IDR1_FCHID3_Pos)                  /**< (PWM_IDR1) Fault Protection Trigger on Channel 3 Interrupt Disable Mask */
#define PWM_IDR1_FCHID3(value)                (PWM_IDR1_FCHID3_Msk & ((value) << PWM_IDR1_FCHID3_Pos))
#define PWM_IDR1_Msk                          (0x000F000FUL)                                 /**< (PWM_IDR1) Register Mask  */

/* -------- PWM_IMR1 : (PWM Offset: 0x18) (R/  32) PWM Interrupt Mask Register 1 -------- */
#define PWM_IMR1_CHID0_Pos                    (0U)                                           /**< (PWM_IMR1) Counter Event on Channel 0 Interrupt Mask Position */
#define PWM_IMR1_CHID0_Msk                    (0x1U << PWM_IMR1_CHID0_Pos)                   /**< (PWM_IMR1) Counter Event on Channel 0 Interrupt Mask Mask */
#define PWM_IMR1_CHID0(value)                 (PWM_IMR1_CHID0_Msk & ((value) << PWM_IMR1_CHID0_Pos))
#define PWM_IMR1_CHID1_Pos                    (1U)                                           /**< (PWM_IMR1) Counter Event on Channel 1 Interrupt Mask Position */
#define PWM_IMR1_CHID1_Msk                    (0x1U << PWM_IMR1_CHID1_Pos)                   /**< (PWM_IMR1) Counter Event on Channel 1 Interrupt Mask Mask */
#define PWM_IMR1_CHID1(value)                 (PWM_IMR1_CHID1_Msk & ((value) << PWM_IMR1_CHID1_Pos))
#define PWM_IMR1_CHID2_Pos                    (2U)                                           /**< (PWM_IMR1) Counter Event on Channel 2 Interrupt Mask Position */
#define PWM_IMR1_CHID2_Msk                    (0x1U << PWM_IMR1_CHID2_Pos)                   /**< (PWM_IMR1) Counter Event on Channel 2 Interrupt Mask Mask */
#define PWM_IMR1_CHID2(value)                 (PWM_IMR1_CHID2_Msk & ((value) << PWM_IMR1_CHID2_Pos))
#define PWM_IMR1_CHID3_Pos                    (3U)                                           /**< (PWM_IMR1) Counter Event on Channel 3 Interrupt Mask Position */
#define PWM_IMR1_CHID3_Msk                    (0x1U << PWM_IMR1_CHID3_Pos)                   /**< (PWM_IMR1) Counter Event on Channel 3 Interrupt Mask Mask */
#define PWM_IMR1_CHID3(value)                 (PWM_IMR1_CHID3_Msk & ((value) << PWM_IMR1_CHID3_Pos))
#define PWM_IMR1_FCHID0_Pos                   (16U)                                          /**< (PWM_IMR1) Fault Protection Trigger on Channel 0 Interrupt Mask Position */
#define PWM_IMR1_FCHID0_Msk                   (0x1U << PWM_IMR1_FCHID0_Pos)                  /**< (PWM_IMR1) Fault Protection Trigger on Channel 0 Interrupt Mask Mask */
#define PWM_IMR1_FCHID0(value)                (PWM_IMR1_FCHID0_Msk & ((value) << PWM_IMR1_FCHID0_Pos))
#define PWM_IMR1_FCHID1_Pos                   (17U)                                          /**< (PWM_IMR1) Fault Protection Trigger on Channel 1 Interrupt Mask Position */
#define PWM_IMR1_FCHID1_Msk                   (0x1U << PWM_IMR1_FCHID1_Pos)                  /**< (PWM_IMR1) Fault Protection Trigger on Channel 1 Interrupt Mask Mask */
#define PWM_IMR1_FCHID1(value)                (PWM_IMR1_FCHID1_Msk & ((value) << PWM_IMR1_FCHID1_Pos))
#define PWM_IMR1_FCHID2_Pos                   (18U)                                          /**< (PWM_IMR1) Fault Protection Trigger on Channel 2 Interrupt Mask Position */
#define PWM_IMR1_FCHID2_Msk                   (0x1U << PWM_IMR1_FCHID2_Pos)                  /**< (PWM_IMR1) Fault Protection Trigger on Channel 2 Interrupt Mask Mask */
#define PWM_IMR1_FCHID2(value)                (PWM_IMR1_FCHID2_Msk & ((value) << PWM_IMR1_FCHID2_Pos))
#define PWM_IMR1_FCHID3_Pos                   (19U)                                          /**< (PWM_IMR1) Fault Protection Trigger on Channel 3 Interrupt Mask Position */
#define PWM_IMR1_FCHID3_Msk                   (0x1U << PWM_IMR1_FCHID3_Pos)                  /**< (PWM_IMR1) Fault Protection Trigger on Channel 3 Interrupt Mask Mask */
#define PWM_IMR1_FCHID3(value)                (PWM_IMR1_FCHID3_Msk & ((value) << PWM_IMR1_FCHID3_Pos))
#define PWM_IMR1_Msk                          (0x000F000FUL)                                 /**< (PWM_IMR1) Register Mask  */

/* -------- PWM_ISR1 : (PWM Offset: 0x1C) (R/  32) PWM Interrupt Status Register 1 -------- */
#define PWM_ISR1_CHID0_Pos                    (0U)                                           /**< (PWM_ISR1) Counter Event on Channel 0 Position */
#define PWM_ISR1_CHID0_Msk                    (0x1U << PWM_ISR1_CHID0_Pos)                   /**< (PWM_ISR1) Counter Event on Channel 0 Mask */
#define PWM_ISR1_CHID0(value)                 (PWM_ISR1_CHID0_Msk & ((value) << PWM_ISR1_CHID0_Pos))
#define PWM_ISR1_CHID1_Pos                    (1U)                                           /**< (PWM_ISR1) Counter Event on Channel 1 Position */
#define PWM_ISR1_CHID1_Msk                    (0x1U << PWM_ISR1_CHID1_Pos)                   /**< (PWM_ISR1) Counter Event on Channel 1 Mask */
#define PWM_ISR1_CHID1(value)                 (PWM_ISR1_CHID1_Msk & ((value) << PWM_ISR1_CHID1_Pos))
#define PWM_ISR1_CHID2_Pos                    (2U)                                           /**< (PWM_ISR1) Counter Event on Channel 2 Position */
#define PWM_ISR1_CHID2_Msk                    (0x1U << PWM_ISR1_CHID2_Pos)                   /**< (PWM_ISR1) Counter Event on Channel 2 Mask */
#define PWM_ISR1_CHID2(value)                 (PWM_ISR1_CHID2_Msk & ((value) << PWM_ISR1_CHID2_Pos))
#define PWM_ISR1_CHID3_Pos                    (3U)                                           /**< (PWM_ISR1) Counter Event on Channel 3 Position */
#define PWM_ISR1_CHID3_Msk                    (0x1U << PWM_ISR1_CHID3_Pos)                   /**< (PWM_ISR1) Counter Event on Channel 3 Mask */
#define PWM_ISR1_CHID3(value)                 (PWM_ISR1_CHID3_Msk & ((value) << PWM_ISR1_CHID3_Pos))
#define PWM_ISR1_FCHID0_Pos                   (16U)                                          /**< (PWM_ISR1) Fault Protection Trigger on Channel 0 Position */
#define PWM_ISR1_FCHID0_Msk                   (0x1U << PWM_ISR1_FCHID0_Pos)                  /**< (PWM_ISR1) Fault Protection Trigger on Channel 0 Mask */
#define PWM_ISR1_FCHID0(value)                (PWM_ISR1_FCHID0_Msk & ((value) << PWM_ISR1_FCHID0_Pos))
#define PWM_ISR1_FCHID1_Pos                   (17U)                                          /**< (PWM_ISR1) Fault Protection Trigger on Channel 1 Position */
#define PWM_ISR1_FCHID1_Msk                   (0x1U << PWM_ISR1_FCHID1_Pos)                  /**< (PWM_ISR1) Fault Protection Trigger on Channel 1 Mask */
#define PWM_ISR1_FCHID1(value)                (PWM_ISR1_FCHID1_Msk & ((value) << PWM_ISR1_FCHID1_Pos))
#define PWM_ISR1_FCHID2_Pos                   (18U)                                          /**< (PWM_ISR1) Fault Protection Trigger on Channel 2 Position */
#define PWM_ISR1_FCHID2_Msk                   (0x1U << PWM_ISR1_FCHID2_Pos)                  /**< (PWM_ISR1) Fault Protection Trigger on Channel 2 Mask */
#define PWM_ISR1_FCHID2(value)                (PWM_ISR1_FCHID2_Msk & ((value) << PWM_ISR1_FCHID2_Pos))
#define PWM_ISR1_FCHID3_Pos                   (19U)                                          /**< (PWM_ISR1) Fault Protection Trigger on Channel 3 Position */
#define PWM_ISR1_FCHID3_Msk                   (0x1U << PWM_ISR1_FCHID3_Pos)                  /**< (PWM_ISR1) Fault Protection Trigger on Channel 3 Mask */
#define PWM_ISR1_FCHID3(value)                (PWM_ISR1_FCHID3_Msk & ((value) << PWM_ISR1_FCHID3_Pos))
#define PWM_ISR1_Msk                          (0x000F000FUL)                                 /**< (PWM_ISR1) Register Mask  */

/* -------- PWM_SCM : (PWM Offset: 0x20) (R/W 32) PWM Sync Channels Mode Register -------- */
#define PWM_SCM_SYNC0_Pos                     (0U)                                           /**< (PWM_SCM) Synchronous Channel 0 Position */
#define PWM_SCM_SYNC0_Msk                     (0x1U << PWM_SCM_SYNC0_Pos)                    /**< (PWM_SCM) Synchronous Channel 0 Mask */
#define PWM_SCM_SYNC0(value)                  (PWM_SCM_SYNC0_Msk & ((value) << PWM_SCM_SYNC0_Pos))
#define PWM_SCM_SYNC1_Pos                     (1U)                                           /**< (PWM_SCM) Synchronous Channel 1 Position */
#define PWM_SCM_SYNC1_Msk                     (0x1U << PWM_SCM_SYNC1_Pos)                    /**< (PWM_SCM) Synchronous Channel 1 Mask */
#define PWM_SCM_SYNC1(value)                  (PWM_SCM_SYNC1_Msk & ((value) << PWM_SCM_SYNC1_Pos))
#define PWM_SCM_SYNC2_Pos                     (2U)                                           /**< (PWM_SCM) Synchronous Channel 2 Position */
#define PWM_SCM_SYNC2_Msk                     (0x1U << PWM_SCM_SYNC2_Pos)                    /**< (PWM_SCM) Synchronous Channel 2 Mask */
#define PWM_SCM_SYNC2(value)                  (PWM_SCM_SYNC2_Msk & ((value) << PWM_SCM_SYNC2_Pos))
#define PWM_SCM_SYNC3_Pos                     (3U)                                           /**< (PWM_SCM) Synchronous Channel 3 Position */
#define PWM_SCM_SYNC3_Msk                     (0x1U << PWM_SCM_SYNC3_Pos)                    /**< (PWM_SCM) Synchronous Channel 3 Mask */
#define PWM_SCM_SYNC3(value)                  (PWM_SCM_SYNC3_Msk & ((value) << PWM_SCM_SYNC3_Pos))
#define PWM_SCM_UPDM_Pos                      (16U)                                          /**< (PWM_SCM) Synchronous Channels Update Mode Position */
#define PWM_SCM_UPDM_Msk                      (0x3U << PWM_SCM_UPDM_Pos)                     /**< (PWM_SCM) Synchronous Channels Update Mode Mask */
#define PWM_SCM_UPDM(value)                   (PWM_SCM_UPDM_Msk & ((value) << PWM_SCM_UPDM_Pos))
#define   PWM_SCM_UPDM_MODE0_Val              (0U)                                           /**< (PWM_SCM) Manual write of double buffer registers and manual update of synchronous channels  */
#define   PWM_SCM_UPDM_MODE1_Val              (1U)                                           /**< (PWM_SCM) Manual write of double buffer registers and automatic update of synchronous channels  */
#define   PWM_SCM_UPDM_MODE2_Val              (2U)                                           /**< (PWM_SCM) Automatic write of duty-cycle update registers by the DMA Controller and automatic update of synchronous channels  */
#define PWM_SCM_UPDM_MODE0                    (PWM_SCM_UPDM_MODE0_Val << PWM_SCM_UPDM_Pos)   /**< (PWM_SCM) Manual write of double buffer registers and manual update of synchronous channels Position  */
#define PWM_SCM_UPDM_MODE1                    (PWM_SCM_UPDM_MODE1_Val << PWM_SCM_UPDM_Pos)   /**< (PWM_SCM) Manual write of double buffer registers and automatic update of synchronous channels Position  */
#define PWM_SCM_UPDM_MODE2                    (PWM_SCM_UPDM_MODE2_Val << PWM_SCM_UPDM_Pos)   /**< (PWM_SCM) Automatic write of duty-cycle update registers by the DMA Controller and automatic update of synchronous channels Position  */
#define PWM_SCM_PTRM_Pos                      (20U)                                          /**< (PWM_SCM) DMA Controller Transfer Request Mode Position */
#define PWM_SCM_PTRM_Msk                      (0x1U << PWM_SCM_PTRM_Pos)                     /**< (PWM_SCM) DMA Controller Transfer Request Mode Mask */
#define PWM_SCM_PTRM(value)                   (PWM_SCM_PTRM_Msk & ((value) << PWM_SCM_PTRM_Pos))
#define PWM_SCM_PTRCS_Pos                     (21U)                                          /**< (PWM_SCM) DMA Controller Transfer Request Comparison Selection Position */
#define PWM_SCM_PTRCS_Msk                     (0x7U << PWM_SCM_PTRCS_Pos)                    /**< (PWM_SCM) DMA Controller Transfer Request Comparison Selection Mask */
#define PWM_SCM_PTRCS(value)                  (PWM_SCM_PTRCS_Msk & ((value) << PWM_SCM_PTRCS_Pos))
#define PWM_SCM_Msk                           (0x00F3000FUL)                                 /**< (PWM_SCM) Register Mask  */

/* -------- PWM_DMAR : (PWM Offset: 0x24) ( /W 32) PWM DMA Register -------- */
#define PWM_DMAR_DMADUTY_Pos                  (0U)                                           /**< (PWM_DMAR) Duty-Cycle Holding Register for DMA Access Position */
#define PWM_DMAR_DMADUTY_Msk                  (0xFFFFFFU << PWM_DMAR_DMADUTY_Pos)            /**< (PWM_DMAR) Duty-Cycle Holding Register for DMA Access Mask */
#define PWM_DMAR_DMADUTY(value)               (PWM_DMAR_DMADUTY_Msk & ((value) << PWM_DMAR_DMADUTY_Pos))
#define PWM_DMAR_Msk                          (0x00FFFFFFUL)                                 /**< (PWM_DMAR) Register Mask  */

/* -------- PWM_SCUC : (PWM Offset: 0x28) (R/W 32) PWM Sync Channels Update Control Register -------- */
#define PWM_SCUC_UPDULOCK_Pos                 (0U)                                           /**< (PWM_SCUC) Synchronous Channels Update Unlock Position */
#define PWM_SCUC_UPDULOCK_Msk                 (0x1U << PWM_SCUC_UPDULOCK_Pos)                /**< (PWM_SCUC) Synchronous Channels Update Unlock Mask */
#define PWM_SCUC_UPDULOCK(value)              (PWM_SCUC_UPDULOCK_Msk & ((value) << PWM_SCUC_UPDULOCK_Pos))
#define PWM_SCUC_Msk                          (0x00000001UL)                                 /**< (PWM_SCUC) Register Mask  */

/* -------- PWM_SCUP : (PWM Offset: 0x2C) (R/W 32) PWM Sync Channels Update Period Register -------- */
#define PWM_SCUP_UPR_Pos                      (0U)                                           /**< (PWM_SCUP) Update Period Position */
#define PWM_SCUP_UPR_Msk                      (0xFU << PWM_SCUP_UPR_Pos)                     /**< (PWM_SCUP) Update Period Mask */
#define PWM_SCUP_UPR(value)                   (PWM_SCUP_UPR_Msk & ((value) << PWM_SCUP_UPR_Pos))
#define PWM_SCUP_UPRCNT_Pos                   (4U)                                           /**< (PWM_SCUP) Update Period Counter Position */
#define PWM_SCUP_UPRCNT_Msk                   (0xFU << PWM_SCUP_UPRCNT_Pos)                  /**< (PWM_SCUP) Update Period Counter Mask */
#define PWM_SCUP_UPRCNT(value)                (PWM_SCUP_UPRCNT_Msk & ((value) << PWM_SCUP_UPRCNT_Pos))
#define PWM_SCUP_Msk                          (0x000000FFUL)                                 /**< (PWM_SCUP) Register Mask  */

/* -------- PWM_SCUPUPD : (PWM Offset: 0x30) ( /W 32) PWM Sync Channels Update Period Update Register -------- */
#define PWM_SCUPUPD_UPRUPD_Pos                (0U)                                           /**< (PWM_SCUPUPD) Update Period Update Position */
#define PWM_SCUPUPD_UPRUPD_Msk                (0xFU << PWM_SCUPUPD_UPRUPD_Pos)               /**< (PWM_SCUPUPD) Update Period Update Mask */
#define PWM_SCUPUPD_UPRUPD(value)             (PWM_SCUPUPD_UPRUPD_Msk & ((value) << PWM_SCUPUPD_UPRUPD_Pos))
#define PWM_SCUPUPD_Msk                       (0x0000000FUL)                                 /**< (PWM_SCUPUPD) Register Mask  */

/* -------- PWM_IER2 : (PWM Offset: 0x34) ( /W 32) PWM Interrupt Enable Register 2 -------- */
#define PWM_IER2_WRDY_Pos                     (0U)                                           /**< (PWM_IER2) Write Ready for Synchronous Channels Update Interrupt Enable Position */
#define PWM_IER2_WRDY_Msk                     (0x1U << PWM_IER2_WRDY_Pos)                    /**< (PWM_IER2) Write Ready for Synchronous Channels Update Interrupt Enable Mask */
#define PWM_IER2_WRDY(value)                  (PWM_IER2_WRDY_Msk & ((value) << PWM_IER2_WRDY_Pos))
#define PWM_IER2_UNRE_Pos                     (3U)                                           /**< (PWM_IER2) Synchronous Channels Update Underrun Error Interrupt Enable Position */
#define PWM_IER2_UNRE_Msk                     (0x1U << PWM_IER2_UNRE_Pos)                    /**< (PWM_IER2) Synchronous Channels Update Underrun Error Interrupt Enable Mask */
#define PWM_IER2_UNRE(value)                  (PWM_IER2_UNRE_Msk & ((value) << PWM_IER2_UNRE_Pos))
#define PWM_IER2_CMPM0_Pos                    (8U)                                           /**< (PWM_IER2) Comparison 0 Match Interrupt Enable Position */
#define PWM_IER2_CMPM0_Msk                    (0x1U << PWM_IER2_CMPM0_Pos)                   /**< (PWM_IER2) Comparison 0 Match Interrupt Enable Mask */
#define PWM_IER2_CMPM0(value)                 (PWM_IER2_CMPM0_Msk & ((value) << PWM_IER2_CMPM0_Pos))
#define PWM_IER2_CMPM1_Pos                    (9U)                                           /**< (PWM_IER2) Comparison 1 Match Interrupt Enable Position */
#define PWM_IER2_CMPM1_Msk                    (0x1U << PWM_IER2_CMPM1_Pos)                   /**< (PWM_IER2) Comparison 1 Match Interrupt Enable Mask */
#define PWM_IER2_CMPM1(value)                 (PWM_IER2_CMPM1_Msk & ((value) << PWM_IER2_CMPM1_Pos))
#define PWM_IER2_CMPM2_Pos                    (10U)                                          /**< (PWM_IER2) Comparison 2 Match Interrupt Enable Position */
#define PWM_IER2_CMPM2_Msk                    (0x1U << PWM_IER2_CMPM2_Pos)                   /**< (PWM_IER2) Comparison 2 Match Interrupt Enable Mask */
#define PWM_IER2_CMPM2(value)                 (PWM_IER2_CMPM2_Msk & ((value) << PWM_IER2_CMPM2_Pos))
#define PWM_IER2_CMPM3_Pos                    (11U)                                          /**< (PWM_IER2) Comparison 3 Match Interrupt Enable Position */
#define PWM_IER2_CMPM3_Msk                    (0x1U << PWM_IER2_CMPM3_Pos)                   /**< (PWM_IER2) Comparison 3 Match Interrupt Enable Mask */
#define PWM_IER2_CMPM3(value)                 (PWM_IER2_CMPM3_Msk & ((value) << PWM_IER2_CMPM3_Pos))
#define PWM_IER2_CMPM4_Pos                    (12U)                                          /**< (PWM_IER2) Comparison 4 Match Interrupt Enable Position */
#define PWM_IER2_CMPM4_Msk                    (0x1U << PWM_IER2_CMPM4_Pos)                   /**< (PWM_IER2) Comparison 4 Match Interrupt Enable Mask */
#define PWM_IER2_CMPM4(value)                 (PWM_IER2_CMPM4_Msk & ((value) << PWM_IER2_CMPM4_Pos))
#define PWM_IER2_CMPM5_Pos                    (13U)                                          /**< (PWM_IER2) Comparison 5 Match Interrupt Enable Position */
#define PWM_IER2_CMPM5_Msk                    (0x1U << PWM_IER2_CMPM5_Pos)                   /**< (PWM_IER2) Comparison 5 Match Interrupt Enable Mask */
#define PWM_IER2_CMPM5(value)                 (PWM_IER2_CMPM5_Msk & ((value) << PWM_IER2_CMPM5_Pos))
#define PWM_IER2_CMPM6_Pos                    (14U)                                          /**< (PWM_IER2) Comparison 6 Match Interrupt Enable Position */
#define PWM_IER2_CMPM6_Msk                    (0x1U << PWM_IER2_CMPM6_Pos)                   /**< (PWM_IER2) Comparison 6 Match Interrupt Enable Mask */
#define PWM_IER2_CMPM6(value)                 (PWM_IER2_CMPM6_Msk & ((value) << PWM_IER2_CMPM6_Pos))
#define PWM_IER2_CMPM7_Pos                    (15U)                                          /**< (PWM_IER2) Comparison 7 Match Interrupt Enable Position */
#define PWM_IER2_CMPM7_Msk                    (0x1U << PWM_IER2_CMPM7_Pos)                   /**< (PWM_IER2) Comparison 7 Match Interrupt Enable Mask */
#define PWM_IER2_CMPM7(value)                 (PWM_IER2_CMPM7_Msk & ((value) << PWM_IER2_CMPM7_Pos))
#define PWM_IER2_CMPU0_Pos                    (16U)                                          /**< (PWM_IER2) Comparison 0 Update Interrupt Enable Position */
#define PWM_IER2_CMPU0_Msk                    (0x1U << PWM_IER2_CMPU0_Pos)                   /**< (PWM_IER2) Comparison 0 Update Interrupt Enable Mask */
#define PWM_IER2_CMPU0(value)                 (PWM_IER2_CMPU0_Msk & ((value) << PWM_IER2_CMPU0_Pos))
#define PWM_IER2_CMPU1_Pos                    (17U)                                          /**< (PWM_IER2) Comparison 1 Update Interrupt Enable Position */
#define PWM_IER2_CMPU1_Msk                    (0x1U << PWM_IER2_CMPU1_Pos)                   /**< (PWM_IER2) Comparison 1 Update Interrupt Enable Mask */
#define PWM_IER2_CMPU1(value)                 (PWM_IER2_CMPU1_Msk & ((value) << PWM_IER2_CMPU1_Pos))
#define PWM_IER2_CMPU2_Pos                    (18U)                                          /**< (PWM_IER2) Comparison 2 Update Interrupt Enable Position */
#define PWM_IER2_CMPU2_Msk                    (0x1U << PWM_IER2_CMPU2_Pos)                   /**< (PWM_IER2) Comparison 2 Update Interrupt Enable Mask */
#define PWM_IER2_CMPU2(value)                 (PWM_IER2_CMPU2_Msk & ((value) << PWM_IER2_CMPU2_Pos))
#define PWM_IER2_CMPU3_Pos                    (19U)                                          /**< (PWM_IER2) Comparison 3 Update Interrupt Enable Position */
#define PWM_IER2_CMPU3_Msk                    (0x1U << PWM_IER2_CMPU3_Pos)                   /**< (PWM_IER2) Comparison 3 Update Interrupt Enable Mask */
#define PWM_IER2_CMPU3(value)                 (PWM_IER2_CMPU3_Msk & ((value) << PWM_IER2_CMPU3_Pos))
#define PWM_IER2_CMPU4_Pos                    (20U)                                          /**< (PWM_IER2) Comparison 4 Update Interrupt Enable Position */
#define PWM_IER2_CMPU4_Msk                    (0x1U << PWM_IER2_CMPU4_Pos)                   /**< (PWM_IER2) Comparison 4 Update Interrupt Enable Mask */
#define PWM_IER2_CMPU4(value)                 (PWM_IER2_CMPU4_Msk & ((value) << PWM_IER2_CMPU4_Pos))
#define PWM_IER2_CMPU5_Pos                    (21U)                                          /**< (PWM_IER2) Comparison 5 Update Interrupt Enable Position */
#define PWM_IER2_CMPU5_Msk                    (0x1U << PWM_IER2_CMPU5_Pos)                   /**< (PWM_IER2) Comparison 5 Update Interrupt Enable Mask */
#define PWM_IER2_CMPU5(value)                 (PWM_IER2_CMPU5_Msk & ((value) << PWM_IER2_CMPU5_Pos))
#define PWM_IER2_CMPU6_Pos                    (22U)                                          /**< (PWM_IER2) Comparison 6 Update Interrupt Enable Position */
#define PWM_IER2_CMPU6_Msk                    (0x1U << PWM_IER2_CMPU6_Pos)                   /**< (PWM_IER2) Comparison 6 Update Interrupt Enable Mask */
#define PWM_IER2_CMPU6(value)                 (PWM_IER2_CMPU6_Msk & ((value) << PWM_IER2_CMPU6_Pos))
#define PWM_IER2_CMPU7_Pos                    (23U)                                          /**< (PWM_IER2) Comparison 7 Update Interrupt Enable Position */
#define PWM_IER2_CMPU7_Msk                    (0x1U << PWM_IER2_CMPU7_Pos)                   /**< (PWM_IER2) Comparison 7 Update Interrupt Enable Mask */
#define PWM_IER2_CMPU7(value)                 (PWM_IER2_CMPU7_Msk & ((value) << PWM_IER2_CMPU7_Pos))
#define PWM_IER2_Msk                          (0x00FFFF09UL)                                 /**< (PWM_IER2) Register Mask  */

/* -------- PWM_IDR2 : (PWM Offset: 0x38) ( /W 32) PWM Interrupt Disable Register 2 -------- */
#define PWM_IDR2_WRDY_Pos                     (0U)                                           /**< (PWM_IDR2) Write Ready for Synchronous Channels Update Interrupt Disable Position */
#define PWM_IDR2_WRDY_Msk                     (0x1U << PWM_IDR2_WRDY_Pos)                    /**< (PWM_IDR2) Write Ready for Synchronous Channels Update Interrupt Disable Mask */
#define PWM_IDR2_WRDY(value)                  (PWM_IDR2_WRDY_Msk & ((value) << PWM_IDR2_WRDY_Pos))
#define PWM_IDR2_UNRE_Pos                     (3U)                                           /**< (PWM_IDR2) Synchronous Channels Update Underrun Error Interrupt Disable Position */
#define PWM_IDR2_UNRE_Msk                     (0x1U << PWM_IDR2_UNRE_Pos)                    /**< (PWM_IDR2) Synchronous Channels Update Underrun Error Interrupt Disable Mask */
#define PWM_IDR2_UNRE(value)                  (PWM_IDR2_UNRE_Msk & ((value) << PWM_IDR2_UNRE_Pos))
#define PWM_IDR2_CMPM0_Pos                    (8U)                                           /**< (PWM_IDR2) Comparison 0 Match Interrupt Disable Position */
#define PWM_IDR2_CMPM0_Msk                    (0x1U << PWM_IDR2_CMPM0_Pos)                   /**< (PWM_IDR2) Comparison 0 Match Interrupt Disable Mask */
#define PWM_IDR2_CMPM0(value)                 (PWM_IDR2_CMPM0_Msk & ((value) << PWM_IDR2_CMPM0_Pos))
#define PWM_IDR2_CMPM1_Pos                    (9U)                                           /**< (PWM_IDR2) Comparison 1 Match Interrupt Disable Position */
#define PWM_IDR2_CMPM1_Msk                    (0x1U << PWM_IDR2_CMPM1_Pos)                   /**< (PWM_IDR2) Comparison 1 Match Interrupt Disable Mask */
#define PWM_IDR2_CMPM1(value)                 (PWM_IDR2_CMPM1_Msk & ((value) << PWM_IDR2_CMPM1_Pos))
#define PWM_IDR2_CMPM2_Pos                    (10U)                                          /**< (PWM_IDR2) Comparison 2 Match Interrupt Disable Position */
#define PWM_IDR2_CMPM2_Msk                    (0x1U << PWM_IDR2_CMPM2_Pos)                   /**< (PWM_IDR2) Comparison 2 Match Interrupt Disable Mask */
#define PWM_IDR2_CMPM2(value)                 (PWM_IDR2_CMPM2_Msk & ((value) << PWM_IDR2_CMPM2_Pos))
#define PWM_IDR2_CMPM3_Pos                    (11U)                                          /**< (PWM_IDR2) Comparison 3 Match Interrupt Disable Position */
#define PWM_IDR2_CMPM3_Msk                    (0x1U << PWM_IDR2_CMPM3_Pos)                   /**< (PWM_IDR2) Comparison 3 Match Interrupt Disable Mask */
#define PWM_IDR2_CMPM3(value)                 (PWM_IDR2_CMPM3_Msk & ((value) << PWM_IDR2_CMPM3_Pos))
#define PWM_IDR2_CMPM4_Pos                    (12U)                                          /**< (PWM_IDR2) Comparison 4 Match Interrupt Disable Position */
#define PWM_IDR2_CMPM4_Msk                    (0x1U << PWM_IDR2_CMPM4_Pos)                   /**< (PWM_IDR2) Comparison 4 Match Interrupt Disable Mask */
#define PWM_IDR2_CMPM4(value)                 (PWM_IDR2_CMPM4_Msk & ((value) << PWM_IDR2_CMPM4_Pos))
#define PWM_IDR2_CMPM5_Pos                    (13U)                                          /**< (PWM_IDR2) Comparison 5 Match Interrupt Disable Position */
#define PWM_IDR2_CMPM5_Msk                    (0x1U << PWM_IDR2_CMPM5_Pos)                   /**< (PWM_IDR2) Comparison 5 Match Interrupt Disable Mask */
#define PWM_IDR2_CMPM5(value)                 (PWM_IDR2_CMPM5_Msk & ((value) << PWM_IDR2_CMPM5_Pos))
#define PWM_IDR2_CMPM6_Pos                    (14U)                                          /**< (PWM_IDR2) Comparison 6 Match Interrupt Disable Position */
#define PWM_IDR2_CMPM6_Msk                    (0x1U << PWM_IDR2_CMPM6_Pos)                   /**< (PWM_IDR2) Comparison 6 Match Interrupt Disable Mask */
#define PWM_IDR2_CMPM6(value)                 (PWM_IDR2_CMPM6_Msk & ((value) << PWM_IDR2_CMPM6_Pos))
#define PWM_IDR2_CMPM7_Pos                    (15U)                                          /**< (PWM_IDR2) Comparison 7 Match Interrupt Disable Position */
#define PWM_IDR2_CMPM7_Msk                    (0x1U << PWM_IDR2_CMPM7_Pos)                   /**< (PWM_IDR2) Comparison 7 Match Interrupt Disable Mask */
#define PWM_IDR2_CMPM7(value)                 (PWM_IDR2_CMPM7_Msk & ((value) << PWM_IDR2_CMPM7_Pos))
#define PWM_IDR2_CMPU0_Pos                    (16U)                                          /**< (PWM_IDR2) Comparison 0 Update Interrupt Disable Position */
#define PWM_IDR2_CMPU0_Msk                    (0x1U << PWM_IDR2_CMPU0_Pos)                   /**< (PWM_IDR2) Comparison 0 Update Interrupt Disable Mask */
#define PWM_IDR2_CMPU0(value)                 (PWM_IDR2_CMPU0_Msk & ((value) << PWM_IDR2_CMPU0_Pos))
#define PWM_IDR2_CMPU1_Pos                    (17U)                                          /**< (PWM_IDR2) Comparison 1 Update Interrupt Disable Position */
#define PWM_IDR2_CMPU1_Msk                    (0x1U << PWM_IDR2_CMPU1_Pos)                   /**< (PWM_IDR2) Comparison 1 Update Interrupt Disable Mask */
#define PWM_IDR2_CMPU1(value)                 (PWM_IDR2_CMPU1_Msk & ((value) << PWM_IDR2_CMPU1_Pos))
#define PWM_IDR2_CMPU2_Pos                    (18U)                                          /**< (PWM_IDR2) Comparison 2 Update Interrupt Disable Position */
#define PWM_IDR2_CMPU2_Msk                    (0x1U << PWM_IDR2_CMPU2_Pos)                   /**< (PWM_IDR2) Comparison 2 Update Interrupt Disable Mask */
#define PWM_IDR2_CMPU2(value)                 (PWM_IDR2_CMPU2_Msk & ((value) << PWM_IDR2_CMPU2_Pos))
#define PWM_IDR2_CMPU3_Pos                    (19U)                                          /**< (PWM_IDR2) Comparison 3 Update Interrupt Disable Position */
#define PWM_IDR2_CMPU3_Msk                    (0x1U << PWM_IDR2_CMPU3_Pos)                   /**< (PWM_IDR2) Comparison 3 Update Interrupt Disable Mask */
#define PWM_IDR2_CMPU3(value)                 (PWM_IDR2_CMPU3_Msk & ((value) << PWM_IDR2_CMPU3_Pos))
#define PWM_IDR2_CMPU4_Pos                    (20U)                                          /**< (PWM_IDR2) Comparison 4 Update Interrupt Disable Position */
#define PWM_IDR2_CMPU4_Msk                    (0x1U << PWM_IDR2_CMPU4_Pos)                   /**< (PWM_IDR2) Comparison 4 Update Interrupt Disable Mask */
#define PWM_IDR2_CMPU4(value)                 (PWM_IDR2_CMPU4_Msk & ((value) << PWM_IDR2_CMPU4_Pos))
#define PWM_IDR2_CMPU5_Pos                    (21U)                                          /**< (PWM_IDR2) Comparison 5 Update Interrupt Disable Position */
#define PWM_IDR2_CMPU5_Msk                    (0x1U << PWM_IDR2_CMPU5_Pos)                   /**< (PWM_IDR2) Comparison 5 Update Interrupt Disable Mask */
#define PWM_IDR2_CMPU5(value)                 (PWM_IDR2_CMPU5_Msk & ((value) << PWM_IDR2_CMPU5_Pos))
#define PWM_IDR2_CMPU6_Pos                    (22U)                                          /**< (PWM_IDR2) Comparison 6 Update Interrupt Disable Position */
#define PWM_IDR2_CMPU6_Msk                    (0x1U << PWM_IDR2_CMPU6_Pos)                   /**< (PWM_IDR2) Comparison 6 Update Interrupt Disable Mask */
#define PWM_IDR2_CMPU6(value)                 (PWM_IDR2_CMPU6_Msk & ((value) << PWM_IDR2_CMPU6_Pos))
#define PWM_IDR2_CMPU7_Pos                    (23U)                                          /**< (PWM_IDR2) Comparison 7 Update Interrupt Disable Position */
#define PWM_IDR2_CMPU7_Msk                    (0x1U << PWM_IDR2_CMPU7_Pos)                   /**< (PWM_IDR2) Comparison 7 Update Interrupt Disable Mask */
#define PWM_IDR2_CMPU7(value)                 (PWM_IDR2_CMPU7_Msk & ((value) << PWM_IDR2_CMPU7_Pos))
#define PWM_IDR2_Msk                          (0x00FFFF09UL)                                 /**< (PWM_IDR2) Register Mask  */

/* -------- PWM_IMR2 : (PWM Offset: 0x3C) (R/  32) PWM Interrupt Mask Register 2 -------- */
#define PWM_IMR2_WRDY_Pos                     (0U)                                           /**< (PWM_IMR2) Write Ready for Synchronous Channels Update Interrupt Mask Position */
#define PWM_IMR2_WRDY_Msk                     (0x1U << PWM_IMR2_WRDY_Pos)                    /**< (PWM_IMR2) Write Ready for Synchronous Channels Update Interrupt Mask Mask */
#define PWM_IMR2_WRDY(value)                  (PWM_IMR2_WRDY_Msk & ((value) << PWM_IMR2_WRDY_Pos))
#define PWM_IMR2_UNRE_Pos                     (3U)                                           /**< (PWM_IMR2) Synchronous Channels Update Underrun Error Interrupt Mask Position */
#define PWM_IMR2_UNRE_Msk                     (0x1U << PWM_IMR2_UNRE_Pos)                    /**< (PWM_IMR2) Synchronous Channels Update Underrun Error Interrupt Mask Mask */
#define PWM_IMR2_UNRE(value)                  (PWM_IMR2_UNRE_Msk & ((value) << PWM_IMR2_UNRE_Pos))
#define PWM_IMR2_CMPM0_Pos                    (8U)                                           /**< (PWM_IMR2) Comparison 0 Match Interrupt Mask Position */
#define PWM_IMR2_CMPM0_Msk                    (0x1U << PWM_IMR2_CMPM0_Pos)                   /**< (PWM_IMR2) Comparison 0 Match Interrupt Mask Mask */
#define PWM_IMR2_CMPM0(value)                 (PWM_IMR2_CMPM0_Msk & ((value) << PWM_IMR2_CMPM0_Pos))
#define PWM_IMR2_CMPM1_Pos                    (9U)                                           /**< (PWM_IMR2) Comparison 1 Match Interrupt Mask Position */
#define PWM_IMR2_CMPM1_Msk                    (0x1U << PWM_IMR2_CMPM1_Pos)                   /**< (PWM_IMR2) Comparison 1 Match Interrupt Mask Mask */
#define PWM_IMR2_CMPM1(value)                 (PWM_IMR2_CMPM1_Msk & ((value) << PWM_IMR2_CMPM1_Pos))
#define PWM_IMR2_CMPM2_Pos                    (10U)                                          /**< (PWM_IMR2) Comparison 2 Match Interrupt Mask Position */
#define PWM_IMR2_CMPM2_Msk                    (0x1U << PWM_IMR2_CMPM2_Pos)                   /**< (PWM_IMR2) Comparison 2 Match Interrupt Mask Mask */
#define PWM_IMR2_CMPM2(value)                 (PWM_IMR2_CMPM2_Msk & ((value) << PWM_IMR2_CMPM2_Pos))
#define PWM_IMR2_CMPM3_Pos                    (11U)                                          /**< (PWM_IMR2) Comparison 3 Match Interrupt Mask Position */
#define PWM_IMR2_CMPM3_Msk                    (0x1U << PWM_IMR2_CMPM3_Pos)                   /**< (PWM_IMR2) Comparison 3 Match Interrupt Mask Mask */
#define PWM_IMR2_CMPM3(value)                 (PWM_IMR2_CMPM3_Msk & ((value) << PWM_IMR2_CMPM3_Pos))
#define PWM_IMR2_CMPM4_Pos                    (12U)                                          /**< (PWM_IMR2) Comparison 4 Match Interrupt Mask Position */
#define PWM_IMR2_CMPM4_Msk                    (0x1U << PWM_IMR2_CMPM4_Pos)                   /**< (PWM_IMR2) Comparison 4 Match Interrupt Mask Mask */
#define PWM_IMR2_CMPM4(value)                 (PWM_IMR2_CMPM4_Msk & ((value) << PWM_IMR2_CMPM4_Pos))
#define PWM_IMR2_CMPM5_Pos                    (13U)                                          /**< (PWM_IMR2) Comparison 5 Match Interrupt Mask Position */
#define PWM_IMR2_CMPM5_Msk                    (0x1U << PWM_IMR2_CMPM5_Pos)                   /**< (PWM_IMR2) Comparison 5 Match Interrupt Mask Mask */
#define PWM_IMR2_CMPM5(value)                 (PWM_IMR2_CMPM5_Msk & ((value) << PWM_IMR2_CMPM5_Pos))
#define PWM_IMR2_CMPM6_Pos                    (14U)                                          /**< (PWM_IMR2) Comparison 6 Match Interrupt Mask Position */
#define PWM_IMR2_CMPM6_Msk                    (0x1U << PWM_IMR2_CMPM6_Pos)                   /**< (PWM_IMR2) Comparison 6 Match Interrupt Mask Mask */
#define PWM_IMR2_CMPM6(value)                 (PWM_IMR2_CMPM6_Msk & ((value) << PWM_IMR2_CMPM6_Pos))
#define PWM_IMR2_CMPM7_Pos                    (15U)                                          /**< (PWM_IMR2) Comparison 7 Match Interrupt Mask Position */
#define PWM_IMR2_CMPM7_Msk                    (0x1U << PWM_IMR2_CMPM7_Pos)                   /**< (PWM_IMR2) Comparison 7 Match Interrupt Mask Mask */
#define PWM_IMR2_CMPM7(value)                 (PWM_IMR2_CMPM7_Msk & ((value) << PWM_IMR2_CMPM7_Pos))
#define PWM_IMR2_CMPU0_Pos                    (16U)                                          /**< (PWM_IMR2) Comparison 0 Update Interrupt Mask Position */
#define PWM_IMR2_CMPU0_Msk                    (0x1U << PWM_IMR2_CMPU0_Pos)                   /**< (PWM_IMR2) Comparison 0 Update Interrupt Mask Mask */
#define PWM_IMR2_CMPU0(value)                 (PWM_IMR2_CMPU0_Msk & ((value) << PWM_IMR2_CMPU0_Pos))
#define PWM_IMR2_CMPU1_Pos                    (17U)                                          /**< (PWM_IMR2) Comparison 1 Update Interrupt Mask Position */
#define PWM_IMR2_CMPU1_Msk                    (0x1U << PWM_IMR2_CMPU1_Pos)                   /**< (PWM_IMR2) Comparison 1 Update Interrupt Mask Mask */
#define PWM_IMR2_CMPU1(value)                 (PWM_IMR2_CMPU1_Msk & ((value) << PWM_IMR2_CMPU1_Pos))
#define PWM_IMR2_CMPU2_Pos                    (18U)                                          /**< (PWM_IMR2) Comparison 2 Update Interrupt Mask Position */
#define PWM_IMR2_CMPU2_Msk                    (0x1U << PWM_IMR2_CMPU2_Pos)                   /**< (PWM_IMR2) Comparison 2 Update Interrupt Mask Mask */
#define PWM_IMR2_CMPU2(value)                 (PWM_IMR2_CMPU2_Msk & ((value) << PWM_IMR2_CMPU2_Pos))
#define PWM_IMR2_CMPU3_Pos                    (19U)                                          /**< (PWM_IMR2) Comparison 3 Update Interrupt Mask Position */
#define PWM_IMR2_CMPU3_Msk                    (0x1U << PWM_IMR2_CMPU3_Pos)                   /**< (PWM_IMR2) Comparison 3 Update Interrupt Mask Mask */
#define PWM_IMR2_CMPU3(value)                 (PWM_IMR2_CMPU3_Msk & ((value) << PWM_IMR2_CMPU3_Pos))
#define PWM_IMR2_CMPU4_Pos                    (20U)                                          /**< (PWM_IMR2) Comparison 4 Update Interrupt Mask Position */
#define PWM_IMR2_CMPU4_Msk                    (0x1U << PWM_IMR2_CMPU4_Pos)                   /**< (PWM_IMR2) Comparison 4 Update Interrupt Mask Mask */
#define PWM_IMR2_CMPU4(value)                 (PWM_IMR2_CMPU4_Msk & ((value) << PWM_IMR2_CMPU4_Pos))
#define PWM_IMR2_CMPU5_Pos                    (21U)                                          /**< (PWM_IMR2) Comparison 5 Update Interrupt Mask Position */
#define PWM_IMR2_CMPU5_Msk                    (0x1U << PWM_IMR2_CMPU5_Pos)                   /**< (PWM_IMR2) Comparison 5 Update Interrupt Mask Mask */
#define PWM_IMR2_CMPU5(value)                 (PWM_IMR2_CMPU5_Msk & ((value) << PWM_IMR2_CMPU5_Pos))
#define PWM_IMR2_CMPU6_Pos                    (22U)                                          /**< (PWM_IMR2) Comparison 6 Update Interrupt Mask Position */
#define PWM_IMR2_CMPU6_Msk                    (0x1U << PWM_IMR2_CMPU6_Pos)                   /**< (PWM_IMR2) Comparison 6 Update Interrupt Mask Mask */
#define PWM_IMR2_CMPU6(value)                 (PWM_IMR2_CMPU6_Msk & ((value) << PWM_IMR2_CMPU6_Pos))
#define PWM_IMR2_CMPU7_Pos                    (23U)                                          /**< (PWM_IMR2) Comparison 7 Update Interrupt Mask Position */
#define PWM_IMR2_CMPU7_Msk                    (0x1U << PWM_IMR2_CMPU7_Pos)                   /**< (PWM_IMR2) Comparison 7 Update Interrupt Mask Mask */
#define PWM_IMR2_CMPU7(value)                 (PWM_IMR2_CMPU7_Msk & ((value) << PWM_IMR2_CMPU7_Pos))
#define PWM_IMR2_Msk                          (0x00FFFF09UL)                                 /**< (PWM_IMR2) Register Mask  */

/* -------- PWM_ISR2 : (PWM Offset: 0x40) (R/  32) PWM Interrupt Status Register 2 -------- */
#define PWM_ISR2_WRDY_Pos                     (0U)                                           /**< (PWM_ISR2) Write Ready for Synchronous Channels Update Position */
#define PWM_ISR2_WRDY_Msk                     (0x1U << PWM_ISR2_WRDY_Pos)                    /**< (PWM_ISR2) Write Ready for Synchronous Channels Update Mask */
#define PWM_ISR2_WRDY(value)                  (PWM_ISR2_WRDY_Msk & ((value) << PWM_ISR2_WRDY_Pos))
#define PWM_ISR2_UNRE_Pos                     (3U)                                           /**< (PWM_ISR2) Synchronous Channels Update Underrun Error Position */
#define PWM_ISR2_UNRE_Msk                     (0x1U << PWM_ISR2_UNRE_Pos)                    /**< (PWM_ISR2) Synchronous Channels Update Underrun Error Mask */
#define PWM_ISR2_UNRE(value)                  (PWM_ISR2_UNRE_Msk & ((value) << PWM_ISR2_UNRE_Pos))
#define PWM_ISR2_CMPM0_Pos                    (8U)                                           /**< (PWM_ISR2) Comparison 0 Match Position */
#define PWM_ISR2_CMPM0_Msk                    (0x1U << PWM_ISR2_CMPM0_Pos)                   /**< (PWM_ISR2) Comparison 0 Match Mask */
#define PWM_ISR2_CMPM0(value)                 (PWM_ISR2_CMPM0_Msk & ((value) << PWM_ISR2_CMPM0_Pos))
#define PWM_ISR2_CMPM1_Pos                    (9U)                                           /**< (PWM_ISR2) Comparison 1 Match Position */
#define PWM_ISR2_CMPM1_Msk                    (0x1U << PWM_ISR2_CMPM1_Pos)                   /**< (PWM_ISR2) Comparison 1 Match Mask */
#define PWM_ISR2_CMPM1(value)                 (PWM_ISR2_CMPM1_Msk & ((value) << PWM_ISR2_CMPM1_Pos))
#define PWM_ISR2_CMPM2_Pos                    (10U)                                          /**< (PWM_ISR2) Comparison 2 Match Position */
#define PWM_ISR2_CMPM2_Msk                    (0x1U << PWM_ISR2_CMPM2_Pos)                   /**< (PWM_ISR2) Comparison 2 Match Mask */
#define PWM_ISR2_CMPM2(value)                 (PWM_ISR2_CMPM2_Msk & ((value) << PWM_ISR2_CMPM2_Pos))
#define PWM_ISR2_CMPM3_Pos                    (11U)                                          /**< (PWM_ISR2) Comparison 3 Match Position */
#define PWM_ISR2_CMPM3_Msk                    (0x1U << PWM_ISR2_CMPM3_Pos)                   /**< (PWM_ISR2) Comparison 3 Match Mask */
#define PWM_ISR2_CMPM3(value)                 (PWM_ISR2_CMPM3_Msk & ((value) << PWM_ISR2_CMPM3_Pos))
#define PWM_ISR2_CMPM4_Pos                    (12U)                                          /**< (PWM_ISR2) Comparison 4 Match Position */
#define PWM_ISR2_CMPM4_Msk                    (0x1U << PWM_ISR2_CMPM4_Pos)                   /**< (PWM_ISR2) Comparison 4 Match Mask */
#define PWM_ISR2_CMPM4(value)                 (PWM_ISR2_CMPM4_Msk & ((value) << PWM_ISR2_CMPM4_Pos))
#define PWM_ISR2_CMPM5_Pos                    (13U)                                          /**< (PWM_ISR2) Comparison 5 Match Position */
#define PWM_ISR2_CMPM5_Msk                    (0x1U << PWM_ISR2_CMPM5_Pos)                   /**< (PWM_ISR2) Comparison 5 Match Mask */
#define PWM_ISR2_CMPM5(value)                 (PWM_ISR2_CMPM5_Msk & ((value) << PWM_ISR2_CMPM5_Pos))
#define PWM_ISR2_CMPM6_Pos                    (14U)                                          /**< (PWM_ISR2) Comparison 6 Match Position */
#define PWM_ISR2_CMPM6_Msk                    (0x1U << PWM_ISR2_CMPM6_Pos)                   /**< (PWM_ISR2) Comparison 6 Match Mask */
#define PWM_ISR2_CMPM6(value)                 (PWM_ISR2_CMPM6_Msk & ((value) << PWM_ISR2_CMPM6_Pos))
#define PWM_ISR2_CMPM7_Pos                    (15U)                                          /**< (PWM_ISR2) Comparison 7 Match Position */
#define PWM_ISR2_CMPM7_Msk                    (0x1U << PWM_ISR2_CMPM7_Pos)                   /**< (PWM_ISR2) Comparison 7 Match Mask */
#define PWM_ISR2_CMPM7(value)                 (PWM_ISR2_CMPM7_Msk & ((value) << PWM_ISR2_CMPM7_Pos))
#define PWM_ISR2_CMPU0_Pos                    (16U)                                          /**< (PWM_ISR2) Comparison 0 Update Position */
#define PWM_ISR2_CMPU0_Msk                    (0x1U << PWM_ISR2_CMPU0_Pos)                   /**< (PWM_ISR2) Comparison 0 Update Mask */
#define PWM_ISR2_CMPU0(value)                 (PWM_ISR2_CMPU0_Msk & ((value) << PWM_ISR2_CMPU0_Pos))
#define PWM_ISR2_CMPU1_Pos                    (17U)                                          /**< (PWM_ISR2) Comparison 1 Update Position */
#define PWM_ISR2_CMPU1_Msk                    (0x1U << PWM_ISR2_CMPU1_Pos)                   /**< (PWM_ISR2) Comparison 1 Update Mask */
#define PWM_ISR2_CMPU1(value)                 (PWM_ISR2_CMPU1_Msk & ((value) << PWM_ISR2_CMPU1_Pos))
#define PWM_ISR2_CMPU2_Pos                    (18U)                                          /**< (PWM_ISR2) Comparison 2 Update Position */
#define PWM_ISR2_CMPU2_Msk                    (0x1U << PWM_ISR2_CMPU2_Pos)                   /**< (PWM_ISR2) Comparison 2 Update Mask */
#define PWM_ISR2_CMPU2(value)                 (PWM_ISR2_CMPU2_Msk & ((value) << PWM_ISR2_CMPU2_Pos))
#define PWM_ISR2_CMPU3_Pos                    (19U)                                          /**< (PWM_ISR2) Comparison 3 Update Position */
#define PWM_ISR2_CMPU3_Msk                    (0x1U << PWM_ISR2_CMPU3_Pos)                   /**< (PWM_ISR2) Comparison 3 Update Mask */
#define PWM_ISR2_CMPU3(value)                 (PWM_ISR2_CMPU3_Msk & ((value) << PWM_ISR2_CMPU3_Pos))
#define PWM_ISR2_CMPU4_Pos                    (20U)                                          /**< (PWM_ISR2) Comparison 4 Update Position */
#define PWM_ISR2_CMPU4_Msk                    (0x1U << PWM_ISR2_CMPU4_Pos)                   /**< (PWM_ISR2) Comparison 4 Update Mask */
#define PWM_ISR2_CMPU4(value)                 (PWM_ISR2_CMPU4_Msk & ((value) << PWM_ISR2_CMPU4_Pos))
#define PWM_ISR2_CMPU5_Pos                    (21U)                                          /**< (PWM_ISR2) Comparison 5 Update Position */
#define PWM_ISR2_CMPU5_Msk                    (0x1U << PWM_ISR2_CMPU5_Pos)                   /**< (PWM_ISR2) Comparison 5 Update Mask */
#define PWM_ISR2_CMPU5(value)                 (PWM_ISR2_CMPU5_Msk & ((value) << PWM_ISR2_CMPU5_Pos))
#define PWM_ISR2_CMPU6_Pos                    (22U)                                          /**< (PWM_ISR2) Comparison 6 Update Position */
#define PWM_ISR2_CMPU6_Msk                    (0x1U << PWM_ISR2_CMPU6_Pos)                   /**< (PWM_ISR2) Comparison 6 Update Mask */
#define PWM_ISR2_CMPU6(value)                 (PWM_ISR2_CMPU6_Msk & ((value) << PWM_ISR2_CMPU6_Pos))
#define PWM_ISR2_CMPU7_Pos                    (23U)                                          /**< (PWM_ISR2) Comparison 7 Update Position */
#define PWM_ISR2_CMPU7_Msk                    (0x1U << PWM_ISR2_CMPU7_Pos)                   /**< (PWM_ISR2) Comparison 7 Update Mask */
#define PWM_ISR2_CMPU7(value)                 (PWM_ISR2_CMPU7_Msk & ((value) << PWM_ISR2_CMPU7_Pos))
#define PWM_ISR2_Msk                          (0x00FFFF09UL)                                 /**< (PWM_ISR2) Register Mask  */

/* -------- PWM_OOV : (PWM Offset: 0x44) (R/W 32) PWM Output Override Value Register -------- */
#define PWM_OOV_OOVH0_Pos                     (0U)                                           /**< (PWM_OOV) Output Override Value for PWMH output of the channel 0 Position */
#define PWM_OOV_OOVH0_Msk                     (0x1U << PWM_OOV_OOVH0_Pos)                    /**< (PWM_OOV) Output Override Value for PWMH output of the channel 0 Mask */
#define PWM_OOV_OOVH0(value)                  (PWM_OOV_OOVH0_Msk & ((value) << PWM_OOV_OOVH0_Pos))
#define PWM_OOV_OOVH1_Pos                     (1U)                                           /**< (PWM_OOV) Output Override Value for PWMH output of the channel 1 Position */
#define PWM_OOV_OOVH1_Msk                     (0x1U << PWM_OOV_OOVH1_Pos)                    /**< (PWM_OOV) Output Override Value for PWMH output of the channel 1 Mask */
#define PWM_OOV_OOVH1(value)                  (PWM_OOV_OOVH1_Msk & ((value) << PWM_OOV_OOVH1_Pos))
#define PWM_OOV_OOVH2_Pos                     (2U)                                           /**< (PWM_OOV) Output Override Value for PWMH output of the channel 2 Position */
#define PWM_OOV_OOVH2_Msk                     (0x1U << PWM_OOV_OOVH2_Pos)                    /**< (PWM_OOV) Output Override Value for PWMH output of the channel 2 Mask */
#define PWM_OOV_OOVH2(value)                  (PWM_OOV_OOVH2_Msk & ((value) << PWM_OOV_OOVH2_Pos))
#define PWM_OOV_OOVH3_Pos                     (3U)                                           /**< (PWM_OOV) Output Override Value for PWMH output of the channel 3 Position */
#define PWM_OOV_OOVH3_Msk                     (0x1U << PWM_OOV_OOVH3_Pos)                    /**< (PWM_OOV) Output Override Value for PWMH output of the channel 3 Mask */
#define PWM_OOV_OOVH3(value)                  (PWM_OOV_OOVH3_Msk & ((value) << PWM_OOV_OOVH3_Pos))
#define PWM_OOV_OOVL0_Pos                     (16U)                                          /**< (PWM_OOV) Output Override Value for PWML output of the channel 0 Position */
#define PWM_OOV_OOVL0_Msk                     (0x1U << PWM_OOV_OOVL0_Pos)                    /**< (PWM_OOV) Output Override Value for PWML output of the channel 0 Mask */
#define PWM_OOV_OOVL0(value)                  (PWM_OOV_OOVL0_Msk & ((value) << PWM_OOV_OOVL0_Pos))
#define PWM_OOV_OOVL1_Pos                     (17U)                                          /**< (PWM_OOV) Output Override Value for PWML output of the channel 1 Position */
#define PWM_OOV_OOVL1_Msk                     (0x1U << PWM_OOV_OOVL1_Pos)                    /**< (PWM_OOV) Output Override Value for PWML output of the channel 1 Mask */
#define PWM_OOV_OOVL1(value)                  (PWM_OOV_OOVL1_Msk & ((value) << PWM_OOV_OOVL1_Pos))
#define PWM_OOV_OOVL2_Pos                     (18U)                                          /**< (PWM_OOV) Output Override Value for PWML output of the channel 2 Position */
#define PWM_OOV_OOVL2_Msk                     (0x1U << PWM_OOV_OOVL2_Pos)                    /**< (PWM_OOV) Output Override Value for PWML output of the channel 2 Mask */
#define PWM_OOV_OOVL2(value)                  (PWM_OOV_OOVL2_Msk & ((value) << PWM_OOV_OOVL2_Pos))
#define PWM_OOV_OOVL3_Pos                     (19U)                                          /**< (PWM_OOV) Output Override Value for PWML output of the channel 3 Position */
#define PWM_OOV_OOVL3_Msk                     (0x1U << PWM_OOV_OOVL3_Pos)                    /**< (PWM_OOV) Output Override Value for PWML output of the channel 3 Mask */
#define PWM_OOV_OOVL3(value)                  (PWM_OOV_OOVL3_Msk & ((value) << PWM_OOV_OOVL3_Pos))
#define PWM_OOV_Msk                           (0x000F000FUL)                                 /**< (PWM_OOV) Register Mask  */

/* -------- PWM_OS : (PWM Offset: 0x48) (R/W 32) PWM Output Selection Register -------- */
#define PWM_OS_OSH0_Pos                       (0U)                                           /**< (PWM_OS) Output Selection for PWMH output of the channel 0 Position */
#define PWM_OS_OSH0_Msk                       (0x1U << PWM_OS_OSH0_Pos)                      /**< (PWM_OS) Output Selection for PWMH output of the channel 0 Mask */
#define PWM_OS_OSH0(value)                    (PWM_OS_OSH0_Msk & ((value) << PWM_OS_OSH0_Pos))
#define PWM_OS_OSH1_Pos                       (1U)                                           /**< (PWM_OS) Output Selection for PWMH output of the channel 1 Position */
#define PWM_OS_OSH1_Msk                       (0x1U << PWM_OS_OSH1_Pos)                      /**< (PWM_OS) Output Selection for PWMH output of the channel 1 Mask */
#define PWM_OS_OSH1(value)                    (PWM_OS_OSH1_Msk & ((value) << PWM_OS_OSH1_Pos))
#define PWM_OS_OSH2_Pos                       (2U)                                           /**< (PWM_OS) Output Selection for PWMH output of the channel 2 Position */
#define PWM_OS_OSH2_Msk                       (0x1U << PWM_OS_OSH2_Pos)                      /**< (PWM_OS) Output Selection for PWMH output of the channel 2 Mask */
#define PWM_OS_OSH2(value)                    (PWM_OS_OSH2_Msk & ((value) << PWM_OS_OSH2_Pos))
#define PWM_OS_OSH3_Pos                       (3U)                                           /**< (PWM_OS) Output Selection for PWMH output of the channel 3 Position */
#define PWM_OS_OSH3_Msk                       (0x1U << PWM_OS_OSH3_Pos)                      /**< (PWM_OS) Output Selection for PWMH output of the channel 3 Mask */
#define PWM_OS_OSH3(value)                    (PWM_OS_OSH3_Msk & ((value) << PWM_OS_OSH3_Pos))
#define PWM_OS_OSL0_Pos                       (16U)                                          /**< (PWM_OS) Output Selection for PWML output of the channel 0 Position */
#define PWM_OS_OSL0_Msk                       (0x1U << PWM_OS_OSL0_Pos)                      /**< (PWM_OS) Output Selection for PWML output of the channel 0 Mask */
#define PWM_OS_OSL0(value)                    (PWM_OS_OSL0_Msk & ((value) << PWM_OS_OSL0_Pos))
#define PWM_OS_OSL1_Pos                       (17U)                                          /**< (PWM_OS) Output Selection for PWML output of the channel 1 Position */
#define PWM_OS_OSL1_Msk                       (0x1U << PWM_OS_OSL1_Pos)                      /**< (PWM_OS) Output Selection for PWML output of the channel 1 Mask */
#define PWM_OS_OSL1(value)                    (PWM_OS_OSL1_Msk & ((value) << PWM_OS_OSL1_Pos))
#define PWM_OS_OSL2_Pos                       (18U)                                          /**< (PWM_OS) Output Selection for PWML output of the channel 2 Position */
#define PWM_OS_OSL2_Msk                       (0x1U << PWM_OS_OSL2_Pos)                      /**< (PWM_OS) Output Selection for PWML output of the channel 2 Mask */
#define PWM_OS_OSL2(value)                    (PWM_OS_OSL2_Msk & ((value) << PWM_OS_OSL2_Pos))
#define PWM_OS_OSL3_Pos                       (19U)                                          /**< (PWM_OS) Output Selection for PWML output of the channel 3 Position */
#define PWM_OS_OSL3_Msk                       (0x1U << PWM_OS_OSL3_Pos)                      /**< (PWM_OS) Output Selection for PWML output of the channel 3 Mask */
#define PWM_OS_OSL3(value)                    (PWM_OS_OSL3_Msk & ((value) << PWM_OS_OSL3_Pos))
#define PWM_OS_Msk                            (0x000F000FUL)                                 /**< (PWM_OS) Register Mask  */

/* -------- PWM_OSS : (PWM Offset: 0x4C) ( /W 32) PWM Output Selection Set Register -------- */
#define PWM_OSS_OSSH0_Pos                     (0U)                                           /**< (PWM_OSS) Output Selection Set for PWMH output of the channel 0 Position */
#define PWM_OSS_OSSH0_Msk                     (0x1U << PWM_OSS_OSSH0_Pos)                    /**< (PWM_OSS) Output Selection Set for PWMH output of the channel 0 Mask */
#define PWM_OSS_OSSH0(value)                  (PWM_OSS_OSSH0_Msk & ((value) << PWM_OSS_OSSH0_Pos))
#define PWM_OSS_OSSH1_Pos                     (1U)                                           /**< (PWM_OSS) Output Selection Set for PWMH output of the channel 1 Position */
#define PWM_OSS_OSSH1_Msk                     (0x1U << PWM_OSS_OSSH1_Pos)                    /**< (PWM_OSS) Output Selection Set for PWMH output of the channel 1 Mask */
#define PWM_OSS_OSSH1(value)                  (PWM_OSS_OSSH1_Msk & ((value) << PWM_OSS_OSSH1_Pos))
#define PWM_OSS_OSSH2_Pos                     (2U)                                           /**< (PWM_OSS) Output Selection Set for PWMH output of the channel 2 Position */
#define PWM_OSS_OSSH2_Msk                     (0x1U << PWM_OSS_OSSH2_Pos)                    /**< (PWM_OSS) Output Selection Set for PWMH output of the channel 2 Mask */
#define PWM_OSS_OSSH2(value)                  (PWM_OSS_OSSH2_Msk & ((value) << PWM_OSS_OSSH2_Pos))
#define PWM_OSS_OSSH3_Pos                     (3U)                                           /**< (PWM_OSS) Output Selection Set for PWMH output of the channel 3 Position */
#define PWM_OSS_OSSH3_Msk                     (0x1U << PWM_OSS_OSSH3_Pos)                    /**< (PWM_OSS) Output Selection Set for PWMH output of the channel 3 Mask */
#define PWM_OSS_OSSH3(value)                  (PWM_OSS_OSSH3_Msk & ((value) << PWM_OSS_OSSH3_Pos))
#define PWM_OSS_OSSL0_Pos                     (16U)                                          /**< (PWM_OSS) Output Selection Set for PWML output of the channel 0 Position */
#define PWM_OSS_OSSL0_Msk                     (0x1U << PWM_OSS_OSSL0_Pos)                    /**< (PWM_OSS) Output Selection Set for PWML output of the channel 0 Mask */
#define PWM_OSS_OSSL0(value)                  (PWM_OSS_OSSL0_Msk & ((value) << PWM_OSS_OSSL0_Pos))
#define PWM_OSS_OSSL1_Pos                     (17U)                                          /**< (PWM_OSS) Output Selection Set for PWML output of the channel 1 Position */
#define PWM_OSS_OSSL1_Msk                     (0x1U << PWM_OSS_OSSL1_Pos)                    /**< (PWM_OSS) Output Selection Set for PWML output of the channel 1 Mask */
#define PWM_OSS_OSSL1(value)                  (PWM_OSS_OSSL1_Msk & ((value) << PWM_OSS_OSSL1_Pos))
#define PWM_OSS_OSSL2_Pos                     (18U)                                          /**< (PWM_OSS) Output Selection Set for PWML output of the channel 2 Position */
#define PWM_OSS_OSSL2_Msk                     (0x1U << PWM_OSS_OSSL2_Pos)                    /**< (PWM_OSS) Output Selection Set for PWML output of the channel 2 Mask */
#define PWM_OSS_OSSL2(value)                  (PWM_OSS_OSSL2_Msk & ((value) << PWM_OSS_OSSL2_Pos))
#define PWM_OSS_OSSL3_Pos                     (19U)                                          /**< (PWM_OSS) Output Selection Set for PWML output of the channel 3 Position */
#define PWM_OSS_OSSL3_Msk                     (0x1U << PWM_OSS_OSSL3_Pos)                    /**< (PWM_OSS) Output Selection Set for PWML output of the channel 3 Mask */
#define PWM_OSS_OSSL3(value)                  (PWM_OSS_OSSL3_Msk & ((value) << PWM_OSS_OSSL3_Pos))
#define PWM_OSS_Msk                           (0x000F000FUL)                                 /**< (PWM_OSS) Register Mask  */

/* -------- PWM_OSC : (PWM Offset: 0x50) ( /W 32) PWM Output Selection Clear Register -------- */
#define PWM_OSC_OSCH0_Pos                     (0U)                                           /**< (PWM_OSC) Output Selection Clear for PWMH output of the channel 0 Position */
#define PWM_OSC_OSCH0_Msk                     (0x1U << PWM_OSC_OSCH0_Pos)                    /**< (PWM_OSC) Output Selection Clear for PWMH output of the channel 0 Mask */
#define PWM_OSC_OSCH0(value)                  (PWM_OSC_OSCH0_Msk & ((value) << PWM_OSC_OSCH0_Pos))
#define PWM_OSC_OSCH1_Pos                     (1U)                                           /**< (PWM_OSC) Output Selection Clear for PWMH output of the channel 1 Position */
#define PWM_OSC_OSCH1_Msk                     (0x1U << PWM_OSC_OSCH1_Pos)                    /**< (PWM_OSC) Output Selection Clear for PWMH output of the channel 1 Mask */
#define PWM_OSC_OSCH1(value)                  (PWM_OSC_OSCH1_Msk & ((value) << PWM_OSC_OSCH1_Pos))
#define PWM_OSC_OSCH2_Pos                     (2U)                                           /**< (PWM_OSC) Output Selection Clear for PWMH output of the channel 2 Position */
#define PWM_OSC_OSCH2_Msk                     (0x1U << PWM_OSC_OSCH2_Pos)                    /**< (PWM_OSC) Output Selection Clear for PWMH output of the channel 2 Mask */
#define PWM_OSC_OSCH2(value)                  (PWM_OSC_OSCH2_Msk & ((value) << PWM_OSC_OSCH2_Pos))
#define PWM_OSC_OSCH3_Pos                     (3U)                                           /**< (PWM_OSC) Output Selection Clear for PWMH output of the channel 3 Position */
#define PWM_OSC_OSCH3_Msk                     (0x1U << PWM_OSC_OSCH3_Pos)                    /**< (PWM_OSC) Output Selection Clear for PWMH output of the channel 3 Mask */
#define PWM_OSC_OSCH3(value)                  (PWM_OSC_OSCH3_Msk & ((value) << PWM_OSC_OSCH3_Pos))
#define PWM_OSC_OSCL0_Pos                     (16U)                                          /**< (PWM_OSC) Output Selection Clear for PWML output of the channel 0 Position */
#define PWM_OSC_OSCL0_Msk                     (0x1U << PWM_OSC_OSCL0_Pos)                    /**< (PWM_OSC) Output Selection Clear for PWML output of the channel 0 Mask */
#define PWM_OSC_OSCL0(value)                  (PWM_OSC_OSCL0_Msk & ((value) << PWM_OSC_OSCL0_Pos))
#define PWM_OSC_OSCL1_Pos                     (17U)                                          /**< (PWM_OSC) Output Selection Clear for PWML output of the channel 1 Position */
#define PWM_OSC_OSCL1_Msk                     (0x1U << PWM_OSC_OSCL1_Pos)                    /**< (PWM_OSC) Output Selection Clear for PWML output of the channel 1 Mask */
#define PWM_OSC_OSCL1(value)                  (PWM_OSC_OSCL1_Msk & ((value) << PWM_OSC_OSCL1_Pos))
#define PWM_OSC_OSCL2_Pos                     (18U)                                          /**< (PWM_OSC) Output Selection Clear for PWML output of the channel 2 Position */
#define PWM_OSC_OSCL2_Msk                     (0x1U << PWM_OSC_OSCL2_Pos)                    /**< (PWM_OSC) Output Selection Clear for PWML output of the channel 2 Mask */
#define PWM_OSC_OSCL2(value)                  (PWM_OSC_OSCL2_Msk & ((value) << PWM_OSC_OSCL2_Pos))
#define PWM_OSC_OSCL3_Pos                     (19U)                                          /**< (PWM_OSC) Output Selection Clear for PWML output of the channel 3 Position */
#define PWM_OSC_OSCL3_Msk                     (0x1U << PWM_OSC_OSCL3_Pos)                    /**< (PWM_OSC) Output Selection Clear for PWML output of the channel 3 Mask */
#define PWM_OSC_OSCL3(value)                  (PWM_OSC_OSCL3_Msk & ((value) << PWM_OSC_OSCL3_Pos))
#define PWM_OSC_Msk                           (0x000F000FUL)                                 /**< (PWM_OSC) Register Mask  */

/* -------- PWM_OSSUPD : (PWM Offset: 0x54) ( /W 32) PWM Output Selection Set Update Register -------- */
#define PWM_OSSUPD_OSSUPH0_Pos                (0U)                                           /**< (PWM_OSSUPD) Output Selection Set for PWMH output of the channel 0 Position */
#define PWM_OSSUPD_OSSUPH0_Msk                (0x1U << PWM_OSSUPD_OSSUPH0_Pos)               /**< (PWM_OSSUPD) Output Selection Set for PWMH output of the channel 0 Mask */
#define PWM_OSSUPD_OSSUPH0(value)             (PWM_OSSUPD_OSSUPH0_Msk & ((value) << PWM_OSSUPD_OSSUPH0_Pos))
#define PWM_OSSUPD_OSSUPH1_Pos                (1U)                                           /**< (PWM_OSSUPD) Output Selection Set for PWMH output of the channel 1 Position */
#define PWM_OSSUPD_OSSUPH1_Msk                (0x1U << PWM_OSSUPD_OSSUPH1_Pos)               /**< (PWM_OSSUPD) Output Selection Set for PWMH output of the channel 1 Mask */
#define PWM_OSSUPD_OSSUPH1(value)             (PWM_OSSUPD_OSSUPH1_Msk & ((value) << PWM_OSSUPD_OSSUPH1_Pos))
#define PWM_OSSUPD_OSSUPH2_Pos                (2U)                                           /**< (PWM_OSSUPD) Output Selection Set for PWMH output of the channel 2 Position */
#define PWM_OSSUPD_OSSUPH2_Msk                (0x1U << PWM_OSSUPD_OSSUPH2_Pos)               /**< (PWM_OSSUPD) Output Selection Set for PWMH output of the channel 2 Mask */
#define PWM_OSSUPD_OSSUPH2(value)             (PWM_OSSUPD_OSSUPH2_Msk & ((value) << PWM_OSSUPD_OSSUPH2_Pos))
#define PWM_OSSUPD_OSSUPH3_Pos                (3U)                                           /**< (PWM_OSSUPD) Output Selection Set for PWMH output of the channel 3 Position */
#define PWM_OSSUPD_OSSUPH3_Msk                (0x1U << PWM_OSSUPD_OSSUPH3_Pos)               /**< (PWM_OSSUPD) Output Selection Set for PWMH output of the channel 3 Mask */
#define PWM_OSSUPD_OSSUPH3(value)             (PWM_OSSUPD_OSSUPH3_Msk & ((value) << PWM_OSSUPD_OSSUPH3_Pos))
#define PWM_OSSUPD_OSSUPL0_Pos                (16U)                                          /**< (PWM_OSSUPD) Output Selection Set for PWML output of the channel 0 Position */
#define PWM_OSSUPD_OSSUPL0_Msk                (0x1U << PWM_OSSUPD_OSSUPL0_Pos)               /**< (PWM_OSSUPD) Output Selection Set for PWML output of the channel 0 Mask */
#define PWM_OSSUPD_OSSUPL0(value)             (PWM_OSSUPD_OSSUPL0_Msk & ((value) << PWM_OSSUPD_OSSUPL0_Pos))
#define PWM_OSSUPD_OSSUPL1_Pos                (17U)                                          /**< (PWM_OSSUPD) Output Selection Set for PWML output of the channel 1 Position */
#define PWM_OSSUPD_OSSUPL1_Msk                (0x1U << PWM_OSSUPD_OSSUPL1_Pos)               /**< (PWM_OSSUPD) Output Selection Set for PWML output of the channel 1 Mask */
#define PWM_OSSUPD_OSSUPL1(value)             (PWM_OSSUPD_OSSUPL1_Msk & ((value) << PWM_OSSUPD_OSSUPL1_Pos))
#define PWM_OSSUPD_OSSUPL2_Pos                (18U)                                          /**< (PWM_OSSUPD) Output Selection Set for PWML output of the channel 2 Position */
#define PWM_OSSUPD_OSSUPL2_Msk                (0x1U << PWM_OSSUPD_OSSUPL2_Pos)               /**< (PWM_OSSUPD) Output Selection Set for PWML output of the channel 2 Mask */
#define PWM_OSSUPD_OSSUPL2(value)             (PWM_OSSUPD_OSSUPL2_Msk & ((value) << PWM_OSSUPD_OSSUPL2_Pos))
#define PWM_OSSUPD_OSSUPL3_Pos                (19U)                                          /**< (PWM_OSSUPD) Output Selection Set for PWML output of the channel 3 Position */
#define PWM_OSSUPD_OSSUPL3_Msk                (0x1U << PWM_OSSUPD_OSSUPL3_Pos)               /**< (PWM_OSSUPD) Output Selection Set for PWML output of the channel 3 Mask */
#define PWM_OSSUPD_OSSUPL3(value)             (PWM_OSSUPD_OSSUPL3_Msk & ((value) << PWM_OSSUPD_OSSUPL3_Pos))
#define PWM_OSSUPD_Msk                        (0x000F000FUL)                                 /**< (PWM_OSSUPD) Register Mask  */

/* -------- PWM_OSCUPD : (PWM Offset: 0x58) ( /W 32) PWM Output Selection Clear Update Register -------- */
#define PWM_OSCUPD_OSCUPH0_Pos                (0U)                                           /**< (PWM_OSCUPD) Output Selection Clear for PWMH output of the channel 0 Position */
#define PWM_OSCUPD_OSCUPH0_Msk                (0x1U << PWM_OSCUPD_OSCUPH0_Pos)               /**< (PWM_OSCUPD) Output Selection Clear for PWMH output of the channel 0 Mask */
#define PWM_OSCUPD_OSCUPH0(value)             (PWM_OSCUPD_OSCUPH0_Msk & ((value) << PWM_OSCUPD_OSCUPH0_Pos))
#define PWM_OSCUPD_OSCUPH1_Pos                (1U)                                           /**< (PWM_OSCUPD) Output Selection Clear for PWMH output of the channel 1 Position */
#define PWM_OSCUPD_OSCUPH1_Msk                (0x1U << PWM_OSCUPD_OSCUPH1_Pos)               /**< (PWM_OSCUPD) Output Selection Clear for PWMH output of the channel 1 Mask */
#define PWM_OSCUPD_OSCUPH1(value)             (PWM_OSCUPD_OSCUPH1_Msk & ((value) << PWM_OSCUPD_OSCUPH1_Pos))
#define PWM_OSCUPD_OSCUPH2_Pos                (2U)                                           /**< (PWM_OSCUPD) Output Selection Clear for PWMH output of the channel 2 Position */
#define PWM_OSCUPD_OSCUPH2_Msk                (0x1U << PWM_OSCUPD_OSCUPH2_Pos)               /**< (PWM_OSCUPD) Output Selection Clear for PWMH output of the channel 2 Mask */
#define PWM_OSCUPD_OSCUPH2(value)             (PWM_OSCUPD_OSCUPH2_Msk & ((value) << PWM_OSCUPD_OSCUPH2_Pos))
#define PWM_OSCUPD_OSCUPH3_Pos                (3U)                                           /**< (PWM_OSCUPD) Output Selection Clear for PWMH output of the channel 3 Position */
#define PWM_OSCUPD_OSCUPH3_Msk                (0x1U << PWM_OSCUPD_OSCUPH3_Pos)               /**< (PWM_OSCUPD) Output Selection Clear for PWMH output of the channel 3 Mask */
#define PWM_OSCUPD_OSCUPH3(value)             (PWM_OSCUPD_OSCUPH3_Msk & ((value) << PWM_OSCUPD_OSCUPH3_Pos))
#define PWM_OSCUPD_OSCUPL0_Pos                (16U)                                          /**< (PWM_OSCUPD) Output Selection Clear for PWML output of the channel 0 Position */
#define PWM_OSCUPD_OSCUPL0_Msk                (0x1U << PWM_OSCUPD_OSCUPL0_Pos)               /**< (PWM_OSCUPD) Output Selection Clear for PWML output of the channel 0 Mask */
#define PWM_OSCUPD_OSCUPL0(value)             (PWM_OSCUPD_OSCUPL0_Msk & ((value) << PWM_OSCUPD_OSCUPL0_Pos))
#define PWM_OSCUPD_OSCUPL1_Pos                (17U)                                          /**< (PWM_OSCUPD) Output Selection Clear for PWML output of the channel 1 Position */
#define PWM_OSCUPD_OSCUPL1_Msk                (0x1U << PWM_OSCUPD_OSCUPL1_Pos)               /**< (PWM_OSCUPD) Output Selection Clear for PWML output of the channel 1 Mask */
#define PWM_OSCUPD_OSCUPL1(value)             (PWM_OSCUPD_OSCUPL1_Msk & ((value) << PWM_OSCUPD_OSCUPL1_Pos))
#define PWM_OSCUPD_OSCUPL2_Pos                (18U)                                          /**< (PWM_OSCUPD) Output Selection Clear for PWML output of the channel 2 Position */
#define PWM_OSCUPD_OSCUPL2_Msk                (0x1U << PWM_OSCUPD_OSCUPL2_Pos)               /**< (PWM_OSCUPD) Output Selection Clear for PWML output of the channel 2 Mask */
#define PWM_OSCUPD_OSCUPL2(value)             (PWM_OSCUPD_OSCUPL2_Msk & ((value) << PWM_OSCUPD_OSCUPL2_Pos))
#define PWM_OSCUPD_OSCUPL3_Pos                (19U)                                          /**< (PWM_OSCUPD) Output Selection Clear for PWML output of the channel 3 Position */
#define PWM_OSCUPD_OSCUPL3_Msk                (0x1U << PWM_OSCUPD_OSCUPL3_Pos)               /**< (PWM_OSCUPD) Output Selection Clear for PWML output of the channel 3 Mask */
#define PWM_OSCUPD_OSCUPL3(value)             (PWM_OSCUPD_OSCUPL3_Msk & ((value) << PWM_OSCUPD_OSCUPL3_Pos))
#define PWM_OSCUPD_Msk                        (0x000F000FUL)                                 /**< (PWM_OSCUPD) Register Mask  */

/* -------- PWM_FMR : (PWM Offset: 0x5C) (R/W 32) PWM Fault Mode Register -------- */
#define PWM_FMR_FPOL_Pos                      (0U)                                           /**< (PWM_FMR) Fault Polarity Position */
#define PWM_FMR_FPOL_Msk                      (0xFFU << PWM_FMR_FPOL_Pos)                    /**< (PWM_FMR) Fault Polarity Mask */
#define PWM_FMR_FPOL(value)                   (PWM_FMR_FPOL_Msk & ((value) << PWM_FMR_FPOL_Pos))
#define PWM_FMR_FMOD_Pos                      (8U)                                           /**< (PWM_FMR) Fault Activation Mode Position */
#define PWM_FMR_FMOD_Msk                      (0xFFU << PWM_FMR_FMOD_Pos)                    /**< (PWM_FMR) Fault Activation Mode Mask */
#define PWM_FMR_FMOD(value)                   (PWM_FMR_FMOD_Msk & ((value) << PWM_FMR_FMOD_Pos))
#define PWM_FMR_FFIL_Pos                      (16U)                                          /**< (PWM_FMR) Fault Filtering Position */
#define PWM_FMR_FFIL_Msk                      (0xFFU << PWM_FMR_FFIL_Pos)                    /**< (PWM_FMR) Fault Filtering Mask */
#define PWM_FMR_FFIL(value)                   (PWM_FMR_FFIL_Msk & ((value) << PWM_FMR_FFIL_Pos))
#define PWM_FMR_Msk                           (0x00FFFFFFUL)                                 /**< (PWM_FMR) Register Mask  */

/* -------- PWM_FSR : (PWM Offset: 0x60) (R/  32) PWM Fault Status Register -------- */
#define PWM_FSR_FIV_Pos                       (0U)                                           /**< (PWM_FSR) Fault Input Value Position */
#define PWM_FSR_FIV_Msk                       (0xFFU << PWM_FSR_FIV_Pos)                     /**< (PWM_FSR) Fault Input Value Mask */
#define PWM_FSR_FIV(value)                    (PWM_FSR_FIV_Msk & ((value) << PWM_FSR_FIV_Pos))
#define PWM_FSR_FS_Pos                        (8U)                                           /**< (PWM_FSR) Fault Status Position */
#define PWM_FSR_FS_Msk                        (0xFFU << PWM_FSR_FS_Pos)                      /**< (PWM_FSR) Fault Status Mask */
#define PWM_FSR_FS(value)                     (PWM_FSR_FS_Msk & ((value) << PWM_FSR_FS_Pos))
#define PWM_FSR_Msk                           (0x0000FFFFUL)                                 /**< (PWM_FSR) Register Mask  */

/* -------- PWM_FCR : (PWM Offset: 0x64) ( /W 32) PWM Fault Clear Register -------- */
#define PWM_FCR_FCLR_Pos                      (0U)                                           /**< (PWM_FCR) Fault Clear Position */
#define PWM_FCR_FCLR_Msk                      (0xFFU << PWM_FCR_FCLR_Pos)                    /**< (PWM_FCR) Fault Clear Mask */
#define PWM_FCR_FCLR(value)                   (PWM_FCR_FCLR_Msk & ((value) << PWM_FCR_FCLR_Pos))
#define PWM_FCR_Msk                           (0x000000FFUL)                                 /**< (PWM_FCR) Register Mask  */

/* -------- PWM_FPV1 : (PWM Offset: 0x68) (R/W 32) PWM Fault Protection Value Register 1 -------- */
#define PWM_FPV1_FPVH0_Pos                    (0U)                                           /**< (PWM_FPV1) Fault Protection Value for PWMH output on channel 0 Position */
#define PWM_FPV1_FPVH0_Msk                    (0x1U << PWM_FPV1_FPVH0_Pos)                   /**< (PWM_FPV1) Fault Protection Value for PWMH output on channel 0 Mask */
#define PWM_FPV1_FPVH0(value)                 (PWM_FPV1_FPVH0_Msk & ((value) << PWM_FPV1_FPVH0_Pos))
#define PWM_FPV1_FPVH1_Pos                    (1U)                                           /**< (PWM_FPV1) Fault Protection Value for PWMH output on channel 1 Position */
#define PWM_FPV1_FPVH1_Msk                    (0x1U << PWM_FPV1_FPVH1_Pos)                   /**< (PWM_FPV1) Fault Protection Value for PWMH output on channel 1 Mask */
#define PWM_FPV1_FPVH1(value)                 (PWM_FPV1_FPVH1_Msk & ((value) << PWM_FPV1_FPVH1_Pos))
#define PWM_FPV1_FPVH2_Pos                    (2U)                                           /**< (PWM_FPV1) Fault Protection Value for PWMH output on channel 2 Position */
#define PWM_FPV1_FPVH2_Msk                    (0x1U << PWM_FPV1_FPVH2_Pos)                   /**< (PWM_FPV1) Fault Protection Value for PWMH output on channel 2 Mask */
#define PWM_FPV1_FPVH2(value)                 (PWM_FPV1_FPVH2_Msk & ((value) << PWM_FPV1_FPVH2_Pos))
#define PWM_FPV1_FPVH3_Pos                    (3U)                                           /**< (PWM_FPV1) Fault Protection Value for PWMH output on channel 3 Position */
#define PWM_FPV1_FPVH3_Msk                    (0x1U << PWM_FPV1_FPVH3_Pos)                   /**< (PWM_FPV1) Fault Protection Value for PWMH output on channel 3 Mask */
#define PWM_FPV1_FPVH3(value)                 (PWM_FPV1_FPVH3_Msk & ((value) << PWM_FPV1_FPVH3_Pos))
#define PWM_FPV1_FPVL0_Pos                    (16U)                                          /**< (PWM_FPV1) Fault Protection Value for PWML output on channel 0 Position */
#define PWM_FPV1_FPVL0_Msk                    (0x1U << PWM_FPV1_FPVL0_Pos)                   /**< (PWM_FPV1) Fault Protection Value for PWML output on channel 0 Mask */
#define PWM_FPV1_FPVL0(value)                 (PWM_FPV1_FPVL0_Msk & ((value) << PWM_FPV1_FPVL0_Pos))
#define PWM_FPV1_FPVL1_Pos                    (17U)                                          /**< (PWM_FPV1) Fault Protection Value for PWML output on channel 1 Position */
#define PWM_FPV1_FPVL1_Msk                    (0x1U << PWM_FPV1_FPVL1_Pos)                   /**< (PWM_FPV1) Fault Protection Value for PWML output on channel 1 Mask */
#define PWM_FPV1_FPVL1(value)                 (PWM_FPV1_FPVL1_Msk & ((value) << PWM_FPV1_FPVL1_Pos))
#define PWM_FPV1_FPVL2_Pos                    (18U)                                          /**< (PWM_FPV1) Fault Protection Value for PWML output on channel 2 Position */
#define PWM_FPV1_FPVL2_Msk                    (0x1U << PWM_FPV1_FPVL2_Pos)                   /**< (PWM_FPV1) Fault Protection Value for PWML output on channel 2 Mask */
#define PWM_FPV1_FPVL2(value)                 (PWM_FPV1_FPVL2_Msk & ((value) << PWM_FPV1_FPVL2_Pos))
#define PWM_FPV1_FPVL3_Pos                    (19U)                                          /**< (PWM_FPV1) Fault Protection Value for PWML output on channel 3 Position */
#define PWM_FPV1_FPVL3_Msk                    (0x1U << PWM_FPV1_FPVL3_Pos)                   /**< (PWM_FPV1) Fault Protection Value for PWML output on channel 3 Mask */
#define PWM_FPV1_FPVL3(value)                 (PWM_FPV1_FPVL3_Msk & ((value) << PWM_FPV1_FPVL3_Pos))
#define PWM_FPV1_Msk                          (0x000F000FUL)                                 /**< (PWM_FPV1) Register Mask  */

/* -------- PWM_FPE : (PWM Offset: 0x6C) (R/W 32) PWM Fault Protection Enable Register -------- */
#define PWM_FPE_FPE0_Pos                      (0U)                                           /**< (PWM_FPE) Fault Protection Enable for channel 0 Position */
#define PWM_FPE_FPE0_Msk                      (0xFFU << PWM_FPE_FPE0_Pos)                    /**< (PWM_FPE) Fault Protection Enable for channel 0 Mask */
#define PWM_FPE_FPE0(value)                   (PWM_FPE_FPE0_Msk & ((value) << PWM_FPE_FPE0_Pos))
#define PWM_FPE_FPE1_Pos                      (8U)                                           /**< (PWM_FPE) Fault Protection Enable for channel 1 Position */
#define PWM_FPE_FPE1_Msk                      (0xFFU << PWM_FPE_FPE1_Pos)                    /**< (PWM_FPE) Fault Protection Enable for channel 1 Mask */
#define PWM_FPE_FPE1(value)                   (PWM_FPE_FPE1_Msk & ((value) << PWM_FPE_FPE1_Pos))
#define PWM_FPE_FPE2_Pos                      (16U)                                          /**< (PWM_FPE) Fault Protection Enable for channel 2 Position */
#define PWM_FPE_FPE2_Msk                      (0xFFU << PWM_FPE_FPE2_Pos)                    /**< (PWM_FPE) Fault Protection Enable for channel 2 Mask */
#define PWM_FPE_FPE2(value)                   (PWM_FPE_FPE2_Msk & ((value) << PWM_FPE_FPE2_Pos))
#define PWM_FPE_FPE3_Pos                      (24U)                                          /**< (PWM_FPE) Fault Protection Enable for channel 3 Position */
#define PWM_FPE_FPE3_Msk                      (0xFFU << PWM_FPE_FPE3_Pos)                    /**< (PWM_FPE) Fault Protection Enable for channel 3 Mask */
#define PWM_FPE_FPE3(value)                   (PWM_FPE_FPE3_Msk & ((value) << PWM_FPE_FPE3_Pos))
#define PWM_FPE_Msk                           (0xFFFFFFFFUL)                                 /**< (PWM_FPE) Register Mask  */

/* -------- PWM_ELMR : (PWM Offset: 0x7C) (R/W 32) PWM Event Line 0 Mode Register 0 -------- */
#define PWM_ELMR_CSEL0_Pos                    (0U)                                           /**< (PWM_ELMR) Comparison 0 Selection Position */
#define PWM_ELMR_CSEL0_Msk                    (0x1U << PWM_ELMR_CSEL0_Pos)                   /**< (PWM_ELMR) Comparison 0 Selection Mask */
#define PWM_ELMR_CSEL0(value)                 (PWM_ELMR_CSEL0_Msk & ((value) << PWM_ELMR_CSEL0_Pos))
#define PWM_ELMR_CSEL1_Pos                    (1U)                                           /**< (PWM_ELMR) Comparison 1 Selection Position */
#define PWM_ELMR_CSEL1_Msk                    (0x1U << PWM_ELMR_CSEL1_Pos)                   /**< (PWM_ELMR) Comparison 1 Selection Mask */
#define PWM_ELMR_CSEL1(value)                 (PWM_ELMR_CSEL1_Msk & ((value) << PWM_ELMR_CSEL1_Pos))
#define PWM_ELMR_CSEL2_Pos                    (2U)                                           /**< (PWM_ELMR) Comparison 2 Selection Position */
#define PWM_ELMR_CSEL2_Msk                    (0x1U << PWM_ELMR_CSEL2_Pos)                   /**< (PWM_ELMR) Comparison 2 Selection Mask */
#define PWM_ELMR_CSEL2(value)                 (PWM_ELMR_CSEL2_Msk & ((value) << PWM_ELMR_CSEL2_Pos))
#define PWM_ELMR_CSEL3_Pos                    (3U)                                           /**< (PWM_ELMR) Comparison 3 Selection Position */
#define PWM_ELMR_CSEL3_Msk                    (0x1U << PWM_ELMR_CSEL3_Pos)                   /**< (PWM_ELMR) Comparison 3 Selection Mask */
#define PWM_ELMR_CSEL3(value)                 (PWM_ELMR_CSEL3_Msk & ((value) << PWM_ELMR_CSEL3_Pos))
#define PWM_ELMR_CSEL4_Pos                    (4U)                                           /**< (PWM_ELMR) Comparison 4 Selection Position */
#define PWM_ELMR_CSEL4_Msk                    (0x1U << PWM_ELMR_CSEL4_Pos)                   /**< (PWM_ELMR) Comparison 4 Selection Mask */
#define PWM_ELMR_CSEL4(value)                 (PWM_ELMR_CSEL4_Msk & ((value) << PWM_ELMR_CSEL4_Pos))
#define PWM_ELMR_CSEL5_Pos                    (5U)                                           /**< (PWM_ELMR) Comparison 5 Selection Position */
#define PWM_ELMR_CSEL5_Msk                    (0x1U << PWM_ELMR_CSEL5_Pos)                   /**< (PWM_ELMR) Comparison 5 Selection Mask */
#define PWM_ELMR_CSEL5(value)                 (PWM_ELMR_CSEL5_Msk & ((value) << PWM_ELMR_CSEL5_Pos))
#define PWM_ELMR_CSEL6_Pos                    (6U)                                           /**< (PWM_ELMR) Comparison 6 Selection Position */
#define PWM_ELMR_CSEL6_Msk                    (0x1U << PWM_ELMR_CSEL6_Pos)                   /**< (PWM_ELMR) Comparison 6 Selection Mask */
#define PWM_ELMR_CSEL6(value)                 (PWM_ELMR_CSEL6_Msk & ((value) << PWM_ELMR_CSEL6_Pos))
#define PWM_ELMR_CSEL7_Pos                    (7U)                                           /**< (PWM_ELMR) Comparison 7 Selection Position */
#define PWM_ELMR_CSEL7_Msk                    (0x1U << PWM_ELMR_CSEL7_Pos)                   /**< (PWM_ELMR) Comparison 7 Selection Mask */
#define PWM_ELMR_CSEL7(value)                 (PWM_ELMR_CSEL7_Msk & ((value) << PWM_ELMR_CSEL7_Pos))
#define PWM_ELMR_Msk                          (0x000000FFUL)                                 /**< (PWM_ELMR) Register Mask  */

/* -------- PWM_SSPR : (PWM Offset: 0xA0) (R/W 32) PWM Spread Spectrum Register -------- */
#define PWM_SSPR_SPRD_Pos                     (0U)                                           /**< (PWM_SSPR) Spread Spectrum Limit Value Position */
#define PWM_SSPR_SPRD_Msk                     (0xFFFFFFU << PWM_SSPR_SPRD_Pos)               /**< (PWM_SSPR) Spread Spectrum Limit Value Mask */
#define PWM_SSPR_SPRD(value)                  (PWM_SSPR_SPRD_Msk & ((value) << PWM_SSPR_SPRD_Pos))
#define PWM_SSPR_SPRDM_Pos                    (24U)                                          /**< (PWM_SSPR) Spread Spectrum Counter Mode Position */
#define PWM_SSPR_SPRDM_Msk                    (0x1U << PWM_SSPR_SPRDM_Pos)                   /**< (PWM_SSPR) Spread Spectrum Counter Mode Mask */
#define PWM_SSPR_SPRDM(value)                 (PWM_SSPR_SPRDM_Msk & ((value) << PWM_SSPR_SPRDM_Pos))
#define PWM_SSPR_Msk                          (0x01FFFFFFUL)                                 /**< (PWM_SSPR) Register Mask  */

/* -------- PWM_SSPUP : (PWM Offset: 0xA4) ( /W 32) PWM Spread Spectrum Update Register -------- */
#define PWM_SSPUP_SPRDUP_Pos                  (0U)                                           /**< (PWM_SSPUP) Spread Spectrum Limit Value Update Position */
#define PWM_SSPUP_SPRDUP_Msk                  (0xFFFFFFU << PWM_SSPUP_SPRDUP_Pos)            /**< (PWM_SSPUP) Spread Spectrum Limit Value Update Mask */
#define PWM_SSPUP_SPRDUP(value)               (PWM_SSPUP_SPRDUP_Msk & ((value) << PWM_SSPUP_SPRDUP_Pos))
#define PWM_SSPUP_Msk                         (0x00FFFFFFUL)                                 /**< (PWM_SSPUP) Register Mask  */

/* -------- PWM_SMMR : (PWM Offset: 0xB0) (R/W 32) PWM Stepper Motor Mode Register -------- */
#define PWM_SMMR_GCEN0_Pos                    (0U)                                           /**< (PWM_SMMR) Gray Count ENable Position */
#define PWM_SMMR_GCEN0_Msk                    (0x1U << PWM_SMMR_GCEN0_Pos)                   /**< (PWM_SMMR) Gray Count ENable Mask */
#define PWM_SMMR_GCEN0(value)                 (PWM_SMMR_GCEN0_Msk & ((value) << PWM_SMMR_GCEN0_Pos))
#define PWM_SMMR_GCEN1_Pos                    (1U)                                           /**< (PWM_SMMR) Gray Count ENable Position */
#define PWM_SMMR_GCEN1_Msk                    (0x1U << PWM_SMMR_GCEN1_Pos)                   /**< (PWM_SMMR) Gray Count ENable Mask */
#define PWM_SMMR_GCEN1(value)                 (PWM_SMMR_GCEN1_Msk & ((value) << PWM_SMMR_GCEN1_Pos))
#define PWM_SMMR_DOWN0_Pos                    (16U)                                          /**< (PWM_SMMR) DOWN Count Position */
#define PWM_SMMR_DOWN0_Msk                    (0x1U << PWM_SMMR_DOWN0_Pos)                   /**< (PWM_SMMR) DOWN Count Mask */
#define PWM_SMMR_DOWN0(value)                 (PWM_SMMR_DOWN0_Msk & ((value) << PWM_SMMR_DOWN0_Pos))
#define PWM_SMMR_DOWN1_Pos                    (17U)                                          /**< (PWM_SMMR) DOWN Count Position */
#define PWM_SMMR_DOWN1_Msk                    (0x1U << PWM_SMMR_DOWN1_Pos)                   /**< (PWM_SMMR) DOWN Count Mask */
#define PWM_SMMR_DOWN1(value)                 (PWM_SMMR_DOWN1_Msk & ((value) << PWM_SMMR_DOWN1_Pos))
#define PWM_SMMR_Msk                          (0x00030003UL)                                 /**< (PWM_SMMR) Register Mask  */

/* -------- PWM_FPV2 : (PWM Offset: 0xC0) (R/W 32) PWM Fault Protection Value 2 Register -------- */
#define PWM_FPV2_FPZH0_Pos                    (0U)                                           /**< (PWM_FPV2) Fault Protection to Hi-Z for PWMH output on channel 0 Position */
#define PWM_FPV2_FPZH0_Msk                    (0x1U << PWM_FPV2_FPZH0_Pos)                   /**< (PWM_FPV2) Fault Protection to Hi-Z for PWMH output on channel 0 Mask */
#define PWM_FPV2_FPZH0(value)                 (PWM_FPV2_FPZH0_Msk & ((value) << PWM_FPV2_FPZH0_Pos))
#define PWM_FPV2_FPZH1_Pos                    (1U)                                           /**< (PWM_FPV2) Fault Protection to Hi-Z for PWMH output on channel 1 Position */
#define PWM_FPV2_FPZH1_Msk                    (0x1U << PWM_FPV2_FPZH1_Pos)                   /**< (PWM_FPV2) Fault Protection to Hi-Z for PWMH output on channel 1 Mask */
#define PWM_FPV2_FPZH1(value)                 (PWM_FPV2_FPZH1_Msk & ((value) << PWM_FPV2_FPZH1_Pos))
#define PWM_FPV2_FPZH2_Pos                    (2U)                                           /**< (PWM_FPV2) Fault Protection to Hi-Z for PWMH output on channel 2 Position */
#define PWM_FPV2_FPZH2_Msk                    (0x1U << PWM_FPV2_FPZH2_Pos)                   /**< (PWM_FPV2) Fault Protection to Hi-Z for PWMH output on channel 2 Mask */
#define PWM_FPV2_FPZH2(value)                 (PWM_FPV2_FPZH2_Msk & ((value) << PWM_FPV2_FPZH2_Pos))
#define PWM_FPV2_FPZH3_Pos                    (3U)                                           /**< (PWM_FPV2) Fault Protection to Hi-Z for PWMH output on channel 3 Position */
#define PWM_FPV2_FPZH3_Msk                    (0x1U << PWM_FPV2_FPZH3_Pos)                   /**< (PWM_FPV2) Fault Protection to Hi-Z for PWMH output on channel 3 Mask */
#define PWM_FPV2_FPZH3(value)                 (PWM_FPV2_FPZH3_Msk & ((value) << PWM_FPV2_FPZH3_Pos))
#define PWM_FPV2_FPZL0_Pos                    (16U)                                          /**< (PWM_FPV2) Fault Protection to Hi-Z for PWML output on channel 0 Position */
#define PWM_FPV2_FPZL0_Msk                    (0x1U << PWM_FPV2_FPZL0_Pos)                   /**< (PWM_FPV2) Fault Protection to Hi-Z for PWML output on channel 0 Mask */
#define PWM_FPV2_FPZL0(value)                 (PWM_FPV2_FPZL0_Msk & ((value) << PWM_FPV2_FPZL0_Pos))
#define PWM_FPV2_FPZL1_Pos                    (17U)                                          /**< (PWM_FPV2) Fault Protection to Hi-Z for PWML output on channel 1 Position */
#define PWM_FPV2_FPZL1_Msk                    (0x1U << PWM_FPV2_FPZL1_Pos)                   /**< (PWM_FPV2) Fault Protection to Hi-Z for PWML output on channel 1 Mask */
#define PWM_FPV2_FPZL1(value)                 (PWM_FPV2_FPZL1_Msk & ((value) << PWM_FPV2_FPZL1_Pos))
#define PWM_FPV2_FPZL2_Pos                    (18U)                                          /**< (PWM_FPV2) Fault Protection to Hi-Z for PWML output on channel 2 Position */
#define PWM_FPV2_FPZL2_Msk                    (0x1U << PWM_FPV2_FPZL2_Pos)                   /**< (PWM_FPV2) Fault Protection to Hi-Z for PWML output on channel 2 Mask */
#define PWM_FPV2_FPZL2(value)                 (PWM_FPV2_FPZL2_Msk & ((value) << PWM_FPV2_FPZL2_Pos))
#define PWM_FPV2_FPZL3_Pos                    (19U)                                          /**< (PWM_FPV2) Fault Protection to Hi-Z for PWML output on channel 3 Position */
#define PWM_FPV2_FPZL3_Msk                    (0x1U << PWM_FPV2_FPZL3_Pos)                   /**< (PWM_FPV2) Fault Protection to Hi-Z for PWML output on channel 3 Mask */
#define PWM_FPV2_FPZL3(value)                 (PWM_FPV2_FPZL3_Msk & ((value) << PWM_FPV2_FPZL3_Pos))
#define PWM_FPV2_Msk                          (0x000F000FUL)                                 /**< (PWM_FPV2) Register Mask  */

/* -------- PWM_WPCR : (PWM Offset: 0xE4) ( /W 32) PWM Write Protection Control Register -------- */
#define PWM_WPCR_WPCMD_Pos                    (0U)                                           /**< (PWM_WPCR) Write Protection Command Position */
#define PWM_WPCR_WPCMD_Msk                    (0x3U << PWM_WPCR_WPCMD_Pos)                   /**< (PWM_WPCR) Write Protection Command Mask */
#define PWM_WPCR_WPCMD(value)                 (PWM_WPCR_WPCMD_Msk & ((value) << PWM_WPCR_WPCMD_Pos))
#define   PWM_WPCR_WPCMD_DISABLE_SW_PROT_Val  (0U)                                           /**< (PWM_WPCR) Disables the software write protection of the register groups of which the bit WPRGx is at '1'.  */
#define   PWM_WPCR_WPCMD_ENABLE_SW_PROT_Val   (1U)                                           /**< (PWM_WPCR) Enables the software write protection of the register groups of which the bit WPRGx is at '1'.  */
#define   PWM_WPCR_WPCMD_ENABLE_HW_PROT_Val   (2U)                                           /**< (PWM_WPCR) Enables the hardware write protection of the register groups of which the bit WPRGx is at '1'. Only a hardware reset of the PWM controller can disable the hardware write protection. Moreover, to meet security requirements, the PIO lines associated with the PWM can not be configured through the PIO interface.  */
#define PWM_WPCR_WPCMD_DISABLE_SW_PROT        (PWM_WPCR_WPCMD_DISABLE_SW_PROT_Val << PWM_WPCR_WPCMD_Pos) /**< (PWM_WPCR) Disables the software write protection of the register groups of which the bit WPRGx is at '1'. Position  */
#define PWM_WPCR_WPCMD_ENABLE_SW_PROT         (PWM_WPCR_WPCMD_ENABLE_SW_PROT_Val << PWM_WPCR_WPCMD_Pos) /**< (PWM_WPCR) Enables the software write protection of the register groups of which the bit WPRGx is at '1'. Position  */
#define PWM_WPCR_WPCMD_ENABLE_HW_PROT         (PWM_WPCR_WPCMD_ENABLE_HW_PROT_Val << PWM_WPCR_WPCMD_Pos) /**< (PWM_WPCR) Enables the hardware write protection of the register groups of which the bit WPRGx is at '1'. Only a hardware reset of the PWM controller can disable the hardware write protection. Moreover, to meet security requirements, the PIO lines associated with the PWM can not be configured through the PIO interface. Position  */
#define PWM_WPCR_WPRG0_Pos                    (2U)                                           /**< (PWM_WPCR) Write Protection Register Group 0 Position */
#define PWM_WPCR_WPRG0_Msk                    (0x1U << PWM_WPCR_WPRG0_Pos)                   /**< (PWM_WPCR) Write Protection Register Group 0 Mask */
#define PWM_WPCR_WPRG0(value)                 (PWM_WPCR_WPRG0_Msk & ((value) << PWM_WPCR_WPRG0_Pos))
#define PWM_WPCR_WPRG1_Pos                    (3U)                                           /**< (PWM_WPCR) Write Protection Register Group 1 Position */
#define PWM_WPCR_WPRG1_Msk                    (0x1U << PWM_WPCR_WPRG1_Pos)                   /**< (PWM_WPCR) Write Protection Register Group 1 Mask */
#define PWM_WPCR_WPRG1(value)                 (PWM_WPCR_WPRG1_Msk & ((value) << PWM_WPCR_WPRG1_Pos))
#define PWM_WPCR_WPRG2_Pos                    (4U)                                           /**< (PWM_WPCR) Write Protection Register Group 2 Position */
#define PWM_WPCR_WPRG2_Msk                    (0x1U << PWM_WPCR_WPRG2_Pos)                   /**< (PWM_WPCR) Write Protection Register Group 2 Mask */
#define PWM_WPCR_WPRG2(value)                 (PWM_WPCR_WPRG2_Msk & ((value) << PWM_WPCR_WPRG2_Pos))
#define PWM_WPCR_WPRG3_Pos                    (5U)                                           /**< (PWM_WPCR) Write Protection Register Group 3 Position */
#define PWM_WPCR_WPRG3_Msk                    (0x1U << PWM_WPCR_WPRG3_Pos)                   /**< (PWM_WPCR) Write Protection Register Group 3 Mask */
#define PWM_WPCR_WPRG3(value)                 (PWM_WPCR_WPRG3_Msk & ((value) << PWM_WPCR_WPRG3_Pos))
#define PWM_WPCR_WPRG4_Pos                    (6U)                                           /**< (PWM_WPCR) Write Protection Register Group 4 Position */
#define PWM_WPCR_WPRG4_Msk                    (0x1U << PWM_WPCR_WPRG4_Pos)                   /**< (PWM_WPCR) Write Protection Register Group 4 Mask */
#define PWM_WPCR_WPRG4(value)                 (PWM_WPCR_WPRG4_Msk & ((value) << PWM_WPCR_WPRG4_Pos))
#define PWM_WPCR_WPRG5_Pos                    (7U)                                           /**< (PWM_WPCR) Write Protection Register Group 5 Position */
#define PWM_WPCR_WPRG5_Msk                    (0x1U << PWM_WPCR_WPRG5_Pos)                   /**< (PWM_WPCR) Write Protection Register Group 5 Mask */
#define PWM_WPCR_WPRG5(value)                 (PWM_WPCR_WPRG5_Msk & ((value) << PWM_WPCR_WPRG5_Pos))
#define PWM_WPCR_WPKEY_Pos                    (8U)                                           /**< (PWM_WPCR) Write Protection Key Position */
#define PWM_WPCR_WPKEY_Msk                    (0xFFFFFFU << PWM_WPCR_WPKEY_Pos)              /**< (PWM_WPCR) Write Protection Key Mask */
#define PWM_WPCR_WPKEY(value)                 (PWM_WPCR_WPKEY_Msk & ((value) << PWM_WPCR_WPKEY_Pos))
#define   PWM_WPCR_WPKEY_PASSWD_Val           (5265229U)                                     /**< (PWM_WPCR) Writing any other value in this field aborts the write operation of the WPCMD field.Always reads as 0  */
#define PWM_WPCR_WPKEY_PASSWD                 (PWM_WPCR_WPKEY_PASSWD_Val << PWM_WPCR_WPKEY_Pos) /**< (PWM_WPCR) Writing any other value in this field aborts the write operation of the WPCMD field.Always reads as 0 Position  */
#define PWM_WPCR_Msk                          (0xFFFFFFFFUL)                                 /**< (PWM_WPCR) Register Mask  */

/* -------- PWM_WPSR : (PWM Offset: 0xE8) (R/  32) PWM Write Protection Status Register -------- */
#define PWM_WPSR_WPSWS0_Pos                   (0U)                                           /**< (PWM_WPSR) Write Protect SW Status Position */
#define PWM_WPSR_WPSWS0_Msk                   (0x1U << PWM_WPSR_WPSWS0_Pos)                  /**< (PWM_WPSR) Write Protect SW Status Mask */
#define PWM_WPSR_WPSWS0(value)                (PWM_WPSR_WPSWS0_Msk & ((value) << PWM_WPSR_WPSWS0_Pos))
#define PWM_WPSR_WPSWS1_Pos                   (1U)                                           /**< (PWM_WPSR) Write Protect SW Status Position */
#define PWM_WPSR_WPSWS1_Msk                   (0x1U << PWM_WPSR_WPSWS1_Pos)                  /**< (PWM_WPSR) Write Protect SW Status Mask */
#define PWM_WPSR_WPSWS1(value)                (PWM_WPSR_WPSWS1_Msk & ((value) << PWM_WPSR_WPSWS1_Pos))
#define PWM_WPSR_WPSWS2_Pos                   (2U)                                           /**< (PWM_WPSR) Write Protect SW Status Position */
#define PWM_WPSR_WPSWS2_Msk                   (0x1U << PWM_WPSR_WPSWS2_Pos)                  /**< (PWM_WPSR) Write Protect SW Status Mask */
#define PWM_WPSR_WPSWS2(value)                (PWM_WPSR_WPSWS2_Msk & ((value) << PWM_WPSR_WPSWS2_Pos))
#define PWM_WPSR_WPSWS3_Pos                   (3U)                                           /**< (PWM_WPSR) Write Protect SW Status Position */
#define PWM_WPSR_WPSWS3_Msk                   (0x1U << PWM_WPSR_WPSWS3_Pos)                  /**< (PWM_WPSR) Write Protect SW Status Mask */
#define PWM_WPSR_WPSWS3(value)                (PWM_WPSR_WPSWS3_Msk & ((value) << PWM_WPSR_WPSWS3_Pos))
#define PWM_WPSR_WPSWS4_Pos                   (4U)                                           /**< (PWM_WPSR) Write Protect SW Status Position */
#define PWM_WPSR_WPSWS4_Msk                   (0x1U << PWM_WPSR_WPSWS4_Pos)                  /**< (PWM_WPSR) Write Protect SW Status Mask */
#define PWM_WPSR_WPSWS4(value)                (PWM_WPSR_WPSWS4_Msk & ((value) << PWM_WPSR_WPSWS4_Pos))
#define PWM_WPSR_WPSWS5_Pos                   (5U)                                           /**< (PWM_WPSR) Write Protect SW Status Position */
#define PWM_WPSR_WPSWS5_Msk                   (0x1U << PWM_WPSR_WPSWS5_Pos)                  /**< (PWM_WPSR) Write Protect SW Status Mask */
#define PWM_WPSR_WPSWS5(value)                (PWM_WPSR_WPSWS5_Msk & ((value) << PWM_WPSR_WPSWS5_Pos))
#define PWM_WPSR_WPVS_Pos                     (7U)                                           /**< (PWM_WPSR) Write Protect Violation Status Position */
#define PWM_WPSR_WPVS_Msk                     (0x1U << PWM_WPSR_WPVS_Pos)                    /**< (PWM_WPSR) Write Protect Violation Status Mask */
#define PWM_WPSR_WPVS(value)                  (PWM_WPSR_WPVS_Msk & ((value) << PWM_WPSR_WPVS_Pos))
#define PWM_WPSR_WPHWS0_Pos                   (8U)                                           /**< (PWM_WPSR) Write Protect HW Status Position */
#define PWM_WPSR_WPHWS0_Msk                   (0x1U << PWM_WPSR_WPHWS0_Pos)                  /**< (PWM_WPSR) Write Protect HW Status Mask */
#define PWM_WPSR_WPHWS0(value)                (PWM_WPSR_WPHWS0_Msk & ((value) << PWM_WPSR_WPHWS0_Pos))
#define PWM_WPSR_WPHWS1_Pos                   (9U)                                           /**< (PWM_WPSR) Write Protect HW Status Position */
#define PWM_WPSR_WPHWS1_Msk                   (0x1U << PWM_WPSR_WPHWS1_Pos)                  /**< (PWM_WPSR) Write Protect HW Status Mask */
#define PWM_WPSR_WPHWS1(value)                (PWM_WPSR_WPHWS1_Msk & ((value) << PWM_WPSR_WPHWS1_Pos))
#define PWM_WPSR_WPHWS2_Pos                   (10U)                                          /**< (PWM_WPSR) Write Protect HW Status Position */
#define PWM_WPSR_WPHWS2_Msk                   (0x1U << PWM_WPSR_WPHWS2_Pos)                  /**< (PWM_WPSR) Write Protect HW Status Mask */
#define PWM_WPSR_WPHWS2(value)                (PWM_WPSR_WPHWS2_Msk & ((value) << PWM_WPSR_WPHWS2_Pos))
#define PWM_WPSR_WPHWS3_Pos                   (11U)                                          /**< (PWM_WPSR) Write Protect HW Status Position */
#define PWM_WPSR_WPHWS3_Msk                   (0x1U << PWM_WPSR_WPHWS3_Pos)                  /**< (PWM_WPSR) Write Protect HW Status Mask */
#define PWM_WPSR_WPHWS3(value)                (PWM_WPSR_WPHWS3_Msk & ((value) << PWM_WPSR_WPHWS3_Pos))
#define PWM_WPSR_WPHWS4_Pos                   (12U)                                          /**< (PWM_WPSR) Write Protect HW Status Position */
#define PWM_WPSR_WPHWS4_Msk                   (0x1U << PWM_WPSR_WPHWS4_Pos)                  /**< (PWM_WPSR) Write Protect HW Status Mask */
#define PWM_WPSR_WPHWS4(value)                (PWM_WPSR_WPHWS4_Msk & ((value) << PWM_WPSR_WPHWS4_Pos))
#define PWM_WPSR_WPHWS5_Pos                   (13U)                                          /**< (PWM_WPSR) Write Protect HW Status Position */
#define PWM_WPSR_WPHWS5_Msk                   (0x1U << PWM_WPSR_WPHWS5_Pos)                  /**< (PWM_WPSR) Write Protect HW Status Mask */
#define PWM_WPSR_WPHWS5(value)                (PWM_WPSR_WPHWS5_Msk & ((value) << PWM_WPSR_WPHWS5_Pos))
#define PWM_WPSR_WPVSRC_Pos                   (16U)                                          /**< (PWM_WPSR) Write Protect Violation Source Position */
#define PWM_WPSR_WPVSRC_Msk                   (0xFFFFU << PWM_WPSR_WPVSRC_Pos)               /**< (PWM_WPSR) Write Protect Violation Source Mask */
#define PWM_WPSR_WPVSRC(value)                (PWM_WPSR_WPVSRC_Msk & ((value) << PWM_WPSR_WPVSRC_Pos))
#define PWM_WPSR_Msk                          (0xFFFF3FBFUL)                                 /**< (PWM_WPSR) Register Mask  */

/* -------- PWM_CMPV : (PWM Offset: 0x00) (R/W 32) PWM Comparison 0 Value Register -------- */
#define PWM_CMPV_CV_Pos                       (0U)                                           /**< (PWM_CMPV) Comparison x Value Position */
#define PWM_CMPV_CV_Msk                       (0xFFFFFFU << PWM_CMPV_CV_Pos)                 /**< (PWM_CMPV) Comparison x Value Mask */
#define PWM_CMPV_CV(value)                    (PWM_CMPV_CV_Msk & ((value) << PWM_CMPV_CV_Pos))
#define PWM_CMPV_CVM_Pos                      (24U)                                          /**< (PWM_CMPV) Comparison x Value Mode Position */
#define PWM_CMPV_CVM_Msk                      (0x1U << PWM_CMPV_CVM_Pos)                     /**< (PWM_CMPV) Comparison x Value Mode Mask */
#define PWM_CMPV_CVM(value)                   (PWM_CMPV_CVM_Msk & ((value) << PWM_CMPV_CVM_Pos))
#define   PWM_CMPV_CVM_COMPARE_AT_INCREMENT_Val (0U)                                           /**< (PWM_CMPV) Compare when counter is incrementing  */
#define   PWM_CMPV_CVM_COMPARE_AT_DECREMENT_Val (1U)                                           /**< (PWM_CMPV) Compare when counter is decrementing  */
#define PWM_CMPV_CVM_COMPARE_AT_INCREMENT     (PWM_CMPV_CVM_COMPARE_AT_INCREMENT_Val << PWM_CMPV_CVM_Pos) /**< (PWM_CMPV) Compare when counter is incrementing Position  */
#define PWM_CMPV_CVM_COMPARE_AT_DECREMENT     (PWM_CMPV_CVM_COMPARE_AT_DECREMENT_Val << PWM_CMPV_CVM_Pos) /**< (PWM_CMPV) Compare when counter is decrementing Position  */
#define PWM_CMPV_Msk                          (0x01FFFFFFUL)                                 /**< (PWM_CMPV) Register Mask  */

/* -------- PWM_CMPVUPD : (PWM Offset: 0x04) ( /W 32) PWM Comparison 0 Value Update Register -------- */
#define PWM_CMPVUPD_CVUPD_Pos                 (0U)                                           /**< (PWM_CMPVUPD) Comparison x Value Update Position */
#define PWM_CMPVUPD_CVUPD_Msk                 (0xFFFFFFU << PWM_CMPVUPD_CVUPD_Pos)           /**< (PWM_CMPVUPD) Comparison x Value Update Mask */
#define PWM_CMPVUPD_CVUPD(value)              (PWM_CMPVUPD_CVUPD_Msk & ((value) << PWM_CMPVUPD_CVUPD_Pos))
#define PWM_CMPVUPD_CVMUPD_Pos                (24U)                                          /**< (PWM_CMPVUPD) Comparison x Value Mode Update Position */
#define PWM_CMPVUPD_CVMUPD_Msk                (0x1U << PWM_CMPVUPD_CVMUPD_Pos)               /**< (PWM_CMPVUPD) Comparison x Value Mode Update Mask */
#define PWM_CMPVUPD_CVMUPD(value)             (PWM_CMPVUPD_CVMUPD_Msk & ((value) << PWM_CMPVUPD_CVMUPD_Pos))
#define PWM_CMPVUPD_Msk                       (0x01FFFFFFUL)                                 /**< (PWM_CMPVUPD) Register Mask  */

/* -------- PWM_CMPM : (PWM Offset: 0x08) (R/W 32) PWM Comparison 0 Mode Register -------- */
#define PWM_CMPM_CEN_Pos                      (0U)                                           /**< (PWM_CMPM) Comparison x Enable Position */
#define PWM_CMPM_CEN_Msk                      (0x1U << PWM_CMPM_CEN_Pos)                     /**< (PWM_CMPM) Comparison x Enable Mask */
#define PWM_CMPM_CEN(value)                   (PWM_CMPM_CEN_Msk & ((value) << PWM_CMPM_CEN_Pos))
#define PWM_CMPM_CTR_Pos                      (4U)                                           /**< (PWM_CMPM) Comparison x Trigger Position */
#define PWM_CMPM_CTR_Msk                      (0xFU << PWM_CMPM_CTR_Pos)                     /**< (PWM_CMPM) Comparison x Trigger Mask */
#define PWM_CMPM_CTR(value)                   (PWM_CMPM_CTR_Msk & ((value) << PWM_CMPM_CTR_Pos))
#define PWM_CMPM_CPR_Pos                      (8U)                                           /**< (PWM_CMPM) Comparison x Period Position */
#define PWM_CMPM_CPR_Msk                      (0xFU << PWM_CMPM_CPR_Pos)                     /**< (PWM_CMPM) Comparison x Period Mask */
#define PWM_CMPM_CPR(value)                   (PWM_CMPM_CPR_Msk & ((value) << PWM_CMPM_CPR_Pos))
#define PWM_CMPM_CPRCNT_Pos                   (12U)                                          /**< (PWM_CMPM) Comparison x Period Counter Position */
#define PWM_CMPM_CPRCNT_Msk                   (0xFU << PWM_CMPM_CPRCNT_Pos)                  /**< (PWM_CMPM) Comparison x Period Counter Mask */
#define PWM_CMPM_CPRCNT(value)                (PWM_CMPM_CPRCNT_Msk & ((value) << PWM_CMPM_CPRCNT_Pos))
#define PWM_CMPM_CUPR_Pos                     (16U)                                          /**< (PWM_CMPM) Comparison x Update Period Position */
#define PWM_CMPM_CUPR_Msk                     (0xFU << PWM_CMPM_CUPR_Pos)                    /**< (PWM_CMPM) Comparison x Update Period Mask */
#define PWM_CMPM_CUPR(value)                  (PWM_CMPM_CUPR_Msk & ((value) << PWM_CMPM_CUPR_Pos))
#define PWM_CMPM_CUPRCNT_Pos                  (20U)                                          /**< (PWM_CMPM) Comparison x Update Period Counter Position */
#define PWM_CMPM_CUPRCNT_Msk                  (0xFU << PWM_CMPM_CUPRCNT_Pos)                 /**< (PWM_CMPM) Comparison x Update Period Counter Mask */
#define PWM_CMPM_CUPRCNT(value)               (PWM_CMPM_CUPRCNT_Msk & ((value) << PWM_CMPM_CUPRCNT_Pos))
#define PWM_CMPM_Msk                          (0x00FFFFF1UL)                                 /**< (PWM_CMPM) Register Mask  */

/* -------- PWM_CMPMUPD : (PWM Offset: 0x0C) ( /W 32) PWM Comparison 0 Mode Update Register -------- */
#define PWM_CMPMUPD_CENUPD_Pos                (0U)                                           /**< (PWM_CMPMUPD) Comparison x Enable Update Position */
#define PWM_CMPMUPD_CENUPD_Msk                (0x1U << PWM_CMPMUPD_CENUPD_Pos)               /**< (PWM_CMPMUPD) Comparison x Enable Update Mask */
#define PWM_CMPMUPD_CENUPD(value)             (PWM_CMPMUPD_CENUPD_Msk & ((value) << PWM_CMPMUPD_CENUPD_Pos))
#define PWM_CMPMUPD_CTRUPD_Pos                (4U)                                           /**< (PWM_CMPMUPD) Comparison x Trigger Update Position */
#define PWM_CMPMUPD_CTRUPD_Msk                (0xFU << PWM_CMPMUPD_CTRUPD_Pos)               /**< (PWM_CMPMUPD) Comparison x Trigger Update Mask */
#define PWM_CMPMUPD_CTRUPD(value)             (PWM_CMPMUPD_CTRUPD_Msk & ((value) << PWM_CMPMUPD_CTRUPD_Pos))
#define PWM_CMPMUPD_CPRUPD_Pos                (8U)                                           /**< (PWM_CMPMUPD) Comparison x Period Update Position */
#define PWM_CMPMUPD_CPRUPD_Msk                (0xFU << PWM_CMPMUPD_CPRUPD_Pos)               /**< (PWM_CMPMUPD) Comparison x Period Update Mask */
#define PWM_CMPMUPD_CPRUPD(value)             (PWM_CMPMUPD_CPRUPD_Msk & ((value) << PWM_CMPMUPD_CPRUPD_Pos))
#define PWM_CMPMUPD_CUPRUPD_Pos               (16U)                                          /**< (PWM_CMPMUPD) Comparison x Update Period Update Position */
#define PWM_CMPMUPD_CUPRUPD_Msk               (0xFU << PWM_CMPMUPD_CUPRUPD_Pos)              /**< (PWM_CMPMUPD) Comparison x Update Period Update Mask */
#define PWM_CMPMUPD_CUPRUPD(value)            (PWM_CMPMUPD_CUPRUPD_Msk & ((value) << PWM_CMPMUPD_CUPRUPD_Pos))
#define PWM_CMPMUPD_Msk                       (0x000F0FF1UL)                                 /**< (PWM_CMPMUPD) Register Mask  */

/* -------- PWM_CMR : (PWM Offset: 0x00) (R/W 32) PWM Channel Mode Register -------- */
#define PWM_CMR_CPRE_Pos                      (0U)                                           /**< (PWM_CMR) Channel Pre-scaler Position */
#define PWM_CMR_CPRE_Msk                      (0xFU << PWM_CMR_CPRE_Pos)                     /**< (PWM_CMR) Channel Pre-scaler Mask */
#define PWM_CMR_CPRE(value)                   (PWM_CMR_CPRE_Msk & ((value) << PWM_CMR_CPRE_Pos))
#define   PWM_CMR_CPRE_MCK_Val                (0U)                                           /**< (PWM_CMR) Peripheral clock  */
#define   PWM_CMR_CPRE_MCK_DIV_2_Val          (1U)                                           /**< (PWM_CMR) Peripheral clock/2  */
#define   PWM_CMR_CPRE_MCK_DIV_4_Val          (2U)                                           /**< (PWM_CMR) Peripheral clock/4  */
#define   PWM_CMR_CPRE_MCK_DIV_8_Val          (3U)                                           /**< (PWM_CMR) Peripheral clock/8  */
#define   PWM_CMR_CPRE_MCK_DIV_16_Val         (4U)                                           /**< (PWM_CMR) Peripheral clock/16  */
#define   PWM_CMR_CPRE_MCK_DIV_32_Val         (5U)                                           /**< (PWM_CMR) Peripheral clock/32  */
#define   PWM_CMR_CPRE_MCK_DIV_64_Val         (6U)                                           /**< (PWM_CMR) Peripheral clock/64  */
#define   PWM_CMR_CPRE_MCK_DIV_128_Val        (7U)                                           /**< (PWM_CMR) Peripheral clock/128  */
#define   PWM_CMR_CPRE_MCK_DIV_256_Val        (8U)                                           /**< (PWM_CMR) Peripheral clock/256  */
#define   PWM_CMR_CPRE_MCK_DIV_512_Val        (9U)                                           /**< (PWM_CMR) Peripheral clock/512  */
#define   PWM_CMR_CPRE_MCK_DIV_1024_Val       (10U)                                          /**< (PWM_CMR) Peripheral clock/1024  */
#define   PWM_CMR_CPRE_CLKA_Val               (11U)                                          /**< (PWM_CMR) Clock A  */
#define   PWM_CMR_CPRE_CLKB_Val               (12U)                                          /**< (PWM_CMR) Clock B  */
#define PWM_CMR_CPRE_MCK                      (PWM_CMR_CPRE_MCK_Val << PWM_CMR_CPRE_Pos)     /**< (PWM_CMR) Peripheral clock Position  */
#define PWM_CMR_CPRE_MCK_DIV_2                (PWM_CMR_CPRE_MCK_DIV_2_Val << PWM_CMR_CPRE_Pos) /**< (PWM_CMR) Peripheral clock/2 Position  */
#define PWM_CMR_CPRE_MCK_DIV_4                (PWM_CMR_CPRE_MCK_DIV_4_Val << PWM_CMR_CPRE_Pos) /**< (PWM_CMR) Peripheral clock/4 Position  */
#define PWM_CMR_CPRE_MCK_DIV_8                (PWM_CMR_CPRE_MCK_DIV_8_Val << PWM_CMR_CPRE_Pos) /**< (PWM_CMR) Peripheral clock/8 Position  */
#define PWM_CMR_CPRE_MCK_DIV_16               (PWM_CMR_CPRE_MCK_DIV_16_Val << PWM_CMR_CPRE_Pos) /**< (PWM_CMR) Peripheral clock/16 Position  */
#define PWM_CMR_CPRE_MCK_DIV_32               (PWM_CMR_CPRE_MCK_DIV_32_Val << PWM_CMR_CPRE_Pos) /**< (PWM_CMR) Peripheral clock/32 Position  */
#define PWM_CMR_CPRE_MCK_DIV_64               (PWM_CMR_CPRE_MCK_DIV_64_Val << PWM_CMR_CPRE_Pos) /**< (PWM_CMR) Peripheral clock/64 Position  */
#define PWM_CMR_CPRE_MCK_DIV_128              (PWM_CMR_CPRE_MCK_DIV_128_Val << PWM_CMR_CPRE_Pos) /**< (PWM_CMR) Peripheral clock/128 Position  */
#define PWM_CMR_CPRE_MCK_DIV_256              (PWM_CMR_CPRE_MCK_DIV_256_Val << PWM_CMR_CPRE_Pos) /**< (PWM_CMR) Peripheral clock/256 Position  */
#define PWM_CMR_CPRE_MCK_DIV_512              (PWM_CMR_CPRE_MCK_DIV_512_Val << PWM_CMR_CPRE_Pos) /**< (PWM_CMR) Peripheral clock/512 Position  */
#define PWM_CMR_CPRE_MCK_DIV_1024             (PWM_CMR_CPRE_MCK_DIV_1024_Val << PWM_CMR_CPRE_Pos) /**< (PWM_CMR) Peripheral clock/1024 Position  */
#define PWM_CMR_CPRE_CLKA                     (PWM_CMR_CPRE_CLKA_Val << PWM_CMR_CPRE_Pos)    /**< (PWM_CMR) Clock A Position  */
#define PWM_CMR_CPRE_CLKB                     (PWM_CMR_CPRE_CLKB_Val << PWM_CMR_CPRE_Pos)    /**< (PWM_CMR) Clock B Position  */
#define PWM_CMR_CALG_Pos                      (8U)                                           /**< (PWM_CMR) Channel Alignment Position */
#define PWM_CMR_CALG_Msk                      (0x1U << PWM_CMR_CALG_Pos)                     /**< (PWM_CMR) Channel Alignment Mask */
#define PWM_CMR_CALG(value)                   (PWM_CMR_CALG_Msk & ((value) << PWM_CMR_CALG_Pos))
#define   PWM_CMR_CALG_LEFT_ALIGNED_Val       (0U)                                           /**< (PWM_CMR) Left aligned  */
#define   PWM_CMR_CALG_CENTER_ALIGNED_Val     (1U)                                           /**< (PWM_CMR) Center aligned  */
#define PWM_CMR_CALG_LEFT_ALIGNED             (PWM_CMR_CALG_LEFT_ALIGNED_Val << PWM_CMR_CALG_Pos) /**< (PWM_CMR) Left aligned Position  */
#define PWM_CMR_CALG_CENTER_ALIGNED           (PWM_CMR_CALG_CENTER_ALIGNED_Val << PWM_CMR_CALG_Pos) /**< (PWM_CMR) Center aligned Position  */
#define PWM_CMR_CPOL_Pos                      (9U)                                           /**< (PWM_CMR) Channel Polarity Position */
#define PWM_CMR_CPOL_Msk                      (0x1U << PWM_CMR_CPOL_Pos)                     /**< (PWM_CMR) Channel Polarity Mask */
#define PWM_CMR_CPOL(value)                   (PWM_CMR_CPOL_Msk & ((value) << PWM_CMR_CPOL_Pos))
#define   PWM_CMR_CPOL_LOW_POLARITY_Val       (0U)                                           /**< (PWM_CMR) Waveform starts at low level  */
#define   PWM_CMR_CPOL_HIGH_POLARITY_Val      (1U)                                           /**< (PWM_CMR) Waveform starts at high level  */
#define PWM_CMR_CPOL_LOW_POLARITY             (PWM_CMR_CPOL_LOW_POLARITY_Val << PWM_CMR_CPOL_Pos) /**< (PWM_CMR) Waveform starts at low level Position  */
#define PWM_CMR_CPOL_HIGH_POLARITY            (PWM_CMR_CPOL_HIGH_POLARITY_Val << PWM_CMR_CPOL_Pos) /**< (PWM_CMR) Waveform starts at high level Position  */
#define PWM_CMR_CES_Pos                       (10U)                                          /**< (PWM_CMR) Counter Event Selection Position */
#define PWM_CMR_CES_Msk                       (0x1U << PWM_CMR_CES_Pos)                      /**< (PWM_CMR) Counter Event Selection Mask */
#define PWM_CMR_CES(value)                    (PWM_CMR_CES_Msk & ((value) << PWM_CMR_CES_Pos))
#define   PWM_CMR_CES_SINGLE_EVENT_Val        (0U)                                           /**< (PWM_CMR) At the end of PWM period  */
#define   PWM_CMR_CES_DOUBLE_EVENT_Val        (1U)                                           /**< (PWM_CMR) At half of PWM period AND at the end of PWM period  */
#define PWM_CMR_CES_SINGLE_EVENT              (PWM_CMR_CES_SINGLE_EVENT_Val << PWM_CMR_CES_Pos) /**< (PWM_CMR) At the end of PWM period Position  */
#define PWM_CMR_CES_DOUBLE_EVENT              (PWM_CMR_CES_DOUBLE_EVENT_Val << PWM_CMR_CES_Pos) /**< (PWM_CMR) At half of PWM period AND at the end of PWM period Position  */
#define PWM_CMR_UPDS_Pos                      (11U)                                          /**< (PWM_CMR) Update Selection Position */
#define PWM_CMR_UPDS_Msk                      (0x1U << PWM_CMR_UPDS_Pos)                     /**< (PWM_CMR) Update Selection Mask */
#define PWM_CMR_UPDS(value)                   (PWM_CMR_UPDS_Msk & ((value) << PWM_CMR_UPDS_Pos))
#define   PWM_CMR_UPDS_UPDATE_AT_PERIOD_Val   (0U)                                           /**< (PWM_CMR) At the next end of PWM period  */
#define   PWM_CMR_UPDS_UPDATE_AT_HALF_PERIOD_Val (1U)                                           /**< (PWM_CMR) At the next end of Half PWM period  */
#define PWM_CMR_UPDS_UPDATE_AT_PERIOD         (PWM_CMR_UPDS_UPDATE_AT_PERIOD_Val << PWM_CMR_UPDS_Pos) /**< (PWM_CMR) At the next end of PWM period Position  */
#define PWM_CMR_UPDS_UPDATE_AT_HALF_PERIOD    (PWM_CMR_UPDS_UPDATE_AT_HALF_PERIOD_Val << PWM_CMR_UPDS_Pos) /**< (PWM_CMR) At the next end of Half PWM period Position  */
#define PWM_CMR_DPOLI_Pos                     (12U)                                          /**< (PWM_CMR) Disabled Polarity Inverted Position */
#define PWM_CMR_DPOLI_Msk                     (0x1U << PWM_CMR_DPOLI_Pos)                    /**< (PWM_CMR) Disabled Polarity Inverted Mask */
#define PWM_CMR_DPOLI(value)                  (PWM_CMR_DPOLI_Msk & ((value) << PWM_CMR_DPOLI_Pos))
#define PWM_CMR_TCTS_Pos                      (13U)                                          /**< (PWM_CMR) Timer Counter Trigger Selection Position */
#define PWM_CMR_TCTS_Msk                      (0x1U << PWM_CMR_TCTS_Pos)                     /**< (PWM_CMR) Timer Counter Trigger Selection Mask */
#define PWM_CMR_TCTS(value)                   (PWM_CMR_TCTS_Msk & ((value) << PWM_CMR_TCTS_Pos))
#define PWM_CMR_DTE_Pos                       (16U)                                          /**< (PWM_CMR) Dead-Time Generator Enable Position */
#define PWM_CMR_DTE_Msk                       (0x1U << PWM_CMR_DTE_Pos)                      /**< (PWM_CMR) Dead-Time Generator Enable Mask */
#define PWM_CMR_DTE(value)                    (PWM_CMR_DTE_Msk & ((value) << PWM_CMR_DTE_Pos))
#define PWM_CMR_DTHI_Pos                      (17U)                                          /**< (PWM_CMR) Dead-Time PWMHx Output Inverted Position */
#define PWM_CMR_DTHI_Msk                      (0x1U << PWM_CMR_DTHI_Pos)                     /**< (PWM_CMR) Dead-Time PWMHx Output Inverted Mask */
#define PWM_CMR_DTHI(value)                   (PWM_CMR_DTHI_Msk & ((value) << PWM_CMR_DTHI_Pos))
#define PWM_CMR_DTLI_Pos                      (18U)                                          /**< (PWM_CMR) Dead-Time PWMLx Output Inverted Position */
#define PWM_CMR_DTLI_Msk                      (0x1U << PWM_CMR_DTLI_Pos)                     /**< (PWM_CMR) Dead-Time PWMLx Output Inverted Mask */
#define PWM_CMR_DTLI(value)                   (PWM_CMR_DTLI_Msk & ((value) << PWM_CMR_DTLI_Pos))
#define PWM_CMR_PPM_Pos                       (19U)                                          /**< (PWM_CMR) Push-Pull Mode Position */
#define PWM_CMR_PPM_Msk                       (0x1U << PWM_CMR_PPM_Pos)                      /**< (PWM_CMR) Push-Pull Mode Mask */
#define PWM_CMR_PPM(value)                    (PWM_CMR_PPM_Msk & ((value) << PWM_CMR_PPM_Pos))
#define PWM_CMR_Msk                           (0x000F3F0FUL)                                 /**< (PWM_CMR) Register Mask  */

/* -------- PWM_CDTY : (PWM Offset: 0x04) (R/W 32) PWM Channel Duty Cycle Register -------- */
#define PWM_CDTY_CDTY_Pos                     (0U)                                           /**< (PWM_CDTY) Channel Duty-Cycle Position */
#define PWM_CDTY_CDTY_Msk                     (0xFFFFFFU << PWM_CDTY_CDTY_Pos)               /**< (PWM_CDTY) Channel Duty-Cycle Mask */
#define PWM_CDTY_CDTY(value)                  (PWM_CDTY_CDTY_Msk & ((value) << PWM_CDTY_CDTY_Pos))
#define PWM_CDTY_Msk                          (0x00FFFFFFUL)                                 /**< (PWM_CDTY) Register Mask  */

/* -------- PWM_CDTYUPD : (PWM Offset: 0x08) ( /W 32) PWM Channel Duty Cycle Update Register -------- */
#define PWM_CDTYUPD_CDTYUPD_Pos               (0U)                                           /**< (PWM_CDTYUPD) Channel Duty-Cycle Update Position */
#define PWM_CDTYUPD_CDTYUPD_Msk               (0xFFFFFFU << PWM_CDTYUPD_CDTYUPD_Pos)         /**< (PWM_CDTYUPD) Channel Duty-Cycle Update Mask */
#define PWM_CDTYUPD_CDTYUPD(value)            (PWM_CDTYUPD_CDTYUPD_Msk & ((value) << PWM_CDTYUPD_CDTYUPD_Pos))
#define PWM_CDTYUPD_Msk                       (0x00FFFFFFUL)                                 /**< (PWM_CDTYUPD) Register Mask  */

/* -------- PWM_CPRD : (PWM Offset: 0x0C) (R/W 32) PWM Channel Period Register -------- */
#define PWM_CPRD_CPRD_Pos                     (0U)                                           /**< (PWM_CPRD) Channel Period Position */
#define PWM_CPRD_CPRD_Msk                     (0xFFFFFFU << PWM_CPRD_CPRD_Pos)               /**< (PWM_CPRD) Channel Period Mask */
#define PWM_CPRD_CPRD(value)                  (PWM_CPRD_CPRD_Msk & ((value) << PWM_CPRD_CPRD_Pos))
#define PWM_CPRD_Msk                          (0x00FFFFFFUL)                                 /**< (PWM_CPRD) Register Mask  */

/* -------- PWM_CPRDUPD : (PWM Offset: 0x10) ( /W 32) PWM Channel Period Update Register -------- */
#define PWM_CPRDUPD_CPRDUPD_Pos               (0U)                                           /**< (PWM_CPRDUPD) Channel Period Update Position */
#define PWM_CPRDUPD_CPRDUPD_Msk               (0xFFFFFFU << PWM_CPRDUPD_CPRDUPD_Pos)         /**< (PWM_CPRDUPD) Channel Period Update Mask */
#define PWM_CPRDUPD_CPRDUPD(value)            (PWM_CPRDUPD_CPRDUPD_Msk & ((value) << PWM_CPRDUPD_CPRDUPD_Pos))
#define PWM_CPRDUPD_Msk                       (0x00FFFFFFUL)                                 /**< (PWM_CPRDUPD) Register Mask  */

/* -------- PWM_CCNT : (PWM Offset: 0x14) (R/  32) PWM Channel Counter Register -------- */
#define PWM_CCNT_CNT_Pos                      (0U)                                           /**< (PWM_CCNT) Channel Counter Register Position */
#define PWM_CCNT_CNT_Msk                      (0xFFFFFFU << PWM_CCNT_CNT_Pos)                /**< (PWM_CCNT) Channel Counter Register Mask */
#define PWM_CCNT_CNT(value)                   (PWM_CCNT_CNT_Msk & ((value) << PWM_CCNT_CNT_Pos))
#define PWM_CCNT_Msk                          (0x00FFFFFFUL)                                 /**< (PWM_CCNT) Register Mask  */

/* -------- PWM_DT : (PWM Offset: 0x18) (R/W 32) PWM Channel Dead Time Register -------- */
#define PWM_DT_DTH_Pos                        (0U)                                           /**< (PWM_DT) Dead-Time Value for PWMHx Output Position */
#define PWM_DT_DTH_Msk                        (0xFFFFU << PWM_DT_DTH_Pos)                    /**< (PWM_DT) Dead-Time Value for PWMHx Output Mask */
#define PWM_DT_DTH(value)                     (PWM_DT_DTH_Msk & ((value) << PWM_DT_DTH_Pos))
#define PWM_DT_DTL_Pos                        (16U)                                          /**< (PWM_DT) Dead-Time Value for PWMLx Output Position */
#define PWM_DT_DTL_Msk                        (0xFFFFU << PWM_DT_DTL_Pos)                    /**< (PWM_DT) Dead-Time Value for PWMLx Output Mask */
#define PWM_DT_DTL(value)                     (PWM_DT_DTL_Msk & ((value) << PWM_DT_DTL_Pos))
#define PWM_DT_Msk                            (0xFFFFFFFFUL)                                 /**< (PWM_DT) Register Mask  */

/* -------- PWM_DTUPD : (PWM Offset: 0x1C) ( /W 32) PWM Channel Dead Time Update Register -------- */
#define PWM_DTUPD_DTHUPD_Pos                  (0U)                                           /**< (PWM_DTUPD) Dead-Time Value Update for PWMHx Output Position */
#define PWM_DTUPD_DTHUPD_Msk                  (0xFFFFU << PWM_DTUPD_DTHUPD_Pos)              /**< (PWM_DTUPD) Dead-Time Value Update for PWMHx Output Mask */
#define PWM_DTUPD_DTHUPD(value)               (PWM_DTUPD_DTHUPD_Msk & ((value) << PWM_DTUPD_DTHUPD_Pos))
#define PWM_DTUPD_DTLUPD_Pos                  (16U)                                          /**< (PWM_DTUPD) Dead-Time Value Update for PWMLx Output Position */
#define PWM_DTUPD_DTLUPD_Msk                  (0xFFFFU << PWM_DTUPD_DTLUPD_Pos)              /**< (PWM_DTUPD) Dead-Time Value Update for PWMLx Output Mask */
#define PWM_DTUPD_DTLUPD(value)               (PWM_DTUPD_DTLUPD_Msk & ((value) << PWM_DTUPD_DTLUPD_Pos))
#define PWM_DTUPD_Msk                         (0xFFFFFFFFUL)                                 /**< (PWM_DTUPD) Register Mask  */

/* -------- PWM_CMUPD0 : (PWM Offset: 0x400) ( /W 32) PWM Channel Mode Update Register (ch_num = 0) -------- */
#define PWM_CMUPD0_CPOLUP_Pos                 (9U)                                           /**< (PWM_CMUPD0) Channel Polarity Update Position */
#define PWM_CMUPD0_CPOLUP_Msk                 (0x1U << PWM_CMUPD0_CPOLUP_Pos)                /**< (PWM_CMUPD0) Channel Polarity Update Mask */
#define PWM_CMUPD0_CPOLUP(value)              (PWM_CMUPD0_CPOLUP_Msk & ((value) << PWM_CMUPD0_CPOLUP_Pos))
#define PWM_CMUPD0_CPOLINVUP_Pos              (13U)                                          /**< (PWM_CMUPD0) Channel Polarity Inversion Update Position */
#define PWM_CMUPD0_CPOLINVUP_Msk              (0x1U << PWM_CMUPD0_CPOLINVUP_Pos)             /**< (PWM_CMUPD0) Channel Polarity Inversion Update Mask */
#define PWM_CMUPD0_CPOLINVUP(value)           (PWM_CMUPD0_CPOLINVUP_Msk & ((value) << PWM_CMUPD0_CPOLINVUP_Pos))
#define PWM_CMUPD0_Msk                        (0x00002200UL)                                 /**< (PWM_CMUPD0) Register Mask  */

/* -------- PWM_CMUPD1 : (PWM Offset: 0x420) ( /W 32) PWM Channel Mode Update Register (ch_num = 1) -------- */
#define PWM_CMUPD1_CPOLUP_Pos                 (9U)                                           /**< (PWM_CMUPD1) Channel Polarity Update Position */
#define PWM_CMUPD1_CPOLUP_Msk                 (0x1U << PWM_CMUPD1_CPOLUP_Pos)                /**< (PWM_CMUPD1) Channel Polarity Update Mask */
#define PWM_CMUPD1_CPOLUP(value)              (PWM_CMUPD1_CPOLUP_Msk & ((value) << PWM_CMUPD1_CPOLUP_Pos))
#define PWM_CMUPD1_CPOLINVUP_Pos              (13U)                                          /**< (PWM_CMUPD1) Channel Polarity Inversion Update Position */
#define PWM_CMUPD1_CPOLINVUP_Msk              (0x1U << PWM_CMUPD1_CPOLINVUP_Pos)             /**< (PWM_CMUPD1) Channel Polarity Inversion Update Mask */
#define PWM_CMUPD1_CPOLINVUP(value)           (PWM_CMUPD1_CPOLINVUP_Msk & ((value) << PWM_CMUPD1_CPOLINVUP_Pos))
#define PWM_CMUPD1_Msk                        (0x00002200UL)                                 /**< (PWM_CMUPD1) Register Mask  */

/* -------- PWM_ETRG1 : (PWM Offset: 0x42C) (R/W 32) PWM External Trigger Register (trg_num = 1) -------- */
#define PWM_ETRG1_MAXCNT_Pos                  (0U)                                           /**< (PWM_ETRG1) Maximum Counter value Position */
#define PWM_ETRG1_MAXCNT_Msk                  (0xFFFFFFU << PWM_ETRG1_MAXCNT_Pos)            /**< (PWM_ETRG1) Maximum Counter value Mask */
#define PWM_ETRG1_MAXCNT(value)               (PWM_ETRG1_MAXCNT_Msk & ((value) << PWM_ETRG1_MAXCNT_Pos))
#define PWM_ETRG1_TRGMODE_Pos                 (24U)                                          /**< (PWM_ETRG1) External Trigger Mode Position */
#define PWM_ETRG1_TRGMODE_Msk                 (0x3U << PWM_ETRG1_TRGMODE_Pos)                /**< (PWM_ETRG1) External Trigger Mode Mask */
#define PWM_ETRG1_TRGMODE(value)              (PWM_ETRG1_TRGMODE_Msk & ((value) << PWM_ETRG1_TRGMODE_Pos))
#define   PWM_ETRG1_TRGMODE_OFF_Val           (0U)                                           /**< (PWM_ETRG1) External trigger is not enabled.  */
#define   PWM_ETRG1_TRGMODE_MODE1_Val         (1U)                                           /**< (PWM_ETRG1) External PWM Reset Mode  */
#define   PWM_ETRG1_TRGMODE_MODE2_Val         (2U)                                           /**< (PWM_ETRG1) External PWM Start Mode  */
#define   PWM_ETRG1_TRGMODE_MODE3_Val         (3U)                                           /**< (PWM_ETRG1) Cycle-by-cycle Duty Mode  */
#define PWM_ETRG1_TRGMODE_OFF                 (PWM_ETRG1_TRGMODE_OFF_Val << PWM_ETRG1_TRGMODE_Pos) /**< (PWM_ETRG1) External trigger is not enabled. Position  */
#define PWM_ETRG1_TRGMODE_MODE1               (PWM_ETRG1_TRGMODE_MODE1_Val << PWM_ETRG1_TRGMODE_Pos) /**< (PWM_ETRG1) External PWM Reset Mode Position  */
#define PWM_ETRG1_TRGMODE_MODE2               (PWM_ETRG1_TRGMODE_MODE2_Val << PWM_ETRG1_TRGMODE_Pos) /**< (PWM_ETRG1) External PWM Start Mode Position  */
#define PWM_ETRG1_TRGMODE_MODE3               (PWM_ETRG1_TRGMODE_MODE3_Val << PWM_ETRG1_TRGMODE_Pos) /**< (PWM_ETRG1) Cycle-by-cycle Duty Mode Position  */
#define PWM_ETRG1_TRGEDGE_Pos                 (28U)                                          /**< (PWM_ETRG1) Edge Selection Position */
#define PWM_ETRG1_TRGEDGE_Msk                 (0x1U << PWM_ETRG1_TRGEDGE_Pos)                /**< (PWM_ETRG1) Edge Selection Mask */
#define PWM_ETRG1_TRGEDGE(value)              (PWM_ETRG1_TRGEDGE_Msk & ((value) << PWM_ETRG1_TRGEDGE_Pos))
#define   PWM_ETRG1_TRGEDGE_FALLING_ZERO_Val  (0U)                                           /**< (PWM_ETRG1) TRGMODE = 1: TRGINx event detection on falling edge.TRGMODE = 2, 3: TRGINx active level is 0  */
#define   PWM_ETRG1_TRGEDGE_RISING_ONE_Val    (1U)                                           /**< (PWM_ETRG1) TRGMODE = 1: TRGINx event detection on rising edge.TRGMODE = 2, 3: TRGINx active level is 1  */
#define PWM_ETRG1_TRGEDGE_FALLING_ZERO        (PWM_ETRG1_TRGEDGE_FALLING_ZERO_Val << PWM_ETRG1_TRGEDGE_Pos) /**< (PWM_ETRG1) TRGMODE = 1: TRGINx event detection on falling edge.TRGMODE = 2, 3: TRGINx active level is 0 Position  */
#define PWM_ETRG1_TRGEDGE_RISING_ONE          (PWM_ETRG1_TRGEDGE_RISING_ONE_Val << PWM_ETRG1_TRGEDGE_Pos) /**< (PWM_ETRG1) TRGMODE = 1: TRGINx event detection on rising edge.TRGMODE = 2, 3: TRGINx active level is 1 Position  */
#define PWM_ETRG1_TRGFILT_Pos                 (29U)                                          /**< (PWM_ETRG1) Filtered input Position */
#define PWM_ETRG1_TRGFILT_Msk                 (0x1U << PWM_ETRG1_TRGFILT_Pos)                /**< (PWM_ETRG1) Filtered input Mask */
#define PWM_ETRG1_TRGFILT(value)              (PWM_ETRG1_TRGFILT_Msk & ((value) << PWM_ETRG1_TRGFILT_Pos))
#define PWM_ETRG1_TRGSRC_Pos                  (30U)                                          /**< (PWM_ETRG1) Trigger Source Position */
#define PWM_ETRG1_TRGSRC_Msk                  (0x1U << PWM_ETRG1_TRGSRC_Pos)                 /**< (PWM_ETRG1) Trigger Source Mask */
#define PWM_ETRG1_TRGSRC(value)               (PWM_ETRG1_TRGSRC_Msk & ((value) << PWM_ETRG1_TRGSRC_Pos))
#define PWM_ETRG1_RFEN_Pos                    (31U)                                          /**< (PWM_ETRG1) Recoverable Fault Enable Position */
#define PWM_ETRG1_RFEN_Msk                    (0x1U << PWM_ETRG1_RFEN_Pos)                   /**< (PWM_ETRG1) Recoverable Fault Enable Mask */
#define PWM_ETRG1_RFEN(value)                 (PWM_ETRG1_RFEN_Msk & ((value) << PWM_ETRG1_RFEN_Pos))
#define PWM_ETRG1_Msk                         (0xF3FFFFFFUL)                                 /**< (PWM_ETRG1) Register Mask  */

/* -------- PWM_LEBR1 : (PWM Offset: 0x430) (R/W 32) PWM Leading-Edge Blanking Register (trg_num = 1) -------- */
#define PWM_LEBR1_LEBDELAY_Pos                (0U)                                           /**< (PWM_LEBR1) Leading-Edge Blanking Delay for TRGINx Position */
#define PWM_LEBR1_LEBDELAY_Msk                (0x7FU << PWM_LEBR1_LEBDELAY_Pos)              /**< (PWM_LEBR1) Leading-Edge Blanking Delay for TRGINx Mask */
#define PWM_LEBR1_LEBDELAY(value)             (PWM_LEBR1_LEBDELAY_Msk & ((value) << PWM_LEBR1_LEBDELAY_Pos))
#define PWM_LEBR1_PWMLFEN_Pos                 (16U)                                          /**< (PWM_LEBR1) PWML Falling Edge Enable Position */
#define PWM_LEBR1_PWMLFEN_Msk                 (0x1U << PWM_LEBR1_PWMLFEN_Pos)                /**< (PWM_LEBR1) PWML Falling Edge Enable Mask */
#define PWM_LEBR1_PWMLFEN(value)              (PWM_LEBR1_PWMLFEN_Msk & ((value) << PWM_LEBR1_PWMLFEN_Pos))
#define PWM_LEBR1_PWMLREN_Pos                 (17U)                                          /**< (PWM_LEBR1) PWML Rising Edge Enable Position */
#define PWM_LEBR1_PWMLREN_Msk                 (0x1U << PWM_LEBR1_PWMLREN_Pos)                /**< (PWM_LEBR1) PWML Rising Edge Enable Mask */
#define PWM_LEBR1_PWMLREN(value)              (PWM_LEBR1_PWMLREN_Msk & ((value) << PWM_LEBR1_PWMLREN_Pos))
#define PWM_LEBR1_PWMHFEN_Pos                 (18U)                                          /**< (PWM_LEBR1) PWMH Falling Edge Enable Position */
#define PWM_LEBR1_PWMHFEN_Msk                 (0x1U << PWM_LEBR1_PWMHFEN_Pos)                /**< (PWM_LEBR1) PWMH Falling Edge Enable Mask */
#define PWM_LEBR1_PWMHFEN(value)              (PWM_LEBR1_PWMHFEN_Msk & ((value) << PWM_LEBR1_PWMHFEN_Pos))
#define PWM_LEBR1_PWMHREN_Pos                 (19U)                                          /**< (PWM_LEBR1) PWMH Rising Edge Enable Position */
#define PWM_LEBR1_PWMHREN_Msk                 (0x1U << PWM_LEBR1_PWMHREN_Pos)                /**< (PWM_LEBR1) PWMH Rising Edge Enable Mask */
#define PWM_LEBR1_PWMHREN(value)              (PWM_LEBR1_PWMHREN_Msk & ((value) << PWM_LEBR1_PWMHREN_Pos))
#define PWM_LEBR1_Msk                         (0x000F007FUL)                                 /**< (PWM_LEBR1) Register Mask  */

/* -------- PWM_CMUPD2 : (PWM Offset: 0x440) ( /W 32) PWM Channel Mode Update Register (ch_num = 2) -------- */
#define PWM_CMUPD2_CPOLUP_Pos                 (9U)                                           /**< (PWM_CMUPD2) Channel Polarity Update Position */
#define PWM_CMUPD2_CPOLUP_Msk                 (0x1U << PWM_CMUPD2_CPOLUP_Pos)                /**< (PWM_CMUPD2) Channel Polarity Update Mask */
#define PWM_CMUPD2_CPOLUP(value)              (PWM_CMUPD2_CPOLUP_Msk & ((value) << PWM_CMUPD2_CPOLUP_Pos))
#define PWM_CMUPD2_CPOLINVUP_Pos              (13U)                                          /**< (PWM_CMUPD2) Channel Polarity Inversion Update Position */
#define PWM_CMUPD2_CPOLINVUP_Msk              (0x1U << PWM_CMUPD2_CPOLINVUP_Pos)             /**< (PWM_CMUPD2) Channel Polarity Inversion Update Mask */
#define PWM_CMUPD2_CPOLINVUP(value)           (PWM_CMUPD2_CPOLINVUP_Msk & ((value) << PWM_CMUPD2_CPOLINVUP_Pos))
#define PWM_CMUPD2_Msk                        (0x00002200UL)                                 /**< (PWM_CMUPD2) Register Mask  */

/* -------- PWM_ETRG2 : (PWM Offset: 0x44C) (R/W 32) PWM External Trigger Register (trg_num = 2) -------- */
#define PWM_ETRG2_MAXCNT_Pos                  (0U)                                           /**< (PWM_ETRG2) Maximum Counter value Position */
#define PWM_ETRG2_MAXCNT_Msk                  (0xFFFFFFU << PWM_ETRG2_MAXCNT_Pos)            /**< (PWM_ETRG2) Maximum Counter value Mask */
#define PWM_ETRG2_MAXCNT(value)               (PWM_ETRG2_MAXCNT_Msk & ((value) << PWM_ETRG2_MAXCNT_Pos))
#define PWM_ETRG2_TRGMODE_Pos                 (24U)                                          /**< (PWM_ETRG2) External Trigger Mode Position */
#define PWM_ETRG2_TRGMODE_Msk                 (0x3U << PWM_ETRG2_TRGMODE_Pos)                /**< (PWM_ETRG2) External Trigger Mode Mask */
#define PWM_ETRG2_TRGMODE(value)              (PWM_ETRG2_TRGMODE_Msk & ((value) << PWM_ETRG2_TRGMODE_Pos))
#define   PWM_ETRG2_TRGMODE_OFF_Val           (0U)                                           /**< (PWM_ETRG2) External trigger is not enabled.  */
#define   PWM_ETRG2_TRGMODE_MODE1_Val         (1U)                                           /**< (PWM_ETRG2) External PWM Reset Mode  */
#define   PWM_ETRG2_TRGMODE_MODE2_Val         (2U)                                           /**< (PWM_ETRG2) External PWM Start Mode  */
#define   PWM_ETRG2_TRGMODE_MODE3_Val         (3U)                                           /**< (PWM_ETRG2) Cycle-by-cycle Duty Mode  */
#define PWM_ETRG2_TRGMODE_OFF                 (PWM_ETRG2_TRGMODE_OFF_Val << PWM_ETRG2_TRGMODE_Pos) /**< (PWM_ETRG2) External trigger is not enabled. Position  */
#define PWM_ETRG2_TRGMODE_MODE1               (PWM_ETRG2_TRGMODE_MODE1_Val << PWM_ETRG2_TRGMODE_Pos) /**< (PWM_ETRG2) External PWM Reset Mode Position  */
#define PWM_ETRG2_TRGMODE_MODE2               (PWM_ETRG2_TRGMODE_MODE2_Val << PWM_ETRG2_TRGMODE_Pos) /**< (PWM_ETRG2) External PWM Start Mode Position  */
#define PWM_ETRG2_TRGMODE_MODE3               (PWM_ETRG2_TRGMODE_MODE3_Val << PWM_ETRG2_TRGMODE_Pos) /**< (PWM_ETRG2) Cycle-by-cycle Duty Mode Position  */
#define PWM_ETRG2_TRGEDGE_Pos                 (28U)                                          /**< (PWM_ETRG2) Edge Selection Position */
#define PWM_ETRG2_TRGEDGE_Msk                 (0x1U << PWM_ETRG2_TRGEDGE_Pos)                /**< (PWM_ETRG2) Edge Selection Mask */
#define PWM_ETRG2_TRGEDGE(value)              (PWM_ETRG2_TRGEDGE_Msk & ((value) << PWM_ETRG2_TRGEDGE_Pos))
#define   PWM_ETRG2_TRGEDGE_FALLING_ZERO_Val  (0U)                                           /**< (PWM_ETRG2) TRGMODE = 1: TRGINx event detection on falling edge.TRGMODE = 2, 3: TRGINx active level is 0  */
#define   PWM_ETRG2_TRGEDGE_RISING_ONE_Val    (1U)                                           /**< (PWM_ETRG2) TRGMODE = 1: TRGINx event detection on rising edge.TRGMODE = 2, 3: TRGINx active level is 1  */
#define PWM_ETRG2_TRGEDGE_FALLING_ZERO        (PWM_ETRG2_TRGEDGE_FALLING_ZERO_Val << PWM_ETRG2_TRGEDGE_Pos) /**< (PWM_ETRG2) TRGMODE = 1: TRGINx event detection on falling edge.TRGMODE = 2, 3: TRGINx active level is 0 Position  */
#define PWM_ETRG2_TRGEDGE_RISING_ONE          (PWM_ETRG2_TRGEDGE_RISING_ONE_Val << PWM_ETRG2_TRGEDGE_Pos) /**< (PWM_ETRG2) TRGMODE = 1: TRGINx event detection on rising edge.TRGMODE = 2, 3: TRGINx active level is 1 Position  */
#define PWM_ETRG2_TRGFILT_Pos                 (29U)                                          /**< (PWM_ETRG2) Filtered input Position */
#define PWM_ETRG2_TRGFILT_Msk                 (0x1U << PWM_ETRG2_TRGFILT_Pos)                /**< (PWM_ETRG2) Filtered input Mask */
#define PWM_ETRG2_TRGFILT(value)              (PWM_ETRG2_TRGFILT_Msk & ((value) << PWM_ETRG2_TRGFILT_Pos))
#define PWM_ETRG2_TRGSRC_Pos                  (30U)                                          /**< (PWM_ETRG2) Trigger Source Position */
#define PWM_ETRG2_TRGSRC_Msk                  (0x1U << PWM_ETRG2_TRGSRC_Pos)                 /**< (PWM_ETRG2) Trigger Source Mask */
#define PWM_ETRG2_TRGSRC(value)               (PWM_ETRG2_TRGSRC_Msk & ((value) << PWM_ETRG2_TRGSRC_Pos))
#define PWM_ETRG2_RFEN_Pos                    (31U)                                          /**< (PWM_ETRG2) Recoverable Fault Enable Position */
#define PWM_ETRG2_RFEN_Msk                    (0x1U << PWM_ETRG2_RFEN_Pos)                   /**< (PWM_ETRG2) Recoverable Fault Enable Mask */
#define PWM_ETRG2_RFEN(value)                 (PWM_ETRG2_RFEN_Msk & ((value) << PWM_ETRG2_RFEN_Pos))
#define PWM_ETRG2_Msk                         (0xF3FFFFFFUL)                                 /**< (PWM_ETRG2) Register Mask  */

/* -------- PWM_LEBR2 : (PWM Offset: 0x450) (R/W 32) PWM Leading-Edge Blanking Register (trg_num = 2) -------- */
#define PWM_LEBR2_LEBDELAY_Pos                (0U)                                           /**< (PWM_LEBR2) Leading-Edge Blanking Delay for TRGINx Position */
#define PWM_LEBR2_LEBDELAY_Msk                (0x7FU << PWM_LEBR2_LEBDELAY_Pos)              /**< (PWM_LEBR2) Leading-Edge Blanking Delay for TRGINx Mask */
#define PWM_LEBR2_LEBDELAY(value)             (PWM_LEBR2_LEBDELAY_Msk & ((value) << PWM_LEBR2_LEBDELAY_Pos))
#define PWM_LEBR2_PWMLFEN_Pos                 (16U)                                          /**< (PWM_LEBR2) PWML Falling Edge Enable Position */
#define PWM_LEBR2_PWMLFEN_Msk                 (0x1U << PWM_LEBR2_PWMLFEN_Pos)                /**< (PWM_LEBR2) PWML Falling Edge Enable Mask */
#define PWM_LEBR2_PWMLFEN(value)              (PWM_LEBR2_PWMLFEN_Msk & ((value) << PWM_LEBR2_PWMLFEN_Pos))
#define PWM_LEBR2_PWMLREN_Pos                 (17U)                                          /**< (PWM_LEBR2) PWML Rising Edge Enable Position */
#define PWM_LEBR2_PWMLREN_Msk                 (0x1U << PWM_LEBR2_PWMLREN_Pos)                /**< (PWM_LEBR2) PWML Rising Edge Enable Mask */
#define PWM_LEBR2_PWMLREN(value)              (PWM_LEBR2_PWMLREN_Msk & ((value) << PWM_LEBR2_PWMLREN_Pos))
#define PWM_LEBR2_PWMHFEN_Pos                 (18U)                                          /**< (PWM_LEBR2) PWMH Falling Edge Enable Position */
#define PWM_LEBR2_PWMHFEN_Msk                 (0x1U << PWM_LEBR2_PWMHFEN_Pos)                /**< (PWM_LEBR2) PWMH Falling Edge Enable Mask */
#define PWM_LEBR2_PWMHFEN(value)              (PWM_LEBR2_PWMHFEN_Msk & ((value) << PWM_LEBR2_PWMHFEN_Pos))
#define PWM_LEBR2_PWMHREN_Pos                 (19U)                                          /**< (PWM_LEBR2) PWMH Rising Edge Enable Position */
#define PWM_LEBR2_PWMHREN_Msk                 (0x1U << PWM_LEBR2_PWMHREN_Pos)                /**< (PWM_LEBR2) PWMH Rising Edge Enable Mask */
#define PWM_LEBR2_PWMHREN(value)              (PWM_LEBR2_PWMHREN_Msk & ((value) << PWM_LEBR2_PWMHREN_Pos))
#define PWM_LEBR2_Msk                         (0x000F007FUL)                                 /**< (PWM_LEBR2) Register Mask  */

/* -------- PWM_CMUPD3 : (PWM Offset: 0x460) ( /W 32) PWM Channel Mode Update Register (ch_num = 3) -------- */
#define PWM_CMUPD3_CPOLUP_Pos                 (9U)                                           /**< (PWM_CMUPD3) Channel Polarity Update Position */
#define PWM_CMUPD3_CPOLUP_Msk                 (0x1U << PWM_CMUPD3_CPOLUP_Pos)                /**< (PWM_CMUPD3) Channel Polarity Update Mask */
#define PWM_CMUPD3_CPOLUP(value)              (PWM_CMUPD3_CPOLUP_Msk & ((value) << PWM_CMUPD3_CPOLUP_Pos))
#define PWM_CMUPD3_CPOLINVUP_Pos              (13U)                                          /**< (PWM_CMUPD3) Channel Polarity Inversion Update Position */
#define PWM_CMUPD3_CPOLINVUP_Msk              (0x1U << PWM_CMUPD3_CPOLINVUP_Pos)             /**< (PWM_CMUPD3) Channel Polarity Inversion Update Mask */
#define PWM_CMUPD3_CPOLINVUP(value)           (PWM_CMUPD3_CPOLINVUP_Msk & ((value) << PWM_CMUPD3_CPOLINVUP_Pos))
#define PWM_CMUPD3_Msk                        (0x00002200UL)                                 /**< (PWM_CMUPD3) Register Mask  */

/** \brief PWM register offsets definitions */
#define PWM_CLK_OFFSET                 (0x00)         /**< (PWM_CLK) PWM Clock Register Offset */
#define PWM_ENA_OFFSET                 (0x04)         /**< (PWM_ENA) PWM Enable Register Offset */
#define PWM_DIS_OFFSET                 (0x08)         /**< (PWM_DIS) PWM Disable Register Offset */
#define PWM_SR_OFFSET                  (0x0C)         /**< (PWM_SR) PWM Status Register Offset */
#define PWM_IER1_OFFSET                (0x10)         /**< (PWM_IER1) PWM Interrupt Enable Register 1 Offset */
#define PWM_IDR1_OFFSET                (0x14)         /**< (PWM_IDR1) PWM Interrupt Disable Register 1 Offset */
#define PWM_IMR1_OFFSET                (0x18)         /**< (PWM_IMR1) PWM Interrupt Mask Register 1 Offset */
#define PWM_ISR1_OFFSET                (0x1C)         /**< (PWM_ISR1) PWM Interrupt Status Register 1 Offset */
#define PWM_SCM_OFFSET                 (0x20)         /**< (PWM_SCM) PWM Sync Channels Mode Register Offset */
#define PWM_DMAR_OFFSET                (0x24)         /**< (PWM_DMAR) PWM DMA Register Offset */
#define PWM_SCUC_OFFSET                (0x28)         /**< (PWM_SCUC) PWM Sync Channels Update Control Register Offset */
#define PWM_SCUP_OFFSET                (0x2C)         /**< (PWM_SCUP) PWM Sync Channels Update Period Register Offset */
#define PWM_SCUPUPD_OFFSET             (0x30)         /**< (PWM_SCUPUPD) PWM Sync Channels Update Period Update Register Offset */
#define PWM_IER2_OFFSET                (0x34)         /**< (PWM_IER2) PWM Interrupt Enable Register 2 Offset */
#define PWM_IDR2_OFFSET                (0x38)         /**< (PWM_IDR2) PWM Interrupt Disable Register 2 Offset */
#define PWM_IMR2_OFFSET                (0x3C)         /**< (PWM_IMR2) PWM Interrupt Mask Register 2 Offset */
#define PWM_ISR2_OFFSET                (0x40)         /**< (PWM_ISR2) PWM Interrupt Status Register 2 Offset */
#define PWM_OOV_OFFSET                 (0x44)         /**< (PWM_OOV) PWM Output Override Value Register Offset */
#define PWM_OS_OFFSET                  (0x48)         /**< (PWM_OS) PWM Output Selection Register Offset */
#define PWM_OSS_OFFSET                 (0x4C)         /**< (PWM_OSS) PWM Output Selection Set Register Offset */
#define PWM_OSC_OFFSET                 (0x50)         /**< (PWM_OSC) PWM Output Selection Clear Register Offset */
#define PWM_OSSUPD_OFFSET              (0x54)         /**< (PWM_OSSUPD) PWM Output Selection Set Update Register Offset */
#define PWM_OSCUPD_OFFSET              (0x58)         /**< (PWM_OSCUPD) PWM Output Selection Clear Update Register Offset */
#define PWM_FMR_OFFSET                 (0x5C)         /**< (PWM_FMR) PWM Fault Mode Register Offset */
#define PWM_FSR_OFFSET                 (0x60)         /**< (PWM_FSR) PWM Fault Status Register Offset */
#define PWM_FCR_OFFSET                 (0x64)         /**< (PWM_FCR) PWM Fault Clear Register Offset */
#define PWM_FPV1_OFFSET                (0x68)         /**< (PWM_FPV1) PWM Fault Protection Value Register 1 Offset */
#define PWM_FPE_OFFSET                 (0x6C)         /**< (PWM_FPE) PWM Fault Protection Enable Register Offset */
#define PWM_ELMR_OFFSET                (0x7C)         /**< (PWM_ELMR) PWM Event Line 0 Mode Register 0 Offset */
#define PWM_SSPR_OFFSET                (0xA0)         /**< (PWM_SSPR) PWM Spread Spectrum Register Offset */
#define PWM_SSPUP_OFFSET               (0xA4)         /**< (PWM_SSPUP) PWM Spread Spectrum Update Register Offset */
#define PWM_SMMR_OFFSET                (0xB0)         /**< (PWM_SMMR) PWM Stepper Motor Mode Register Offset */
#define PWM_FPV2_OFFSET                (0xC0)         /**< (PWM_FPV2) PWM Fault Protection Value 2 Register Offset */
#define PWM_WPCR_OFFSET                (0xE4)         /**< (PWM_WPCR) PWM Write Protection Control Register Offset */
#define PWM_WPSR_OFFSET                (0xE8)         /**< (PWM_WPSR) PWM Write Protection Status Register Offset */
#define PWM_CMP_OFFSET                 (0x130)        /**< (PWM_CMP) PWM Comparison 0 Value Register Offset */
  #define PWM_CMPV_OFFSET                (0x00)         /**< (PWM_CMPV) PWM Comparison 0 Value Register Offset */
  #define PWM_CMPVUPD_OFFSET             (0x04)         /**< (PWM_CMPVUPD) PWM Comparison 0 Value Update Register Offset */
  #define PWM_CMPM_OFFSET                (0x08)         /**< (PWM_CMPM) PWM Comparison 0 Mode Register Offset */
  #define PWM_CMPMUPD_OFFSET             (0x0C)         /**< (PWM_CMPMUPD) PWM Comparison 0 Mode Update Register Offset */
#define PWM_CH_NUM_OFFSET              (0x200)        /**< (PWM_CH_NUM) PWM Channel Mode Register Offset */
  #define PWM_CMR_OFFSET                 (0x00)         /**< (PWM_CMR) PWM Channel Mode Register Offset */
  #define PWM_CDTY_OFFSET                (0x04)         /**< (PWM_CDTY) PWM Channel Duty Cycle Register Offset */
  #define PWM_CDTYUPD_OFFSET             (0x08)         /**< (PWM_CDTYUPD) PWM Channel Duty Cycle Update Register Offset */
  #define PWM_CPRD_OFFSET                (0x0C)         /**< (PWM_CPRD) PWM Channel Period Register Offset */
  #define PWM_CPRDUPD_OFFSET             (0x10)         /**< (PWM_CPRDUPD) PWM Channel Period Update Register Offset */
  #define PWM_CCNT_OFFSET                (0x14)         /**< (PWM_CCNT) PWM Channel Counter Register Offset */
  #define PWM_DT_OFFSET                  (0x18)         /**< (PWM_DT) PWM Channel Dead Time Register Offset */
  #define PWM_DTUPD_OFFSET               (0x1C)         /**< (PWM_DTUPD) PWM Channel Dead Time Update Register Offset */
#define PWM_CMUPD0_OFFSET              (0x400)        /**< (PWM_CMUPD0) PWM Channel Mode Update Register (ch_num = 0) Offset */
#define PWM_CMUPD1_OFFSET              (0x420)        /**< (PWM_CMUPD1) PWM Channel Mode Update Register (ch_num = 1) Offset */
#define PWM_ETRG1_OFFSET               (0x42C)        /**< (PWM_ETRG1) PWM External Trigger Register (trg_num = 1) Offset */
#define PWM_LEBR1_OFFSET               (0x430)        /**< (PWM_LEBR1) PWM Leading-Edge Blanking Register (trg_num = 1) Offset */
#define PWM_CMUPD2_OFFSET              (0x440)        /**< (PWM_CMUPD2) PWM Channel Mode Update Register (ch_num = 2) Offset */
#define PWM_ETRG2_OFFSET               (0x44C)        /**< (PWM_ETRG2) PWM External Trigger Register (trg_num = 2) Offset */
#define PWM_LEBR2_OFFSET               (0x450)        /**< (PWM_LEBR2) PWM Leading-Edge Blanking Register (trg_num = 2) Offset */
#define PWM_CMUPD3_OFFSET              (0x460)        /**< (PWM_CMUPD3) PWM Channel Mode Update Register (ch_num = 3) Offset */

/** \brief PWM_CMP register API structure */
typedef struct
{
  __IO  uint32_t                       PWM_CMPV;        /**< Offset: 0x00 (R/W  32) PWM Comparison 0 Value Register */
  __O   uint32_t                       PWM_CMPVUPD;     /**< Offset: 0x04 ( /W  32) PWM Comparison 0 Value Update Register */
  __IO  uint32_t                       PWM_CMPM;        /**< Offset: 0x08 (R/W  32) PWM Comparison 0 Mode Register */
  __O   uint32_t                       PWM_CMPMUPD;     /**< Offset: 0x0c ( /W  32) PWM Comparison 0 Mode Update Register */
} pwm_cmp_registers_t;
#define PWM_CMP_NUMBER (8U)

/** \brief PWM_CH_NUM register API structure */
typedef struct
{
  __IO  uint32_t                       PWM_CMR;         /**< Offset: 0x00 (R/W  32) PWM Channel Mode Register */
  __IO  uint32_t                       PWM_CDTY;        /**< Offset: 0x04 (R/W  32) PWM Channel Duty Cycle Register */
  __O   uint32_t                       PWM_CDTYUPD;     /**< Offset: 0x08 ( /W  32) PWM Channel Duty Cycle Update Register */
  __IO  uint32_t                       PWM_CPRD;        /**< Offset: 0x0c (R/W  32) PWM Channel Period Register */
  __O   uint32_t                       PWM_CPRDUPD;     /**< Offset: 0x10 ( /W  32) PWM Channel Period Update Register */
  __I   uint32_t                       PWM_CCNT;        /**< Offset: 0x14 (R/   32) PWM Channel Counter Register */
  __IO  uint32_t                       PWM_DT;          /**< Offset: 0x18 (R/W  32) PWM Channel Dead Time Register */
  __O   uint32_t                       PWM_DTUPD;       /**< Offset: 0x1c ( /W  32) PWM Channel Dead Time Update Register */
} pwm_ch_num_registers_t;
#define PWM_CH_NUM_NUMBER (4U)

/** \brief PWM register API structure */
typedef struct
{
  __IO  uint32_t                       PWM_CLK;         /**< Offset: 0x00 (R/W  32) PWM Clock Register */
  __O   uint32_t                       PWM_ENA;         /**< Offset: 0x04 ( /W  32) PWM Enable Register */
  __O   uint32_t                       PWM_DIS;         /**< Offset: 0x08 ( /W  32) PWM Disable Register */
  __I   uint32_t                       PWM_SR;          /**< Offset: 0x0c (R/   32) PWM Status Register */
  __O   uint32_t                       PWM_IER1;        /**< Offset: 0x10 ( /W  32) PWM Interrupt Enable Register 1 */
  __O   uint32_t                       PWM_IDR1;        /**< Offset: 0x14 ( /W  32) PWM Interrupt Disable Register 1 */
  __I   uint32_t                       PWM_IMR1;        /**< Offset: 0x18 (R/   32) PWM Interrupt Mask Register 1 */
  __I   uint32_t                       PWM_ISR1;        /**< Offset: 0x1c (R/   32) PWM Interrupt Status Register 1 */
  __IO  uint32_t                       PWM_SCM;         /**< Offset: 0x20 (R/W  32) PWM Sync Channels Mode Register */
  __O   uint32_t                       PWM_DMAR;        /**< Offset: 0x24 ( /W  32) PWM DMA Register */
  __IO  uint32_t                       PWM_SCUC;        /**< Offset: 0x28 (R/W  32) PWM Sync Channels Update Control Register */
  __IO  uint32_t                       PWM_SCUP;        /**< Offset: 0x2c (R/W  32) PWM Sync Channels Update Period Register */
  __O   uint32_t                       PWM_SCUPUPD;     /**< Offset: 0x30 ( /W  32) PWM Sync Channels Update Period Update Register */
  __O   uint32_t                       PWM_IER2;        /**< Offset: 0x34 ( /W  32) PWM Interrupt Enable Register 2 */
  __O   uint32_t                       PWM_IDR2;        /**< Offset: 0x38 ( /W  32) PWM Interrupt Disable Register 2 */
  __I   uint32_t                       PWM_IMR2;        /**< Offset: 0x3c (R/   32) PWM Interrupt Mask Register 2 */
  __I   uint32_t                       PWM_ISR2;        /**< Offset: 0x40 (R/   32) PWM Interrupt Status Register 2 */
  __IO  uint32_t                       PWM_OOV;         /**< Offset: 0x44 (R/W  32) PWM Output Override Value Register */
  __IO  uint32_t                       PWM_OS;          /**< Offset: 0x48 (R/W  32) PWM Output Selection Register */
  __O   uint32_t                       PWM_OSS;         /**< Offset: 0x4c ( /W  32) PWM Output Selection Set Register */
  __O   uint32_t                       PWM_OSC;         /**< Offset: 0x50 ( /W  32) PWM Output Selection Clear Register */
  __O   uint32_t                       PWM_OSSUPD;      /**< Offset: 0x54 ( /W  32) PWM Output Selection Set Update Register */
  __O   uint32_t                       PWM_OSCUPD;      /**< Offset: 0x58 ( /W  32) PWM Output Selection Clear Update Register */
  __IO  uint32_t                       PWM_FMR;         /**< Offset: 0x5c (R/W  32) PWM Fault Mode Register */
  __I   uint32_t                       PWM_FSR;         /**< Offset: 0x60 (R/   32) PWM Fault Status Register */
  __O   uint32_t                       PWM_FCR;         /**< Offset: 0x64 ( /W  32) PWM Fault Clear Register */
  __IO  uint32_t                       PWM_FPV1;        /**< Offset: 0x68 (R/W  32) PWM Fault Protection Value Register 1 */
  __IO  uint32_t                       PWM_FPE;         /**< Offset: 0x6c (R/W  32) PWM Fault Protection Enable Register */
  __I   uint8_t                        Reserved1[0x0C];
  __IO  uint32_t                       PWM_ELMR[2];     /**< Offset: 0x7c (R/W  32) PWM Event Line 0 Mode Register 0 */
  __I   uint8_t                        Reserved2[0x1C];
  __IO  uint32_t                       PWM_SSPR;        /**< Offset: 0xa0 (R/W  32) PWM Spread Spectrum Register */
  __O   uint32_t                       PWM_SSPUP;       /**< Offset: 0xa4 ( /W  32) PWM Spread Spectrum Update Register */
  __I   uint8_t                        Reserved3[0x08];
  __IO  uint32_t                       PWM_SMMR;        /**< Offset: 0xb0 (R/W  32) PWM Stepper Motor Mode Register */
  __I   uint8_t                        Reserved4[0x0C];
  __IO  uint32_t                       PWM_FPV2;        /**< Offset: 0xc0 (R/W  32) PWM Fault Protection Value 2 Register */
  __I   uint8_t                        Reserved5[0x20];
  __O   uint32_t                       PWM_WPCR;        /**< Offset: 0xe4 ( /W  32) PWM Write Protection Control Register */
  __I   uint32_t                       PWM_WPSR;        /**< Offset: 0xe8 (R/   32) PWM Write Protection Status Register */
  __I   uint8_t                        Reserved6[0x44];
        pwm_cmp_registers_t            PWM_CMP[PWM_CMP_NUMBER]; /**< Offset: 0x130 PWM Comparison 0 Value Register */
  __I   uint8_t                        Reserved7[0x50];
        pwm_ch_num_registers_t         PWM_CH_NUM[PWM_CH_NUM_NUMBER]; /**< Offset: 0x200 PWM Channel Mode Register */
  __I   uint8_t                        Reserved8[0x180];
  __O   uint32_t                       PWM_CMUPD0;      /**< Offset: 0x400 ( /W  32) PWM Channel Mode Update Register (ch_num = 0) */
  __I   uint8_t                        Reserved9[0x1C];
  __O   uint32_t                       PWM_CMUPD1;      /**< Offset: 0x420 ( /W  32) PWM Channel Mode Update Register (ch_num = 1) */
  __I   uint8_t                        Reserved10[0x08];
  __IO  uint32_t                       PWM_ETRG1;       /**< Offset: 0x42c (R/W  32) PWM External Trigger Register (trg_num = 1) */
  __IO  uint32_t                       PWM_LEBR1;       /**< Offset: 0x430 (R/W  32) PWM Leading-Edge Blanking Register (trg_num = 1) */
  __I   uint8_t                        Reserved11[0x0C];
  __O   uint32_t                       PWM_CMUPD2;      /**< Offset: 0x440 ( /W  32) PWM Channel Mode Update Register (ch_num = 2) */
  __I   uint8_t                        Reserved12[0x08];
  __IO  uint32_t                       PWM_ETRG2;       /**< Offset: 0x44c (R/W  32) PWM External Trigger Register (trg_num = 2) */
  __IO  uint32_t                       PWM_LEBR2;       /**< Offset: 0x450 (R/W  32) PWM Leading-Edge Blanking Register (trg_num = 2) */
  __I   uint8_t                        Reserved13[0x0C];
  __O   uint32_t                       PWM_CMUPD3;      /**< Offset: 0x460 ( /W  32) PWM Channel Mode Update Register (ch_num = 3) */
} pwm_registers_t;
/** @}  end of Pulse Width Modulation Controller */

#endif /* SAME_SAME70_PWM_MODULE_H */

