/**
 * \brief Header file for SAME/SAME70 SMC
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
#ifndef SAME_SAME70_SMC_MODULE_H
#define SAME_SAME70_SMC_MODULE_H

/** \addtogroup SAME_SAME70 Static Memory Controller
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_SMC                                   */
/* ========================================================================== */

/* -------- SMC_SETUP : (SMC Offset: 0x00) (R/W 32) SMC Setup Register -------- */
#define SMC_SETUP_NWE_SETUP_Pos               (0U)                                           /**< (SMC_SETUP) NWE Setup Length Position */
#define SMC_SETUP_NWE_SETUP_Msk               (0x3FU << SMC_SETUP_NWE_SETUP_Pos)             /**< (SMC_SETUP) NWE Setup Length Mask */
#define SMC_SETUP_NWE_SETUP(value)            (SMC_SETUP_NWE_SETUP_Msk & ((value) << SMC_SETUP_NWE_SETUP_Pos))
#define SMC_SETUP_NCS_WR_SETUP_Pos            (8U)                                           /**< (SMC_SETUP) NCS Setup Length in WRITE Access Position */
#define SMC_SETUP_NCS_WR_SETUP_Msk            (0x3FU << SMC_SETUP_NCS_WR_SETUP_Pos)          /**< (SMC_SETUP) NCS Setup Length in WRITE Access Mask */
#define SMC_SETUP_NCS_WR_SETUP(value)         (SMC_SETUP_NCS_WR_SETUP_Msk & ((value) << SMC_SETUP_NCS_WR_SETUP_Pos))
#define SMC_SETUP_NRD_SETUP_Pos               (16U)                                          /**< (SMC_SETUP) NRD Setup Length Position */
#define SMC_SETUP_NRD_SETUP_Msk               (0x3FU << SMC_SETUP_NRD_SETUP_Pos)             /**< (SMC_SETUP) NRD Setup Length Mask */
#define SMC_SETUP_NRD_SETUP(value)            (SMC_SETUP_NRD_SETUP_Msk & ((value) << SMC_SETUP_NRD_SETUP_Pos))
#define SMC_SETUP_NCS_RD_SETUP_Pos            (24U)                                          /**< (SMC_SETUP) NCS Setup Length in READ Access Position */
#define SMC_SETUP_NCS_RD_SETUP_Msk            (0x3FU << SMC_SETUP_NCS_RD_SETUP_Pos)          /**< (SMC_SETUP) NCS Setup Length in READ Access Mask */
#define SMC_SETUP_NCS_RD_SETUP(value)         (SMC_SETUP_NCS_RD_SETUP_Msk & ((value) << SMC_SETUP_NCS_RD_SETUP_Pos))
#define SMC_SETUP_Msk                         (0x3F3F3F3FUL)                                 /**< (SMC_SETUP) Register Mask  */

