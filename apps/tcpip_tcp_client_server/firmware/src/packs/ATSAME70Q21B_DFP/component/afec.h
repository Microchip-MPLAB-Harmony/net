/**
 * \brief Header file for SAME/SAME70 AFEC
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
#ifndef SAME_SAME70_AFEC_MODULE_H
#define SAME_SAME70_AFEC_MODULE_H

/** \addtogroup SAME_SAME70 Analog Front-End Controller
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_AFEC                                  */
/* ========================================================================== */

/* -------- AFEC_CR : (AFEC Offset: 0x00) ( /W 32) AFEC Control Register -------- */
#define AFEC_CR_SWRST_Pos                     (0U)                                           /**< (AFEC_CR) Software Reset Position */
#define AFEC_CR_SWRST_Msk                     (0x1U << AFEC_CR_SWRST_Pos)                    /**< (AFEC_CR) Software Reset Mask */
#define AFEC_CR_SWRST(value)                  (AFEC_CR_SWRST_Msk & ((value) << AFEC_CR_SWRST_Pos))
#define AFEC_CR_START_Pos                     (1U)                                           /**< (AFEC_CR) Start Conversion Position */
#define AFEC_CR_START_Msk                     (0x1U << AFEC_CR_START_Pos)                    /**< (AFEC_CR) Start Conversion Mask */
#define AFEC_CR_START(value)                  (AFEC_CR_START_Msk & ((value) << AFEC_CR_START_Pos))
#define AFEC_CR_Msk                           (0x00000003UL)                                 /**< (AFEC_CR) Register Mask  */

/* -------- AFEC_MR : (AFEC Offset: 0x04) (R/W 32) AFEC Mode Register -------- */
#define AFEC_MR_TRGEN_Pos                     (0U)                                           /**< (AFEC_MR) Trigger Enable Position */
#define AFEC_MR_TRGEN_Msk                     (0x1U << AFEC_MR_TRGEN_Pos)                    /**< (AFEC_MR) Trigger Enable Mask */
#define AFEC_MR_TRGEN(value)                  (AFEC_MR_TRGEN_Msk & ((value) << AFEC_MR_TRGEN_Pos))
#define   AFEC_MR_TRGEN_DIS_Val               (0U)                                           /**< (AFEC_MR) Hardware triggers are disabled. Starting a conversion is only possible by software.  */
#define   AFEC_MR_TRGEN_EN_Val                (1U)                                           /**< (AFEC_MR) Hardware trigger selected by TRGSEL field is enabled.  */
#define AFEC_MR_TRGEN_DIS                     (AFEC_MR_TRGEN_DIS_Val << AFEC_MR_TRGEN_Pos)   /**< (AFEC_MR) Hardware triggers are disabled. Starting a conversion is only possible by software. Position  */
#define AFEC_MR_TRGEN_EN                      (AFEC_MR_TRGEN_EN_Val << AFEC_MR_TRGEN_Pos)    /**< (AFEC_MR) Hardware trigger selected by TRGSEL field is enabled. Position  */
#define AFEC_MR_TRGSEL_Pos                    (1U)                                           /**< (AFEC_MR) Trigger Selection Position */
#define AFEC_MR_TRGSEL_Msk                    (0x7U << AFEC_MR_TRGSEL_Pos)                   /**< (AFEC_MR) Trigger Selection Mask */
#define AFEC_MR_TRGSEL(value)                 (AFEC_MR_TRGSEL_Msk & ((value) << AFEC_MR_TRGSEL_Pos))
#define   AFEC_MR_TRGSEL_AFEC_TRIG0_Val       (0U)                                           /**< (AFEC_MR) AFE0_ADTRG for AFEC0 / AFE1_ADTRG for AFEC1  */
#define   AFEC_MR_TRGSEL_AFEC_TRIG1_Val       (1U)                                           /**< (AFEC_MR) TIOA Output of the Timer Counter Channel 0 for AFEC0/TIOA Output of the Timer Counter Channel 3 for AFEC1  */
#define   AFEC_MR_TRGSEL_AFEC_TRIG2_Val       (2U)                                           /**< (AFEC_MR) TIOA Output of the Timer Counter Channel 1 for AFEC0/TIOA Output of the Timer Counter Channel 4 for AFEC1  */
#define   AFEC_MR_TRGSEL_AFEC_TRIG3_Val       (3U)                                           /**< (AFEC_MR) TIOA Output of the Timer Counter Channel 2 for AFEC0/TIOA Output of the Timer Counter Channel 5 for AFEC1  */
#define   AFEC_MR_TRGSEL_AFEC_TRIG4_Val       (4U)                                           /**< (AFEC_MR) PWM0 event line 0 for AFEC0 / PWM1 event line 0 for AFEC1  */
#define   AFEC_MR_TRGSEL_AFEC_TRIG5_Val       (5U)                                           /**< (AFEC_MR) PWM0 event line 1 for AFEC0 / PWM1 event line 1 for AFEC1  */
#define   AFEC_MR_TRGSEL_AFEC_TRIG6_Val       (6U)                                           /**< (AFEC_MR) Analog Comparator  */
#define AFEC_MR_TRGSEL_AFEC_TRIG0             (AFEC_MR_TRGSEL_AFEC_TRIG0_Val << AFEC_MR_TRGSEL_Pos) /**< (AFEC_MR) AFE0_ADTRG for AFEC0 / AFE1_ADTRG for AFEC1 Position  */
#define AFEC_MR_TRGSEL_AFEC_TRIG1             (AFEC_MR_TRGSEL_AFEC_TRIG1_Val << AFEC_MR_TRGSEL_Pos) /**< (AFEC_MR) TIOA Output of the Timer Counter Channel 0 for AFEC0/TIOA Output of the Timer Counter Channel 3 for AFEC1 Position  */
#define AFEC_MR_TRGSEL_AFEC_TRIG2             (AFEC_MR_TRGSEL_AFEC_TRIG2_Val << AFEC_MR_TRGSEL_Pos) /**< (AFEC_MR) TIOA Output of the Timer Counter Channel 1 for AFEC0/TIOA Output of the Timer Counter Channel 4 for AFEC1 Position  */
#define AFEC_MR_TRGSEL_AFEC_TRIG3             (AFEC_MR_TRGSEL_AFEC_TRIG3_Val << AFEC_MR_TRGSEL_Pos) /**< (AFEC_MR) TIOA Output of the Timer Counter Channel 2 for AFEC0/TIOA Output of the Timer Counter Channel 5 for AFEC1 Position  */
#define AFEC_MR_TRGSEL_AFEC_TRIG4             (AFEC_MR_TRGSEL_AFEC_TRIG4_Val << AFEC_MR_TRGSEL_Pos) /**< (AFEC_MR) PWM0 event line 0 for AFEC0 / PWM1 event line 0 for AFEC1 Position  */
#define AFEC_MR_TRGSEL_AFEC_TRIG5             (AFEC_MR_TRGSEL_AFEC_TRIG5_Val << AFEC_MR_TRGSEL_Pos) /**< (AFEC_MR) PWM0 event line 1 for AFEC0 / PWM1 event line 1 for AFEC1 Position  */
#define AFEC_MR_TRGSEL_AFEC_TRIG6             (AFEC_MR_TRGSEL_AFEC_TRIG6_Val << AFEC_MR_TRGSEL_Pos) /**< (AFEC_MR) Analog Comparator Position  */
#define AFEC_MR_SLEEP_Pos                     (5U)                                           /**< (AFEC_MR) Sleep Mode Position */
#define AFEC_MR_SLEEP_Msk                     (0x1U << AFEC_MR_SLEEP_Pos)                    /**< (AFEC_MR) Sleep Mode Mask */
#define AFEC_MR_SLEEP(value)                  (AFEC_MR_SLEEP_Msk & ((value) << AFEC_MR_SLEEP_Pos))
#define   AFEC_MR_SLEEP_NORMAL_Val            (0U)                                           /**< (AFEC_MR) Normal mode: The AFE and reference voltage circuitry are kept ON between conversions.  */
#define   AFEC_MR_SLEEP_SLEEP_Val             (1U)                                           /**< (AFEC_MR) Sleep mode: The AFE and reference voltage circuitry are OFF between conversions.  */
#define AFEC_MR_SLEEP_NORMAL                  (AFEC_MR_SLEEP_NORMAL_Val << AFEC_MR_SLEEP_Pos) /**< (AFEC_MR) Normal mode: The AFE and reference voltage circuitry are kept ON between conversions. Position  */
#define AFEC_MR_SLEEP_SLEEP                   (AFEC_MR_SLEEP_SLEEP_Val << AFEC_MR_SLEEP_Pos) /**< (AFEC_MR) Sleep mode: The AFE and reference voltage circuitry are OFF between conversions. Position  */
#define AFEC_MR_FWUP_Pos                      (6U)                                           /**< (AFEC_MR) Fast Wake-up Position */
#define AFEC_MR_FWUP_Msk                      (0x1U << AFEC_MR_FWUP_Pos)                     /**< (AFEC_MR) Fast Wake-up Mask */
#define AFEC_MR_FWUP(value)                   (AFEC_MR_FWUP_Msk & ((value) << AFEC_MR_FWUP_Pos))
#define   AFEC_MR_FWUP_OFF_Val                (0U)                                           /**< (AFEC_MR) Normal Sleep mode: The sleep mode is defined by the SLEEP bit.  */
#define   AFEC_MR_FWUP_ON_Val                 (1U)                                           /**< (AFEC_MR) Fast wake-up Sleep mode: The voltage reference is ON between conversions and AFE is OFF.  */
#define AFEC_MR_FWUP_OFF                      (AFEC_MR_FWUP_OFF_Val << AFEC_MR_FWUP_Pos)     /**< (AFEC_MR) Normal Sleep mode: The sleep mode is defined by the SLEEP bit. Position  */
#define AFEC_MR_FWUP_ON                       (AFEC_MR_FWUP_ON_Val << AFEC_MR_FWUP_Pos)      /**< (AFEC_MR) Fast wake-up Sleep mode: The voltage reference is ON between conversions and AFE is OFF. Position  */
#define AFEC_MR_FREERUN_Pos                   (7U)                                           /**< (AFEC_MR) Free Run Mode Position */
#define AFEC_MR_FREERUN_Msk                   (0x1U << AFEC_MR_FREERUN_Pos)                  /**< (AFEC_MR) Free Run Mode Mask */
#define AFEC_MR_FREERUN(value)                (AFEC_MR_FREERUN_Msk & ((value) << AFEC_MR_FREERUN_Pos))
#define   AFEC_MR_FREERUN_OFF_Val             (0U)                                           /**< (AFEC_MR) Normal mode  */
#define   AFEC_MR_FREERUN_ON_Val              (1U)                                           /**< (AFEC_MR) Free Run mode: Never wait for any trigger.  */
#define AFEC_MR_FREERUN_OFF                   (AFEC_MR_FREERUN_OFF_Val << AFEC_MR_FREERUN_Pos) /**< (AFEC_MR) Normal mode Position  */
#define AFEC_MR_FREERUN_ON                    (AFEC_MR_FREERUN_ON_Val << AFEC_MR_FREERUN_Pos) /**< (AFEC_MR) Free Run mode: Never wait for any trigger. Position  */
#define AFEC_MR_PRESCAL_Pos                   (8U)                                           /**< (AFEC_MR) Prescaler Rate Selection Position */
#define AFEC_MR_PRESCAL_Msk                   (0xFFU << AFEC_MR_PRESCAL_Pos)                 /**< (AFEC_MR) Prescaler Rate Selection Mask */
#define AFEC_MR_PRESCAL(value)                (AFEC_MR_PRESCAL_Msk & ((value) << AFEC_MR_PRESCAL_Pos))
#define AFEC_MR_STARTUP_Pos                   (16U)                                          /**< (AFEC_MR) Start-up Time Position */
#define AFEC_MR_STARTUP_Msk                   (0xFU << AFEC_MR_STARTUP_Pos)                  /**< (AFEC_MR) Start-up Time Mask */
#define AFEC_MR_STARTUP(value)                (AFEC_MR_STARTUP_Msk & ((value) << AFEC_MR_STARTUP_Pos))
#define   AFEC_MR_STARTUP_SUT0_Val            (0U)                                           /**< (AFEC_MR) 0 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT8_Val            (1U)                                           /**< (AFEC_MR) 8 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT16_Val           (2U)                                           /**< (AFEC_MR) 16 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT24_Val           (3U)                                           /**< (AFEC_MR) 24 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT64_Val           (4U)                                           /**< (AFEC_MR) 64 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT80_Val           (5U)                                           /**< (AFEC_MR) 80 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT96_Val           (6U)                                           /**< (AFEC_MR) 96 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT112_Val          (7U)                                           /**< (AFEC_MR) 112 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT512_Val          (8U)                                           /**< (AFEC_MR) 512 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT576_Val          (9U)                                           /**< (AFEC_MR) 576 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT640_Val          (10U)                                          /**< (AFEC_MR) 640 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT704_Val          (11U)                                          /**< (AFEC_MR) 704 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT768_Val          (12U)                                          /**< (AFEC_MR) 768 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT832_Val          (13U)                                          /**< (AFEC_MR) 832 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT896_Val          (14U)                                          /**< (AFEC_MR) 896 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT960_Val          (15U)                                          /**< (AFEC_MR) 960 periods of AFE clock  */
#define AFEC_MR_STARTUP_SUT0                  (AFEC_MR_STARTUP_SUT0_Val << AFEC_MR_STARTUP_Pos) /**< (AFEC_MR) 0 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT8                  (AFEC_MR_STARTUP_SUT8_Val << AFEC_MR_STARTUP_Pos) /**< (AFEC_MR) 8 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT16                 (AFEC_MR_STARTUP_SUT16_Val << AFEC_MR_STARTUP_Pos) /**< (AFEC_MR) 16 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT24                 (AFEC_MR_STARTUP_SUT24_Val << AFEC_MR_STARTUP_Pos) /**< (AFEC_MR) 24 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT64                 (AFEC_MR_STARTUP_SUT64_Val << AFEC_MR_STARTUP_Pos) /**< (AFEC_MR) 64 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT80                 (AFEC_MR_STARTUP_SUT80_Val << AFEC_MR_STARTUP_Pos) /**< (AFEC_MR) 80 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT96                 (AFEC_MR_STARTUP_SUT96_Val << AFEC_MR_STARTUP_Pos) /**< (AFEC_MR) 96 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT112                (AFEC_MR_STARTUP_SUT112_Val << AFEC_MR_STARTUP_Pos) /**< (AFEC_MR) 112 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT512                (AFEC_MR_STARTUP_SUT512_Val << AFEC_MR_STARTUP_Pos) /**< (AFEC_MR) 512 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT576                (AFEC_MR_STARTUP_SUT576_Val << AFEC_MR_STARTUP_Pos) /**< (AFEC_MR) 576 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT640                (AFEC_MR_STARTUP_SUT640_Val << AFEC_MR_STARTUP_Pos) /**< (AFEC_MR) 640 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT704                (AFEC_MR_STARTUP_SUT704_Val << AFEC_MR_STARTUP_Pos) /**< (AFEC_MR) 704 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT768                (AFEC_MR_STARTUP_SUT768_Val << AFEC_MR_STARTUP_Pos) /**< (AFEC_MR) 768 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT832                (AFEC_MR_STARTUP_SUT832_Val << AFEC_MR_STARTUP_Pos) /**< (AFEC_MR) 832 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT896                (AFEC_MR_STARTUP_SUT896_Val << AFEC_MR_STARTUP_Pos) /**< (AFEC_MR) 896 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT960                (AFEC_MR_STARTUP_SUT960_Val << AFEC_MR_STARTUP_Pos) /**< (AFEC_MR) 960 periods of AFE clock Position  */
#define AFEC_MR_ONE_Pos                       (23U)                                          /**< (AFEC_MR) One Position */
#define AFEC_MR_ONE_Msk                       (0x1U << AFEC_MR_ONE_Pos)                      /**< (AFEC_MR) One Mask */
#define AFEC_MR_ONE(value)                    (AFEC_MR_ONE_Msk & ((value) << AFEC_MR_ONE_Pos))
#define AFEC_MR_TRACKTIM_Pos                  (24U)                                          /**< (AFEC_MR) Tracking Time Position */
#define AFEC_MR_TRACKTIM_Msk                  (0xFU << AFEC_MR_TRACKTIM_Pos)                 /**< (AFEC_MR) Tracking Time Mask */
#define AFEC_MR_TRACKTIM(value)               (AFEC_MR_TRACKTIM_Msk & ((value) << AFEC_MR_TRACKTIM_Pos))
#define AFEC_MR_TRANSFER_Pos                  (28U)                                          /**< (AFEC_MR) Transfer Period Position */
#define AFEC_MR_TRANSFER_Msk                  (0x3U << AFEC_MR_TRANSFER_Pos)                 /**< (AFEC_MR) Transfer Period Mask */
#define AFEC_MR_TRANSFER(value)               (AFEC_MR_TRANSFER_Msk & ((value) << AFEC_MR_TRANSFER_Pos))
#define AFEC_MR_USEQ_Pos                      (31U)                                          /**< (AFEC_MR) User Sequence Enable Position */
#define AFEC_MR_USEQ_Msk                      (0x1U << AFEC_MR_USEQ_Pos)                     /**< (AFEC_MR) User Sequence Enable Mask */
#define AFEC_MR_USEQ(value)                   (AFEC_MR_USEQ_Msk & ((value) << AFEC_MR_USEQ_Pos))
#define   AFEC_MR_USEQ_NUM_ORDER_Val          (0U)                                           /**< (AFEC_MR) Normal mode: The controller converts channels in a simple numeric order.  */
#define   AFEC_MR_USEQ_REG_ORDER_Val          (1U)                                           /**< (AFEC_MR) User Sequence mode: The sequence respects what is defined in AFEC_SEQ1R and AFEC_SEQ1R.  */
#define AFEC_MR_USEQ_NUM_ORDER                (AFEC_MR_USEQ_NUM_ORDER_Val << AFEC_MR_USEQ_Pos) /**< (AFEC_MR) Normal mode: The controller converts channels in a simple numeric order. Position  */
#define AFEC_MR_USEQ_REG_ORDER                (AFEC_MR_USEQ_REG_ORDER_Val << AFEC_MR_USEQ_Pos) /**< (AFEC_MR) User Sequence mode: The sequence respects what is defined in AFEC_SEQ1R and AFEC_SEQ1R. Position  */
#define AFEC_MR_Msk                           (0xBF8FFFEFUL)                                 /**< (AFEC_MR) Register Mask  */

