/**
 * \brief Header file for SAME/SAME70 TWIHS
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
#ifndef SAME_SAME70_TWIHS_MODULE_H
#define SAME_SAME70_TWIHS_MODULE_H

/** \addtogroup SAME_SAME70 Two-wire Interface High Speed
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_TWIHS                                 */
/* ========================================================================== */

/* -------- TWIHS_CR : (TWIHS Offset: 0x00) ( /W 32) Control Register -------- */
#define TWIHS_CR_START_Pos                    (0U)                                           /**< (TWIHS_CR) Send a START Condition Position */
#define TWIHS_CR_START_Msk                    (0x1U << TWIHS_CR_START_Pos)                   /**< (TWIHS_CR) Send a START Condition Mask */
#define TWIHS_CR_START(value)                 (TWIHS_CR_START_Msk & ((value) << TWIHS_CR_START_Pos))
#define TWIHS_CR_STOP_Pos                     (1U)                                           /**< (TWIHS_CR) Send a STOP Condition Position */
#define TWIHS_CR_STOP_Msk                     (0x1U << TWIHS_CR_STOP_Pos)                    /**< (TWIHS_CR) Send a STOP Condition Mask */
#define TWIHS_CR_STOP(value)                  (TWIHS_CR_STOP_Msk & ((value) << TWIHS_CR_STOP_Pos))
#define TWIHS_CR_MSEN_Pos                     (2U)                                           /**< (TWIHS_CR) TWIHS Master Mode Enabled Position */
#define TWIHS_CR_MSEN_Msk                     (0x1U << TWIHS_CR_MSEN_Pos)                    /**< (TWIHS_CR) TWIHS Master Mode Enabled Mask */
#define TWIHS_CR_MSEN(value)                  (TWIHS_CR_MSEN_Msk & ((value) << TWIHS_CR_MSEN_Pos))
#define TWIHS_CR_MSDIS_Pos                    (3U)                                           /**< (TWIHS_CR) TWIHS Master Mode Disabled Position */
#define TWIHS_CR_MSDIS_Msk                    (0x1U << TWIHS_CR_MSDIS_Pos)                   /**< (TWIHS_CR) TWIHS Master Mode Disabled Mask */
#define TWIHS_CR_MSDIS(value)                 (TWIHS_CR_MSDIS_Msk & ((value) << TWIHS_CR_MSDIS_Pos))
#define TWIHS_CR_SVEN_Pos                     (4U)                                           /**< (TWIHS_CR) TWIHS Slave Mode Enabled Position */
#define TWIHS_CR_SVEN_Msk                     (0x1U << TWIHS_CR_SVEN_Pos)                    /**< (TWIHS_CR) TWIHS Slave Mode Enabled Mask */
#define TWIHS_CR_SVEN(value)                  (TWIHS_CR_SVEN_Msk & ((value) << TWIHS_CR_SVEN_Pos))
#define TWIHS_CR_SVDIS_Pos                    (5U)                                           /**< (TWIHS_CR) TWIHS Slave Mode Disabled Position */
#define TWIHS_CR_SVDIS_Msk                    (0x1U << TWIHS_CR_SVDIS_Pos)                   /**< (TWIHS_CR) TWIHS Slave Mode Disabled Mask */
#define TWIHS_CR_SVDIS(value)                 (TWIHS_CR_SVDIS_Msk & ((value) << TWIHS_CR_SVDIS_Pos))
#define TWIHS_CR_QUICK_Pos                    (6U)                                           /**< (TWIHS_CR) SMBus Quick Command Position */
#define TWIHS_CR_QUICK_Msk                    (0x1U << TWIHS_CR_QUICK_Pos)                   /**< (TWIHS_CR) SMBus Quick Command Mask */
#define TWIHS_CR_QUICK(value)                 (TWIHS_CR_QUICK_Msk & ((value) << TWIHS_CR_QUICK_Pos))
#define TWIHS_CR_SWRST_Pos                    (7U)                                           /**< (TWIHS_CR) Software Reset Position */
#define TWIHS_CR_SWRST_Msk                    (0x1U << TWIHS_CR_SWRST_Pos)                   /**< (TWIHS_CR) Software Reset Mask */
#define TWIHS_CR_SWRST(value)                 (TWIHS_CR_SWRST_Msk & ((value) << TWIHS_CR_SWRST_Pos))
#define TWIHS_CR_HSEN_Pos                     (8U)                                           /**< (TWIHS_CR) TWIHS High-Speed Mode Enabled Position */
#define TWIHS_CR_HSEN_Msk                     (0x1U << TWIHS_CR_HSEN_Pos)                    /**< (TWIHS_CR) TWIHS High-Speed Mode Enabled Mask */
#define TWIHS_CR_HSEN(value)                  (TWIHS_CR_HSEN_Msk & ((value) << TWIHS_CR_HSEN_Pos))
#define TWIHS_CR_HSDIS_Pos                    (9U)                                           /**< (TWIHS_CR) TWIHS High-Speed Mode Disabled Position */
#define TWIHS_CR_HSDIS_Msk                    (0x1U << TWIHS_CR_HSDIS_Pos)                   /**< (TWIHS_CR) TWIHS High-Speed Mode Disabled Mask */
#define TWIHS_CR_HSDIS(value)                 (TWIHS_CR_HSDIS_Msk & ((value) << TWIHS_CR_HSDIS_Pos))
#define TWIHS_CR_SMBEN_Pos                    (10U)                                          /**< (TWIHS_CR) SMBus Mode Enabled Position */
#define TWIHS_CR_SMBEN_Msk                    (0x1U << TWIHS_CR_SMBEN_Pos)                   /**< (TWIHS_CR) SMBus Mode Enabled Mask */
#define TWIHS_CR_SMBEN(value)                 (TWIHS_CR_SMBEN_Msk & ((value) << TWIHS_CR_SMBEN_Pos))
#define TWIHS_CR_SMBDIS_Pos                   (11U)                                          /**< (TWIHS_CR) SMBus Mode Disabled Position */
#define TWIHS_CR_SMBDIS_Msk                   (0x1U << TWIHS_CR_SMBDIS_Pos)                  /**< (TWIHS_CR) SMBus Mode Disabled Mask */
#define TWIHS_CR_SMBDIS(value)                (TWIHS_CR_SMBDIS_Msk & ((value) << TWIHS_CR_SMBDIS_Pos))
#define TWIHS_CR_PECEN_Pos                    (12U)                                          /**< (TWIHS_CR) Packet Error Checking Enable Position */
#define TWIHS_CR_PECEN_Msk                    (0x1U << TWIHS_CR_PECEN_Pos)                   /**< (TWIHS_CR) Packet Error Checking Enable Mask */
#define TWIHS_CR_PECEN(value)                 (TWIHS_CR_PECEN_Msk & ((value) << TWIHS_CR_PECEN_Pos))
#define TWIHS_CR_PECDIS_Pos                   (13U)                                          /**< (TWIHS_CR) Packet Error Checking Disable Position */
#define TWIHS_CR_PECDIS_Msk                   (0x1U << TWIHS_CR_PECDIS_Pos)                  /**< (TWIHS_CR) Packet Error Checking Disable Mask */
#define TWIHS_CR_PECDIS(value)                (TWIHS_CR_PECDIS_Msk & ((value) << TWIHS_CR_PECDIS_Pos))
#define TWIHS_CR_PECRQ_Pos                    (14U)                                          /**< (TWIHS_CR) PEC Request Position */
#define TWIHS_CR_PECRQ_Msk                    (0x1U << TWIHS_CR_PECRQ_Pos)                   /**< (TWIHS_CR) PEC Request Mask */
#define TWIHS_CR_PECRQ(value)                 (TWIHS_CR_PECRQ_Msk & ((value) << TWIHS_CR_PECRQ_Pos))
#define TWIHS_CR_CLEAR_Pos                    (15U)                                          /**< (TWIHS_CR) Bus CLEAR Command Position */
#define TWIHS_CR_CLEAR_Msk                    (0x1U << TWIHS_CR_CLEAR_Pos)                   /**< (TWIHS_CR) Bus CLEAR Command Mask */
#define TWIHS_CR_CLEAR(value)                 (TWIHS_CR_CLEAR_Msk & ((value) << TWIHS_CR_CLEAR_Pos))
#define TWIHS_CR_ACMEN_Pos                    (16U)                                          /**< (TWIHS_CR) Alternative Command Mode Enable Position */
#define TWIHS_CR_ACMEN_Msk                    (0x1U << TWIHS_CR_ACMEN_Pos)                   /**< (TWIHS_CR) Alternative Command Mode Enable Mask */
#define TWIHS_CR_ACMEN(value)                 (TWIHS_CR_ACMEN_Msk & ((value) << TWIHS_CR_ACMEN_Pos))
#define TWIHS_CR_ACMDIS_Pos                   (17U)                                          /**< (TWIHS_CR) Alternative Command Mode Disable Position */
#define TWIHS_CR_ACMDIS_Msk                   (0x1U << TWIHS_CR_ACMDIS_Pos)                  /**< (TWIHS_CR) Alternative Command Mode Disable Mask */
#define TWIHS_CR_ACMDIS(value)                (TWIHS_CR_ACMDIS_Msk & ((value) << TWIHS_CR_ACMDIS_Pos))
#define TWIHS_CR_THRCLR_Pos                   (24U)                                          /**< (TWIHS_CR) Transmit Holding Register Clear Position */
#define TWIHS_CR_THRCLR_Msk                   (0x1U << TWIHS_CR_THRCLR_Pos)                  /**< (TWIHS_CR) Transmit Holding Register Clear Mask */
#define TWIHS_CR_THRCLR(value)                (TWIHS_CR_THRCLR_Msk & ((value) << TWIHS_CR_THRCLR_Pos))
#define TWIHS_CR_LOCKCLR_Pos                  (26U)                                          /**< (TWIHS_CR) Lock Clear Position */
#define TWIHS_CR_LOCKCLR_Msk                  (0x1U << TWIHS_CR_LOCKCLR_Pos)                 /**< (TWIHS_CR) Lock Clear Mask */
#define TWIHS_CR_LOCKCLR(value)               (TWIHS_CR_LOCKCLR_Msk & ((value) << TWIHS_CR_LOCKCLR_Pos))
#define TWIHS_CR_FIFOEN_Pos                   (28U)                                          /**< (TWIHS_CR) FIFO Enable Position */
#define TWIHS_CR_FIFOEN_Msk                   (0x1U << TWIHS_CR_FIFOEN_Pos)                  /**< (TWIHS_CR) FIFO Enable Mask */
#define TWIHS_CR_FIFOEN(value)                (TWIHS_CR_FIFOEN_Msk & ((value) << TWIHS_CR_FIFOEN_Pos))
#define TWIHS_CR_FIFODIS_Pos                  (29U)                                          /**< (TWIHS_CR) FIFO Disable Position */
#define TWIHS_CR_FIFODIS_Msk                  (0x1U << TWIHS_CR_FIFODIS_Pos)                 /**< (TWIHS_CR) FIFO Disable Mask */
#define TWIHS_CR_FIFODIS(value)               (TWIHS_CR_FIFODIS_Msk & ((value) << TWIHS_CR_FIFODIS_Pos))
#define TWIHS_CR_Msk                          (0x3503FFFFUL)                                 /**< (TWIHS_CR) Register Mask  */