/* -------- SMC_PULSE : (SMC Offset: 0x04) (R/W 32) SMC Pulse Register -------- */
#define SMC_PULSE_NWE_PULSE_Pos               (0U)                                           /**< (SMC_PULSE) NWE Pulse Length Position */
#define SMC_PULSE_NWE_PULSE_Msk               (0x7FU << SMC_PULSE_NWE_PULSE_Pos)             /**< (SMC_PULSE) NWE Pulse Length Mask */
#define SMC_PULSE_NWE_PULSE(value)            (SMC_PULSE_NWE_PULSE_Msk & ((value) << SMC_PULSE_NWE_PULSE_Pos))
#define SMC_PULSE_NCS_WR_PULSE_Pos            (8U)                                           /**< (SMC_PULSE) NCS Pulse Length in WRITE Access Position */
#define SMC_PULSE_NCS_WR_PULSE_Msk            (0x7FU << SMC_PULSE_NCS_WR_PULSE_Pos)          /**< (SMC_PULSE) NCS Pulse Length in WRITE Access Mask */
#define SMC_PULSE_NCS_WR_PULSE(value)         (SMC_PULSE_NCS_WR_PULSE_Msk & ((value) << SMC_PULSE_NCS_WR_PULSE_Pos))
#define SMC_PULSE_NRD_PULSE_Pos               (16U)                                          /**< (SMC_PULSE) NRD Pulse Length Position */
#define SMC_PULSE_NRD_PULSE_Msk               (0x7FU << SMC_PULSE_NRD_PULSE_Pos)             /**< (SMC_PULSE) NRD Pulse Length Mask */
#define SMC_PULSE_NRD_PULSE(value)            (SMC_PULSE_NRD_PULSE_Msk & ((value) << SMC_PULSE_NRD_PULSE_Pos))
#define SMC_PULSE_NCS_RD_PULSE_Pos            (24U)                                          /**< (SMC_PULSE) NCS Pulse Length in READ Access Position */
#define SMC_PULSE_NCS_RD_PULSE_Msk            (0x7FU << SMC_PULSE_NCS_RD_PULSE_Pos)          /**< (SMC_PULSE) NCS Pulse Length in READ Access Mask */
#define SMC_PULSE_NCS_RD_PULSE(value)         (SMC_PULSE_NCS_RD_PULSE_Msk & ((value) << SMC_PULSE_NCS_RD_PULSE_Pos))
#define SMC_PULSE_Msk                         (0x7F7F7F7FUL)                                 /**< (SMC_PULSE) Register Mask  */

/* -------- SMC_CYCLE : (SMC Offset: 0x08) (R/W 32) SMC Cycle Register -------- */
#define SMC_CYCLE_NWE_CYCLE_Pos               (0U)                                           /**< (SMC_CYCLE) Total Write Cycle Length Position */
#define SMC_CYCLE_NWE_CYCLE_Msk               (0x1FFU << SMC_CYCLE_NWE_CYCLE_Pos)            /**< (SMC_CYCLE) Total Write Cycle Length Mask */
#define SMC_CYCLE_NWE_CYCLE(value)            (SMC_CYCLE_NWE_CYCLE_Msk & ((value) << SMC_CYCLE_NWE_CYCLE_Pos))
#define SMC_CYCLE_NRD_CYCLE_Pos               (16U)                                          /**< (SMC_CYCLE) Total Read Cycle Length Position */
#define SMC_CYCLE_NRD_CYCLE_Msk               (0x1FFU << SMC_CYCLE_NRD_CYCLE_Pos)            /**< (SMC_CYCLE) Total Read Cycle Length Mask */
#define SMC_CYCLE_NRD_CYCLE(value)            (SMC_CYCLE_NRD_CYCLE_Msk & ((value) << SMC_CYCLE_NRD_CYCLE_Pos))
#define SMC_CYCLE_Msk                         (0x01FF01FFUL)                                 /**< (SMC_CYCLE) Register Mask  */

