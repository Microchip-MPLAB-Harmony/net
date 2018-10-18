/**
 * \brief Header file for SAME/SAME70 SSC
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
#ifndef SAME_SAME70_SSC_MODULE_H
#define SAME_SAME70_SSC_MODULE_H

/** \addtogroup SAME_SAME70 Synchronous Serial Controller
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_SSC                                   */
/* ========================================================================== */

/* -------- SSC_CR : (SSC Offset: 0x00) ( /W 32) Control Register -------- */
#define SSC_CR_RXEN_Pos                       (0U)                                           /**< (SSC_CR) Receive Enable Position */
#define SSC_CR_RXEN_Msk                       (0x1U << SSC_CR_RXEN_Pos)                      /**< (SSC_CR) Receive Enable Mask */
#define SSC_CR_RXEN(value)                    (SSC_CR_RXEN_Msk & ((value) << SSC_CR_RXEN_Pos))
#define SSC_CR_RXDIS_Pos                      (1U)                                           /**< (SSC_CR) Receive Disable Position */
#define SSC_CR_RXDIS_Msk                      (0x1U << SSC_CR_RXDIS_Pos)                     /**< (SSC_CR) Receive Disable Mask */
#define SSC_CR_RXDIS(value)                   (SSC_CR_RXDIS_Msk & ((value) << SSC_CR_RXDIS_Pos))
#define SSC_CR_TXEN_Pos                       (8U)                                           /**< (SSC_CR) Transmit Enable Position */
#define SSC_CR_TXEN_Msk                       (0x1U << SSC_CR_TXEN_Pos)                      /**< (SSC_CR) Transmit Enable Mask */
#define SSC_CR_TXEN(value)                    (SSC_CR_TXEN_Msk & ((value) << SSC_CR_TXEN_Pos))
#define SSC_CR_TXDIS_Pos                      (9U)                                           /**< (SSC_CR) Transmit Disable Position */
#define SSC_CR_TXDIS_Msk                      (0x1U << SSC_CR_TXDIS_Pos)                     /**< (SSC_CR) Transmit Disable Mask */
#define SSC_CR_TXDIS(value)                   (SSC_CR_TXDIS_Msk & ((value) << SSC_CR_TXDIS_Pos))
#define SSC_CR_SWRST_Pos                      (15U)                                          /**< (SSC_CR) Software Reset Position */
#define SSC_CR_SWRST_Msk                      (0x1U << SSC_CR_SWRST_Pos)                     /**< (SSC_CR) Software Reset Mask */
#define SSC_CR_SWRST(value)                   (SSC_CR_SWRST_Msk & ((value) << SSC_CR_SWRST_Pos))
#define SSC_CR_Msk                            (0x00008303UL)                                 /**< (SSC_CR) Register Mask  */

/* -------- SSC_CMR : (SSC Offset: 0x04) (R/W 32) Clock Mode Register -------- */
#define SSC_CMR_DIV_Pos                       (0U)                                           /**< (SSC_CMR) Clock Divider Position */
#define SSC_CMR_DIV_Msk                       (0xFFFU << SSC_CMR_DIV_Pos)                    /**< (SSC_CMR) Clock Divider Mask */
#define SSC_CMR_DIV(value)                    (SSC_CMR_DIV_Msk & ((value) << SSC_CMR_DIV_Pos))
#define SSC_CMR_Msk                           (0x00000FFFUL)                                 /**< (SSC_CMR) Register Mask  */

