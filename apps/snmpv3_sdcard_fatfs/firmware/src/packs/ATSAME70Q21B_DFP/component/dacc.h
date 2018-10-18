/**
 * \brief Header file for SAME/SAME70 DACC
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
#ifndef SAME_SAME70_DACC_MODULE_H
#define SAME_SAME70_DACC_MODULE_H

/** \addtogroup SAME_SAME70 Digital-to-Analog Converter Controller
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_DACC                                  */
/* ========================================================================== */

/* -------- DACC_CR : (DACC Offset: 0x00) ( /W 32) Control Register -------- */
#define DACC_CR_SWRST_Pos                     (0U)                                           /**< (DACC_CR) Software Reset Position */
#define DACC_CR_SWRST_Msk                     (0x1U << DACC_CR_SWRST_Pos)                    /**< (DACC_CR) Software Reset Mask */
#define DACC_CR_SWRST(value)                  (DACC_CR_SWRST_Msk & ((value) << DACC_CR_SWRST_Pos))
#define DACC_CR_Msk                           (0x00000001UL)                                 /**< (DACC_CR) Register Mask  */

/* -------- DACC_MR : (DACC Offset: 0x04) (R/W 32) Mode Register -------- */
#define DACC_MR_MAXS0_Pos                     (0U)                                           /**< (DACC_MR) Max Speed Mode for Channel 0 Position */
#define DACC_MR_MAXS0_Msk                     (0x1U << DACC_MR_MAXS0_Pos)                    /**< (DACC_MR) Max Speed Mode for Channel 0 Mask */
#define DACC_MR_MAXS0(value)                  (DACC_MR_MAXS0_Msk & ((value) << DACC_MR_MAXS0_Pos))
#define   DACC_MR_MAXS0_TRIG_EVENT_Val        (0U)                                           /**< (DACC_MR) External trigger mode or Free-running mode enabled. (See TRGENx.DACC_TRIGR.)  */
#define   DACC_MR_MAXS0_MAXIMUM_Val           (1U)                                           /**< (DACC_MR) Max speed mode enabled.  */
#define DACC_MR_MAXS0_TRIG_EVENT              (DACC_MR_MAXS0_TRIG_EVENT_Val << DACC_MR_MAXS0_Pos) /**< (DACC_MR) External trigger mode or Free-running mode enabled. (See TRGENx.DACC_TRIGR.) Position  */
#define DACC_MR_MAXS0_MAXIMUM                 (DACC_MR_MAXS0_MAXIMUM_Val << DACC_MR_MAXS0_Pos) /**< (DACC_MR) Max speed mode enabled. Position  */
#define DACC_MR_MAXS1_Pos                     (1U)                                           /**< (DACC_MR) Max Speed Mode for Channel 1 Position */
#define DACC_MR_MAXS1_Msk                     (0x1U << DACC_MR_MAXS1_Pos)                    /**< (DACC_MR) Max Speed Mode for Channel 1 Mask */
#define DACC_MR_MAXS1(value)                  (DACC_MR_MAXS1_Msk & ((value) << DACC_MR_MAXS1_Pos))
#define   DACC_MR_MAXS1_TRIG_EVENT_Val        (0U)                                           /**< (DACC_MR) External trigger mode or Free-running mode enabled. (See TRGENx.DACC_TRIGR.)  */
#define   DACC_MR_MAXS1_MAXIMUM_Val           (1U)                                           /**< (DACC_MR) Max speed mode enabled.  */
#define DACC_MR_MAXS1_TRIG_EVENT              (DACC_MR_MAXS1_TRIG_EVENT_Val << DACC_MR_MAXS1_Pos) /**< (DACC_MR) External trigger mode or Free-running mode enabled. (See TRGENx.DACC_TRIGR.) Position  */
#define DACC_MR_MAXS1_MAXIMUM                 (DACC_MR_MAXS1_MAXIMUM_Val << DACC_MR_MAXS1_Pos) /**< (DACC_MR) Max speed mode enabled. Position  */
#define DACC_MR_WORD_Pos                      (4U)                                           /**< (DACC_MR) Word Transfer Mode Position */
#define DACC_MR_WORD_Msk                      (0x1U << DACC_MR_WORD_Pos)                     /**< (DACC_MR) Word Transfer Mode Mask */
#define DACC_MR_WORD(value)                   (DACC_MR_WORD_Msk & ((value) << DACC_MR_WORD_Pos))
#define   DACC_MR_WORD_DISABLED_Val           (0U)                                           /**< (DACC_MR) One data to convert is written to the FIFO per access to DACC.  */
#define   DACC_MR_WORD_ENABLED_Val            (1U)                                           /**< (DACC_MR) Two data to convert are written to the FIFO per access to DACC (reduces the number of requests to DMA and the number of system bus accesses).  */
#define DACC_MR_WORD_DISABLED                 (DACC_MR_WORD_DISABLED_Val << DACC_MR_WORD_Pos) /**< (DACC_MR) One data to convert is written to the FIFO per access to DACC. Position  */
#define DACC_MR_WORD_ENABLED                  (DACC_MR_WORD_ENABLED_Val << DACC_MR_WORD_Pos) /**< (DACC_MR) Two data to convert are written to the FIFO per access to DACC (reduces the number of requests to DMA and the number of system bus accesses). Position  */
#define DACC_MR_ZERO_Pos                      (5U)                                           /**< (DACC_MR) Must always be written to 0. Position */
#define DACC_MR_ZERO_Msk                      (0x1U << DACC_MR_ZERO_Pos)                     /**< (DACC_MR) Must always be written to 0. Mask */
#define DACC_MR_ZERO(value)                   (DACC_MR_ZERO_Msk & ((value) << DACC_MR_ZERO_Pos))
#define DACC_MR_DIFF_Pos                      (23U)                                          /**< (DACC_MR) Differential Mode Position */
#define DACC_MR_DIFF_Msk                      (0x1U << DACC_MR_DIFF_Pos)                     /**< (DACC_MR) Differential Mode Mask */
#define DACC_MR_DIFF(value)                   (DACC_MR_DIFF_Msk & ((value) << DACC_MR_DIFF_Pos))
#define   DACC_MR_DIFF_DISABLED_Val           (0U)                                           /**< (DACC_MR) DAC0 and DAC1 are single-ended outputs.  */
#define   DACC_MR_DIFF_ENABLED_Val            (1U)                                           /**< (DACC_MR) DACP and DACN are differential outputs. The differential level is configured by the channel 0 value.  */
#define DACC_MR_DIFF_DISABLED                 (DACC_MR_DIFF_DISABLED_Val << DACC_MR_DIFF_Pos) /**< (DACC_MR) DAC0 and DAC1 are single-ended outputs. Position  */
#define DACC_MR_DIFF_ENABLED                  (DACC_MR_DIFF_ENABLED_Val << DACC_MR_DIFF_Pos) /**< (DACC_MR) DACP and DACN are differential outputs. The differential level is configured by the channel 0 value. Position  */
#define DACC_MR_PRESCALER_Pos                 (24U)                                          /**< (DACC_MR) Peripheral Clock to DAC Clock Ratio Position */
#define DACC_MR_PRESCALER_Msk                 (0xFU << DACC_MR_PRESCALER_Pos)                /**< (DACC_MR) Peripheral Clock to DAC Clock Ratio Mask */
#define DACC_MR_PRESCALER(value)              (DACC_MR_PRESCALER_Msk & ((value) << DACC_MR_PRESCALER_Pos))
#define DACC_MR_Msk                           (0x0F800033UL)                                 /**< (DACC_MR) Register Mask  */

