/**
 * \brief Header file for SAME/SAME70 ICM
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
#ifndef SAME_SAME70_ICM_MODULE_H
#define SAME_SAME70_ICM_MODULE_H

/** \addtogroup SAME_SAME70 Integrity Check Monitor
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_ICM                                   */
/* ========================================================================== */

/* -------- ICM_CFG : (ICM Offset: 0x00) (R/W 32) Configuration Register -------- */
#define ICM_CFG_WBDIS_Pos                     (0U)                                           /**< (ICM_CFG) Write Back Disable Position */
#define ICM_CFG_WBDIS_Msk                     (0x1U << ICM_CFG_WBDIS_Pos)                    /**< (ICM_CFG) Write Back Disable Mask */
#define ICM_CFG_WBDIS(value)                  (ICM_CFG_WBDIS_Msk & ((value) << ICM_CFG_WBDIS_Pos))
#define ICM_CFG_EOMDIS_Pos                    (1U)                                           /**< (ICM_CFG) End of Monitoring Disable Position */
#define ICM_CFG_EOMDIS_Msk                    (0x1U << ICM_CFG_EOMDIS_Pos)                   /**< (ICM_CFG) End of Monitoring Disable Mask */
#define ICM_CFG_EOMDIS(value)                 (ICM_CFG_EOMDIS_Msk & ((value) << ICM_CFG_EOMDIS_Pos))
#define ICM_CFG_SLBDIS_Pos                    (2U)                                           /**< (ICM_CFG) Secondary List Branching Disable Position */
#define ICM_CFG_SLBDIS_Msk                    (0x1U << ICM_CFG_SLBDIS_Pos)                   /**< (ICM_CFG) Secondary List Branching Disable Mask */
#define ICM_CFG_SLBDIS(value)                 (ICM_CFG_SLBDIS_Msk & ((value) << ICM_CFG_SLBDIS_Pos))
#define ICM_CFG_BBC_Pos                       (4U)                                           /**< (ICM_CFG) Bus Burden Control Position */
#define ICM_CFG_BBC_Msk                       (0xFU << ICM_CFG_BBC_Pos)                      /**< (ICM_CFG) Bus Burden Control Mask */
#define ICM_CFG_BBC(value)                    (ICM_CFG_BBC_Msk & ((value) << ICM_CFG_BBC_Pos))
#define ICM_CFG_ASCD_Pos                      (8U)                                           /**< (ICM_CFG) Automatic Switch To Compare Digest Position */
#define ICM_CFG_ASCD_Msk                      (0x1U << ICM_CFG_ASCD_Pos)                     /**< (ICM_CFG) Automatic Switch To Compare Digest Mask */
#define ICM_CFG_ASCD(value)                   (ICM_CFG_ASCD_Msk & ((value) << ICM_CFG_ASCD_Pos))
#define ICM_CFG_DUALBUFF_Pos                  (9U)                                           /**< (ICM_CFG) Dual Input Buffer Position */
#define ICM_CFG_DUALBUFF_Msk                  (0x1U << ICM_CFG_DUALBUFF_Pos)                 /**< (ICM_CFG) Dual Input Buffer Mask */
#define ICM_CFG_DUALBUFF(value)               (ICM_CFG_DUALBUFF_Msk & ((value) << ICM_CFG_DUALBUFF_Pos))
#define ICM_CFG_UIHASH_Pos                    (12U)                                          /**< (ICM_CFG) User Initial Hash Value Position */
#define ICM_CFG_UIHASH_Msk                    (0x1U << ICM_CFG_UIHASH_Pos)                   /**< (ICM_CFG) User Initial Hash Value Mask */
#define ICM_CFG_UIHASH(value)                 (ICM_CFG_UIHASH_Msk & ((value) << ICM_CFG_UIHASH_Pos))
#define ICM_CFG_UALGO_Pos                     (13U)                                          /**< (ICM_CFG) User SHA Algorithm Position */
#define ICM_CFG_UALGO_Msk                     (0x7U << ICM_CFG_UALGO_Pos)                    /**< (ICM_CFG) User SHA Algorithm Mask */
#define ICM_CFG_UALGO(value)                  (ICM_CFG_UALGO_Msk & ((value) << ICM_CFG_UALGO_Pos))
#define   ICM_CFG_UALGO_SHA1_Val              (0U)                                           /**< (ICM_CFG) SHA1 algorithm processed  */
#define   ICM_CFG_UALGO_SHA256_Val            (1U)                                           /**< (ICM_CFG) SHA256 algorithm processed  */
#define   ICM_CFG_UALGO_SHA224_Val            (4U)                                           /**< (ICM_CFG) SHA224 algorithm processed  */
#define ICM_CFG_UALGO_SHA1                    (ICM_CFG_UALGO_SHA1_Val << ICM_CFG_UALGO_Pos)  /**< (ICM_CFG) SHA1 algorithm processed Position  */
#define ICM_CFG_UALGO_SHA256                  (ICM_CFG_UALGO_SHA256_Val << ICM_CFG_UALGO_Pos) /**< (ICM_CFG) SHA256 algorithm processed Position  */
#define ICM_CFG_UALGO_SHA224                  (ICM_CFG_UALGO_SHA224_Val << ICM_CFG_UALGO_Pos) /**< (ICM_CFG) SHA224 algorithm processed Position  */
#define ICM_CFG_Msk                           (0x0000F3F7UL)                                 /**< (ICM_CFG) Register Mask  */

