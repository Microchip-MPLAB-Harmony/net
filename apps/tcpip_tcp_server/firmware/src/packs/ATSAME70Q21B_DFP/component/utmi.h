/**
 * \brief Header file for SAME/SAME70 UTMI
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
#ifndef SAME_SAME70_UTMI_MODULE_H
#define SAME_SAME70_UTMI_MODULE_H

/** \addtogroup SAME_SAME70 USB Transmitter Interface Macrocell
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_UTMI                                  */
/* ========================================================================== */

/* -------- UTMI_OHCIICR : (UTMI Offset: 0x10) (R/W 32) OHCI Interrupt Configuration Register -------- */
#define UTMI_OHCIICR_RES0_Pos                 (0U)                                           /**< (UTMI_OHCIICR) USB PORTx Reset Position */
#define UTMI_OHCIICR_RES0_Msk                 (0x1U << UTMI_OHCIICR_RES0_Pos)                /**< (UTMI_OHCIICR) USB PORTx Reset Mask */
#define UTMI_OHCIICR_RES0(value)              (UTMI_OHCIICR_RES0_Msk & ((value) << UTMI_OHCIICR_RES0_Pos))
#define UTMI_OHCIICR_ARIE_Pos                 (4U)                                           /**< (UTMI_OHCIICR) OHCI Asynchronous Resume Interrupt Enable Position */
#define UTMI_OHCIICR_ARIE_Msk                 (0x1U << UTMI_OHCIICR_ARIE_Pos)                /**< (UTMI_OHCIICR) OHCI Asynchronous Resume Interrupt Enable Mask */
#define UTMI_OHCIICR_ARIE(value)              (UTMI_OHCIICR_ARIE_Msk & ((value) << UTMI_OHCIICR_ARIE_Pos))
#define UTMI_OHCIICR_APPSTART_Pos             (5U)                                           /**< (UTMI_OHCIICR) Reserved Position */
#define UTMI_OHCIICR_APPSTART_Msk             (0x1U << UTMI_OHCIICR_APPSTART_Pos)            /**< (UTMI_OHCIICR) Reserved Mask */
#define UTMI_OHCIICR_APPSTART(value)          (UTMI_OHCIICR_APPSTART_Msk & ((value) << UTMI_OHCIICR_APPSTART_Pos))
#define UTMI_OHCIICR_UDPPUDIS_Pos             (23U)                                          /**< (UTMI_OHCIICR) USB Device Pull-up Disable Position */
#define UTMI_OHCIICR_UDPPUDIS_Msk             (0x1U << UTMI_OHCIICR_UDPPUDIS_Pos)            /**< (UTMI_OHCIICR) USB Device Pull-up Disable Mask */
#define UTMI_OHCIICR_UDPPUDIS(value)          (UTMI_OHCIICR_UDPPUDIS_Msk & ((value) << UTMI_OHCIICR_UDPPUDIS_Pos))
#define UTMI_OHCIICR_Msk                      (0x00800031UL)                                 /**< (UTMI_OHCIICR) Register Mask  */

/* -------- UTMI_CKTRIM : (UTMI Offset: 0x30) (R/W 32) UTMI Clock Trimming Register -------- */
#define UTMI_CKTRIM_FREQ_Pos                  (0U)                                           /**< (UTMI_CKTRIM) UTMI Reference Clock Frequency Position */
#define UTMI_CKTRIM_FREQ_Msk                  (0x3U << UTMI_CKTRIM_FREQ_Pos)                 /**< (UTMI_CKTRIM) UTMI Reference Clock Frequency Mask */
#define UTMI_CKTRIM_FREQ(value)               (UTMI_CKTRIM_FREQ_Msk & ((value) << UTMI_CKTRIM_FREQ_Pos))
#define   UTMI_CKTRIM_FREQ_XTAL12_Val         (0U)                                           /**< (UTMI_CKTRIM) 12 MHz reference clock  */
#define   UTMI_CKTRIM_FREQ_XTAL16_Val         (1U)                                           /**< (UTMI_CKTRIM) 16 MHz reference clock  */
#define UTMI_CKTRIM_FREQ_XTAL12               (UTMI_CKTRIM_FREQ_XTAL12_Val << UTMI_CKTRIM_FREQ_Pos) /**< (UTMI_CKTRIM) 12 MHz reference clock Position  */
#define UTMI_CKTRIM_FREQ_XTAL16               (UTMI_CKTRIM_FREQ_XTAL16_Val << UTMI_CKTRIM_FREQ_Pos) /**< (UTMI_CKTRIM) 16 MHz reference clock Position  */
#define UTMI_CKTRIM_Msk                       (0x00000003UL)                                 /**< (UTMI_CKTRIM) Register Mask  */

/** \brief UTMI register offsets definitions */
#define UTMI_OHCIICR_OFFSET            (0x10)         /**< (UTMI_OHCIICR) OHCI Interrupt Configuration Register Offset */
#define UTMI_CKTRIM_OFFSET             (0x30)         /**< (UTMI_CKTRIM) UTMI Clock Trimming Register Offset */

/** \brief UTMI register API structure */
typedef struct
{
  __I   uint8_t                        Reserved1[0x10];
  __IO  uint32_t                       UTMI_OHCIICR;    /**< Offset: 0x10 (R/W  32) OHCI Interrupt Configuration Register */
  __I   uint8_t                        Reserved2[0x1C];
  __IO  uint32_t                       UTMI_CKTRIM;     /**< Offset: 0x30 (R/W  32) UTMI Clock Trimming Register */
} utmi_registers_t;
/** @}  end of USB Transmitter Interface Macrocell */

#endif /* SAME_SAME70_UTMI_MODULE_H */

