/**
 * \brief Header file for SAME/SAME70 SPI
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
#ifndef SAME_SAME70_SPI_MODULE_H
#define SAME_SAME70_SPI_MODULE_H

/** \addtogroup SAME_SAME70 Serial Peripheral Interface
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_SPI                                   */
/* ========================================================================== */

/* -------- SPI_CR : (SPI Offset: 0x00) ( /W 32) Control Register -------- */
#define SPI_CR_SPIEN_Pos                      (0U)                                           /**< (SPI_CR) SPI Enable Position */
#define SPI_CR_SPIEN_Msk                      (0x1U << SPI_CR_SPIEN_Pos)                     /**< (SPI_CR) SPI Enable Mask */
#define SPI_CR_SPIEN(value)                   (SPI_CR_SPIEN_Msk & ((value) << SPI_CR_SPIEN_Pos))
#define SPI_CR_SPIDIS_Pos                     (1U)                                           /**< (SPI_CR) SPI Disable Position */
#define SPI_CR_SPIDIS_Msk                     (0x1U << SPI_CR_SPIDIS_Pos)                    /**< (SPI_CR) SPI Disable Mask */
#define SPI_CR_SPIDIS(value)                  (SPI_CR_SPIDIS_Msk & ((value) << SPI_CR_SPIDIS_Pos))
#define SPI_CR_SWRST_Pos                      (7U)                                           /**< (SPI_CR) SPI Software Reset Position */
#define SPI_CR_SWRST_Msk                      (0x1U << SPI_CR_SWRST_Pos)                     /**< (SPI_CR) SPI Software Reset Mask */
#define SPI_CR_SWRST(value)                   (SPI_CR_SWRST_Msk & ((value) << SPI_CR_SWRST_Pos))
#define SPI_CR_REQCLR_Pos                     (12U)                                          /**< (SPI_CR) Request to Clear the Comparison Trigger Position */
#define SPI_CR_REQCLR_Msk                     (0x1U << SPI_CR_REQCLR_Pos)                    /**< (SPI_CR) Request to Clear the Comparison Trigger Mask */
#define SPI_CR_REQCLR(value)                  (SPI_CR_REQCLR_Msk & ((value) << SPI_CR_REQCLR_Pos))
#define SPI_CR_LASTXFER_Pos                   (24U)                                          /**< (SPI_CR) Last Transfer Position */
#define SPI_CR_LASTXFER_Msk                   (0x1U << SPI_CR_LASTXFER_Pos)                  /**< (SPI_CR) Last Transfer Mask */
#define SPI_CR_LASTXFER(value)                (SPI_CR_LASTXFER_Msk & ((value) << SPI_CR_LASTXFER_Pos))
#define SPI_CR_Msk                            (0x01001083UL)                                 /**< (SPI_CR) Register Mask  */