/* -------- ICM_CTRL : (ICM Offset: 0x04) ( /W 32) Control Register -------- */
#define ICM_CTRL_ENABLE_Pos                   (0U)                                           /**< (ICM_CTRL) ICM Enable Position */
#define ICM_CTRL_ENABLE_Msk                   (0x1U << ICM_CTRL_ENABLE_Pos)                  /**< (ICM_CTRL) ICM Enable Mask */
#define ICM_CTRL_ENABLE(value)                (ICM_CTRL_ENABLE_Msk & ((value) << ICM_CTRL_ENABLE_Pos))
#define ICM_CTRL_DISABLE_Pos                  (1U)                                           /**< (ICM_CTRL) ICM Disable Register Position */
#define ICM_CTRL_DISABLE_Msk                  (0x1U << ICM_CTRL_DISABLE_Pos)                 /**< (ICM_CTRL) ICM Disable Register Mask */
#define ICM_CTRL_DISABLE(value)               (ICM_CTRL_DISABLE_Msk & ((value) << ICM_CTRL_DISABLE_Pos))
#define ICM_CTRL_SWRST_Pos                    (2U)                                           /**< (ICM_CTRL) Software Reset Position */
#define ICM_CTRL_SWRST_Msk                    (0x1U << ICM_CTRL_SWRST_Pos)                   /**< (ICM_CTRL) Software Reset Mask */
#define ICM_CTRL_SWRST(value)                 (ICM_CTRL_SWRST_Msk & ((value) << ICM_CTRL_SWRST_Pos))
#define ICM_CTRL_REHASH_Pos                   (4U)                                           /**< (ICM_CTRL) Recompute Internal Hash Position */
#define ICM_CTRL_REHASH_Msk                   (0xFU << ICM_CTRL_REHASH_Pos)                  /**< (ICM_CTRL) Recompute Internal Hash Mask */
#define ICM_CTRL_REHASH(value)                (ICM_CTRL_REHASH_Msk & ((value) << ICM_CTRL_REHASH_Pos))
#define ICM_CTRL_RMDIS_Pos                    (8U)                                           /**< (ICM_CTRL) Region Monitoring Disable Position */
#define ICM_CTRL_RMDIS_Msk                    (0xFU << ICM_CTRL_RMDIS_Pos)                   /**< (ICM_CTRL) Region Monitoring Disable Mask */
#define ICM_CTRL_RMDIS(value)                 (ICM_CTRL_RMDIS_Msk & ((value) << ICM_CTRL_RMDIS_Pos))
#define ICM_CTRL_RMEN_Pos                     (12U)                                          /**< (ICM_CTRL) Region Monitoring Enable Position */
#define ICM_CTRL_RMEN_Msk                     (0xFU << ICM_CTRL_RMEN_Pos)                    /**< (ICM_CTRL) Region Monitoring Enable Mask */
#define ICM_CTRL_RMEN(value)                  (ICM_CTRL_RMEN_Msk & ((value) << ICM_CTRL_RMEN_Pos))
#define ICM_CTRL_Msk                          (0x0000FFF7UL)                                 /**< (ICM_CTRL) Register Mask  */