/* -------- TWIHS_MMR : (TWIHS Offset: 0x04) (R/W 32) Master Mode Register -------- */
#define TWIHS_MMR_IADRSZ_Pos                  (8U)                                           /**< (TWIHS_MMR) Internal Device Address Size Position */
#define TWIHS_MMR_IADRSZ_Msk                  (0x3U << TWIHS_MMR_IADRSZ_Pos)                 /**< (TWIHS_MMR) Internal Device Address Size Mask */
#define TWIHS_MMR_IADRSZ(value)               (TWIHS_MMR_IADRSZ_Msk & ((value) << TWIHS_MMR_IADRSZ_Pos))
#define   TWIHS_MMR_IADRSZ_NONE_Val           (0U)                                           /**< (TWIHS_MMR) No internal device address  */
#define   TWIHS_MMR_IADRSZ_1_BYTE_Val         (1U)                                           /**< (TWIHS_MMR) One-byte internal device address  */
#define   TWIHS_MMR_IADRSZ_2_BYTE_Val         (2U)                                           /**< (TWIHS_MMR) Two-byte internal device address  */
#define   TWIHS_MMR_IADRSZ_3_BYTE_Val         (3U)                                           /**< (TWIHS_MMR) Three-byte internal device address  */
#define TWIHS_MMR_IADRSZ_NONE                 (TWIHS_MMR_IADRSZ_NONE_Val << TWIHS_MMR_IADRSZ_Pos) /**< (TWIHS_MMR) No internal device address Position  */
#define TWIHS_MMR_IADRSZ_1_BYTE               (TWIHS_MMR_IADRSZ_1_BYTE_Val << TWIHS_MMR_IADRSZ_Pos) /**< (TWIHS_MMR) One-byte internal device address Position  */
#define TWIHS_MMR_IADRSZ_2_BYTE               (TWIHS_MMR_IADRSZ_2_BYTE_Val << TWIHS_MMR_IADRSZ_Pos) /**< (TWIHS_MMR) Two-byte internal device address Position  */
#define TWIHS_MMR_IADRSZ_3_BYTE               (TWIHS_MMR_IADRSZ_3_BYTE_Val << TWIHS_MMR_IADRSZ_Pos) /**< (TWIHS_MMR) Three-byte internal device address Position  */
#define TWIHS_MMR_MREAD_Pos                   (12U)                                          /**< (TWIHS_MMR) Master Read Direction Position */
#define TWIHS_MMR_MREAD_Msk                   (0x1U << TWIHS_MMR_MREAD_Pos)                  /**< (TWIHS_MMR) Master Read Direction Mask */
#define TWIHS_MMR_MREAD(value)                (TWIHS_MMR_MREAD_Msk & ((value) << TWIHS_MMR_MREAD_Pos))
#define TWIHS_MMR_DADR_Pos                    (16U)                                          /**< (TWIHS_MMR) Device Address Position */
#define TWIHS_MMR_DADR_Msk                    (0x7FU << TWIHS_MMR_DADR_Pos)                  /**< (TWIHS_MMR) Device Address Mask */
#define TWIHS_MMR_DADR(value)                 (TWIHS_MMR_DADR_Msk & ((value) << TWIHS_MMR_DADR_Pos))
#define TWIHS_MMR_Msk                         (0x007F1300UL)                                 /**< (TWIHS_MMR) Register Mask  */

/* -------- TWIHS_SMR : (TWIHS Offset: 0x08) (R/W 32) Slave Mode Register -------- */
#define TWIHS_SMR_NACKEN_Pos                  (0U)                                           /**< (TWIHS_SMR) Slave Receiver Data Phase NACK enable Position */
#define TWIHS_SMR_NACKEN_Msk                  (0x1U << TWIHS_SMR_NACKEN_Pos)                 /**< (TWIHS_SMR) Slave Receiver Data Phase NACK enable Mask */
#define TWIHS_SMR_NACKEN(value)               (TWIHS_SMR_NACKEN_Msk & ((value) << TWIHS_SMR_NACKEN_Pos))
#define TWIHS_SMR_SMDA_Pos                    (2U)                                           /**< (TWIHS_SMR) SMBus Default Address Position */
#define TWIHS_SMR_SMDA_Msk                    (0x1U << TWIHS_SMR_SMDA_Pos)                   /**< (TWIHS_SMR) SMBus Default Address Mask */
#define TWIHS_SMR_SMDA(value)                 (TWIHS_SMR_SMDA_Msk & ((value) << TWIHS_SMR_SMDA_Pos))
#define TWIHS_SMR_SMHH_Pos                    (3U)                                           /**< (TWIHS_SMR) SMBus Host Header Position */
#define TWIHS_SMR_SMHH_Msk                    (0x1U << TWIHS_SMR_SMHH_Pos)                   /**< (TWIHS_SMR) SMBus Host Header Mask */
#define TWIHS_SMR_SMHH(value)                 (TWIHS_SMR_SMHH_Msk & ((value) << TWIHS_SMR_SMHH_Pos))
#define TWIHS_SMR_SCLWSDIS_Pos                (6U)                                           /**< (TWIHS_SMR) Clock Wait State Disable Position */
#define TWIHS_SMR_SCLWSDIS_Msk                (0x1U << TWIHS_SMR_SCLWSDIS_Pos)               /**< (TWIHS_SMR) Clock Wait State Disable Mask */
#define TWIHS_SMR_SCLWSDIS(value)             (TWIHS_SMR_SCLWSDIS_Msk & ((value) << TWIHS_SMR_SCLWSDIS_Pos))
#define TWIHS_SMR_MASK_Pos                    (8U)                                           /**< (TWIHS_SMR) Slave Address Mask Position */
#define TWIHS_SMR_MASK_Msk                    (0x7FU << TWIHS_SMR_MASK_Pos)                  /**< (TWIHS_SMR) Slave Address Mask Mask */
#define TWIHS_SMR_MASK(value)                 (TWIHS_SMR_MASK_Msk & ((value) << TWIHS_SMR_MASK_Pos))
#define TWIHS_SMR_SADR_Pos                    (16U)                                          /**< (TWIHS_SMR) Slave Address Position */
#define TWIHS_SMR_SADR_Msk                    (0x7FU << TWIHS_SMR_SADR_Pos)                  /**< (TWIHS_SMR) Slave Address Mask */
#define TWIHS_SMR_SADR(value)                 (TWIHS_SMR_SADR_Msk & ((value) << TWIHS_SMR_SADR_Pos))
#define TWIHS_SMR_SADR1EN_Pos                 (28U)                                          /**< (TWIHS_SMR) Slave Address 1 Enable Position */
#define TWIHS_SMR_SADR1EN_Msk                 (0x1U << TWIHS_SMR_SADR1EN_Pos)                /**< (TWIHS_SMR) Slave Address 1 Enable Mask */
#define TWIHS_SMR_SADR1EN(value)              (TWIHS_SMR_SADR1EN_Msk & ((value) << TWIHS_SMR_SADR1EN_Pos))
#define TWIHS_SMR_SADR2EN_Pos                 (29U)                                          /**< (TWIHS_SMR) Slave Address 2 Enable Position */
#define TWIHS_SMR_SADR2EN_Msk                 (0x1U << TWIHS_SMR_SADR2EN_Pos)                /**< (TWIHS_SMR) Slave Address 2 Enable Mask */
#define TWIHS_SMR_SADR2EN(value)              (TWIHS_SMR_SADR2EN_Msk & ((value) << TWIHS_SMR_SADR2EN_Pos))
#define TWIHS_SMR_SADR3EN_Pos                 (30U)                                          /**< (TWIHS_SMR) Slave Address 3 Enable Position */
#define TWIHS_SMR_SADR3EN_Msk                 (0x1U << TWIHS_SMR_SADR3EN_Pos)                /**< (TWIHS_SMR) Slave Address 3 Enable Mask */
#define TWIHS_SMR_SADR3EN(value)              (TWIHS_SMR_SADR3EN_Msk & ((value) << TWIHS_SMR_SADR3EN_Pos))
#define TWIHS_SMR_DATAMEN_Pos                 (31U)                                          /**< (TWIHS_SMR) Data Matching Enable Position */
#define TWIHS_SMR_DATAMEN_Msk                 (0x1U << TWIHS_SMR_DATAMEN_Pos)                /**< (TWIHS_SMR) Data Matching Enable Mask */
#define TWIHS_SMR_DATAMEN(value)              (TWIHS_SMR_DATAMEN_Msk & ((value) << TWIHS_SMR_DATAMEN_Pos))
#define TWIHS_SMR_Msk                         (0xF07F7F4DUL)                                 /**< (TWIHS_SMR) Register Mask  */

