/**
 * \brief Header file for SAME/SAME70 USBHS
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
#ifndef SAME_SAME70_USBHS_MODULE_H
#define SAME_SAME70_USBHS_MODULE_H

/** \addtogroup SAME_SAME70 USB High-Speed Interface
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_USBHS                                 */
/* ========================================================================== */

/* -------- USBHS_DEVCTRL : (USBHS Offset: 0x00) (R/W 32) Device General Control Register -------- */
#define USBHS_DEVCTRL_UADD_Pos                (0U)                                           /**< (USBHS_DEVCTRL) USB Address Position */
#define USBHS_DEVCTRL_UADD_Msk                (0x7FU << USBHS_DEVCTRL_UADD_Pos)              /**< (USBHS_DEVCTRL) USB Address Mask */
#define USBHS_DEVCTRL_UADD(value)             (USBHS_DEVCTRL_UADD_Msk & ((value) << USBHS_DEVCTRL_UADD_Pos))
#define USBHS_DEVCTRL_ADDEN_Pos               (7U)                                           /**< (USBHS_DEVCTRL) Address Enable Position */
#define USBHS_DEVCTRL_ADDEN_Msk               (0x1U << USBHS_DEVCTRL_ADDEN_Pos)              /**< (USBHS_DEVCTRL) Address Enable Mask */
#define USBHS_DEVCTRL_ADDEN(value)            (USBHS_DEVCTRL_ADDEN_Msk & ((value) << USBHS_DEVCTRL_ADDEN_Pos))
#define USBHS_DEVCTRL_DETACH_Pos              (8U)                                           /**< (USBHS_DEVCTRL) Detach Position */
#define USBHS_DEVCTRL_DETACH_Msk              (0x1U << USBHS_DEVCTRL_DETACH_Pos)             /**< (USBHS_DEVCTRL) Detach Mask */
#define USBHS_DEVCTRL_DETACH(value)           (USBHS_DEVCTRL_DETACH_Msk & ((value) << USBHS_DEVCTRL_DETACH_Pos))
#define USBHS_DEVCTRL_RMWKUP_Pos              (9U)                                           /**< (USBHS_DEVCTRL) Remote Wake-Up Position */
#define USBHS_DEVCTRL_RMWKUP_Msk              (0x1U << USBHS_DEVCTRL_RMWKUP_Pos)             /**< (USBHS_DEVCTRL) Remote Wake-Up Mask */
#define USBHS_DEVCTRL_RMWKUP(value)           (USBHS_DEVCTRL_RMWKUP_Msk & ((value) << USBHS_DEVCTRL_RMWKUP_Pos))
#define USBHS_DEVCTRL_SPDCONF_Pos             (10U)                                          /**< (USBHS_DEVCTRL) Mode Configuration Position */
#define USBHS_DEVCTRL_SPDCONF_Msk             (0x3U << USBHS_DEVCTRL_SPDCONF_Pos)            /**< (USBHS_DEVCTRL) Mode Configuration Mask */
#define USBHS_DEVCTRL_SPDCONF(value)          (USBHS_DEVCTRL_SPDCONF_Msk & ((value) << USBHS_DEVCTRL_SPDCONF_Pos))
#define   USBHS_DEVCTRL_SPDCONF_NORMAL_Val    (0U)                                           /**< (USBHS_DEVCTRL) The peripheral starts in Full-speed mode and performs a high-speed reset to switch to High-speed mode if the host is high-speed-capable.  */
#define   USBHS_DEVCTRL_SPDCONF_LOW_POWER_Val (1U)                                           /**< (USBHS_DEVCTRL) For a better consumption, if high speed is not needed.  */
#define   USBHS_DEVCTRL_SPDCONF_HIGH_SPEED_Val (2U)                                           /**< (USBHS_DEVCTRL) Forced high speed.  */
#define   USBHS_DEVCTRL_SPDCONF_FORCED_FS_Val (3U)                                           /**< (USBHS_DEVCTRL) The peripheral remains in Full-speed mode whatever the host speed capability.  */
#define USBHS_DEVCTRL_SPDCONF_NORMAL          (USBHS_DEVCTRL_SPDCONF_NORMAL_Val << USBHS_DEVCTRL_SPDCONF_Pos) /**< (USBHS_DEVCTRL) The peripheral starts in Full-speed mode and performs a high-speed reset to switch to High-speed mode if the host is high-speed-capable. Position  */
#define USBHS_DEVCTRL_SPDCONF_LOW_POWER       (USBHS_DEVCTRL_SPDCONF_LOW_POWER_Val << USBHS_DEVCTRL_SPDCONF_Pos) /**< (USBHS_DEVCTRL) For a better consumption, if high speed is not needed. Position  */
#define USBHS_DEVCTRL_SPDCONF_HIGH_SPEED      (USBHS_DEVCTRL_SPDCONF_HIGH_SPEED_Val << USBHS_DEVCTRL_SPDCONF_Pos) /**< (USBHS_DEVCTRL) Forced high speed. Position  */
#define USBHS_DEVCTRL_SPDCONF_FORCED_FS       (USBHS_DEVCTRL_SPDCONF_FORCED_FS_Val << USBHS_DEVCTRL_SPDCONF_Pos) /**< (USBHS_DEVCTRL) The peripheral remains in Full-speed mode whatever the host speed capability. Position  */
#define USBHS_DEVCTRL_LS_Pos                  (12U)                                          /**< (USBHS_DEVCTRL) Low-Speed Mode Force Position */
#define USBHS_DEVCTRL_LS_Msk                  (0x1U << USBHS_DEVCTRL_LS_Pos)                 /**< (USBHS_DEVCTRL) Low-Speed Mode Force Mask */
#define USBHS_DEVCTRL_LS(value)               (USBHS_DEVCTRL_LS_Msk & ((value) << USBHS_DEVCTRL_LS_Pos))
#define USBHS_DEVCTRL_TSTJ_Pos                (13U)                                          /**< (USBHS_DEVCTRL) Test mode J Position */
#define USBHS_DEVCTRL_TSTJ_Msk                (0x1U << USBHS_DEVCTRL_TSTJ_Pos)               /**< (USBHS_DEVCTRL) Test mode J Mask */
#define USBHS_DEVCTRL_TSTJ(value)             (USBHS_DEVCTRL_TSTJ_Msk & ((value) << USBHS_DEVCTRL_TSTJ_Pos))
#define USBHS_DEVCTRL_TSTK_Pos                (14U)                                          /**< (USBHS_DEVCTRL) Test mode K Position */
#define USBHS_DEVCTRL_TSTK_Msk                (0x1U << USBHS_DEVCTRL_TSTK_Pos)               /**< (USBHS_DEVCTRL) Test mode K Mask */
#define USBHS_DEVCTRL_TSTK(value)             (USBHS_DEVCTRL_TSTK_Msk & ((value) << USBHS_DEVCTRL_TSTK_Pos))
#define USBHS_DEVCTRL_TSTPCKT_Pos             (15U)                                          /**< (USBHS_DEVCTRL) Test packet mode Position */
#define USBHS_DEVCTRL_TSTPCKT_Msk             (0x1U << USBHS_DEVCTRL_TSTPCKT_Pos)            /**< (USBHS_DEVCTRL) Test packet mode Mask */
#define USBHS_DEVCTRL_TSTPCKT(value)          (USBHS_DEVCTRL_TSTPCKT_Msk & ((value) << USBHS_DEVCTRL_TSTPCKT_Pos))
#define USBHS_DEVCTRL_OPMODE2_Pos             (16U)                                          /**< (USBHS_DEVCTRL) Specific Operational mode Position */
#define USBHS_DEVCTRL_OPMODE2_Msk             (0x1U << USBHS_DEVCTRL_OPMODE2_Pos)            /**< (USBHS_DEVCTRL) Specific Operational mode Mask */
#define USBHS_DEVCTRL_OPMODE2(value)          (USBHS_DEVCTRL_OPMODE2_Msk & ((value) << USBHS_DEVCTRL_OPMODE2_Pos))
#define USBHS_DEVCTRL_Msk                     (0x0001FFFFUL)                                 /**< (USBHS_DEVCTRL) Register Mask  */

/* -------- USBHS_DEVISR : (USBHS Offset: 0x04) (R/  32) Device Global Interrupt Status Register -------- */
#define USBHS_DEVISR_SUSP_Pos                 (0U)                                           /**< (USBHS_DEVISR) Suspend Interrupt Position */
#define USBHS_DEVISR_SUSP_Msk                 (0x1U << USBHS_DEVISR_SUSP_Pos)                /**< (USBHS_DEVISR) Suspend Interrupt Mask */
#define USBHS_DEVISR_SUSP(value)              (USBHS_DEVISR_SUSP_Msk & ((value) << USBHS_DEVISR_SUSP_Pos))
#define USBHS_DEVISR_MSOF_Pos                 (1U)                                           /**< (USBHS_DEVISR) Micro Start of Frame Interrupt Position */
#define USBHS_DEVISR_MSOF_Msk                 (0x1U << USBHS_DEVISR_MSOF_Pos)                /**< (USBHS_DEVISR) Micro Start of Frame Interrupt Mask */
#define USBHS_DEVISR_MSOF(value)              (USBHS_DEVISR_MSOF_Msk & ((value) << USBHS_DEVISR_MSOF_Pos))
#define USBHS_DEVISR_SOF_Pos                  (2U)                                           /**< (USBHS_DEVISR) Start of Frame Interrupt Position */
#define USBHS_DEVISR_SOF_Msk                  (0x1U << USBHS_DEVISR_SOF_Pos)                 /**< (USBHS_DEVISR) Start of Frame Interrupt Mask */
#define USBHS_DEVISR_SOF(value)               (USBHS_DEVISR_SOF_Msk & ((value) << USBHS_DEVISR_SOF_Pos))
#define USBHS_DEVISR_EORST_Pos                (3U)                                           /**< (USBHS_DEVISR) End of Reset Interrupt Position */
#define USBHS_DEVISR_EORST_Msk                (0x1U << USBHS_DEVISR_EORST_Pos)               /**< (USBHS_DEVISR) End of Reset Interrupt Mask */
#define USBHS_DEVISR_EORST(value)             (USBHS_DEVISR_EORST_Msk & ((value) << USBHS_DEVISR_EORST_Pos))
#define USBHS_DEVISR_WAKEUP_Pos               (4U)                                           /**< (USBHS_DEVISR) Wake-Up Interrupt Position */
#define USBHS_DEVISR_WAKEUP_Msk               (0x1U << USBHS_DEVISR_WAKEUP_Pos)              /**< (USBHS_DEVISR) Wake-Up Interrupt Mask */
#define USBHS_DEVISR_WAKEUP(value)            (USBHS_DEVISR_WAKEUP_Msk & ((value) << USBHS_DEVISR_WAKEUP_Pos))
#define USBHS_DEVISR_EORSM_Pos                (5U)                                           /**< (USBHS_DEVISR) End of Resume Interrupt Position */
#define USBHS_DEVISR_EORSM_Msk                (0x1U << USBHS_DEVISR_EORSM_Pos)               /**< (USBHS_DEVISR) End of Resume Interrupt Mask */
#define USBHS_DEVISR_EORSM(value)             (USBHS_DEVISR_EORSM_Msk & ((value) << USBHS_DEVISR_EORSM_Pos))
#define USBHS_DEVISR_UPRSM_Pos                (6U)                                           /**< (USBHS_DEVISR) Upstream Resume Interrupt Position */
#define USBHS_DEVISR_UPRSM_Msk                (0x1U << USBHS_DEVISR_UPRSM_Pos)               /**< (USBHS_DEVISR) Upstream Resume Interrupt Mask */
#define USBHS_DEVISR_UPRSM(value)             (USBHS_DEVISR_UPRSM_Msk & ((value) << USBHS_DEVISR_UPRSM_Pos))
#define USBHS_DEVISR_PEP_0_Pos                (12U)                                          /**< (USBHS_DEVISR) Endpoint 0 Interrupt Position */
#define USBHS_DEVISR_PEP_0_Msk                (0x1U << USBHS_DEVISR_PEP_0_Pos)               /**< (USBHS_DEVISR) Endpoint 0 Interrupt Mask */
#define USBHS_DEVISR_PEP_0(value)             (USBHS_DEVISR_PEP_0_Msk & ((value) << USBHS_DEVISR_PEP_0_Pos))
#define USBHS_DEVISR_PEP_1_Pos                (13U)                                          /**< (USBHS_DEVISR) Endpoint 1 Interrupt Position */
#define USBHS_DEVISR_PEP_1_Msk                (0x1U << USBHS_DEVISR_PEP_1_Pos)               /**< (USBHS_DEVISR) Endpoint 1 Interrupt Mask */
#define USBHS_DEVISR_PEP_1(value)             (USBHS_DEVISR_PEP_1_Msk & ((value) << USBHS_DEVISR_PEP_1_Pos))
#define USBHS_DEVISR_PEP_2_Pos                (14U)                                          /**< (USBHS_DEVISR) Endpoint 2 Interrupt Position */
#define USBHS_DEVISR_PEP_2_Msk                (0x1U << USBHS_DEVISR_PEP_2_Pos)               /**< (USBHS_DEVISR) Endpoint 2 Interrupt Mask */
#define USBHS_DEVISR_PEP_2(value)             (USBHS_DEVISR_PEP_2_Msk & ((value) << USBHS_DEVISR_PEP_2_Pos))
#define USBHS_DEVISR_PEP_3_Pos                (15U)                                          /**< (USBHS_DEVISR) Endpoint 3 Interrupt Position */
#define USBHS_DEVISR_PEP_3_Msk                (0x1U << USBHS_DEVISR_PEP_3_Pos)               /**< (USBHS_DEVISR) Endpoint 3 Interrupt Mask */
#define USBHS_DEVISR_PEP_3(value)             (USBHS_DEVISR_PEP_3_Msk & ((value) << USBHS_DEVISR_PEP_3_Pos))
#define USBHS_DEVISR_PEP_4_Pos                (16U)                                          /**< (USBHS_DEVISR) Endpoint 4 Interrupt Position */
#define USBHS_DEVISR_PEP_4_Msk                (0x1U << USBHS_DEVISR_PEP_4_Pos)               /**< (USBHS_DEVISR) Endpoint 4 Interrupt Mask */
#define USBHS_DEVISR_PEP_4(value)             (USBHS_DEVISR_PEP_4_Msk & ((value) << USBHS_DEVISR_PEP_4_Pos))
#define USBHS_DEVISR_PEP_5_Pos                (17U)                                          /**< (USBHS_DEVISR) Endpoint 5 Interrupt Position */
#define USBHS_DEVISR_PEP_5_Msk                (0x1U << USBHS_DEVISR_PEP_5_Pos)               /**< (USBHS_DEVISR) Endpoint 5 Interrupt Mask */
#define USBHS_DEVISR_PEP_5(value)             (USBHS_DEVISR_PEP_5_Msk & ((value) << USBHS_DEVISR_PEP_5_Pos))
#define USBHS_DEVISR_PEP_6_Pos                (18U)                                          /**< (USBHS_DEVISR) Endpoint 6 Interrupt Position */
#define USBHS_DEVISR_PEP_6_Msk                (0x1U << USBHS_DEVISR_PEP_6_Pos)               /**< (USBHS_DEVISR) Endpoint 6 Interrupt Mask */
#define USBHS_DEVISR_PEP_6(value)             (USBHS_DEVISR_PEP_6_Msk & ((value) << USBHS_DEVISR_PEP_6_Pos))
#define USBHS_DEVISR_PEP_7_Pos                (19U)                                          /**< (USBHS_DEVISR) Endpoint 7 Interrupt Position */
#define USBHS_DEVISR_PEP_7_Msk                (0x1U << USBHS_DEVISR_PEP_7_Pos)               /**< (USBHS_DEVISR) Endpoint 7 Interrupt Mask */
#define USBHS_DEVISR_PEP_7(value)             (USBHS_DEVISR_PEP_7_Msk & ((value) << USBHS_DEVISR_PEP_7_Pos))
#define USBHS_DEVISR_PEP_8_Pos                (20U)                                          /**< (USBHS_DEVISR) Endpoint 8 Interrupt Position */
#define USBHS_DEVISR_PEP_8_Msk                (0x1U << USBHS_DEVISR_PEP_8_Pos)               /**< (USBHS_DEVISR) Endpoint 8 Interrupt Mask */
#define USBHS_DEVISR_PEP_8(value)             (USBHS_DEVISR_PEP_8_Msk & ((value) << USBHS_DEVISR_PEP_8_Pos))
#define USBHS_DEVISR_PEP_9_Pos                (21U)                                          /**< (USBHS_DEVISR) Endpoint 9 Interrupt Position */
#define USBHS_DEVISR_PEP_9_Msk                (0x1U << USBHS_DEVISR_PEP_9_Pos)               /**< (USBHS_DEVISR) Endpoint 9 Interrupt Mask */
#define USBHS_DEVISR_PEP_9(value)             (USBHS_DEVISR_PEP_9_Msk & ((value) << USBHS_DEVISR_PEP_9_Pos))
#define USBHS_DEVISR_DMA_1_Pos                (25U)                                          /**< (USBHS_DEVISR) DMA Channel 1 Interrupt Position */
#define USBHS_DEVISR_DMA_1_Msk                (0x1U << USBHS_DEVISR_DMA_1_Pos)               /**< (USBHS_DEVISR) DMA Channel 1 Interrupt Mask */
#define USBHS_DEVISR_DMA_1(value)             (USBHS_DEVISR_DMA_1_Msk & ((value) << USBHS_DEVISR_DMA_1_Pos))
#define USBHS_DEVISR_DMA_2_Pos                (26U)                                          /**< (USBHS_DEVISR) DMA Channel 2 Interrupt Position */
#define USBHS_DEVISR_DMA_2_Msk                (0x1U << USBHS_DEVISR_DMA_2_Pos)               /**< (USBHS_DEVISR) DMA Channel 2 Interrupt Mask */
#define USBHS_DEVISR_DMA_2(value)             (USBHS_DEVISR_DMA_2_Msk & ((value) << USBHS_DEVISR_DMA_2_Pos))
#define USBHS_DEVISR_DMA_3_Pos                (27U)                                          /**< (USBHS_DEVISR) DMA Channel 3 Interrupt Position */
#define USBHS_DEVISR_DMA_3_Msk                (0x1U << USBHS_DEVISR_DMA_3_Pos)               /**< (USBHS_DEVISR) DMA Channel 3 Interrupt Mask */
#define USBHS_DEVISR_DMA_3(value)             (USBHS_DEVISR_DMA_3_Msk & ((value) << USBHS_DEVISR_DMA_3_Pos))
#define USBHS_DEVISR_DMA_4_Pos                (28U)                                          /**< (USBHS_DEVISR) DMA Channel 4 Interrupt Position */
#define USBHS_DEVISR_DMA_4_Msk                (0x1U << USBHS_DEVISR_DMA_4_Pos)               /**< (USBHS_DEVISR) DMA Channel 4 Interrupt Mask */
#define USBHS_DEVISR_DMA_4(value)             (USBHS_DEVISR_DMA_4_Msk & ((value) << USBHS_DEVISR_DMA_4_Pos))
#define USBHS_DEVISR_DMA_5_Pos                (29U)                                          /**< (USBHS_DEVISR) DMA Channel 5 Interrupt Position */
#define USBHS_DEVISR_DMA_5_Msk                (0x1U << USBHS_DEVISR_DMA_5_Pos)               /**< (USBHS_DEVISR) DMA Channel 5 Interrupt Mask */
#define USBHS_DEVISR_DMA_5(value)             (USBHS_DEVISR_DMA_5_Msk & ((value) << USBHS_DEVISR_DMA_5_Pos))
#define USBHS_DEVISR_DMA_6_Pos                (30U)                                          /**< (USBHS_DEVISR) DMA Channel 6 Interrupt Position */
#define USBHS_DEVISR_DMA_6_Msk                (0x1U << USBHS_DEVISR_DMA_6_Pos)               /**< (USBHS_DEVISR) DMA Channel 6 Interrupt Mask */
#define USBHS_DEVISR_DMA_6(value)             (USBHS_DEVISR_DMA_6_Msk & ((value) << USBHS_DEVISR_DMA_6_Pos))
#define USBHS_DEVISR_DMA_7_Pos                (31U)                                          /**< (USBHS_DEVISR) DMA Channel 7 Interrupt Position */
#define USBHS_DEVISR_DMA_7_Msk                (0x1U << USBHS_DEVISR_DMA_7_Pos)               /**< (USBHS_DEVISR) DMA Channel 7 Interrupt Mask */
#define USBHS_DEVISR_DMA_7(value)             (USBHS_DEVISR_DMA_7_Msk & ((value) << USBHS_DEVISR_DMA_7_Pos))
#define USBHS_DEVISR_Msk                      (0xFE3FF07FUL)                                 /**< (USBHS_DEVISR) Register Mask  */

/* -------- USBHS_DEVICR : (USBHS Offset: 0x08) ( /W 32) Device Global Interrupt Clear Register -------- */
#define USBHS_DEVICR_SUSPC_Pos                (0U)                                           /**< (USBHS_DEVICR) Suspend Interrupt Clear Position */
#define USBHS_DEVICR_SUSPC_Msk                (0x1U << USBHS_DEVICR_SUSPC_Pos)               /**< (USBHS_DEVICR) Suspend Interrupt Clear Mask */
#define USBHS_DEVICR_SUSPC(value)             (USBHS_DEVICR_SUSPC_Msk & ((value) << USBHS_DEVICR_SUSPC_Pos))
#define USBHS_DEVICR_MSOFC_Pos                (1U)                                           /**< (USBHS_DEVICR) Micro Start of Frame Interrupt Clear Position */
#define USBHS_DEVICR_MSOFC_Msk                (0x1U << USBHS_DEVICR_MSOFC_Pos)               /**< (USBHS_DEVICR) Micro Start of Frame Interrupt Clear Mask */
#define USBHS_DEVICR_MSOFC(value)             (USBHS_DEVICR_MSOFC_Msk & ((value) << USBHS_DEVICR_MSOFC_Pos))
#define USBHS_DEVICR_SOFC_Pos                 (2U)                                           /**< (USBHS_DEVICR) Start of Frame Interrupt Clear Position */
#define USBHS_DEVICR_SOFC_Msk                 (0x1U << USBHS_DEVICR_SOFC_Pos)                /**< (USBHS_DEVICR) Start of Frame Interrupt Clear Mask */
#define USBHS_DEVICR_SOFC(value)              (USBHS_DEVICR_SOFC_Msk & ((value) << USBHS_DEVICR_SOFC_Pos))
#define USBHS_DEVICR_EORSTC_Pos               (3U)                                           /**< (USBHS_DEVICR) End of Reset Interrupt Clear Position */
#define USBHS_DEVICR_EORSTC_Msk               (0x1U << USBHS_DEVICR_EORSTC_Pos)              /**< (USBHS_DEVICR) End of Reset Interrupt Clear Mask */
#define USBHS_DEVICR_EORSTC(value)            (USBHS_DEVICR_EORSTC_Msk & ((value) << USBHS_DEVICR_EORSTC_Pos))
#define USBHS_DEVICR_WAKEUPC_Pos              (4U)                                           /**< (USBHS_DEVICR) Wake-Up Interrupt Clear Position */
#define USBHS_DEVICR_WAKEUPC_Msk              (0x1U << USBHS_DEVICR_WAKEUPC_Pos)             /**< (USBHS_DEVICR) Wake-Up Interrupt Clear Mask */
#define USBHS_DEVICR_WAKEUPC(value)           (USBHS_DEVICR_WAKEUPC_Msk & ((value) << USBHS_DEVICR_WAKEUPC_Pos))
#define USBHS_DEVICR_EORSMC_Pos               (5U)                                           /**< (USBHS_DEVICR) End of Resume Interrupt Clear Position */
#define USBHS_DEVICR_EORSMC_Msk               (0x1U << USBHS_DEVICR_EORSMC_Pos)              /**< (USBHS_DEVICR) End of Resume Interrupt Clear Mask */
#define USBHS_DEVICR_EORSMC(value)            (USBHS_DEVICR_EORSMC_Msk & ((value) << USBHS_DEVICR_EORSMC_Pos))
#define USBHS_DEVICR_UPRSMC_Pos               (6U)                                           /**< (USBHS_DEVICR) Upstream Resume Interrupt Clear Position */
#define USBHS_DEVICR_UPRSMC_Msk               (0x1U << USBHS_DEVICR_UPRSMC_Pos)              /**< (USBHS_DEVICR) Upstream Resume Interrupt Clear Mask */
#define USBHS_DEVICR_UPRSMC(value)            (USBHS_DEVICR_UPRSMC_Msk & ((value) << USBHS_DEVICR_UPRSMC_Pos))
#define USBHS_DEVICR_Msk                      (0x0000007FUL)                                 /**< (USBHS_DEVICR) Register Mask  */

/* -------- USBHS_DEVIFR : (USBHS Offset: 0x0C) ( /W 32) Device Global Interrupt Set Register -------- */
#define USBHS_DEVIFR_SUSPS_Pos                (0U)                                           /**< (USBHS_DEVIFR) Suspend Interrupt Set Position */
#define USBHS_DEVIFR_SUSPS_Msk                (0x1U << USBHS_DEVIFR_SUSPS_Pos)               /**< (USBHS_DEVIFR) Suspend Interrupt Set Mask */
#define USBHS_DEVIFR_SUSPS(value)             (USBHS_DEVIFR_SUSPS_Msk & ((value) << USBHS_DEVIFR_SUSPS_Pos))
#define USBHS_DEVIFR_MSOFS_Pos                (1U)                                           /**< (USBHS_DEVIFR) Micro Start of Frame Interrupt Set Position */
#define USBHS_DEVIFR_MSOFS_Msk                (0x1U << USBHS_DEVIFR_MSOFS_Pos)               /**< (USBHS_DEVIFR) Micro Start of Frame Interrupt Set Mask */
#define USBHS_DEVIFR_MSOFS(value)             (USBHS_DEVIFR_MSOFS_Msk & ((value) << USBHS_DEVIFR_MSOFS_Pos))
#define USBHS_DEVIFR_SOFS_Pos                 (2U)                                           /**< (USBHS_DEVIFR) Start of Frame Interrupt Set Position */
#define USBHS_DEVIFR_SOFS_Msk                 (0x1U << USBHS_DEVIFR_SOFS_Pos)                /**< (USBHS_DEVIFR) Start of Frame Interrupt Set Mask */
#define USBHS_DEVIFR_SOFS(value)              (USBHS_DEVIFR_SOFS_Msk & ((value) << USBHS_DEVIFR_SOFS_Pos))
#define USBHS_DEVIFR_EORSTS_Pos               (3U)                                           /**< (USBHS_DEVIFR) End of Reset Interrupt Set Position */
#define USBHS_DEVIFR_EORSTS_Msk               (0x1U << USBHS_DEVIFR_EORSTS_Pos)              /**< (USBHS_DEVIFR) End of Reset Interrupt Set Mask */
#define USBHS_DEVIFR_EORSTS(value)            (USBHS_DEVIFR_EORSTS_Msk & ((value) << USBHS_DEVIFR_EORSTS_Pos))
#define USBHS_DEVIFR_WAKEUPS_Pos              (4U)                                           /**< (USBHS_DEVIFR) Wake-Up Interrupt Set Position */
#define USBHS_DEVIFR_WAKEUPS_Msk              (0x1U << USBHS_DEVIFR_WAKEUPS_Pos)             /**< (USBHS_DEVIFR) Wake-Up Interrupt Set Mask */
#define USBHS_DEVIFR_WAKEUPS(value)           (USBHS_DEVIFR_WAKEUPS_Msk & ((value) << USBHS_DEVIFR_WAKEUPS_Pos))
#define USBHS_DEVIFR_EORSMS_Pos               (5U)                                           /**< (USBHS_DEVIFR) End of Resume Interrupt Set Position */
#define USBHS_DEVIFR_EORSMS_Msk               (0x1U << USBHS_DEVIFR_EORSMS_Pos)              /**< (USBHS_DEVIFR) End of Resume Interrupt Set Mask */
#define USBHS_DEVIFR_EORSMS(value)            (USBHS_DEVIFR_EORSMS_Msk & ((value) << USBHS_DEVIFR_EORSMS_Pos))
#define USBHS_DEVIFR_UPRSMS_Pos               (6U)                                           /**< (USBHS_DEVIFR) Upstream Resume Interrupt Set Position */
#define USBHS_DEVIFR_UPRSMS_Msk               (0x1U << USBHS_DEVIFR_UPRSMS_Pos)              /**< (USBHS_DEVIFR) Upstream Resume Interrupt Set Mask */
#define USBHS_DEVIFR_UPRSMS(value)            (USBHS_DEVIFR_UPRSMS_Msk & ((value) << USBHS_DEVIFR_UPRSMS_Pos))
#define USBHS_DEVIFR_DMA_1_Pos                (25U)                                          /**< (USBHS_DEVIFR) DMA Channel 1 Interrupt Set Position */
#define USBHS_DEVIFR_DMA_1_Msk                (0x1U << USBHS_DEVIFR_DMA_1_Pos)               /**< (USBHS_DEVIFR) DMA Channel 1 Interrupt Set Mask */
#define USBHS_DEVIFR_DMA_1(value)             (USBHS_DEVIFR_DMA_1_Msk & ((value) << USBHS_DEVIFR_DMA_1_Pos))
#define USBHS_DEVIFR_DMA_2_Pos                (26U)                                          /**< (USBHS_DEVIFR) DMA Channel 2 Interrupt Set Position */
#define USBHS_DEVIFR_DMA_2_Msk                (0x1U << USBHS_DEVIFR_DMA_2_Pos)               /**< (USBHS_DEVIFR) DMA Channel 2 Interrupt Set Mask */
#define USBHS_DEVIFR_DMA_2(value)             (USBHS_DEVIFR_DMA_2_Msk & ((value) << USBHS_DEVIFR_DMA_2_Pos))
#define USBHS_DEVIFR_DMA_3_Pos                (27U)                                          /**< (USBHS_DEVIFR) DMA Channel 3 Interrupt Set Position */
#define USBHS_DEVIFR_DMA_3_Msk                (0x1U << USBHS_DEVIFR_DMA_3_Pos)               /**< (USBHS_DEVIFR) DMA Channel 3 Interrupt Set Mask */
#define USBHS_DEVIFR_DMA_3(value)             (USBHS_DEVIFR_DMA_3_Msk & ((value) << USBHS_DEVIFR_DMA_3_Pos))
#define USBHS_DEVIFR_DMA_4_Pos                (28U)                                          /**< (USBHS_DEVIFR) DMA Channel 4 Interrupt Set Position */
#define USBHS_DEVIFR_DMA_4_Msk                (0x1U << USBHS_DEVIFR_DMA_4_Pos)               /**< (USBHS_DEVIFR) DMA Channel 4 Interrupt Set Mask */
#define USBHS_DEVIFR_DMA_4(value)             (USBHS_DEVIFR_DMA_4_Msk & ((value) << USBHS_DEVIFR_DMA_4_Pos))
#define USBHS_DEVIFR_DMA_5_Pos                (29U)                                          /**< (USBHS_DEVIFR) DMA Channel 5 Interrupt Set Position */
#define USBHS_DEVIFR_DMA_5_Msk                (0x1U << USBHS_DEVIFR_DMA_5_Pos)               /**< (USBHS_DEVIFR) DMA Channel 5 Interrupt Set Mask */
#define USBHS_DEVIFR_DMA_5(value)             (USBHS_DEVIFR_DMA_5_Msk & ((value) << USBHS_DEVIFR_DMA_5_Pos))
#define USBHS_DEVIFR_DMA_6_Pos                (30U)                                          /**< (USBHS_DEVIFR) DMA Channel 6 Interrupt Set Position */
#define USBHS_DEVIFR_DMA_6_Msk                (0x1U << USBHS_DEVIFR_DMA_6_Pos)               /**< (USBHS_DEVIFR) DMA Channel 6 Interrupt Set Mask */
#define USBHS_DEVIFR_DMA_6(value)             (USBHS_DEVIFR_DMA_6_Msk & ((value) << USBHS_DEVIFR_DMA_6_Pos))
#define USBHS_DEVIFR_DMA_7_Pos                (31U)                                          /**< (USBHS_DEVIFR) DMA Channel 7 Interrupt Set Position */
#define USBHS_DEVIFR_DMA_7_Msk                (0x1U << USBHS_DEVIFR_DMA_7_Pos)               /**< (USBHS_DEVIFR) DMA Channel 7 Interrupt Set Mask */
#define USBHS_DEVIFR_DMA_7(value)             (USBHS_DEVIFR_DMA_7_Msk & ((value) << USBHS_DEVIFR_DMA_7_Pos))
#define USBHS_DEVIFR_Msk                      (0xFE00007FUL)                                 /**< (USBHS_DEVIFR) Register Mask  */

/* -------- USBHS_DEVIMR : (USBHS Offset: 0x10) (R/  32) Device Global Interrupt Mask Register -------- */
#define USBHS_DEVIMR_SUSPE_Pos                (0U)                                           /**< (USBHS_DEVIMR) Suspend Interrupt Mask Position */
#define USBHS_DEVIMR_SUSPE_Msk                (0x1U << USBHS_DEVIMR_SUSPE_Pos)               /**< (USBHS_DEVIMR) Suspend Interrupt Mask Mask */
#define USBHS_DEVIMR_SUSPE(value)             (USBHS_DEVIMR_SUSPE_Msk & ((value) << USBHS_DEVIMR_SUSPE_Pos))
#define USBHS_DEVIMR_MSOFE_Pos                (1U)                                           /**< (USBHS_DEVIMR) Micro Start of Frame Interrupt Mask Position */
#define USBHS_DEVIMR_MSOFE_Msk                (0x1U << USBHS_DEVIMR_MSOFE_Pos)               /**< (USBHS_DEVIMR) Micro Start of Frame Interrupt Mask Mask */
#define USBHS_DEVIMR_MSOFE(value)             (USBHS_DEVIMR_MSOFE_Msk & ((value) << USBHS_DEVIMR_MSOFE_Pos))
#define USBHS_DEVIMR_SOFE_Pos                 (2U)                                           /**< (USBHS_DEVIMR) Start of Frame Interrupt Mask Position */
#define USBHS_DEVIMR_SOFE_Msk                 (0x1U << USBHS_DEVIMR_SOFE_Pos)                /**< (USBHS_DEVIMR) Start of Frame Interrupt Mask Mask */
#define USBHS_DEVIMR_SOFE(value)              (USBHS_DEVIMR_SOFE_Msk & ((value) << USBHS_DEVIMR_SOFE_Pos))
#define USBHS_DEVIMR_EORSTE_Pos               (3U)                                           /**< (USBHS_DEVIMR) End of Reset Interrupt Mask Position */
#define USBHS_DEVIMR_EORSTE_Msk               (0x1U << USBHS_DEVIMR_EORSTE_Pos)              /**< (USBHS_DEVIMR) End of Reset Interrupt Mask Mask */
#define USBHS_DEVIMR_EORSTE(value)            (USBHS_DEVIMR_EORSTE_Msk & ((value) << USBHS_DEVIMR_EORSTE_Pos))
#define USBHS_DEVIMR_WAKEUPE_Pos              (4U)                                           /**< (USBHS_DEVIMR) Wake-Up Interrupt Mask Position */
#define USBHS_DEVIMR_WAKEUPE_Msk              (0x1U << USBHS_DEVIMR_WAKEUPE_Pos)             /**< (USBHS_DEVIMR) Wake-Up Interrupt Mask Mask */
#define USBHS_DEVIMR_WAKEUPE(value)           (USBHS_DEVIMR_WAKEUPE_Msk & ((value) << USBHS_DEVIMR_WAKEUPE_Pos))
#define USBHS_DEVIMR_EORSME_Pos               (5U)                                           /**< (USBHS_DEVIMR) End of Resume Interrupt Mask Position */
#define USBHS_DEVIMR_EORSME_Msk               (0x1U << USBHS_DEVIMR_EORSME_Pos)              /**< (USBHS_DEVIMR) End of Resume Interrupt Mask Mask */
#define USBHS_DEVIMR_EORSME(value)            (USBHS_DEVIMR_EORSME_Msk & ((value) << USBHS_DEVIMR_EORSME_Pos))
#define USBHS_DEVIMR_UPRSME_Pos               (6U)                                           /**< (USBHS_DEVIMR) Upstream Resume Interrupt Mask Position */
#define USBHS_DEVIMR_UPRSME_Msk               (0x1U << USBHS_DEVIMR_UPRSME_Pos)              /**< (USBHS_DEVIMR) Upstream Resume Interrupt Mask Mask */
#define USBHS_DEVIMR_UPRSME(value)            (USBHS_DEVIMR_UPRSME_Msk & ((value) << USBHS_DEVIMR_UPRSME_Pos))
#define USBHS_DEVIMR_PEP_0_Pos                (12U)                                          /**< (USBHS_DEVIMR) Endpoint 0 Interrupt Mask Position */
#define USBHS_DEVIMR_PEP_0_Msk                (0x1U << USBHS_DEVIMR_PEP_0_Pos)               /**< (USBHS_DEVIMR) Endpoint 0 Interrupt Mask Mask */
#define USBHS_DEVIMR_PEP_0(value)             (USBHS_DEVIMR_PEP_0_Msk & ((value) << USBHS_DEVIMR_PEP_0_Pos))
#define USBHS_DEVIMR_PEP_1_Pos                (13U)                                          /**< (USBHS_DEVIMR) Endpoint 1 Interrupt Mask Position */
#define USBHS_DEVIMR_PEP_1_Msk                (0x1U << USBHS_DEVIMR_PEP_1_Pos)               /**< (USBHS_DEVIMR) Endpoint 1 Interrupt Mask Mask */
#define USBHS_DEVIMR_PEP_1(value)             (USBHS_DEVIMR_PEP_1_Msk & ((value) << USBHS_DEVIMR_PEP_1_Pos))
#define USBHS_DEVIMR_PEP_2_Pos                (14U)                                          /**< (USBHS_DEVIMR) Endpoint 2 Interrupt Mask Position */
#define USBHS_DEVIMR_PEP_2_Msk                (0x1U << USBHS_DEVIMR_PEP_2_Pos)               /**< (USBHS_DEVIMR) Endpoint 2 Interrupt Mask Mask */
#define USBHS_DEVIMR_PEP_2(value)             (USBHS_DEVIMR_PEP_2_Msk & ((value) << USBHS_DEVIMR_PEP_2_Pos))
#define USBHS_DEVIMR_PEP_3_Pos                (15U)                                          /**< (USBHS_DEVIMR) Endpoint 3 Interrupt Mask Position */
#define USBHS_DEVIMR_PEP_3_Msk                (0x1U << USBHS_DEVIMR_PEP_3_Pos)               /**< (USBHS_DEVIMR) Endpoint 3 Interrupt Mask Mask */
#define USBHS_DEVIMR_PEP_3(value)             (USBHS_DEVIMR_PEP_3_Msk & ((value) << USBHS_DEVIMR_PEP_3_Pos))
#define USBHS_DEVIMR_PEP_4_Pos                (16U)                                          /**< (USBHS_DEVIMR) Endpoint 4 Interrupt Mask Position */
#define USBHS_DEVIMR_PEP_4_Msk                (0x1U << USBHS_DEVIMR_PEP_4_Pos)               /**< (USBHS_DEVIMR) Endpoint 4 Interrupt Mask Mask */
#define USBHS_DEVIMR_PEP_4(value)             (USBHS_DEVIMR_PEP_4_Msk & ((value) << USBHS_DEVIMR_PEP_4_Pos))
#define USBHS_DEVIMR_PEP_5_Pos                (17U)                                          /**< (USBHS_DEVIMR) Endpoint 5 Interrupt Mask Position */
#define USBHS_DEVIMR_PEP_5_Msk                (0x1U << USBHS_DEVIMR_PEP_5_Pos)               /**< (USBHS_DEVIMR) Endpoint 5 Interrupt Mask Mask */
#define USBHS_DEVIMR_PEP_5(value)             (USBHS_DEVIMR_PEP_5_Msk & ((value) << USBHS_DEVIMR_PEP_5_Pos))
#define USBHS_DEVIMR_PEP_6_Pos                (18U)                                          /**< (USBHS_DEVIMR) Endpoint 6 Interrupt Mask Position */
#define USBHS_DEVIMR_PEP_6_Msk                (0x1U << USBHS_DEVIMR_PEP_6_Pos)               /**< (USBHS_DEVIMR) Endpoint 6 Interrupt Mask Mask */
#define USBHS_DEVIMR_PEP_6(value)             (USBHS_DEVIMR_PEP_6_Msk & ((value) << USBHS_DEVIMR_PEP_6_Pos))
#define USBHS_DEVIMR_PEP_7_Pos                (19U)                                          /**< (USBHS_DEVIMR) Endpoint 7 Interrupt Mask Position */
#define USBHS_DEVIMR_PEP_7_Msk                (0x1U << USBHS_DEVIMR_PEP_7_Pos)               /**< (USBHS_DEVIMR) Endpoint 7 Interrupt Mask Mask */
#define USBHS_DEVIMR_PEP_7(value)             (USBHS_DEVIMR_PEP_7_Msk & ((value) << USBHS_DEVIMR_PEP_7_Pos))
#define USBHS_DEVIMR_PEP_8_Pos                (20U)                                          /**< (USBHS_DEVIMR) Endpoint 8 Interrupt Mask Position */
#define USBHS_DEVIMR_PEP_8_Msk                (0x1U << USBHS_DEVIMR_PEP_8_Pos)               /**< (USBHS_DEVIMR) Endpoint 8 Interrupt Mask Mask */
#define USBHS_DEVIMR_PEP_8(value)             (USBHS_DEVIMR_PEP_8_Msk & ((value) << USBHS_DEVIMR_PEP_8_Pos))
#define USBHS_DEVIMR_PEP_9_Pos                (21U)                                          /**< (USBHS_DEVIMR) Endpoint 9 Interrupt Mask Position */
#define USBHS_DEVIMR_PEP_9_Msk                (0x1U << USBHS_DEVIMR_PEP_9_Pos)               /**< (USBHS_DEVIMR) Endpoint 9 Interrupt Mask Mask */
#define USBHS_DEVIMR_PEP_9(value)             (USBHS_DEVIMR_PEP_9_Msk & ((value) << USBHS_DEVIMR_PEP_9_Pos))
#define USBHS_DEVIMR_DMA_1_Pos                (25U)                                          /**< (USBHS_DEVIMR) DMA Channel 1 Interrupt Mask Position */
#define USBHS_DEVIMR_DMA_1_Msk                (0x1U << USBHS_DEVIMR_DMA_1_Pos)               /**< (USBHS_DEVIMR) DMA Channel 1 Interrupt Mask Mask */
#define USBHS_DEVIMR_DMA_1(value)             (USBHS_DEVIMR_DMA_1_Msk & ((value) << USBHS_DEVIMR_DMA_1_Pos))
#define USBHS_DEVIMR_DMA_2_Pos                (26U)                                          /**< (USBHS_DEVIMR) DMA Channel 2 Interrupt Mask Position */
#define USBHS_DEVIMR_DMA_2_Msk                (0x1U << USBHS_DEVIMR_DMA_2_Pos)               /**< (USBHS_DEVIMR) DMA Channel 2 Interrupt Mask Mask */
#define USBHS_DEVIMR_DMA_2(value)             (USBHS_DEVIMR_DMA_2_Msk & ((value) << USBHS_DEVIMR_DMA_2_Pos))
#define USBHS_DEVIMR_DMA_3_Pos                (27U)                                          /**< (USBHS_DEVIMR) DMA Channel 3 Interrupt Mask Position */
#define USBHS_DEVIMR_DMA_3_Msk                (0x1U << USBHS_DEVIMR_DMA_3_Pos)               /**< (USBHS_DEVIMR) DMA Channel 3 Interrupt Mask Mask */
#define USBHS_DEVIMR_DMA_3(value)             (USBHS_DEVIMR_DMA_3_Msk & ((value) << USBHS_DEVIMR_DMA_3_Pos))
#define USBHS_DEVIMR_DMA_4_Pos                (28U)                                          /**< (USBHS_DEVIMR) DMA Channel 4 Interrupt Mask Position */
#define USBHS_DEVIMR_DMA_4_Msk                (0x1U << USBHS_DEVIMR_DMA_4_Pos)               /**< (USBHS_DEVIMR) DMA Channel 4 Interrupt Mask Mask */
#define USBHS_DEVIMR_DMA_4(value)             (USBHS_DEVIMR_DMA_4_Msk & ((value) << USBHS_DEVIMR_DMA_4_Pos))
#define USBHS_DEVIMR_DMA_5_Pos                (29U)                                          /**< (USBHS_DEVIMR) DMA Channel 5 Interrupt Mask Position */
#define USBHS_DEVIMR_DMA_5_Msk                (0x1U << USBHS_DEVIMR_DMA_5_Pos)               /**< (USBHS_DEVIMR) DMA Channel 5 Interrupt Mask Mask */
#define USBHS_DEVIMR_DMA_5(value)             (USBHS_DEVIMR_DMA_5_Msk & ((value) << USBHS_DEVIMR_DMA_5_Pos))
#define USBHS_DEVIMR_DMA_6_Pos                (30U)                                          /**< (USBHS_DEVIMR) DMA Channel 6 Interrupt Mask Position */
#define USBHS_DEVIMR_DMA_6_Msk                (0x1U << USBHS_DEVIMR_DMA_6_Pos)               /**< (USBHS_DEVIMR) DMA Channel 6 Interrupt Mask Mask */
#define USBHS_DEVIMR_DMA_6(value)             (USBHS_DEVIMR_DMA_6_Msk & ((value) << USBHS_DEVIMR_DMA_6_Pos))
#define USBHS_DEVIMR_DMA_7_Pos                (31U)                                          /**< (USBHS_DEVIMR) DMA Channel 7 Interrupt Mask Position */
#define USBHS_DEVIMR_DMA_7_Msk                (0x1U << USBHS_DEVIMR_DMA_7_Pos)               /**< (USBHS_DEVIMR) DMA Channel 7 Interrupt Mask Mask */
#define USBHS_DEVIMR_DMA_7(value)             (USBHS_DEVIMR_DMA_7_Msk & ((value) << USBHS_DEVIMR_DMA_7_Pos))
#define USBHS_DEVIMR_Msk                      (0xFE3FF07FUL)                                 /**< (USBHS_DEVIMR) Register Mask  */

/* -------- USBHS_DEVIDR : (USBHS Offset: 0x14) ( /W 32) Device Global Interrupt Disable Register -------- */
#define USBHS_DEVIDR_SUSPEC_Pos               (0U)                                           /**< (USBHS_DEVIDR) Suspend Interrupt Disable Position */
#define USBHS_DEVIDR_SUSPEC_Msk               (0x1U << USBHS_DEVIDR_SUSPEC_Pos)              /**< (USBHS_DEVIDR) Suspend Interrupt Disable Mask */
#define USBHS_DEVIDR_SUSPEC(value)            (USBHS_DEVIDR_SUSPEC_Msk & ((value) << USBHS_DEVIDR_SUSPEC_Pos))
#define USBHS_DEVIDR_MSOFEC_Pos               (1U)                                           /**< (USBHS_DEVIDR) Micro Start of Frame Interrupt Disable Position */
#define USBHS_DEVIDR_MSOFEC_Msk               (0x1U << USBHS_DEVIDR_MSOFEC_Pos)              /**< (USBHS_DEVIDR) Micro Start of Frame Interrupt Disable Mask */
#define USBHS_DEVIDR_MSOFEC(value)            (USBHS_DEVIDR_MSOFEC_Msk & ((value) << USBHS_DEVIDR_MSOFEC_Pos))
#define USBHS_DEVIDR_SOFEC_Pos                (2U)                                           /**< (USBHS_DEVIDR) Start of Frame Interrupt Disable Position */
#define USBHS_DEVIDR_SOFEC_Msk                (0x1U << USBHS_DEVIDR_SOFEC_Pos)               /**< (USBHS_DEVIDR) Start of Frame Interrupt Disable Mask */
#define USBHS_DEVIDR_SOFEC(value)             (USBHS_DEVIDR_SOFEC_Msk & ((value) << USBHS_DEVIDR_SOFEC_Pos))
#define USBHS_DEVIDR_EORSTEC_Pos              (3U)                                           /**< (USBHS_DEVIDR) End of Reset Interrupt Disable Position */
#define USBHS_DEVIDR_EORSTEC_Msk              (0x1U << USBHS_DEVIDR_EORSTEC_Pos)             /**< (USBHS_DEVIDR) End of Reset Interrupt Disable Mask */
#define USBHS_DEVIDR_EORSTEC(value)           (USBHS_DEVIDR_EORSTEC_Msk & ((value) << USBHS_DEVIDR_EORSTEC_Pos))
#define USBHS_DEVIDR_WAKEUPEC_Pos             (4U)                                           /**< (USBHS_DEVIDR) Wake-Up Interrupt Disable Position */
#define USBHS_DEVIDR_WAKEUPEC_Msk             (0x1U << USBHS_DEVIDR_WAKEUPEC_Pos)            /**< (USBHS_DEVIDR) Wake-Up Interrupt Disable Mask */
#define USBHS_DEVIDR_WAKEUPEC(value)          (USBHS_DEVIDR_WAKEUPEC_Msk & ((value) << USBHS_DEVIDR_WAKEUPEC_Pos))
#define USBHS_DEVIDR_EORSMEC_Pos              (5U)                                           /**< (USBHS_DEVIDR) End of Resume Interrupt Disable Position */
#define USBHS_DEVIDR_EORSMEC_Msk              (0x1U << USBHS_DEVIDR_EORSMEC_Pos)             /**< (USBHS_DEVIDR) End of Resume Interrupt Disable Mask */
#define USBHS_DEVIDR_EORSMEC(value)           (USBHS_DEVIDR_EORSMEC_Msk & ((value) << USBHS_DEVIDR_EORSMEC_Pos))
#define USBHS_DEVIDR_UPRSMEC_Pos              (6U)                                           /**< (USBHS_DEVIDR) Upstream Resume Interrupt Disable Position */
#define USBHS_DEVIDR_UPRSMEC_Msk              (0x1U << USBHS_DEVIDR_UPRSMEC_Pos)             /**< (USBHS_DEVIDR) Upstream Resume Interrupt Disable Mask */
#define USBHS_DEVIDR_UPRSMEC(value)           (USBHS_DEVIDR_UPRSMEC_Msk & ((value) << USBHS_DEVIDR_UPRSMEC_Pos))
#define USBHS_DEVIDR_PEP_0_Pos                (12U)                                          /**< (USBHS_DEVIDR) Endpoint 0 Interrupt Disable Position */
#define USBHS_DEVIDR_PEP_0_Msk                (0x1U << USBHS_DEVIDR_PEP_0_Pos)               /**< (USBHS_DEVIDR) Endpoint 0 Interrupt Disable Mask */
#define USBHS_DEVIDR_PEP_0(value)             (USBHS_DEVIDR_PEP_0_Msk & ((value) << USBHS_DEVIDR_PEP_0_Pos))
#define USBHS_DEVIDR_PEP_1_Pos                (13U)                                          /**< (USBHS_DEVIDR) Endpoint 1 Interrupt Disable Position */
#define USBHS_DEVIDR_PEP_1_Msk                (0x1U << USBHS_DEVIDR_PEP_1_Pos)               /**< (USBHS_DEVIDR) Endpoint 1 Interrupt Disable Mask */
#define USBHS_DEVIDR_PEP_1(value)             (USBHS_DEVIDR_PEP_1_Msk & ((value) << USBHS_DEVIDR_PEP_1_Pos))
#define USBHS_DEVIDR_PEP_2_Pos                (14U)                                          /**< (USBHS_DEVIDR) Endpoint 2 Interrupt Disable Position */
#define USBHS_DEVIDR_PEP_2_Msk                (0x1U << USBHS_DEVIDR_PEP_2_Pos)               /**< (USBHS_DEVIDR) Endpoint 2 Interrupt Disable Mask */
#define USBHS_DEVIDR_PEP_2(value)             (USBHS_DEVIDR_PEP_2_Msk & ((value) << USBHS_DEVIDR_PEP_2_Pos))
#define USBHS_DEVIDR_PEP_3_Pos                (15U)                                          /**< (USBHS_DEVIDR) Endpoint 3 Interrupt Disable Position */
#define USBHS_DEVIDR_PEP_3_Msk                (0x1U << USBHS_DEVIDR_PEP_3_Pos)               /**< (USBHS_DEVIDR) Endpoint 3 Interrupt Disable Mask */
#define USBHS_DEVIDR_PEP_3(value)             (USBHS_DEVIDR_PEP_3_Msk & ((value) << USBHS_DEVIDR_PEP_3_Pos))
#define USBHS_DEVIDR_PEP_4_Pos                (16U)                                          /**< (USBHS_DEVIDR) Endpoint 4 Interrupt Disable Position */
#define USBHS_DEVIDR_PEP_4_Msk                (0x1U << USBHS_DEVIDR_PEP_4_Pos)               /**< (USBHS_DEVIDR) Endpoint 4 Interrupt Disable Mask */
#define USBHS_DEVIDR_PEP_4(value)             (USBHS_DEVIDR_PEP_4_Msk & ((value) << USBHS_DEVIDR_PEP_4_Pos))
#define USBHS_DEVIDR_PEP_5_Pos                (17U)                                          /**< (USBHS_DEVIDR) Endpoint 5 Interrupt Disable Position */
#define USBHS_DEVIDR_PEP_5_Msk                (0x1U << USBHS_DEVIDR_PEP_5_Pos)               /**< (USBHS_DEVIDR) Endpoint 5 Interrupt Disable Mask */
#define USBHS_DEVIDR_PEP_5(value)             (USBHS_DEVIDR_PEP_5_Msk & ((value) << USBHS_DEVIDR_PEP_5_Pos))
#define USBHS_DEVIDR_PEP_6_Pos                (18U)                                          /**< (USBHS_DEVIDR) Endpoint 6 Interrupt Disable Position */
#define USBHS_DEVIDR_PEP_6_Msk                (0x1U << USBHS_DEVIDR_PEP_6_Pos)               /**< (USBHS_DEVIDR) Endpoint 6 Interrupt Disable Mask */
#define USBHS_DEVIDR_PEP_6(value)             (USBHS_DEVIDR_PEP_6_Msk & ((value) << USBHS_DEVIDR_PEP_6_Pos))
#define USBHS_DEVIDR_PEP_7_Pos                (19U)                                          /**< (USBHS_DEVIDR) Endpoint 7 Interrupt Disable Position */
#define USBHS_DEVIDR_PEP_7_Msk                (0x1U << USBHS_DEVIDR_PEP_7_Pos)               /**< (USBHS_DEVIDR) Endpoint 7 Interrupt Disable Mask */
#define USBHS_DEVIDR_PEP_7(value)             (USBHS_DEVIDR_PEP_7_Msk & ((value) << USBHS_DEVIDR_PEP_7_Pos))
#define USBHS_DEVIDR_PEP_8_Pos                (20U)                                          /**< (USBHS_DEVIDR) Endpoint 8 Interrupt Disable Position */
#define USBHS_DEVIDR_PEP_8_Msk                (0x1U << USBHS_DEVIDR_PEP_8_Pos)               /**< (USBHS_DEVIDR) Endpoint 8 Interrupt Disable Mask */
#define USBHS_DEVIDR_PEP_8(value)             (USBHS_DEVIDR_PEP_8_Msk & ((value) << USBHS_DEVIDR_PEP_8_Pos))
#define USBHS_DEVIDR_PEP_9_Pos                (21U)                                          /**< (USBHS_DEVIDR) Endpoint 9 Interrupt Disable Position */
#define USBHS_DEVIDR_PEP_9_Msk                (0x1U << USBHS_DEVIDR_PEP_9_Pos)               /**< (USBHS_DEVIDR) Endpoint 9 Interrupt Disable Mask */
#define USBHS_DEVIDR_PEP_9(value)             (USBHS_DEVIDR_PEP_9_Msk & ((value) << USBHS_DEVIDR_PEP_9_Pos))
#define USBHS_DEVIDR_DMA_1_Pos                (25U)                                          /**< (USBHS_DEVIDR) DMA Channel 1 Interrupt Disable Position */
#define USBHS_DEVIDR_DMA_1_Msk                (0x1U << USBHS_DEVIDR_DMA_1_Pos)               /**< (USBHS_DEVIDR) DMA Channel 1 Interrupt Disable Mask */
#define USBHS_DEVIDR_DMA_1(value)             (USBHS_DEVIDR_DMA_1_Msk & ((value) << USBHS_DEVIDR_DMA_1_Pos))
#define USBHS_DEVIDR_DMA_2_Pos                (26U)                                          /**< (USBHS_DEVIDR) DMA Channel 2 Interrupt Disable Position */
#define USBHS_DEVIDR_DMA_2_Msk                (0x1U << USBHS_DEVIDR_DMA_2_Pos)               /**< (USBHS_DEVIDR) DMA Channel 2 Interrupt Disable Mask */
#define USBHS_DEVIDR_DMA_2(value)             (USBHS_DEVIDR_DMA_2_Msk & ((value) << USBHS_DEVIDR_DMA_2_Pos))
#define USBHS_DEVIDR_DMA_3_Pos                (27U)                                          /**< (USBHS_DEVIDR) DMA Channel 3 Interrupt Disable Position */
#define USBHS_DEVIDR_DMA_3_Msk                (0x1U << USBHS_DEVIDR_DMA_3_Pos)               /**< (USBHS_DEVIDR) DMA Channel 3 Interrupt Disable Mask */
#define USBHS_DEVIDR_DMA_3(value)             (USBHS_DEVIDR_DMA_3_Msk & ((value) << USBHS_DEVIDR_DMA_3_Pos))
#define USBHS_DEVIDR_DMA_4_Pos                (28U)                                          /**< (USBHS_DEVIDR) DMA Channel 4 Interrupt Disable Position */
#define USBHS_DEVIDR_DMA_4_Msk                (0x1U << USBHS_DEVIDR_DMA_4_Pos)               /**< (USBHS_DEVIDR) DMA Channel 4 Interrupt Disable Mask */
#define USBHS_DEVIDR_DMA_4(value)             (USBHS_DEVIDR_DMA_4_Msk & ((value) << USBHS_DEVIDR_DMA_4_Pos))
#define USBHS_DEVIDR_DMA_5_Pos                (29U)                                          /**< (USBHS_DEVIDR) DMA Channel 5 Interrupt Disable Position */
#define USBHS_DEVIDR_DMA_5_Msk                (0x1U << USBHS_DEVIDR_DMA_5_Pos)               /**< (USBHS_DEVIDR) DMA Channel 5 Interrupt Disable Mask */
#define USBHS_DEVIDR_DMA_5(value)             (USBHS_DEVIDR_DMA_5_Msk & ((value) << USBHS_DEVIDR_DMA_5_Pos))
#define USBHS_DEVIDR_DMA_6_Pos                (30U)                                          /**< (USBHS_DEVIDR) DMA Channel 6 Interrupt Disable Position */
#define USBHS_DEVIDR_DMA_6_Msk                (0x1U << USBHS_DEVIDR_DMA_6_Pos)               /**< (USBHS_DEVIDR) DMA Channel 6 Interrupt Disable Mask */
#define USBHS_DEVIDR_DMA_6(value)             (USBHS_DEVIDR_DMA_6_Msk & ((value) << USBHS_DEVIDR_DMA_6_Pos))
#define USBHS_DEVIDR_DMA_7_Pos                (31U)                                          /**< (USBHS_DEVIDR) DMA Channel 7 Interrupt Disable Position */
#define USBHS_DEVIDR_DMA_7_Msk                (0x1U << USBHS_DEVIDR_DMA_7_Pos)               /**< (USBHS_DEVIDR) DMA Channel 7 Interrupt Disable Mask */
#define USBHS_DEVIDR_DMA_7(value)             (USBHS_DEVIDR_DMA_7_Msk & ((value) << USBHS_DEVIDR_DMA_7_Pos))
#define USBHS_DEVIDR_Msk                      (0xFE3FF07FUL)                                 /**< (USBHS_DEVIDR) Register Mask  */

/* -------- USBHS_DEVIER : (USBHS Offset: 0x18) ( /W 32) Device Global Interrupt Enable Register -------- */
#define USBHS_DEVIER_SUSPES_Pos               (0U)                                           /**< (USBHS_DEVIER) Suspend Interrupt Enable Position */
#define USBHS_DEVIER_SUSPES_Msk               (0x1U << USBHS_DEVIER_SUSPES_Pos)              /**< (USBHS_DEVIER) Suspend Interrupt Enable Mask */
#define USBHS_DEVIER_SUSPES(value)            (USBHS_DEVIER_SUSPES_Msk & ((value) << USBHS_DEVIER_SUSPES_Pos))
#define USBHS_DEVIER_MSOFES_Pos               (1U)                                           /**< (USBHS_DEVIER) Micro Start of Frame Interrupt Enable Position */
#define USBHS_DEVIER_MSOFES_Msk               (0x1U << USBHS_DEVIER_MSOFES_Pos)              /**< (USBHS_DEVIER) Micro Start of Frame Interrupt Enable Mask */
#define USBHS_DEVIER_MSOFES(value)            (USBHS_DEVIER_MSOFES_Msk & ((value) << USBHS_DEVIER_MSOFES_Pos))
#define USBHS_DEVIER_SOFES_Pos                (2U)                                           /**< (USBHS_DEVIER) Start of Frame Interrupt Enable Position */
#define USBHS_DEVIER_SOFES_Msk                (0x1U << USBHS_DEVIER_SOFES_Pos)               /**< (USBHS_DEVIER) Start of Frame Interrupt Enable Mask */
#define USBHS_DEVIER_SOFES(value)             (USBHS_DEVIER_SOFES_Msk & ((value) << USBHS_DEVIER_SOFES_Pos))
#define USBHS_DEVIER_EORSTES_Pos              (3U)                                           /**< (USBHS_DEVIER) End of Reset Interrupt Enable Position */
#define USBHS_DEVIER_EORSTES_Msk              (0x1U << USBHS_DEVIER_EORSTES_Pos)             /**< (USBHS_DEVIER) End of Reset Interrupt Enable Mask */
#define USBHS_DEVIER_EORSTES(value)           (USBHS_DEVIER_EORSTES_Msk & ((value) << USBHS_DEVIER_EORSTES_Pos))
#define USBHS_DEVIER_WAKEUPES_Pos             (4U)                                           /**< (USBHS_DEVIER) Wake-Up Interrupt Enable Position */
#define USBHS_DEVIER_WAKEUPES_Msk             (0x1U << USBHS_DEVIER_WAKEUPES_Pos)            /**< (USBHS_DEVIER) Wake-Up Interrupt Enable Mask */
#define USBHS_DEVIER_WAKEUPES(value)          (USBHS_DEVIER_WAKEUPES_Msk & ((value) << USBHS_DEVIER_WAKEUPES_Pos))
#define USBHS_DEVIER_EORSMES_Pos              (5U)                                           /**< (USBHS_DEVIER) End of Resume Interrupt Enable Position */
#define USBHS_DEVIER_EORSMES_Msk              (0x1U << USBHS_DEVIER_EORSMES_Pos)             /**< (USBHS_DEVIER) End of Resume Interrupt Enable Mask */
#define USBHS_DEVIER_EORSMES(value)           (USBHS_DEVIER_EORSMES_Msk & ((value) << USBHS_DEVIER_EORSMES_Pos))
#define USBHS_DEVIER_UPRSMES_Pos              (6U)                                           /**< (USBHS_DEVIER) Upstream Resume Interrupt Enable Position */
#define USBHS_DEVIER_UPRSMES_Msk              (0x1U << USBHS_DEVIER_UPRSMES_Pos)             /**< (USBHS_DEVIER) Upstream Resume Interrupt Enable Mask */
#define USBHS_DEVIER_UPRSMES(value)           (USBHS_DEVIER_UPRSMES_Msk & ((value) << USBHS_DEVIER_UPRSMES_Pos))
#define USBHS_DEVIER_PEP_0_Pos                (12U)                                          /**< (USBHS_DEVIER) Endpoint 0 Interrupt Enable Position */
#define USBHS_DEVIER_PEP_0_Msk                (0x1U << USBHS_DEVIER_PEP_0_Pos)               /**< (USBHS_DEVIER) Endpoint 0 Interrupt Enable Mask */
#define USBHS_DEVIER_PEP_0(value)             (USBHS_DEVIER_PEP_0_Msk & ((value) << USBHS_DEVIER_PEP_0_Pos))
#define USBHS_DEVIER_PEP_1_Pos                (13U)                                          /**< (USBHS_DEVIER) Endpoint 1 Interrupt Enable Position */
#define USBHS_DEVIER_PEP_1_Msk                (0x1U << USBHS_DEVIER_PEP_1_Pos)               /**< (USBHS_DEVIER) Endpoint 1 Interrupt Enable Mask */
#define USBHS_DEVIER_PEP_1(value)             (USBHS_DEVIER_PEP_1_Msk & ((value) << USBHS_DEVIER_PEP_1_Pos))
#define USBHS_DEVIER_PEP_2_Pos                (14U)                                          /**< (USBHS_DEVIER) Endpoint 2 Interrupt Enable Position */
#define USBHS_DEVIER_PEP_2_Msk                (0x1U << USBHS_DEVIER_PEP_2_Pos)               /**< (USBHS_DEVIER) Endpoint 2 Interrupt Enable Mask */
#define USBHS_DEVIER_PEP_2(value)             (USBHS_DEVIER_PEP_2_Msk & ((value) << USBHS_DEVIER_PEP_2_Pos))
#define USBHS_DEVIER_PEP_3_Pos                (15U)                                          /**< (USBHS_DEVIER) Endpoint 3 Interrupt Enable Position */
#define USBHS_DEVIER_PEP_3_Msk                (0x1U << USBHS_DEVIER_PEP_3_Pos)               /**< (USBHS_DEVIER) Endpoint 3 Interrupt Enable Mask */
#define USBHS_DEVIER_PEP_3(value)             (USBHS_DEVIER_PEP_3_Msk & ((value) << USBHS_DEVIER_PEP_3_Pos))
#define USBHS_DEVIER_PEP_4_Pos                (16U)                                          /**< (USBHS_DEVIER) Endpoint 4 Interrupt Enable Position */
#define USBHS_DEVIER_PEP_4_Msk                (0x1U << USBHS_DEVIER_PEP_4_Pos)               /**< (USBHS_DEVIER) Endpoint 4 Interrupt Enable Mask */
#define USBHS_DEVIER_PEP_4(value)             (USBHS_DEVIER_PEP_4_Msk & ((value) << USBHS_DEVIER_PEP_4_Pos))
#define USBHS_DEVIER_PEP_5_Pos                (17U)                                          /**< (USBHS_DEVIER) Endpoint 5 Interrupt Enable Position */
#define USBHS_DEVIER_PEP_5_Msk                (0x1U << USBHS_DEVIER_PEP_5_Pos)               /**< (USBHS_DEVIER) Endpoint 5 Interrupt Enable Mask */
#define USBHS_DEVIER_PEP_5(value)             (USBHS_DEVIER_PEP_5_Msk & ((value) << USBHS_DEVIER_PEP_5_Pos))
#define USBHS_DEVIER_PEP_6_Pos                (18U)                                          /**< (USBHS_DEVIER) Endpoint 6 Interrupt Enable Position */
#define USBHS_DEVIER_PEP_6_Msk                (0x1U << USBHS_DEVIER_PEP_6_Pos)               /**< (USBHS_DEVIER) Endpoint 6 Interrupt Enable Mask */
#define USBHS_DEVIER_PEP_6(value)             (USBHS_DEVIER_PEP_6_Msk & ((value) << USBHS_DEVIER_PEP_6_Pos))
#define USBHS_DEVIER_PEP_7_Pos                (19U)                                          /**< (USBHS_DEVIER) Endpoint 7 Interrupt Enable Position */
#define USBHS_DEVIER_PEP_7_Msk                (0x1U << USBHS_DEVIER_PEP_7_Pos)               /**< (USBHS_DEVIER) Endpoint 7 Interrupt Enable Mask */
#define USBHS_DEVIER_PEP_7(value)             (USBHS_DEVIER_PEP_7_Msk & ((value) << USBHS_DEVIER_PEP_7_Pos))
#define USBHS_DEVIER_PEP_8_Pos                (20U)                                          /**< (USBHS_DEVIER) Endpoint 8 Interrupt Enable Position */
#define USBHS_DEVIER_PEP_8_Msk                (0x1U << USBHS_DEVIER_PEP_8_Pos)               /**< (USBHS_DEVIER) Endpoint 8 Interrupt Enable Mask */
#define USBHS_DEVIER_PEP_8(value)             (USBHS_DEVIER_PEP_8_Msk & ((value) << USBHS_DEVIER_PEP_8_Pos))
#define USBHS_DEVIER_PEP_9_Pos                (21U)                                          /**< (USBHS_DEVIER) Endpoint 9 Interrupt Enable Position */
#define USBHS_DEVIER_PEP_9_Msk                (0x1U << USBHS_DEVIER_PEP_9_Pos)               /**< (USBHS_DEVIER) Endpoint 9 Interrupt Enable Mask */
#define USBHS_DEVIER_PEP_9(value)             (USBHS_DEVIER_PEP_9_Msk & ((value) << USBHS_DEVIER_PEP_9_Pos))
#define USBHS_DEVIER_DMA_1_Pos                (25U)                                          /**< (USBHS_DEVIER) DMA Channel 1 Interrupt Enable Position */
#define USBHS_DEVIER_DMA_1_Msk                (0x1U << USBHS_DEVIER_DMA_1_Pos)               /**< (USBHS_DEVIER) DMA Channel 1 Interrupt Enable Mask */
#define USBHS_DEVIER_DMA_1(value)             (USBHS_DEVIER_DMA_1_Msk & ((value) << USBHS_DEVIER_DMA_1_Pos))
#define USBHS_DEVIER_DMA_2_Pos                (26U)                                          /**< (USBHS_DEVIER) DMA Channel 2 Interrupt Enable Position */
#define USBHS_DEVIER_DMA_2_Msk                (0x1U << USBHS_DEVIER_DMA_2_Pos)               /**< (USBHS_DEVIER) DMA Channel 2 Interrupt Enable Mask */
#define USBHS_DEVIER_DMA_2(value)             (USBHS_DEVIER_DMA_2_Msk & ((value) << USBHS_DEVIER_DMA_2_Pos))
#define USBHS_DEVIER_DMA_3_Pos                (27U)                                          /**< (USBHS_DEVIER) DMA Channel 3 Interrupt Enable Position */
#define USBHS_DEVIER_DMA_3_Msk                (0x1U << USBHS_DEVIER_DMA_3_Pos)               /**< (USBHS_DEVIER) DMA Channel 3 Interrupt Enable Mask */
#define USBHS_DEVIER_DMA_3(value)             (USBHS_DEVIER_DMA_3_Msk & ((value) << USBHS_DEVIER_DMA_3_Pos))
#define USBHS_DEVIER_DMA_4_Pos                (28U)                                          /**< (USBHS_DEVIER) DMA Channel 4 Interrupt Enable Position */
#define USBHS_DEVIER_DMA_4_Msk                (0x1U << USBHS_DEVIER_DMA_4_Pos)               /**< (USBHS_DEVIER) DMA Channel 4 Interrupt Enable Mask */
#define USBHS_DEVIER_DMA_4(value)             (USBHS_DEVIER_DMA_4_Msk & ((value) << USBHS_DEVIER_DMA_4_Pos))
#define USBHS_DEVIER_DMA_5_Pos                (29U)                                          /**< (USBHS_DEVIER) DMA Channel 5 Interrupt Enable Position */
#define USBHS_DEVIER_DMA_5_Msk                (0x1U << USBHS_DEVIER_DMA_5_Pos)               /**< (USBHS_DEVIER) DMA Channel 5 Interrupt Enable Mask */
#define USBHS_DEVIER_DMA_5(value)             (USBHS_DEVIER_DMA_5_Msk & ((value) << USBHS_DEVIER_DMA_5_Pos))
#define USBHS_DEVIER_DMA_6_Pos                (30U)                                          /**< (USBHS_DEVIER) DMA Channel 6 Interrupt Enable Position */
#define USBHS_DEVIER_DMA_6_Msk                (0x1U << USBHS_DEVIER_DMA_6_Pos)               /**< (USBHS_DEVIER) DMA Channel 6 Interrupt Enable Mask */
#define USBHS_DEVIER_DMA_6(value)             (USBHS_DEVIER_DMA_6_Msk & ((value) << USBHS_DEVIER_DMA_6_Pos))
#define USBHS_DEVIER_DMA_7_Pos                (31U)                                          /**< (USBHS_DEVIER) DMA Channel 7 Interrupt Enable Position */
#define USBHS_DEVIER_DMA_7_Msk                (0x1U << USBHS_DEVIER_DMA_7_Pos)               /**< (USBHS_DEVIER) DMA Channel 7 Interrupt Enable Mask */
#define USBHS_DEVIER_DMA_7(value)             (USBHS_DEVIER_DMA_7_Msk & ((value) << USBHS_DEVIER_DMA_7_Pos))
#define USBHS_DEVIER_Msk                      (0xFE3FF07FUL)                                 /**< (USBHS_DEVIER) Register Mask  */

/* -------- USBHS_DEVEPT : (USBHS Offset: 0x1C) (R/W 32) Device Endpoint Register -------- */
#define USBHS_DEVEPT_EPEN0_Pos                (0U)                                           /**< (USBHS_DEVEPT) Endpoint 0 Enable Position */
#define USBHS_DEVEPT_EPEN0_Msk                (0x1U << USBHS_DEVEPT_EPEN0_Pos)               /**< (USBHS_DEVEPT) Endpoint 0 Enable Mask */
#define USBHS_DEVEPT_EPEN0(value)             (USBHS_DEVEPT_EPEN0_Msk & ((value) << USBHS_DEVEPT_EPEN0_Pos))
#define USBHS_DEVEPT_EPEN1_Pos                (1U)                                           /**< (USBHS_DEVEPT) Endpoint 1 Enable Position */
#define USBHS_DEVEPT_EPEN1_Msk                (0x1U << USBHS_DEVEPT_EPEN1_Pos)               /**< (USBHS_DEVEPT) Endpoint 1 Enable Mask */
#define USBHS_DEVEPT_EPEN1(value)             (USBHS_DEVEPT_EPEN1_Msk & ((value) << USBHS_DEVEPT_EPEN1_Pos))
#define USBHS_DEVEPT_EPEN2_Pos                (2U)                                           /**< (USBHS_DEVEPT) Endpoint 2 Enable Position */
#define USBHS_DEVEPT_EPEN2_Msk                (0x1U << USBHS_DEVEPT_EPEN2_Pos)               /**< (USBHS_DEVEPT) Endpoint 2 Enable Mask */
#define USBHS_DEVEPT_EPEN2(value)             (USBHS_DEVEPT_EPEN2_Msk & ((value) << USBHS_DEVEPT_EPEN2_Pos))
#define USBHS_DEVEPT_EPEN3_Pos                (3U)                                           /**< (USBHS_DEVEPT) Endpoint 3 Enable Position */
#define USBHS_DEVEPT_EPEN3_Msk                (0x1U << USBHS_DEVEPT_EPEN3_Pos)               /**< (USBHS_DEVEPT) Endpoint 3 Enable Mask */
#define USBHS_DEVEPT_EPEN3(value)             (USBHS_DEVEPT_EPEN3_Msk & ((value) << USBHS_DEVEPT_EPEN3_Pos))
#define USBHS_DEVEPT_EPEN4_Pos                (4U)                                           /**< (USBHS_DEVEPT) Endpoint 4 Enable Position */
#define USBHS_DEVEPT_EPEN4_Msk                (0x1U << USBHS_DEVEPT_EPEN4_Pos)               /**< (USBHS_DEVEPT) Endpoint 4 Enable Mask */
#define USBHS_DEVEPT_EPEN4(value)             (USBHS_DEVEPT_EPEN4_Msk & ((value) << USBHS_DEVEPT_EPEN4_Pos))
#define USBHS_DEVEPT_EPEN5_Pos                (5U)                                           /**< (USBHS_DEVEPT) Endpoint 5 Enable Position */
#define USBHS_DEVEPT_EPEN5_Msk                (0x1U << USBHS_DEVEPT_EPEN5_Pos)               /**< (USBHS_DEVEPT) Endpoint 5 Enable Mask */
#define USBHS_DEVEPT_EPEN5(value)             (USBHS_DEVEPT_EPEN5_Msk & ((value) << USBHS_DEVEPT_EPEN5_Pos))
#define USBHS_DEVEPT_EPEN6_Pos                (6U)                                           /**< (USBHS_DEVEPT) Endpoint 6 Enable Position */
#define USBHS_DEVEPT_EPEN6_Msk                (0x1U << USBHS_DEVEPT_EPEN6_Pos)               /**< (USBHS_DEVEPT) Endpoint 6 Enable Mask */
#define USBHS_DEVEPT_EPEN6(value)             (USBHS_DEVEPT_EPEN6_Msk & ((value) << USBHS_DEVEPT_EPEN6_Pos))
#define USBHS_DEVEPT_EPEN7_Pos                (7U)                                           /**< (USBHS_DEVEPT) Endpoint 7 Enable Position */
#define USBHS_DEVEPT_EPEN7_Msk                (0x1U << USBHS_DEVEPT_EPEN7_Pos)               /**< (USBHS_DEVEPT) Endpoint 7 Enable Mask */
#define USBHS_DEVEPT_EPEN7(value)             (USBHS_DEVEPT_EPEN7_Msk & ((value) << USBHS_DEVEPT_EPEN7_Pos))
#define USBHS_DEVEPT_EPEN8_Pos                (8U)                                           /**< (USBHS_DEVEPT) Endpoint 8 Enable Position */
#define USBHS_DEVEPT_EPEN8_Msk                (0x1U << USBHS_DEVEPT_EPEN8_Pos)               /**< (USBHS_DEVEPT) Endpoint 8 Enable Mask */
#define USBHS_DEVEPT_EPEN8(value)             (USBHS_DEVEPT_EPEN8_Msk & ((value) << USBHS_DEVEPT_EPEN8_Pos))
#define USBHS_DEVEPT_EPEN9_Pos                (9U)                                           /**< (USBHS_DEVEPT) Endpoint 9 Enable Position */
#define USBHS_DEVEPT_EPEN9_Msk                (0x1U << USBHS_DEVEPT_EPEN9_Pos)               /**< (USBHS_DEVEPT) Endpoint 9 Enable Mask */
#define USBHS_DEVEPT_EPEN9(value)             (USBHS_DEVEPT_EPEN9_Msk & ((value) << USBHS_DEVEPT_EPEN9_Pos))
#define USBHS_DEVEPT_EPRST0_Pos               (16U)                                          /**< (USBHS_DEVEPT) Endpoint 0 Reset Position */
#define USBHS_DEVEPT_EPRST0_Msk               (0x1U << USBHS_DEVEPT_EPRST0_Pos)              /**< (USBHS_DEVEPT) Endpoint 0 Reset Mask */
#define USBHS_DEVEPT_EPRST0(value)            (USBHS_DEVEPT_EPRST0_Msk & ((value) << USBHS_DEVEPT_EPRST0_Pos))
#define USBHS_DEVEPT_EPRST1_Pos               (17U)                                          /**< (USBHS_DEVEPT) Endpoint 1 Reset Position */
#define USBHS_DEVEPT_EPRST1_Msk               (0x1U << USBHS_DEVEPT_EPRST1_Pos)              /**< (USBHS_DEVEPT) Endpoint 1 Reset Mask */
#define USBHS_DEVEPT_EPRST1(value)            (USBHS_DEVEPT_EPRST1_Msk & ((value) << USBHS_DEVEPT_EPRST1_Pos))
#define USBHS_DEVEPT_EPRST2_Pos               (18U)                                          /**< (USBHS_DEVEPT) Endpoint 2 Reset Position */
#define USBHS_DEVEPT_EPRST2_Msk               (0x1U << USBHS_DEVEPT_EPRST2_Pos)              /**< (USBHS_DEVEPT) Endpoint 2 Reset Mask */
#define USBHS_DEVEPT_EPRST2(value)            (USBHS_DEVEPT_EPRST2_Msk & ((value) << USBHS_DEVEPT_EPRST2_Pos))
#define USBHS_DEVEPT_EPRST3_Pos               (19U)                                          /**< (USBHS_DEVEPT) Endpoint 3 Reset Position */
#define USBHS_DEVEPT_EPRST3_Msk               (0x1U << USBHS_DEVEPT_EPRST3_Pos)              /**< (USBHS_DEVEPT) Endpoint 3 Reset Mask */
#define USBHS_DEVEPT_EPRST3(value)            (USBHS_DEVEPT_EPRST3_Msk & ((value) << USBHS_DEVEPT_EPRST3_Pos))
#define USBHS_DEVEPT_EPRST4_Pos               (20U)                                          /**< (USBHS_DEVEPT) Endpoint 4 Reset Position */
#define USBHS_DEVEPT_EPRST4_Msk               (0x1U << USBHS_DEVEPT_EPRST4_Pos)              /**< (USBHS_DEVEPT) Endpoint 4 Reset Mask */
#define USBHS_DEVEPT_EPRST4(value)            (USBHS_DEVEPT_EPRST4_Msk & ((value) << USBHS_DEVEPT_EPRST4_Pos))
#define USBHS_DEVEPT_EPRST5_Pos               (21U)                                          /**< (USBHS_DEVEPT) Endpoint 5 Reset Position */
#define USBHS_DEVEPT_EPRST5_Msk               (0x1U << USBHS_DEVEPT_EPRST5_Pos)              /**< (USBHS_DEVEPT) Endpoint 5 Reset Mask */
#define USBHS_DEVEPT_EPRST5(value)            (USBHS_DEVEPT_EPRST5_Msk & ((value) << USBHS_DEVEPT_EPRST5_Pos))
#define USBHS_DEVEPT_EPRST6_Pos               (22U)                                          /**< (USBHS_DEVEPT) Endpoint 6 Reset Position */
#define USBHS_DEVEPT_EPRST6_Msk               (0x1U << USBHS_DEVEPT_EPRST6_Pos)              /**< (USBHS_DEVEPT) Endpoint 6 Reset Mask */
#define USBHS_DEVEPT_EPRST6(value)            (USBHS_DEVEPT_EPRST6_Msk & ((value) << USBHS_DEVEPT_EPRST6_Pos))
#define USBHS_DEVEPT_EPRST7_Pos               (23U)                                          /**< (USBHS_DEVEPT) Endpoint 7 Reset Position */
#define USBHS_DEVEPT_EPRST7_Msk               (0x1U << USBHS_DEVEPT_EPRST7_Pos)              /**< (USBHS_DEVEPT) Endpoint 7 Reset Mask */
#define USBHS_DEVEPT_EPRST7(value)            (USBHS_DEVEPT_EPRST7_Msk & ((value) << USBHS_DEVEPT_EPRST7_Pos))
#define USBHS_DEVEPT_EPRST8_Pos               (24U)                                          /**< (USBHS_DEVEPT) Endpoint 8 Reset Position */
#define USBHS_DEVEPT_EPRST8_Msk               (0x1U << USBHS_DEVEPT_EPRST8_Pos)              /**< (USBHS_DEVEPT) Endpoint 8 Reset Mask */
#define USBHS_DEVEPT_EPRST8(value)            (USBHS_DEVEPT_EPRST8_Msk & ((value) << USBHS_DEVEPT_EPRST8_Pos))
#define USBHS_DEVEPT_EPRST9_Pos               (25U)                                          /**< (USBHS_DEVEPT) Endpoint 9 Reset Position */
#define USBHS_DEVEPT_EPRST9_Msk               (0x1U << USBHS_DEVEPT_EPRST9_Pos)              /**< (USBHS_DEVEPT) Endpoint 9 Reset Mask */
#define USBHS_DEVEPT_EPRST9(value)            (USBHS_DEVEPT_EPRST9_Msk & ((value) << USBHS_DEVEPT_EPRST9_Pos))
#define USBHS_DEVEPT_Msk                      (0x03FF03FFUL)                                 /**< (USBHS_DEVEPT) Register Mask  */

/* -------- USBHS_DEVFNUM : (USBHS Offset: 0x20) (R/  32) Device Frame Number Register -------- */
#define USBHS_DEVFNUM_MFNUM_Pos               (0U)                                           /**< (USBHS_DEVFNUM) Micro Frame Number Position */
#define USBHS_DEVFNUM_MFNUM_Msk               (0x7U << USBHS_DEVFNUM_MFNUM_Pos)              /**< (USBHS_DEVFNUM) Micro Frame Number Mask */
#define USBHS_DEVFNUM_MFNUM(value)            (USBHS_DEVFNUM_MFNUM_Msk & ((value) << USBHS_DEVFNUM_MFNUM_Pos))
#define USBHS_DEVFNUM_FNUM_Pos                (3U)                                           /**< (USBHS_DEVFNUM) Frame Number Position */
#define USBHS_DEVFNUM_FNUM_Msk                (0x7FFU << USBHS_DEVFNUM_FNUM_Pos)             /**< (USBHS_DEVFNUM) Frame Number Mask */
#define USBHS_DEVFNUM_FNUM(value)             (USBHS_DEVFNUM_FNUM_Msk & ((value) << USBHS_DEVFNUM_FNUM_Pos))
#define USBHS_DEVFNUM_FNCERR_Pos              (15U)                                          /**< (USBHS_DEVFNUM) Frame Number CRC Error Position */
#define USBHS_DEVFNUM_FNCERR_Msk              (0x1U << USBHS_DEVFNUM_FNCERR_Pos)             /**< (USBHS_DEVFNUM) Frame Number CRC Error Mask */
#define USBHS_DEVFNUM_FNCERR(value)           (USBHS_DEVFNUM_FNCERR_Msk & ((value) << USBHS_DEVFNUM_FNCERR_Pos))
#define USBHS_DEVFNUM_Msk                     (0x0000BFFFUL)                                 /**< (USBHS_DEVFNUM) Register Mask  */

/* -------- USBHS_DEVEPTCFG : (USBHS Offset: 0x100) (R/W 32) Device Endpoint Configuration Register -------- */
#define USBHS_DEVEPTCFG_ALLOC_Pos             (1U)                                           /**< (USBHS_DEVEPTCFG) Endpoint Memory Allocate Position */
#define USBHS_DEVEPTCFG_ALLOC_Msk             (0x1U << USBHS_DEVEPTCFG_ALLOC_Pos)            /**< (USBHS_DEVEPTCFG) Endpoint Memory Allocate Mask */
#define USBHS_DEVEPTCFG_ALLOC(value)          (USBHS_DEVEPTCFG_ALLOC_Msk & ((value) << USBHS_DEVEPTCFG_ALLOC_Pos))
#define USBHS_DEVEPTCFG_EPBK_Pos              (2U)                                           /**< (USBHS_DEVEPTCFG) Endpoint Banks Position */
#define USBHS_DEVEPTCFG_EPBK_Msk              (0x3U << USBHS_DEVEPTCFG_EPBK_Pos)             /**< (USBHS_DEVEPTCFG) Endpoint Banks Mask */
#define USBHS_DEVEPTCFG_EPBK(value)           (USBHS_DEVEPTCFG_EPBK_Msk & ((value) << USBHS_DEVEPTCFG_EPBK_Pos))
#define   USBHS_DEVEPTCFG_EPBK_1_BANK_Val     (0U)                                           /**< (USBHS_DEVEPTCFG) Single-bank endpoint  */
#define   USBHS_DEVEPTCFG_EPBK_2_BANK_Val     (1U)                                           /**< (USBHS_DEVEPTCFG) Double-bank endpoint  */
#define   USBHS_DEVEPTCFG_EPBK_3_BANK_Val     (2U)                                           /**< (USBHS_DEVEPTCFG) Triple-bank endpoint  */
#define USBHS_DEVEPTCFG_EPBK_1_BANK           (USBHS_DEVEPTCFG_EPBK_1_BANK_Val << USBHS_DEVEPTCFG_EPBK_Pos) /**< (USBHS_DEVEPTCFG) Single-bank endpoint Position  */
#define USBHS_DEVEPTCFG_EPBK_2_BANK           (USBHS_DEVEPTCFG_EPBK_2_BANK_Val << USBHS_DEVEPTCFG_EPBK_Pos) /**< (USBHS_DEVEPTCFG) Double-bank endpoint Position  */
#define USBHS_DEVEPTCFG_EPBK_3_BANK           (USBHS_DEVEPTCFG_EPBK_3_BANK_Val << USBHS_DEVEPTCFG_EPBK_Pos) /**< (USBHS_DEVEPTCFG) Triple-bank endpoint Position  */
#define USBHS_DEVEPTCFG_EPSIZE_Pos            (4U)                                           /**< (USBHS_DEVEPTCFG) Endpoint Size Position */
#define USBHS_DEVEPTCFG_EPSIZE_Msk            (0x7U << USBHS_DEVEPTCFG_EPSIZE_Pos)           /**< (USBHS_DEVEPTCFG) Endpoint Size Mask */
#define USBHS_DEVEPTCFG_EPSIZE(value)         (USBHS_DEVEPTCFG_EPSIZE_Msk & ((value) << USBHS_DEVEPTCFG_EPSIZE_Pos))
#define   USBHS_DEVEPTCFG_EPSIZE_8_BYTE_Val   (0U)                                           /**< (USBHS_DEVEPTCFG) 8 bytes  */
#define   USBHS_DEVEPTCFG_EPSIZE_16_BYTE_Val  (1U)                                           /**< (USBHS_DEVEPTCFG) 16 bytes  */
#define   USBHS_DEVEPTCFG_EPSIZE_32_BYTE_Val  (2U)                                           /**< (USBHS_DEVEPTCFG) 32 bytes  */
#define   USBHS_DEVEPTCFG_EPSIZE_64_BYTE_Val  (3U)                                           /**< (USBHS_DEVEPTCFG) 64 bytes  */
#define   USBHS_DEVEPTCFG_EPSIZE_128_BYTE_Val (4U)                                           /**< (USBHS_DEVEPTCFG) 128 bytes  */
#define   USBHS_DEVEPTCFG_EPSIZE_256_BYTE_Val (5U)                                           /**< (USBHS_DEVEPTCFG) 256 bytes  */
#define   USBHS_DEVEPTCFG_EPSIZE_512_BYTE_Val (6U)                                           /**< (USBHS_DEVEPTCFG) 512 bytes  */
#define   USBHS_DEVEPTCFG_EPSIZE_1024_BYTE_Val (7U)                                           /**< (USBHS_DEVEPTCFG) 1024 bytes  */
#define USBHS_DEVEPTCFG_EPSIZE_8_BYTE         (USBHS_DEVEPTCFG_EPSIZE_8_BYTE_Val << USBHS_DEVEPTCFG_EPSIZE_Pos) /**< (USBHS_DEVEPTCFG) 8 bytes Position  */
#define USBHS_DEVEPTCFG_EPSIZE_16_BYTE        (USBHS_DEVEPTCFG_EPSIZE_16_BYTE_Val << USBHS_DEVEPTCFG_EPSIZE_Pos) /**< (USBHS_DEVEPTCFG) 16 bytes Position  */
#define USBHS_DEVEPTCFG_EPSIZE_32_BYTE        (USBHS_DEVEPTCFG_EPSIZE_32_BYTE_Val << USBHS_DEVEPTCFG_EPSIZE_Pos) /**< (USBHS_DEVEPTCFG) 32 bytes Position  */
#define USBHS_DEVEPTCFG_EPSIZE_64_BYTE        (USBHS_DEVEPTCFG_EPSIZE_64_BYTE_Val << USBHS_DEVEPTCFG_EPSIZE_Pos) /**< (USBHS_DEVEPTCFG) 64 bytes Position  */
#define USBHS_DEVEPTCFG_EPSIZE_128_BYTE       (USBHS_DEVEPTCFG_EPSIZE_128_BYTE_Val << USBHS_DEVEPTCFG_EPSIZE_Pos) /**< (USBHS_DEVEPTCFG) 128 bytes Position  */
#define USBHS_DEVEPTCFG_EPSIZE_256_BYTE       (USBHS_DEVEPTCFG_EPSIZE_256_BYTE_Val << USBHS_DEVEPTCFG_EPSIZE_Pos) /**< (USBHS_DEVEPTCFG) 256 bytes Position  */
#define USBHS_DEVEPTCFG_EPSIZE_512_BYTE       (USBHS_DEVEPTCFG_EPSIZE_512_BYTE_Val << USBHS_DEVEPTCFG_EPSIZE_Pos) /**< (USBHS_DEVEPTCFG) 512 bytes Position  */
#define USBHS_DEVEPTCFG_EPSIZE_1024_BYTE      (USBHS_DEVEPTCFG_EPSIZE_1024_BYTE_Val << USBHS_DEVEPTCFG_EPSIZE_Pos) /**< (USBHS_DEVEPTCFG) 1024 bytes Position  */
#define USBHS_DEVEPTCFG_EPDIR_Pos             (8U)                                           /**< (USBHS_DEVEPTCFG) Endpoint Direction Position */
#define USBHS_DEVEPTCFG_EPDIR_Msk             (0x1U << USBHS_DEVEPTCFG_EPDIR_Pos)            /**< (USBHS_DEVEPTCFG) Endpoint Direction Mask */
#define USBHS_DEVEPTCFG_EPDIR(value)          (USBHS_DEVEPTCFG_EPDIR_Msk & ((value) << USBHS_DEVEPTCFG_EPDIR_Pos))
#define   USBHS_DEVEPTCFG_EPDIR_OUT_Val       (0U)                                           /**< (USBHS_DEVEPTCFG) The endpoint direction is OUT.  */
#define   USBHS_DEVEPTCFG_EPDIR_IN_Val        (1U)                                           /**< (USBHS_DEVEPTCFG) The endpoint direction is IN (nor for control endpoints).  */
#define USBHS_DEVEPTCFG_EPDIR_OUT             (USBHS_DEVEPTCFG_EPDIR_OUT_Val << USBHS_DEVEPTCFG_EPDIR_Pos) /**< (USBHS_DEVEPTCFG) The endpoint direction is OUT. Position  */
#define USBHS_DEVEPTCFG_EPDIR_IN              (USBHS_DEVEPTCFG_EPDIR_IN_Val << USBHS_DEVEPTCFG_EPDIR_Pos) /**< (USBHS_DEVEPTCFG) The endpoint direction is IN (nor for control endpoints). Position  */
#define USBHS_DEVEPTCFG_AUTOSW_Pos            (9U)                                           /**< (USBHS_DEVEPTCFG) Automatic Switch Position */
#define USBHS_DEVEPTCFG_AUTOSW_Msk            (0x1U << USBHS_DEVEPTCFG_AUTOSW_Pos)           /**< (USBHS_DEVEPTCFG) Automatic Switch Mask */
#define USBHS_DEVEPTCFG_AUTOSW(value)         (USBHS_DEVEPTCFG_AUTOSW_Msk & ((value) << USBHS_DEVEPTCFG_AUTOSW_Pos))
#define USBHS_DEVEPTCFG_EPTYPE_Pos            (11U)                                          /**< (USBHS_DEVEPTCFG) Endpoint Type Position */
#define USBHS_DEVEPTCFG_EPTYPE_Msk            (0x3U << USBHS_DEVEPTCFG_EPTYPE_Pos)           /**< (USBHS_DEVEPTCFG) Endpoint Type Mask */
#define USBHS_DEVEPTCFG_EPTYPE(value)         (USBHS_DEVEPTCFG_EPTYPE_Msk & ((value) << USBHS_DEVEPTCFG_EPTYPE_Pos))
#define   USBHS_DEVEPTCFG_EPTYPE_CTRL_Val     (0U)                                           /**< (USBHS_DEVEPTCFG) Control  */
#define   USBHS_DEVEPTCFG_EPTYPE_ISO_Val      (1U)                                           /**< (USBHS_DEVEPTCFG) Isochronous  */
#define   USBHS_DEVEPTCFG_EPTYPE_BLK_Val      (2U)                                           /**< (USBHS_DEVEPTCFG) Bulk  */
#define   USBHS_DEVEPTCFG_EPTYPE_INTRPT_Val   (3U)                                           /**< (USBHS_DEVEPTCFG) Interrupt  */
#define USBHS_DEVEPTCFG_EPTYPE_CTRL           (USBHS_DEVEPTCFG_EPTYPE_CTRL_Val << USBHS_DEVEPTCFG_EPTYPE_Pos) /**< (USBHS_DEVEPTCFG) Control Position  */
#define USBHS_DEVEPTCFG_EPTYPE_ISO            (USBHS_DEVEPTCFG_EPTYPE_ISO_Val << USBHS_DEVEPTCFG_EPTYPE_Pos) /**< (USBHS_DEVEPTCFG) Isochronous Position  */
#define USBHS_DEVEPTCFG_EPTYPE_BLK            (USBHS_DEVEPTCFG_EPTYPE_BLK_Val << USBHS_DEVEPTCFG_EPTYPE_Pos) /**< (USBHS_DEVEPTCFG) Bulk Position  */
#define USBHS_DEVEPTCFG_EPTYPE_INTRPT         (USBHS_DEVEPTCFG_EPTYPE_INTRPT_Val << USBHS_DEVEPTCFG_EPTYPE_Pos) /**< (USBHS_DEVEPTCFG) Interrupt Position  */
#define USBHS_DEVEPTCFG_NBTRANS_Pos           (13U)                                          /**< (USBHS_DEVEPTCFG) Number of transactions per microframe for isochronous endpoint Position */
#define USBHS_DEVEPTCFG_NBTRANS_Msk           (0x3U << USBHS_DEVEPTCFG_NBTRANS_Pos)          /**< (USBHS_DEVEPTCFG) Number of transactions per microframe for isochronous endpoint Mask */
#define USBHS_DEVEPTCFG_NBTRANS(value)        (USBHS_DEVEPTCFG_NBTRANS_Msk & ((value) << USBHS_DEVEPTCFG_NBTRANS_Pos))
#define   USBHS_DEVEPTCFG_NBTRANS_0_TRANS_Val (0U)                                           /**< (USBHS_DEVEPTCFG) Reserved to endpoint that does not have the high-bandwidth isochronous capability.  */
#define   USBHS_DEVEPTCFG_NBTRANS_1_TRANS_Val (1U)                                           /**< (USBHS_DEVEPTCFG) Default value: one transaction per microframe.  */
#define   USBHS_DEVEPTCFG_NBTRANS_2_TRANS_Val (2U)                                           /**< (USBHS_DEVEPTCFG) Two transactions per microframe. This endpoint should be configured as double-bank.  */
#define   USBHS_DEVEPTCFG_NBTRANS_3_TRANS_Val (3U)                                           /**< (USBHS_DEVEPTCFG) Three transactions per microframe. This endpoint should be configured as triple-bank.  */
#define USBHS_DEVEPTCFG_NBTRANS_0_TRANS       (USBHS_DEVEPTCFG_NBTRANS_0_TRANS_Val << USBHS_DEVEPTCFG_NBTRANS_Pos) /**< (USBHS_DEVEPTCFG) Reserved to endpoint that does not have the high-bandwidth isochronous capability. Position  */
#define USBHS_DEVEPTCFG_NBTRANS_1_TRANS       (USBHS_DEVEPTCFG_NBTRANS_1_TRANS_Val << USBHS_DEVEPTCFG_NBTRANS_Pos) /**< (USBHS_DEVEPTCFG) Default value: one transaction per microframe. Position  */
#define USBHS_DEVEPTCFG_NBTRANS_2_TRANS       (USBHS_DEVEPTCFG_NBTRANS_2_TRANS_Val << USBHS_DEVEPTCFG_NBTRANS_Pos) /**< (USBHS_DEVEPTCFG) Two transactions per microframe. This endpoint should be configured as double-bank. Position  */
#define USBHS_DEVEPTCFG_NBTRANS_3_TRANS       (USBHS_DEVEPTCFG_NBTRANS_3_TRANS_Val << USBHS_DEVEPTCFG_NBTRANS_Pos) /**< (USBHS_DEVEPTCFG) Three transactions per microframe. This endpoint should be configured as triple-bank. Position  */
#define USBHS_DEVEPTCFG_Msk                   (0x00007B7EUL)                                 /**< (USBHS_DEVEPTCFG) Register Mask  */

/* -------- USBHS_DEVEPTISR : (USBHS Offset: 0x130) (R/  32) Device Endpoint Interrupt Status Register -------- */
#define USBHS_DEVEPTISR_TXINI_Pos             (0U)                                           /**< (USBHS_DEVEPTISR) Transmitted IN Data Interrupt Position */
#define USBHS_DEVEPTISR_TXINI_Msk             (0x1U << USBHS_DEVEPTISR_TXINI_Pos)            /**< (USBHS_DEVEPTISR) Transmitted IN Data Interrupt Mask */
#define USBHS_DEVEPTISR_TXINI(value)          (USBHS_DEVEPTISR_TXINI_Msk & ((value) << USBHS_DEVEPTISR_TXINI_Pos))
#define USBHS_DEVEPTISR_RXOUTI_Pos            (1U)                                           /**< (USBHS_DEVEPTISR) Received OUT Data Interrupt Position */
#define USBHS_DEVEPTISR_RXOUTI_Msk            (0x1U << USBHS_DEVEPTISR_RXOUTI_Pos)           /**< (USBHS_DEVEPTISR) Received OUT Data Interrupt Mask */
#define USBHS_DEVEPTISR_RXOUTI(value)         (USBHS_DEVEPTISR_RXOUTI_Msk & ((value) << USBHS_DEVEPTISR_RXOUTI_Pos))
#define USBHS_DEVEPTISR_RXSTPI_Pos            (2U)                                           /**< (USBHS_DEVEPTISR) Received SETUP Interrupt Position */
#define USBHS_DEVEPTISR_RXSTPI_Msk            (0x1U << USBHS_DEVEPTISR_RXSTPI_Pos)           /**< (USBHS_DEVEPTISR) Received SETUP Interrupt Mask */
#define USBHS_DEVEPTISR_RXSTPI(value)         (USBHS_DEVEPTISR_RXSTPI_Msk & ((value) << USBHS_DEVEPTISR_RXSTPI_Pos))
#define USBHS_DEVEPTISR_UNDERFI_Pos           (2U)                                           /**< (USBHS_DEVEPTISR) Underflow Interrupt Position */
#define USBHS_DEVEPTISR_UNDERFI_Msk           (0x1U << USBHS_DEVEPTISR_UNDERFI_Pos)          /**< (USBHS_DEVEPTISR) Underflow Interrupt Mask */
#define USBHS_DEVEPTISR_UNDERFI(value)        (USBHS_DEVEPTISR_UNDERFI_Msk & ((value) << USBHS_DEVEPTISR_UNDERFI_Pos))
#define USBHS_DEVEPTISR_NAKOUTI_Pos           (3U)                                           /**< (USBHS_DEVEPTISR) NAKed OUT Interrupt Position */
#define USBHS_DEVEPTISR_NAKOUTI_Msk           (0x1U << USBHS_DEVEPTISR_NAKOUTI_Pos)          /**< (USBHS_DEVEPTISR) NAKed OUT Interrupt Mask */
#define USBHS_DEVEPTISR_NAKOUTI(value)        (USBHS_DEVEPTISR_NAKOUTI_Msk & ((value) << USBHS_DEVEPTISR_NAKOUTI_Pos))
#define USBHS_DEVEPTISR_HBISOINERRI_Pos       (3U)                                           /**< (USBHS_DEVEPTISR) High Bandwidth Isochronous IN Underflow Error Interrupt Position */
#define USBHS_DEVEPTISR_HBISOINERRI_Msk       (0x1U << USBHS_DEVEPTISR_HBISOINERRI_Pos)      /**< (USBHS_DEVEPTISR) High Bandwidth Isochronous IN Underflow Error Interrupt Mask */
#define USBHS_DEVEPTISR_HBISOINERRI(value)    (USBHS_DEVEPTISR_HBISOINERRI_Msk & ((value) << USBHS_DEVEPTISR_HBISOINERRI_Pos))
#define USBHS_DEVEPTISR_NAKINI_Pos            (4U)                                           /**< (USBHS_DEVEPTISR) NAKed IN Interrupt Position */
#define USBHS_DEVEPTISR_NAKINI_Msk            (0x1U << USBHS_DEVEPTISR_NAKINI_Pos)           /**< (USBHS_DEVEPTISR) NAKed IN Interrupt Mask */
#define USBHS_DEVEPTISR_NAKINI(value)         (USBHS_DEVEPTISR_NAKINI_Msk & ((value) << USBHS_DEVEPTISR_NAKINI_Pos))
#define USBHS_DEVEPTISR_HBISOFLUSHI_Pos       (4U)                                           /**< (USBHS_DEVEPTISR) High Bandwidth Isochronous IN Flush Interrupt Position */
#define USBHS_DEVEPTISR_HBISOFLUSHI_Msk       (0x1U << USBHS_DEVEPTISR_HBISOFLUSHI_Pos)      /**< (USBHS_DEVEPTISR) High Bandwidth Isochronous IN Flush Interrupt Mask */
#define USBHS_DEVEPTISR_HBISOFLUSHI(value)    (USBHS_DEVEPTISR_HBISOFLUSHI_Msk & ((value) << USBHS_DEVEPTISR_HBISOFLUSHI_Pos))
#define USBHS_DEVEPTISR_OVERFI_Pos            (5U)                                           /**< (USBHS_DEVEPTISR) Overflow Interrupt Position */
#define USBHS_DEVEPTISR_OVERFI_Msk            (0x1U << USBHS_DEVEPTISR_OVERFI_Pos)           /**< (USBHS_DEVEPTISR) Overflow Interrupt Mask */
#define USBHS_DEVEPTISR_OVERFI(value)         (USBHS_DEVEPTISR_OVERFI_Msk & ((value) << USBHS_DEVEPTISR_OVERFI_Pos))
#define USBHS_DEVEPTISR_STALLEDI_Pos          (6U)                                           /**< (USBHS_DEVEPTISR) STALLed Interrupt Position */
#define USBHS_DEVEPTISR_STALLEDI_Msk          (0x1U << USBHS_DEVEPTISR_STALLEDI_Pos)         /**< (USBHS_DEVEPTISR) STALLed Interrupt Mask */
#define USBHS_DEVEPTISR_STALLEDI(value)       (USBHS_DEVEPTISR_STALLEDI_Msk & ((value) << USBHS_DEVEPTISR_STALLEDI_Pos))
#define USBHS_DEVEPTISR_CRCERRI_Pos           (6U)                                           /**< (USBHS_DEVEPTISR) CRC Error Interrupt Position */
#define USBHS_DEVEPTISR_CRCERRI_Msk           (0x1U << USBHS_DEVEPTISR_CRCERRI_Pos)          /**< (USBHS_DEVEPTISR) CRC Error Interrupt Mask */
#define USBHS_DEVEPTISR_CRCERRI(value)        (USBHS_DEVEPTISR_CRCERRI_Msk & ((value) << USBHS_DEVEPTISR_CRCERRI_Pos))
#define USBHS_DEVEPTISR_SHORTPACKET_Pos       (7U)                                           /**< (USBHS_DEVEPTISR) Short Packet Interrupt Position */
#define USBHS_DEVEPTISR_SHORTPACKET_Msk       (0x1U << USBHS_DEVEPTISR_SHORTPACKET_Pos)      /**< (USBHS_DEVEPTISR) Short Packet Interrupt Mask */
#define USBHS_DEVEPTISR_SHORTPACKET(value)    (USBHS_DEVEPTISR_SHORTPACKET_Msk & ((value) << USBHS_DEVEPTISR_SHORTPACKET_Pos))
#define USBHS_DEVEPTISR_DTSEQ_Pos             (8U)                                           /**< (USBHS_DEVEPTISR) Data Toggle Sequence Position */
#define USBHS_DEVEPTISR_DTSEQ_Msk             (0x3U << USBHS_DEVEPTISR_DTSEQ_Pos)            /**< (USBHS_DEVEPTISR) Data Toggle Sequence Mask */
#define USBHS_DEVEPTISR_DTSEQ(value)          (USBHS_DEVEPTISR_DTSEQ_Msk & ((value) << USBHS_DEVEPTISR_DTSEQ_Pos))
#define   USBHS_DEVEPTISR_DTSEQ_DATA0_Val     (0U)                                           /**< (USBHS_DEVEPTISR) Data0 toggle sequence  */
#define   USBHS_DEVEPTISR_DTSEQ_DATA1_Val     (1U)                                           /**< (USBHS_DEVEPTISR) Data1 toggle sequence  */
#define   USBHS_DEVEPTISR_DTSEQ_DATA2_Val     (2U)                                           /**< (USBHS_DEVEPTISR) Reserved for high-bandwidth isochronous endpoint  */
#define   USBHS_DEVEPTISR_DTSEQ_MDATA_Val     (3U)                                           /**< (USBHS_DEVEPTISR) Reserved for high-bandwidth isochronous endpoint  */
#define USBHS_DEVEPTISR_DTSEQ_DATA0           (USBHS_DEVEPTISR_DTSEQ_DATA0_Val << USBHS_DEVEPTISR_DTSEQ_Pos) /**< (USBHS_DEVEPTISR) Data0 toggle sequence Position  */
#define USBHS_DEVEPTISR_DTSEQ_DATA1           (USBHS_DEVEPTISR_DTSEQ_DATA1_Val << USBHS_DEVEPTISR_DTSEQ_Pos) /**< (USBHS_DEVEPTISR) Data1 toggle sequence Position  */
#define USBHS_DEVEPTISR_DTSEQ_DATA2           (USBHS_DEVEPTISR_DTSEQ_DATA2_Val << USBHS_DEVEPTISR_DTSEQ_Pos) /**< (USBHS_DEVEPTISR) Reserved for high-bandwidth isochronous endpoint Position  */
#define USBHS_DEVEPTISR_DTSEQ_MDATA           (USBHS_DEVEPTISR_DTSEQ_MDATA_Val << USBHS_DEVEPTISR_DTSEQ_Pos) /**< (USBHS_DEVEPTISR) Reserved for high-bandwidth isochronous endpoint Position  */
#define USBHS_DEVEPTISR_ERRORTRANS_Pos        (10U)                                          /**< (USBHS_DEVEPTISR) High-bandwidth Isochronous OUT Endpoint Transaction Error Interrupt Position */
#define USBHS_DEVEPTISR_ERRORTRANS_Msk        (0x1U << USBHS_DEVEPTISR_ERRORTRANS_Pos)       /**< (USBHS_DEVEPTISR) High-bandwidth Isochronous OUT Endpoint Transaction Error Interrupt Mask */
#define USBHS_DEVEPTISR_ERRORTRANS(value)     (USBHS_DEVEPTISR_ERRORTRANS_Msk & ((value) << USBHS_DEVEPTISR_ERRORTRANS_Pos))
#define USBHS_DEVEPTISR_NBUSYBK_Pos           (12U)                                          /**< (USBHS_DEVEPTISR) Number of Busy Banks Position */
#define USBHS_DEVEPTISR_NBUSYBK_Msk           (0x3U << USBHS_DEVEPTISR_NBUSYBK_Pos)          /**< (USBHS_DEVEPTISR) Number of Busy Banks Mask */
#define USBHS_DEVEPTISR_NBUSYBK(value)        (USBHS_DEVEPTISR_NBUSYBK_Msk & ((value) << USBHS_DEVEPTISR_NBUSYBK_Pos))
#define   USBHS_DEVEPTISR_NBUSYBK_0_BUSY_Val  (0U)                                           /**< (USBHS_DEVEPTISR) 0 busy bank (all banks free)  */
#define   USBHS_DEVEPTISR_NBUSYBK_1_BUSY_Val  (1U)                                           /**< (USBHS_DEVEPTISR) 1 busy bank  */
#define   USBHS_DEVEPTISR_NBUSYBK_2_BUSY_Val  (2U)                                           /**< (USBHS_DEVEPTISR) 2 busy banks  */
#define   USBHS_DEVEPTISR_NBUSYBK_3_BUSY_Val  (3U)                                           /**< (USBHS_DEVEPTISR) 3 busy banks  */
#define USBHS_DEVEPTISR_NBUSYBK_0_BUSY        (USBHS_DEVEPTISR_NBUSYBK_0_BUSY_Val << USBHS_DEVEPTISR_NBUSYBK_Pos) /**< (USBHS_DEVEPTISR) 0 busy bank (all banks free) Position  */
#define USBHS_DEVEPTISR_NBUSYBK_1_BUSY        (USBHS_DEVEPTISR_NBUSYBK_1_BUSY_Val << USBHS_DEVEPTISR_NBUSYBK_Pos) /**< (USBHS_DEVEPTISR) 1 busy bank Position  */
#define USBHS_DEVEPTISR_NBUSYBK_2_BUSY        (USBHS_DEVEPTISR_NBUSYBK_2_BUSY_Val << USBHS_DEVEPTISR_NBUSYBK_Pos) /**< (USBHS_DEVEPTISR) 2 busy banks Position  */
#define USBHS_DEVEPTISR_NBUSYBK_3_BUSY        (USBHS_DEVEPTISR_NBUSYBK_3_BUSY_Val << USBHS_DEVEPTISR_NBUSYBK_Pos) /**< (USBHS_DEVEPTISR) 3 busy banks Position  */
#define USBHS_DEVEPTISR_CURRBK_Pos            (14U)                                          /**< (USBHS_DEVEPTISR) Current Bank Position */
#define USBHS_DEVEPTISR_CURRBK_Msk            (0x3U << USBHS_DEVEPTISR_CURRBK_Pos)           /**< (USBHS_DEVEPTISR) Current Bank Mask */
#define USBHS_DEVEPTISR_CURRBK(value)         (USBHS_DEVEPTISR_CURRBK_Msk & ((value) << USBHS_DEVEPTISR_CURRBK_Pos))
#define   USBHS_DEVEPTISR_CURRBK_BANK0_Val    (0U)                                           /**< (USBHS_DEVEPTISR) Current bank is bank0  */
#define   USBHS_DEVEPTISR_CURRBK_BANK1_Val    (1U)                                           /**< (USBHS_DEVEPTISR) Current bank is bank1  */
#define   USBHS_DEVEPTISR_CURRBK_BANK2_Val    (2U)                                           /**< (USBHS_DEVEPTISR) Current bank is bank2  */
#define USBHS_DEVEPTISR_CURRBK_BANK0          (USBHS_DEVEPTISR_CURRBK_BANK0_Val << USBHS_DEVEPTISR_CURRBK_Pos) /**< (USBHS_DEVEPTISR) Current bank is bank0 Position  */
#define USBHS_DEVEPTISR_CURRBK_BANK1          (USBHS_DEVEPTISR_CURRBK_BANK1_Val << USBHS_DEVEPTISR_CURRBK_Pos) /**< (USBHS_DEVEPTISR) Current bank is bank1 Position  */
#define USBHS_DEVEPTISR_CURRBK_BANK2          (USBHS_DEVEPTISR_CURRBK_BANK2_Val << USBHS_DEVEPTISR_CURRBK_Pos) /**< (USBHS_DEVEPTISR) Current bank is bank2 Position  */
#define USBHS_DEVEPTISR_RWALL_Pos             (16U)                                          /**< (USBHS_DEVEPTISR) Read/Write Allowed Position */
#define USBHS_DEVEPTISR_RWALL_Msk             (0x1U << USBHS_DEVEPTISR_RWALL_Pos)            /**< (USBHS_DEVEPTISR) Read/Write Allowed Mask */
#define USBHS_DEVEPTISR_RWALL(value)          (USBHS_DEVEPTISR_RWALL_Msk & ((value) << USBHS_DEVEPTISR_RWALL_Pos))
#define USBHS_DEVEPTISR_CTRLDIR_Pos           (17U)                                          /**< (USBHS_DEVEPTISR) Control Direction Position */
#define USBHS_DEVEPTISR_CTRLDIR_Msk           (0x1U << USBHS_DEVEPTISR_CTRLDIR_Pos)          /**< (USBHS_DEVEPTISR) Control Direction Mask */
#define USBHS_DEVEPTISR_CTRLDIR(value)        (USBHS_DEVEPTISR_CTRLDIR_Msk & ((value) << USBHS_DEVEPTISR_CTRLDIR_Pos))
#define USBHS_DEVEPTISR_CFGOK_Pos             (18U)                                          /**< (USBHS_DEVEPTISR) Configuration OK Status Position */
#define USBHS_DEVEPTISR_CFGOK_Msk             (0x1U << USBHS_DEVEPTISR_CFGOK_Pos)            /**< (USBHS_DEVEPTISR) Configuration OK Status Mask */
#define USBHS_DEVEPTISR_CFGOK(value)          (USBHS_DEVEPTISR_CFGOK_Msk & ((value) << USBHS_DEVEPTISR_CFGOK_Pos))
#define USBHS_DEVEPTISR_BYCT_Pos              (20U)                                          /**< (USBHS_DEVEPTISR) Byte Count Position */
#define USBHS_DEVEPTISR_BYCT_Msk              (0x7FFU << USBHS_DEVEPTISR_BYCT_Pos)           /**< (USBHS_DEVEPTISR) Byte Count Mask */
#define USBHS_DEVEPTISR_BYCT(value)           (USBHS_DEVEPTISR_BYCT_Msk & ((value) << USBHS_DEVEPTISR_BYCT_Pos))
#define USBHS_DEVEPTISR_Msk                   (0x7FF7F7FFUL)                                 /**< (USBHS_DEVEPTISR) Register Mask  */

/* -------- USBHS_DEVEPTICR : (USBHS Offset: 0x160) ( /W 32) Device Endpoint Interrupt Clear Register -------- */
#define USBHS_DEVEPTICR_TXINIC_Pos            (0U)                                           /**< (USBHS_DEVEPTICR) Transmitted IN Data Interrupt Clear Position */
#define USBHS_DEVEPTICR_TXINIC_Msk            (0x1U << USBHS_DEVEPTICR_TXINIC_Pos)           /**< (USBHS_DEVEPTICR) Transmitted IN Data Interrupt Clear Mask */
#define USBHS_DEVEPTICR_TXINIC(value)         (USBHS_DEVEPTICR_TXINIC_Msk & ((value) << USBHS_DEVEPTICR_TXINIC_Pos))
#define USBHS_DEVEPTICR_RXOUTIC_Pos           (1U)                                           /**< (USBHS_DEVEPTICR) Received OUT Data Interrupt Clear Position */
#define USBHS_DEVEPTICR_RXOUTIC_Msk           (0x1U << USBHS_DEVEPTICR_RXOUTIC_Pos)          /**< (USBHS_DEVEPTICR) Received OUT Data Interrupt Clear Mask */
#define USBHS_DEVEPTICR_RXOUTIC(value)        (USBHS_DEVEPTICR_RXOUTIC_Msk & ((value) << USBHS_DEVEPTICR_RXOUTIC_Pos))
#define USBHS_DEVEPTICR_RXSTPIC_Pos           (2U)                                           /**< (USBHS_DEVEPTICR) Received SETUP Interrupt Clear Position */
#define USBHS_DEVEPTICR_RXSTPIC_Msk           (0x1U << USBHS_DEVEPTICR_RXSTPIC_Pos)          /**< (USBHS_DEVEPTICR) Received SETUP Interrupt Clear Mask */
#define USBHS_DEVEPTICR_RXSTPIC(value)        (USBHS_DEVEPTICR_RXSTPIC_Msk & ((value) << USBHS_DEVEPTICR_RXSTPIC_Pos))
#define USBHS_DEVEPTICR_UNDERFIC_Pos          (2U)                                           /**< (USBHS_DEVEPTICR) Underflow Interrupt Clear Position */
#define USBHS_DEVEPTICR_UNDERFIC_Msk          (0x1U << USBHS_DEVEPTICR_UNDERFIC_Pos)         /**< (USBHS_DEVEPTICR) Underflow Interrupt Clear Mask */
#define USBHS_DEVEPTICR_UNDERFIC(value)       (USBHS_DEVEPTICR_UNDERFIC_Msk & ((value) << USBHS_DEVEPTICR_UNDERFIC_Pos))
#define USBHS_DEVEPTICR_NAKOUTIC_Pos          (3U)                                           /**< (USBHS_DEVEPTICR) NAKed OUT Interrupt Clear Position */
#define USBHS_DEVEPTICR_NAKOUTIC_Msk          (0x1U << USBHS_DEVEPTICR_NAKOUTIC_Pos)         /**< (USBHS_DEVEPTICR) NAKed OUT Interrupt Clear Mask */
#define USBHS_DEVEPTICR_NAKOUTIC(value)       (USBHS_DEVEPTICR_NAKOUTIC_Msk & ((value) << USBHS_DEVEPTICR_NAKOUTIC_Pos))
#define USBHS_DEVEPTICR_HBISOINERRIC_Pos      (3U)                                           /**< (USBHS_DEVEPTICR) High Bandwidth Isochronous IN Underflow Error Interrupt Clear Position */
#define USBHS_DEVEPTICR_HBISOINERRIC_Msk      (0x1U << USBHS_DEVEPTICR_HBISOINERRIC_Pos)     /**< (USBHS_DEVEPTICR) High Bandwidth Isochronous IN Underflow Error Interrupt Clear Mask */
#define USBHS_DEVEPTICR_HBISOINERRIC(value)   (USBHS_DEVEPTICR_HBISOINERRIC_Msk & ((value) << USBHS_DEVEPTICR_HBISOINERRIC_Pos))
#define USBHS_DEVEPTICR_NAKINIC_Pos           (4U)                                           /**< (USBHS_DEVEPTICR) NAKed IN Interrupt Clear Position */
#define USBHS_DEVEPTICR_NAKINIC_Msk           (0x1U << USBHS_DEVEPTICR_NAKINIC_Pos)          /**< (USBHS_DEVEPTICR) NAKed IN Interrupt Clear Mask */
#define USBHS_DEVEPTICR_NAKINIC(value)        (USBHS_DEVEPTICR_NAKINIC_Msk & ((value) << USBHS_DEVEPTICR_NAKINIC_Pos))
#define USBHS_DEVEPTICR_HBISOFLUSHIC_Pos      (4U)                                           /**< (USBHS_DEVEPTICR) High Bandwidth Isochronous IN Flush Interrupt Clear Position */
#define USBHS_DEVEPTICR_HBISOFLUSHIC_Msk      (0x1U << USBHS_DEVEPTICR_HBISOFLUSHIC_Pos)     /**< (USBHS_DEVEPTICR) High Bandwidth Isochronous IN Flush Interrupt Clear Mask */
#define USBHS_DEVEPTICR_HBISOFLUSHIC(value)   (USBHS_DEVEPTICR_HBISOFLUSHIC_Msk & ((value) << USBHS_DEVEPTICR_HBISOFLUSHIC_Pos))
#define USBHS_DEVEPTICR_OVERFIC_Pos           (5U)                                           /**< (USBHS_DEVEPTICR) Overflow Interrupt Clear Position */
#define USBHS_DEVEPTICR_OVERFIC_Msk           (0x1U << USBHS_DEVEPTICR_OVERFIC_Pos)          /**< (USBHS_DEVEPTICR) Overflow Interrupt Clear Mask */
#define USBHS_DEVEPTICR_OVERFIC(value)        (USBHS_DEVEPTICR_OVERFIC_Msk & ((value) << USBHS_DEVEPTICR_OVERFIC_Pos))
#define USBHS_DEVEPTICR_STALLEDIC_Pos         (6U)                                           /**< (USBHS_DEVEPTICR) STALLed Interrupt Clear Position */
#define USBHS_DEVEPTICR_STALLEDIC_Msk         (0x1U << USBHS_DEVEPTICR_STALLEDIC_Pos)        /**< (USBHS_DEVEPTICR) STALLed Interrupt Clear Mask */
#define USBHS_DEVEPTICR_STALLEDIC(value)      (USBHS_DEVEPTICR_STALLEDIC_Msk & ((value) << USBHS_DEVEPTICR_STALLEDIC_Pos))
#define USBHS_DEVEPTICR_CRCERRIC_Pos          (6U)                                           /**< (USBHS_DEVEPTICR) CRC Error Interrupt Clear Position */
#define USBHS_DEVEPTICR_CRCERRIC_Msk          (0x1U << USBHS_DEVEPTICR_CRCERRIC_Pos)         /**< (USBHS_DEVEPTICR) CRC Error Interrupt Clear Mask */
#define USBHS_DEVEPTICR_CRCERRIC(value)       (USBHS_DEVEPTICR_CRCERRIC_Msk & ((value) << USBHS_DEVEPTICR_CRCERRIC_Pos))
#define USBHS_DEVEPTICR_SHORTPACKETC_Pos      (7U)                                           /**< (USBHS_DEVEPTICR) Short Packet Interrupt Clear Position */
#define USBHS_DEVEPTICR_SHORTPACKETC_Msk      (0x1U << USBHS_DEVEPTICR_SHORTPACKETC_Pos)     /**< (USBHS_DEVEPTICR) Short Packet Interrupt Clear Mask */
#define USBHS_DEVEPTICR_SHORTPACKETC(value)   (USBHS_DEVEPTICR_SHORTPACKETC_Msk & ((value) << USBHS_DEVEPTICR_SHORTPACKETC_Pos))
#define USBHS_DEVEPTICR_Msk                   (0x000000FFUL)                                 /**< (USBHS_DEVEPTICR) Register Mask  */

/* -------- USBHS_DEVEPTIFR : (USBHS Offset: 0x190) ( /W 32) Device Endpoint Interrupt Set Register -------- */
#define USBHS_DEVEPTIFR_TXINIS_Pos            (0U)                                           /**< (USBHS_DEVEPTIFR) Transmitted IN Data Interrupt Set Position */
#define USBHS_DEVEPTIFR_TXINIS_Msk            (0x1U << USBHS_DEVEPTIFR_TXINIS_Pos)           /**< (USBHS_DEVEPTIFR) Transmitted IN Data Interrupt Set Mask */
#define USBHS_DEVEPTIFR_TXINIS(value)         (USBHS_DEVEPTIFR_TXINIS_Msk & ((value) << USBHS_DEVEPTIFR_TXINIS_Pos))
#define USBHS_DEVEPTIFR_RXOUTIS_Pos           (1U)                                           /**< (USBHS_DEVEPTIFR) Received OUT Data Interrupt Set Position */
#define USBHS_DEVEPTIFR_RXOUTIS_Msk           (0x1U << USBHS_DEVEPTIFR_RXOUTIS_Pos)          /**< (USBHS_DEVEPTIFR) Received OUT Data Interrupt Set Mask */
#define USBHS_DEVEPTIFR_RXOUTIS(value)        (USBHS_DEVEPTIFR_RXOUTIS_Msk & ((value) << USBHS_DEVEPTIFR_RXOUTIS_Pos))
#define USBHS_DEVEPTIFR_RXSTPIS_Pos           (2U)                                           /**< (USBHS_DEVEPTIFR) Received SETUP Interrupt Set Position */
#define USBHS_DEVEPTIFR_RXSTPIS_Msk           (0x1U << USBHS_DEVEPTIFR_RXSTPIS_Pos)          /**< (USBHS_DEVEPTIFR) Received SETUP Interrupt Set Mask */
#define USBHS_DEVEPTIFR_RXSTPIS(value)        (USBHS_DEVEPTIFR_RXSTPIS_Msk & ((value) << USBHS_DEVEPTIFR_RXSTPIS_Pos))
#define USBHS_DEVEPTIFR_UNDERFIS_Pos          (2U)                                           /**< (USBHS_DEVEPTIFR) Underflow Interrupt Set Position */
#define USBHS_DEVEPTIFR_UNDERFIS_Msk          (0x1U << USBHS_DEVEPTIFR_UNDERFIS_Pos)         /**< (USBHS_DEVEPTIFR) Underflow Interrupt Set Mask */
#define USBHS_DEVEPTIFR_UNDERFIS(value)       (USBHS_DEVEPTIFR_UNDERFIS_Msk & ((value) << USBHS_DEVEPTIFR_UNDERFIS_Pos))
#define USBHS_DEVEPTIFR_NAKOUTIS_Pos          (3U)                                           /**< (USBHS_DEVEPTIFR) NAKed OUT Interrupt Set Position */
#define USBHS_DEVEPTIFR_NAKOUTIS_Msk          (0x1U << USBHS_DEVEPTIFR_NAKOUTIS_Pos)         /**< (USBHS_DEVEPTIFR) NAKed OUT Interrupt Set Mask */
#define USBHS_DEVEPTIFR_NAKOUTIS(value)       (USBHS_DEVEPTIFR_NAKOUTIS_Msk & ((value) << USBHS_DEVEPTIFR_NAKOUTIS_Pos))
#define USBHS_DEVEPTIFR_HBISOINERRIS_Pos      (3U)                                           /**< (USBHS_DEVEPTIFR) High Bandwidth Isochronous IN Underflow Error Interrupt Set Position */
#define USBHS_DEVEPTIFR_HBISOINERRIS_Msk      (0x1U << USBHS_DEVEPTIFR_HBISOINERRIS_Pos)     /**< (USBHS_DEVEPTIFR) High Bandwidth Isochronous IN Underflow Error Interrupt Set Mask */
#define USBHS_DEVEPTIFR_HBISOINERRIS(value)   (USBHS_DEVEPTIFR_HBISOINERRIS_Msk & ((value) << USBHS_DEVEPTIFR_HBISOINERRIS_Pos))
#define USBHS_DEVEPTIFR_NAKINIS_Pos           (4U)                                           /**< (USBHS_DEVEPTIFR) NAKed IN Interrupt Set Position */
#define USBHS_DEVEPTIFR_NAKINIS_Msk           (0x1U << USBHS_DEVEPTIFR_NAKINIS_Pos)          /**< (USBHS_DEVEPTIFR) NAKed IN Interrupt Set Mask */
#define USBHS_DEVEPTIFR_NAKINIS(value)        (USBHS_DEVEPTIFR_NAKINIS_Msk & ((value) << USBHS_DEVEPTIFR_NAKINIS_Pos))
#define USBHS_DEVEPTIFR_HBISOFLUSHIS_Pos      (4U)                                           /**< (USBHS_DEVEPTIFR) High Bandwidth Isochronous IN Flush Interrupt Set Position */
#define USBHS_DEVEPTIFR_HBISOFLUSHIS_Msk      (0x1U << USBHS_DEVEPTIFR_HBISOFLUSHIS_Pos)     /**< (USBHS_DEVEPTIFR) High Bandwidth Isochronous IN Flush Interrupt Set Mask */
#define USBHS_DEVEPTIFR_HBISOFLUSHIS(value)   (USBHS_DEVEPTIFR_HBISOFLUSHIS_Msk & ((value) << USBHS_DEVEPTIFR_HBISOFLUSHIS_Pos))
#define USBHS_DEVEPTIFR_OVERFIS_Pos           (5U)                                           /**< (USBHS_DEVEPTIFR) Overflow Interrupt Set Position */
#define USBHS_DEVEPTIFR_OVERFIS_Msk           (0x1U << USBHS_DEVEPTIFR_OVERFIS_Pos)          /**< (USBHS_DEVEPTIFR) Overflow Interrupt Set Mask */
#define USBHS_DEVEPTIFR_OVERFIS(value)        (USBHS_DEVEPTIFR_OVERFIS_Msk & ((value) << USBHS_DEVEPTIFR_OVERFIS_Pos))
#define USBHS_DEVEPTIFR_STALLEDIS_Pos         (6U)                                           /**< (USBHS_DEVEPTIFR) STALLed Interrupt Set Position */
#define USBHS_DEVEPTIFR_STALLEDIS_Msk         (0x1U << USBHS_DEVEPTIFR_STALLEDIS_Pos)        /**< (USBHS_DEVEPTIFR) STALLed Interrupt Set Mask */
#define USBHS_DEVEPTIFR_STALLEDIS(value)      (USBHS_DEVEPTIFR_STALLEDIS_Msk & ((value) << USBHS_DEVEPTIFR_STALLEDIS_Pos))
#define USBHS_DEVEPTIFR_CRCERRIS_Pos          (6U)                                           /**< (USBHS_DEVEPTIFR) CRC Error Interrupt Set Position */
#define USBHS_DEVEPTIFR_CRCERRIS_Msk          (0x1U << USBHS_DEVEPTIFR_CRCERRIS_Pos)         /**< (USBHS_DEVEPTIFR) CRC Error Interrupt Set Mask */
#define USBHS_DEVEPTIFR_CRCERRIS(value)       (USBHS_DEVEPTIFR_CRCERRIS_Msk & ((value) << USBHS_DEVEPTIFR_CRCERRIS_Pos))
#define USBHS_DEVEPTIFR_SHORTPACKETS_Pos      (7U)                                           /**< (USBHS_DEVEPTIFR) Short Packet Interrupt Set Position */
#define USBHS_DEVEPTIFR_SHORTPACKETS_Msk      (0x1U << USBHS_DEVEPTIFR_SHORTPACKETS_Pos)     /**< (USBHS_DEVEPTIFR) Short Packet Interrupt Set Mask */
#define USBHS_DEVEPTIFR_SHORTPACKETS(value)   (USBHS_DEVEPTIFR_SHORTPACKETS_Msk & ((value) << USBHS_DEVEPTIFR_SHORTPACKETS_Pos))
#define USBHS_DEVEPTIFR_NBUSYBKS_Pos          (12U)                                          /**< (USBHS_DEVEPTIFR) Number of Busy Banks Interrupt Set Position */
#define USBHS_DEVEPTIFR_NBUSYBKS_Msk          (0x1U << USBHS_DEVEPTIFR_NBUSYBKS_Pos)         /**< (USBHS_DEVEPTIFR) Number of Busy Banks Interrupt Set Mask */
#define USBHS_DEVEPTIFR_NBUSYBKS(value)       (USBHS_DEVEPTIFR_NBUSYBKS_Msk & ((value) << USBHS_DEVEPTIFR_NBUSYBKS_Pos))
#define USBHS_DEVEPTIFR_Msk                   (0x000010FFUL)                                 /**< (USBHS_DEVEPTIFR) Register Mask  */

/* -------- USBHS_DEVEPTIMR : (USBHS Offset: 0x1C0) (R/  32) Device Endpoint Interrupt Mask Register -------- */
#define USBHS_DEVEPTIMR_TXINE_Pos             (0U)                                           /**< (USBHS_DEVEPTIMR) Transmitted IN Data Interrupt Position */
#define USBHS_DEVEPTIMR_TXINE_Msk             (0x1U << USBHS_DEVEPTIMR_TXINE_Pos)            /**< (USBHS_DEVEPTIMR) Transmitted IN Data Interrupt Mask */
#define USBHS_DEVEPTIMR_TXINE(value)          (USBHS_DEVEPTIMR_TXINE_Msk & ((value) << USBHS_DEVEPTIMR_TXINE_Pos))
#define USBHS_DEVEPTIMR_RXOUTE_Pos            (1U)                                           /**< (USBHS_DEVEPTIMR) Received OUT Data Interrupt Position */
#define USBHS_DEVEPTIMR_RXOUTE_Msk            (0x1U << USBHS_DEVEPTIMR_RXOUTE_Pos)           /**< (USBHS_DEVEPTIMR) Received OUT Data Interrupt Mask */
#define USBHS_DEVEPTIMR_RXOUTE(value)         (USBHS_DEVEPTIMR_RXOUTE_Msk & ((value) << USBHS_DEVEPTIMR_RXOUTE_Pos))
#define USBHS_DEVEPTIMR_RXSTPE_Pos            (2U)                                           /**< (USBHS_DEVEPTIMR) Received SETUP Interrupt Position */
#define USBHS_DEVEPTIMR_RXSTPE_Msk            (0x1U << USBHS_DEVEPTIMR_RXSTPE_Pos)           /**< (USBHS_DEVEPTIMR) Received SETUP Interrupt Mask */
#define USBHS_DEVEPTIMR_RXSTPE(value)         (USBHS_DEVEPTIMR_RXSTPE_Msk & ((value) << USBHS_DEVEPTIMR_RXSTPE_Pos))
#define USBHS_DEVEPTIMR_UNDERFE_Pos           (2U)                                           /**< (USBHS_DEVEPTIMR) Underflow Interrupt Position */
#define USBHS_DEVEPTIMR_UNDERFE_Msk           (0x1U << USBHS_DEVEPTIMR_UNDERFE_Pos)          /**< (USBHS_DEVEPTIMR) Underflow Interrupt Mask */
#define USBHS_DEVEPTIMR_UNDERFE(value)        (USBHS_DEVEPTIMR_UNDERFE_Msk & ((value) << USBHS_DEVEPTIMR_UNDERFE_Pos))
#define USBHS_DEVEPTIMR_NAKOUTE_Pos           (3U)                                           /**< (USBHS_DEVEPTIMR) NAKed OUT Interrupt Position */
#define USBHS_DEVEPTIMR_NAKOUTE_Msk           (0x1U << USBHS_DEVEPTIMR_NAKOUTE_Pos)          /**< (USBHS_DEVEPTIMR) NAKed OUT Interrupt Mask */
#define USBHS_DEVEPTIMR_NAKOUTE(value)        (USBHS_DEVEPTIMR_NAKOUTE_Msk & ((value) << USBHS_DEVEPTIMR_NAKOUTE_Pos))
#define USBHS_DEVEPTIMR_HBISOINERRE_Pos       (3U)                                           /**< (USBHS_DEVEPTIMR) High Bandwidth Isochronous IN Underflow Error Interrupt Position */
#define USBHS_DEVEPTIMR_HBISOINERRE_Msk       (0x1U << USBHS_DEVEPTIMR_HBISOINERRE_Pos)      /**< (USBHS_DEVEPTIMR) High Bandwidth Isochronous IN Underflow Error Interrupt Mask */
#define USBHS_DEVEPTIMR_HBISOINERRE(value)    (USBHS_DEVEPTIMR_HBISOINERRE_Msk & ((value) << USBHS_DEVEPTIMR_HBISOINERRE_Pos))
#define USBHS_DEVEPTIMR_NAKINE_Pos            (4U)                                           /**< (USBHS_DEVEPTIMR) NAKed IN Interrupt Position */
#define USBHS_DEVEPTIMR_NAKINE_Msk            (0x1U << USBHS_DEVEPTIMR_NAKINE_Pos)           /**< (USBHS_DEVEPTIMR) NAKed IN Interrupt Mask */
#define USBHS_DEVEPTIMR_NAKINE(value)         (USBHS_DEVEPTIMR_NAKINE_Msk & ((value) << USBHS_DEVEPTIMR_NAKINE_Pos))
#define USBHS_DEVEPTIMR_HBISOFLUSHE_Pos       (4U)                                           /**< (USBHS_DEVEPTIMR) High Bandwidth Isochronous IN Flush Interrupt Position */
#define USBHS_DEVEPTIMR_HBISOFLUSHE_Msk       (0x1U << USBHS_DEVEPTIMR_HBISOFLUSHE_Pos)      /**< (USBHS_DEVEPTIMR) High Bandwidth Isochronous IN Flush Interrupt Mask */
#define USBHS_DEVEPTIMR_HBISOFLUSHE(value)    (USBHS_DEVEPTIMR_HBISOFLUSHE_Msk & ((value) << USBHS_DEVEPTIMR_HBISOFLUSHE_Pos))
#define USBHS_DEVEPTIMR_OVERFE_Pos            (5U)                                           /**< (USBHS_DEVEPTIMR) Overflow Interrupt Position */
#define USBHS_DEVEPTIMR_OVERFE_Msk            (0x1U << USBHS_DEVEPTIMR_OVERFE_Pos)           /**< (USBHS_DEVEPTIMR) Overflow Interrupt Mask */
#define USBHS_DEVEPTIMR_OVERFE(value)         (USBHS_DEVEPTIMR_OVERFE_Msk & ((value) << USBHS_DEVEPTIMR_OVERFE_Pos))
#define USBHS_DEVEPTIMR_STALLEDE_Pos          (6U)                                           /**< (USBHS_DEVEPTIMR) STALLed Interrupt Position */
#define USBHS_DEVEPTIMR_STALLEDE_Msk          (0x1U << USBHS_DEVEPTIMR_STALLEDE_Pos)         /**< (USBHS_DEVEPTIMR) STALLed Interrupt Mask */
#define USBHS_DEVEPTIMR_STALLEDE(value)       (USBHS_DEVEPTIMR_STALLEDE_Msk & ((value) << USBHS_DEVEPTIMR_STALLEDE_Pos))
#define USBHS_DEVEPTIMR_CRCERRE_Pos           (6U)                                           /**< (USBHS_DEVEPTIMR) CRC Error Interrupt Position */
#define USBHS_DEVEPTIMR_CRCERRE_Msk           (0x1U << USBHS_DEVEPTIMR_CRCERRE_Pos)          /**< (USBHS_DEVEPTIMR) CRC Error Interrupt Mask */
#define USBHS_DEVEPTIMR_CRCERRE(value)        (USBHS_DEVEPTIMR_CRCERRE_Msk & ((value) << USBHS_DEVEPTIMR_CRCERRE_Pos))
#define USBHS_DEVEPTIMR_SHORTPACKETE_Pos      (7U)                                           /**< (USBHS_DEVEPTIMR) Short Packet Interrupt Position */
#define USBHS_DEVEPTIMR_SHORTPACKETE_Msk      (0x1U << USBHS_DEVEPTIMR_SHORTPACKETE_Pos)     /**< (USBHS_DEVEPTIMR) Short Packet Interrupt Mask */
#define USBHS_DEVEPTIMR_SHORTPACKETE(value)   (USBHS_DEVEPTIMR_SHORTPACKETE_Msk & ((value) << USBHS_DEVEPTIMR_SHORTPACKETE_Pos))
#define USBHS_DEVEPTIMR_MDATAE_Pos            (8U)                                           /**< (USBHS_DEVEPTIMR) MData Interrupt Position */
#define USBHS_DEVEPTIMR_MDATAE_Msk            (0x1U << USBHS_DEVEPTIMR_MDATAE_Pos)           /**< (USBHS_DEVEPTIMR) MData Interrupt Mask */
#define USBHS_DEVEPTIMR_MDATAE(value)         (USBHS_DEVEPTIMR_MDATAE_Msk & ((value) << USBHS_DEVEPTIMR_MDATAE_Pos))
#define USBHS_DEVEPTIMR_DATAXE_Pos            (9U)                                           /**< (USBHS_DEVEPTIMR) DataX Interrupt Position */
#define USBHS_DEVEPTIMR_DATAXE_Msk            (0x1U << USBHS_DEVEPTIMR_DATAXE_Pos)           /**< (USBHS_DEVEPTIMR) DataX Interrupt Mask */
#define USBHS_DEVEPTIMR_DATAXE(value)         (USBHS_DEVEPTIMR_DATAXE_Msk & ((value) << USBHS_DEVEPTIMR_DATAXE_Pos))
#define USBHS_DEVEPTIMR_ERRORTRANSE_Pos       (10U)                                          /**< (USBHS_DEVEPTIMR) Transaction Error Interrupt Position */
#define USBHS_DEVEPTIMR_ERRORTRANSE_Msk       (0x1U << USBHS_DEVEPTIMR_ERRORTRANSE_Pos)      /**< (USBHS_DEVEPTIMR) Transaction Error Interrupt Mask */
#define USBHS_DEVEPTIMR_ERRORTRANSE(value)    (USBHS_DEVEPTIMR_ERRORTRANSE_Msk & ((value) << USBHS_DEVEPTIMR_ERRORTRANSE_Pos))
#define USBHS_DEVEPTIMR_NBUSYBKE_Pos          (12U)                                          /**< (USBHS_DEVEPTIMR) Number of Busy Banks Interrupt Position */
#define USBHS_DEVEPTIMR_NBUSYBKE_Msk          (0x1U << USBHS_DEVEPTIMR_NBUSYBKE_Pos)         /**< (USBHS_DEVEPTIMR) Number of Busy Banks Interrupt Mask */
#define USBHS_DEVEPTIMR_NBUSYBKE(value)       (USBHS_DEVEPTIMR_NBUSYBKE_Msk & ((value) << USBHS_DEVEPTIMR_NBUSYBKE_Pos))
#define USBHS_DEVEPTIMR_KILLBK_Pos            (13U)                                          /**< (USBHS_DEVEPTIMR) Kill IN Bank Position */
#define USBHS_DEVEPTIMR_KILLBK_Msk            (0x1U << USBHS_DEVEPTIMR_KILLBK_Pos)           /**< (USBHS_DEVEPTIMR) Kill IN Bank Mask */
#define USBHS_DEVEPTIMR_KILLBK(value)         (USBHS_DEVEPTIMR_KILLBK_Msk & ((value) << USBHS_DEVEPTIMR_KILLBK_Pos))
#define USBHS_DEVEPTIMR_FIFOCON_Pos           (14U)                                          /**< (USBHS_DEVEPTIMR) FIFO Control Position */
#define USBHS_DEVEPTIMR_FIFOCON_Msk           (0x1U << USBHS_DEVEPTIMR_FIFOCON_Pos)          /**< (USBHS_DEVEPTIMR) FIFO Control Mask */
#define USBHS_DEVEPTIMR_FIFOCON(value)        (USBHS_DEVEPTIMR_FIFOCON_Msk & ((value) << USBHS_DEVEPTIMR_FIFOCON_Pos))
#define USBHS_DEVEPTIMR_EPDISHDMA_Pos         (16U)                                          /**< (USBHS_DEVEPTIMR) Endpoint Interrupts Disable HDMA Request Position */
#define USBHS_DEVEPTIMR_EPDISHDMA_Msk         (0x1U << USBHS_DEVEPTIMR_EPDISHDMA_Pos)        /**< (USBHS_DEVEPTIMR) Endpoint Interrupts Disable HDMA Request Mask */
#define USBHS_DEVEPTIMR_EPDISHDMA(value)      (USBHS_DEVEPTIMR_EPDISHDMA_Msk & ((value) << USBHS_DEVEPTIMR_EPDISHDMA_Pos))
#define USBHS_DEVEPTIMR_NYETDIS_Pos           (17U)                                          /**< (USBHS_DEVEPTIMR) NYET Token Disable Position */
#define USBHS_DEVEPTIMR_NYETDIS_Msk           (0x1U << USBHS_DEVEPTIMR_NYETDIS_Pos)          /**< (USBHS_DEVEPTIMR) NYET Token Disable Mask */
#define USBHS_DEVEPTIMR_NYETDIS(value)        (USBHS_DEVEPTIMR_NYETDIS_Msk & ((value) << USBHS_DEVEPTIMR_NYETDIS_Pos))
#define USBHS_DEVEPTIMR_RSTDT_Pos             (18U)                                          /**< (USBHS_DEVEPTIMR) Reset Data Toggle Position */
#define USBHS_DEVEPTIMR_RSTDT_Msk             (0x1U << USBHS_DEVEPTIMR_RSTDT_Pos)            /**< (USBHS_DEVEPTIMR) Reset Data Toggle Mask */
#define USBHS_DEVEPTIMR_RSTDT(value)          (USBHS_DEVEPTIMR_RSTDT_Msk & ((value) << USBHS_DEVEPTIMR_RSTDT_Pos))
#define USBHS_DEVEPTIMR_STALLRQ_Pos           (19U)                                          /**< (USBHS_DEVEPTIMR) STALL Request Position */
#define USBHS_DEVEPTIMR_STALLRQ_Msk           (0x1U << USBHS_DEVEPTIMR_STALLRQ_Pos)          /**< (USBHS_DEVEPTIMR) STALL Request Mask */
#define USBHS_DEVEPTIMR_STALLRQ(value)        (USBHS_DEVEPTIMR_STALLRQ_Msk & ((value) << USBHS_DEVEPTIMR_STALLRQ_Pos))
#define USBHS_DEVEPTIMR_Msk                   (0x000F77FFUL)                                 /**< (USBHS_DEVEPTIMR) Register Mask  */

/* -------- USBHS_DEVEPTIER : (USBHS Offset: 0x1F0) ( /W 32) Device Endpoint Interrupt Enable Register -------- */
#define USBHS_DEVEPTIER_TXINES_Pos            (0U)                                           /**< (USBHS_DEVEPTIER) Transmitted IN Data Interrupt Enable Position */
#define USBHS_DEVEPTIER_TXINES_Msk            (0x1U << USBHS_DEVEPTIER_TXINES_Pos)           /**< (USBHS_DEVEPTIER) Transmitted IN Data Interrupt Enable Mask */
#define USBHS_DEVEPTIER_TXINES(value)         (USBHS_DEVEPTIER_TXINES_Msk & ((value) << USBHS_DEVEPTIER_TXINES_Pos))
#define USBHS_DEVEPTIER_RXOUTES_Pos           (1U)                                           /**< (USBHS_DEVEPTIER) Received OUT Data Interrupt Enable Position */
#define USBHS_DEVEPTIER_RXOUTES_Msk           (0x1U << USBHS_DEVEPTIER_RXOUTES_Pos)          /**< (USBHS_DEVEPTIER) Received OUT Data Interrupt Enable Mask */
#define USBHS_DEVEPTIER_RXOUTES(value)        (USBHS_DEVEPTIER_RXOUTES_Msk & ((value) << USBHS_DEVEPTIER_RXOUTES_Pos))
#define USBHS_DEVEPTIER_RXSTPES_Pos           (2U)                                           /**< (USBHS_DEVEPTIER) Received SETUP Interrupt Enable Position */
#define USBHS_DEVEPTIER_RXSTPES_Msk           (0x1U << USBHS_DEVEPTIER_RXSTPES_Pos)          /**< (USBHS_DEVEPTIER) Received SETUP Interrupt Enable Mask */
#define USBHS_DEVEPTIER_RXSTPES(value)        (USBHS_DEVEPTIER_RXSTPES_Msk & ((value) << USBHS_DEVEPTIER_RXSTPES_Pos))
#define USBHS_DEVEPTIER_UNDERFES_Pos          (2U)                                           /**< (USBHS_DEVEPTIER) Underflow Interrupt Enable Position */
#define USBHS_DEVEPTIER_UNDERFES_Msk          (0x1U << USBHS_DEVEPTIER_UNDERFES_Pos)         /**< (USBHS_DEVEPTIER) Underflow Interrupt Enable Mask */
#define USBHS_DEVEPTIER_UNDERFES(value)       (USBHS_DEVEPTIER_UNDERFES_Msk & ((value) << USBHS_DEVEPTIER_UNDERFES_Pos))
#define USBHS_DEVEPTIER_NAKOUTES_Pos          (3U)                                           /**< (USBHS_DEVEPTIER) NAKed OUT Interrupt Enable Position */
#define USBHS_DEVEPTIER_NAKOUTES_Msk          (0x1U << USBHS_DEVEPTIER_NAKOUTES_Pos)         /**< (USBHS_DEVEPTIER) NAKed OUT Interrupt Enable Mask */
#define USBHS_DEVEPTIER_NAKOUTES(value)       (USBHS_DEVEPTIER_NAKOUTES_Msk & ((value) << USBHS_DEVEPTIER_NAKOUTES_Pos))
#define USBHS_DEVEPTIER_HBISOINERRES_Pos      (3U)                                           /**< (USBHS_DEVEPTIER) High Bandwidth Isochronous IN Underflow Error Interrupt Enable Position */
#define USBHS_DEVEPTIER_HBISOINERRES_Msk      (0x1U << USBHS_DEVEPTIER_HBISOINERRES_Pos)     /**< (USBHS_DEVEPTIER) High Bandwidth Isochronous IN Underflow Error Interrupt Enable Mask */
#define USBHS_DEVEPTIER_HBISOINERRES(value)   (USBHS_DEVEPTIER_HBISOINERRES_Msk & ((value) << USBHS_DEVEPTIER_HBISOINERRES_Pos))
#define USBHS_DEVEPTIER_NAKINES_Pos           (4U)                                           /**< (USBHS_DEVEPTIER) NAKed IN Interrupt Enable Position */
#define USBHS_DEVEPTIER_NAKINES_Msk           (0x1U << USBHS_DEVEPTIER_NAKINES_Pos)          /**< (USBHS_DEVEPTIER) NAKed IN Interrupt Enable Mask */
#define USBHS_DEVEPTIER_NAKINES(value)        (USBHS_DEVEPTIER_NAKINES_Msk & ((value) << USBHS_DEVEPTIER_NAKINES_Pos))
#define USBHS_DEVEPTIER_HBISOFLUSHES_Pos      (4U)                                           /**< (USBHS_DEVEPTIER) High Bandwidth Isochronous IN Flush Interrupt Enable Position */
#define USBHS_DEVEPTIER_HBISOFLUSHES_Msk      (0x1U << USBHS_DEVEPTIER_HBISOFLUSHES_Pos)     /**< (USBHS_DEVEPTIER) High Bandwidth Isochronous IN Flush Interrupt Enable Mask */
#define USBHS_DEVEPTIER_HBISOFLUSHES(value)   (USBHS_DEVEPTIER_HBISOFLUSHES_Msk & ((value) << USBHS_DEVEPTIER_HBISOFLUSHES_Pos))
#define USBHS_DEVEPTIER_OVERFES_Pos           (5U)                                           /**< (USBHS_DEVEPTIER) Overflow Interrupt Enable Position */
#define USBHS_DEVEPTIER_OVERFES_Msk           (0x1U << USBHS_DEVEPTIER_OVERFES_Pos)          /**< (USBHS_DEVEPTIER) Overflow Interrupt Enable Mask */
#define USBHS_DEVEPTIER_OVERFES(value)        (USBHS_DEVEPTIER_OVERFES_Msk & ((value) << USBHS_DEVEPTIER_OVERFES_Pos))
#define USBHS_DEVEPTIER_STALLEDES_Pos         (6U)                                           /**< (USBHS_DEVEPTIER) STALLed Interrupt Enable Position */
#define USBHS_DEVEPTIER_STALLEDES_Msk         (0x1U << USBHS_DEVEPTIER_STALLEDES_Pos)        /**< (USBHS_DEVEPTIER) STALLed Interrupt Enable Mask */
#define USBHS_DEVEPTIER_STALLEDES(value)      (USBHS_DEVEPTIER_STALLEDES_Msk & ((value) << USBHS_DEVEPTIER_STALLEDES_Pos))
#define USBHS_DEVEPTIER_CRCERRES_Pos          (6U)                                           /**< (USBHS_DEVEPTIER) CRC Error Interrupt Enable Position */
#define USBHS_DEVEPTIER_CRCERRES_Msk          (0x1U << USBHS_DEVEPTIER_CRCERRES_Pos)         /**< (USBHS_DEVEPTIER) CRC Error Interrupt Enable Mask */
#define USBHS_DEVEPTIER_CRCERRES(value)       (USBHS_DEVEPTIER_CRCERRES_Msk & ((value) << USBHS_DEVEPTIER_CRCERRES_Pos))
#define USBHS_DEVEPTIER_SHORTPACKETES_Pos     (7U)                                           /**< (USBHS_DEVEPTIER) Short Packet Interrupt Enable Position */
#define USBHS_DEVEPTIER_SHORTPACKETES_Msk     (0x1U << USBHS_DEVEPTIER_SHORTPACKETES_Pos)    /**< (USBHS_DEVEPTIER) Short Packet Interrupt Enable Mask */
#define USBHS_DEVEPTIER_SHORTPACKETES(value)  (USBHS_DEVEPTIER_SHORTPACKETES_Msk & ((value) << USBHS_DEVEPTIER_SHORTPACKETES_Pos))
#define USBHS_DEVEPTIER_MDATAES_Pos           (8U)                                           /**< (USBHS_DEVEPTIER) MData Interrupt Enable Position */
#define USBHS_DEVEPTIER_MDATAES_Msk           (0x1U << USBHS_DEVEPTIER_MDATAES_Pos)          /**< (USBHS_DEVEPTIER) MData Interrupt Enable Mask */
#define USBHS_DEVEPTIER_MDATAES(value)        (USBHS_DEVEPTIER_MDATAES_Msk & ((value) << USBHS_DEVEPTIER_MDATAES_Pos))
#define USBHS_DEVEPTIER_DATAXES_Pos           (9U)                                           /**< (USBHS_DEVEPTIER) DataX Interrupt Enable Position */
#define USBHS_DEVEPTIER_DATAXES_Msk           (0x1U << USBHS_DEVEPTIER_DATAXES_Pos)          /**< (USBHS_DEVEPTIER) DataX Interrupt Enable Mask */
#define USBHS_DEVEPTIER_DATAXES(value)        (USBHS_DEVEPTIER_DATAXES_Msk & ((value) << USBHS_DEVEPTIER_DATAXES_Pos))
#define USBHS_DEVEPTIER_ERRORTRANSES_Pos      (10U)                                          /**< (USBHS_DEVEPTIER) Transaction Error Interrupt Enable Position */
#define USBHS_DEVEPTIER_ERRORTRANSES_Msk      (0x1U << USBHS_DEVEPTIER_ERRORTRANSES_Pos)     /**< (USBHS_DEVEPTIER) Transaction Error Interrupt Enable Mask */
#define USBHS_DEVEPTIER_ERRORTRANSES(value)   (USBHS_DEVEPTIER_ERRORTRANSES_Msk & ((value) << USBHS_DEVEPTIER_ERRORTRANSES_Pos))
#define USBHS_DEVEPTIER_NBUSYBKES_Pos         (12U)                                          /**< (USBHS_DEVEPTIER) Number of Busy Banks Interrupt Enable Position */
#define USBHS_DEVEPTIER_NBUSYBKES_Msk         (0x1U << USBHS_DEVEPTIER_NBUSYBKES_Pos)        /**< (USBHS_DEVEPTIER) Number of Busy Banks Interrupt Enable Mask */
#define USBHS_DEVEPTIER_NBUSYBKES(value)      (USBHS_DEVEPTIER_NBUSYBKES_Msk & ((value) << USBHS_DEVEPTIER_NBUSYBKES_Pos))
#define USBHS_DEVEPTIER_KILLBKS_Pos           (13U)                                          /**< (USBHS_DEVEPTIER) Kill IN Bank Position */
#define USBHS_DEVEPTIER_KILLBKS_Msk           (0x1U << USBHS_DEVEPTIER_KILLBKS_Pos)          /**< (USBHS_DEVEPTIER) Kill IN Bank Mask */
#define USBHS_DEVEPTIER_KILLBKS(value)        (USBHS_DEVEPTIER_KILLBKS_Msk & ((value) << USBHS_DEVEPTIER_KILLBKS_Pos))
#define USBHS_DEVEPTIER_FIFOCONS_Pos          (14U)                                          /**< (USBHS_DEVEPTIER) FIFO Control Position */
#define USBHS_DEVEPTIER_FIFOCONS_Msk          (0x1U << USBHS_DEVEPTIER_FIFOCONS_Pos)         /**< (USBHS_DEVEPTIER) FIFO Control Mask */
#define USBHS_DEVEPTIER_FIFOCONS(value)       (USBHS_DEVEPTIER_FIFOCONS_Msk & ((value) << USBHS_DEVEPTIER_FIFOCONS_Pos))
#define USBHS_DEVEPTIER_EPDISHDMAS_Pos        (16U)                                          /**< (USBHS_DEVEPTIER) Endpoint Interrupts Disable HDMA Request Enable Position */
#define USBHS_DEVEPTIER_EPDISHDMAS_Msk        (0x1U << USBHS_DEVEPTIER_EPDISHDMAS_Pos)       /**< (USBHS_DEVEPTIER) Endpoint Interrupts Disable HDMA Request Enable Mask */
#define USBHS_DEVEPTIER_EPDISHDMAS(value)     (USBHS_DEVEPTIER_EPDISHDMAS_Msk & ((value) << USBHS_DEVEPTIER_EPDISHDMAS_Pos))
#define USBHS_DEVEPTIER_NYETDISS_Pos          (17U)                                          /**< (USBHS_DEVEPTIER) NYET Token Disable Enable Position */
#define USBHS_DEVEPTIER_NYETDISS_Msk          (0x1U << USBHS_DEVEPTIER_NYETDISS_Pos)         /**< (USBHS_DEVEPTIER) NYET Token Disable Enable Mask */
#define USBHS_DEVEPTIER_NYETDISS(value)       (USBHS_DEVEPTIER_NYETDISS_Msk & ((value) << USBHS_DEVEPTIER_NYETDISS_Pos))
#define USBHS_DEVEPTIER_RSTDTS_Pos            (18U)                                          /**< (USBHS_DEVEPTIER) Reset Data Toggle Enable Position */
#define USBHS_DEVEPTIER_RSTDTS_Msk            (0x1U << USBHS_DEVEPTIER_RSTDTS_Pos)           /**< (USBHS_DEVEPTIER) Reset Data Toggle Enable Mask */
#define USBHS_DEVEPTIER_RSTDTS(value)         (USBHS_DEVEPTIER_RSTDTS_Msk & ((value) << USBHS_DEVEPTIER_RSTDTS_Pos))
#define USBHS_DEVEPTIER_STALLRQS_Pos          (19U)                                          /**< (USBHS_DEVEPTIER) STALL Request Enable Position */
#define USBHS_DEVEPTIER_STALLRQS_Msk          (0x1U << USBHS_DEVEPTIER_STALLRQS_Pos)         /**< (USBHS_DEVEPTIER) STALL Request Enable Mask */
#define USBHS_DEVEPTIER_STALLRQS(value)       (USBHS_DEVEPTIER_STALLRQS_Msk & ((value) << USBHS_DEVEPTIER_STALLRQS_Pos))
#define USBHS_DEVEPTIER_Msk                   (0x000F77FFUL)                                 /**< (USBHS_DEVEPTIER) Register Mask  */

/* -------- USBHS_DEVEPTIDR : (USBHS Offset: 0x220) ( /W 32) Device Endpoint Interrupt Disable Register -------- */
#define USBHS_DEVEPTIDR_TXINEC_Pos            (0U)                                           /**< (USBHS_DEVEPTIDR) Transmitted IN Interrupt Clear Position */
#define USBHS_DEVEPTIDR_TXINEC_Msk            (0x1U << USBHS_DEVEPTIDR_TXINEC_Pos)           /**< (USBHS_DEVEPTIDR) Transmitted IN Interrupt Clear Mask */
#define USBHS_DEVEPTIDR_TXINEC(value)         (USBHS_DEVEPTIDR_TXINEC_Msk & ((value) << USBHS_DEVEPTIDR_TXINEC_Pos))
#define USBHS_DEVEPTIDR_RXOUTEC_Pos           (1U)                                           /**< (USBHS_DEVEPTIDR) Received OUT Data Interrupt Clear Position */
#define USBHS_DEVEPTIDR_RXOUTEC_Msk           (0x1U << USBHS_DEVEPTIDR_RXOUTEC_Pos)          /**< (USBHS_DEVEPTIDR) Received OUT Data Interrupt Clear Mask */
#define USBHS_DEVEPTIDR_RXOUTEC(value)        (USBHS_DEVEPTIDR_RXOUTEC_Msk & ((value) << USBHS_DEVEPTIDR_RXOUTEC_Pos))
#define USBHS_DEVEPTIDR_RXSTPEC_Pos           (2U)                                           /**< (USBHS_DEVEPTIDR) Received SETUP Interrupt Clear Position */
#define USBHS_DEVEPTIDR_RXSTPEC_Msk           (0x1U << USBHS_DEVEPTIDR_RXSTPEC_Pos)          /**< (USBHS_DEVEPTIDR) Received SETUP Interrupt Clear Mask */
#define USBHS_DEVEPTIDR_RXSTPEC(value)        (USBHS_DEVEPTIDR_RXSTPEC_Msk & ((value) << USBHS_DEVEPTIDR_RXSTPEC_Pos))
#define USBHS_DEVEPTIDR_UNDERFEC_Pos          (2U)                                           /**< (USBHS_DEVEPTIDR) Underflow Interrupt Clear Position */
#define USBHS_DEVEPTIDR_UNDERFEC_Msk          (0x1U << USBHS_DEVEPTIDR_UNDERFEC_Pos)         /**< (USBHS_DEVEPTIDR) Underflow Interrupt Clear Mask */
#define USBHS_DEVEPTIDR_UNDERFEC(value)       (USBHS_DEVEPTIDR_UNDERFEC_Msk & ((value) << USBHS_DEVEPTIDR_UNDERFEC_Pos))
#define USBHS_DEVEPTIDR_NAKOUTEC_Pos          (3U)                                           /**< (USBHS_DEVEPTIDR) NAKed OUT Interrupt Clear Position */
#define USBHS_DEVEPTIDR_NAKOUTEC_Msk          (0x1U << USBHS_DEVEPTIDR_NAKOUTEC_Pos)         /**< (USBHS_DEVEPTIDR) NAKed OUT Interrupt Clear Mask */
#define USBHS_DEVEPTIDR_NAKOUTEC(value)       (USBHS_DEVEPTIDR_NAKOUTEC_Msk & ((value) << USBHS_DEVEPTIDR_NAKOUTEC_Pos))
#define USBHS_DEVEPTIDR_HBISOINERREC_Pos      (3U)                                           /**< (USBHS_DEVEPTIDR) High Bandwidth Isochronous IN Underflow Error Interrupt Clear Position */
#define USBHS_DEVEPTIDR_HBISOINERREC_Msk      (0x1U << USBHS_DEVEPTIDR_HBISOINERREC_Pos)     /**< (USBHS_DEVEPTIDR) High Bandwidth Isochronous IN Underflow Error Interrupt Clear Mask */
#define USBHS_DEVEPTIDR_HBISOINERREC(value)   (USBHS_DEVEPTIDR_HBISOINERREC_Msk & ((value) << USBHS_DEVEPTIDR_HBISOINERREC_Pos))
#define USBHS_DEVEPTIDR_NAKINEC_Pos           (4U)                                           /**< (USBHS_DEVEPTIDR) NAKed IN Interrupt Clear Position */
#define USBHS_DEVEPTIDR_NAKINEC_Msk           (0x1U << USBHS_DEVEPTIDR_NAKINEC_Pos)          /**< (USBHS_DEVEPTIDR) NAKed IN Interrupt Clear Mask */
#define USBHS_DEVEPTIDR_NAKINEC(value)        (USBHS_DEVEPTIDR_NAKINEC_Msk & ((value) << USBHS_DEVEPTIDR_NAKINEC_Pos))
#define USBHS_DEVEPTIDR_HBISOFLUSHEC_Pos      (4U)                                           /**< (USBHS_DEVEPTIDR) High Bandwidth Isochronous IN Flush Interrupt Clear Position */
#define USBHS_DEVEPTIDR_HBISOFLUSHEC_Msk      (0x1U << USBHS_DEVEPTIDR_HBISOFLUSHEC_Pos)     /**< (USBHS_DEVEPTIDR) High Bandwidth Isochronous IN Flush Interrupt Clear Mask */
#define USBHS_DEVEPTIDR_HBISOFLUSHEC(value)   (USBHS_DEVEPTIDR_HBISOFLUSHEC_Msk & ((value) << USBHS_DEVEPTIDR_HBISOFLUSHEC_Pos))
#define USBHS_DEVEPTIDR_OVERFEC_Pos           (5U)                                           /**< (USBHS_DEVEPTIDR) Overflow Interrupt Clear Position */
#define USBHS_DEVEPTIDR_OVERFEC_Msk           (0x1U << USBHS_DEVEPTIDR_OVERFEC_Pos)          /**< (USBHS_DEVEPTIDR) Overflow Interrupt Clear Mask */
#define USBHS_DEVEPTIDR_OVERFEC(value)        (USBHS_DEVEPTIDR_OVERFEC_Msk & ((value) << USBHS_DEVEPTIDR_OVERFEC_Pos))
#define USBHS_DEVEPTIDR_STALLEDEC_Pos         (6U)                                           /**< (USBHS_DEVEPTIDR) STALLed Interrupt Clear Position */
#define USBHS_DEVEPTIDR_STALLEDEC_Msk         (0x1U << USBHS_DEVEPTIDR_STALLEDEC_Pos)        /**< (USBHS_DEVEPTIDR) STALLed Interrupt Clear Mask */
#define USBHS_DEVEPTIDR_STALLEDEC(value)      (USBHS_DEVEPTIDR_STALLEDEC_Msk & ((value) << USBHS_DEVEPTIDR_STALLEDEC_Pos))
#define USBHS_DEVEPTIDR_SHORTPACKETEC_Pos     (7U)                                           /**< (USBHS_DEVEPTIDR) Shortpacket Interrupt Clear Position */
#define USBHS_DEVEPTIDR_SHORTPACKETEC_Msk     (0x1U << USBHS_DEVEPTIDR_SHORTPACKETEC_Pos)    /**< (USBHS_DEVEPTIDR) Shortpacket Interrupt Clear Mask */
#define USBHS_DEVEPTIDR_SHORTPACKETEC(value)  (USBHS_DEVEPTIDR_SHORTPACKETEC_Msk & ((value) << USBHS_DEVEPTIDR_SHORTPACKETEC_Pos))
#define USBHS_DEVEPTIDR_MDATAEC_Pos           (8U)                                           /**< (USBHS_DEVEPTIDR) MData Interrupt Clear Position */
#define USBHS_DEVEPTIDR_MDATAEC_Msk           (0x1U << USBHS_DEVEPTIDR_MDATAEC_Pos)          /**< (USBHS_DEVEPTIDR) MData Interrupt Clear Mask */
#define USBHS_DEVEPTIDR_MDATAEC(value)        (USBHS_DEVEPTIDR_MDATAEC_Msk & ((value) << USBHS_DEVEPTIDR_MDATAEC_Pos))
#define USBHS_DEVEPTIDR_DATAXEC_Pos           (9U)                                           /**< (USBHS_DEVEPTIDR) DataX Interrupt Clear Position */
#define USBHS_DEVEPTIDR_DATAXEC_Msk           (0x1U << USBHS_DEVEPTIDR_DATAXEC_Pos)          /**< (USBHS_DEVEPTIDR) DataX Interrupt Clear Mask */
#define USBHS_DEVEPTIDR_DATAXEC(value)        (USBHS_DEVEPTIDR_DATAXEC_Msk & ((value) << USBHS_DEVEPTIDR_DATAXEC_Pos))
#define USBHS_DEVEPTIDR_ERRORTRANSEC_Pos      (10U)                                          /**< (USBHS_DEVEPTIDR) Transaction Error Interrupt Clear Position */
#define USBHS_DEVEPTIDR_ERRORTRANSEC_Msk      (0x1U << USBHS_DEVEPTIDR_ERRORTRANSEC_Pos)     /**< (USBHS_DEVEPTIDR) Transaction Error Interrupt Clear Mask */
#define USBHS_DEVEPTIDR_ERRORTRANSEC(value)   (USBHS_DEVEPTIDR_ERRORTRANSEC_Msk & ((value) << USBHS_DEVEPTIDR_ERRORTRANSEC_Pos))
#define USBHS_DEVEPTIDR_NBUSYBKEC_Pos         (12U)                                          /**< (USBHS_DEVEPTIDR) Number of Busy Banks Interrupt Clear Position */
#define USBHS_DEVEPTIDR_NBUSYBKEC_Msk         (0x1U << USBHS_DEVEPTIDR_NBUSYBKEC_Pos)        /**< (USBHS_DEVEPTIDR) Number of Busy Banks Interrupt Clear Mask */
#define USBHS_DEVEPTIDR_NBUSYBKEC(value)      (USBHS_DEVEPTIDR_NBUSYBKEC_Msk & ((value) << USBHS_DEVEPTIDR_NBUSYBKEC_Pos))
#define USBHS_DEVEPTIDR_FIFOCONC_Pos          (14U)                                          /**< (USBHS_DEVEPTIDR) FIFO Control Clear Position */
#define USBHS_DEVEPTIDR_FIFOCONC_Msk          (0x1U << USBHS_DEVEPTIDR_FIFOCONC_Pos)         /**< (USBHS_DEVEPTIDR) FIFO Control Clear Mask */
#define USBHS_DEVEPTIDR_FIFOCONC(value)       (USBHS_DEVEPTIDR_FIFOCONC_Msk & ((value) << USBHS_DEVEPTIDR_FIFOCONC_Pos))
#define USBHS_DEVEPTIDR_EPDISHDMAC_Pos        (16U)                                          /**< (USBHS_DEVEPTIDR) Endpoint Interrupts Disable HDMA Request Clear Position */
#define USBHS_DEVEPTIDR_EPDISHDMAC_Msk        (0x1U << USBHS_DEVEPTIDR_EPDISHDMAC_Pos)       /**< (USBHS_DEVEPTIDR) Endpoint Interrupts Disable HDMA Request Clear Mask */
#define USBHS_DEVEPTIDR_EPDISHDMAC(value)     (USBHS_DEVEPTIDR_EPDISHDMAC_Msk & ((value) << USBHS_DEVEPTIDR_EPDISHDMAC_Pos))
#define USBHS_DEVEPTIDR_NYETDISC_Pos          (17U)                                          /**< (USBHS_DEVEPTIDR) NYET Token Disable Clear Position */
#define USBHS_DEVEPTIDR_NYETDISC_Msk          (0x1U << USBHS_DEVEPTIDR_NYETDISC_Pos)         /**< (USBHS_DEVEPTIDR) NYET Token Disable Clear Mask */
#define USBHS_DEVEPTIDR_NYETDISC(value)       (USBHS_DEVEPTIDR_NYETDISC_Msk & ((value) << USBHS_DEVEPTIDR_NYETDISC_Pos))
#define USBHS_DEVEPTIDR_STALLRQC_Pos          (19U)                                          /**< (USBHS_DEVEPTIDR) STALL Request Clear Position */
#define USBHS_DEVEPTIDR_STALLRQC_Msk          (0x1U << USBHS_DEVEPTIDR_STALLRQC_Pos)         /**< (USBHS_DEVEPTIDR) STALL Request Clear Mask */
#define USBHS_DEVEPTIDR_STALLRQC(value)       (USBHS_DEVEPTIDR_STALLRQC_Msk & ((value) << USBHS_DEVEPTIDR_STALLRQC_Pos))
#define USBHS_DEVEPTIDR_Msk                   (0x000B57FFUL)                                 /**< (USBHS_DEVEPTIDR) Register Mask  */

/* -------- USBHS_DEVDMANXTDSC : (USBHS Offset: 0x00) (R/W 32) Device DMA Channel Next Descriptor Address Register -------- */
#define USBHS_DEVDMANXTDSC_NXT_DSC_ADD_Pos    (0U)                                           /**< (USBHS_DEVDMANXTDSC) Next Descriptor Address Position */
#define USBHS_DEVDMANXTDSC_NXT_DSC_ADD_Msk    (0xFFFFFFFFU << USBHS_DEVDMANXTDSC_NXT_DSC_ADD_Pos) /**< (USBHS_DEVDMANXTDSC) Next Descriptor Address Mask */
#define USBHS_DEVDMANXTDSC_NXT_DSC_ADD(value) (USBHS_DEVDMANXTDSC_NXT_DSC_ADD_Msk & ((value) << USBHS_DEVDMANXTDSC_NXT_DSC_ADD_Pos))
#define USBHS_DEVDMANXTDSC_Msk                (0xFFFFFFFFUL)                                 /**< (USBHS_DEVDMANXTDSC) Register Mask  */

/* -------- USBHS_DEVDMAADDRESS : (USBHS Offset: 0x04) (R/W 32) Device DMA Channel Address Register -------- */
#define USBHS_DEVDMAADDRESS_BUFF_ADD_Pos      (0U)                                           /**< (USBHS_DEVDMAADDRESS) Buffer Address Position */
#define USBHS_DEVDMAADDRESS_BUFF_ADD_Msk      (0xFFFFFFFFU << USBHS_DEVDMAADDRESS_BUFF_ADD_Pos) /**< (USBHS_DEVDMAADDRESS) Buffer Address Mask */
#define USBHS_DEVDMAADDRESS_BUFF_ADD(value)   (USBHS_DEVDMAADDRESS_BUFF_ADD_Msk & ((value) << USBHS_DEVDMAADDRESS_BUFF_ADD_Pos))
#define USBHS_DEVDMAADDRESS_Msk               (0xFFFFFFFFUL)                                 /**< (USBHS_DEVDMAADDRESS) Register Mask  */

/* -------- USBHS_DEVDMACONTROL : (USBHS Offset: 0x08) (R/W 32) Device DMA Channel Control Register -------- */
#define USBHS_DEVDMACONTROL_CHANN_ENB_Pos     (0U)                                           /**< (USBHS_DEVDMACONTROL) Channel Enable Command Position */
#define USBHS_DEVDMACONTROL_CHANN_ENB_Msk     (0x1U << USBHS_DEVDMACONTROL_CHANN_ENB_Pos)    /**< (USBHS_DEVDMACONTROL) Channel Enable Command Mask */
#define USBHS_DEVDMACONTROL_CHANN_ENB(value)  (USBHS_DEVDMACONTROL_CHANN_ENB_Msk & ((value) << USBHS_DEVDMACONTROL_CHANN_ENB_Pos))
#define USBHS_DEVDMACONTROL_LDNXT_DSC_Pos     (1U)                                           /**< (USBHS_DEVDMACONTROL) Load Next Channel Transfer Descriptor Enable Command Position */
#define USBHS_DEVDMACONTROL_LDNXT_DSC_Msk     (0x1U << USBHS_DEVDMACONTROL_LDNXT_DSC_Pos)    /**< (USBHS_DEVDMACONTROL) Load Next Channel Transfer Descriptor Enable Command Mask */
#define USBHS_DEVDMACONTROL_LDNXT_DSC(value)  (USBHS_DEVDMACONTROL_LDNXT_DSC_Msk & ((value) << USBHS_DEVDMACONTROL_LDNXT_DSC_Pos))
#define USBHS_DEVDMACONTROL_END_TR_EN_Pos     (2U)                                           /**< (USBHS_DEVDMACONTROL) End of Transfer Enable Control (OUT transfers only) Position */
#define USBHS_DEVDMACONTROL_END_TR_EN_Msk     (0x1U << USBHS_DEVDMACONTROL_END_TR_EN_Pos)    /**< (USBHS_DEVDMACONTROL) End of Transfer Enable Control (OUT transfers only) Mask */
#define USBHS_DEVDMACONTROL_END_TR_EN(value)  (USBHS_DEVDMACONTROL_END_TR_EN_Msk & ((value) << USBHS_DEVDMACONTROL_END_TR_EN_Pos))
#define USBHS_DEVDMACONTROL_END_B_EN_Pos      (3U)                                           /**< (USBHS_DEVDMACONTROL) End of Buffer Enable Control Position */
#define USBHS_DEVDMACONTROL_END_B_EN_Msk      (0x1U << USBHS_DEVDMACONTROL_END_B_EN_Pos)     /**< (USBHS_DEVDMACONTROL) End of Buffer Enable Control Mask */
#define USBHS_DEVDMACONTROL_END_B_EN(value)   (USBHS_DEVDMACONTROL_END_B_EN_Msk & ((value) << USBHS_DEVDMACONTROL_END_B_EN_Pos))
#define USBHS_DEVDMACONTROL_END_TR_IT_Pos     (4U)                                           /**< (USBHS_DEVDMACONTROL) End of Transfer Interrupt Enable Position */
#define USBHS_DEVDMACONTROL_END_TR_IT_Msk     (0x1U << USBHS_DEVDMACONTROL_END_TR_IT_Pos)    /**< (USBHS_DEVDMACONTROL) End of Transfer Interrupt Enable Mask */
#define USBHS_DEVDMACONTROL_END_TR_IT(value)  (USBHS_DEVDMACONTROL_END_TR_IT_Msk & ((value) << USBHS_DEVDMACONTROL_END_TR_IT_Pos))
#define USBHS_DEVDMACONTROL_END_BUFFIT_Pos    (5U)                                           /**< (USBHS_DEVDMACONTROL) End of Buffer Interrupt Enable Position */
#define USBHS_DEVDMACONTROL_END_BUFFIT_Msk    (0x1U << USBHS_DEVDMACONTROL_END_BUFFIT_Pos)   /**< (USBHS_DEVDMACONTROL) End of Buffer Interrupt Enable Mask */
#define USBHS_DEVDMACONTROL_END_BUFFIT(value) (USBHS_DEVDMACONTROL_END_BUFFIT_Msk & ((value) << USBHS_DEVDMACONTROL_END_BUFFIT_Pos))
#define USBHS_DEVDMACONTROL_DESC_LD_IT_Pos    (6U)                                           /**< (USBHS_DEVDMACONTROL) Descriptor Loaded Interrupt Enable Position */
#define USBHS_DEVDMACONTROL_DESC_LD_IT_Msk    (0x1U << USBHS_DEVDMACONTROL_DESC_LD_IT_Pos)   /**< (USBHS_DEVDMACONTROL) Descriptor Loaded Interrupt Enable Mask */
#define USBHS_DEVDMACONTROL_DESC_LD_IT(value) (USBHS_DEVDMACONTROL_DESC_LD_IT_Msk & ((value) << USBHS_DEVDMACONTROL_DESC_LD_IT_Pos))
#define USBHS_DEVDMACONTROL_BURST_LCK_Pos     (7U)                                           /**< (USBHS_DEVDMACONTROL) Burst Lock Enable Position */
#define USBHS_DEVDMACONTROL_BURST_LCK_Msk     (0x1U << USBHS_DEVDMACONTROL_BURST_LCK_Pos)    /**< (USBHS_DEVDMACONTROL) Burst Lock Enable Mask */
#define USBHS_DEVDMACONTROL_BURST_LCK(value)  (USBHS_DEVDMACONTROL_BURST_LCK_Msk & ((value) << USBHS_DEVDMACONTROL_BURST_LCK_Pos))
#define USBHS_DEVDMACONTROL_BUFF_LENGTH_Pos   (16U)                                          /**< (USBHS_DEVDMACONTROL) Buffer Byte Length (Write-only) Position */
#define USBHS_DEVDMACONTROL_BUFF_LENGTH_Msk   (0xFFFFU << USBHS_DEVDMACONTROL_BUFF_LENGTH_Pos) /**< (USBHS_DEVDMACONTROL) Buffer Byte Length (Write-only) Mask */
#define USBHS_DEVDMACONTROL_BUFF_LENGTH(value) (USBHS_DEVDMACONTROL_BUFF_LENGTH_Msk & ((value) << USBHS_DEVDMACONTROL_BUFF_LENGTH_Pos))
#define USBHS_DEVDMACONTROL_Msk               (0xFFFF00FFUL)                                 /**< (USBHS_DEVDMACONTROL) Register Mask  */

/* -------- USBHS_DEVDMASTATUS : (USBHS Offset: 0x0C) (R/W 32) Device DMA Channel Status Register -------- */
#define USBHS_DEVDMASTATUS_CHANN_ENB_Pos      (0U)                                           /**< (USBHS_DEVDMASTATUS) Channel Enable Status Position */
#define USBHS_DEVDMASTATUS_CHANN_ENB_Msk      (0x1U << USBHS_DEVDMASTATUS_CHANN_ENB_Pos)     /**< (USBHS_DEVDMASTATUS) Channel Enable Status Mask */
#define USBHS_DEVDMASTATUS_CHANN_ENB(value)   (USBHS_DEVDMASTATUS_CHANN_ENB_Msk & ((value) << USBHS_DEVDMASTATUS_CHANN_ENB_Pos))
#define USBHS_DEVDMASTATUS_CHANN_ACT_Pos      (1U)                                           /**< (USBHS_DEVDMASTATUS) Channel Active Status Position */
#define USBHS_DEVDMASTATUS_CHANN_ACT_Msk      (0x1U << USBHS_DEVDMASTATUS_CHANN_ACT_Pos)     /**< (USBHS_DEVDMASTATUS) Channel Active Status Mask */
#define USBHS_DEVDMASTATUS_CHANN_ACT(value)   (USBHS_DEVDMASTATUS_CHANN_ACT_Msk & ((value) << USBHS_DEVDMASTATUS_CHANN_ACT_Pos))
#define USBHS_DEVDMASTATUS_END_TR_ST_Pos      (4U)                                           /**< (USBHS_DEVDMASTATUS) End of Channel Transfer Status Position */
#define USBHS_DEVDMASTATUS_END_TR_ST_Msk      (0x1U << USBHS_DEVDMASTATUS_END_TR_ST_Pos)     /**< (USBHS_DEVDMASTATUS) End of Channel Transfer Status Mask */
#define USBHS_DEVDMASTATUS_END_TR_ST(value)   (USBHS_DEVDMASTATUS_END_TR_ST_Msk & ((value) << USBHS_DEVDMASTATUS_END_TR_ST_Pos))
#define USBHS_DEVDMASTATUS_END_BF_ST_Pos      (5U)                                           /**< (USBHS_DEVDMASTATUS) End of Channel Buffer Status Position */
#define USBHS_DEVDMASTATUS_END_BF_ST_Msk      (0x1U << USBHS_DEVDMASTATUS_END_BF_ST_Pos)     /**< (USBHS_DEVDMASTATUS) End of Channel Buffer Status Mask */
#define USBHS_DEVDMASTATUS_END_BF_ST(value)   (USBHS_DEVDMASTATUS_END_BF_ST_Msk & ((value) << USBHS_DEVDMASTATUS_END_BF_ST_Pos))
#define USBHS_DEVDMASTATUS_DESC_LDST_Pos      (6U)                                           /**< (USBHS_DEVDMASTATUS) Descriptor Loaded Status Position */
#define USBHS_DEVDMASTATUS_DESC_LDST_Msk      (0x1U << USBHS_DEVDMASTATUS_DESC_LDST_Pos)     /**< (USBHS_DEVDMASTATUS) Descriptor Loaded Status Mask */
#define USBHS_DEVDMASTATUS_DESC_LDST(value)   (USBHS_DEVDMASTATUS_DESC_LDST_Msk & ((value) << USBHS_DEVDMASTATUS_DESC_LDST_Pos))
#define USBHS_DEVDMASTATUS_BUFF_COUNT_Pos     (16U)                                          /**< (USBHS_DEVDMASTATUS) Buffer Byte Count Position */
#define USBHS_DEVDMASTATUS_BUFF_COUNT_Msk     (0xFFFFU << USBHS_DEVDMASTATUS_BUFF_COUNT_Pos) /**< (USBHS_DEVDMASTATUS) Buffer Byte Count Mask */
#define USBHS_DEVDMASTATUS_BUFF_COUNT(value)  (USBHS_DEVDMASTATUS_BUFF_COUNT_Msk & ((value) << USBHS_DEVDMASTATUS_BUFF_COUNT_Pos))
#define USBHS_DEVDMASTATUS_Msk                (0xFFFF0073UL)                                 /**< (USBHS_DEVDMASTATUS) Register Mask  */

/* -------- USBHS_HSTCTRL : (USBHS Offset: 0x400) (R/W 32) Host General Control Register -------- */
#define USBHS_HSTCTRL_SOFE_Pos                (8U)                                           /**< (USBHS_HSTCTRL) Start of Frame Generation Enable Position */
#define USBHS_HSTCTRL_SOFE_Msk                (0x1U << USBHS_HSTCTRL_SOFE_Pos)               /**< (USBHS_HSTCTRL) Start of Frame Generation Enable Mask */
#define USBHS_HSTCTRL_SOFE(value)             (USBHS_HSTCTRL_SOFE_Msk & ((value) << USBHS_HSTCTRL_SOFE_Pos))
#define USBHS_HSTCTRL_RESET_Pos               (9U)                                           /**< (USBHS_HSTCTRL) Send USB Reset Position */
#define USBHS_HSTCTRL_RESET_Msk               (0x1U << USBHS_HSTCTRL_RESET_Pos)              /**< (USBHS_HSTCTRL) Send USB Reset Mask */
#define USBHS_HSTCTRL_RESET(value)            (USBHS_HSTCTRL_RESET_Msk & ((value) << USBHS_HSTCTRL_RESET_Pos))
#define USBHS_HSTCTRL_RESUME_Pos              (10U)                                          /**< (USBHS_HSTCTRL) Send USB Resume Position */
#define USBHS_HSTCTRL_RESUME_Msk              (0x1U << USBHS_HSTCTRL_RESUME_Pos)             /**< (USBHS_HSTCTRL) Send USB Resume Mask */
#define USBHS_HSTCTRL_RESUME(value)           (USBHS_HSTCTRL_RESUME_Msk & ((value) << USBHS_HSTCTRL_RESUME_Pos))
#define USBHS_HSTCTRL_SPDCONF_Pos             (12U)                                          /**< (USBHS_HSTCTRL) Mode Configuration Position */
#define USBHS_HSTCTRL_SPDCONF_Msk             (0x3U << USBHS_HSTCTRL_SPDCONF_Pos)            /**< (USBHS_HSTCTRL) Mode Configuration Mask */
#define USBHS_HSTCTRL_SPDCONF(value)          (USBHS_HSTCTRL_SPDCONF_Msk & ((value) << USBHS_HSTCTRL_SPDCONF_Pos))
#define   USBHS_HSTCTRL_SPDCONF_NORMAL_Val    (0U)                                           /**< (USBHS_HSTCTRL) The host starts in Full-speed mode and performs a high-speed reset to switch to High-speed mode if the downstream peripheral is high-speed capable.  */
#define   USBHS_HSTCTRL_SPDCONF_LOW_POWER_Val (1U)                                           /**< (USBHS_HSTCTRL) For a better consumption, if high speed is not needed.  */
#define   USBHS_HSTCTRL_SPDCONF_HIGH_SPEED_Val (2U)                                           /**< (USBHS_HSTCTRL) Forced high speed.  */
#define   USBHS_HSTCTRL_SPDCONF_FORCED_FS_Val (3U)                                           /**< (USBHS_HSTCTRL) The host remains in Full-speed mode whatever the peripheral speed capability.  */
#define USBHS_HSTCTRL_SPDCONF_NORMAL          (USBHS_HSTCTRL_SPDCONF_NORMAL_Val << USBHS_HSTCTRL_SPDCONF_Pos) /**< (USBHS_HSTCTRL) The host starts in Full-speed mode and performs a high-speed reset to switch to High-speed mode if the downstream peripheral is high-speed capable. Position  */
#define USBHS_HSTCTRL_SPDCONF_LOW_POWER       (USBHS_HSTCTRL_SPDCONF_LOW_POWER_Val << USBHS_HSTCTRL_SPDCONF_Pos) /**< (USBHS_HSTCTRL) For a better consumption, if high speed is not needed. Position  */
#define USBHS_HSTCTRL_SPDCONF_HIGH_SPEED      (USBHS_HSTCTRL_SPDCONF_HIGH_SPEED_Val << USBHS_HSTCTRL_SPDCONF_Pos) /**< (USBHS_HSTCTRL) Forced high speed. Position  */
#define USBHS_HSTCTRL_SPDCONF_FORCED_FS       (USBHS_HSTCTRL_SPDCONF_FORCED_FS_Val << USBHS_HSTCTRL_SPDCONF_Pos) /**< (USBHS_HSTCTRL) The host remains in Full-speed mode whatever the peripheral speed capability. Position  */
#define USBHS_HSTCTRL_Msk                     (0x00003700UL)                                 /**< (USBHS_HSTCTRL) Register Mask  */

/* -------- USBHS_HSTISR : (USBHS Offset: 0x404) (R/  32) Host Global Interrupt Status Register -------- */
#define USBHS_HSTISR_DCONNI_Pos               (0U)                                           /**< (USBHS_HSTISR) Device Connection Interrupt Position */
#define USBHS_HSTISR_DCONNI_Msk               (0x1U << USBHS_HSTISR_DCONNI_Pos)              /**< (USBHS_HSTISR) Device Connection Interrupt Mask */
#define USBHS_HSTISR_DCONNI(value)            (USBHS_HSTISR_DCONNI_Msk & ((value) << USBHS_HSTISR_DCONNI_Pos))
#define USBHS_HSTISR_DDISCI_Pos               (1U)                                           /**< (USBHS_HSTISR) Device Disconnection Interrupt Position */
#define USBHS_HSTISR_DDISCI_Msk               (0x1U << USBHS_HSTISR_DDISCI_Pos)              /**< (USBHS_HSTISR) Device Disconnection Interrupt Mask */
#define USBHS_HSTISR_DDISCI(value)            (USBHS_HSTISR_DDISCI_Msk & ((value) << USBHS_HSTISR_DDISCI_Pos))
#define USBHS_HSTISR_RSTI_Pos                 (2U)                                           /**< (USBHS_HSTISR) USB Reset Sent Interrupt Position */
#define USBHS_HSTISR_RSTI_Msk                 (0x1U << USBHS_HSTISR_RSTI_Pos)                /**< (USBHS_HSTISR) USB Reset Sent Interrupt Mask */
#define USBHS_HSTISR_RSTI(value)              (USBHS_HSTISR_RSTI_Msk & ((value) << USBHS_HSTISR_RSTI_Pos))
#define USBHS_HSTISR_RSMEDI_Pos               (3U)                                           /**< (USBHS_HSTISR) Downstream Resume Sent Interrupt Position */
#define USBHS_HSTISR_RSMEDI_Msk               (0x1U << USBHS_HSTISR_RSMEDI_Pos)              /**< (USBHS_HSTISR) Downstream Resume Sent Interrupt Mask */
#define USBHS_HSTISR_RSMEDI(value)            (USBHS_HSTISR_RSMEDI_Msk & ((value) << USBHS_HSTISR_RSMEDI_Pos))
#define USBHS_HSTISR_RXRSMI_Pos               (4U)                                           /**< (USBHS_HSTISR) Upstream Resume Received Interrupt Position */
#define USBHS_HSTISR_RXRSMI_Msk               (0x1U << USBHS_HSTISR_RXRSMI_Pos)              /**< (USBHS_HSTISR) Upstream Resume Received Interrupt Mask */
#define USBHS_HSTISR_RXRSMI(value)            (USBHS_HSTISR_RXRSMI_Msk & ((value) << USBHS_HSTISR_RXRSMI_Pos))
#define USBHS_HSTISR_HSOFI_Pos                (5U)                                           /**< (USBHS_HSTISR) Host Start of Frame Interrupt Position */
#define USBHS_HSTISR_HSOFI_Msk                (0x1U << USBHS_HSTISR_HSOFI_Pos)               /**< (USBHS_HSTISR) Host Start of Frame Interrupt Mask */
#define USBHS_HSTISR_HSOFI(value)             (USBHS_HSTISR_HSOFI_Msk & ((value) << USBHS_HSTISR_HSOFI_Pos))
#define USBHS_HSTISR_HWUPI_Pos                (6U)                                           /**< (USBHS_HSTISR) Host Wake-Up Interrupt Position */
#define USBHS_HSTISR_HWUPI_Msk                (0x1U << USBHS_HSTISR_HWUPI_Pos)               /**< (USBHS_HSTISR) Host Wake-Up Interrupt Mask */
#define USBHS_HSTISR_HWUPI(value)             (USBHS_HSTISR_HWUPI_Msk & ((value) << USBHS_HSTISR_HWUPI_Pos))
#define USBHS_HSTISR_PEP_0_Pos                (8U)                                           /**< (USBHS_HSTISR) Pipe 0 Interrupt Position */
#define USBHS_HSTISR_PEP_0_Msk                (0x1U << USBHS_HSTISR_PEP_0_Pos)               /**< (USBHS_HSTISR) Pipe 0 Interrupt Mask */
#define USBHS_HSTISR_PEP_0(value)             (USBHS_HSTISR_PEP_0_Msk & ((value) << USBHS_HSTISR_PEP_0_Pos))
#define USBHS_HSTISR_PEP_1_Pos                (9U)                                           /**< (USBHS_HSTISR) Pipe 1 Interrupt Position */
#define USBHS_HSTISR_PEP_1_Msk                (0x1U << USBHS_HSTISR_PEP_1_Pos)               /**< (USBHS_HSTISR) Pipe 1 Interrupt Mask */
#define USBHS_HSTISR_PEP_1(value)             (USBHS_HSTISR_PEP_1_Msk & ((value) << USBHS_HSTISR_PEP_1_Pos))
#define USBHS_HSTISR_PEP_2_Pos                (10U)                                          /**< (USBHS_HSTISR) Pipe 2 Interrupt Position */
#define USBHS_HSTISR_PEP_2_Msk                (0x1U << USBHS_HSTISR_PEP_2_Pos)               /**< (USBHS_HSTISR) Pipe 2 Interrupt Mask */
#define USBHS_HSTISR_PEP_2(value)             (USBHS_HSTISR_PEP_2_Msk & ((value) << USBHS_HSTISR_PEP_2_Pos))
#define USBHS_HSTISR_PEP_3_Pos                (11U)                                          /**< (USBHS_HSTISR) Pipe 3 Interrupt Position */
#define USBHS_HSTISR_PEP_3_Msk                (0x1U << USBHS_HSTISR_PEP_3_Pos)               /**< (USBHS_HSTISR) Pipe 3 Interrupt Mask */
#define USBHS_HSTISR_PEP_3(value)             (USBHS_HSTISR_PEP_3_Msk & ((value) << USBHS_HSTISR_PEP_3_Pos))
#define USBHS_HSTISR_PEP_4_Pos                (12U)                                          /**< (USBHS_HSTISR) Pipe 4 Interrupt Position */
#define USBHS_HSTISR_PEP_4_Msk                (0x1U << USBHS_HSTISR_PEP_4_Pos)               /**< (USBHS_HSTISR) Pipe 4 Interrupt Mask */
#define USBHS_HSTISR_PEP_4(value)             (USBHS_HSTISR_PEP_4_Msk & ((value) << USBHS_HSTISR_PEP_4_Pos))
#define USBHS_HSTISR_PEP_5_Pos                (13U)                                          /**< (USBHS_HSTISR) Pipe 5 Interrupt Position */
#define USBHS_HSTISR_PEP_5_Msk                (0x1U << USBHS_HSTISR_PEP_5_Pos)               /**< (USBHS_HSTISR) Pipe 5 Interrupt Mask */
#define USBHS_HSTISR_PEP_5(value)             (USBHS_HSTISR_PEP_5_Msk & ((value) << USBHS_HSTISR_PEP_5_Pos))
#define USBHS_HSTISR_PEP_6_Pos                (14U)                                          /**< (USBHS_HSTISR) Pipe 6 Interrupt Position */
#define USBHS_HSTISR_PEP_6_Msk                (0x1U << USBHS_HSTISR_PEP_6_Pos)               /**< (USBHS_HSTISR) Pipe 6 Interrupt Mask */
#define USBHS_HSTISR_PEP_6(value)             (USBHS_HSTISR_PEP_6_Msk & ((value) << USBHS_HSTISR_PEP_6_Pos))
#define USBHS_HSTISR_PEP_7_Pos                (15U)                                          /**< (USBHS_HSTISR) Pipe 7 Interrupt Position */
#define USBHS_HSTISR_PEP_7_Msk                (0x1U << USBHS_HSTISR_PEP_7_Pos)               /**< (USBHS_HSTISR) Pipe 7 Interrupt Mask */
#define USBHS_HSTISR_PEP_7(value)             (USBHS_HSTISR_PEP_7_Msk & ((value) << USBHS_HSTISR_PEP_7_Pos))
#define USBHS_HSTISR_PEP_8_Pos                (16U)                                          /**< (USBHS_HSTISR) Pipe 8 Interrupt Position */
#define USBHS_HSTISR_PEP_8_Msk                (0x1U << USBHS_HSTISR_PEP_8_Pos)               /**< (USBHS_HSTISR) Pipe 8 Interrupt Mask */
#define USBHS_HSTISR_PEP_8(value)             (USBHS_HSTISR_PEP_8_Msk & ((value) << USBHS_HSTISR_PEP_8_Pos))
#define USBHS_HSTISR_PEP_9_Pos                (17U)                                          /**< (USBHS_HSTISR) Pipe 9 Interrupt Position */
#define USBHS_HSTISR_PEP_9_Msk                (0x1U << USBHS_HSTISR_PEP_9_Pos)               /**< (USBHS_HSTISR) Pipe 9 Interrupt Mask */
#define USBHS_HSTISR_PEP_9(value)             (USBHS_HSTISR_PEP_9_Msk & ((value) << USBHS_HSTISR_PEP_9_Pos))
#define USBHS_HSTISR_DMA_0_Pos                (25U)                                          /**< (USBHS_HSTISR) DMA Channel 0 Interrupt Position */
#define USBHS_HSTISR_DMA_0_Msk                (0x1U << USBHS_HSTISR_DMA_0_Pos)               /**< (USBHS_HSTISR) DMA Channel 0 Interrupt Mask */
#define USBHS_HSTISR_DMA_0(value)             (USBHS_HSTISR_DMA_0_Msk & ((value) << USBHS_HSTISR_DMA_0_Pos))
#define USBHS_HSTISR_DMA_1_Pos                (26U)                                          /**< (USBHS_HSTISR) DMA Channel 1 Interrupt Position */
#define USBHS_HSTISR_DMA_1_Msk                (0x1U << USBHS_HSTISR_DMA_1_Pos)               /**< (USBHS_HSTISR) DMA Channel 1 Interrupt Mask */
#define USBHS_HSTISR_DMA_1(value)             (USBHS_HSTISR_DMA_1_Msk & ((value) << USBHS_HSTISR_DMA_1_Pos))
#define USBHS_HSTISR_DMA_2_Pos                (27U)                                          /**< (USBHS_HSTISR) DMA Channel 2 Interrupt Position */
#define USBHS_HSTISR_DMA_2_Msk                (0x1U << USBHS_HSTISR_DMA_2_Pos)               /**< (USBHS_HSTISR) DMA Channel 2 Interrupt Mask */
#define USBHS_HSTISR_DMA_2(value)             (USBHS_HSTISR_DMA_2_Msk & ((value) << USBHS_HSTISR_DMA_2_Pos))
#define USBHS_HSTISR_DMA_3_Pos                (28U)                                          /**< (USBHS_HSTISR) DMA Channel 3 Interrupt Position */
#define USBHS_HSTISR_DMA_3_Msk                (0x1U << USBHS_HSTISR_DMA_3_Pos)               /**< (USBHS_HSTISR) DMA Channel 3 Interrupt Mask */
#define USBHS_HSTISR_DMA_3(value)             (USBHS_HSTISR_DMA_3_Msk & ((value) << USBHS_HSTISR_DMA_3_Pos))
#define USBHS_HSTISR_DMA_4_Pos                (29U)                                          /**< (USBHS_HSTISR) DMA Channel 4 Interrupt Position */
#define USBHS_HSTISR_DMA_4_Msk                (0x1U << USBHS_HSTISR_DMA_4_Pos)               /**< (USBHS_HSTISR) DMA Channel 4 Interrupt Mask */
#define USBHS_HSTISR_DMA_4(value)             (USBHS_HSTISR_DMA_4_Msk & ((value) << USBHS_HSTISR_DMA_4_Pos))
#define USBHS_HSTISR_DMA_5_Pos                (30U)                                          /**< (USBHS_HSTISR) DMA Channel 5 Interrupt Position */
#define USBHS_HSTISR_DMA_5_Msk                (0x1U << USBHS_HSTISR_DMA_5_Pos)               /**< (USBHS_HSTISR) DMA Channel 5 Interrupt Mask */
#define USBHS_HSTISR_DMA_5(value)             (USBHS_HSTISR_DMA_5_Msk & ((value) << USBHS_HSTISR_DMA_5_Pos))
#define USBHS_HSTISR_DMA_6_Pos                (31U)                                          /**< (USBHS_HSTISR) DMA Channel 6 Interrupt Position */
#define USBHS_HSTISR_DMA_6_Msk                (0x1U << USBHS_HSTISR_DMA_6_Pos)               /**< (USBHS_HSTISR) DMA Channel 6 Interrupt Mask */
#define USBHS_HSTISR_DMA_6(value)             (USBHS_HSTISR_DMA_6_Msk & ((value) << USBHS_HSTISR_DMA_6_Pos))
#define USBHS_HSTISR_Msk                      (0xFE03FF7FUL)                                 /**< (USBHS_HSTISR) Register Mask  */

/* -------- USBHS_HSTICR : (USBHS Offset: 0x408) ( /W 32) Host Global Interrupt Clear Register -------- */
#define USBHS_HSTICR_DCONNIC_Pos              (0U)                                           /**< (USBHS_HSTICR) Device Connection Interrupt Clear Position */
#define USBHS_HSTICR_DCONNIC_Msk              (0x1U << USBHS_HSTICR_DCONNIC_Pos)             /**< (USBHS_HSTICR) Device Connection Interrupt Clear Mask */
#define USBHS_HSTICR_DCONNIC(value)           (USBHS_HSTICR_DCONNIC_Msk & ((value) << USBHS_HSTICR_DCONNIC_Pos))
#define USBHS_HSTICR_DDISCIC_Pos              (1U)                                           /**< (USBHS_HSTICR) Device Disconnection Interrupt Clear Position */
#define USBHS_HSTICR_DDISCIC_Msk              (0x1U << USBHS_HSTICR_DDISCIC_Pos)             /**< (USBHS_HSTICR) Device Disconnection Interrupt Clear Mask */
#define USBHS_HSTICR_DDISCIC(value)           (USBHS_HSTICR_DDISCIC_Msk & ((value) << USBHS_HSTICR_DDISCIC_Pos))
#define USBHS_HSTICR_RSTIC_Pos                (2U)                                           /**< (USBHS_HSTICR) USB Reset Sent Interrupt Clear Position */
#define USBHS_HSTICR_RSTIC_Msk                (0x1U << USBHS_HSTICR_RSTIC_Pos)               /**< (USBHS_HSTICR) USB Reset Sent Interrupt Clear Mask */
#define USBHS_HSTICR_RSTIC(value)             (USBHS_HSTICR_RSTIC_Msk & ((value) << USBHS_HSTICR_RSTIC_Pos))
#define USBHS_HSTICR_RSMEDIC_Pos              (3U)                                           /**< (USBHS_HSTICR) Downstream Resume Sent Interrupt Clear Position */
#define USBHS_HSTICR_RSMEDIC_Msk              (0x1U << USBHS_HSTICR_RSMEDIC_Pos)             /**< (USBHS_HSTICR) Downstream Resume Sent Interrupt Clear Mask */
#define USBHS_HSTICR_RSMEDIC(value)           (USBHS_HSTICR_RSMEDIC_Msk & ((value) << USBHS_HSTICR_RSMEDIC_Pos))
#define USBHS_HSTICR_RXRSMIC_Pos              (4U)                                           /**< (USBHS_HSTICR) Upstream Resume Received Interrupt Clear Position */
#define USBHS_HSTICR_RXRSMIC_Msk              (0x1U << USBHS_HSTICR_RXRSMIC_Pos)             /**< (USBHS_HSTICR) Upstream Resume Received Interrupt Clear Mask */
#define USBHS_HSTICR_RXRSMIC(value)           (USBHS_HSTICR_RXRSMIC_Msk & ((value) << USBHS_HSTICR_RXRSMIC_Pos))
#define USBHS_HSTICR_HSOFIC_Pos               (5U)                                           /**< (USBHS_HSTICR) Host Start of Frame Interrupt Clear Position */
#define USBHS_HSTICR_HSOFIC_Msk               (0x1U << USBHS_HSTICR_HSOFIC_Pos)              /**< (USBHS_HSTICR) Host Start of Frame Interrupt Clear Mask */
#define USBHS_HSTICR_HSOFIC(value)            (USBHS_HSTICR_HSOFIC_Msk & ((value) << USBHS_HSTICR_HSOFIC_Pos))
#define USBHS_HSTICR_HWUPIC_Pos               (6U)                                           /**< (USBHS_HSTICR) Host Wake-Up Interrupt Clear Position */
#define USBHS_HSTICR_HWUPIC_Msk               (0x1U << USBHS_HSTICR_HWUPIC_Pos)              /**< (USBHS_HSTICR) Host Wake-Up Interrupt Clear Mask */
#define USBHS_HSTICR_HWUPIC(value)            (USBHS_HSTICR_HWUPIC_Msk & ((value) << USBHS_HSTICR_HWUPIC_Pos))
#define USBHS_HSTICR_Msk                      (0x0000007FUL)                                 /**< (USBHS_HSTICR) Register Mask  */

/* -------- USBHS_HSTIFR : (USBHS Offset: 0x40C) ( /W 32) Host Global Interrupt Set Register -------- */
#define USBHS_HSTIFR_DCONNIS_Pos              (0U)                                           /**< (USBHS_HSTIFR) Device Connection Interrupt Set Position */
#define USBHS_HSTIFR_DCONNIS_Msk              (0x1U << USBHS_HSTIFR_DCONNIS_Pos)             /**< (USBHS_HSTIFR) Device Connection Interrupt Set Mask */
#define USBHS_HSTIFR_DCONNIS(value)           (USBHS_HSTIFR_DCONNIS_Msk & ((value) << USBHS_HSTIFR_DCONNIS_Pos))
#define USBHS_HSTIFR_DDISCIS_Pos              (1U)                                           /**< (USBHS_HSTIFR) Device Disconnection Interrupt Set Position */
#define USBHS_HSTIFR_DDISCIS_Msk              (0x1U << USBHS_HSTIFR_DDISCIS_Pos)             /**< (USBHS_HSTIFR) Device Disconnection Interrupt Set Mask */
#define USBHS_HSTIFR_DDISCIS(value)           (USBHS_HSTIFR_DDISCIS_Msk & ((value) << USBHS_HSTIFR_DDISCIS_Pos))
#define USBHS_HSTIFR_RSTIS_Pos                (2U)                                           /**< (USBHS_HSTIFR) USB Reset Sent Interrupt Set Position */
#define USBHS_HSTIFR_RSTIS_Msk                (0x1U << USBHS_HSTIFR_RSTIS_Pos)               /**< (USBHS_HSTIFR) USB Reset Sent Interrupt Set Mask */
#define USBHS_HSTIFR_RSTIS(value)             (USBHS_HSTIFR_RSTIS_Msk & ((value) << USBHS_HSTIFR_RSTIS_Pos))
#define USBHS_HSTIFR_RSMEDIS_Pos              (3U)                                           /**< (USBHS_HSTIFR) Downstream Resume Sent Interrupt Set Position */
#define USBHS_HSTIFR_RSMEDIS_Msk              (0x1U << USBHS_HSTIFR_RSMEDIS_Pos)             /**< (USBHS_HSTIFR) Downstream Resume Sent Interrupt Set Mask */
#define USBHS_HSTIFR_RSMEDIS(value)           (USBHS_HSTIFR_RSMEDIS_Msk & ((value) << USBHS_HSTIFR_RSMEDIS_Pos))
#define USBHS_HSTIFR_RXRSMIS_Pos              (4U)                                           /**< (USBHS_HSTIFR) Upstream Resume Received Interrupt Set Position */
#define USBHS_HSTIFR_RXRSMIS_Msk              (0x1U << USBHS_HSTIFR_RXRSMIS_Pos)             /**< (USBHS_HSTIFR) Upstream Resume Received Interrupt Set Mask */
#define USBHS_HSTIFR_RXRSMIS(value)           (USBHS_HSTIFR_RXRSMIS_Msk & ((value) << USBHS_HSTIFR_RXRSMIS_Pos))
#define USBHS_HSTIFR_HSOFIS_Pos               (5U)                                           /**< (USBHS_HSTIFR) Host Start of Frame Interrupt Set Position */
#define USBHS_HSTIFR_HSOFIS_Msk               (0x1U << USBHS_HSTIFR_HSOFIS_Pos)              /**< (USBHS_HSTIFR) Host Start of Frame Interrupt Set Mask */
#define USBHS_HSTIFR_HSOFIS(value)            (USBHS_HSTIFR_HSOFIS_Msk & ((value) << USBHS_HSTIFR_HSOFIS_Pos))
#define USBHS_HSTIFR_HWUPIS_Pos               (6U)                                           /**< (USBHS_HSTIFR) Host Wake-Up Interrupt Set Position */
#define USBHS_HSTIFR_HWUPIS_Msk               (0x1U << USBHS_HSTIFR_HWUPIS_Pos)              /**< (USBHS_HSTIFR) Host Wake-Up Interrupt Set Mask */
#define USBHS_HSTIFR_HWUPIS(value)            (USBHS_HSTIFR_HWUPIS_Msk & ((value) << USBHS_HSTIFR_HWUPIS_Pos))
#define USBHS_HSTIFR_DMA_0_Pos                (25U)                                          /**< (USBHS_HSTIFR) DMA Channel 0 Interrupt Set Position */
#define USBHS_HSTIFR_DMA_0_Msk                (0x1U << USBHS_HSTIFR_DMA_0_Pos)               /**< (USBHS_HSTIFR) DMA Channel 0 Interrupt Set Mask */
#define USBHS_HSTIFR_DMA_0(value)             (USBHS_HSTIFR_DMA_0_Msk & ((value) << USBHS_HSTIFR_DMA_0_Pos))
#define USBHS_HSTIFR_DMA_1_Pos                (26U)                                          /**< (USBHS_HSTIFR) DMA Channel 1 Interrupt Set Position */
#define USBHS_HSTIFR_DMA_1_Msk                (0x1U << USBHS_HSTIFR_DMA_1_Pos)               /**< (USBHS_HSTIFR) DMA Channel 1 Interrupt Set Mask */
#define USBHS_HSTIFR_DMA_1(value)             (USBHS_HSTIFR_DMA_1_Msk & ((value) << USBHS_HSTIFR_DMA_1_Pos))
#define USBHS_HSTIFR_DMA_2_Pos                (27U)                                          /**< (USBHS_HSTIFR) DMA Channel 2 Interrupt Set Position */
#define USBHS_HSTIFR_DMA_2_Msk                (0x1U << USBHS_HSTIFR_DMA_2_Pos)               /**< (USBHS_HSTIFR) DMA Channel 2 Interrupt Set Mask */
#define USBHS_HSTIFR_DMA_2(value)             (USBHS_HSTIFR_DMA_2_Msk & ((value) << USBHS_HSTIFR_DMA_2_Pos))
#define USBHS_HSTIFR_DMA_3_Pos                (28U)                                          /**< (USBHS_HSTIFR) DMA Channel 3 Interrupt Set Position */
#define USBHS_HSTIFR_DMA_3_Msk                (0x1U << USBHS_HSTIFR_DMA_3_Pos)               /**< (USBHS_HSTIFR) DMA Channel 3 Interrupt Set Mask */
#define USBHS_HSTIFR_DMA_3(value)             (USBHS_HSTIFR_DMA_3_Msk & ((value) << USBHS_HSTIFR_DMA_3_Pos))
#define USBHS_HSTIFR_DMA_4_Pos                (29U)                                          /**< (USBHS_HSTIFR) DMA Channel 4 Interrupt Set Position */
#define USBHS_HSTIFR_DMA_4_Msk                (0x1U << USBHS_HSTIFR_DMA_4_Pos)               /**< (USBHS_HSTIFR) DMA Channel 4 Interrupt Set Mask */
#define USBHS_HSTIFR_DMA_4(value)             (USBHS_HSTIFR_DMA_4_Msk & ((value) << USBHS_HSTIFR_DMA_4_Pos))
#define USBHS_HSTIFR_DMA_5_Pos                (30U)                                          /**< (USBHS_HSTIFR) DMA Channel 5 Interrupt Set Position */
#define USBHS_HSTIFR_DMA_5_Msk                (0x1U << USBHS_HSTIFR_DMA_5_Pos)               /**< (USBHS_HSTIFR) DMA Channel 5 Interrupt Set Mask */
#define USBHS_HSTIFR_DMA_5(value)             (USBHS_HSTIFR_DMA_5_Msk & ((value) << USBHS_HSTIFR_DMA_5_Pos))
#define USBHS_HSTIFR_DMA_6_Pos                (31U)                                          /**< (USBHS_HSTIFR) DMA Channel 6 Interrupt Set Position */
#define USBHS_HSTIFR_DMA_6_Msk                (0x1U << USBHS_HSTIFR_DMA_6_Pos)               /**< (USBHS_HSTIFR) DMA Channel 6 Interrupt Set Mask */
#define USBHS_HSTIFR_DMA_6(value)             (USBHS_HSTIFR_DMA_6_Msk & ((value) << USBHS_HSTIFR_DMA_6_Pos))
#define USBHS_HSTIFR_Msk                      (0xFE00007FUL)                                 /**< (USBHS_HSTIFR) Register Mask  */

/* -------- USBHS_HSTIMR : (USBHS Offset: 0x410) (R/  32) Host Global Interrupt Mask Register -------- */
#define USBHS_HSTIMR_DCONNIE_Pos              (0U)                                           /**< (USBHS_HSTIMR) Device Connection Interrupt Enable Position */
#define USBHS_HSTIMR_DCONNIE_Msk              (0x1U << USBHS_HSTIMR_DCONNIE_Pos)             /**< (USBHS_HSTIMR) Device Connection Interrupt Enable Mask */
#define USBHS_HSTIMR_DCONNIE(value)           (USBHS_HSTIMR_DCONNIE_Msk & ((value) << USBHS_HSTIMR_DCONNIE_Pos))
#define USBHS_HSTIMR_DDISCIE_Pos              (1U)                                           /**< (USBHS_HSTIMR) Device Disconnection Interrupt Enable Position */
#define USBHS_HSTIMR_DDISCIE_Msk              (0x1U << USBHS_HSTIMR_DDISCIE_Pos)             /**< (USBHS_HSTIMR) Device Disconnection Interrupt Enable Mask */
#define USBHS_HSTIMR_DDISCIE(value)           (USBHS_HSTIMR_DDISCIE_Msk & ((value) << USBHS_HSTIMR_DDISCIE_Pos))
#define USBHS_HSTIMR_RSTIE_Pos                (2U)                                           /**< (USBHS_HSTIMR) USB Reset Sent Interrupt Enable Position */
#define USBHS_HSTIMR_RSTIE_Msk                (0x1U << USBHS_HSTIMR_RSTIE_Pos)               /**< (USBHS_HSTIMR) USB Reset Sent Interrupt Enable Mask */
#define USBHS_HSTIMR_RSTIE(value)             (USBHS_HSTIMR_RSTIE_Msk & ((value) << USBHS_HSTIMR_RSTIE_Pos))
#define USBHS_HSTIMR_RSMEDIE_Pos              (3U)                                           /**< (USBHS_HSTIMR) Downstream Resume Sent Interrupt Enable Position */
#define USBHS_HSTIMR_RSMEDIE_Msk              (0x1U << USBHS_HSTIMR_RSMEDIE_Pos)             /**< (USBHS_HSTIMR) Downstream Resume Sent Interrupt Enable Mask */
#define USBHS_HSTIMR_RSMEDIE(value)           (USBHS_HSTIMR_RSMEDIE_Msk & ((value) << USBHS_HSTIMR_RSMEDIE_Pos))
#define USBHS_HSTIMR_RXRSMIE_Pos              (4U)                                           /**< (USBHS_HSTIMR) Upstream Resume Received Interrupt Enable Position */
#define USBHS_HSTIMR_RXRSMIE_Msk              (0x1U << USBHS_HSTIMR_RXRSMIE_Pos)             /**< (USBHS_HSTIMR) Upstream Resume Received Interrupt Enable Mask */
#define USBHS_HSTIMR_RXRSMIE(value)           (USBHS_HSTIMR_RXRSMIE_Msk & ((value) << USBHS_HSTIMR_RXRSMIE_Pos))
#define USBHS_HSTIMR_HSOFIE_Pos               (5U)                                           /**< (USBHS_HSTIMR) Host Start of Frame Interrupt Enable Position */
#define USBHS_HSTIMR_HSOFIE_Msk               (0x1U << USBHS_HSTIMR_HSOFIE_Pos)              /**< (USBHS_HSTIMR) Host Start of Frame Interrupt Enable Mask */
#define USBHS_HSTIMR_HSOFIE(value)            (USBHS_HSTIMR_HSOFIE_Msk & ((value) << USBHS_HSTIMR_HSOFIE_Pos))
#define USBHS_HSTIMR_HWUPIE_Pos               (6U)                                           /**< (USBHS_HSTIMR) Host Wake-Up Interrupt Enable Position */
#define USBHS_HSTIMR_HWUPIE_Msk               (0x1U << USBHS_HSTIMR_HWUPIE_Pos)              /**< (USBHS_HSTIMR) Host Wake-Up Interrupt Enable Mask */
#define USBHS_HSTIMR_HWUPIE(value)            (USBHS_HSTIMR_HWUPIE_Msk & ((value) << USBHS_HSTIMR_HWUPIE_Pos))
#define USBHS_HSTIMR_PEP_0_Pos                (8U)                                           /**< (USBHS_HSTIMR) Pipe 0 Interrupt Enable Position */
#define USBHS_HSTIMR_PEP_0_Msk                (0x1U << USBHS_HSTIMR_PEP_0_Pos)               /**< (USBHS_HSTIMR) Pipe 0 Interrupt Enable Mask */
#define USBHS_HSTIMR_PEP_0(value)             (USBHS_HSTIMR_PEP_0_Msk & ((value) << USBHS_HSTIMR_PEP_0_Pos))
#define USBHS_HSTIMR_PEP_1_Pos                (9U)                                           /**< (USBHS_HSTIMR) Pipe 1 Interrupt Enable Position */
#define USBHS_HSTIMR_PEP_1_Msk                (0x1U << USBHS_HSTIMR_PEP_1_Pos)               /**< (USBHS_HSTIMR) Pipe 1 Interrupt Enable Mask */
#define USBHS_HSTIMR_PEP_1(value)             (USBHS_HSTIMR_PEP_1_Msk & ((value) << USBHS_HSTIMR_PEP_1_Pos))
#define USBHS_HSTIMR_PEP_2_Pos                (10U)                                          /**< (USBHS_HSTIMR) Pipe 2 Interrupt Enable Position */
#define USBHS_HSTIMR_PEP_2_Msk                (0x1U << USBHS_HSTIMR_PEP_2_Pos)               /**< (USBHS_HSTIMR) Pipe 2 Interrupt Enable Mask */
#define USBHS_HSTIMR_PEP_2(value)             (USBHS_HSTIMR_PEP_2_Msk & ((value) << USBHS_HSTIMR_PEP_2_Pos))
#define USBHS_HSTIMR_PEP_3_Pos                (11U)                                          /**< (USBHS_HSTIMR) Pipe 3 Interrupt Enable Position */
#define USBHS_HSTIMR_PEP_3_Msk                (0x1U << USBHS_HSTIMR_PEP_3_Pos)               /**< (USBHS_HSTIMR) Pipe 3 Interrupt Enable Mask */
#define USBHS_HSTIMR_PEP_3(value)             (USBHS_HSTIMR_PEP_3_Msk & ((value) << USBHS_HSTIMR_PEP_3_Pos))
#define USBHS_HSTIMR_PEP_4_Pos                (12U)                                          /**< (USBHS_HSTIMR) Pipe 4 Interrupt Enable Position */
#define USBHS_HSTIMR_PEP_4_Msk                (0x1U << USBHS_HSTIMR_PEP_4_Pos)               /**< (USBHS_HSTIMR) Pipe 4 Interrupt Enable Mask */
#define USBHS_HSTIMR_PEP_4(value)             (USBHS_HSTIMR_PEP_4_Msk & ((value) << USBHS_HSTIMR_PEP_4_Pos))
#define USBHS_HSTIMR_PEP_5_Pos                (13U)                                          /**< (USBHS_HSTIMR) Pipe 5 Interrupt Enable Position */
#define USBHS_HSTIMR_PEP_5_Msk                (0x1U << USBHS_HSTIMR_PEP_5_Pos)               /**< (USBHS_HSTIMR) Pipe 5 Interrupt Enable Mask */
#define USBHS_HSTIMR_PEP_5(value)             (USBHS_HSTIMR_PEP_5_Msk & ((value) << USBHS_HSTIMR_PEP_5_Pos))
#define USBHS_HSTIMR_PEP_6_Pos                (14U)                                          /**< (USBHS_HSTIMR) Pipe 6 Interrupt Enable Position */
#define USBHS_HSTIMR_PEP_6_Msk                (0x1U << USBHS_HSTIMR_PEP_6_Pos)               /**< (USBHS_HSTIMR) Pipe 6 Interrupt Enable Mask */
#define USBHS_HSTIMR_PEP_6(value)             (USBHS_HSTIMR_PEP_6_Msk & ((value) << USBHS_HSTIMR_PEP_6_Pos))
#define USBHS_HSTIMR_PEP_7_Pos                (15U)                                          /**< (USBHS_HSTIMR) Pipe 7 Interrupt Enable Position */
#define USBHS_HSTIMR_PEP_7_Msk                (0x1U << USBHS_HSTIMR_PEP_7_Pos)               /**< (USBHS_HSTIMR) Pipe 7 Interrupt Enable Mask */
#define USBHS_HSTIMR_PEP_7(value)             (USBHS_HSTIMR_PEP_7_Msk & ((value) << USBHS_HSTIMR_PEP_7_Pos))
#define USBHS_HSTIMR_PEP_8_Pos                (16U)                                          /**< (USBHS_HSTIMR) Pipe 8 Interrupt Enable Position */
#define USBHS_HSTIMR_PEP_8_Msk                (0x1U << USBHS_HSTIMR_PEP_8_Pos)               /**< (USBHS_HSTIMR) Pipe 8 Interrupt Enable Mask */
#define USBHS_HSTIMR_PEP_8(value)             (USBHS_HSTIMR_PEP_8_Msk & ((value) << USBHS_HSTIMR_PEP_8_Pos))
#define USBHS_HSTIMR_PEP_9_Pos                (17U)                                          /**< (USBHS_HSTIMR) Pipe 9 Interrupt Enable Position */
#define USBHS_HSTIMR_PEP_9_Msk                (0x1U << USBHS_HSTIMR_PEP_9_Pos)               /**< (USBHS_HSTIMR) Pipe 9 Interrupt Enable Mask */
#define USBHS_HSTIMR_PEP_9(value)             (USBHS_HSTIMR_PEP_9_Msk & ((value) << USBHS_HSTIMR_PEP_9_Pos))
#define USBHS_HSTIMR_DMA_0_Pos                (25U)                                          /**< (USBHS_HSTIMR) DMA Channel 0 Interrupt Enable Position */
#define USBHS_HSTIMR_DMA_0_Msk                (0x1U << USBHS_HSTIMR_DMA_0_Pos)               /**< (USBHS_HSTIMR) DMA Channel 0 Interrupt Enable Mask */
#define USBHS_HSTIMR_DMA_0(value)             (USBHS_HSTIMR_DMA_0_Msk & ((value) << USBHS_HSTIMR_DMA_0_Pos))
#define USBHS_HSTIMR_DMA_1_Pos                (26U)                                          /**< (USBHS_HSTIMR) DMA Channel 1 Interrupt Enable Position */
#define USBHS_HSTIMR_DMA_1_Msk                (0x1U << USBHS_HSTIMR_DMA_1_Pos)               /**< (USBHS_HSTIMR) DMA Channel 1 Interrupt Enable Mask */
#define USBHS_HSTIMR_DMA_1(value)             (USBHS_HSTIMR_DMA_1_Msk & ((value) << USBHS_HSTIMR_DMA_1_Pos))
#define USBHS_HSTIMR_DMA_2_Pos                (27U)                                          /**< (USBHS_HSTIMR) DMA Channel 2 Interrupt Enable Position */
#define USBHS_HSTIMR_DMA_2_Msk                (0x1U << USBHS_HSTIMR_DMA_2_Pos)               /**< (USBHS_HSTIMR) DMA Channel 2 Interrupt Enable Mask */
#define USBHS_HSTIMR_DMA_2(value)             (USBHS_HSTIMR_DMA_2_Msk & ((value) << USBHS_HSTIMR_DMA_2_Pos))
#define USBHS_HSTIMR_DMA_3_Pos                (28U)                                          /**< (USBHS_HSTIMR) DMA Channel 3 Interrupt Enable Position */
#define USBHS_HSTIMR_DMA_3_Msk                (0x1U << USBHS_HSTIMR_DMA_3_Pos)               /**< (USBHS_HSTIMR) DMA Channel 3 Interrupt Enable Mask */
#define USBHS_HSTIMR_DMA_3(value)             (USBHS_HSTIMR_DMA_3_Msk & ((value) << USBHS_HSTIMR_DMA_3_Pos))
#define USBHS_HSTIMR_DMA_4_Pos                (29U)                                          /**< (USBHS_HSTIMR) DMA Channel 4 Interrupt Enable Position */
#define USBHS_HSTIMR_DMA_4_Msk                (0x1U << USBHS_HSTIMR_DMA_4_Pos)               /**< (USBHS_HSTIMR) DMA Channel 4 Interrupt Enable Mask */
#define USBHS_HSTIMR_DMA_4(value)             (USBHS_HSTIMR_DMA_4_Msk & ((value) << USBHS_HSTIMR_DMA_4_Pos))
#define USBHS_HSTIMR_DMA_5_Pos                (30U)                                          /**< (USBHS_HSTIMR) DMA Channel 5 Interrupt Enable Position */
#define USBHS_HSTIMR_DMA_5_Msk                (0x1U << USBHS_HSTIMR_DMA_5_Pos)               /**< (USBHS_HSTIMR) DMA Channel 5 Interrupt Enable Mask */
#define USBHS_HSTIMR_DMA_5(value)             (USBHS_HSTIMR_DMA_5_Msk & ((value) << USBHS_HSTIMR_DMA_5_Pos))
#define USBHS_HSTIMR_DMA_6_Pos                (31U)                                          /**< (USBHS_HSTIMR) DMA Channel 6 Interrupt Enable Position */
#define USBHS_HSTIMR_DMA_6_Msk                (0x1U << USBHS_HSTIMR_DMA_6_Pos)               /**< (USBHS_HSTIMR) DMA Channel 6 Interrupt Enable Mask */
#define USBHS_HSTIMR_DMA_6(value)             (USBHS_HSTIMR_DMA_6_Msk & ((value) << USBHS_HSTIMR_DMA_6_Pos))
#define USBHS_HSTIMR_Msk                      (0xFE03FF7FUL)                                 /**< (USBHS_HSTIMR) Register Mask  */

/* -------- USBHS_HSTIDR : (USBHS Offset: 0x414) ( /W 32) Host Global Interrupt Disable Register -------- */
#define USBHS_HSTIDR_DCONNIEC_Pos             (0U)                                           /**< (USBHS_HSTIDR) Device Connection Interrupt Disable Position */
#define USBHS_HSTIDR_DCONNIEC_Msk             (0x1U << USBHS_HSTIDR_DCONNIEC_Pos)            /**< (USBHS_HSTIDR) Device Connection Interrupt Disable Mask */
#define USBHS_HSTIDR_DCONNIEC(value)          (USBHS_HSTIDR_DCONNIEC_Msk & ((value) << USBHS_HSTIDR_DCONNIEC_Pos))
#define USBHS_HSTIDR_DDISCIEC_Pos             (1U)                                           /**< (USBHS_HSTIDR) Device Disconnection Interrupt Disable Position */
#define USBHS_HSTIDR_DDISCIEC_Msk             (0x1U << USBHS_HSTIDR_DDISCIEC_Pos)            /**< (USBHS_HSTIDR) Device Disconnection Interrupt Disable Mask */
#define USBHS_HSTIDR_DDISCIEC(value)          (USBHS_HSTIDR_DDISCIEC_Msk & ((value) << USBHS_HSTIDR_DDISCIEC_Pos))
#define USBHS_HSTIDR_RSTIEC_Pos               (2U)                                           /**< (USBHS_HSTIDR) USB Reset Sent Interrupt Disable Position */
#define USBHS_HSTIDR_RSTIEC_Msk               (0x1U << USBHS_HSTIDR_RSTIEC_Pos)              /**< (USBHS_HSTIDR) USB Reset Sent Interrupt Disable Mask */
#define USBHS_HSTIDR_RSTIEC(value)            (USBHS_HSTIDR_RSTIEC_Msk & ((value) << USBHS_HSTIDR_RSTIEC_Pos))
#define USBHS_HSTIDR_RSMEDIEC_Pos             (3U)                                           /**< (USBHS_HSTIDR) Downstream Resume Sent Interrupt Disable Position */
#define USBHS_HSTIDR_RSMEDIEC_Msk             (0x1U << USBHS_HSTIDR_RSMEDIEC_Pos)            /**< (USBHS_HSTIDR) Downstream Resume Sent Interrupt Disable Mask */
#define USBHS_HSTIDR_RSMEDIEC(value)          (USBHS_HSTIDR_RSMEDIEC_Msk & ((value) << USBHS_HSTIDR_RSMEDIEC_Pos))
#define USBHS_HSTIDR_RXRSMIEC_Pos             (4U)                                           /**< (USBHS_HSTIDR) Upstream Resume Received Interrupt Disable Position */
#define USBHS_HSTIDR_RXRSMIEC_Msk             (0x1U << USBHS_HSTIDR_RXRSMIEC_Pos)            /**< (USBHS_HSTIDR) Upstream Resume Received Interrupt Disable Mask */
#define USBHS_HSTIDR_RXRSMIEC(value)          (USBHS_HSTIDR_RXRSMIEC_Msk & ((value) << USBHS_HSTIDR_RXRSMIEC_Pos))
#define USBHS_HSTIDR_HSOFIEC_Pos              (5U)                                           /**< (USBHS_HSTIDR) Host Start of Frame Interrupt Disable Position */
#define USBHS_HSTIDR_HSOFIEC_Msk              (0x1U << USBHS_HSTIDR_HSOFIEC_Pos)             /**< (USBHS_HSTIDR) Host Start of Frame Interrupt Disable Mask */
#define USBHS_HSTIDR_HSOFIEC(value)           (USBHS_HSTIDR_HSOFIEC_Msk & ((value) << USBHS_HSTIDR_HSOFIEC_Pos))
#define USBHS_HSTIDR_HWUPIEC_Pos              (6U)                                           /**< (USBHS_HSTIDR) Host Wake-Up Interrupt Disable Position */
#define USBHS_HSTIDR_HWUPIEC_Msk              (0x1U << USBHS_HSTIDR_HWUPIEC_Pos)             /**< (USBHS_HSTIDR) Host Wake-Up Interrupt Disable Mask */
#define USBHS_HSTIDR_HWUPIEC(value)           (USBHS_HSTIDR_HWUPIEC_Msk & ((value) << USBHS_HSTIDR_HWUPIEC_Pos))
#define USBHS_HSTIDR_PEP_0_Pos                (8U)                                           /**< (USBHS_HSTIDR) Pipe 0 Interrupt Disable Position */
#define USBHS_HSTIDR_PEP_0_Msk                (0x1U << USBHS_HSTIDR_PEP_0_Pos)               /**< (USBHS_HSTIDR) Pipe 0 Interrupt Disable Mask */
#define USBHS_HSTIDR_PEP_0(value)             (USBHS_HSTIDR_PEP_0_Msk & ((value) << USBHS_HSTIDR_PEP_0_Pos))
#define USBHS_HSTIDR_PEP_1_Pos                (9U)                                           /**< (USBHS_HSTIDR) Pipe 1 Interrupt Disable Position */
#define USBHS_HSTIDR_PEP_1_Msk                (0x1U << USBHS_HSTIDR_PEP_1_Pos)               /**< (USBHS_HSTIDR) Pipe 1 Interrupt Disable Mask */
#define USBHS_HSTIDR_PEP_1(value)             (USBHS_HSTIDR_PEP_1_Msk & ((value) << USBHS_HSTIDR_PEP_1_Pos))
#define USBHS_HSTIDR_PEP_2_Pos                (10U)                                          /**< (USBHS_HSTIDR) Pipe 2 Interrupt Disable Position */
#define USBHS_HSTIDR_PEP_2_Msk                (0x1U << USBHS_HSTIDR_PEP_2_Pos)               /**< (USBHS_HSTIDR) Pipe 2 Interrupt Disable Mask */
#define USBHS_HSTIDR_PEP_2(value)             (USBHS_HSTIDR_PEP_2_Msk & ((value) << USBHS_HSTIDR_PEP_2_Pos))
#define USBHS_HSTIDR_PEP_3_Pos                (11U)                                          /**< (USBHS_HSTIDR) Pipe 3 Interrupt Disable Position */
#define USBHS_HSTIDR_PEP_3_Msk                (0x1U << USBHS_HSTIDR_PEP_3_Pos)               /**< (USBHS_HSTIDR) Pipe 3 Interrupt Disable Mask */
#define USBHS_HSTIDR_PEP_3(value)             (USBHS_HSTIDR_PEP_3_Msk & ((value) << USBHS_HSTIDR_PEP_3_Pos))
#define USBHS_HSTIDR_PEP_4_Pos                (12U)                                          /**< (USBHS_HSTIDR) Pipe 4 Interrupt Disable Position */
#define USBHS_HSTIDR_PEP_4_Msk                (0x1U << USBHS_HSTIDR_PEP_4_Pos)               /**< (USBHS_HSTIDR) Pipe 4 Interrupt Disable Mask */
#define USBHS_HSTIDR_PEP_4(value)             (USBHS_HSTIDR_PEP_4_Msk & ((value) << USBHS_HSTIDR_PEP_4_Pos))
#define USBHS_HSTIDR_PEP_5_Pos                (13U)                                          /**< (USBHS_HSTIDR) Pipe 5 Interrupt Disable Position */
#define USBHS_HSTIDR_PEP_5_Msk                (0x1U << USBHS_HSTIDR_PEP_5_Pos)               /**< (USBHS_HSTIDR) Pipe 5 Interrupt Disable Mask */
#define USBHS_HSTIDR_PEP_5(value)             (USBHS_HSTIDR_PEP_5_Msk & ((value) << USBHS_HSTIDR_PEP_5_Pos))
#define USBHS_HSTIDR_PEP_6_Pos                (14U)                                          /**< (USBHS_HSTIDR) Pipe 6 Interrupt Disable Position */
#define USBHS_HSTIDR_PEP_6_Msk                (0x1U << USBHS_HSTIDR_PEP_6_Pos)               /**< (USBHS_HSTIDR) Pipe 6 Interrupt Disable Mask */
#define USBHS_HSTIDR_PEP_6(value)             (USBHS_HSTIDR_PEP_6_Msk & ((value) << USBHS_HSTIDR_PEP_6_Pos))
#define USBHS_HSTIDR_PEP_7_Pos                (15U)                                          /**< (USBHS_HSTIDR) Pipe 7 Interrupt Disable Position */
#define USBHS_HSTIDR_PEP_7_Msk                (0x1U << USBHS_HSTIDR_PEP_7_Pos)               /**< (USBHS_HSTIDR) Pipe 7 Interrupt Disable Mask */
#define USBHS_HSTIDR_PEP_7(value)             (USBHS_HSTIDR_PEP_7_Msk & ((value) << USBHS_HSTIDR_PEP_7_Pos))
#define USBHS_HSTIDR_PEP_8_Pos                (16U)                                          /**< (USBHS_HSTIDR) Pipe 8 Interrupt Disable Position */
#define USBHS_HSTIDR_PEP_8_Msk                (0x1U << USBHS_HSTIDR_PEP_8_Pos)               /**< (USBHS_HSTIDR) Pipe 8 Interrupt Disable Mask */
#define USBHS_HSTIDR_PEP_8(value)             (USBHS_HSTIDR_PEP_8_Msk & ((value) << USBHS_HSTIDR_PEP_8_Pos))
#define USBHS_HSTIDR_PEP_9_Pos                (17U)                                          /**< (USBHS_HSTIDR) Pipe 9 Interrupt Disable Position */
#define USBHS_HSTIDR_PEP_9_Msk                (0x1U << USBHS_HSTIDR_PEP_9_Pos)               /**< (USBHS_HSTIDR) Pipe 9 Interrupt Disable Mask */
#define USBHS_HSTIDR_PEP_9(value)             (USBHS_HSTIDR_PEP_9_Msk & ((value) << USBHS_HSTIDR_PEP_9_Pos))
#define USBHS_HSTIDR_DMA_0_Pos                (25U)                                          /**< (USBHS_HSTIDR) DMA Channel 0 Interrupt Disable Position */
#define USBHS_HSTIDR_DMA_0_Msk                (0x1U << USBHS_HSTIDR_DMA_0_Pos)               /**< (USBHS_HSTIDR) DMA Channel 0 Interrupt Disable Mask */
#define USBHS_HSTIDR_DMA_0(value)             (USBHS_HSTIDR_DMA_0_Msk & ((value) << USBHS_HSTIDR_DMA_0_Pos))
#define USBHS_HSTIDR_DMA_1_Pos                (26U)                                          /**< (USBHS_HSTIDR) DMA Channel 1 Interrupt Disable Position */
#define USBHS_HSTIDR_DMA_1_Msk                (0x1U << USBHS_HSTIDR_DMA_1_Pos)               /**< (USBHS_HSTIDR) DMA Channel 1 Interrupt Disable Mask */
#define USBHS_HSTIDR_DMA_1(value)             (USBHS_HSTIDR_DMA_1_Msk & ((value) << USBHS_HSTIDR_DMA_1_Pos))
#define USBHS_HSTIDR_DMA_2_Pos                (27U)                                          /**< (USBHS_HSTIDR) DMA Channel 2 Interrupt Disable Position */
#define USBHS_HSTIDR_DMA_2_Msk                (0x1U << USBHS_HSTIDR_DMA_2_Pos)               /**< (USBHS_HSTIDR) DMA Channel 2 Interrupt Disable Mask */
#define USBHS_HSTIDR_DMA_2(value)             (USBHS_HSTIDR_DMA_2_Msk & ((value) << USBHS_HSTIDR_DMA_2_Pos))
#define USBHS_HSTIDR_DMA_3_Pos                (28U)                                          /**< (USBHS_HSTIDR) DMA Channel 3 Interrupt Disable Position */
#define USBHS_HSTIDR_DMA_3_Msk                (0x1U << USBHS_HSTIDR_DMA_3_Pos)               /**< (USBHS_HSTIDR) DMA Channel 3 Interrupt Disable Mask */
#define USBHS_HSTIDR_DMA_3(value)             (USBHS_HSTIDR_DMA_3_Msk & ((value) << USBHS_HSTIDR_DMA_3_Pos))
#define USBHS_HSTIDR_DMA_4_Pos                (29U)                                          /**< (USBHS_HSTIDR) DMA Channel 4 Interrupt Disable Position */
#define USBHS_HSTIDR_DMA_4_Msk                (0x1U << USBHS_HSTIDR_DMA_4_Pos)               /**< (USBHS_HSTIDR) DMA Channel 4 Interrupt Disable Mask */
#define USBHS_HSTIDR_DMA_4(value)             (USBHS_HSTIDR_DMA_4_Msk & ((value) << USBHS_HSTIDR_DMA_4_Pos))
#define USBHS_HSTIDR_DMA_5_Pos                (30U)                                          /**< (USBHS_HSTIDR) DMA Channel 5 Interrupt Disable Position */
#define USBHS_HSTIDR_DMA_5_Msk                (0x1U << USBHS_HSTIDR_DMA_5_Pos)               /**< (USBHS_HSTIDR) DMA Channel 5 Interrupt Disable Mask */
#define USBHS_HSTIDR_DMA_5(value)             (USBHS_HSTIDR_DMA_5_Msk & ((value) << USBHS_HSTIDR_DMA_5_Pos))
#define USBHS_HSTIDR_DMA_6_Pos                (31U)                                          /**< (USBHS_HSTIDR) DMA Channel 6 Interrupt Disable Position */
#define USBHS_HSTIDR_DMA_6_Msk                (0x1U << USBHS_HSTIDR_DMA_6_Pos)               /**< (USBHS_HSTIDR) DMA Channel 6 Interrupt Disable Mask */
#define USBHS_HSTIDR_DMA_6(value)             (USBHS_HSTIDR_DMA_6_Msk & ((value) << USBHS_HSTIDR_DMA_6_Pos))
#define USBHS_HSTIDR_Msk                      (0xFE03FF7FUL)                                 /**< (USBHS_HSTIDR) Register Mask  */

/* -------- USBHS_HSTIER : (USBHS Offset: 0x418) ( /W 32) Host Global Interrupt Enable Register -------- */
#define USBHS_HSTIER_DCONNIES_Pos             (0U)                                           /**< (USBHS_HSTIER) Device Connection Interrupt Enable Position */
#define USBHS_HSTIER_DCONNIES_Msk             (0x1U << USBHS_HSTIER_DCONNIES_Pos)            /**< (USBHS_HSTIER) Device Connection Interrupt Enable Mask */
#define USBHS_HSTIER_DCONNIES(value)          (USBHS_HSTIER_DCONNIES_Msk & ((value) << USBHS_HSTIER_DCONNIES_Pos))
#define USBHS_HSTIER_DDISCIES_Pos             (1U)                                           /**< (USBHS_HSTIER) Device Disconnection Interrupt Enable Position */
#define USBHS_HSTIER_DDISCIES_Msk             (0x1U << USBHS_HSTIER_DDISCIES_Pos)            /**< (USBHS_HSTIER) Device Disconnection Interrupt Enable Mask */
#define USBHS_HSTIER_DDISCIES(value)          (USBHS_HSTIER_DDISCIES_Msk & ((value) << USBHS_HSTIER_DDISCIES_Pos))
#define USBHS_HSTIER_RSTIES_Pos               (2U)                                           /**< (USBHS_HSTIER) USB Reset Sent Interrupt Enable Position */
#define USBHS_HSTIER_RSTIES_Msk               (0x1U << USBHS_HSTIER_RSTIES_Pos)              /**< (USBHS_HSTIER) USB Reset Sent Interrupt Enable Mask */
#define USBHS_HSTIER_RSTIES(value)            (USBHS_HSTIER_RSTIES_Msk & ((value) << USBHS_HSTIER_RSTIES_Pos))
#define USBHS_HSTIER_RSMEDIES_Pos             (3U)                                           /**< (USBHS_HSTIER) Downstream Resume Sent Interrupt Enable Position */
#define USBHS_HSTIER_RSMEDIES_Msk             (0x1U << USBHS_HSTIER_RSMEDIES_Pos)            /**< (USBHS_HSTIER) Downstream Resume Sent Interrupt Enable Mask */
#define USBHS_HSTIER_RSMEDIES(value)          (USBHS_HSTIER_RSMEDIES_Msk & ((value) << USBHS_HSTIER_RSMEDIES_Pos))
#define USBHS_HSTIER_RXRSMIES_Pos             (4U)                                           /**< (USBHS_HSTIER) Upstream Resume Received Interrupt Enable Position */
#define USBHS_HSTIER_RXRSMIES_Msk             (0x1U << USBHS_HSTIER_RXRSMIES_Pos)            /**< (USBHS_HSTIER) Upstream Resume Received Interrupt Enable Mask */
#define USBHS_HSTIER_RXRSMIES(value)          (USBHS_HSTIER_RXRSMIES_Msk & ((value) << USBHS_HSTIER_RXRSMIES_Pos))
#define USBHS_HSTIER_HSOFIES_Pos              (5U)                                           /**< (USBHS_HSTIER) Host Start of Frame Interrupt Enable Position */
#define USBHS_HSTIER_HSOFIES_Msk              (0x1U << USBHS_HSTIER_HSOFIES_Pos)             /**< (USBHS_HSTIER) Host Start of Frame Interrupt Enable Mask */
#define USBHS_HSTIER_HSOFIES(value)           (USBHS_HSTIER_HSOFIES_Msk & ((value) << USBHS_HSTIER_HSOFIES_Pos))
#define USBHS_HSTIER_HWUPIES_Pos              (6U)                                           /**< (USBHS_HSTIER) Host Wake-Up Interrupt Enable Position */
#define USBHS_HSTIER_HWUPIES_Msk              (0x1U << USBHS_HSTIER_HWUPIES_Pos)             /**< (USBHS_HSTIER) Host Wake-Up Interrupt Enable Mask */
#define USBHS_HSTIER_HWUPIES(value)           (USBHS_HSTIER_HWUPIES_Msk & ((value) << USBHS_HSTIER_HWUPIES_Pos))
#define USBHS_HSTIER_PEP_0_Pos                (8U)                                           /**< (USBHS_HSTIER) Pipe 0 Interrupt Enable Position */
#define USBHS_HSTIER_PEP_0_Msk                (0x1U << USBHS_HSTIER_PEP_0_Pos)               /**< (USBHS_HSTIER) Pipe 0 Interrupt Enable Mask */
#define USBHS_HSTIER_PEP_0(value)             (USBHS_HSTIER_PEP_0_Msk & ((value) << USBHS_HSTIER_PEP_0_Pos))
#define USBHS_HSTIER_PEP_1_Pos                (9U)                                           /**< (USBHS_HSTIER) Pipe 1 Interrupt Enable Position */
#define USBHS_HSTIER_PEP_1_Msk                (0x1U << USBHS_HSTIER_PEP_1_Pos)               /**< (USBHS_HSTIER) Pipe 1 Interrupt Enable Mask */
#define USBHS_HSTIER_PEP_1(value)             (USBHS_HSTIER_PEP_1_Msk & ((value) << USBHS_HSTIER_PEP_1_Pos))
#define USBHS_HSTIER_PEP_2_Pos                (10U)                                          /**< (USBHS_HSTIER) Pipe 2 Interrupt Enable Position */
#define USBHS_HSTIER_PEP_2_Msk                (0x1U << USBHS_HSTIER_PEP_2_Pos)               /**< (USBHS_HSTIER) Pipe 2 Interrupt Enable Mask */
#define USBHS_HSTIER_PEP_2(value)             (USBHS_HSTIER_PEP_2_Msk & ((value) << USBHS_HSTIER_PEP_2_Pos))
#define USBHS_HSTIER_PEP_3_Pos                (11U)                                          /**< (USBHS_HSTIER) Pipe 3 Interrupt Enable Position */
#define USBHS_HSTIER_PEP_3_Msk                (0x1U << USBHS_HSTIER_PEP_3_Pos)               /**< (USBHS_HSTIER) Pipe 3 Interrupt Enable Mask */
#define USBHS_HSTIER_PEP_3(value)             (USBHS_HSTIER_PEP_3_Msk & ((value) << USBHS_HSTIER_PEP_3_Pos))
#define USBHS_HSTIER_PEP_4_Pos                (12U)                                          /**< (USBHS_HSTIER) Pipe 4 Interrupt Enable Position */
#define USBHS_HSTIER_PEP_4_Msk                (0x1U << USBHS_HSTIER_PEP_4_Pos)               /**< (USBHS_HSTIER) Pipe 4 Interrupt Enable Mask */
#define USBHS_HSTIER_PEP_4(value)             (USBHS_HSTIER_PEP_4_Msk & ((value) << USBHS_HSTIER_PEP_4_Pos))
#define USBHS_HSTIER_PEP_5_Pos                (13U)                                          /**< (USBHS_HSTIER) Pipe 5 Interrupt Enable Position */
#define USBHS_HSTIER_PEP_5_Msk                (0x1U << USBHS_HSTIER_PEP_5_Pos)               /**< (USBHS_HSTIER) Pipe 5 Interrupt Enable Mask */
#define USBHS_HSTIER_PEP_5(value)             (USBHS_HSTIER_PEP_5_Msk & ((value) << USBHS_HSTIER_PEP_5_Pos))
#define USBHS_HSTIER_PEP_6_Pos                (14U)                                          /**< (USBHS_HSTIER) Pipe 6 Interrupt Enable Position */
#define USBHS_HSTIER_PEP_6_Msk                (0x1U << USBHS_HSTIER_PEP_6_Pos)               /**< (USBHS_HSTIER) Pipe 6 Interrupt Enable Mask */
#define USBHS_HSTIER_PEP_6(value)             (USBHS_HSTIER_PEP_6_Msk & ((value) << USBHS_HSTIER_PEP_6_Pos))
#define USBHS_HSTIER_PEP_7_Pos                (15U)                                          /**< (USBHS_HSTIER) Pipe 7 Interrupt Enable Position */
#define USBHS_HSTIER_PEP_7_Msk                (0x1U << USBHS_HSTIER_PEP_7_Pos)               /**< (USBHS_HSTIER) Pipe 7 Interrupt Enable Mask */
#define USBHS_HSTIER_PEP_7(value)             (USBHS_HSTIER_PEP_7_Msk & ((value) << USBHS_HSTIER_PEP_7_Pos))
#define USBHS_HSTIER_PEP_8_Pos                (16U)                                          /**< (USBHS_HSTIER) Pipe 8 Interrupt Enable Position */
#define USBHS_HSTIER_PEP_8_Msk                (0x1U << USBHS_HSTIER_PEP_8_Pos)               /**< (USBHS_HSTIER) Pipe 8 Interrupt Enable Mask */
#define USBHS_HSTIER_PEP_8(value)             (USBHS_HSTIER_PEP_8_Msk & ((value) << USBHS_HSTIER_PEP_8_Pos))
#define USBHS_HSTIER_PEP_9_Pos                (17U)                                          /**< (USBHS_HSTIER) Pipe 9 Interrupt Enable Position */
#define USBHS_HSTIER_PEP_9_Msk                (0x1U << USBHS_HSTIER_PEP_9_Pos)               /**< (USBHS_HSTIER) Pipe 9 Interrupt Enable Mask */
#define USBHS_HSTIER_PEP_9(value)             (USBHS_HSTIER_PEP_9_Msk & ((value) << USBHS_HSTIER_PEP_9_Pos))
#define USBHS_HSTIER_DMA_0_Pos                (25U)                                          /**< (USBHS_HSTIER) DMA Channel 0 Interrupt Enable Position */
#define USBHS_HSTIER_DMA_0_Msk                (0x1U << USBHS_HSTIER_DMA_0_Pos)               /**< (USBHS_HSTIER) DMA Channel 0 Interrupt Enable Mask */
#define USBHS_HSTIER_DMA_0(value)             (USBHS_HSTIER_DMA_0_Msk & ((value) << USBHS_HSTIER_DMA_0_Pos))
#define USBHS_HSTIER_DMA_1_Pos                (26U)                                          /**< (USBHS_HSTIER) DMA Channel 1 Interrupt Enable Position */
#define USBHS_HSTIER_DMA_1_Msk                (0x1U << USBHS_HSTIER_DMA_1_Pos)               /**< (USBHS_HSTIER) DMA Channel 1 Interrupt Enable Mask */
#define USBHS_HSTIER_DMA_1(value)             (USBHS_HSTIER_DMA_1_Msk & ((value) << USBHS_HSTIER_DMA_1_Pos))
#define USBHS_HSTIER_DMA_2_Pos                (27U)                                          /**< (USBHS_HSTIER) DMA Channel 2 Interrupt Enable Position */
#define USBHS_HSTIER_DMA_2_Msk                (0x1U << USBHS_HSTIER_DMA_2_Pos)               /**< (USBHS_HSTIER) DMA Channel 2 Interrupt Enable Mask */
#define USBHS_HSTIER_DMA_2(value)             (USBHS_HSTIER_DMA_2_Msk & ((value) << USBHS_HSTIER_DMA_2_Pos))
#define USBHS_HSTIER_DMA_3_Pos                (28U)                                          /**< (USBHS_HSTIER) DMA Channel 3 Interrupt Enable Position */
#define USBHS_HSTIER_DMA_3_Msk                (0x1U << USBHS_HSTIER_DMA_3_Pos)               /**< (USBHS_HSTIER) DMA Channel 3 Interrupt Enable Mask */
#define USBHS_HSTIER_DMA_3(value)             (USBHS_HSTIER_DMA_3_Msk & ((value) << USBHS_HSTIER_DMA_3_Pos))
#define USBHS_HSTIER_DMA_4_Pos                (29U)                                          /**< (USBHS_HSTIER) DMA Channel 4 Interrupt Enable Position */
#define USBHS_HSTIER_DMA_4_Msk                (0x1U << USBHS_HSTIER_DMA_4_Pos)               /**< (USBHS_HSTIER) DMA Channel 4 Interrupt Enable Mask */
#define USBHS_HSTIER_DMA_4(value)             (USBHS_HSTIER_DMA_4_Msk & ((value) << USBHS_HSTIER_DMA_4_Pos))
#define USBHS_HSTIER_DMA_5_Pos                (30U)                                          /**< (USBHS_HSTIER) DMA Channel 5 Interrupt Enable Position */
#define USBHS_HSTIER_DMA_5_Msk                (0x1U << USBHS_HSTIER_DMA_5_Pos)               /**< (USBHS_HSTIER) DMA Channel 5 Interrupt Enable Mask */
#define USBHS_HSTIER_DMA_5(value)             (USBHS_HSTIER_DMA_5_Msk & ((value) << USBHS_HSTIER_DMA_5_Pos))
#define USBHS_HSTIER_DMA_6_Pos                (31U)                                          /**< (USBHS_HSTIER) DMA Channel 6 Interrupt Enable Position */
#define USBHS_HSTIER_DMA_6_Msk                (0x1U << USBHS_HSTIER_DMA_6_Pos)               /**< (USBHS_HSTIER) DMA Channel 6 Interrupt Enable Mask */
#define USBHS_HSTIER_DMA_6(value)             (USBHS_HSTIER_DMA_6_Msk & ((value) << USBHS_HSTIER_DMA_6_Pos))
#define USBHS_HSTIER_Msk                      (0xFE03FF7FUL)                                 /**< (USBHS_HSTIER) Register Mask  */

/* -------- USBHS_HSTPIP : (USBHS Offset: 0x41C) (R/W 32) Host Pipe Register -------- */
#define USBHS_HSTPIP_PEN0_Pos                 (0U)                                           /**< (USBHS_HSTPIP) Pipe 0 Enable Position */
#define USBHS_HSTPIP_PEN0_Msk                 (0x1U << USBHS_HSTPIP_PEN0_Pos)                /**< (USBHS_HSTPIP) Pipe 0 Enable Mask */
#define USBHS_HSTPIP_PEN0(value)              (USBHS_HSTPIP_PEN0_Msk & ((value) << USBHS_HSTPIP_PEN0_Pos))
#define USBHS_HSTPIP_PEN1_Pos                 (1U)                                           /**< (USBHS_HSTPIP) Pipe 1 Enable Position */
#define USBHS_HSTPIP_PEN1_Msk                 (0x1U << USBHS_HSTPIP_PEN1_Pos)                /**< (USBHS_HSTPIP) Pipe 1 Enable Mask */
#define USBHS_HSTPIP_PEN1(value)              (USBHS_HSTPIP_PEN1_Msk & ((value) << USBHS_HSTPIP_PEN1_Pos))
#define USBHS_HSTPIP_PEN2_Pos                 (2U)                                           /**< (USBHS_HSTPIP) Pipe 2 Enable Position */
#define USBHS_HSTPIP_PEN2_Msk                 (0x1U << USBHS_HSTPIP_PEN2_Pos)                /**< (USBHS_HSTPIP) Pipe 2 Enable Mask */
#define USBHS_HSTPIP_PEN2(value)              (USBHS_HSTPIP_PEN2_Msk & ((value) << USBHS_HSTPIP_PEN2_Pos))
#define USBHS_HSTPIP_PEN3_Pos                 (3U)                                           /**< (USBHS_HSTPIP) Pipe 3 Enable Position */
#define USBHS_HSTPIP_PEN3_Msk                 (0x1U << USBHS_HSTPIP_PEN3_Pos)                /**< (USBHS_HSTPIP) Pipe 3 Enable Mask */
#define USBHS_HSTPIP_PEN3(value)              (USBHS_HSTPIP_PEN3_Msk & ((value) << USBHS_HSTPIP_PEN3_Pos))
#define USBHS_HSTPIP_PEN4_Pos                 (4U)                                           /**< (USBHS_HSTPIP) Pipe 4 Enable Position */
#define USBHS_HSTPIP_PEN4_Msk                 (0x1U << USBHS_HSTPIP_PEN4_Pos)                /**< (USBHS_HSTPIP) Pipe 4 Enable Mask */
#define USBHS_HSTPIP_PEN4(value)              (USBHS_HSTPIP_PEN4_Msk & ((value) << USBHS_HSTPIP_PEN4_Pos))
#define USBHS_HSTPIP_PEN5_Pos                 (5U)                                           /**< (USBHS_HSTPIP) Pipe 5 Enable Position */
#define USBHS_HSTPIP_PEN5_Msk                 (0x1U << USBHS_HSTPIP_PEN5_Pos)                /**< (USBHS_HSTPIP) Pipe 5 Enable Mask */
#define USBHS_HSTPIP_PEN5(value)              (USBHS_HSTPIP_PEN5_Msk & ((value) << USBHS_HSTPIP_PEN5_Pos))
#define USBHS_HSTPIP_PEN6_Pos                 (6U)                                           /**< (USBHS_HSTPIP) Pipe 6 Enable Position */
#define USBHS_HSTPIP_PEN6_Msk                 (0x1U << USBHS_HSTPIP_PEN6_Pos)                /**< (USBHS_HSTPIP) Pipe 6 Enable Mask */
#define USBHS_HSTPIP_PEN6(value)              (USBHS_HSTPIP_PEN6_Msk & ((value) << USBHS_HSTPIP_PEN6_Pos))
#define USBHS_HSTPIP_PEN7_Pos                 (7U)                                           /**< (USBHS_HSTPIP) Pipe 7 Enable Position */
#define USBHS_HSTPIP_PEN7_Msk                 (0x1U << USBHS_HSTPIP_PEN7_Pos)                /**< (USBHS_HSTPIP) Pipe 7 Enable Mask */
#define USBHS_HSTPIP_PEN7(value)              (USBHS_HSTPIP_PEN7_Msk & ((value) << USBHS_HSTPIP_PEN7_Pos))
#define USBHS_HSTPIP_PEN8_Pos                 (8U)                                           /**< (USBHS_HSTPIP) Pipe 8 Enable Position */
#define USBHS_HSTPIP_PEN8_Msk                 (0x1U << USBHS_HSTPIP_PEN8_Pos)                /**< (USBHS_HSTPIP) Pipe 8 Enable Mask */
#define USBHS_HSTPIP_PEN8(value)              (USBHS_HSTPIP_PEN8_Msk & ((value) << USBHS_HSTPIP_PEN8_Pos))
#define USBHS_HSTPIP_PRST0_Pos                (16U)                                          /**< (USBHS_HSTPIP) Pipe 0 Reset Position */
#define USBHS_HSTPIP_PRST0_Msk                (0x1U << USBHS_HSTPIP_PRST0_Pos)               /**< (USBHS_HSTPIP) Pipe 0 Reset Mask */
#define USBHS_HSTPIP_PRST0(value)             (USBHS_HSTPIP_PRST0_Msk & ((value) << USBHS_HSTPIP_PRST0_Pos))
#define USBHS_HSTPIP_PRST1_Pos                (17U)                                          /**< (USBHS_HSTPIP) Pipe 1 Reset Position */
#define USBHS_HSTPIP_PRST1_Msk                (0x1U << USBHS_HSTPIP_PRST1_Pos)               /**< (USBHS_HSTPIP) Pipe 1 Reset Mask */
#define USBHS_HSTPIP_PRST1(value)             (USBHS_HSTPIP_PRST1_Msk & ((value) << USBHS_HSTPIP_PRST1_Pos))
#define USBHS_HSTPIP_PRST2_Pos                (18U)                                          /**< (USBHS_HSTPIP) Pipe 2 Reset Position */
#define USBHS_HSTPIP_PRST2_Msk                (0x1U << USBHS_HSTPIP_PRST2_Pos)               /**< (USBHS_HSTPIP) Pipe 2 Reset Mask */
#define USBHS_HSTPIP_PRST2(value)             (USBHS_HSTPIP_PRST2_Msk & ((value) << USBHS_HSTPIP_PRST2_Pos))
#define USBHS_HSTPIP_PRST3_Pos                (19U)                                          /**< (USBHS_HSTPIP) Pipe 3 Reset Position */
#define USBHS_HSTPIP_PRST3_Msk                (0x1U << USBHS_HSTPIP_PRST3_Pos)               /**< (USBHS_HSTPIP) Pipe 3 Reset Mask */
#define USBHS_HSTPIP_PRST3(value)             (USBHS_HSTPIP_PRST3_Msk & ((value) << USBHS_HSTPIP_PRST3_Pos))
#define USBHS_HSTPIP_PRST4_Pos                (20U)                                          /**< (USBHS_HSTPIP) Pipe 4 Reset Position */
#define USBHS_HSTPIP_PRST4_Msk                (0x1U << USBHS_HSTPIP_PRST4_Pos)               /**< (USBHS_HSTPIP) Pipe 4 Reset Mask */
#define USBHS_HSTPIP_PRST4(value)             (USBHS_HSTPIP_PRST4_Msk & ((value) << USBHS_HSTPIP_PRST4_Pos))
#define USBHS_HSTPIP_PRST5_Pos                (21U)                                          /**< (USBHS_HSTPIP) Pipe 5 Reset Position */
#define USBHS_HSTPIP_PRST5_Msk                (0x1U << USBHS_HSTPIP_PRST5_Pos)               /**< (USBHS_HSTPIP) Pipe 5 Reset Mask */
#define USBHS_HSTPIP_PRST5(value)             (USBHS_HSTPIP_PRST5_Msk & ((value) << USBHS_HSTPIP_PRST5_Pos))
#define USBHS_HSTPIP_PRST6_Pos                (22U)                                          /**< (USBHS_HSTPIP) Pipe 6 Reset Position */
#define USBHS_HSTPIP_PRST6_Msk                (0x1U << USBHS_HSTPIP_PRST6_Pos)               /**< (USBHS_HSTPIP) Pipe 6 Reset Mask */
#define USBHS_HSTPIP_PRST6(value)             (USBHS_HSTPIP_PRST6_Msk & ((value) << USBHS_HSTPIP_PRST6_Pos))
#define USBHS_HSTPIP_PRST7_Pos                (23U)                                          /**< (USBHS_HSTPIP) Pipe 7 Reset Position */
#define USBHS_HSTPIP_PRST7_Msk                (0x1U << USBHS_HSTPIP_PRST7_Pos)               /**< (USBHS_HSTPIP) Pipe 7 Reset Mask */
#define USBHS_HSTPIP_PRST7(value)             (USBHS_HSTPIP_PRST7_Msk & ((value) << USBHS_HSTPIP_PRST7_Pos))
#define USBHS_HSTPIP_PRST8_Pos                (24U)                                          /**< (USBHS_HSTPIP) Pipe 8 Reset Position */
#define USBHS_HSTPIP_PRST8_Msk                (0x1U << USBHS_HSTPIP_PRST8_Pos)               /**< (USBHS_HSTPIP) Pipe 8 Reset Mask */
#define USBHS_HSTPIP_PRST8(value)             (USBHS_HSTPIP_PRST8_Msk & ((value) << USBHS_HSTPIP_PRST8_Pos))
#define USBHS_HSTPIP_Msk                      (0x01FF01FFUL)                                 /**< (USBHS_HSTPIP) Register Mask  */

/* -------- USBHS_HSTFNUM : (USBHS Offset: 0x420) (R/W 32) Host Frame Number Register -------- */
#define USBHS_HSTFNUM_MFNUM_Pos               (0U)                                           /**< (USBHS_HSTFNUM) Micro Frame Number Position */
#define USBHS_HSTFNUM_MFNUM_Msk               (0x7U << USBHS_HSTFNUM_MFNUM_Pos)              /**< (USBHS_HSTFNUM) Micro Frame Number Mask */
#define USBHS_HSTFNUM_MFNUM(value)            (USBHS_HSTFNUM_MFNUM_Msk & ((value) << USBHS_HSTFNUM_MFNUM_Pos))
#define USBHS_HSTFNUM_FNUM_Pos                (3U)                                           /**< (USBHS_HSTFNUM) Frame Number Position */
#define USBHS_HSTFNUM_FNUM_Msk                (0x7FFU << USBHS_HSTFNUM_FNUM_Pos)             /**< (USBHS_HSTFNUM) Frame Number Mask */
#define USBHS_HSTFNUM_FNUM(value)             (USBHS_HSTFNUM_FNUM_Msk & ((value) << USBHS_HSTFNUM_FNUM_Pos))
#define USBHS_HSTFNUM_FLENHIGH_Pos            (16U)                                          /**< (USBHS_HSTFNUM) Frame Length Position */
#define USBHS_HSTFNUM_FLENHIGH_Msk            (0xFFU << USBHS_HSTFNUM_FLENHIGH_Pos)          /**< (USBHS_HSTFNUM) Frame Length Mask */
#define USBHS_HSTFNUM_FLENHIGH(value)         (USBHS_HSTFNUM_FLENHIGH_Msk & ((value) << USBHS_HSTFNUM_FLENHIGH_Pos))
#define USBHS_HSTFNUM_Msk                     (0x00FF3FFFUL)                                 /**< (USBHS_HSTFNUM) Register Mask  */

/* -------- USBHS_HSTADDR1 : (USBHS Offset: 0x424) (R/W 32) Host Address 1 Register -------- */
#define USBHS_HSTADDR1_HSTADDRP0_Pos          (0U)                                           /**< (USBHS_HSTADDR1) USB Host Address Position */
#define USBHS_HSTADDR1_HSTADDRP0_Msk          (0x7FU << USBHS_HSTADDR1_HSTADDRP0_Pos)        /**< (USBHS_HSTADDR1) USB Host Address Mask */
#define USBHS_HSTADDR1_HSTADDRP0(value)       (USBHS_HSTADDR1_HSTADDRP0_Msk & ((value) << USBHS_HSTADDR1_HSTADDRP0_Pos))
#define USBHS_HSTADDR1_HSTADDRP1_Pos          (8U)                                           /**< (USBHS_HSTADDR1) USB Host Address Position */
#define USBHS_HSTADDR1_HSTADDRP1_Msk          (0x7FU << USBHS_HSTADDR1_HSTADDRP1_Pos)        /**< (USBHS_HSTADDR1) USB Host Address Mask */
#define USBHS_HSTADDR1_HSTADDRP1(value)       (USBHS_HSTADDR1_HSTADDRP1_Msk & ((value) << USBHS_HSTADDR1_HSTADDRP1_Pos))
#define USBHS_HSTADDR1_HSTADDRP2_Pos          (16U)                                          /**< (USBHS_HSTADDR1) USB Host Address Position */
#define USBHS_HSTADDR1_HSTADDRP2_Msk          (0x7FU << USBHS_HSTADDR1_HSTADDRP2_Pos)        /**< (USBHS_HSTADDR1) USB Host Address Mask */
#define USBHS_HSTADDR1_HSTADDRP2(value)       (USBHS_HSTADDR1_HSTADDRP2_Msk & ((value) << USBHS_HSTADDR1_HSTADDRP2_Pos))
#define USBHS_HSTADDR1_HSTADDRP3_Pos          (24U)                                          /**< (USBHS_HSTADDR1) USB Host Address Position */
#define USBHS_HSTADDR1_HSTADDRP3_Msk          (0x7FU << USBHS_HSTADDR1_HSTADDRP3_Pos)        /**< (USBHS_HSTADDR1) USB Host Address Mask */
#define USBHS_HSTADDR1_HSTADDRP3(value)       (USBHS_HSTADDR1_HSTADDRP3_Msk & ((value) << USBHS_HSTADDR1_HSTADDRP3_Pos))
#define USBHS_HSTADDR1_Msk                    (0x7F7F7F7FUL)                                 /**< (USBHS_HSTADDR1) Register Mask  */

/* -------- USBHS_HSTADDR2 : (USBHS Offset: 0x428) (R/W 32) Host Address 2 Register -------- */
#define USBHS_HSTADDR2_HSTADDRP4_Pos          (0U)                                           /**< (USBHS_HSTADDR2) USB Host Address Position */
#define USBHS_HSTADDR2_HSTADDRP4_Msk          (0x7FU << USBHS_HSTADDR2_HSTADDRP4_Pos)        /**< (USBHS_HSTADDR2) USB Host Address Mask */
#define USBHS_HSTADDR2_HSTADDRP4(value)       (USBHS_HSTADDR2_HSTADDRP4_Msk & ((value) << USBHS_HSTADDR2_HSTADDRP4_Pos))
#define USBHS_HSTADDR2_HSTADDRP5_Pos          (8U)                                           /**< (USBHS_HSTADDR2) USB Host Address Position */
#define USBHS_HSTADDR2_HSTADDRP5_Msk          (0x7FU << USBHS_HSTADDR2_HSTADDRP5_Pos)        /**< (USBHS_HSTADDR2) USB Host Address Mask */
#define USBHS_HSTADDR2_HSTADDRP5(value)       (USBHS_HSTADDR2_HSTADDRP5_Msk & ((value) << USBHS_HSTADDR2_HSTADDRP5_Pos))
#define USBHS_HSTADDR2_HSTADDRP6_Pos          (16U)                                          /**< (USBHS_HSTADDR2) USB Host Address Position */
#define USBHS_HSTADDR2_HSTADDRP6_Msk          (0x7FU << USBHS_HSTADDR2_HSTADDRP6_Pos)        /**< (USBHS_HSTADDR2) USB Host Address Mask */
#define USBHS_HSTADDR2_HSTADDRP6(value)       (USBHS_HSTADDR2_HSTADDRP6_Msk & ((value) << USBHS_HSTADDR2_HSTADDRP6_Pos))
#define USBHS_HSTADDR2_HSTADDRP7_Pos          (24U)                                          /**< (USBHS_HSTADDR2) USB Host Address Position */
#define USBHS_HSTADDR2_HSTADDRP7_Msk          (0x7FU << USBHS_HSTADDR2_HSTADDRP7_Pos)        /**< (USBHS_HSTADDR2) USB Host Address Mask */
#define USBHS_HSTADDR2_HSTADDRP7(value)       (USBHS_HSTADDR2_HSTADDRP7_Msk & ((value) << USBHS_HSTADDR2_HSTADDRP7_Pos))
#define USBHS_HSTADDR2_Msk                    (0x7F7F7F7FUL)                                 /**< (USBHS_HSTADDR2) Register Mask  */

/* -------- USBHS_HSTADDR3 : (USBHS Offset: 0x42C) (R/W 32) Host Address 3 Register -------- */
#define USBHS_HSTADDR3_HSTADDRP8_Pos          (0U)                                           /**< (USBHS_HSTADDR3) USB Host Address Position */
#define USBHS_HSTADDR3_HSTADDRP8_Msk          (0x7FU << USBHS_HSTADDR3_HSTADDRP8_Pos)        /**< (USBHS_HSTADDR3) USB Host Address Mask */
#define USBHS_HSTADDR3_HSTADDRP8(value)       (USBHS_HSTADDR3_HSTADDRP8_Msk & ((value) << USBHS_HSTADDR3_HSTADDRP8_Pos))
#define USBHS_HSTADDR3_HSTADDRP9_Pos          (8U)                                           /**< (USBHS_HSTADDR3) USB Host Address Position */
#define USBHS_HSTADDR3_HSTADDRP9_Msk          (0x7FU << USBHS_HSTADDR3_HSTADDRP9_Pos)        /**< (USBHS_HSTADDR3) USB Host Address Mask */
#define USBHS_HSTADDR3_HSTADDRP9(value)       (USBHS_HSTADDR3_HSTADDRP9_Msk & ((value) << USBHS_HSTADDR3_HSTADDRP9_Pos))
#define USBHS_HSTADDR3_Msk                    (0x00007F7FUL)                                 /**< (USBHS_HSTADDR3) Register Mask  */

/* -------- USBHS_HSTPIPCFG : (USBHS Offset: 0x500) (R/W 32) Host Pipe Configuration Register -------- */
#define USBHS_HSTPIPCFG_ALLOC_Pos             (1U)                                           /**< (USBHS_HSTPIPCFG) Pipe Memory Allocate Position */
#define USBHS_HSTPIPCFG_ALLOC_Msk             (0x1U << USBHS_HSTPIPCFG_ALLOC_Pos)            /**< (USBHS_HSTPIPCFG) Pipe Memory Allocate Mask */
#define USBHS_HSTPIPCFG_ALLOC(value)          (USBHS_HSTPIPCFG_ALLOC_Msk & ((value) << USBHS_HSTPIPCFG_ALLOC_Pos))
#define USBHS_HSTPIPCFG_PBK_Pos               (2U)                                           /**< (USBHS_HSTPIPCFG) Pipe Banks Position */
#define USBHS_HSTPIPCFG_PBK_Msk               (0x3U << USBHS_HSTPIPCFG_PBK_Pos)              /**< (USBHS_HSTPIPCFG) Pipe Banks Mask */
#define USBHS_HSTPIPCFG_PBK(value)            (USBHS_HSTPIPCFG_PBK_Msk & ((value) << USBHS_HSTPIPCFG_PBK_Pos))
#define   USBHS_HSTPIPCFG_PBK_1_BANK_Val      (0U)                                           /**< (USBHS_HSTPIPCFG) Single-bank pipe  */
#define   USBHS_HSTPIPCFG_PBK_2_BANK_Val      (1U)                                           /**< (USBHS_HSTPIPCFG) Double-bank pipe  */
#define   USBHS_HSTPIPCFG_PBK_3_BANK_Val      (2U)                                           /**< (USBHS_HSTPIPCFG) Triple-bank pipe  */
#define USBHS_HSTPIPCFG_PBK_1_BANK            (USBHS_HSTPIPCFG_PBK_1_BANK_Val << USBHS_HSTPIPCFG_PBK_Pos) /**< (USBHS_HSTPIPCFG) Single-bank pipe Position  */
#define USBHS_HSTPIPCFG_PBK_2_BANK            (USBHS_HSTPIPCFG_PBK_2_BANK_Val << USBHS_HSTPIPCFG_PBK_Pos) /**< (USBHS_HSTPIPCFG) Double-bank pipe Position  */
#define USBHS_HSTPIPCFG_PBK_3_BANK            (USBHS_HSTPIPCFG_PBK_3_BANK_Val << USBHS_HSTPIPCFG_PBK_Pos) /**< (USBHS_HSTPIPCFG) Triple-bank pipe Position  */
#define USBHS_HSTPIPCFG_PSIZE_Pos             (4U)                                           /**< (USBHS_HSTPIPCFG) Pipe Size Position */
#define USBHS_HSTPIPCFG_PSIZE_Msk             (0x7U << USBHS_HSTPIPCFG_PSIZE_Pos)            /**< (USBHS_HSTPIPCFG) Pipe Size Mask */
#define USBHS_HSTPIPCFG_PSIZE(value)          (USBHS_HSTPIPCFG_PSIZE_Msk & ((value) << USBHS_HSTPIPCFG_PSIZE_Pos))
#define   USBHS_HSTPIPCFG_PSIZE_8_BYTE_Val    (0U)                                           /**< (USBHS_HSTPIPCFG) 8 bytes  */
#define   USBHS_HSTPIPCFG_PSIZE_16_BYTE_Val   (1U)                                           /**< (USBHS_HSTPIPCFG) 16 bytes  */
#define   USBHS_HSTPIPCFG_PSIZE_32_BYTE_Val   (2U)                                           /**< (USBHS_HSTPIPCFG) 32 bytes  */
#define   USBHS_HSTPIPCFG_PSIZE_64_BYTE_Val   (3U)                                           /**< (USBHS_HSTPIPCFG) 64 bytes  */
#define   USBHS_HSTPIPCFG_PSIZE_128_BYTE_Val  (4U)                                           /**< (USBHS_HSTPIPCFG) 128 bytes  */
#define   USBHS_HSTPIPCFG_PSIZE_256_BYTE_Val  (5U)                                           /**< (USBHS_HSTPIPCFG) 256 bytes  */
#define   USBHS_HSTPIPCFG_PSIZE_512_BYTE_Val  (6U)                                           /**< (USBHS_HSTPIPCFG) 512 bytes  */
#define   USBHS_HSTPIPCFG_PSIZE_1024_BYTE_Val (7U)                                           /**< (USBHS_HSTPIPCFG) 1024 bytes  */
#define USBHS_HSTPIPCFG_PSIZE_8_BYTE          (USBHS_HSTPIPCFG_PSIZE_8_BYTE_Val << USBHS_HSTPIPCFG_PSIZE_Pos) /**< (USBHS_HSTPIPCFG) 8 bytes Position  */
#define USBHS_HSTPIPCFG_PSIZE_16_BYTE         (USBHS_HSTPIPCFG_PSIZE_16_BYTE_Val << USBHS_HSTPIPCFG_PSIZE_Pos) /**< (USBHS_HSTPIPCFG) 16 bytes Position  */
#define USBHS_HSTPIPCFG_PSIZE_32_BYTE         (USBHS_HSTPIPCFG_PSIZE_32_BYTE_Val << USBHS_HSTPIPCFG_PSIZE_Pos) /**< (USBHS_HSTPIPCFG) 32 bytes Position  */
#define USBHS_HSTPIPCFG_PSIZE_64_BYTE         (USBHS_HSTPIPCFG_PSIZE_64_BYTE_Val << USBHS_HSTPIPCFG_PSIZE_Pos) /**< (USBHS_HSTPIPCFG) 64 bytes Position  */
#define USBHS_HSTPIPCFG_PSIZE_128_BYTE        (USBHS_HSTPIPCFG_PSIZE_128_BYTE_Val << USBHS_HSTPIPCFG_PSIZE_Pos) /**< (USBHS_HSTPIPCFG) 128 bytes Position  */
#define USBHS_HSTPIPCFG_PSIZE_256_BYTE        (USBHS_HSTPIPCFG_PSIZE_256_BYTE_Val << USBHS_HSTPIPCFG_PSIZE_Pos) /**< (USBHS_HSTPIPCFG) 256 bytes Position  */
#define USBHS_HSTPIPCFG_PSIZE_512_BYTE        (USBHS_HSTPIPCFG_PSIZE_512_BYTE_Val << USBHS_HSTPIPCFG_PSIZE_Pos) /**< (USBHS_HSTPIPCFG) 512 bytes Position  */
#define USBHS_HSTPIPCFG_PSIZE_1024_BYTE       (USBHS_HSTPIPCFG_PSIZE_1024_BYTE_Val << USBHS_HSTPIPCFG_PSIZE_Pos) /**< (USBHS_HSTPIPCFG) 1024 bytes Position  */
#define USBHS_HSTPIPCFG_PTOKEN_Pos            (8U)                                           /**< (USBHS_HSTPIPCFG) Pipe Token Position */
#define USBHS_HSTPIPCFG_PTOKEN_Msk            (0x3U << USBHS_HSTPIPCFG_PTOKEN_Pos)           /**< (USBHS_HSTPIPCFG) Pipe Token Mask */
#define USBHS_HSTPIPCFG_PTOKEN(value)         (USBHS_HSTPIPCFG_PTOKEN_Msk & ((value) << USBHS_HSTPIPCFG_PTOKEN_Pos))
#define   USBHS_HSTPIPCFG_PTOKEN_SETUP_Val    (0U)                                           /**< (USBHS_HSTPIPCFG) SETUP  */
#define   USBHS_HSTPIPCFG_PTOKEN_IN_Val       (1U)                                           /**< (USBHS_HSTPIPCFG) IN  */
#define   USBHS_HSTPIPCFG_PTOKEN_OUT_Val      (2U)                                           /**< (USBHS_HSTPIPCFG) OUT  */
#define USBHS_HSTPIPCFG_PTOKEN_SETUP          (USBHS_HSTPIPCFG_PTOKEN_SETUP_Val << USBHS_HSTPIPCFG_PTOKEN_Pos) /**< (USBHS_HSTPIPCFG) SETUP Position  */
#define USBHS_HSTPIPCFG_PTOKEN_IN             (USBHS_HSTPIPCFG_PTOKEN_IN_Val << USBHS_HSTPIPCFG_PTOKEN_Pos) /**< (USBHS_HSTPIPCFG) IN Position  */
#define USBHS_HSTPIPCFG_PTOKEN_OUT            (USBHS_HSTPIPCFG_PTOKEN_OUT_Val << USBHS_HSTPIPCFG_PTOKEN_Pos) /**< (USBHS_HSTPIPCFG) OUT Position  */
#define USBHS_HSTPIPCFG_AUTOSW_Pos            (10U)                                          /**< (USBHS_HSTPIPCFG) Automatic Switch Position */
#define USBHS_HSTPIPCFG_AUTOSW_Msk            (0x1U << USBHS_HSTPIPCFG_AUTOSW_Pos)           /**< (USBHS_HSTPIPCFG) Automatic Switch Mask */
#define USBHS_HSTPIPCFG_AUTOSW(value)         (USBHS_HSTPIPCFG_AUTOSW_Msk & ((value) << USBHS_HSTPIPCFG_AUTOSW_Pos))
#define USBHS_HSTPIPCFG_PTYPE_Pos             (12U)                                          /**< (USBHS_HSTPIPCFG) Pipe Type Position */
#define USBHS_HSTPIPCFG_PTYPE_Msk             (0x3U << USBHS_HSTPIPCFG_PTYPE_Pos)            /**< (USBHS_HSTPIPCFG) Pipe Type Mask */
#define USBHS_HSTPIPCFG_PTYPE(value)          (USBHS_HSTPIPCFG_PTYPE_Msk & ((value) << USBHS_HSTPIPCFG_PTYPE_Pos))
#define   USBHS_HSTPIPCFG_PTYPE_CTRL_Val      (0U)                                           /**< (USBHS_HSTPIPCFG) Control  */
#define   USBHS_HSTPIPCFG_PTYPE_ISO_Val       (1U)                                           /**< (USBHS_HSTPIPCFG) Isochronous  */
#define   USBHS_HSTPIPCFG_PTYPE_BLK_Val       (2U)                                           /**< (USBHS_HSTPIPCFG) Bulk  */
#define   USBHS_HSTPIPCFG_PTYPE_INTRPT_Val    (3U)                                           /**< (USBHS_HSTPIPCFG) Interrupt  */
#define USBHS_HSTPIPCFG_PTYPE_CTRL            (USBHS_HSTPIPCFG_PTYPE_CTRL_Val << USBHS_HSTPIPCFG_PTYPE_Pos) /**< (USBHS_HSTPIPCFG) Control Position  */
#define USBHS_HSTPIPCFG_PTYPE_ISO             (USBHS_HSTPIPCFG_PTYPE_ISO_Val << USBHS_HSTPIPCFG_PTYPE_Pos) /**< (USBHS_HSTPIPCFG) Isochronous Position  */
#define USBHS_HSTPIPCFG_PTYPE_BLK             (USBHS_HSTPIPCFG_PTYPE_BLK_Val << USBHS_HSTPIPCFG_PTYPE_Pos) /**< (USBHS_HSTPIPCFG) Bulk Position  */
#define USBHS_HSTPIPCFG_PTYPE_INTRPT          (USBHS_HSTPIPCFG_PTYPE_INTRPT_Val << USBHS_HSTPIPCFG_PTYPE_Pos) /**< (USBHS_HSTPIPCFG) Interrupt Position  */
#define USBHS_HSTPIPCFG_PEPNUM_Pos            (16U)                                          /**< (USBHS_HSTPIPCFG) Pipe Endpoint Number Position */
#define USBHS_HSTPIPCFG_PEPNUM_Msk            (0xFU << USBHS_HSTPIPCFG_PEPNUM_Pos)           /**< (USBHS_HSTPIPCFG) Pipe Endpoint Number Mask */
#define USBHS_HSTPIPCFG_PEPNUM(value)         (USBHS_HSTPIPCFG_PEPNUM_Msk & ((value) << USBHS_HSTPIPCFG_PEPNUM_Pos))
#define USBHS_HSTPIPCFG_PINGEN_Pos            (20U)                                          /**< (USBHS_HSTPIPCFG) Ping Enable Position */
#define USBHS_HSTPIPCFG_PINGEN_Msk            (0x1U << USBHS_HSTPIPCFG_PINGEN_Pos)           /**< (USBHS_HSTPIPCFG) Ping Enable Mask */
#define USBHS_HSTPIPCFG_PINGEN(value)         (USBHS_HSTPIPCFG_PINGEN_Msk & ((value) << USBHS_HSTPIPCFG_PINGEN_Pos))
#define USBHS_HSTPIPCFG_INTFRQ_Pos            (24U)                                          /**< (USBHS_HSTPIPCFG) Pipe Interrupt Request Frequency Position */
#define USBHS_HSTPIPCFG_INTFRQ_Msk            (0xFFU << USBHS_HSTPIPCFG_INTFRQ_Pos)          /**< (USBHS_HSTPIPCFG) Pipe Interrupt Request Frequency Mask */
#define USBHS_HSTPIPCFG_INTFRQ(value)         (USBHS_HSTPIPCFG_INTFRQ_Msk & ((value) << USBHS_HSTPIPCFG_INTFRQ_Pos))
#define USBHS_HSTPIPCFG_BINTERVAL_Pos         (24U)                                          /**< (USBHS_HSTPIPCFG) bInterval Parameter for the Bulk-Out/Ping Transaction Position */
#define USBHS_HSTPIPCFG_BINTERVAL_Msk         (0xFFU << USBHS_HSTPIPCFG_BINTERVAL_Pos)       /**< (USBHS_HSTPIPCFG) bInterval Parameter for the Bulk-Out/Ping Transaction Mask */
#define USBHS_HSTPIPCFG_BINTERVAL(value)      (USBHS_HSTPIPCFG_BINTERVAL_Msk & ((value) << USBHS_HSTPIPCFG_BINTERVAL_Pos))
#define USBHS_HSTPIPCFG_Msk                   (0xFF1F377EUL)                                 /**< (USBHS_HSTPIPCFG) Register Mask  */

/* -------- USBHS_HSTPIPISR : (USBHS Offset: 0x530) (R/  32) Host Pipe Status Register -------- */
#define USBHS_HSTPIPISR_RXINI_Pos             (0U)                                           /**< (USBHS_HSTPIPISR) Received IN Data Interrupt Position */
#define USBHS_HSTPIPISR_RXINI_Msk             (0x1U << USBHS_HSTPIPISR_RXINI_Pos)            /**< (USBHS_HSTPIPISR) Received IN Data Interrupt Mask */
#define USBHS_HSTPIPISR_RXINI(value)          (USBHS_HSTPIPISR_RXINI_Msk & ((value) << USBHS_HSTPIPISR_RXINI_Pos))
#define USBHS_HSTPIPISR_TXOUTI_Pos            (1U)                                           /**< (USBHS_HSTPIPISR) Transmitted OUT Data Interrupt Position */
#define USBHS_HSTPIPISR_TXOUTI_Msk            (0x1U << USBHS_HSTPIPISR_TXOUTI_Pos)           /**< (USBHS_HSTPIPISR) Transmitted OUT Data Interrupt Mask */
#define USBHS_HSTPIPISR_TXOUTI(value)         (USBHS_HSTPIPISR_TXOUTI_Msk & ((value) << USBHS_HSTPIPISR_TXOUTI_Pos))
#define USBHS_HSTPIPISR_TXSTPI_Pos            (2U)                                           /**< (USBHS_HSTPIPISR) Transmitted SETUP Interrupt Position */
#define USBHS_HSTPIPISR_TXSTPI_Msk            (0x1U << USBHS_HSTPIPISR_TXSTPI_Pos)           /**< (USBHS_HSTPIPISR) Transmitted SETUP Interrupt Mask */
#define USBHS_HSTPIPISR_TXSTPI(value)         (USBHS_HSTPIPISR_TXSTPI_Msk & ((value) << USBHS_HSTPIPISR_TXSTPI_Pos))
#define USBHS_HSTPIPISR_UNDERFI_Pos           (2U)                                           /**< (USBHS_HSTPIPISR) Underflow Interrupt Position */
#define USBHS_HSTPIPISR_UNDERFI_Msk           (0x1U << USBHS_HSTPIPISR_UNDERFI_Pos)          /**< (USBHS_HSTPIPISR) Underflow Interrupt Mask */
#define USBHS_HSTPIPISR_UNDERFI(value)        (USBHS_HSTPIPISR_UNDERFI_Msk & ((value) << USBHS_HSTPIPISR_UNDERFI_Pos))
#define USBHS_HSTPIPISR_PERRI_Pos             (3U)                                           /**< (USBHS_HSTPIPISR) Pipe Error Interrupt Position */
#define USBHS_HSTPIPISR_PERRI_Msk             (0x1U << USBHS_HSTPIPISR_PERRI_Pos)            /**< (USBHS_HSTPIPISR) Pipe Error Interrupt Mask */
#define USBHS_HSTPIPISR_PERRI(value)          (USBHS_HSTPIPISR_PERRI_Msk & ((value) << USBHS_HSTPIPISR_PERRI_Pos))
#define USBHS_HSTPIPISR_NAKEDI_Pos            (4U)                                           /**< (USBHS_HSTPIPISR) NAKed Interrupt Position */
#define USBHS_HSTPIPISR_NAKEDI_Msk            (0x1U << USBHS_HSTPIPISR_NAKEDI_Pos)           /**< (USBHS_HSTPIPISR) NAKed Interrupt Mask */
#define USBHS_HSTPIPISR_NAKEDI(value)         (USBHS_HSTPIPISR_NAKEDI_Msk & ((value) << USBHS_HSTPIPISR_NAKEDI_Pos))
#define USBHS_HSTPIPISR_OVERFI_Pos            (5U)                                           /**< (USBHS_HSTPIPISR) Overflow Interrupt Position */
#define USBHS_HSTPIPISR_OVERFI_Msk            (0x1U << USBHS_HSTPIPISR_OVERFI_Pos)           /**< (USBHS_HSTPIPISR) Overflow Interrupt Mask */
#define USBHS_HSTPIPISR_OVERFI(value)         (USBHS_HSTPIPISR_OVERFI_Msk & ((value) << USBHS_HSTPIPISR_OVERFI_Pos))
#define USBHS_HSTPIPISR_RXSTALLDI_Pos         (6U)                                           /**< (USBHS_HSTPIPISR) Received STALLed Interrupt Position */
#define USBHS_HSTPIPISR_RXSTALLDI_Msk         (0x1U << USBHS_HSTPIPISR_RXSTALLDI_Pos)        /**< (USBHS_HSTPIPISR) Received STALLed Interrupt Mask */
#define USBHS_HSTPIPISR_RXSTALLDI(value)      (USBHS_HSTPIPISR_RXSTALLDI_Msk & ((value) << USBHS_HSTPIPISR_RXSTALLDI_Pos))
#define USBHS_HSTPIPISR_CRCERRI_Pos           (6U)                                           /**< (USBHS_HSTPIPISR) CRC Error Interrupt Position */
#define USBHS_HSTPIPISR_CRCERRI_Msk           (0x1U << USBHS_HSTPIPISR_CRCERRI_Pos)          /**< (USBHS_HSTPIPISR) CRC Error Interrupt Mask */
#define USBHS_HSTPIPISR_CRCERRI(value)        (USBHS_HSTPIPISR_CRCERRI_Msk & ((value) << USBHS_HSTPIPISR_CRCERRI_Pos))
#define USBHS_HSTPIPISR_SHORTPACKETI_Pos      (7U)                                           /**< (USBHS_HSTPIPISR) Short Packet Interrupt Position */
#define USBHS_HSTPIPISR_SHORTPACKETI_Msk      (0x1U << USBHS_HSTPIPISR_SHORTPACKETI_Pos)     /**< (USBHS_HSTPIPISR) Short Packet Interrupt Mask */
#define USBHS_HSTPIPISR_SHORTPACKETI(value)   (USBHS_HSTPIPISR_SHORTPACKETI_Msk & ((value) << USBHS_HSTPIPISR_SHORTPACKETI_Pos))
#define USBHS_HSTPIPISR_DTSEQ_Pos             (8U)                                           /**< (USBHS_HSTPIPISR) Data Toggle Sequence Position */
#define USBHS_HSTPIPISR_DTSEQ_Msk             (0x3U << USBHS_HSTPIPISR_DTSEQ_Pos)            /**< (USBHS_HSTPIPISR) Data Toggle Sequence Mask */
#define USBHS_HSTPIPISR_DTSEQ(value)          (USBHS_HSTPIPISR_DTSEQ_Msk & ((value) << USBHS_HSTPIPISR_DTSEQ_Pos))
#define   USBHS_HSTPIPISR_DTSEQ_DATA0_Val     (0U)                                           /**< (USBHS_HSTPIPISR) Data0 toggle sequence  */
#define   USBHS_HSTPIPISR_DTSEQ_DATA1_Val     (1U)                                           /**< (USBHS_HSTPIPISR) Data1 toggle sequence  */
#define USBHS_HSTPIPISR_DTSEQ_DATA0           (USBHS_HSTPIPISR_DTSEQ_DATA0_Val << USBHS_HSTPIPISR_DTSEQ_Pos) /**< (USBHS_HSTPIPISR) Data0 toggle sequence Position  */
#define USBHS_HSTPIPISR_DTSEQ_DATA1           (USBHS_HSTPIPISR_DTSEQ_DATA1_Val << USBHS_HSTPIPISR_DTSEQ_Pos) /**< (USBHS_HSTPIPISR) Data1 toggle sequence Position  */
#define USBHS_HSTPIPISR_NBUSYBK_Pos           (12U)                                          /**< (USBHS_HSTPIPISR) Number of Busy Banks Position */
#define USBHS_HSTPIPISR_NBUSYBK_Msk           (0x3U << USBHS_HSTPIPISR_NBUSYBK_Pos)          /**< (USBHS_HSTPIPISR) Number of Busy Banks Mask */
#define USBHS_HSTPIPISR_NBUSYBK(value)        (USBHS_HSTPIPISR_NBUSYBK_Msk & ((value) << USBHS_HSTPIPISR_NBUSYBK_Pos))
#define   USBHS_HSTPIPISR_NBUSYBK_0_BUSY_Val  (0U)                                           /**< (USBHS_HSTPIPISR) 0 busy bank (all banks free)  */
#define   USBHS_HSTPIPISR_NBUSYBK_1_BUSY_Val  (1U)                                           /**< (USBHS_HSTPIPISR) 1 busy bank  */
#define   USBHS_HSTPIPISR_NBUSYBK_2_BUSY_Val  (2U)                                           /**< (USBHS_HSTPIPISR) 2 busy banks  */
#define   USBHS_HSTPIPISR_NBUSYBK_3_BUSY_Val  (3U)                                           /**< (USBHS_HSTPIPISR) 3 busy banks  */
#define USBHS_HSTPIPISR_NBUSYBK_0_BUSY        (USBHS_HSTPIPISR_NBUSYBK_0_BUSY_Val << USBHS_HSTPIPISR_NBUSYBK_Pos) /**< (USBHS_HSTPIPISR) 0 busy bank (all banks free) Position  */
#define USBHS_HSTPIPISR_NBUSYBK_1_BUSY        (USBHS_HSTPIPISR_NBUSYBK_1_BUSY_Val << USBHS_HSTPIPISR_NBUSYBK_Pos) /**< (USBHS_HSTPIPISR) 1 busy bank Position  */
#define USBHS_HSTPIPISR_NBUSYBK_2_BUSY        (USBHS_HSTPIPISR_NBUSYBK_2_BUSY_Val << USBHS_HSTPIPISR_NBUSYBK_Pos) /**< (USBHS_HSTPIPISR) 2 busy banks Position  */
#define USBHS_HSTPIPISR_NBUSYBK_3_BUSY        (USBHS_HSTPIPISR_NBUSYBK_3_BUSY_Val << USBHS_HSTPIPISR_NBUSYBK_Pos) /**< (USBHS_HSTPIPISR) 3 busy banks Position  */
#define USBHS_HSTPIPISR_CURRBK_Pos            (14U)                                          /**< (USBHS_HSTPIPISR) Current Bank Position */
#define USBHS_HSTPIPISR_CURRBK_Msk            (0x3U << USBHS_HSTPIPISR_CURRBK_Pos)           /**< (USBHS_HSTPIPISR) Current Bank Mask */
#define USBHS_HSTPIPISR_CURRBK(value)         (USBHS_HSTPIPISR_CURRBK_Msk & ((value) << USBHS_HSTPIPISR_CURRBK_Pos))
#define   USBHS_HSTPIPISR_CURRBK_BANK0_Val    (0U)                                           /**< (USBHS_HSTPIPISR) Current bank is bank0  */
#define   USBHS_HSTPIPISR_CURRBK_BANK1_Val    (1U)                                           /**< (USBHS_HSTPIPISR) Current bank is bank1  */
#define   USBHS_HSTPIPISR_CURRBK_BANK2_Val    (2U)                                           /**< (USBHS_HSTPIPISR) Current bank is bank2  */
#define USBHS_HSTPIPISR_CURRBK_BANK0          (USBHS_HSTPIPISR_CURRBK_BANK0_Val << USBHS_HSTPIPISR_CURRBK_Pos) /**< (USBHS_HSTPIPISR) Current bank is bank0 Position  */
#define USBHS_HSTPIPISR_CURRBK_BANK1          (USBHS_HSTPIPISR_CURRBK_BANK1_Val << USBHS_HSTPIPISR_CURRBK_Pos) /**< (USBHS_HSTPIPISR) Current bank is bank1 Position  */
#define USBHS_HSTPIPISR_CURRBK_BANK2          (USBHS_HSTPIPISR_CURRBK_BANK2_Val << USBHS_HSTPIPISR_CURRBK_Pos) /**< (USBHS_HSTPIPISR) Current bank is bank2 Position  */
#define USBHS_HSTPIPISR_RWALL_Pos             (16U)                                          /**< (USBHS_HSTPIPISR) Read/Write Allowed Position */
#define USBHS_HSTPIPISR_RWALL_Msk             (0x1U << USBHS_HSTPIPISR_RWALL_Pos)            /**< (USBHS_HSTPIPISR) Read/Write Allowed Mask */
#define USBHS_HSTPIPISR_RWALL(value)          (USBHS_HSTPIPISR_RWALL_Msk & ((value) << USBHS_HSTPIPISR_RWALL_Pos))
#define USBHS_HSTPIPISR_CFGOK_Pos             (18U)                                          /**< (USBHS_HSTPIPISR) Configuration OK Status Position */
#define USBHS_HSTPIPISR_CFGOK_Msk             (0x1U << USBHS_HSTPIPISR_CFGOK_Pos)            /**< (USBHS_HSTPIPISR) Configuration OK Status Mask */
#define USBHS_HSTPIPISR_CFGOK(value)          (USBHS_HSTPIPISR_CFGOK_Msk & ((value) << USBHS_HSTPIPISR_CFGOK_Pos))
#define USBHS_HSTPIPISR_PBYCT_Pos             (20U)                                          /**< (USBHS_HSTPIPISR) Pipe Byte Count Position */
#define USBHS_HSTPIPISR_PBYCT_Msk             (0x7FFU << USBHS_HSTPIPISR_PBYCT_Pos)          /**< (USBHS_HSTPIPISR) Pipe Byte Count Mask */
#define USBHS_HSTPIPISR_PBYCT(value)          (USBHS_HSTPIPISR_PBYCT_Msk & ((value) << USBHS_HSTPIPISR_PBYCT_Pos))
#define USBHS_HSTPIPISR_Msk                   (0x7FF5F3FFUL)                                 /**< (USBHS_HSTPIPISR) Register Mask  */

/* -------- USBHS_HSTPIPICR : (USBHS Offset: 0x560) ( /W 32) Host Pipe Clear Register -------- */
#define USBHS_HSTPIPICR_RXINIC_Pos            (0U)                                           /**< (USBHS_HSTPIPICR) Received IN Data Interrupt Clear Position */
#define USBHS_HSTPIPICR_RXINIC_Msk            (0x1U << USBHS_HSTPIPICR_RXINIC_Pos)           /**< (USBHS_HSTPIPICR) Received IN Data Interrupt Clear Mask */
#define USBHS_HSTPIPICR_RXINIC(value)         (USBHS_HSTPIPICR_RXINIC_Msk & ((value) << USBHS_HSTPIPICR_RXINIC_Pos))
#define USBHS_HSTPIPICR_TXOUTIC_Pos           (1U)                                           /**< (USBHS_HSTPIPICR) Transmitted OUT Data Interrupt Clear Position */
#define USBHS_HSTPIPICR_TXOUTIC_Msk           (0x1U << USBHS_HSTPIPICR_TXOUTIC_Pos)          /**< (USBHS_HSTPIPICR) Transmitted OUT Data Interrupt Clear Mask */
#define USBHS_HSTPIPICR_TXOUTIC(value)        (USBHS_HSTPIPICR_TXOUTIC_Msk & ((value) << USBHS_HSTPIPICR_TXOUTIC_Pos))
#define USBHS_HSTPIPICR_TXSTPIC_Pos           (2U)                                           /**< (USBHS_HSTPIPICR) Transmitted SETUP Interrupt Clear Position */
#define USBHS_HSTPIPICR_TXSTPIC_Msk           (0x1U << USBHS_HSTPIPICR_TXSTPIC_Pos)          /**< (USBHS_HSTPIPICR) Transmitted SETUP Interrupt Clear Mask */
#define USBHS_HSTPIPICR_TXSTPIC(value)        (USBHS_HSTPIPICR_TXSTPIC_Msk & ((value) << USBHS_HSTPIPICR_TXSTPIC_Pos))
#define USBHS_HSTPIPICR_UNDERFIC_Pos          (2U)                                           /**< (USBHS_HSTPIPICR) Underflow Interrupt Clear Position */
#define USBHS_HSTPIPICR_UNDERFIC_Msk          (0x1U << USBHS_HSTPIPICR_UNDERFIC_Pos)         /**< (USBHS_HSTPIPICR) Underflow Interrupt Clear Mask */
#define USBHS_HSTPIPICR_UNDERFIC(value)       (USBHS_HSTPIPICR_UNDERFIC_Msk & ((value) << USBHS_HSTPIPICR_UNDERFIC_Pos))
#define USBHS_HSTPIPICR_NAKEDIC_Pos           (4U)                                           /**< (USBHS_HSTPIPICR) NAKed Interrupt Clear Position */
#define USBHS_HSTPIPICR_NAKEDIC_Msk           (0x1U << USBHS_HSTPIPICR_NAKEDIC_Pos)          /**< (USBHS_HSTPIPICR) NAKed Interrupt Clear Mask */
#define USBHS_HSTPIPICR_NAKEDIC(value)        (USBHS_HSTPIPICR_NAKEDIC_Msk & ((value) << USBHS_HSTPIPICR_NAKEDIC_Pos))
#define USBHS_HSTPIPICR_OVERFIC_Pos           (5U)                                           /**< (USBHS_HSTPIPICR) Overflow Interrupt Clear Position */
#define USBHS_HSTPIPICR_OVERFIC_Msk           (0x1U << USBHS_HSTPIPICR_OVERFIC_Pos)          /**< (USBHS_HSTPIPICR) Overflow Interrupt Clear Mask */
#define USBHS_HSTPIPICR_OVERFIC(value)        (USBHS_HSTPIPICR_OVERFIC_Msk & ((value) << USBHS_HSTPIPICR_OVERFIC_Pos))
#define USBHS_HSTPIPICR_RXSTALLDIC_Pos        (6U)                                           /**< (USBHS_HSTPIPICR) Received STALLed Interrupt Clear Position */
#define USBHS_HSTPIPICR_RXSTALLDIC_Msk        (0x1U << USBHS_HSTPIPICR_RXSTALLDIC_Pos)       /**< (USBHS_HSTPIPICR) Received STALLed Interrupt Clear Mask */
#define USBHS_HSTPIPICR_RXSTALLDIC(value)     (USBHS_HSTPIPICR_RXSTALLDIC_Msk & ((value) << USBHS_HSTPIPICR_RXSTALLDIC_Pos))
#define USBHS_HSTPIPICR_CRCERRIC_Pos          (6U)                                           /**< (USBHS_HSTPIPICR) CRC Error Interrupt Clear Position */
#define USBHS_HSTPIPICR_CRCERRIC_Msk          (0x1U << USBHS_HSTPIPICR_CRCERRIC_Pos)         /**< (USBHS_HSTPIPICR) CRC Error Interrupt Clear Mask */
#define USBHS_HSTPIPICR_CRCERRIC(value)       (USBHS_HSTPIPICR_CRCERRIC_Msk & ((value) << USBHS_HSTPIPICR_CRCERRIC_Pos))
#define USBHS_HSTPIPICR_SHORTPACKETIC_Pos     (7U)                                           /**< (USBHS_HSTPIPICR) Short Packet Interrupt Clear Position */
#define USBHS_HSTPIPICR_SHORTPACKETIC_Msk     (0x1U << USBHS_HSTPIPICR_SHORTPACKETIC_Pos)    /**< (USBHS_HSTPIPICR) Short Packet Interrupt Clear Mask */
#define USBHS_HSTPIPICR_SHORTPACKETIC(value)  (USBHS_HSTPIPICR_SHORTPACKETIC_Msk & ((value) << USBHS_HSTPIPICR_SHORTPACKETIC_Pos))
#define USBHS_HSTPIPICR_Msk                   (0x000000F7UL)                                 /**< (USBHS_HSTPIPICR) Register Mask  */

/* -------- USBHS_HSTPIPIFR : (USBHS Offset: 0x590) ( /W 32) Host Pipe Set Register -------- */
#define USBHS_HSTPIPIFR_RXINIS_Pos            (0U)                                           /**< (USBHS_HSTPIPIFR) Received IN Data Interrupt Set Position */
#define USBHS_HSTPIPIFR_RXINIS_Msk            (0x1U << USBHS_HSTPIPIFR_RXINIS_Pos)           /**< (USBHS_HSTPIPIFR) Received IN Data Interrupt Set Mask */
#define USBHS_HSTPIPIFR_RXINIS(value)         (USBHS_HSTPIPIFR_RXINIS_Msk & ((value) << USBHS_HSTPIPIFR_RXINIS_Pos))
#define USBHS_HSTPIPIFR_TXOUTIS_Pos           (1U)                                           /**< (USBHS_HSTPIPIFR) Transmitted OUT Data Interrupt Set Position */
#define USBHS_HSTPIPIFR_TXOUTIS_Msk           (0x1U << USBHS_HSTPIPIFR_TXOUTIS_Pos)          /**< (USBHS_HSTPIPIFR) Transmitted OUT Data Interrupt Set Mask */
#define USBHS_HSTPIPIFR_TXOUTIS(value)        (USBHS_HSTPIPIFR_TXOUTIS_Msk & ((value) << USBHS_HSTPIPIFR_TXOUTIS_Pos))
#define USBHS_HSTPIPIFR_TXSTPIS_Pos           (2U)                                           /**< (USBHS_HSTPIPIFR) Transmitted SETUP Interrupt Set Position */
#define USBHS_HSTPIPIFR_TXSTPIS_Msk           (0x1U << USBHS_HSTPIPIFR_TXSTPIS_Pos)          /**< (USBHS_HSTPIPIFR) Transmitted SETUP Interrupt Set Mask */
#define USBHS_HSTPIPIFR_TXSTPIS(value)        (USBHS_HSTPIPIFR_TXSTPIS_Msk & ((value) << USBHS_HSTPIPIFR_TXSTPIS_Pos))
#define USBHS_HSTPIPIFR_UNDERFIS_Pos          (2U)                                           /**< (USBHS_HSTPIPIFR) Underflow Interrupt Set Position */
#define USBHS_HSTPIPIFR_UNDERFIS_Msk          (0x1U << USBHS_HSTPIPIFR_UNDERFIS_Pos)         /**< (USBHS_HSTPIPIFR) Underflow Interrupt Set Mask */
#define USBHS_HSTPIPIFR_UNDERFIS(value)       (USBHS_HSTPIPIFR_UNDERFIS_Msk & ((value) << USBHS_HSTPIPIFR_UNDERFIS_Pos))
#define USBHS_HSTPIPIFR_PERRIS_Pos            (3U)                                           /**< (USBHS_HSTPIPIFR) Pipe Error Interrupt Set Position */
#define USBHS_HSTPIPIFR_PERRIS_Msk            (0x1U << USBHS_HSTPIPIFR_PERRIS_Pos)           /**< (USBHS_HSTPIPIFR) Pipe Error Interrupt Set Mask */
#define USBHS_HSTPIPIFR_PERRIS(value)         (USBHS_HSTPIPIFR_PERRIS_Msk & ((value) << USBHS_HSTPIPIFR_PERRIS_Pos))
#define USBHS_HSTPIPIFR_NAKEDIS_Pos           (4U)                                           /**< (USBHS_HSTPIPIFR) NAKed Interrupt Set Position */
#define USBHS_HSTPIPIFR_NAKEDIS_Msk           (0x1U << USBHS_HSTPIPIFR_NAKEDIS_Pos)          /**< (USBHS_HSTPIPIFR) NAKed Interrupt Set Mask */
#define USBHS_HSTPIPIFR_NAKEDIS(value)        (USBHS_HSTPIPIFR_NAKEDIS_Msk & ((value) << USBHS_HSTPIPIFR_NAKEDIS_Pos))
#define USBHS_HSTPIPIFR_OVERFIS_Pos           (5U)                                           /**< (USBHS_HSTPIPIFR) Overflow Interrupt Set Position */
#define USBHS_HSTPIPIFR_OVERFIS_Msk           (0x1U << USBHS_HSTPIPIFR_OVERFIS_Pos)          /**< (USBHS_HSTPIPIFR) Overflow Interrupt Set Mask */
#define USBHS_HSTPIPIFR_OVERFIS(value)        (USBHS_HSTPIPIFR_OVERFIS_Msk & ((value) << USBHS_HSTPIPIFR_OVERFIS_Pos))
#define USBHS_HSTPIPIFR_RXSTALLDIS_Pos        (6U)                                           /**< (USBHS_HSTPIPIFR) Received STALLed Interrupt Set Position */
#define USBHS_HSTPIPIFR_RXSTALLDIS_Msk        (0x1U << USBHS_HSTPIPIFR_RXSTALLDIS_Pos)       /**< (USBHS_HSTPIPIFR) Received STALLed Interrupt Set Mask */
#define USBHS_HSTPIPIFR_RXSTALLDIS(value)     (USBHS_HSTPIPIFR_RXSTALLDIS_Msk & ((value) << USBHS_HSTPIPIFR_RXSTALLDIS_Pos))
#define USBHS_HSTPIPIFR_CRCERRIS_Pos          (6U)                                           /**< (USBHS_HSTPIPIFR) CRC Error Interrupt Set Position */
#define USBHS_HSTPIPIFR_CRCERRIS_Msk          (0x1U << USBHS_HSTPIPIFR_CRCERRIS_Pos)         /**< (USBHS_HSTPIPIFR) CRC Error Interrupt Set Mask */
#define USBHS_HSTPIPIFR_CRCERRIS(value)       (USBHS_HSTPIPIFR_CRCERRIS_Msk & ((value) << USBHS_HSTPIPIFR_CRCERRIS_Pos))
#define USBHS_HSTPIPIFR_SHORTPACKETIS_Pos     (7U)                                           /**< (USBHS_HSTPIPIFR) Short Packet Interrupt Set Position */
#define USBHS_HSTPIPIFR_SHORTPACKETIS_Msk     (0x1U << USBHS_HSTPIPIFR_SHORTPACKETIS_Pos)    /**< (USBHS_HSTPIPIFR) Short Packet Interrupt Set Mask */
#define USBHS_HSTPIPIFR_SHORTPACKETIS(value)  (USBHS_HSTPIPIFR_SHORTPACKETIS_Msk & ((value) << USBHS_HSTPIPIFR_SHORTPACKETIS_Pos))
#define USBHS_HSTPIPIFR_NBUSYBKS_Pos          (12U)                                          /**< (USBHS_HSTPIPIFR) Number of Busy Banks Set Position */
#define USBHS_HSTPIPIFR_NBUSYBKS_Msk          (0x1U << USBHS_HSTPIPIFR_NBUSYBKS_Pos)         /**< (USBHS_HSTPIPIFR) Number of Busy Banks Set Mask */
#define USBHS_HSTPIPIFR_NBUSYBKS(value)       (USBHS_HSTPIPIFR_NBUSYBKS_Msk & ((value) << USBHS_HSTPIPIFR_NBUSYBKS_Pos))
#define USBHS_HSTPIPIFR_Msk                   (0x000010FFUL)                                 /**< (USBHS_HSTPIPIFR) Register Mask  */

/* -------- USBHS_HSTPIPIMR : (USBHS Offset: 0x5C0) (R/  32) Host Pipe Mask Register -------- */
#define USBHS_HSTPIPIMR_RXINE_Pos             (0U)                                           /**< (USBHS_HSTPIPIMR) Received IN Data Interrupt Enable Position */
#define USBHS_HSTPIPIMR_RXINE_Msk             (0x1U << USBHS_HSTPIPIMR_RXINE_Pos)            /**< (USBHS_HSTPIPIMR) Received IN Data Interrupt Enable Mask */
#define USBHS_HSTPIPIMR_RXINE(value)          (USBHS_HSTPIPIMR_RXINE_Msk & ((value) << USBHS_HSTPIPIMR_RXINE_Pos))
#define USBHS_HSTPIPIMR_TXOUTE_Pos            (1U)                                           /**< (USBHS_HSTPIPIMR) Transmitted OUT Data Interrupt Enable Position */
#define USBHS_HSTPIPIMR_TXOUTE_Msk            (0x1U << USBHS_HSTPIPIMR_TXOUTE_Pos)           /**< (USBHS_HSTPIPIMR) Transmitted OUT Data Interrupt Enable Mask */
#define USBHS_HSTPIPIMR_TXOUTE(value)         (USBHS_HSTPIPIMR_TXOUTE_Msk & ((value) << USBHS_HSTPIPIMR_TXOUTE_Pos))
#define USBHS_HSTPIPIMR_TXSTPE_Pos            (2U)                                           /**< (USBHS_HSTPIPIMR) Transmitted SETUP Interrupt Enable Position */
#define USBHS_HSTPIPIMR_TXSTPE_Msk            (0x1U << USBHS_HSTPIPIMR_TXSTPE_Pos)           /**< (USBHS_HSTPIPIMR) Transmitted SETUP Interrupt Enable Mask */
#define USBHS_HSTPIPIMR_TXSTPE(value)         (USBHS_HSTPIPIMR_TXSTPE_Msk & ((value) << USBHS_HSTPIPIMR_TXSTPE_Pos))
#define USBHS_HSTPIPIMR_UNDERFIE_Pos          (2U)                                           /**< (USBHS_HSTPIPIMR) Underflow Interrupt Enable Position */
#define USBHS_HSTPIPIMR_UNDERFIE_Msk          (0x1U << USBHS_HSTPIPIMR_UNDERFIE_Pos)         /**< (USBHS_HSTPIPIMR) Underflow Interrupt Enable Mask */
#define USBHS_HSTPIPIMR_UNDERFIE(value)       (USBHS_HSTPIPIMR_UNDERFIE_Msk & ((value) << USBHS_HSTPIPIMR_UNDERFIE_Pos))
#define USBHS_HSTPIPIMR_PERRE_Pos             (3U)                                           /**< (USBHS_HSTPIPIMR) Pipe Error Interrupt Enable Position */
#define USBHS_HSTPIPIMR_PERRE_Msk             (0x1U << USBHS_HSTPIPIMR_PERRE_Pos)            /**< (USBHS_HSTPIPIMR) Pipe Error Interrupt Enable Mask */
#define USBHS_HSTPIPIMR_PERRE(value)          (USBHS_HSTPIPIMR_PERRE_Msk & ((value) << USBHS_HSTPIPIMR_PERRE_Pos))
#define USBHS_HSTPIPIMR_NAKEDE_Pos            (4U)                                           /**< (USBHS_HSTPIPIMR) NAKed Interrupt Enable Position */
#define USBHS_HSTPIPIMR_NAKEDE_Msk            (0x1U << USBHS_HSTPIPIMR_NAKEDE_Pos)           /**< (USBHS_HSTPIPIMR) NAKed Interrupt Enable Mask */
#define USBHS_HSTPIPIMR_NAKEDE(value)         (USBHS_HSTPIPIMR_NAKEDE_Msk & ((value) << USBHS_HSTPIPIMR_NAKEDE_Pos))
#define USBHS_HSTPIPIMR_OVERFIE_Pos           (5U)                                           /**< (USBHS_HSTPIPIMR) Overflow Interrupt Enable Position */
#define USBHS_HSTPIPIMR_OVERFIE_Msk           (0x1U << USBHS_HSTPIPIMR_OVERFIE_Pos)          /**< (USBHS_HSTPIPIMR) Overflow Interrupt Enable Mask */
#define USBHS_HSTPIPIMR_OVERFIE(value)        (USBHS_HSTPIPIMR_OVERFIE_Msk & ((value) << USBHS_HSTPIPIMR_OVERFIE_Pos))
#define USBHS_HSTPIPIMR_RXSTALLDE_Pos         (6U)                                           /**< (USBHS_HSTPIPIMR) Received STALLed Interrupt Enable Position */
#define USBHS_HSTPIPIMR_RXSTALLDE_Msk         (0x1U << USBHS_HSTPIPIMR_RXSTALLDE_Pos)        /**< (USBHS_HSTPIPIMR) Received STALLed Interrupt Enable Mask */
#define USBHS_HSTPIPIMR_RXSTALLDE(value)      (USBHS_HSTPIPIMR_RXSTALLDE_Msk & ((value) << USBHS_HSTPIPIMR_RXSTALLDE_Pos))
#define USBHS_HSTPIPIMR_CRCERRE_Pos           (6U)                                           /**< (USBHS_HSTPIPIMR) CRC Error Interrupt Enable Position */
#define USBHS_HSTPIPIMR_CRCERRE_Msk           (0x1U << USBHS_HSTPIPIMR_CRCERRE_Pos)          /**< (USBHS_HSTPIPIMR) CRC Error Interrupt Enable Mask */
#define USBHS_HSTPIPIMR_CRCERRE(value)        (USBHS_HSTPIPIMR_CRCERRE_Msk & ((value) << USBHS_HSTPIPIMR_CRCERRE_Pos))
#define USBHS_HSTPIPIMR_SHORTPACKETIE_Pos     (7U)                                           /**< (USBHS_HSTPIPIMR) Short Packet Interrupt Enable Position */
#define USBHS_HSTPIPIMR_SHORTPACKETIE_Msk     (0x1U << USBHS_HSTPIPIMR_SHORTPACKETIE_Pos)    /**< (USBHS_HSTPIPIMR) Short Packet Interrupt Enable Mask */
#define USBHS_HSTPIPIMR_SHORTPACKETIE(value)  (USBHS_HSTPIPIMR_SHORTPACKETIE_Msk & ((value) << USBHS_HSTPIPIMR_SHORTPACKETIE_Pos))
#define USBHS_HSTPIPIMR_NBUSYBKE_Pos          (12U)                                          /**< (USBHS_HSTPIPIMR) Number of Busy Banks Interrupt Enable Position */
#define USBHS_HSTPIPIMR_NBUSYBKE_Msk          (0x1U << USBHS_HSTPIPIMR_NBUSYBKE_Pos)         /**< (USBHS_HSTPIPIMR) Number of Busy Banks Interrupt Enable Mask */
#define USBHS_HSTPIPIMR_NBUSYBKE(value)       (USBHS_HSTPIPIMR_NBUSYBKE_Msk & ((value) << USBHS_HSTPIPIMR_NBUSYBKE_Pos))
#define USBHS_HSTPIPIMR_FIFOCON_Pos           (14U)                                          /**< (USBHS_HSTPIPIMR) FIFO Control Position */
#define USBHS_HSTPIPIMR_FIFOCON_Msk           (0x1U << USBHS_HSTPIPIMR_FIFOCON_Pos)          /**< (USBHS_HSTPIPIMR) FIFO Control Mask */
#define USBHS_HSTPIPIMR_FIFOCON(value)        (USBHS_HSTPIPIMR_FIFOCON_Msk & ((value) << USBHS_HSTPIPIMR_FIFOCON_Pos))
#define USBHS_HSTPIPIMR_PDISHDMA_Pos          (16U)                                          /**< (USBHS_HSTPIPIMR) Pipe Interrupts Disable HDMA Request Enable Position */
#define USBHS_HSTPIPIMR_PDISHDMA_Msk          (0x1U << USBHS_HSTPIPIMR_PDISHDMA_Pos)         /**< (USBHS_HSTPIPIMR) Pipe Interrupts Disable HDMA Request Enable Mask */
#define USBHS_HSTPIPIMR_PDISHDMA(value)       (USBHS_HSTPIPIMR_PDISHDMA_Msk & ((value) << USBHS_HSTPIPIMR_PDISHDMA_Pos))
#define USBHS_HSTPIPIMR_PFREEZE_Pos           (17U)                                          /**< (USBHS_HSTPIPIMR) Pipe Freeze Position */
#define USBHS_HSTPIPIMR_PFREEZE_Msk           (0x1U << USBHS_HSTPIPIMR_PFREEZE_Pos)          /**< (USBHS_HSTPIPIMR) Pipe Freeze Mask */
#define USBHS_HSTPIPIMR_PFREEZE(value)        (USBHS_HSTPIPIMR_PFREEZE_Msk & ((value) << USBHS_HSTPIPIMR_PFREEZE_Pos))
#define USBHS_HSTPIPIMR_RSTDT_Pos             (18U)                                          /**< (USBHS_HSTPIPIMR) Reset Data Toggle Position */
#define USBHS_HSTPIPIMR_RSTDT_Msk             (0x1U << USBHS_HSTPIPIMR_RSTDT_Pos)            /**< (USBHS_HSTPIPIMR) Reset Data Toggle Mask */
#define USBHS_HSTPIPIMR_RSTDT(value)          (USBHS_HSTPIPIMR_RSTDT_Msk & ((value) << USBHS_HSTPIPIMR_RSTDT_Pos))
#define USBHS_HSTPIPIMR_Msk                   (0x000750FFUL)                                 /**< (USBHS_HSTPIPIMR) Register Mask  */

/* -------- USBHS_HSTPIPIER : (USBHS Offset: 0x5F0) ( /W 32) Host Pipe Enable Register -------- */
#define USBHS_HSTPIPIER_RXINES_Pos            (0U)                                           /**< (USBHS_HSTPIPIER) Received IN Data Interrupt Enable Position */
#define USBHS_HSTPIPIER_RXINES_Msk            (0x1U << USBHS_HSTPIPIER_RXINES_Pos)           /**< (USBHS_HSTPIPIER) Received IN Data Interrupt Enable Mask */
#define USBHS_HSTPIPIER_RXINES(value)         (USBHS_HSTPIPIER_RXINES_Msk & ((value) << USBHS_HSTPIPIER_RXINES_Pos))
#define USBHS_HSTPIPIER_TXOUTES_Pos           (1U)                                           /**< (USBHS_HSTPIPIER) Transmitted OUT Data Interrupt Enable Position */
#define USBHS_HSTPIPIER_TXOUTES_Msk           (0x1U << USBHS_HSTPIPIER_TXOUTES_Pos)          /**< (USBHS_HSTPIPIER) Transmitted OUT Data Interrupt Enable Mask */
#define USBHS_HSTPIPIER_TXOUTES(value)        (USBHS_HSTPIPIER_TXOUTES_Msk & ((value) << USBHS_HSTPIPIER_TXOUTES_Pos))
#define USBHS_HSTPIPIER_TXSTPES_Pos           (2U)                                           /**< (USBHS_HSTPIPIER) Transmitted SETUP Interrupt Enable Position */
#define USBHS_HSTPIPIER_TXSTPES_Msk           (0x1U << USBHS_HSTPIPIER_TXSTPES_Pos)          /**< (USBHS_HSTPIPIER) Transmitted SETUP Interrupt Enable Mask */
#define USBHS_HSTPIPIER_TXSTPES(value)        (USBHS_HSTPIPIER_TXSTPES_Msk & ((value) << USBHS_HSTPIPIER_TXSTPES_Pos))
#define USBHS_HSTPIPIER_UNDERFIES_Pos         (2U)                                           /**< (USBHS_HSTPIPIER) Underflow Interrupt Enable Position */
#define USBHS_HSTPIPIER_UNDERFIES_Msk         (0x1U << USBHS_HSTPIPIER_UNDERFIES_Pos)        /**< (USBHS_HSTPIPIER) Underflow Interrupt Enable Mask */
#define USBHS_HSTPIPIER_UNDERFIES(value)      (USBHS_HSTPIPIER_UNDERFIES_Msk & ((value) << USBHS_HSTPIPIER_UNDERFIES_Pos))
#define USBHS_HSTPIPIER_PERRES_Pos            (3U)                                           /**< (USBHS_HSTPIPIER) Pipe Error Interrupt Enable Position */
#define USBHS_HSTPIPIER_PERRES_Msk            (0x1U << USBHS_HSTPIPIER_PERRES_Pos)           /**< (USBHS_HSTPIPIER) Pipe Error Interrupt Enable Mask */
#define USBHS_HSTPIPIER_PERRES(value)         (USBHS_HSTPIPIER_PERRES_Msk & ((value) << USBHS_HSTPIPIER_PERRES_Pos))
#define USBHS_HSTPIPIER_NAKEDES_Pos           (4U)                                           /**< (USBHS_HSTPIPIER) NAKed Interrupt Enable Position */
#define USBHS_HSTPIPIER_NAKEDES_Msk           (0x1U << USBHS_HSTPIPIER_NAKEDES_Pos)          /**< (USBHS_HSTPIPIER) NAKed Interrupt Enable Mask */
#define USBHS_HSTPIPIER_NAKEDES(value)        (USBHS_HSTPIPIER_NAKEDES_Msk & ((value) << USBHS_HSTPIPIER_NAKEDES_Pos))
#define USBHS_HSTPIPIER_OVERFIES_Pos          (5U)                                           /**< (USBHS_HSTPIPIER) Overflow Interrupt Enable Position */
#define USBHS_HSTPIPIER_OVERFIES_Msk          (0x1U << USBHS_HSTPIPIER_OVERFIES_Pos)         /**< (USBHS_HSTPIPIER) Overflow Interrupt Enable Mask */
#define USBHS_HSTPIPIER_OVERFIES(value)       (USBHS_HSTPIPIER_OVERFIES_Msk & ((value) << USBHS_HSTPIPIER_OVERFIES_Pos))
#define USBHS_HSTPIPIER_RXSTALLDES_Pos        (6U)                                           /**< (USBHS_HSTPIPIER) Received STALLed Interrupt Enable Position */
#define USBHS_HSTPIPIER_RXSTALLDES_Msk        (0x1U << USBHS_HSTPIPIER_RXSTALLDES_Pos)       /**< (USBHS_HSTPIPIER) Received STALLed Interrupt Enable Mask */
#define USBHS_HSTPIPIER_RXSTALLDES(value)     (USBHS_HSTPIPIER_RXSTALLDES_Msk & ((value) << USBHS_HSTPIPIER_RXSTALLDES_Pos))
#define USBHS_HSTPIPIER_CRCERRES_Pos          (6U)                                           /**< (USBHS_HSTPIPIER) CRC Error Interrupt Enable Position */
#define USBHS_HSTPIPIER_CRCERRES_Msk          (0x1U << USBHS_HSTPIPIER_CRCERRES_Pos)         /**< (USBHS_HSTPIPIER) CRC Error Interrupt Enable Mask */
#define USBHS_HSTPIPIER_CRCERRES(value)       (USBHS_HSTPIPIER_CRCERRES_Msk & ((value) << USBHS_HSTPIPIER_CRCERRES_Pos))
#define USBHS_HSTPIPIER_SHORTPACKETIES_Pos    (7U)                                           /**< (USBHS_HSTPIPIER) Short Packet Interrupt Enable Position */
#define USBHS_HSTPIPIER_SHORTPACKETIES_Msk    (0x1U << USBHS_HSTPIPIER_SHORTPACKETIES_Pos)   /**< (USBHS_HSTPIPIER) Short Packet Interrupt Enable Mask */
#define USBHS_HSTPIPIER_SHORTPACKETIES(value) (USBHS_HSTPIPIER_SHORTPACKETIES_Msk & ((value) << USBHS_HSTPIPIER_SHORTPACKETIES_Pos))
#define USBHS_HSTPIPIER_NBUSYBKES_Pos         (12U)                                          /**< (USBHS_HSTPIPIER) Number of Busy Banks Enable Position */
#define USBHS_HSTPIPIER_NBUSYBKES_Msk         (0x1U << USBHS_HSTPIPIER_NBUSYBKES_Pos)        /**< (USBHS_HSTPIPIER) Number of Busy Banks Enable Mask */
#define USBHS_HSTPIPIER_NBUSYBKES(value)      (USBHS_HSTPIPIER_NBUSYBKES_Msk & ((value) << USBHS_HSTPIPIER_NBUSYBKES_Pos))
#define USBHS_HSTPIPIER_PDISHDMAS_Pos         (16U)                                          /**< (USBHS_HSTPIPIER) Pipe Interrupts Disable HDMA Request Enable Position */
#define USBHS_HSTPIPIER_PDISHDMAS_Msk         (0x1U << USBHS_HSTPIPIER_PDISHDMAS_Pos)        /**< (USBHS_HSTPIPIER) Pipe Interrupts Disable HDMA Request Enable Mask */
#define USBHS_HSTPIPIER_PDISHDMAS(value)      (USBHS_HSTPIPIER_PDISHDMAS_Msk & ((value) << USBHS_HSTPIPIER_PDISHDMAS_Pos))
#define USBHS_HSTPIPIER_PFREEZES_Pos          (17U)                                          /**< (USBHS_HSTPIPIER) Pipe Freeze Enable Position */
#define USBHS_HSTPIPIER_PFREEZES_Msk          (0x1U << USBHS_HSTPIPIER_PFREEZES_Pos)         /**< (USBHS_HSTPIPIER) Pipe Freeze Enable Mask */
#define USBHS_HSTPIPIER_PFREEZES(value)       (USBHS_HSTPIPIER_PFREEZES_Msk & ((value) << USBHS_HSTPIPIER_PFREEZES_Pos))
#define USBHS_HSTPIPIER_RSTDTS_Pos            (18U)                                          /**< (USBHS_HSTPIPIER) Reset Data Toggle Enable Position */
#define USBHS_HSTPIPIER_RSTDTS_Msk            (0x1U << USBHS_HSTPIPIER_RSTDTS_Pos)           /**< (USBHS_HSTPIPIER) Reset Data Toggle Enable Mask */
#define USBHS_HSTPIPIER_RSTDTS(value)         (USBHS_HSTPIPIER_RSTDTS_Msk & ((value) << USBHS_HSTPIPIER_RSTDTS_Pos))
#define USBHS_HSTPIPIER_Msk                   (0x000710FFUL)                                 /**< (USBHS_HSTPIPIER) Register Mask  */

/* -------- USBHS_HSTPIPIDR : (USBHS Offset: 0x620) ( /W 32) Host Pipe Disable Register -------- */
#define USBHS_HSTPIPIDR_RXINEC_Pos            (0U)                                           /**< (USBHS_HSTPIPIDR) Received IN Data Interrupt Disable Position */
#define USBHS_HSTPIPIDR_RXINEC_Msk            (0x1U << USBHS_HSTPIPIDR_RXINEC_Pos)           /**< (USBHS_HSTPIPIDR) Received IN Data Interrupt Disable Mask */
#define USBHS_HSTPIPIDR_RXINEC(value)         (USBHS_HSTPIPIDR_RXINEC_Msk & ((value) << USBHS_HSTPIPIDR_RXINEC_Pos))
#define USBHS_HSTPIPIDR_TXOUTEC_Pos           (1U)                                           /**< (USBHS_HSTPIPIDR) Transmitted OUT Data Interrupt Disable Position */
#define USBHS_HSTPIPIDR_TXOUTEC_Msk           (0x1U << USBHS_HSTPIPIDR_TXOUTEC_Pos)          /**< (USBHS_HSTPIPIDR) Transmitted OUT Data Interrupt Disable Mask */
#define USBHS_HSTPIPIDR_TXOUTEC(value)        (USBHS_HSTPIPIDR_TXOUTEC_Msk & ((value) << USBHS_HSTPIPIDR_TXOUTEC_Pos))
#define USBHS_HSTPIPIDR_TXSTPEC_Pos           (2U)                                           /**< (USBHS_HSTPIPIDR) Transmitted SETUP Interrupt Disable Position */
#define USBHS_HSTPIPIDR_TXSTPEC_Msk           (0x1U << USBHS_HSTPIPIDR_TXSTPEC_Pos)          /**< (USBHS_HSTPIPIDR) Transmitted SETUP Interrupt Disable Mask */
#define USBHS_HSTPIPIDR_TXSTPEC(value)        (USBHS_HSTPIPIDR_TXSTPEC_Msk & ((value) << USBHS_HSTPIPIDR_TXSTPEC_Pos))
#define USBHS_HSTPIPIDR_UNDERFIEC_Pos         (2U)                                           /**< (USBHS_HSTPIPIDR) Underflow Interrupt Disable Position */
#define USBHS_HSTPIPIDR_UNDERFIEC_Msk         (0x1U << USBHS_HSTPIPIDR_UNDERFIEC_Pos)        /**< (USBHS_HSTPIPIDR) Underflow Interrupt Disable Mask */
#define USBHS_HSTPIPIDR_UNDERFIEC(value)      (USBHS_HSTPIPIDR_UNDERFIEC_Msk & ((value) << USBHS_HSTPIPIDR_UNDERFIEC_Pos))
#define USBHS_HSTPIPIDR_PERREC_Pos            (3U)                                           /**< (USBHS_HSTPIPIDR) Pipe Error Interrupt Disable Position */
#define USBHS_HSTPIPIDR_PERREC_Msk            (0x1U << USBHS_HSTPIPIDR_PERREC_Pos)           /**< (USBHS_HSTPIPIDR) Pipe Error Interrupt Disable Mask */
#define USBHS_HSTPIPIDR_PERREC(value)         (USBHS_HSTPIPIDR_PERREC_Msk & ((value) << USBHS_HSTPIPIDR_PERREC_Pos))
#define USBHS_HSTPIPIDR_NAKEDEC_Pos           (4U)                                           /**< (USBHS_HSTPIPIDR) NAKed Interrupt Disable Position */
#define USBHS_HSTPIPIDR_NAKEDEC_Msk           (0x1U << USBHS_HSTPIPIDR_NAKEDEC_Pos)          /**< (USBHS_HSTPIPIDR) NAKed Interrupt Disable Mask */
#define USBHS_HSTPIPIDR_NAKEDEC(value)        (USBHS_HSTPIPIDR_NAKEDEC_Msk & ((value) << USBHS_HSTPIPIDR_NAKEDEC_Pos))
#define USBHS_HSTPIPIDR_OVERFIEC_Pos          (5U)                                           /**< (USBHS_HSTPIPIDR) Overflow Interrupt Disable Position */
#define USBHS_HSTPIPIDR_OVERFIEC_Msk          (0x1U << USBHS_HSTPIPIDR_OVERFIEC_Pos)         /**< (USBHS_HSTPIPIDR) Overflow Interrupt Disable Mask */
#define USBHS_HSTPIPIDR_OVERFIEC(value)       (USBHS_HSTPIPIDR_OVERFIEC_Msk & ((value) << USBHS_HSTPIPIDR_OVERFIEC_Pos))
#define USBHS_HSTPIPIDR_RXSTALLDEC_Pos        (6U)                                           /**< (USBHS_HSTPIPIDR) Received STALLed Interrupt Disable Position */
#define USBHS_HSTPIPIDR_RXSTALLDEC_Msk        (0x1U << USBHS_HSTPIPIDR_RXSTALLDEC_Pos)       /**< (USBHS_HSTPIPIDR) Received STALLed Interrupt Disable Mask */
#define USBHS_HSTPIPIDR_RXSTALLDEC(value)     (USBHS_HSTPIPIDR_RXSTALLDEC_Msk & ((value) << USBHS_HSTPIPIDR_RXSTALLDEC_Pos))
#define USBHS_HSTPIPIDR_CRCERREC_Pos          (6U)                                           /**< (USBHS_HSTPIPIDR) CRC Error Interrupt Disable Position */
#define USBHS_HSTPIPIDR_CRCERREC_Msk          (0x1U << USBHS_HSTPIPIDR_CRCERREC_Pos)         /**< (USBHS_HSTPIPIDR) CRC Error Interrupt Disable Mask */
#define USBHS_HSTPIPIDR_CRCERREC(value)       (USBHS_HSTPIPIDR_CRCERREC_Msk & ((value) << USBHS_HSTPIPIDR_CRCERREC_Pos))
#define USBHS_HSTPIPIDR_SHORTPACKETIEC_Pos    (7U)                                           /**< (USBHS_HSTPIPIDR) Short Packet Interrupt Disable Position */
#define USBHS_HSTPIPIDR_SHORTPACKETIEC_Msk    (0x1U << USBHS_HSTPIPIDR_SHORTPACKETIEC_Pos)   /**< (USBHS_HSTPIPIDR) Short Packet Interrupt Disable Mask */
#define USBHS_HSTPIPIDR_SHORTPACKETIEC(value) (USBHS_HSTPIPIDR_SHORTPACKETIEC_Msk & ((value) << USBHS_HSTPIPIDR_SHORTPACKETIEC_Pos))
#define USBHS_HSTPIPIDR_NBUSYBKEC_Pos         (12U)                                          /**< (USBHS_HSTPIPIDR) Number of Busy Banks Disable Position */
#define USBHS_HSTPIPIDR_NBUSYBKEC_Msk         (0x1U << USBHS_HSTPIPIDR_NBUSYBKEC_Pos)        /**< (USBHS_HSTPIPIDR) Number of Busy Banks Disable Mask */
#define USBHS_HSTPIPIDR_NBUSYBKEC(value)      (USBHS_HSTPIPIDR_NBUSYBKEC_Msk & ((value) << USBHS_HSTPIPIDR_NBUSYBKEC_Pos))
#define USBHS_HSTPIPIDR_FIFOCONC_Pos          (14U)                                          /**< (USBHS_HSTPIPIDR) FIFO Control Disable Position */
#define USBHS_HSTPIPIDR_FIFOCONC_Msk          (0x1U << USBHS_HSTPIPIDR_FIFOCONC_Pos)         /**< (USBHS_HSTPIPIDR) FIFO Control Disable Mask */
#define USBHS_HSTPIPIDR_FIFOCONC(value)       (USBHS_HSTPIPIDR_FIFOCONC_Msk & ((value) << USBHS_HSTPIPIDR_FIFOCONC_Pos))
#define USBHS_HSTPIPIDR_PDISHDMAC_Pos         (16U)                                          /**< (USBHS_HSTPIPIDR) Pipe Interrupts Disable HDMA Request Disable Position */
#define USBHS_HSTPIPIDR_PDISHDMAC_Msk         (0x1U << USBHS_HSTPIPIDR_PDISHDMAC_Pos)        /**< (USBHS_HSTPIPIDR) Pipe Interrupts Disable HDMA Request Disable Mask */
#define USBHS_HSTPIPIDR_PDISHDMAC(value)      (USBHS_HSTPIPIDR_PDISHDMAC_Msk & ((value) << USBHS_HSTPIPIDR_PDISHDMAC_Pos))
#define USBHS_HSTPIPIDR_PFREEZEC_Pos          (17U)                                          /**< (USBHS_HSTPIPIDR) Pipe Freeze Disable Position */
#define USBHS_HSTPIPIDR_PFREEZEC_Msk          (0x1U << USBHS_HSTPIPIDR_PFREEZEC_Pos)         /**< (USBHS_HSTPIPIDR) Pipe Freeze Disable Mask */
#define USBHS_HSTPIPIDR_PFREEZEC(value)       (USBHS_HSTPIPIDR_PFREEZEC_Msk & ((value) << USBHS_HSTPIPIDR_PFREEZEC_Pos))
#define USBHS_HSTPIPIDR_Msk                   (0x000350FFUL)                                 /**< (USBHS_HSTPIPIDR) Register Mask  */

/* -------- USBHS_HSTPIPINRQ : (USBHS Offset: 0x650) (R/W 32) Host Pipe IN Request Register -------- */
#define USBHS_HSTPIPINRQ_INRQ_Pos             (0U)                                           /**< (USBHS_HSTPIPINRQ) IN Request Number before Freeze Position */
#define USBHS_HSTPIPINRQ_INRQ_Msk             (0xFFU << USBHS_HSTPIPINRQ_INRQ_Pos)           /**< (USBHS_HSTPIPINRQ) IN Request Number before Freeze Mask */
#define USBHS_HSTPIPINRQ_INRQ(value)          (USBHS_HSTPIPINRQ_INRQ_Msk & ((value) << USBHS_HSTPIPINRQ_INRQ_Pos))
#define USBHS_HSTPIPINRQ_INMODE_Pos           (8U)                                           /**< (USBHS_HSTPIPINRQ) IN Request Mode Position */
#define USBHS_HSTPIPINRQ_INMODE_Msk           (0x1U << USBHS_HSTPIPINRQ_INMODE_Pos)          /**< (USBHS_HSTPIPINRQ) IN Request Mode Mask */
#define USBHS_HSTPIPINRQ_INMODE(value)        (USBHS_HSTPIPINRQ_INMODE_Msk & ((value) << USBHS_HSTPIPINRQ_INMODE_Pos))
#define USBHS_HSTPIPINRQ_Msk                  (0x000001FFUL)                                 /**< (USBHS_HSTPIPINRQ) Register Mask  */

/* -------- USBHS_HSTPIPERR : (USBHS Offset: 0x680) (R/W 32) Host Pipe Error Register -------- */
#define USBHS_HSTPIPERR_DATATGL_Pos           (0U)                                           /**< (USBHS_HSTPIPERR) Data Toggle Error Position */
#define USBHS_HSTPIPERR_DATATGL_Msk           (0x1U << USBHS_HSTPIPERR_DATATGL_Pos)          /**< (USBHS_HSTPIPERR) Data Toggle Error Mask */
#define USBHS_HSTPIPERR_DATATGL(value)        (USBHS_HSTPIPERR_DATATGL_Msk & ((value) << USBHS_HSTPIPERR_DATATGL_Pos))
#define USBHS_HSTPIPERR_DATAPID_Pos           (1U)                                           /**< (USBHS_HSTPIPERR) Data PID Error Position */
#define USBHS_HSTPIPERR_DATAPID_Msk           (0x1U << USBHS_HSTPIPERR_DATAPID_Pos)          /**< (USBHS_HSTPIPERR) Data PID Error Mask */
#define USBHS_HSTPIPERR_DATAPID(value)        (USBHS_HSTPIPERR_DATAPID_Msk & ((value) << USBHS_HSTPIPERR_DATAPID_Pos))
#define USBHS_HSTPIPERR_PID_Pos               (2U)                                           /**< (USBHS_HSTPIPERR) Data PID Error Position */
#define USBHS_HSTPIPERR_PID_Msk               (0x1U << USBHS_HSTPIPERR_PID_Pos)              /**< (USBHS_HSTPIPERR) Data PID Error Mask */
#define USBHS_HSTPIPERR_PID(value)            (USBHS_HSTPIPERR_PID_Msk & ((value) << USBHS_HSTPIPERR_PID_Pos))
#define USBHS_HSTPIPERR_TIMEOUT_Pos           (3U)                                           /**< (USBHS_HSTPIPERR) Time-Out Error Position */
#define USBHS_HSTPIPERR_TIMEOUT_Msk           (0x1U << USBHS_HSTPIPERR_TIMEOUT_Pos)          /**< (USBHS_HSTPIPERR) Time-Out Error Mask */
#define USBHS_HSTPIPERR_TIMEOUT(value)        (USBHS_HSTPIPERR_TIMEOUT_Msk & ((value) << USBHS_HSTPIPERR_TIMEOUT_Pos))
#define USBHS_HSTPIPERR_CRC16_Pos             (4U)                                           /**< (USBHS_HSTPIPERR) CRC16 Error Position */
#define USBHS_HSTPIPERR_CRC16_Msk             (0x1U << USBHS_HSTPIPERR_CRC16_Pos)            /**< (USBHS_HSTPIPERR) CRC16 Error Mask */
#define USBHS_HSTPIPERR_CRC16(value)          (USBHS_HSTPIPERR_CRC16_Msk & ((value) << USBHS_HSTPIPERR_CRC16_Pos))
#define USBHS_HSTPIPERR_COUNTER_Pos           (5U)                                           /**< (USBHS_HSTPIPERR) Error Counter Position */
#define USBHS_HSTPIPERR_COUNTER_Msk           (0x3U << USBHS_HSTPIPERR_COUNTER_Pos)          /**< (USBHS_HSTPIPERR) Error Counter Mask */
#define USBHS_HSTPIPERR_COUNTER(value)        (USBHS_HSTPIPERR_COUNTER_Msk & ((value) << USBHS_HSTPIPERR_COUNTER_Pos))
#define USBHS_HSTPIPERR_Msk                   (0x0000007FUL)                                 /**< (USBHS_HSTPIPERR) Register Mask  */

/* -------- USBHS_HSTDMANXTDSC : (USBHS Offset: 0x00) (R/W 32) Host DMA Channel Next Descriptor Address Register -------- */
#define USBHS_HSTDMANXTDSC_NXT_DSC_ADD_Pos    (0U)                                           /**< (USBHS_HSTDMANXTDSC) Next Descriptor Address Position */
#define USBHS_HSTDMANXTDSC_NXT_DSC_ADD_Msk    (0xFFFFFFFFU << USBHS_HSTDMANXTDSC_NXT_DSC_ADD_Pos) /**< (USBHS_HSTDMANXTDSC) Next Descriptor Address Mask */
#define USBHS_HSTDMANXTDSC_NXT_DSC_ADD(value) (USBHS_HSTDMANXTDSC_NXT_DSC_ADD_Msk & ((value) << USBHS_HSTDMANXTDSC_NXT_DSC_ADD_Pos))
#define USBHS_HSTDMANXTDSC_Msk                (0xFFFFFFFFUL)                                 /**< (USBHS_HSTDMANXTDSC) Register Mask  */

/* -------- USBHS_HSTDMAADDRESS : (USBHS Offset: 0x04) (R/W 32) Host DMA Channel Address Register -------- */
#define USBHS_HSTDMAADDRESS_BUFF_ADD_Pos      (0U)                                           /**< (USBHS_HSTDMAADDRESS) Buffer Address Position */
#define USBHS_HSTDMAADDRESS_BUFF_ADD_Msk      (0xFFFFFFFFU << USBHS_HSTDMAADDRESS_BUFF_ADD_Pos) /**< (USBHS_HSTDMAADDRESS) Buffer Address Mask */
#define USBHS_HSTDMAADDRESS_BUFF_ADD(value)   (USBHS_HSTDMAADDRESS_BUFF_ADD_Msk & ((value) << USBHS_HSTDMAADDRESS_BUFF_ADD_Pos))
#define USBHS_HSTDMAADDRESS_Msk               (0xFFFFFFFFUL)                                 /**< (USBHS_HSTDMAADDRESS) Register Mask  */

/* -------- USBHS_HSTDMACONTROL : (USBHS Offset: 0x08) (R/W 32) Host DMA Channel Control Register -------- */
#define USBHS_HSTDMACONTROL_CHANN_ENB_Pos     (0U)                                           /**< (USBHS_HSTDMACONTROL) Channel Enable Command Position */
#define USBHS_HSTDMACONTROL_CHANN_ENB_Msk     (0x1U << USBHS_HSTDMACONTROL_CHANN_ENB_Pos)    /**< (USBHS_HSTDMACONTROL) Channel Enable Command Mask */
#define USBHS_HSTDMACONTROL_CHANN_ENB(value)  (USBHS_HSTDMACONTROL_CHANN_ENB_Msk & ((value) << USBHS_HSTDMACONTROL_CHANN_ENB_Pos))
#define USBHS_HSTDMACONTROL_LDNXT_DSC_Pos     (1U)                                           /**< (USBHS_HSTDMACONTROL) Load Next Channel Transfer Descriptor Enable Command Position */
#define USBHS_HSTDMACONTROL_LDNXT_DSC_Msk     (0x1U << USBHS_HSTDMACONTROL_LDNXT_DSC_Pos)    /**< (USBHS_HSTDMACONTROL) Load Next Channel Transfer Descriptor Enable Command Mask */
#define USBHS_HSTDMACONTROL_LDNXT_DSC(value)  (USBHS_HSTDMACONTROL_LDNXT_DSC_Msk & ((value) << USBHS_HSTDMACONTROL_LDNXT_DSC_Pos))
#define USBHS_HSTDMACONTROL_END_TR_EN_Pos     (2U)                                           /**< (USBHS_HSTDMACONTROL) End of Transfer Enable Control (OUT transfers only) Position */
#define USBHS_HSTDMACONTROL_END_TR_EN_Msk     (0x1U << USBHS_HSTDMACONTROL_END_TR_EN_Pos)    /**< (USBHS_HSTDMACONTROL) End of Transfer Enable Control (OUT transfers only) Mask */
#define USBHS_HSTDMACONTROL_END_TR_EN(value)  (USBHS_HSTDMACONTROL_END_TR_EN_Msk & ((value) << USBHS_HSTDMACONTROL_END_TR_EN_Pos))
#define USBHS_HSTDMACONTROL_END_B_EN_Pos      (3U)                                           /**< (USBHS_HSTDMACONTROL) End of Buffer Enable Control Position */
#define USBHS_HSTDMACONTROL_END_B_EN_Msk      (0x1U << USBHS_HSTDMACONTROL_END_B_EN_Pos)     /**< (USBHS_HSTDMACONTROL) End of Buffer Enable Control Mask */
#define USBHS_HSTDMACONTROL_END_B_EN(value)   (USBHS_HSTDMACONTROL_END_B_EN_Msk & ((value) << USBHS_HSTDMACONTROL_END_B_EN_Pos))
#define USBHS_HSTDMACONTROL_END_TR_IT_Pos     (4U)                                           /**< (USBHS_HSTDMACONTROL) End of Transfer Interrupt Enable Position */
#define USBHS_HSTDMACONTROL_END_TR_IT_Msk     (0x1U << USBHS_HSTDMACONTROL_END_TR_IT_Pos)    /**< (USBHS_HSTDMACONTROL) End of Transfer Interrupt Enable Mask */
#define USBHS_HSTDMACONTROL_END_TR_IT(value)  (USBHS_HSTDMACONTROL_END_TR_IT_Msk & ((value) << USBHS_HSTDMACONTROL_END_TR_IT_Pos))
#define USBHS_HSTDMACONTROL_END_BUFFIT_Pos    (5U)                                           /**< (USBHS_HSTDMACONTROL) End of Buffer Interrupt Enable Position */
#define USBHS_HSTDMACONTROL_END_BUFFIT_Msk    (0x1U << USBHS_HSTDMACONTROL_END_BUFFIT_Pos)   /**< (USBHS_HSTDMACONTROL) End of Buffer Interrupt Enable Mask */
#define USBHS_HSTDMACONTROL_END_BUFFIT(value) (USBHS_HSTDMACONTROL_END_BUFFIT_Msk & ((value) << USBHS_HSTDMACONTROL_END_BUFFIT_Pos))
#define USBHS_HSTDMACONTROL_DESC_LD_IT_Pos    (6U)                                           /**< (USBHS_HSTDMACONTROL) Descriptor Loaded Interrupt Enable Position */
#define USBHS_HSTDMACONTROL_DESC_LD_IT_Msk    (0x1U << USBHS_HSTDMACONTROL_DESC_LD_IT_Pos)   /**< (USBHS_HSTDMACONTROL) Descriptor Loaded Interrupt Enable Mask */
#define USBHS_HSTDMACONTROL_DESC_LD_IT(value) (USBHS_HSTDMACONTROL_DESC_LD_IT_Msk & ((value) << USBHS_HSTDMACONTROL_DESC_LD_IT_Pos))
#define USBHS_HSTDMACONTROL_BURST_LCK_Pos     (7U)                                           /**< (USBHS_HSTDMACONTROL) Burst Lock Enable Position */
#define USBHS_HSTDMACONTROL_BURST_LCK_Msk     (0x1U << USBHS_HSTDMACONTROL_BURST_LCK_Pos)    /**< (USBHS_HSTDMACONTROL) Burst Lock Enable Mask */
#define USBHS_HSTDMACONTROL_BURST_LCK(value)  (USBHS_HSTDMACONTROL_BURST_LCK_Msk & ((value) << USBHS_HSTDMACONTROL_BURST_LCK_Pos))
#define USBHS_HSTDMACONTROL_BUFF_LENGTH_Pos   (16U)                                          /**< (USBHS_HSTDMACONTROL) Buffer Byte Length (Write-only) Position */
#define USBHS_HSTDMACONTROL_BUFF_LENGTH_Msk   (0xFFFFU << USBHS_HSTDMACONTROL_BUFF_LENGTH_Pos) /**< (USBHS_HSTDMACONTROL) Buffer Byte Length (Write-only) Mask */
#define USBHS_HSTDMACONTROL_BUFF_LENGTH(value) (USBHS_HSTDMACONTROL_BUFF_LENGTH_Msk & ((value) << USBHS_HSTDMACONTROL_BUFF_LENGTH_Pos))
#define USBHS_HSTDMACONTROL_Msk               (0xFFFF00FFUL)                                 /**< (USBHS_HSTDMACONTROL) Register Mask  */

/* -------- USBHS_HSTDMASTATUS : (USBHS Offset: 0x0C) (R/W 32) Host DMA Channel Status Register -------- */
#define USBHS_HSTDMASTATUS_CHANN_ENB_Pos      (0U)                                           /**< (USBHS_HSTDMASTATUS) Channel Enable Status Position */
#define USBHS_HSTDMASTATUS_CHANN_ENB_Msk      (0x1U << USBHS_HSTDMASTATUS_CHANN_ENB_Pos)     /**< (USBHS_HSTDMASTATUS) Channel Enable Status Mask */
#define USBHS_HSTDMASTATUS_CHANN_ENB(value)   (USBHS_HSTDMASTATUS_CHANN_ENB_Msk & ((value) << USBHS_HSTDMASTATUS_CHANN_ENB_Pos))
#define USBHS_HSTDMASTATUS_CHANN_ACT_Pos      (1U)                                           /**< (USBHS_HSTDMASTATUS) Channel Active Status Position */
#define USBHS_HSTDMASTATUS_CHANN_ACT_Msk      (0x1U << USBHS_HSTDMASTATUS_CHANN_ACT_Pos)     /**< (USBHS_HSTDMASTATUS) Channel Active Status Mask */
#define USBHS_HSTDMASTATUS_CHANN_ACT(value)   (USBHS_HSTDMASTATUS_CHANN_ACT_Msk & ((value) << USBHS_HSTDMASTATUS_CHANN_ACT_Pos))
#define USBHS_HSTDMASTATUS_END_TR_ST_Pos      (4U)                                           /**< (USBHS_HSTDMASTATUS) End of Channel Transfer Status Position */
#define USBHS_HSTDMASTATUS_END_TR_ST_Msk      (0x1U << USBHS_HSTDMASTATUS_END_TR_ST_Pos)     /**< (USBHS_HSTDMASTATUS) End of Channel Transfer Status Mask */
#define USBHS_HSTDMASTATUS_END_TR_ST(value)   (USBHS_HSTDMASTATUS_END_TR_ST_Msk & ((value) << USBHS_HSTDMASTATUS_END_TR_ST_Pos))
#define USBHS_HSTDMASTATUS_END_BF_ST_Pos      (5U)                                           /**< (USBHS_HSTDMASTATUS) End of Channel Buffer Status Position */
#define USBHS_HSTDMASTATUS_END_BF_ST_Msk      (0x1U << USBHS_HSTDMASTATUS_END_BF_ST_Pos)     /**< (USBHS_HSTDMASTATUS) End of Channel Buffer Status Mask */
#define USBHS_HSTDMASTATUS_END_BF_ST(value)   (USBHS_HSTDMASTATUS_END_BF_ST_Msk & ((value) << USBHS_HSTDMASTATUS_END_BF_ST_Pos))
#define USBHS_HSTDMASTATUS_DESC_LDST_Pos      (6U)                                           /**< (USBHS_HSTDMASTATUS) Descriptor Loaded Status Position */
#define USBHS_HSTDMASTATUS_DESC_LDST_Msk      (0x1U << USBHS_HSTDMASTATUS_DESC_LDST_Pos)     /**< (USBHS_HSTDMASTATUS) Descriptor Loaded Status Mask */
#define USBHS_HSTDMASTATUS_DESC_LDST(value)   (USBHS_HSTDMASTATUS_DESC_LDST_Msk & ((value) << USBHS_HSTDMASTATUS_DESC_LDST_Pos))
#define USBHS_HSTDMASTATUS_BUFF_COUNT_Pos     (16U)                                          /**< (USBHS_HSTDMASTATUS) Buffer Byte Count Position */
#define USBHS_HSTDMASTATUS_BUFF_COUNT_Msk     (0xFFFFU << USBHS_HSTDMASTATUS_BUFF_COUNT_Pos) /**< (USBHS_HSTDMASTATUS) Buffer Byte Count Mask */
#define USBHS_HSTDMASTATUS_BUFF_COUNT(value)  (USBHS_HSTDMASTATUS_BUFF_COUNT_Msk & ((value) << USBHS_HSTDMASTATUS_BUFF_COUNT_Pos))
#define USBHS_HSTDMASTATUS_Msk                (0xFFFF0073UL)                                 /**< (USBHS_HSTDMASTATUS) Register Mask  */

/* -------- USBHS_CTRL : (USBHS Offset: 0x800) (R/W 32) General Control Register -------- */
#define USBHS_CTRL_RDERRE_Pos                 (4U)                                           /**< (USBHS_CTRL) Remote Device Connection Error Interrupt Enable Position */
#define USBHS_CTRL_RDERRE_Msk                 (0x1U << USBHS_CTRL_RDERRE_Pos)                /**< (USBHS_CTRL) Remote Device Connection Error Interrupt Enable Mask */
#define USBHS_CTRL_RDERRE(value)              (USBHS_CTRL_RDERRE_Msk & ((value) << USBHS_CTRL_RDERRE_Pos))
#define USBHS_CTRL_VBUSHWC_Pos                (8U)                                           /**< (USBHS_CTRL) VBUS Hardware Control Position */
#define USBHS_CTRL_VBUSHWC_Msk                (0x1U << USBHS_CTRL_VBUSHWC_Pos)               /**< (USBHS_CTRL) VBUS Hardware Control Mask */
#define USBHS_CTRL_VBUSHWC(value)             (USBHS_CTRL_VBUSHWC_Msk & ((value) << USBHS_CTRL_VBUSHWC_Pos))
#define USBHS_CTRL_FRZCLK_Pos                 (14U)                                          /**< (USBHS_CTRL) Freeze USB Clock Position */
#define USBHS_CTRL_FRZCLK_Msk                 (0x1U << USBHS_CTRL_FRZCLK_Pos)                /**< (USBHS_CTRL) Freeze USB Clock Mask */
#define USBHS_CTRL_FRZCLK(value)              (USBHS_CTRL_FRZCLK_Msk & ((value) << USBHS_CTRL_FRZCLK_Pos))
#define USBHS_CTRL_USBE_Pos                   (15U)                                          /**< (USBHS_CTRL) USBHS Enable Position */
#define USBHS_CTRL_USBE_Msk                   (0x1U << USBHS_CTRL_USBE_Pos)                  /**< (USBHS_CTRL) USBHS Enable Mask */
#define USBHS_CTRL_USBE(value)                (USBHS_CTRL_USBE_Msk & ((value) << USBHS_CTRL_USBE_Pos))
#define USBHS_CTRL_UID_Pos                    (24U)                                          /**< (USBHS_CTRL) UID Pin Enable Position */
#define USBHS_CTRL_UID_Msk                    (0x1U << USBHS_CTRL_UID_Pos)                   /**< (USBHS_CTRL) UID Pin Enable Mask */
#define USBHS_CTRL_UID(value)                 (USBHS_CTRL_UID_Msk & ((value) << USBHS_CTRL_UID_Pos))
#define USBHS_CTRL_UIMOD_Pos                  (25U)                                          /**< (USBHS_CTRL) USBHS Mode Position */
#define USBHS_CTRL_UIMOD_Msk                  (0x1U << USBHS_CTRL_UIMOD_Pos)                 /**< (USBHS_CTRL) USBHS Mode Mask */
#define USBHS_CTRL_UIMOD(value)               (USBHS_CTRL_UIMOD_Msk & ((value) << USBHS_CTRL_UIMOD_Pos))
#define   USBHS_CTRL_UIMOD_HOST_Val           (0U)                                           /**< (USBHS_CTRL) The module is in USB Host mode.  */
#define   USBHS_CTRL_UIMOD_DEVICE_Val         (1U)                                           /**< (USBHS_CTRL) The module is in USB Device mode.  */
#define USBHS_CTRL_UIMOD_HOST                 (USBHS_CTRL_UIMOD_HOST_Val << USBHS_CTRL_UIMOD_Pos) /**< (USBHS_CTRL) The module is in USB Host mode. Position  */
#define USBHS_CTRL_UIMOD_DEVICE               (USBHS_CTRL_UIMOD_DEVICE_Val << USBHS_CTRL_UIMOD_Pos) /**< (USBHS_CTRL) The module is in USB Device mode. Position  */
#define USBHS_CTRL_Msk                        (0x0300C110UL)                                 /**< (USBHS_CTRL) Register Mask  */

/* -------- USBHS_SR : (USBHS Offset: 0x804) (R/  32) General Status Register -------- */
#define USBHS_SR_RDERRI_Pos                   (4U)                                           /**< (USBHS_SR) Remote Device Connection Error Interrupt (Host mode only) Position */
#define USBHS_SR_RDERRI_Msk                   (0x1U << USBHS_SR_RDERRI_Pos)                  /**< (USBHS_SR) Remote Device Connection Error Interrupt (Host mode only) Mask */
#define USBHS_SR_RDERRI(value)                (USBHS_SR_RDERRI_Msk & ((value) << USBHS_SR_RDERRI_Pos))
#define USBHS_SR_SPEED_Pos                    (12U)                                          /**< (USBHS_SR) Speed Status (Device mode only) Position */
#define USBHS_SR_SPEED_Msk                    (0x3U << USBHS_SR_SPEED_Pos)                   /**< (USBHS_SR) Speed Status (Device mode only) Mask */
#define USBHS_SR_SPEED(value)                 (USBHS_SR_SPEED_Msk & ((value) << USBHS_SR_SPEED_Pos))
#define   USBHS_SR_SPEED_FULL_SPEED_Val       (0U)                                           /**< (USBHS_SR) Full-Speed mode  */
#define   USBHS_SR_SPEED_HIGH_SPEED_Val       (1U)                                           /**< (USBHS_SR) High-Speed mode  */
#define   USBHS_SR_SPEED_LOW_SPEED_Val        (2U)                                           /**< (USBHS_SR) Low-Speed mode  */
#define USBHS_SR_SPEED_FULL_SPEED             (USBHS_SR_SPEED_FULL_SPEED_Val << USBHS_SR_SPEED_Pos) /**< (USBHS_SR) Full-Speed mode Position  */
#define USBHS_SR_SPEED_HIGH_SPEED             (USBHS_SR_SPEED_HIGH_SPEED_Val << USBHS_SR_SPEED_Pos) /**< (USBHS_SR) High-Speed mode Position  */
#define USBHS_SR_SPEED_LOW_SPEED              (USBHS_SR_SPEED_LOW_SPEED_Val << USBHS_SR_SPEED_Pos) /**< (USBHS_SR) Low-Speed mode Position  */
#define USBHS_SR_CLKUSABLE_Pos                (14U)                                          /**< (USBHS_SR) UTMI Clock Usable Position */
#define USBHS_SR_CLKUSABLE_Msk                (0x1U << USBHS_SR_CLKUSABLE_Pos)               /**< (USBHS_SR) UTMI Clock Usable Mask */
#define USBHS_SR_CLKUSABLE(value)             (USBHS_SR_CLKUSABLE_Msk & ((value) << USBHS_SR_CLKUSABLE_Pos))
#define USBHS_SR_Msk                          (0x00007010UL)                                 /**< (USBHS_SR) Register Mask  */

/* -------- USBHS_SCR : (USBHS Offset: 0x808) ( /W 32) General Status Clear Register -------- */
#define USBHS_SCR_RDERRIC_Pos                 (4U)                                           /**< (USBHS_SCR) Remote Device Connection Error Interrupt Clear Position */
#define USBHS_SCR_RDERRIC_Msk                 (0x1U << USBHS_SCR_RDERRIC_Pos)                /**< (USBHS_SCR) Remote Device Connection Error Interrupt Clear Mask */
#define USBHS_SCR_RDERRIC(value)              (USBHS_SCR_RDERRIC_Msk & ((value) << USBHS_SCR_RDERRIC_Pos))
#define USBHS_SCR_Msk                         (0x00000010UL)                                 /**< (USBHS_SCR) Register Mask  */

/* -------- USBHS_SFR : (USBHS Offset: 0x80C) ( /W 32) General Status Set Register -------- */
#define USBHS_SFR_RDERRIS_Pos                 (4U)                                           /**< (USBHS_SFR) Remote Device Connection Error Interrupt Set Position */
#define USBHS_SFR_RDERRIS_Msk                 (0x1U << USBHS_SFR_RDERRIS_Pos)                /**< (USBHS_SFR) Remote Device Connection Error Interrupt Set Mask */
#define USBHS_SFR_RDERRIS(value)              (USBHS_SFR_RDERRIS_Msk & ((value) << USBHS_SFR_RDERRIS_Pos))
#define USBHS_SFR_VBUSRQS_Pos                 (9U)                                           /**< (USBHS_SFR) VBUS Request Set Position */
#define USBHS_SFR_VBUSRQS_Msk                 (0x1U << USBHS_SFR_VBUSRQS_Pos)                /**< (USBHS_SFR) VBUS Request Set Mask */
#define USBHS_SFR_VBUSRQS(value)              (USBHS_SFR_VBUSRQS_Msk & ((value) << USBHS_SFR_VBUSRQS_Pos))
#define USBHS_SFR_Msk                         (0x00000210UL)                                 /**< (USBHS_SFR) Register Mask  */

/** \brief USBHS register offsets definitions */
#define USBHS_DEVCTRL_OFFSET           (0x00)         /**< (USBHS_DEVCTRL) Device General Control Register Offset */
#define USBHS_DEVISR_OFFSET            (0x04)         /**< (USBHS_DEVISR) Device Global Interrupt Status Register Offset */
#define USBHS_DEVICR_OFFSET            (0x08)         /**< (USBHS_DEVICR) Device Global Interrupt Clear Register Offset */
#define USBHS_DEVIFR_OFFSET            (0x0C)         /**< (USBHS_DEVIFR) Device Global Interrupt Set Register Offset */
#define USBHS_DEVIMR_OFFSET            (0x10)         /**< (USBHS_DEVIMR) Device Global Interrupt Mask Register Offset */
#define USBHS_DEVIDR_OFFSET            (0x14)         /**< (USBHS_DEVIDR) Device Global Interrupt Disable Register Offset */
#define USBHS_DEVIER_OFFSET            (0x18)         /**< (USBHS_DEVIER) Device Global Interrupt Enable Register Offset */
#define USBHS_DEVEPT_OFFSET            (0x1C)         /**< (USBHS_DEVEPT) Device Endpoint Register Offset */
#define USBHS_DEVFNUM_OFFSET           (0x20)         /**< (USBHS_DEVFNUM) Device Frame Number Register Offset */
#define USBHS_DEVEPTCFG_OFFSET         (0x100)        /**< (USBHS_DEVEPTCFG) Device Endpoint Configuration Register Offset */
#define USBHS_DEVEPTISR_OFFSET         (0x130)        /**< (USBHS_DEVEPTISR) Device Endpoint Interrupt Status Register Offset */
#define USBHS_DEVEPTICR_OFFSET         (0x160)        /**< (USBHS_DEVEPTICR) Device Endpoint Interrupt Clear Register Offset */
#define USBHS_DEVEPTIFR_OFFSET         (0x190)        /**< (USBHS_DEVEPTIFR) Device Endpoint Interrupt Set Register Offset */
#define USBHS_DEVEPTIMR_OFFSET         (0x1C0)        /**< (USBHS_DEVEPTIMR) Device Endpoint Interrupt Mask Register Offset */
#define USBHS_DEVEPTIER_OFFSET         (0x1F0)        /**< (USBHS_DEVEPTIER) Device Endpoint Interrupt Enable Register Offset */
#define USBHS_DEVEPTIDR_OFFSET         (0x220)        /**< (USBHS_DEVEPTIDR) Device Endpoint Interrupt Disable Register Offset */
#define USBHS_DEVDMA_OFFSET            (0x310)        /**< (USBHS_DEVDMA) Device DMA Channel Next Descriptor Address Register Offset */
  #define USBHS_DEVDMANXTDSC_OFFSET      (0x00)         /**< (USBHS_DEVDMANXTDSC) Device DMA Channel Next Descriptor Address Register Offset */
  #define USBHS_DEVDMAADDRESS_OFFSET     (0x04)         /**< (USBHS_DEVDMAADDRESS) Device DMA Channel Address Register Offset */
  #define USBHS_DEVDMACONTROL_OFFSET     (0x08)         /**< (USBHS_DEVDMACONTROL) Device DMA Channel Control Register Offset */
  #define USBHS_DEVDMASTATUS_OFFSET      (0x0C)         /**< (USBHS_DEVDMASTATUS) Device DMA Channel Status Register Offset */
#define USBHS_HSTCTRL_OFFSET           (0x400)        /**< (USBHS_HSTCTRL) Host General Control Register Offset */
#define USBHS_HSTISR_OFFSET            (0x404)        /**< (USBHS_HSTISR) Host Global Interrupt Status Register Offset */
#define USBHS_HSTICR_OFFSET            (0x408)        /**< (USBHS_HSTICR) Host Global Interrupt Clear Register Offset */
#define USBHS_HSTIFR_OFFSET            (0x40C)        /**< (USBHS_HSTIFR) Host Global Interrupt Set Register Offset */
#define USBHS_HSTIMR_OFFSET            (0x410)        /**< (USBHS_HSTIMR) Host Global Interrupt Mask Register Offset */
#define USBHS_HSTIDR_OFFSET            (0x414)        /**< (USBHS_HSTIDR) Host Global Interrupt Disable Register Offset */
#define USBHS_HSTIER_OFFSET            (0x418)        /**< (USBHS_HSTIER) Host Global Interrupt Enable Register Offset */
#define USBHS_HSTPIP_OFFSET            (0x41C)        /**< (USBHS_HSTPIP) Host Pipe Register Offset */
#define USBHS_HSTFNUM_OFFSET           (0x420)        /**< (USBHS_HSTFNUM) Host Frame Number Register Offset */
#define USBHS_HSTADDR1_OFFSET          (0x424)        /**< (USBHS_HSTADDR1) Host Address 1 Register Offset */
#define USBHS_HSTADDR2_OFFSET          (0x428)        /**< (USBHS_HSTADDR2) Host Address 2 Register Offset */
#define USBHS_HSTADDR3_OFFSET          (0x42C)        /**< (USBHS_HSTADDR3) Host Address 3 Register Offset */
#define USBHS_HSTPIPCFG_OFFSET         (0x500)        /**< (USBHS_HSTPIPCFG) Host Pipe Configuration Register Offset */
#define USBHS_HSTPIPISR_OFFSET         (0x530)        /**< (USBHS_HSTPIPISR) Host Pipe Status Register Offset */
#define USBHS_HSTPIPICR_OFFSET         (0x560)        /**< (USBHS_HSTPIPICR) Host Pipe Clear Register Offset */
#define USBHS_HSTPIPIFR_OFFSET         (0x590)        /**< (USBHS_HSTPIPIFR) Host Pipe Set Register Offset */
#define USBHS_HSTPIPIMR_OFFSET         (0x5C0)        /**< (USBHS_HSTPIPIMR) Host Pipe Mask Register Offset */
#define USBHS_HSTPIPIER_OFFSET         (0x5F0)        /**< (USBHS_HSTPIPIER) Host Pipe Enable Register Offset */
#define USBHS_HSTPIPIDR_OFFSET         (0x620)        /**< (USBHS_HSTPIPIDR) Host Pipe Disable Register Offset */
#define USBHS_HSTPIPINRQ_OFFSET        (0x650)        /**< (USBHS_HSTPIPINRQ) Host Pipe IN Request Register Offset */
#define USBHS_HSTPIPERR_OFFSET         (0x680)        /**< (USBHS_HSTPIPERR) Host Pipe Error Register Offset */
#define USBHS_HSTDMA_OFFSET            (0x710)        /**< (USBHS_HSTDMA) Host DMA Channel Next Descriptor Address Register Offset */
  #define USBHS_HSTDMANXTDSC_OFFSET      (0x00)         /**< (USBHS_HSTDMANXTDSC) Host DMA Channel Next Descriptor Address Register Offset */
  #define USBHS_HSTDMAADDRESS_OFFSET     (0x04)         /**< (USBHS_HSTDMAADDRESS) Host DMA Channel Address Register Offset */
  #define USBHS_HSTDMACONTROL_OFFSET     (0x08)         /**< (USBHS_HSTDMACONTROL) Host DMA Channel Control Register Offset */
  #define USBHS_HSTDMASTATUS_OFFSET      (0x0C)         /**< (USBHS_HSTDMASTATUS) Host DMA Channel Status Register Offset */
#define USBHS_CTRL_OFFSET              (0x800)        /**< (USBHS_CTRL) General Control Register Offset */
#define USBHS_SR_OFFSET                (0x804)        /**< (USBHS_SR) General Status Register Offset */
#define USBHS_SCR_OFFSET               (0x808)        /**< (USBHS_SCR) General Status Clear Register Offset */
#define USBHS_SFR_OFFSET               (0x80C)        /**< (USBHS_SFR) General Status Set Register Offset */

/** \brief USBHS_DEVDMA register API structure */
typedef struct
{
  __IO  uint32_t                       USBHS_DEVDMANXTDSC; /**< Offset: 0x00 (R/W  32) Device DMA Channel Next Descriptor Address Register */
  __IO  uint32_t                       USBHS_DEVDMAADDRESS; /**< Offset: 0x04 (R/W  32) Device DMA Channel Address Register */
  __IO  uint32_t                       USBHS_DEVDMACONTROL; /**< Offset: 0x08 (R/W  32) Device DMA Channel Control Register */
  __IO  uint32_t                       USBHS_DEVDMASTATUS; /**< Offset: 0x0c (R/W  32) Device DMA Channel Status Register */
} usbhs_devdma_registers_t;
#define USBHS_DEVDMA_NUMBER (7U)

/** \brief USBHS_HSTDMA register API structure */
typedef struct
{
  __IO  uint32_t                       USBHS_HSTDMANXTDSC; /**< Offset: 0x00 (R/W  32) Host DMA Channel Next Descriptor Address Register */
  __IO  uint32_t                       USBHS_HSTDMAADDRESS; /**< Offset: 0x04 (R/W  32) Host DMA Channel Address Register */
  __IO  uint32_t                       USBHS_HSTDMACONTROL; /**< Offset: 0x08 (R/W  32) Host DMA Channel Control Register */
  __IO  uint32_t                       USBHS_HSTDMASTATUS; /**< Offset: 0x0c (R/W  32) Host DMA Channel Status Register */
} usbhs_hstdma_registers_t;
#define USBHS_HSTDMA_NUMBER (7U)

/** \brief USBHS register API structure */
typedef struct
{
  __IO  uint32_t                       USBHS_DEVCTRL;   /**< Offset: 0x00 (R/W  32) Device General Control Register */
  __I   uint32_t                       USBHS_DEVISR;    /**< Offset: 0x04 (R/   32) Device Global Interrupt Status Register */
  __O   uint32_t                       USBHS_DEVICR;    /**< Offset: 0x08 ( /W  32) Device Global Interrupt Clear Register */
  __O   uint32_t                       USBHS_DEVIFR;    /**< Offset: 0x0c ( /W  32) Device Global Interrupt Set Register */
  __I   uint32_t                       USBHS_DEVIMR;    /**< Offset: 0x10 (R/   32) Device Global Interrupt Mask Register */
  __O   uint32_t                       USBHS_DEVIDR;    /**< Offset: 0x14 ( /W  32) Device Global Interrupt Disable Register */
  __O   uint32_t                       USBHS_DEVIER;    /**< Offset: 0x18 ( /W  32) Device Global Interrupt Enable Register */
  __IO  uint32_t                       USBHS_DEVEPT;    /**< Offset: 0x1c (R/W  32) Device Endpoint Register */
  __I   uint32_t                       USBHS_DEVFNUM;   /**< Offset: 0x20 (R/   32) Device Frame Number Register */
  __I   uint8_t                        Reserved1[0xDC];
  __IO  uint32_t                       USBHS_DEVEPTCFG[10]; /**< Offset: 0x100 (R/W  32) Device Endpoint Configuration Register */
  __I   uint8_t                        Reserved2[0x08];
  __I   uint32_t                       USBHS_DEVEPTISR[10]; /**< Offset: 0x130 (R/   32) Device Endpoint Interrupt Status Register */
  __I   uint8_t                        Reserved3[0x08];
  __O   uint32_t                       USBHS_DEVEPTICR[10]; /**< Offset: 0x160 ( /W  32) Device Endpoint Interrupt Clear Register */
  __I   uint8_t                        Reserved4[0x08];
  __O   uint32_t                       USBHS_DEVEPTIFR[10]; /**< Offset: 0x190 ( /W  32) Device Endpoint Interrupt Set Register */
  __I   uint8_t                        Reserved5[0x08];
  __I   uint32_t                       USBHS_DEVEPTIMR[10]; /**< Offset: 0x1c0 (R/   32) Device Endpoint Interrupt Mask Register */
  __I   uint8_t                        Reserved6[0x08];
  __O   uint32_t                       USBHS_DEVEPTIER[10]; /**< Offset: 0x1f0 ( /W  32) Device Endpoint Interrupt Enable Register */
  __I   uint8_t                        Reserved7[0x08];
  __O   uint32_t                       USBHS_DEVEPTIDR[10]; /**< Offset: 0x220 ( /W  32) Device Endpoint Interrupt Disable Register */
  __I   uint8_t                        Reserved8[0xC8];
        usbhs_devdma_registers_t       USBHS_DEVDMA[USBHS_DEVDMA_NUMBER]; /**< Offset: 0x310 Device DMA Channel Next Descriptor Address Register */
  __I   uint8_t                        Reserved9[0x80];
  __IO  uint32_t                       USBHS_HSTCTRL;   /**< Offset: 0x400 (R/W  32) Host General Control Register */
  __I   uint32_t                       USBHS_HSTISR;    /**< Offset: 0x404 (R/   32) Host Global Interrupt Status Register */
  __O   uint32_t                       USBHS_HSTICR;    /**< Offset: 0x408 ( /W  32) Host Global Interrupt Clear Register */
  __O   uint32_t                       USBHS_HSTIFR;    /**< Offset: 0x40c ( /W  32) Host Global Interrupt Set Register */
  __I   uint32_t                       USBHS_HSTIMR;    /**< Offset: 0x410 (R/   32) Host Global Interrupt Mask Register */
  __O   uint32_t                       USBHS_HSTIDR;    /**< Offset: 0x414 ( /W  32) Host Global Interrupt Disable Register */
  __O   uint32_t                       USBHS_HSTIER;    /**< Offset: 0x418 ( /W  32) Host Global Interrupt Enable Register */
  __IO  uint32_t                       USBHS_HSTPIP;    /**< Offset: 0x41c (R/W  32) Host Pipe Register */
  __IO  uint32_t                       USBHS_HSTFNUM;   /**< Offset: 0x420 (R/W  32) Host Frame Number Register */
  __IO  uint32_t                       USBHS_HSTADDR1;  /**< Offset: 0x424 (R/W  32) Host Address 1 Register */
  __IO  uint32_t                       USBHS_HSTADDR2;  /**< Offset: 0x428 (R/W  32) Host Address 2 Register */
  __IO  uint32_t                       USBHS_HSTADDR3;  /**< Offset: 0x42c (R/W  32) Host Address 3 Register */
  __I   uint8_t                        Reserved10[0xD0];
  __IO  uint32_t                       USBHS_HSTPIPCFG[10]; /**< Offset: 0x500 (R/W  32) Host Pipe Configuration Register */
  __I   uint8_t                        Reserved11[0x08];
  __I   uint32_t                       USBHS_HSTPIPISR[10]; /**< Offset: 0x530 (R/   32) Host Pipe Status Register */
  __I   uint8_t                        Reserved12[0x08];
  __O   uint32_t                       USBHS_HSTPIPICR[10]; /**< Offset: 0x560 ( /W  32) Host Pipe Clear Register */
  __I   uint8_t                        Reserved13[0x08];
  __O   uint32_t                       USBHS_HSTPIPIFR[10]; /**< Offset: 0x590 ( /W  32) Host Pipe Set Register */
  __I   uint8_t                        Reserved14[0x08];
  __I   uint32_t                       USBHS_HSTPIPIMR[10]; /**< Offset: 0x5c0 (R/   32) Host Pipe Mask Register */
  __I   uint8_t                        Reserved15[0x08];
  __O   uint32_t                       USBHS_HSTPIPIER[10]; /**< Offset: 0x5f0 ( /W  32) Host Pipe Enable Register */
  __I   uint8_t                        Reserved16[0x08];
  __O   uint32_t                       USBHS_HSTPIPIDR[10]; /**< Offset: 0x620 ( /W  32) Host Pipe Disable Register */
  __I   uint8_t                        Reserved17[0x08];
  __IO  uint32_t                       USBHS_HSTPIPINRQ[10]; /**< Offset: 0x650 (R/W  32) Host Pipe IN Request Register */
  __I   uint8_t                        Reserved18[0x08];
  __IO  uint32_t                       USBHS_HSTPIPERR[10]; /**< Offset: 0x680 (R/W  32) Host Pipe Error Register */
  __I   uint8_t                        Reserved19[0x68];
        usbhs_hstdma_registers_t       USBHS_HSTDMA[USBHS_HSTDMA_NUMBER]; /**< Offset: 0x710 Host DMA Channel Next Descriptor Address Register */
  __I   uint8_t                        Reserved20[0x80];
  __IO  uint32_t                       USBHS_CTRL;      /**< Offset: 0x800 (R/W  32) General Control Register */
  __I   uint32_t                       USBHS_SR;        /**< Offset: 0x804 (R/   32) General Status Register */
  __O   uint32_t                       USBHS_SCR;       /**< Offset: 0x808 ( /W  32) General Status Clear Register */
  __O   uint32_t                       USBHS_SFR;       /**< Offset: 0x80c ( /W  32) General Status Set Register */
} usbhs_registers_t;
/** @}  end of USB High-Speed Interface */

#endif /* SAME_SAME70_USBHS_MODULE_H */

