/**
 * \brief Header file for SAMV/SAMV71 MLB
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

/* file generated from device description version 2018-09-19T14:05:05Z */
#ifndef SAMV_SAMV71_MLB_MODULE_H
#define SAMV_SAMV71_MLB_MODULE_H

/** \addtogroup SAMV_SAMV71 MediaLB
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAMV71_MLB                                   */
/* ========================================================================== */

/* -------- MLB_MLBC0 : (MLB Offset: 0x00) (R/W 32) MediaLB Control 0 Register -------- */
#define MLB_MLBC0_MLBEN_Pos                   (0U)                                           /**< (MLB_MLBC0) MediaLB Enable Position */
#define MLB_MLBC0_MLBEN_Msk                   (0x1U << MLB_MLBC0_MLBEN_Pos)                  /**< (MLB_MLBC0) MediaLB Enable Mask */
#define MLB_MLBC0_MLBEN(value)                (MLB_MLBC0_MLBEN_Msk & ((value) << MLB_MLBC0_MLBEN_Pos))
#define MLB_MLBC0_MLBCLK_Pos                  (2U)                                           /**< (MLB_MLBC0) MLBCLK (MediaLB clock) Speed Select Position */
#define MLB_MLBC0_MLBCLK_Msk                  (0x7U << MLB_MLBC0_MLBCLK_Pos)                 /**< (MLB_MLBC0) MLBCLK (MediaLB clock) Speed Select Mask */
#define MLB_MLBC0_MLBCLK(value)               (MLB_MLBC0_MLBCLK_Msk & ((value) << MLB_MLBC0_MLBCLK_Pos))
#define   MLB_MLBC0_MLBCLK_256_FS_Val         (0U)                                           /**< (MLB_MLBC0) 256xFs (for MLBPEN = 0)  */
#define   MLB_MLBC0_MLBCLK_512_FS_Val         (1U)                                           /**< (MLB_MLBC0) 512xFs (for MLBPEN = 0)  */
#define   MLB_MLBC0_MLBCLK_1024_FS_Val        (2U)                                           /**< (MLB_MLBC0) 1024xFs (for MLBPEN = 0)  */
#define MLB_MLBC0_MLBCLK_256_FS               (MLB_MLBC0_MLBCLK_256_FS_Val << MLB_MLBC0_MLBCLK_Pos) /**< (MLB_MLBC0) 256xFs (for MLBPEN = 0) Position  */
#define MLB_MLBC0_MLBCLK_512_FS               (MLB_MLBC0_MLBCLK_512_FS_Val << MLB_MLBC0_MLBCLK_Pos) /**< (MLB_MLBC0) 512xFs (for MLBPEN = 0) Position  */
#define MLB_MLBC0_MLBCLK_1024_FS              (MLB_MLBC0_MLBCLK_1024_FS_Val << MLB_MLBC0_MLBCLK_Pos) /**< (MLB_MLBC0) 1024xFs (for MLBPEN = 0) Position  */
#define MLB_MLBC0_ZERO_Pos                    (5U)                                           /**< (MLB_MLBC0) Must be Written to 0 Position */
#define MLB_MLBC0_ZERO_Msk                    (0x1U << MLB_MLBC0_ZERO_Pos)                   /**< (MLB_MLBC0) Must be Written to 0 Mask */
#define MLB_MLBC0_ZERO(value)                 (MLB_MLBC0_ZERO_Msk & ((value) << MLB_MLBC0_ZERO_Pos))
#define MLB_MLBC0_MLBLK_Pos                   (7U)                                           /**< (MLB_MLBC0) MediaLB Lock Status (read-only) Position */
#define MLB_MLBC0_MLBLK_Msk                   (0x1U << MLB_MLBC0_MLBLK_Pos)                  /**< (MLB_MLBC0) MediaLB Lock Status (read-only) Mask */
#define MLB_MLBC0_MLBLK(value)                (MLB_MLBC0_MLBLK_Msk & ((value) << MLB_MLBC0_MLBLK_Pos))
#define MLB_MLBC0_ASYRETRY_Pos                (12U)                                          /**< (MLB_MLBC0) Asynchronous Tx Packet Retry Position */
#define MLB_MLBC0_ASYRETRY_Msk                (0x1U << MLB_MLBC0_ASYRETRY_Pos)               /**< (MLB_MLBC0) Asynchronous Tx Packet Retry Mask */
#define MLB_MLBC0_ASYRETRY(value)             (MLB_MLBC0_ASYRETRY_Msk & ((value) << MLB_MLBC0_ASYRETRY_Pos))
#define MLB_MLBC0_CTLRETRY_Pos                (14U)                                          /**< (MLB_MLBC0) Control Tx Packet Retry Position */
#define MLB_MLBC0_CTLRETRY_Msk                (0x1U << MLB_MLBC0_CTLRETRY_Pos)               /**< (MLB_MLBC0) Control Tx Packet Retry Mask */
#define MLB_MLBC0_CTLRETRY(value)             (MLB_MLBC0_CTLRETRY_Msk & ((value) << MLB_MLBC0_CTLRETRY_Pos))
#define MLB_MLBC0_FCNT_Pos                    (15U)                                          /**< (MLB_MLBC0) The number of frames per sub-buffer for synchronous channels Position */
#define MLB_MLBC0_FCNT_Msk                    (0x7U << MLB_MLBC0_FCNT_Pos)                   /**< (MLB_MLBC0) The number of frames per sub-buffer for synchronous channels Mask */
#define MLB_MLBC0_FCNT(value)                 (MLB_MLBC0_FCNT_Msk & ((value) << MLB_MLBC0_FCNT_Pos))
#define   MLB_MLBC0_FCNT_1_FRAME_Val          (0U)                                           /**< (MLB_MLBC0) 1 frame per sub-buffer (Operation is the same as Standard mode.)  */
#define   MLB_MLBC0_FCNT_2_FRAMES_Val         (1U)                                           /**< (MLB_MLBC0) 2 frames per sub-buffer  */
#define   MLB_MLBC0_FCNT_4_FRAMES_Val         (2U)                                           /**< (MLB_MLBC0) 4 frames per sub-buffer  */
#define   MLB_MLBC0_FCNT_8_FRAMES_Val         (3U)                                           /**< (MLB_MLBC0) 8 frames per sub-buffer  */
#define   MLB_MLBC0_FCNT_16_FRAMES_Val        (4U)                                           /**< (MLB_MLBC0) 16 frames per sub-buffer  */
#define   MLB_MLBC0_FCNT_32_FRAMES_Val        (5U)                                           /**< (MLB_MLBC0) 32 frames per sub-buffer  */
#define   MLB_MLBC0_FCNT_64_FRAMES_Val        (6U)                                           /**< (MLB_MLBC0) 64 frames per sub-buffer  */
#define MLB_MLBC0_FCNT_1_FRAME                (MLB_MLBC0_FCNT_1_FRAME_Val << MLB_MLBC0_FCNT_Pos) /**< (MLB_MLBC0) 1 frame per sub-buffer (Operation is the same as Standard mode.) Position  */
#define MLB_MLBC0_FCNT_2_FRAMES               (MLB_MLBC0_FCNT_2_FRAMES_Val << MLB_MLBC0_FCNT_Pos) /**< (MLB_MLBC0) 2 frames per sub-buffer Position  */
#define MLB_MLBC0_FCNT_4_FRAMES               (MLB_MLBC0_FCNT_4_FRAMES_Val << MLB_MLBC0_FCNT_Pos) /**< (MLB_MLBC0) 4 frames per sub-buffer Position  */
#define MLB_MLBC0_FCNT_8_FRAMES               (MLB_MLBC0_FCNT_8_FRAMES_Val << MLB_MLBC0_FCNT_Pos) /**< (MLB_MLBC0) 8 frames per sub-buffer Position  */
#define MLB_MLBC0_FCNT_16_FRAMES              (MLB_MLBC0_FCNT_16_FRAMES_Val << MLB_MLBC0_FCNT_Pos) /**< (MLB_MLBC0) 16 frames per sub-buffer Position  */
#define MLB_MLBC0_FCNT_32_FRAMES              (MLB_MLBC0_FCNT_32_FRAMES_Val << MLB_MLBC0_FCNT_Pos) /**< (MLB_MLBC0) 32 frames per sub-buffer Position  */
#define MLB_MLBC0_FCNT_64_FRAMES              (MLB_MLBC0_FCNT_64_FRAMES_Val << MLB_MLBC0_FCNT_Pos) /**< (MLB_MLBC0) 64 frames per sub-buffer Position  */
#define MLB_MLBC0_Msk                         (0x0003D0BDUL)                                 /**< (MLB_MLBC0) Register Mask  */