/* -------- SSC_RCMR : (SSC Offset: 0x10) (R/W 32) Receive Clock Mode Register -------- */
#define SSC_RCMR_CKS_Pos                      (0U)                                           /**< (SSC_RCMR) Receive Clock Selection Position */
#define SSC_RCMR_CKS_Msk                      (0x3U << SSC_RCMR_CKS_Pos)                     /**< (SSC_RCMR) Receive Clock Selection Mask */
#define SSC_RCMR_CKS(value)                   (SSC_RCMR_CKS_Msk & ((value) << SSC_RCMR_CKS_Pos))
#define   SSC_RCMR_CKS_MCK_Val                (0U)                                           /**< (SSC_RCMR) Divided Clock  */
#define   SSC_RCMR_CKS_TK_Val                 (1U)                                           /**< (SSC_RCMR) TK Clock signal  */
#define   SSC_RCMR_CKS_RK_Val                 (2U)                                           /**< (SSC_RCMR) RK pin  */
#define SSC_RCMR_CKS_MCK                      (SSC_RCMR_CKS_MCK_Val << SSC_RCMR_CKS_Pos)     /**< (SSC_RCMR) Divided Clock Position  */
#define SSC_RCMR_CKS_TK                       (SSC_RCMR_CKS_TK_Val << SSC_RCMR_CKS_Pos)      /**< (SSC_RCMR) TK Clock signal Position  */
#define SSC_RCMR_CKS_RK                       (SSC_RCMR_CKS_RK_Val << SSC_RCMR_CKS_Pos)      /**< (SSC_RCMR) RK pin Position  */
#define SSC_RCMR_CKO_Pos                      (2U)                                           /**< (SSC_RCMR) Receive Clock Output Mode Selection Position */
#define SSC_RCMR_CKO_Msk                      (0x7U << SSC_RCMR_CKO_Pos)                     /**< (SSC_RCMR) Receive Clock Output Mode Selection Mask */
#define SSC_RCMR_CKO(value)                   (SSC_RCMR_CKO_Msk & ((value) << SSC_RCMR_CKO_Pos))
#define   SSC_RCMR_CKO_NONE_Val               (0U)                                           /**< (SSC_RCMR) None, RK pin is an input  */
#define   SSC_RCMR_CKO_CONTINUOUS_Val         (1U)                                           /**< (SSC_RCMR) Continuous Receive Clock, RK pin is an output  */
#define   SSC_RCMR_CKO_TRANSFER_Val           (2U)                                           /**< (SSC_RCMR) Receive Clock only during data transfers, RK pin is an output  */
#define SSC_RCMR_CKO_NONE                     (SSC_RCMR_CKO_NONE_Val << SSC_RCMR_CKO_Pos)    /**< (SSC_RCMR) None, RK pin is an input Position  */
#define SSC_RCMR_CKO_CONTINUOUS               (SSC_RCMR_CKO_CONTINUOUS_Val << SSC_RCMR_CKO_Pos) /**< (SSC_RCMR) Continuous Receive Clock, RK pin is an output Position  */
#define SSC_RCMR_CKO_TRANSFER                 (SSC_RCMR_CKO_TRANSFER_Val << SSC_RCMR_CKO_Pos) /**< (SSC_RCMR) Receive Clock only during data transfers, RK pin is an output Position  */
#define SSC_RCMR_CKI_Pos                      (5U)                                           /**< (SSC_RCMR) Receive Clock Inversion Position */
#define SSC_RCMR_CKI_Msk                      (0x1U << SSC_RCMR_CKI_Pos)                     /**< (SSC_RCMR) Receive Clock Inversion Mask */
#define SSC_RCMR_CKI(value)                   (SSC_RCMR_CKI_Msk & ((value) << SSC_RCMR_CKI_Pos))
#define SSC_RCMR_CKG_Pos                      (6U)                                           /**< (SSC_RCMR) Receive Clock Gating Selection Position */
#define SSC_RCMR_CKG_Msk                      (0x3U << SSC_RCMR_CKG_Pos)                     /**< (SSC_RCMR) Receive Clock Gating Selection Mask */
#define SSC_RCMR_CKG(value)                   (SSC_RCMR_CKG_Msk & ((value) << SSC_RCMR_CKG_Pos))
#define   SSC_RCMR_CKG_CONTINUOUS_Val         (0U)                                           /**< (SSC_RCMR) None  */
#define   SSC_RCMR_CKG_EN_RF_LOW_Val          (1U)                                           /**< (SSC_RCMR) Receive Clock enabled only if RF Low  */
#define   SSC_RCMR_CKG_EN_RF_HIGH_Val         (2U)                                           /**< (SSC_RCMR) Receive Clock enabled only if RF High  */
#define SSC_RCMR_CKG_CONTINUOUS               (SSC_RCMR_CKG_CONTINUOUS_Val << SSC_RCMR_CKG_Pos) /**< (SSC_RCMR) None Position  */
#define SSC_RCMR_CKG_EN_RF_LOW                (SSC_RCMR_CKG_EN_RF_LOW_Val << SSC_RCMR_CKG_Pos) /**< (SSC_RCMR) Receive Clock enabled only if RF Low Position  */
#define SSC_RCMR_CKG_EN_RF_HIGH               (SSC_RCMR_CKG_EN_RF_HIGH_Val << SSC_RCMR_CKG_Pos) /**< (SSC_RCMR) Receive Clock enabled only if RF High Position  */
#define SSC_RCMR_START_Pos                    (8U)                                           /**< (SSC_RCMR) Receive Start Selection Position */
#define SSC_RCMR_START_Msk                    (0xFU << SSC_RCMR_START_Pos)                   /**< (SSC_RCMR) Receive Start Selection Mask */
#define SSC_RCMR_START(value)                 (SSC_RCMR_START_Msk & ((value) << SSC_RCMR_START_Pos))
#define   SSC_RCMR_START_CONTINUOUS_Val       (0U)                                           /**< (SSC_RCMR) Continuous, as soon as the receiver is enabled, and immediately after the end of transfer of the previous data.  */
#define   SSC_RCMR_START_TRANSMIT_Val         (1U)                                           /**< (SSC_RCMR) Transmit start  */
#define   SSC_RCMR_START_RF_LOW_Val           (2U)                                           /**< (SSC_RCMR) Detection of a low level on RF signal  */
#define   SSC_RCMR_START_RF_HIGH_Val          (3U)                                           /**< (SSC_RCMR) Detection of a high level on RF signal  */
#define   SSC_RCMR_START_RF_FALLING_Val       (4U)                                           /**< (SSC_RCMR) Detection of a falling edge on RF signal  */
#define   SSC_RCMR_START_RF_RISING_Val        (5U)                                           /**< (SSC_RCMR) Detection of a rising edge on RF signal  */
#define   SSC_RCMR_START_RF_LEVEL_Val         (6U)                                           /**< (SSC_RCMR) Detection of any level change on RF signal  */
#define   SSC_RCMR_START_RF_EDGE_Val          (7U)                                           /**< (SSC_RCMR) Detection of any edge on RF signal  */
#define   SSC_RCMR_START_CMP_0_Val            (8U)                                           /**< (SSC_RCMR) Compare 0  */
#define SSC_RCMR_START_CONTINUOUS             (SSC_RCMR_START_CONTINUOUS_Val << SSC_RCMR_START_Pos) /**< (SSC_RCMR) Continuous, as soon as the receiver is enabled, and immediately after the end of transfer of the previous data. Position  */
#define SSC_RCMR_START_TRANSMIT               (SSC_RCMR_START_TRANSMIT_Val << SSC_RCMR_START_Pos) /**< (SSC_RCMR) Transmit start Position  */
#define SSC_RCMR_START_RF_LOW                 (SSC_RCMR_START_RF_LOW_Val << SSC_RCMR_START_Pos) /**< (SSC_RCMR) Detection of a low level on RF signal Position  */
#define SSC_RCMR_START_RF_HIGH                (SSC_RCMR_START_RF_HIGH_Val << SSC_RCMR_START_Pos) /**< (SSC_RCMR) Detection of a high level on RF signal Position  */
#define SSC_RCMR_START_RF_FALLING             (SSC_RCMR_START_RF_FALLING_Val << SSC_RCMR_START_Pos) /**< (SSC_RCMR) Detection of a falling edge on RF signal Position  */
#define SSC_RCMR_START_RF_RISING              (SSC_RCMR_START_RF_RISING_Val << SSC_RCMR_START_Pos) /**< (SSC_RCMR) Detection of a rising edge on RF signal Position  */
#define SSC_RCMR_START_RF_LEVEL               (SSC_RCMR_START_RF_LEVEL_Val << SSC_RCMR_START_Pos) /**< (SSC_RCMR) Detection of any level change on RF signal Position  */
#define SSC_RCMR_START_RF_EDGE                (SSC_RCMR_START_RF_EDGE_Val << SSC_RCMR_START_Pos) /**< (SSC_RCMR) Detection of any edge on RF signal Position  */
#define SSC_RCMR_START_CMP_0                  (SSC_RCMR_START_CMP_0_Val << SSC_RCMR_START_Pos) /**< (SSC_RCMR) Compare 0 Position  */
#define SSC_RCMR_STOP_Pos                     (12U)                                          /**< (SSC_RCMR) Receive Stop Selection Position */
#define SSC_RCMR_STOP_Msk                     (0x1U << SSC_RCMR_STOP_Pos)                    /**< (SSC_RCMR) Receive Stop Selection Mask */
#define SSC_RCMR_STOP(value)                  (SSC_RCMR_STOP_Msk & ((value) << SSC_RCMR_STOP_Pos))
#define SSC_RCMR_STTDLY_Pos                   (16U)                                          /**< (SSC_RCMR) Receive Start Delay Position */
#define SSC_RCMR_STTDLY_Msk                   (0xFFU << SSC_RCMR_STTDLY_Pos)                 /**< (SSC_RCMR) Receive Start Delay Mask */
#define SSC_RCMR_STTDLY(value)                (SSC_RCMR_STTDLY_Msk & ((value) << SSC_RCMR_STTDLY_Pos))
#define SSC_RCMR_PERIOD_Pos                   (24U)                                          /**< (SSC_RCMR) Receive Period Divider Selection Position */
#define SSC_RCMR_PERIOD_Msk                   (0xFFU << SSC_RCMR_PERIOD_Pos)                 /**< (SSC_RCMR) Receive Period Divider Selection Mask */
#define SSC_RCMR_PERIOD(value)                (SSC_RCMR_PERIOD_Msk & ((value) << SSC_RCMR_PERIOD_Pos))
#define SSC_RCMR_Msk                          (0xFFFF1FFFUL)                                 /**< (SSC_RCMR) Register Mask  */

