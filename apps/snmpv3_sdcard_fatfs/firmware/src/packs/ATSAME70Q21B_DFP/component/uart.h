/**
 * \brief Header file for SAME/SAME70 UART
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
#ifndef SAME_SAME70_UART_MODULE_H
#define SAME_SAME70_UART_MODULE_H

/** \addtogroup SAME_SAME70 Universal Asynchronous Receiver Transmitter
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_UART                                  */
/* ========================================================================== */

/* -------- UART_CR : (UART Offset: 0x00) ( /W 32) Control Register -------- */
#define UART_CR_RSTRX_Pos                     (2U)                                           /**< (UART_CR) Reset Receiver Position */
#define UART_CR_RSTRX_Msk                     (0x1U << UART_CR_RSTRX_Pos)                    /**< (UART_CR) Reset Receiver Mask */
#define UART_CR_RSTRX(value)                  (UART_CR_RSTRX_Msk & ((value) << UART_CR_RSTRX_Pos))
#define UART_CR_RSTTX_Pos                     (3U)                                           /**< (UART_CR) Reset Transmitter Position */
#define UART_CR_RSTTX_Msk                     (0x1U << UART_CR_RSTTX_Pos)                    /**< (UART_CR) Reset Transmitter Mask */
#define UART_CR_RSTTX(value)                  (UART_CR_RSTTX_Msk & ((value) << UART_CR_RSTTX_Pos))
#define UART_CR_RXEN_Pos                      (4U)                                           /**< (UART_CR) Receiver Enable Position */
#define UART_CR_RXEN_Msk                      (0x1U << UART_CR_RXEN_Pos)                     /**< (UART_CR) Receiver Enable Mask */
#define UART_CR_RXEN(value)                   (UART_CR_RXEN_Msk & ((value) << UART_CR_RXEN_Pos))
#define UART_CR_RXDIS_Pos                     (5U)                                           /**< (UART_CR) Receiver Disable Position */
#define UART_CR_RXDIS_Msk                     (0x1U << UART_CR_RXDIS_Pos)                    /**< (UART_CR) Receiver Disable Mask */
#define UART_CR_RXDIS(value)                  (UART_CR_RXDIS_Msk & ((value) << UART_CR_RXDIS_Pos))
#define UART_CR_TXEN_Pos                      (6U)                                           /**< (UART_CR) Transmitter Enable Position */
#define UART_CR_TXEN_Msk                      (0x1U << UART_CR_TXEN_Pos)                     /**< (UART_CR) Transmitter Enable Mask */
#define UART_CR_TXEN(value)                   (UART_CR_TXEN_Msk & ((value) << UART_CR_TXEN_Pos))
#define UART_CR_TXDIS_Pos                     (7U)                                           /**< (UART_CR) Transmitter Disable Position */
#define UART_CR_TXDIS_Msk                     (0x1U << UART_CR_TXDIS_Pos)                    /**< (UART_CR) Transmitter Disable Mask */
#define UART_CR_TXDIS(value)                  (UART_CR_TXDIS_Msk & ((value) << UART_CR_TXDIS_Pos))
#define UART_CR_RSTSTA_Pos                    (8U)                                           /**< (UART_CR) Reset Status Position */
#define UART_CR_RSTSTA_Msk                    (0x1U << UART_CR_RSTSTA_Pos)                   /**< (UART_CR) Reset Status Mask */
#define UART_CR_RSTSTA(value)                 (UART_CR_RSTSTA_Msk & ((value) << UART_CR_RSTSTA_Pos))
#define UART_CR_REQCLR_Pos                    (12U)                                          /**< (UART_CR) Request Clear Position */
#define UART_CR_REQCLR_Msk                    (0x1U << UART_CR_REQCLR_Pos)                   /**< (UART_CR) Request Clear Mask */
#define UART_CR_REQCLR(value)                 (UART_CR_REQCLR_Msk & ((value) << UART_CR_REQCLR_Pos))
#define UART_CR_Msk                           (0x000011FCUL)                                 /**< (UART_CR) Register Mask  */