/* -------- MLB_MS0 : (MLB Offset: 0x0C) (R/W 32) MediaLB Channel Status 0 Register -------- */
#define MLB_MS0_MCS_Pos                       (0U)                                           /**< (MLB_MS0) MediaLB Channel Status [31:0] (cleared by writing a 0) Position */
#define MLB_MS0_MCS_Msk                       (0xFFFFFFFFU << MLB_MS0_MCS_Pos)               /**< (MLB_MS0) MediaLB Channel Status [31:0] (cleared by writing a 0) Mask */
#define MLB_MS0_MCS(value)                    (MLB_MS0_MCS_Msk & ((value) << MLB_MS0_MCS_Pos))
#define MLB_MS0_Msk                           (0xFFFFFFFFUL)                                 /**< (MLB_MS0) Register Mask  */

/* -------- MLB_MS1 : (MLB Offset: 0x14) (R/W 32) MediaLB Channel Status1 Register -------- */
#define MLB_MS1_MCS_Pos                       (0U)                                           /**< (MLB_MS1) MediaLB Channel Status [63:32] (cleared by writing a 0) Position */
#define MLB_MS1_MCS_Msk                       (0xFFFFFFFFU << MLB_MS1_MCS_Pos)               /**< (MLB_MS1) MediaLB Channel Status [63:32] (cleared by writing a 0) Mask */
#define MLB_MS1_MCS(value)                    (MLB_MS1_MCS_Msk & ((value) << MLB_MS1_MCS_Pos))
#define MLB_MS1_Msk                           (0xFFFFFFFFUL)                                 /**< (MLB_MS1) Register Mask  */

