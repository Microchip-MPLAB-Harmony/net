/**
 * \brief Header file for SAME/SAME70 USART
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
#ifndef SAME_SAME70_USART_MODULE_H
#define SAME_SAME70_USART_MODULE_H

/** \addtogroup SAME_SAME70 Universal Synchronous Asynchronous Receiver Transmitter
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_USART                                 */
/* ========================================================================== */

/* -------- US_CR : (USART Offset: 0x00) ( /W 32) Control Register -------- */
#define US_CR_RSTRX_Pos                       (2U)                                           /**< (US_CR) Reset Receiver Position */
#define US_CR_RSTRX_Msk                       (0x1U << US_CR_RSTRX_Pos)                      /**< (US_CR) Reset Receiver Mask */
#define US_CR_RSTRX(value)                    (US_CR_RSTRX_Msk & ((value) << US_CR_RSTRX_Pos))
#define US_CR_RSTTX_Pos                       (3U)                                           /**< (US_CR) Reset Transmitter Position */
#define US_CR_RSTTX_Msk                       (0x1U << US_CR_RSTTX_Pos)                      /**< (US_CR) Reset Transmitter Mask */
#define US_CR_RSTTX(value)                    (US_CR_RSTTX_Msk & ((value) << US_CR_RSTTX_Pos))
#define US_CR_RXEN_Pos                        (4U)                                           /**< (US_CR) Receiver Enable Position */
#define US_CR_RXEN_Msk                        (0x1U << US_CR_RXEN_Pos)                       /**< (US_CR) Receiver Enable Mask */
#define US_CR_RXEN(value)                     (US_CR_RXEN_Msk & ((value) << US_CR_RXEN_Pos))
#define US_CR_RXDIS_Pos                       (5U)                                           /**< (US_CR) Receiver Disable Position */
#define US_CR_RXDIS_Msk                       (0x1U << US_CR_RXDIS_Pos)                      /**< (US_CR) Receiver Disable Mask */
#define US_CR_RXDIS(value)                    (US_CR_RXDIS_Msk & ((value) << US_CR_RXDIS_Pos))
#define US_CR_TXEN_Pos                        (6U)                                           /**< (US_CR) Transmitter Enable Position */
#define US_CR_TXEN_Msk                        (0x1U << US_CR_TXEN_Pos)                       /**< (US_CR) Transmitter Enable Mask */
#define US_CR_TXEN(value)                     (US_CR_TXEN_Msk & ((value) << US_CR_TXEN_Pos))
#define US_CR_TXDIS_Pos                       (7U)                                           /**< (US_CR) Transmitter Disable Position */
#define US_CR_TXDIS_Msk                       (0x1U << US_CR_TXDIS_Pos)                      /**< (US_CR) Transmitter Disable Mask */
#define US_CR_TXDIS(value)                    (US_CR_TXDIS_Msk & ((value) << US_CR_TXDIS_Pos))
#define US_CR_RSTSTA_Pos                      (8U)                                           /**< (US_CR) Reset Status Bits Position */
#define US_CR_RSTSTA_Msk                      (0x1U << US_CR_RSTSTA_Pos)                     /**< (US_CR) Reset Status Bits Mask */
#define US_CR_RSTSTA(value)                   (US_CR_RSTSTA_Msk & ((value) << US_CR_RSTSTA_Pos))
#define US_CR_STTBRK_Pos                      (9U)                                           /**< (US_CR) Start Break Position */
#define US_CR_STTBRK_Msk                      (0x1U << US_CR_STTBRK_Pos)                     /**< (US_CR) Start Break Mask */
#define US_CR_STTBRK(value)                   (US_CR_STTBRK_Msk & ((value) << US_CR_STTBRK_Pos))
#define US_CR_STPBRK_Pos                      (10U)                                          /**< (US_CR) Stop Break Position */
#define US_CR_STPBRK_Msk                      (0x1U << US_CR_STPBRK_Pos)                     /**< (US_CR) Stop Break Mask */
#define US_CR_STPBRK(value)                   (US_CR_STPBRK_Msk & ((value) << US_CR_STPBRK_Pos))
#define US_CR_STTTO_Pos                       (11U)                                          /**< (US_CR) Clear TIMEOUT Flag and Start Timeout After Next Character Received Position */
#define US_CR_STTTO_Msk                       (0x1U << US_CR_STTTO_Pos)                      /**< (US_CR) Clear TIMEOUT Flag and Start Timeout After Next Character Received Mask */
#define US_CR_STTTO(value)                    (US_CR_STTTO_Msk & ((value) << US_CR_STTTO_Pos))
#define US_CR_SENDA_Pos                       (12U)                                          /**< (US_CR) Send Address Position */
#define US_CR_SENDA_Msk                       (0x1U << US_CR_SENDA_Pos)                      /**< (US_CR) Send Address Mask */
#define US_CR_SENDA(value)                    (US_CR_SENDA_Msk & ((value) << US_CR_SENDA_Pos))
#define US_CR_RSTIT_Pos                       (13U)                                          /**< (US_CR) Reset Iterations Position */
#define US_CR_RSTIT_Msk                       (0x1U << US_CR_RSTIT_Pos)                      /**< (US_CR) Reset Iterations Mask */
#define US_CR_RSTIT(value)                    (US_CR_RSTIT_Msk & ((value) << US_CR_RSTIT_Pos))
#define US_CR_RSTNACK_Pos                     (14U)                                          /**< (US_CR) Reset Non Acknowledge Position */
#define US_CR_RSTNACK_Msk                     (0x1U << US_CR_RSTNACK_Pos)                    /**< (US_CR) Reset Non Acknowledge Mask */
#define US_CR_RSTNACK(value)                  (US_CR_RSTNACK_Msk & ((value) << US_CR_RSTNACK_Pos))
#define US_CR_RETTO_Pos                       (15U)                                          /**< (US_CR) Start Timeout Immediately Position */
#define US_CR_RETTO_Msk                       (0x1U << US_CR_RETTO_Pos)                      /**< (US_CR) Start Timeout Immediately Mask */
#define US_CR_RETTO(value)                    (US_CR_RETTO_Msk & ((value) << US_CR_RETTO_Pos))
#define US_CR_DTREN_Pos                       (16U)                                          /**< (US_CR) Data Terminal Ready Enable Position */
#define US_CR_DTREN_Msk                       (0x1U << US_CR_DTREN_Pos)                      /**< (US_CR) Data Terminal Ready Enable Mask */
#define US_CR_DTREN(value)                    (US_CR_DTREN_Msk & ((value) << US_CR_DTREN_Pos))
#define US_CR_DTRDIS_Pos                      (17U)                                          /**< (US_CR) Data Terminal Ready Disable Position */
#define US_CR_DTRDIS_Msk                      (0x1U << US_CR_DTRDIS_Pos)                     /**< (US_CR) Data Terminal Ready Disable Mask */
#define US_CR_DTRDIS(value)                   (US_CR_DTRDIS_Msk & ((value) << US_CR_DTRDIS_Pos))
#define US_CR_RTSEN_Pos                       (18U)                                          /**< (US_CR) Request to Send Enable Position */
#define US_CR_RTSEN_Msk                       (0x1U << US_CR_RTSEN_Pos)                      /**< (US_CR) Request to Send Enable Mask */
#define US_CR_RTSEN(value)                    (US_CR_RTSEN_Msk & ((value) << US_CR_RTSEN_Pos))
#define US_CR_RTSDIS_Pos                      (19U)                                          /**< (US_CR) Request to Send Disable Position */
#define US_CR_RTSDIS_Msk                      (0x1U << US_CR_RTSDIS_Pos)                     /**< (US_CR) Request to Send Disable Mask */
#define US_CR_RTSDIS(value)                   (US_CR_RTSDIS_Msk & ((value) << US_CR_RTSDIS_Pos))
#define US_CR_LINABT_Pos                      (20U)                                          /**< (US_CR) Abort LIN Transmission Position */
#define US_CR_LINABT_Msk                      (0x1U << US_CR_LINABT_Pos)                     /**< (US_CR) Abort LIN Transmission Mask */
#define US_CR_LINABT(value)                   (US_CR_LINABT_Msk & ((value) << US_CR_LINABT_Pos))
#define US_CR_LINWKUP_Pos                     (21U)                                          /**< (US_CR) Send LIN Wakeup Signal Position */
#define US_CR_LINWKUP_Msk                     (0x1U << US_CR_LINWKUP_Pos)                    /**< (US_CR) Send LIN Wakeup Signal Mask */
#define US_CR_LINWKUP(value)                  (US_CR_LINWKUP_Msk & ((value) << US_CR_LINWKUP_Pos))
#define US_CR_FCS_Pos                         (18U)                                          /**< (US_CR) Force SPI Chip Select Position */
#define US_CR_FCS_Msk                         (0x1U << US_CR_FCS_Pos)                        /**< (US_CR) Force SPI Chip Select Mask */
#define US_CR_FCS(value)                      (US_CR_FCS_Msk & ((value) << US_CR_FCS_Pos))  
#define US_CR_RCS_Pos                         (19U)                                          /**< (US_CR) Release SPI Chip Select Position */
#define US_CR_RCS_Msk                         (0x1U << US_CR_RCS_Pos)                        /**< (US_CR) Release SPI Chip Select Mask */
#define US_CR_RCS(value)                      (US_CR_RCS_Msk & ((value) << US_CR_RCS_Pos))  
#define US_CR_Msk                             (0x003FFFFCUL)                                 /**< (US_CR) Register Mask  */