/* -------- TWIHS_IADR : (TWIHS Offset: 0x0C) (R/W 32) Internal Address Register -------- */
#define TWIHS_IADR_IADR_Pos                   (0U)                                           /**< (TWIHS_IADR) Internal Address Position */
#define TWIHS_IADR_IADR_Msk                   (0xFFFFFFU << TWIHS_IADR_IADR_Pos)             /**< (TWIHS_IADR) Internal Address Mask */
#define TWIHS_IADR_IADR(value)                (TWIHS_IADR_IADR_Msk & ((value) << TWIHS_IADR_IADR_Pos))
#define TWIHS_IADR_Msk                        (0x00FFFFFFUL)                                 /**< (TWIHS_IADR) Register Mask  */

/* -------- TWIHS_CWGR : (TWIHS Offset: 0x10) (R/W 32) Clock Waveform Generator Register -------- */
#define TWIHS_CWGR_CLDIV_Pos                  (0U)                                           /**< (TWIHS_CWGR) Clock Low Divider Position */
#define TWIHS_CWGR_CLDIV_Msk                  (0xFFU << TWIHS_CWGR_CLDIV_Pos)                /**< (TWIHS_CWGR) Clock Low Divider Mask */
#define TWIHS_CWGR_CLDIV(value)               (TWIHS_CWGR_CLDIV_Msk & ((value) << TWIHS_CWGR_CLDIV_Pos))
#define TWIHS_CWGR_CHDIV_Pos                  (8U)                                           /**< (TWIHS_CWGR) Clock High Divider Position */
#define TWIHS_CWGR_CHDIV_Msk                  (0xFFU << TWIHS_CWGR_CHDIV_Pos)                /**< (TWIHS_CWGR) Clock High Divider Mask */
#define TWIHS_CWGR_CHDIV(value)               (TWIHS_CWGR_CHDIV_Msk & ((value) << TWIHS_CWGR_CHDIV_Pos))
#define TWIHS_CWGR_CKDIV_Pos                  (16U)                                          /**< (TWIHS_CWGR) Clock Divider Position */
#define TWIHS_CWGR_CKDIV_Msk                  (0x7U << TWIHS_CWGR_CKDIV_Pos)                 /**< (TWIHS_CWGR) Clock Divider Mask */
#define TWIHS_CWGR_CKDIV(value)               (TWIHS_CWGR_CKDIV_Msk & ((value) << TWIHS_CWGR_CKDIV_Pos))
#define TWIHS_CWGR_HOLD_Pos                   (24U)                                          /**< (TWIHS_CWGR) TWD Hold Time Versus TWCK Falling Position */
#define TWIHS_CWGR_HOLD_Msk                   (0x3FU << TWIHS_CWGR_HOLD_Pos)                 /**< (TWIHS_CWGR) TWD Hold Time Versus TWCK Falling Mask */
#define TWIHS_CWGR_HOLD(value)                (TWIHS_CWGR_HOLD_Msk & ((value) << TWIHS_CWGR_HOLD_Pos))
#define TWIHS_CWGR_Msk                        (0x3F07FFFFUL)                                 /**< (TWIHS_CWGR) Register Mask  */

/* -------- TWIHS_SR : (TWIHS Offset: 0x20) (R/  32) Status Register -------- */
#define TWIHS_SR_TXCOMP_Pos                   (0U)                                           /**< (TWIHS_SR) Transmission Completed (cleared by writing TWIHS_THR) Position */
#define TWIHS_SR_TXCOMP_Msk                   (0x1U << TWIHS_SR_TXCOMP_Pos)                  /**< (TWIHS_SR) Transmission Completed (cleared by writing TWIHS_THR) Mask */
#define TWIHS_SR_TXCOMP(value)                (TWIHS_SR_TXCOMP_Msk & ((value) << TWIHS_SR_TXCOMP_Pos))
#define TWIHS_SR_RXRDY_Pos                    (1U)                                           /**< (TWIHS_SR) Receive Holding Register Ready (cleared by reading TWIHS_RHR) Position */
#define TWIHS_SR_RXRDY_Msk                    (0x1U << TWIHS_SR_RXRDY_Pos)                   /**< (TWIHS_SR) Receive Holding Register Ready (cleared by reading TWIHS_RHR) Mask */
#define TWIHS_SR_RXRDY(value)                 (TWIHS_SR_RXRDY_Msk & ((value) << TWIHS_SR_RXRDY_Pos))
#define TWIHS_SR_TXRDY_Pos                    (2U)                                           /**< (TWIHS_SR) Transmit Holding Register Ready (cleared by writing TWIHS_THR) Position */
#define TWIHS_SR_TXRDY_Msk                    (0x1U << TWIHS_SR_TXRDY_Pos)                   /**< (TWIHS_SR) Transmit Holding Register Ready (cleared by writing TWIHS_THR) Mask */
#define TWIHS_SR_TXRDY(value)                 (TWIHS_SR_TXRDY_Msk & ((value) << TWIHS_SR_TXRDY_Pos))
#define TWIHS_SR_SVREAD_Pos                   (3U)                                           /**< (TWIHS_SR) Slave Read Position */
#define TWIHS_SR_SVREAD_Msk                   (0x1U << TWIHS_SR_SVREAD_Pos)                  /**< (TWIHS_SR) Slave Read Mask */
#define TWIHS_SR_SVREAD(value)                (TWIHS_SR_SVREAD_Msk & ((value) << TWIHS_SR_SVREAD_Pos))
#define TWIHS_SR_SVACC_Pos                    (4U)                                           /**< (TWIHS_SR) Slave Access Position */
#define TWIHS_SR_SVACC_Msk                    (0x1U << TWIHS_SR_SVACC_Pos)                   /**< (TWIHS_SR) Slave Access Mask */
#define TWIHS_SR_SVACC(value)                 (TWIHS_SR_SVACC_Msk & ((value) << TWIHS_SR_SVACC_Pos))
#define TWIHS_SR_GACC_Pos                     (5U)                                           /**< (TWIHS_SR) General Call Access (cleared on read) Position */
#define TWIHS_SR_GACC_Msk                     (0x1U << TWIHS_SR_GACC_Pos)                    /**< (TWIHS_SR) General Call Access (cleared on read) Mask */
#define TWIHS_SR_GACC(value)                  (TWIHS_SR_GACC_Msk & ((value) << TWIHS_SR_GACC_Pos))
#define TWIHS_SR_OVRE_Pos                     (6U)                                           /**< (TWIHS_SR) Overrun Error (cleared on read) Position */
#define TWIHS_SR_OVRE_Msk                     (0x1U << TWIHS_SR_OVRE_Pos)                    /**< (TWIHS_SR) Overrun Error (cleared on read) Mask */
#define TWIHS_SR_OVRE(value)                  (TWIHS_SR_OVRE_Msk & ((value) << TWIHS_SR_OVRE_Pos))
#define TWIHS_SR_UNRE_Pos                     (7U)                                           /**< (TWIHS_SR) Underrun Error (cleared on read) Position */
#define TWIHS_SR_UNRE_Msk                     (0x1U << TWIHS_SR_UNRE_Pos)                    /**< (TWIHS_SR) Underrun Error (cleared on read) Mask */
#define TWIHS_SR_UNRE(value)                  (TWIHS_SR_UNRE_Msk & ((value) << TWIHS_SR_UNRE_Pos))
#define TWIHS_SR_NACK_Pos                     (8U)                                           /**< (TWIHS_SR) Not Acknowledged (cleared on read) Position */
#define TWIHS_SR_NACK_Msk                     (0x1U << TWIHS_SR_NACK_Pos)                    /**< (TWIHS_SR) Not Acknowledged (cleared on read) Mask */
#define TWIHS_SR_NACK(value)                  (TWIHS_SR_NACK_Msk & ((value) << TWIHS_SR_NACK_Pos))
#define TWIHS_SR_ARBLST_Pos                   (9U)                                           /**< (TWIHS_SR) Arbitration Lost (cleared on read) Position */
#define TWIHS_SR_ARBLST_Msk                   (0x1U << TWIHS_SR_ARBLST_Pos)                  /**< (TWIHS_SR) Arbitration Lost (cleared on read) Mask */
#define TWIHS_SR_ARBLST(value)                (TWIHS_SR_ARBLST_Msk & ((value) << TWIHS_SR_ARBLST_Pos))
#define TWIHS_SR_SCLWS_Pos                    (10U)                                          /**< (TWIHS_SR) Clock Wait State Position */
#define TWIHS_SR_SCLWS_Msk                    (0x1U << TWIHS_SR_SCLWS_Pos)                   /**< (TWIHS_SR) Clock Wait State Mask */
#define TWIHS_SR_SCLWS(value)                 (TWIHS_SR_SCLWS_Msk & ((value) << TWIHS_SR_SCLWS_Pos))
#define TWIHS_SR_EOSACC_Pos                   (11U)                                          /**< (TWIHS_SR) End Of Slave Access (cleared on read) Position */
#define TWIHS_SR_EOSACC_Msk                   (0x1U << TWIHS_SR_EOSACC_Pos)                  /**< (TWIHS_SR) End Of Slave Access (cleared on read) Mask */
#define TWIHS_SR_EOSACC(value)                (TWIHS_SR_EOSACC_Msk & ((value) << TWIHS_SR_EOSACC_Pos))
#define TWIHS_SR_MCACK_Pos                    (16U)                                          /**< (TWIHS_SR) Master Code Acknowledge (cleared on read) Position */
#define TWIHS_SR_MCACK_Msk                    (0x1U << TWIHS_SR_MCACK_Pos)                   /**< (TWIHS_SR) Master Code Acknowledge (cleared on read) Mask */
#define TWIHS_SR_MCACK(value)                 (TWIHS_SR_MCACK_Msk & ((value) << TWIHS_SR_MCACK_Pos))
#define TWIHS_SR_TOUT_Pos                     (18U)                                          /**< (TWIHS_SR) Timeout Error (cleared on read) Position */
#define TWIHS_SR_TOUT_Msk                     (0x1U << TWIHS_SR_TOUT_Pos)                    /**< (TWIHS_SR) Timeout Error (cleared on read) Mask */
#define TWIHS_SR_TOUT(value)                  (TWIHS_SR_TOUT_Msk & ((value) << TWIHS_SR_TOUT_Pos))
#define TWIHS_SR_PECERR_Pos                   (19U)                                          /**< (TWIHS_SR) PEC Error (cleared on read) Position */
#define TWIHS_SR_PECERR_Msk                   (0x1U << TWIHS_SR_PECERR_Pos)                  /**< (TWIHS_SR) PEC Error (cleared on read) Mask */
#define TWIHS_SR_PECERR(value)                (TWIHS_SR_PECERR_Msk & ((value) << TWIHS_SR_PECERR_Pos))
#define TWIHS_SR_SMBDAM_Pos                   (20U)                                          /**< (TWIHS_SR) SMBus Default Address Match (cleared on read) Position */
#define TWIHS_SR_SMBDAM_Msk                   (0x1U << TWIHS_SR_SMBDAM_Pos)                  /**< (TWIHS_SR) SMBus Default Address Match (cleared on read) Mask */
#define TWIHS_SR_SMBDAM(value)                (TWIHS_SR_SMBDAM_Msk & ((value) << TWIHS_SR_SMBDAM_Pos))
#define TWIHS_SR_SMBHHM_Pos                   (21U)                                          /**< (TWIHS_SR) SMBus Host Header Address Match (cleared on read) Position */
#define TWIHS_SR_SMBHHM_Msk                   (0x1U << TWIHS_SR_SMBHHM_Pos)                  /**< (TWIHS_SR) SMBus Host Header Address Match (cleared on read) Mask */
#define TWIHS_SR_SMBHHM(value)                (TWIHS_SR_SMBHHM_Msk & ((value) << TWIHS_SR_SMBHHM_Pos))
#define TWIHS_SR_SCL_Pos                      (24U)                                          /**< (TWIHS_SR) SCL Line Value Position */
#define TWIHS_SR_SCL_Msk                      (0x1U << TWIHS_SR_SCL_Pos)                     /**< (TWIHS_SR) SCL Line Value Mask */
#define TWIHS_SR_SCL(value)                   (TWIHS_SR_SCL_Msk & ((value) << TWIHS_SR_SCL_Pos))
#define TWIHS_SR_SDA_Pos                      (25U)                                          /**< (TWIHS_SR) SDA Line Value Position */
#define TWIHS_SR_SDA_Msk                      (0x1U << TWIHS_SR_SDA_Pos)                     /**< (TWIHS_SR) SDA Line Value Mask */
#define TWIHS_SR_SDA(value)                   (TWIHS_SR_SDA_Msk & ((value) << TWIHS_SR_SDA_Pos))
#define TWIHS_SR_Msk                          (0x033D0FFFUL)                                 /**< (TWIHS_SR) Register Mask  */