/* -------- UART_MR : (UART Offset: 0x04) (R/W 32) Mode Register -------- */
#define UART_MR_FILTER_Pos                    (4U)                                           /**< (UART_MR) Receiver Digital Filter Position */
#define UART_MR_FILTER_Msk                    (0x1U << UART_MR_FILTER_Pos)                   /**< (UART_MR) Receiver Digital Filter Mask */
#define UART_MR_FILTER(value)                 (UART_MR_FILTER_Msk & ((value) << UART_MR_FILTER_Pos))
#define   UART_MR_FILTER_DISABLED_Val         (0U)                                           /**< (UART_MR) UART does not filter the receive line.  */
#define   UART_MR_FILTER_ENABLED_Val          (1U)                                           /**< (UART_MR) UART filters the receive line using a three-sample filter (16x-bit clock) (2 over 3 majority).  */
#define UART_MR_FILTER_DISABLED               (UART_MR_FILTER_DISABLED_Val << UART_MR_FILTER_Pos) /**< (UART_MR) UART does not filter the receive line. Position  */
#define UART_MR_FILTER_ENABLED                (UART_MR_FILTER_ENABLED_Val << UART_MR_FILTER_Pos) /**< (UART_MR) UART filters the receive line using a three-sample filter (16x-bit clock) (2 over 3 majority). Position  */
#define UART_MR_PAR_Pos                       (9U)                                           /**< (UART_MR) Parity Type Position */
#define UART_MR_PAR_Msk                       (0x7U << UART_MR_PAR_Pos)                      /**< (UART_MR) Parity Type Mask */
#define UART_MR_PAR(value)                    (UART_MR_PAR_Msk & ((value) << UART_MR_PAR_Pos))
#define   UART_MR_PAR_EVEN_Val                (0U)                                           /**< (UART_MR) Even Parity  */
#define   UART_MR_PAR_ODD_Val                 (1U)                                           /**< (UART_MR) Odd Parity  */
#define   UART_MR_PAR_SPACE_Val               (2U)                                           /**< (UART_MR) Space: parity forced to 0  */
#define   UART_MR_PAR_MARK_Val                (3U)                                           /**< (UART_MR) Mark: parity forced to 1  */
#define   UART_MR_PAR_NO_Val                  (4U)                                           /**< (UART_MR) No parity  */
#define UART_MR_PAR_EVEN                      (UART_MR_PAR_EVEN_Val << UART_MR_PAR_Pos)      /**< (UART_MR) Even Parity Position  */
#define UART_MR_PAR_ODD                       (UART_MR_PAR_ODD_Val << UART_MR_PAR_Pos)       /**< (UART_MR) Odd Parity Position  */
#define UART_MR_PAR_SPACE                     (UART_MR_PAR_SPACE_Val << UART_MR_PAR_Pos)     /**< (UART_MR) Space: parity forced to 0 Position  */
#define UART_MR_PAR_MARK                      (UART_MR_PAR_MARK_Val << UART_MR_PAR_Pos)      /**< (UART_MR) Mark: parity forced to 1 Position  */
#define UART_MR_PAR_NO                        (UART_MR_PAR_NO_Val << UART_MR_PAR_Pos)        /**< (UART_MR) No parity Position  */
#define UART_MR_BRSRCCK_Pos                   (12U)                                          /**< (UART_MR) Baud Rate Source Clock Position */
#define UART_MR_BRSRCCK_Msk                   (0x1U << UART_MR_BRSRCCK_Pos)                  /**< (UART_MR) Baud Rate Source Clock Mask */
#define UART_MR_BRSRCCK(value)                (UART_MR_BRSRCCK_Msk & ((value) << UART_MR_BRSRCCK_Pos))
#define   UART_MR_BRSRCCK_PERIPH_CLK_Val      (0U)                                           /**< (UART_MR) The baud rate is driven by the peripheral clock  */
#define   UART_MR_BRSRCCK_PMC_PCK_Val         (1U)                                           /**< (UART_MR) The baud rate is driven by a PMC-programmable clock PCK (see section Power Management Controller (PMC)).  */
#define UART_MR_BRSRCCK_PERIPH_CLK            (UART_MR_BRSRCCK_PERIPH_CLK_Val << UART_MR_BRSRCCK_Pos) /**< (UART_MR) The baud rate is driven by the peripheral clock Position  */
#define UART_MR_BRSRCCK_PMC_PCK               (UART_MR_BRSRCCK_PMC_PCK_Val << UART_MR_BRSRCCK_Pos) /**< (UART_MR) The baud rate is driven by a PMC-programmable clock PCK (see section Power Management Controller (PMC)). Position  */
#define UART_MR_CHMODE_Pos                    (14U)                                          /**< (UART_MR) Channel Mode Position */
#define UART_MR_CHMODE_Msk                    (0x3U << UART_MR_CHMODE_Pos)                   /**< (UART_MR) Channel Mode Mask */
#define UART_MR_CHMODE(value)                 (UART_MR_CHMODE_Msk & ((value) << UART_MR_CHMODE_Pos))
#define   UART_MR_CHMODE_NORMAL_Val           (0U)                                           /**< (UART_MR) Normal mode  */
#define   UART_MR_CHMODE_AUTOMATIC_Val        (1U)                                           /**< (UART_MR) Automatic echo  */
#define   UART_MR_CHMODE_LOCAL_LOOPBACK_Val   (2U)                                           /**< (UART_MR) Local loopback  */
#define   UART_MR_CHMODE_REMOTE_LOOPBACK_Val  (3U)                                           /**< (UART_MR) Remote loopback  */
#define UART_MR_CHMODE_NORMAL                 (UART_MR_CHMODE_NORMAL_Val << UART_MR_CHMODE_Pos) /**< (UART_MR) Normal mode Position  */
#define UART_MR_CHMODE_AUTOMATIC              (UART_MR_CHMODE_AUTOMATIC_Val << UART_MR_CHMODE_Pos) /**< (UART_MR) Automatic echo Position  */
#define UART_MR_CHMODE_LOCAL_LOOPBACK         (UART_MR_CHMODE_LOCAL_LOOPBACK_Val << UART_MR_CHMODE_Pos) /**< (UART_MR) Local loopback Position  */
#define UART_MR_CHMODE_REMOTE_LOOPBACK        (UART_MR_CHMODE_REMOTE_LOOPBACK_Val << UART_MR_CHMODE_Pos) /**< (UART_MR) Remote loopback Position  */
#define UART_MR_Msk                           (0x0000DE10UL)                                 /**< (UART_MR) Register Mask  */