/* -------- US_MR : (USART Offset: 0x04) (R/W 32) Mode Register -------- */
#define US_MR_USART_MODE_Pos                  (0U)                                           /**< (US_MR) USART Mode of Operation Position */
#define US_MR_USART_MODE_Msk                  (0xFU << US_MR_USART_MODE_Pos)                 /**< (US_MR) USART Mode of Operation Mask */
#define US_MR_USART_MODE(value)               (US_MR_USART_MODE_Msk & ((value) << US_MR_USART_MODE_Pos))
#define   US_MR_USART_MODE_NORMAL_Val         (0U)                                           /**< (US_MR) Normal mode  */
#define   US_MR_USART_MODE_RS485_Val          (1U)                                           /**< (US_MR) RS485  */
#define   US_MR_USART_MODE_HW_HANDSHAKING_Val (2U)                                           /**< (US_MR) Hardware handshaking  */
#define   US_MR_USART_MODE_MODEM_Val          (3U)                                           /**< (US_MR) Modem  */
#define   US_MR_USART_MODE_IS07816_T_0_Val    (4U)                                           /**< (US_MR) IS07816 Protocol: T = 0  */
#define   US_MR_USART_MODE_IS07816_T_1_Val    (6U)                                           /**< (US_MR) IS07816 Protocol: T = 1  */
#define   US_MR_USART_MODE_IRDA_Val           (8U)                                           /**< (US_MR) IrDA  */
#define   US_MR_USART_MODE_LON_Val            (9U)                                           /**< (US_MR) LON  */
#define   US_MR_USART_MODE_LIN_MASTER_Val     (10U)                                          /**< (US_MR) LIN Master mode  */
#define   US_MR_USART_MODE_LIN_SLAVE_Val      (11U)                                          /**< (US_MR) LIN Slave mode  */
#define   US_MR_USART_MODE_SPI_MASTER_Val     (14U)                                          /**< (US_MR) SPI Master mode (CLKO must be written to 1 and USCLKS = 0, 1 or 2)  */
#define   US_MR_USART_MODE_SPI_SLAVE_Val      (15U)                                          /**< (US_MR) SPI Slave mode  */
#define US_MR_USART_MODE_NORMAL               (US_MR_USART_MODE_NORMAL_Val << US_MR_USART_MODE_Pos) /**< (US_MR) Normal mode Position  */
#define US_MR_USART_MODE_RS485                (US_MR_USART_MODE_RS485_Val << US_MR_USART_MODE_Pos) /**< (US_MR) RS485 Position  */
#define US_MR_USART_MODE_HW_HANDSHAKING       (US_MR_USART_MODE_HW_HANDSHAKING_Val << US_MR_USART_MODE_Pos) /**< (US_MR) Hardware handshaking Position  */
#define US_MR_USART_MODE_MODEM                (US_MR_USART_MODE_MODEM_Val << US_MR_USART_MODE_Pos) /**< (US_MR) Modem Position  */
#define US_MR_USART_MODE_IS07816_T_0          (US_MR_USART_MODE_IS07816_T_0_Val << US_MR_USART_MODE_Pos) /**< (US_MR) IS07816 Protocol: T = 0 Position  */
#define US_MR_USART_MODE_IS07816_T_1          (US_MR_USART_MODE_IS07816_T_1_Val << US_MR_USART_MODE_Pos) /**< (US_MR) IS07816 Protocol: T = 1 Position  */
#define US_MR_USART_MODE_IRDA                 (US_MR_USART_MODE_IRDA_Val << US_MR_USART_MODE_Pos) /**< (US_MR) IrDA Position  */
#define US_MR_USART_MODE_LON                  (US_MR_USART_MODE_LON_Val << US_MR_USART_MODE_Pos) /**< (US_MR) LON Position  */
#define US_MR_USART_MODE_LIN_MASTER           (US_MR_USART_MODE_LIN_MASTER_Val << US_MR_USART_MODE_Pos) /**< (US_MR) LIN Master mode Position  */
#define US_MR_USART_MODE_LIN_SLAVE            (US_MR_USART_MODE_LIN_SLAVE_Val << US_MR_USART_MODE_Pos) /**< (US_MR) LIN Slave mode Position  */
#define US_MR_USART_MODE_SPI_MASTER           (US_MR_USART_MODE_SPI_MASTER_Val << US_MR_USART_MODE_Pos) /**< (US_MR) SPI Master mode (CLKO must be written to 1 and USCLKS = 0, 1 or 2) Position  */
#define US_MR_USART_MODE_SPI_SLAVE            (US_MR_USART_MODE_SPI_SLAVE_Val << US_MR_USART_MODE_Pos) /**< (US_MR) SPI Slave mode Position  */
#define US_MR_USCLKS_Pos                      (4U)                                           /**< (US_MR) Clock Selection Position */
#define US_MR_USCLKS_Msk                      (0x3U << US_MR_USCLKS_Pos)                     /**< (US_MR) Clock Selection Mask */
#define US_MR_USCLKS(value)                   (US_MR_USCLKS_Msk & ((value) << US_MR_USCLKS_Pos))
#define   US_MR_USCLKS_MCK_Val                (0U)                                           /**< (US_MR) Peripheral clock is selected  */
#define   US_MR_USCLKS_DIV_Val                (1U)                                           /**< (US_MR) Peripheral clock divided (DIV = 8) is selected  */
#define   US_MR_USCLKS_PCK_Val                (2U)                                           /**< (US_MR) PMC programmable clock (PCK) is selected. If the SCK pin is driven (CLKO = 1), the CD field must be greater than 1.  */
#define   US_MR_USCLKS_SCK_Val                (3U)                                           /**< (US_MR) Serial clock (SCK) is selected  */
#define US_MR_USCLKS_MCK                      (US_MR_USCLKS_MCK_Val << US_MR_USCLKS_Pos)     /**< (US_MR) Peripheral clock is selected Position  */
#define US_MR_USCLKS_DIV                      (US_MR_USCLKS_DIV_Val << US_MR_USCLKS_Pos)     /**< (US_MR) Peripheral clock divided (DIV = 8) is selected Position  */
#define US_MR_USCLKS_PCK                      (US_MR_USCLKS_PCK_Val << US_MR_USCLKS_Pos)     /**< (US_MR) PMC programmable clock (PCK) is selected. If the SCK pin is driven (CLKO = 1), the CD field must be greater than 1. Position  */
#define US_MR_USCLKS_SCK                      (US_MR_USCLKS_SCK_Val << US_MR_USCLKS_Pos)     /**< (US_MR) Serial clock (SCK) is selected Position  */
#define US_MR_CHRL_Pos                        (6U)                                           /**< (US_MR) Character Length Position */
#define US_MR_CHRL_Msk                        (0x3U << US_MR_CHRL_Pos)                       /**< (US_MR) Character Length Mask */
#define US_MR_CHRL(value)                     (US_MR_CHRL_Msk & ((value) << US_MR_CHRL_Pos))
#define   US_MR_CHRL_5_BIT_Val                (0U)                                           /**< (US_MR) Character length is 5 bits  */
#define   US_MR_CHRL_6_BIT_Val                (1U)                                           /**< (US_MR) Character length is 6 bits  */
#define   US_MR_CHRL_7_BIT_Val                (2U)                                           /**< (US_MR) Character length is 7 bits  */
#define   US_MR_CHRL_8_BIT_Val                (3U)                                           /**< (US_MR) Character length is 8 bits  */
#define US_MR_CHRL_5_BIT                      (US_MR_CHRL_5_BIT_Val << US_MR_CHRL_Pos)       /**< (US_MR) Character length is 5 bits Position  */
#define US_MR_CHRL_6_BIT                      (US_MR_CHRL_6_BIT_Val << US_MR_CHRL_Pos)       /**< (US_MR) Character length is 6 bits Position  */
#define US_MR_CHRL_7_BIT                      (US_MR_CHRL_7_BIT_Val << US_MR_CHRL_Pos)       /**< (US_MR) Character length is 7 bits Position  */
#define US_MR_CHRL_8_BIT                      (US_MR_CHRL_8_BIT_Val << US_MR_CHRL_Pos)       /**< (US_MR) Character length is 8 bits Position  */
#define US_MR_SYNC_Pos                        (8U)                                           /**< (US_MR) Synchronous Mode Select Position */
#define US_MR_SYNC_Msk                        (0x1U << US_MR_SYNC_Pos)                       /**< (US_MR) Synchronous Mode Select Mask */
#define US_MR_SYNC(value)                     (US_MR_SYNC_Msk & ((value) << US_MR_SYNC_Pos))
#define US_MR_PAR_Pos                         (9U)                                           /**< (US_MR) Parity Type Position */
#define US_MR_PAR_Msk                         (0x7U << US_MR_PAR_Pos)                        /**< (US_MR) Parity Type Mask */
#define US_MR_PAR(value)                      (US_MR_PAR_Msk & ((value) << US_MR_PAR_Pos))  
#define   US_MR_PAR_EVEN_Val                  (0U)                                           /**< (US_MR) Even parity  */
#define   US_MR_PAR_ODD_Val                   (1U)                                           /**< (US_MR) Odd parity  */
#define   US_MR_PAR_SPACE_Val                 (2U)                                           /**< (US_MR) Parity forced to 0 (Space)  */
#define   US_MR_PAR_MARK_Val                  (3U)                                           /**< (US_MR) Parity forced to 1 (Mark)  */
#define   US_MR_PAR_NO_Val                    (4U)                                           /**< (US_MR) No parity  */
#define   US_MR_PAR_MULTIDROP_Val             (6U)                                           /**< (US_MR) Multidrop mode  */
#define US_MR_PAR_EVEN                        (US_MR_PAR_EVEN_Val << US_MR_PAR_Pos)          /**< (US_MR) Even parity Position  */
#define US_MR_PAR_ODD                         (US_MR_PAR_ODD_Val << US_MR_PAR_Pos)           /**< (US_MR) Odd parity Position  */
#define US_MR_PAR_SPACE                       (US_MR_PAR_SPACE_Val << US_MR_PAR_Pos)         /**< (US_MR) Parity forced to 0 (Space) Position  */
#define US_MR_PAR_MARK                        (US_MR_PAR_MARK_Val << US_MR_PAR_Pos)          /**< (US_MR) Parity forced to 1 (Mark) Position  */
#define US_MR_PAR_NO                          (US_MR_PAR_NO_Val << US_MR_PAR_Pos)            /**< (US_MR) No parity Position  */
#define US_MR_PAR_MULTIDROP                   (US_MR_PAR_MULTIDROP_Val << US_MR_PAR_Pos)     /**< (US_MR) Multidrop mode Position  */
#define US_MR_NBSTOP_Pos                      (12U)                                          /**< (US_MR) Number of Stop Bits Position */
#define US_MR_NBSTOP_Msk                      (0x3U << US_MR_NBSTOP_Pos)                     /**< (US_MR) Number of Stop Bits Mask */
#define US_MR_NBSTOP(value)                   (US_MR_NBSTOP_Msk & ((value) << US_MR_NBSTOP_Pos))
#define   US_MR_NBSTOP_1_BIT_Val              (0U)                                           /**< (US_MR) 1 stop bit  */
#define   US_MR_NBSTOP_1_5_BIT_Val            (1U)                                           /**< (US_MR) 1.5 stop bit (SYNC = 0) or reserved (SYNC = 1)  */
#define   US_MR_NBSTOP_2_BIT_Val              (2U)                                           /**< (US_MR) 2 stop bits  */
#define US_MR_NBSTOP_1_BIT                    (US_MR_NBSTOP_1_BIT_Val << US_MR_NBSTOP_Pos)   /**< (US_MR) 1 stop bit Position  */
#define US_MR_NBSTOP_1_5_BIT                  (US_MR_NBSTOP_1_5_BIT_Val << US_MR_NBSTOP_Pos) /**< (US_MR) 1.5 stop bit (SYNC = 0) or reserved (SYNC = 1) Position  */
#define US_MR_NBSTOP_2_BIT                    (US_MR_NBSTOP_2_BIT_Val << US_MR_NBSTOP_Pos)   /**< (US_MR) 2 stop bits Position  */
#define US_MR_CHMODE_Pos                      (14U)                                          /**< (US_MR) Channel Mode Position */
#define US_MR_CHMODE_Msk                      (0x3U << US_MR_CHMODE_Pos)                     /**< (US_MR) Channel Mode Mask */
#define US_MR_CHMODE(value)                   (US_MR_CHMODE_Msk & ((value) << US_MR_CHMODE_Pos))
#define   US_MR_CHMODE_NORMAL_Val             (0U)                                           /**< (US_MR) Normal mode  */
#define   US_MR_CHMODE_AUTOMATIC_Val          (1U)                                           /**< (US_MR) Automatic Echo. Receiver input is connected to the TXD pin.  */
#define   US_MR_CHMODE_LOCAL_LOOPBACK_Val     (2U)                                           /**< (US_MR) Local Loopback. Transmitter output is connected to the Receiver Input.  */
#define   US_MR_CHMODE_REMOTE_LOOPBACK_Val    (3U)                                           /**< (US_MR) Remote Loopback. RXD pin is internally connected to the TXD pin.  */
#define US_MR_CHMODE_NORMAL                   (US_MR_CHMODE_NORMAL_Val << US_MR_CHMODE_Pos)  /**< (US_MR) Normal mode Position  */
#define US_MR_CHMODE_AUTOMATIC                (US_MR_CHMODE_AUTOMATIC_Val << US_MR_CHMODE_Pos) /**< (US_MR) Automatic Echo. Receiver input is connected to the TXD pin. Position  */
#define US_MR_CHMODE_LOCAL_LOOPBACK           (US_MR_CHMODE_LOCAL_LOOPBACK_Val << US_MR_CHMODE_Pos) /**< (US_MR) Local Loopback. Transmitter output is connected to the Receiver Input. Position  */
#define US_MR_CHMODE_REMOTE_LOOPBACK          (US_MR_CHMODE_REMOTE_LOOPBACK_Val << US_MR_CHMODE_Pos) /**< (US_MR) Remote Loopback. RXD pin is internally connected to the TXD pin. Position  */
#define US_MR_MSBF_Pos                        (16U)                                          /**< (US_MR) Bit Order Position */
#define US_MR_MSBF_Msk                        (0x1U << US_MR_MSBF_Pos)                       /**< (US_MR) Bit Order Mask */
#define US_MR_MSBF(value)                     (US_MR_MSBF_Msk & ((value) << US_MR_MSBF_Pos))
#define US_MR_MODE9_Pos                       (17U)                                          /**< (US_MR) 9-bit Character Length Position */
#define US_MR_MODE9_Msk                       (0x1U << US_MR_MODE9_Pos)                      /**< (US_MR) 9-bit Character Length Mask */
#define US_MR_MODE9(value)                    (US_MR_MODE9_Msk & ((value) << US_MR_MODE9_Pos))
#define US_MR_CLKO_Pos                        (18U)                                          /**< (US_MR) Clock Output Select Position */
#define US_MR_CLKO_Msk                        (0x1U << US_MR_CLKO_Pos)                       /**< (US_MR) Clock Output Select Mask */
#define US_MR_CLKO(value)                     (US_MR_CLKO_Msk & ((value) << US_MR_CLKO_Pos))
#define US_MR_OVER_Pos                        (19U)                                          /**< (US_MR) Oversampling Mode Position */
#define US_MR_OVER_Msk                        (0x1U << US_MR_OVER_Pos)                       /**< (US_MR) Oversampling Mode Mask */
#define US_MR_OVER(value)                     (US_MR_OVER_Msk & ((value) << US_MR_OVER_Pos))
#define US_MR_INACK_Pos                       (20U)                                          /**< (US_MR) Inhibit Non Acknowledge Position */
#define US_MR_INACK_Msk                       (0x1U << US_MR_INACK_Pos)                      /**< (US_MR) Inhibit Non Acknowledge Mask */
#define US_MR_INACK(value)                    (US_MR_INACK_Msk & ((value) << US_MR_INACK_Pos))
#define US_MR_DSNACK_Pos                      (21U)                                          /**< (US_MR) Disable Successive NACK Position */
#define US_MR_DSNACK_Msk                      (0x1U << US_MR_DSNACK_Pos)                     /**< (US_MR) Disable Successive NACK Mask */
#define US_MR_DSNACK(value)                   (US_MR_DSNACK_Msk & ((value) << US_MR_DSNACK_Pos))
#define US_MR_VAR_SYNC_Pos                    (22U)                                          /**< (US_MR) Variable Synchronization of Command/Data Sync Start Frame Delimiter Position */
#define US_MR_VAR_SYNC_Msk                    (0x1U << US_MR_VAR_SYNC_Pos)                   /**< (US_MR) Variable Synchronization of Command/Data Sync Start Frame Delimiter Mask */
#define US_MR_VAR_SYNC(value)                 (US_MR_VAR_SYNC_Msk & ((value) << US_MR_VAR_SYNC_Pos))
#define US_MR_INVDATA_Pos                     (23U)                                          /**< (US_MR) Inverted Data Position */
#define US_MR_INVDATA_Msk                     (0x1U << US_MR_INVDATA_Pos)                    /**< (US_MR) Inverted Data Mask */
#define US_MR_INVDATA(value)                  (US_MR_INVDATA_Msk & ((value) << US_MR_INVDATA_Pos))
#define US_MR_MAX_ITERATION_Pos               (24U)                                          /**< (US_MR) Maximum Number of Automatic Iteration Position */
#define US_MR_MAX_ITERATION_Msk               (0x7U << US_MR_MAX_ITERATION_Pos)              /**< (US_MR) Maximum Number of Automatic Iteration Mask */
#define US_MR_MAX_ITERATION(value)            (US_MR_MAX_ITERATION_Msk & ((value) << US_MR_MAX_ITERATION_Pos))
#define US_MR_FILTER_Pos                      (28U)                                          /**< (US_MR) Receive Line Filter Position */
#define US_MR_FILTER_Msk                      (0x1U << US_MR_FILTER_Pos)                     /**< (US_MR) Receive Line Filter Mask */
#define US_MR_FILTER(value)                   (US_MR_FILTER_Msk & ((value) << US_MR_FILTER_Pos))
#define US_MR_MAN_Pos                         (29U)                                          /**< (US_MR) Manchester Encoder/Decoder Enable Position */
#define US_MR_MAN_Msk                         (0x1U << US_MR_MAN_Pos)                        /**< (US_MR) Manchester Encoder/Decoder Enable Mask */
#define US_MR_MAN(value)                      (US_MR_MAN_Msk & ((value) << US_MR_MAN_Pos))  
#define US_MR_MODSYNC_Pos                     (30U)                                          /**< (US_MR) Manchester Synchronization Mode Position */
#define US_MR_MODSYNC_Msk                     (0x1U << US_MR_MODSYNC_Pos)                    /**< (US_MR) Manchester Synchronization Mode Mask */
#define US_MR_MODSYNC(value)                  (US_MR_MODSYNC_Msk & ((value) << US_MR_MODSYNC_Pos))
#define US_MR_ONEBIT_Pos                      (31U)                                          /**< (US_MR) Start Frame Delimiter Selector Position */
#define US_MR_ONEBIT_Msk                      (0x1U << US_MR_ONEBIT_Pos)                     /**< (US_MR) Start Frame Delimiter Selector Mask */
#define US_MR_ONEBIT(value)                   (US_MR_ONEBIT_Msk & ((value) << US_MR_ONEBIT_Pos))
#define US_MR_CPHA_Pos                        (8U)                                           /**< (US_MR) SPI Clock Phase Position */
#define US_MR_CPHA_Msk                        (0x1U << US_MR_CPHA_Pos)                       /**< (US_MR) SPI Clock Phase Mask */
#define US_MR_CPHA(value)                     (US_MR_CPHA_Msk & ((value) << US_MR_CPHA_Pos))
#define US_MR_CPOL_Pos                        (16U)                                          /**< (US_MR) SPI Clock Polarity Position */
#define US_MR_CPOL_Msk                        (0x1U << US_MR_CPOL_Pos)                       /**< (US_MR) SPI Clock Polarity Mask */
#define US_MR_CPOL(value)                     (US_MR_CPOL_Msk & ((value) << US_MR_CPOL_Pos))
#define US_MR_WRDBT_Pos                       (20U)                                          /**< (US_MR) Wait Read Data Before Transfer Position */
#define US_MR_WRDBT_Msk                       (0x1U << US_MR_WRDBT_Pos)                      /**< (US_MR) Wait Read Data Before Transfer Mask */
#define US_MR_WRDBT(value)                    (US_MR_WRDBT_Msk & ((value) << US_MR_WRDBT_Pos))
#define US_MR_Msk                             (0xF7FFFFFFUL)                                 /**< (US_MR) Register Mask  */