/* -------- MLB_MSS : (MLB Offset: 0x20) (R/W 32) MediaLB System Status Register -------- */
#define MLB_MSS_RSTSYSCMD_Pos                 (0U)                                           /**< (MLB_MSS) Reset System Command Detected in the System Quadlet (cleared by writing a 0) Position */
#define MLB_MSS_RSTSYSCMD_Msk                 (0x1U << MLB_MSS_RSTSYSCMD_Pos)                /**< (MLB_MSS) Reset System Command Detected in the System Quadlet (cleared by writing a 0) Mask */
#define MLB_MSS_RSTSYSCMD(value)              (MLB_MSS_RSTSYSCMD_Msk & ((value) << MLB_MSS_RSTSYSCMD_Pos))
#define MLB_MSS_LKSYSCMD_Pos                  (1U)                                           /**< (MLB_MSS) Network Lock System Command Detected in the System Quadlet (cleared by writing a 0) Position */
#define MLB_MSS_LKSYSCMD_Msk                  (0x1U << MLB_MSS_LKSYSCMD_Pos)                 /**< (MLB_MSS) Network Lock System Command Detected in the System Quadlet (cleared by writing a 0) Mask */
#define MLB_MSS_LKSYSCMD(value)               (MLB_MSS_LKSYSCMD_Msk & ((value) << MLB_MSS_LKSYSCMD_Pos))
#define MLB_MSS_ULKSYSCMD_Pos                 (2U)                                           /**< (MLB_MSS) Network Unlock System Command Detected in the System Quadlet (cleared by writing a 0) Position */
#define MLB_MSS_ULKSYSCMD_Msk                 (0x1U << MLB_MSS_ULKSYSCMD_Pos)                /**< (MLB_MSS) Network Unlock System Command Detected in the System Quadlet (cleared by writing a 0) Mask */
#define MLB_MSS_ULKSYSCMD(value)              (MLB_MSS_ULKSYSCMD_Msk & ((value) << MLB_MSS_ULKSYSCMD_Pos))
#define MLB_MSS_CSSYSCMD_Pos                  (3U)                                           /**< (MLB_MSS) Channel Scan System Command Detected in the System Quadlet (cleared by writing a 0) Position */
#define MLB_MSS_CSSYSCMD_Msk                  (0x1U << MLB_MSS_CSSYSCMD_Pos)                 /**< (MLB_MSS) Channel Scan System Command Detected in the System Quadlet (cleared by writing a 0) Mask */
#define MLB_MSS_CSSYSCMD(value)               (MLB_MSS_CSSYSCMD_Msk & ((value) << MLB_MSS_CSSYSCMD_Pos))
#define MLB_MSS_SWSYSCMD_Pos                  (4U)                                           /**< (MLB_MSS) Software System Command Detected in the System Quadlet (cleared by writing a 0) Position */
#define MLB_MSS_SWSYSCMD_Msk                  (0x1U << MLB_MSS_SWSYSCMD_Pos)                 /**< (MLB_MSS) Software System Command Detected in the System Quadlet (cleared by writing a 0) Mask */
#define MLB_MSS_SWSYSCMD(value)               (MLB_MSS_SWSYSCMD_Msk & ((value) << MLB_MSS_SWSYSCMD_Pos))
#define MLB_MSS_SERVREQ_Pos                   (5U)                                           /**< (MLB_MSS) Service Request Enabled Position */
#define MLB_MSS_SERVREQ_Msk                   (0x1U << MLB_MSS_SERVREQ_Pos)                  /**< (MLB_MSS) Service Request Enabled Mask */
#define MLB_MSS_SERVREQ(value)                (MLB_MSS_SERVREQ_Msk & ((value) << MLB_MSS_SERVREQ_Pos))
#define MLB_MSS_Msk                           (0x0000003FUL)                                 /**< (MLB_MSS) Register Mask  */

/* -------- MLB_MSD : (MLB Offset: 0x24) (R/  32) MediaLB System Data Register -------- */
#define MLB_MSD_SD0_Pos                       (0U)                                           /**< (MLB_MSD) System Data (Byte 0) Position */
#define MLB_MSD_SD0_Msk                       (0xFFU << MLB_MSD_SD0_Pos)                     /**< (MLB_MSD) System Data (Byte 0) Mask */
#define MLB_MSD_SD0(value)                    (MLB_MSD_SD0_Msk & ((value) << MLB_MSD_SD0_Pos))
#define MLB_MSD_SD1_Pos                       (8U)                                           /**< (MLB_MSD) System Data (Byte 1) Position */
#define MLB_MSD_SD1_Msk                       (0xFFU << MLB_MSD_SD1_Pos)                     /**< (MLB_MSD) System Data (Byte 1) Mask */
#define MLB_MSD_SD1(value)                    (MLB_MSD_SD1_Msk & ((value) << MLB_MSD_SD1_Pos))
#define MLB_MSD_SD2_Pos                       (16U)                                          /**< (MLB_MSD) System Data (Byte 2) Position */
#define MLB_MSD_SD2_Msk                       (0xFFU << MLB_MSD_SD2_Pos)                     /**< (MLB_MSD) System Data (Byte 2) Mask */
#define MLB_MSD_SD2(value)                    (MLB_MSD_SD2_Msk & ((value) << MLB_MSD_SD2_Pos))
#define MLB_MSD_SD3_Pos                       (24U)                                          /**< (MLB_MSD) System Data (Byte 3) Position */
#define MLB_MSD_SD3_Msk                       (0xFFU << MLB_MSD_SD3_Pos)                     /**< (MLB_MSD) System Data (Byte 3) Mask */
#define MLB_MSD_SD3(value)                    (MLB_MSD_SD3_Msk & ((value) << MLB_MSD_SD3_Pos))
#define MLB_MSD_Msk                           (0xFFFFFFFFUL)                                 /**< (MLB_MSD) Register Mask  */