/* -------- SSC_RFMR : (SSC Offset: 0x14) (R/W 32) Receive Frame Mode Register -------- */
#define SSC_RFMR_DATLEN_Pos                   (0U)                                           /**< (SSC_RFMR) Data Length Position */
#define SSC_RFMR_DATLEN_Msk                   (0x1FU << SSC_RFMR_DATLEN_Pos)                 /**< (SSC_RFMR) Data Length Mask */
#define SSC_RFMR_DATLEN(value)                (SSC_RFMR_DATLEN_Msk & ((value) << SSC_RFMR_DATLEN_Pos))
#define SSC_RFMR_LOOP_Pos                     (5U)                                           /**< (SSC_RFMR) Loop Mode Position */
#define SSC_RFMR_LOOP_Msk                     (0x1U << SSC_RFMR_LOOP_Pos)                    /**< (SSC_RFMR) Loop Mode Mask */
#define SSC_RFMR_LOOP(value)                  (SSC_RFMR_LOOP_Msk & ((value) << SSC_RFMR_LOOP_Pos))
#define SSC_RFMR_MSBF_Pos                     (7U)                                           /**< (SSC_RFMR) Most Significant Bit First Position */
#define SSC_RFMR_MSBF_Msk                     (0x1U << SSC_RFMR_MSBF_Pos)                    /**< (SSC_RFMR) Most Significant Bit First Mask */
#define SSC_RFMR_MSBF(value)                  (SSC_RFMR_MSBF_Msk & ((value) << SSC_RFMR_MSBF_Pos))
#define SSC_RFMR_DATNB_Pos                    (8U)                                           /**< (SSC_RFMR) Data Number per Frame Position */
#define SSC_RFMR_DATNB_Msk                    (0xFU << SSC_RFMR_DATNB_Pos)                   /**< (SSC_RFMR) Data Number per Frame Mask */
#define SSC_RFMR_DATNB(value)                 (SSC_RFMR_DATNB_Msk & ((value) << SSC_RFMR_DATNB_Pos))
#define SSC_RFMR_FSLEN_Pos                    (16U)                                          /**< (SSC_RFMR) Receive Frame Sync Length Position */
#define SSC_RFMR_FSLEN_Msk                    (0xFU << SSC_RFMR_FSLEN_Pos)                   /**< (SSC_RFMR) Receive Frame Sync Length Mask */
#define SSC_RFMR_FSLEN(value)                 (SSC_RFMR_FSLEN_Msk & ((value) << SSC_RFMR_FSLEN_Pos))
#define SSC_RFMR_FSOS_Pos                     (20U)                                          /**< (SSC_RFMR) Receive Frame Sync Output Selection Position */
#define SSC_RFMR_FSOS_Msk                     (0x7U << SSC_RFMR_FSOS_Pos)                    /**< (SSC_RFMR) Receive Frame Sync Output Selection Mask */
#define SSC_RFMR_FSOS(value)                  (SSC_RFMR_FSOS_Msk & ((value) << SSC_RFMR_FSOS_Pos))
#define   SSC_RFMR_FSOS_NONE_Val              (0U)                                           /**< (SSC_RFMR) None, RF pin is an input  */
#define   SSC_RFMR_FSOS_NEGATIVE_Val          (1U)                                           /**< (SSC_RFMR) Negative Pulse, RF pin is an output  */
#define   SSC_RFMR_FSOS_POSITIVE_Val          (2U)                                           /**< (SSC_RFMR) Positive Pulse, RF pin is an output  */
#define   SSC_RFMR_FSOS_LOW_Val               (3U)                                           /**< (SSC_RFMR) Driven Low during data transfer, RF pin is an output  */
#define   SSC_RFMR_FSOS_HIGH_Val              (4U)                                           /**< (SSC_RFMR) Driven High during data transfer, RF pin is an output  */
#define   SSC_RFMR_FSOS_TOGGLING_Val          (5U)                                           /**< (SSC_RFMR) Toggling at each start of data transfer, RF pin is an output  */
#define SSC_RFMR_FSOS_NONE                    (SSC_RFMR_FSOS_NONE_Val << SSC_RFMR_FSOS_Pos)  /**< (SSC_RFMR) None, RF pin is an input Position  */
#define SSC_RFMR_FSOS_NEGATIVE                (SSC_RFMR_FSOS_NEGATIVE_Val << SSC_RFMR_FSOS_Pos) /**< (SSC_RFMR) Negative Pulse, RF pin is an output Position  */
#define SSC_RFMR_FSOS_POSITIVE                (SSC_RFMR_FSOS_POSITIVE_Val << SSC_RFMR_FSOS_Pos) /**< (SSC_RFMR) Positive Pulse, RF pin is an output Position  */
#define SSC_RFMR_FSOS_LOW                     (SSC_RFMR_FSOS_LOW_Val << SSC_RFMR_FSOS_Pos)   /**< (SSC_RFMR) Driven Low during data transfer, RF pin is an output Position  */
#define SSC_RFMR_FSOS_HIGH                    (SSC_RFMR_FSOS_HIGH_Val << SSC_RFMR_FSOS_Pos)  /**< (SSC_RFMR) Driven High during data transfer, RF pin is an output Position  */
#define SSC_RFMR_FSOS_TOGGLING                (SSC_RFMR_FSOS_TOGGLING_Val << SSC_RFMR_FSOS_Pos) /**< (SSC_RFMR) Toggling at each start of data transfer, RF pin is an output Position  */
#define SSC_RFMR_FSEDGE_Pos                   (24U)                                          /**< (SSC_RFMR) Frame Sync Edge Detection Position */
#define SSC_RFMR_FSEDGE_Msk                   (0x1U << SSC_RFMR_FSEDGE_Pos)                  /**< (SSC_RFMR) Frame Sync Edge Detection Mask */
#define SSC_RFMR_FSEDGE(value)                (SSC_RFMR_FSEDGE_Msk & ((value) << SSC_RFMR_FSEDGE_Pos))
#define   SSC_RFMR_FSEDGE_POSITIVE_Val        (0U)                                           /**< (SSC_RFMR) Positive Edge Detection  */
#define   SSC_RFMR_FSEDGE_NEGATIVE_Val        (1U)                                           /**< (SSC_RFMR) Negative Edge Detection  */
#define SSC_RFMR_FSEDGE_POSITIVE              (SSC_RFMR_FSEDGE_POSITIVE_Val << SSC_RFMR_FSEDGE_Pos) /**< (SSC_RFMR) Positive Edge Detection Position  */
#define SSC_RFMR_FSEDGE_NEGATIVE              (SSC_RFMR_FSEDGE_NEGATIVE_Val << SSC_RFMR_FSEDGE_Pos) /**< (SSC_RFMR) Negative Edge Detection Position  */
#define SSC_RFMR_FSLEN_EXT_Pos                (28U)                                          /**< (SSC_RFMR) FSLEN Field Extension Position */
#define SSC_RFMR_FSLEN_EXT_Msk                (0xFU << SSC_RFMR_FSLEN_EXT_Pos)               /**< (SSC_RFMR) FSLEN Field Extension Mask */
#define SSC_RFMR_FSLEN_EXT(value)             (SSC_RFMR_FSLEN_EXT_Msk & ((value) << SSC_RFMR_FSLEN_EXT_Pos))
#define SSC_RFMR_Msk                          (0xF17F0FBFUL)                                 /**< (SSC_RFMR) Register Mask  */