/* -------- US_IER : (USART Offset: 0x08) ( /W 32) Interrupt Enable Register -------- */
#define US_IER_RXRDY_Pos                      (0U)                                           /**< (US_IER) RXRDY Interrupt Enable Position */
#define US_IER_RXRDY_Msk                      (0x1U << US_IER_RXRDY_Pos)                     /**< (US_IER) RXRDY Interrupt Enable Mask */
#define US_IER_RXRDY(value)                   (US_IER_RXRDY_Msk & ((value) << US_IER_RXRDY_Pos))
#define US_IER_TXRDY_Pos                      (1U)                                           /**< (US_IER) TXRDY Interrupt Enable Position */
#define US_IER_TXRDY_Msk                      (0x1U << US_IER_TXRDY_Pos)                     /**< (US_IER) TXRDY Interrupt Enable Mask */
#define US_IER_TXRDY(value)                   (US_IER_TXRDY_Msk & ((value) << US_IER_TXRDY_Pos))
#define US_IER_RXBRK_Pos                      (2U)                                           /**< (US_IER) Receiver Break Interrupt Enable Position */
#define US_IER_RXBRK_Msk                      (0x1U << US_IER_RXBRK_Pos)                     /**< (US_IER) Receiver Break Interrupt Enable Mask */
#define US_IER_RXBRK(value)                   (US_IER_RXBRK_Msk & ((value) << US_IER_RXBRK_Pos))
#define US_IER_OVRE_Pos                       (5U)                                           /**< (US_IER) Overrun Error Interrupt Enable Position */
#define US_IER_OVRE_Msk                       (0x1U << US_IER_OVRE_Pos)                      /**< (US_IER) Overrun Error Interrupt Enable Mask */
#define US_IER_OVRE(value)                    (US_IER_OVRE_Msk & ((value) << US_IER_OVRE_Pos))
#define US_IER_FRAME_Pos                      (6U)                                           /**< (US_IER) Framing Error Interrupt Enable Position */
#define US_IER_FRAME_Msk                      (0x1U << US_IER_FRAME_Pos)                     /**< (US_IER) Framing Error Interrupt Enable Mask */
#define US_IER_FRAME(value)                   (US_IER_FRAME_Msk & ((value) << US_IER_FRAME_Pos))
#define US_IER_PARE_Pos                       (7U)                                           /**< (US_IER) Parity Error Interrupt Enable Position */
#define US_IER_PARE_Msk                       (0x1U << US_IER_PARE_Pos)                      /**< (US_IER) Parity Error Interrupt Enable Mask */
#define US_IER_PARE(value)                    (US_IER_PARE_Msk & ((value) << US_IER_PARE_Pos))
#define US_IER_TIMEOUT_Pos                    (8U)                                           /**< (US_IER) Timeout Interrupt Enable Position */
#define US_IER_TIMEOUT_Msk                    (0x1U << US_IER_TIMEOUT_Pos)                   /**< (US_IER) Timeout Interrupt Enable Mask */
#define US_IER_TIMEOUT(value)                 (US_IER_TIMEOUT_Msk & ((value) << US_IER_TIMEOUT_Pos))
#define US_IER_TXEMPTY_Pos                    (9U)                                           /**< (US_IER) TXEMPTY Interrupt Enable Position */
#define US_IER_TXEMPTY_Msk                    (0x1U << US_IER_TXEMPTY_Pos)                   /**< (US_IER) TXEMPTY Interrupt Enable Mask */
#define US_IER_TXEMPTY(value)                 (US_IER_TXEMPTY_Msk & ((value) << US_IER_TXEMPTY_Pos))
#define US_IER_ITER_Pos                       (10U)                                          /**< (US_IER) Max number of Repetitions Reached Interrupt Enable Position */
#define US_IER_ITER_Msk                       (0x1U << US_IER_ITER_Pos)                      /**< (US_IER) Max number of Repetitions Reached Interrupt Enable Mask */
#define US_IER_ITER(value)                    (US_IER_ITER_Msk & ((value) << US_IER_ITER_Pos))
#define US_IER_NACK_Pos                       (13U)                                          /**< (US_IER) Non Acknowledge Interrupt Enable Position */
#define US_IER_NACK_Msk                       (0x1U << US_IER_NACK_Pos)                      /**< (US_IER) Non Acknowledge Interrupt Enable Mask */
#define US_IER_NACK(value)                    (US_IER_NACK_Msk & ((value) << US_IER_NACK_Pos))
#define US_IER_RIIC_Pos                       (16U)                                          /**< (US_IER) Ring Indicator Input Change Enable Position */
#define US_IER_RIIC_Msk                       (0x1U << US_IER_RIIC_Pos)                      /**< (US_IER) Ring Indicator Input Change Enable Mask */
#define US_IER_RIIC(value)                    (US_IER_RIIC_Msk & ((value) << US_IER_RIIC_Pos))
#define US_IER_DSRIC_Pos                      (17U)                                          /**< (US_IER) Data Set Ready Input Change Enable Position */
#define US_IER_DSRIC_Msk                      (0x1U << US_IER_DSRIC_Pos)                     /**< (US_IER) Data Set Ready Input Change Enable Mask */
#define US_IER_DSRIC(value)                   (US_IER_DSRIC_Msk & ((value) << US_IER_DSRIC_Pos))
#define US_IER_DCDIC_Pos                      (18U)                                          /**< (US_IER) Data Carrier Detect Input Change Interrupt Enable Position */
#define US_IER_DCDIC_Msk                      (0x1U << US_IER_DCDIC_Pos)                     /**< (US_IER) Data Carrier Detect Input Change Interrupt Enable Mask */
#define US_IER_DCDIC(value)                   (US_IER_DCDIC_Msk & ((value) << US_IER_DCDIC_Pos))
#define US_IER_CTSIC_Pos                      (19U)                                          /**< (US_IER) Clear to Send Input Change Interrupt Enable Position */
#define US_IER_CTSIC_Msk                      (0x1U << US_IER_CTSIC_Pos)                     /**< (US_IER) Clear to Send Input Change Interrupt Enable Mask */
#define US_IER_CTSIC(value)                   (US_IER_CTSIC_Msk & ((value) << US_IER_CTSIC_Pos))
#define US_IER_MANE_Pos                       (24U)                                          /**< (US_IER) Manchester Error Interrupt Enable Position */
#define US_IER_MANE_Msk                       (0x1U << US_IER_MANE_Pos)                      /**< (US_IER) Manchester Error Interrupt Enable Mask */
#define US_IER_MANE(value)                    (US_IER_MANE_Msk & ((value) << US_IER_MANE_Pos))
#define US_IER_UNRE_Pos                       (10U)                                          /**< (US_IER) Underrun Error Interrupt Enable Position */
#define US_IER_UNRE_Msk                       (0x1U << US_IER_UNRE_Pos)                      /**< (US_IER) Underrun Error Interrupt Enable Mask */
#define US_IER_UNRE(value)                    (US_IER_UNRE_Msk & ((value) << US_IER_UNRE_Pos))
#define US_IER_NSSE_Pos                       (19U)                                          /**< (US_IER) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Position */
#define US_IER_NSSE_Msk                       (0x1U << US_IER_NSSE_Pos)                      /**< (US_IER) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Mask */
#define US_IER_NSSE(value)                    (US_IER_NSSE_Msk & ((value) << US_IER_NSSE_Pos))
#define US_IER_LINBK_Pos                      (13U)                                          /**< (US_IER) LIN Break Sent or LIN Break Received Interrupt Enable Position */
#define US_IER_LINBK_Msk                      (0x1U << US_IER_LINBK_Pos)                     /**< (US_IER) LIN Break Sent or LIN Break Received Interrupt Enable Mask */
#define US_IER_LINBK(value)                   (US_IER_LINBK_Msk & ((value) << US_IER_LINBK_Pos))
#define US_IER_LINID_Pos                      (14U)                                          /**< (US_IER) LIN Identifier Sent or LIN Identifier Received Interrupt Enable Position */
#define US_IER_LINID_Msk                      (0x1U << US_IER_LINID_Pos)                     /**< (US_IER) LIN Identifier Sent or LIN Identifier Received Interrupt Enable Mask */
#define US_IER_LINID(value)                   (US_IER_LINID_Msk & ((value) << US_IER_LINID_Pos))
#define US_IER_LINTC_Pos                      (15U)                                          /**< (US_IER) LIN Transfer Completed Interrupt Enable Position */
#define US_IER_LINTC_Msk                      (0x1U << US_IER_LINTC_Pos)                     /**< (US_IER) LIN Transfer Completed Interrupt Enable Mask */
#define US_IER_LINTC(value)                   (US_IER_LINTC_Msk & ((value) << US_IER_LINTC_Pos))
#define US_IER_LINBE_Pos                      (25U)                                          /**< (US_IER) LIN Bus Error Interrupt Enable Position */
#define US_IER_LINBE_Msk                      (0x1U << US_IER_LINBE_Pos)                     /**< (US_IER) LIN Bus Error Interrupt Enable Mask */
#define US_IER_LINBE(value)                   (US_IER_LINBE_Msk & ((value) << US_IER_LINBE_Pos))
#define US_IER_LINISFE_Pos                    (26U)                                          /**< (US_IER) LIN Inconsistent Synch Field Error Interrupt Enable Position */
#define US_IER_LINISFE_Msk                    (0x1U << US_IER_LINISFE_Pos)                   /**< (US_IER) LIN Inconsistent Synch Field Error Interrupt Enable Mask */
#define US_IER_LINISFE(value)                 (US_IER_LINISFE_Msk & ((value) << US_IER_LINISFE_Pos))
#define US_IER_LINIPE_Pos                     (27U)                                          /**< (US_IER) LIN Identifier Parity Interrupt Enable Position */
#define US_IER_LINIPE_Msk                     (0x1U << US_IER_LINIPE_Pos)                    /**< (US_IER) LIN Identifier Parity Interrupt Enable Mask */
#define US_IER_LINIPE(value)                  (US_IER_LINIPE_Msk & ((value) << US_IER_LINIPE_Pos))
#define US_IER_LINCE_Pos                      (28U)                                          /**< (US_IER) LIN Checksum Error Interrupt Enable Position */
#define US_IER_LINCE_Msk                      (0x1U << US_IER_LINCE_Pos)                     /**< (US_IER) LIN Checksum Error Interrupt Enable Mask */
#define US_IER_LINCE(value)                   (US_IER_LINCE_Msk & ((value) << US_IER_LINCE_Pos))
#define US_IER_LINSNRE_Pos                    (29U)                                          /**< (US_IER) LIN Slave Not Responding Error Interrupt Enable Position */
#define US_IER_LINSNRE_Msk                    (0x1U << US_IER_LINSNRE_Pos)                   /**< (US_IER) LIN Slave Not Responding Error Interrupt Enable Mask */
#define US_IER_LINSNRE(value)                 (US_IER_LINSNRE_Msk & ((value) << US_IER_LINSNRE_Pos))
#define US_IER_LINSTE_Pos                     (30U)                                          /**< (US_IER) LIN Synch Tolerance Error Interrupt Enable Position */
#define US_IER_LINSTE_Msk                     (0x1U << US_IER_LINSTE_Pos)                    /**< (US_IER) LIN Synch Tolerance Error Interrupt Enable Mask */
#define US_IER_LINSTE(value)                  (US_IER_LINSTE_Msk & ((value) << US_IER_LINSTE_Pos))
#define US_IER_LINHTE_Pos                     (31U)                                          /**< (US_IER) LIN Header Timeout Error Interrupt Enable Position */
#define US_IER_LINHTE_Msk                     (0x1U << US_IER_LINHTE_Pos)                    /**< (US_IER) LIN Header Timeout Error Interrupt Enable Mask */
#define US_IER_LINHTE(value)                  (US_IER_LINHTE_Msk & ((value) << US_IER_LINHTE_Pos))
#define US_IER_LSFE_Pos                       (6U)                                           /**< (US_IER) LON Short Frame Error Interrupt Enable Position */
#define US_IER_LSFE_Msk                       (0x1U << US_IER_LSFE_Pos)                      /**< (US_IER) LON Short Frame Error Interrupt Enable Mask */
#define US_IER_LSFE(value)                    (US_IER_LSFE_Msk & ((value) << US_IER_LSFE_Pos))
#define US_IER_LCRCE_Pos                      (7U)                                           /**< (US_IER) LON CRC Error Interrupt Enable Position */
#define US_IER_LCRCE_Msk                      (0x1U << US_IER_LCRCE_Pos)                     /**< (US_IER) LON CRC Error Interrupt Enable Mask */
#define US_IER_LCRCE(value)                   (US_IER_LCRCE_Msk & ((value) << US_IER_LCRCE_Pos))
#define US_IER_LTXD_Pos                       (24U)                                          /**< (US_IER) LON Transmission Done Interrupt Enable Position */
#define US_IER_LTXD_Msk                       (0x1U << US_IER_LTXD_Pos)                      /**< (US_IER) LON Transmission Done Interrupt Enable Mask */
#define US_IER_LTXD(value)                    (US_IER_LTXD_Msk & ((value) << US_IER_LTXD_Pos))
#define US_IER_LCOL_Pos                       (25U)                                          /**< (US_IER) LON Collision Interrupt Enable Position */
#define US_IER_LCOL_Msk                       (0x1U << US_IER_LCOL_Pos)                      /**< (US_IER) LON Collision Interrupt Enable Mask */
#define US_IER_LCOL(value)                    (US_IER_LCOL_Msk & ((value) << US_IER_LCOL_Pos))
#define US_IER_LFET_Pos                       (26U)                                          /**< (US_IER) LON Frame Early Termination Interrupt Enable Position */
#define US_IER_LFET_Msk                       (0x1U << US_IER_LFET_Pos)                      /**< (US_IER) LON Frame Early Termination Interrupt Enable Mask */
#define US_IER_LFET(value)                    (US_IER_LFET_Msk & ((value) << US_IER_LFET_Pos))
#define US_IER_LRXD_Pos                       (27U)                                          /**< (US_IER) LON Reception Done Interrupt Enable Position */
#define US_IER_LRXD_Msk                       (0x1U << US_IER_LRXD_Pos)                      /**< (US_IER) LON Reception Done Interrupt Enable Mask */
#define US_IER_LRXD(value)                    (US_IER_LRXD_Msk & ((value) << US_IER_LRXD_Pos))
#define US_IER_LBLOVFE_Pos                    (28U)                                          /**< (US_IER) LON Backlog Overflow Error Interrupt Enable Position */
#define US_IER_LBLOVFE_Msk                    (0x1U << US_IER_LBLOVFE_Pos)                   /**< (US_IER) LON Backlog Overflow Error Interrupt Enable Mask */
#define US_IER_LBLOVFE(value)                 (US_IER_LBLOVFE_Msk & ((value) << US_IER_LBLOVFE_Pos))
#define US_IER_Msk                            (0xFF0FE7E7UL)                                 /**< (US_IER) Register Mask  */