/* -------- MLB_MIEN : (MLB Offset: 0x2C) (R/W 32) MediaLB Interrupt Enable Register -------- */
#define MLB_MIEN_ISOC_PE_Pos                  (0U)                                           /**< (MLB_MIEN) Isochronous Rx Protocol Error Enable Position */
#define MLB_MIEN_ISOC_PE_Msk                  (0x1U << MLB_MIEN_ISOC_PE_Pos)                 /**< (MLB_MIEN) Isochronous Rx Protocol Error Enable Mask */
#define MLB_MIEN_ISOC_PE(value)               (MLB_MIEN_ISOC_PE_Msk & ((value) << MLB_MIEN_ISOC_PE_Pos))
#define MLB_MIEN_ISOC_BUFO_Pos                (1U)                                           /**< (MLB_MIEN) Isochronous Rx Buffer Overflow Enable Position */
#define MLB_MIEN_ISOC_BUFO_Msk                (0x1U << MLB_MIEN_ISOC_BUFO_Pos)               /**< (MLB_MIEN) Isochronous Rx Buffer Overflow Enable Mask */
#define MLB_MIEN_ISOC_BUFO(value)             (MLB_MIEN_ISOC_BUFO_Msk & ((value) << MLB_MIEN_ISOC_BUFO_Pos))
#define MLB_MIEN_SYNC_PE_Pos                  (16U)                                          /**< (MLB_MIEN) Synchronous Protocol Error Enable Position */
#define MLB_MIEN_SYNC_PE_Msk                  (0x1U << MLB_MIEN_SYNC_PE_Pos)                 /**< (MLB_MIEN) Synchronous Protocol Error Enable Mask */
#define MLB_MIEN_SYNC_PE(value)               (MLB_MIEN_SYNC_PE_Msk & ((value) << MLB_MIEN_SYNC_PE_Pos))
#define MLB_MIEN_ARX_DONE_Pos                 (17U)                                          /**< (MLB_MIEN) Asynchronous Rx Done Enable Position */
#define MLB_MIEN_ARX_DONE_Msk                 (0x1U << MLB_MIEN_ARX_DONE_Pos)                /**< (MLB_MIEN) Asynchronous Rx Done Enable Mask */
#define MLB_MIEN_ARX_DONE(value)              (MLB_MIEN_ARX_DONE_Msk & ((value) << MLB_MIEN_ARX_DONE_Pos))
#define MLB_MIEN_ARX_PE_Pos                   (18U)                                          /**< (MLB_MIEN) Asynchronous Rx Protocol Error Enable Position */
#define MLB_MIEN_ARX_PE_Msk                   (0x1U << MLB_MIEN_ARX_PE_Pos)                  /**< (MLB_MIEN) Asynchronous Rx Protocol Error Enable Mask */
#define MLB_MIEN_ARX_PE(value)                (MLB_MIEN_ARX_PE_Msk & ((value) << MLB_MIEN_ARX_PE_Pos))
#define MLB_MIEN_ARX_BREAK_Pos                (19U)                                          /**< (MLB_MIEN) Asynchronous Rx Break Enable Position */
#define MLB_MIEN_ARX_BREAK_Msk                (0x1U << MLB_MIEN_ARX_BREAK_Pos)               /**< (MLB_MIEN) Asynchronous Rx Break Enable Mask */
#define MLB_MIEN_ARX_BREAK(value)             (MLB_MIEN_ARX_BREAK_Msk & ((value) << MLB_MIEN_ARX_BREAK_Pos))
#define MLB_MIEN_ATX_DONE_Pos                 (20U)                                          /**< (MLB_MIEN) Asynchronous Tx Packet Done Enable Position */
#define MLB_MIEN_ATX_DONE_Msk                 (0x1U << MLB_MIEN_ATX_DONE_Pos)                /**< (MLB_MIEN) Asynchronous Tx Packet Done Enable Mask */
#define MLB_MIEN_ATX_DONE(value)              (MLB_MIEN_ATX_DONE_Msk & ((value) << MLB_MIEN_ATX_DONE_Pos))
#define MLB_MIEN_ATX_PE_Pos                   (21U)                                          /**< (MLB_MIEN) Asynchronous Tx Protocol Error Enable Position */
#define MLB_MIEN_ATX_PE_Msk                   (0x1U << MLB_MIEN_ATX_PE_Pos)                  /**< (MLB_MIEN) Asynchronous Tx Protocol Error Enable Mask */
#define MLB_MIEN_ATX_PE(value)                (MLB_MIEN_ATX_PE_Msk & ((value) << MLB_MIEN_ATX_PE_Pos))
#define MLB_MIEN_ATX_BREAK_Pos                (22U)                                          /**< (MLB_MIEN) Asynchronous Tx Break Enable Position */
#define MLB_MIEN_ATX_BREAK_Msk                (0x1U << MLB_MIEN_ATX_BREAK_Pos)               /**< (MLB_MIEN) Asynchronous Tx Break Enable Mask */
#define MLB_MIEN_ATX_BREAK(value)             (MLB_MIEN_ATX_BREAK_Msk & ((value) << MLB_MIEN_ATX_BREAK_Pos))
#define MLB_MIEN_CRX_DONE_Pos                 (24U)                                          /**< (MLB_MIEN) Control Rx Packet Done Enable Position */
#define MLB_MIEN_CRX_DONE_Msk                 (0x1U << MLB_MIEN_CRX_DONE_Pos)                /**< (MLB_MIEN) Control Rx Packet Done Enable Mask */
#define MLB_MIEN_CRX_DONE(value)              (MLB_MIEN_CRX_DONE_Msk & ((value) << MLB_MIEN_CRX_DONE_Pos))
#define MLB_MIEN_CRX_PE_Pos                   (25U)                                          /**< (MLB_MIEN) Control Rx Protocol Error Enable Position */
#define MLB_MIEN_CRX_PE_Msk                   (0x1U << MLB_MIEN_CRX_PE_Pos)                  /**< (MLB_MIEN) Control Rx Protocol Error Enable Mask */
#define MLB_MIEN_CRX_PE(value)                (MLB_MIEN_CRX_PE_Msk & ((value) << MLB_MIEN_CRX_PE_Pos))
#define MLB_MIEN_CRX_BREAK_Pos                (26U)                                          /**< (MLB_MIEN) Control Rx Break Enable Position */
#define MLB_MIEN_CRX_BREAK_Msk                (0x1U << MLB_MIEN_CRX_BREAK_Pos)               /**< (MLB_MIEN) Control Rx Break Enable Mask */
#define MLB_MIEN_CRX_BREAK(value)             (MLB_MIEN_CRX_BREAK_Msk & ((value) << MLB_MIEN_CRX_BREAK_Pos))
#define MLB_MIEN_CTX_DONE_Pos                 (27U)                                          /**< (MLB_MIEN) Control Tx Packet Done Enable Position */
#define MLB_MIEN_CTX_DONE_Msk                 (0x1U << MLB_MIEN_CTX_DONE_Pos)                /**< (MLB_MIEN) Control Tx Packet Done Enable Mask */
#define MLB_MIEN_CTX_DONE(value)              (MLB_MIEN_CTX_DONE_Msk & ((value) << MLB_MIEN_CTX_DONE_Pos))
#define MLB_MIEN_CTX_PE_Pos                   (28U)                                          /**< (MLB_MIEN) Control Tx Protocol Error Enable Position */
#define MLB_MIEN_CTX_PE_Msk                   (0x1U << MLB_MIEN_CTX_PE_Pos)                  /**< (MLB_MIEN) Control Tx Protocol Error Enable Mask */
#define MLB_MIEN_CTX_PE(value)                (MLB_MIEN_CTX_PE_Msk & ((value) << MLB_MIEN_CTX_PE_Pos))
#define MLB_MIEN_CTX_BREAK_Pos                (29U)                                          /**< (MLB_MIEN) Control Tx Break Enable Position */
#define MLB_MIEN_CTX_BREAK_Msk                (0x1U << MLB_MIEN_CTX_BREAK_Pos)               /**< (MLB_MIEN) Control Tx Break Enable Mask */
#define MLB_MIEN_CTX_BREAK(value)             (MLB_MIEN_CTX_BREAK_Msk & ((value) << MLB_MIEN_CTX_BREAK_Pos))
#define MLB_MIEN_Msk                          (0x3F7F0003UL)                                 /**< (MLB_MIEN) Register Mask  */