/* -------- TWIHS_IER : (TWIHS Offset: 0x24) ( /W 32) Interrupt Enable Register -------- */
#define TWIHS_IER_TXCOMP_Pos                  (0U)                                           /**< (TWIHS_IER) Transmission Completed Interrupt Enable Position */
#define TWIHS_IER_TXCOMP_Msk                  (0x1U << TWIHS_IER_TXCOMP_Pos)                 /**< (TWIHS_IER) Transmission Completed Interrupt Enable Mask */
#define TWIHS_IER_TXCOMP(value)               (TWIHS_IER_TXCOMP_Msk & ((value) << TWIHS_IER_TXCOMP_Pos))
#define TWIHS_IER_RXRDY_Pos                   (1U)                                           /**< (TWIHS_IER) Receive Holding Register Ready Interrupt Enable Position */
#define TWIHS_IER_RXRDY_Msk                   (0x1U << TWIHS_IER_RXRDY_Pos)                  /**< (TWIHS_IER) Receive Holding Register Ready Interrupt Enable Mask */
#define TWIHS_IER_RXRDY(value)                (TWIHS_IER_RXRDY_Msk & ((value) << TWIHS_IER_RXRDY_Pos))
#define TWIHS_IER_TXRDY_Pos                   (2U)                                           /**< (TWIHS_IER) Transmit Holding Register Ready Interrupt Enable Position */
#define TWIHS_IER_TXRDY_Msk                   (0x1U << TWIHS_IER_TXRDY_Pos)                  /**< (TWIHS_IER) Transmit Holding Register Ready Interrupt Enable Mask */
#define TWIHS_IER_TXRDY(value)                (TWIHS_IER_TXRDY_Msk & ((value) << TWIHS_IER_TXRDY_Pos))
#define TWIHS_IER_SVACC_Pos                   (4U)                                           /**< (TWIHS_IER) Slave Access Interrupt Enable Position */
#define TWIHS_IER_SVACC_Msk                   (0x1U << TWIHS_IER_SVACC_Pos)                  /**< (TWIHS_IER) Slave Access Interrupt Enable Mask */
#define TWIHS_IER_SVACC(value)                (TWIHS_IER_SVACC_Msk & ((value) << TWIHS_IER_SVACC_Pos))
#define TWIHS_IER_GACC_Pos                    (5U)                                           /**< (TWIHS_IER) General Call Access Interrupt Enable Position */
#define TWIHS_IER_GACC_Msk                    (0x1U << TWIHS_IER_GACC_Pos)                   /**< (TWIHS_IER) General Call Access Interrupt Enable Mask */
#define TWIHS_IER_GACC(value)                 (TWIHS_IER_GACC_Msk & ((value) << TWIHS_IER_GACC_Pos))
#define TWIHS_IER_OVRE_Pos                    (6U)                                           /**< (TWIHS_IER) Overrun Error Interrupt Enable Position */
#define TWIHS_IER_OVRE_Msk                    (0x1U << TWIHS_IER_OVRE_Pos)                   /**< (TWIHS_IER) Overrun Error Interrupt Enable Mask */
#define TWIHS_IER_OVRE(value)                 (TWIHS_IER_OVRE_Msk & ((value) << TWIHS_IER_OVRE_Pos))
#define TWIHS_IER_UNRE_Pos                    (7U)                                           /**< (TWIHS_IER) Underrun Error Interrupt Enable Position */
#define TWIHS_IER_UNRE_Msk                    (0x1U << TWIHS_IER_UNRE_Pos)                   /**< (TWIHS_IER) Underrun Error Interrupt Enable Mask */
#define TWIHS_IER_UNRE(value)                 (TWIHS_IER_UNRE_Msk & ((value) << TWIHS_IER_UNRE_Pos))
#define TWIHS_IER_NACK_Pos                    (8U)                                           /**< (TWIHS_IER) Not Acknowledge Interrupt Enable Position */
#define TWIHS_IER_NACK_Msk                    (0x1U << TWIHS_IER_NACK_Pos)                   /**< (TWIHS_IER) Not Acknowledge Interrupt Enable Mask */
#define TWIHS_IER_NACK(value)                 (TWIHS_IER_NACK_Msk & ((value) << TWIHS_IER_NACK_Pos))
#define TWIHS_IER_ARBLST_Pos                  (9U)                                           /**< (TWIHS_IER) Arbitration Lost Interrupt Enable Position */
#define TWIHS_IER_ARBLST_Msk                  (0x1U << TWIHS_IER_ARBLST_Pos)                 /**< (TWIHS_IER) Arbitration Lost Interrupt Enable Mask */
#define TWIHS_IER_ARBLST(value)               (TWIHS_IER_ARBLST_Msk & ((value) << TWIHS_IER_ARBLST_Pos))
#define TWIHS_IER_SCL_WS_Pos                  (10U)                                          /**< (TWIHS_IER) Clock Wait State Interrupt Enable Position */
#define TWIHS_IER_SCL_WS_Msk                  (0x1U << TWIHS_IER_SCL_WS_Pos)                 /**< (TWIHS_IER) Clock Wait State Interrupt Enable Mask */
#define TWIHS_IER_SCL_WS(value)               (TWIHS_IER_SCL_WS_Msk & ((value) << TWIHS_IER_SCL_WS_Pos))
#define TWIHS_IER_EOSACC_Pos                  (11U)                                          /**< (TWIHS_IER) End Of Slave Access Interrupt Enable Position */
#define TWIHS_IER_EOSACC_Msk                  (0x1U << TWIHS_IER_EOSACC_Pos)                 /**< (TWIHS_IER) End Of Slave Access Interrupt Enable Mask */
#define TWIHS_IER_EOSACC(value)               (TWIHS_IER_EOSACC_Msk & ((value) << TWIHS_IER_EOSACC_Pos))
#define TWIHS_IER_MCACK_Pos                   (16U)                                          /**< (TWIHS_IER) Master Code Acknowledge Interrupt Enable Position */
#define TWIHS_IER_MCACK_Msk                   (0x1U << TWIHS_IER_MCACK_Pos)                  /**< (TWIHS_IER) Master Code Acknowledge Interrupt Enable Mask */
#define TWIHS_IER_MCACK(value)                (TWIHS_IER_MCACK_Msk & ((value) << TWIHS_IER_MCACK_Pos))
#define TWIHS_IER_TOUT_Pos                    (18U)                                          /**< (TWIHS_IER) Timeout Error Interrupt Enable Position */
#define TWIHS_IER_TOUT_Msk                    (0x1U << TWIHS_IER_TOUT_Pos)                   /**< (TWIHS_IER) Timeout Error Interrupt Enable Mask */
#define TWIHS_IER_TOUT(value)                 (TWIHS_IER_TOUT_Msk & ((value) << TWIHS_IER_TOUT_Pos))
#define TWIHS_IER_PECERR_Pos                  (19U)                                          /**< (TWIHS_IER) PEC Error Interrupt Enable Position */
#define TWIHS_IER_PECERR_Msk                  (0x1U << TWIHS_IER_PECERR_Pos)                 /**< (TWIHS_IER) PEC Error Interrupt Enable Mask */
#define TWIHS_IER_PECERR(value)               (TWIHS_IER_PECERR_Msk & ((value) << TWIHS_IER_PECERR_Pos))
#define TWIHS_IER_SMBDAM_Pos                  (20U)                                          /**< (TWIHS_IER) SMBus Default Address Match Interrupt Enable Position */
#define TWIHS_IER_SMBDAM_Msk                  (0x1U << TWIHS_IER_SMBDAM_Pos)                 /**< (TWIHS_IER) SMBus Default Address Match Interrupt Enable Mask */
#define TWIHS_IER_SMBDAM(value)               (TWIHS_IER_SMBDAM_Msk & ((value) << TWIHS_IER_SMBDAM_Pos))
#define TWIHS_IER_SMBHHM_Pos                  (21U)                                          /**< (TWIHS_IER) SMBus Host Header Address Match Interrupt Enable Position */
#define TWIHS_IER_SMBHHM_Msk                  (0x1U << TWIHS_IER_SMBHHM_Pos)                 /**< (TWIHS_IER) SMBus Host Header Address Match Interrupt Enable Mask */
#define TWIHS_IER_SMBHHM(value)               (TWIHS_IER_SMBHHM_Msk & ((value) << TWIHS_IER_SMBHHM_Pos))
#define TWIHS_IER_Msk                         (0x003D0FF7UL)                                 /**< (TWIHS_IER) Register Mask  */