/* -------- SMC_MODE : (SMC Offset: 0x0C) (R/W 32) SMC Mode Register -------- */
#define SMC_MODE_READ_MODE_Pos                (0U)                                           /**< (SMC_MODE) Read Mode Position */
#define SMC_MODE_READ_MODE_Msk                (0x1U << SMC_MODE_READ_MODE_Pos)               /**< (SMC_MODE) Read Mode Mask */
#define SMC_MODE_READ_MODE(value)             (SMC_MODE_READ_MODE_Msk & ((value) << SMC_MODE_READ_MODE_Pos))
#define SMC_MODE_WRITE_MODE_Pos               (1U)                                           /**< (SMC_MODE) Write Mode Position */
#define SMC_MODE_WRITE_MODE_Msk               (0x1U << SMC_MODE_WRITE_MODE_Pos)              /**< (SMC_MODE) Write Mode Mask */
#define SMC_MODE_WRITE_MODE(value)            (SMC_MODE_WRITE_MODE_Msk & ((value) << SMC_MODE_WRITE_MODE_Pos))
#define SMC_MODE_EXNW_MODE_Pos                (4U)                                           /**< (SMC_MODE) NWAIT Mode Position */
#define SMC_MODE_EXNW_MODE_Msk                (0x3U << SMC_MODE_EXNW_MODE_Pos)               /**< (SMC_MODE) NWAIT Mode Mask */
#define SMC_MODE_EXNW_MODE(value)             (SMC_MODE_EXNW_MODE_Msk & ((value) << SMC_MODE_EXNW_MODE_Pos))
#define   SMC_MODE_EXNW_MODE_DISABLED_Val     (0U)                                           /**< (SMC_MODE) Disabled-The NWAIT input signal is ignored on the corresponding chip select.  */
#define   SMC_MODE_EXNW_MODE_FROZEN_Val       (2U)                                           /**< (SMC_MODE) Frozen Mode-If asserted, the NWAIT signal freezes the current read or write cycle. After deassertion, the read/write cycle is resumed from the point where it was stopped.  */
#define   SMC_MODE_EXNW_MODE_READY_Val        (3U)                                           /**< (SMC_MODE) Ready Mode-The NWAIT signal indicates the availability of the external device at the end of the pulse of the controlling read or write signal, to complete the access. If high, the access normally completes. If low, the access is extended until NWAIT returns high.  */
#define SMC_MODE_EXNW_MODE_DISABLED           (SMC_MODE_EXNW_MODE_DISABLED_Val << SMC_MODE_EXNW_MODE_Pos) /**< (SMC_MODE) Disabled-The NWAIT input signal is ignored on the corresponding chip select. Position  */
#define SMC_MODE_EXNW_MODE_FROZEN             (SMC_MODE_EXNW_MODE_FROZEN_Val << SMC_MODE_EXNW_MODE_Pos) /**< (SMC_MODE) Frozen Mode-If asserted, the NWAIT signal freezes the current read or write cycle. After deassertion, the read/write cycle is resumed from the point where it was stopped. Position  */
#define SMC_MODE_EXNW_MODE_READY              (SMC_MODE_EXNW_MODE_READY_Val << SMC_MODE_EXNW_MODE_Pos) /**< (SMC_MODE) Ready Mode-The NWAIT signal indicates the availability of the external device at the end of the pulse of the controlling read or write signal, to complete the access. If high, the access normally completes. If low, the access is extended until NWAIT returns high. Position  */
#define SMC_MODE_BAT_Pos                      (8U)                                           /**< (SMC_MODE) Byte Access Type Position */
#define SMC_MODE_BAT_Msk                      (0x1U << SMC_MODE_BAT_Pos)                     /**< (SMC_MODE) Byte Access Type Mask */
#define SMC_MODE_BAT(value)                   (SMC_MODE_BAT_Msk & ((value) << SMC_MODE_BAT_Pos))
#define   SMC_MODE_BAT_BYTE_SELECT_Val        (0U)                                           /**< (SMC_MODE) Byte select access type:- Write operation is controlled using NCS, NWE, NBS0, NBS1.- Read operation is controlled using NCS, NRD, NBS0, NBS1.  */
#define   SMC_MODE_BAT_BYTE_WRITE_Val         (1U)                                           /**< (SMC_MODE) Byte write access type:- Write operation is controlled using NCS, NWR0, NWR1.- Read operation is controlled using NCS and NRD.  */
#define SMC_MODE_BAT_BYTE_SELECT              (SMC_MODE_BAT_BYTE_SELECT_Val << SMC_MODE_BAT_Pos) /**< (SMC_MODE) Byte select access type:- Write operation is controlled using NCS, NWE, NBS0, NBS1.- Read operation is controlled using NCS, NRD, NBS0, NBS1. Position  */
#define SMC_MODE_BAT_BYTE_WRITE               (SMC_MODE_BAT_BYTE_WRITE_Val << SMC_MODE_BAT_Pos) /**< (SMC_MODE) Byte write access type:- Write operation is controlled using NCS, NWR0, NWR1.- Read operation is controlled using NCS and NRD. Position  */
#define SMC_MODE_DBW_Pos                      (12U)                                          /**< (SMC_MODE) Data Bus Width Position */
#define SMC_MODE_DBW_Msk                      (0x1U << SMC_MODE_DBW_Pos)                     /**< (SMC_MODE) Data Bus Width Mask */
#define SMC_MODE_DBW(value)                   (SMC_MODE_DBW_Msk & ((value) << SMC_MODE_DBW_Pos))
#define   SMC_MODE_DBW_8_BIT_Val              (0U)                                           /**< (SMC_MODE) 8-bit Data Bus  */
#define   SMC_MODE_DBW_16_BIT_Val             (1U)                                           /**< (SMC_MODE) 16-bit Data Bus  */
#define SMC_MODE_DBW_8_BIT                    (SMC_MODE_DBW_8_BIT_Val << SMC_MODE_DBW_Pos)   /**< (SMC_MODE) 8-bit Data Bus Position  */
#define SMC_MODE_DBW_16_BIT                   (SMC_MODE_DBW_16_BIT_Val << SMC_MODE_DBW_Pos)  /**< (SMC_MODE) 16-bit Data Bus Position  */
#define SMC_MODE_TDF_CYCLES_Pos               (16U)                                          /**< (SMC_MODE) Data Float Time Position */
#define SMC_MODE_TDF_CYCLES_Msk               (0xFU << SMC_MODE_TDF_CYCLES_Pos)              /**< (SMC_MODE) Data Float Time Mask */
#define SMC_MODE_TDF_CYCLES(value)            (SMC_MODE_TDF_CYCLES_Msk & ((value) << SMC_MODE_TDF_CYCLES_Pos))
#define SMC_MODE_TDF_MODE_Pos                 (20U)                                          /**< (SMC_MODE) TDF Optimization Position */
#define SMC_MODE_TDF_MODE_Msk                 (0x1U << SMC_MODE_TDF_MODE_Pos)                /**< (SMC_MODE) TDF Optimization Mask */
#define SMC_MODE_TDF_MODE(value)              (SMC_MODE_TDF_MODE_Msk & ((value) << SMC_MODE_TDF_MODE_Pos))
#define SMC_MODE_PMEN_Pos                     (24U)                                          /**< (SMC_MODE) Page Mode Enabled Position */
#define SMC_MODE_PMEN_Msk                     (0x1U << SMC_MODE_PMEN_Pos)                    /**< (SMC_MODE) Page Mode Enabled Mask */
#define SMC_MODE_PMEN(value)                  (SMC_MODE_PMEN_Msk & ((value) << SMC_MODE_PMEN_Pos))
#define SMC_MODE_PS_Pos                       (28U)                                          /**< (SMC_MODE) Page Size Position */
#define SMC_MODE_PS_Msk                       (0x3U << SMC_MODE_PS_Pos)                      /**< (SMC_MODE) Page Size Mask */
#define SMC_MODE_PS(value)                    (SMC_MODE_PS_Msk & ((value) << SMC_MODE_PS_Pos))
#define   SMC_MODE_PS_4_BYTE_Val              (0U)                                           /**< (SMC_MODE) 4-byte page  */
#define   SMC_MODE_PS_8_BYTE_Val              (1U)                                           /**< (SMC_MODE) 8-byte page  */
#define   SMC_MODE_PS_16_BYTE_Val             (2U)                                           /**< (SMC_MODE) 16-byte page  */
#define   SMC_MODE_PS_32_BYTE_Val             (3U)                                           /**< (SMC_MODE) 32-byte page  */
#define SMC_MODE_PS_4_BYTE                    (SMC_MODE_PS_4_BYTE_Val << SMC_MODE_PS_Pos)    /**< (SMC_MODE) 4-byte page Position  */
#define SMC_MODE_PS_8_BYTE                    (SMC_MODE_PS_8_BYTE_Val << SMC_MODE_PS_Pos)    /**< (SMC_MODE) 8-byte page Position  */
#define SMC_MODE_PS_16_BYTE                   (SMC_MODE_PS_16_BYTE_Val << SMC_MODE_PS_Pos)   /**< (SMC_MODE) 16-byte page Position  */
#define SMC_MODE_PS_32_BYTE                   (SMC_MODE_PS_32_BYTE_Val << SMC_MODE_PS_Pos)   /**< (SMC_MODE) 32-byte page Position  */
#define SMC_MODE_Msk                          (0x311F1133UL)                                 /**< (SMC_MODE) Register Mask  */