/* -------- MLB_MLBC1 : (MLB Offset: 0x3C) (R/W 32) MediaLB Control 1 Register -------- */
#define MLB_MLBC1_LOCK_Pos                    (6U)                                           /**< (MLB_MLBC1) MediaLB Lock Error Status (cleared by writing a 0) Position */
#define MLB_MLBC1_LOCK_Msk                    (0x1U << MLB_MLBC1_LOCK_Pos)                   /**< (MLB_MLBC1) MediaLB Lock Error Status (cleared by writing a 0) Mask */
#define MLB_MLBC1_LOCK(value)                 (MLB_MLBC1_LOCK_Msk & ((value) << MLB_MLBC1_LOCK_Pos))
#define MLB_MLBC1_CLKM_Pos                    (7U)                                           /**< (MLB_MLBC1) MediaLB Clock Missing Status (cleared by writing a 0) Position */
#define MLB_MLBC1_CLKM_Msk                    (0x1U << MLB_MLBC1_CLKM_Pos)                   /**< (MLB_MLBC1) MediaLB Clock Missing Status (cleared by writing a 0) Mask */
#define MLB_MLBC1_CLKM(value)                 (MLB_MLBC1_CLKM_Msk & ((value) << MLB_MLBC1_CLKM_Pos))
#define MLB_MLBC1_NDA_Pos                     (8U)                                           /**< (MLB_MLBC1) Node Device Address Position */
#define MLB_MLBC1_NDA_Msk                     (0xFFU << MLB_MLBC1_NDA_Pos)                   /**< (MLB_MLBC1) Node Device Address Mask */
#define MLB_MLBC1_NDA(value)                  (MLB_MLBC1_NDA_Msk & ((value) << MLB_MLBC1_NDA_Pos))
#define MLB_MLBC1_Msk                         (0x0000FFC0UL)                                 /**< (MLB_MLBC1) Register Mask  */

/* -------- MLB_HCTL : (MLB Offset: 0x80) (R/W 32) HBI Control Register -------- */
#define MLB_HCTL_RST0_Pos                     (0U)                                           /**< (MLB_HCTL) Address Generation Unit 0 Software Reset Position */
#define MLB_HCTL_RST0_Msk                     (0x1U << MLB_HCTL_RST0_Pos)                    /**< (MLB_HCTL) Address Generation Unit 0 Software Reset Mask */
#define MLB_HCTL_RST0(value)                  (MLB_HCTL_RST0_Msk & ((value) << MLB_HCTL_RST0_Pos))
#define MLB_HCTL_RST1_Pos                     (1U)                                           /**< (MLB_HCTL) Address Generation Unit 1 Software Reset Position */
#define MLB_HCTL_RST1_Msk                     (0x1U << MLB_HCTL_RST1_Pos)                    /**< (MLB_HCTL) Address Generation Unit 1 Software Reset Mask */
#define MLB_HCTL_RST1(value)                  (MLB_HCTL_RST1_Msk & ((value) << MLB_HCTL_RST1_Pos))
#define MLB_HCTL_EN_Pos                       (15U)                                          /**< (MLB_HCTL) HBI Enable Position */
#define MLB_HCTL_EN_Msk                       (0x1U << MLB_HCTL_EN_Pos)                      /**< (MLB_HCTL) HBI Enable Mask */
#define MLB_HCTL_EN(value)                    (MLB_HCTL_EN_Msk & ((value) << MLB_HCTL_EN_Pos))
#define MLB_HCTL_Msk                          (0x00008003UL)                                 /**< (MLB_HCTL) Register Mask  */