/* -------- AFEC_EMR : (AFEC Offset: 0x08) (R/W 32) AFEC Extended Mode Register -------- */
#define AFEC_EMR_CMPMODE_Pos                  (0U)                                           /**< (AFEC_EMR) Comparison Mode Position */
#define AFEC_EMR_CMPMODE_Msk                  (0x3U << AFEC_EMR_CMPMODE_Pos)                 /**< (AFEC_EMR) Comparison Mode Mask */
#define AFEC_EMR_CMPMODE(value)               (AFEC_EMR_CMPMODE_Msk & ((value) << AFEC_EMR_CMPMODE_Pos))
#define   AFEC_EMR_CMPMODE_LOW_Val            (0U)                                           /**< (AFEC_EMR) Generates an event when the converted data is lower than the low threshold of the window.  */
#define   AFEC_EMR_CMPMODE_HIGH_Val           (1U)                                           /**< (AFEC_EMR) Generates an event when the converted data is higher than the high threshold of the window.  */
#define   AFEC_EMR_CMPMODE_IN_Val             (2U)                                           /**< (AFEC_EMR) Generates an event when the converted data is in the comparison window.  */
#define   AFEC_EMR_CMPMODE_OUT_Val            (3U)                                           /**< (AFEC_EMR) Generates an event when the converted data is out of the comparison window.  */
#define AFEC_EMR_CMPMODE_LOW                  (AFEC_EMR_CMPMODE_LOW_Val << AFEC_EMR_CMPMODE_Pos) /**< (AFEC_EMR) Generates an event when the converted data is lower than the low threshold of the window. Position  */
#define AFEC_EMR_CMPMODE_HIGH                 (AFEC_EMR_CMPMODE_HIGH_Val << AFEC_EMR_CMPMODE_Pos) /**< (AFEC_EMR) Generates an event when the converted data is higher than the high threshold of the window. Position  */
#define AFEC_EMR_CMPMODE_IN                   (AFEC_EMR_CMPMODE_IN_Val << AFEC_EMR_CMPMODE_Pos) /**< (AFEC_EMR) Generates an event when the converted data is in the comparison window. Position  */
#define AFEC_EMR_CMPMODE_OUT                  (AFEC_EMR_CMPMODE_OUT_Val << AFEC_EMR_CMPMODE_Pos) /**< (AFEC_EMR) Generates an event when the converted data is out of the comparison window. Position  */
#define AFEC_EMR_CMPSEL_Pos                   (3U)                                           /**< (AFEC_EMR) Comparison Selected Channel Position */
#define AFEC_EMR_CMPSEL_Msk                   (0x1FU << AFEC_EMR_CMPSEL_Pos)                 /**< (AFEC_EMR) Comparison Selected Channel Mask */
#define AFEC_EMR_CMPSEL(value)                (AFEC_EMR_CMPSEL_Msk & ((value) << AFEC_EMR_CMPSEL_Pos))
#define AFEC_EMR_CMPALL_Pos                   (9U)                                           /**< (AFEC_EMR) Compare All Channels Position */
#define AFEC_EMR_CMPALL_Msk                   (0x1U << AFEC_EMR_CMPALL_Pos)                  /**< (AFEC_EMR) Compare All Channels Mask */
#define AFEC_EMR_CMPALL(value)                (AFEC_EMR_CMPALL_Msk & ((value) << AFEC_EMR_CMPALL_Pos))
#define AFEC_EMR_CMPFILTER_Pos                (12U)                                          /**< (AFEC_EMR) Compare Event Filtering Position */
#define AFEC_EMR_CMPFILTER_Msk                (0x3U << AFEC_EMR_CMPFILTER_Pos)               /**< (AFEC_EMR) Compare Event Filtering Mask */
#define AFEC_EMR_CMPFILTER(value)             (AFEC_EMR_CMPFILTER_Msk & ((value) << AFEC_EMR_CMPFILTER_Pos))
#define AFEC_EMR_RES_Pos                      (16U)                                          /**< (AFEC_EMR) Resolution Position */
#define AFEC_EMR_RES_Msk                      (0x7U << AFEC_EMR_RES_Pos)                     /**< (AFEC_EMR) Resolution Mask */
#define AFEC_EMR_RES(value)                   (AFEC_EMR_RES_Msk & ((value) << AFEC_EMR_RES_Pos))
#define   AFEC_EMR_RES_NO_AVERAGE_Val         (0U)                                           /**< (AFEC_EMR) 12-bit resolution, AFE sample rate is maximum (no averaging).  */
#define   AFEC_EMR_RES_OSR4_Val               (2U)                                           /**< (AFEC_EMR) 13-bit resolution, AFE sample rate divided by 4 (averaging).  */
#define   AFEC_EMR_RES_OSR16_Val              (3U)                                           /**< (AFEC_EMR) 14-bit resolution, AFE sample rate divided by 16 (averaging).  */
#define   AFEC_EMR_RES_OSR64_Val              (4U)                                           /**< (AFEC_EMR) 15-bit resolution, AFE sample rate divided by 64 (averaging).  */
#define   AFEC_EMR_RES_OSR256_Val             (5U)                                           /**< (AFEC_EMR) 16-bit resolution, AFE sample rate divided by 256 (averaging).  */
#define AFEC_EMR_RES_NO_AVERAGE               (AFEC_EMR_RES_NO_AVERAGE_Val << AFEC_EMR_RES_Pos) /**< (AFEC_EMR) 12-bit resolution, AFE sample rate is maximum (no averaging). Position  */
#define AFEC_EMR_RES_OSR4                     (AFEC_EMR_RES_OSR4_Val << AFEC_EMR_RES_Pos)    /**< (AFEC_EMR) 13-bit resolution, AFE sample rate divided by 4 (averaging). Position  */
#define AFEC_EMR_RES_OSR16                    (AFEC_EMR_RES_OSR16_Val << AFEC_EMR_RES_Pos)   /**< (AFEC_EMR) 14-bit resolution, AFE sample rate divided by 16 (averaging). Position  */
#define AFEC_EMR_RES_OSR64                    (AFEC_EMR_RES_OSR64_Val << AFEC_EMR_RES_Pos)   /**< (AFEC_EMR) 15-bit resolution, AFE sample rate divided by 64 (averaging). Position  */
#define AFEC_EMR_RES_OSR256                   (AFEC_EMR_RES_OSR256_Val << AFEC_EMR_RES_Pos)  /**< (AFEC_EMR) 16-bit resolution, AFE sample rate divided by 256 (averaging). Position  */
#define AFEC_EMR_TAG_Pos                      (24U)                                          /**< (AFEC_EMR) TAG of the AFEC_LDCR Position */
#define AFEC_EMR_TAG_Msk                      (0x1U << AFEC_EMR_TAG_Pos)                     /**< (AFEC_EMR) TAG of the AFEC_LDCR Mask */
#define AFEC_EMR_TAG(value)                   (AFEC_EMR_TAG_Msk & ((value) << AFEC_EMR_TAG_Pos))
#define AFEC_EMR_STM_Pos                      (25U)                                          /**< (AFEC_EMR) Single Trigger Mode Position */
#define AFEC_EMR_STM_Msk                      (0x1U << AFEC_EMR_STM_Pos)                     /**< (AFEC_EMR) Single Trigger Mode Mask */
#define AFEC_EMR_STM(value)                   (AFEC_EMR_STM_Msk & ((value) << AFEC_EMR_STM_Pos))
#define AFEC_EMR_SIGNMODE_Pos                 (28U)                                          /**< (AFEC_EMR) Sign Mode Position */
#define AFEC_EMR_SIGNMODE_Msk                 (0x3U << AFEC_EMR_SIGNMODE_Pos)                /**< (AFEC_EMR) Sign Mode Mask */
#define AFEC_EMR_SIGNMODE(value)              (AFEC_EMR_SIGNMODE_Msk & ((value) << AFEC_EMR_SIGNMODE_Pos))
#define   AFEC_EMR_SIGNMODE_SE_UNSG_DF_SIGN_Val (0U)                                           /**< (AFEC_EMR) Single-Ended channels: Unsigned conversions.Differential channels: Signed conversions.  */
#define   AFEC_EMR_SIGNMODE_SE_SIGN_DF_UNSG_Val (1U)                                           /**< (AFEC_EMR) Single-Ended channels: Signed conversions.Differential channels: Unsigned conversions.  */
#define   AFEC_EMR_SIGNMODE_ALL_UNSIGNED_Val  (2U)                                           /**< (AFEC_EMR) All channels: Unsigned conversions.  */
#define   AFEC_EMR_SIGNMODE_ALL_SIGNED_Val    (3U)                                           /**< (AFEC_EMR) All channels: Signed conversions.  */
#define AFEC_EMR_SIGNMODE_SE_UNSG_DF_SIGN     (AFEC_EMR_SIGNMODE_SE_UNSG_DF_SIGN_Val << AFEC_EMR_SIGNMODE_Pos) /**< (AFEC_EMR) Single-Ended channels: Unsigned conversions.Differential channels: Signed conversions. Position  */
#define AFEC_EMR_SIGNMODE_SE_SIGN_DF_UNSG     (AFEC_EMR_SIGNMODE_SE_SIGN_DF_UNSG_Val << AFEC_EMR_SIGNMODE_Pos) /**< (AFEC_EMR) Single-Ended channels: Signed conversions.Differential channels: Unsigned conversions. Position  */
#define AFEC_EMR_SIGNMODE_ALL_UNSIGNED        (AFEC_EMR_SIGNMODE_ALL_UNSIGNED_Val << AFEC_EMR_SIGNMODE_Pos) /**< (AFEC_EMR) All channels: Unsigned conversions. Position  */
#define AFEC_EMR_SIGNMODE_ALL_SIGNED          (AFEC_EMR_SIGNMODE_ALL_SIGNED_Val << AFEC_EMR_SIGNMODE_Pos) /**< (AFEC_EMR) All channels: Signed conversions. Position  */
#define AFEC_EMR_Msk                          (0x330732FBUL)                                 /**< (AFEC_EMR) Register Mask  */

/* -------- AFEC_SEQ1R : (AFEC Offset: 0x0C) (R/W 32) AFEC Channel Sequence 1 Register -------- */
#define AFEC_SEQ1R_USCH0_Pos                  (0U)                                           /**< (AFEC_SEQ1R) User Sequence Number 0 Position */
#define AFEC_SEQ1R_USCH0_Msk                  (0xFU << AFEC_SEQ1R_USCH0_Pos)                 /**< (AFEC_SEQ1R) User Sequence Number 0 Mask */
#define AFEC_SEQ1R_USCH0(value)               (AFEC_SEQ1R_USCH0_Msk & ((value) << AFEC_SEQ1R_USCH0_Pos))
#define AFEC_SEQ1R_USCH1_Pos                  (4U)                                           /**< (AFEC_SEQ1R) User Sequence Number 1 Position */
#define AFEC_SEQ1R_USCH1_Msk                  (0xFU << AFEC_SEQ1R_USCH1_Pos)                 /**< (AFEC_SEQ1R) User Sequence Number 1 Mask */
#define AFEC_SEQ1R_USCH1(value)               (AFEC_SEQ1R_USCH1_Msk & ((value) << AFEC_SEQ1R_USCH1_Pos))
#define AFEC_SEQ1R_USCH2_Pos                  (8U)                                           /**< (AFEC_SEQ1R) User Sequence Number 2 Position */
#define AFEC_SEQ1R_USCH2_Msk                  (0xFU << AFEC_SEQ1R_USCH2_Pos)                 /**< (AFEC_SEQ1R) User Sequence Number 2 Mask */
#define AFEC_SEQ1R_USCH2(value)               (AFEC_SEQ1R_USCH2_Msk & ((value) << AFEC_SEQ1R_USCH2_Pos))
#define AFEC_SEQ1R_USCH3_Pos                  (12U)                                          /**< (AFEC_SEQ1R) User Sequence Number 3 Position */
#define AFEC_SEQ1R_USCH3_Msk                  (0xFU << AFEC_SEQ1R_USCH3_Pos)                 /**< (AFEC_SEQ1R) User Sequence Number 3 Mask */
#define AFEC_SEQ1R_USCH3(value)               (AFEC_SEQ1R_USCH3_Msk & ((value) << AFEC_SEQ1R_USCH3_Pos))
#define AFEC_SEQ1R_USCH4_Pos                  (16U)                                          /**< (AFEC_SEQ1R) User Sequence Number 4 Position */
#define AFEC_SEQ1R_USCH4_Msk                  (0xFU << AFEC_SEQ1R_USCH4_Pos)                 /**< (AFEC_SEQ1R) User Sequence Number 4 Mask */
#define AFEC_SEQ1R_USCH4(value)               (AFEC_SEQ1R_USCH4_Msk & ((value) << AFEC_SEQ1R_USCH4_Pos))
#define AFEC_SEQ1R_USCH5_Pos                  (20U)                                          /**< (AFEC_SEQ1R) User Sequence Number 5 Position */
#define AFEC_SEQ1R_USCH5_Msk                  (0xFU << AFEC_SEQ1R_USCH5_Pos)                 /**< (AFEC_SEQ1R) User Sequence Number 5 Mask */
#define AFEC_SEQ1R_USCH5(value)               (AFEC_SEQ1R_USCH5_Msk & ((value) << AFEC_SEQ1R_USCH5_Pos))
#define AFEC_SEQ1R_USCH6_Pos                  (24U)                                          /**< (AFEC_SEQ1R) User Sequence Number 6 Position */
#define AFEC_SEQ1R_USCH6_Msk                  (0xFU << AFEC_SEQ1R_USCH6_Pos)                 /**< (AFEC_SEQ1R) User Sequence Number 6 Mask */
#define AFEC_SEQ1R_USCH6(value)               (AFEC_SEQ1R_USCH6_Msk & ((value) << AFEC_SEQ1R_USCH6_Pos))
#define AFEC_SEQ1R_USCH7_Pos                  (28U)                                          /**< (AFEC_SEQ1R) User Sequence Number 7 Position */
#define AFEC_SEQ1R_USCH7_Msk                  (0xFU << AFEC_SEQ1R_USCH7_Pos)                 /**< (AFEC_SEQ1R) User Sequence Number 7 Mask */
#define AFEC_SEQ1R_USCH7(value)               (AFEC_SEQ1R_USCH7_Msk & ((value) << AFEC_SEQ1R_USCH7_Pos))
#define AFEC_SEQ1R_Msk                        (0xFFFFFFFFUL)                                 /**< (AFEC_SEQ1R) Register Mask  */

/* -------- AFEC_SEQ2R : (AFEC Offset: 0x10) (R/W 32) AFEC Channel Sequence 2 Register -------- */
#define AFEC_SEQ2R_USCH8_Pos                  (0U)                                           /**< (AFEC_SEQ2R) User Sequence Number 8 Position */
#define AFEC_SEQ2R_USCH8_Msk                  (0xFU << AFEC_SEQ2R_USCH8_Pos)                 /**< (AFEC_SEQ2R) User Sequence Number 8 Mask */
#define AFEC_SEQ2R_USCH8(value)               (AFEC_SEQ2R_USCH8_Msk & ((value) << AFEC_SEQ2R_USCH8_Pos))
#define AFEC_SEQ2R_USCH9_Pos                  (4U)                                           /**< (AFEC_SEQ2R) User Sequence Number 9 Position */
#define AFEC_SEQ2R_USCH9_Msk                  (0xFU << AFEC_SEQ2R_USCH9_Pos)                 /**< (AFEC_SEQ2R) User Sequence Number 9 Mask */
#define AFEC_SEQ2R_USCH9(value)               (AFEC_SEQ2R_USCH9_Msk & ((value) << AFEC_SEQ2R_USCH9_Pos))
#define AFEC_SEQ2R_USCH10_Pos                 (8U)                                           /**< (AFEC_SEQ2R) User Sequence Number 10 Position */
#define AFEC_SEQ2R_USCH10_Msk                 (0xFU << AFEC_SEQ2R_USCH10_Pos)                /**< (AFEC_SEQ2R) User Sequence Number 10 Mask */
#define AFEC_SEQ2R_USCH10(value)              (AFEC_SEQ2R_USCH10_Msk & ((value) << AFEC_SEQ2R_USCH10_Pos))
#define AFEC_SEQ2R_USCH11_Pos                 (12U)                                          /**< (AFEC_SEQ2R) User Sequence Number 11 Position */
#define AFEC_SEQ2R_USCH11_Msk                 (0xFU << AFEC_SEQ2R_USCH11_Pos)                /**< (AFEC_SEQ2R) User Sequence Number 11 Mask */
#define AFEC_SEQ2R_USCH11(value)              (AFEC_SEQ2R_USCH11_Msk & ((value) << AFEC_SEQ2R_USCH11_Pos))
#define AFEC_SEQ2R_Msk                        (0x0000FFFFUL)                                 /**< (AFEC_SEQ2R) Register Mask  */