/* -------- TWIHS_IDR : (TWIHS Offset: 0x28) ( /W 32) Interrupt Disable Register -------- */
#define TWIHS_IDR_TXCOMP_Pos                  (0U)                                           /**< (TWIHS_IDR) Transmission Completed Interrupt Disable Position */
#define TWIHS_IDR_TXCOMP_Msk                  (0x1U << TWIHS_IDR_TXCOMP_Pos)                 /**< (TWIHS_IDR) Transmission Completed Interrupt Disable Mask */
#define TWIHS_IDR_TXCOMP(value)               (TWIHS_IDR_TXCOMP_Msk & ((value) << TWIHS_IDR_TXCOMP_Pos))
#define TWIHS_IDR_RXRDY_Pos                   (1U)                                           /**< (TWIHS_IDR) Receive Holding Register Ready Interrupt Disable Position */
#define TWIHS_IDR_RXRDY_Msk                   (0x1U << TWIHS_IDR_RXRDY_Pos)                  /**< (TWIHS_IDR) Receive Holding Register Ready Interrupt Disable Mask */
#define TWIHS_IDR_RXRDY(value)                (TWIHS_IDR_RXRDY_Msk & ((value) << TWIHS_IDR_RXRDY_Pos))
#define TWIHS_IDR_TXRDY_Pos                   (2U)                                           /**< (TWIHS_IDR) Transmit Holding Register Ready Interrupt Disable Position */
#define TWIHS_IDR_TXRDY_Msk                   (0x1U << TWIHS_IDR_TXRDY_Pos)                  /**< (TWIHS_IDR) Transmit Holding Register Ready Interrupt Disable Mask */
#define TWIHS_IDR_TXRDY(value)                (TWIHS_IDR_TXRDY_Msk & ((value) << TWIHS_IDR_TXRDY_Pos))
#define TWIHS_IDR_SVACC_Pos                   (4U)                                           /**< (TWIHS_IDR) Slave Access Interrupt Disable Position */
#define TWIHS_IDR_SVACC_Msk                   (0x1U << TWIHS_IDR_SVACC_Pos)                  /**< (TWIHS_IDR) Slave Access Interrupt Disable Mask */
#define TWIHS_IDR_SVACC(value)                (TWIHS_IDR_SVACC_Msk & ((value) << TWIHS_IDR_SVACC_Pos))
#define TWIHS_IDR_GACC_Pos                    (5U)                                           /**< (TWIHS_IDR) General Call Access Interrupt Disable Position */
#define TWIHS_IDR_GACC_Msk                    (0x1U << TWIHS_IDR_GACC_Pos)                   /**< (TWIHS_IDR) General Call Access Interrupt Disable Mask */
#define TWIHS_IDR_GACC(value)                 (TWIHS_IDR_GACC_Msk & ((value) << TWIHS_IDR_GACC_Pos))
#define TWIHS_IDR_OVRE_Pos                    (6U)                                           /**< (TWIHS_IDR) Overrun Error Interrupt Disable Position */
#define TWIHS_IDR_OVRE_Msk                    (0x1U << TWIHS_IDR_OVRE_Pos)                   /**< (TWIHS_IDR) Overrun Error Interrupt Disable Mask */
#define TWIHS_IDR_OVRE(value)                 (TWIHS_IDR_OVRE_Msk & ((value) << TWIHS_IDR_OVRE_Pos))
#define TWIHS_IDR_UNRE_Pos                    (7U)                                           /**< (TWIHS_IDR) Underrun Error Interrupt Disable Position */
#define TWIHS_IDR_UNRE_Msk                    (0x1U << TWIHS_IDR_UNRE_Pos)                   /**< (TWIHS_IDR) Underrun Error Interrupt Disable Mask */
#define TWIHS_IDR_UNRE(value)                 (TWIHS_IDR_UNRE_Msk & ((value) << TWIHS_IDR_UNRE_Pos))
#define TWIHS_IDR_NACK_Pos                    (8U)                                           /**< (TWIHS_IDR) Not Acknowledge Interrupt Disable Position */
#define TWIHS_IDR_NACK_Msk                    (0x1U << TWIHS_IDR_NACK_Pos)                   /**< (TWIHS_IDR) Not Acknowledge Interrupt Disable Mask */
#define TWIHS_IDR_NACK(value)                 (TWIHS_IDR_NACK_Msk & ((value) << TWIHS_IDR_NACK_Pos))
#define TWIHS_IDR_ARBLST_Pos                  (9U)                                           /**< (TWIHS_IDR) Arbitration Lost Interrupt Disable Position */
#define TWIHS_IDR_ARBLST_Msk                  (0x1U << TWIHS_IDR_ARBLST_Pos)                 /**< (TWIHS_IDR) Arbitration Lost Interrupt Disable Mask */
#define TWIHS_IDR_ARBLST(value)               (TWIHS_IDR_ARBLST_Msk & ((value) << TWIHS_IDR_ARBLST_Pos))
#define TWIHS_IDR_SCL_WS_Pos                  (10U)                                          /**< (TWIHS_IDR) Clock Wait State Interrupt Disable Position */
#define TWIHS_IDR_SCL_WS_Msk                  (0x1U << TWIHS_IDR_SCL_WS_Pos)                 /**< (TWIHS_IDR) Clock Wait State Interrupt Disable Mask */
#define TWIHS_IDR_SCL_WS(value)               (TWIHS_IDR_SCL_WS_Msk & ((value) << TWIHS_IDR_SCL_WS_Pos))
#define TWIHS_IDR_EOSACC_Pos                  (11U)                                          /**< (TWIHS_IDR) End Of Slave Access Interrupt Disable Position */
#define TWIHS_IDR_EOSACC_Msk                  (0x1U << TWIHS_IDR_EOSACC_Pos)                 /**< (TWIHS_IDR) End Of Slave Access Interrupt Disable Mask */
#define TWIHS_IDR_EOSACC(value)               (TWIHS_IDR_EOSACC_Msk & ((value) << TWIHS_IDR_EOSACC_Pos))
#define TWIHS_IDR_MCACK_Pos                   (16U)                                          /**< (TWIHS_IDR) Master Code Acknowledge Interrupt Disable Position */
#define TWIHS_IDR_MCACK_Msk                   (0x1U << TWIHS_IDR_MCACK_Pos)                  /**< (TWIHS_IDR) Master Code Acknowledge Interrupt Disable Mask */
#define TWIHS_IDR_MCACK(value)                (TWIHS_IDR_MCACK_Msk & ((value) << TWIHS_IDR_MCACK_Pos))
#define TWIHS_IDR_TOUT_Pos                    (18U)                                          /**< (TWIHS_IDR) Timeout Error Interrupt Disable Position */
#define TWIHS_IDR_TOUT_Msk                    (0x1U << TWIHS_IDR_TOUT_Pos)                   /**< (TWIHS_IDR) Timeout Error Interrupt Disable Mask */
#define TWIHS_IDR_TOUT(value)                 (TWIHS_IDR_TOUT_Msk & ((value) << TWIHS_IDR_TOUT_Pos))
#define TWIHS_IDR_PECERR_Pos                  (19U)                                          /**< (TWIHS_IDR) PEC Error Interrupt Disable Position */
#define TWIHS_IDR_PECERR_Msk                  (0x1U << TWIHS_IDR_PECERR_Pos)                 /**< (TWIHS_IDR) PEC Error Interrupt Disable Mask */
#define TWIHS_IDR_PECERR(value)               (TWIHS_IDR_PECERR_Msk & ((value) << TWIHS_IDR_PECERR_Pos))
#define TWIHS_IDR_SMBDAM_Pos                  (20U)                                          /**< (TWIHS_IDR) SMBus Default Address Match Interrupt Disable Position */
#define TWIHS_IDR_SMBDAM_Msk                  (0x1U << TWIHS_IDR_SMBDAM_Pos)                 /**< (TWIHS_IDR) SMBus Default Address Match Interrupt Disable Mask */
#define TWIHS_IDR_SMBDAM(value)               (TWIHS_IDR_SMBDAM_Msk & ((value) << TWIHS_IDR_SMBDAM_Pos))
#define TWIHS_IDR_SMBHHM_Pos                  (21U)                                          /**< (TWIHS_IDR) SMBus Host Header Address Match Interrupt Disable Position */
#define TWIHS_IDR_SMBHHM_Msk                  (0x1U << TWIHS_IDR_SMBHHM_Pos)                 /**< (TWIHS_IDR) SMBus Host Header Address Match Interrupt Disable Mask */
#define TWIHS_IDR_SMBHHM(value)               (TWIHS_IDR_SMBHHM_Msk & ((value) << TWIHS_IDR_SMBHHM_Pos))
#define TWIHS_IDR_Msk                         (0x003D0FF7UL)                                 /**< (TWIHS_IDR) Register Mask  */