/* -------- US_IDR : (USART Offset: 0x0C) ( /W 32) Interrupt Disable Register -------- */
#define US_IDR_RXRDY_Pos                      (0U)                                           /**< (US_IDR) RXRDY Interrupt Disable Position */
#define US_IDR_RXRDY_Msk                      (0x1U << US_IDR_RXRDY_Pos)                     /**< (US_IDR) RXRDY Interrupt Disable Mask */
#define US_IDR_RXRDY(value)                   (US_IDR_RXRDY_Msk & ((value) << US_IDR_RXRDY_Pos))
#define US_IDR_TXRDY_Pos                      (1U)                                           /**< (US_IDR) TXRDY Interrupt Disable Position */
#define US_IDR_TXRDY_Msk                      (0x1U << US_IDR_TXRDY_Pos)                     /**< (US_IDR) TXRDY Interrupt Disable Mask */
#define US_IDR_TXRDY(value)                   (US_IDR_TXRDY_Msk & ((value) << US_IDR_TXRDY_Pos))
#define US_IDR_RXBRK_Pos                      (2U)                                           /**< (US_IDR) Receiver Break Interrupt Disable Position */
#define US_IDR_RXBRK_Msk                      (0x1U << US_IDR_RXBRK_Pos)                     /**< (US_IDR) Receiver Break Interrupt Disable Mask */
#define US_IDR_RXBRK(value)                   (US_IDR_RXBRK_Msk & ((value) << US_IDR_RXBRK_Pos))
#define US_IDR_OVRE_Pos                       (5U)                                           /**< (US_IDR) Overrun Error Interrupt Enable Position */
#define US_IDR_OVRE_Msk                       (0x1U << US_IDR_OVRE_Pos)                      /**< (US_IDR) Overrun Error Interrupt Enable Mask */
#define US_IDR_OVRE(value)                    (US_IDR_OVRE_Msk & ((value) << US_IDR_OVRE_Pos))
#define US_IDR_FRAME_Pos                      (6U)                                           /**< (US_IDR) Framing Error Interrupt Disable Position */
#define US_IDR_FRAME_Msk                      (0x1U << US_IDR_FRAME_Pos)                     /**< (US_IDR) Framing Error Interrupt Disable Mask */
#define US_IDR_FRAME(value)                   (US_IDR_FRAME_Msk & ((value) << US_IDR_FRAME_Pos))
#define US_IDR_PARE_Pos                       (7U)                                           /**< (US_IDR) Parity Error Interrupt Disable Position */
#define US_IDR_PARE_Msk                       (0x1U << US_IDR_PARE_Pos)                      /**< (US_IDR) Parity Error Interrupt Disable Mask */
#define US_IDR_PARE(value)                    (US_IDR_PARE_Msk & ((value) << US_IDR_PARE_Pos))
#define US_IDR_TIMEOUT_Pos                    (8U)                                           /**< (US_IDR) Timeout Interrupt Disable Position */
#define US_IDR_TIMEOUT_Msk                    (0x1U << US_IDR_TIMEOUT_Pos)                   /**< (US_IDR) Timeout Interrupt Disable Mask */
#define US_IDR_TIMEOUT(value)                 (US_IDR_TIMEOUT_Msk & ((value) << US_IDR_TIMEOUT_Pos))
#define US_IDR_TXEMPTY_Pos                    (9U)                                           /**< (US_IDR) TXEMPTY Interrupt Disable Position */
#define US_IDR_TXEMPTY_Msk                    (0x1U << US_IDR_TXEMPTY_Pos)                   /**< (US_IDR) TXEMPTY Interrupt Disable Mask */
#define US_IDR_TXEMPTY(value)                 (US_IDR_TXEMPTY_Msk & ((value) << US_IDR_TXEMPTY_Pos))
#define US_IDR_ITER_Pos                       (10U)                                          /**< (US_IDR) Max Number of Repetitions Reached Interrupt Disable Position */
#define US_IDR_ITER_Msk                       (0x1U << US_IDR_ITER_Pos)                      /**< (US_IDR) Max Number of Repetitions Reached Interrupt Disable Mask */
#define US_IDR_ITER(value)                    (US_IDR_ITER_Msk & ((value) << US_IDR_ITER_Pos))
#define US_IDR_NACK_Pos                       (13U)                                          /**< (US_IDR) Non Acknowledge Interrupt Disable Position */
#define US_IDR_NACK_Msk                       (0x1U << US_IDR_NACK_Pos)                      /**< (US_IDR) Non Acknowledge Interrupt Disable Mask */
#define US_IDR_NACK(value)                    (US_IDR_NACK_Msk & ((value) << US_IDR_NACK_Pos))
#define US_IDR_RIIC_Pos                       (16U)                                          /**< (US_IDR) Ring Indicator Input Change Disable Position */
#define US_IDR_RIIC_Msk                       (0x1U << US_IDR_RIIC_Pos)                      /**< (US_IDR) Ring Indicator Input Change Disable Mask */
#define US_IDR_RIIC(value)                    (US_IDR_RIIC_Msk & ((value) << US_IDR_RIIC_Pos))
#define US_IDR_DSRIC_Pos                      (17U)                                          /**< (US_IDR) Data Set Ready Input Change Disable Position */
#define US_IDR_DSRIC_Msk                      (0x1U << US_IDR_DSRIC_Pos)                     /**< (US_IDR) Data Set Ready Input Change Disable Mask */
#define US_IDR_DSRIC(value)                   (US_IDR_DSRIC_Msk & ((value) << US_IDR_DSRIC_Pos))
#define US_IDR_DCDIC_Pos                      (18U)                                          /**< (US_IDR) Data Carrier Detect Input Change Interrupt Disable Position */
#define US_IDR_DCDIC_Msk                      (0x1U << US_IDR_DCDIC_Pos)                     /**< (US_IDR) Data Carrier Detect Input Change Interrupt Disable Mask */
#define US_IDR_DCDIC(value)                   (US_IDR_DCDIC_Msk & ((value) << US_IDR_DCDIC_Pos))
#define US_IDR_CTSIC_Pos                      (19U)                                          /**< (US_IDR) Clear to Send Input Change Interrupt Disable Position */
#define US_IDR_CTSIC_Msk                      (0x1U << US_IDR_CTSIC_Pos)                     /**< (US_IDR) Clear to Send Input Change Interrupt Disable Mask */
#define US_IDR_CTSIC(value)                   (US_IDR_CTSIC_Msk & ((value) << US_IDR_CTSIC_Pos))
#define US_IDR_MANE_Pos                       (24U)                                          /**< (US_IDR) Manchester Error Interrupt Disable Position */
#define US_IDR_MANE_Msk                       (0x1U << US_IDR_MANE_Pos)                      /**< (US_IDR) Manchester Error Interrupt Disable Mask */
#define US_IDR_MANE(value)                    (US_IDR_MANE_Msk & ((value) << US_IDR_MANE_Pos))
#define US_IDR_UNRE_Pos                       (10U)                                          /**< (US_IDR) SPI Underrun Error Interrupt Disable Position */
#define US_IDR_UNRE_Msk                       (0x1U << US_IDR_UNRE_Pos)                      /**< (US_IDR) SPI Underrun Error Interrupt Disable Mask */
#define US_IDR_UNRE(value)                    (US_IDR_UNRE_Msk & ((value) << US_IDR_UNRE_Pos))
#define US_IDR_NSSE_Pos                       (19U)                                          /**< (US_IDR) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Position */
#define US_IDR_NSSE_Msk                       (0x1U << US_IDR_NSSE_Pos)                      /**< (US_IDR) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Mask */
#define US_IDR_NSSE(value)                    (US_IDR_NSSE_Msk & ((value) << US_IDR_NSSE_Pos))
#define US_IDR_LINBK_Pos                      (13U)                                          /**< (US_IDR) LIN Break Sent or LIN Break Received Interrupt Disable Position */
#define US_IDR_LINBK_Msk                      (0x1U << US_IDR_LINBK_Pos)                     /**< (US_IDR) LIN Break Sent or LIN Break Received Interrupt Disable Mask */
#define US_IDR_LINBK(value)                   (US_IDR_LINBK_Msk & ((value) << US_IDR_LINBK_Pos))
#define US_IDR_LINID_Pos                      (14U)                                          /**< (US_IDR) LIN Identifier Sent or LIN Identifier Received Interrupt Disable Position */
#define US_IDR_LINID_Msk                      (0x1U << US_IDR_LINID_Pos)                     /**< (US_IDR) LIN Identifier Sent or LIN Identifier Received Interrupt Disable Mask */
#define US_IDR_LINID(value)                   (US_IDR_LINID_Msk & ((value) << US_IDR_LINID_Pos))
#define US_IDR_LINTC_Pos                      (15U)                                          /**< (US_IDR) LIN Transfer Completed Interrupt Disable Position */
#define US_IDR_LINTC_Msk                      (0x1U << US_IDR_LINTC_Pos)                     /**< (US_IDR) LIN Transfer Completed Interrupt Disable Mask */
#define US_IDR_LINTC(value)                   (US_IDR_LINTC_Msk & ((value) << US_IDR_LINTC_Pos))
#define US_IDR_LINBE_Pos                      (25U)                                          /**< (US_IDR) LIN Bus Error Interrupt Disable Position */
#define US_IDR_LINBE_Msk                      (0x1U << US_IDR_LINBE_Pos)                     /**< (US_IDR) LIN Bus Error Interrupt Disable Mask */
#define US_IDR_LINBE(value)                   (US_IDR_LINBE_Msk & ((value) << US_IDR_LINBE_Pos))
#define US_IDR_LINISFE_Pos                    (26U)                                          /**< (US_IDR) LIN Inconsistent Synch Field Error Interrupt Disable Position */
#define US_IDR_LINISFE_Msk                    (0x1U << US_IDR_LINISFE_Pos)                   /**< (US_IDR) LIN Inconsistent Synch Field Error Interrupt Disable Mask */
#define US_IDR_LINISFE(value)                 (US_IDR_LINISFE_Msk & ((value) << US_IDR_LINISFE_Pos))
#define US_IDR_LINIPE_Pos                     (27U)                                          /**< (US_IDR) LIN Identifier Parity Interrupt Disable Position */
#define US_IDR_LINIPE_Msk                     (0x1U << US_IDR_LINIPE_Pos)                    /**< (US_IDR) LIN Identifier Parity Interrupt Disable Mask */
#define US_IDR_LINIPE(value)                  (US_IDR_LINIPE_Msk & ((value) << US_IDR_LINIPE_Pos))
#define US_IDR_LINCE_Pos                      (28U)                                          /**< (US_IDR) LIN Checksum Error Interrupt Disable Position */
#define US_IDR_LINCE_Msk                      (0x1U << US_IDR_LINCE_Pos)                     /**< (US_IDR) LIN Checksum Error Interrupt Disable Mask */
#define US_IDR_LINCE(value)                   (US_IDR_LINCE_Msk & ((value) << US_IDR_LINCE_Pos))
#define US_IDR_LINSNRE_Pos                    (29U)                                          /**< (US_IDR) LIN Slave Not Responding Error Interrupt Disable Position */
#define US_IDR_LINSNRE_Msk                    (0x1U << US_IDR_LINSNRE_Pos)                   /**< (US_IDR) LIN Slave Not Responding Error Interrupt Disable Mask */
#define US_IDR_LINSNRE(value)                 (US_IDR_LINSNRE_Msk & ((value) << US_IDR_LINSNRE_Pos))
#define US_IDR_LINSTE_Pos                     (30U)                                          /**< (US_IDR) LIN Synch Tolerance Error Interrupt Disable Position */
#define US_IDR_LINSTE_Msk                     (0x1U << US_IDR_LINSTE_Pos)                    /**< (US_IDR) LIN Synch Tolerance Error Interrupt Disable Mask */
#define US_IDR_LINSTE(value)                  (US_IDR_LINSTE_Msk & ((value) << US_IDR_LINSTE_Pos))
#define US_IDR_LINHTE_Pos                     (31U)                                          /**< (US_IDR) LIN Header Timeout Error Interrupt Disable Position */
#define US_IDR_LINHTE_Msk                     (0x1U << US_IDR_LINHTE_Pos)                    /**< (US_IDR) LIN Header Timeout Error Interrupt Disable Mask */
#define US_IDR_LINHTE(value)                  (US_IDR_LINHTE_Msk & ((value) << US_IDR_LINHTE_Pos))
#define US_IDR_LSFE_Pos                       (6U)                                           /**< (US_IDR) LON Short Frame Error Interrupt Disable Position */
#define US_IDR_LSFE_Msk                       (0x1U << US_IDR_LSFE_Pos)                      /**< (US_IDR) LON Short Frame Error Interrupt Disable Mask */
#define US_IDR_LSFE(value)                    (US_IDR_LSFE_Msk & ((value) << US_IDR_LSFE_Pos))
#define US_IDR_LCRCE_Pos                      (7U)                                           /**< (US_IDR) LON CRC Error Interrupt Disable Position */
#define US_IDR_LCRCE_Msk                      (0x1U << US_IDR_LCRCE_Pos)                     /**< (US_IDR) LON CRC Error Interrupt Disable Mask */
#define US_IDR_LCRCE(value)                   (US_IDR_LCRCE_Msk & ((value) << US_IDR_LCRCE_Pos))
#define US_IDR_LTXD_Pos                       (24U)                                          /**< (US_IDR) LON Transmission Done Interrupt Disable Position */
#define US_IDR_LTXD_Msk                       (0x1U << US_IDR_LTXD_Pos)                      /**< (US_IDR) LON Transmission Done Interrupt Disable Mask */
#define US_IDR_LTXD(value)                    (US_IDR_LTXD_Msk & ((value) << US_IDR_LTXD_Pos))
#define US_IDR_LCOL_Pos                       (25U)                                          /**< (US_IDR) LON Collision Interrupt Disable Position */
#define US_IDR_LCOL_Msk                       (0x1U << US_IDR_LCOL_Pos)                      /**< (US_IDR) LON Collision Interrupt Disable Mask */
#define US_IDR_LCOL(value)                    (US_IDR_LCOL_Msk & ((value) << US_IDR_LCOL_Pos))
#define US_IDR_LFET_Pos                       (26U)                                          /**< (US_IDR) LON Frame Early Termination Interrupt Disable Position */
#define US_IDR_LFET_Msk                       (0x1U << US_IDR_LFET_Pos)                      /**< (US_IDR) LON Frame Early Termination Interrupt Disable Mask */
#define US_IDR_LFET(value)                    (US_IDR_LFET_Msk & ((value) << US_IDR_LFET_Pos))
#define US_IDR_LRXD_Pos                       (27U)                                          /**< (US_IDR) LON Reception Done Interrupt Disable Position */
#define US_IDR_LRXD_Msk                       (0x1U << US_IDR_LRXD_Pos)                      /**< (US_IDR) LON Reception Done Interrupt Disable Mask */
#define US_IDR_LRXD(value)                    (US_IDR_LRXD_Msk & ((value) << US_IDR_LRXD_Pos))
#define US_IDR_LBLOVFE_Pos                    (28U)                                          /**< (US_IDR) LON Backlog Overflow Error Interrupt Disable Position */
#define US_IDR_LBLOVFE_Msk                    (0x1U << US_IDR_LBLOVFE_Pos)                   /**< (US_IDR) LON Backlog Overflow Error Interrupt Disable Mask */
#define US_IDR_LBLOVFE(value)                 (US_IDR_LBLOVFE_Msk & ((value) << US_IDR_LBLOVFE_Pos))
#define US_IDR_Msk                            (0xFF0FE7E7UL)                                 /**< (US_IDR) Register Mask  */

/* -------- US_IMR : (USART Offset: 0x10) (R/  32) Interrupt Mask Register -------- */
#define US_IMR_RXRDY_Pos                      (0U)                                           /**< (US_IMR) RXRDY Interrupt Mask Position */
#define US_IMR_RXRDY_Msk                      (0x1U << US_IMR_RXRDY_Pos)                     /**< (US_IMR) RXRDY Interrupt Mask Mask */
#define US_IMR_RXRDY(value)                   (US_IMR_RXRDY_Msk & ((value) << US_IMR_RXRDY_Pos))
#define US_IMR_TXRDY_Pos                      (1U)                                           /**< (US_IMR) TXRDY Interrupt Mask Position */
#define US_IMR_TXRDY_Msk                      (0x1U << US_IMR_TXRDY_Pos)                     /**< (US_IMR) TXRDY Interrupt Mask Mask */
#define US_IMR_TXRDY(value)                   (US_IMR_TXRDY_Msk & ((value) << US_IMR_TXRDY_Pos))
#define US_IMR_RXBRK_Pos                      (2U)                                           /**< (US_IMR) Receiver Break Interrupt Mask Position */
#define US_IMR_RXBRK_Msk                      (0x1U << US_IMR_RXBRK_Pos)                     /**< (US_IMR) Receiver Break Interrupt Mask Mask */
#define US_IMR_RXBRK(value)                   (US_IMR_RXBRK_Msk & ((value) << US_IMR_RXBRK_Pos))
#define US_IMR_OVRE_Pos                       (5U)                                           /**< (US_IMR) Overrun Error Interrupt Mask Position */
#define US_IMR_OVRE_Msk                       (0x1U << US_IMR_OVRE_Pos)                      /**< (US_IMR) Overrun Error Interrupt Mask Mask */
#define US_IMR_OVRE(value)                    (US_IMR_OVRE_Msk & ((value) << US_IMR_OVRE_Pos))
#define US_IMR_FRAME_Pos                      (6U)                                           /**< (US_IMR) Framing Error Interrupt Mask Position */
#define US_IMR_FRAME_Msk                      (0x1U << US_IMR_FRAME_Pos)                     /**< (US_IMR) Framing Error Interrupt Mask Mask */
#define US_IMR_FRAME(value)                   (US_IMR_FRAME_Msk & ((value) << US_IMR_FRAME_Pos))
#define US_IMR_PARE_Pos                       (7U)                                           /**< (US_IMR) Parity Error Interrupt Mask Position */
#define US_IMR_PARE_Msk                       (0x1U << US_IMR_PARE_Pos)                      /**< (US_IMR) Parity Error Interrupt Mask Mask */
#define US_IMR_PARE(value)                    (US_IMR_PARE_Msk & ((value) << US_IMR_PARE_Pos))
#define US_IMR_TIMEOUT_Pos                    (8U)                                           /**< (US_IMR) Timeout Interrupt Mask Position */
#define US_IMR_TIMEOUT_Msk                    (0x1U << US_IMR_TIMEOUT_Pos)                   /**< (US_IMR) Timeout Interrupt Mask Mask */
#define US_IMR_TIMEOUT(value)                 (US_IMR_TIMEOUT_Msk & ((value) << US_IMR_TIMEOUT_Pos))
#define US_IMR_TXEMPTY_Pos                    (9U)                                           /**< (US_IMR) TXEMPTY Interrupt Mask Position */
#define US_IMR_TXEMPTY_Msk                    (0x1U << US_IMR_TXEMPTY_Pos)                   /**< (US_IMR) TXEMPTY Interrupt Mask Mask */
#define US_IMR_TXEMPTY(value)                 (US_IMR_TXEMPTY_Msk & ((value) << US_IMR_TXEMPTY_Pos))
#define US_IMR_ITER_Pos                       (10U)                                          /**< (US_IMR) Max Number of Repetitions Reached Interrupt Mask Position */
#define US_IMR_ITER_Msk                       (0x1U << US_IMR_ITER_Pos)                      /**< (US_IMR) Max Number of Repetitions Reached Interrupt Mask Mask */
#define US_IMR_ITER(value)                    (US_IMR_ITER_Msk & ((value) << US_IMR_ITER_Pos))
#define US_IMR_NACK_Pos                       (13U)                                          /**< (US_IMR) Non Acknowledge Interrupt Mask Position */
#define US_IMR_NACK_Msk                       (0x1U << US_IMR_NACK_Pos)                      /**< (US_IMR) Non Acknowledge Interrupt Mask Mask */
#define US_IMR_NACK(value)                    (US_IMR_NACK_Msk & ((value) << US_IMR_NACK_Pos))
#define US_IMR_RIIC_Pos                       (16U)                                          /**< (US_IMR) Ring Indicator Input Change Mask Position */
#define US_IMR_RIIC_Msk                       (0x1U << US_IMR_RIIC_Pos)                      /**< (US_IMR) Ring Indicator Input Change Mask Mask */
#define US_IMR_RIIC(value)                    (US_IMR_RIIC_Msk & ((value) << US_IMR_RIIC_Pos))
#define US_IMR_DSRIC_Pos                      (17U)                                          /**< (US_IMR) Data Set Ready Input Change Mask Position */
#define US_IMR_DSRIC_Msk                      (0x1U << US_IMR_DSRIC_Pos)                     /**< (US_IMR) Data Set Ready Input Change Mask Mask */
#define US_IMR_DSRIC(value)                   (US_IMR_DSRIC_Msk & ((value) << US_IMR_DSRIC_Pos))
#define US_IMR_DCDIC_Pos                      (18U)                                          /**< (US_IMR) Data Carrier Detect Input Change Interrupt Mask Position */
#define US_IMR_DCDIC_Msk                      (0x1U << US_IMR_DCDIC_Pos)                     /**< (US_IMR) Data Carrier Detect Input Change Interrupt Mask Mask */
#define US_IMR_DCDIC(value)                   (US_IMR_DCDIC_Msk & ((value) << US_IMR_DCDIC_Pos))
#define US_IMR_CTSIC_Pos                      (19U)                                          /**< (US_IMR) Clear to Send Input Change Interrupt Mask Position */
#define US_IMR_CTSIC_Msk                      (0x1U << US_IMR_CTSIC_Pos)                     /**< (US_IMR) Clear to Send Input Change Interrupt Mask Mask */
#define US_IMR_CTSIC(value)                   (US_IMR_CTSIC_Msk & ((value) << US_IMR_CTSIC_Pos))
#define US_IMR_MANE_Pos                       (24U)                                          /**< (US_IMR) Manchester Error Interrupt Mask Position */
#define US_IMR_MANE_Msk                       (0x1U << US_IMR_MANE_Pos)                      /**< (US_IMR) Manchester Error Interrupt Mask Mask */
#define US_IMR_MANE(value)                    (US_IMR_MANE_Msk & ((value) << US_IMR_MANE_Pos))
#define US_IMR_UNRE_Pos                       (10U)                                          /**< (US_IMR) SPI Underrun Error Interrupt Mask Position */
#define US_IMR_UNRE_Msk                       (0x1U << US_IMR_UNRE_Pos)                      /**< (US_IMR) SPI Underrun Error Interrupt Mask Mask */
#define US_IMR_UNRE(value)                    (US_IMR_UNRE_Msk & ((value) << US_IMR_UNRE_Pos))
#define US_IMR_NSSE_Pos                       (19U)                                          /**< (US_IMR) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Position */
#define US_IMR_NSSE_Msk                       (0x1U << US_IMR_NSSE_Pos)                      /**< (US_IMR) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Mask */
#define US_IMR_NSSE(value)                    (US_IMR_NSSE_Msk & ((value) << US_IMR_NSSE_Pos))
#define US_IMR_LINBK_Pos                      (13U)                                          /**< (US_IMR) LIN Break Sent or LIN Break Received Interrupt Mask Position */
#define US_IMR_LINBK_Msk                      (0x1U << US_IMR_LINBK_Pos)                     /**< (US_IMR) LIN Break Sent or LIN Break Received Interrupt Mask Mask */
#define US_IMR_LINBK(value)                   (US_IMR_LINBK_Msk & ((value) << US_IMR_LINBK_Pos))
#define US_IMR_LINID_Pos                      (14U)                                          /**< (US_IMR) LIN Identifier Sent or LIN Identifier Received Interrupt Mask Position */
#define US_IMR_LINID_Msk                      (0x1U << US_IMR_LINID_Pos)                     /**< (US_IMR) LIN Identifier Sent or LIN Identifier Received Interrupt Mask Mask */
#define US_IMR_LINID(value)                   (US_IMR_LINID_Msk & ((value) << US_IMR_LINID_Pos))
#define US_IMR_LINTC_Pos                      (15U)                                          /**< (US_IMR) LIN Transfer Completed Interrupt Mask Position */
#define US_IMR_LINTC_Msk                      (0x1U << US_IMR_LINTC_Pos)                     /**< (US_IMR) LIN Transfer Completed Interrupt Mask Mask */
#define US_IMR_LINTC(value)                   (US_IMR_LINTC_Msk & ((value) << US_IMR_LINTC_Pos))
#define US_IMR_LINBE_Pos                      (25U)                                          /**< (US_IMR) LIN Bus Error Interrupt Mask Position */
#define US_IMR_LINBE_Msk                      (0x1U << US_IMR_LINBE_Pos)                     /**< (US_IMR) LIN Bus Error Interrupt Mask Mask */
#define US_IMR_LINBE(value)                   (US_IMR_LINBE_Msk & ((value) << US_IMR_LINBE_Pos))
#define US_IMR_LINISFE_Pos                    (26U)                                          /**< (US_IMR) LIN Inconsistent Synch Field Error Interrupt Mask Position */
#define US_IMR_LINISFE_Msk                    (0x1U << US_IMR_LINISFE_Pos)                   /**< (US_IMR) LIN Inconsistent Synch Field Error Interrupt Mask Mask */
#define US_IMR_LINISFE(value)                 (US_IMR_LINISFE_Msk & ((value) << US_IMR_LINISFE_Pos))
#define US_IMR_LINIPE_Pos                     (27U)                                          /**< (US_IMR) LIN Identifier Parity Interrupt Mask Position */
#define US_IMR_LINIPE_Msk                     (0x1U << US_IMR_LINIPE_Pos)                    /**< (US_IMR) LIN Identifier Parity Interrupt Mask Mask */
#define US_IMR_LINIPE(value)                  (US_IMR_LINIPE_Msk & ((value) << US_IMR_LINIPE_Pos))
#define US_IMR_LINCE_Pos                      (28U)                                          /**< (US_IMR) LIN Checksum Error Interrupt Mask Position */
#define US_IMR_LINCE_Msk                      (0x1U << US_IMR_LINCE_Pos)                     /**< (US_IMR) LIN Checksum Error Interrupt Mask Mask */
#define US_IMR_LINCE(value)                   (US_IMR_LINCE_Msk & ((value) << US_IMR_LINCE_Pos))
#define US_IMR_LINSNRE_Pos                    (29U)                                          /**< (US_IMR) LIN Slave Not Responding Error Interrupt Mask Position */
#define US_IMR_LINSNRE_Msk                    (0x1U << US_IMR_LINSNRE_Pos)                   /**< (US_IMR) LIN Slave Not Responding Error Interrupt Mask Mask */
#define US_IMR_LINSNRE(value)                 (US_IMR_LINSNRE_Msk & ((value) << US_IMR_LINSNRE_Pos))
#define US_IMR_LINSTE_Pos                     (30U)                                          /**< (US_IMR) LIN Synch Tolerance Error Interrupt Mask Position */
#define US_IMR_LINSTE_Msk                     (0x1U << US_IMR_LINSTE_Pos)                    /**< (US_IMR) LIN Synch Tolerance Error Interrupt Mask Mask */
#define US_IMR_LINSTE(value)                  (US_IMR_LINSTE_Msk & ((value) << US_IMR_LINSTE_Pos))
#define US_IMR_LINHTE_Pos                     (31U)                                          /**< (US_IMR) LIN Header Timeout Error Interrupt Mask Position */
#define US_IMR_LINHTE_Msk                     (0x1U << US_IMR_LINHTE_Pos)                    /**< (US_IMR) LIN Header Timeout Error Interrupt Mask Mask */
#define US_IMR_LINHTE(value)                  (US_IMR_LINHTE_Msk & ((value) << US_IMR_LINHTE_Pos))
#define US_IMR_LSFE_Pos                       (6U)                                           /**< (US_IMR) LON Short Frame Error Interrupt Mask Position */
#define US_IMR_LSFE_Msk                       (0x1U << US_IMR_LSFE_Pos)                      /**< (US_IMR) LON Short Frame Error Interrupt Mask Mask */
#define US_IMR_LSFE(value)                    (US_IMR_LSFE_Msk & ((value) << US_IMR_LSFE_Pos))
#define US_IMR_LCRCE_Pos                      (7U)                                           /**< (US_IMR) LON CRC Error Interrupt Mask Position */
#define US_IMR_LCRCE_Msk                      (0x1U << US_IMR_LCRCE_Pos)                     /**< (US_IMR) LON CRC Error Interrupt Mask Mask */
#define US_IMR_LCRCE(value)                   (US_IMR_LCRCE_Msk & ((value) << US_IMR_LCRCE_Pos))
#define US_IMR_LTXD_Pos                       (24U)                                          /**< (US_IMR) LON Transmission Done Interrupt Mask Position */
#define US_IMR_LTXD_Msk                       (0x1U << US_IMR_LTXD_Pos)                      /**< (US_IMR) LON Transmission Done Interrupt Mask Mask */
#define US_IMR_LTXD(value)                    (US_IMR_LTXD_Msk & ((value) << US_IMR_LTXD_Pos))
#define US_IMR_LCOL_Pos                       (25U)                                          /**< (US_IMR) LON Collision Interrupt Mask Position */
#define US_IMR_LCOL_Msk                       (0x1U << US_IMR_LCOL_Pos)                      /**< (US_IMR) LON Collision Interrupt Mask Mask */
#define US_IMR_LCOL(value)                    (US_IMR_LCOL_Msk & ((value) << US_IMR_LCOL_Pos))
#define US_IMR_LFET_Pos                       (26U)                                          /**< (US_IMR) LON Frame Early Termination Interrupt Mask Position */
#define US_IMR_LFET_Msk                       (0x1U << US_IMR_LFET_Pos)                      /**< (US_IMR) LON Frame Early Termination Interrupt Mask Mask */
#define US_IMR_LFET(value)                    (US_IMR_LFET_Msk & ((value) << US_IMR_LFET_Pos))
#define US_IMR_LRXD_Pos                       (27U)                                          /**< (US_IMR) LON Reception Done Interrupt Mask Position */
#define US_IMR_LRXD_Msk                       (0x1U << US_IMR_LRXD_Pos)                      /**< (US_IMR) LON Reception Done Interrupt Mask Mask */
#define US_IMR_LRXD(value)                    (US_IMR_LRXD_Msk & ((value) << US_IMR_LRXD_Pos))
#define US_IMR_LBLOVFE_Pos                    (28U)                                          /**< (US_IMR) LON Backlog Overflow Error Interrupt Mask Position */
#define US_IMR_LBLOVFE_Msk                    (0x1U << US_IMR_LBLOVFE_Pos)                   /**< (US_IMR) LON Backlog Overflow Error Interrupt Mask Mask */
#define US_IMR_LBLOVFE(value)                 (US_IMR_LBLOVFE_Msk & ((value) << US_IMR_LBLOVFE_Pos))
#define US_IMR_Msk                            (0xFF0FE7E7UL)                                 /**< (US_IMR) Register Mask  */

