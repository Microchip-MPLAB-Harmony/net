/**
 * \brief Header file for SAME/SAME70 TRNG
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
#ifndef SAME_SAME70_TRNG_MODULE_H
#define SAME_SAME70_TRNG_MODULE_H

/** \addtogroup SAME_SAME70 True Random Number Generator
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_TRNG                                  */
/* ========================================================================== */

/* -------- TRNG_CR : (TRNG Offset: 0x00) ( /W 32) Control Register -------- */
#define TRNG_CR_ENABLE_Pos                    (0U)                                           /**< (TRNG_CR) Enables the TRNG to Provide Random Values Position */
#define TRNG_CR_ENABLE_Msk                    (0x1U << TRNG_CR_ENABLE_Pos)                   /**< (TRNG_CR) Enables the TRNG to Provide Random Values Mask */
#define TRNG_CR_ENABLE(value)                 (TRNG_CR_ENABLE_Msk & ((value) << TRNG_CR_ENABLE_Pos))
#define TRNG_CR_KEY_Pos                       (8U)                                           /**< (TRNG_CR) Security Key Position */
#define TRNG_CR_KEY_Msk                       (0xFFFFFFU << TRNG_CR_KEY_Pos)                 /**< (TRNG_CR) Security Key Mask */
#define TRNG_CR_KEY(value)                    (TRNG_CR_KEY_Msk & ((value) << TRNG_CR_KEY_Pos))
#define   TRNG_CR_KEY_PASSWD_Val              (5393991U)                                     /**< (TRNG_CR) Writing any other value in this field aborts the write operation.  */
#define TRNG_CR_KEY_PASSWD                    (TRNG_CR_KEY_PASSWD_Val << TRNG_CR_KEY_Pos)    /**< (TRNG_CR) Writing any other value in this field aborts the write operation. Position  */
#define TRNG_CR_Msk                           (0xFFFFFF01UL)                                 /**< (TRNG_CR) Register Mask  */

/* -------- TRNG_IER : (TRNG Offset: 0x10) ( /W 32) Interrupt Enable Register -------- */
#define TRNG_IER_DATRDY_Pos                   (0U)                                           /**< (TRNG_IER) Data Ready Interrupt Enable Position */
#define TRNG_IER_DATRDY_Msk                   (0x1U << TRNG_IER_DATRDY_Pos)                  /**< (TRNG_IER) Data Ready Interrupt Enable Mask */
#define TRNG_IER_DATRDY(value)                (TRNG_IER_DATRDY_Msk & ((value) << TRNG_IER_DATRDY_Pos))
#define TRNG_IER_Msk                          (0x00000001UL)                                 /**< (TRNG_IER) Register Mask  */

/* -------- TRNG_IDR : (TRNG Offset: 0x14) ( /W 32) Interrupt Disable Register -------- */
#define TRNG_IDR_DATRDY_Pos                   (0U)                                           /**< (TRNG_IDR) Data Ready Interrupt Disable Position */
#define TRNG_IDR_DATRDY_Msk                   (0x1U << TRNG_IDR_DATRDY_Pos)                  /**< (TRNG_IDR) Data Ready Interrupt Disable Mask */
#define TRNG_IDR_DATRDY(value)                (TRNG_IDR_DATRDY_Msk & ((value) << TRNG_IDR_DATRDY_Pos))
#define TRNG_IDR_Msk                          (0x00000001UL)                                 /**< (TRNG_IDR) Register Mask  */

/* -------- TRNG_IMR : (TRNG Offset: 0x18) (R/  32) Interrupt Mask Register -------- */
#define TRNG_IMR_DATRDY_Pos                   (0U)                                           /**< (TRNG_IMR) Data Ready Interrupt Mask Position */
#define TRNG_IMR_DATRDY_Msk                   (0x1U << TRNG_IMR_DATRDY_Pos)                  /**< (TRNG_IMR) Data Ready Interrupt Mask Mask */
#define TRNG_IMR_DATRDY(value)                (TRNG_IMR_DATRDY_Msk & ((value) << TRNG_IMR_DATRDY_Pos))
#define TRNG_IMR_Msk                          (0x00000001UL)                                 /**< (TRNG_IMR) Register Mask  */

/* -------- TRNG_ISR : (TRNG Offset: 0x1C) (R/  32) Interrupt Status Register -------- */
#define TRNG_ISR_DATRDY_Pos                   (0U)                                           /**< (TRNG_ISR) Data Ready Position */
#define TRNG_ISR_DATRDY_Msk                   (0x1U << TRNG_ISR_DATRDY_Pos)                  /**< (TRNG_ISR) Data Ready Mask */
#define TRNG_ISR_DATRDY(value)                (TRNG_ISR_DATRDY_Msk & ((value) << TRNG_ISR_DATRDY_Pos))
#define TRNG_ISR_Msk                          (0x00000001UL)                                 /**< (TRNG_ISR) Register Mask  */

/* -------- TRNG_ODATA : (TRNG Offset: 0x50) (R/  32) Output Data Register -------- */
#define TRNG_ODATA_ODATA_Pos                  (0U)                                           /**< (TRNG_ODATA) Output Data Position */
#define TRNG_ODATA_ODATA_Msk                  (0xFFFFFFFFU << TRNG_ODATA_ODATA_Pos)          /**< (TRNG_ODATA) Output Data Mask */
#define TRNG_ODATA_ODATA(value)               (TRNG_ODATA_ODATA_Msk & ((value) << TRNG_ODATA_ODATA_Pos))
#define TRNG_ODATA_Msk                        (0xFFFFFFFFUL)                                 /**< (TRNG_ODATA) Register Mask  */

/** \brief TRNG register offsets definitions */
#define TRNG_CR_OFFSET                 (0x00)         /**< (TRNG_CR) Control Register Offset */
#define TRNG_IER_OFFSET                (0x10)         /**< (TRNG_IER) Interrupt Enable Register Offset */
#define TRNG_IDR_OFFSET                (0x14)         /**< (TRNG_IDR) Interrupt Disable Register Offset */
#define TRNG_IMR_OFFSET                (0x18)         /**< (TRNG_IMR) Interrupt Mask Register Offset */
#define TRNG_ISR_OFFSET                (0x1C)         /**< (TRNG_ISR) Interrupt Status Register Offset */
#define TRNG_ODATA_OFFSET              (0x50)         /**< (TRNG_ODATA) Output Data Register Offset */

/** \brief TRNG register API structure */
typedef struct
{
  __O   uint32_t                       TRNG_CR;         /**< Offset: 0x00 ( /W  32) Control Register */
  __I   uint8_t                        Reserved1[0x0C];
  __O   uint32_t                       TRNG_IER;        /**< Offset: 0x10 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       TRNG_IDR;        /**< Offset: 0x14 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       TRNG_IMR;        /**< Offset: 0x18 (R/   32) Interrupt Mask Register */
  __I   uint32_t                       TRNG_ISR;        /**< Offset: 0x1c (R/   32) Interrupt Status Register */
  __I   uint8_t                        Reserved2[0x30];
  __I   uint32_t                       TRNG_ODATA;      /**< Offset: 0x50 (R/   32) Output Data Register */
} trng_registers_t;
/** @}  end of True Random Number Generator */

#endif /* SAME_SAME70_TRNG_MODULE_H */