/* -------- UART_IER : (UART Offset: 0x08) ( /W 32) Interrupt Enable Register -------- */
#define UART_IER_RXRDY_Pos                    (0U)                                           /**< (UART_IER) Enable RXRDY Interrupt Position */
#define UART_IER_RXRDY_Msk                    (0x1U << UART_IER_RXRDY_Pos)                   /**< (UART_IER) Enable RXRDY Interrupt Mask */
#define UART_IER_RXRDY(value)                 (UART_IER_RXRDY_Msk & ((value) << UART_IER_RXRDY_Pos))
#define UART_IER_TXRDY_Pos                    (1U)                                           /**< (UART_IER) Enable TXRDY Interrupt Position */
#define UART_IER_TXRDY_Msk                    (0x1U << UART_IER_TXRDY_Pos)                   /**< (UART_IER) Enable TXRDY Interrupt Mask */
#define UART_IER_TXRDY(value)                 (UART_IER_TXRDY_Msk & ((value) << UART_IER_TXRDY_Pos))
#define UART_IER_OVRE_Pos                     (5U)                                           /**< (UART_IER) Enable Overrun Error Interrupt Position */
#define UART_IER_OVRE_Msk                     (0x1U << UART_IER_OVRE_Pos)                    /**< (UART_IER) Enable Overrun Error Interrupt Mask */
#define UART_IER_OVRE(value)                  (UART_IER_OVRE_Msk & ((value) << UART_IER_OVRE_Pos))
#define UART_IER_FRAME_Pos                    (6U)                                           /**< (UART_IER) Enable Framing Error Interrupt Position */
#define UART_IER_FRAME_Msk                    (0x1U << UART_IER_FRAME_Pos)                   /**< (UART_IER) Enable Framing Error Interrupt Mask */
#define UART_IER_FRAME(value)                 (UART_IER_FRAME_Msk & ((value) << UART_IER_FRAME_Pos))
#define UART_IER_PARE_Pos                     (7U)                                           /**< (UART_IER) Enable Parity Error Interrupt Position */
#define UART_IER_PARE_Msk                     (0x1U << UART_IER_PARE_Pos)                    /**< (UART_IER) Enable Parity Error Interrupt Mask */
#define UART_IER_PARE(value)                  (UART_IER_PARE_Msk & ((value) << UART_IER_PARE_Pos))
#define UART_IER_TXEMPTY_Pos                  (9U)                                           /**< (UART_IER) Enable TXEMPTY Interrupt Position */
#define UART_IER_TXEMPTY_Msk                  (0x1U << UART_IER_TXEMPTY_Pos)                 /**< (UART_IER) Enable TXEMPTY Interrupt Mask */
#define UART_IER_TXEMPTY(value)               (UART_IER_TXEMPTY_Msk & ((value) << UART_IER_TXEMPTY_Pos))
#define UART_IER_CMP_Pos                      (15U)                                          /**< (UART_IER) Enable Comparison Interrupt Position */
#define UART_IER_CMP_Msk                      (0x1U << UART_IER_CMP_Pos)                     /**< (UART_IER) Enable Comparison Interrupt Mask */
#define UART_IER_CMP(value)                   (UART_IER_CMP_Msk & ((value) << UART_IER_CMP_Pos))
#define UART_IER_Msk                          (0x000082E3UL)                                 /**< (UART_IER) Register Mask  */