/* -------- SSC_TCMR : (SSC Offset: 0x18) (R/W 32) Transmit Clock Mode Register -------- */
#define SSC_TCMR_CKS_Pos                      (0U)                                           /**< (SSC_TCMR) Transmit Clock Selection Position */
#define SSC_TCMR_CKS_Msk                      (0x3U << SSC_TCMR_CKS_Pos)                     /**< (SSC_TCMR) Transmit Clock Selection Mask */
#define SSC_TCMR_CKS(value)                   (SSC_TCMR_CKS_Msk & ((value) << SSC_TCMR_CKS_Pos))
#define   SSC_TCMR_CKS_MCK_Val                (0U)                                           /**< (SSC_TCMR) Divided Clock  */
#define   SSC_TCMR_CKS_RK_Val                 (1U)                                           /**< (SSC_TCMR) RK Clock signal  */
#define   SSC_TCMR_CKS_TK_Val                 (2U)                                           /**< (SSC_TCMR) TK pin  */
#define SSC_TCMR_CKS_MCK                      (SSC_TCMR_CKS_MCK_Val << SSC_TCMR_CKS_Pos)     /**< (SSC_TCMR) Divided Clock Position  */
#define SSC_TCMR_CKS_RK                       (SSC_TCMR_CKS_RK_Val << SSC_TCMR_CKS_Pos)      /**< (SSC_TCMR) RK Clock signal Position  */
#define SSC_TCMR_CKS_TK                       (SSC_TCMR_CKS_TK_Val << SSC_TCMR_CKS_Pos)      /**< (SSC_TCMR) TK pin Position  */
#define SSC_TCMR_CKO_Pos                      (2U)                                           /**< (SSC_TCMR) Transmit Clock Output Mode Selection Position */
#define SSC_TCMR_CKO_Msk                      (0x7U << SSC_TCMR_CKO_Pos)                     /**< (SSC_TCMR) Transmit Clock Output Mode Selection Mask */
#define SSC_TCMR_CKO(value)                   (SSC_TCMR_CKO_Msk & ((value) << SSC_TCMR_CKO_Pos))
#define   SSC_TCMR_CKO_NONE_Val               (0U)                                           /**< (SSC_TCMR) None, TK pin is an input  */
#define   SSC_TCMR_CKO_CONTINUOUS_Val         (1U)                                           /**< (SSC_TCMR) Continuous Transmit Clock, TK pin is an output  */
#define   SSC_TCMR_CKO_TRANSFER_Val           (2U)                                           /**< (SSC_TCMR) Transmit Clock only during data transfers, TK pin is an output  */
#define SSC_TCMR_CKO_NONE                     (SSC_TCMR_CKO_NONE_Val << SSC_TCMR_CKO_Pos)    /**< (SSC_TCMR) None, TK pin is an input Position  */
#define SSC_TCMR_CKO_CONTINUOUS               (SSC_TCMR_CKO_CONTINUOUS_Val << SSC_TCMR_CKO_Pos) /**< (SSC_TCMR) Continuous Transmit Clock, TK pin is an output Position  */
#define SSC_TCMR_CKO_TRANSFER                 (SSC_TCMR_CKO_TRANSFER_Val << SSC_TCMR_CKO_Pos) /**< (SSC_TCMR) Transmit Clock only during data transfers, TK pin is an output Position  */
#define SSC_TCMR_CKI_Pos                      (5U)                                           /**< (SSC_TCMR) Transmit Clock Inversion Position */
#define SSC_TCMR_CKI_Msk                      (0x1U << SSC_TCMR_CKI_Pos)                     /**< (SSC_TCMR) Transmit Clock Inversion Mask */
#define SSC_TCMR_CKI(value)                   (SSC_TCMR_CKI_Msk & ((value) << SSC_TCMR_CKI_Pos))
#define SSC_TCMR_CKG_Pos                      (6U)                                           /**< (SSC_TCMR) Transmit Clock Gating Selection Position */
#define SSC_TCMR_CKG_Msk                      (0x3U << SSC_TCMR_CKG_Pos)                     /**< (SSC_TCMR) Transmit Clock Gating Selection Mask */
#define SSC_TCMR_CKG(value)                   (SSC_TCMR_CKG_Msk & ((value) << SSC_TCMR_CKG_Pos))
#define   SSC_TCMR_CKG_CONTINUOUS_Val         (0U)                                           /**< (SSC_TCMR) None  */
#define   SSC_TCMR_CKG_EN_TF_LOW_Val          (1U)                                           /**< (SSC_TCMR) Transmit Clock enabled only if TF Low  */
#define   SSC_TCMR_CKG_EN_TF_HIGH_Val         (2U)                                           /**< (SSC_TCMR) Transmit Clock enabled only if TF High  */
#define SSC_TCMR_CKG_CONTINUOUS               (SSC_TCMR_CKG_CONTINUOUS_Val << SSC_TCMR_CKG_Pos) /**< (SSC_TCMR) None Position  */
#define SSC_TCMR_CKG_EN_TF_LOW                (SSC_TCMR_CKG_EN_TF_LOW_Val << SSC_TCMR_CKG_Pos) /**< (SSC_TCMR) Transmit Clock enabled only if TF Low Position  */
#define SSC_TCMR_CKG_EN_TF_HIGH               (SSC_TCMR_CKG_EN_TF_HIGH_Val << SSC_TCMR_CKG_Pos) /**< (SSC_TCMR) Transmit Clock enabled only if TF High Position  */
#define SSC_TCMR_START_Pos                    (8U)                                           /**< (SSC_TCMR) Transmit Start Selection Position */
#define SSC_TCMR_START_Msk                    (0xFU << SSC_TCMR_START_Pos)                   /**< (SSC_TCMR) Transmit Start Selection Mask */
#define SSC_TCMR_START(value)                 (SSC_TCMR_START_Msk & ((value) << SSC_TCMR_START_Pos))
#define   SSC_TCMR_START_CONTINUOUS_Val       (0U)                                           /**< (SSC_TCMR) Continuous, as soon as a word is written in the SSC_THR (if Transmit is enabled), and immediately after the end of transfer of the previous data  */
#define   SSC_TCMR_START_RECEIVE_Val          (1U)                                           /**< (SSC_TCMR) Receive start  */
#define   SSC_TCMR_START_TF_LOW_Val           (2U)                                           /**< (SSC_TCMR) Detection of a low level on TF signal  */
#define   SSC_TCMR_START_TF_HIGH_Val          (3U)                                           /**< (SSC_TCMR) Detection of a high level on TF signal  */
#define   SSC_TCMR_START_TF_FALLING_Val       (4U)                                           /**< (SSC_TCMR) Detection of a falling edge on TF signal  */
#define   SSC_TCMR_START_TF_RISING_Val        (5U)                                           /**< (SSC_TCMR) Detection of a rising edge on TF signal  */
#define   SSC_TCMR_START_TF_LEVEL_Val         (6U)                                           /**< (SSC_TCMR) Detection of any level change on TF signal  */
#define   SSC_TCMR_START_TF_EDGE_Val          (7U)                                           /**< (SSC_TCMR) Detection of any edge on TF signal  */
#define SSC_TCMR_START_CONTINUOUS             (SSC_TCMR_START_CONTINUOUS_Val << SSC_TCMR_START_Pos) /**< (SSC_TCMR) Continuous, as soon as a word is written in the SSC_THR (if Transmit is enabled), and immediately after the end of transfer of the previous data Position  */
#define SSC_TCMR_START_RECEIVE                (SSC_TCMR_START_RECEIVE_Val << SSC_TCMR_START_Pos) /**< (SSC_TCMR) Receive start Position  */
#define SSC_TCMR_START_TF_LOW                 (SSC_TCMR_START_TF_LOW_Val << SSC_TCMR_START_Pos) /**< (SSC_TCMR) Detection of a low level on TF signal Position  */
#define SSC_TCMR_START_TF_HIGH                (SSC_TCMR_START_TF_HIGH_Val << SSC_TCMR_START_Pos) /**< (SSC_TCMR) Detection of a high level on TF signal Position  */
#define SSC_TCMR_START_TF_FALLING             (SSC_TCMR_START_TF_FALLING_Val << SSC_TCMR_START_Pos) /**< (SSC_TCMR) Detection of a falling edge on TF signal Position  */
#define SSC_TCMR_START_TF_RISING              (SSC_TCMR_START_TF_RISING_Val << SSC_TCMR_START_Pos) /**< (SSC_TCMR) Detection of a rising edge on TF signal Position  */
#define SSC_TCMR_START_TF_LEVEL               (SSC_TCMR_START_TF_LEVEL_Val << SSC_TCMR_START_Pos) /**< (SSC_TCMR) Detection of any level change on TF signal Position  */
#define SSC_TCMR_START_TF_EDGE                (SSC_TCMR_START_TF_EDGE_Val << SSC_TCMR_START_Pos) /**< (SSC_TCMR) Detection of any edge on TF signal Position  */
#define SSC_TCMR_STTDLY_Pos                   (16U)                                          /**< (SSC_TCMR) Transmit Start Delay Position */
#define SSC_TCMR_STTDLY_Msk                   (0xFFU << SSC_TCMR_STTDLY_Pos)                 /**< (SSC_TCMR) Transmit Start Delay Mask */
#define SSC_TCMR_STTDLY(value)                (SSC_TCMR_STTDLY_Msk & ((value) << SSC_TCMR_STTDLY_Pos))
#define SSC_TCMR_PERIOD_Pos                   (24U)                                          /**< (SSC_TCMR) Transmit Period Divider Selection Position */
#define SSC_TCMR_PERIOD_Msk                   (0xFFU << SSC_TCMR_PERIOD_Pos)                 /**< (SSC_TCMR) Transmit Period Divider Selection Mask */
#define SSC_TCMR_PERIOD(value)                (SSC_TCMR_PERIOD_Msk & ((value) << SSC_TCMR_PERIOD_Pos))
#define SSC_TCMR_Msk                          (0xFFFF0FFFUL)                                 /**< (SSC_TCMR) Register Mask  */