/* -------- ICM_SR : (ICM Offset: 0x08) (R/  32) Status Register -------- */
#define ICM_SR_ENABLE_Pos                     (0U)                                           /**< (ICM_SR) ICM Controller Enable Register Position */
#define ICM_SR_ENABLE_Msk                     (0x1U << ICM_SR_ENABLE_Pos)                    /**< (ICM_SR) ICM Controller Enable Register Mask */
#define ICM_SR_ENABLE(value)                  (ICM_SR_ENABLE_Msk & ((value) << ICM_SR_ENABLE_Pos))
#define ICM_SR_RAWRMDIS_Pos                   (8U)                                           /**< (ICM_SR) Region Monitoring Disabled Raw Status Position */
#define ICM_SR_RAWRMDIS_Msk                   (0xFU << ICM_SR_RAWRMDIS_Pos)                  /**< (ICM_SR) Region Monitoring Disabled Raw Status Mask */
#define ICM_SR_RAWRMDIS(value)                (ICM_SR_RAWRMDIS_Msk & ((value) << ICM_SR_RAWRMDIS_Pos))
#define ICM_SR_RMDIS_Pos                      (12U)                                          /**< (ICM_SR) Region Monitoring Disabled Status Position */
#define ICM_SR_RMDIS_Msk                      (0xFU << ICM_SR_RMDIS_Pos)                     /**< (ICM_SR) Region Monitoring Disabled Status Mask */
#define ICM_SR_RMDIS(value)                   (ICM_SR_RMDIS_Msk & ((value) << ICM_SR_RMDIS_Pos))
#define ICM_SR_Msk                            (0x0000FF01UL)                                 /**< (ICM_SR) Register Mask  */

/* -------- ICM_IER : (ICM Offset: 0x10) ( /W 32) Interrupt Enable Register -------- */
#define ICM_IER_RHC_Pos                       (0U)                                           /**< (ICM_IER) Region Hash Completed Interrupt Enable Position */
#define ICM_IER_RHC_Msk                       (0xFU << ICM_IER_RHC_Pos)                      /**< (ICM_IER) Region Hash Completed Interrupt Enable Mask */
#define ICM_IER_RHC(value)                    (ICM_IER_RHC_Msk & ((value) << ICM_IER_RHC_Pos))
#define ICM_IER_RDM_Pos                       (4U)                                           /**< (ICM_IER) Region Digest Mismatch Interrupt Enable Position */
#define ICM_IER_RDM_Msk                       (0xFU << ICM_IER_RDM_Pos)                      /**< (ICM_IER) Region Digest Mismatch Interrupt Enable Mask */
#define ICM_IER_RDM(value)                    (ICM_IER_RDM_Msk & ((value) << ICM_IER_RDM_Pos))
#define ICM_IER_RBE_Pos                       (8U)                                           /**< (ICM_IER) Region Bus Error Interrupt Enable Position */
#define ICM_IER_RBE_Msk                       (0xFU << ICM_IER_RBE_Pos)                      /**< (ICM_IER) Region Bus Error Interrupt Enable Mask */
#define ICM_IER_RBE(value)                    (ICM_IER_RBE_Msk & ((value) << ICM_IER_RBE_Pos))
#define ICM_IER_RWC_Pos                       (12U)                                          /**< (ICM_IER) Region Wrap Condition detected Interrupt Enable Position */
#define ICM_IER_RWC_Msk                       (0xFU << ICM_IER_RWC_Pos)                      /**< (ICM_IER) Region Wrap Condition detected Interrupt Enable Mask */
#define ICM_IER_RWC(value)                    (ICM_IER_RWC_Msk & ((value) << ICM_IER_RWC_Pos))
#define ICM_IER_REC_Pos                       (16U)                                          /**< (ICM_IER) Region End bit Condition Detected Interrupt Enable Position */
#define ICM_IER_REC_Msk                       (0xFU << ICM_IER_REC_Pos)                      /**< (ICM_IER) Region End bit Condition Detected Interrupt Enable Mask */
#define ICM_IER_REC(value)                    (ICM_IER_REC_Msk & ((value) << ICM_IER_REC_Pos))
#define ICM_IER_RSU_Pos                       (20U)                                          /**< (ICM_IER) Region Status Updated Interrupt Disable Position */
#define ICM_IER_RSU_Msk                       (0xFU << ICM_IER_RSU_Pos)                      /**< (ICM_IER) Region Status Updated Interrupt Disable Mask */
#define ICM_IER_RSU(value)                    (ICM_IER_RSU_Msk & ((value) << ICM_IER_RSU_Pos))
#define ICM_IER_URAD_Pos                      (24U)                                          /**< (ICM_IER) Undefined Register Access Detection Interrupt Enable Position */
#define ICM_IER_URAD_Msk                      (0x1U << ICM_IER_URAD_Pos)                     /**< (ICM_IER) Undefined Register Access Detection Interrupt Enable Mask */
#define ICM_IER_URAD(value)                   (ICM_IER_URAD_Msk & ((value) << ICM_IER_URAD_Pos))
#define ICM_IER_Msk                           (0x01FFFFFFUL)                                 /**< (ICM_IER) Register Mask  */