/* -------- US_CSR : (USART Offset: 0x14) (R/  32) Channel Status Register -------- */
#define US_CSR_RXRDY_Pos                      (0U)                                           /**< (US_CSR) Receiver Ready (cleared by reading US_RHR) Position */
#define US_CSR_RXRDY_Msk                      (0x1U << US_CSR_RXRDY_Pos)                     /**< (US_CSR) Receiver Ready (cleared by reading US_RHR) Mask */
#define US_CSR_RXRDY(value)                   (US_CSR_RXRDY_Msk & ((value) << US_CSR_RXRDY_Pos))
#define US_CSR_TXRDY_Pos                      (1U)                                           /**< (US_CSR) Transmitter Ready (cleared by writing US_THR) Position */
#define US_CSR_TXRDY_Msk                      (0x1U << US_CSR_TXRDY_Pos)                     /**< (US_CSR) Transmitter Ready (cleared by writing US_THR) Mask */
#define US_CSR_TXRDY(value)                   (US_CSR_TXRDY_Msk & ((value) << US_CSR_TXRDY_Pos))
#define US_CSR_RXBRK_Pos                      (2U)                                           /**< (US_CSR) Break Received/End of Break (cleared by writing a one to bit US_CR.RSTSTA) Position */
#define US_CSR_RXBRK_Msk                      (0x1U << US_CSR_RXBRK_Pos)                     /**< (US_CSR) Break Received/End of Break (cleared by writing a one to bit US_CR.RSTSTA) Mask */
#define US_CSR_RXBRK(value)                   (US_CSR_RXBRK_Msk & ((value) << US_CSR_RXBRK_Pos))
#define US_CSR_OVRE_Pos                       (5U)                                           /**< (US_CSR) Overrun Error (cleared by writing a one to bit US_CR.RSTSTA) Position */
#define US_CSR_OVRE_Msk                       (0x1U << US_CSR_OVRE_Pos)                      /**< (US_CSR) Overrun Error (cleared by writing a one to bit US_CR.RSTSTA) Mask */
#define US_CSR_OVRE(value)                    (US_CSR_OVRE_Msk & ((value) << US_CSR_OVRE_Pos))
#define US_CSR_FRAME_Pos                      (6U)                                           /**< (US_CSR) Framing Error (cleared by writing a one to bit US_CR.RSTSTA) Position */
#define US_CSR_FRAME_Msk                      (0x1U << US_CSR_FRAME_Pos)                     /**< (US_CSR) Framing Error (cleared by writing a one to bit US_CR.RSTSTA) Mask */
#define US_CSR_FRAME(value)                   (US_CSR_FRAME_Msk & ((value) << US_CSR_FRAME_Pos))
#define US_CSR_PARE_Pos                       (7U)                                           /**< (US_CSR) Parity Error (cleared by writing a one to bit US_CR.RSTSTA) Position */
#define US_CSR_PARE_Msk                       (0x1U << US_CSR_PARE_Pos)                      /**< (US_CSR) Parity Error (cleared by writing a one to bit US_CR.RSTSTA) Mask */
#define US_CSR_PARE(value)                    (US_CSR_PARE_Msk & ((value) << US_CSR_PARE_Pos))
#define US_CSR_TIMEOUT_Pos                    (8U)                                           /**< (US_CSR) Receiver Timeout (cleared by writing a one to bit US_CR.STTTO) Position */
#define US_CSR_TIMEOUT_Msk                    (0x1U << US_CSR_TIMEOUT_Pos)                   /**< (US_CSR) Receiver Timeout (cleared by writing a one to bit US_CR.STTTO) Mask */
#define US_CSR_TIMEOUT(value)                 (US_CSR_TIMEOUT_Msk & ((value) << US_CSR_TIMEOUT_Pos))
#define US_CSR_TXEMPTY_Pos                    (9U)                                           /**< (US_CSR) Transmitter Empty (cleared by writing US_THR) Position */
#define US_CSR_TXEMPTY_Msk                    (0x1U << US_CSR_TXEMPTY_Pos)                   /**< (US_CSR) Transmitter Empty (cleared by writing US_THR) Mask */
#define US_CSR_TXEMPTY(value)                 (US_CSR_TXEMPTY_Msk & ((value) << US_CSR_TXEMPTY_Pos))
#define US_CSR_ITER_Pos                       (10U)                                          /**< (US_CSR) Max Number of Repetitions Reached (cleared by writing a one to bit US_CR.RSTIT) Position */
#define US_CSR_ITER_Msk                       (0x1U << US_CSR_ITER_Pos)                      /**< (US_CSR) Max Number of Repetitions Reached (cleared by writing a one to bit US_CR.RSTIT) Mask */
#define US_CSR_ITER(value)                    (US_CSR_ITER_Msk & ((value) << US_CSR_ITER_Pos))
#define US_CSR_NACK_Pos                       (13U)                                          /**< (US_CSR) Non Acknowledge Interrupt (cleared by writing a one to bit US_CR.RSTNACK) Position */
#define US_CSR_NACK_Msk                       (0x1U << US_CSR_NACK_Pos)                      /**< (US_CSR) Non Acknowledge Interrupt (cleared by writing a one to bit US_CR.RSTNACK) Mask */
#define US_CSR_NACK(value)                    (US_CSR_NACK_Msk & ((value) << US_CSR_NACK_Pos))
#define US_CSR_RIIC_Pos                       (16U)                                          /**< (US_CSR) Ring Indicator Input Change Flag (cleared on read) Position */
#define US_CSR_RIIC_Msk                       (0x1U << US_CSR_RIIC_Pos)                      /**< (US_CSR) Ring Indicator Input Change Flag (cleared on read) Mask */
#define US_CSR_RIIC(value)                    (US_CSR_RIIC_Msk & ((value) << US_CSR_RIIC_Pos))
#define US_CSR_DSRIC_Pos                      (17U)                                          /**< (US_CSR) Data Set Ready Input Change Flag (cleared on read) Position */
#define US_CSR_DSRIC_Msk                      (0x1U << US_CSR_DSRIC_Pos)                     /**< (US_CSR) Data Set Ready Input Change Flag (cleared on read) Mask */
#define US_CSR_DSRIC(value)                   (US_CSR_DSRIC_Msk & ((value) << US_CSR_DSRIC_Pos))
#define US_CSR_DCDIC_Pos                      (18U)                                          /**< (US_CSR) Data Carrier Detect Input Change Flag (cleared on read) Position */
#define US_CSR_DCDIC_Msk                      (0x1U << US_CSR_DCDIC_Pos)                     /**< (US_CSR) Data Carrier Detect Input Change Flag (cleared on read) Mask */
#define US_CSR_DCDIC(value)                   (US_CSR_DCDIC_Msk & ((value) << US_CSR_DCDIC_Pos))
#define US_CSR_CTSIC_Pos                      (19U)                                          /**< (US_CSR) Clear to Send Input Change Flag (cleared on read) Position */
#define US_CSR_CTSIC_Msk                      (0x1U << US_CSR_CTSIC_Pos)                     /**< (US_CSR) Clear to Send Input Change Flag (cleared on read) Mask */
#define US_CSR_CTSIC(value)                   (US_CSR_CTSIC_Msk & ((value) << US_CSR_CTSIC_Pos))
#define US_CSR_RI_Pos                         (20U)                                          /**< (US_CSR) Image of RI Input Position */
#define US_CSR_RI_Msk                         (0x1U << US_CSR_RI_Pos)                        /**< (US_CSR) Image of RI Input Mask */
#define US_CSR_RI(value)                      (US_CSR_RI_Msk & ((value) << US_CSR_RI_Pos))  
#define US_CSR_DSR_Pos                        (21U)                                          /**< (US_CSR) Image of DSR Input Position */
#define US_CSR_DSR_Msk                        (0x1U << US_CSR_DSR_Pos)                       /**< (US_CSR) Image of DSR Input Mask */
#define US_CSR_DSR(value)                     (US_CSR_DSR_Msk & ((value) << US_CSR_DSR_Pos))
#define US_CSR_DCD_Pos                        (22U)                                          /**< (US_CSR) Image of DCD Input Position */
#define US_CSR_DCD_Msk                        (0x1U << US_CSR_DCD_Pos)                       /**< (US_CSR) Image of DCD Input Mask */
#define US_CSR_DCD(value)                     (US_CSR_DCD_Msk & ((value) << US_CSR_DCD_Pos))
#define US_CSR_CTS_Pos                        (23U)                                          /**< (US_CSR) Image of CTS Input Position */
#define US_CSR_CTS_Msk                        (0x1U << US_CSR_CTS_Pos)                       /**< (US_CSR) Image of CTS Input Mask */
#define US_CSR_CTS(value)                     (US_CSR_CTS_Msk & ((value) << US_CSR_CTS_Pos))
#define US_CSR_MANERR_Pos                     (24U)                                          /**< (US_CSR) Manchester Error (cleared by writing a one to the bit US_CR.RSTSTA) Position */
#define US_CSR_MANERR_Msk                     (0x1U << US_CSR_MANERR_Pos)                    /**< (US_CSR) Manchester Error (cleared by writing a one to the bit US_CR.RSTSTA) Mask */
#define US_CSR_MANERR(value)                  (US_CSR_MANERR_Msk & ((value) << US_CSR_MANERR_Pos))
#define US_CSR_UNRE_Pos                       (10U)                                          /**< (US_CSR) SPI Underrun Error Position */
#define US_CSR_UNRE_Msk                       (0x1U << US_CSR_UNRE_Pos)                      /**< (US_CSR) SPI Underrun Error Mask */
#define US_CSR_UNRE(value)                    (US_CSR_UNRE_Msk & ((value) << US_CSR_UNRE_Pos))
#define US_CSR_NSSE_Pos                       (19U)                                          /**< (US_CSR) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Position */
#define US_CSR_NSSE_Msk                       (0x1U << US_CSR_NSSE_Pos)                      /**< (US_CSR) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Mask */
#define US_CSR_NSSE(value)                    (US_CSR_NSSE_Msk & ((value) << US_CSR_NSSE_Pos))
#define US_CSR_NSS_Pos                        (23U)                                          /**< (US_CSR) Image of NSS Line Position */
#define US_CSR_NSS_Msk                        (0x1U << US_CSR_NSS_Pos)                       /**< (US_CSR) Image of NSS Line Mask */
#define US_CSR_NSS(value)                     (US_CSR_NSS_Msk & ((value) << US_CSR_NSS_Pos))
#define US_CSR_LINBK_Pos                      (13U)                                          /**< (US_CSR) LIN Break Sent or LIN Break Received Position */
#define US_CSR_LINBK_Msk                      (0x1U << US_CSR_LINBK_Pos)                     /**< (US_CSR) LIN Break Sent or LIN Break Received Mask */
#define US_CSR_LINBK(value)                   (US_CSR_LINBK_Msk & ((value) << US_CSR_LINBK_Pos))
#define US_CSR_LINID_Pos                      (14U)                                          /**< (US_CSR) LIN Identifier Sent or LIN Identifier Received Position */
#define US_CSR_LINID_Msk                      (0x1U << US_CSR_LINID_Pos)                     /**< (US_CSR) LIN Identifier Sent or LIN Identifier Received Mask */
#define US_CSR_LINID(value)                   (US_CSR_LINID_Msk & ((value) << US_CSR_LINID_Pos))
#define US_CSR_LINTC_Pos                      (15U)                                          /**< (US_CSR) LIN Transfer Completed Position */
#define US_CSR_LINTC_Msk                      (0x1U << US_CSR_LINTC_Pos)                     /**< (US_CSR) LIN Transfer Completed Mask */
#define US_CSR_LINTC(value)                   (US_CSR_LINTC_Msk & ((value) << US_CSR_LINTC_Pos))
#define US_CSR_LINBLS_Pos                     (23U)                                          /**< (US_CSR) LIN Bus Line Status Position */
#define US_CSR_LINBLS_Msk                     (0x1U << US_CSR_LINBLS_Pos)                    /**< (US_CSR) LIN Bus Line Status Mask */
#define US_CSR_LINBLS(value)                  (US_CSR_LINBLS_Msk & ((value) << US_CSR_LINBLS_Pos))
#define US_CSR_LINBE_Pos                      (25U)                                          /**< (US_CSR) LIN Bus Error Position */
#define US_CSR_LINBE_Msk                      (0x1U << US_CSR_LINBE_Pos)                     /**< (US_CSR) LIN Bus Error Mask */
#define US_CSR_LINBE(value)                   (US_CSR_LINBE_Msk & ((value) << US_CSR_LINBE_Pos))
#define US_CSR_LINISFE_Pos                    (26U)                                          /**< (US_CSR) LIN Inconsistent Synch Field Error Position */
#define US_CSR_LINISFE_Msk                    (0x1U << US_CSR_LINISFE_Pos)                   /**< (US_CSR) LIN Inconsistent Synch Field Error Mask */
#define US_CSR_LINISFE(value)                 (US_CSR_LINISFE_Msk & ((value) << US_CSR_LINISFE_Pos))
#define US_CSR_LINIPE_Pos                     (27U)                                          /**< (US_CSR) LIN Identifier Parity Error Position */
#define US_CSR_LINIPE_Msk                     (0x1U << US_CSR_LINIPE_Pos)                    /**< (US_CSR) LIN Identifier Parity Error Mask */
#define US_CSR_LINIPE(value)                  (US_CSR_LINIPE_Msk & ((value) << US_CSR_LINIPE_Pos))
#define US_CSR_LINCE_Pos                      (28U)                                          /**< (US_CSR) LIN Checksum Error Position */
#define US_CSR_LINCE_Msk                      (0x1U << US_CSR_LINCE_Pos)                     /**< (US_CSR) LIN Checksum Error Mask */
#define US_CSR_LINCE(value)                   (US_CSR_LINCE_Msk & ((value) << US_CSR_LINCE_Pos))
#define US_CSR_LINSNRE_Pos                    (29U)                                          /**< (US_CSR) LIN Slave Not Responding Error Interrupt Mask Position */
#define US_CSR_LINSNRE_Msk                    (0x1U << US_CSR_LINSNRE_Pos)                   /**< (US_CSR) LIN Slave Not Responding Error Interrupt Mask Mask */
#define US_CSR_LINSNRE(value)                 (US_CSR_LINSNRE_Msk & ((value) << US_CSR_LINSNRE_Pos))
#define US_CSR_LINSTE_Pos                     (30U)                                          /**< (US_CSR) LIN Synch Tolerance Error Position */
#define US_CSR_LINSTE_Msk                     (0x1U << US_CSR_LINSTE_Pos)                    /**< (US_CSR) LIN Synch Tolerance Error Mask */
#define US_CSR_LINSTE(value)                  (US_CSR_LINSTE_Msk & ((value) << US_CSR_LINSTE_Pos))
#define US_CSR_LINHTE_Pos                     (31U)                                          /**< (US_CSR) LIN Header Timeout Error Position */
#define US_CSR_LINHTE_Msk                     (0x1U << US_CSR_LINHTE_Pos)                    /**< (US_CSR) LIN Header Timeout Error Mask */
#define US_CSR_LINHTE(value)                  (US_CSR_LINHTE_Msk & ((value) << US_CSR_LINHTE_Pos))
#define US_CSR_LSFE_Pos                       (6U)                                           /**< (US_CSR) LON Short Frame Error Position */
#define US_CSR_LSFE_Msk                       (0x1U << US_CSR_LSFE_Pos)                      /**< (US_CSR) LON Short Frame Error Mask */
#define US_CSR_LSFE(value)                    (US_CSR_LSFE_Msk & ((value) << US_CSR_LSFE_Pos))
#define US_CSR_LCRCE_Pos                      (7U)                                           /**< (US_CSR) LON CRC Error Position */
#define US_CSR_LCRCE_Msk                      (0x1U << US_CSR_LCRCE_Pos)                     /**< (US_CSR) LON CRC Error Mask */
#define US_CSR_LCRCE(value)                   (US_CSR_LCRCE_Msk & ((value) << US_CSR_LCRCE_Pos))
#define US_CSR_LTXD_Pos                       (24U)                                          /**< (US_CSR) LON Transmission End Flag Position */
#define US_CSR_LTXD_Msk                       (0x1U << US_CSR_LTXD_Pos)                      /**< (US_CSR) LON Transmission End Flag Mask */
#define US_CSR_LTXD(value)                    (US_CSR_LTXD_Msk & ((value) << US_CSR_LTXD_Pos))
#define US_CSR_LCOL_Pos                       (25U)                                          /**< (US_CSR) LON Collision Detected Flag Position */
#define US_CSR_LCOL_Msk                       (0x1U << US_CSR_LCOL_Pos)                      /**< (US_CSR) LON Collision Detected Flag Mask */
#define US_CSR_LCOL(value)                    (US_CSR_LCOL_Msk & ((value) << US_CSR_LCOL_Pos))
#define US_CSR_LFET_Pos                       (26U)                                          /**< (US_CSR) LON Frame Early Termination Position */
#define US_CSR_LFET_Msk                       (0x1U << US_CSR_LFET_Pos)                      /**< (US_CSR) LON Frame Early Termination Mask */
#define US_CSR_LFET(value)                    (US_CSR_LFET_Msk & ((value) << US_CSR_LFET_Pos))
#define US_CSR_LRXD_Pos                       (27U)                                          /**< (US_CSR) LON Reception End Flag Position */
#define US_CSR_LRXD_Msk                       (0x1U << US_CSR_LRXD_Pos)                      /**< (US_CSR) LON Reception End Flag Mask */
#define US_CSR_LRXD(value)                    (US_CSR_LRXD_Msk & ((value) << US_CSR_LRXD_Pos))
#define US_CSR_LBLOVFE_Pos                    (28U)                                          /**< (US_CSR) LON Backlog Overflow Error Position */
#define US_CSR_LBLOVFE_Msk                    (0x1U << US_CSR_LBLOVFE_Pos)                   /**< (US_CSR) LON Backlog Overflow Error Mask */
#define US_CSR_LBLOVFE(value)                 (US_CSR_LBLOVFE_Msk & ((value) << US_CSR_LBLOVFE_Pos))
#define US_CSR_Msk                            (0xFFFFE7E7UL)                                 /**< (US_CSR) Register Mask  */