/* -------- UART_IDR : (UART Offset: 0x0C) ( /W 32) Interrupt Disable Register -------- */
#define UART_IDR_RXRDY_Pos                    (0U)                                           /**< (UART_IDR) Disable RXRDY Interrupt Position */
#define UART_IDR_RXRDY_Msk                    (0x1U << UART_IDR_RXRDY_Pos)                   /**< (UART_IDR) Disable RXRDY Interrupt Mask */
#define UART_IDR_RXRDY(value)                 (UART_IDR_RXRDY_Msk & ((value) << UART_IDR_RXRDY_Pos))
#define UART_IDR_TXRDY_Pos                    (1U)                                           /**< (UART_IDR) Disable TXRDY Interrupt Position */
#define UART_IDR_TXRDY_Msk                    (0x1U << UART_IDR_TXRDY_Pos)                   /**< (UART_IDR) Disable TXRDY Interrupt Mask */
#define UART_IDR_TXRDY(value)                 (UART_IDR_TXRDY_Msk & ((value) << UART_IDR_TXRDY_Pos))
#define UART_IDR_OVRE_Pos                     (5U)                                           /**< (UART_IDR) Disable Overrun Error Interrupt Position */
#define UART_IDR_OVRE_Msk                     (0x1U << UART_IDR_OVRE_Pos)                    /**< (UART_IDR) Disable Overrun Error Interrupt Mask */
#define UART_IDR_OVRE(value)                  (UART_IDR_OVRE_Msk & ((value) << UART_IDR_OVRE_Pos))
#define UART_IDR_FRAME_Pos                    (6U)                                           /**< (UART_IDR) Disable Framing Error Interrupt Position */
#define UART_IDR_FRAME_Msk                    (0x1U << UART_IDR_FRAME_Pos)                   /**< (UART_IDR) Disable Framing Error Interrupt Mask */
#define UART_IDR_FRAME(value)                 (UART_IDR_FRAME_Msk & ((value) << UART_IDR_FRAME_Pos))
#define UART_IDR_PARE_Pos                     (7U)                                           /**< (UART_IDR) Disable Parity Error Interrupt Position */
#define UART_IDR_PARE_Msk                     (0x1U << UART_IDR_PARE_Pos)                    /**< (UART_IDR) Disable Parity Error Interrupt Mask */
#define UART_IDR_PARE(value)                  (UART_IDR_PARE_Msk & ((value) << UART_IDR_PARE_Pos))
#define UART_IDR_TXEMPTY_Pos                  (9U)                                           /**< (UART_IDR) Disable TXEMPTY Interrupt Position */
#define UART_IDR_TXEMPTY_Msk                  (0x1U << UART_IDR_TXEMPTY_Pos)                 /**< (UART_IDR) Disable TXEMPTY Interrupt Mask */
#define UART_IDR_TXEMPTY(value)               (UART_IDR_TXEMPTY_Msk & ((value) << UART_IDR_TXEMPTY_Pos))
#define UART_IDR_CMP_Pos                      (15U)                                          /**< (UART_IDR) Disable Comparison Interrupt Position */
#define UART_IDR_CMP_Msk                      (0x1U << UART_IDR_CMP_Pos)                     /**< (UART_IDR) Disable Comparison Interrupt Mask */
#define UART_IDR_CMP(value)                   (UART_IDR_CMP_Msk & ((value) << UART_IDR_CMP_Pos))
#define UART_IDR_Msk                          (0x000082E3UL)                                 /**< (UART_IDR) Register Mask  */