/* -------- ICM_IDR : (ICM Offset: 0x14) ( /W 32) Interrupt Disable Register -------- */
#define ICM_IDR_RHC_Pos                       (0U)                                           /**< (ICM_IDR) Region Hash Completed Interrupt Disable Position */
#define ICM_IDR_RHC_Msk                       (0xFU << ICM_IDR_RHC_Pos)                      /**< (ICM_IDR) Region Hash Completed Interrupt Disable Mask */
#define ICM_IDR_RHC(value)                    (ICM_IDR_RHC_Msk & ((value) << ICM_IDR_RHC_Pos))
#define ICM_IDR_RDM_Pos                       (4U)                                           /**< (ICM_IDR) Region Digest Mismatch Interrupt Disable Position */
#define ICM_IDR_RDM_Msk                       (0xFU << ICM_IDR_RDM_Pos)                      /**< (ICM_IDR) Region Digest Mismatch Interrupt Disable Mask */
#define ICM_IDR_RDM(value)                    (ICM_IDR_RDM_Msk & ((value) << ICM_IDR_RDM_Pos))
#define ICM_IDR_RBE_Pos                       (8U)                                           /**< (ICM_IDR) Region Bus Error Interrupt Disable Position */
#define ICM_IDR_RBE_Msk                       (0xFU << ICM_IDR_RBE_Pos)                      /**< (ICM_IDR) Region Bus Error Interrupt Disable Mask */
#define ICM_IDR_RBE(value)                    (ICM_IDR_RBE_Msk & ((value) << ICM_IDR_RBE_Pos))
#define ICM_IDR_RWC_Pos                       (12U)                                          /**< (ICM_IDR) Region Wrap Condition Detected Interrupt Disable Position */
#define ICM_IDR_RWC_Msk                       (0xFU << ICM_IDR_RWC_Pos)                      /**< (ICM_IDR) Region Wrap Condition Detected Interrupt Disable Mask */
#define ICM_IDR_RWC(value)                    (ICM_IDR_RWC_Msk & ((value) << ICM_IDR_RWC_Pos))
#define ICM_IDR_REC_Pos                       (16U)                                          /**< (ICM_IDR) Region End bit Condition detected Interrupt Disable Position */
#define ICM_IDR_REC_Msk                       (0xFU << ICM_IDR_REC_Pos)                      /**< (ICM_IDR) Region End bit Condition detected Interrupt Disable Mask */
#define ICM_IDR_REC(value)                    (ICM_IDR_REC_Msk & ((value) << ICM_IDR_REC_Pos))
#define ICM_IDR_RSU_Pos                       (20U)                                          /**< (ICM_IDR) Region Status Updated Interrupt Disable Position */
#define ICM_IDR_RSU_Msk                       (0xFU << ICM_IDR_RSU_Pos)                      /**< (ICM_IDR) Region Status Updated Interrupt Disable Mask */
#define ICM_IDR_RSU(value)                    (ICM_IDR_RSU_Msk & ((value) << ICM_IDR_RSU_Pos))
#define ICM_IDR_URAD_Pos                      (24U)                                          /**< (ICM_IDR) Undefined Register Access Detection Interrupt Disable Position */
#define ICM_IDR_URAD_Msk                      (0x1U << ICM_IDR_URAD_Pos)                     /**< (ICM_IDR) Undefined Register Access Detection Interrupt Disable Mask */
#define ICM_IDR_URAD(value)                   (ICM_IDR_URAD_Msk & ((value) << ICM_IDR_URAD_Pos))
#define ICM_IDR_Msk                           (0x01FFFFFFUL)                                 /**< (ICM_IDR) Register Mask  */