/* -------- SSC_TFMR : (SSC Offset: 0x1C) (R/W 32) Transmit Frame Mode Register -------- */
#define SSC_TFMR_DATLEN_Pos                   (0U)                                           /**< (SSC_TFMR) Data Length Position */
#define SSC_TFMR_DATLEN_Msk                   (0x1FU << SSC_TFMR_DATLEN_Pos)                 /**< (SSC_TFMR) Data Length Mask */
#define SSC_TFMR_DATLEN(value)                (SSC_TFMR_DATLEN_Msk & ((value) << SSC_TFMR_DATLEN_Pos))
#define SSC_TFMR_DATDEF_Pos                   (5U)                                           /**< (SSC_TFMR) Data Default Value Position */
#define SSC_TFMR_DATDEF_Msk                   (0x1U << SSC_TFMR_DATDEF_Pos)                  /**< (SSC_TFMR) Data Default Value Mask */
#define SSC_TFMR_DATDEF(value)                (SSC_TFMR_DATDEF_Msk & ((value) << SSC_TFMR_DATDEF_Pos))
#define SSC_TFMR_MSBF_Pos                     (7U)                                           /**< (SSC_TFMR) Most Significant Bit First Position */
#define SSC_TFMR_MSBF_Msk                     (0x1U << SSC_TFMR_MSBF_Pos)                    /**< (SSC_TFMR) Most Significant Bit First Mask */
#define SSC_TFMR_MSBF(value)                  (SSC_TFMR_MSBF_Msk & ((value) << SSC_TFMR_MSBF_Pos))
#define SSC_TFMR_DATNB_Pos                    (8U)                                           /**< (SSC_TFMR) Data Number per Frame Position */
#define SSC_TFMR_DATNB_Msk                    (0xFU << SSC_TFMR_DATNB_Pos)                   /**< (SSC_TFMR) Data Number per Frame Mask */
#define SSC_TFMR_DATNB(value)                 (SSC_TFMR_DATNB_Msk & ((value) << SSC_TFMR_DATNB_Pos))
#define SSC_TFMR_FSLEN_Pos                    (16U)                                          /**< (SSC_TFMR) Transmit Frame Sync Length Position */
#define SSC_TFMR_FSLEN_Msk                    (0xFU << SSC_TFMR_FSLEN_Pos)                   /**< (SSC_TFMR) Transmit Frame Sync Length Mask */
#define SSC_TFMR_FSLEN(value)                 (SSC_TFMR_FSLEN_Msk & ((value) << SSC_TFMR_FSLEN_Pos))
#define SSC_TFMR_FSOS_Pos                     (20U)                                          /**< (SSC_TFMR) Transmit Frame Sync Output Selection Position */
#define SSC_TFMR_FSOS_Msk                     (0x7U << SSC_TFMR_FSOS_Pos)                    /**< (SSC_TFMR) Transmit Frame Sync Output Selection Mask */
#define SSC_TFMR_FSOS(value)                  (SSC_TFMR_FSOS_Msk & ((value) << SSC_TFMR_FSOS_Pos))
#define   SSC_TFMR_FSOS_NONE_Val              (0U)                                           /**< (SSC_TFMR) None, TF pin is an input  */
#define   SSC_TFMR_FSOS_NEGATIVE_Val          (1U)                                           /**< (SSC_TFMR) Negative Pulse, TF pin is an output  */
#define   SSC_TFMR_FSOS_POSITIVE_Val          (2U)                                           /**< (SSC_TFMR) Positive Pulse, TF pin is an output  */
#define   SSC_TFMR_FSOS_LOW_Val               (3U)                                           /**< (SSC_TFMR) Driven Low during data transfer  */
#define   SSC_TFMR_FSOS_HIGH_Val              (4U)                                           /**< (SSC_TFMR) Driven High during data transfer  */
#define   SSC_TFMR_FSOS_TOGGLING_Val          (5U)                                           /**< (SSC_TFMR) Toggling at each start of data transfer  */
#define SSC_TFMR_FSOS_NONE                    (SSC_TFMR_FSOS_NONE_Val << SSC_TFMR_FSOS_Pos)  /**< (SSC_TFMR) None, TF pin is an input Position  */
#define SSC_TFMR_FSOS_NEGATIVE                (SSC_TFMR_FSOS_NEGATIVE_Val << SSC_TFMR_FSOS_Pos) /**< (SSC_TFMR) Negative Pulse, TF pin is an output Position  */
#define SSC_TFMR_FSOS_POSITIVE                (SSC_TFMR_FSOS_POSITIVE_Val << SSC_TFMR_FSOS_Pos) /**< (SSC_TFMR) Positive Pulse, TF pin is an output Position  */
#define SSC_TFMR_FSOS_LOW                     (SSC_TFMR_FSOS_LOW_Val << SSC_TFMR_FSOS_Pos)   /**< (SSC_TFMR) Driven Low during data transfer Position  */
#define SSC_TFMR_FSOS_HIGH                    (SSC_TFMR_FSOS_HIGH_Val << SSC_TFMR_FSOS_Pos)  /**< (SSC_TFMR) Driven High during data transfer Position  */
#define SSC_TFMR_FSOS_TOGGLING                (SSC_TFMR_FSOS_TOGGLING_Val << SSC_TFMR_FSOS_Pos) /**< (SSC_TFMR) Toggling at each start of data transfer Position  */
#define SSC_TFMR_FSDEN_Pos                    (23U)                                          /**< (SSC_TFMR) Frame Sync Data Enable Position */
#define SSC_TFMR_FSDEN_Msk                    (0x1U << SSC_TFMR_FSDEN_Pos)                   /**< (SSC_TFMR) Frame Sync Data Enable Mask */
#define SSC_TFMR_FSDEN(value)                 (SSC_TFMR_FSDEN_Msk & ((value) << SSC_TFMR_FSDEN_Pos))
#define SSC_TFMR_FSEDGE_Pos                   (24U)                                          /**< (SSC_TFMR) Frame Sync Edge Detection Position */
#define SSC_TFMR_FSEDGE_Msk                   (0x1U << SSC_TFMR_FSEDGE_Pos)                  /**< (SSC_TFMR) Frame Sync Edge Detection Mask */
#define SSC_TFMR_FSEDGE(value)                (SSC_TFMR_FSEDGE_Msk & ((value) << SSC_TFMR_FSEDGE_Pos))
#define   SSC_TFMR_FSEDGE_POSITIVE_Val        (0U)                                           /**< (SSC_TFMR) Positive Edge Detection  */
#define   SSC_TFMR_FSEDGE_NEGATIVE_Val        (1U)                                           /**< (SSC_TFMR) Negative Edge Detection  */
#define SSC_TFMR_FSEDGE_POSITIVE              (SSC_TFMR_FSEDGE_POSITIVE_Val << SSC_TFMR_FSEDGE_Pos) /**< (SSC_TFMR) Positive Edge Detection Position  */
#define SSC_TFMR_FSEDGE_NEGATIVE              (SSC_TFMR_FSEDGE_NEGATIVE_Val << SSC_TFMR_FSEDGE_Pos) /**< (SSC_TFMR) Negative Edge Detection Position  */
#define SSC_TFMR_FSLEN_EXT_Pos                (28U)                                          /**< (SSC_TFMR) FSLEN Field Extension Position */
#define SSC_TFMR_FSLEN_EXT_Msk                (0xFU << SSC_TFMR_FSLEN_EXT_Pos)               /**< (SSC_TFMR) FSLEN Field Extension Mask */
#define SSC_TFMR_FSLEN_EXT(value)             (SSC_TFMR_FSLEN_EXT_Msk & ((value) << SSC_TFMR_FSLEN_EXT_Pos))
#define SSC_TFMR_Msk                          (0xF1FF0FBFUL)                                 /**< (SSC_TFMR) Register Mask  */

/* -------- SSC_RHR : (SSC Offset: 0x20) (R/  32) Receive Holding Register -------- */
#define SSC_RHR_RDAT_Pos                      (0U)                                           /**< (SSC_RHR) Receive Data Position */
#define SSC_RHR_RDAT_Msk                      (0xFFFFFFFFU << SSC_RHR_RDAT_Pos)              /**< (SSC_RHR) Receive Data Mask */
#define SSC_RHR_RDAT(value)                   (SSC_RHR_RDAT_Msk & ((value) << SSC_RHR_RDAT_Pos))
#define SSC_RHR_Msk                           (0xFFFFFFFFUL)                                 /**< (SSC_RHR) Register Mask  */

/* -------- SSC_THR : (SSC Offset: 0x24) ( /W 32) Transmit Holding Register -------- */
#define SSC_THR_TDAT_Pos                      (0U)                                           /**< (SSC_THR) Transmit Data Position */
#define SSC_THR_TDAT_Msk                      (0xFFFFFFFFU << SSC_THR_TDAT_Pos)              /**< (SSC_THR) Transmit Data Mask */
#define SSC_THR_TDAT(value)                   (SSC_THR_TDAT_Msk & ((value) << SSC_THR_TDAT_Pos))
#define SSC_THR_Msk                           (0xFFFFFFFFUL)                                 /**< (SSC_THR) Register Mask  */

/* -------- SSC_RSHR : (SSC Offset: 0x30) (R/  32) Receive Sync. Holding Register -------- */
#define SSC_RSHR_RSDAT_Pos                    (0U)                                           /**< (SSC_RSHR) Receive Synchronization Data Position */
#define SSC_RSHR_RSDAT_Msk                    (0xFFFFU << SSC_RSHR_RSDAT_Pos)                /**< (SSC_RSHR) Receive Synchronization Data Mask */
#define SSC_RSHR_RSDAT(value)                 (SSC_RSHR_RSDAT_Msk & ((value) << SSC_RSHR_RSDAT_Pos))
#define SSC_RSHR_Msk                          (0x0000FFFFUL)                                 /**< (SSC_RSHR) Register Mask  */

/* -------- SSC_TSHR : (SSC Offset: 0x34) (R/W 32) Transmit Sync. Holding Register -------- */
#define SSC_TSHR_TSDAT_Pos                    (0U)                                           /**< (SSC_TSHR) Transmit Synchronization Data Position */
#define SSC_TSHR_TSDAT_Msk                    (0xFFFFU << SSC_TSHR_TSDAT_Pos)                /**< (SSC_TSHR) Transmit Synchronization Data Mask */
#define SSC_TSHR_TSDAT(value)                 (SSC_TSHR_TSDAT_Msk & ((value) << SSC_TSHR_TSDAT_Pos))
#define SSC_TSHR_Msk                          (0x0000FFFFUL)                                 /**< (SSC_TSHR) Register Mask  */