/* -------- MLB_HCMR : (MLB Offset: 0x88) (R/W 32) HBI Channel Mask 0 Register 0 -------- */
#define MLB_HCMR_CHM_Pos                      (0U)                                           /**< (MLB_HCMR) Bitwise Channel Mask Bit [31:0] Position */
#define MLB_HCMR_CHM_Msk                      (0xFFFFFFFFU << MLB_HCMR_CHM_Pos)              /**< (MLB_HCMR) Bitwise Channel Mask Bit [31:0] Mask */
#define MLB_HCMR_CHM(value)                   (MLB_HCMR_CHM_Msk & ((value) << MLB_HCMR_CHM_Pos))
#define MLB_HCMR_Msk                          (0xFFFFFFFFUL)                                 /**< (MLB_HCMR) Register Mask  */

/* -------- MLB_HCER : (MLB Offset: 0x90) (R/  32) HBI Channel Error 0 Register 0 -------- */
#define MLB_HCER_CERR_Pos                     (0U)                                           /**< (MLB_HCER) Bitwise Channel Error Bit [31:0] Position */
#define MLB_HCER_CERR_Msk                     (0xFFFFFFFFU << MLB_HCER_CERR_Pos)             /**< (MLB_HCER) Bitwise Channel Error Bit [31:0] Mask */
#define MLB_HCER_CERR(value)                  (MLB_HCER_CERR_Msk & ((value) << MLB_HCER_CERR_Pos))
#define MLB_HCER_Msk                          (0xFFFFFFFFUL)                                 /**< (MLB_HCER) Register Mask  */

/* -------- MLB_HCBR : (MLB Offset: 0x98) (R/  32) HBI Channel Busy 0 Register 0 -------- */
#define MLB_HCBR_CHB_Pos                      (0U)                                           /**< (MLB_HCBR) Bitwise Channel Busy Bit [31:0] Position */
#define MLB_HCBR_CHB_Msk                      (0xFFFFFFFFU << MLB_HCBR_CHB_Pos)              /**< (MLB_HCBR) Bitwise Channel Busy Bit [31:0] Mask */
#define MLB_HCBR_CHB(value)                   (MLB_HCBR_CHB_Msk & ((value) << MLB_HCBR_CHB_Pos))
#define MLB_HCBR_Msk                          (0xFFFFFFFFUL)                                 /**< (MLB_HCBR) Register Mask  */

/* -------- MLB_MDAT : (MLB Offset: 0xC0) (R/W 32) MIF Data 0 Register 0 -------- */
#define MLB_MDAT_DATA_Pos                     (0U)                                           /**< (MLB_MDAT) CRT or DBR Data Position */
#define MLB_MDAT_DATA_Msk                     (0xFFFFFFFFU << MLB_MDAT_DATA_Pos)             /**< (MLB_MDAT) CRT or DBR Data Mask */
#define MLB_MDAT_DATA(value)                  (MLB_MDAT_DATA_Msk & ((value) << MLB_MDAT_DATA_Pos))
#define MLB_MDAT_Msk                          (0xFFFFFFFFUL)                                 /**< (MLB_MDAT) Register Mask  */

/* -------- MLB_MDWE : (MLB Offset: 0xD0) (R/W 32) MIF Data Write Enable 0 Register 0 -------- */
#define MLB_MDWE_MASK_Pos                     (0U)                                           /**< (MLB_MDWE) Bitwise Write Enable for CTR Data - bits[31:0] Position */
#define MLB_MDWE_MASK_Msk                     (0xFFFFFFFFU << MLB_MDWE_MASK_Pos)             /**< (MLB_MDWE) Bitwise Write Enable for CTR Data - bits[31:0] Mask */
#define MLB_MDWE_MASK(value)                  (MLB_MDWE_MASK_Msk & ((value) << MLB_MDWE_MASK_Pos))
#define MLB_MDWE_Msk                          (0xFFFFFFFFUL)                                 /**< (MLB_MDWE) Register Mask  */

/* -------- MLB_MCTL : (MLB Offset: 0xE0) (R/W 32) MIF Control Register -------- */
#define MLB_MCTL_XCMP_Pos                     (0U)                                           /**< (MLB_MCTL) Transfer Complete (Write 0 to Clear) Position */
#define MLB_MCTL_XCMP_Msk                     (0x1U << MLB_MCTL_XCMP_Pos)                    /**< (MLB_MCTL) Transfer Complete (Write 0 to Clear) Mask */
#define MLB_MCTL_XCMP(value)                  (MLB_MCTL_XCMP_Msk & ((value) << MLB_MCTL_XCMP_Pos))
#define MLB_MCTL_Msk                          (0x00000001UL)                                 /**< (MLB_MCTL) Register Mask  */

/* -------- MLB_MADR : (MLB Offset: 0xE4) (R/W 32) MIF Address Register -------- */
#define MLB_MADR_ADDR_Pos                     (0U)                                           /**< (MLB_MADR) CTR or DBR Address Position */
#define MLB_MADR_ADDR_Msk                     (0x3FFFU << MLB_MADR_ADDR_Pos)                 /**< (MLB_MADR) CTR or DBR Address Mask */
#define MLB_MADR_ADDR(value)                  (MLB_MADR_ADDR_Msk & ((value) << MLB_MADR_ADDR_Pos))
#define MLB_MADR_TB_Pos                       (30U)                                          /**< (MLB_MADR) Target Location Bit Position */
#define MLB_MADR_TB_Msk                       (0x1U << MLB_MADR_TB_Pos)                      /**< (MLB_MADR) Target Location Bit Mask */
#define MLB_MADR_TB(value)                    (MLB_MADR_TB_Msk & ((value) << MLB_MADR_TB_Pos))
#define   MLB_MADR_TB_CTR_Val                 (0U)                                           /**< (MLB_MADR) Selects CTR  */
#define   MLB_MADR_TB_DBR_Val                 (1U)                                           /**< (MLB_MADR) Selects DBR  */
#define MLB_MADR_TB_CTR                       (MLB_MADR_TB_CTR_Val << MLB_MADR_TB_Pos)       /**< (MLB_MADR) Selects CTR Position  */
#define MLB_MADR_TB_DBR                       (MLB_MADR_TB_DBR_Val << MLB_MADR_TB_Pos)       /**< (MLB_MADR) Selects DBR Position  */
#define MLB_MADR_WNR_Pos                      (31U)                                          /**< (MLB_MADR) Write-Not-Read Selection Position */
#define MLB_MADR_WNR_Msk                      (0x1U << MLB_MADR_WNR_Pos)                     /**< (MLB_MADR) Write-Not-Read Selection Mask */
#define MLB_MADR_WNR(value)                   (MLB_MADR_WNR_Msk & ((value) << MLB_MADR_WNR_Pos))
#define MLB_MADR_Msk                          (0xC0003FFFUL)                                 /**< (MLB_MADR) Register Mask  */