/* -------- ICM_IMR : (ICM Offset: 0x18) (R/  32) Interrupt Mask Register -------- */
#define ICM_IMR_RHC_Pos                       (0U)                                           /**< (ICM_IMR) Region Hash Completed Interrupt Mask Position */
#define ICM_IMR_RHC_Msk                       (0xFU << ICM_IMR_RHC_Pos)                      /**< (ICM_IMR) Region Hash Completed Interrupt Mask Mask */
#define ICM_IMR_RHC(value)                    (ICM_IMR_RHC_Msk & ((value) << ICM_IMR_RHC_Pos))
#define ICM_IMR_RDM_Pos                       (4U)                                           /**< (ICM_IMR) Region Digest Mismatch Interrupt Mask Position */
#define ICM_IMR_RDM_Msk                       (0xFU << ICM_IMR_RDM_Pos)                      /**< (ICM_IMR) Region Digest Mismatch Interrupt Mask Mask */
#define ICM_IMR_RDM(value)                    (ICM_IMR_RDM_Msk & ((value) << ICM_IMR_RDM_Pos))
#define ICM_IMR_RBE_Pos                       (8U)                                           /**< (ICM_IMR) Region Bus Error Interrupt Mask Position */
#define ICM_IMR_RBE_Msk                       (0xFU << ICM_IMR_RBE_Pos)                      /**< (ICM_IMR) Region Bus Error Interrupt Mask Mask */
#define ICM_IMR_RBE(value)                    (ICM_IMR_RBE_Msk & ((value) << ICM_IMR_RBE_Pos))
#define ICM_IMR_RWC_Pos                       (12U)                                          /**< (ICM_IMR) Region Wrap Condition Detected Interrupt Mask Position */
#define ICM_IMR_RWC_Msk                       (0xFU << ICM_IMR_RWC_Pos)                      /**< (ICM_IMR) Region Wrap Condition Detected Interrupt Mask Mask */
#define ICM_IMR_RWC(value)                    (ICM_IMR_RWC_Msk & ((value) << ICM_IMR_RWC_Pos))
#define ICM_IMR_REC_Pos                       (16U)                                          /**< (ICM_IMR) Region End bit Condition Detected Interrupt Mask Position */
#define ICM_IMR_REC_Msk                       (0xFU << ICM_IMR_REC_Pos)                      /**< (ICM_IMR) Region End bit Condition Detected Interrupt Mask Mask */
#define ICM_IMR_REC(value)                    (ICM_IMR_REC_Msk & ((value) << ICM_IMR_REC_Pos))
#define ICM_IMR_RSU_Pos                       (20U)                                          /**< (ICM_IMR) Region Status Updated Interrupt Mask Position */
#define ICM_IMR_RSU_Msk                       (0xFU << ICM_IMR_RSU_Pos)                      /**< (ICM_IMR) Region Status Updated Interrupt Mask Mask */
#define ICM_IMR_RSU(value)                    (ICM_IMR_RSU_Msk & ((value) << ICM_IMR_RSU_Pos))
#define ICM_IMR_URAD_Pos                      (24U)                                          /**< (ICM_IMR) Undefined Register Access Detection Interrupt Mask Position */
#define ICM_IMR_URAD_Msk                      (0x1U << ICM_IMR_URAD_Pos)                     /**< (ICM_IMR) Undefined Register Access Detection Interrupt Mask Mask */
#define ICM_IMR_URAD(value)                   (ICM_IMR_URAD_Msk & ((value) << ICM_IMR_URAD_Pos))
#define ICM_IMR_Msk                           (0x01FFFFFFUL)                                 /**< (ICM_IMR) Register Mask  */

