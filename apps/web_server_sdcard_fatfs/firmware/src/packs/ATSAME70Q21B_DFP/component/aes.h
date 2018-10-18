/**
 * \brief Header file for SAME/SAME70 AES
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
#ifndef SAME_SAME70_AES_MODULE_H
#define SAME_SAME70_AES_MODULE_H

/** \addtogroup SAME_SAME70 Advanced Encryption Standard
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_AES                                   */
/* ========================================================================== */

/* -------- AES_CR : (AES Offset: 0x00) ( /W 32) Control Register -------- */
#define AES_CR_START_Pos                      (0U)                                           /**< (AES_CR) Start Processing Position */
#define AES_CR_START_Msk                      (0x1U << AES_CR_START_Pos)                     /**< (AES_CR) Start Processing Mask */
#define AES_CR_START(value)                   (AES_CR_START_Msk & ((value) << AES_CR_START_Pos))
#define AES_CR_SWRST_Pos                      (8U)                                           /**< (AES_CR) Software Reset Position */
#define AES_CR_SWRST_Msk                      (0x1U << AES_CR_SWRST_Pos)                     /**< (AES_CR) Software Reset Mask */
#define AES_CR_SWRST(value)                   (AES_CR_SWRST_Msk & ((value) << AES_CR_SWRST_Pos))
#define AES_CR_LOADSEED_Pos                   (16U)                                          /**< (AES_CR) Random Number Generator Seed Loading Position */
#define AES_CR_LOADSEED_Msk                   (0x1U << AES_CR_LOADSEED_Pos)                  /**< (AES_CR) Random Number Generator Seed Loading Mask */
#define AES_CR_LOADSEED(value)                (AES_CR_LOADSEED_Msk & ((value) << AES_CR_LOADSEED_Pos))
#define AES_CR_Msk                            (0x00010101UL)                                 /**< (AES_CR) Register Mask  */