/* -------- DACC_TRIGR : (DACC Offset: 0x08) (R/W 32) Trigger Register -------- */
#define DACC_TRIGR_TRGEN0_Pos                 (0U)                                           /**< (DACC_TRIGR) Trigger Enable of Channel 0 Position */
#define DACC_TRIGR_TRGEN0_Msk                 (0x1U << DACC_TRIGR_TRGEN0_Pos)                /**< (DACC_TRIGR) Trigger Enable of Channel 0 Mask */
#define DACC_TRIGR_TRGEN0(value)              (DACC_TRIGR_TRGEN0_Msk & ((value) << DACC_TRIGR_TRGEN0_Pos))
#define   DACC_TRIGR_TRGEN0_DIS_Val           (0U)                                           /**< (DACC_TRIGR) External trigger mode disabled. DACC is in Free-running mode or Max speed mode.  */
#define   DACC_TRIGR_TRGEN0_EN_Val            (1U)                                           /**< (DACC_TRIGR) External trigger mode enabled.  */
#define DACC_TRIGR_TRGEN0_DIS                 (DACC_TRIGR_TRGEN0_DIS_Val << DACC_TRIGR_TRGEN0_Pos) /**< (DACC_TRIGR) External trigger mode disabled. DACC is in Free-running mode or Max speed mode. Position  */
#define DACC_TRIGR_TRGEN0_EN                  (DACC_TRIGR_TRGEN0_EN_Val << DACC_TRIGR_TRGEN0_Pos) /**< (DACC_TRIGR) External trigger mode enabled. Position  */
#define DACC_TRIGR_TRGEN1_Pos                 (1U)                                           /**< (DACC_TRIGR) Trigger Enable of Channel 1 Position */
#define DACC_TRIGR_TRGEN1_Msk                 (0x1U << DACC_TRIGR_TRGEN1_Pos)                /**< (DACC_TRIGR) Trigger Enable of Channel 1 Mask */
#define DACC_TRIGR_TRGEN1(value)              (DACC_TRIGR_TRGEN1_Msk & ((value) << DACC_TRIGR_TRGEN1_Pos))
#define   DACC_TRIGR_TRGEN1_DIS_Val           (0U)                                           /**< (DACC_TRIGR) External trigger mode disabled. DACC is in Free-running mode or Max speed mode.  */
#define   DACC_TRIGR_TRGEN1_EN_Val            (1U)                                           /**< (DACC_TRIGR) External trigger mode enabled.  */
#define DACC_TRIGR_TRGEN1_DIS                 (DACC_TRIGR_TRGEN1_DIS_Val << DACC_TRIGR_TRGEN1_Pos) /**< (DACC_TRIGR) External trigger mode disabled. DACC is in Free-running mode or Max speed mode. Position  */
#define DACC_TRIGR_TRGEN1_EN                  (DACC_TRIGR_TRGEN1_EN_Val << DACC_TRIGR_TRGEN1_Pos) /**< (DACC_TRIGR) External trigger mode enabled. Position  */
#define DACC_TRIGR_TRGSEL0_Pos                (4U)                                           /**< (DACC_TRIGR) Trigger Selection of Channel 0 Position */
#define DACC_TRIGR_TRGSEL0_Msk                (0x7U << DACC_TRIGR_TRGSEL0_Pos)               /**< (DACC_TRIGR) Trigger Selection of Channel 0 Mask */
#define DACC_TRIGR_TRGSEL0(value)             (DACC_TRIGR_TRGSEL0_Msk & ((value) << DACC_TRIGR_TRGSEL0_Pos))
#define   DACC_TRIGR_TRGSEL0_TRGSEL0_Val      (0U)                                           /**< (DACC_TRIGR) DAC External Trigger Input (DATRG)  */
#define   DACC_TRIGR_TRGSEL0_TRGSEL1_Val      (1U)                                           /**< (DACC_TRIGR) TC0 Channel 0 Output (TIOA0)  */
#define   DACC_TRIGR_TRGSEL0_TRGSEL2_Val      (2U)                                           /**< (DACC_TRIGR) TC0 Channel 1 Output (TIOA1)  */
#define   DACC_TRIGR_TRGSEL0_TRGSEL3_Val      (3U)                                           /**< (DACC_TRIGR) TC0 Channel 2 Output (TIOA2)  */
#define   DACC_TRIGR_TRGSEL0_TRGSEL4_Val      (4U)                                           /**< (DACC_TRIGR) PWM0 Event Line 0  */
#define   DACC_TRIGR_TRGSEL0_TRGSEL5_Val      (5U)                                           /**< (DACC_TRIGR) PWM0 Event Line 1  */
#define   DACC_TRIGR_TRGSEL0_TRGSEL6_Val      (6U)                                           /**< (DACC_TRIGR) PWM1 Event Line 0  */
#define   DACC_TRIGR_TRGSEL0_TRGSEL7_Val      (7U)                                           /**< (DACC_TRIGR) PWM1 Event Line 1  */
#define DACC_TRIGR_TRGSEL0_TRGSEL0            (DACC_TRIGR_TRGSEL0_TRGSEL0_Val << DACC_TRIGR_TRGSEL0_Pos) /**< (DACC_TRIGR) DAC External Trigger Input (DATRG) Position  */
#define DACC_TRIGR_TRGSEL0_TRGSEL1            (DACC_TRIGR_TRGSEL0_TRGSEL1_Val << DACC_TRIGR_TRGSEL0_Pos) /**< (DACC_TRIGR) TC0 Channel 0 Output (TIOA0) Position  */
#define DACC_TRIGR_TRGSEL0_TRGSEL2            (DACC_TRIGR_TRGSEL0_TRGSEL2_Val << DACC_TRIGR_TRGSEL0_Pos) /**< (DACC_TRIGR) TC0 Channel 1 Output (TIOA1) Position  */
#define DACC_TRIGR_TRGSEL0_TRGSEL3            (DACC_TRIGR_TRGSEL0_TRGSEL3_Val << DACC_TRIGR_TRGSEL0_Pos) /**< (DACC_TRIGR) TC0 Channel 2 Output (TIOA2) Position  */
#define DACC_TRIGR_TRGSEL0_TRGSEL4            (DACC_TRIGR_TRGSEL0_TRGSEL4_Val << DACC_TRIGR_TRGSEL0_Pos) /**< (DACC_TRIGR) PWM0 Event Line 0 Position  */
#define DACC_TRIGR_TRGSEL0_TRGSEL5            (DACC_TRIGR_TRGSEL0_TRGSEL5_Val << DACC_TRIGR_TRGSEL0_Pos) /**< (DACC_TRIGR) PWM0 Event Line 1 Position  */
#define DACC_TRIGR_TRGSEL0_TRGSEL6            (DACC_TRIGR_TRGSEL0_TRGSEL6_Val << DACC_TRIGR_TRGSEL0_Pos) /**< (DACC_TRIGR) PWM1 Event Line 0 Position  */
#define DACC_TRIGR_TRGSEL0_TRGSEL7            (DACC_TRIGR_TRGSEL0_TRGSEL7_Val << DACC_TRIGR_TRGSEL0_Pos) /**< (DACC_TRIGR) PWM1 Event Line 1 Position  */
#define DACC_TRIGR_TRGSEL1_Pos                (8U)                                           /**< (DACC_TRIGR) Trigger Selection of Channel 1 Position */
#define DACC_TRIGR_TRGSEL1_Msk                (0x7U << DACC_TRIGR_TRGSEL1_Pos)               /**< (DACC_TRIGR) Trigger Selection of Channel 1 Mask */
#define DACC_TRIGR_TRGSEL1(value)             (DACC_TRIGR_TRGSEL1_Msk & ((value) << DACC_TRIGR_TRGSEL1_Pos))
#define   DACC_TRIGR_TRGSEL1_TRGSEL0_Val      (0U)                                           /**< (DACC_TRIGR) DAC External Trigger Input (DATRG)  */
#define   DACC_TRIGR_TRGSEL1_TRGSEL1_Val      (1U)                                           /**< (DACC_TRIGR) TC0 Channel 0 Output (TIOA0)  */
#define   DACC_TRIGR_TRGSEL1_TRGSEL2_Val      (2U)                                           /**< (DACC_TRIGR) TC0 Channel 1 Output (TIOA1)  */
#define   DACC_TRIGR_TRGSEL1_TRGSEL3_Val      (3U)                                           /**< (DACC_TRIGR) TC0 Channel 2 Output (TIOA2)  */
#define   DACC_TRIGR_TRGSEL1_TRGSEL4_Val      (4U)                                           /**< (DACC_TRIGR) PWM0 Event Line 0  */
#define   DACC_TRIGR_TRGSEL1_TRGSEL5_Val      (5U)                                           /**< (DACC_TRIGR) PWM0 Event Line 1  */
#define   DACC_TRIGR_TRGSEL1_TRGSEL6_Val      (6U)                                           /**< (DACC_TRIGR) PWM1 Event Line 0  */
#define   DACC_TRIGR_TRGSEL1_TRGSEL7_Val      (7U)                                           /**< (DACC_TRIGR) PWM1 Event Line 1  */
#define DACC_TRIGR_TRGSEL1_TRGSEL0            (DACC_TRIGR_TRGSEL1_TRGSEL0_Val << DACC_TRIGR_TRGSEL1_Pos) /**< (DACC_TRIGR) DAC External Trigger Input (DATRG) Position  */
#define DACC_TRIGR_TRGSEL1_TRGSEL1            (DACC_TRIGR_TRGSEL1_TRGSEL1_Val << DACC_TRIGR_TRGSEL1_Pos) /**< (DACC_TRIGR) TC0 Channel 0 Output (TIOA0) Position  */
#define DACC_TRIGR_TRGSEL1_TRGSEL2            (DACC_TRIGR_TRGSEL1_TRGSEL2_Val << DACC_TRIGR_TRGSEL1_Pos) /**< (DACC_TRIGR) TC0 Channel 1 Output (TIOA1) Position  */
#define DACC_TRIGR_TRGSEL1_TRGSEL3            (DACC_TRIGR_TRGSEL1_TRGSEL3_Val << DACC_TRIGR_TRGSEL1_Pos) /**< (DACC_TRIGR) TC0 Channel 2 Output (TIOA2) Position  */
#define DACC_TRIGR_TRGSEL1_TRGSEL4            (DACC_TRIGR_TRGSEL1_TRGSEL4_Val << DACC_TRIGR_TRGSEL1_Pos) /**< (DACC_TRIGR) PWM0 Event Line 0 Position  */
#define DACC_TRIGR_TRGSEL1_TRGSEL5            (DACC_TRIGR_TRGSEL1_TRGSEL5_Val << DACC_TRIGR_TRGSEL1_Pos) /**< (DACC_TRIGR) PWM0 Event Line 1 Position  */
#define DACC_TRIGR_TRGSEL1_TRGSEL6            (DACC_TRIGR_TRGSEL1_TRGSEL6_Val << DACC_TRIGR_TRGSEL1_Pos) /**< (DACC_TRIGR) PWM1 Event Line 0 Position  */
#define DACC_TRIGR_TRGSEL1_TRGSEL7            (DACC_TRIGR_TRGSEL1_TRGSEL7_Val << DACC_TRIGR_TRGSEL1_Pos) /**< (DACC_TRIGR) PWM1 Event Line 1 Position  */
#define DACC_TRIGR_OSR0_Pos                   (16U)                                          /**< (DACC_TRIGR) Over Sampling Ratio of Channel 0 Position */
#define DACC_TRIGR_OSR0_Msk                   (0x7U << DACC_TRIGR_OSR0_Pos)                  /**< (DACC_TRIGR) Over Sampling Ratio of Channel 0 Mask */
#define DACC_TRIGR_OSR0(value)                (DACC_TRIGR_OSR0_Msk & ((value) << DACC_TRIGR_OSR0_Pos))
#define   DACC_TRIGR_OSR0_OSR_1_Val           (0U)                                           /**< (DACC_TRIGR) OSR = 1  */
#define   DACC_TRIGR_OSR0_OSR_2_Val           (1U)                                           /**< (DACC_TRIGR) OSR = 2  */
#define   DACC_TRIGR_OSR0_OSR_4_Val           (2U)                                           /**< (DACC_TRIGR) OSR = 4  */
#define   DACC_TRIGR_OSR0_OSR_8_Val           (3U)                                           /**< (DACC_TRIGR) OSR = 8  */
#define   DACC_TRIGR_OSR0_OSR_16_Val          (4U)                                           /**< (DACC_TRIGR) OSR = 16  */
#define   DACC_TRIGR_OSR0_OSR_32_Val          (5U)                                           /**< (DACC_TRIGR) OSR = 32  */
#define DACC_TRIGR_OSR0_OSR_1                 (DACC_TRIGR_OSR0_OSR_1_Val << DACC_TRIGR_OSR0_Pos) /**< (DACC_TRIGR) OSR = 1 Position  */
#define DACC_TRIGR_OSR0_OSR_2                 (DACC_TRIGR_OSR0_OSR_2_Val << DACC_TRIGR_OSR0_Pos) /**< (DACC_TRIGR) OSR = 2 Position  */
#define DACC_TRIGR_OSR0_OSR_4                 (DACC_TRIGR_OSR0_OSR_4_Val << DACC_TRIGR_OSR0_Pos) /**< (DACC_TRIGR) OSR = 4 Position  */
#define DACC_TRIGR_OSR0_OSR_8                 (DACC_TRIGR_OSR0_OSR_8_Val << DACC_TRIGR_OSR0_Pos) /**< (DACC_TRIGR) OSR = 8 Position  */
#define DACC_TRIGR_OSR0_OSR_16                (DACC_TRIGR_OSR0_OSR_16_Val << DACC_TRIGR_OSR0_Pos) /**< (DACC_TRIGR) OSR = 16 Position  */
#define DACC_TRIGR_OSR0_OSR_32                (DACC_TRIGR_OSR0_OSR_32_Val << DACC_TRIGR_OSR0_Pos) /**< (DACC_TRIGR) OSR = 32 Position  */
#define DACC_TRIGR_OSR1_Pos                   (20U)                                          /**< (DACC_TRIGR) Over Sampling Ratio of Channel 1 Position */
#define DACC_TRIGR_OSR1_Msk                   (0x7U << DACC_TRIGR_OSR1_Pos)                  /**< (DACC_TRIGR) Over Sampling Ratio of Channel 1 Mask */
#define DACC_TRIGR_OSR1(value)                (DACC_TRIGR_OSR1_Msk & ((value) << DACC_TRIGR_OSR1_Pos))
#define   DACC_TRIGR_OSR1_OSR_1_Val           (0U)                                           /**< (DACC_TRIGR) OSR = 1  */
#define   DACC_TRIGR_OSR1_OSR_2_Val           (1U)                                           /**< (DACC_TRIGR) OSR = 2  */
#define   DACC_TRIGR_OSR1_OSR_4_Val           (2U)                                           /**< (DACC_TRIGR) OSR = 4  */
#define   DACC_TRIGR_OSR1_OSR_8_Val           (3U)                                           /**< (DACC_TRIGR) OSR = 8  */
#define   DACC_TRIGR_OSR1_OSR_16_Val          (4U)                                           /**< (DACC_TRIGR) OSR = 16  */
#define   DACC_TRIGR_OSR1_OSR_32_Val          (5U)                                           /**< (DACC_TRIGR) OSR = 32  */
#define DACC_TRIGR_OSR1_OSR_1                 (DACC_TRIGR_OSR1_OSR_1_Val << DACC_TRIGR_OSR1_Pos) /**< (DACC_TRIGR) OSR = 1 Position  */
#define DACC_TRIGR_OSR1_OSR_2                 (DACC_TRIGR_OSR1_OSR_2_Val << DACC_TRIGR_OSR1_Pos) /**< (DACC_TRIGR) OSR = 2 Position  */
#define DACC_TRIGR_OSR1_OSR_4                 (DACC_TRIGR_OSR1_OSR_4_Val << DACC_TRIGR_OSR1_Pos) /**< (DACC_TRIGR) OSR = 4 Position  */
#define DACC_TRIGR_OSR1_OSR_8                 (DACC_TRIGR_OSR1_OSR_8_Val << DACC_TRIGR_OSR1_Pos) /**< (DACC_TRIGR) OSR = 8 Position  */
#define DACC_TRIGR_OSR1_OSR_16                (DACC_TRIGR_OSR1_OSR_16_Val << DACC_TRIGR_OSR1_Pos) /**< (DACC_TRIGR) OSR = 16 Position  */
#define DACC_TRIGR_OSR1_OSR_32                (DACC_TRIGR_OSR1_OSR_32_Val << DACC_TRIGR_OSR1_Pos) /**< (DACC_TRIGR) OSR = 32 Position  */
#define DACC_TRIGR_Msk                        (0x00770773UL)                                 /**< (DACC_TRIGR) Register Mask  */