/* -------- US_RHR : (USART Offset: 0x18) (R/  32) Receive Holding Register -------- */
#define US_RHR_RXCHR_Pos                      (0U)                                           /**< (US_RHR) Received Character Position */
#define US_RHR_RXCHR_Msk                      (0x1FFU << US_RHR_RXCHR_Pos)                   /**< (US_RHR) Received Character Mask */
#define US_RHR_RXCHR(value)                   (US_RHR_RXCHR_Msk & ((value) << US_RHR_RXCHR_Pos))
#define US_RHR_RXSYNH_Pos                     (15U)                                          /**< (US_RHR) Received Sync Position */
#define US_RHR_RXSYNH_Msk                     (0x1U << US_RHR_RXSYNH_Pos)                    /**< (US_RHR) Received Sync Mask */
#define US_RHR_RXSYNH(value)                  (US_RHR_RXSYNH_Msk & ((value) << US_RHR_RXSYNH_Pos))
#define US_RHR_Msk                            (0x000081FFUL)                                 /**< (US_RHR) Register Mask  */

/* -------- US_THR : (USART Offset: 0x1C) ( /W 32) Transmit Holding Register -------- */
#define US_THR_TXCHR_Pos                      (0U)                                           /**< (US_THR) Character to be Transmitted Position */
#define US_THR_TXCHR_Msk                      (0x1FFU << US_THR_TXCHR_Pos)                   /**< (US_THR) Character to be Transmitted Mask */
#define US_THR_TXCHR(value)                   (US_THR_TXCHR_Msk & ((value) << US_THR_TXCHR_Pos))
#define US_THR_TXSYNH_Pos                     (15U)                                          /**< (US_THR) Sync Field to be Transmitted Position */
#define US_THR_TXSYNH_Msk                     (0x1U << US_THR_TXSYNH_Pos)                    /**< (US_THR) Sync Field to be Transmitted Mask */
#define US_THR_TXSYNH(value)                  (US_THR_TXSYNH_Msk & ((value) << US_THR_TXSYNH_Pos))
#define US_THR_Msk                            (0x000081FFUL)                                 /**< (US_THR) Register Mask  */

/* -------- US_BRGR : (USART Offset: 0x20) (R/W 32) Baud Rate Generator Register -------- */
#define US_BRGR_CD_Pos                        (0U)                                           /**< (US_BRGR) Clock Divider Position */
#define US_BRGR_CD_Msk                        (0xFFFFU << US_BRGR_CD_Pos)                    /**< (US_BRGR) Clock Divider Mask */
#define US_BRGR_CD(value)                     (US_BRGR_CD_Msk & ((value) << US_BRGR_CD_Pos))
#define US_BRGR_FP_Pos                        (16U)                                          /**< (US_BRGR) Fractional Part Position */
#define US_BRGR_FP_Msk                        (0x7U << US_BRGR_FP_Pos)                       /**< (US_BRGR) Fractional Part Mask */
#define US_BRGR_FP(value)                     (US_BRGR_FP_Msk & ((value) << US_BRGR_FP_Pos))
#define US_BRGR_Msk                           (0x0007FFFFUL)                                 /**< (US_BRGR) Register Mask  */

/* -------- US_RTOR : (USART Offset: 0x24) (R/W 32) Receiver Timeout Register -------- */
#define US_RTOR_TO_Pos                        (0U)                                           /**< (US_RTOR) Timeout Value Position */
#define US_RTOR_TO_Msk                        (0x1FFFFU << US_RTOR_TO_Pos)                   /**< (US_RTOR) Timeout Value Mask */
#define US_RTOR_TO(value)                     (US_RTOR_TO_Msk & ((value) << US_RTOR_TO_Pos))
#define US_RTOR_Msk                           (0x0001FFFFUL)                                 /**< (US_RTOR) Register Mask  */

/* -------- US_TTGR : (USART Offset: 0x28) (R/W 32) Transmitter Timeguard Register -------- */
#define US_TTGR_TG_Pos                        (0U)                                           /**< (US_TTGR) Timeguard Value Position */
#define US_TTGR_TG_Msk                        (0xFFU << US_TTGR_TG_Pos)                      /**< (US_TTGR) Timeguard Value Mask */
#define US_TTGR_TG(value)                     (US_TTGR_TG_Msk & ((value) << US_TTGR_TG_Pos))
#define US_TTGR_PCYCLE_Pos                    (0U)                                           /**< (US_TTGR) LON PCYCLE Length Position */
#define US_TTGR_PCYCLE_Msk                    (0xFFFFFFU << US_TTGR_PCYCLE_Pos)              /**< (US_TTGR) LON PCYCLE Length Mask */
#define US_TTGR_PCYCLE(value)                 (US_TTGR_PCYCLE_Msk & ((value) << US_TTGR_PCYCLE_Pos))
#define US_TTGR_Msk                           (0x00FFFFFFUL)                                 /**< (US_TTGR) Register Mask  */

/* -------- US_FIDI : (USART Offset: 0x40) (R/W 32) FI DI Ratio Register -------- */
#define US_FIDI_FI_DI_RATIO_Pos               (0U)                                           /**< (US_FIDI) FI Over DI Ratio Value Position */
#define US_FIDI_FI_DI_RATIO_Msk               (0xFFFFU << US_FIDI_FI_DI_RATIO_Pos)           /**< (US_FIDI) FI Over DI Ratio Value Mask */
#define US_FIDI_FI_DI_RATIO(value)            (US_FIDI_FI_DI_RATIO_Msk & ((value) << US_FIDI_FI_DI_RATIO_Pos))
#define US_FIDI_BETA2_Pos                     (0U)                                           /**< (US_FIDI) LON BETA2 Length Position */
#define US_FIDI_BETA2_Msk                     (0xFFFFFFU << US_FIDI_BETA2_Pos)               /**< (US_FIDI) LON BETA2 Length Mask */
#define US_FIDI_BETA2(value)                  (US_FIDI_BETA2_Msk & ((value) << US_FIDI_BETA2_Pos))
#define US_FIDI_Msk                           (0x00FFFFFFUL)                                 /**< (US_FIDI) Register Mask  */

/* -------- US_NER : (USART Offset: 0x44) (R/  32) Number of Errors Register -------- */
#define US_NER_NB_ERRORS_Pos                  (0U)                                           /**< (US_NER) Number of Errors Position */
#define US_NER_NB_ERRORS_Msk                  (0xFFU << US_NER_NB_ERRORS_Pos)                /**< (US_NER) Number of Errors Mask */
#define US_NER_NB_ERRORS(value)               (US_NER_NB_ERRORS_Msk & ((value) << US_NER_NB_ERRORS_Pos))
#define US_NER_Msk                            (0x000000FFUL)                                 /**< (US_NER) Register Mask  */

/* -------- US_IF : (USART Offset: 0x4C) (R/W 32) IrDA Filter Register -------- */
#define US_IF_IRDA_FILTER_Pos                 (0U)                                           /**< (US_IF) IrDA Filter Position */
#define US_IF_IRDA_FILTER_Msk                 (0xFFU << US_IF_IRDA_FILTER_Pos)               /**< (US_IF) IrDA Filter Mask */
#define US_IF_IRDA_FILTER(value)              (US_IF_IRDA_FILTER_Msk & ((value) << US_IF_IRDA_FILTER_Pos))
#define US_IF_Msk                             (0x000000FFUL)                                 /**< (US_IF) Register Mask  */