/* -------- AES_MR : (AES Offset: 0x04) (R/W 32) Mode Register -------- */
#define AES_MR_CIPHER_Pos                     (0U)                                           /**< (AES_MR) Processing Mode Position */
#define AES_MR_CIPHER_Msk                     (0x1U << AES_MR_CIPHER_Pos)                    /**< (AES_MR) Processing Mode Mask */
#define AES_MR_CIPHER(value)                  (AES_MR_CIPHER_Msk & ((value) << AES_MR_CIPHER_Pos))
#define AES_MR_GTAGEN_Pos                     (1U)                                           /**< (AES_MR) GCM Automatic Tag Generation Enable Position */
#define AES_MR_GTAGEN_Msk                     (0x1U << AES_MR_GTAGEN_Pos)                    /**< (AES_MR) GCM Automatic Tag Generation Enable Mask */
#define AES_MR_GTAGEN(value)                  (AES_MR_GTAGEN_Msk & ((value) << AES_MR_GTAGEN_Pos))
#define AES_MR_DUALBUFF_Pos                   (3U)                                           /**< (AES_MR) Dual Input Buffer Position */
#define AES_MR_DUALBUFF_Msk                   (0x1U << AES_MR_DUALBUFF_Pos)                  /**< (AES_MR) Dual Input Buffer Mask */
#define AES_MR_DUALBUFF(value)                (AES_MR_DUALBUFF_Msk & ((value) << AES_MR_DUALBUFF_Pos))
#define   AES_MR_DUALBUFF_INACTIVE_Val        (0U)                                           /**< (AES_MR) AES_IDATARx cannot be written during processing of previous block.  */
#define   AES_MR_DUALBUFF_ACTIVE_Val          (1U)                                           /**< (AES_MR) AES_IDATARx can be written during processing of previous block when SMOD = 2. It speeds up the overall runtime of large files.  */
#define AES_MR_DUALBUFF_INACTIVE              (AES_MR_DUALBUFF_INACTIVE_Val << AES_MR_DUALBUFF_Pos) /**< (AES_MR) AES_IDATARx cannot be written during processing of previous block. Position  */
#define AES_MR_DUALBUFF_ACTIVE                (AES_MR_DUALBUFF_ACTIVE_Val << AES_MR_DUALBUFF_Pos) /**< (AES_MR) AES_IDATARx can be written during processing of previous block when SMOD = 2. It speeds up the overall runtime of large files. Position  */
#define AES_MR_PROCDLY_Pos                    (4U)                                           /**< (AES_MR) Processing Delay Position */
#define AES_MR_PROCDLY_Msk                    (0xFU << AES_MR_PROCDLY_Pos)                   /**< (AES_MR) Processing Delay Mask */
#define AES_MR_PROCDLY(value)                 (AES_MR_PROCDLY_Msk & ((value) << AES_MR_PROCDLY_Pos))
#define AES_MR_SMOD_Pos                       (8U)                                           /**< (AES_MR) Start Mode Position */
#define AES_MR_SMOD_Msk                       (0x3U << AES_MR_SMOD_Pos)                      /**< (AES_MR) Start Mode Mask */
#define AES_MR_SMOD(value)                    (AES_MR_SMOD_Msk & ((value) << AES_MR_SMOD_Pos))
#define   AES_MR_SMOD_MANUAL_START_Val        (0U)                                           /**< (AES_MR) Manual Mode  */
#define   AES_MR_SMOD_AUTO_START_Val          (1U)                                           /**< (AES_MR) Auto Mode  */
#define   AES_MR_SMOD_IDATAR0_START_Val       (2U)                                           /**< (AES_MR) AES_IDATAR0 access only Auto Mode (DMA)  */
#define AES_MR_SMOD_MANUAL_START              (AES_MR_SMOD_MANUAL_START_Val << AES_MR_SMOD_Pos) /**< (AES_MR) Manual Mode Position  */
#define AES_MR_SMOD_AUTO_START                (AES_MR_SMOD_AUTO_START_Val << AES_MR_SMOD_Pos) /**< (AES_MR) Auto Mode Position  */
#define AES_MR_SMOD_IDATAR0_START             (AES_MR_SMOD_IDATAR0_START_Val << AES_MR_SMOD_Pos) /**< (AES_MR) AES_IDATAR0 access only Auto Mode (DMA) Position  */
#define AES_MR_KEYSIZE_Pos                    (10U)                                          /**< (AES_MR) Key Size Position */
#define AES_MR_KEYSIZE_Msk                    (0x3U << AES_MR_KEYSIZE_Pos)                   /**< (AES_MR) Key Size Mask */
#define AES_MR_KEYSIZE(value)                 (AES_MR_KEYSIZE_Msk & ((value) << AES_MR_KEYSIZE_Pos))
#define   AES_MR_KEYSIZE_AES128_Val           (0U)                                           /**< (AES_MR) AES Key Size is 128 bits  */
#define   AES_MR_KEYSIZE_AES192_Val           (1U)                                           /**< (AES_MR) AES Key Size is 192 bits  */
#define   AES_MR_KEYSIZE_AES256_Val           (2U)                                           /**< (AES_MR) AES Key Size is 256 bits  */
#define AES_MR_KEYSIZE_AES128                 (AES_MR_KEYSIZE_AES128_Val << AES_MR_KEYSIZE_Pos) /**< (AES_MR) AES Key Size is 128 bits Position  */
#define AES_MR_KEYSIZE_AES192                 (AES_MR_KEYSIZE_AES192_Val << AES_MR_KEYSIZE_Pos) /**< (AES_MR) AES Key Size is 192 bits Position  */
#define AES_MR_KEYSIZE_AES256                 (AES_MR_KEYSIZE_AES256_Val << AES_MR_KEYSIZE_Pos) /**< (AES_MR) AES Key Size is 256 bits Position  */
#define AES_MR_OPMOD_Pos                      (12U)                                          /**< (AES_MR) Operating Mode Position */
#define AES_MR_OPMOD_Msk                      (0x7U << AES_MR_OPMOD_Pos)                     /**< (AES_MR) Operating Mode Mask */
#define AES_MR_OPMOD(value)                   (AES_MR_OPMOD_Msk & ((value) << AES_MR_OPMOD_Pos))
#define   AES_MR_OPMOD_ECB_Val                (0U)                                           /**< (AES_MR) ECB: Electronic Code Book mode  */
#define   AES_MR_OPMOD_CBC_Val                (1U)                                           /**< (AES_MR) CBC: Cipher Block Chaining mode  */
#define   AES_MR_OPMOD_OFB_Val                (2U)                                           /**< (AES_MR) OFB: Output Feedback mode  */
#define   AES_MR_OPMOD_CFB_Val                (3U)                                           /**< (AES_MR) CFB: Cipher Feedback mode  */
#define   AES_MR_OPMOD_CTR_Val                (4U)                                           /**< (AES_MR) CTR: Counter mode (16-bit internal counter)  */
#define   AES_MR_OPMOD_GCM_Val                (5U)                                           /**< (AES_MR) GCM: Galois/Counter mode  */
#define AES_MR_OPMOD_ECB                      (AES_MR_OPMOD_ECB_Val << AES_MR_OPMOD_Pos)     /**< (AES_MR) ECB: Electronic Code Book mode Position  */
#define AES_MR_OPMOD_CBC                      (AES_MR_OPMOD_CBC_Val << AES_MR_OPMOD_Pos)     /**< (AES_MR) CBC: Cipher Block Chaining mode Position  */
#define AES_MR_OPMOD_OFB                      (AES_MR_OPMOD_OFB_Val << AES_MR_OPMOD_Pos)     /**< (AES_MR) OFB: Output Feedback mode Position  */
#define AES_MR_OPMOD_CFB                      (AES_MR_OPMOD_CFB_Val << AES_MR_OPMOD_Pos)     /**< (AES_MR) CFB: Cipher Feedback mode Position  */
#define AES_MR_OPMOD_CTR                      (AES_MR_OPMOD_CTR_Val << AES_MR_OPMOD_Pos)     /**< (AES_MR) CTR: Counter mode (16-bit internal counter) Position  */
#define AES_MR_OPMOD_GCM                      (AES_MR_OPMOD_GCM_Val << AES_MR_OPMOD_Pos)     /**< (AES_MR) GCM: Galois/Counter mode Position  */
#define AES_MR_LOD_Pos                        (15U)                                          /**< (AES_MR) Last Output Data Mode Position */
#define AES_MR_LOD_Msk                        (0x1U << AES_MR_LOD_Pos)                       /**< (AES_MR) Last Output Data Mode Mask */
#define AES_MR_LOD(value)                     (AES_MR_LOD_Msk & ((value) << AES_MR_LOD_Pos))
#define AES_MR_CFBS_Pos                       (16U)                                          /**< (AES_MR) Cipher Feedback Data Size Position */
#define AES_MR_CFBS_Msk                       (0x7U << AES_MR_CFBS_Pos)                      /**< (AES_MR) Cipher Feedback Data Size Mask */
#define AES_MR_CFBS(value)                    (AES_MR_CFBS_Msk & ((value) << AES_MR_CFBS_Pos))
#define   AES_MR_CFBS_SIZE_128BIT_Val         (0U)                                           /**< (AES_MR) 128-bit  */
#define   AES_MR_CFBS_SIZE_64BIT_Val          (1U)                                           /**< (AES_MR) 64-bit  */
#define   AES_MR_CFBS_SIZE_32BIT_Val          (2U)                                           /**< (AES_MR) 32-bit  */
#define   AES_MR_CFBS_SIZE_16BIT_Val          (3U)                                           /**< (AES_MR) 16-bit  */
#define   AES_MR_CFBS_SIZE_8BIT_Val           (4U)                                           /**< (AES_MR) 8-bit  */
#define AES_MR_CFBS_SIZE_128BIT               (AES_MR_CFBS_SIZE_128BIT_Val << AES_MR_CFBS_Pos) /**< (AES_MR) 128-bit Position  */
#define AES_MR_CFBS_SIZE_64BIT                (AES_MR_CFBS_SIZE_64BIT_Val << AES_MR_CFBS_Pos) /**< (AES_MR) 64-bit Position  */
#define AES_MR_CFBS_SIZE_32BIT                (AES_MR_CFBS_SIZE_32BIT_Val << AES_MR_CFBS_Pos) /**< (AES_MR) 32-bit Position  */
#define AES_MR_CFBS_SIZE_16BIT                (AES_MR_CFBS_SIZE_16BIT_Val << AES_MR_CFBS_Pos) /**< (AES_MR) 16-bit Position  */
#define AES_MR_CFBS_SIZE_8BIT                 (AES_MR_CFBS_SIZE_8BIT_Val << AES_MR_CFBS_Pos) /**< (AES_MR) 8-bit Position  */
#define AES_MR_CKEY_Pos                       (20U)                                          /**< (AES_MR) Countermeasure Key Position */
#define AES_MR_CKEY_Msk                       (0xFU << AES_MR_CKEY_Pos)                      /**< (AES_MR) Countermeasure Key Mask */
#define AES_MR_CKEY(value)                    (AES_MR_CKEY_Msk & ((value) << AES_MR_CKEY_Pos))
#define   AES_MR_CKEY_PASSWD_Val              (14U)                                          /**< (AES_MR) This field must be written with 0xE to allow CMTYPx bit configuration changes. Any other values will abort the write operation in CMTYPx bits.Always reads as 0.  */
#define AES_MR_CKEY_PASSWD                    (AES_MR_CKEY_PASSWD_Val << AES_MR_CKEY_Pos)    /**< (AES_MR) This field must be written with 0xE to allow CMTYPx bit configuration changes. Any other values will abort the write operation in CMTYPx bits.Always reads as 0. Position  */
#define AES_MR_Msk                            (0x00F7FFFBUL)                                 /**< (AES_MR) Register Mask  */