/* -------- DACC_CHER : (DACC Offset: 0x10) ( /W 32) Channel Enable Register -------- */
#define DACC_CHER_CH0_Pos                     (0U)                                           /**< (DACC_CHER) Channel 0 Enable Position */
#define DACC_CHER_CH0_Msk                     (0x1U << DACC_CHER_CH0_Pos)                    /**< (DACC_CHER) Channel 0 Enable Mask */
#define DACC_CHER_CH0(value)                  (DACC_CHER_CH0_Msk & ((value) << DACC_CHER_CH0_Pos))
#define DACC_CHER_CH1_Pos                     (1U)                                           /**< (DACC_CHER) Channel 1 Enable Position */
#define DACC_CHER_CH1_Msk                     (0x1U << DACC_CHER_CH1_Pos)                    /**< (DACC_CHER) Channel 1 Enable Mask */
#define DACC_CHER_CH1(value)                  (DACC_CHER_CH1_Msk & ((value) << DACC_CHER_CH1_Pos))
#define DACC_CHER_Msk                         (0x00000003UL)                                 /**< (DACC_CHER) Register Mask  */

/* -------- DACC_CHDR : (DACC Offset: 0x14) ( /W 32) Channel Disable Register -------- */
#define DACC_CHDR_CH0_Pos                     (0U)                                           /**< (DACC_CHDR) Channel 0 Disable Position */
#define DACC_CHDR_CH0_Msk                     (0x1U << DACC_CHDR_CH0_Pos)                    /**< (DACC_CHDR) Channel 0 Disable Mask */
#define DACC_CHDR_CH0(value)                  (DACC_CHDR_CH0_Msk & ((value) << DACC_CHDR_CH0_Pos))
#define DACC_CHDR_CH1_Pos                     (1U)                                           /**< (DACC_CHDR) Channel 1 Disable Position */
#define DACC_CHDR_CH1_Msk                     (0x1U << DACC_CHDR_CH1_Pos)                    /**< (DACC_CHDR) Channel 1 Disable Mask */
#define DACC_CHDR_CH1(value)                  (DACC_CHDR_CH1_Msk & ((value) << DACC_CHDR_CH1_Pos))
#define DACC_CHDR_Msk                         (0x00000003UL)                                 /**< (DACC_CHDR) Register Mask  */