/* -------- SPI_MR : (SPI Offset: 0x04) (R/W 32) Mode Register -------- */
#define SPI_MR_MSTR_Pos                       (0U)                                           /**< (SPI_MR) Master/Slave Mode Position */
#define SPI_MR_MSTR_Msk                       (0x1U << SPI_MR_MSTR_Pos)                      /**< (SPI_MR) Master/Slave Mode Mask */
#define SPI_MR_MSTR(value)                    (SPI_MR_MSTR_Msk & ((value) << SPI_MR_MSTR_Pos))
#define   SPI_MR_MSTR_MASTER_Val              (1U)                                           /**< (SPI_MR) Master  */
#define   SPI_MR_MSTR_SLAVE_Val               (0U)                                           /**< (SPI_MR) Slave  */
#define SPI_MR_MSTR_MASTER                    (SPI_MR_MSTR_MASTER_Val << SPI_MR_MSTR_Pos)    /**< (SPI_MR) Master Position  */
#define SPI_MR_MSTR_SLAVE                     (SPI_MR_MSTR_SLAVE_Val << SPI_MR_MSTR_Pos)     /**< (SPI_MR) Slave Position  */
#define SPI_MR_PS_Pos                         (1U)                                           /**< (SPI_MR) Peripheral Select Position */
#define SPI_MR_PS_Msk                         (0x1U << SPI_MR_PS_Pos)                        /**< (SPI_MR) Peripheral Select Mask */
#define SPI_MR_PS(value)                      (SPI_MR_PS_Msk & ((value) << SPI_MR_PS_Pos))  
#define SPI_MR_PCSDEC_Pos                     (2U)                                           /**< (SPI_MR) Chip Select Decode Position */
#define SPI_MR_PCSDEC_Msk                     (0x1U << SPI_MR_PCSDEC_Pos)                    /**< (SPI_MR) Chip Select Decode Mask */
#define SPI_MR_PCSDEC(value)                  (SPI_MR_PCSDEC_Msk & ((value) << SPI_MR_PCSDEC_Pos))
#define SPI_MR_MODFDIS_Pos                    (4U)                                           /**< (SPI_MR) Mode Fault Detection Position */
#define SPI_MR_MODFDIS_Msk                    (0x1U << SPI_MR_MODFDIS_Pos)                   /**< (SPI_MR) Mode Fault Detection Mask */
#define SPI_MR_MODFDIS(value)                 (SPI_MR_MODFDIS_Msk & ((value) << SPI_MR_MODFDIS_Pos))
#define SPI_MR_WDRBT_Pos                      (5U)                                           /**< (SPI_MR) Wait Data Read Before Transfer Position */
#define SPI_MR_WDRBT_Msk                      (0x1U << SPI_MR_WDRBT_Pos)                     /**< (SPI_MR) Wait Data Read Before Transfer Mask */
#define SPI_MR_WDRBT(value)                   (SPI_MR_WDRBT_Msk & ((value) << SPI_MR_WDRBT_Pos))
#define SPI_MR_LLB_Pos                        (7U)                                           /**< (SPI_MR) Local Loopback Enable Position */
#define SPI_MR_LLB_Msk                        (0x1U << SPI_MR_LLB_Pos)                       /**< (SPI_MR) Local Loopback Enable Mask */
#define SPI_MR_LLB(value)                     (SPI_MR_LLB_Msk & ((value) << SPI_MR_LLB_Pos))
#define SPI_MR_PCS_Pos                        (16U)                                          /**< (SPI_MR) Peripheral Chip Select Position */
#define SPI_MR_PCS_Msk                        (0xFU << SPI_MR_PCS_Pos)                       /**< (SPI_MR) Peripheral Chip Select Mask */
#define SPI_MR_PCS(value)                     (SPI_MR_PCS_Msk & ((value) << SPI_MR_PCS_Pos))
#define   SPI_MR_PCS_NPCS0_Val                (14U)                                          /**< (SPI_MR) NPCS0 as Chip Select  */
#define   SPI_MR_PCS_NPCS1_Val                (13U)                                          /**< (SPI_MR) NPCS1 as Chip Select  */
#define   SPI_MR_PCS_NPCS2_Val                (11U)                                          /**< (SPI_MR) NPCS2 as Chip Select  */
#define   SPI_MR_PCS_NPCS3_Val                (7U)                                           /**< (SPI_MR) NPCS3 as Chip Select  */
#define SPI_MR_PCS_NPCS0                      (SPI_MR_PCS_NPCS0_Val << SPI_MR_PCS_Pos)       /**< (SPI_MR) NPCS0 as Chip Select Position  */
#define SPI_MR_PCS_NPCS1                      (SPI_MR_PCS_NPCS1_Val << SPI_MR_PCS_Pos)       /**< (SPI_MR) NPCS1 as Chip Select Position  */
#define SPI_MR_PCS_NPCS2                      (SPI_MR_PCS_NPCS2_Val << SPI_MR_PCS_Pos)       /**< (SPI_MR) NPCS2 as Chip Select Position  */
#define SPI_MR_PCS_NPCS3                      (SPI_MR_PCS_NPCS3_Val << SPI_MR_PCS_Pos)       /**< (SPI_MR) NPCS3 as Chip Select Position  */
#define SPI_MR_DLYBCS_Pos                     (24U)                                          /**< (SPI_MR) Delay Between Chip Selects Position */
#define SPI_MR_DLYBCS_Msk                     (0xFFU << SPI_MR_DLYBCS_Pos)                   /**< (SPI_MR) Delay Between Chip Selects Mask */
#define SPI_MR_DLYBCS(value)                  (SPI_MR_DLYBCS_Msk & ((value) << SPI_MR_DLYBCS_Pos))
#define SPI_MR_Msk                            (0xFF0F00B7UL)                                 /**< (SPI_MR) Register Mask  */

/* -------- SPI_RDR : (SPI Offset: 0x08) (R/  32) Receive Data Register -------- */
#define SPI_RDR_RD_Pos                        (0U)                                           /**< (SPI_RDR) Receive Data Position */
#define SPI_RDR_RD_Msk                        (0xFFFFU << SPI_RDR_RD_Pos)                    /**< (SPI_RDR) Receive Data Mask */
#define SPI_RDR_RD(value)                     (SPI_RDR_RD_Msk & ((value) << SPI_RDR_RD_Pos))
#define SPI_RDR_PCS_Pos                       (16U)                                          /**< (SPI_RDR) Peripheral Chip Select Position */
#define SPI_RDR_PCS_Msk                       (0xFU << SPI_RDR_PCS_Pos)                      /**< (SPI_RDR) Peripheral Chip Select Mask */
#define SPI_RDR_PCS(value)                    (SPI_RDR_PCS_Msk & ((value) << SPI_RDR_PCS_Pos))
#define SPI_RDR_Msk                           (0x000FFFFFUL)                                 /**< (SPI_RDR) Register Mask  */