/* -------- MLB_ACTL : (MLB Offset: 0x3C0) (R/W 32) AHB Control Register -------- */
#define MLB_ACTL_SCE_Pos                      (0U)                                           /**< (MLB_ACTL) Software Clear Enable Position */
#define MLB_ACTL_SCE_Msk                      (0x1U << MLB_ACTL_SCE_Pos)                     /**< (MLB_ACTL) Software Clear Enable Mask */
#define MLB_ACTL_SCE(value)                   (MLB_ACTL_SCE_Msk & ((value) << MLB_ACTL_SCE_Pos))
#define MLB_ACTL_SMX_Pos                      (1U)                                           /**< (MLB_ACTL) AHB Interrupt Mux Enable Position */
#define MLB_ACTL_SMX_Msk                      (0x1U << MLB_ACTL_SMX_Pos)                     /**< (MLB_ACTL) AHB Interrupt Mux Enable Mask */
#define MLB_ACTL_SMX(value)                   (MLB_ACTL_SMX_Msk & ((value) << MLB_ACTL_SMX_Pos))
#define MLB_ACTL_DMA_MODE_Pos                 (2U)                                           /**< (MLB_ACTL) DMA Mode Position */
#define MLB_ACTL_DMA_MODE_Msk                 (0x1U << MLB_ACTL_DMA_MODE_Pos)                /**< (MLB_ACTL) DMA Mode Mask */
#define MLB_ACTL_DMA_MODE(value)              (MLB_ACTL_DMA_MODE_Msk & ((value) << MLB_ACTL_DMA_MODE_Pos))
#define MLB_ACTL_MPB_Pos                      (4U)                                           /**< (MLB_ACTL) DMA Packet Buffering Mode Position */
#define MLB_ACTL_MPB_Msk                      (0x1U << MLB_ACTL_MPB_Pos)                     /**< (MLB_ACTL) DMA Packet Buffering Mode Mask */
#define MLB_ACTL_MPB(value)                   (MLB_ACTL_MPB_Msk & ((value) << MLB_ACTL_MPB_Pos))
#define   MLB_ACTL_MPB_SINGLE_PACKET_Val      (0U)                                           /**< (MLB_ACTL) Single-packet mode  */
#define   MLB_ACTL_MPB_MULTIPLE_PACKET_Val    (1U)                                           /**< (MLB_ACTL) Multiple-packet mode  */
#define MLB_ACTL_MPB_SINGLE_PACKET            (MLB_ACTL_MPB_SINGLE_PACKET_Val << MLB_ACTL_MPB_Pos) /**< (MLB_ACTL) Single-packet mode Position  */
#define MLB_ACTL_MPB_MULTIPLE_PACKET          (MLB_ACTL_MPB_MULTIPLE_PACKET_Val << MLB_ACTL_MPB_Pos) /**< (MLB_ACTL) Multiple-packet mode Position  */
#define MLB_ACTL_Msk                          (0x00000017UL)                                 /**< (MLB_ACTL) Register Mask  */

/* -------- MLB_ACSR : (MLB Offset: 0x3D0) (R/W 32) AHB Channel Status 0 Register 0 -------- */
#define MLB_ACSR_CHS_Pos                      (0U)                                           /**< (MLB_ACSR) Interrupt Status for Logical Channels [31:0] (cleared by writing a 1) Position */
#define MLB_ACSR_CHS_Msk                      (0xFFFFFFFFU << MLB_ACSR_CHS_Pos)              /**< (MLB_ACSR) Interrupt Status for Logical Channels [31:0] (cleared by writing a 1) Mask */
#define MLB_ACSR_CHS(value)                   (MLB_ACSR_CHS_Msk & ((value) << MLB_ACSR_CHS_Pos))
#define MLB_ACSR_Msk                          (0xFFFFFFFFUL)                                 /**< (MLB_ACSR) Register Mask  */

/* -------- MLB_ACMR : (MLB Offset: 0x3D8) (R/W 32) AHB Channel Mask 0 Register 0 -------- */
#define MLB_ACMR_CHM_Pos                      (0U)                                           /**< (MLB_ACMR) Bitwise Channel Mask Bits 31 to 0 Position */
#define MLB_ACMR_CHM_Msk                      (0xFFFFFFFFU << MLB_ACMR_CHM_Pos)              /**< (MLB_ACMR) Bitwise Channel Mask Bits 31 to 0 Mask */
#define MLB_ACMR_CHM(value)                   (MLB_ACMR_CHM_Msk & ((value) << MLB_ACMR_CHM_Pos))
#define MLB_ACMR_Msk                          (0xFFFFFFFFUL)                                 /**< (MLB_ACMR) Register Mask  */