/* -------- UART_IMR : (UART Offset: 0x10) (R/  32) Interrupt Mask Register -------- */
#define UART_IMR_RXRDY_Pos                    (0U)                                           /**< (UART_IMR) Mask RXRDY Interrupt Position */
#define UART_IMR_RXRDY_Msk                    (0x1U << UART_IMR_RXRDY_Pos)                   /**< (UART_IMR) Mask RXRDY Interrupt Mask */
#define UART_IMR_RXRDY(value)                 (UART_IMR_RXRDY_Msk & ((value) << UART_IMR_RXRDY_Pos))
#define UART_IMR_TXRDY_Pos                    (1U)                                           /**< (UART_IMR) Disable TXRDY Interrupt Position */
#define UART_IMR_TXRDY_Msk                    (0x1U << UART_IMR_TXRDY_Pos)                   /**< (UART_IMR) Disable TXRDY Interrupt Mask */
#define UART_IMR_TXRDY(value)                 (UART_IMR_TXRDY_Msk & ((value) << UART_IMR_TXRDY_Pos))
#define UART_IMR_OVRE_Pos                     (5U)                                           /**< (UART_IMR) Mask Overrun Error Interrupt Position */
#define UART_IMR_OVRE_Msk                     (0x1U << UART_IMR_OVRE_Pos)                    /**< (UART_IMR) Mask Overrun Error Interrupt Mask */
#define UART_IMR_OVRE(value)                  (UART_IMR_OVRE_Msk & ((value) << UART_IMR_OVRE_Pos))
#define UART_IMR_FRAME_Pos                    (6U)                                           /**< (UART_IMR) Mask Framing Error Interrupt Position */
#define UART_IMR_FRAME_Msk                    (0x1U << UART_IMR_FRAME_Pos)                   /**< (UART_IMR) Mask Framing Error Interrupt Mask */
#define UART_IMR_FRAME(value)                 (UART_IMR_FRAME_Msk & ((value) << UART_IMR_FRAME_Pos))
#define UART_IMR_PARE_Pos                     (7U)                                           /**< (UART_IMR) Mask Parity Error Interrupt Position */
#define UART_IMR_PARE_Msk                     (0x1U << UART_IMR_PARE_Pos)                    /**< (UART_IMR) Mask Parity Error Interrupt Mask */
#define UART_IMR_PARE(value)                  (UART_IMR_PARE_Msk & ((value) << UART_IMR_PARE_Pos))
#define UART_IMR_TXEMPTY_Pos                  (9U)                                           /**< (UART_IMR) Mask TXEMPTY Interrupt Position */
#define UART_IMR_TXEMPTY_Msk                  (0x1U << UART_IMR_TXEMPTY_Pos)                 /**< (UART_IMR) Mask TXEMPTY Interrupt Mask */
#define UART_IMR_TXEMPTY(value)               (UART_IMR_TXEMPTY_Msk & ((value) << UART_IMR_TXEMPTY_Pos))
#define UART_IMR_CMP_Pos                      (15U)                                          /**< (UART_IMR) Mask Comparison Interrupt Position */
#define UART_IMR_CMP_Msk                      (0x1U << UART_IMR_CMP_Pos)                     /**< (UART_IMR) Mask Comparison Interrupt Mask */
#define UART_IMR_CMP(value)                   (UART_IMR_CMP_Msk & ((value) << UART_IMR_CMP_Pos))
#define UART_IMR_Msk                          (0x000082E3UL)                                 /**< (UART_IMR) Register Mask  */