/* -------- AFEC_CHER : (AFEC Offset: 0x14) ( /W 32) AFEC Channel Enable Register -------- */
#define AFEC_CHER_CH0_Pos                     (0U)                                           /**< (AFEC_CHER) Channel 0 Enable Position */
#define AFEC_CHER_CH0_Msk                     (0x1U << AFEC_CHER_CH0_Pos)                    /**< (AFEC_CHER) Channel 0 Enable Mask */
#define AFEC_CHER_CH0(value)                  (AFEC_CHER_CH0_Msk & ((value) << AFEC_CHER_CH0_Pos))
#define AFEC_CHER_CH1_Pos                     (1U)                                           /**< (AFEC_CHER) Channel 1 Enable Position */
#define AFEC_CHER_CH1_Msk                     (0x1U << AFEC_CHER_CH1_Pos)                    /**< (AFEC_CHER) Channel 1 Enable Mask */
#define AFEC_CHER_CH1(value)                  (AFEC_CHER_CH1_Msk & ((value) << AFEC_CHER_CH1_Pos))
#define AFEC_CHER_CH2_Pos                     (2U)                                           /**< (AFEC_CHER) Channel 2 Enable Position */
#define AFEC_CHER_CH2_Msk                     (0x1U << AFEC_CHER_CH2_Pos)                    /**< (AFEC_CHER) Channel 2 Enable Mask */
#define AFEC_CHER_CH2(value)                  (AFEC_CHER_CH2_Msk & ((value) << AFEC_CHER_CH2_Pos))
#define AFEC_CHER_CH3_Pos                     (3U)                                           /**< (AFEC_CHER) Channel 3 Enable Position */
#define AFEC_CHER_CH3_Msk                     (0x1U << AFEC_CHER_CH3_Pos)                    /**< (AFEC_CHER) Channel 3 Enable Mask */
#define AFEC_CHER_CH3(value)                  (AFEC_CHER_CH3_Msk & ((value) << AFEC_CHER_CH3_Pos))
#define AFEC_CHER_CH4_Pos                     (4U)                                           /**< (AFEC_CHER) Channel 4 Enable Position */
#define AFEC_CHER_CH4_Msk                     (0x1U << AFEC_CHER_CH4_Pos)                    /**< (AFEC_CHER) Channel 4 Enable Mask */
#define AFEC_CHER_CH4(value)                  (AFEC_CHER_CH4_Msk & ((value) << AFEC_CHER_CH4_Pos))
#define AFEC_CHER_CH5_Pos                     (5U)                                           /**< (AFEC_CHER) Channel 5 Enable Position */
#define AFEC_CHER_CH5_Msk                     (0x1U << AFEC_CHER_CH5_Pos)                    /**< (AFEC_CHER) Channel 5 Enable Mask */
#define AFEC_CHER_CH5(value)                  (AFEC_CHER_CH5_Msk & ((value) << AFEC_CHER_CH5_Pos))
#define AFEC_CHER_CH6_Pos                     (6U)                                           /**< (AFEC_CHER) Channel 6 Enable Position */
#define AFEC_CHER_CH6_Msk                     (0x1U << AFEC_CHER_CH6_Pos)                    /**< (AFEC_CHER) Channel 6 Enable Mask */
#define AFEC_CHER_CH6(value)                  (AFEC_CHER_CH6_Msk & ((value) << AFEC_CHER_CH6_Pos))
#define AFEC_CHER_CH7_Pos                     (7U)                                           /**< (AFEC_CHER) Channel 7 Enable Position */
#define AFEC_CHER_CH7_Msk                     (0x1U << AFEC_CHER_CH7_Pos)                    /**< (AFEC_CHER) Channel 7 Enable Mask */
#define AFEC_CHER_CH7(value)                  (AFEC_CHER_CH7_Msk & ((value) << AFEC_CHER_CH7_Pos))
#define AFEC_CHER_CH8_Pos                     (8U)                                           /**< (AFEC_CHER) Channel 8 Enable Position */
#define AFEC_CHER_CH8_Msk                     (0x1U << AFEC_CHER_CH8_Pos)                    /**< (AFEC_CHER) Channel 8 Enable Mask */
#define AFEC_CHER_CH8(value)                  (AFEC_CHER_CH8_Msk & ((value) << AFEC_CHER_CH8_Pos))
#define AFEC_CHER_CH9_Pos                     (9U)                                           /**< (AFEC_CHER) Channel 9 Enable Position */
#define AFEC_CHER_CH9_Msk                     (0x1U << AFEC_CHER_CH9_Pos)                    /**< (AFEC_CHER) Channel 9 Enable Mask */
#define AFEC_CHER_CH9(value)                  (AFEC_CHER_CH9_Msk & ((value) << AFEC_CHER_CH9_Pos))
#define AFEC_CHER_CH10_Pos                    (10U)                                          /**< (AFEC_CHER) Channel 10 Enable Position */
#define AFEC_CHER_CH10_Msk                    (0x1U << AFEC_CHER_CH10_Pos)                   /**< (AFEC_CHER) Channel 10 Enable Mask */
#define AFEC_CHER_CH10(value)                 (AFEC_CHER_CH10_Msk & ((value) << AFEC_CHER_CH10_Pos))
#define AFEC_CHER_CH11_Pos                    (11U)                                          /**< (AFEC_CHER) Channel 11 Enable Position */
#define AFEC_CHER_CH11_Msk                    (0x1U << AFEC_CHER_CH11_Pos)                   /**< (AFEC_CHER) Channel 11 Enable Mask */
#define AFEC_CHER_CH11(value)                 (AFEC_CHER_CH11_Msk & ((value) << AFEC_CHER_CH11_Pos))
#define AFEC_CHER_Msk                         (0x00000FFFUL)                                 /**< (AFEC_CHER) Register Mask  */

/* -------- AFEC_CHDR : (AFEC Offset: 0x18) ( /W 32) AFEC Channel Disable Register -------- */
#define AFEC_CHDR_CH0_Pos                     (0U)                                           /**< (AFEC_CHDR) Channel 0 Disable Position */
#define AFEC_CHDR_CH0_Msk                     (0x1U << AFEC_CHDR_CH0_Pos)                    /**< (AFEC_CHDR) Channel 0 Disable Mask */
#define AFEC_CHDR_CH0(value)                  (AFEC_CHDR_CH0_Msk & ((value) << AFEC_CHDR_CH0_Pos))
#define AFEC_CHDR_CH1_Pos                     (1U)                                           /**< (AFEC_CHDR) Channel 1 Disable Position */
#define AFEC_CHDR_CH1_Msk                     (0x1U << AFEC_CHDR_CH1_Pos)                    /**< (AFEC_CHDR) Channel 1 Disable Mask */
#define AFEC_CHDR_CH1(value)                  (AFEC_CHDR_CH1_Msk & ((value) << AFEC_CHDR_CH1_Pos))
#define AFEC_CHDR_CH2_Pos                     (2U)                                           /**< (AFEC_CHDR) Channel 2 Disable Position */
#define AFEC_CHDR_CH2_Msk                     (0x1U << AFEC_CHDR_CH2_Pos)                    /**< (AFEC_CHDR) Channel 2 Disable Mask */
#define AFEC_CHDR_CH2(value)                  (AFEC_CHDR_CH2_Msk & ((value) << AFEC_CHDR_CH2_Pos))
#define AFEC_CHDR_CH3_Pos                     (3U)                                           /**< (AFEC_CHDR) Channel 3 Disable Position */
#define AFEC_CHDR_CH3_Msk                     (0x1U << AFEC_CHDR_CH3_Pos)                    /**< (AFEC_CHDR) Channel 3 Disable Mask */
#define AFEC_CHDR_CH3(value)                  (AFEC_CHDR_CH3_Msk & ((value) << AFEC_CHDR_CH3_Pos))
#define AFEC_CHDR_CH4_Pos                     (4U)                                           /**< (AFEC_CHDR) Channel 4 Disable Position */
#define AFEC_CHDR_CH4_Msk                     (0x1U << AFEC_CHDR_CH4_Pos)                    /**< (AFEC_CHDR) Channel 4 Disable Mask */
#define AFEC_CHDR_CH4(value)                  (AFEC_CHDR_CH4_Msk & ((value) << AFEC_CHDR_CH4_Pos))
#define AFEC_CHDR_CH5_Pos                     (5U)                                           /**< (AFEC_CHDR) Channel 5 Disable Position */
#define AFEC_CHDR_CH5_Msk                     (0x1U << AFEC_CHDR_CH5_Pos)                    /**< (AFEC_CHDR) Channel 5 Disable Mask */
#define AFEC_CHDR_CH5(value)                  (AFEC_CHDR_CH5_Msk & ((value) << AFEC_CHDR_CH5_Pos))
#define AFEC_CHDR_CH6_Pos                     (6U)                                           /**< (AFEC_CHDR) Channel 6 Disable Position */
#define AFEC_CHDR_CH6_Msk                     (0x1U << AFEC_CHDR_CH6_Pos)                    /**< (AFEC_CHDR) Channel 6 Disable Mask */
#define AFEC_CHDR_CH6(value)                  (AFEC_CHDR_CH6_Msk & ((value) << AFEC_CHDR_CH6_Pos))
#define AFEC_CHDR_CH7_Pos                     (7U)                                           /**< (AFEC_CHDR) Channel 7 Disable Position */
#define AFEC_CHDR_CH7_Msk                     (0x1U << AFEC_CHDR_CH7_Pos)                    /**< (AFEC_CHDR) Channel 7 Disable Mask */
#define AFEC_CHDR_CH7(value)                  (AFEC_CHDR_CH7_Msk & ((value) << AFEC_CHDR_CH7_Pos))
#define AFEC_CHDR_CH8_Pos                     (8U)                                           /**< (AFEC_CHDR) Channel 8 Disable Position */
#define AFEC_CHDR_CH8_Msk                     (0x1U << AFEC_CHDR_CH8_Pos)                    /**< (AFEC_CHDR) Channel 8 Disable Mask */
#define AFEC_CHDR_CH8(value)                  (AFEC_CHDR_CH8_Msk & ((value) << AFEC_CHDR_CH8_Pos))
#define AFEC_CHDR_CH9_Pos                     (9U)                                           /**< (AFEC_CHDR) Channel 9 Disable Position */
#define AFEC_CHDR_CH9_Msk                     (0x1U << AFEC_CHDR_CH9_Pos)                    /**< (AFEC_CHDR) Channel 9 Disable Mask */
#define AFEC_CHDR_CH9(value)                  (AFEC_CHDR_CH9_Msk & ((value) << AFEC_CHDR_CH9_Pos))
#define AFEC_CHDR_CH10_Pos                    (10U)                                          /**< (AFEC_CHDR) Channel 10 Disable Position */
#define AFEC_CHDR_CH10_Msk                    (0x1U << AFEC_CHDR_CH10_Pos)                   /**< (AFEC_CHDR) Channel 10 Disable Mask */
#define AFEC_CHDR_CH10(value)                 (AFEC_CHDR_CH10_Msk & ((value) << AFEC_CHDR_CH10_Pos))
#define AFEC_CHDR_CH11_Pos                    (11U)                                          /**< (AFEC_CHDR) Channel 11 Disable Position */
#define AFEC_CHDR_CH11_Msk                    (0x1U << AFEC_CHDR_CH11_Pos)                   /**< (AFEC_CHDR) Channel 11 Disable Mask */
#define AFEC_CHDR_CH11(value)                 (AFEC_CHDR_CH11_Msk & ((value) << AFEC_CHDR_CH11_Pos))
#define AFEC_CHDR_Msk                         (0x00000FFFUL)                                 /**< (AFEC_CHDR) Register Mask  */

/* -------- AFEC_CHSR : (AFEC Offset: 0x1C) (R/  32) AFEC Channel Status Register -------- */
#define AFEC_CHSR_CH0_Pos                     (0U)                                           /**< (AFEC_CHSR) Channel 0 Status Position */
#define AFEC_CHSR_CH0_Msk                     (0x1U << AFEC_CHSR_CH0_Pos)                    /**< (AFEC_CHSR) Channel 0 Status Mask */
#define AFEC_CHSR_CH0(value)                  (AFEC_CHSR_CH0_Msk & ((value) << AFEC_CHSR_CH0_Pos))
#define AFEC_CHSR_CH1_Pos                     (1U)                                           /**< (AFEC_CHSR) Channel 1 Status Position */
#define AFEC_CHSR_CH1_Msk                     (0x1U << AFEC_CHSR_CH1_Pos)                    /**< (AFEC_CHSR) Channel 1 Status Mask */
#define AFEC_CHSR_CH1(value)                  (AFEC_CHSR_CH1_Msk & ((value) << AFEC_CHSR_CH1_Pos))
#define AFEC_CHSR_CH2_Pos                     (2U)                                           /**< (AFEC_CHSR) Channel 2 Status Position */
#define AFEC_CHSR_CH2_Msk                     (0x1U << AFEC_CHSR_CH2_Pos)                    /**< (AFEC_CHSR) Channel 2 Status Mask */
#define AFEC_CHSR_CH2(value)                  (AFEC_CHSR_CH2_Msk & ((value) << AFEC_CHSR_CH2_Pos))
#define AFEC_CHSR_CH3_Pos                     (3U)                                           /**< (AFEC_CHSR) Channel 3 Status Position */
#define AFEC_CHSR_CH3_Msk                     (0x1U << AFEC_CHSR_CH3_Pos)                    /**< (AFEC_CHSR) Channel 3 Status Mask */
#define AFEC_CHSR_CH3(value)                  (AFEC_CHSR_CH3_Msk & ((value) << AFEC_CHSR_CH3_Pos))
#define AFEC_CHSR_CH4_Pos                     (4U)                                           /**< (AFEC_CHSR) Channel 4 Status Position */
#define AFEC_CHSR_CH4_Msk                     (0x1U << AFEC_CHSR_CH4_Pos)                    /**< (AFEC_CHSR) Channel 4 Status Mask */
#define AFEC_CHSR_CH4(value)                  (AFEC_CHSR_CH4_Msk & ((value) << AFEC_CHSR_CH4_Pos))
#define AFEC_CHSR_CH5_Pos                     (5U)                                           /**< (AFEC_CHSR) Channel 5 Status Position */
#define AFEC_CHSR_CH5_Msk                     (0x1U << AFEC_CHSR_CH5_Pos)                    /**< (AFEC_CHSR) Channel 5 Status Mask */
#define AFEC_CHSR_CH5(value)                  (AFEC_CHSR_CH5_Msk & ((value) << AFEC_CHSR_CH5_Pos))
#define AFEC_CHSR_CH6_Pos                     (6U)                                           /**< (AFEC_CHSR) Channel 6 Status Position */
#define AFEC_CHSR_CH6_Msk                     (0x1U << AFEC_CHSR_CH6_Pos)                    /**< (AFEC_CHSR) Channel 6 Status Mask */
#define AFEC_CHSR_CH6(value)                  (AFEC_CHSR_CH6_Msk & ((value) << AFEC_CHSR_CH6_Pos))
#define AFEC_CHSR_CH7_Pos                     (7U)                                           /**< (AFEC_CHSR) Channel 7 Status Position */
#define AFEC_CHSR_CH7_Msk                     (0x1U << AFEC_CHSR_CH7_Pos)                    /**< (AFEC_CHSR) Channel 7 Status Mask */
#define AFEC_CHSR_CH7(value)                  (AFEC_CHSR_CH7_Msk & ((value) << AFEC_CHSR_CH7_Pos))
#define AFEC_CHSR_CH8_Pos                     (8U)                                           /**< (AFEC_CHSR) Channel 8 Status Position */
#define AFEC_CHSR_CH8_Msk                     (0x1U << AFEC_CHSR_CH8_Pos)                    /**< (AFEC_CHSR) Channel 8 Status Mask */
#define AFEC_CHSR_CH8(value)                  (AFEC_CHSR_CH8_Msk & ((value) << AFEC_CHSR_CH8_Pos))
#define AFEC_CHSR_CH9_Pos                     (9U)                                           /**< (AFEC_CHSR) Channel 9 Status Position */
#define AFEC_CHSR_CH9_Msk                     (0x1U << AFEC_CHSR_CH9_Pos)                    /**< (AFEC_CHSR) Channel 9 Status Mask */
#define AFEC_CHSR_CH9(value)                  (AFEC_CHSR_CH9_Msk & ((value) << AFEC_CHSR_CH9_Pos))
#define AFEC_CHSR_CH10_Pos                    (10U)                                          /**< (AFEC_CHSR) Channel 10 Status Position */
#define AFEC_CHSR_CH10_Msk                    (0x1U << AFEC_CHSR_CH10_Pos)                   /**< (AFEC_CHSR) Channel 10 Status Mask */
#define AFEC_CHSR_CH10(value)                 (AFEC_CHSR_CH10_Msk & ((value) << AFEC_CHSR_CH10_Pos))
#define AFEC_CHSR_CH11_Pos                    (11U)                                          /**< (AFEC_CHSR) Channel 11 Status Position */
#define AFEC_CHSR_CH11_Msk                    (0x1U << AFEC_CHSR_CH11_Pos)                   /**< (AFEC_CHSR) Channel 11 Status Mask */
#define AFEC_CHSR_CH11(value)                 (AFEC_CHSR_CH11_Msk & ((value) << AFEC_CHSR_CH11_Pos))
#define AFEC_CHSR_Msk                         (0x00000FFFUL)                                 /**< (AFEC_CHSR) Register Mask  */