/** \brief MLB register offsets definitions */
#define MLB_MLBC0_OFFSET               (0x00)         /**< (MLB_MLBC0) MediaLB Control 0 Register Offset */
#define MLB_MS0_OFFSET                 (0x0C)         /**< (MLB_MS0) MediaLB Channel Status 0 Register Offset */
#define MLB_MS1_OFFSET                 (0x14)         /**< (MLB_MS1) MediaLB Channel Status1 Register Offset */
#define MLB_MSS_OFFSET                 (0x20)         /**< (MLB_MSS) MediaLB System Status Register Offset */
#define MLB_MSD_OFFSET                 (0x24)         /**< (MLB_MSD) MediaLB System Data Register Offset */
#define MLB_MIEN_OFFSET                (0x2C)         /**< (MLB_MIEN) MediaLB Interrupt Enable Register Offset */
#define MLB_MLBC1_OFFSET               (0x3C)         /**< (MLB_MLBC1) MediaLB Control 1 Register Offset */
#define MLB_HCTL_OFFSET                (0x80)         /**< (MLB_HCTL) HBI Control Register Offset */
#define MLB_HCMR_OFFSET                (0x88)         /**< (MLB_HCMR) HBI Channel Mask 0 Register 0 Offset */
#define MLB_HCER_OFFSET                (0x90)         /**< (MLB_HCER) HBI Channel Error 0 Register 0 Offset */
#define MLB_HCBR_OFFSET                (0x98)         /**< (MLB_HCBR) HBI Channel Busy 0 Register 0 Offset */
#define MLB_MDAT_OFFSET                (0xC0)         /**< (MLB_MDAT) MIF Data 0 Register 0 Offset */
#define MLB_MDWE_OFFSET                (0xD0)         /**< (MLB_MDWE) MIF Data Write Enable 0 Register 0 Offset */
#define MLB_MCTL_OFFSET                (0xE0)         /**< (MLB_MCTL) MIF Control Register Offset */
#define MLB_MADR_OFFSET                (0xE4)         /**< (MLB_MADR) MIF Address Register Offset */
#define MLB_ACTL_OFFSET                (0x3C0)        /**< (MLB_ACTL) AHB Control Register Offset */
#define MLB_ACSR_OFFSET                (0x3D0)        /**< (MLB_ACSR) AHB Channel Status 0 Register 0 Offset */
#define MLB_ACMR_OFFSET                (0x3D8)        /**< (MLB_ACMR) AHB Channel Mask 0 Register 0 Offset */

/** \brief MLB register API structure */
typedef struct
{
  __IO  uint32_t                       MLB_MLBC0;       /**< Offset: 0x00 (R/W  32) MediaLB Control 0 Register */
  __I   uint8_t                        Reserved1[0x08];
  __IO  uint32_t                       MLB_MS0;         /**< Offset: 0x0c (R/W  32) MediaLB Channel Status 0 Register */
  __I   uint8_t                        Reserved2[0x04];
  __IO  uint32_t                       MLB_MS1;         /**< Offset: 0x14 (R/W  32) MediaLB Channel Status1 Register */
  __I   uint8_t                        Reserved3[0x08];
  __IO  uint32_t                       MLB_MSS;         /**< Offset: 0x20 (R/W  32) MediaLB System Status Register */
  __I   uint32_t                       MLB_MSD;         /**< Offset: 0x24 (R/   32) MediaLB System Data Register */
  __I   uint8_t                        Reserved4[0x04];
  __IO  uint32_t                       MLB_MIEN;        /**< Offset: 0x2c (R/W  32) MediaLB Interrupt Enable Register */
  __I   uint8_t                        Reserved5[0x0C];
  __IO  uint32_t                       MLB_MLBC1;       /**< Offset: 0x3c (R/W  32) MediaLB Control 1 Register */
  __I   uint8_t                        Reserved6[0x40];
  __IO  uint32_t                       MLB_HCTL;        /**< Offset: 0x80 (R/W  32) HBI Control Register */
  __I   uint8_t                        Reserved7[0x04];
  __IO  uint32_t                       MLB_HCMR[2];     /**< Offset: 0x88 (R/W  32) HBI Channel Mask 0 Register 0 */
  __I   uint32_t                       MLB_HCER[2];     /**< Offset: 0x90 (R/   32) HBI Channel Error 0 Register 0 */
  __I   uint32_t                       MLB_HCBR[2];     /**< Offset: 0x98 (R/   32) HBI Channel Busy 0 Register 0 */
  __I   uint8_t                        Reserved8[0x20];
  __IO  uint32_t                       MLB_MDAT[4];     /**< Offset: 0xc0 (R/W  32) MIF Data 0 Register 0 */
  __IO  uint32_t                       MLB_MDWE[4];     /**< Offset: 0xd0 (R/W  32) MIF Data Write Enable 0 Register 0 */
  __IO  uint32_t                       MLB_MCTL;        /**< Offset: 0xe0 (R/W  32) MIF Control Register */
  __IO  uint32_t                       MLB_MADR;        /**< Offset: 0xe4 (R/W  32) MIF Address Register */
  __I   uint8_t                        Reserved9[0x2D8];
  __IO  uint32_t                       MLB_ACTL;        /**< Offset: 0x3c0 (R/W  32) AHB Control Register */
  __I   uint8_t                        Reserved10[0x0C];
  __IO  uint32_t                       MLB_ACSR[2];     /**< Offset: 0x3d0 (R/W  32) AHB Channel Status 0 Register 0 */
  __IO  uint32_t                       MLB_ACMR[2];     /**< Offset: 0x3d8 (R/W  32) AHB Channel Mask 0 Register 0 */
} mlb_registers_t;
/** @}  end of MediaLB */

#endif /* SAMV_SAMV71_MLB_MODULE_H */