/* -------- SMC_OCMS : (SMC Offset: 0x80) (R/W 32) SMC Off-Chip Memory Scrambling Register -------- */
#define SMC_OCMS_SMSE_Pos                     (0U)                                           /**< (SMC_OCMS) Static Memory Controller Scrambling Enable Position */
#define SMC_OCMS_SMSE_Msk                     (0x1U << SMC_OCMS_SMSE_Pos)                    /**< (SMC_OCMS) Static Memory Controller Scrambling Enable Mask */
#define SMC_OCMS_SMSE(value)                  (SMC_OCMS_SMSE_Msk & ((value) << SMC_OCMS_SMSE_Pos))
#define SMC_OCMS_CS0SE_Pos                    (8U)                                           /**< (SMC_OCMS) Chip Select (x = 0 to 3) Scrambling Enable Position */
#define SMC_OCMS_CS0SE_Msk                    (0x1U << SMC_OCMS_CS0SE_Pos)                   /**< (SMC_OCMS) Chip Select (x = 0 to 3) Scrambling Enable Mask */
#define SMC_OCMS_CS0SE(value)                 (SMC_OCMS_CS0SE_Msk & ((value) << SMC_OCMS_CS0SE_Pos))
#define SMC_OCMS_CS1SE_Pos                    (9U)                                           /**< (SMC_OCMS) Chip Select (x = 0 to 3) Scrambling Enable Position */
#define SMC_OCMS_CS1SE_Msk                    (0x1U << SMC_OCMS_CS1SE_Pos)                   /**< (SMC_OCMS) Chip Select (x = 0 to 3) Scrambling Enable Mask */
#define SMC_OCMS_CS1SE(value)                 (SMC_OCMS_CS1SE_Msk & ((value) << SMC_OCMS_CS1SE_Pos))
#define SMC_OCMS_CS2SE_Pos                    (10U)                                          /**< (SMC_OCMS) Chip Select (x = 0 to 3) Scrambling Enable Position */
#define SMC_OCMS_CS2SE_Msk                    (0x1U << SMC_OCMS_CS2SE_Pos)                   /**< (SMC_OCMS) Chip Select (x = 0 to 3) Scrambling Enable Mask */
#define SMC_OCMS_CS2SE(value)                 (SMC_OCMS_CS2SE_Msk & ((value) << SMC_OCMS_CS2SE_Pos))
#define SMC_OCMS_CS3SE_Pos                    (11U)                                          /**< (SMC_OCMS) Chip Select (x = 0 to 3) Scrambling Enable Position */
#define SMC_OCMS_CS3SE_Msk                    (0x1U << SMC_OCMS_CS3SE_Pos)                   /**< (SMC_OCMS) Chip Select (x = 0 to 3) Scrambling Enable Mask */
#define SMC_OCMS_CS3SE(value)                 (SMC_OCMS_CS3SE_Msk & ((value) << SMC_OCMS_CS3SE_Pos))
#define SMC_OCMS_Msk                          (0x00000F01UL)                                 /**< (SMC_OCMS) Register Mask  */