/* -------- ICM_ISR : (ICM Offset: 0x1C) (R/  32) Interrupt Status Register -------- */
#define ICM_ISR_RHC_Pos                       (0U)                                           /**< (ICM_ISR) Region Hash Completed Position */
#define ICM_ISR_RHC_Msk                       (0xFU << ICM_ISR_RHC_Pos)                      /**< (ICM_ISR) Region Hash Completed Mask */
#define ICM_ISR_RHC(value)                    (ICM_ISR_RHC_Msk & ((value) << ICM_ISR_RHC_Pos))
#define ICM_ISR_RDM_Pos                       (4U)                                           /**< (ICM_ISR) Region Digest Mismatch Position */
#define ICM_ISR_RDM_Msk                       (0xFU << ICM_ISR_RDM_Pos)                      /**< (ICM_ISR) Region Digest Mismatch Mask */
#define ICM_ISR_RDM(value)                    (ICM_ISR_RDM_Msk & ((value) << ICM_ISR_RDM_Pos))
#define ICM_ISR_RBE_Pos                       (8U)                                           /**< (ICM_ISR) Region Bus Error Position */
#define ICM_ISR_RBE_Msk                       (0xFU << ICM_ISR_RBE_Pos)                      /**< (ICM_ISR) Region Bus Error Mask */
#define ICM_ISR_RBE(value)                    (ICM_ISR_RBE_Msk & ((value) << ICM_ISR_RBE_Pos))
#define ICM_ISR_RWC_Pos                       (12U)                                          /**< (ICM_ISR) Region Wrap Condition Detected Position */
#define ICM_ISR_RWC_Msk                       (0xFU << ICM_ISR_RWC_Pos)                      /**< (ICM_ISR) Region Wrap Condition Detected Mask */
#define ICM_ISR_RWC(value)                    (ICM_ISR_RWC_Msk & ((value) << ICM_ISR_RWC_Pos))
#define ICM_ISR_REC_Pos                       (16U)                                          /**< (ICM_ISR) Region End bit Condition Detected Position */
#define ICM_ISR_REC_Msk                       (0xFU << ICM_ISR_REC_Pos)                      /**< (ICM_ISR) Region End bit Condition Detected Mask */
#define ICM_ISR_REC(value)                    (ICM_ISR_REC_Msk & ((value) << ICM_ISR_REC_Pos))
#define ICM_ISR_RSU_Pos                       (20U)                                          /**< (ICM_ISR) Region Status Updated Detected Position */
#define ICM_ISR_RSU_Msk                       (0xFU << ICM_ISR_RSU_Pos)                      /**< (ICM_ISR) Region Status Updated Detected Mask */
#define ICM_ISR_RSU(value)                    (ICM_ISR_RSU_Msk & ((value) << ICM_ISR_RSU_Pos))
#define ICM_ISR_URAD_Pos                      (24U)                                          /**< (ICM_ISR) Undefined Register Access Detection Status Position */
#define ICM_ISR_URAD_Msk                      (0x1U << ICM_ISR_URAD_Pos)                     /**< (ICM_ISR) Undefined Register Access Detection Status Mask */
#define ICM_ISR_URAD(value)                   (ICM_ISR_URAD_Msk & ((value) << ICM_ISR_URAD_Pos))
#define ICM_ISR_Msk                           (0x01FFFFFFUL)                                 /**< (ICM_ISR) Register Mask  */