/* -------- SPI_TDR : (SPI Offset: 0x0C) ( /W 32) Transmit Data Register -------- */
#define SPI_TDR_TD_Pos                        (0U)                                           /**< (SPI_TDR) Transmit Data Position */
#define SPI_TDR_TD_Msk                        (0xFFFFU << SPI_TDR_TD_Pos)                    /**< (SPI_TDR) Transmit Data Mask */
#define SPI_TDR_TD(value)                     (SPI_TDR_TD_Msk & ((value) << SPI_TDR_TD_Pos))
#define SPI_TDR_PCS_Pos                       (16U)                                          /**< (SPI_TDR) Peripheral Chip Select Position */
#define SPI_TDR_PCS_Msk                       (0xFU << SPI_TDR_PCS_Pos)                      /**< (SPI_TDR) Peripheral Chip Select Mask */
#define SPI_TDR_PCS(value)                    (SPI_TDR_PCS_Msk & ((value) << SPI_TDR_PCS_Pos))
#define   SPI_TDR_PCS_NPCS0_Val               (14U)                                          /**< (SPI_TDR) NPCS0 as Chip Select  */
#define   SPI_TDR_PCS_NPCS1_Val               (13U)                                          /**< (SPI_TDR) NPCS1 as Chip Select  */
#define   SPI_TDR_PCS_NPCS2_Val               (11U)                                          /**< (SPI_TDR) NPCS2 as Chip Select  */
#define   SPI_TDR_PCS_NPCS3_Val               (7U)                                           /**< (SPI_TDR) NPCS3 as Chip Select  */
#define SPI_TDR_PCS_NPCS0                     (SPI_TDR_PCS_NPCS0_Val << SPI_TDR_PCS_Pos)     /**< (SPI_TDR) NPCS0 as Chip Select Position  */
#define SPI_TDR_PCS_NPCS1                     (SPI_TDR_PCS_NPCS1_Val << SPI_TDR_PCS_Pos)     /**< (SPI_TDR) NPCS1 as Chip Select Position  */
#define SPI_TDR_PCS_NPCS2                     (SPI_TDR_PCS_NPCS2_Val << SPI_TDR_PCS_Pos)     /**< (SPI_TDR) NPCS2 as Chip Select Position  */
#define SPI_TDR_PCS_NPCS3                     (SPI_TDR_PCS_NPCS3_Val << SPI_TDR_PCS_Pos)     /**< (SPI_TDR) NPCS3 as Chip Select Position  */
#define SPI_TDR_LASTXFER_Pos                  (24U)                                          /**< (SPI_TDR) Last Transfer Position */
#define SPI_TDR_LASTXFER_Msk                  (0x1U << SPI_TDR_LASTXFER_Pos)                 /**< (SPI_TDR) Last Transfer Mask */
#define SPI_TDR_LASTXFER(value)               (SPI_TDR_LASTXFER_Msk & ((value) << SPI_TDR_LASTXFER_Pos))
#define SPI_TDR_Msk                           (0x010FFFFFUL)                                 /**< (SPI_TDR) Register Mask  */

/* -------- SPI_SR : (SPI Offset: 0x10) (R/  32) Status Register -------- */
#define SPI_SR_RDRF_Pos                       (0U)                                           /**< (SPI_SR) Receive Data Register Full (cleared by reading SPI_RDR) Position */
#define SPI_SR_RDRF_Msk                       (0x1U << SPI_SR_RDRF_Pos)                      /**< (SPI_SR) Receive Data Register Full (cleared by reading SPI_RDR) Mask */
#define SPI_SR_RDRF(value)                    (SPI_SR_RDRF_Msk & ((value) << SPI_SR_RDRF_Pos))
#define SPI_SR_TDRE_Pos                       (1U)                                           /**< (SPI_SR) Transmit Data Register Empty (cleared by writing SPI_TDR) Position */
#define SPI_SR_TDRE_Msk                       (0x1U << SPI_SR_TDRE_Pos)                      /**< (SPI_SR) Transmit Data Register Empty (cleared by writing SPI_TDR) Mask */
#define SPI_SR_TDRE(value)                    (SPI_SR_TDRE_Msk & ((value) << SPI_SR_TDRE_Pos))
#define SPI_SR_MODF_Pos                       (2U)                                           /**< (SPI_SR) Mode Fault Error (cleared on read) Position */
#define SPI_SR_MODF_Msk                       (0x1U << SPI_SR_MODF_Pos)                      /**< (SPI_SR) Mode Fault Error (cleared on read) Mask */
#define SPI_SR_MODF(value)                    (SPI_SR_MODF_Msk & ((value) << SPI_SR_MODF_Pos))
#define SPI_SR_OVRES_Pos                      (3U)                                           /**< (SPI_SR) Overrun Error Status (cleared on read) Position */
#define SPI_SR_OVRES_Msk                      (0x1U << SPI_SR_OVRES_Pos)                     /**< (SPI_SR) Overrun Error Status (cleared on read) Mask */
#define SPI_SR_OVRES(value)                   (SPI_SR_OVRES_Msk & ((value) << SPI_SR_OVRES_Pos))
#define SPI_SR_NSSR_Pos                       (8U)                                           /**< (SPI_SR) NSS Rising (cleared on read) Position */
#define SPI_SR_NSSR_Msk                       (0x1U << SPI_SR_NSSR_Pos)                      /**< (SPI_SR) NSS Rising (cleared on read) Mask */
#define SPI_SR_NSSR(value)                    (SPI_SR_NSSR_Msk & ((value) << SPI_SR_NSSR_Pos))
#define SPI_SR_TXEMPTY_Pos                    (9U)                                           /**< (SPI_SR) Transmission Registers Empty (cleared by writing SPI_TDR) Position */
#define SPI_SR_TXEMPTY_Msk                    (0x1U << SPI_SR_TXEMPTY_Pos)                   /**< (SPI_SR) Transmission Registers Empty (cleared by writing SPI_TDR) Mask */
#define SPI_SR_TXEMPTY(value)                 (SPI_SR_TXEMPTY_Msk & ((value) << SPI_SR_TXEMPTY_Pos))
#define SPI_SR_UNDES_Pos                      (10U)                                          /**< (SPI_SR) Underrun Error Status (Slave mode only) (cleared on read) Position */
#define SPI_SR_UNDES_Msk                      (0x1U << SPI_SR_UNDES_Pos)                     /**< (SPI_SR) Underrun Error Status (Slave mode only) (cleared on read) Mask */
#define SPI_SR_UNDES(value)                   (SPI_SR_UNDES_Msk & ((value) << SPI_SR_UNDES_Pos))
#define SPI_SR_SPIENS_Pos                     (16U)                                          /**< (SPI_SR) SPI Enable Status Position */
#define SPI_SR_SPIENS_Msk                     (0x1U << SPI_SR_SPIENS_Pos)                    /**< (SPI_SR) SPI Enable Status Mask */
#define SPI_SR_SPIENS(value)                  (SPI_SR_SPIENS_Msk & ((value) << SPI_SR_SPIENS_Pos))
#define SPI_SR_Msk                            (0x0001070FUL)                                 /**< (SPI_SR) Register Mask  */