/* -------- AFEC_LCDR : (AFEC Offset: 0x20) (R/  32) AFEC Last Converted Data Register -------- */
#define AFEC_LCDR_LDATA_Pos                   (0U)                                           /**< (AFEC_LCDR) Last Data Converted Position */
#define AFEC_LCDR_LDATA_Msk                   (0xFFFFU << AFEC_LCDR_LDATA_Pos)               /**< (AFEC_LCDR) Last Data Converted Mask */
#define AFEC_LCDR_LDATA(value)                (AFEC_LCDR_LDATA_Msk & ((value) << AFEC_LCDR_LDATA_Pos))
#define AFEC_LCDR_CHNB_Pos                    (24U)                                          /**< (AFEC_LCDR) Channel Number Position */
#define AFEC_LCDR_CHNB_Msk                    (0xFU << AFEC_LCDR_CHNB_Pos)                   /**< (AFEC_LCDR) Channel Number Mask */
#define AFEC_LCDR_CHNB(value)                 (AFEC_LCDR_CHNB_Msk & ((value) << AFEC_LCDR_CHNB_Pos))
#define AFEC_LCDR_Msk                         (0x0F00FFFFUL)                                 /**< (AFEC_LCDR) Register Mask  */

/* -------- AFEC_IER : (AFEC Offset: 0x24) ( /W 32) AFEC Interrupt Enable Register -------- */
#define AFEC_IER_EOC0_Pos                     (0U)                                           /**< (AFEC_IER) End of Conversion Interrupt Enable 0 Position */
#define AFEC_IER_EOC0_Msk                     (0x1U << AFEC_IER_EOC0_Pos)                    /**< (AFEC_IER) End of Conversion Interrupt Enable 0 Mask */
#define AFEC_IER_EOC0(value)                  (AFEC_IER_EOC0_Msk & ((value) << AFEC_IER_EOC0_Pos))
#define AFEC_IER_EOC1_Pos                     (1U)                                           /**< (AFEC_IER) End of Conversion Interrupt Enable 1 Position */
#define AFEC_IER_EOC1_Msk                     (0x1U << AFEC_IER_EOC1_Pos)                    /**< (AFEC_IER) End of Conversion Interrupt Enable 1 Mask */
#define AFEC_IER_EOC1(value)                  (AFEC_IER_EOC1_Msk & ((value) << AFEC_IER_EOC1_Pos))
#define AFEC_IER_EOC2_Pos                     (2U)                                           /**< (AFEC_IER) End of Conversion Interrupt Enable 2 Position */
#define AFEC_IER_EOC2_Msk                     (0x1U << AFEC_IER_EOC2_Pos)                    /**< (AFEC_IER) End of Conversion Interrupt Enable 2 Mask */
#define AFEC_IER_EOC2(value)                  (AFEC_IER_EOC2_Msk & ((value) << AFEC_IER_EOC2_Pos))
#define AFEC_IER_EOC3_Pos                     (3U)                                           /**< (AFEC_IER) End of Conversion Interrupt Enable 3 Position */
#define AFEC_IER_EOC3_Msk                     (0x1U << AFEC_IER_EOC3_Pos)                    /**< (AFEC_IER) End of Conversion Interrupt Enable 3 Mask */
#define AFEC_IER_EOC3(value)                  (AFEC_IER_EOC3_Msk & ((value) << AFEC_IER_EOC3_Pos))
#define AFEC_IER_EOC4_Pos                     (4U)                                           /**< (AFEC_IER) End of Conversion Interrupt Enable 4 Position */
#define AFEC_IER_EOC4_Msk                     (0x1U << AFEC_IER_EOC4_Pos)                    /**< (AFEC_IER) End of Conversion Interrupt Enable 4 Mask */
#define AFEC_IER_EOC4(value)                  (AFEC_IER_EOC4_Msk & ((value) << AFEC_IER_EOC4_Pos))
#define AFEC_IER_EOC5_Pos                     (5U)                                           /**< (AFEC_IER) End of Conversion Interrupt Enable 5 Position */
#define AFEC_IER_EOC5_Msk                     (0x1U << AFEC_IER_EOC5_Pos)                    /**< (AFEC_IER) End of Conversion Interrupt Enable 5 Mask */
#define AFEC_IER_EOC5(value)                  (AFEC_IER_EOC5_Msk & ((value) << AFEC_IER_EOC5_Pos))
#define AFEC_IER_EOC6_Pos                     (6U)                                           /**< (AFEC_IER) End of Conversion Interrupt Enable 6 Position */
#define AFEC_IER_EOC6_Msk                     (0x1U << AFEC_IER_EOC6_Pos)                    /**< (AFEC_IER) End of Conversion Interrupt Enable 6 Mask */
#define AFEC_IER_EOC6(value)                  (AFEC_IER_EOC6_Msk & ((value) << AFEC_IER_EOC6_Pos))
#define AFEC_IER_EOC7_Pos                     (7U)                                           /**< (AFEC_IER) End of Conversion Interrupt Enable 7 Position */
#define AFEC_IER_EOC7_Msk                     (0x1U << AFEC_IER_EOC7_Pos)                    /**< (AFEC_IER) End of Conversion Interrupt Enable 7 Mask */
#define AFEC_IER_EOC7(value)                  (AFEC_IER_EOC7_Msk & ((value) << AFEC_IER_EOC7_Pos))
#define AFEC_IER_EOC8_Pos                     (8U)                                           /**< (AFEC_IER) End of Conversion Interrupt Enable 8 Position */
#define AFEC_IER_EOC8_Msk                     (0x1U << AFEC_IER_EOC8_Pos)                    /**< (AFEC_IER) End of Conversion Interrupt Enable 8 Mask */
#define AFEC_IER_EOC8(value)                  (AFEC_IER_EOC8_Msk & ((value) << AFEC_IER_EOC8_Pos))
#define AFEC_IER_EOC9_Pos                     (9U)                                           /**< (AFEC_IER) End of Conversion Interrupt Enable 9 Position */
#define AFEC_IER_EOC9_Msk                     (0x1U << AFEC_IER_EOC9_Pos)                    /**< (AFEC_IER) End of Conversion Interrupt Enable 9 Mask */
#define AFEC_IER_EOC9(value)                  (AFEC_IER_EOC9_Msk & ((value) << AFEC_IER_EOC9_Pos))
#define AFEC_IER_EOC10_Pos                    (10U)                                          /**< (AFEC_IER) End of Conversion Interrupt Enable 10 Position */
#define AFEC_IER_EOC10_Msk                    (0x1U << AFEC_IER_EOC10_Pos)                   /**< (AFEC_IER) End of Conversion Interrupt Enable 10 Mask */
#define AFEC_IER_EOC10(value)                 (AFEC_IER_EOC10_Msk & ((value) << AFEC_IER_EOC10_Pos))
#define AFEC_IER_EOC11_Pos                    (11U)                                          /**< (AFEC_IER) End of Conversion Interrupt Enable 11 Position */
#define AFEC_IER_EOC11_Msk                    (0x1U << AFEC_IER_EOC11_Pos)                   /**< (AFEC_IER) End of Conversion Interrupt Enable 11 Mask */
#define AFEC_IER_EOC11(value)                 (AFEC_IER_EOC11_Msk & ((value) << AFEC_IER_EOC11_Pos))
#define AFEC_IER_DRDY_Pos                     (24U)                                          /**< (AFEC_IER) Data Ready Interrupt Enable Position */
#define AFEC_IER_DRDY_Msk                     (0x1U << AFEC_IER_DRDY_Pos)                    /**< (AFEC_IER) Data Ready Interrupt Enable Mask */
#define AFEC_IER_DRDY(value)                  (AFEC_IER_DRDY_Msk & ((value) << AFEC_IER_DRDY_Pos))
#define AFEC_IER_GOVRE_Pos                    (25U)                                          /**< (AFEC_IER) General Overrun Error Interrupt Enable Position */
#define AFEC_IER_GOVRE_Msk                    (0x1U << AFEC_IER_GOVRE_Pos)                   /**< (AFEC_IER) General Overrun Error Interrupt Enable Mask */
#define AFEC_IER_GOVRE(value)                 (AFEC_IER_GOVRE_Msk & ((value) << AFEC_IER_GOVRE_Pos))
#define AFEC_IER_COMPE_Pos                    (26U)                                          /**< (AFEC_IER) Comparison Event Interrupt Enable Position */
#define AFEC_IER_COMPE_Msk                    (0x1U << AFEC_IER_COMPE_Pos)                   /**< (AFEC_IER) Comparison Event Interrupt Enable Mask */
#define AFEC_IER_COMPE(value)                 (AFEC_IER_COMPE_Msk & ((value) << AFEC_IER_COMPE_Pos))
#define AFEC_IER_TEMPCHG_Pos                  (30U)                                          /**< (AFEC_IER) Temperature Change Interrupt Enable Position */
#define AFEC_IER_TEMPCHG_Msk                  (0x1U << AFEC_IER_TEMPCHG_Pos)                 /**< (AFEC_IER) Temperature Change Interrupt Enable Mask */
#define AFEC_IER_TEMPCHG(value)               (AFEC_IER_TEMPCHG_Msk & ((value) << AFEC_IER_TEMPCHG_Pos))
#define AFEC_IER_Msk                          (0x47000FFFUL)                                 /**< (AFEC_IER) Register Mask  */

/* -------- AFEC_IDR : (AFEC Offset: 0x28) ( /W 32) AFEC Interrupt Disable Register -------- */
#define AFEC_IDR_EOC0_Pos                     (0U)                                           /**< (AFEC_IDR) End of Conversion Interrupt Disable 0 Position */
#define AFEC_IDR_EOC0_Msk                     (0x1U << AFEC_IDR_EOC0_Pos)                    /**< (AFEC_IDR) End of Conversion Interrupt Disable 0 Mask */
#define AFEC_IDR_EOC0(value)                  (AFEC_IDR_EOC0_Msk & ((value) << AFEC_IDR_EOC0_Pos))
#define AFEC_IDR_EOC1_Pos                     (1U)                                           /**< (AFEC_IDR) End of Conversion Interrupt Disable 1 Position */
#define AFEC_IDR_EOC1_Msk                     (0x1U << AFEC_IDR_EOC1_Pos)                    /**< (AFEC_IDR) End of Conversion Interrupt Disable 1 Mask */
#define AFEC_IDR_EOC1(value)                  (AFEC_IDR_EOC1_Msk & ((value) << AFEC_IDR_EOC1_Pos))
#define AFEC_IDR_EOC2_Pos                     (2U)                                           /**< (AFEC_IDR) End of Conversion Interrupt Disable 2 Position */
#define AFEC_IDR_EOC2_Msk                     (0x1U << AFEC_IDR_EOC2_Pos)                    /**< (AFEC_IDR) End of Conversion Interrupt Disable 2 Mask */
#define AFEC_IDR_EOC2(value)                  (AFEC_IDR_EOC2_Msk & ((value) << AFEC_IDR_EOC2_Pos))
#define AFEC_IDR_EOC3_Pos                     (3U)                                           /**< (AFEC_IDR) End of Conversion Interrupt Disable 3 Position */
#define AFEC_IDR_EOC3_Msk                     (0x1U << AFEC_IDR_EOC3_Pos)                    /**< (AFEC_IDR) End of Conversion Interrupt Disable 3 Mask */
#define AFEC_IDR_EOC3(value)                  (AFEC_IDR_EOC3_Msk & ((value) << AFEC_IDR_EOC3_Pos))
#define AFEC_IDR_EOC4_Pos                     (4U)                                           /**< (AFEC_IDR) End of Conversion Interrupt Disable 4 Position */
#define AFEC_IDR_EOC4_Msk                     (0x1U << AFEC_IDR_EOC4_Pos)                    /**< (AFEC_IDR) End of Conversion Interrupt Disable 4 Mask */
#define AFEC_IDR_EOC4(value)                  (AFEC_IDR_EOC4_Msk & ((value) << AFEC_IDR_EOC4_Pos))
#define AFEC_IDR_EOC5_Pos                     (5U)                                           /**< (AFEC_IDR) End of Conversion Interrupt Disable 5 Position */
#define AFEC_IDR_EOC5_Msk                     (0x1U << AFEC_IDR_EOC5_Pos)                    /**< (AFEC_IDR) End of Conversion Interrupt Disable 5 Mask */
#define AFEC_IDR_EOC5(value)                  (AFEC_IDR_EOC5_Msk & ((value) << AFEC_IDR_EOC5_Pos))
#define AFEC_IDR_EOC6_Pos                     (6U)                                           /**< (AFEC_IDR) End of Conversion Interrupt Disable 6 Position */
#define AFEC_IDR_EOC6_Msk                     (0x1U << AFEC_IDR_EOC6_Pos)                    /**< (AFEC_IDR) End of Conversion Interrupt Disable 6 Mask */
#define AFEC_IDR_EOC6(value)                  (AFEC_IDR_EOC6_Msk & ((value) << AFEC_IDR_EOC6_Pos))
#define AFEC_IDR_EOC7_Pos                     (7U)                                           /**< (AFEC_IDR) End of Conversion Interrupt Disable 7 Position */
#define AFEC_IDR_EOC7_Msk                     (0x1U << AFEC_IDR_EOC7_Pos)                    /**< (AFEC_IDR) End of Conversion Interrupt Disable 7 Mask */
#define AFEC_IDR_EOC7(value)                  (AFEC_IDR_EOC7_Msk & ((value) << AFEC_IDR_EOC7_Pos))
#define AFEC_IDR_EOC8_Pos                     (8U)                                           /**< (AFEC_IDR) End of Conversion Interrupt Disable 8 Position */
#define AFEC_IDR_EOC8_Msk                     (0x1U << AFEC_IDR_EOC8_Pos)                    /**< (AFEC_IDR) End of Conversion Interrupt Disable 8 Mask */
#define AFEC_IDR_EOC8(value)                  (AFEC_IDR_EOC8_Msk & ((value) << AFEC_IDR_EOC8_Pos))
#define AFEC_IDR_EOC9_Pos                     (9U)                                           /**< (AFEC_IDR) End of Conversion Interrupt Disable 9 Position */
#define AFEC_IDR_EOC9_Msk                     (0x1U << AFEC_IDR_EOC9_Pos)                    /**< (AFEC_IDR) End of Conversion Interrupt Disable 9 Mask */
#define AFEC_IDR_EOC9(value)                  (AFEC_IDR_EOC9_Msk & ((value) << AFEC_IDR_EOC9_Pos))
#define AFEC_IDR_EOC10_Pos                    (10U)                                          /**< (AFEC_IDR) End of Conversion Interrupt Disable 10 Position */
#define AFEC_IDR_EOC10_Msk                    (0x1U << AFEC_IDR_EOC10_Pos)                   /**< (AFEC_IDR) End of Conversion Interrupt Disable 10 Mask */
#define AFEC_IDR_EOC10(value)                 (AFEC_IDR_EOC10_Msk & ((value) << AFEC_IDR_EOC10_Pos))
#define AFEC_IDR_EOC11_Pos                    (11U)                                          /**< (AFEC_IDR) End of Conversion Interrupt Disable 11 Position */
#define AFEC_IDR_EOC11_Msk                    (0x1U << AFEC_IDR_EOC11_Pos)                   /**< (AFEC_IDR) End of Conversion Interrupt Disable 11 Mask */
#define AFEC_IDR_EOC11(value)                 (AFEC_IDR_EOC11_Msk & ((value) << AFEC_IDR_EOC11_Pos))
#define AFEC_IDR_DRDY_Pos                     (24U)                                          /**< (AFEC_IDR) Data Ready Interrupt Disable Position */
#define AFEC_IDR_DRDY_Msk                     (0x1U << AFEC_IDR_DRDY_Pos)                    /**< (AFEC_IDR) Data Ready Interrupt Disable Mask */
#define AFEC_IDR_DRDY(value)                  (AFEC_IDR_DRDY_Msk & ((value) << AFEC_IDR_DRDY_Pos))
#define AFEC_IDR_GOVRE_Pos                    (25U)                                          /**< (AFEC_IDR) General Overrun Error Interrupt Disable Position */
#define AFEC_IDR_GOVRE_Msk                    (0x1U << AFEC_IDR_GOVRE_Pos)                   /**< (AFEC_IDR) General Overrun Error Interrupt Disable Mask */
#define AFEC_IDR_GOVRE(value)                 (AFEC_IDR_GOVRE_Msk & ((value) << AFEC_IDR_GOVRE_Pos))
#define AFEC_IDR_COMPE_Pos                    (26U)                                          /**< (AFEC_IDR) Comparison Event Interrupt Disable Position */
#define AFEC_IDR_COMPE_Msk                    (0x1U << AFEC_IDR_COMPE_Pos)                   /**< (AFEC_IDR) Comparison Event Interrupt Disable Mask */
#define AFEC_IDR_COMPE(value)                 (AFEC_IDR_COMPE_Msk & ((value) << AFEC_IDR_COMPE_Pos))
#define AFEC_IDR_TEMPCHG_Pos                  (30U)                                          /**< (AFEC_IDR) Temperature Change Interrupt Disable Position */
#define AFEC_IDR_TEMPCHG_Msk                  (0x1U << AFEC_IDR_TEMPCHG_Pos)                 /**< (AFEC_IDR) Temperature Change Interrupt Disable Mask */
#define AFEC_IDR_TEMPCHG(value)               (AFEC_IDR_TEMPCHG_Msk & ((value) << AFEC_IDR_TEMPCHG_Pos))
#define AFEC_IDR_Msk                          (0x47000FFFUL)                                 /**< (AFEC_IDR) Register Mask  */