/* -------- TWIHS_IMR : (TWIHS Offset: 0x2C) (R/  32) Interrupt Mask Register -------- */
#define TWIHS_IMR_TXCOMP_Pos                  (0U)                                           /**< (TWIHS_IMR) Transmission Completed Interrupt Mask Position */
#define TWIHS_IMR_TXCOMP_Msk                  (0x1U << TWIHS_IMR_TXCOMP_Pos)                 /**< (TWIHS_IMR) Transmission Completed Interrupt Mask Mask */
#define TWIHS_IMR_TXCOMP(value)               (TWIHS_IMR_TXCOMP_Msk & ((value) << TWIHS_IMR_TXCOMP_Pos))
#define TWIHS_IMR_RXRDY_Pos                   (1U)                                           /**< (TWIHS_IMR) Receive Holding Register Ready Interrupt Mask Position */
#define TWIHS_IMR_RXRDY_Msk                   (0x1U << TWIHS_IMR_RXRDY_Pos)                  /**< (TWIHS_IMR) Receive Holding Register Ready Interrupt Mask Mask */
#define TWIHS_IMR_RXRDY(value)                (TWIHS_IMR_RXRDY_Msk & ((value) << TWIHS_IMR_RXRDY_Pos))
#define TWIHS_IMR_TXRDY_Pos                   (2U)                                           /**< (TWIHS_IMR) Transmit Holding Register Ready Interrupt Mask Position */
#define TWIHS_IMR_TXRDY_Msk                   (0x1U << TWIHS_IMR_TXRDY_Pos)                  /**< (TWIHS_IMR) Transmit Holding Register Ready Interrupt Mask Mask */
#define TWIHS_IMR_TXRDY(value)                (TWIHS_IMR_TXRDY_Msk & ((value) << TWIHS_IMR_TXRDY_Pos))
#define TWIHS_IMR_SVACC_Pos                   (4U)                                           /**< (TWIHS_IMR) Slave Access Interrupt Mask Position */
#define TWIHS_IMR_SVACC_Msk                   (0x1U << TWIHS_IMR_SVACC_Pos)                  /**< (TWIHS_IMR) Slave Access Interrupt Mask Mask */
#define TWIHS_IMR_SVACC(value)                (TWIHS_IMR_SVACC_Msk & ((value) << TWIHS_IMR_SVACC_Pos))
#define TWIHS_IMR_GACC_Pos                    (5U)                                           /**< (TWIHS_IMR) General Call Access Interrupt Mask Position */
#define TWIHS_IMR_GACC_Msk                    (0x1U << TWIHS_IMR_GACC_Pos)                   /**< (TWIHS_IMR) General Call Access Interrupt Mask Mask */
#define TWIHS_IMR_GACC(value)                 (TWIHS_IMR_GACC_Msk & ((value) << TWIHS_IMR_GACC_Pos))
#define TWIHS_IMR_OVRE_Pos                    (6U)                                           /**< (TWIHS_IMR) Overrun Error Interrupt Mask Position */
#define TWIHS_IMR_OVRE_Msk                    (0x1U << TWIHS_IMR_OVRE_Pos)                   /**< (TWIHS_IMR) Overrun Error Interrupt Mask Mask */
#define TWIHS_IMR_OVRE(value)                 (TWIHS_IMR_OVRE_Msk & ((value) << TWIHS_IMR_OVRE_Pos))
#define TWIHS_IMR_UNRE_Pos                    (7U)                                           /**< (TWIHS_IMR) Underrun Error Interrupt Mask Position */
#define TWIHS_IMR_UNRE_Msk                    (0x1U << TWIHS_IMR_UNRE_Pos)                   /**< (TWIHS_IMR) Underrun Error Interrupt Mask Mask */
#define TWIHS_IMR_UNRE(value)                 (TWIHS_IMR_UNRE_Msk & ((value) << TWIHS_IMR_UNRE_Pos))
#define TWIHS_IMR_NACK_Pos                    (8U)                                           /**< (TWIHS_IMR) Not Acknowledge Interrupt Mask Position */
#define TWIHS_IMR_NACK_Msk                    (0x1U << TWIHS_IMR_NACK_Pos)                   /**< (TWIHS_IMR) Not Acknowledge Interrupt Mask Mask */
#define TWIHS_IMR_NACK(value)                 (TWIHS_IMR_NACK_Msk & ((value) << TWIHS_IMR_NACK_Pos))
#define TWIHS_IMR_ARBLST_Pos                  (9U)                                           /**< (TWIHS_IMR) Arbitration Lost Interrupt Mask Position */
#define TWIHS_IMR_ARBLST_Msk                  (0x1U << TWIHS_IMR_ARBLST_Pos)                 /**< (TWIHS_IMR) Arbitration Lost Interrupt Mask Mask */
#define TWIHS_IMR_ARBLST(value)               (TWIHS_IMR_ARBLST_Msk & ((value) << TWIHS_IMR_ARBLST_Pos))
#define TWIHS_IMR_SCL_WS_Pos                  (10U)                                          /**< (TWIHS_IMR) Clock Wait State Interrupt Mask Position */
#define TWIHS_IMR_SCL_WS_Msk                  (0x1U << TWIHS_IMR_SCL_WS_Pos)                 /**< (TWIHS_IMR) Clock Wait State Interrupt Mask Mask */
#define TWIHS_IMR_SCL_WS(value)               (TWIHS_IMR_SCL_WS_Msk & ((value) << TWIHS_IMR_SCL_WS_Pos))
#define TWIHS_IMR_EOSACC_Pos                  (11U)                                          /**< (TWIHS_IMR) End Of Slave Access Interrupt Mask Position */
#define TWIHS_IMR_EOSACC_Msk                  (0x1U << TWIHS_IMR_EOSACC_Pos)                 /**< (TWIHS_IMR) End Of Slave Access Interrupt Mask Mask */
#define TWIHS_IMR_EOSACC(value)               (TWIHS_IMR_EOSACC_Msk & ((value) << TWIHS_IMR_EOSACC_Pos))
#define TWIHS_IMR_MCACK_Pos                   (16U)                                          /**< (TWIHS_IMR) Master Code Acknowledge Interrupt Mask Position */
#define TWIHS_IMR_MCACK_Msk                   (0x1U << TWIHS_IMR_MCACK_Pos)                  /**< (TWIHS_IMR) Master Code Acknowledge Interrupt Mask Mask */
#define TWIHS_IMR_MCACK(value)                (TWIHS_IMR_MCACK_Msk & ((value) << TWIHS_IMR_MCACK_Pos))
#define TWIHS_IMR_TOUT_Pos                    (18U)                                          /**< (TWIHS_IMR) Timeout Error Interrupt Mask Position */
#define TWIHS_IMR_TOUT_Msk                    (0x1U << TWIHS_IMR_TOUT_Pos)                   /**< (TWIHS_IMR) Timeout Error Interrupt Mask Mask */
#define TWIHS_IMR_TOUT(value)                 (TWIHS_IMR_TOUT_Msk & ((value) << TWIHS_IMR_TOUT_Pos))
#define TWIHS_IMR_PECERR_Pos                  (19U)                                          /**< (TWIHS_IMR) PEC Error Interrupt Mask Position */
#define TWIHS_IMR_PECERR_Msk                  (0x1U << TWIHS_IMR_PECERR_Pos)                 /**< (TWIHS_IMR) PEC Error Interrupt Mask Mask */
#define TWIHS_IMR_PECERR(value)               (TWIHS_IMR_PECERR_Msk & ((value) << TWIHS_IMR_PECERR_Pos))
#define TWIHS_IMR_SMBDAM_Pos                  (20U)                                          /**< (TWIHS_IMR) SMBus Default Address Match Interrupt Mask Position */
#define TWIHS_IMR_SMBDAM_Msk                  (0x1U << TWIHS_IMR_SMBDAM_Pos)                 /**< (TWIHS_IMR) SMBus Default Address Match Interrupt Mask Mask */
#define TWIHS_IMR_SMBDAM(value)               (TWIHS_IMR_SMBDAM_Msk & ((value) << TWIHS_IMR_SMBDAM_Pos))
#define TWIHS_IMR_SMBHHM_Pos                  (21U)                                          /**< (TWIHS_IMR) SMBus Host Header Address Match Interrupt Mask Position */
#define TWIHS_IMR_SMBHHM_Msk                  (0x1U << TWIHS_IMR_SMBHHM_Pos)                 /**< (TWIHS_IMR) SMBus Host Header Address Match Interrupt Mask Mask */
#define TWIHS_IMR_SMBHHM(value)               (TWIHS_IMR_SMBHHM_Msk & ((value) << TWIHS_IMR_SMBHHM_Pos))
#define TWIHS_IMR_Msk                         (0x003D0FF7UL)                                 /**< (TWIHS_IMR) Register Mask  */