/* -------- SPI_IER : (SPI Offset: 0x14) ( /W 32) Interrupt Enable Register -------- */
#define SPI_IER_RDRF_Pos                      (0U)                                           /**< (SPI_IER) Receive Data Register Full Interrupt Enable Position */
#define SPI_IER_RDRF_Msk                      (0x1U << SPI_IER_RDRF_Pos)                     /**< (SPI_IER) Receive Data Register Full Interrupt Enable Mask */
#define SPI_IER_RDRF(value)                   (SPI_IER_RDRF_Msk & ((value) << SPI_IER_RDRF_Pos))
#define SPI_IER_TDRE_Pos                      (1U)                                           /**< (SPI_IER) SPI Transmit Data Register Empty Interrupt Enable Position */
#define SPI_IER_TDRE_Msk                      (0x1U << SPI_IER_TDRE_Pos)                     /**< (SPI_IER) SPI Transmit Data Register Empty Interrupt Enable Mask */
#define SPI_IER_TDRE(value)                   (SPI_IER_TDRE_Msk & ((value) << SPI_IER_TDRE_Pos))
#define SPI_IER_MODF_Pos                      (2U)                                           /**< (SPI_IER) Mode Fault Error Interrupt Enable Position */
#define SPI_IER_MODF_Msk                      (0x1U << SPI_IER_MODF_Pos)                     /**< (SPI_IER) Mode Fault Error Interrupt Enable Mask */
#define SPI_IER_MODF(value)                   (SPI_IER_MODF_Msk & ((value) << SPI_IER_MODF_Pos))
#define SPI_IER_OVRES_Pos                     (3U)                                           /**< (SPI_IER) Overrun Error Interrupt Enable Position */
#define SPI_IER_OVRES_Msk                     (0x1U << SPI_IER_OVRES_Pos)                    /**< (SPI_IER) Overrun Error Interrupt Enable Mask */
#define SPI_IER_OVRES(value)                  (SPI_IER_OVRES_Msk & ((value) << SPI_IER_OVRES_Pos))
#define SPI_IER_NSSR_Pos                      (8U)                                           /**< (SPI_IER) NSS Rising Interrupt Enable Position */
#define SPI_IER_NSSR_Msk                      (0x1U << SPI_IER_NSSR_Pos)                     /**< (SPI_IER) NSS Rising Interrupt Enable Mask */
#define SPI_IER_NSSR(value)                   (SPI_IER_NSSR_Msk & ((value) << SPI_IER_NSSR_Pos))
#define SPI_IER_TXEMPTY_Pos                   (9U)                                           /**< (SPI_IER) Transmission Registers Empty Enable Position */
#define SPI_IER_TXEMPTY_Msk                   (0x1U << SPI_IER_TXEMPTY_Pos)                  /**< (SPI_IER) Transmission Registers Empty Enable Mask */
#define SPI_IER_TXEMPTY(value)                (SPI_IER_TXEMPTY_Msk & ((value) << SPI_IER_TXEMPTY_Pos))
#define SPI_IER_UNDES_Pos                     (10U)                                          /**< (SPI_IER) Underrun Error Interrupt Enable Position */
#define SPI_IER_UNDES_Msk                     (0x1U << SPI_IER_UNDES_Pos)                    /**< (SPI_IER) Underrun Error Interrupt Enable Mask */
#define SPI_IER_UNDES(value)                  (SPI_IER_UNDES_Msk & ((value) << SPI_IER_UNDES_Pos))
#define SPI_IER_Msk                           (0x0000070FUL)                                 /**< (SPI_IER) Register Mask  */