/* -------- AFEC_IMR : (AFEC Offset: 0x2C) (R/  32) AFEC Interrupt Mask Register -------- */
#define AFEC_IMR_EOC0_Pos                     (0U)                                           /**< (AFEC_IMR) End of Conversion Interrupt Mask 0 Position */
#define AFEC_IMR_EOC0_Msk                     (0x1U << AFEC_IMR_EOC0_Pos)                    /**< (AFEC_IMR) End of Conversion Interrupt Mask 0 Mask */
#define AFEC_IMR_EOC0(value)                  (AFEC_IMR_EOC0_Msk & ((value) << AFEC_IMR_EOC0_Pos))
#define AFEC_IMR_EOC1_Pos                     (1U)                                           /**< (AFEC_IMR) End of Conversion Interrupt Mask 1 Position */
#define AFEC_IMR_EOC1_Msk                     (0x1U << AFEC_IMR_EOC1_Pos)                    /**< (AFEC_IMR) End of Conversion Interrupt Mask 1 Mask */
#define AFEC_IMR_EOC1(value)                  (AFEC_IMR_EOC1_Msk & ((value) << AFEC_IMR_EOC1_Pos))
#define AFEC_IMR_EOC2_Pos                     (2U)                                           /**< (AFEC_IMR) End of Conversion Interrupt Mask 2 Position */
#define AFEC_IMR_EOC2_Msk                     (0x1U << AFEC_IMR_EOC2_Pos)                    /**< (AFEC_IMR) End of Conversion Interrupt Mask 2 Mask */
#define AFEC_IMR_EOC2(value)                  (AFEC_IMR_EOC2_Msk & ((value) << AFEC_IMR_EOC2_Pos))
#define AFEC_IMR_EOC3_Pos                     (3U)                                           /**< (AFEC_IMR) End of Conversion Interrupt Mask 3 Position */
#define AFEC_IMR_EOC3_Msk                     (0x1U << AFEC_IMR_EOC3_Pos)                    /**< (AFEC_IMR) End of Conversion Interrupt Mask 3 Mask */
#define AFEC_IMR_EOC3(value)                  (AFEC_IMR_EOC3_Msk & ((value) << AFEC_IMR_EOC3_Pos))
#define AFEC_IMR_EOC4_Pos                     (4U)                                           /**< (AFEC_IMR) End of Conversion Interrupt Mask 4 Position */
#define AFEC_IMR_EOC4_Msk                     (0x1U << AFEC_IMR_EOC4_Pos)                    /**< (AFEC_IMR) End of Conversion Interrupt Mask 4 Mask */
#define AFEC_IMR_EOC4(value)                  (AFEC_IMR_EOC4_Msk & ((value) << AFEC_IMR_EOC4_Pos))
#define AFEC_IMR_EOC5_Pos                     (5U)                                           /**< (AFEC_IMR) End of Conversion Interrupt Mask 5 Position */
#define AFEC_IMR_EOC5_Msk                     (0x1U << AFEC_IMR_EOC5_Pos)                    /**< (AFEC_IMR) End of Conversion Interrupt Mask 5 Mask */
#define AFEC_IMR_EOC5(value)                  (AFEC_IMR_EOC5_Msk & ((value) << AFEC_IMR_EOC5_Pos))
#define AFEC_IMR_EOC6_Pos                     (6U)                                           /**< (AFEC_IMR) End of Conversion Interrupt Mask 6 Position */
#define AFEC_IMR_EOC6_Msk                     (0x1U << AFEC_IMR_EOC6_Pos)                    /**< (AFEC_IMR) End of Conversion Interrupt Mask 6 Mask */
#define AFEC_IMR_EOC6(value)                  (AFEC_IMR_EOC6_Msk & ((value) << AFEC_IMR_EOC6_Pos))
#define AFEC_IMR_EOC7_Pos                     (7U)                                           /**< (AFEC_IMR) End of Conversion Interrupt Mask 7 Position */
#define AFEC_IMR_EOC7_Msk                     (0x1U << AFEC_IMR_EOC7_Pos)                    /**< (AFEC_IMR) End of Conversion Interrupt Mask 7 Mask */
#define AFEC_IMR_EOC7(value)                  (AFEC_IMR_EOC7_Msk & ((value) << AFEC_IMR_EOC7_Pos))
#define AFEC_IMR_EOC8_Pos                     (8U)                                           /**< (AFEC_IMR) End of Conversion Interrupt Mask 8 Position */
#define AFEC_IMR_EOC8_Msk                     (0x1U << AFEC_IMR_EOC8_Pos)                    /**< (AFEC_IMR) End of Conversion Interrupt Mask 8 Mask */
#define AFEC_IMR_EOC8(value)                  (AFEC_IMR_EOC8_Msk & ((value) << AFEC_IMR_EOC8_Pos))
#define AFEC_IMR_EOC9_Pos                     (9U)                                           /**< (AFEC_IMR) End of Conversion Interrupt Mask 9 Position */
#define AFEC_IMR_EOC9_Msk                     (0x1U << AFEC_IMR_EOC9_Pos)                    /**< (AFEC_IMR) End of Conversion Interrupt Mask 9 Mask */
#define AFEC_IMR_EOC9(value)                  (AFEC_IMR_EOC9_Msk & ((value) << AFEC_IMR_EOC9_Pos))
#define AFEC_IMR_EOC10_Pos                    (10U)                                          /**< (AFEC_IMR) End of Conversion Interrupt Mask 10 Position */
#define AFEC_IMR_EOC10_Msk                    (0x1U << AFEC_IMR_EOC10_Pos)                   /**< (AFEC_IMR) End of Conversion Interrupt Mask 10 Mask */
#define AFEC_IMR_EOC10(value)                 (AFEC_IMR_EOC10_Msk & ((value) << AFEC_IMR_EOC10_Pos))
#define AFEC_IMR_EOC11_Pos                    (11U)                                          /**< (AFEC_IMR) End of Conversion Interrupt Mask 11 Position */
#define AFEC_IMR_EOC11_Msk                    (0x1U << AFEC_IMR_EOC11_Pos)                   /**< (AFEC_IMR) End of Conversion Interrupt Mask 11 Mask */
#define AFEC_IMR_EOC11(value)                 (AFEC_IMR_EOC11_Msk & ((value) << AFEC_IMR_EOC11_Pos))
#define AFEC_IMR_DRDY_Pos                     (24U)                                          /**< (AFEC_IMR) Data Ready Interrupt Mask Position */
#define AFEC_IMR_DRDY_Msk                     (0x1U << AFEC_IMR_DRDY_Pos)                    /**< (AFEC_IMR) Data Ready Interrupt Mask Mask */
#define AFEC_IMR_DRDY(value)                  (AFEC_IMR_DRDY_Msk & ((value) << AFEC_IMR_DRDY_Pos))
#define AFEC_IMR_GOVRE_Pos                    (25U)                                          /**< (AFEC_IMR) General Overrun Error Interrupt Mask Position */
#define AFEC_IMR_GOVRE_Msk                    (0x1U << AFEC_IMR_GOVRE_Pos)                   /**< (AFEC_IMR) General Overrun Error Interrupt Mask Mask */
#define AFEC_IMR_GOVRE(value)                 (AFEC_IMR_GOVRE_Msk & ((value) << AFEC_IMR_GOVRE_Pos))
#define AFEC_IMR_COMPE_Pos                    (26U)                                          /**< (AFEC_IMR) Comparison Event Interrupt Mask Position */
#define AFEC_IMR_COMPE_Msk                    (0x1U << AFEC_IMR_COMPE_Pos)                   /**< (AFEC_IMR) Comparison Event Interrupt Mask Mask */
#define AFEC_IMR_COMPE(value)                 (AFEC_IMR_COMPE_Msk & ((value) << AFEC_IMR_COMPE_Pos))
#define AFEC_IMR_TEMPCHG_Pos                  (30U)                                          /**< (AFEC_IMR) Temperature Change Interrupt Mask Position */
#define AFEC_IMR_TEMPCHG_Msk                  (0x1U << AFEC_IMR_TEMPCHG_Pos)                 /**< (AFEC_IMR) Temperature Change Interrupt Mask Mask */
#define AFEC_IMR_TEMPCHG(value)               (AFEC_IMR_TEMPCHG_Msk & ((value) << AFEC_IMR_TEMPCHG_Pos))
#define AFEC_IMR_Msk                          (0x47000FFFUL)                                 /**< (AFEC_IMR) Register Mask  */

/* -------- AFEC_ISR : (AFEC Offset: 0x30) (R/  32) AFEC Interrupt Status Register -------- */
#define AFEC_ISR_EOC0_Pos                     (0U)                                           /**< (AFEC_ISR) End of Conversion 0 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC0_Msk                     (0x1U << AFEC_ISR_EOC0_Pos)                    /**< (AFEC_ISR) End of Conversion 0 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC0(value)                  (AFEC_ISR_EOC0_Msk & ((value) << AFEC_ISR_EOC0_Pos))
#define AFEC_ISR_EOC1_Pos                     (1U)                                           /**< (AFEC_ISR) End of Conversion 1 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC1_Msk                     (0x1U << AFEC_ISR_EOC1_Pos)                    /**< (AFEC_ISR) End of Conversion 1 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC1(value)                  (AFEC_ISR_EOC1_Msk & ((value) << AFEC_ISR_EOC1_Pos))
#define AFEC_ISR_EOC2_Pos                     (2U)                                           /**< (AFEC_ISR) End of Conversion 2 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC2_Msk                     (0x1U << AFEC_ISR_EOC2_Pos)                    /**< (AFEC_ISR) End of Conversion 2 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC2(value)                  (AFEC_ISR_EOC2_Msk & ((value) << AFEC_ISR_EOC2_Pos))
#define AFEC_ISR_EOC3_Pos                     (3U)                                           /**< (AFEC_ISR) End of Conversion 3 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC3_Msk                     (0x1U << AFEC_ISR_EOC3_Pos)                    /**< (AFEC_ISR) End of Conversion 3 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC3(value)                  (AFEC_ISR_EOC3_Msk & ((value) << AFEC_ISR_EOC3_Pos))
#define AFEC_ISR_EOC4_Pos                     (4U)                                           /**< (AFEC_ISR) End of Conversion 4 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC4_Msk                     (0x1U << AFEC_ISR_EOC4_Pos)                    /**< (AFEC_ISR) End of Conversion 4 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC4(value)                  (AFEC_ISR_EOC4_Msk & ((value) << AFEC_ISR_EOC4_Pos))
#define AFEC_ISR_EOC5_Pos                     (5U)                                           /**< (AFEC_ISR) End of Conversion 5 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC5_Msk                     (0x1U << AFEC_ISR_EOC5_Pos)                    /**< (AFEC_ISR) End of Conversion 5 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC5(value)                  (AFEC_ISR_EOC5_Msk & ((value) << AFEC_ISR_EOC5_Pos))
#define AFEC_ISR_EOC6_Pos                     (6U)                                           /**< (AFEC_ISR) End of Conversion 6 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC6_Msk                     (0x1U << AFEC_ISR_EOC6_Pos)                    /**< (AFEC_ISR) End of Conversion 6 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC6(value)                  (AFEC_ISR_EOC6_Msk & ((value) << AFEC_ISR_EOC6_Pos))
#define AFEC_ISR_EOC7_Pos                     (7U)                                           /**< (AFEC_ISR) End of Conversion 7 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC7_Msk                     (0x1U << AFEC_ISR_EOC7_Pos)                    /**< (AFEC_ISR) End of Conversion 7 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC7(value)                  (AFEC_ISR_EOC7_Msk & ((value) << AFEC_ISR_EOC7_Pos))
#define AFEC_ISR_EOC8_Pos                     (8U)                                           /**< (AFEC_ISR) End of Conversion 8 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC8_Msk                     (0x1U << AFEC_ISR_EOC8_Pos)                    /**< (AFEC_ISR) End of Conversion 8 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC8(value)                  (AFEC_ISR_EOC8_Msk & ((value) << AFEC_ISR_EOC8_Pos))
#define AFEC_ISR_EOC9_Pos                     (9U)                                           /**< (AFEC_ISR) End of Conversion 9 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC9_Msk                     (0x1U << AFEC_ISR_EOC9_Pos)                    /**< (AFEC_ISR) End of Conversion 9 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC9(value)                  (AFEC_ISR_EOC9_Msk & ((value) << AFEC_ISR_EOC9_Pos))
#define AFEC_ISR_EOC10_Pos                    (10U)                                          /**< (AFEC_ISR) End of Conversion 10 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC10_Msk                    (0x1U << AFEC_ISR_EOC10_Pos)                   /**< (AFEC_ISR) End of Conversion 10 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC10(value)                 (AFEC_ISR_EOC10_Msk & ((value) << AFEC_ISR_EOC10_Pos))
#define AFEC_ISR_EOC11_Pos                    (11U)                                          /**< (AFEC_ISR) End of Conversion 11 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC11_Msk                    (0x1U << AFEC_ISR_EOC11_Pos)                   /**< (AFEC_ISR) End of Conversion 11 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC11(value)                 (AFEC_ISR_EOC11_Msk & ((value) << AFEC_ISR_EOC11_Pos))
#define AFEC_ISR_DRDY_Pos                     (24U)                                          /**< (AFEC_ISR) Data Ready (cleared by reading AFEC_LCDR) Position */
#define AFEC_ISR_DRDY_Msk                     (0x1U << AFEC_ISR_DRDY_Pos)                    /**< (AFEC_ISR) Data Ready (cleared by reading AFEC_LCDR) Mask */
#define AFEC_ISR_DRDY(value)                  (AFEC_ISR_DRDY_Msk & ((value) << AFEC_ISR_DRDY_Pos))
#define AFEC_ISR_GOVRE_Pos                    (25U)                                          /**< (AFEC_ISR) General Overrun Error (cleared by reading AFEC_ISR) Position */
#define AFEC_ISR_GOVRE_Msk                    (0x1U << AFEC_ISR_GOVRE_Pos)                   /**< (AFEC_ISR) General Overrun Error (cleared by reading AFEC_ISR) Mask */
#define AFEC_ISR_GOVRE(value)                 (AFEC_ISR_GOVRE_Msk & ((value) << AFEC_ISR_GOVRE_Pos))
#define AFEC_ISR_COMPE_Pos                    (26U)                                          /**< (AFEC_ISR) Comparison Error (cleared by reading AFEC_ISR) Position */
#define AFEC_ISR_COMPE_Msk                    (0x1U << AFEC_ISR_COMPE_Pos)                   /**< (AFEC_ISR) Comparison Error (cleared by reading AFEC_ISR) Mask */
#define AFEC_ISR_COMPE(value)                 (AFEC_ISR_COMPE_Msk & ((value) << AFEC_ISR_COMPE_Pos))
#define AFEC_ISR_TEMPCHG_Pos                  (30U)                                          /**< (AFEC_ISR) Temperature Change (cleared on read) Position */
#define AFEC_ISR_TEMPCHG_Msk                  (0x1U << AFEC_ISR_TEMPCHG_Pos)                 /**< (AFEC_ISR) Temperature Change (cleared on read) Mask */
#define AFEC_ISR_TEMPCHG(value)               (AFEC_ISR_TEMPCHG_Msk & ((value) << AFEC_ISR_TEMPCHG_Pos))
#define AFEC_ISR_Msk                          (0x47000FFFUL)                                 /**< (AFEC_ISR) Register Mask  */