/* -------- ICM_UASR : (ICM Offset: 0x20) (R/  32) Undefined Access Status Register -------- */
#define ICM_UASR_URAT_Pos                     (0U)                                           /**< (ICM_UASR) Undefined Register Access Trace Position */
#define ICM_UASR_URAT_Msk                     (0x7U << ICM_UASR_URAT_Pos)                    /**< (ICM_UASR) Undefined Register Access Trace Mask */
#define ICM_UASR_URAT(value)                  (ICM_UASR_URAT_Msk & ((value) << ICM_UASR_URAT_Pos))
#define   ICM_UASR_URAT_UNSPEC_STRUCT_MEMBER_Val (0U)                                           /**< (ICM_UASR) Unspecified structure member set to one detected when the descriptor is loaded.  */
#define   ICM_UASR_URAT_ICM_CFG_MODIFIED_Val  (1U)                                           /**< (ICM_UASR) ICM_CFG modified during active monitoring.  */
#define   ICM_UASR_URAT_ICM_DSCR_MODIFIED_Val (2U)                                           /**< (ICM_UASR) ICM_DSCR modified during active monitoring.  */
#define   ICM_UASR_URAT_ICM_HASH_MODIFIED_Val (3U)                                           /**< (ICM_UASR) ICM_HASH modified during active monitoring  */
#define   ICM_UASR_URAT_READ_ACCESS_Val       (4U)                                           /**< (ICM_UASR) Write-only register read access  */
#define ICM_UASR_URAT_UNSPEC_STRUCT_MEMBER    (ICM_UASR_URAT_UNSPEC_STRUCT_MEMBER_Val << ICM_UASR_URAT_Pos) /**< (ICM_UASR) Unspecified structure member set to one detected when the descriptor is loaded. Position  */
#define ICM_UASR_URAT_ICM_CFG_MODIFIED        (ICM_UASR_URAT_ICM_CFG_MODIFIED_Val << ICM_UASR_URAT_Pos) /**< (ICM_UASR) ICM_CFG modified during active monitoring. Position  */
#define ICM_UASR_URAT_ICM_DSCR_MODIFIED       (ICM_UASR_URAT_ICM_DSCR_MODIFIED_Val << ICM_UASR_URAT_Pos) /**< (ICM_UASR) ICM_DSCR modified during active monitoring. Position  */
#define ICM_UASR_URAT_ICM_HASH_MODIFIED       (ICM_UASR_URAT_ICM_HASH_MODIFIED_Val << ICM_UASR_URAT_Pos) /**< (ICM_UASR) ICM_HASH modified during active monitoring Position  */
#define ICM_UASR_URAT_READ_ACCESS             (ICM_UASR_URAT_READ_ACCESS_Val << ICM_UASR_URAT_Pos) /**< (ICM_UASR) Write-only register read access Position  */
#define ICM_UASR_Msk                          (0x00000007UL)                                 /**< (ICM_UASR) Register Mask  */

/* -------- ICM_DSCR : (ICM Offset: 0x30) (R/W 32) Region Descriptor Area Start Address Register -------- */
#define ICM_DSCR_DASA_Pos                     (6U)                                           /**< (ICM_DSCR) Descriptor Area Start Address Position */
#define ICM_DSCR_DASA_Msk                     (0x3FFFFFFU << ICM_DSCR_DASA_Pos)              /**< (ICM_DSCR) Descriptor Area Start Address Mask */
#define ICM_DSCR_DASA(value)                  (ICM_DSCR_DASA_Msk & ((value) << ICM_DSCR_DASA_Pos))
#define ICM_DSCR_Msk                          (0xFFFFFFC0UL)                                 /**< (ICM_DSCR) Register Mask  */

/* -------- ICM_HASH : (ICM Offset: 0x34) (R/W 32) Region Hash Area Start Address Register -------- */
#define ICM_HASH_HASA_Pos                     (7U)                                           /**< (ICM_HASH) Hash Area Start Address Position */
#define ICM_HASH_HASA_Msk                     (0x1FFFFFFU << ICM_HASH_HASA_Pos)              /**< (ICM_HASH) Hash Area Start Address Mask */
#define ICM_HASH_HASA(value)                  (ICM_HASH_HASA_Msk & ((value) << ICM_HASH_HASA_Pos))
#define ICM_HASH_Msk                          (0xFFFFFF80UL)                                 /**< (ICM_HASH) Register Mask  */