/* -------- DACC_CHSR : (DACC Offset: 0x18) (R/  32) Channel Status Register -------- */
#define DACC_CHSR_CH0_Pos                     (0U)                                           /**< (DACC_CHSR) Channel 0 Status Position */
#define DACC_CHSR_CH0_Msk                     (0x1U << DACC_CHSR_CH0_Pos)                    /**< (DACC_CHSR) Channel 0 Status Mask */
#define DACC_CHSR_CH0(value)                  (DACC_CHSR_CH0_Msk & ((value) << DACC_CHSR_CH0_Pos))
#define DACC_CHSR_CH1_Pos                     (1U)                                           /**< (DACC_CHSR) Channel 1 Status Position */
#define DACC_CHSR_CH1_Msk                     (0x1U << DACC_CHSR_CH1_Pos)                    /**< (DACC_CHSR) Channel 1 Status Mask */
#define DACC_CHSR_CH1(value)                  (DACC_CHSR_CH1_Msk & ((value) << DACC_CHSR_CH1_Pos))
#define DACC_CHSR_DACRDY0_Pos                 (8U)                                           /**< (DACC_CHSR) DAC Ready Flag Position */
#define DACC_CHSR_DACRDY0_Msk                 (0x1U << DACC_CHSR_DACRDY0_Pos)                /**< (DACC_CHSR) DAC Ready Flag Mask */
#define DACC_CHSR_DACRDY0(value)              (DACC_CHSR_DACRDY0_Msk & ((value) << DACC_CHSR_DACRDY0_Pos))
#define DACC_CHSR_DACRDY1_Pos                 (9U)                                           /**< (DACC_CHSR) DAC Ready Flag Position */
#define DACC_CHSR_DACRDY1_Msk                 (0x1U << DACC_CHSR_DACRDY1_Pos)                /**< (DACC_CHSR) DAC Ready Flag Mask */
#define DACC_CHSR_DACRDY1(value)              (DACC_CHSR_DACRDY1_Msk & ((value) << DACC_CHSR_DACRDY1_Pos))
#define DACC_CHSR_Msk                         (0x00000303UL)                                 /**< (DACC_CHSR) Register Mask  */