/* -------- TWIHS_RHR : (TWIHS Offset: 0x30) (R/  32) Receive Holding Register -------- */
#define TWIHS_RHR_RXDATA_Pos                  (0U)                                           /**< (TWIHS_RHR) Master or Slave Receive Holding Data Position */
#define TWIHS_RHR_RXDATA_Msk                  (0xFFU << TWIHS_RHR_RXDATA_Pos)                /**< (TWIHS_RHR) Master or Slave Receive Holding Data Mask */
#define TWIHS_RHR_RXDATA(value)               (TWIHS_RHR_RXDATA_Msk & ((value) << TWIHS_RHR_RXDATA_Pos))
#define TWIHS_RHR_Msk                         (0x000000FFUL)                                 /**< (TWIHS_RHR) Register Mask  */

/* -------- TWIHS_THR : (TWIHS Offset: 0x34) ( /W 32) Transmit Holding Register -------- */
#define TWIHS_THR_TXDATA_Pos                  (0U)                                           /**< (TWIHS_THR) Master or Slave Transmit Holding Data Position */
#define TWIHS_THR_TXDATA_Msk                  (0xFFU << TWIHS_THR_TXDATA_Pos)                /**< (TWIHS_THR) Master or Slave Transmit Holding Data Mask */
#define TWIHS_THR_TXDATA(value)               (TWIHS_THR_TXDATA_Msk & ((value) << TWIHS_THR_TXDATA_Pos))
#define TWIHS_THR_Msk                         (0x000000FFUL)                                 /**< (TWIHS_THR) Register Mask  */

/* -------- TWIHS_SMBTR : (TWIHS Offset: 0x38) (R/W 32) SMBus Timing Register -------- */
#define TWIHS_SMBTR_PRESC_Pos                 (0U)                                           /**< (TWIHS_SMBTR) SMBus Clock Prescaler Position */
#define TWIHS_SMBTR_PRESC_Msk                 (0xFU << TWIHS_SMBTR_PRESC_Pos)                /**< (TWIHS_SMBTR) SMBus Clock Prescaler Mask */
#define TWIHS_SMBTR_PRESC(value)              (TWIHS_SMBTR_PRESC_Msk & ((value) << TWIHS_SMBTR_PRESC_Pos))
#define TWIHS_SMBTR_TLOWS_Pos                 (8U)                                           /**< (TWIHS_SMBTR) Slave Clock Stretch Maximum Cycles Position */
#define TWIHS_SMBTR_TLOWS_Msk                 (0xFFU << TWIHS_SMBTR_TLOWS_Pos)               /**< (TWIHS_SMBTR) Slave Clock Stretch Maximum Cycles Mask */
#define TWIHS_SMBTR_TLOWS(value)              (TWIHS_SMBTR_TLOWS_Msk & ((value) << TWIHS_SMBTR_TLOWS_Pos))
#define TWIHS_SMBTR_TLOWM_Pos                 (16U)                                          /**< (TWIHS_SMBTR) Master Clock Stretch Maximum Cycles Position */
#define TWIHS_SMBTR_TLOWM_Msk                 (0xFFU << TWIHS_SMBTR_TLOWM_Pos)               /**< (TWIHS_SMBTR) Master Clock Stretch Maximum Cycles Mask */
#define TWIHS_SMBTR_TLOWM(value)              (TWIHS_SMBTR_TLOWM_Msk & ((value) << TWIHS_SMBTR_TLOWM_Pos))
#define TWIHS_SMBTR_THMAX_Pos                 (24U)                                          /**< (TWIHS_SMBTR) Clock High Maximum Cycles Position */
#define TWIHS_SMBTR_THMAX_Msk                 (0xFFU << TWIHS_SMBTR_THMAX_Pos)               /**< (TWIHS_SMBTR) Clock High Maximum Cycles Mask */
#define TWIHS_SMBTR_THMAX(value)              (TWIHS_SMBTR_THMAX_Msk & ((value) << TWIHS_SMBTR_THMAX_Pos))
#define TWIHS_SMBTR_Msk                       (0xFFFFFF0FUL)                                 /**< (TWIHS_SMBTR) Register Mask  */

/* -------- TWIHS_FILTR : (TWIHS Offset: 0x44) (R/W 32) Filter Register -------- */
#define TWIHS_FILTR_FILT_Pos                  (0U)                                           /**< (TWIHS_FILTR) RX Digital Filter Position */
#define TWIHS_FILTR_FILT_Msk                  (0x1U << TWIHS_FILTR_FILT_Pos)                 /**< (TWIHS_FILTR) RX Digital Filter Mask */
#define TWIHS_FILTR_FILT(value)               (TWIHS_FILTR_FILT_Msk & ((value) << TWIHS_FILTR_FILT_Pos))
#define TWIHS_FILTR_PADFEN_Pos                (1U)                                           /**< (TWIHS_FILTR) PAD Filter Enable Position */
#define TWIHS_FILTR_PADFEN_Msk                (0x1U << TWIHS_FILTR_PADFEN_Pos)               /**< (TWIHS_FILTR) PAD Filter Enable Mask */
#define TWIHS_FILTR_PADFEN(value)             (TWIHS_FILTR_PADFEN_Msk & ((value) << TWIHS_FILTR_PADFEN_Pos))
#define TWIHS_FILTR_PADFCFG_Pos               (2U)                                           /**< (TWIHS_FILTR) PAD Filter Config Position */
#define TWIHS_FILTR_PADFCFG_Msk               (0x1U << TWIHS_FILTR_PADFCFG_Pos)              /**< (TWIHS_FILTR) PAD Filter Config Mask */
#define TWIHS_FILTR_PADFCFG(value)            (TWIHS_FILTR_PADFCFG_Msk & ((value) << TWIHS_FILTR_PADFCFG_Pos))
#define TWIHS_FILTR_THRES_Pos                 (8U)                                           /**< (TWIHS_FILTR) Digital Filter Threshold Position */
#define TWIHS_FILTR_THRES_Msk                 (0x7U << TWIHS_FILTR_THRES_Pos)                /**< (TWIHS_FILTR) Digital Filter Threshold Mask */
#define TWIHS_FILTR_THRES(value)              (TWIHS_FILTR_THRES_Msk & ((value) << TWIHS_FILTR_THRES_Pos))
#define TWIHS_FILTR_Msk                       (0x00000707UL)                                 /**< (TWIHS_FILTR) Register Mask  */