/* -------- SPI_IDR : (SPI Offset: 0x18) ( /W 32) Interrupt Disable Register -------- */
#define SPI_IDR_RDRF_Pos                      (0U)                                           /**< (SPI_IDR) Receive Data Register Full Interrupt Disable Position */
#define SPI_IDR_RDRF_Msk                      (0x1U << SPI_IDR_RDRF_Pos)                     /**< (SPI_IDR) Receive Data Register Full Interrupt Disable Mask */
#define SPI_IDR_RDRF(value)                   (SPI_IDR_RDRF_Msk & ((value) << SPI_IDR_RDRF_Pos))
#define SPI_IDR_TDRE_Pos                      (1U)                                           /**< (SPI_IDR) SPI Transmit Data Register Empty Interrupt Disable Position */
#define SPI_IDR_TDRE_Msk                      (0x1U << SPI_IDR_TDRE_Pos)                     /**< (SPI_IDR) SPI Transmit Data Register Empty Interrupt Disable Mask */
#define SPI_IDR_TDRE(value)                   (SPI_IDR_TDRE_Msk & ((value) << SPI_IDR_TDRE_Pos))
#define SPI_IDR_MODF_Pos                      (2U)                                           /**< (SPI_IDR) Mode Fault Error Interrupt Disable Position */
#define SPI_IDR_MODF_Msk                      (0x1U << SPI_IDR_MODF_Pos)                     /**< (SPI_IDR) Mode Fault Error Interrupt Disable Mask */
#define SPI_IDR_MODF(value)                   (SPI_IDR_MODF_Msk & ((value) << SPI_IDR_MODF_Pos))
#define SPI_IDR_OVRES_Pos                     (3U)                                           /**< (SPI_IDR) Overrun Error Interrupt Disable Position */
#define SPI_IDR_OVRES_Msk                     (0x1U << SPI_IDR_OVRES_Pos)                    /**< (SPI_IDR) Overrun Error Interrupt Disable Mask */
#define SPI_IDR_OVRES(value)                  (SPI_IDR_OVRES_Msk & ((value) << SPI_IDR_OVRES_Pos))
#define SPI_IDR_NSSR_Pos                      (8U)                                           /**< (SPI_IDR) NSS Rising Interrupt Disable Position */
#define SPI_IDR_NSSR_Msk                      (0x1U << SPI_IDR_NSSR_Pos)                     /**< (SPI_IDR) NSS Rising Interrupt Disable Mask */
#define SPI_IDR_NSSR(value)                   (SPI_IDR_NSSR_Msk & ((value) << SPI_IDR_NSSR_Pos))
#define SPI_IDR_TXEMPTY_Pos                   (9U)                                           /**< (SPI_IDR) Transmission Registers Empty Disable Position */
#define SPI_IDR_TXEMPTY_Msk                   (0x1U << SPI_IDR_TXEMPTY_Pos)                  /**< (SPI_IDR) Transmission Registers Empty Disable Mask */
#define SPI_IDR_TXEMPTY(value)                (SPI_IDR_TXEMPTY_Msk & ((value) << SPI_IDR_TXEMPTY_Pos))
#define SPI_IDR_UNDES_Pos                     (10U)                                          /**< (SPI_IDR) Underrun Error Interrupt Disable Position */
#define SPI_IDR_UNDES_Msk                     (0x1U << SPI_IDR_UNDES_Pos)                    /**< (SPI_IDR) Underrun Error Interrupt Disable Mask */
#define SPI_IDR_UNDES(value)                  (SPI_IDR_UNDES_Msk & ((value) << SPI_IDR_UNDES_Pos))
#define SPI_IDR_Msk                           (0x0000070FUL)                                 /**< (SPI_IDR) Register Mask  */

/* -------- SPI_IMR : (SPI Offset: 0x1C) (R/  32) Interrupt Mask Register -------- */
#define SPI_IMR_RDRF_Pos                      (0U)                                           /**< (SPI_IMR) Receive Data Register Full Interrupt Mask Position */
#define SPI_IMR_RDRF_Msk                      (0x1U << SPI_IMR_RDRF_Pos)                     /**< (SPI_IMR) Receive Data Register Full Interrupt Mask Mask */
#define SPI_IMR_RDRF(value)                   (SPI_IMR_RDRF_Msk & ((value) << SPI_IMR_RDRF_Pos))
#define SPI_IMR_TDRE_Pos                      (1U)                                           /**< (SPI_IMR) SPI Transmit Data Register Empty Interrupt Mask Position */
#define SPI_IMR_TDRE_Msk                      (0x1U << SPI_IMR_TDRE_Pos)                     /**< (SPI_IMR) SPI Transmit Data Register Empty Interrupt Mask Mask */
#define SPI_IMR_TDRE(value)                   (SPI_IMR_TDRE_Msk & ((value) << SPI_IMR_TDRE_Pos))
#define SPI_IMR_MODF_Pos                      (2U)                                           /**< (SPI_IMR) Mode Fault Error Interrupt Mask Position */
#define SPI_IMR_MODF_Msk                      (0x1U << SPI_IMR_MODF_Pos)                     /**< (SPI_IMR) Mode Fault Error Interrupt Mask Mask */
#define SPI_IMR_MODF(value)                   (SPI_IMR_MODF_Msk & ((value) << SPI_IMR_MODF_Pos))
#define SPI_IMR_OVRES_Pos                     (3U)                                           /**< (SPI_IMR) Overrun Error Interrupt Mask Position */
#define SPI_IMR_OVRES_Msk                     (0x1U << SPI_IMR_OVRES_Pos)                    /**< (SPI_IMR) Overrun Error Interrupt Mask Mask */
#define SPI_IMR_OVRES(value)                  (SPI_IMR_OVRES_Msk & ((value) << SPI_IMR_OVRES_Pos))
#define SPI_IMR_NSSR_Pos                      (8U)                                           /**< (SPI_IMR) NSS Rising Interrupt Mask Position */
#define SPI_IMR_NSSR_Msk                      (0x1U << SPI_IMR_NSSR_Pos)                     /**< (SPI_IMR) NSS Rising Interrupt Mask Mask */
#define SPI_IMR_NSSR(value)                   (SPI_IMR_NSSR_Msk & ((value) << SPI_IMR_NSSR_Pos))
#define SPI_IMR_TXEMPTY_Pos                   (9U)                                           /**< (SPI_IMR) Transmission Registers Empty Mask Position */
#define SPI_IMR_TXEMPTY_Msk                   (0x1U << SPI_IMR_TXEMPTY_Pos)                  /**< (SPI_IMR) Transmission Registers Empty Mask Mask */
#define SPI_IMR_TXEMPTY(value)                (SPI_IMR_TXEMPTY_Msk & ((value) << SPI_IMR_TXEMPTY_Pos))
#define SPI_IMR_UNDES_Pos                     (10U)                                          /**< (SPI_IMR) Underrun Error Interrupt Mask Position */
#define SPI_IMR_UNDES_Msk                     (0x1U << SPI_IMR_UNDES_Pos)                    /**< (SPI_IMR) Underrun Error Interrupt Mask Mask */
#define SPI_IMR_UNDES(value)                  (SPI_IMR_UNDES_Msk & ((value) << SPI_IMR_UNDES_Pos))
#define SPI_IMR_Msk                           (0x0000070FUL)                                 /**< (SPI_IMR) Register Mask  */