/* -------- SMC_KEY1 : (SMC Offset: 0x84) ( /W 32) SMC Off-Chip Memory Scrambling KEY1 Register -------- */
#define SMC_KEY1_KEY1_Pos                     (0U)                                           /**< (SMC_KEY1) Off-Chip Memory Scrambling (OCMS) Key Part 1 Position */
#define SMC_KEY1_KEY1_Msk                     (0xFFFFFFFFU << SMC_KEY1_KEY1_Pos)             /**< (SMC_KEY1) Off-Chip Memory Scrambling (OCMS) Key Part 1 Mask */
#define SMC_KEY1_KEY1(value)                  (SMC_KEY1_KEY1_Msk & ((value) << SMC_KEY1_KEY1_Pos))
#define SMC_KEY1_Msk                          (0xFFFFFFFFUL)                                 /**< (SMC_KEY1) Register Mask  */

/* -------- SMC_KEY2 : (SMC Offset: 0x88) ( /W 32) SMC Off-Chip Memory Scrambling KEY2 Register -------- */
#define SMC_KEY2_KEY2_Pos                     (0U)                                           /**< (SMC_KEY2) Off-Chip Memory Scrambling (OCMS) Key Part 2 Position */
#define SMC_KEY2_KEY2_Msk                     (0xFFFFFFFFU << SMC_KEY2_KEY2_Pos)             /**< (SMC_KEY2) Off-Chip Memory Scrambling (OCMS) Key Part 2 Mask */
#define SMC_KEY2_KEY2(value)                  (SMC_KEY2_KEY2_Msk & ((value) << SMC_KEY2_KEY2_Pos))
#define SMC_KEY2_Msk                          (0xFFFFFFFFUL)                                 /**< (SMC_KEY2) Register Mask  */