/* -------- TWIHS_SWMR : (TWIHS Offset: 0x4C) (R/W 32) SleepWalking Matching Register -------- */
#define TWIHS_SWMR_SADR1_Pos                  (0U)                                           /**< (TWIHS_SWMR) Slave Address 1 Position */
#define TWIHS_SWMR_SADR1_Msk                  (0x7FU << TWIHS_SWMR_SADR1_Pos)                /**< (TWIHS_SWMR) Slave Address 1 Mask */
#define TWIHS_SWMR_SADR1(value)               (TWIHS_SWMR_SADR1_Msk & ((value) << TWIHS_SWMR_SADR1_Pos))
#define TWIHS_SWMR_SADR2_Pos                  (8U)                                           /**< (TWIHS_SWMR) Slave Address 2 Position */
#define TWIHS_SWMR_SADR2_Msk                  (0x7FU << TWIHS_SWMR_SADR2_Pos)                /**< (TWIHS_SWMR) Slave Address 2 Mask */
#define TWIHS_SWMR_SADR2(value)               (TWIHS_SWMR_SADR2_Msk & ((value) << TWIHS_SWMR_SADR2_Pos))
#define TWIHS_SWMR_SADR3_Pos                  (16U)                                          /**< (TWIHS_SWMR) Slave Address 3 Position */
#define TWIHS_SWMR_SADR3_Msk                  (0x7FU << TWIHS_SWMR_SADR3_Pos)                /**< (TWIHS_SWMR) Slave Address 3 Mask */
#define TWIHS_SWMR_SADR3(value)               (TWIHS_SWMR_SADR3_Msk & ((value) << TWIHS_SWMR_SADR3_Pos))
#define TWIHS_SWMR_DATAM_Pos                  (24U)                                          /**< (TWIHS_SWMR) Data Match Position */
#define TWIHS_SWMR_DATAM_Msk                  (0xFFU << TWIHS_SWMR_DATAM_Pos)                /**< (TWIHS_SWMR) Data Match Mask */
#define TWIHS_SWMR_DATAM(value)               (TWIHS_SWMR_DATAM_Msk & ((value) << TWIHS_SWMR_DATAM_Pos))
#define TWIHS_SWMR_Msk                        (0xFF7F7F7FUL)                                 /**< (TWIHS_SWMR) Register Mask  */

/* -------- TWIHS_WPMR : (TWIHS Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define TWIHS_WPMR_WPEN_Pos                   (0U)                                           /**< (TWIHS_WPMR) Write Protection Enable Position */
#define TWIHS_WPMR_WPEN_Msk                   (0x1U << TWIHS_WPMR_WPEN_Pos)                  /**< (TWIHS_WPMR) Write Protection Enable Mask */
#define TWIHS_WPMR_WPEN(value)                (TWIHS_WPMR_WPEN_Msk & ((value) << TWIHS_WPMR_WPEN_Pos))
#define TWIHS_WPMR_WPKEY_Pos                  (8U)                                           /**< (TWIHS_WPMR) Write Protection Key Position */
#define TWIHS_WPMR_WPKEY_Msk                  (0xFFFFFFU << TWIHS_WPMR_WPKEY_Pos)            /**< (TWIHS_WPMR) Write Protection Key Mask */
#define TWIHS_WPMR_WPKEY(value)               (TWIHS_WPMR_WPKEY_Msk & ((value) << TWIHS_WPMR_WPKEY_Pos))
#define   TWIHS_WPMR_WPKEY_PASSWD_Val         (5527369U)                                     /**< (TWIHS_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0  */
#define TWIHS_WPMR_WPKEY_PASSWD               (TWIHS_WPMR_WPKEY_PASSWD_Val << TWIHS_WPMR_WPKEY_Pos) /**< (TWIHS_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0 Position  */
#define TWIHS_WPMR_Msk                        (0xFFFFFF01UL)                                 /**< (TWIHS_WPMR) Register Mask  */

/* -------- TWIHS_WPSR : (TWIHS Offset: 0xE8) (R/  32) Write Protection Status Register -------- */
#define TWIHS_WPSR_WPVS_Pos                   (0U)                                           /**< (TWIHS_WPSR) Write Protection Violation Status Position */
#define TWIHS_WPSR_WPVS_Msk                   (0x1U << TWIHS_WPSR_WPVS_Pos)                  /**< (TWIHS_WPSR) Write Protection Violation Status Mask */
#define TWIHS_WPSR_WPVS(value)                (TWIHS_WPSR_WPVS_Msk & ((value) << TWIHS_WPSR_WPVS_Pos))
#define TWIHS_WPSR_WPVSRC_Pos                 (8U)                                           /**< (TWIHS_WPSR) Write Protection Violation Source Position */
#define TWIHS_WPSR_WPVSRC_Msk                 (0xFFFFFFU << TWIHS_WPSR_WPVSRC_Pos)           /**< (TWIHS_WPSR) Write Protection Violation Source Mask */
#define TWIHS_WPSR_WPVSRC(value)              (TWIHS_WPSR_WPVSRC_Msk & ((value) << TWIHS_WPSR_WPVSRC_Pos))
#define TWIHS_WPSR_Msk                        (0xFFFFFF01UL)                                 /**< (TWIHS_WPSR) Register Mask  */

/** \brief TWIHS register offsets definitions */
#define TWIHS_CR_OFFSET                (0x00)         /**< (TWIHS_CR) Control Register Offset */
#define TWIHS_MMR_OFFSET               (0x04)         /**< (TWIHS_MMR) Master Mode Register Offset */
#define TWIHS_SMR_OFFSET               (0x08)         /**< (TWIHS_SMR) Slave Mode Register Offset */
#define TWIHS_IADR_OFFSET              (0x0C)         /**< (TWIHS_IADR) Internal Address Register Offset */
#define TWIHS_CWGR_OFFSET              (0x10)         /**< (TWIHS_CWGR) Clock Waveform Generator Register Offset */
#define TWIHS_SR_OFFSET                (0x20)         /**< (TWIHS_SR) Status Register Offset */
#define TWIHS_IER_OFFSET               (0x24)         /**< (TWIHS_IER) Interrupt Enable Register Offset */
#define TWIHS_IDR_OFFSET               (0x28)         /**< (TWIHS_IDR) Interrupt Disable Register Offset */
#define TWIHS_IMR_OFFSET               (0x2C)         /**< (TWIHS_IMR) Interrupt Mask Register Offset */
#define TWIHS_RHR_OFFSET               (0x30)         /**< (TWIHS_RHR) Receive Holding Register Offset */
#define TWIHS_THR_OFFSET               (0x34)         /**< (TWIHS_THR) Transmit Holding Register Offset */
#define TWIHS_SMBTR_OFFSET             (0x38)         /**< (TWIHS_SMBTR) SMBus Timing Register Offset */
#define TWIHS_FILTR_OFFSET             (0x44)         /**< (TWIHS_FILTR) Filter Register Offset */
#define TWIHS_SWMR_OFFSET              (0x4C)         /**< (TWIHS_SWMR) SleepWalking Matching Register Offset */
#define TWIHS_WPMR_OFFSET              (0xE4)         /**< (TWIHS_WPMR) Write Protection Mode Register Offset */
#define TWIHS_WPSR_OFFSET              (0xE8)         /**< (TWIHS_WPSR) Write Protection Status Register Offset */

/** \brief TWIHS register API structure */
typedef struct
{
  __O   uint32_t                       TWIHS_CR;        /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       TWIHS_MMR;       /**< Offset: 0x04 (R/W  32) Master Mode Register */
  __IO  uint32_t                       TWIHS_SMR;       /**< Offset: 0x08 (R/W  32) Slave Mode Register */
  __IO  uint32_t                       TWIHS_IADR;      /**< Offset: 0x0c (R/W  32) Internal Address Register */
  __IO  uint32_t                       TWIHS_CWGR;      /**< Offset: 0x10 (R/W  32) Clock Waveform Generator Register */
  __I   uint8_t                        Reserved1[0x0C];
  __I   uint32_t                       TWIHS_SR;        /**< Offset: 0x20 (R/   32) Status Register */
  __O   uint32_t                       TWIHS_IER;       /**< Offset: 0x24 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       TWIHS_IDR;       /**< Offset: 0x28 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       TWIHS_IMR;       /**< Offset: 0x2c (R/   32) Interrupt Mask Register */
  __I   uint32_t                       TWIHS_RHR;       /**< Offset: 0x30 (R/   32) Receive Holding Register */
  __O   uint32_t                       TWIHS_THR;       /**< Offset: 0x34 ( /W  32) Transmit Holding Register */
  __IO  uint32_t                       TWIHS_SMBTR;     /**< Offset: 0x38 (R/W  32) SMBus Timing Register */
  __I   uint8_t                        Reserved2[0x08];
  __IO  uint32_t                       TWIHS_FILTR;     /**< Offset: 0x44 (R/W  32) Filter Register */
  __I   uint8_t                        Reserved3[0x04];
  __IO  uint32_t                       TWIHS_SWMR;      /**< Offset: 0x4c (R/W  32) SleepWalking Matching Register */
  __I   uint8_t                        Reserved4[0x94];
  __IO  uint32_t                       TWIHS_WPMR;      /**< Offset: 0xe4 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       TWIHS_WPSR;      /**< Offset: 0xe8 (R/   32) Write Protection Status Register */
} twihs_registers_t;
/** @}  end of Two-wire Interface High Speed */

#endif /* SAME_SAME70_TWIHS_MODULE_H */