/* -------- US_MAN : (USART Offset: 0x50) (R/W 32) Manchester Configuration Register -------- */
#define US_MAN_TX_PL_Pos                      (0U)                                           /**< (US_MAN) Transmitter Preamble Length Position */
#define US_MAN_TX_PL_Msk                      (0xFU << US_MAN_TX_PL_Pos)                     /**< (US_MAN) Transmitter Preamble Length Mask */
#define US_MAN_TX_PL(value)                   (US_MAN_TX_PL_Msk & ((value) << US_MAN_TX_PL_Pos))
#define US_MAN_TX_PP_Pos                      (8U)                                           /**< (US_MAN) Transmitter Preamble Pattern Position */
#define US_MAN_TX_PP_Msk                      (0x3U << US_MAN_TX_PP_Pos)                     /**< (US_MAN) Transmitter Preamble Pattern Mask */
#define US_MAN_TX_PP(value)                   (US_MAN_TX_PP_Msk & ((value) << US_MAN_TX_PP_Pos))
#define   US_MAN_TX_PP_ALL_ONE_Val            (0U)                                           /**< (US_MAN) The preamble is composed of '1's  */
#define   US_MAN_TX_PP_ALL_ZERO_Val           (1U)                                           /**< (US_MAN) The preamble is composed of '0's  */
#define   US_MAN_TX_PP_ZERO_ONE_Val           (2U)                                           /**< (US_MAN) The preamble is composed of '01's  */
#define   US_MAN_TX_PP_ONE_ZERO_Val           (3U)                                           /**< (US_MAN) The preamble is composed of '10's  */
#define US_MAN_TX_PP_ALL_ONE                  (US_MAN_TX_PP_ALL_ONE_Val << US_MAN_TX_PP_Pos) /**< (US_MAN) The preamble is composed of '1's Position  */
#define US_MAN_TX_PP_ALL_ZERO                 (US_MAN_TX_PP_ALL_ZERO_Val << US_MAN_TX_PP_Pos) /**< (US_MAN) The preamble is composed of '0's Position  */
#define US_MAN_TX_PP_ZERO_ONE                 (US_MAN_TX_PP_ZERO_ONE_Val << US_MAN_TX_PP_Pos) /**< (US_MAN) The preamble is composed of '01's Position  */
#define US_MAN_TX_PP_ONE_ZERO                 (US_MAN_TX_PP_ONE_ZERO_Val << US_MAN_TX_PP_Pos) /**< (US_MAN) The preamble is composed of '10's Position  */
#define US_MAN_TX_MPOL_Pos                    (12U)                                          /**< (US_MAN) Transmitter Manchester Polarity Position */
#define US_MAN_TX_MPOL_Msk                    (0x1U << US_MAN_TX_MPOL_Pos)                   /**< (US_MAN) Transmitter Manchester Polarity Mask */
#define US_MAN_TX_MPOL(value)                 (US_MAN_TX_MPOL_Msk & ((value) << US_MAN_TX_MPOL_Pos))
#define US_MAN_RX_PL_Pos                      (16U)                                          /**< (US_MAN) Receiver Preamble Length Position */
#define US_MAN_RX_PL_Msk                      (0xFU << US_MAN_RX_PL_Pos)                     /**< (US_MAN) Receiver Preamble Length Mask */
#define US_MAN_RX_PL(value)                   (US_MAN_RX_PL_Msk & ((value) << US_MAN_RX_PL_Pos))
#define US_MAN_RX_PP_Pos                      (24U)                                          /**< (US_MAN) Receiver Preamble Pattern detected Position */
#define US_MAN_RX_PP_Msk                      (0x3U << US_MAN_RX_PP_Pos)                     /**< (US_MAN) Receiver Preamble Pattern detected Mask */
#define US_MAN_RX_PP(value)                   (US_MAN_RX_PP_Msk & ((value) << US_MAN_RX_PP_Pos))
#define   US_MAN_RX_PP_ALL_ONE_Val            (0U)                                           /**< (US_MAN) The preamble is composed of '1's  */
#define   US_MAN_RX_PP_ALL_ZERO_Val           (1U)                                           /**< (US_MAN) The preamble is composed of '0's  */
#define   US_MAN_RX_PP_ZERO_ONE_Val           (2U)                                           /**< (US_MAN) The preamble is composed of '01's  */
#define   US_MAN_RX_PP_ONE_ZERO_Val           (3U)                                           /**< (US_MAN) The preamble is composed of '10's  */
#define US_MAN_RX_PP_ALL_ONE                  (US_MAN_RX_PP_ALL_ONE_Val << US_MAN_RX_PP_Pos) /**< (US_MAN) The preamble is composed of '1's Position  */
#define US_MAN_RX_PP_ALL_ZERO                 (US_MAN_RX_PP_ALL_ZERO_Val << US_MAN_RX_PP_Pos) /**< (US_MAN) The preamble is composed of '0's Position  */
#define US_MAN_RX_PP_ZERO_ONE                 (US_MAN_RX_PP_ZERO_ONE_Val << US_MAN_RX_PP_Pos) /**< (US_MAN) The preamble is composed of '01's Position  */
#define US_MAN_RX_PP_ONE_ZERO                 (US_MAN_RX_PP_ONE_ZERO_Val << US_MAN_RX_PP_Pos) /**< (US_MAN) The preamble is composed of '10's Position  */
#define US_MAN_RX_MPOL_Pos                    (28U)                                          /**< (US_MAN) Receiver Manchester Polarity Position */
#define US_MAN_RX_MPOL_Msk                    (0x1U << US_MAN_RX_MPOL_Pos)                   /**< (US_MAN) Receiver Manchester Polarity Mask */
#define US_MAN_RX_MPOL(value)                 (US_MAN_RX_MPOL_Msk & ((value) << US_MAN_RX_MPOL_Pos))
#define US_MAN_ONE_Pos                        (29U)                                          /**< (US_MAN) Must Be Set to 1 Position */
#define US_MAN_ONE_Msk                        (0x1U << US_MAN_ONE_Pos)                       /**< (US_MAN) Must Be Set to 1 Mask */
#define US_MAN_ONE(value)                     (US_MAN_ONE_Msk & ((value) << US_MAN_ONE_Pos))
#define US_MAN_DRIFT_Pos                      (30U)                                          /**< (US_MAN) Drift Compensation Position */
#define US_MAN_DRIFT_Msk                      (0x1U << US_MAN_DRIFT_Pos)                     /**< (US_MAN) Drift Compensation Mask */
#define US_MAN_DRIFT(value)                   (US_MAN_DRIFT_Msk & ((value) << US_MAN_DRIFT_Pos))
#define US_MAN_RXIDLEV_Pos                    (31U)                                          /**< (US_MAN) Receiver Idle Value Position */
#define US_MAN_RXIDLEV_Msk                    (0x1U << US_MAN_RXIDLEV_Pos)                   /**< (US_MAN) Receiver Idle Value Mask */
#define US_MAN_RXIDLEV(value)                 (US_MAN_RXIDLEV_Msk & ((value) << US_MAN_RXIDLEV_Pos))
#define US_MAN_Msk                            (0xF30F130FUL)                                 /**< (US_MAN) Register Mask  */

/* -------- US_LINMR : (USART Offset: 0x54) (R/W 32) LIN Mode Register -------- */
#define US_LINMR_NACT_Pos                     (0U)                                           /**< (US_LINMR) LIN Node Action Position */
#define US_LINMR_NACT_Msk                     (0x3U << US_LINMR_NACT_Pos)                    /**< (US_LINMR) LIN Node Action Mask */
#define US_LINMR_NACT(value)                  (US_LINMR_NACT_Msk & ((value) << US_LINMR_NACT_Pos))
#define   US_LINMR_NACT_PUBLISH_Val           (0U)                                           /**< (US_LINMR) The USART transmits the response.  */
#define   US_LINMR_NACT_SUBSCRIBE_Val         (1U)                                           /**< (US_LINMR) The USART receives the response.  */
#define   US_LINMR_NACT_IGNORE_Val            (2U)                                           /**< (US_LINMR) The USART does not transmit and does not receive the response.  */
#define US_LINMR_NACT_PUBLISH                 (US_LINMR_NACT_PUBLISH_Val << US_LINMR_NACT_Pos) /**< (US_LINMR) The USART transmits the response. Position  */
#define US_LINMR_NACT_SUBSCRIBE               (US_LINMR_NACT_SUBSCRIBE_Val << US_LINMR_NACT_Pos) /**< (US_LINMR) The USART receives the response. Position  */
#define US_LINMR_NACT_IGNORE                  (US_LINMR_NACT_IGNORE_Val << US_LINMR_NACT_Pos) /**< (US_LINMR) The USART does not transmit and does not receive the response. Position  */
#define US_LINMR_PARDIS_Pos                   (2U)                                           /**< (US_LINMR) Parity Disable Position */
#define US_LINMR_PARDIS_Msk                   (0x1U << US_LINMR_PARDIS_Pos)                  /**< (US_LINMR) Parity Disable Mask */
#define US_LINMR_PARDIS(value)                (US_LINMR_PARDIS_Msk & ((value) << US_LINMR_PARDIS_Pos))
#define US_LINMR_CHKDIS_Pos                   (3U)                                           /**< (US_LINMR) Checksum Disable Position */
#define US_LINMR_CHKDIS_Msk                   (0x1U << US_LINMR_CHKDIS_Pos)                  /**< (US_LINMR) Checksum Disable Mask */
#define US_LINMR_CHKDIS(value)                (US_LINMR_CHKDIS_Msk & ((value) << US_LINMR_CHKDIS_Pos))
#define US_LINMR_CHKTYP_Pos                   (4U)                                           /**< (US_LINMR) Checksum Type Position */
#define US_LINMR_CHKTYP_Msk                   (0x1U << US_LINMR_CHKTYP_Pos)                  /**< (US_LINMR) Checksum Type Mask */
#define US_LINMR_CHKTYP(value)                (US_LINMR_CHKTYP_Msk & ((value) << US_LINMR_CHKTYP_Pos))
#define US_LINMR_DLM_Pos                      (5U)                                           /**< (US_LINMR) Data Length Mode Position */
#define US_LINMR_DLM_Msk                      (0x1U << US_LINMR_DLM_Pos)                     /**< (US_LINMR) Data Length Mode Mask */
#define US_LINMR_DLM(value)                   (US_LINMR_DLM_Msk & ((value) << US_LINMR_DLM_Pos))
#define US_LINMR_FSDIS_Pos                    (6U)                                           /**< (US_LINMR) Frame Slot Mode Disable Position */
#define US_LINMR_FSDIS_Msk                    (0x1U << US_LINMR_FSDIS_Pos)                   /**< (US_LINMR) Frame Slot Mode Disable Mask */
#define US_LINMR_FSDIS(value)                 (US_LINMR_FSDIS_Msk & ((value) << US_LINMR_FSDIS_Pos))
#define US_LINMR_WKUPTYP_Pos                  (7U)                                           /**< (US_LINMR) Wakeup Signal Type Position */
#define US_LINMR_WKUPTYP_Msk                  (0x1U << US_LINMR_WKUPTYP_Pos)                 /**< (US_LINMR) Wakeup Signal Type Mask */
#define US_LINMR_WKUPTYP(value)               (US_LINMR_WKUPTYP_Msk & ((value) << US_LINMR_WKUPTYP_Pos))
#define US_LINMR_DLC_Pos                      (8U)                                           /**< (US_LINMR) Data Length Control Position */
#define US_LINMR_DLC_Msk                      (0xFFU << US_LINMR_DLC_Pos)                    /**< (US_LINMR) Data Length Control Mask */
#define US_LINMR_DLC(value)                   (US_LINMR_DLC_Msk & ((value) << US_LINMR_DLC_Pos))
#define US_LINMR_PDCM_Pos                     (16U)                                          /**< (US_LINMR) DMAC Mode Position */
#define US_LINMR_PDCM_Msk                     (0x1U << US_LINMR_PDCM_Pos)                    /**< (US_LINMR) DMAC Mode Mask */
#define US_LINMR_PDCM(value)                  (US_LINMR_PDCM_Msk & ((value) << US_LINMR_PDCM_Pos))
#define US_LINMR_SYNCDIS_Pos                  (17U)                                          /**< (US_LINMR) Synchronization Disable Position */
#define US_LINMR_SYNCDIS_Msk                  (0x1U << US_LINMR_SYNCDIS_Pos)                 /**< (US_LINMR) Synchronization Disable Mask */
#define US_LINMR_SYNCDIS(value)               (US_LINMR_SYNCDIS_Msk & ((value) << US_LINMR_SYNCDIS_Pos))
#define US_LINMR_Msk                          (0x0003FFFFUL)                                 /**< (US_LINMR) Register Mask  */

/* -------- US_LINIR : (USART Offset: 0x58) (R/W 32) LIN Identifier Register -------- */
#define US_LINIR_IDCHR_Pos                    (0U)                                           /**< (US_LINIR) Identifier Character Position */
#define US_LINIR_IDCHR_Msk                    (0xFFU << US_LINIR_IDCHR_Pos)                  /**< (US_LINIR) Identifier Character Mask */
#define US_LINIR_IDCHR(value)                 (US_LINIR_IDCHR_Msk & ((value) << US_LINIR_IDCHR_Pos))
#define US_LINIR_Msk                          (0x000000FFUL)                                 /**< (US_LINIR) Register Mask  */

/* -------- US_LINBRR : (USART Offset: 0x5C) (R/  32) LIN Baud Rate Register -------- */
#define US_LINBRR_LINCD_Pos                   (0U)                                           /**< (US_LINBRR) Clock Divider after Synchronization Position */
#define US_LINBRR_LINCD_Msk                   (0xFFFFU << US_LINBRR_LINCD_Pos)               /**< (US_LINBRR) Clock Divider after Synchronization Mask */
#define US_LINBRR_LINCD(value)                (US_LINBRR_LINCD_Msk & ((value) << US_LINBRR_LINCD_Pos))
#define US_LINBRR_LINFP_Pos                   (16U)                                          /**< (US_LINBRR) Fractional Part after Synchronization Position */
#define US_LINBRR_LINFP_Msk                   (0x7U << US_LINBRR_LINFP_Pos)                  /**< (US_LINBRR) Fractional Part after Synchronization Mask */
#define US_LINBRR_LINFP(value)                (US_LINBRR_LINFP_Msk & ((value) << US_LINBRR_LINFP_Pos))
#define US_LINBRR_Msk                         (0x0007FFFFUL)                                 /**< (US_LINBRR) Register Mask  */

/* -------- US_LONMR : (USART Offset: 0x60) (R/W 32) LON Mode Register -------- */
#define US_LONMR_COMMT_Pos                    (0U)                                           /**< (US_LONMR) LON comm_type Parameter Value Position */
#define US_LONMR_COMMT_Msk                    (0x1U << US_LONMR_COMMT_Pos)                   /**< (US_LONMR) LON comm_type Parameter Value Mask */
#define US_LONMR_COMMT(value)                 (US_LONMR_COMMT_Msk & ((value) << US_LONMR_COMMT_Pos))
#define US_LONMR_COLDET_Pos                   (1U)                                           /**< (US_LONMR) LON Collision Detection Feature Position */
#define US_LONMR_COLDET_Msk                   (0x1U << US_LONMR_COLDET_Pos)                  /**< (US_LONMR) LON Collision Detection Feature Mask */
#define US_LONMR_COLDET(value)                (US_LONMR_COLDET_Msk & ((value) << US_LONMR_COLDET_Pos))
#define US_LONMR_TCOL_Pos                     (2U)                                           /**< (US_LONMR) Terminate Frame upon Collision Notification Position */
#define US_LONMR_TCOL_Msk                     (0x1U << US_LONMR_TCOL_Pos)                    /**< (US_LONMR) Terminate Frame upon Collision Notification Mask */
#define US_LONMR_TCOL(value)                  (US_LONMR_TCOL_Msk & ((value) << US_LONMR_TCOL_Pos))
#define US_LONMR_CDTAIL_Pos                   (3U)                                           /**< (US_LONMR) LON Collision Detection on Frame Tail Position */
#define US_LONMR_CDTAIL_Msk                   (0x1U << US_LONMR_CDTAIL_Pos)                  /**< (US_LONMR) LON Collision Detection on Frame Tail Mask */
#define US_LONMR_CDTAIL(value)                (US_LONMR_CDTAIL_Msk & ((value) << US_LONMR_CDTAIL_Pos))
#define US_LONMR_DMAM_Pos                     (4U)                                           /**< (US_LONMR) LON DMA Mode Position */
#define US_LONMR_DMAM_Msk                     (0x1U << US_LONMR_DMAM_Pos)                    /**< (US_LONMR) LON DMA Mode Mask */
#define US_LONMR_DMAM(value)                  (US_LONMR_DMAM_Msk & ((value) << US_LONMR_DMAM_Pos))
#define US_LONMR_LCDS_Pos                     (5U)                                           /**< (US_LONMR) LON Collision Detection Source Position */
#define US_LONMR_LCDS_Msk                     (0x1U << US_LONMR_LCDS_Pos)                    /**< (US_LONMR) LON Collision Detection Source Mask */
#define US_LONMR_LCDS(value)                  (US_LONMR_LCDS_Msk & ((value) << US_LONMR_LCDS_Pos))
#define US_LONMR_EOFS_Pos                     (16U)                                          /**< (US_LONMR) End of Frame Condition Size Position */
#define US_LONMR_EOFS_Msk                     (0xFFU << US_LONMR_EOFS_Pos)                   /**< (US_LONMR) End of Frame Condition Size Mask */
#define US_LONMR_EOFS(value)                  (US_LONMR_EOFS_Msk & ((value) << US_LONMR_EOFS_Pos))
#define US_LONMR_Msk                          (0x00FF003FUL)                                 /**< (US_LONMR) Register Mask  */

/* -------- US_LONPR : (USART Offset: 0x64) (R/W 32) LON Preamble Register -------- */
#define US_LONPR_LONPL_Pos                    (0U)                                           /**< (US_LONPR) LON Preamble Length Position */
#define US_LONPR_LONPL_Msk                    (0x3FFFU << US_LONPR_LONPL_Pos)                /**< (US_LONPR) LON Preamble Length Mask */
#define US_LONPR_LONPL(value)                 (US_LONPR_LONPL_Msk & ((value) << US_LONPR_LONPL_Pos))
#define US_LONPR_Msk                          (0x00003FFFUL)                                 /**< (US_LONPR) Register Mask  */

/* -------- US_LONDL : (USART Offset: 0x68) (R/W 32) LON Data Length Register -------- */
#define US_LONDL_LONDL_Pos                    (0U)                                           /**< (US_LONDL) LON Data Length Position */
#define US_LONDL_LONDL_Msk                    (0xFFU << US_LONDL_LONDL_Pos)                  /**< (US_LONDL) LON Data Length Mask */
#define US_LONDL_LONDL(value)                 (US_LONDL_LONDL_Msk & ((value) << US_LONDL_LONDL_Pos))
#define US_LONDL_Msk                          (0x000000FFUL)                                 /**< (US_LONDL) Register Mask  */