/* -------- SMC_WPMR : (SMC Offset: 0xE4) (R/W 32) SMC Write Protection Mode Register -------- */
#define SMC_WPMR_WPEN_Pos                     (0U)                                           /**< (SMC_WPMR) Write Protect Enable Position */
#define SMC_WPMR_WPEN_Msk                     (0x1U << SMC_WPMR_WPEN_Pos)                    /**< (SMC_WPMR) Write Protect Enable Mask */
#define SMC_WPMR_WPEN(value)                  (SMC_WPMR_WPEN_Msk & ((value) << SMC_WPMR_WPEN_Pos))
#define SMC_WPMR_WPKEY_Pos                    (8U)                                           /**< (SMC_WPMR) Write Protection Key Position */
#define SMC_WPMR_WPKEY_Msk                    (0xFFFFFFU << SMC_WPMR_WPKEY_Pos)              /**< (SMC_WPMR) Write Protection Key Mask */
#define SMC_WPMR_WPKEY(value)                 (SMC_WPMR_WPKEY_Msk & ((value) << SMC_WPMR_WPKEY_Pos))
#define   SMC_WPMR_WPKEY_PASSWD_Val           (5459267U)                                     /**< (SMC_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0.  */
#define SMC_WPMR_WPKEY_PASSWD                 (SMC_WPMR_WPKEY_PASSWD_Val << SMC_WPMR_WPKEY_Pos) /**< (SMC_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0. Position  */
#define SMC_WPMR_Msk                          (0xFFFFFF01UL)                                 /**< (SMC_WPMR) Register Mask  */

/* -------- SMC_WPSR : (SMC Offset: 0xE8) (R/  32) SMC Write Protection Status Register -------- */
#define SMC_WPSR_WPVS_Pos                     (0U)                                           /**< (SMC_WPSR) Write Protection Violation Status Position */
#define SMC_WPSR_WPVS_Msk                     (0x1U << SMC_WPSR_WPVS_Pos)                    /**< (SMC_WPSR) Write Protection Violation Status Mask */
#define SMC_WPSR_WPVS(value)                  (SMC_WPSR_WPVS_Msk & ((value) << SMC_WPSR_WPVS_Pos))
#define SMC_WPSR_WPVSRC_Pos                   (8U)                                           /**< (SMC_WPSR) Write Protection Violation Source Position */
#define SMC_WPSR_WPVSRC_Msk                   (0xFFFFU << SMC_WPSR_WPVSRC_Pos)               /**< (SMC_WPSR) Write Protection Violation Source Mask */
#define SMC_WPSR_WPVSRC(value)                (SMC_WPSR_WPVSRC_Msk & ((value) << SMC_WPSR_WPVSRC_Pos))
#define SMC_WPSR_Msk                          (0x00FFFF01UL)                                 /**< (SMC_WPSR) Register Mask  */