/* -------- AFEC_OVER : (AFEC Offset: 0x4C) (R/  32) AFEC Overrun Status Register -------- */
#define AFEC_OVER_OVRE0_Pos                   (0U)                                           /**< (AFEC_OVER) Overrun Error 0 Position */
#define AFEC_OVER_OVRE0_Msk                   (0x1U << AFEC_OVER_OVRE0_Pos)                  /**< (AFEC_OVER) Overrun Error 0 Mask */
#define AFEC_OVER_OVRE0(value)                (AFEC_OVER_OVRE0_Msk & ((value) << AFEC_OVER_OVRE0_Pos))
#define AFEC_OVER_OVRE1_Pos                   (1U)                                           /**< (AFEC_OVER) Overrun Error 1 Position */
#define AFEC_OVER_OVRE1_Msk                   (0x1U << AFEC_OVER_OVRE1_Pos)                  /**< (AFEC_OVER) Overrun Error 1 Mask */
#define AFEC_OVER_OVRE1(value)                (AFEC_OVER_OVRE1_Msk & ((value) << AFEC_OVER_OVRE1_Pos))
#define AFEC_OVER_OVRE2_Pos                   (2U)                                           /**< (AFEC_OVER) Overrun Error 2 Position */
#define AFEC_OVER_OVRE2_Msk                   (0x1U << AFEC_OVER_OVRE2_Pos)                  /**< (AFEC_OVER) Overrun Error 2 Mask */
#define AFEC_OVER_OVRE2(value)                (AFEC_OVER_OVRE2_Msk & ((value) << AFEC_OVER_OVRE2_Pos))
#define AFEC_OVER_OVRE3_Pos                   (3U)                                           /**< (AFEC_OVER) Overrun Error 3 Position */
#define AFEC_OVER_OVRE3_Msk                   (0x1U << AFEC_OVER_OVRE3_Pos)                  /**< (AFEC_OVER) Overrun Error 3 Mask */
#define AFEC_OVER_OVRE3(value)                (AFEC_OVER_OVRE3_Msk & ((value) << AFEC_OVER_OVRE3_Pos))
#define AFEC_OVER_OVRE4_Pos                   (4U)                                           /**< (AFEC_OVER) Overrun Error 4 Position */
#define AFEC_OVER_OVRE4_Msk                   (0x1U << AFEC_OVER_OVRE4_Pos)                  /**< (AFEC_OVER) Overrun Error 4 Mask */
#define AFEC_OVER_OVRE4(value)                (AFEC_OVER_OVRE4_Msk & ((value) << AFEC_OVER_OVRE4_Pos))
#define AFEC_OVER_OVRE5_Pos                   (5U)                                           /**< (AFEC_OVER) Overrun Error 5 Position */
#define AFEC_OVER_OVRE5_Msk                   (0x1U << AFEC_OVER_OVRE5_Pos)                  /**< (AFEC_OVER) Overrun Error 5 Mask */
#define AFEC_OVER_OVRE5(value)                (AFEC_OVER_OVRE5_Msk & ((value) << AFEC_OVER_OVRE5_Pos))
#define AFEC_OVER_OVRE6_Pos                   (6U)                                           /**< (AFEC_OVER) Overrun Error 6 Position */
#define AFEC_OVER_OVRE6_Msk                   (0x1U << AFEC_OVER_OVRE6_Pos)                  /**< (AFEC_OVER) Overrun Error 6 Mask */
#define AFEC_OVER_OVRE6(value)                (AFEC_OVER_OVRE6_Msk & ((value) << AFEC_OVER_OVRE6_Pos))
#define AFEC_OVER_OVRE7_Pos                   (7U)                                           /**< (AFEC_OVER) Overrun Error 7 Position */
#define AFEC_OVER_OVRE7_Msk                   (0x1U << AFEC_OVER_OVRE7_Pos)                  /**< (AFEC_OVER) Overrun Error 7 Mask */
#define AFEC_OVER_OVRE7(value)                (AFEC_OVER_OVRE7_Msk & ((value) << AFEC_OVER_OVRE7_Pos))
#define AFEC_OVER_OVRE8_Pos                   (8U)                                           /**< (AFEC_OVER) Overrun Error 8 Position */
#define AFEC_OVER_OVRE8_Msk                   (0x1U << AFEC_OVER_OVRE8_Pos)                  /**< (AFEC_OVER) Overrun Error 8 Mask */
#define AFEC_OVER_OVRE8(value)                (AFEC_OVER_OVRE8_Msk & ((value) << AFEC_OVER_OVRE8_Pos))
#define AFEC_OVER_OVRE9_Pos                   (9U)                                           /**< (AFEC_OVER) Overrun Error 9 Position */
#define AFEC_OVER_OVRE9_Msk                   (0x1U << AFEC_OVER_OVRE9_Pos)                  /**< (AFEC_OVER) Overrun Error 9 Mask */
#define AFEC_OVER_OVRE9(value)                (AFEC_OVER_OVRE9_Msk & ((value) << AFEC_OVER_OVRE9_Pos))
#define AFEC_OVER_OVRE10_Pos                  (10U)                                          /**< (AFEC_OVER) Overrun Error 10 Position */
#define AFEC_OVER_OVRE10_Msk                  (0x1U << AFEC_OVER_OVRE10_Pos)                 /**< (AFEC_OVER) Overrun Error 10 Mask */
#define AFEC_OVER_OVRE10(value)               (AFEC_OVER_OVRE10_Msk & ((value) << AFEC_OVER_OVRE10_Pos))
#define AFEC_OVER_OVRE11_Pos                  (11U)                                          /**< (AFEC_OVER) Overrun Error 11 Position */
#define AFEC_OVER_OVRE11_Msk                  (0x1U << AFEC_OVER_OVRE11_Pos)                 /**< (AFEC_OVER) Overrun Error 11 Mask */
#define AFEC_OVER_OVRE11(value)               (AFEC_OVER_OVRE11_Msk & ((value) << AFEC_OVER_OVRE11_Pos))
#define AFEC_OVER_Msk                         (0x00000FFFUL)                                 /**< (AFEC_OVER) Register Mask  */

/* -------- AFEC_CWR : (AFEC Offset: 0x50) (R/W 32) AFEC Compare Window Register -------- */
#define AFEC_CWR_LOWTHRES_Pos                 (0U)                                           /**< (AFEC_CWR) Low Threshold Position */
#define AFEC_CWR_LOWTHRES_Msk                 (0xFFFFU << AFEC_CWR_LOWTHRES_Pos)             /**< (AFEC_CWR) Low Threshold Mask */
#define AFEC_CWR_LOWTHRES(value)              (AFEC_CWR_LOWTHRES_Msk & ((value) << AFEC_CWR_LOWTHRES_Pos))
#define AFEC_CWR_HIGHTHRES_Pos                (16U)                                          /**< (AFEC_CWR) High Threshold Position */
#define AFEC_CWR_HIGHTHRES_Msk                (0xFFFFU << AFEC_CWR_HIGHTHRES_Pos)            /**< (AFEC_CWR) High Threshold Mask */
#define AFEC_CWR_HIGHTHRES(value)             (AFEC_CWR_HIGHTHRES_Msk & ((value) << AFEC_CWR_HIGHTHRES_Pos))
#define AFEC_CWR_Msk                          (0xFFFFFFFFUL)                                 /**< (AFEC_CWR) Register Mask  */

/* -------- AFEC_CGR : (AFEC Offset: 0x54) (R/W 32) AFEC Channel Gain Register -------- */
#define AFEC_CGR_GAIN0_Pos                    (0U)                                           /**< (AFEC_CGR) Gain for Channel 0 Position */
#define AFEC_CGR_GAIN0_Msk                    (0x3U << AFEC_CGR_GAIN0_Pos)                   /**< (AFEC_CGR) Gain for Channel 0 Mask */
#define AFEC_CGR_GAIN0(value)                 (AFEC_CGR_GAIN0_Msk & ((value) << AFEC_CGR_GAIN0_Pos))
#define AFEC_CGR_GAIN1_Pos                    (2U)                                           /**< (AFEC_CGR) Gain for Channel 1 Position */
#define AFEC_CGR_GAIN1_Msk                    (0x3U << AFEC_CGR_GAIN1_Pos)                   /**< (AFEC_CGR) Gain for Channel 1 Mask */
#define AFEC_CGR_GAIN1(value)                 (AFEC_CGR_GAIN1_Msk & ((value) << AFEC_CGR_GAIN1_Pos))
#define AFEC_CGR_GAIN2_Pos                    (4U)                                           /**< (AFEC_CGR) Gain for Channel 2 Position */
#define AFEC_CGR_GAIN2_Msk                    (0x3U << AFEC_CGR_GAIN2_Pos)                   /**< (AFEC_CGR) Gain for Channel 2 Mask */
#define AFEC_CGR_GAIN2(value)                 (AFEC_CGR_GAIN2_Msk & ((value) << AFEC_CGR_GAIN2_Pos))
#define AFEC_CGR_GAIN3_Pos                    (6U)                                           /**< (AFEC_CGR) Gain for Channel 3 Position */
#define AFEC_CGR_GAIN3_Msk                    (0x3U << AFEC_CGR_GAIN3_Pos)                   /**< (AFEC_CGR) Gain for Channel 3 Mask */
#define AFEC_CGR_GAIN3(value)                 (AFEC_CGR_GAIN3_Msk & ((value) << AFEC_CGR_GAIN3_Pos))
#define AFEC_CGR_GAIN4_Pos                    (8U)                                           /**< (AFEC_CGR) Gain for Channel 4 Position */
#define AFEC_CGR_GAIN4_Msk                    (0x3U << AFEC_CGR_GAIN4_Pos)                   /**< (AFEC_CGR) Gain for Channel 4 Mask */
#define AFEC_CGR_GAIN4(value)                 (AFEC_CGR_GAIN4_Msk & ((value) << AFEC_CGR_GAIN4_Pos))
#define AFEC_CGR_GAIN5_Pos                    (10U)                                          /**< (AFEC_CGR) Gain for Channel 5 Position */
#define AFEC_CGR_GAIN5_Msk                    (0x3U << AFEC_CGR_GAIN5_Pos)                   /**< (AFEC_CGR) Gain for Channel 5 Mask */
#define AFEC_CGR_GAIN5(value)                 (AFEC_CGR_GAIN5_Msk & ((value) << AFEC_CGR_GAIN5_Pos))
#define AFEC_CGR_GAIN6_Pos                    (12U)                                          /**< (AFEC_CGR) Gain for Channel 6 Position */
#define AFEC_CGR_GAIN6_Msk                    (0x3U << AFEC_CGR_GAIN6_Pos)                   /**< (AFEC_CGR) Gain for Channel 6 Mask */
#define AFEC_CGR_GAIN6(value)                 (AFEC_CGR_GAIN6_Msk & ((value) << AFEC_CGR_GAIN6_Pos))
#define AFEC_CGR_GAIN7_Pos                    (14U)                                          /**< (AFEC_CGR) Gain for Channel 7 Position */
#define AFEC_CGR_GAIN7_Msk                    (0x3U << AFEC_CGR_GAIN7_Pos)                   /**< (AFEC_CGR) Gain for Channel 7 Mask */
#define AFEC_CGR_GAIN7(value)                 (AFEC_CGR_GAIN7_Msk & ((value) << AFEC_CGR_GAIN7_Pos))
#define AFEC_CGR_GAIN8_Pos                    (16U)                                          /**< (AFEC_CGR) Gain for Channel 8 Position */
#define AFEC_CGR_GAIN8_Msk                    (0x3U << AFEC_CGR_GAIN8_Pos)                   /**< (AFEC_CGR) Gain for Channel 8 Mask */
#define AFEC_CGR_GAIN8(value)                 (AFEC_CGR_GAIN8_Msk & ((value) << AFEC_CGR_GAIN8_Pos))
#define AFEC_CGR_GAIN9_Pos                    (18U)                                          /**< (AFEC_CGR) Gain for Channel 9 Position */
#define AFEC_CGR_GAIN9_Msk                    (0x3U << AFEC_CGR_GAIN9_Pos)                   /**< (AFEC_CGR) Gain for Channel 9 Mask */
#define AFEC_CGR_GAIN9(value)                 (AFEC_CGR_GAIN9_Msk & ((value) << AFEC_CGR_GAIN9_Pos))
#define AFEC_CGR_GAIN10_Pos                   (20U)                                          /**< (AFEC_CGR) Gain for Channel 10 Position */
#define AFEC_CGR_GAIN10_Msk                   (0x3U << AFEC_CGR_GAIN10_Pos)                  /**< (AFEC_CGR) Gain for Channel 10 Mask */
#define AFEC_CGR_GAIN10(value)                (AFEC_CGR_GAIN10_Msk & ((value) << AFEC_CGR_GAIN10_Pos))
#define AFEC_CGR_GAIN11_Pos                   (22U)                                          /**< (AFEC_CGR) Gain for Channel 11 Position */
#define AFEC_CGR_GAIN11_Msk                   (0x3U << AFEC_CGR_GAIN11_Pos)                  /**< (AFEC_CGR) Gain for Channel 11 Mask */
#define AFEC_CGR_GAIN11(value)                (AFEC_CGR_GAIN11_Msk & ((value) << AFEC_CGR_GAIN11_Pos))
#define AFEC_CGR_Msk                          (0x00FFFFFFUL)                                 /**< (AFEC_CGR) Register Mask  */

/* -------- AFEC_DIFFR : (AFEC Offset: 0x60) (R/W 32) AFEC Channel Differential Register -------- */
#define AFEC_DIFFR_DIFF0_Pos                  (0U)                                           /**< (AFEC_DIFFR) Differential inputs for channel 0 Position */
#define AFEC_DIFFR_DIFF0_Msk                  (0x1U << AFEC_DIFFR_DIFF0_Pos)                 /**< (AFEC_DIFFR) Differential inputs for channel 0 Mask */
#define AFEC_DIFFR_DIFF0(value)               (AFEC_DIFFR_DIFF0_Msk & ((value) << AFEC_DIFFR_DIFF0_Pos))
#define AFEC_DIFFR_DIFF1_Pos                  (1U)                                           /**< (AFEC_DIFFR) Differential inputs for channel 1 Position */
#define AFEC_DIFFR_DIFF1_Msk                  (0x1U << AFEC_DIFFR_DIFF1_Pos)                 /**< (AFEC_DIFFR) Differential inputs for channel 1 Mask */
#define AFEC_DIFFR_DIFF1(value)               (AFEC_DIFFR_DIFF1_Msk & ((value) << AFEC_DIFFR_DIFF1_Pos))
#define AFEC_DIFFR_DIFF2_Pos                  (2U)                                           /**< (AFEC_DIFFR) Differential inputs for channel 2 Position */
#define AFEC_DIFFR_DIFF2_Msk                  (0x1U << AFEC_DIFFR_DIFF2_Pos)                 /**< (AFEC_DIFFR) Differential inputs for channel 2 Mask */
#define AFEC_DIFFR_DIFF2(value)               (AFEC_DIFFR_DIFF2_Msk & ((value) << AFEC_DIFFR_DIFF2_Pos))
#define AFEC_DIFFR_DIFF3_Pos                  (3U)                                           /**< (AFEC_DIFFR) Differential inputs for channel 3 Position */
#define AFEC_DIFFR_DIFF3_Msk                  (0x1U << AFEC_DIFFR_DIFF3_Pos)                 /**< (AFEC_DIFFR) Differential inputs for channel 3 Mask */
#define AFEC_DIFFR_DIFF3(value)               (AFEC_DIFFR_DIFF3_Msk & ((value) << AFEC_DIFFR_DIFF3_Pos))
#define AFEC_DIFFR_DIFF4_Pos                  (4U)                                           /**< (AFEC_DIFFR) Differential inputs for channel 4 Position */
#define AFEC_DIFFR_DIFF4_Msk                  (0x1U << AFEC_DIFFR_DIFF4_Pos)                 /**< (AFEC_DIFFR) Differential inputs for channel 4 Mask */
#define AFEC_DIFFR_DIFF4(value)               (AFEC_DIFFR_DIFF4_Msk & ((value) << AFEC_DIFFR_DIFF4_Pos))
#define AFEC_DIFFR_DIFF5_Pos                  (5U)                                           /**< (AFEC_DIFFR) Differential inputs for channel 5 Position */
#define AFEC_DIFFR_DIFF5_Msk                  (0x1U << AFEC_DIFFR_DIFF5_Pos)                 /**< (AFEC_DIFFR) Differential inputs for channel 5 Mask */
#define AFEC_DIFFR_DIFF5(value)               (AFEC_DIFFR_DIFF5_Msk & ((value) << AFEC_DIFFR_DIFF5_Pos))
#define AFEC_DIFFR_DIFF6_Pos                  (6U)                                           /**< (AFEC_DIFFR) Differential inputs for channel 6 Position */
#define AFEC_DIFFR_DIFF6_Msk                  (0x1U << AFEC_DIFFR_DIFF6_Pos)                 /**< (AFEC_DIFFR) Differential inputs for channel 6 Mask */
#define AFEC_DIFFR_DIFF6(value)               (AFEC_DIFFR_DIFF6_Msk & ((value) << AFEC_DIFFR_DIFF6_Pos))
#define AFEC_DIFFR_DIFF7_Pos                  (7U)                                           /**< (AFEC_DIFFR) Differential inputs for channel 7 Position */
#define AFEC_DIFFR_DIFF7_Msk                  (0x1U << AFEC_DIFFR_DIFF7_Pos)                 /**< (AFEC_DIFFR) Differential inputs for channel 7 Mask */
#define AFEC_DIFFR_DIFF7(value)               (AFEC_DIFFR_DIFF7_Msk & ((value) << AFEC_DIFFR_DIFF7_Pos))
#define AFEC_DIFFR_DIFF8_Pos                  (8U)                                           /**< (AFEC_DIFFR) Differential inputs for channel 8 Position */
#define AFEC_DIFFR_DIFF8_Msk                  (0x1U << AFEC_DIFFR_DIFF8_Pos)                 /**< (AFEC_DIFFR) Differential inputs for channel 8 Mask */
#define AFEC_DIFFR_DIFF8(value)               (AFEC_DIFFR_DIFF8_Msk & ((value) << AFEC_DIFFR_DIFF8_Pos))
#define AFEC_DIFFR_DIFF9_Pos                  (9U)                                           /**< (AFEC_DIFFR) Differential inputs for channel 9 Position */
#define AFEC_DIFFR_DIFF9_Msk                  (0x1U << AFEC_DIFFR_DIFF9_Pos)                 /**< (AFEC_DIFFR) Differential inputs for channel 9 Mask */
#define AFEC_DIFFR_DIFF9(value)               (AFEC_DIFFR_DIFF9_Msk & ((value) << AFEC_DIFFR_DIFF9_Pos))
#define AFEC_DIFFR_DIFF10_Pos                 (10U)                                          /**< (AFEC_DIFFR) Differential inputs for channel 10 Position */
#define AFEC_DIFFR_DIFF10_Msk                 (0x1U << AFEC_DIFFR_DIFF10_Pos)                /**< (AFEC_DIFFR) Differential inputs for channel 10 Mask */
#define AFEC_DIFFR_DIFF10(value)              (AFEC_DIFFR_DIFF10_Msk & ((value) << AFEC_DIFFR_DIFF10_Pos))
#define AFEC_DIFFR_DIFF11_Pos                 (11U)                                          /**< (AFEC_DIFFR) Differential inputs for channel 11 Position */
#define AFEC_DIFFR_DIFF11_Msk                 (0x1U << AFEC_DIFFR_DIFF11_Pos)                /**< (AFEC_DIFFR) Differential inputs for channel 11 Mask */
#define AFEC_DIFFR_DIFF11(value)              (AFEC_DIFFR_DIFF11_Msk & ((value) << AFEC_DIFFR_DIFF11_Pos))
#define AFEC_DIFFR_Msk                        (0x00000FFFUL)                                 /**< (AFEC_DIFFR) Register Mask  */