/* -------- UART_SR : (UART Offset: 0x14) (R/  32) Status Register -------- */
#define UART_SR_RXRDY_Pos                     (0U)                                           /**< (UART_SR) Receiver Ready Position */
#define UART_SR_RXRDY_Msk                     (0x1U << UART_SR_RXRDY_Pos)                    /**< (UART_SR) Receiver Ready Mask */
#define UART_SR_RXRDY(value)                  (UART_SR_RXRDY_Msk & ((value) << UART_SR_RXRDY_Pos))
#define UART_SR_TXRDY_Pos                     (1U)                                           /**< (UART_SR) Transmitter Ready Position */
#define UART_SR_TXRDY_Msk                     (0x1U << UART_SR_TXRDY_Pos)                    /**< (UART_SR) Transmitter Ready Mask */
#define UART_SR_TXRDY(value)                  (UART_SR_TXRDY_Msk & ((value) << UART_SR_TXRDY_Pos))
#define UART_SR_OVRE_Pos                      (5U)                                           /**< (UART_SR) Overrun Error Position */
#define UART_SR_OVRE_Msk                      (0x1U << UART_SR_OVRE_Pos)                     /**< (UART_SR) Overrun Error Mask */
#define UART_SR_OVRE(value)                   (UART_SR_OVRE_Msk & ((value) << UART_SR_OVRE_Pos))
#define UART_SR_FRAME_Pos                     (6U)                                           /**< (UART_SR) Framing Error Position */
#define UART_SR_FRAME_Msk                     (0x1U << UART_SR_FRAME_Pos)                    /**< (UART_SR) Framing Error Mask */
#define UART_SR_FRAME(value)                  (UART_SR_FRAME_Msk & ((value) << UART_SR_FRAME_Pos))
#define UART_SR_PARE_Pos                      (7U)                                           /**< (UART_SR) Parity Error Position */
#define UART_SR_PARE_Msk                      (0x1U << UART_SR_PARE_Pos)                     /**< (UART_SR) Parity Error Mask */
#define UART_SR_PARE(value)                   (UART_SR_PARE_Msk & ((value) << UART_SR_PARE_Pos))
#define UART_SR_TXEMPTY_Pos                   (9U)                                           /**< (UART_SR) Transmitter Empty Position */
#define UART_SR_TXEMPTY_Msk                   (0x1U << UART_SR_TXEMPTY_Pos)                  /**< (UART_SR) Transmitter Empty Mask */
#define UART_SR_TXEMPTY(value)                (UART_SR_TXEMPTY_Msk & ((value) << UART_SR_TXEMPTY_Pos))
#define UART_SR_CMP_Pos                       (15U)                                          /**< (UART_SR) Comparison Match Position */
#define UART_SR_CMP_Msk                       (0x1U << UART_SR_CMP_Pos)                      /**< (UART_SR) Comparison Match Mask */
#define UART_SR_CMP(value)                    (UART_SR_CMP_Msk & ((value) << UART_SR_CMP_Pos))
#define UART_SR_Msk                           (0x000082E3UL)                                 /**< (UART_SR) Register Mask  */

/* -------- UART_RHR : (UART Offset: 0x18) (R/  32) Receive Holding Register -------- */
#define UART_RHR_RXCHR_Pos                    (0U)                                           /**< (UART_RHR) Received Character Position */
#define UART_RHR_RXCHR_Msk                    (0xFFU << UART_RHR_RXCHR_Pos)                  /**< (UART_RHR) Received Character Mask */
#define UART_RHR_RXCHR(value)                 (UART_RHR_RXCHR_Msk & ((value) << UART_RHR_RXCHR_Pos))
#define UART_RHR_Msk                          (0x000000FFUL)                                 /**< (UART_RHR) Register Mask  */