/* -------- AES_IER : (AES Offset: 0x10) ( /W 32) Interrupt Enable Register -------- */
#define AES_IER_DATRDY_Pos                    (0U)                                           /**< (AES_IER) Data Ready Interrupt Enable Position */
#define AES_IER_DATRDY_Msk                    (0x1U << AES_IER_DATRDY_Pos)                   /**< (AES_IER) Data Ready Interrupt Enable Mask */
#define AES_IER_DATRDY(value)                 (AES_IER_DATRDY_Msk & ((value) << AES_IER_DATRDY_Pos))
#define AES_IER_URAD_Pos                      (8U)                                           /**< (AES_IER) Unspecified Register Access Detection Interrupt Enable Position */
#define AES_IER_URAD_Msk                      (0x1U << AES_IER_URAD_Pos)                     /**< (AES_IER) Unspecified Register Access Detection Interrupt Enable Mask */
#define AES_IER_URAD(value)                   (AES_IER_URAD_Msk & ((value) << AES_IER_URAD_Pos))
#define AES_IER_TAGRDY_Pos                    (16U)                                          /**< (AES_IER) GCM Tag Ready Interrupt Enable Position */
#define AES_IER_TAGRDY_Msk                    (0x1U << AES_IER_TAGRDY_Pos)                   /**< (AES_IER) GCM Tag Ready Interrupt Enable Mask */
#define AES_IER_TAGRDY(value)                 (AES_IER_TAGRDY_Msk & ((value) << AES_IER_TAGRDY_Pos))
#define AES_IER_Msk                           (0x00010101UL)                                 /**< (AES_IER) Register Mask  */