/* -------- AFEC_CSELR : (AFEC Offset: 0x64) (R/W 32) AFEC Channel Selection Register -------- */
#define AFEC_CSELR_CSEL_Pos                   (0U)                                           /**< (AFEC_CSELR) Channel Selection Position */
#define AFEC_CSELR_CSEL_Msk                   (0xFU << AFEC_CSELR_CSEL_Pos)                  /**< (AFEC_CSELR) Channel Selection Mask */
#define AFEC_CSELR_CSEL(value)                (AFEC_CSELR_CSEL_Msk & ((value) << AFEC_CSELR_CSEL_Pos))
#define AFEC_CSELR_Msk                        (0x0000000FUL)                                 /**< (AFEC_CSELR) Register Mask  */

/* -------- AFEC_CDR : (AFEC Offset: 0x68) (R/  32) AFEC Channel Data Register -------- */
#define AFEC_CDR_DATA_Pos                     (0U)                                           /**< (AFEC_CDR) Converted Data Position */
#define AFEC_CDR_DATA_Msk                     (0xFFFFU << AFEC_CDR_DATA_Pos)                 /**< (AFEC_CDR) Converted Data Mask */
#define AFEC_CDR_DATA(value)                  (AFEC_CDR_DATA_Msk & ((value) << AFEC_CDR_DATA_Pos))
#define AFEC_CDR_Msk                          (0x0000FFFFUL)                                 /**< (AFEC_CDR) Register Mask  */

/* -------- AFEC_COCR : (AFEC Offset: 0x6C) (R/W 32) AFEC Channel Offset Compensation Register -------- */
#define AFEC_COCR_AOFF_Pos                    (0U)                                           /**< (AFEC_COCR) Analog Offset Position */
#define AFEC_COCR_AOFF_Msk                    (0x3FFU << AFEC_COCR_AOFF_Pos)                 /**< (AFEC_COCR) Analog Offset Mask */
#define AFEC_COCR_AOFF(value)                 (AFEC_COCR_AOFF_Msk & ((value) << AFEC_COCR_AOFF_Pos))
#define AFEC_COCR_Msk                         (0x000003FFUL)                                 /**< (AFEC_COCR) Register Mask  */

/* -------- AFEC_TEMPMR : (AFEC Offset: 0x70) (R/W 32) AFEC Temperature Sensor Mode Register -------- */
#define AFEC_TEMPMR_RTCT_Pos                  (0U)                                           /**< (AFEC_TEMPMR) Temperature Sensor RTC Trigger Mode Position */
#define AFEC_TEMPMR_RTCT_Msk                  (0x1U << AFEC_TEMPMR_RTCT_Pos)                 /**< (AFEC_TEMPMR) Temperature Sensor RTC Trigger Mode Mask */
#define AFEC_TEMPMR_RTCT(value)               (AFEC_TEMPMR_RTCT_Msk & ((value) << AFEC_TEMPMR_RTCT_Pos))
#define AFEC_TEMPMR_TEMPCMPMOD_Pos            (4U)                                           /**< (AFEC_TEMPMR) Temperature Comparison Mode Position */
#define AFEC_TEMPMR_TEMPCMPMOD_Msk            (0x3U << AFEC_TEMPMR_TEMPCMPMOD_Pos)           /**< (AFEC_TEMPMR) Temperature Comparison Mode Mask */
#define AFEC_TEMPMR_TEMPCMPMOD(value)         (AFEC_TEMPMR_TEMPCMPMOD_Msk & ((value) << AFEC_TEMPMR_TEMPCMPMOD_Pos))
#define   AFEC_TEMPMR_TEMPCMPMOD_LOW_Val      (0U)                                           /**< (AFEC_TEMPMR) Generates an event when the converted data is lower than the low threshold of the window.  */
#define   AFEC_TEMPMR_TEMPCMPMOD_HIGH_Val     (1U)                                           /**< (AFEC_TEMPMR) Generates an event when the converted data is higher than the high threshold of the window.  */
#define   AFEC_TEMPMR_TEMPCMPMOD_IN_Val       (2U)                                           /**< (AFEC_TEMPMR) Generates an event when the converted data is in the comparison window.  */
#define   AFEC_TEMPMR_TEMPCMPMOD_OUT_Val      (3U)                                           /**< (AFEC_TEMPMR) Generates an event when the converted data is out of the comparison window.  */
#define AFEC_TEMPMR_TEMPCMPMOD_LOW            (AFEC_TEMPMR_TEMPCMPMOD_LOW_Val << AFEC_TEMPMR_TEMPCMPMOD_Pos) /**< (AFEC_TEMPMR) Generates an event when the converted data is lower than the low threshold of the window. Position  */
#define AFEC_TEMPMR_TEMPCMPMOD_HIGH           (AFEC_TEMPMR_TEMPCMPMOD_HIGH_Val << AFEC_TEMPMR_TEMPCMPMOD_Pos) /**< (AFEC_TEMPMR) Generates an event when the converted data is higher than the high threshold of the window. Position  */
#define AFEC_TEMPMR_TEMPCMPMOD_IN             (AFEC_TEMPMR_TEMPCMPMOD_IN_Val << AFEC_TEMPMR_TEMPCMPMOD_Pos) /**< (AFEC_TEMPMR) Generates an event when the converted data is in the comparison window. Position  */
#define AFEC_TEMPMR_TEMPCMPMOD_OUT            (AFEC_TEMPMR_TEMPCMPMOD_OUT_Val << AFEC_TEMPMR_TEMPCMPMOD_Pos) /**< (AFEC_TEMPMR) Generates an event when the converted data is out of the comparison window. Position  */
#define AFEC_TEMPMR_Msk                       (0x00000031UL)                                 /**< (AFEC_TEMPMR) Register Mask  */

/* -------- AFEC_TEMPCWR : (AFEC Offset: 0x74) (R/W 32) AFEC Temperature Compare Window Register -------- */
#define AFEC_TEMPCWR_TLOWTHRES_Pos            (0U)                                           /**< (AFEC_TEMPCWR) Temperature Low Threshold Position */
#define AFEC_TEMPCWR_TLOWTHRES_Msk            (0xFFFFU << AFEC_TEMPCWR_TLOWTHRES_Pos)        /**< (AFEC_TEMPCWR) Temperature Low Threshold Mask */
#define AFEC_TEMPCWR_TLOWTHRES(value)         (AFEC_TEMPCWR_TLOWTHRES_Msk & ((value) << AFEC_TEMPCWR_TLOWTHRES_Pos))
#define AFEC_TEMPCWR_THIGHTHRES_Pos           (16U)                                          /**< (AFEC_TEMPCWR) Temperature High Threshold Position */
#define AFEC_TEMPCWR_THIGHTHRES_Msk           (0xFFFFU << AFEC_TEMPCWR_THIGHTHRES_Pos)       /**< (AFEC_TEMPCWR) Temperature High Threshold Mask */
#define AFEC_TEMPCWR_THIGHTHRES(value)        (AFEC_TEMPCWR_THIGHTHRES_Msk & ((value) << AFEC_TEMPCWR_THIGHTHRES_Pos))
#define AFEC_TEMPCWR_Msk                      (0xFFFFFFFFUL)                                 /**< (AFEC_TEMPCWR) Register Mask  */

/* -------- AFEC_ACR : (AFEC Offset: 0x94) (R/W 32) AFEC Analog Control Register -------- */
#define AFEC_ACR_PGA0EN_Pos                   (2U)                                           /**< (AFEC_ACR) PGA0 Enable Position */
#define AFEC_ACR_PGA0EN_Msk                   (0x1U << AFEC_ACR_PGA0EN_Pos)                  /**< (AFEC_ACR) PGA0 Enable Mask */
#define AFEC_ACR_PGA0EN(value)                (AFEC_ACR_PGA0EN_Msk & ((value) << AFEC_ACR_PGA0EN_Pos))
#define AFEC_ACR_PGA1EN_Pos                   (3U)                                           /**< (AFEC_ACR) PGA1 Enable Position */
#define AFEC_ACR_PGA1EN_Msk                   (0x1U << AFEC_ACR_PGA1EN_Pos)                  /**< (AFEC_ACR) PGA1 Enable Mask */
#define AFEC_ACR_PGA1EN(value)                (AFEC_ACR_PGA1EN_Msk & ((value) << AFEC_ACR_PGA1EN_Pos))
#define AFEC_ACR_IBCTL_Pos                    (8U)                                           /**< (AFEC_ACR) AFE Bias Current Control Position */
#define AFEC_ACR_IBCTL_Msk                    (0x3U << AFEC_ACR_IBCTL_Pos)                   /**< (AFEC_ACR) AFE Bias Current Control Mask */
#define AFEC_ACR_IBCTL(value)                 (AFEC_ACR_IBCTL_Msk & ((value) << AFEC_ACR_IBCTL_Pos))
#define AFEC_ACR_Msk                          (0x0000030CUL)                                 /**< (AFEC_ACR) Register Mask  */

/* -------- AFEC_SHMR : (AFEC Offset: 0xA0) (R/W 32) AFEC Sample & Hold Mode Register -------- */
#define AFEC_SHMR_DUAL0_Pos                   (0U)                                           /**< (AFEC_SHMR) Dual Sample & Hold for channel 0 Position */
#define AFEC_SHMR_DUAL0_Msk                   (0x1U << AFEC_SHMR_DUAL0_Pos)                  /**< (AFEC_SHMR) Dual Sample & Hold for channel 0 Mask */
#define AFEC_SHMR_DUAL0(value)                (AFEC_SHMR_DUAL0_Msk & ((value) << AFEC_SHMR_DUAL0_Pos))
#define AFEC_SHMR_DUAL1_Pos                   (1U)                                           /**< (AFEC_SHMR) Dual Sample & Hold for channel 1 Position */
#define AFEC_SHMR_DUAL1_Msk                   (0x1U << AFEC_SHMR_DUAL1_Pos)                  /**< (AFEC_SHMR) Dual Sample & Hold for channel 1 Mask */
#define AFEC_SHMR_DUAL1(value)                (AFEC_SHMR_DUAL1_Msk & ((value) << AFEC_SHMR_DUAL1_Pos))
#define AFEC_SHMR_DUAL2_Pos                   (2U)                                           /**< (AFEC_SHMR) Dual Sample & Hold for channel 2 Position */
#define AFEC_SHMR_DUAL2_Msk                   (0x1U << AFEC_SHMR_DUAL2_Pos)                  /**< (AFEC_SHMR) Dual Sample & Hold for channel 2 Mask */
#define AFEC_SHMR_DUAL2(value)                (AFEC_SHMR_DUAL2_Msk & ((value) << AFEC_SHMR_DUAL2_Pos))
#define AFEC_SHMR_DUAL3_Pos                   (3U)                                           /**< (AFEC_SHMR) Dual Sample & Hold for channel 3 Position */
#define AFEC_SHMR_DUAL3_Msk                   (0x1U << AFEC_SHMR_DUAL3_Pos)                  /**< (AFEC_SHMR) Dual Sample & Hold for channel 3 Mask */
#define AFEC_SHMR_DUAL3(value)                (AFEC_SHMR_DUAL3_Msk & ((value) << AFEC_SHMR_DUAL3_Pos))
#define AFEC_SHMR_DUAL4_Pos                   (4U)                                           /**< (AFEC_SHMR) Dual Sample & Hold for channel 4 Position */
#define AFEC_SHMR_DUAL4_Msk                   (0x1U << AFEC_SHMR_DUAL4_Pos)                  /**< (AFEC_SHMR) Dual Sample & Hold for channel 4 Mask */
#define AFEC_SHMR_DUAL4(value)                (AFEC_SHMR_DUAL4_Msk & ((value) << AFEC_SHMR_DUAL4_Pos))
#define AFEC_SHMR_DUAL5_Pos                   (5U)                                           /**< (AFEC_SHMR) Dual Sample & Hold for channel 5 Position */
#define AFEC_SHMR_DUAL5_Msk                   (0x1U << AFEC_SHMR_DUAL5_Pos)                  /**< (AFEC_SHMR) Dual Sample & Hold for channel 5 Mask */
#define AFEC_SHMR_DUAL5(value)                (AFEC_SHMR_DUAL5_Msk & ((value) << AFEC_SHMR_DUAL5_Pos))
#define AFEC_SHMR_DUAL6_Pos                   (6U)                                           /**< (AFEC_SHMR) Dual Sample & Hold for channel 6 Position */
#define AFEC_SHMR_DUAL6_Msk                   (0x1U << AFEC_SHMR_DUAL6_Pos)                  /**< (AFEC_SHMR) Dual Sample & Hold for channel 6 Mask */
#define AFEC_SHMR_DUAL6(value)                (AFEC_SHMR_DUAL6_Msk & ((value) << AFEC_SHMR_DUAL6_Pos))
#define AFEC_SHMR_DUAL7_Pos                   (7U)                                           /**< (AFEC_SHMR) Dual Sample & Hold for channel 7 Position */
#define AFEC_SHMR_DUAL7_Msk                   (0x1U << AFEC_SHMR_DUAL7_Pos)                  /**< (AFEC_SHMR) Dual Sample & Hold for channel 7 Mask */
#define AFEC_SHMR_DUAL7(value)                (AFEC_SHMR_DUAL7_Msk & ((value) << AFEC_SHMR_DUAL7_Pos))
#define AFEC_SHMR_DUAL8_Pos                   (8U)                                           /**< (AFEC_SHMR) Dual Sample & Hold for channel 8 Position */
#define AFEC_SHMR_DUAL8_Msk                   (0x1U << AFEC_SHMR_DUAL8_Pos)                  /**< (AFEC_SHMR) Dual Sample & Hold for channel 8 Mask */
#define AFEC_SHMR_DUAL8(value)                (AFEC_SHMR_DUAL8_Msk & ((value) << AFEC_SHMR_DUAL8_Pos))
#define AFEC_SHMR_DUAL9_Pos                   (9U)                                           /**< (AFEC_SHMR) Dual Sample & Hold for channel 9 Position */
#define AFEC_SHMR_DUAL9_Msk                   (0x1U << AFEC_SHMR_DUAL9_Pos)                  /**< (AFEC_SHMR) Dual Sample & Hold for channel 9 Mask */
#define AFEC_SHMR_DUAL9(value)                (AFEC_SHMR_DUAL9_Msk & ((value) << AFEC_SHMR_DUAL9_Pos))
#define AFEC_SHMR_DUAL10_Pos                  (10U)                                          /**< (AFEC_SHMR) Dual Sample & Hold for channel 10 Position */
#define AFEC_SHMR_DUAL10_Msk                  (0x1U << AFEC_SHMR_DUAL10_Pos)                 /**< (AFEC_SHMR) Dual Sample & Hold for channel 10 Mask */
#define AFEC_SHMR_DUAL10(value)               (AFEC_SHMR_DUAL10_Msk & ((value) << AFEC_SHMR_DUAL10_Pos))
#define AFEC_SHMR_DUAL11_Pos                  (11U)                                          /**< (AFEC_SHMR) Dual Sample & Hold for channel 11 Position */
#define AFEC_SHMR_DUAL11_Msk                  (0x1U << AFEC_SHMR_DUAL11_Pos)                 /**< (AFEC_SHMR) Dual Sample & Hold for channel 11 Mask */
#define AFEC_SHMR_DUAL11(value)               (AFEC_SHMR_DUAL11_Msk & ((value) << AFEC_SHMR_DUAL11_Pos))
#define AFEC_SHMR_Msk                         (0x00000FFFUL)                                 /**< (AFEC_SHMR) Register Mask  */

/* -------- AFEC_COSR : (AFEC Offset: 0xD0) (R/W 32) AFEC Correction Select Register -------- */
#define AFEC_COSR_CSEL_Pos                    (0U)                                           /**< (AFEC_COSR) Sample & Hold unit Correction Select Position */
#define AFEC_COSR_CSEL_Msk                    (0x1U << AFEC_COSR_CSEL_Pos)                   /**< (AFEC_COSR) Sample & Hold unit Correction Select Mask */
#define AFEC_COSR_CSEL(value)                 (AFEC_COSR_CSEL_Msk & ((value) << AFEC_COSR_CSEL_Pos))
#define AFEC_COSR_Msk                         (0x00000001UL)                                 /**< (AFEC_COSR) Register Mask  */

/* -------- AFEC_CVR : (AFEC Offset: 0xD4) (R/W 32) AFEC Correction Values Register -------- */
#define AFEC_CVR_OFFSETCORR_Pos               (0U)                                           /**< (AFEC_CVR) Offset Correction Position */
#define AFEC_CVR_OFFSETCORR_Msk               (0xFFFFU << AFEC_CVR_OFFSETCORR_Pos)           /**< (AFEC_CVR) Offset Correction Mask */
#define AFEC_CVR_OFFSETCORR(value)            (AFEC_CVR_OFFSETCORR_Msk & ((value) << AFEC_CVR_OFFSETCORR_Pos))
#define AFEC_CVR_GAINCORR_Pos                 (16U)                                          /**< (AFEC_CVR) Gain Correction Position */
#define AFEC_CVR_GAINCORR_Msk                 (0xFFFFU << AFEC_CVR_GAINCORR_Pos)             /**< (AFEC_CVR) Gain Correction Mask */
#define AFEC_CVR_GAINCORR(value)              (AFEC_CVR_GAINCORR_Msk & ((value) << AFEC_CVR_GAINCORR_Pos))
#define AFEC_CVR_Msk                          (0xFFFFFFFFUL)                                 /**< (AFEC_CVR) Register Mask  */