/* -------- DACC_CDR : (DACC Offset: 0x1C) ( /W 32) Conversion Data Register 0 -------- */
#define DACC_CDR_DATA0_Pos                    (0U)                                           /**< (DACC_CDR) Data to Convert for channel 0 Position */
#define DACC_CDR_DATA0_Msk                    (0xFFFFU << DACC_CDR_DATA0_Pos)                /**< (DACC_CDR) Data to Convert for channel 0 Mask */
#define DACC_CDR_DATA0(value)                 (DACC_CDR_DATA0_Msk & ((value) << DACC_CDR_DATA0_Pos))
#define DACC_CDR_DATA1_Pos                    (16U)                                          /**< (DACC_CDR) Data to Convert for channel 1 Position */
#define DACC_CDR_DATA1_Msk                    (0xFFFFU << DACC_CDR_DATA1_Pos)                /**< (DACC_CDR) Data to Convert for channel 1 Mask */
#define DACC_CDR_DATA1(value)                 (DACC_CDR_DATA1_Msk & ((value) << DACC_CDR_DATA1_Pos))
#define DACC_CDR_Msk                          (0xFFFFFFFFUL)                                 /**< (DACC_CDR) Register Mask  */

/* -------- DACC_IER : (DACC Offset: 0x24) ( /W 32) Interrupt Enable Register -------- */
#define DACC_IER_TXRDY0_Pos                   (0U)                                           /**< (DACC_IER) Transmit Ready Interrupt Enable of channel 0 Position */
#define DACC_IER_TXRDY0_Msk                   (0x1U << DACC_IER_TXRDY0_Pos)                  /**< (DACC_IER) Transmit Ready Interrupt Enable of channel 0 Mask */
#define DACC_IER_TXRDY0(value)                (DACC_IER_TXRDY0_Msk & ((value) << DACC_IER_TXRDY0_Pos))
#define DACC_IER_TXRDY1_Pos                   (1U)                                           /**< (DACC_IER) Transmit Ready Interrupt Enable of channel 1 Position */
#define DACC_IER_TXRDY1_Msk                   (0x1U << DACC_IER_TXRDY1_Pos)                  /**< (DACC_IER) Transmit Ready Interrupt Enable of channel 1 Mask */
#define DACC_IER_TXRDY1(value)                (DACC_IER_TXRDY1_Msk & ((value) << DACC_IER_TXRDY1_Pos))
#define DACC_IER_EOC0_Pos                     (4U)                                           /**< (DACC_IER) End of Conversion Interrupt Enable of channel 0 Position */
#define DACC_IER_EOC0_Msk                     (0x1U << DACC_IER_EOC0_Pos)                    /**< (DACC_IER) End of Conversion Interrupt Enable of channel 0 Mask */
#define DACC_IER_EOC0(value)                  (DACC_IER_EOC0_Msk & ((value) << DACC_IER_EOC0_Pos))
#define DACC_IER_EOC1_Pos                     (5U)                                           /**< (DACC_IER) End of Conversion Interrupt Enable of channel 1 Position */
#define DACC_IER_EOC1_Msk                     (0x1U << DACC_IER_EOC1_Pos)                    /**< (DACC_IER) End of Conversion Interrupt Enable of channel 1 Mask */
#define DACC_IER_EOC1(value)                  (DACC_IER_EOC1_Msk & ((value) << DACC_IER_EOC1_Pos))
#define DACC_IER_Msk                          (0x00000033UL)                                 /**< (DACC_IER) Register Mask  */