/* -------- AES_IDR : (AES Offset: 0x14) ( /W 32) Interrupt Disable Register -------- */
#define AES_IDR_DATRDY_Pos                    (0U)                                           /**< (AES_IDR) Data Ready Interrupt Disable Position */
#define AES_IDR_DATRDY_Msk                    (0x1U << AES_IDR_DATRDY_Pos)                   /**< (AES_IDR) Data Ready Interrupt Disable Mask */
#define AES_IDR_DATRDY(value)                 (AES_IDR_DATRDY_Msk & ((value) << AES_IDR_DATRDY_Pos))
#define AES_IDR_URAD_Pos                      (8U)                                           /**< (AES_IDR) Unspecified Register Access Detection Interrupt Disable Position */
#define AES_IDR_URAD_Msk                      (0x1U << AES_IDR_URAD_Pos)                     /**< (AES_IDR) Unspecified Register Access Detection Interrupt Disable Mask */
#define AES_IDR_URAD(value)                   (AES_IDR_URAD_Msk & ((value) << AES_IDR_URAD_Pos))
#define AES_IDR_TAGRDY_Pos                    (16U)                                          /**< (AES_IDR) GCM Tag Ready Interrupt Disable Position */
#define AES_IDR_TAGRDY_Msk                    (0x1U << AES_IDR_TAGRDY_Pos)                   /**< (AES_IDR) GCM Tag Ready Interrupt Disable Mask */
#define AES_IDR_TAGRDY(value)                 (AES_IDR_TAGRDY_Msk & ((value) << AES_IDR_TAGRDY_Pos))
#define AES_IDR_Msk                           (0x00010101UL)                                 /**< (AES_IDR) Register Mask  */