/** \brief SMC register offsets definitions */
#define SMC_CS_NUMBER_OFFSET           (0x00)         /**< (SMC_CS_NUMBER) SMC Setup Register Offset */
  #define SMC_SETUP_OFFSET               (0x00)         /**< (SMC_SETUP) SMC Setup Register Offset */
  #define SMC_PULSE_OFFSET               (0x04)         /**< (SMC_PULSE) SMC Pulse Register Offset */
  #define SMC_CYCLE_OFFSET               (0x08)         /**< (SMC_CYCLE) SMC Cycle Register Offset */
  #define SMC_MODE_OFFSET                (0x0C)         /**< (SMC_MODE) SMC Mode Register Offset */
#define SMC_OCMS_OFFSET                (0x80)         /**< (SMC_OCMS) SMC Off-Chip Memory Scrambling Register Offset */
#define SMC_KEY1_OFFSET                (0x84)         /**< (SMC_KEY1) SMC Off-Chip Memory Scrambling KEY1 Register Offset */
#define SMC_KEY2_OFFSET                (0x88)         /**< (SMC_KEY2) SMC Off-Chip Memory Scrambling KEY2 Register Offset */
#define SMC_WPMR_OFFSET                (0xE4)         /**< (SMC_WPMR) SMC Write Protection Mode Register Offset */
#define SMC_WPSR_OFFSET                (0xE8)         /**< (SMC_WPSR) SMC Write Protection Status Register Offset */

/** \brief SMC_CS_NUMBER register API structure */
typedef struct
{
  __IO  uint32_t                       SMC_SETUP;       /**< Offset: 0x00 (R/W  32) SMC Setup Register */
  __IO  uint32_t                       SMC_PULSE;       /**< Offset: 0x04 (R/W  32) SMC Pulse Register */
  __IO  uint32_t                       SMC_CYCLE;       /**< Offset: 0x08 (R/W  32) SMC Cycle Register */
  __IO  uint32_t                       SMC_MODE;        /**< Offset: 0x0c (R/W  32) SMC Mode Register */
} smc_cs_number_registers_t;
#define SMC_CS_NUMBER_NUMBER (4U)

/** \brief SMC register API structure */
typedef struct
{
        smc_cs_number_registers_t      SMC_CS_NUMBER[SMC_CS_NUMBER_NUMBER]; /**< Offset: 0x00 SMC Setup Register */
  __I   uint8_t                        Reserved1[0x40];
  __IO  uint32_t                       SMC_OCMS;        /**< Offset: 0x80 (R/W  32) SMC Off-Chip Memory Scrambling Register */
  __O   uint32_t                       SMC_KEY1;        /**< Offset: 0x84 ( /W  32) SMC Off-Chip Memory Scrambling KEY1 Register */
  __O   uint32_t                       SMC_KEY2;        /**< Offset: 0x88 ( /W  32) SMC Off-Chip Memory Scrambling KEY2 Register */
  __I   uint8_t                        Reserved2[0x58];
  __IO  uint32_t                       SMC_WPMR;        /**< Offset: 0xe4 (R/W  32) SMC Write Protection Mode Register */
  __I   uint32_t                       SMC_WPSR;        /**< Offset: 0xe8 (R/   32) SMC Write Protection Status Register */
} smc_registers_t;
/** @}  end of Static Memory Controller */

#endif /* SAME_SAME70_SMC_MODULE_H */