/* -------- UART_THR : (UART Offset: 0x1C) ( /W 32) Transmit Holding Register -------- */
#define UART_THR_TXCHR_Pos                    (0U)                                           /**< (UART_THR) Character to be Transmitted Position */
#define UART_THR_TXCHR_Msk                    (0xFFU << UART_THR_TXCHR_Pos)                  /**< (UART_THR) Character to be Transmitted Mask */
#define UART_THR_TXCHR(value)                 (UART_THR_TXCHR_Msk & ((value) << UART_THR_TXCHR_Pos))
#define UART_THR_Msk                          (0x000000FFUL)                                 /**< (UART_THR) Register Mask  */

/* -------- UART_BRGR : (UART Offset: 0x20) (R/W 32) Baud Rate Generator Register -------- */
#define UART_BRGR_CD_Pos                      (0U)                                           /**< (UART_BRGR) Clock Divisor Position */
#define UART_BRGR_CD_Msk                      (0xFFFFU << UART_BRGR_CD_Pos)                  /**< (UART_BRGR) Clock Divisor Mask */
#define UART_BRGR_CD(value)                   (UART_BRGR_CD_Msk & ((value) << UART_BRGR_CD_Pos))
#define UART_BRGR_Msk                         (0x0000FFFFUL)                                 /**< (UART_BRGR) Register Mask  */

/* -------- UART_CMPR : (UART Offset: 0x24) (R/W 32) Comparison Register -------- */
#define UART_CMPR_VAL1_Pos                    (0U)                                           /**< (UART_CMPR) First Comparison Value for Received Character Position */
#define UART_CMPR_VAL1_Msk                    (0xFFU << UART_CMPR_VAL1_Pos)                  /**< (UART_CMPR) First Comparison Value for Received Character Mask */
#define UART_CMPR_VAL1(value)                 (UART_CMPR_VAL1_Msk & ((value) << UART_CMPR_VAL1_Pos))
#define UART_CMPR_CMPMODE_Pos                 (12U)                                          /**< (UART_CMPR) Comparison Mode Position */
#define UART_CMPR_CMPMODE_Msk                 (0x1U << UART_CMPR_CMPMODE_Pos)                /**< (UART_CMPR) Comparison Mode Mask */
#define UART_CMPR_CMPMODE(value)              (UART_CMPR_CMPMODE_Msk & ((value) << UART_CMPR_CMPMODE_Pos))
#define   UART_CMPR_CMPMODE_FLAG_ONLY_Val     (0U)                                           /**< (UART_CMPR) Any character is received and comparison function drives CMP flag.  */
#define   UART_CMPR_CMPMODE_START_CONDITION_Val (1U)                                           /**< (UART_CMPR) Comparison condition must be met to start reception.  */
#define UART_CMPR_CMPMODE_FLAG_ONLY           (UART_CMPR_CMPMODE_FLAG_ONLY_Val << UART_CMPR_CMPMODE_Pos) /**< (UART_CMPR) Any character is received and comparison function drives CMP flag. Position  */
#define UART_CMPR_CMPMODE_START_CONDITION     (UART_CMPR_CMPMODE_START_CONDITION_Val << UART_CMPR_CMPMODE_Pos) /**< (UART_CMPR) Comparison condition must be met to start reception. Position  */
#define UART_CMPR_CMPPAR_Pos                  (14U)                                          /**< (UART_CMPR) Compare Parity Position */
#define UART_CMPR_CMPPAR_Msk                  (0x1U << UART_CMPR_CMPPAR_Pos)                 /**< (UART_CMPR) Compare Parity Mask */
#define UART_CMPR_CMPPAR(value)               (UART_CMPR_CMPPAR_Msk & ((value) << UART_CMPR_CMPPAR_Pos))
#define UART_CMPR_VAL2_Pos                    (16U)                                          /**< (UART_CMPR) Second Comparison Value for Received Character Position */
#define UART_CMPR_VAL2_Msk                    (0xFFU << UART_CMPR_VAL2_Pos)                  /**< (UART_CMPR) Second Comparison Value for Received Character Mask */
#define UART_CMPR_VAL2(value)                 (UART_CMPR_VAL2_Msk & ((value) << UART_CMPR_VAL2_Pos))
#define UART_CMPR_Msk                         (0x00FF50FFUL)                                 /**< (UART_CMPR) Register Mask  */