/* -------- AES_IMR : (AES Offset: 0x18) (R/  32) Interrupt Mask Register -------- */
#define AES_IMR_DATRDY_Pos                    (0U)                                           /**< (AES_IMR) Data Ready Interrupt Mask Position */
#define AES_IMR_DATRDY_Msk                    (0x1U << AES_IMR_DATRDY_Pos)                   /**< (AES_IMR) Data Ready Interrupt Mask Mask */
#define AES_IMR_DATRDY(value)                 (AES_IMR_DATRDY_Msk & ((value) << AES_IMR_DATRDY_Pos))
#define AES_IMR_URAD_Pos                      (8U)                                           /**< (AES_IMR) Unspecified Register Access Detection Interrupt Mask Position */
#define AES_IMR_URAD_Msk                      (0x1U << AES_IMR_URAD_Pos)                     /**< (AES_IMR) Unspecified Register Access Detection Interrupt Mask Mask */
#define AES_IMR_URAD(value)                   (AES_IMR_URAD_Msk & ((value) << AES_IMR_URAD_Pos))
#define AES_IMR_TAGRDY_Pos                    (16U)                                          /**< (AES_IMR) GCM Tag Ready Interrupt Mask Position */
#define AES_IMR_TAGRDY_Msk                    (0x1U << AES_IMR_TAGRDY_Pos)                   /**< (AES_IMR) GCM Tag Ready Interrupt Mask Mask */
#define AES_IMR_TAGRDY(value)                 (AES_IMR_TAGRDY_Msk & ((value) << AES_IMR_TAGRDY_Pos))
#define AES_IMR_Msk                           (0x00010101UL)                                 /**< (AES_IMR) Register Mask  */

/* -------- AES_ISR : (AES Offset: 0x1C) (R/  32) Interrupt Status Register -------- */
#define AES_ISR_DATRDY_Pos                    (0U)                                           /**< (AES_ISR) Data Ready (cleared by setting bit START or bit SWRST in AES_CR or by reading AES_ODATARx) Position */
#define AES_ISR_DATRDY_Msk                    (0x1U << AES_ISR_DATRDY_Pos)                   /**< (AES_ISR) Data Ready (cleared by setting bit START or bit SWRST in AES_CR or by reading AES_ODATARx) Mask */
#define AES_ISR_DATRDY(value)                 (AES_ISR_DATRDY_Msk & ((value) << AES_ISR_DATRDY_Pos))
#define AES_ISR_URAD_Pos                      (8U)                                           /**< (AES_ISR) Unspecified Register Access Detection Status (cleared by writing SWRST in AES_CR) Position */
#define AES_ISR_URAD_Msk                      (0x1U << AES_ISR_URAD_Pos)                     /**< (AES_ISR) Unspecified Register Access Detection Status (cleared by writing SWRST in AES_CR) Mask */
#define AES_ISR_URAD(value)                   (AES_ISR_URAD_Msk & ((value) << AES_ISR_URAD_Pos))
#define AES_ISR_URAT_Pos                      (12U)                                          /**< (AES_ISR) Unspecified Register Access (cleared by writing SWRST in AES_CR) Position */
#define AES_ISR_URAT_Msk                      (0xFU << AES_ISR_URAT_Pos)                     /**< (AES_ISR) Unspecified Register Access (cleared by writing SWRST in AES_CR) Mask */
#define AES_ISR_URAT(value)                   (AES_ISR_URAT_Msk & ((value) << AES_ISR_URAT_Pos))
#define   AES_ISR_URAT_IDR_WR_PROCESSING_Val  (0U)                                           /**< (AES_ISR) Input Data Register written during the data processing when SMOD = 0x2 mode.  */
#define   AES_ISR_URAT_ODR_RD_PROCESSING_Val  (1U)                                           /**< (AES_ISR) Output Data Register read during the data processing.  */
#define   AES_ISR_URAT_MR_WR_PROCESSING_Val   (2U)                                           /**< (AES_ISR) Mode Register written during the data processing.  */
#define   AES_ISR_URAT_ODR_RD_SUBKGEN_Val     (3U)                                           /**< (AES_ISR) Output Data Register read during the sub-keys generation.  */
#define   AES_ISR_URAT_MR_WR_SUBKGEN_Val      (4U)                                           /**< (AES_ISR) Mode Register written during the sub-keys generation.  */
#define   AES_ISR_URAT_WOR_RD_ACCESS_Val      (5U)                                           /**< (AES_ISR) Write-only register read access.  */
#define AES_ISR_URAT_IDR_WR_PROCESSING        (AES_ISR_URAT_IDR_WR_PROCESSING_Val << AES_ISR_URAT_Pos) /**< (AES_ISR) Input Data Register written during the data processing when SMOD = 0x2 mode. Position  */
#define AES_ISR_URAT_ODR_RD_PROCESSING        (AES_ISR_URAT_ODR_RD_PROCESSING_Val << AES_ISR_URAT_Pos) /**< (AES_ISR) Output Data Register read during the data processing. Position  */
#define AES_ISR_URAT_MR_WR_PROCESSING         (AES_ISR_URAT_MR_WR_PROCESSING_Val << AES_ISR_URAT_Pos) /**< (AES_ISR) Mode Register written during the data processing. Position  */
#define AES_ISR_URAT_ODR_RD_SUBKGEN           (AES_ISR_URAT_ODR_RD_SUBKGEN_Val << AES_ISR_URAT_Pos) /**< (AES_ISR) Output Data Register read during the sub-keys generation. Position  */
#define AES_ISR_URAT_MR_WR_SUBKGEN            (AES_ISR_URAT_MR_WR_SUBKGEN_Val << AES_ISR_URAT_Pos) /**< (AES_ISR) Mode Register written during the sub-keys generation. Position  */
#define AES_ISR_URAT_WOR_RD_ACCESS            (AES_ISR_URAT_WOR_RD_ACCESS_Val << AES_ISR_URAT_Pos) /**< (AES_ISR) Write-only register read access. Position  */
#define AES_ISR_TAGRDY_Pos                    (16U)                                          /**< (AES_ISR) GCM Tag Ready Position */
#define AES_ISR_TAGRDY_Msk                    (0x1U << AES_ISR_TAGRDY_Pos)                   /**< (AES_ISR) GCM Tag Ready Mask */
#define AES_ISR_TAGRDY(value)                 (AES_ISR_TAGRDY_Msk & ((value) << AES_ISR_TAGRDY_Pos))
#define AES_ISR_Msk                           (0x0001F101UL)                                 /**< (AES_ISR) Register Mask  */