/* -------- SSC_RC0R : (SSC Offset: 0x38) (R/W 32) Receive Compare 0 Register -------- */
#define SSC_RC0R_CP0_Pos                      (0U)                                           /**< (SSC_RC0R) Receive Compare Data 0 Position */
#define SSC_RC0R_CP0_Msk                      (0xFFFFU << SSC_RC0R_CP0_Pos)                  /**< (SSC_RC0R) Receive Compare Data 0 Mask */
#define SSC_RC0R_CP0(value)                   (SSC_RC0R_CP0_Msk & ((value) << SSC_RC0R_CP0_Pos))
#define SSC_RC0R_Msk                          (0x0000FFFFUL)                                 /**< (SSC_RC0R) Register Mask  */

/* -------- SSC_RC1R : (SSC Offset: 0x3C) (R/W 32) Receive Compare 1 Register -------- */
#define SSC_RC1R_CP1_Pos                      (0U)                                           /**< (SSC_RC1R) Receive Compare Data 1 Position */
#define SSC_RC1R_CP1_Msk                      (0xFFFFU << SSC_RC1R_CP1_Pos)                  /**< (SSC_RC1R) Receive Compare Data 1 Mask */
#define SSC_RC1R_CP1(value)                   (SSC_RC1R_CP1_Msk & ((value) << SSC_RC1R_CP1_Pos))
#define SSC_RC1R_Msk                          (0x0000FFFFUL)                                 /**< (SSC_RC1R) Register Mask  */

/* -------- SSC_SR : (SSC Offset: 0x40) (R/  32) Status Register -------- */
#define SSC_SR_TXRDY_Pos                      (0U)                                           /**< (SSC_SR) Transmit Ready Position */
#define SSC_SR_TXRDY_Msk                      (0x1U << SSC_SR_TXRDY_Pos)                     /**< (SSC_SR) Transmit Ready Mask */
#define SSC_SR_TXRDY(value)                   (SSC_SR_TXRDY_Msk & ((value) << SSC_SR_TXRDY_Pos))
#define SSC_SR_TXEMPTY_Pos                    (1U)                                           /**< (SSC_SR) Transmit Empty Position */
#define SSC_SR_TXEMPTY_Msk                    (0x1U << SSC_SR_TXEMPTY_Pos)                   /**< (SSC_SR) Transmit Empty Mask */
#define SSC_SR_TXEMPTY(value)                 (SSC_SR_TXEMPTY_Msk & ((value) << SSC_SR_TXEMPTY_Pos))
#define SSC_SR_RXRDY_Pos                      (4U)                                           /**< (SSC_SR) Receive Ready Position */
#define SSC_SR_RXRDY_Msk                      (0x1U << SSC_SR_RXRDY_Pos)                     /**< (SSC_SR) Receive Ready Mask */
#define SSC_SR_RXRDY(value)                   (SSC_SR_RXRDY_Msk & ((value) << SSC_SR_RXRDY_Pos))
#define SSC_SR_OVRUN_Pos                      (5U)                                           /**< (SSC_SR) Receive Overrun Position */
#define SSC_SR_OVRUN_Msk                      (0x1U << SSC_SR_OVRUN_Pos)                     /**< (SSC_SR) Receive Overrun Mask */
#define SSC_SR_OVRUN(value)                   (SSC_SR_OVRUN_Msk & ((value) << SSC_SR_OVRUN_Pos))
#define SSC_SR_CP0_Pos                        (8U)                                           /**< (SSC_SR) Compare 0 Position */
#define SSC_SR_CP0_Msk                        (0x1U << SSC_SR_CP0_Pos)                       /**< (SSC_SR) Compare 0 Mask */
#define SSC_SR_CP0(value)                     (SSC_SR_CP0_Msk & ((value) << SSC_SR_CP0_Pos))
#define SSC_SR_CP1_Pos                        (9U)                                           /**< (SSC_SR) Compare 1 Position */
#define SSC_SR_CP1_Msk                        (0x1U << SSC_SR_CP1_Pos)                       /**< (SSC_SR) Compare 1 Mask */
#define SSC_SR_CP1(value)                     (SSC_SR_CP1_Msk & ((value) << SSC_SR_CP1_Pos))
#define SSC_SR_TXSYN_Pos                      (10U)                                          /**< (SSC_SR) Transmit Sync Position */
#define SSC_SR_TXSYN_Msk                      (0x1U << SSC_SR_TXSYN_Pos)                     /**< (SSC_SR) Transmit Sync Mask */
#define SSC_SR_TXSYN(value)                   (SSC_SR_TXSYN_Msk & ((value) << SSC_SR_TXSYN_Pos))
#define SSC_SR_RXSYN_Pos                      (11U)                                          /**< (SSC_SR) Receive Sync Position */
#define SSC_SR_RXSYN_Msk                      (0x1U << SSC_SR_RXSYN_Pos)                     /**< (SSC_SR) Receive Sync Mask */
#define SSC_SR_RXSYN(value)                   (SSC_SR_RXSYN_Msk & ((value) << SSC_SR_RXSYN_Pos))
#define SSC_SR_TXEN_Pos                       (16U)                                          /**< (SSC_SR) Transmit Enable Position */
#define SSC_SR_TXEN_Msk                       (0x1U << SSC_SR_TXEN_Pos)                      /**< (SSC_SR) Transmit Enable Mask */
#define SSC_SR_TXEN(value)                    (SSC_SR_TXEN_Msk & ((value) << SSC_SR_TXEN_Pos))
#define SSC_SR_RXEN_Pos                       (17U)                                          /**< (SSC_SR) Receive Enable Position */
#define SSC_SR_RXEN_Msk                       (0x1U << SSC_SR_RXEN_Pos)                      /**< (SSC_SR) Receive Enable Mask */
#define SSC_SR_RXEN(value)                    (SSC_SR_RXEN_Msk & ((value) << SSC_SR_RXEN_Pos))
#define SSC_SR_Msk                            (0x00030F33UL)                                 /**< (SSC_SR) Register Mask  */

/* -------- SSC_IER : (SSC Offset: 0x44) ( /W 32) Interrupt Enable Register -------- */
#define SSC_IER_TXRDY_Pos                     (0U)                                           /**< (SSC_IER) Transmit Ready Interrupt Enable Position */
#define SSC_IER_TXRDY_Msk                     (0x1U << SSC_IER_TXRDY_Pos)                    /**< (SSC_IER) Transmit Ready Interrupt Enable Mask */
#define SSC_IER_TXRDY(value)                  (SSC_IER_TXRDY_Msk & ((value) << SSC_IER_TXRDY_Pos))
#define SSC_IER_TXEMPTY_Pos                   (1U)                                           /**< (SSC_IER) Transmit Empty Interrupt Enable Position */
#define SSC_IER_TXEMPTY_Msk                   (0x1U << SSC_IER_TXEMPTY_Pos)                  /**< (SSC_IER) Transmit Empty Interrupt Enable Mask */
#define SSC_IER_TXEMPTY(value)                (SSC_IER_TXEMPTY_Msk & ((value) << SSC_IER_TXEMPTY_Pos))
#define SSC_IER_RXRDY_Pos                     (4U)                                           /**< (SSC_IER) Receive Ready Interrupt Enable Position */
#define SSC_IER_RXRDY_Msk                     (0x1U << SSC_IER_RXRDY_Pos)                    /**< (SSC_IER) Receive Ready Interrupt Enable Mask */
#define SSC_IER_RXRDY(value)                  (SSC_IER_RXRDY_Msk & ((value) << SSC_IER_RXRDY_Pos))
#define SSC_IER_OVRUN_Pos                     (5U)                                           /**< (SSC_IER) Receive Overrun Interrupt Enable Position */
#define SSC_IER_OVRUN_Msk                     (0x1U << SSC_IER_OVRUN_Pos)                    /**< (SSC_IER) Receive Overrun Interrupt Enable Mask */
#define SSC_IER_OVRUN(value)                  (SSC_IER_OVRUN_Msk & ((value) << SSC_IER_OVRUN_Pos))
#define SSC_IER_CP0_Pos                       (8U)                                           /**< (SSC_IER) Compare 0 Interrupt Enable Position */
#define SSC_IER_CP0_Msk                       (0x1U << SSC_IER_CP0_Pos)                      /**< (SSC_IER) Compare 0 Interrupt Enable Mask */
#define SSC_IER_CP0(value)                    (SSC_IER_CP0_Msk & ((value) << SSC_IER_CP0_Pos))
#define SSC_IER_CP1_Pos                       (9U)                                           /**< (SSC_IER) Compare 1 Interrupt Enable Position */
#define SSC_IER_CP1_Msk                       (0x1U << SSC_IER_CP1_Pos)                      /**< (SSC_IER) Compare 1 Interrupt Enable Mask */
#define SSC_IER_CP1(value)                    (SSC_IER_CP1_Msk & ((value) << SSC_IER_CP1_Pos))
#define SSC_IER_TXSYN_Pos                     (10U)                                          /**< (SSC_IER) Tx Sync Interrupt Enable Position */
#define SSC_IER_TXSYN_Msk                     (0x1U << SSC_IER_TXSYN_Pos)                    /**< (SSC_IER) Tx Sync Interrupt Enable Mask */
#define SSC_IER_TXSYN(value)                  (SSC_IER_TXSYN_Msk & ((value) << SSC_IER_TXSYN_Pos))
#define SSC_IER_RXSYN_Pos                     (11U)                                          /**< (SSC_IER) Rx Sync Interrupt Enable Position */
#define SSC_IER_RXSYN_Msk                     (0x1U << SSC_IER_RXSYN_Pos)                    /**< (SSC_IER) Rx Sync Interrupt Enable Mask */
#define SSC_IER_RXSYN(value)                  (SSC_IER_RXSYN_Msk & ((value) << SSC_IER_RXSYN_Pos))
#define SSC_IER_Msk                           (0x00000F33UL)                                 /**< (SSC_IER) Register Mask  */