/* -------- DACC_IDR : (DACC Offset: 0x28) ( /W 32) Interrupt Disable Register -------- */
#define DACC_IDR_TXRDY0_Pos                   (0U)                                           /**< (DACC_IDR) Transmit Ready Interrupt Disable of channel 0 Position */
#define DACC_IDR_TXRDY0_Msk                   (0x1U << DACC_IDR_TXRDY0_Pos)                  /**< (DACC_IDR) Transmit Ready Interrupt Disable of channel 0 Mask */
#define DACC_IDR_TXRDY0(value)                (DACC_IDR_TXRDY0_Msk & ((value) << DACC_IDR_TXRDY0_Pos))
#define DACC_IDR_TXRDY1_Pos                   (1U)                                           /**< (DACC_IDR) Transmit Ready Interrupt Disable of channel 1 Position */
#define DACC_IDR_TXRDY1_Msk                   (0x1U << DACC_IDR_TXRDY1_Pos)                  /**< (DACC_IDR) Transmit Ready Interrupt Disable of channel 1 Mask */
#define DACC_IDR_TXRDY1(value)                (DACC_IDR_TXRDY1_Msk & ((value) << DACC_IDR_TXRDY1_Pos))
#define DACC_IDR_EOC0_Pos                     (4U)                                           /**< (DACC_IDR) End of Conversion Interrupt Disable of channel 0 Position */
#define DACC_IDR_EOC0_Msk                     (0x1U << DACC_IDR_EOC0_Pos)                    /**< (DACC_IDR) End of Conversion Interrupt Disable of channel 0 Mask */
#define DACC_IDR_EOC0(value)                  (DACC_IDR_EOC0_Msk & ((value) << DACC_IDR_EOC0_Pos))
#define DACC_IDR_EOC1_Pos                     (5U)                                           /**< (DACC_IDR) End of Conversion Interrupt Disable of channel 1 Position */
#define DACC_IDR_EOC1_Msk                     (0x1U << DACC_IDR_EOC1_Pos)                    /**< (DACC_IDR) End of Conversion Interrupt Disable of channel 1 Mask */
#define DACC_IDR_EOC1(value)                  (DACC_IDR_EOC1_Msk & ((value) << DACC_IDR_EOC1_Pos))
#define DACC_IDR_Msk                          (0x00000033UL)                                 /**< (DACC_IDR) Register Mask  */