/* -------- AES_KEYWR : (AES Offset: 0x20) ( /W 32) Key Word Register -------- */
#define AES_KEYWR_KEYW_Pos                    (0U)                                           /**< (AES_KEYWR) Key Word Position */
#define AES_KEYWR_KEYW_Msk                    (0xFFFFFFFFU << AES_KEYWR_KEYW_Pos)            /**< (AES_KEYWR) Key Word Mask */
#define AES_KEYWR_KEYW(value)                 (AES_KEYWR_KEYW_Msk & ((value) << AES_KEYWR_KEYW_Pos))
#define AES_KEYWR_Msk                         (0xFFFFFFFFUL)                                 /**< (AES_KEYWR) Register Mask  */

/* -------- AES_IDATAR : (AES Offset: 0x40) ( /W 32) Input Data Register -------- */
#define AES_IDATAR_IDATA_Pos                  (0U)                                           /**< (AES_IDATAR) Input Data Word Position */
#define AES_IDATAR_IDATA_Msk                  (0xFFFFFFFFU << AES_IDATAR_IDATA_Pos)          /**< (AES_IDATAR) Input Data Word Mask */
#define AES_IDATAR_IDATA(value)               (AES_IDATAR_IDATA_Msk & ((value) << AES_IDATAR_IDATA_Pos))
#define AES_IDATAR_Msk                        (0xFFFFFFFFUL)                                 /**< (AES_IDATAR) Register Mask  */

/* -------- AES_ODATAR : (AES Offset: 0x50) (R/  32) Output Data Register -------- */
#define AES_ODATAR_ODATA_Pos                  (0U)                                           /**< (AES_ODATAR) Output Data Position */
#define AES_ODATAR_ODATA_Msk                  (0xFFFFFFFFU << AES_ODATAR_ODATA_Pos)          /**< (AES_ODATAR) Output Data Mask */
#define AES_ODATAR_ODATA(value)               (AES_ODATAR_ODATA_Msk & ((value) << AES_ODATAR_ODATA_Pos))
#define AES_ODATAR_Msk                        (0xFFFFFFFFUL)                                 /**< (AES_ODATAR) Register Mask  */

/* -------- AES_IVR : (AES Offset: 0x60) ( /W 32) Initialization Vector Register -------- */
#define AES_IVR_IV_Pos                        (0U)                                           /**< (AES_IVR) Initialization Vector Position */
#define AES_IVR_IV_Msk                        (0xFFFFFFFFU << AES_IVR_IV_Pos)                /**< (AES_IVR) Initialization Vector Mask */
#define AES_IVR_IV(value)                     (AES_IVR_IV_Msk & ((value) << AES_IVR_IV_Pos))
#define AES_IVR_Msk                           (0xFFFFFFFFUL)                                 /**< (AES_IVR) Register Mask  */