/* -------- SSC_IDR : (SSC Offset: 0x48) ( /W 32) Interrupt Disable Register -------- */
#define SSC_IDR_TXRDY_Pos                     (0U)                                           /**< (SSC_IDR) Transmit Ready Interrupt Disable Position */
#define SSC_IDR_TXRDY_Msk                     (0x1U << SSC_IDR_TXRDY_Pos)                    /**< (SSC_IDR) Transmit Ready Interrupt Disable Mask */
#define SSC_IDR_TXRDY(value)                  (SSC_IDR_TXRDY_Msk & ((value) << SSC_IDR_TXRDY_Pos))
#define SSC_IDR_TXEMPTY_Pos                   (1U)                                           /**< (SSC_IDR) Transmit Empty Interrupt Disable Position */
#define SSC_IDR_TXEMPTY_Msk                   (0x1U << SSC_IDR_TXEMPTY_Pos)                  /**< (SSC_IDR) Transmit Empty Interrupt Disable Mask */
#define SSC_IDR_TXEMPTY(value)                (SSC_IDR_TXEMPTY_Msk & ((value) << SSC_IDR_TXEMPTY_Pos))
#define SSC_IDR_RXRDY_Pos                     (4U)                                           /**< (SSC_IDR) Receive Ready Interrupt Disable Position */
#define SSC_IDR_RXRDY_Msk                     (0x1U << SSC_IDR_RXRDY_Pos)                    /**< (SSC_IDR) Receive Ready Interrupt Disable Mask */
#define SSC_IDR_RXRDY(value)                  (SSC_IDR_RXRDY_Msk & ((value) << SSC_IDR_RXRDY_Pos))
#define SSC_IDR_OVRUN_Pos                     (5U)                                           /**< (SSC_IDR) Receive Overrun Interrupt Disable Position */
#define SSC_IDR_OVRUN_Msk                     (0x1U << SSC_IDR_OVRUN_Pos)                    /**< (SSC_IDR) Receive Overrun Interrupt Disable Mask */
#define SSC_IDR_OVRUN(value)                  (SSC_IDR_OVRUN_Msk & ((value) << SSC_IDR_OVRUN_Pos))
#define SSC_IDR_CP0_Pos                       (8U)                                           /**< (SSC_IDR) Compare 0 Interrupt Disable Position */
#define SSC_IDR_CP0_Msk                       (0x1U << SSC_IDR_CP0_Pos)                      /**< (SSC_IDR) Compare 0 Interrupt Disable Mask */
#define SSC_IDR_CP0(value)                    (SSC_IDR_CP0_Msk & ((value) << SSC_IDR_CP0_Pos))
#define SSC_IDR_CP1_Pos                       (9U)                                           /**< (SSC_IDR) Compare 1 Interrupt Disable Position */
#define SSC_IDR_CP1_Msk                       (0x1U << SSC_IDR_CP1_Pos)                      /**< (SSC_IDR) Compare 1 Interrupt Disable Mask */
#define SSC_IDR_CP1(value)                    (SSC_IDR_CP1_Msk & ((value) << SSC_IDR_CP1_Pos))
#define SSC_IDR_TXSYN_Pos                     (10U)                                          /**< (SSC_IDR) Tx Sync Interrupt Enable Position */
#define SSC_IDR_TXSYN_Msk                     (0x1U << SSC_IDR_TXSYN_Pos)                    /**< (SSC_IDR) Tx Sync Interrupt Enable Mask */
#define SSC_IDR_TXSYN(value)                  (SSC_IDR_TXSYN_Msk & ((value) << SSC_IDR_TXSYN_Pos))
#define SSC_IDR_RXSYN_Pos                     (11U)                                          /**< (SSC_IDR) Rx Sync Interrupt Enable Position */
#define SSC_IDR_RXSYN_Msk                     (0x1U << SSC_IDR_RXSYN_Pos)                    /**< (SSC_IDR) Rx Sync Interrupt Enable Mask */
#define SSC_IDR_RXSYN(value)                  (SSC_IDR_RXSYN_Msk & ((value) << SSC_IDR_RXSYN_Pos))
#define SSC_IDR_Msk                           (0x00000F33UL)                                 /**< (SSC_IDR) Register Mask  */

/* -------- SSC_IMR : (SSC Offset: 0x4C) (R/  32) Interrupt Mask Register -------- */
#define SSC_IMR_TXRDY_Pos                     (0U)                                           /**< (SSC_IMR) Transmit Ready Interrupt Mask Position */
#define SSC_IMR_TXRDY_Msk                     (0x1U << SSC_IMR_TXRDY_Pos)                    /**< (SSC_IMR) Transmit Ready Interrupt Mask Mask */
#define SSC_IMR_TXRDY(value)                  (SSC_IMR_TXRDY_Msk & ((value) << SSC_IMR_TXRDY_Pos))
#define SSC_IMR_TXEMPTY_Pos                   (1U)                                           /**< (SSC_IMR) Transmit Empty Interrupt Mask Position */
#define SSC_IMR_TXEMPTY_Msk                   (0x1U << SSC_IMR_TXEMPTY_Pos)                  /**< (SSC_IMR) Transmit Empty Interrupt Mask Mask */
#define SSC_IMR_TXEMPTY(value)                (SSC_IMR_TXEMPTY_Msk & ((value) << SSC_IMR_TXEMPTY_Pos))
#define SSC_IMR_RXRDY_Pos                     (4U)                                           /**< (SSC_IMR) Receive Ready Interrupt Mask Position */
#define SSC_IMR_RXRDY_Msk                     (0x1U << SSC_IMR_RXRDY_Pos)                    /**< (SSC_IMR) Receive Ready Interrupt Mask Mask */
#define SSC_IMR_RXRDY(value)                  (SSC_IMR_RXRDY_Msk & ((value) << SSC_IMR_RXRDY_Pos))
#define SSC_IMR_OVRUN_Pos                     (5U)                                           /**< (SSC_IMR) Receive Overrun Interrupt Mask Position */
#define SSC_IMR_OVRUN_Msk                     (0x1U << SSC_IMR_OVRUN_Pos)                    /**< (SSC_IMR) Receive Overrun Interrupt Mask Mask */
#define SSC_IMR_OVRUN(value)                  (SSC_IMR_OVRUN_Msk & ((value) << SSC_IMR_OVRUN_Pos))
#define SSC_IMR_CP0_Pos                       (8U)                                           /**< (SSC_IMR) Compare 0 Interrupt Mask Position */
#define SSC_IMR_CP0_Msk                       (0x1U << SSC_IMR_CP0_Pos)                      /**< (SSC_IMR) Compare 0 Interrupt Mask Mask */
#define SSC_IMR_CP0(value)                    (SSC_IMR_CP0_Msk & ((value) << SSC_IMR_CP0_Pos))
#define SSC_IMR_CP1_Pos                       (9U)                                           /**< (SSC_IMR) Compare 1 Interrupt Mask Position */
#define SSC_IMR_CP1_Msk                       (0x1U << SSC_IMR_CP1_Pos)                      /**< (SSC_IMR) Compare 1 Interrupt Mask Mask */
#define SSC_IMR_CP1(value)                    (SSC_IMR_CP1_Msk & ((value) << SSC_IMR_CP1_Pos))
#define SSC_IMR_TXSYN_Pos                     (10U)                                          /**< (SSC_IMR) Tx Sync Interrupt Mask Position */
#define SSC_IMR_TXSYN_Msk                     (0x1U << SSC_IMR_TXSYN_Pos)                    /**< (SSC_IMR) Tx Sync Interrupt Mask Mask */
#define SSC_IMR_TXSYN(value)                  (SSC_IMR_TXSYN_Msk & ((value) << SSC_IMR_TXSYN_Pos))
#define SSC_IMR_RXSYN_Pos                     (11U)                                          /**< (SSC_IMR) Rx Sync Interrupt Mask Position */
#define SSC_IMR_RXSYN_Msk                     (0x1U << SSC_IMR_RXSYN_Pos)                    /**< (SSC_IMR) Rx Sync Interrupt Mask Mask */
#define SSC_IMR_RXSYN(value)                  (SSC_IMR_RXSYN_Msk & ((value) << SSC_IMR_RXSYN_Pos))
#define SSC_IMR_Msk                           (0x00000F33UL)                                 /**< (SSC_IMR) Register Mask  */