/* -------- DACC_IMR : (DACC Offset: 0x2C) (R/  32) Interrupt Mask Register -------- */
#define DACC_IMR_TXRDY0_Pos                   (0U)                                           /**< (DACC_IMR) Transmit Ready Interrupt Mask of channel 0 Position */
#define DACC_IMR_TXRDY0_Msk                   (0x1U << DACC_IMR_TXRDY0_Pos)                  /**< (DACC_IMR) Transmit Ready Interrupt Mask of channel 0 Mask */
#define DACC_IMR_TXRDY0(value)                (DACC_IMR_TXRDY0_Msk & ((value) << DACC_IMR_TXRDY0_Pos))
#define DACC_IMR_TXRDY1_Pos                   (1U)                                           /**< (DACC_IMR) Transmit Ready Interrupt Mask of channel 1 Position */
#define DACC_IMR_TXRDY1_Msk                   (0x1U << DACC_IMR_TXRDY1_Pos)                  /**< (DACC_IMR) Transmit Ready Interrupt Mask of channel 1 Mask */
#define DACC_IMR_TXRDY1(value)                (DACC_IMR_TXRDY1_Msk & ((value) << DACC_IMR_TXRDY1_Pos))
#define DACC_IMR_EOC0_Pos                     (4U)                                           /**< (DACC_IMR) End of Conversion Interrupt Mask of channel 0 Position */
#define DACC_IMR_EOC0_Msk                     (0x1U << DACC_IMR_EOC0_Pos)                    /**< (DACC_IMR) End of Conversion Interrupt Mask of channel 0 Mask */
#define DACC_IMR_EOC0(value)                  (DACC_IMR_EOC0_Msk & ((value) << DACC_IMR_EOC0_Pos))
#define DACC_IMR_EOC1_Pos                     (5U)                                           /**< (DACC_IMR) End of Conversion Interrupt Mask of channel 1 Position */
#define DACC_IMR_EOC1_Msk                     (0x1U << DACC_IMR_EOC1_Pos)                    /**< (DACC_IMR) End of Conversion Interrupt Mask of channel 1 Mask */
#define DACC_IMR_EOC1(value)                  (DACC_IMR_EOC1_Msk & ((value) << DACC_IMR_EOC1_Pos))
#define DACC_IMR_Msk                          (0x00000033UL)                                 /**< (DACC_IMR) Register Mask  */

/* -------- DACC_ISR : (DACC Offset: 0x30) (R/  32) Interrupt Status Register -------- */
#define DACC_ISR_TXRDY0_Pos                   (0U)                                           /**< (DACC_ISR) Transmit Ready Interrupt Flag of channel 0 Position */
#define DACC_ISR_TXRDY0_Msk                   (0x1U << DACC_ISR_TXRDY0_Pos)                  /**< (DACC_ISR) Transmit Ready Interrupt Flag of channel 0 Mask */
#define DACC_ISR_TXRDY0(value)                (DACC_ISR_TXRDY0_Msk & ((value) << DACC_ISR_TXRDY0_Pos))
#define DACC_ISR_TXRDY1_Pos                   (1U)                                           /**< (DACC_ISR) Transmit Ready Interrupt Flag of channel 1 Position */
#define DACC_ISR_TXRDY1_Msk                   (0x1U << DACC_ISR_TXRDY1_Pos)                  /**< (DACC_ISR) Transmit Ready Interrupt Flag of channel 1 Mask */
#define DACC_ISR_TXRDY1(value)                (DACC_ISR_TXRDY1_Msk & ((value) << DACC_ISR_TXRDY1_Pos))
#define DACC_ISR_EOC0_Pos                     (4U)                                           /**< (DACC_ISR) End of Conversion Interrupt Flag of channel 0 Position */
#define DACC_ISR_EOC0_Msk                     (0x1U << DACC_ISR_EOC0_Pos)                    /**< (DACC_ISR) End of Conversion Interrupt Flag of channel 0 Mask */
#define DACC_ISR_EOC0(value)                  (DACC_ISR_EOC0_Msk & ((value) << DACC_ISR_EOC0_Pos))
#define DACC_ISR_EOC1_Pos                     (5U)                                           /**< (DACC_ISR) End of Conversion Interrupt Flag of channel 1 Position */
#define DACC_ISR_EOC1_Msk                     (0x1U << DACC_ISR_EOC1_Pos)                    /**< (DACC_ISR) End of Conversion Interrupt Flag of channel 1 Mask */
#define DACC_ISR_EOC1(value)                  (DACC_ISR_EOC1_Msk & ((value) << DACC_ISR_EOC1_Pos))
#define DACC_ISR_Msk                          (0x00000033UL)                                 /**< (DACC_ISR) Register Mask  */

/* -------- DACC_ACR : (DACC Offset: 0x94) (R/W 32) Analog Current Register -------- */
#define DACC_ACR_IBCTLCH0_Pos                 (0U)                                           /**< (DACC_ACR) Analog Output Current Control Position */
#define DACC_ACR_IBCTLCH0_Msk                 (0x3U << DACC_ACR_IBCTLCH0_Pos)                /**< (DACC_ACR) Analog Output Current Control Mask */
#define DACC_ACR_IBCTLCH0(value)              (DACC_ACR_IBCTLCH0_Msk & ((value) << DACC_ACR_IBCTLCH0_Pos))
#define DACC_ACR_IBCTLCH1_Pos                 (2U)                                           /**< (DACC_ACR) Analog Output Current Control Position */
#define DACC_ACR_IBCTLCH1_Msk                 (0x3U << DACC_ACR_IBCTLCH1_Pos)                /**< (DACC_ACR) Analog Output Current Control Mask */
#define DACC_ACR_IBCTLCH1(value)              (DACC_ACR_IBCTLCH1_Msk & ((value) << DACC_ACR_IBCTLCH1_Pos))
#define DACC_ACR_Msk                          (0x0000000FUL)                                 /**< (DACC_ACR) Register Mask  */