/* -------- AES_AADLENR : (AES Offset: 0x70) (R/W 32) Additional Authenticated Data Length Register -------- */
#define AES_AADLENR_AADLEN_Pos                (0U)                                           /**< (AES_AADLENR) Additional Authenticated Data Length Position */
#define AES_AADLENR_AADLEN_Msk                (0xFFFFFFFFU << AES_AADLENR_AADLEN_Pos)        /**< (AES_AADLENR) Additional Authenticated Data Length Mask */
#define AES_AADLENR_AADLEN(value)             (AES_AADLENR_AADLEN_Msk & ((value) << AES_AADLENR_AADLEN_Pos))
#define AES_AADLENR_Msk                       (0xFFFFFFFFUL)                                 /**< (AES_AADLENR) Register Mask  */

/* -------- AES_CLENR : (AES Offset: 0x74) (R/W 32) Plaintext/Ciphertext Length Register -------- */
#define AES_CLENR_CLEN_Pos                    (0U)                                           /**< (AES_CLENR) Plaintext/Ciphertext Length Position */
#define AES_CLENR_CLEN_Msk                    (0xFFFFFFFFU << AES_CLENR_CLEN_Pos)            /**< (AES_CLENR) Plaintext/Ciphertext Length Mask */
#define AES_CLENR_CLEN(value)                 (AES_CLENR_CLEN_Msk & ((value) << AES_CLENR_CLEN_Pos))
#define AES_CLENR_Msk                         (0xFFFFFFFFUL)                                 /**< (AES_CLENR) Register Mask  */

/* -------- AES_GHASHR : (AES Offset: 0x78) (R/W 32) GCM Intermediate Hash Word Register -------- */
#define AES_GHASHR_GHASH_Pos                  (0U)                                           /**< (AES_GHASHR) Intermediate GCM Hash Word x Position */
#define AES_GHASHR_GHASH_Msk                  (0xFFFFFFFFU << AES_GHASHR_GHASH_Pos)          /**< (AES_GHASHR) Intermediate GCM Hash Word x Mask */
#define AES_GHASHR_GHASH(value)               (AES_GHASHR_GHASH_Msk & ((value) << AES_GHASHR_GHASH_Pos))
#define AES_GHASHR_Msk                        (0xFFFFFFFFUL)                                 /**< (AES_GHASHR) Register Mask  */

/* -------- AES_TAGR : (AES Offset: 0x88) (R/  32) GCM Authentication Tag Word Register -------- */
#define AES_TAGR_TAG_Pos                      (0U)                                           /**< (AES_TAGR) GCM Authentication Tag x Position */
#define AES_TAGR_TAG_Msk                      (0xFFFFFFFFU << AES_TAGR_TAG_Pos)              /**< (AES_TAGR) GCM Authentication Tag x Mask */
#define AES_TAGR_TAG(value)                   (AES_TAGR_TAG_Msk & ((value) << AES_TAGR_TAG_Pos))
#define AES_TAGR_Msk                          (0xFFFFFFFFUL)                                 /**< (AES_TAGR) Register Mask  */

/* -------- AES_CTRR : (AES Offset: 0x98) (R/  32) GCM Encryption Counter Value Register -------- */
#define AES_CTRR_CTR_Pos                      (0U)                                           /**< (AES_CTRR) GCM Encryption Counter Position */
#define AES_CTRR_CTR_Msk                      (0xFFFFFFFFU << AES_CTRR_CTR_Pos)              /**< (AES_CTRR) GCM Encryption Counter Mask */
#define AES_CTRR_CTR(value)                   (AES_CTRR_CTR_Msk & ((value) << AES_CTRR_CTR_Pos))
#define AES_CTRR_Msk                          (0xFFFFFFFFUL)                                 /**< (AES_CTRR) Register Mask  */

/* -------- AES_GCMHR : (AES Offset: 0x9C) (R/W 32) GCM H Word Register -------- */
#define AES_GCMHR_H_Pos                       (0U)                                           /**< (AES_GCMHR) GCM H Word x Position */
#define AES_GCMHR_H_Msk                       (0xFFFFFFFFU << AES_GCMHR_H_Pos)               /**< (AES_GCMHR) GCM H Word x Mask */
#define AES_GCMHR_H(value)                    (AES_GCMHR_H_Msk & ((value) << AES_GCMHR_H_Pos))
#define AES_GCMHR_Msk                         (0xFFFFFFFFUL)                                 /**< (AES_GCMHR) Register Mask  */