/* -------- SSC_WPMR : (SSC Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define SSC_WPMR_WPEN_Pos                     (0U)                                           /**< (SSC_WPMR) Write Protection Enable Position */
#define SSC_WPMR_WPEN_Msk                     (0x1U << SSC_WPMR_WPEN_Pos)                    /**< (SSC_WPMR) Write Protection Enable Mask */
#define SSC_WPMR_WPEN(value)                  (SSC_WPMR_WPEN_Msk & ((value) << SSC_WPMR_WPEN_Pos))
#define SSC_WPMR_WPKEY_Pos                    (8U)                                           /**< (SSC_WPMR) Write Protection Key Position */
#define SSC_WPMR_WPKEY_Msk                    (0xFFFFFFU << SSC_WPMR_WPKEY_Pos)              /**< (SSC_WPMR) Write Protection Key Mask */
#define SSC_WPMR_WPKEY(value)                 (SSC_WPMR_WPKEY_Msk & ((value) << SSC_WPMR_WPKEY_Pos))
#define   SSC_WPMR_WPKEY_PASSWD_Val           (5460803U)                                     /**< (SSC_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0.  */
#define SSC_WPMR_WPKEY_PASSWD                 (SSC_WPMR_WPKEY_PASSWD_Val << SSC_WPMR_WPKEY_Pos) /**< (SSC_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0. Position  */
#define SSC_WPMR_Msk                          (0xFFFFFF01UL)                                 /**< (SSC_WPMR) Register Mask  */

/* -------- SSC_WPSR : (SSC Offset: 0xE8) (R/  32) Write Protection Status Register -------- */
#define SSC_WPSR_WPVS_Pos                     (0U)                                           /**< (SSC_WPSR) Write Protection Violation Status Position */
#define SSC_WPSR_WPVS_Msk                     (0x1U << SSC_WPSR_WPVS_Pos)                    /**< (SSC_WPSR) Write Protection Violation Status Mask */
#define SSC_WPSR_WPVS(value)                  (SSC_WPSR_WPVS_Msk & ((value) << SSC_WPSR_WPVS_Pos))
#define SSC_WPSR_WPVSRC_Pos                   (8U)                                           /**< (SSC_WPSR) Write Protect Violation Source Position */
#define SSC_WPSR_WPVSRC_Msk                   (0xFFFFU << SSC_WPSR_WPVSRC_Pos)               /**< (SSC_WPSR) Write Protect Violation Source Mask */
#define SSC_WPSR_WPVSRC(value)                (SSC_WPSR_WPVSRC_Msk & ((value) << SSC_WPSR_WPVSRC_Pos))
#define SSC_WPSR_Msk                          (0x00FFFF01UL)                                 /**< (SSC_WPSR) Register Mask  */

/** \brief SSC register offsets definitions */
#define SSC_CR_OFFSET                  (0x00)         /**< (SSC_CR) Control Register Offset */
#define SSC_CMR_OFFSET                 (0x04)         /**< (SSC_CMR) Clock Mode Register Offset */
#define SSC_RCMR_OFFSET                (0x10)         /**< (SSC_RCMR) Receive Clock Mode Register Offset */
#define SSC_RFMR_OFFSET                (0x14)         /**< (SSC_RFMR) Receive Frame Mode Register Offset */
#define SSC_TCMR_OFFSET                (0x18)         /**< (SSC_TCMR) Transmit Clock Mode Register Offset */
#define SSC_TFMR_OFFSET                (0x1C)         /**< (SSC_TFMR) Transmit Frame Mode Register Offset */
#define SSC_RHR_OFFSET                 (0x20)         /**< (SSC_RHR) Receive Holding Register Offset */
#define SSC_THR_OFFSET                 (0x24)         /**< (SSC_THR) Transmit Holding Register Offset */
#define SSC_RSHR_OFFSET                (0x30)         /**< (SSC_RSHR) Receive Sync. Holding Register Offset */
#define SSC_TSHR_OFFSET                (0x34)         /**< (SSC_TSHR) Transmit Sync. Holding Register Offset */
#define SSC_RC0R_OFFSET                (0x38)         /**< (SSC_RC0R) Receive Compare 0 Register Offset */
#define SSC_RC1R_OFFSET                (0x3C)         /**< (SSC_RC1R) Receive Compare 1 Register Offset */
#define SSC_SR_OFFSET                  (0x40)         /**< (SSC_SR) Status Register Offset */
#define SSC_IER_OFFSET                 (0x44)         /**< (SSC_IER) Interrupt Enable Register Offset */
#define SSC_IDR_OFFSET                 (0x48)         /**< (SSC_IDR) Interrupt Disable Register Offset */
#define SSC_IMR_OFFSET                 (0x4C)         /**< (SSC_IMR) Interrupt Mask Register Offset */
#define SSC_WPMR_OFFSET                (0xE4)         /**< (SSC_WPMR) Write Protection Mode Register Offset */
#define SSC_WPSR_OFFSET                (0xE8)         /**< (SSC_WPSR) Write Protection Status Register Offset */

/** \brief SSC register API structure */
typedef struct
{
  __O   uint32_t                       SSC_CR;          /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       SSC_CMR;         /**< Offset: 0x04 (R/W  32) Clock Mode Register */
  __I   uint8_t                        Reserved1[0x08];
  __IO  uint32_t                       SSC_RCMR;        /**< Offset: 0x10 (R/W  32) Receive Clock Mode Register */
  __IO  uint32_t                       SSC_RFMR;        /**< Offset: 0x14 (R/W  32) Receive Frame Mode Register */
  __IO  uint32_t                       SSC_TCMR;        /**< Offset: 0x18 (R/W  32) Transmit Clock Mode Register */
  __IO  uint32_t                       SSC_TFMR;        /**< Offset: 0x1c (R/W  32) Transmit Frame Mode Register */
  __I   uint32_t                       SSC_RHR;         /**< Offset: 0x20 (R/   32) Receive Holding Register */
  __O   uint32_t                       SSC_THR;         /**< Offset: 0x24 ( /W  32) Transmit Holding Register */
  __I   uint8_t                        Reserved2[0x08];
  __I   uint32_t                       SSC_RSHR;        /**< Offset: 0x30 (R/   32) Receive Sync. Holding Register */
  __IO  uint32_t                       SSC_TSHR;        /**< Offset: 0x34 (R/W  32) Transmit Sync. Holding Register */
  __IO  uint32_t                       SSC_RC0R;        /**< Offset: 0x38 (R/W  32) Receive Compare 0 Register */
  __IO  uint32_t                       SSC_RC1R;        /**< Offset: 0x3c (R/W  32) Receive Compare 1 Register */
  __I   uint32_t                       SSC_SR;          /**< Offset: 0x40 (R/   32) Status Register */
  __O   uint32_t                       SSC_IER;         /**< Offset: 0x44 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       SSC_IDR;         /**< Offset: 0x48 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       SSC_IMR;         /**< Offset: 0x4c (R/   32) Interrupt Mask Register */
  __I   uint8_t                        Reserved3[0x94];
  __IO  uint32_t                       SSC_WPMR;        /**< Offset: 0xe4 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       SSC_WPSR;        /**< Offset: 0xe8 (R/   32) Write Protection Status Register */
} ssc_registers_t;
/** @}  end of Synchronous Serial Controller */

#endif /* SAME_SAME70_SSC_MODULE_H */