/* -------- SPI_CSR : (SPI Offset: 0x30) (R/W 32) Chip Select Register -------- */
#define SPI_CSR_CPOL_Pos                      (0U)                                           /**< (SPI_CSR) Clock Polarity Position */
#define SPI_CSR_CPOL_Msk                      (0x1U << SPI_CSR_CPOL_Pos)                     /**< (SPI_CSR) Clock Polarity Mask */
#define SPI_CSR_CPOL(value)                   (SPI_CSR_CPOL_Msk & ((value) << SPI_CSR_CPOL_Pos))
#define   SPI_CSR_CPOL_IDLE_LOW_Val           (0U)                                           /**< (SPI_CSR) Clock is low when inactive (CPOL=0)  */
#define   SPI_CSR_CPOL_IDLE_HIGH_Val          (1U)                                           /**< (SPI_CSR) Clock is high when inactive (CPOL=1)  */
#define SPI_CSR_CPOL_IDLE_LOW                 (SPI_CSR_CPOL_IDLE_LOW_Val << SPI_CSR_CPOL_Pos) /**< (SPI_CSR) Clock is low when inactive (CPOL=0) Position  */
#define SPI_CSR_CPOL_IDLE_HIGH                (SPI_CSR_CPOL_IDLE_HIGH_Val << SPI_CSR_CPOL_Pos) /**< (SPI_CSR) Clock is high when inactive (CPOL=1) Position  */
#define SPI_CSR_NCPHA_Pos                     (1U)                                           /**< (SPI_CSR) Clock Phase Position */
#define SPI_CSR_NCPHA_Msk                     (0x1U << SPI_CSR_NCPHA_Pos)                    /**< (SPI_CSR) Clock Phase Mask */
#define SPI_CSR_NCPHA(value)                  (SPI_CSR_NCPHA_Msk & ((value) << SPI_CSR_NCPHA_Pos))
#define   SPI_CSR_NCPHA_VALID_LEADING_EDGE_Val (1U)                                           /**< (SPI_CSR) Data is valid on clock leading edge (CPHA=0)  */
#define   SPI_CSR_NCPHA_VALID_TRAILING_EDGE_Val (0U)                                           /**< (SPI_CSR) Data is valid on clock trailing edge (CPHA=1)  */
#define SPI_CSR_NCPHA_VALID_LEADING_EDGE      (SPI_CSR_NCPHA_VALID_LEADING_EDGE_Val << SPI_CSR_NCPHA_Pos) /**< (SPI_CSR) Data is valid on clock leading edge (CPHA=0) Position  */
#define SPI_CSR_NCPHA_VALID_TRAILING_EDGE     (SPI_CSR_NCPHA_VALID_TRAILING_EDGE_Val << SPI_CSR_NCPHA_Pos) /**< (SPI_CSR) Data is valid on clock trailing edge (CPHA=1) Position  */
#define SPI_CSR_CSNAAT_Pos                    (2U)                                           /**< (SPI_CSR) Chip Select Not Active After Transfer (Ignored if CSAAT = 1) Position */
#define SPI_CSR_CSNAAT_Msk                    (0x1U << SPI_CSR_CSNAAT_Pos)                   /**< (SPI_CSR) Chip Select Not Active After Transfer (Ignored if CSAAT = 1) Mask */
#define SPI_CSR_CSNAAT(value)                 (SPI_CSR_CSNAAT_Msk & ((value) << SPI_CSR_CSNAAT_Pos))
#define SPI_CSR_CSAAT_Pos                     (3U)                                           /**< (SPI_CSR) Chip Select Active After Transfer Position */
#define SPI_CSR_CSAAT_Msk                     (0x1U << SPI_CSR_CSAAT_Pos)                    /**< (SPI_CSR) Chip Select Active After Transfer Mask */
#define SPI_CSR_CSAAT(value)                  (SPI_CSR_CSAAT_Msk & ((value) << SPI_CSR_CSAAT_Pos))
#define SPI_CSR_BITS_Pos                      (4U)                                           /**< (SPI_CSR) Bits Per Transfer Position */
#define SPI_CSR_BITS_Msk                      (0xFU << SPI_CSR_BITS_Pos)                     /**< (SPI_CSR) Bits Per Transfer Mask */
#define SPI_CSR_BITS(value)                   (SPI_CSR_BITS_Msk & ((value) << SPI_CSR_BITS_Pos))
#define   SPI_CSR_BITS_8_BIT_Val              (0U)                                           /**< (SPI_CSR) 8 bits for transfer  */
#define   SPI_CSR_BITS_9_BIT_Val              (1U)                                           /**< (SPI_CSR) 9 bits for transfer  */
#define   SPI_CSR_BITS_10_BIT_Val             (2U)                                           /**< (SPI_CSR) 10 bits for transfer  */
#define   SPI_CSR_BITS_11_BIT_Val             (3U)                                           /**< (SPI_CSR) 11 bits for transfer  */
#define   SPI_CSR_BITS_12_BIT_Val             (4U)                                           /**< (SPI_CSR) 12 bits for transfer  */
#define   SPI_CSR_BITS_13_BIT_Val             (5U)                                           /**< (SPI_CSR) 13 bits for transfer  */
#define   SPI_CSR_BITS_14_BIT_Val             (6U)                                           /**< (SPI_CSR) 14 bits for transfer  */
#define   SPI_CSR_BITS_15_BIT_Val             (7U)                                           /**< (SPI_CSR) 15 bits for transfer  */
#define   SPI_CSR_BITS_16_BIT_Val             (8U)                                           /**< (SPI_CSR) 16 bits for transfer  */
#define SPI_CSR_BITS_8_BIT                    (SPI_CSR_BITS_8_BIT_Val << SPI_CSR_BITS_Pos)   /**< (SPI_CSR) 8 bits for transfer Position  */
#define SPI_CSR_BITS_9_BIT                    (SPI_CSR_BITS_9_BIT_Val << SPI_CSR_BITS_Pos)   /**< (SPI_CSR) 9 bits for transfer Position  */
#define SPI_CSR_BITS_10_BIT                   (SPI_CSR_BITS_10_BIT_Val << SPI_CSR_BITS_Pos)  /**< (SPI_CSR) 10 bits for transfer Position  */
#define SPI_CSR_BITS_11_BIT                   (SPI_CSR_BITS_11_BIT_Val << SPI_CSR_BITS_Pos)  /**< (SPI_CSR) 11 bits for transfer Position  */
#define SPI_CSR_BITS_12_BIT                   (SPI_CSR_BITS_12_BIT_Val << SPI_CSR_BITS_Pos)  /**< (SPI_CSR) 12 bits for transfer Position  */
#define SPI_CSR_BITS_13_BIT                   (SPI_CSR_BITS_13_BIT_Val << SPI_CSR_BITS_Pos)  /**< (SPI_CSR) 13 bits for transfer Position  */
#define SPI_CSR_BITS_14_BIT                   (SPI_CSR_BITS_14_BIT_Val << SPI_CSR_BITS_Pos)  /**< (SPI_CSR) 14 bits for transfer Position  */
#define SPI_CSR_BITS_15_BIT                   (SPI_CSR_BITS_15_BIT_Val << SPI_CSR_BITS_Pos)  /**< (SPI_CSR) 15 bits for transfer Position  */
#define SPI_CSR_BITS_16_BIT                   (SPI_CSR_BITS_16_BIT_Val << SPI_CSR_BITS_Pos)  /**< (SPI_CSR) 16 bits for transfer Position  */
#define SPI_CSR_SCBR_Pos                      (8U)                                           /**< (SPI_CSR) Serial Clock Bit Rate Position */
#define SPI_CSR_SCBR_Msk                      (0xFFU << SPI_CSR_SCBR_Pos)                    /**< (SPI_CSR) Serial Clock Bit Rate Mask */
#define SPI_CSR_SCBR(value)                   (SPI_CSR_SCBR_Msk & ((value) << SPI_CSR_SCBR_Pos))
#define SPI_CSR_DLYBS_Pos                     (16U)                                          /**< (SPI_CSR) Delay Before SPCK Position */
#define SPI_CSR_DLYBS_Msk                     (0xFFU << SPI_CSR_DLYBS_Pos)                   /**< (SPI_CSR) Delay Before SPCK Mask */
#define SPI_CSR_DLYBS(value)                  (SPI_CSR_DLYBS_Msk & ((value) << SPI_CSR_DLYBS_Pos))
#define SPI_CSR_DLYBCT_Pos                    (24U)                                          /**< (SPI_CSR) Delay Between Consecutive Transfers Position */
#define SPI_CSR_DLYBCT_Msk                    (0xFFU << SPI_CSR_DLYBCT_Pos)                  /**< (SPI_CSR) Delay Between Consecutive Transfers Mask */
#define SPI_CSR_DLYBCT(value)                 (SPI_CSR_DLYBCT_Msk & ((value) << SPI_CSR_DLYBCT_Pos))
#define SPI_CSR_Msk                           (0xFFFFFFFFUL)                                 /**< (SPI_CSR) Register Mask  */