/** \brief AES register offsets definitions */
#define AES_CR_OFFSET                  (0x00)         /**< (AES_CR) Control Register Offset */
#define AES_MR_OFFSET                  (0x04)         /**< (AES_MR) Mode Register Offset */
#define AES_IER_OFFSET                 (0x10)         /**< (AES_IER) Interrupt Enable Register Offset */
#define AES_IDR_OFFSET                 (0x14)         /**< (AES_IDR) Interrupt Disable Register Offset */
#define AES_IMR_OFFSET                 (0x18)         /**< (AES_IMR) Interrupt Mask Register Offset */
#define AES_ISR_OFFSET                 (0x1C)         /**< (AES_ISR) Interrupt Status Register Offset */
#define AES_KEYWR_OFFSET               (0x20)         /**< (AES_KEYWR) Key Word Register Offset */
#define AES_IDATAR_OFFSET              (0x40)         /**< (AES_IDATAR) Input Data Register Offset */
#define AES_ODATAR_OFFSET              (0x50)         /**< (AES_ODATAR) Output Data Register Offset */
#define AES_IVR_OFFSET                 (0x60)         /**< (AES_IVR) Initialization Vector Register Offset */
#define AES_AADLENR_OFFSET             (0x70)         /**< (AES_AADLENR) Additional Authenticated Data Length Register Offset */
#define AES_CLENR_OFFSET               (0x74)         /**< (AES_CLENR) Plaintext/Ciphertext Length Register Offset */
#define AES_GHASHR_OFFSET              (0x78)         /**< (AES_GHASHR) GCM Intermediate Hash Word Register Offset */
#define AES_TAGR_OFFSET                (0x88)         /**< (AES_TAGR) GCM Authentication Tag Word Register Offset */
#define AES_CTRR_OFFSET                (0x98)         /**< (AES_CTRR) GCM Encryption Counter Value Register Offset */
#define AES_GCMHR_OFFSET               (0x9C)         /**< (AES_GCMHR) GCM H Word Register Offset */

/** \brief AES register API structure */
typedef struct
{
  __O   uint32_t                       AES_CR;          /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       AES_MR;          /**< Offset: 0x04 (R/W  32) Mode Register */
  __I   uint8_t                        Reserved1[0x08];
  __O   uint32_t                       AES_IER;         /**< Offset: 0x10 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       AES_IDR;         /**< Offset: 0x14 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       AES_IMR;         /**< Offset: 0x18 (R/   32) Interrupt Mask Register */
  __I   uint32_t                       AES_ISR;         /**< Offset: 0x1c (R/   32) Interrupt Status Register */
  __O   uint32_t                       AES_KEYWR[8];    /**< Offset: 0x20 ( /W  32) Key Word Register */
  __O   uint32_t                       AES_IDATAR[4];   /**< Offset: 0x40 ( /W  32) Input Data Register */
  __I   uint32_t                       AES_ODATAR[4];   /**< Offset: 0x50 (R/   32) Output Data Register */
  __O   uint32_t                       AES_IVR[4];      /**< Offset: 0x60 ( /W  32) Initialization Vector Register */
  __IO  uint32_t                       AES_AADLENR;     /**< Offset: 0x70 (R/W  32) Additional Authenticated Data Length Register */
  __IO  uint32_t                       AES_CLENR;       /**< Offset: 0x74 (R/W  32) Plaintext/Ciphertext Length Register */
  __IO  uint32_t                       AES_GHASHR[4];   /**< Offset: 0x78 (R/W  32) GCM Intermediate Hash Word Register */
  __I   uint32_t                       AES_TAGR[4];     /**< Offset: 0x88 (R/   32) GCM Authentication Tag Word Register */
  __I   uint32_t                       AES_CTRR;        /**< Offset: 0x98 (R/   32) GCM Encryption Counter Value Register */
  __IO  uint32_t                       AES_GCMHR[4];    /**< Offset: 0x9c (R/W  32) GCM H Word Register */
} aes_registers_t;
/** @}  end of Advanced Encryption Standard */

#endif /* SAME_SAME70_AES_MODULE_H */