/* -------- AFEC_CECR : (AFEC Offset: 0xD8) (R/W 32) AFEC Channel Error Correction Register -------- */
#define AFEC_CECR_ECORR0_Pos                  (0U)                                           /**< (AFEC_CECR) Error Correction Enable for channel 0 Position */
#define AFEC_CECR_ECORR0_Msk                  (0x1U << AFEC_CECR_ECORR0_Pos)                 /**< (AFEC_CECR) Error Correction Enable for channel 0 Mask */
#define AFEC_CECR_ECORR0(value)               (AFEC_CECR_ECORR0_Msk & ((value) << AFEC_CECR_ECORR0_Pos))
#define AFEC_CECR_ECORR1_Pos                  (1U)                                           /**< (AFEC_CECR) Error Correction Enable for channel 1 Position */
#define AFEC_CECR_ECORR1_Msk                  (0x1U << AFEC_CECR_ECORR1_Pos)                 /**< (AFEC_CECR) Error Correction Enable for channel 1 Mask */
#define AFEC_CECR_ECORR1(value)               (AFEC_CECR_ECORR1_Msk & ((value) << AFEC_CECR_ECORR1_Pos))
#define AFEC_CECR_ECORR2_Pos                  (2U)                                           /**< (AFEC_CECR) Error Correction Enable for channel 2 Position */
#define AFEC_CECR_ECORR2_Msk                  (0x1U << AFEC_CECR_ECORR2_Pos)                 /**< (AFEC_CECR) Error Correction Enable for channel 2 Mask */
#define AFEC_CECR_ECORR2(value)               (AFEC_CECR_ECORR2_Msk & ((value) << AFEC_CECR_ECORR2_Pos))
#define AFEC_CECR_ECORR3_Pos                  (3U)                                           /**< (AFEC_CECR) Error Correction Enable for channel 3 Position */
#define AFEC_CECR_ECORR3_Msk                  (0x1U << AFEC_CECR_ECORR3_Pos)                 /**< (AFEC_CECR) Error Correction Enable for channel 3 Mask */
#define AFEC_CECR_ECORR3(value)               (AFEC_CECR_ECORR3_Msk & ((value) << AFEC_CECR_ECORR3_Pos))
#define AFEC_CECR_ECORR4_Pos                  (4U)                                           /**< (AFEC_CECR) Error Correction Enable for channel 4 Position */
#define AFEC_CECR_ECORR4_Msk                  (0x1U << AFEC_CECR_ECORR4_Pos)                 /**< (AFEC_CECR) Error Correction Enable for channel 4 Mask */
#define AFEC_CECR_ECORR4(value)               (AFEC_CECR_ECORR4_Msk & ((value) << AFEC_CECR_ECORR4_Pos))
#define AFEC_CECR_ECORR5_Pos                  (5U)                                           /**< (AFEC_CECR) Error Correction Enable for channel 5 Position */
#define AFEC_CECR_ECORR5_Msk                  (0x1U << AFEC_CECR_ECORR5_Pos)                 /**< (AFEC_CECR) Error Correction Enable for channel 5 Mask */
#define AFEC_CECR_ECORR5(value)               (AFEC_CECR_ECORR5_Msk & ((value) << AFEC_CECR_ECORR5_Pos))
#define AFEC_CECR_ECORR6_Pos                  (6U)                                           /**< (AFEC_CECR) Error Correction Enable for channel 6 Position */
#define AFEC_CECR_ECORR6_Msk                  (0x1U << AFEC_CECR_ECORR6_Pos)                 /**< (AFEC_CECR) Error Correction Enable for channel 6 Mask */
#define AFEC_CECR_ECORR6(value)               (AFEC_CECR_ECORR6_Msk & ((value) << AFEC_CECR_ECORR6_Pos))
#define AFEC_CECR_ECORR7_Pos                  (7U)                                           /**< (AFEC_CECR) Error Correction Enable for channel 7 Position */
#define AFEC_CECR_ECORR7_Msk                  (0x1U << AFEC_CECR_ECORR7_Pos)                 /**< (AFEC_CECR) Error Correction Enable for channel 7 Mask */
#define AFEC_CECR_ECORR7(value)               (AFEC_CECR_ECORR7_Msk & ((value) << AFEC_CECR_ECORR7_Pos))
#define AFEC_CECR_ECORR8_Pos                  (8U)                                           /**< (AFEC_CECR) Error Correction Enable for channel 8 Position */
#define AFEC_CECR_ECORR8_Msk                  (0x1U << AFEC_CECR_ECORR8_Pos)                 /**< (AFEC_CECR) Error Correction Enable for channel 8 Mask */
#define AFEC_CECR_ECORR8(value)               (AFEC_CECR_ECORR8_Msk & ((value) << AFEC_CECR_ECORR8_Pos))
#define AFEC_CECR_ECORR9_Pos                  (9U)                                           /**< (AFEC_CECR) Error Correction Enable for channel 9 Position */
#define AFEC_CECR_ECORR9_Msk                  (0x1U << AFEC_CECR_ECORR9_Pos)                 /**< (AFEC_CECR) Error Correction Enable for channel 9 Mask */
#define AFEC_CECR_ECORR9(value)               (AFEC_CECR_ECORR9_Msk & ((value) << AFEC_CECR_ECORR9_Pos))
#define AFEC_CECR_ECORR10_Pos                 (10U)                                          /**< (AFEC_CECR) Error Correction Enable for channel 10 Position */
#define AFEC_CECR_ECORR10_Msk                 (0x1U << AFEC_CECR_ECORR10_Pos)                /**< (AFEC_CECR) Error Correction Enable for channel 10 Mask */
#define AFEC_CECR_ECORR10(value)              (AFEC_CECR_ECORR10_Msk & ((value) << AFEC_CECR_ECORR10_Pos))
#define AFEC_CECR_ECORR11_Pos                 (11U)                                          /**< (AFEC_CECR) Error Correction Enable for channel 11 Position */
#define AFEC_CECR_ECORR11_Msk                 (0x1U << AFEC_CECR_ECORR11_Pos)                /**< (AFEC_CECR) Error Correction Enable for channel 11 Mask */
#define AFEC_CECR_ECORR11(value)              (AFEC_CECR_ECORR11_Msk & ((value) << AFEC_CECR_ECORR11_Pos))
#define AFEC_CECR_Msk                         (0x00000FFFUL)                                 /**< (AFEC_CECR) Register Mask  */

/* -------- AFEC_WPMR : (AFEC Offset: 0xE4) (R/W 32) AFEC Write Protection Mode Register -------- */
#define AFEC_WPMR_WPEN_Pos                    (0U)                                           /**< (AFEC_WPMR) Write Protection Enable Position */
#define AFEC_WPMR_WPEN_Msk                    (0x1U << AFEC_WPMR_WPEN_Pos)                   /**< (AFEC_WPMR) Write Protection Enable Mask */
#define AFEC_WPMR_WPEN(value)                 (AFEC_WPMR_WPEN_Msk & ((value) << AFEC_WPMR_WPEN_Pos))
#define AFEC_WPMR_WPKEY_Pos                   (8U)                                           /**< (AFEC_WPMR) Write Protect KEY Position */
#define AFEC_WPMR_WPKEY_Msk                   (0xFFFFFFU << AFEC_WPMR_WPKEY_Pos)             /**< (AFEC_WPMR) Write Protect KEY Mask */
#define AFEC_WPMR_WPKEY(value)                (AFEC_WPMR_WPKEY_Msk & ((value) << AFEC_WPMR_WPKEY_Pos))
#define   AFEC_WPMR_WPKEY_PASSWD_Val          (4277315U)                                     /**< (AFEC_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0.  */
#define AFEC_WPMR_WPKEY_PASSWD                (AFEC_WPMR_WPKEY_PASSWD_Val << AFEC_WPMR_WPKEY_Pos) /**< (AFEC_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0. Position  */
#define AFEC_WPMR_Msk                         (0xFFFFFF01UL)                                 /**< (AFEC_WPMR) Register Mask  */

/* -------- AFEC_WPSR : (AFEC Offset: 0xE8) (R/  32) AFEC Write Protection Status Register -------- */
#define AFEC_WPSR_WPVS_Pos                    (0U)                                           /**< (AFEC_WPSR) Write Protect Violation Status Position */
#define AFEC_WPSR_WPVS_Msk                    (0x1U << AFEC_WPSR_WPVS_Pos)                   /**< (AFEC_WPSR) Write Protect Violation Status Mask */
#define AFEC_WPSR_WPVS(value)                 (AFEC_WPSR_WPVS_Msk & ((value) << AFEC_WPSR_WPVS_Pos))
#define AFEC_WPSR_WPVSRC_Pos                  (8U)                                           /**< (AFEC_WPSR) Write Protect Violation Source Position */
#define AFEC_WPSR_WPVSRC_Msk                  (0xFFFFU << AFEC_WPSR_WPVSRC_Pos)              /**< (AFEC_WPSR) Write Protect Violation Source Mask */
#define AFEC_WPSR_WPVSRC(value)               (AFEC_WPSR_WPVSRC_Msk & ((value) << AFEC_WPSR_WPVSRC_Pos))
#define AFEC_WPSR_Msk                         (0x00FFFF01UL)                                 /**< (AFEC_WPSR) Register Mask  */

/** \brief AFEC register offsets definitions */
#define AFEC_CR_OFFSET                 (0x00)         /**< (AFEC_CR) AFEC Control Register Offset */
#define AFEC_MR_OFFSET                 (0x04)         /**< (AFEC_MR) AFEC Mode Register Offset */
#define AFEC_EMR_OFFSET                (0x08)         /**< (AFEC_EMR) AFEC Extended Mode Register Offset */
#define AFEC_SEQ1R_OFFSET              (0x0C)         /**< (AFEC_SEQ1R) AFEC Channel Sequence 1 Register Offset */
#define AFEC_SEQ2R_OFFSET              (0x10)         /**< (AFEC_SEQ2R) AFEC Channel Sequence 2 Register Offset */
#define AFEC_CHER_OFFSET               (0x14)         /**< (AFEC_CHER) AFEC Channel Enable Register Offset */
#define AFEC_CHDR_OFFSET               (0x18)         /**< (AFEC_CHDR) AFEC Channel Disable Register Offset */
#define AFEC_CHSR_OFFSET               (0x1C)         /**< (AFEC_CHSR) AFEC Channel Status Register Offset */
#define AFEC_LCDR_OFFSET               (0x20)         /**< (AFEC_LCDR) AFEC Last Converted Data Register Offset */
#define AFEC_IER_OFFSET                (0x24)         /**< (AFEC_IER) AFEC Interrupt Enable Register Offset */
#define AFEC_IDR_OFFSET                (0x28)         /**< (AFEC_IDR) AFEC Interrupt Disable Register Offset */
#define AFEC_IMR_OFFSET                (0x2C)         /**< (AFEC_IMR) AFEC Interrupt Mask Register Offset */
#define AFEC_ISR_OFFSET                (0x30)         /**< (AFEC_ISR) AFEC Interrupt Status Register Offset */
#define AFEC_OVER_OFFSET               (0x4C)         /**< (AFEC_OVER) AFEC Overrun Status Register Offset */
#define AFEC_CWR_OFFSET                (0x50)         /**< (AFEC_CWR) AFEC Compare Window Register Offset */
#define AFEC_CGR_OFFSET                (0x54)         /**< (AFEC_CGR) AFEC Channel Gain Register Offset */
#define AFEC_DIFFR_OFFSET              (0x60)         /**< (AFEC_DIFFR) AFEC Channel Differential Register Offset */
#define AFEC_CSELR_OFFSET              (0x64)         /**< (AFEC_CSELR) AFEC Channel Selection Register Offset */
#define AFEC_CDR_OFFSET                (0x68)         /**< (AFEC_CDR) AFEC Channel Data Register Offset */
#define AFEC_COCR_OFFSET               (0x6C)         /**< (AFEC_COCR) AFEC Channel Offset Compensation Register Offset */
#define AFEC_TEMPMR_OFFSET             (0x70)         /**< (AFEC_TEMPMR) AFEC Temperature Sensor Mode Register Offset */
#define AFEC_TEMPCWR_OFFSET            (0x74)         /**< (AFEC_TEMPCWR) AFEC Temperature Compare Window Register Offset */
#define AFEC_ACR_OFFSET                (0x94)         /**< (AFEC_ACR) AFEC Analog Control Register Offset */
#define AFEC_SHMR_OFFSET               (0xA0)         /**< (AFEC_SHMR) AFEC Sample & Hold Mode Register Offset */
#define AFEC_COSR_OFFSET               (0xD0)         /**< (AFEC_COSR) AFEC Correction Select Register Offset */
#define AFEC_CVR_OFFSET                (0xD4)         /**< (AFEC_CVR) AFEC Correction Values Register Offset */
#define AFEC_CECR_OFFSET               (0xD8)         /**< (AFEC_CECR) AFEC Channel Error Correction Register Offset */
#define AFEC_WPMR_OFFSET               (0xE4)         /**< (AFEC_WPMR) AFEC Write Protection Mode Register Offset */
#define AFEC_WPSR_OFFSET               (0xE8)         /**< (AFEC_WPSR) AFEC Write Protection Status Register Offset */

/** \brief AFEC register API structure */
typedef struct
{
  __O   uint32_t                       AFEC_CR;         /**< Offset: 0x00 ( /W  32) AFEC Control Register */
  __IO  uint32_t                       AFEC_MR;         /**< Offset: 0x04 (R/W  32) AFEC Mode Register */
  __IO  uint32_t                       AFEC_EMR;        /**< Offset: 0x08 (R/W  32) AFEC Extended Mode Register */
  __IO  uint32_t                       AFEC_SEQ1R;      /**< Offset: 0x0c (R/W  32) AFEC Channel Sequence 1 Register */
  __IO  uint32_t                       AFEC_SEQ2R;      /**< Offset: 0x10 (R/W  32) AFEC Channel Sequence 2 Register */
  __O   uint32_t                       AFEC_CHER;       /**< Offset: 0x14 ( /W  32) AFEC Channel Enable Register */
  __O   uint32_t                       AFEC_CHDR;       /**< Offset: 0x18 ( /W  32) AFEC Channel Disable Register */
  __I   uint32_t                       AFEC_CHSR;       /**< Offset: 0x1c (R/   32) AFEC Channel Status Register */
  __I   uint32_t                       AFEC_LCDR;       /**< Offset: 0x20 (R/   32) AFEC Last Converted Data Register */
  __O   uint32_t                       AFEC_IER;        /**< Offset: 0x24 ( /W  32) AFEC Interrupt Enable Register */
  __O   uint32_t                       AFEC_IDR;        /**< Offset: 0x28 ( /W  32) AFEC Interrupt Disable Register */
  __I   uint32_t                       AFEC_IMR;        /**< Offset: 0x2c (R/   32) AFEC Interrupt Mask Register */
  __I   uint32_t                       AFEC_ISR;        /**< Offset: 0x30 (R/   32) AFEC Interrupt Status Register */
  __I   uint8_t                        Reserved1[0x18];
  __I   uint32_t                       AFEC_OVER;       /**< Offset: 0x4c (R/   32) AFEC Overrun Status Register */
  __IO  uint32_t                       AFEC_CWR;        /**< Offset: 0x50 (R/W  32) AFEC Compare Window Register */
  __IO  uint32_t                       AFEC_CGR;        /**< Offset: 0x54 (R/W  32) AFEC Channel Gain Register */
  __I   uint8_t                        Reserved2[0x08];
  __IO  uint32_t                       AFEC_DIFFR;      /**< Offset: 0x60 (R/W  32) AFEC Channel Differential Register */
  __IO  uint32_t                       AFEC_CSELR;      /**< Offset: 0x64 (R/W  32) AFEC Channel Selection Register */
  __I   uint32_t                       AFEC_CDR;        /**< Offset: 0x68 (R/   32) AFEC Channel Data Register */
  __IO  uint32_t                       AFEC_COCR;       /**< Offset: 0x6c (R/W  32) AFEC Channel Offset Compensation Register */
  __IO  uint32_t                       AFEC_TEMPMR;     /**< Offset: 0x70 (R/W  32) AFEC Temperature Sensor Mode Register */
  __IO  uint32_t                       AFEC_TEMPCWR;    /**< Offset: 0x74 (R/W  32) AFEC Temperature Compare Window Register */
  __I   uint8_t                        Reserved3[0x1C];
  __IO  uint32_t                       AFEC_ACR;        /**< Offset: 0x94 (R/W  32) AFEC Analog Control Register */
  __I   uint8_t                        Reserved4[0x08];
  __IO  uint32_t                       AFEC_SHMR;       /**< Offset: 0xa0 (R/W  32) AFEC Sample & Hold Mode Register */
  __I   uint8_t                        Reserved5[0x2C];
  __IO  uint32_t                       AFEC_COSR;       /**< Offset: 0xd0 (R/W  32) AFEC Correction Select Register */
  __IO  uint32_t                       AFEC_CVR;        /**< Offset: 0xd4 (R/W  32) AFEC Correction Values Register */
  __IO  uint32_t                       AFEC_CECR;       /**< Offset: 0xd8 (R/W  32) AFEC Channel Error Correction Register */
  __I   uint8_t                        Reserved6[0x08];
  __IO  uint32_t                       AFEC_WPMR;       /**< Offset: 0xe4 (R/W  32) AFEC Write Protection Mode Register */
  __I   uint32_t                       AFEC_WPSR;       /**< Offset: 0xe8 (R/   32) AFEC Write Protection Status Register */
} afec_registers_t;
/** @}  end of Analog Front-End Controller */

#endif /* SAME_SAME70_AFEC_MODULE_H */