/* -------- SPI_WPMR : (SPI Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define SPI_WPMR_WPEN_Pos                     (0U)                                           /**< (SPI_WPMR) Write Protection Enable Position */
#define SPI_WPMR_WPEN_Msk                     (0x1U << SPI_WPMR_WPEN_Pos)                    /**< (SPI_WPMR) Write Protection Enable Mask */
#define SPI_WPMR_WPEN(value)                  (SPI_WPMR_WPEN_Msk & ((value) << SPI_WPMR_WPEN_Pos))
#define SPI_WPMR_WPKEY_Pos                    (8U)                                           /**< (SPI_WPMR) Write Protection Key Position */
#define SPI_WPMR_WPKEY_Msk                    (0xFFFFFFU << SPI_WPMR_WPKEY_Pos)              /**< (SPI_WPMR) Write Protection Key Mask */
#define SPI_WPMR_WPKEY(value)                 (SPI_WPMR_WPKEY_Msk & ((value) << SPI_WPMR_WPKEY_Pos))
#define   SPI_WPMR_WPKEY_PASSWD_Val           (5460041U)                                     /**< (SPI_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0.  */
#define SPI_WPMR_WPKEY_PASSWD                 (SPI_WPMR_WPKEY_PASSWD_Val << SPI_WPMR_WPKEY_Pos) /**< (SPI_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0. Position  */
#define SPI_WPMR_Msk                          (0xFFFFFF01UL)                                 /**< (SPI_WPMR) Register Mask  */