/* -------- UART_WPMR : (UART Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define UART_WPMR_WPEN_Pos                    (0U)                                           /**< (UART_WPMR) Write Protection Enable Position */
#define UART_WPMR_WPEN_Msk                    (0x1U << UART_WPMR_WPEN_Pos)                   /**< (UART_WPMR) Write Protection Enable Mask */
#define UART_WPMR_WPEN(value)                 (UART_WPMR_WPEN_Msk & ((value) << UART_WPMR_WPEN_Pos))
#define UART_WPMR_WPKEY_Pos                   (8U)                                           /**< (UART_WPMR) Write Protection Key Position */
#define UART_WPMR_WPKEY_Msk                   (0xFFFFFFU << UART_WPMR_WPKEY_Pos)             /**< (UART_WPMR) Write Protection Key Mask */
#define UART_WPMR_WPKEY(value)                (UART_WPMR_WPKEY_Msk & ((value) << UART_WPMR_WPKEY_Pos))
#define   UART_WPMR_WPKEY_PASSWD_Val          (5587282U)                                     /**< (UART_WPMR) Writing any other value in this field aborts the write operation.Always reads as 0.  */
#define UART_WPMR_WPKEY_PASSWD                (UART_WPMR_WPKEY_PASSWD_Val << UART_WPMR_WPKEY_Pos) /**< (UART_WPMR) Writing any other value in this field aborts the write operation.Always reads as 0. Position  */
#define UART_WPMR_Msk                         (0xFFFFFF01UL)                                 /**< (UART_WPMR) Register Mask  */

/** \brief UART register offsets definitions */
#define UART_CR_OFFSET                 (0x00)         /**< (UART_CR) Control Register Offset */
#define UART_MR_OFFSET                 (0x04)         /**< (UART_MR) Mode Register Offset */
#define UART_IER_OFFSET                (0x08)         /**< (UART_IER) Interrupt Enable Register Offset */
#define UART_IDR_OFFSET                (0x0C)         /**< (UART_IDR) Interrupt Disable Register Offset */
#define UART_IMR_OFFSET                (0x10)         /**< (UART_IMR) Interrupt Mask Register Offset */
#define UART_SR_OFFSET                 (0x14)         /**< (UART_SR) Status Register Offset */
#define UART_RHR_OFFSET                (0x18)         /**< (UART_RHR) Receive Holding Register Offset */
#define UART_THR_OFFSET                (0x1C)         /**< (UART_THR) Transmit Holding Register Offset */
#define UART_BRGR_OFFSET               (0x20)         /**< (UART_BRGR) Baud Rate Generator Register Offset */
#define UART_CMPR_OFFSET               (0x24)         /**< (UART_CMPR) Comparison Register Offset */
#define UART_WPMR_OFFSET               (0xE4)         /**< (UART_WPMR) Write Protection Mode Register Offset */

/** \brief UART register API structure */
typedef struct
{
  __O   uint32_t                       UART_CR;         /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       UART_MR;         /**< Offset: 0x04 (R/W  32) Mode Register */
  __O   uint32_t                       UART_IER;        /**< Offset: 0x08 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       UART_IDR;        /**< Offset: 0x0c ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       UART_IMR;        /**< Offset: 0x10 (R/   32) Interrupt Mask Register */
  __I   uint32_t                       UART_SR;         /**< Offset: 0x14 (R/   32) Status Register */
  __I   uint32_t                       UART_RHR;        /**< Offset: 0x18 (R/   32) Receive Holding Register */
  __O   uint32_t                       UART_THR;        /**< Offset: 0x1c ( /W  32) Transmit Holding Register */
  __IO  uint32_t                       UART_BRGR;       /**< Offset: 0x20 (R/W  32) Baud Rate Generator Register */
  __IO  uint32_t                       UART_CMPR;       /**< Offset: 0x24 (R/W  32) Comparison Register */
  __I   uint8_t                        Reserved1[0xBC];
  __IO  uint32_t                       UART_WPMR;       /**< Offset: 0xe4 (R/W  32) Write Protection Mode Register */
} uart_registers_t;
/** @}  end of Universal Asynchronous Receiver Transmitter */

#endif /* SAME_SAME70_UART_MODULE_H */