/* -------- US_LONL2HDR : (USART Offset: 0x6C) (R/W 32) LON L2HDR Register -------- */
#define US_LONL2HDR_BLI_Pos                   (0U)                                           /**< (US_LONL2HDR) LON Backlog Increment Position */
#define US_LONL2HDR_BLI_Msk                   (0x3FU << US_LONL2HDR_BLI_Pos)                 /**< (US_LONL2HDR) LON Backlog Increment Mask */
#define US_LONL2HDR_BLI(value)                (US_LONL2HDR_BLI_Msk & ((value) << US_LONL2HDR_BLI_Pos))
#define US_LONL2HDR_ALTP_Pos                  (6U)                                           /**< (US_LONL2HDR) LON Alternate Path Bit Position */
#define US_LONL2HDR_ALTP_Msk                  (0x1U << US_LONL2HDR_ALTP_Pos)                 /**< (US_LONL2HDR) LON Alternate Path Bit Mask */
#define US_LONL2HDR_ALTP(value)               (US_LONL2HDR_ALTP_Msk & ((value) << US_LONL2HDR_ALTP_Pos))
#define US_LONL2HDR_PB_Pos                    (7U)                                           /**< (US_LONL2HDR) LON Priority Bit Position */
#define US_LONL2HDR_PB_Msk                    (0x1U << US_LONL2HDR_PB_Pos)                   /**< (US_LONL2HDR) LON Priority Bit Mask */
#define US_LONL2HDR_PB(value)                 (US_LONL2HDR_PB_Msk & ((value) << US_LONL2HDR_PB_Pos))
#define US_LONL2HDR_Msk                       (0x000000FFUL)                                 /**< (US_LONL2HDR) Register Mask  */

/* -------- US_LONBL : (USART Offset: 0x70) (R/  32) LON Backlog Register -------- */
#define US_LONBL_LONBL_Pos                    (0U)                                           /**< (US_LONBL) LON Node Backlog Value Position */
#define US_LONBL_LONBL_Msk                    (0x3FU << US_LONBL_LONBL_Pos)                  /**< (US_LONBL) LON Node Backlog Value Mask */
#define US_LONBL_LONBL(value)                 (US_LONBL_LONBL_Msk & ((value) << US_LONBL_LONBL_Pos))
#define US_LONBL_Msk                          (0x0000003FUL)                                 /**< (US_LONBL) Register Mask  */

/* -------- US_LONB1TX : (USART Offset: 0x74) (R/W 32) LON Beta1 Tx Register -------- */
#define US_LONB1TX_BETA1TX_Pos                (0U)                                           /**< (US_LONB1TX) LON Beta1 Length after Transmission Position */
#define US_LONB1TX_BETA1TX_Msk                (0xFFFFFFU << US_LONB1TX_BETA1TX_Pos)          /**< (US_LONB1TX) LON Beta1 Length after Transmission Mask */
#define US_LONB1TX_BETA1TX(value)             (US_LONB1TX_BETA1TX_Msk & ((value) << US_LONB1TX_BETA1TX_Pos))
#define US_LONB1TX_Msk                        (0x00FFFFFFUL)                                 /**< (US_LONB1TX) Register Mask  */

/* -------- US_LONB1RX : (USART Offset: 0x78) (R/W 32) LON Beta1 Rx Register -------- */
#define US_LONB1RX_BETA1RX_Pos                (0U)                                           /**< (US_LONB1RX) LON Beta1 Length after Reception Position */
#define US_LONB1RX_BETA1RX_Msk                (0xFFFFFFU << US_LONB1RX_BETA1RX_Pos)          /**< (US_LONB1RX) LON Beta1 Length after Reception Mask */
#define US_LONB1RX_BETA1RX(value)             (US_LONB1RX_BETA1RX_Msk & ((value) << US_LONB1RX_BETA1RX_Pos))
#define US_LONB1RX_Msk                        (0x00FFFFFFUL)                                 /**< (US_LONB1RX) Register Mask  */

/* -------- US_LONPRIO : (USART Offset: 0x7C) (R/W 32) LON Priority Register -------- */
#define US_LONPRIO_PSNB_Pos                   (0U)                                           /**< (US_LONPRIO) LON Priority Slot Number Position */
#define US_LONPRIO_PSNB_Msk                   (0x7FU << US_LONPRIO_PSNB_Pos)                 /**< (US_LONPRIO) LON Priority Slot Number Mask */
#define US_LONPRIO_PSNB(value)                (US_LONPRIO_PSNB_Msk & ((value) << US_LONPRIO_PSNB_Pos))
#define US_LONPRIO_NPS_Pos                    (8U)                                           /**< (US_LONPRIO) LON Node Priority Slot Position */
#define US_LONPRIO_NPS_Msk                    (0x7FU << US_LONPRIO_NPS_Pos)                  /**< (US_LONPRIO) LON Node Priority Slot Mask */
#define US_LONPRIO_NPS(value)                 (US_LONPRIO_NPS_Msk & ((value) << US_LONPRIO_NPS_Pos))
#define US_LONPRIO_Msk                        (0x00007F7FUL)                                 /**< (US_LONPRIO) Register Mask  */

/* -------- US_IDTTX : (USART Offset: 0x80) (R/W 32) LON IDT Tx Register -------- */
#define US_IDTTX_IDTTX_Pos                    (0U)                                           /**< (US_IDTTX) LON Indeterminate Time after Transmission (comm_type = 1 mode only) Position */
#define US_IDTTX_IDTTX_Msk                    (0xFFFFFFU << US_IDTTX_IDTTX_Pos)              /**< (US_IDTTX) LON Indeterminate Time after Transmission (comm_type = 1 mode only) Mask */
#define US_IDTTX_IDTTX(value)                 (US_IDTTX_IDTTX_Msk & ((value) << US_IDTTX_IDTTX_Pos))
#define US_IDTTX_Msk                          (0x00FFFFFFUL)                                 /**< (US_IDTTX) Register Mask  */

/* -------- US_IDTRX : (USART Offset: 0x84) (R/W 32) LON IDT Rx Register -------- */
#define US_IDTRX_IDTRX_Pos                    (0U)                                           /**< (US_IDTRX) LON Indeterminate Time after Reception (comm_type = 1 mode only) Position */
#define US_IDTRX_IDTRX_Msk                    (0xFFFFFFU << US_IDTRX_IDTRX_Pos)              /**< (US_IDTRX) LON Indeterminate Time after Reception (comm_type = 1 mode only) Mask */
#define US_IDTRX_IDTRX(value)                 (US_IDTRX_IDTRX_Msk & ((value) << US_IDTRX_IDTRX_Pos))
#define US_IDTRX_Msk                          (0x00FFFFFFUL)                                 /**< (US_IDTRX) Register Mask  */

/* -------- US_ICDIFF : (USART Offset: 0x88) (R/W 32) IC DIFF Register -------- */
#define US_ICDIFF_ICDIFF_Pos                  (0U)                                           /**< (US_ICDIFF) IC Differentiator Number Position */
#define US_ICDIFF_ICDIFF_Msk                  (0xFU << US_ICDIFF_ICDIFF_Pos)                 /**< (US_ICDIFF) IC Differentiator Number Mask */
#define US_ICDIFF_ICDIFF(value)               (US_ICDIFF_ICDIFF_Msk & ((value) << US_ICDIFF_ICDIFF_Pos))
#define US_ICDIFF_Msk                         (0x0000000FUL)                                 /**< (US_ICDIFF) Register Mask  */

/* -------- US_WPMR : (USART Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define US_WPMR_WPEN_Pos                      (0U)                                           /**< (US_WPMR) Write Protection Enable Position */
#define US_WPMR_WPEN_Msk                      (0x1U << US_WPMR_WPEN_Pos)                     /**< (US_WPMR) Write Protection Enable Mask */
#define US_WPMR_WPEN(value)                   (US_WPMR_WPEN_Msk & ((value) << US_WPMR_WPEN_Pos))
#define US_WPMR_WPKEY_Pos                     (8U)                                           /**< (US_WPMR) Write Protection Key Position */
#define US_WPMR_WPKEY_Msk                     (0xFFFFFFU << US_WPMR_WPKEY_Pos)               /**< (US_WPMR) Write Protection Key Mask */
#define US_WPMR_WPKEY(value)                  (US_WPMR_WPKEY_Msk & ((value) << US_WPMR_WPKEY_Pos))
#define   US_WPMR_WPKEY_PASSWD_Val            (5591873U)                                     /**< (US_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0.  */
#define US_WPMR_WPKEY_PASSWD                  (US_WPMR_WPKEY_PASSWD_Val << US_WPMR_WPKEY_Pos) /**< (US_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0. Position  */
#define US_WPMR_Msk                           (0xFFFFFF01UL)                                 /**< (US_WPMR) Register Mask  */

/* -------- US_WPSR : (USART Offset: 0xE8) (R/  32) Write Protection Status Register -------- */
#define US_WPSR_WPVS_Pos                      (0U)                                           /**< (US_WPSR) Write Protection Violation Status Position */
#define US_WPSR_WPVS_Msk                      (0x1U << US_WPSR_WPVS_Pos)                     /**< (US_WPSR) Write Protection Violation Status Mask */
#define US_WPSR_WPVS(value)                   (US_WPSR_WPVS_Msk & ((value) << US_WPSR_WPVS_Pos))
#define US_WPSR_WPVSRC_Pos                    (8U)                                           /**< (US_WPSR) Write Protection Violation Source Position */
#define US_WPSR_WPVSRC_Msk                    (0xFFFFU << US_WPSR_WPVSRC_Pos)                /**< (US_WPSR) Write Protection Violation Source Mask */
#define US_WPSR_WPVSRC(value)                 (US_WPSR_WPVSRC_Msk & ((value) << US_WPSR_WPVSRC_Pos))
#define US_WPSR_Msk                           (0x00FFFF01UL)                                 /**< (US_WPSR) Register Mask  */

/** \brief USART register offsets definitions */
#define US_CR_OFFSET                   (0x00)         /**< (US_CR) Control Register Offset */
#define US_MR_OFFSET                   (0x04)         /**< (US_MR) Mode Register Offset */
#define US_IER_OFFSET                  (0x08)         /**< (US_IER) Interrupt Enable Register Offset */
#define US_IDR_OFFSET                  (0x0C)         /**< (US_IDR) Interrupt Disable Register Offset */
#define US_IMR_OFFSET                  (0x10)         /**< (US_IMR) Interrupt Mask Register Offset */
#define US_CSR_OFFSET                  (0x14)         /**< (US_CSR) Channel Status Register Offset */
#define US_RHR_OFFSET                  (0x18)         /**< (US_RHR) Receive Holding Register Offset */
#define US_THR_OFFSET                  (0x1C)         /**< (US_THR) Transmit Holding Register Offset */
#define US_BRGR_OFFSET                 (0x20)         /**< (US_BRGR) Baud Rate Generator Register Offset */
#define US_RTOR_OFFSET                 (0x24)         /**< (US_RTOR) Receiver Timeout Register Offset */
#define US_TTGR_OFFSET                 (0x28)         /**< (US_TTGR) Transmitter Timeguard Register Offset */
#define US_FIDI_OFFSET                 (0x40)         /**< (US_FIDI) FI DI Ratio Register Offset */
#define US_NER_OFFSET                  (0x44)         /**< (US_NER) Number of Errors Register Offset */
#define US_IF_OFFSET                   (0x4C)         /**< (US_IF) IrDA Filter Register Offset */
#define US_MAN_OFFSET                  (0x50)         /**< (US_MAN) Manchester Configuration Register Offset */
#define US_LINMR_OFFSET                (0x54)         /**< (US_LINMR) LIN Mode Register Offset */
#define US_LINIR_OFFSET                (0x58)         /**< (US_LINIR) LIN Identifier Register Offset */
#define US_LINBRR_OFFSET               (0x5C)         /**< (US_LINBRR) LIN Baud Rate Register Offset */
#define US_LONMR_OFFSET                (0x60)         /**< (US_LONMR) LON Mode Register Offset */
#define US_LONPR_OFFSET                (0x64)         /**< (US_LONPR) LON Preamble Register Offset */
#define US_LONDL_OFFSET                (0x68)         /**< (US_LONDL) LON Data Length Register Offset */
#define US_LONL2HDR_OFFSET             (0x6C)         /**< (US_LONL2HDR) LON L2HDR Register Offset */
#define US_LONBL_OFFSET                (0x70)         /**< (US_LONBL) LON Backlog Register Offset */
#define US_LONB1TX_OFFSET              (0x74)         /**< (US_LONB1TX) LON Beta1 Tx Register Offset */
#define US_LONB1RX_OFFSET              (0x78)         /**< (US_LONB1RX) LON Beta1 Rx Register Offset */
#define US_LONPRIO_OFFSET              (0x7C)         /**< (US_LONPRIO) LON Priority Register Offset */
#define US_IDTTX_OFFSET                (0x80)         /**< (US_IDTTX) LON IDT Tx Register Offset */
#define US_IDTRX_OFFSET                (0x84)         /**< (US_IDTRX) LON IDT Rx Register Offset */
#define US_ICDIFF_OFFSET               (0x88)         /**< (US_ICDIFF) IC DIFF Register Offset */
#define US_WPMR_OFFSET                 (0xE4)         /**< (US_WPMR) Write Protection Mode Register Offset */
#define US_WPSR_OFFSET                 (0xE8)         /**< (US_WPSR) Write Protection Status Register Offset */

/** \brief USART register API structure */
typedef struct
{
  __O   uint32_t                       US_CR;           /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       US_MR;           /**< Offset: 0x04 (R/W  32) Mode Register */
  __O   uint32_t                       US_IER;          /**< Offset: 0x08 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       US_IDR;          /**< Offset: 0x0c ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       US_IMR;          /**< Offset: 0x10 (R/   32) Interrupt Mask Register */
  __I   uint32_t                       US_CSR;          /**< Offset: 0x14 (R/   32) Channel Status Register */
  __I   uint32_t                       US_RHR;          /**< Offset: 0x18 (R/   32) Receive Holding Register */
  __O   uint32_t                       US_THR;          /**< Offset: 0x1c ( /W  32) Transmit Holding Register */
  __IO  uint32_t                       US_BRGR;         /**< Offset: 0x20 (R/W  32) Baud Rate Generator Register */
  __IO  uint32_t                       US_RTOR;         /**< Offset: 0x24 (R/W  32) Receiver Timeout Register */
  __IO  uint32_t                       US_TTGR;         /**< Offset: 0x28 (R/W  32) Transmitter Timeguard Register */
  __I   uint8_t                        Reserved1[0x14];
  __IO  uint32_t                       US_FIDI;         /**< Offset: 0x40 (R/W  32) FI DI Ratio Register */
  __I   uint32_t                       US_NER;          /**< Offset: 0x44 (R/   32) Number of Errors Register */
  __I   uint8_t                        Reserved2[0x04];
  __IO  uint32_t                       US_IF;           /**< Offset: 0x4c (R/W  32) IrDA Filter Register */
  __IO  uint32_t                       US_MAN;          /**< Offset: 0x50 (R/W  32) Manchester Configuration Register */
  __IO  uint32_t                       US_LINMR;        /**< Offset: 0x54 (R/W  32) LIN Mode Register */
  __IO  uint32_t                       US_LINIR;        /**< Offset: 0x58 (R/W  32) LIN Identifier Register */
  __I   uint32_t                       US_LINBRR;       /**< Offset: 0x5c (R/   32) LIN Baud Rate Register */
  __IO  uint32_t                       US_LONMR;        /**< Offset: 0x60 (R/W  32) LON Mode Register */
  __IO  uint32_t                       US_LONPR;        /**< Offset: 0x64 (R/W  32) LON Preamble Register */
  __IO  uint32_t                       US_LONDL;        /**< Offset: 0x68 (R/W  32) LON Data Length Register */
  __IO  uint32_t                       US_LONL2HDR;     /**< Offset: 0x6c (R/W  32) LON L2HDR Register */
  __I   uint32_t                       US_LONBL;        /**< Offset: 0x70 (R/   32) LON Backlog Register */
  __IO  uint32_t                       US_LONB1TX;      /**< Offset: 0x74 (R/W  32) LON Beta1 Tx Register */
  __IO  uint32_t                       US_LONB1RX;      /**< Offset: 0x78 (R/W  32) LON Beta1 Rx Register */
  __IO  uint32_t                       US_LONPRIO;      /**< Offset: 0x7c (R/W  32) LON Priority Register */
  __IO  uint32_t                       US_IDTTX;        /**< Offset: 0x80 (R/W  32) LON IDT Tx Register */
  __IO  uint32_t                       US_IDTRX;        /**< Offset: 0x84 (R/W  32) LON IDT Rx Register */
  __IO  uint32_t                       US_ICDIFF;       /**< Offset: 0x88 (R/W  32) IC DIFF Register */
  __I   uint8_t                        Reserved3[0x58];
  __IO  uint32_t                       US_WPMR;         /**< Offset: 0xe4 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       US_WPSR;         /**< Offset: 0xe8 (R/   32) Write Protection Status Register */
} usart_registers_t;
/** @}  end of Universal Synchronous Asynchronous Receiver Transmitter */

#endif /* SAME_SAME70_USART_MODULE_H */