/* -------- SPI_WPSR : (SPI Offset: 0xE8) (R/  32) Write Protection Status Register -------- */
#define SPI_WPSR_WPVS_Pos                     (0U)                                           /**< (SPI_WPSR) Write Protection Violation Status Position */
#define SPI_WPSR_WPVS_Msk                     (0x1U << SPI_WPSR_WPVS_Pos)                    /**< (SPI_WPSR) Write Protection Violation Status Mask */
#define SPI_WPSR_WPVS(value)                  (SPI_WPSR_WPVS_Msk & ((value) << SPI_WPSR_WPVS_Pos))
#define SPI_WPSR_WPVSRC_Pos                   (8U)                                           /**< (SPI_WPSR) Write Protection Violation Source Position */
#define SPI_WPSR_WPVSRC_Msk                   (0xFFU << SPI_WPSR_WPVSRC_Pos)                 /**< (SPI_WPSR) Write Protection Violation Source Mask */
#define SPI_WPSR_WPVSRC(value)                (SPI_WPSR_WPVSRC_Msk & ((value) << SPI_WPSR_WPVSRC_Pos))
#define SPI_WPSR_Msk                          (0x0000FF01UL)                                 /**< (SPI_WPSR) Register Mask  */

/** \brief SPI register offsets definitions */
#define SPI_CR_OFFSET                  (0x00)         /**< (SPI_CR) Control Register Offset */
#define SPI_MR_OFFSET                  (0x04)         /**< (SPI_MR) Mode Register Offset */
#define SPI_RDR_OFFSET                 (0x08)         /**< (SPI_RDR) Receive Data Register Offset */
#define SPI_TDR_OFFSET                 (0x0C)         /**< (SPI_TDR) Transmit Data Register Offset */
#define SPI_SR_OFFSET                  (0x10)         /**< (SPI_SR) Status Register Offset */
#define SPI_IER_OFFSET                 (0x14)         /**< (SPI_IER) Interrupt Enable Register Offset */
#define SPI_IDR_OFFSET                 (0x18)         /**< (SPI_IDR) Interrupt Disable Register Offset */
#define SPI_IMR_OFFSET                 (0x1C)         /**< (SPI_IMR) Interrupt Mask Register Offset */
#define SPI_CSR_OFFSET                 (0x30)         /**< (SPI_CSR) Chip Select Register Offset */
#define SPI_WPMR_OFFSET                (0xE4)         /**< (SPI_WPMR) Write Protection Mode Register Offset */
#define SPI_WPSR_OFFSET                (0xE8)         /**< (SPI_WPSR) Write Protection Status Register Offset */

/** \brief SPI register API structure */
typedef struct
{
  __O   uint32_t                       SPI_CR;          /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       SPI_MR;          /**< Offset: 0x04 (R/W  32) Mode Register */
  __I   uint32_t                       SPI_RDR;         /**< Offset: 0x08 (R/   32) Receive Data Register */
  __O   uint32_t                       SPI_TDR;         /**< Offset: 0x0c ( /W  32) Transmit Data Register */
  __I   uint32_t                       SPI_SR;          /**< Offset: 0x10 (R/   32) Status Register */
  __O   uint32_t                       SPI_IER;         /**< Offset: 0x14 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       SPI_IDR;         /**< Offset: 0x18 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       SPI_IMR;         /**< Offset: 0x1c (R/   32) Interrupt Mask Register */
  __I   uint8_t                        Reserved1[0x10];
  __IO  uint32_t                       SPI_CSR[4];      /**< Offset: 0x30 (R/W  32) Chip Select Register */
  __I   uint8_t                        Reserved2[0xA4];
  __IO  uint32_t                       SPI_WPMR;        /**< Offset: 0xe4 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       SPI_WPSR;        /**< Offset: 0xe8 (R/   32) Write Protection Status Register */
} spi_registers_t;
/** @}  end of Serial Peripheral Interface */

#endif /* SAME_SAME70_SPI_MODULE_H */