/* -------- DACC_WPMR : (DACC Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define DACC_WPMR_WPEN_Pos                    (0U)                                           /**< (DACC_WPMR) Write Protection Enable Position */
#define DACC_WPMR_WPEN_Msk                    (0x1U << DACC_WPMR_WPEN_Pos)                   /**< (DACC_WPMR) Write Protection Enable Mask */
#define DACC_WPMR_WPEN(value)                 (DACC_WPMR_WPEN_Msk & ((value) << DACC_WPMR_WPEN_Pos))
#define DACC_WPMR_WPKEY_Pos                   (8U)                                           /**< (DACC_WPMR) Write Protect Key Position */
#define DACC_WPMR_WPKEY_Msk                   (0xFFFFFFU << DACC_WPMR_WPKEY_Pos)             /**< (DACC_WPMR) Write Protect Key Mask */
#define DACC_WPMR_WPKEY(value)                (DACC_WPMR_WPKEY_Msk & ((value) << DACC_WPMR_WPKEY_Pos))
#define   DACC_WPMR_WPKEY_PASSWD_Val          (4473155U)                                     /**< (DACC_WPMR) Writing any other value in this field aborts the write operation of bit WPEN.Always reads as 0.  */
#define DACC_WPMR_WPKEY_PASSWD                (DACC_WPMR_WPKEY_PASSWD_Val << DACC_WPMR_WPKEY_Pos) /**< (DACC_WPMR) Writing any other value in this field aborts the write operation of bit WPEN.Always reads as 0. Position  */
#define DACC_WPMR_Msk                         (0xFFFFFF01UL)                                 /**< (DACC_WPMR) Register Mask  */

/* -------- DACC_WPSR : (DACC Offset: 0xE8) (R/  32) Write Protection Status Register -------- */
#define DACC_WPSR_WPVS_Pos                    (0U)                                           /**< (DACC_WPSR) Write Protection Violation Status Position */
#define DACC_WPSR_WPVS_Msk                    (0x1U << DACC_WPSR_WPVS_Pos)                   /**< (DACC_WPSR) Write Protection Violation Status Mask */
#define DACC_WPSR_WPVS(value)                 (DACC_WPSR_WPVS_Msk & ((value) << DACC_WPSR_WPVS_Pos))
#define DACC_WPSR_WPVSRC_Pos                  (8U)                                           /**< (DACC_WPSR) Write Protection Violation Source Position */
#define DACC_WPSR_WPVSRC_Msk                  (0xFFU << DACC_WPSR_WPVSRC_Pos)                /**< (DACC_WPSR) Write Protection Violation Source Mask */
#define DACC_WPSR_WPVSRC(value)               (DACC_WPSR_WPVSRC_Msk & ((value) << DACC_WPSR_WPVSRC_Pos))
#define DACC_WPSR_Msk                         (0x0000FF01UL)                                 /**< (DACC_WPSR) Register Mask  */

/** \brief DACC register offsets definitions */
#define DACC_CR_OFFSET                 (0x00)         /**< (DACC_CR) Control Register Offset */
#define DACC_MR_OFFSET                 (0x04)         /**< (DACC_MR) Mode Register Offset */
#define DACC_TRIGR_OFFSET              (0x08)         /**< (DACC_TRIGR) Trigger Register Offset */
#define DACC_CHER_OFFSET               (0x10)         /**< (DACC_CHER) Channel Enable Register Offset */
#define DACC_CHDR_OFFSET               (0x14)         /**< (DACC_CHDR) Channel Disable Register Offset */
#define DACC_CHSR_OFFSET               (0x18)         /**< (DACC_CHSR) Channel Status Register Offset */
#define DACC_CDR_OFFSET                (0x1C)         /**< (DACC_CDR) Conversion Data Register 0 Offset */
#define DACC_IER_OFFSET                (0x24)         /**< (DACC_IER) Interrupt Enable Register Offset */
#define DACC_IDR_OFFSET                (0x28)         /**< (DACC_IDR) Interrupt Disable Register Offset */
#define DACC_IMR_OFFSET                (0x2C)         /**< (DACC_IMR) Interrupt Mask Register Offset */
#define DACC_ISR_OFFSET                (0x30)         /**< (DACC_ISR) Interrupt Status Register Offset */
#define DACC_ACR_OFFSET                (0x94)         /**< (DACC_ACR) Analog Current Register Offset */
#define DACC_WPMR_OFFSET               (0xE4)         /**< (DACC_WPMR) Write Protection Mode Register Offset */
#define DACC_WPSR_OFFSET               (0xE8)         /**< (DACC_WPSR) Write Protection Status Register Offset */

/** \brief DACC register API structure */
typedef struct
{
  __O   uint32_t                       DACC_CR;         /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       DACC_MR;         /**< Offset: 0x04 (R/W  32) Mode Register */
  __IO  uint32_t                       DACC_TRIGR;      /**< Offset: 0x08 (R/W  32) Trigger Register */
  __I   uint8_t                        Reserved1[0x04];
  __O   uint32_t                       DACC_CHER;       /**< Offset: 0x10 ( /W  32) Channel Enable Register */
  __O   uint32_t                       DACC_CHDR;       /**< Offset: 0x14 ( /W  32) Channel Disable Register */
  __I   uint32_t                       DACC_CHSR;       /**< Offset: 0x18 (R/   32) Channel Status Register */
  __O   uint32_t                       DACC_CDR[2];     /**< Offset: 0x1c ( /W  32) Conversion Data Register 0 */
  __O   uint32_t                       DACC_IER;        /**< Offset: 0x24 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       DACC_IDR;        /**< Offset: 0x28 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       DACC_IMR;        /**< Offset: 0x2c (R/   32) Interrupt Mask Register */
  __I   uint32_t                       DACC_ISR;        /**< Offset: 0x30 (R/   32) Interrupt Status Register */
  __I   uint8_t                        Reserved2[0x60];
  __IO  uint32_t                       DACC_ACR;        /**< Offset: 0x94 (R/W  32) Analog Current Register */
  __I   uint8_t                        Reserved3[0x4C];
  __IO  uint32_t                       DACC_WPMR;       /**< Offset: 0xe4 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       DACC_WPSR;       /**< Offset: 0xe8 (R/   32) Write Protection Status Register */
} dacc_registers_t;
/** @}  end of Digital-to-Analog Converter Controller */

#endif /* SAME_SAME70_DACC_MODULE_H */