/* -------- ICM_UIHVAL : (ICM Offset: 0x38) ( /W 32) User Initial Hash Value 0 Register 0 -------- */
#define ICM_UIHVAL_VAL_Pos                    (0U)                                           /**< (ICM_UIHVAL) Initial Hash Value Position */
#define ICM_UIHVAL_VAL_Msk                    (0xFFFFFFFFU << ICM_UIHVAL_VAL_Pos)            /**< (ICM_UIHVAL) Initial Hash Value Mask */
#define ICM_UIHVAL_VAL(value)                 (ICM_UIHVAL_VAL_Msk & ((value) << ICM_UIHVAL_VAL_Pos))
#define ICM_UIHVAL_Msk                        (0xFFFFFFFFUL)                                 /**< (ICM_UIHVAL) Register Mask  */

/** \brief ICM register offsets definitions */
#define ICM_CFG_OFFSET                 (0x00)         /**< (ICM_CFG) Configuration Register Offset */
#define ICM_CTRL_OFFSET                (0x04)         /**< (ICM_CTRL) Control Register Offset */
#define ICM_SR_OFFSET                  (0x08)         /**< (ICM_SR) Status Register Offset */
#define ICM_IER_OFFSET                 (0x10)         /**< (ICM_IER) Interrupt Enable Register Offset */
#define ICM_IDR_OFFSET                 (0x14)         /**< (ICM_IDR) Interrupt Disable Register Offset */
#define ICM_IMR_OFFSET                 (0x18)         /**< (ICM_IMR) Interrupt Mask Register Offset */
#define ICM_ISR_OFFSET                 (0x1C)         /**< (ICM_ISR) Interrupt Status Register Offset */
#define ICM_UASR_OFFSET                (0x20)         /**< (ICM_UASR) Undefined Access Status Register Offset */
#define ICM_DSCR_OFFSET                (0x30)         /**< (ICM_DSCR) Region Descriptor Area Start Address Register Offset */
#define ICM_HASH_OFFSET                (0x34)         /**< (ICM_HASH) Region Hash Area Start Address Register Offset */
#define ICM_UIHVAL_OFFSET              (0x38)         /**< (ICM_UIHVAL) User Initial Hash Value 0 Register 0 Offset */

/** \brief ICM register API structure */
typedef struct
{
  __IO  uint32_t                       ICM_CFG;         /**< Offset: 0x00 (R/W  32) Configuration Register */
  __O   uint32_t                       ICM_CTRL;        /**< Offset: 0x04 ( /W  32) Control Register */
  __I   uint32_t                       ICM_SR;          /**< Offset: 0x08 (R/   32) Status Register */
  __I   uint8_t                        Reserved1[0x04];
  __O   uint32_t                       ICM_IER;         /**< Offset: 0x10 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       ICM_IDR;         /**< Offset: 0x14 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       ICM_IMR;         /**< Offset: 0x18 (R/   32) Interrupt Mask Register */
  __I   uint32_t                       ICM_ISR;         /**< Offset: 0x1c (R/   32) Interrupt Status Register */
  __I   uint32_t                       ICM_UASR;        /**< Offset: 0x20 (R/   32) Undefined Access Status Register */
  __I   uint8_t                        Reserved2[0x0C];
  __IO  uint32_t                       ICM_DSCR;        /**< Offset: 0x30 (R/W  32) Region Descriptor Area Start Address Register */
  __IO  uint32_t                       ICM_HASH;        /**< Offset: 0x34 (R/W  32) Region Hash Area Start Address Register */
  __O   uint32_t                       ICM_UIHVAL[8];   /**< Offset: 0x38 ( /W  32) User Initial Hash Value 0 Register 0 */
} icm_registers_t;
/** @}  end of Integrity Check Monitor */

#endif /* SAME_SAME70_ICM_MODULE_H */

