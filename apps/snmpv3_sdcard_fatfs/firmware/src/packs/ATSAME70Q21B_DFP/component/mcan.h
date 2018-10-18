/**
 * \brief Header file for SAME/SAME70 MCAN
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
#ifndef SAME_SAME70_MCAN_MODULE_H
#define SAME_SAME70_MCAN_MODULE_H

/** \addtogroup SAME_SAME70 Controller Area Network
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_MCAN                                  */
/* ========================================================================== */

/* -------- MCAN_CREL : (MCAN Offset: 0x00) (R/  32) Core Release Register -------- */
#define MCAN_CREL_DAY_Pos                     (0U)                                           /**< (MCAN_CREL) Timestamp Day Position */
#define MCAN_CREL_DAY_Msk                     (0xFFU << MCAN_CREL_DAY_Pos)                   /**< (MCAN_CREL) Timestamp Day Mask */
#define MCAN_CREL_DAY(value)                  (MCAN_CREL_DAY_Msk & ((value) << MCAN_CREL_DAY_Pos))
#define MCAN_CREL_MON_Pos                     (8U)                                           /**< (MCAN_CREL) Timestamp Month Position */
#define MCAN_CREL_MON_Msk                     (0xFFU << MCAN_CREL_MON_Pos)                   /**< (MCAN_CREL) Timestamp Month Mask */
#define MCAN_CREL_MON(value)                  (MCAN_CREL_MON_Msk & ((value) << MCAN_CREL_MON_Pos))
#define MCAN_CREL_YEAR_Pos                    (16U)                                          /**< (MCAN_CREL) Timestamp Year Position */
#define MCAN_CREL_YEAR_Msk                    (0xFU << MCAN_CREL_YEAR_Pos)                   /**< (MCAN_CREL) Timestamp Year Mask */
#define MCAN_CREL_YEAR(value)                 (MCAN_CREL_YEAR_Msk & ((value) << MCAN_CREL_YEAR_Pos))
#define MCAN_CREL_SUBSTEP_Pos                 (20U)                                          /**< (MCAN_CREL) Sub-step of Core Release Position */
#define MCAN_CREL_SUBSTEP_Msk                 (0xFU << MCAN_CREL_SUBSTEP_Pos)                /**< (MCAN_CREL) Sub-step of Core Release Mask */
#define MCAN_CREL_SUBSTEP(value)              (MCAN_CREL_SUBSTEP_Msk & ((value) << MCAN_CREL_SUBSTEP_Pos))
#define MCAN_CREL_STEP_Pos                    (24U)                                          /**< (MCAN_CREL) Step of Core Release Position */
#define MCAN_CREL_STEP_Msk                    (0xFU << MCAN_CREL_STEP_Pos)                   /**< (MCAN_CREL) Step of Core Release Mask */
#define MCAN_CREL_STEP(value)                 (MCAN_CREL_STEP_Msk & ((value) << MCAN_CREL_STEP_Pos))
#define MCAN_CREL_REL_Pos                     (28U)                                          /**< (MCAN_CREL) Core Release Position */
#define MCAN_CREL_REL_Msk                     (0xFU << MCAN_CREL_REL_Pos)                    /**< (MCAN_CREL) Core Release Mask */
#define MCAN_CREL_REL(value)                  (MCAN_CREL_REL_Msk & ((value) << MCAN_CREL_REL_Pos))
#define MCAN_CREL_Msk                         (0xFFFFFFFFUL)                                 /**< (MCAN_CREL) Register Mask  */

/* -------- MCAN_ENDN : (MCAN Offset: 0x04) (R/  32) Endian Register -------- */
#define MCAN_ENDN_ETV_Pos                     (0U)                                           /**< (MCAN_ENDN) Endianness Test Value Position */
#define MCAN_ENDN_ETV_Msk                     (0xFFFFFFFFU << MCAN_ENDN_ETV_Pos)             /**< (MCAN_ENDN) Endianness Test Value Mask */
#define MCAN_ENDN_ETV(value)                  (MCAN_ENDN_ETV_Msk & ((value) << MCAN_ENDN_ETV_Pos))
#define MCAN_ENDN_Msk                         (0xFFFFFFFFUL)                                 /**< (MCAN_ENDN) Register Mask  */

/* -------- MCAN_CUST : (MCAN Offset: 0x08) (R/W 32) Customer Register -------- */
#define MCAN_CUST_CSV_Pos                     (0U)                                           /**< (MCAN_CUST) Customer-specific Value Position */
#define MCAN_CUST_CSV_Msk                     (0xFFFFFFFFU << MCAN_CUST_CSV_Pos)             /**< (MCAN_CUST) Customer-specific Value Mask */
#define MCAN_CUST_CSV(value)                  (MCAN_CUST_CSV_Msk & ((value) << MCAN_CUST_CSV_Pos))
#define MCAN_CUST_Msk                         (0xFFFFFFFFUL)                                 /**< (MCAN_CUST) Register Mask  */

/* -------- MCAN_DBTP : (MCAN Offset: 0x0C) (R/W 32) Data Bit Timing and Prescaler Register -------- */
#define MCAN_DBTP_DSJW_Pos                    (0U)                                           /**< (MCAN_DBTP) Data (Re) Synchronization Jump Width Position */
#define MCAN_DBTP_DSJW_Msk                    (0x7U << MCAN_DBTP_DSJW_Pos)                   /**< (MCAN_DBTP) Data (Re) Synchronization Jump Width Mask */
#define MCAN_DBTP_DSJW(value)                 (MCAN_DBTP_DSJW_Msk & ((value) << MCAN_DBTP_DSJW_Pos))
#define MCAN_DBTP_DTSEG2_Pos                  (4U)                                           /**< (MCAN_DBTP) Data Time Segment After Sample Point Position */
#define MCAN_DBTP_DTSEG2_Msk                  (0xFU << MCAN_DBTP_DTSEG2_Pos)                 /**< (MCAN_DBTP) Data Time Segment After Sample Point Mask */
#define MCAN_DBTP_DTSEG2(value)               (MCAN_DBTP_DTSEG2_Msk & ((value) << MCAN_DBTP_DTSEG2_Pos))
#define MCAN_DBTP_DTSEG1_Pos                  (8U)                                           /**< (MCAN_DBTP) Data Time Segment Before Sample Point Position */
#define MCAN_DBTP_DTSEG1_Msk                  (0x1FU << MCAN_DBTP_DTSEG1_Pos)                /**< (MCAN_DBTP) Data Time Segment Before Sample Point Mask */
#define MCAN_DBTP_DTSEG1(value)               (MCAN_DBTP_DTSEG1_Msk & ((value) << MCAN_DBTP_DTSEG1_Pos))
#define MCAN_DBTP_DBRP_Pos                    (16U)                                          /**< (MCAN_DBTP) Data Bit Rate Prescaler Position */
#define MCAN_DBTP_DBRP_Msk                    (0x1FU << MCAN_DBTP_DBRP_Pos)                  /**< (MCAN_DBTP) Data Bit Rate Prescaler Mask */
#define MCAN_DBTP_DBRP(value)                 (MCAN_DBTP_DBRP_Msk & ((value) << MCAN_DBTP_DBRP_Pos))
#define MCAN_DBTP_TDC_Pos                     (23U)                                          /**< (MCAN_DBTP) Transmitter Delay Compensation Position */
#define MCAN_DBTP_TDC_Msk                     (0x1U << MCAN_DBTP_TDC_Pos)                    /**< (MCAN_DBTP) Transmitter Delay Compensation Mask */
#define MCAN_DBTP_TDC(value)                  (MCAN_DBTP_TDC_Msk & ((value) << MCAN_DBTP_TDC_Pos))
#define   MCAN_DBTP_TDC_DISABLED_Val          (0U)                                           /**< (MCAN_DBTP) Transmitter Delay Compensation disabled.  */
#define   MCAN_DBTP_TDC_ENABLED_Val           (1U)                                           /**< (MCAN_DBTP) Transmitter Delay Compensation enabled.  */
#define MCAN_DBTP_TDC_DISABLED                (MCAN_DBTP_TDC_DISABLED_Val << MCAN_DBTP_TDC_Pos) /**< (MCAN_DBTP) Transmitter Delay Compensation disabled. Position  */
#define MCAN_DBTP_TDC_ENABLED                 (MCAN_DBTP_TDC_ENABLED_Val << MCAN_DBTP_TDC_Pos) /**< (MCAN_DBTP) Transmitter Delay Compensation enabled. Position  */
#define MCAN_DBTP_Msk                         (0x009F1FF7UL)                                 /**< (MCAN_DBTP) Register Mask  */

/* -------- MCAN_TEST : (MCAN Offset: 0x10) (R/W 32) Test Register -------- */
#define MCAN_TEST_LBCK_Pos                    (4U)                                           /**< (MCAN_TEST) Loop Back Mode (read/write) Position */
#define MCAN_TEST_LBCK_Msk                    (0x1U << MCAN_TEST_LBCK_Pos)                   /**< (MCAN_TEST) Loop Back Mode (read/write) Mask */
#define MCAN_TEST_LBCK(value)                 (MCAN_TEST_LBCK_Msk & ((value) << MCAN_TEST_LBCK_Pos))
#define   MCAN_TEST_LBCK_DISABLED_Val         (0U)                                           /**< (MCAN_TEST) Reset value. Loop Back mode is disabled.  */
#define   MCAN_TEST_LBCK_ENABLED_Val          (1U)                                           /**< (MCAN_TEST) Loop Back mode is enabled (see Section 6.1.9).  */
#define MCAN_TEST_LBCK_DISABLED               (MCAN_TEST_LBCK_DISABLED_Val << MCAN_TEST_LBCK_Pos) /**< (MCAN_TEST) Reset value. Loop Back mode is disabled. Position  */
#define MCAN_TEST_LBCK_ENABLED                (MCAN_TEST_LBCK_ENABLED_Val << MCAN_TEST_LBCK_Pos) /**< (MCAN_TEST) Loop Back mode is enabled (see Section 6.1.9). Position  */
#define MCAN_TEST_TX_Pos                      (5U)                                           /**< (MCAN_TEST) Control of Transmit Pin (read/write) Position */
#define MCAN_TEST_TX_Msk                      (0x3U << MCAN_TEST_TX_Pos)                     /**< (MCAN_TEST) Control of Transmit Pin (read/write) Mask */
#define MCAN_TEST_TX(value)                   (MCAN_TEST_TX_Msk & ((value) << MCAN_TEST_TX_Pos))
#define   MCAN_TEST_TX_RESET_Val              (0U)                                           /**< (MCAN_TEST) Reset value, CANTX controlled by the CAN Core, updated at the end of the CAN bit time.  */
#define   MCAN_TEST_TX_SAMPLE_POINT_MONITORING_Val (1U)                                           /**< (MCAN_TEST) Sample Point can be monitored at pin CANTX.  */
#define   MCAN_TEST_TX_DOMINANT_Val           (2U)                                           /**< (MCAN_TEST) Dominant ('0') level at pin CANTX.  */
#define   MCAN_TEST_TX_RECESSIVE_Val          (3U)                                           /**< (MCAN_TEST) Recessive ('1') at pin CANTX.  */
#define MCAN_TEST_TX_RESET                    (MCAN_TEST_TX_RESET_Val << MCAN_TEST_TX_Pos)   /**< (MCAN_TEST) Reset value, CANTX controlled by the CAN Core, updated at the end of the CAN bit time. Position  */
#define MCAN_TEST_TX_SAMPLE_POINT_MONITORING  (MCAN_TEST_TX_SAMPLE_POINT_MONITORING_Val << MCAN_TEST_TX_Pos) /**< (MCAN_TEST) Sample Point can be monitored at pin CANTX. Position  */
#define MCAN_TEST_TX_DOMINANT                 (MCAN_TEST_TX_DOMINANT_Val << MCAN_TEST_TX_Pos) /**< (MCAN_TEST) Dominant ('0') level at pin CANTX. Position  */
#define MCAN_TEST_TX_RECESSIVE                (MCAN_TEST_TX_RECESSIVE_Val << MCAN_TEST_TX_Pos) /**< (MCAN_TEST) Recessive ('1') at pin CANTX. Position  */
#define MCAN_TEST_RX_Pos                      (7U)                                           /**< (MCAN_TEST) Receive Pin (read-only) Position */
#define MCAN_TEST_RX_Msk                      (0x1U << MCAN_TEST_RX_Pos)                     /**< (MCAN_TEST) Receive Pin (read-only) Mask */
#define MCAN_TEST_RX(value)                   (MCAN_TEST_RX_Msk & ((value) << MCAN_TEST_RX_Pos))
#define MCAN_TEST_Msk                         (0x000000F0UL)                                 /**< (MCAN_TEST) Register Mask  */

/* -------- MCAN_RWD : (MCAN Offset: 0x14) (R/W 32) RAM Watchdog Register -------- */
#define MCAN_RWD_WDC_Pos                      (0U)                                           /**< (MCAN_RWD) Watchdog Configuration (read/write) Position */
#define MCAN_RWD_WDC_Msk                      (0xFFU << MCAN_RWD_WDC_Pos)                    /**< (MCAN_RWD) Watchdog Configuration (read/write) Mask */
#define MCAN_RWD_WDC(value)                   (MCAN_RWD_WDC_Msk & ((value) << MCAN_RWD_WDC_Pos))
#define MCAN_RWD_WDV_Pos                      (8U)                                           /**< (MCAN_RWD) Watchdog Value (read-only) Position */
#define MCAN_RWD_WDV_Msk                      (0xFFU << MCAN_RWD_WDV_Pos)                    /**< (MCAN_RWD) Watchdog Value (read-only) Mask */
#define MCAN_RWD_WDV(value)                   (MCAN_RWD_WDV_Msk & ((value) << MCAN_RWD_WDV_Pos))
#define MCAN_RWD_Msk                          (0x0000FFFFUL)                                 /**< (MCAN_RWD) Register Mask  */

/* -------- MCAN_CCCR : (MCAN Offset: 0x18) (R/W 32) CC Control Register -------- */
#define MCAN_CCCR_INIT_Pos                    (0U)                                           /**< (MCAN_CCCR) Initialization (read/write) Position */
#define MCAN_CCCR_INIT_Msk                    (0x1U << MCAN_CCCR_INIT_Pos)                   /**< (MCAN_CCCR) Initialization (read/write) Mask */
#define MCAN_CCCR_INIT(value)                 (MCAN_CCCR_INIT_Msk & ((value) << MCAN_CCCR_INIT_Pos))
#define   MCAN_CCCR_INIT_DISABLED_Val         (0U)                                           /**< (MCAN_CCCR) Normal operation.  */
#define   MCAN_CCCR_INIT_ENABLED_Val          (1U)                                           /**< (MCAN_CCCR) Initialization is started.  */
#define MCAN_CCCR_INIT_DISABLED               (MCAN_CCCR_INIT_DISABLED_Val << MCAN_CCCR_INIT_Pos) /**< (MCAN_CCCR) Normal operation. Position  */
#define MCAN_CCCR_INIT_ENABLED                (MCAN_CCCR_INIT_ENABLED_Val << MCAN_CCCR_INIT_Pos) /**< (MCAN_CCCR) Initialization is started. Position  */
#define MCAN_CCCR_CCE_Pos                     (1U)                                           /**< (MCAN_CCCR) Configuration Change Enable (read/write, write protection) Position */
#define MCAN_CCCR_CCE_Msk                     (0x1U << MCAN_CCCR_CCE_Pos)                    /**< (MCAN_CCCR) Configuration Change Enable (read/write, write protection) Mask */
#define MCAN_CCCR_CCE(value)                  (MCAN_CCCR_CCE_Msk & ((value) << MCAN_CCCR_CCE_Pos))
#define   MCAN_CCCR_CCE_PROTECTED_Val         (0U)                                           /**< (MCAN_CCCR) The processor has no write access to the protected configuration registers.  */
#define   MCAN_CCCR_CCE_CONFIGURABLE_Val      (1U)                                           /**< (MCAN_CCCR) The processor has write access to the protected configuration registers (while MCAN_CCCR.INIT = '1').  */
#define MCAN_CCCR_CCE_PROTECTED               (MCAN_CCCR_CCE_PROTECTED_Val << MCAN_CCCR_CCE_Pos) /**< (MCAN_CCCR) The processor has no write access to the protected configuration registers. Position  */
#define MCAN_CCCR_CCE_CONFIGURABLE            (MCAN_CCCR_CCE_CONFIGURABLE_Val << MCAN_CCCR_CCE_Pos) /**< (MCAN_CCCR) The processor has write access to the protected configuration registers (while MCAN_CCCR.INIT = '1'). Position  */
#define MCAN_CCCR_ASM_Pos                     (2U)                                           /**< (MCAN_CCCR) Restricted Operation Mode (read/write, write protection against '1') Position */
#define MCAN_CCCR_ASM_Msk                     (0x1U << MCAN_CCCR_ASM_Pos)                    /**< (MCAN_CCCR) Restricted Operation Mode (read/write, write protection against '1') Mask */
#define MCAN_CCCR_ASM(value)                  (MCAN_CCCR_ASM_Msk & ((value) << MCAN_CCCR_ASM_Pos))
#define   MCAN_CCCR_ASM_NORMAL_Val            (0U)                                           /**< (MCAN_CCCR) Normal CAN operation.  */
#define   MCAN_CCCR_ASM_RESTRICTED_Val        (1U)                                           /**< (MCAN_CCCR) Restricted Operation mode active.  */
#define MCAN_CCCR_ASM_NORMAL                  (MCAN_CCCR_ASM_NORMAL_Val << MCAN_CCCR_ASM_Pos) /**< (MCAN_CCCR) Normal CAN operation. Position  */
#define MCAN_CCCR_ASM_RESTRICTED              (MCAN_CCCR_ASM_RESTRICTED_Val << MCAN_CCCR_ASM_Pos) /**< (MCAN_CCCR) Restricted Operation mode active. Position  */
#define MCAN_CCCR_CSA_Pos                     (3U)                                           /**< (MCAN_CCCR) Clock Stop Acknowledge (read-only) Position */
#define MCAN_CCCR_CSA_Msk                     (0x1U << MCAN_CCCR_CSA_Pos)                    /**< (MCAN_CCCR) Clock Stop Acknowledge (read-only) Mask */
#define MCAN_CCCR_CSA(value)                  (MCAN_CCCR_CSA_Msk & ((value) << MCAN_CCCR_CSA_Pos))
#define MCAN_CCCR_CSR_Pos                     (4U)                                           /**< (MCAN_CCCR) Clock Stop Request (read/write) Position */
#define MCAN_CCCR_CSR_Msk                     (0x1U << MCAN_CCCR_CSR_Pos)                    /**< (MCAN_CCCR) Clock Stop Request (read/write) Mask */
#define MCAN_CCCR_CSR(value)                  (MCAN_CCCR_CSR_Msk & ((value) << MCAN_CCCR_CSR_Pos))
#define   MCAN_CCCR_CSR_NO_CLOCK_STOP_Val     (0U)                                           /**< (MCAN_CCCR) No clock stop is requested.  */
#define   MCAN_CCCR_CSR_CLOCK_STOP_Val        (1U)                                           /**< (MCAN_CCCR) Clock stop requested. When clock stop is requested, first INIT and then CSA will be set after all pend-ing transfer requests have been completed and the CAN bus reached idle.  */
#define MCAN_CCCR_CSR_NO_CLOCK_STOP           (MCAN_CCCR_CSR_NO_CLOCK_STOP_Val << MCAN_CCCR_CSR_Pos) /**< (MCAN_CCCR) No clock stop is requested. Position  */
#define MCAN_CCCR_CSR_CLOCK_STOP              (MCAN_CCCR_CSR_CLOCK_STOP_Val << MCAN_CCCR_CSR_Pos) /**< (MCAN_CCCR) Clock stop requested. When clock stop is requested, first INIT and then CSA will be set after all pend-ing transfer requests have been completed and the CAN bus reached idle. Position  */
#define MCAN_CCCR_MON_Pos                     (5U)                                           /**< (MCAN_CCCR) Bus Monitoring Mode (read/write, write protection against '1') Position */
#define MCAN_CCCR_MON_Msk                     (0x1U << MCAN_CCCR_MON_Pos)                    /**< (MCAN_CCCR) Bus Monitoring Mode (read/write, write protection against '1') Mask */
#define MCAN_CCCR_MON(value)                  (MCAN_CCCR_MON_Msk & ((value) << MCAN_CCCR_MON_Pos))
#define   MCAN_CCCR_MON_DISABLED_Val          (0U)                                           /**< (MCAN_CCCR) Bus Monitoring mode is disabled.  */
#define   MCAN_CCCR_MON_ENABLED_Val           (1U)                                           /**< (MCAN_CCCR) Bus Monitoring mode is enabled.  */
#define MCAN_CCCR_MON_DISABLED                (MCAN_CCCR_MON_DISABLED_Val << MCAN_CCCR_MON_Pos) /**< (MCAN_CCCR) Bus Monitoring mode is disabled. Position  */
#define MCAN_CCCR_MON_ENABLED                 (MCAN_CCCR_MON_ENABLED_Val << MCAN_CCCR_MON_Pos) /**< (MCAN_CCCR) Bus Monitoring mode is enabled. Position  */
#define MCAN_CCCR_DAR_Pos                     (6U)                                           /**< (MCAN_CCCR) Disable Automatic Retransmission (read/write, write protection) Position */
#define MCAN_CCCR_DAR_Msk                     (0x1U << MCAN_CCCR_DAR_Pos)                    /**< (MCAN_CCCR) Disable Automatic Retransmission (read/write, write protection) Mask */
#define MCAN_CCCR_DAR(value)                  (MCAN_CCCR_DAR_Msk & ((value) << MCAN_CCCR_DAR_Pos))
#define   MCAN_CCCR_DAR_AUTO_RETX_Val         (0U)                                           /**< (MCAN_CCCR) Automatic retransmission of messages not transmitted successfully enabled.  */
#define   MCAN_CCCR_DAR_NO_AUTO_RETX_Val      (1U)                                           /**< (MCAN_CCCR) Automatic retransmission disabled.  */
#define MCAN_CCCR_DAR_AUTO_RETX               (MCAN_CCCR_DAR_AUTO_RETX_Val << MCAN_CCCR_DAR_Pos) /**< (MCAN_CCCR) Automatic retransmission of messages not transmitted successfully enabled. Position  */
#define MCAN_CCCR_DAR_NO_AUTO_RETX            (MCAN_CCCR_DAR_NO_AUTO_RETX_Val << MCAN_CCCR_DAR_Pos) /**< (MCAN_CCCR) Automatic retransmission disabled. Position  */
#define MCAN_CCCR_TEST_Pos                    (7U)                                           /**< (MCAN_CCCR) Test Mode Enable (read/write, write protection against '1') Position */
#define MCAN_CCCR_TEST_Msk                    (0x1U << MCAN_CCCR_TEST_Pos)                   /**< (MCAN_CCCR) Test Mode Enable (read/write, write protection against '1') Mask */
#define MCAN_CCCR_TEST(value)                 (MCAN_CCCR_TEST_Msk & ((value) << MCAN_CCCR_TEST_Pos))
#define   MCAN_CCCR_TEST_DISABLED_Val         (0U)                                           /**< (MCAN_CCCR) Normal operation, MCAN_TEST register holds reset values.  */
#define   MCAN_CCCR_TEST_ENABLED_Val          (1U)                                           /**< (MCAN_CCCR) Test mode, write access to MCAN_TEST register enabled.  */
#define MCAN_CCCR_TEST_DISABLED               (MCAN_CCCR_TEST_DISABLED_Val << MCAN_CCCR_TEST_Pos) /**< (MCAN_CCCR) Normal operation, MCAN_TEST register holds reset values. Position  */
#define MCAN_CCCR_TEST_ENABLED                (MCAN_CCCR_TEST_ENABLED_Val << MCAN_CCCR_TEST_Pos) /**< (MCAN_CCCR) Test mode, write access to MCAN_TEST register enabled. Position  */
#define MCAN_CCCR_FDOE_Pos                    (8U)                                           /**< (MCAN_CCCR) CAN FD Operation Enable (read/write, write protection) Position */
#define MCAN_CCCR_FDOE_Msk                    (0x1U << MCAN_CCCR_FDOE_Pos)                   /**< (MCAN_CCCR) CAN FD Operation Enable (read/write, write protection) Mask */
#define MCAN_CCCR_FDOE(value)                 (MCAN_CCCR_FDOE_Msk & ((value) << MCAN_CCCR_FDOE_Pos))
#define   MCAN_CCCR_FDOE_DISABLED_Val         (0U)                                           /**< (MCAN_CCCR) FD operation disabled.  */
#define   MCAN_CCCR_FDOE_ENABLED_Val          (1U)                                           /**< (MCAN_CCCR) FD operation enabled.  */
#define MCAN_CCCR_FDOE_DISABLED               (MCAN_CCCR_FDOE_DISABLED_Val << MCAN_CCCR_FDOE_Pos) /**< (MCAN_CCCR) FD operation disabled. Position  */
#define MCAN_CCCR_FDOE_ENABLED                (MCAN_CCCR_FDOE_ENABLED_Val << MCAN_CCCR_FDOE_Pos) /**< (MCAN_CCCR) FD operation enabled. Position  */
#define MCAN_CCCR_BRSE_Pos                    (9U)                                           /**< (MCAN_CCCR) Bit Rate Switching Enable (read/write, write protection) Position */
#define MCAN_CCCR_BRSE_Msk                    (0x1U << MCAN_CCCR_BRSE_Pos)                   /**< (MCAN_CCCR) Bit Rate Switching Enable (read/write, write protection) Mask */
#define MCAN_CCCR_BRSE(value)                 (MCAN_CCCR_BRSE_Msk & ((value) << MCAN_CCCR_BRSE_Pos))
#define   MCAN_CCCR_BRSE_DISABLED_Val         (0U)                                           /**< (MCAN_CCCR) Bit rate switching for transmissions disabled.  */
#define   MCAN_CCCR_BRSE_ENABLED_Val          (1U)                                           /**< (MCAN_CCCR) Bit rate switching for transmissions enabled.  */
#define MCAN_CCCR_BRSE_DISABLED               (MCAN_CCCR_BRSE_DISABLED_Val << MCAN_CCCR_BRSE_Pos) /**< (MCAN_CCCR) Bit rate switching for transmissions disabled. Position  */
#define MCAN_CCCR_BRSE_ENABLED                (MCAN_CCCR_BRSE_ENABLED_Val << MCAN_CCCR_BRSE_Pos) /**< (MCAN_CCCR) Bit rate switching for transmissions enabled. Position  */
#define MCAN_CCCR_PXHD_Pos                    (12U)                                          /**< (MCAN_CCCR) Protocol Exception Event Handling (read/write, write protection) Position */
#define MCAN_CCCR_PXHD_Msk                    (0x1U << MCAN_CCCR_PXHD_Pos)                   /**< (MCAN_CCCR) Protocol Exception Event Handling (read/write, write protection) Mask */
#define MCAN_CCCR_PXHD(value)                 (MCAN_CCCR_PXHD_Msk & ((value) << MCAN_CCCR_PXHD_Pos))
#define MCAN_CCCR_EFBI_Pos                    (13U)                                          /**< (MCAN_CCCR) Edge Filtering during Bus Integration (read/write, write protection) Position */
#define MCAN_CCCR_EFBI_Msk                    (0x1U << MCAN_CCCR_EFBI_Pos)                   /**< (MCAN_CCCR) Edge Filtering during Bus Integration (read/write, write protection) Mask */
#define MCAN_CCCR_EFBI(value)                 (MCAN_CCCR_EFBI_Msk & ((value) << MCAN_CCCR_EFBI_Pos))
#define MCAN_CCCR_TXP_Pos                     (14U)                                          /**< (MCAN_CCCR) Transmit Pause (read/write, write protection) Position */
#define MCAN_CCCR_TXP_Msk                     (0x1U << MCAN_CCCR_TXP_Pos)                    /**< (MCAN_CCCR) Transmit Pause (read/write, write protection) Mask */
#define MCAN_CCCR_TXP(value)                  (MCAN_CCCR_TXP_Msk & ((value) << MCAN_CCCR_TXP_Pos))
#define MCAN_CCCR_NISO_Pos                    (15U)                                          /**< (MCAN_CCCR) Non-ISO Operation Position */
#define MCAN_CCCR_NISO_Msk                    (0x1U << MCAN_CCCR_NISO_Pos)                   /**< (MCAN_CCCR) Non-ISO Operation Mask */
#define MCAN_CCCR_NISO(value)                 (MCAN_CCCR_NISO_Msk & ((value) << MCAN_CCCR_NISO_Pos))
#define MCAN_CCCR_Msk                         (0x0000F3FFUL)                                 /**< (MCAN_CCCR) Register Mask  */

/* -------- MCAN_NBTP : (MCAN Offset: 0x1C) (R/W 32) Nominal Bit Timing and Prescaler Register -------- */
#define MCAN_NBTP_NTSEG2_Pos                  (0U)                                           /**< (MCAN_NBTP) Nominal Time Segment After Sample Point Position */
#define MCAN_NBTP_NTSEG2_Msk                  (0x7FU << MCAN_NBTP_NTSEG2_Pos)                /**< (MCAN_NBTP) Nominal Time Segment After Sample Point Mask */
#define MCAN_NBTP_NTSEG2(value)               (MCAN_NBTP_NTSEG2_Msk & ((value) << MCAN_NBTP_NTSEG2_Pos))
#define MCAN_NBTP_NTSEG1_Pos                  (8U)                                           /**< (MCAN_NBTP) Nominal Time Segment Before Sample Point Position */
#define MCAN_NBTP_NTSEG1_Msk                  (0xFFU << MCAN_NBTP_NTSEG1_Pos)                /**< (MCAN_NBTP) Nominal Time Segment Before Sample Point Mask */
#define MCAN_NBTP_NTSEG1(value)               (MCAN_NBTP_NTSEG1_Msk & ((value) << MCAN_NBTP_NTSEG1_Pos))
#define MCAN_NBTP_NBRP_Pos                    (16U)                                          /**< (MCAN_NBTP) Nominal Bit Rate Prescaler Position */
#define MCAN_NBTP_NBRP_Msk                    (0x1FFU << MCAN_NBTP_NBRP_Pos)                 /**< (MCAN_NBTP) Nominal Bit Rate Prescaler Mask */
#define MCAN_NBTP_NBRP(value)                 (MCAN_NBTP_NBRP_Msk & ((value) << MCAN_NBTP_NBRP_Pos))
#define MCAN_NBTP_NSJW_Pos                    (25U)                                          /**< (MCAN_NBTP) Nominal (Re) Synchronization Jump Width Position */
#define MCAN_NBTP_NSJW_Msk                    (0x7FU << MCAN_NBTP_NSJW_Pos)                  /**< (MCAN_NBTP) Nominal (Re) Synchronization Jump Width Mask */
#define MCAN_NBTP_NSJW(value)                 (MCAN_NBTP_NSJW_Msk & ((value) << MCAN_NBTP_NSJW_Pos))
#define MCAN_NBTP_Msk                         (0xFFFFFF7FUL)                                 /**< (MCAN_NBTP) Register Mask  */

/* -------- MCAN_TSCC : (MCAN Offset: 0x20) (R/W 32) Timestamp Counter Configuration Register -------- */
#define MCAN_TSCC_TSS_Pos                     (0U)                                           /**< (MCAN_TSCC) Timestamp Select Position */
#define MCAN_TSCC_TSS_Msk                     (0x3U << MCAN_TSCC_TSS_Pos)                    /**< (MCAN_TSCC) Timestamp Select Mask */
#define MCAN_TSCC_TSS(value)                  (MCAN_TSCC_TSS_Msk & ((value) << MCAN_TSCC_TSS_Pos))
#define   MCAN_TSCC_TSS_ALWAYS_0_Val          (0U)                                           /**< (MCAN_TSCC) Timestamp counter value always 0x0000  */
#define   MCAN_TSCC_TSS_TCP_INC_Val           (1U)                                           /**< (MCAN_TSCC) Timestamp counter value incremented according to TCP  */
#define   MCAN_TSCC_TSS_EXT_TIMESTAMP_Val     (2U)                                           /**< (MCAN_TSCC) External timestamp counter value used  */
#define MCAN_TSCC_TSS_ALWAYS_0                (MCAN_TSCC_TSS_ALWAYS_0_Val << MCAN_TSCC_TSS_Pos) /**< (MCAN_TSCC) Timestamp counter value always 0x0000 Position  */
#define MCAN_TSCC_TSS_TCP_INC                 (MCAN_TSCC_TSS_TCP_INC_Val << MCAN_TSCC_TSS_Pos) /**< (MCAN_TSCC) Timestamp counter value incremented according to TCP Position  */
#define MCAN_TSCC_TSS_EXT_TIMESTAMP           (MCAN_TSCC_TSS_EXT_TIMESTAMP_Val << MCAN_TSCC_TSS_Pos) /**< (MCAN_TSCC) External timestamp counter value used Position  */
#define MCAN_TSCC_TCP_Pos                     (16U)                                          /**< (MCAN_TSCC) Timestamp Counter Prescaler Position */
#define MCAN_TSCC_TCP_Msk                     (0xFU << MCAN_TSCC_TCP_Pos)                    /**< (MCAN_TSCC) Timestamp Counter Prescaler Mask */
#define MCAN_TSCC_TCP(value)                  (MCAN_TSCC_TCP_Msk & ((value) << MCAN_TSCC_TCP_Pos))
#define MCAN_TSCC_Msk                         (0x000F0003UL)                                 /**< (MCAN_TSCC) Register Mask  */

/* -------- MCAN_TSCV : (MCAN Offset: 0x24) (R/W 32) Timestamp Counter Value Register -------- */
#define MCAN_TSCV_TSC_Pos                     (0U)                                           /**< (MCAN_TSCV) Timestamp Counter (cleared on write) Position */
#define MCAN_TSCV_TSC_Msk                     (0xFFFFU << MCAN_TSCV_TSC_Pos)                 /**< (MCAN_TSCV) Timestamp Counter (cleared on write) Mask */
#define MCAN_TSCV_TSC(value)                  (MCAN_TSCV_TSC_Msk & ((value) << MCAN_TSCV_TSC_Pos))
#define MCAN_TSCV_Msk                         (0x0000FFFFUL)                                 /**< (MCAN_TSCV) Register Mask  */

/* -------- MCAN_TOCC : (MCAN Offset: 0x28) (R/W 32) Timeout Counter Configuration Register -------- */
#define MCAN_TOCC_ETOC_Pos                    (0U)                                           /**< (MCAN_TOCC) Enable Timeout Counter Position */
#define MCAN_TOCC_ETOC_Msk                    (0x1U << MCAN_TOCC_ETOC_Pos)                   /**< (MCAN_TOCC) Enable Timeout Counter Mask */
#define MCAN_TOCC_ETOC(value)                 (MCAN_TOCC_ETOC_Msk & ((value) << MCAN_TOCC_ETOC_Pos))
#define   MCAN_TOCC_ETOC_NO_TIMEOUT_Val       (0U)                                           /**< (MCAN_TOCC) Timeout Counter disabled.  */
#define   MCAN_TOCC_ETOC_TOS_CONTROLLED_Val   (1U)                                           /**< (MCAN_TOCC) Timeout Counter enabled.  */
#define MCAN_TOCC_ETOC_NO_TIMEOUT             (MCAN_TOCC_ETOC_NO_TIMEOUT_Val << MCAN_TOCC_ETOC_Pos) /**< (MCAN_TOCC) Timeout Counter disabled. Position  */
#define MCAN_TOCC_ETOC_TOS_CONTROLLED         (MCAN_TOCC_ETOC_TOS_CONTROLLED_Val << MCAN_TOCC_ETOC_Pos) /**< (MCAN_TOCC) Timeout Counter enabled. Position  */
#define MCAN_TOCC_TOS_Pos                     (1U)                                           /**< (MCAN_TOCC) Timeout Select Position */
#define MCAN_TOCC_TOS_Msk                     (0x3U << MCAN_TOCC_TOS_Pos)                    /**< (MCAN_TOCC) Timeout Select Mask */
#define MCAN_TOCC_TOS(value)                  (MCAN_TOCC_TOS_Msk & ((value) << MCAN_TOCC_TOS_Pos))
#define   MCAN_TOCC_TOS_CONTINUOUS_Val        (0U)                                           /**< (MCAN_TOCC) Continuous operation  */
#define   MCAN_TOCC_TOS_TX_EV_TIMEOUT_Val     (1U)                                           /**< (MCAN_TOCC) Timeout controlled by Tx Event FIFO  */
#define   MCAN_TOCC_TOS_RX0_EV_TIMEOUT_Val    (2U)                                           /**< (MCAN_TOCC) Timeout controlled by Receive FIFO 0  */
#define   MCAN_TOCC_TOS_RX1_EV_TIMEOUT_Val    (3U)                                           /**< (MCAN_TOCC) Timeout controlled by Receive FIFO 1  */
#define MCAN_TOCC_TOS_CONTINUOUS              (MCAN_TOCC_TOS_CONTINUOUS_Val << MCAN_TOCC_TOS_Pos) /**< (MCAN_TOCC) Continuous operation Position  */
#define MCAN_TOCC_TOS_TX_EV_TIMEOUT           (MCAN_TOCC_TOS_TX_EV_TIMEOUT_Val << MCAN_TOCC_TOS_Pos) /**< (MCAN_TOCC) Timeout controlled by Tx Event FIFO Position  */
#define MCAN_TOCC_TOS_RX0_EV_TIMEOUT          (MCAN_TOCC_TOS_RX0_EV_TIMEOUT_Val << MCAN_TOCC_TOS_Pos) /**< (MCAN_TOCC) Timeout controlled by Receive FIFO 0 Position  */
#define MCAN_TOCC_TOS_RX1_EV_TIMEOUT          (MCAN_TOCC_TOS_RX1_EV_TIMEOUT_Val << MCAN_TOCC_TOS_Pos) /**< (MCAN_TOCC) Timeout controlled by Receive FIFO 1 Position  */
#define MCAN_TOCC_TOP_Pos                     (16U)                                          /**< (MCAN_TOCC) Timeout Period Position */
#define MCAN_TOCC_TOP_Msk                     (0xFFFFU << MCAN_TOCC_TOP_Pos)                 /**< (MCAN_TOCC) Timeout Period Mask */
#define MCAN_TOCC_TOP(value)                  (MCAN_TOCC_TOP_Msk & ((value) << MCAN_TOCC_TOP_Pos))
#define MCAN_TOCC_Msk                         (0xFFFF0007UL)                                 /**< (MCAN_TOCC) Register Mask  */

/* -------- MCAN_TOCV : (MCAN Offset: 0x2C) (R/W 32) Timeout Counter Value Register -------- */
#define MCAN_TOCV_TOC_Pos                     (0U)                                           /**< (MCAN_TOCV) Timeout Counter (cleared on write) Position */
#define MCAN_TOCV_TOC_Msk                     (0xFFFFU << MCAN_TOCV_TOC_Pos)                 /**< (MCAN_TOCV) Timeout Counter (cleared on write) Mask */
#define MCAN_TOCV_TOC(value)                  (MCAN_TOCV_TOC_Msk & ((value) << MCAN_TOCV_TOC_Pos))
#define MCAN_TOCV_Msk                         (0x0000FFFFUL)                                 /**< (MCAN_TOCV) Register Mask  */

/* -------- MCAN_ECR : (MCAN Offset: 0x40) (R/  32) Error Counter Register -------- */
#define MCAN_ECR_TEC_Pos                      (0U)                                           /**< (MCAN_ECR) Transmit Error Counter Position */
#define MCAN_ECR_TEC_Msk                      (0xFFU << MCAN_ECR_TEC_Pos)                    /**< (MCAN_ECR) Transmit Error Counter Mask */
#define MCAN_ECR_TEC(value)                   (MCAN_ECR_TEC_Msk & ((value) << MCAN_ECR_TEC_Pos))
#define MCAN_ECR_REC_Pos                      (8U)                                           /**< (MCAN_ECR) Receive Error Counter Position */
#define MCAN_ECR_REC_Msk                      (0x7FU << MCAN_ECR_REC_Pos)                    /**< (MCAN_ECR) Receive Error Counter Mask */
#define MCAN_ECR_REC(value)                   (MCAN_ECR_REC_Msk & ((value) << MCAN_ECR_REC_Pos))
#define MCAN_ECR_RP_Pos                       (15U)                                          /**< (MCAN_ECR) Receive Error Passive Position */
#define MCAN_ECR_RP_Msk                       (0x1U << MCAN_ECR_RP_Pos)                      /**< (MCAN_ECR) Receive Error Passive Mask */
#define MCAN_ECR_RP(value)                    (MCAN_ECR_RP_Msk & ((value) << MCAN_ECR_RP_Pos))
#define MCAN_ECR_CEL_Pos                      (16U)                                          /**< (MCAN_ECR) CAN Error Logging (cleared on read) Position */
#define MCAN_ECR_CEL_Msk                      (0xFFU << MCAN_ECR_CEL_Pos)                    /**< (MCAN_ECR) CAN Error Logging (cleared on read) Mask */
#define MCAN_ECR_CEL(value)                   (MCAN_ECR_CEL_Msk & ((value) << MCAN_ECR_CEL_Pos))
#define MCAN_ECR_Msk                          (0x00FFFFFFUL)                                 /**< (MCAN_ECR) Register Mask  */

/* -------- MCAN_PSR : (MCAN Offset: 0x44) (R/  32) Protocol Status Register -------- */
#define MCAN_PSR_LEC_Pos                      (0U)                                           /**< (MCAN_PSR) Last Error Code (set to 111 on read) Position */
#define MCAN_PSR_LEC_Msk                      (0x7U << MCAN_PSR_LEC_Pos)                     /**< (MCAN_PSR) Last Error Code (set to 111 on read) Mask */
#define MCAN_PSR_LEC(value)                   (MCAN_PSR_LEC_Msk & ((value) << MCAN_PSR_LEC_Pos))
#define   MCAN_PSR_LEC_NO_ERROR_Val           (0U)                                           /**< (MCAN_PSR) No error occurred since LEC has been reset by successful reception or transmission.  */
#define   MCAN_PSR_LEC_STUFF_ERROR_Val        (1U)                                           /**< (MCAN_PSR) More than 5 equal bits in a sequence have occurred in a part of a received message where this is not allowed.  */
#define   MCAN_PSR_LEC_FORM_ERROR_Val         (2U)                                           /**< (MCAN_PSR) A fixed format part of a received frame has the wrong format.  */
#define   MCAN_PSR_LEC_ACK_ERROR_Val          (3U)                                           /**< (MCAN_PSR) The message transmitted by the MCAN was not acknowledged by another node.  */
#define   MCAN_PSR_LEC_BIT1_ERROR_Val         (4U)                                           /**< (MCAN_PSR) During transmission of a message (with the exception of the arbitration field), the device tried to send a recessive level (bit of logical value '1'), but the monitored bus value was dominant.  */
#define   MCAN_PSR_LEC_BIT0_ERROR_Val         (5U)                                           /**< (MCAN_PSR) During transmission of a message (or acknowledge bit, or active error flag, or overload flag), the device tried to send a dominant level (data or identifier bit logical value '0'), but the monitored bus value was recessive. During Bus_Off recovery, this status is set each time a sequence of 11 recessive bits has been monitored. This enables the processor to monitor the proceeding of the Bus_Off recovery sequence (indicating the bus is not stuck at dominant or continuously disturbed).  */
#define   MCAN_PSR_LEC_CRC_ERROR_Val          (6U)                                           /**< (MCAN_PSR) The CRC check sum of a received message was incorrect. The CRC of an incoming message does not match the CRC calculated from the received data.  */
#define   MCAN_PSR_LEC_NO_CHANGE_Val          (7U)                                           /**< (MCAN_PSR) Any read access to the Protocol Status Register re-initializes the LEC to '7'. When the LEC shows value '7', no CAN bus event was detected since the last processor read access to the Protocol Status Register.  */
#define MCAN_PSR_LEC_NO_ERROR                 (MCAN_PSR_LEC_NO_ERROR_Val << MCAN_PSR_LEC_Pos) /**< (MCAN_PSR) No error occurred since LEC has been reset by successful reception or transmission. Position  */
#define MCAN_PSR_LEC_STUFF_ERROR              (MCAN_PSR_LEC_STUFF_ERROR_Val << MCAN_PSR_LEC_Pos) /**< (MCAN_PSR) More than 5 equal bits in a sequence have occurred in a part of a received message where this is not allowed. Position  */
#define MCAN_PSR_LEC_FORM_ERROR               (MCAN_PSR_LEC_FORM_ERROR_Val << MCAN_PSR_LEC_Pos) /**< (MCAN_PSR) A fixed format part of a received frame has the wrong format. Position  */
#define MCAN_PSR_LEC_ACK_ERROR                (MCAN_PSR_LEC_ACK_ERROR_Val << MCAN_PSR_LEC_Pos) /**< (MCAN_PSR) The message transmitted by the MCAN was not acknowledged by another node. Position  */
#define MCAN_PSR_LEC_BIT1_ERROR               (MCAN_PSR_LEC_BIT1_ERROR_Val << MCAN_PSR_LEC_Pos) /**< (MCAN_PSR) During transmission of a message (with the exception of the arbitration field), the device tried to send a recessive level (bit of logical value '1'), but the monitored bus value was dominant. Position  */
#define MCAN_PSR_LEC_BIT0_ERROR               (MCAN_PSR_LEC_BIT0_ERROR_Val << MCAN_PSR_LEC_Pos) /**< (MCAN_PSR) During transmission of a message (or acknowledge bit, or active error flag, or overload flag), the device tried to send a dominant level (data or identifier bit logical value '0'), but the monitored bus value was recessive. During Bus_Off recovery, this status is set each time a sequence of 11 recessive bits has been monitored. This enables the processor to monitor the proceeding of the Bus_Off recovery sequence (indicating the bus is not stuck at dominant or continuously disturbed). Position  */
#define MCAN_PSR_LEC_CRC_ERROR                (MCAN_PSR_LEC_CRC_ERROR_Val << MCAN_PSR_LEC_Pos) /**< (MCAN_PSR) The CRC check sum of a received message was incorrect. The CRC of an incoming message does not match the CRC calculated from the received data. Position  */
#define MCAN_PSR_LEC_NO_CHANGE                (MCAN_PSR_LEC_NO_CHANGE_Val << MCAN_PSR_LEC_Pos) /**< (MCAN_PSR) Any read access to the Protocol Status Register re-initializes the LEC to '7'. When the LEC shows value '7', no CAN bus event was detected since the last processor read access to the Protocol Status Register. Position  */
#define MCAN_PSR_ACT_Pos                      (3U)                                           /**< (MCAN_PSR) Activity Position */
#define MCAN_PSR_ACT_Msk                      (0x3U << MCAN_PSR_ACT_Pos)                     /**< (MCAN_PSR) Activity Mask */
#define MCAN_PSR_ACT(value)                   (MCAN_PSR_ACT_Msk & ((value) << MCAN_PSR_ACT_Pos))
#define   MCAN_PSR_ACT_SYNCHRONIZING_Val      (0U)                                           /**< (MCAN_PSR) Node is synchronizing on CAN communication  */
#define   MCAN_PSR_ACT_IDLE_Val               (1U)                                           /**< (MCAN_PSR) Node is neither receiver nor transmitter  */
#define   MCAN_PSR_ACT_RECEIVER_Val           (2U)                                           /**< (MCAN_PSR) Node is operating as receiver  */
#define   MCAN_PSR_ACT_TRANSMITTER_Val        (3U)                                           /**< (MCAN_PSR) Node is operating as transmitter  */
#define MCAN_PSR_ACT_SYNCHRONIZING            (MCAN_PSR_ACT_SYNCHRONIZING_Val << MCAN_PSR_ACT_Pos) /**< (MCAN_PSR) Node is synchronizing on CAN communication Position  */
#define MCAN_PSR_ACT_IDLE                     (MCAN_PSR_ACT_IDLE_Val << MCAN_PSR_ACT_Pos)    /**< (MCAN_PSR) Node is neither receiver nor transmitter Position  */
#define MCAN_PSR_ACT_RECEIVER                 (MCAN_PSR_ACT_RECEIVER_Val << MCAN_PSR_ACT_Pos) /**< (MCAN_PSR) Node is operating as receiver Position  */
#define MCAN_PSR_ACT_TRANSMITTER              (MCAN_PSR_ACT_TRANSMITTER_Val << MCAN_PSR_ACT_Pos) /**< (MCAN_PSR) Node is operating as transmitter Position  */
#define MCAN_PSR_EP_Pos                       (5U)                                           /**< (MCAN_PSR) Error Passive Position */
#define MCAN_PSR_EP_Msk                       (0x1U << MCAN_PSR_EP_Pos)                      /**< (MCAN_PSR) Error Passive Mask */
#define MCAN_PSR_EP(value)                    (MCAN_PSR_EP_Msk & ((value) << MCAN_PSR_EP_Pos))
#define MCAN_PSR_EW_Pos                       (6U)                                           /**< (MCAN_PSR) Warning Status Position */
#define MCAN_PSR_EW_Msk                       (0x1U << MCAN_PSR_EW_Pos)                      /**< (MCAN_PSR) Warning Status Mask */
#define MCAN_PSR_EW(value)                    (MCAN_PSR_EW_Msk & ((value) << MCAN_PSR_EW_Pos))
#define MCAN_PSR_BO_Pos                       (7U)                                           /**< (MCAN_PSR) Bus_Off Status Position */
#define MCAN_PSR_BO_Msk                       (0x1U << MCAN_PSR_BO_Pos)                      /**< (MCAN_PSR) Bus_Off Status Mask */
#define MCAN_PSR_BO(value)                    (MCAN_PSR_BO_Msk & ((value) << MCAN_PSR_BO_Pos))
#define MCAN_PSR_DLEC_Pos                     (8U)                                           /**< (MCAN_PSR) Data Phase Last Error Code (set to 111 on read) Position */
#define MCAN_PSR_DLEC_Msk                     (0x7U << MCAN_PSR_DLEC_Pos)                    /**< (MCAN_PSR) Data Phase Last Error Code (set to 111 on read) Mask */
#define MCAN_PSR_DLEC(value)                  (MCAN_PSR_DLEC_Msk & ((value) << MCAN_PSR_DLEC_Pos))
#define MCAN_PSR_RESI_Pos                     (11U)                                          /**< (MCAN_PSR) ESI Flag of Last Received CAN FD Message (cleared on read) Position */
#define MCAN_PSR_RESI_Msk                     (0x1U << MCAN_PSR_RESI_Pos)                    /**< (MCAN_PSR) ESI Flag of Last Received CAN FD Message (cleared on read) Mask */
#define MCAN_PSR_RESI(value)                  (MCAN_PSR_RESI_Msk & ((value) << MCAN_PSR_RESI_Pos))
#define MCAN_PSR_RBRS_Pos                     (12U)                                          /**< (MCAN_PSR) BRS Flag of Last Received CAN FD Message (cleared on read) Position */
#define MCAN_PSR_RBRS_Msk                     (0x1U << MCAN_PSR_RBRS_Pos)                    /**< (MCAN_PSR) BRS Flag of Last Received CAN FD Message (cleared on read) Mask */
#define MCAN_PSR_RBRS(value)                  (MCAN_PSR_RBRS_Msk & ((value) << MCAN_PSR_RBRS_Pos))
#define MCAN_PSR_RFDF_Pos                     (13U)                                          /**< (MCAN_PSR) Received a CAN FD Message (cleared on read) Position */
#define MCAN_PSR_RFDF_Msk                     (0x1U << MCAN_PSR_RFDF_Pos)                    /**< (MCAN_PSR) Received a CAN FD Message (cleared on read) Mask */
#define MCAN_PSR_RFDF(value)                  (MCAN_PSR_RFDF_Msk & ((value) << MCAN_PSR_RFDF_Pos))
#define MCAN_PSR_PXE_Pos                      (14U)                                          /**< (MCAN_PSR) Protocol Exception Event (cleared on read) Position */
#define MCAN_PSR_PXE_Msk                      (0x1U << MCAN_PSR_PXE_Pos)                     /**< (MCAN_PSR) Protocol Exception Event (cleared on read) Mask */
#define MCAN_PSR_PXE(value)                   (MCAN_PSR_PXE_Msk & ((value) << MCAN_PSR_PXE_Pos))
#define MCAN_PSR_TDCV_Pos                     (16U)                                          /**< (MCAN_PSR) Transmitter Delay Compensation Value Position */
#define MCAN_PSR_TDCV_Msk                     (0x7FU << MCAN_PSR_TDCV_Pos)                   /**< (MCAN_PSR) Transmitter Delay Compensation Value Mask */
#define MCAN_PSR_TDCV(value)                  (MCAN_PSR_TDCV_Msk & ((value) << MCAN_PSR_TDCV_Pos))
#define MCAN_PSR_Msk                          (0x007F7FFFUL)                                 /**< (MCAN_PSR) Register Mask  */

/* -------- MCAN_TDCR : (MCAN Offset: 0x48) (R/W 32) Transmit Delay Compensation Register -------- */
#define MCAN_TDCR_TDCF_Pos                    (0U)                                           /**< (MCAN_TDCR) Transmitter Delay Compensation Filter Position */
#define MCAN_TDCR_TDCF_Msk                    (0x7FU << MCAN_TDCR_TDCF_Pos)                  /**< (MCAN_TDCR) Transmitter Delay Compensation Filter Mask */
#define MCAN_TDCR_TDCF(value)                 (MCAN_TDCR_TDCF_Msk & ((value) << MCAN_TDCR_TDCF_Pos))
#define MCAN_TDCR_TDCO_Pos                    (8U)                                           /**< (MCAN_TDCR) Transmitter Delay Compensation Offset Position */
#define MCAN_TDCR_TDCO_Msk                    (0x7FU << MCAN_TDCR_TDCO_Pos)                  /**< (MCAN_TDCR) Transmitter Delay Compensation Offset Mask */
#define MCAN_TDCR_TDCO(value)                 (MCAN_TDCR_TDCO_Msk & ((value) << MCAN_TDCR_TDCO_Pos))
#define MCAN_TDCR_Msk                         (0x00007F7FUL)                                 /**< (MCAN_TDCR) Register Mask  */

/* -------- MCAN_IR : (MCAN Offset: 0x50) (R/W 32) Interrupt Register -------- */
#define MCAN_IR_RF0N_Pos                      (0U)                                           /**< (MCAN_IR) Receive FIFO 0 New Message Position */
#define MCAN_IR_RF0N_Msk                      (0x1U << MCAN_IR_RF0N_Pos)                     /**< (MCAN_IR) Receive FIFO 0 New Message Mask */
#define MCAN_IR_RF0N(value)                   (MCAN_IR_RF0N_Msk & ((value) << MCAN_IR_RF0N_Pos))
#define MCAN_IR_RF0W_Pos                      (1U)                                           /**< (MCAN_IR) Receive FIFO 0 Watermark Reached Position */
#define MCAN_IR_RF0W_Msk                      (0x1U << MCAN_IR_RF0W_Pos)                     /**< (MCAN_IR) Receive FIFO 0 Watermark Reached Mask */
#define MCAN_IR_RF0W(value)                   (MCAN_IR_RF0W_Msk & ((value) << MCAN_IR_RF0W_Pos))
#define MCAN_IR_RF0F_Pos                      (2U)                                           /**< (MCAN_IR) Receive FIFO 0 Full Position */
#define MCAN_IR_RF0F_Msk                      (0x1U << MCAN_IR_RF0F_Pos)                     /**< (MCAN_IR) Receive FIFO 0 Full Mask */
#define MCAN_IR_RF0F(value)                   (MCAN_IR_RF0F_Msk & ((value) << MCAN_IR_RF0F_Pos))
#define MCAN_IR_RF0L_Pos                      (3U)                                           /**< (MCAN_IR) Receive FIFO 0 Message Lost Position */
#define MCAN_IR_RF0L_Msk                      (0x1U << MCAN_IR_RF0L_Pos)                     /**< (MCAN_IR) Receive FIFO 0 Message Lost Mask */
#define MCAN_IR_RF0L(value)                   (MCAN_IR_RF0L_Msk & ((value) << MCAN_IR_RF0L_Pos))
#define MCAN_IR_RF1N_Pos                      (4U)                                           /**< (MCAN_IR) Receive FIFO 1 New Message Position */
#define MCAN_IR_RF1N_Msk                      (0x1U << MCAN_IR_RF1N_Pos)                     /**< (MCAN_IR) Receive FIFO 1 New Message Mask */
#define MCAN_IR_RF1N(value)                   (MCAN_IR_RF1N_Msk & ((value) << MCAN_IR_RF1N_Pos))
#define MCAN_IR_RF1W_Pos                      (5U)                                           /**< (MCAN_IR) Receive FIFO 1 Watermark Reached Position */
#define MCAN_IR_RF1W_Msk                      (0x1U << MCAN_IR_RF1W_Pos)                     /**< (MCAN_IR) Receive FIFO 1 Watermark Reached Mask */
#define MCAN_IR_RF1W(value)                   (MCAN_IR_RF1W_Msk & ((value) << MCAN_IR_RF1W_Pos))
#define MCAN_IR_RF1F_Pos                      (6U)                                           /**< (MCAN_IR) Receive FIFO 1 Full Position */
#define MCAN_IR_RF1F_Msk                      (0x1U << MCAN_IR_RF1F_Pos)                     /**< (MCAN_IR) Receive FIFO 1 Full Mask */
#define MCAN_IR_RF1F(value)                   (MCAN_IR_RF1F_Msk & ((value) << MCAN_IR_RF1F_Pos))
#define MCAN_IR_RF1L_Pos                      (7U)                                           /**< (MCAN_IR) Receive FIFO 1 Message Lost Position */
#define MCAN_IR_RF1L_Msk                      (0x1U << MCAN_IR_RF1L_Pos)                     /**< (MCAN_IR) Receive FIFO 1 Message Lost Mask */
#define MCAN_IR_RF1L(value)                   (MCAN_IR_RF1L_Msk & ((value) << MCAN_IR_RF1L_Pos))
#define MCAN_IR_HPM_Pos                       (8U)                                           /**< (MCAN_IR) High Priority Message Position */
#define MCAN_IR_HPM_Msk                       (0x1U << MCAN_IR_HPM_Pos)                      /**< (MCAN_IR) High Priority Message Mask */
#define MCAN_IR_HPM(value)                    (MCAN_IR_HPM_Msk & ((value) << MCAN_IR_HPM_Pos))
#define MCAN_IR_TC_Pos                        (9U)                                           /**< (MCAN_IR) Transmission Completed Position */
#define MCAN_IR_TC_Msk                        (0x1U << MCAN_IR_TC_Pos)                       /**< (MCAN_IR) Transmission Completed Mask */
#define MCAN_IR_TC(value)                     (MCAN_IR_TC_Msk & ((value) << MCAN_IR_TC_Pos))
#define MCAN_IR_TCF_Pos                       (10U)                                          /**< (MCAN_IR) Transmission Cancellation Finished Position */
#define MCAN_IR_TCF_Msk                       (0x1U << MCAN_IR_TCF_Pos)                      /**< (MCAN_IR) Transmission Cancellation Finished Mask */
#define MCAN_IR_TCF(value)                    (MCAN_IR_TCF_Msk & ((value) << MCAN_IR_TCF_Pos))
#define MCAN_IR_TFE_Pos                       (11U)                                          /**< (MCAN_IR) Tx FIFO Empty Position */
#define MCAN_IR_TFE_Msk                       (0x1U << MCAN_IR_TFE_Pos)                      /**< (MCAN_IR) Tx FIFO Empty Mask */
#define MCAN_IR_TFE(value)                    (MCAN_IR_TFE_Msk & ((value) << MCAN_IR_TFE_Pos))
#define MCAN_IR_TEFN_Pos                      (12U)                                          /**< (MCAN_IR) Tx Event FIFO New Entry Position */
#define MCAN_IR_TEFN_Msk                      (0x1U << MCAN_IR_TEFN_Pos)                     /**< (MCAN_IR) Tx Event FIFO New Entry Mask */
#define MCAN_IR_TEFN(value)                   (MCAN_IR_TEFN_Msk & ((value) << MCAN_IR_TEFN_Pos))
#define MCAN_IR_TEFW_Pos                      (13U)                                          /**< (MCAN_IR) Tx Event FIFO Watermark Reached Position */
#define MCAN_IR_TEFW_Msk                      (0x1U << MCAN_IR_TEFW_Pos)                     /**< (MCAN_IR) Tx Event FIFO Watermark Reached Mask */
#define MCAN_IR_TEFW(value)                   (MCAN_IR_TEFW_Msk & ((value) << MCAN_IR_TEFW_Pos))
#define MCAN_IR_TEFF_Pos                      (14U)                                          /**< (MCAN_IR) Tx Event FIFO Full Position */
#define MCAN_IR_TEFF_Msk                      (0x1U << MCAN_IR_TEFF_Pos)                     /**< (MCAN_IR) Tx Event FIFO Full Mask */
#define MCAN_IR_TEFF(value)                   (MCAN_IR_TEFF_Msk & ((value) << MCAN_IR_TEFF_Pos))
#define MCAN_IR_TEFL_Pos                      (15U)                                          /**< (MCAN_IR) Tx Event FIFO Element Lost Position */
#define MCAN_IR_TEFL_Msk                      (0x1U << MCAN_IR_TEFL_Pos)                     /**< (MCAN_IR) Tx Event FIFO Element Lost Mask */
#define MCAN_IR_TEFL(value)                   (MCAN_IR_TEFL_Msk & ((value) << MCAN_IR_TEFL_Pos))
#define MCAN_IR_TSW_Pos                       (16U)                                          /**< (MCAN_IR) Timestamp Wraparound Position */
#define MCAN_IR_TSW_Msk                       (0x1U << MCAN_IR_TSW_Pos)                      /**< (MCAN_IR) Timestamp Wraparound Mask */
#define MCAN_IR_TSW(value)                    (MCAN_IR_TSW_Msk & ((value) << MCAN_IR_TSW_Pos))
#define MCAN_IR_MRAF_Pos                      (17U)                                          /**< (MCAN_IR) Message RAM Access Failure Position */
#define MCAN_IR_MRAF_Msk                      (0x1U << MCAN_IR_MRAF_Pos)                     /**< (MCAN_IR) Message RAM Access Failure Mask */
#define MCAN_IR_MRAF(value)                   (MCAN_IR_MRAF_Msk & ((value) << MCAN_IR_MRAF_Pos))
#define MCAN_IR_TOO_Pos                       (18U)                                          /**< (MCAN_IR) Timeout Occurred Position */
#define MCAN_IR_TOO_Msk                       (0x1U << MCAN_IR_TOO_Pos)                      /**< (MCAN_IR) Timeout Occurred Mask */
#define MCAN_IR_TOO(value)                    (MCAN_IR_TOO_Msk & ((value) << MCAN_IR_TOO_Pos))
#define MCAN_IR_DRX_Pos                       (19U)                                          /**< (MCAN_IR) Message stored to Dedicated Receive Buffer Position */
#define MCAN_IR_DRX_Msk                       (0x1U << MCAN_IR_DRX_Pos)                      /**< (MCAN_IR) Message stored to Dedicated Receive Buffer Mask */
#define MCAN_IR_DRX(value)                    (MCAN_IR_DRX_Msk & ((value) << MCAN_IR_DRX_Pos))
#define MCAN_IR_ELO_Pos                       (22U)                                          /**< (MCAN_IR) Error Logging Overflow Position */
#define MCAN_IR_ELO_Msk                       (0x1U << MCAN_IR_ELO_Pos)                      /**< (MCAN_IR) Error Logging Overflow Mask */
#define MCAN_IR_ELO(value)                    (MCAN_IR_ELO_Msk & ((value) << MCAN_IR_ELO_Pos))
#define MCAN_IR_EP_Pos                        (23U)                                          /**< (MCAN_IR) Error Passive Position */
#define MCAN_IR_EP_Msk                        (0x1U << MCAN_IR_EP_Pos)                       /**< (MCAN_IR) Error Passive Mask */
#define MCAN_IR_EP(value)                     (MCAN_IR_EP_Msk & ((value) << MCAN_IR_EP_Pos))
#define MCAN_IR_EW_Pos                        (24U)                                          /**< (MCAN_IR) Warning Status Position */
#define MCAN_IR_EW_Msk                        (0x1U << MCAN_IR_EW_Pos)                       /**< (MCAN_IR) Warning Status Mask */
#define MCAN_IR_EW(value)                     (MCAN_IR_EW_Msk & ((value) << MCAN_IR_EW_Pos))
#define MCAN_IR_BO_Pos                        (25U)                                          /**< (MCAN_IR) Bus_Off Status Position */
#define MCAN_IR_BO_Msk                        (0x1U << MCAN_IR_BO_Pos)                       /**< (MCAN_IR) Bus_Off Status Mask */
#define MCAN_IR_BO(value)                     (MCAN_IR_BO_Msk & ((value) << MCAN_IR_BO_Pos))
#define MCAN_IR_WDI_Pos                       (26U)                                          /**< (MCAN_IR) Watchdog Interrupt Position */
#define MCAN_IR_WDI_Msk                       (0x1U << MCAN_IR_WDI_Pos)                      /**< (MCAN_IR) Watchdog Interrupt Mask */
#define MCAN_IR_WDI(value)                    (MCAN_IR_WDI_Msk & ((value) << MCAN_IR_WDI_Pos))
#define MCAN_IR_PEA_Pos                       (27U)                                          /**< (MCAN_IR) Protocol Error in Arbitration Phase Position */
#define MCAN_IR_PEA_Msk                       (0x1U << MCAN_IR_PEA_Pos)                      /**< (MCAN_IR) Protocol Error in Arbitration Phase Mask */
#define MCAN_IR_PEA(value)                    (MCAN_IR_PEA_Msk & ((value) << MCAN_IR_PEA_Pos))
#define MCAN_IR_PED_Pos                       (28U)                                          /**< (MCAN_IR) Protocol Error in Data Phase Position */
#define MCAN_IR_PED_Msk                       (0x1U << MCAN_IR_PED_Pos)                      /**< (MCAN_IR) Protocol Error in Data Phase Mask */
#define MCAN_IR_PED(value)                    (MCAN_IR_PED_Msk & ((value) << MCAN_IR_PED_Pos))
#define MCAN_IR_ARA_Pos                       (29U)                                          /**< (MCAN_IR) Access to Reserved Address Position */
#define MCAN_IR_ARA_Msk                       (0x1U << MCAN_IR_ARA_Pos)                      /**< (MCAN_IR) Access to Reserved Address Mask */
#define MCAN_IR_ARA(value)                    (MCAN_IR_ARA_Msk & ((value) << MCAN_IR_ARA_Pos))
#define MCAN_IR_Msk                           (0x3FCFFFFFUL)                                 /**< (MCAN_IR) Register Mask  */

/* -------- MCAN_IE : (MCAN Offset: 0x54) (R/W 32) Interrupt Enable Register -------- */
#define MCAN_IE_RF0NE_Pos                     (0U)                                           /**< (MCAN_IE) Receive FIFO 0 New Message Interrupt Enable Position */
#define MCAN_IE_RF0NE_Msk                     (0x1U << MCAN_IE_RF0NE_Pos)                    /**< (MCAN_IE) Receive FIFO 0 New Message Interrupt Enable Mask */
#define MCAN_IE_RF0NE(value)                  (MCAN_IE_RF0NE_Msk & ((value) << MCAN_IE_RF0NE_Pos))
#define MCAN_IE_RF0WE_Pos                     (1U)                                           /**< (MCAN_IE) Receive FIFO 0 Watermark Reached Interrupt Enable Position */
#define MCAN_IE_RF0WE_Msk                     (0x1U << MCAN_IE_RF0WE_Pos)                    /**< (MCAN_IE) Receive FIFO 0 Watermark Reached Interrupt Enable Mask */
#define MCAN_IE_RF0WE(value)                  (MCAN_IE_RF0WE_Msk & ((value) << MCAN_IE_RF0WE_Pos))
#define MCAN_IE_RF0FE_Pos                     (2U)                                           /**< (MCAN_IE) Receive FIFO 0 Full Interrupt Enable Position */
#define MCAN_IE_RF0FE_Msk                     (0x1U << MCAN_IE_RF0FE_Pos)                    /**< (MCAN_IE) Receive FIFO 0 Full Interrupt Enable Mask */
#define MCAN_IE_RF0FE(value)                  (MCAN_IE_RF0FE_Msk & ((value) << MCAN_IE_RF0FE_Pos))
#define MCAN_IE_RF0LE_Pos                     (3U)                                           /**< (MCAN_IE) Receive FIFO 0 Message Lost Interrupt Enable Position */
#define MCAN_IE_RF0LE_Msk                     (0x1U << MCAN_IE_RF0LE_Pos)                    /**< (MCAN_IE) Receive FIFO 0 Message Lost Interrupt Enable Mask */
#define MCAN_IE_RF0LE(value)                  (MCAN_IE_RF0LE_Msk & ((value) << MCAN_IE_RF0LE_Pos))
#define MCAN_IE_RF1NE_Pos                     (4U)                                           /**< (MCAN_IE) Receive FIFO 1 New Message Interrupt Enable Position */
#define MCAN_IE_RF1NE_Msk                     (0x1U << MCAN_IE_RF1NE_Pos)                    /**< (MCAN_IE) Receive FIFO 1 New Message Interrupt Enable Mask */
#define MCAN_IE_RF1NE(value)                  (MCAN_IE_RF1NE_Msk & ((value) << MCAN_IE_RF1NE_Pos))
#define MCAN_IE_RF1WE_Pos                     (5U)                                           /**< (MCAN_IE) Receive FIFO 1 Watermark Reached Interrupt Enable Position */
#define MCAN_IE_RF1WE_Msk                     (0x1U << MCAN_IE_RF1WE_Pos)                    /**< (MCAN_IE) Receive FIFO 1 Watermark Reached Interrupt Enable Mask */
#define MCAN_IE_RF1WE(value)                  (MCAN_IE_RF1WE_Msk & ((value) << MCAN_IE_RF1WE_Pos))
#define MCAN_IE_RF1FE_Pos                     (6U)                                           /**< (MCAN_IE) Receive FIFO 1 Full Interrupt Enable Position */
#define MCAN_IE_RF1FE_Msk                     (0x1U << MCAN_IE_RF1FE_Pos)                    /**< (MCAN_IE) Receive FIFO 1 Full Interrupt Enable Mask */
#define MCAN_IE_RF1FE(value)                  (MCAN_IE_RF1FE_Msk & ((value) << MCAN_IE_RF1FE_Pos))
#define MCAN_IE_RF1LE_Pos                     (7U)                                           /**< (MCAN_IE) Receive FIFO 1 Message Lost Interrupt Enable Position */
#define MCAN_IE_RF1LE_Msk                     (0x1U << MCAN_IE_RF1LE_Pos)                    /**< (MCAN_IE) Receive FIFO 1 Message Lost Interrupt Enable Mask */
#define MCAN_IE_RF1LE(value)                  (MCAN_IE_RF1LE_Msk & ((value) << MCAN_IE_RF1LE_Pos))
#define MCAN_IE_HPME_Pos                      (8U)                                           /**< (MCAN_IE) High Priority Message Interrupt Enable Position */
#define MCAN_IE_HPME_Msk                      (0x1U << MCAN_IE_HPME_Pos)                     /**< (MCAN_IE) High Priority Message Interrupt Enable Mask */
#define MCAN_IE_HPME(value)                   (MCAN_IE_HPME_Msk & ((value) << MCAN_IE_HPME_Pos))
#define MCAN_IE_TCE_Pos                       (9U)                                           /**< (MCAN_IE) Transmission Completed Interrupt Enable Position */
#define MCAN_IE_TCE_Msk                       (0x1U << MCAN_IE_TCE_Pos)                      /**< (MCAN_IE) Transmission Completed Interrupt Enable Mask */
#define MCAN_IE_TCE(value)                    (MCAN_IE_TCE_Msk & ((value) << MCAN_IE_TCE_Pos))
#define MCAN_IE_TCFE_Pos                      (10U)                                          /**< (MCAN_IE) Transmission Cancellation Finished Interrupt Enable Position */
#define MCAN_IE_TCFE_Msk                      (0x1U << MCAN_IE_TCFE_Pos)                     /**< (MCAN_IE) Transmission Cancellation Finished Interrupt Enable Mask */
#define MCAN_IE_TCFE(value)                   (MCAN_IE_TCFE_Msk & ((value) << MCAN_IE_TCFE_Pos))
#define MCAN_IE_TFEE_Pos                      (11U)                                          /**< (MCAN_IE) Tx FIFO Empty Interrupt Enable Position */
#define MCAN_IE_TFEE_Msk                      (0x1U << MCAN_IE_TFEE_Pos)                     /**< (MCAN_IE) Tx FIFO Empty Interrupt Enable Mask */
#define MCAN_IE_TFEE(value)                   (MCAN_IE_TFEE_Msk & ((value) << MCAN_IE_TFEE_Pos))
#define MCAN_IE_TEFNE_Pos                     (12U)                                          /**< (MCAN_IE) Tx Event FIFO New Entry Interrupt Enable Position */
#define MCAN_IE_TEFNE_Msk                     (0x1U << MCAN_IE_TEFNE_Pos)                    /**< (MCAN_IE) Tx Event FIFO New Entry Interrupt Enable Mask */
#define MCAN_IE_TEFNE(value)                  (MCAN_IE_TEFNE_Msk & ((value) << MCAN_IE_TEFNE_Pos))
#define MCAN_IE_TEFWE_Pos                     (13U)                                          /**< (MCAN_IE) Tx Event FIFO Watermark Reached Interrupt Enable Position */
#define MCAN_IE_TEFWE_Msk                     (0x1U << MCAN_IE_TEFWE_Pos)                    /**< (MCAN_IE) Tx Event FIFO Watermark Reached Interrupt Enable Mask */
#define MCAN_IE_TEFWE(value)                  (MCAN_IE_TEFWE_Msk & ((value) << MCAN_IE_TEFWE_Pos))
#define MCAN_IE_TEFFE_Pos                     (14U)                                          /**< (MCAN_IE) Tx Event FIFO Full Interrupt Enable Position */
#define MCAN_IE_TEFFE_Msk                     (0x1U << MCAN_IE_TEFFE_Pos)                    /**< (MCAN_IE) Tx Event FIFO Full Interrupt Enable Mask */
#define MCAN_IE_TEFFE(value)                  (MCAN_IE_TEFFE_Msk & ((value) << MCAN_IE_TEFFE_Pos))
#define MCAN_IE_TEFLE_Pos                     (15U)                                          /**< (MCAN_IE) Tx Event FIFO Event Lost Interrupt Enable Position */
#define MCAN_IE_TEFLE_Msk                     (0x1U << MCAN_IE_TEFLE_Pos)                    /**< (MCAN_IE) Tx Event FIFO Event Lost Interrupt Enable Mask */
#define MCAN_IE_TEFLE(value)                  (MCAN_IE_TEFLE_Msk & ((value) << MCAN_IE_TEFLE_Pos))
#define MCAN_IE_TSWE_Pos                      (16U)                                          /**< (MCAN_IE) Timestamp Wraparound Interrupt Enable Position */
#define MCAN_IE_TSWE_Msk                      (0x1U << MCAN_IE_TSWE_Pos)                     /**< (MCAN_IE) Timestamp Wraparound Interrupt Enable Mask */
#define MCAN_IE_TSWE(value)                   (MCAN_IE_TSWE_Msk & ((value) << MCAN_IE_TSWE_Pos))
#define MCAN_IE_MRAFE_Pos                     (17U)                                          /**< (MCAN_IE) Message RAM Access Failure Interrupt Enable Position */
#define MCAN_IE_MRAFE_Msk                     (0x1U << MCAN_IE_MRAFE_Pos)                    /**< (MCAN_IE) Message RAM Access Failure Interrupt Enable Mask */
#define MCAN_IE_MRAFE(value)                  (MCAN_IE_MRAFE_Msk & ((value) << MCAN_IE_MRAFE_Pos))
#define MCAN_IE_TOOE_Pos                      (18U)                                          /**< (MCAN_IE) Timeout Occurred Interrupt Enable Position */
#define MCAN_IE_TOOE_Msk                      (0x1U << MCAN_IE_TOOE_Pos)                     /**< (MCAN_IE) Timeout Occurred Interrupt Enable Mask */
#define MCAN_IE_TOOE(value)                   (MCAN_IE_TOOE_Msk & ((value) << MCAN_IE_TOOE_Pos))
#define MCAN_IE_DRXE_Pos                      (19U)                                          /**< (MCAN_IE) Message stored to Dedicated Receive Buffer Interrupt Enable Position */
#define MCAN_IE_DRXE_Msk                      (0x1U << MCAN_IE_DRXE_Pos)                     /**< (MCAN_IE) Message stored to Dedicated Receive Buffer Interrupt Enable Mask */
#define MCAN_IE_DRXE(value)                   (MCAN_IE_DRXE_Msk & ((value) << MCAN_IE_DRXE_Pos))
#define MCAN_IE_ELOE_Pos                      (22U)                                          /**< (MCAN_IE) Error Logging Overflow Interrupt Enable Position */
#define MCAN_IE_ELOE_Msk                      (0x1U << MCAN_IE_ELOE_Pos)                     /**< (MCAN_IE) Error Logging Overflow Interrupt Enable Mask */
#define MCAN_IE_ELOE(value)                   (MCAN_IE_ELOE_Msk & ((value) << MCAN_IE_ELOE_Pos))
#define MCAN_IE_EPE_Pos                       (23U)                                          /**< (MCAN_IE) Error Passive Interrupt Enable Position */
#define MCAN_IE_EPE_Msk                       (0x1U << MCAN_IE_EPE_Pos)                      /**< (MCAN_IE) Error Passive Interrupt Enable Mask */
#define MCAN_IE_EPE(value)                    (MCAN_IE_EPE_Msk & ((value) << MCAN_IE_EPE_Pos))
#define MCAN_IE_EWE_Pos                       (24U)                                          /**< (MCAN_IE) Warning Status Interrupt Enable Position */
#define MCAN_IE_EWE_Msk                       (0x1U << MCAN_IE_EWE_Pos)                      /**< (MCAN_IE) Warning Status Interrupt Enable Mask */
#define MCAN_IE_EWE(value)                    (MCAN_IE_EWE_Msk & ((value) << MCAN_IE_EWE_Pos))
#define MCAN_IE_BOE_Pos                       (25U)                                          /**< (MCAN_IE) Bus_Off Status Interrupt Enable Position */
#define MCAN_IE_BOE_Msk                       (0x1U << MCAN_IE_BOE_Pos)                      /**< (MCAN_IE) Bus_Off Status Interrupt Enable Mask */
#define MCAN_IE_BOE(value)                    (MCAN_IE_BOE_Msk & ((value) << MCAN_IE_BOE_Pos))
#define MCAN_IE_WDIE_Pos                      (26U)                                          /**< (MCAN_IE) Watchdog Interrupt Enable Position */
#define MCAN_IE_WDIE_Msk                      (0x1U << MCAN_IE_WDIE_Pos)                     /**< (MCAN_IE) Watchdog Interrupt Enable Mask */
#define MCAN_IE_WDIE(value)                   (MCAN_IE_WDIE_Msk & ((value) << MCAN_IE_WDIE_Pos))
#define MCAN_IE_PEAE_Pos                      (27U)                                          /**< (MCAN_IE) Protocol Error in Arbitration Phase Enable Position */
#define MCAN_IE_PEAE_Msk                      (0x1U << MCAN_IE_PEAE_Pos)                     /**< (MCAN_IE) Protocol Error in Arbitration Phase Enable Mask */
#define MCAN_IE_PEAE(value)                   (MCAN_IE_PEAE_Msk & ((value) << MCAN_IE_PEAE_Pos))
#define MCAN_IE_PEDE_Pos                      (28U)                                          /**< (MCAN_IE) Protocol Error in Data Phase Enable Position */
#define MCAN_IE_PEDE_Msk                      (0x1U << MCAN_IE_PEDE_Pos)                     /**< (MCAN_IE) Protocol Error in Data Phase Enable Mask */
#define MCAN_IE_PEDE(value)                   (MCAN_IE_PEDE_Msk & ((value) << MCAN_IE_PEDE_Pos))
#define MCAN_IE_ARAE_Pos                      (29U)                                          /**< (MCAN_IE) Access to Reserved Address Enable Position */
#define MCAN_IE_ARAE_Msk                      (0x1U << MCAN_IE_ARAE_Pos)                     /**< (MCAN_IE) Access to Reserved Address Enable Mask */
#define MCAN_IE_ARAE(value)                   (MCAN_IE_ARAE_Msk & ((value) << MCAN_IE_ARAE_Pos))
#define MCAN_IE_Msk                           (0x3FCFFFFFUL)                                 /**< (MCAN_IE) Register Mask  */

/* -------- MCAN_ILS : (MCAN Offset: 0x58) (R/W 32) Interrupt Line Select Register -------- */
#define MCAN_ILS_RF0NL_Pos                    (0U)                                           /**< (MCAN_ILS) Receive FIFO 0 New Message Interrupt Line Position */
#define MCAN_ILS_RF0NL_Msk                    (0x1U << MCAN_ILS_RF0NL_Pos)                   /**< (MCAN_ILS) Receive FIFO 0 New Message Interrupt Line Mask */
#define MCAN_ILS_RF0NL(value)                 (MCAN_ILS_RF0NL_Msk & ((value) << MCAN_ILS_RF0NL_Pos))
#define MCAN_ILS_RF0WL_Pos                    (1U)                                           /**< (MCAN_ILS) Receive FIFO 0 Watermark Reached Interrupt Line Position */
#define MCAN_ILS_RF0WL_Msk                    (0x1U << MCAN_ILS_RF0WL_Pos)                   /**< (MCAN_ILS) Receive FIFO 0 Watermark Reached Interrupt Line Mask */
#define MCAN_ILS_RF0WL(value)                 (MCAN_ILS_RF0WL_Msk & ((value) << MCAN_ILS_RF0WL_Pos))
#define MCAN_ILS_RF0FL_Pos                    (2U)                                           /**< (MCAN_ILS) Receive FIFO 0 Full Interrupt Line Position */
#define MCAN_ILS_RF0FL_Msk                    (0x1U << MCAN_ILS_RF0FL_Pos)                   /**< (MCAN_ILS) Receive FIFO 0 Full Interrupt Line Mask */
#define MCAN_ILS_RF0FL(value)                 (MCAN_ILS_RF0FL_Msk & ((value) << MCAN_ILS_RF0FL_Pos))
#define MCAN_ILS_RF0LL_Pos                    (3U)                                           /**< (MCAN_ILS) Receive FIFO 0 Message Lost Interrupt Line Position */
#define MCAN_ILS_RF0LL_Msk                    (0x1U << MCAN_ILS_RF0LL_Pos)                   /**< (MCAN_ILS) Receive FIFO 0 Message Lost Interrupt Line Mask */
#define MCAN_ILS_RF0LL(value)                 (MCAN_ILS_RF0LL_Msk & ((value) << MCAN_ILS_RF0LL_Pos))
#define MCAN_ILS_RF1NL_Pos                    (4U)                                           /**< (MCAN_ILS) Receive FIFO 1 New Message Interrupt Line Position */
#define MCAN_ILS_RF1NL_Msk                    (0x1U << MCAN_ILS_RF1NL_Pos)                   /**< (MCAN_ILS) Receive FIFO 1 New Message Interrupt Line Mask */
#define MCAN_ILS_RF1NL(value)                 (MCAN_ILS_RF1NL_Msk & ((value) << MCAN_ILS_RF1NL_Pos))
#define MCAN_ILS_RF1WL_Pos                    (5U)                                           /**< (MCAN_ILS) Receive FIFO 1 Watermark Reached Interrupt Line Position */
#define MCAN_ILS_RF1WL_Msk                    (0x1U << MCAN_ILS_RF1WL_Pos)                   /**< (MCAN_ILS) Receive FIFO 1 Watermark Reached Interrupt Line Mask */
#define MCAN_ILS_RF1WL(value)                 (MCAN_ILS_RF1WL_Msk & ((value) << MCAN_ILS_RF1WL_Pos))
#define MCAN_ILS_RF1FL_Pos                    (6U)                                           /**< (MCAN_ILS) Receive FIFO 1 Full Interrupt Line Position */
#define MCAN_ILS_RF1FL_Msk                    (0x1U << MCAN_ILS_RF1FL_Pos)                   /**< (MCAN_ILS) Receive FIFO 1 Full Interrupt Line Mask */
#define MCAN_ILS_RF1FL(value)                 (MCAN_ILS_RF1FL_Msk & ((value) << MCAN_ILS_RF1FL_Pos))
#define MCAN_ILS_RF1LL_Pos                    (7U)                                           /**< (MCAN_ILS) Receive FIFO 1 Message Lost Interrupt Line Position */
#define MCAN_ILS_RF1LL_Msk                    (0x1U << MCAN_ILS_RF1LL_Pos)                   /**< (MCAN_ILS) Receive FIFO 1 Message Lost Interrupt Line Mask */
#define MCAN_ILS_RF1LL(value)                 (MCAN_ILS_RF1LL_Msk & ((value) << MCAN_ILS_RF1LL_Pos))
#define MCAN_ILS_HPML_Pos                     (8U)                                           /**< (MCAN_ILS) High Priority Message Interrupt Line Position */
#define MCAN_ILS_HPML_Msk                     (0x1U << MCAN_ILS_HPML_Pos)                    /**< (MCAN_ILS) High Priority Message Interrupt Line Mask */
#define MCAN_ILS_HPML(value)                  (MCAN_ILS_HPML_Msk & ((value) << MCAN_ILS_HPML_Pos))
#define MCAN_ILS_TCL_Pos                      (9U)                                           /**< (MCAN_ILS) Transmission Completed Interrupt Line Position */
#define MCAN_ILS_TCL_Msk                      (0x1U << MCAN_ILS_TCL_Pos)                     /**< (MCAN_ILS) Transmission Completed Interrupt Line Mask */
#define MCAN_ILS_TCL(value)                   (MCAN_ILS_TCL_Msk & ((value) << MCAN_ILS_TCL_Pos))
#define MCAN_ILS_TCFL_Pos                     (10U)                                          /**< (MCAN_ILS) Transmission Cancellation Finished Interrupt Line Position */
#define MCAN_ILS_TCFL_Msk                     (0x1U << MCAN_ILS_TCFL_Pos)                    /**< (MCAN_ILS) Transmission Cancellation Finished Interrupt Line Mask */
#define MCAN_ILS_TCFL(value)                  (MCAN_ILS_TCFL_Msk & ((value) << MCAN_ILS_TCFL_Pos))
#define MCAN_ILS_TFEL_Pos                     (11U)                                          /**< (MCAN_ILS) Tx FIFO Empty Interrupt Line Position */
#define MCAN_ILS_TFEL_Msk                     (0x1U << MCAN_ILS_TFEL_Pos)                    /**< (MCAN_ILS) Tx FIFO Empty Interrupt Line Mask */
#define MCAN_ILS_TFEL(value)                  (MCAN_ILS_TFEL_Msk & ((value) << MCAN_ILS_TFEL_Pos))
#define MCAN_ILS_TEFNL_Pos                    (12U)                                          /**< (MCAN_ILS) Tx Event FIFO New Entry Interrupt Line Position */
#define MCAN_ILS_TEFNL_Msk                    (0x1U << MCAN_ILS_TEFNL_Pos)                   /**< (MCAN_ILS) Tx Event FIFO New Entry Interrupt Line Mask */
#define MCAN_ILS_TEFNL(value)                 (MCAN_ILS_TEFNL_Msk & ((value) << MCAN_ILS_TEFNL_Pos))
#define MCAN_ILS_TEFWL_Pos                    (13U)                                          /**< (MCAN_ILS) Tx Event FIFO Watermark Reached Interrupt Line Position */
#define MCAN_ILS_TEFWL_Msk                    (0x1U << MCAN_ILS_TEFWL_Pos)                   /**< (MCAN_ILS) Tx Event FIFO Watermark Reached Interrupt Line Mask */
#define MCAN_ILS_TEFWL(value)                 (MCAN_ILS_TEFWL_Msk & ((value) << MCAN_ILS_TEFWL_Pos))
#define MCAN_ILS_TEFFL_Pos                    (14U)                                          /**< (MCAN_ILS) Tx Event FIFO Full Interrupt Line Position */
#define MCAN_ILS_TEFFL_Msk                    (0x1U << MCAN_ILS_TEFFL_Pos)                   /**< (MCAN_ILS) Tx Event FIFO Full Interrupt Line Mask */
#define MCAN_ILS_TEFFL(value)                 (MCAN_ILS_TEFFL_Msk & ((value) << MCAN_ILS_TEFFL_Pos))
#define MCAN_ILS_TEFLL_Pos                    (15U)                                          /**< (MCAN_ILS) Tx Event FIFO Event Lost Interrupt Line Position */
#define MCAN_ILS_TEFLL_Msk                    (0x1U << MCAN_ILS_TEFLL_Pos)                   /**< (MCAN_ILS) Tx Event FIFO Event Lost Interrupt Line Mask */
#define MCAN_ILS_TEFLL(value)                 (MCAN_ILS_TEFLL_Msk & ((value) << MCAN_ILS_TEFLL_Pos))
#define MCAN_ILS_TSWL_Pos                     (16U)                                          /**< (MCAN_ILS) Timestamp Wraparound Interrupt Line Position */
#define MCAN_ILS_TSWL_Msk                     (0x1U << MCAN_ILS_TSWL_Pos)                    /**< (MCAN_ILS) Timestamp Wraparound Interrupt Line Mask */
#define MCAN_ILS_TSWL(value)                  (MCAN_ILS_TSWL_Msk & ((value) << MCAN_ILS_TSWL_Pos))
#define MCAN_ILS_MRAFL_Pos                    (17U)                                          /**< (MCAN_ILS) Message RAM Access Failure Interrupt Line Position */
#define MCAN_ILS_MRAFL_Msk                    (0x1U << MCAN_ILS_MRAFL_Pos)                   /**< (MCAN_ILS) Message RAM Access Failure Interrupt Line Mask */
#define MCAN_ILS_MRAFL(value)                 (MCAN_ILS_MRAFL_Msk & ((value) << MCAN_ILS_MRAFL_Pos))
#define MCAN_ILS_TOOL_Pos                     (18U)                                          /**< (MCAN_ILS) Timeout Occurred Interrupt Line Position */
#define MCAN_ILS_TOOL_Msk                     (0x1U << MCAN_ILS_TOOL_Pos)                    /**< (MCAN_ILS) Timeout Occurred Interrupt Line Mask */
#define MCAN_ILS_TOOL(value)                  (MCAN_ILS_TOOL_Msk & ((value) << MCAN_ILS_TOOL_Pos))
#define MCAN_ILS_DRXL_Pos                     (19U)                                          /**< (MCAN_ILS) Message stored to Dedicated Receive Buffer Interrupt Line Position */
#define MCAN_ILS_DRXL_Msk                     (0x1U << MCAN_ILS_DRXL_Pos)                    /**< (MCAN_ILS) Message stored to Dedicated Receive Buffer Interrupt Line Mask */
#define MCAN_ILS_DRXL(value)                  (MCAN_ILS_DRXL_Msk & ((value) << MCAN_ILS_DRXL_Pos))
#define MCAN_ILS_ELOL_Pos                     (22U)                                          /**< (MCAN_ILS) Error Logging Overflow Interrupt Line Position */
#define MCAN_ILS_ELOL_Msk                     (0x1U << MCAN_ILS_ELOL_Pos)                    /**< (MCAN_ILS) Error Logging Overflow Interrupt Line Mask */
#define MCAN_ILS_ELOL(value)                  (MCAN_ILS_ELOL_Msk & ((value) << MCAN_ILS_ELOL_Pos))
#define MCAN_ILS_EPL_Pos                      (23U)                                          /**< (MCAN_ILS) Error Passive Interrupt Line Position */
#define MCAN_ILS_EPL_Msk                      (0x1U << MCAN_ILS_EPL_Pos)                     /**< (MCAN_ILS) Error Passive Interrupt Line Mask */
#define MCAN_ILS_EPL(value)                   (MCAN_ILS_EPL_Msk & ((value) << MCAN_ILS_EPL_Pos))
#define MCAN_ILS_EWL_Pos                      (24U)                                          /**< (MCAN_ILS) Warning Status Interrupt Line Position */
#define MCAN_ILS_EWL_Msk                      (0x1U << MCAN_ILS_EWL_Pos)                     /**< (MCAN_ILS) Warning Status Interrupt Line Mask */
#define MCAN_ILS_EWL(value)                   (MCAN_ILS_EWL_Msk & ((value) << MCAN_ILS_EWL_Pos))
#define MCAN_ILS_BOL_Pos                      (25U)                                          /**< (MCAN_ILS) Bus_Off Status Interrupt Line Position */
#define MCAN_ILS_BOL_Msk                      (0x1U << MCAN_ILS_BOL_Pos)                     /**< (MCAN_ILS) Bus_Off Status Interrupt Line Mask */
#define MCAN_ILS_BOL(value)                   (MCAN_ILS_BOL_Msk & ((value) << MCAN_ILS_BOL_Pos))
#define MCAN_ILS_WDIL_Pos                     (26U)                                          /**< (MCAN_ILS) Watchdog Interrupt Line Position */
#define MCAN_ILS_WDIL_Msk                     (0x1U << MCAN_ILS_WDIL_Pos)                    /**< (MCAN_ILS) Watchdog Interrupt Line Mask */
#define MCAN_ILS_WDIL(value)                  (MCAN_ILS_WDIL_Msk & ((value) << MCAN_ILS_WDIL_Pos))
#define MCAN_ILS_PEAL_Pos                     (27U)                                          /**< (MCAN_ILS) Protocol Error in Arbitration Phase Line Position */
#define MCAN_ILS_PEAL_Msk                     (0x1U << MCAN_ILS_PEAL_Pos)                    /**< (MCAN_ILS) Protocol Error in Arbitration Phase Line Mask */
#define MCAN_ILS_PEAL(value)                  (MCAN_ILS_PEAL_Msk & ((value) << MCAN_ILS_PEAL_Pos))
#define MCAN_ILS_PEDL_Pos                     (28U)                                          /**< (MCAN_ILS) Protocol Error in Data Phase Line Position */
#define MCAN_ILS_PEDL_Msk                     (0x1U << MCAN_ILS_PEDL_Pos)                    /**< (MCAN_ILS) Protocol Error in Data Phase Line Mask */
#define MCAN_ILS_PEDL(value)                  (MCAN_ILS_PEDL_Msk & ((value) << MCAN_ILS_PEDL_Pos))
#define MCAN_ILS_ARAL_Pos                     (29U)                                          /**< (MCAN_ILS) Access to Reserved Address Line Position */
#define MCAN_ILS_ARAL_Msk                     (0x1U << MCAN_ILS_ARAL_Pos)                    /**< (MCAN_ILS) Access to Reserved Address Line Mask */
#define MCAN_ILS_ARAL(value)                  (MCAN_ILS_ARAL_Msk & ((value) << MCAN_ILS_ARAL_Pos))
#define MCAN_ILS_Msk                          (0x3FCFFFFFUL)                                 /**< (MCAN_ILS) Register Mask  */

/* -------- MCAN_ILE : (MCAN Offset: 0x5C) (R/W 32) Interrupt Line Enable Register -------- */
#define MCAN_ILE_EINT0_Pos                    (0U)                                           /**< (MCAN_ILE) Enable Interrupt Line 0 Position */
#define MCAN_ILE_EINT0_Msk                    (0x1U << MCAN_ILE_EINT0_Pos)                   /**< (MCAN_ILE) Enable Interrupt Line 0 Mask */
#define MCAN_ILE_EINT0(value)                 (MCAN_ILE_EINT0_Msk & ((value) << MCAN_ILE_EINT0_Pos))
#define MCAN_ILE_EINT1_Pos                    (1U)                                           /**< (MCAN_ILE) Enable Interrupt Line 1 Position */
#define MCAN_ILE_EINT1_Msk                    (0x1U << MCAN_ILE_EINT1_Pos)                   /**< (MCAN_ILE) Enable Interrupt Line 1 Mask */
#define MCAN_ILE_EINT1(value)                 (MCAN_ILE_EINT1_Msk & ((value) << MCAN_ILE_EINT1_Pos))
#define MCAN_ILE_Msk                          (0x00000003UL)                                 /**< (MCAN_ILE) Register Mask  */

/* -------- MCAN_GFC : (MCAN Offset: 0x80) (R/W 32) Global Filter Configuration Register -------- */
#define MCAN_GFC_RRFE_Pos                     (0U)                                           /**< (MCAN_GFC) Reject Remote Frames Extended Position */
#define MCAN_GFC_RRFE_Msk                     (0x1U << MCAN_GFC_RRFE_Pos)                    /**< (MCAN_GFC) Reject Remote Frames Extended Mask */
#define MCAN_GFC_RRFE(value)                  (MCAN_GFC_RRFE_Msk & ((value) << MCAN_GFC_RRFE_Pos))
#define   MCAN_GFC_RRFE_FILTER_Val            (0U)                                           /**< (MCAN_GFC) Filter remote frames with 29-bit extended IDs.  */
#define   MCAN_GFC_RRFE_REJECT_Val            (1U)                                           /**< (MCAN_GFC) Reject all remote frames with 29-bit extended IDs.  */
#define MCAN_GFC_RRFE_FILTER                  (MCAN_GFC_RRFE_FILTER_Val << MCAN_GFC_RRFE_Pos) /**< (MCAN_GFC) Filter remote frames with 29-bit extended IDs. Position  */
#define MCAN_GFC_RRFE_REJECT                  (MCAN_GFC_RRFE_REJECT_Val << MCAN_GFC_RRFE_Pos) /**< (MCAN_GFC) Reject all remote frames with 29-bit extended IDs. Position  */
#define MCAN_GFC_RRFS_Pos                     (1U)                                           /**< (MCAN_GFC) Reject Remote Frames Standard Position */
#define MCAN_GFC_RRFS_Msk                     (0x1U << MCAN_GFC_RRFS_Pos)                    /**< (MCAN_GFC) Reject Remote Frames Standard Mask */
#define MCAN_GFC_RRFS(value)                  (MCAN_GFC_RRFS_Msk & ((value) << MCAN_GFC_RRFS_Pos))
#define   MCAN_GFC_RRFS_FILTER_Val            (0U)                                           /**< (MCAN_GFC) Filter remote frames with 11-bit standard IDs.  */
#define   MCAN_GFC_RRFS_REJECT_Val            (1U)                                           /**< (MCAN_GFC) Reject all remote frames with 11-bit standard IDs.  */
#define MCAN_GFC_RRFS_FILTER                  (MCAN_GFC_RRFS_FILTER_Val << MCAN_GFC_RRFS_Pos) /**< (MCAN_GFC) Filter remote frames with 11-bit standard IDs. Position  */
#define MCAN_GFC_RRFS_REJECT                  (MCAN_GFC_RRFS_REJECT_Val << MCAN_GFC_RRFS_Pos) /**< (MCAN_GFC) Reject all remote frames with 11-bit standard IDs. Position  */
#define MCAN_GFC_ANFE_Pos                     (2U)                                           /**< (MCAN_GFC) Accept Non-matching Frames Extended Position */
#define MCAN_GFC_ANFE_Msk                     (0x3U << MCAN_GFC_ANFE_Pos)                    /**< (MCAN_GFC) Accept Non-matching Frames Extended Mask */
#define MCAN_GFC_ANFE(value)                  (MCAN_GFC_ANFE_Msk & ((value) << MCAN_GFC_ANFE_Pos))
#define   MCAN_GFC_ANFE_RX_FIFO_0_Val         (0U)                                           /**< (MCAN_GFC) Accept in Rx FIFO 0  */
#define   MCAN_GFC_ANFE_RX_FIFO_1_Val         (1U)                                           /**< (MCAN_GFC) Accept in Rx FIFO 1  */
#define MCAN_GFC_ANFE_RX_FIFO_0               (MCAN_GFC_ANFE_RX_FIFO_0_Val << MCAN_GFC_ANFE_Pos) /**< (MCAN_GFC) Accept in Rx FIFO 0 Position  */
#define MCAN_GFC_ANFE_RX_FIFO_1               (MCAN_GFC_ANFE_RX_FIFO_1_Val << MCAN_GFC_ANFE_Pos) /**< (MCAN_GFC) Accept in Rx FIFO 1 Position  */
#define MCAN_GFC_ANFS_Pos                     (4U)                                           /**< (MCAN_GFC) Accept Non-matching Frames Standard Position */
#define MCAN_GFC_ANFS_Msk                     (0x3U << MCAN_GFC_ANFS_Pos)                    /**< (MCAN_GFC) Accept Non-matching Frames Standard Mask */
#define MCAN_GFC_ANFS(value)                  (MCAN_GFC_ANFS_Msk & ((value) << MCAN_GFC_ANFS_Pos))
#define   MCAN_GFC_ANFS_RX_FIFO_0_Val         (0U)                                           /**< (MCAN_GFC) Accept in Rx FIFO 0  */
#define   MCAN_GFC_ANFS_RX_FIFO_1_Val         (1U)                                           /**< (MCAN_GFC) Accept in Rx FIFO 1  */
#define MCAN_GFC_ANFS_RX_FIFO_0               (MCAN_GFC_ANFS_RX_FIFO_0_Val << MCAN_GFC_ANFS_Pos) /**< (MCAN_GFC) Accept in Rx FIFO 0 Position  */
#define MCAN_GFC_ANFS_RX_FIFO_1               (MCAN_GFC_ANFS_RX_FIFO_1_Val << MCAN_GFC_ANFS_Pos) /**< (MCAN_GFC) Accept in Rx FIFO 1 Position  */
#define MCAN_GFC_Msk                          (0x0000003FUL)                                 /**< (MCAN_GFC) Register Mask  */

/* -------- MCAN_SIDFC : (MCAN Offset: 0x84) (R/W 32) Standard ID Filter Configuration Register -------- */
#define MCAN_SIDFC_FLSSA_Pos                  (2U)                                           /**< (MCAN_SIDFC) Filter List Standard Start Address Position */
#define MCAN_SIDFC_FLSSA_Msk                  (0x3FFFU << MCAN_SIDFC_FLSSA_Pos)              /**< (MCAN_SIDFC) Filter List Standard Start Address Mask */
#define MCAN_SIDFC_FLSSA(value)               (MCAN_SIDFC_FLSSA_Msk & ((value) << MCAN_SIDFC_FLSSA_Pos))
#define MCAN_SIDFC_LSS_Pos                    (16U)                                          /**< (MCAN_SIDFC) List Size Standard Position */
#define MCAN_SIDFC_LSS_Msk                    (0xFFU << MCAN_SIDFC_LSS_Pos)                  /**< (MCAN_SIDFC) List Size Standard Mask */
#define MCAN_SIDFC_LSS(value)                 (MCAN_SIDFC_LSS_Msk & ((value) << MCAN_SIDFC_LSS_Pos))
#define MCAN_SIDFC_Msk                        (0x00FFFFFCUL)                                 /**< (MCAN_SIDFC) Register Mask  */

/* -------- MCAN_XIDFC : (MCAN Offset: 0x88) (R/W 32) Extended ID Filter Configuration Register -------- */
#define MCAN_XIDFC_FLESA_Pos                  (2U)                                           /**< (MCAN_XIDFC) Filter List Extended Start Address Position */
#define MCAN_XIDFC_FLESA_Msk                  (0x3FFFU << MCAN_XIDFC_FLESA_Pos)              /**< (MCAN_XIDFC) Filter List Extended Start Address Mask */
#define MCAN_XIDFC_FLESA(value)               (MCAN_XIDFC_FLESA_Msk & ((value) << MCAN_XIDFC_FLESA_Pos))
#define MCAN_XIDFC_LSE_Pos                    (16U)                                          /**< (MCAN_XIDFC) List Size Extended Position */
#define MCAN_XIDFC_LSE_Msk                    (0x7FU << MCAN_XIDFC_LSE_Pos)                  /**< (MCAN_XIDFC) List Size Extended Mask */
#define MCAN_XIDFC_LSE(value)                 (MCAN_XIDFC_LSE_Msk & ((value) << MCAN_XIDFC_LSE_Pos))
#define MCAN_XIDFC_Msk                        (0x007FFFFCUL)                                 /**< (MCAN_XIDFC) Register Mask  */

/* -------- MCAN_XIDAM : (MCAN Offset: 0x90) (R/W 32) Extended ID AND Mask Register -------- */
#define MCAN_XIDAM_EIDM_Pos                   (0U)                                           /**< (MCAN_XIDAM) Extended ID Mask Position */
#define MCAN_XIDAM_EIDM_Msk                   (0x1FFFFFFFU << MCAN_XIDAM_EIDM_Pos)           /**< (MCAN_XIDAM) Extended ID Mask Mask */
#define MCAN_XIDAM_EIDM(value)                (MCAN_XIDAM_EIDM_Msk & ((value) << MCAN_XIDAM_EIDM_Pos))
#define MCAN_XIDAM_Msk                        (0x1FFFFFFFUL)                                 /**< (MCAN_XIDAM) Register Mask  */

/* -------- MCAN_HPMS : (MCAN Offset: 0x94) (R/  32) High Priority Message Status Register -------- */
#define MCAN_HPMS_BIDX_Pos                    (0U)                                           /**< (MCAN_HPMS) Buffer Index Position */
#define MCAN_HPMS_BIDX_Msk                    (0x3FU << MCAN_HPMS_BIDX_Pos)                  /**< (MCAN_HPMS) Buffer Index Mask */
#define MCAN_HPMS_BIDX(value)                 (MCAN_HPMS_BIDX_Msk & ((value) << MCAN_HPMS_BIDX_Pos))
#define MCAN_HPMS_MSI_Pos                     (6U)                                           /**< (MCAN_HPMS) Message Storage Indicator Position */
#define MCAN_HPMS_MSI_Msk                     (0x3U << MCAN_HPMS_MSI_Pos)                    /**< (MCAN_HPMS) Message Storage Indicator Mask */
#define MCAN_HPMS_MSI(value)                  (MCAN_HPMS_MSI_Msk & ((value) << MCAN_HPMS_MSI_Pos))
#define   MCAN_HPMS_MSI_NO_FIFO_SEL_Val       (0U)                                           /**< (MCAN_HPMS) No FIFO selected.  */
#define   MCAN_HPMS_MSI_LOST_Val              (1U)                                           /**< (MCAN_HPMS) FIFO message lost.  */
#define   MCAN_HPMS_MSI_FIFO_0_Val            (2U)                                           /**< (MCAN_HPMS) Message stored in FIFO 0.  */
#define   MCAN_HPMS_MSI_FIFO_1_Val            (3U)                                           /**< (MCAN_HPMS) Message stored in FIFO 1.  */
#define MCAN_HPMS_MSI_NO_FIFO_SEL             (MCAN_HPMS_MSI_NO_FIFO_SEL_Val << MCAN_HPMS_MSI_Pos) /**< (MCAN_HPMS) No FIFO selected. Position  */
#define MCAN_HPMS_MSI_LOST                    (MCAN_HPMS_MSI_LOST_Val << MCAN_HPMS_MSI_Pos)  /**< (MCAN_HPMS) FIFO message lost. Position  */
#define MCAN_HPMS_MSI_FIFO_0                  (MCAN_HPMS_MSI_FIFO_0_Val << MCAN_HPMS_MSI_Pos) /**< (MCAN_HPMS) Message stored in FIFO 0. Position  */
#define MCAN_HPMS_MSI_FIFO_1                  (MCAN_HPMS_MSI_FIFO_1_Val << MCAN_HPMS_MSI_Pos) /**< (MCAN_HPMS) Message stored in FIFO 1. Position  */
#define MCAN_HPMS_FIDX_Pos                    (8U)                                           /**< (MCAN_HPMS) Filter Index Position */
#define MCAN_HPMS_FIDX_Msk                    (0x7FU << MCAN_HPMS_FIDX_Pos)                  /**< (MCAN_HPMS) Filter Index Mask */
#define MCAN_HPMS_FIDX(value)                 (MCAN_HPMS_FIDX_Msk & ((value) << MCAN_HPMS_FIDX_Pos))
#define MCAN_HPMS_FLST_Pos                    (15U)                                          /**< (MCAN_HPMS) Filter List Position */
#define MCAN_HPMS_FLST_Msk                    (0x1U << MCAN_HPMS_FLST_Pos)                   /**< (MCAN_HPMS) Filter List Mask */
#define MCAN_HPMS_FLST(value)                 (MCAN_HPMS_FLST_Msk & ((value) << MCAN_HPMS_FLST_Pos))
#define MCAN_HPMS_Msk                         (0x0000FFFFUL)                                 /**< (MCAN_HPMS) Register Mask  */

/* -------- MCAN_NDAT1 : (MCAN Offset: 0x98) (R/W 32) New Data 1 Register -------- */
#define MCAN_NDAT1_ND0_Pos                    (0U)                                           /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND0_Msk                    (0x1U << MCAN_NDAT1_ND0_Pos)                   /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND0(value)                 (MCAN_NDAT1_ND0_Msk & ((value) << MCAN_NDAT1_ND0_Pos))
#define MCAN_NDAT1_ND1_Pos                    (1U)                                           /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND1_Msk                    (0x1U << MCAN_NDAT1_ND1_Pos)                   /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND1(value)                 (MCAN_NDAT1_ND1_Msk & ((value) << MCAN_NDAT1_ND1_Pos))
#define MCAN_NDAT1_ND2_Pos                    (2U)                                           /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND2_Msk                    (0x1U << MCAN_NDAT1_ND2_Pos)                   /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND2(value)                 (MCAN_NDAT1_ND2_Msk & ((value) << MCAN_NDAT1_ND2_Pos))
#define MCAN_NDAT1_ND3_Pos                    (3U)                                           /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND3_Msk                    (0x1U << MCAN_NDAT1_ND3_Pos)                   /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND3(value)                 (MCAN_NDAT1_ND3_Msk & ((value) << MCAN_NDAT1_ND3_Pos))
#define MCAN_NDAT1_ND4_Pos                    (4U)                                           /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND4_Msk                    (0x1U << MCAN_NDAT1_ND4_Pos)                   /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND4(value)                 (MCAN_NDAT1_ND4_Msk & ((value) << MCAN_NDAT1_ND4_Pos))
#define MCAN_NDAT1_ND5_Pos                    (5U)                                           /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND5_Msk                    (0x1U << MCAN_NDAT1_ND5_Pos)                   /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND5(value)                 (MCAN_NDAT1_ND5_Msk & ((value) << MCAN_NDAT1_ND5_Pos))
#define MCAN_NDAT1_ND6_Pos                    (6U)                                           /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND6_Msk                    (0x1U << MCAN_NDAT1_ND6_Pos)                   /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND6(value)                 (MCAN_NDAT1_ND6_Msk & ((value) << MCAN_NDAT1_ND6_Pos))
#define MCAN_NDAT1_ND7_Pos                    (7U)                                           /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND7_Msk                    (0x1U << MCAN_NDAT1_ND7_Pos)                   /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND7(value)                 (MCAN_NDAT1_ND7_Msk & ((value) << MCAN_NDAT1_ND7_Pos))
#define MCAN_NDAT1_ND8_Pos                    (8U)                                           /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND8_Msk                    (0x1U << MCAN_NDAT1_ND8_Pos)                   /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND8(value)                 (MCAN_NDAT1_ND8_Msk & ((value) << MCAN_NDAT1_ND8_Pos))
#define MCAN_NDAT1_ND9_Pos                    (9U)                                           /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND9_Msk                    (0x1U << MCAN_NDAT1_ND9_Pos)                   /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND9(value)                 (MCAN_NDAT1_ND9_Msk & ((value) << MCAN_NDAT1_ND9_Pos))
#define MCAN_NDAT1_ND10_Pos                   (10U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND10_Msk                   (0x1U << MCAN_NDAT1_ND10_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND10(value)                (MCAN_NDAT1_ND10_Msk & ((value) << MCAN_NDAT1_ND10_Pos))
#define MCAN_NDAT1_ND11_Pos                   (11U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND11_Msk                   (0x1U << MCAN_NDAT1_ND11_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND11(value)                (MCAN_NDAT1_ND11_Msk & ((value) << MCAN_NDAT1_ND11_Pos))
#define MCAN_NDAT1_ND12_Pos                   (12U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND12_Msk                   (0x1U << MCAN_NDAT1_ND12_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND12(value)                (MCAN_NDAT1_ND12_Msk & ((value) << MCAN_NDAT1_ND12_Pos))
#define MCAN_NDAT1_ND13_Pos                   (13U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND13_Msk                   (0x1U << MCAN_NDAT1_ND13_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND13(value)                (MCAN_NDAT1_ND13_Msk & ((value) << MCAN_NDAT1_ND13_Pos))
#define MCAN_NDAT1_ND14_Pos                   (14U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND14_Msk                   (0x1U << MCAN_NDAT1_ND14_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND14(value)                (MCAN_NDAT1_ND14_Msk & ((value) << MCAN_NDAT1_ND14_Pos))
#define MCAN_NDAT1_ND15_Pos                   (15U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND15_Msk                   (0x1U << MCAN_NDAT1_ND15_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND15(value)                (MCAN_NDAT1_ND15_Msk & ((value) << MCAN_NDAT1_ND15_Pos))
#define MCAN_NDAT1_ND16_Pos                   (16U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND16_Msk                   (0x1U << MCAN_NDAT1_ND16_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND16(value)                (MCAN_NDAT1_ND16_Msk & ((value) << MCAN_NDAT1_ND16_Pos))
#define MCAN_NDAT1_ND17_Pos                   (17U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND17_Msk                   (0x1U << MCAN_NDAT1_ND17_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND17(value)                (MCAN_NDAT1_ND17_Msk & ((value) << MCAN_NDAT1_ND17_Pos))
#define MCAN_NDAT1_ND18_Pos                   (18U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND18_Msk                   (0x1U << MCAN_NDAT1_ND18_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND18(value)                (MCAN_NDAT1_ND18_Msk & ((value) << MCAN_NDAT1_ND18_Pos))
#define MCAN_NDAT1_ND19_Pos                   (19U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND19_Msk                   (0x1U << MCAN_NDAT1_ND19_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND19(value)                (MCAN_NDAT1_ND19_Msk & ((value) << MCAN_NDAT1_ND19_Pos))
#define MCAN_NDAT1_ND20_Pos                   (20U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND20_Msk                   (0x1U << MCAN_NDAT1_ND20_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND20(value)                (MCAN_NDAT1_ND20_Msk & ((value) << MCAN_NDAT1_ND20_Pos))
#define MCAN_NDAT1_ND21_Pos                   (21U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND21_Msk                   (0x1U << MCAN_NDAT1_ND21_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND21(value)                (MCAN_NDAT1_ND21_Msk & ((value) << MCAN_NDAT1_ND21_Pos))
#define MCAN_NDAT1_ND22_Pos                   (22U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND22_Msk                   (0x1U << MCAN_NDAT1_ND22_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND22(value)                (MCAN_NDAT1_ND22_Msk & ((value) << MCAN_NDAT1_ND22_Pos))
#define MCAN_NDAT1_ND23_Pos                   (23U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND23_Msk                   (0x1U << MCAN_NDAT1_ND23_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND23(value)                (MCAN_NDAT1_ND23_Msk & ((value) << MCAN_NDAT1_ND23_Pos))
#define MCAN_NDAT1_ND24_Pos                   (24U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND24_Msk                   (0x1U << MCAN_NDAT1_ND24_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND24(value)                (MCAN_NDAT1_ND24_Msk & ((value) << MCAN_NDAT1_ND24_Pos))
#define MCAN_NDAT1_ND25_Pos                   (25U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND25_Msk                   (0x1U << MCAN_NDAT1_ND25_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND25(value)                (MCAN_NDAT1_ND25_Msk & ((value) << MCAN_NDAT1_ND25_Pos))
#define MCAN_NDAT1_ND26_Pos                   (26U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND26_Msk                   (0x1U << MCAN_NDAT1_ND26_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND26(value)                (MCAN_NDAT1_ND26_Msk & ((value) << MCAN_NDAT1_ND26_Pos))
#define MCAN_NDAT1_ND27_Pos                   (27U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND27_Msk                   (0x1U << MCAN_NDAT1_ND27_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND27(value)                (MCAN_NDAT1_ND27_Msk & ((value) << MCAN_NDAT1_ND27_Pos))
#define MCAN_NDAT1_ND28_Pos                   (28U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND28_Msk                   (0x1U << MCAN_NDAT1_ND28_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND28(value)                (MCAN_NDAT1_ND28_Msk & ((value) << MCAN_NDAT1_ND28_Pos))
#define MCAN_NDAT1_ND29_Pos                   (29U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND29_Msk                   (0x1U << MCAN_NDAT1_ND29_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND29(value)                (MCAN_NDAT1_ND29_Msk & ((value) << MCAN_NDAT1_ND29_Pos))
#define MCAN_NDAT1_ND30_Pos                   (30U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND30_Msk                   (0x1U << MCAN_NDAT1_ND30_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND30(value)                (MCAN_NDAT1_ND30_Msk & ((value) << MCAN_NDAT1_ND30_Pos))
#define MCAN_NDAT1_ND31_Pos                   (31U)                                          /**< (MCAN_NDAT1) New Data Position */
#define MCAN_NDAT1_ND31_Msk                   (0x1U << MCAN_NDAT1_ND31_Pos)                  /**< (MCAN_NDAT1) New Data Mask */
#define MCAN_NDAT1_ND31(value)                (MCAN_NDAT1_ND31_Msk & ((value) << MCAN_NDAT1_ND31_Pos))
#define MCAN_NDAT1_Msk                        (0xFFFFFFFFUL)                                 /**< (MCAN_NDAT1) Register Mask  */

/* -------- MCAN_NDAT2 : (MCAN Offset: 0x9C) (R/W 32) New Data 2 Register -------- */
#define MCAN_NDAT2_ND32_Pos                   (0U)                                           /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND32_Msk                   (0x1U << MCAN_NDAT2_ND32_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND32(value)                (MCAN_NDAT2_ND32_Msk & ((value) << MCAN_NDAT2_ND32_Pos))
#define MCAN_NDAT2_ND33_Pos                   (1U)                                           /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND33_Msk                   (0x1U << MCAN_NDAT2_ND33_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND33(value)                (MCAN_NDAT2_ND33_Msk & ((value) << MCAN_NDAT2_ND33_Pos))
#define MCAN_NDAT2_ND34_Pos                   (2U)                                           /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND34_Msk                   (0x1U << MCAN_NDAT2_ND34_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND34(value)                (MCAN_NDAT2_ND34_Msk & ((value) << MCAN_NDAT2_ND34_Pos))
#define MCAN_NDAT2_ND35_Pos                   (3U)                                           /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND35_Msk                   (0x1U << MCAN_NDAT2_ND35_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND35(value)                (MCAN_NDAT2_ND35_Msk & ((value) << MCAN_NDAT2_ND35_Pos))
#define MCAN_NDAT2_ND36_Pos                   (4U)                                           /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND36_Msk                   (0x1U << MCAN_NDAT2_ND36_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND36(value)                (MCAN_NDAT2_ND36_Msk & ((value) << MCAN_NDAT2_ND36_Pos))
#define MCAN_NDAT2_ND37_Pos                   (5U)                                           /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND37_Msk                   (0x1U << MCAN_NDAT2_ND37_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND37(value)                (MCAN_NDAT2_ND37_Msk & ((value) << MCAN_NDAT2_ND37_Pos))
#define MCAN_NDAT2_ND38_Pos                   (6U)                                           /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND38_Msk                   (0x1U << MCAN_NDAT2_ND38_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND38(value)                (MCAN_NDAT2_ND38_Msk & ((value) << MCAN_NDAT2_ND38_Pos))
#define MCAN_NDAT2_ND39_Pos                   (7U)                                           /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND39_Msk                   (0x1U << MCAN_NDAT2_ND39_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND39(value)                (MCAN_NDAT2_ND39_Msk & ((value) << MCAN_NDAT2_ND39_Pos))
#define MCAN_NDAT2_ND40_Pos                   (8U)                                           /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND40_Msk                   (0x1U << MCAN_NDAT2_ND40_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND40(value)                (MCAN_NDAT2_ND40_Msk & ((value) << MCAN_NDAT2_ND40_Pos))
#define MCAN_NDAT2_ND41_Pos                   (9U)                                           /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND41_Msk                   (0x1U << MCAN_NDAT2_ND41_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND41(value)                (MCAN_NDAT2_ND41_Msk & ((value) << MCAN_NDAT2_ND41_Pos))
#define MCAN_NDAT2_ND42_Pos                   (10U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND42_Msk                   (0x1U << MCAN_NDAT2_ND42_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND42(value)                (MCAN_NDAT2_ND42_Msk & ((value) << MCAN_NDAT2_ND42_Pos))
#define MCAN_NDAT2_ND43_Pos                   (11U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND43_Msk                   (0x1U << MCAN_NDAT2_ND43_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND43(value)                (MCAN_NDAT2_ND43_Msk & ((value) << MCAN_NDAT2_ND43_Pos))
#define MCAN_NDAT2_ND44_Pos                   (12U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND44_Msk                   (0x1U << MCAN_NDAT2_ND44_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND44(value)                (MCAN_NDAT2_ND44_Msk & ((value) << MCAN_NDAT2_ND44_Pos))
#define MCAN_NDAT2_ND45_Pos                   (13U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND45_Msk                   (0x1U << MCAN_NDAT2_ND45_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND45(value)                (MCAN_NDAT2_ND45_Msk & ((value) << MCAN_NDAT2_ND45_Pos))
#define MCAN_NDAT2_ND46_Pos                   (14U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND46_Msk                   (0x1U << MCAN_NDAT2_ND46_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND46(value)                (MCAN_NDAT2_ND46_Msk & ((value) << MCAN_NDAT2_ND46_Pos))
#define MCAN_NDAT2_ND47_Pos                   (15U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND47_Msk                   (0x1U << MCAN_NDAT2_ND47_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND47(value)                (MCAN_NDAT2_ND47_Msk & ((value) << MCAN_NDAT2_ND47_Pos))
#define MCAN_NDAT2_ND48_Pos                   (16U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND48_Msk                   (0x1U << MCAN_NDAT2_ND48_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND48(value)                (MCAN_NDAT2_ND48_Msk & ((value) << MCAN_NDAT2_ND48_Pos))
#define MCAN_NDAT2_ND49_Pos                   (17U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND49_Msk                   (0x1U << MCAN_NDAT2_ND49_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND49(value)                (MCAN_NDAT2_ND49_Msk & ((value) << MCAN_NDAT2_ND49_Pos))
#define MCAN_NDAT2_ND50_Pos                   (18U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND50_Msk                   (0x1U << MCAN_NDAT2_ND50_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND50(value)                (MCAN_NDAT2_ND50_Msk & ((value) << MCAN_NDAT2_ND50_Pos))
#define MCAN_NDAT2_ND51_Pos                   (19U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND51_Msk                   (0x1U << MCAN_NDAT2_ND51_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND51(value)                (MCAN_NDAT2_ND51_Msk & ((value) << MCAN_NDAT2_ND51_Pos))
#define MCAN_NDAT2_ND52_Pos                   (20U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND52_Msk                   (0x1U << MCAN_NDAT2_ND52_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND52(value)                (MCAN_NDAT2_ND52_Msk & ((value) << MCAN_NDAT2_ND52_Pos))
#define MCAN_NDAT2_ND53_Pos                   (21U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND53_Msk                   (0x1U << MCAN_NDAT2_ND53_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND53(value)                (MCAN_NDAT2_ND53_Msk & ((value) << MCAN_NDAT2_ND53_Pos))
#define MCAN_NDAT2_ND54_Pos                   (22U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND54_Msk                   (0x1U << MCAN_NDAT2_ND54_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND54(value)                (MCAN_NDAT2_ND54_Msk & ((value) << MCAN_NDAT2_ND54_Pos))
#define MCAN_NDAT2_ND55_Pos                   (23U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND55_Msk                   (0x1U << MCAN_NDAT2_ND55_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND55(value)                (MCAN_NDAT2_ND55_Msk & ((value) << MCAN_NDAT2_ND55_Pos))
#define MCAN_NDAT2_ND56_Pos                   (24U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND56_Msk                   (0x1U << MCAN_NDAT2_ND56_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND56(value)                (MCAN_NDAT2_ND56_Msk & ((value) << MCAN_NDAT2_ND56_Pos))
#define MCAN_NDAT2_ND57_Pos                   (25U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND57_Msk                   (0x1U << MCAN_NDAT2_ND57_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND57(value)                (MCAN_NDAT2_ND57_Msk & ((value) << MCAN_NDAT2_ND57_Pos))
#define MCAN_NDAT2_ND58_Pos                   (26U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND58_Msk                   (0x1U << MCAN_NDAT2_ND58_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND58(value)                (MCAN_NDAT2_ND58_Msk & ((value) << MCAN_NDAT2_ND58_Pos))
#define MCAN_NDAT2_ND59_Pos                   (27U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND59_Msk                   (0x1U << MCAN_NDAT2_ND59_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND59(value)                (MCAN_NDAT2_ND59_Msk & ((value) << MCAN_NDAT2_ND59_Pos))
#define MCAN_NDAT2_ND60_Pos                   (28U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND60_Msk                   (0x1U << MCAN_NDAT2_ND60_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND60(value)                (MCAN_NDAT2_ND60_Msk & ((value) << MCAN_NDAT2_ND60_Pos))
#define MCAN_NDAT2_ND61_Pos                   (29U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND61_Msk                   (0x1U << MCAN_NDAT2_ND61_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND61(value)                (MCAN_NDAT2_ND61_Msk & ((value) << MCAN_NDAT2_ND61_Pos))
#define MCAN_NDAT2_ND62_Pos                   (30U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND62_Msk                   (0x1U << MCAN_NDAT2_ND62_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND62(value)                (MCAN_NDAT2_ND62_Msk & ((value) << MCAN_NDAT2_ND62_Pos))
#define MCAN_NDAT2_ND63_Pos                   (31U)                                          /**< (MCAN_NDAT2) New Data Position */
#define MCAN_NDAT2_ND63_Msk                   (0x1U << MCAN_NDAT2_ND63_Pos)                  /**< (MCAN_NDAT2) New Data Mask */
#define MCAN_NDAT2_ND63(value)                (MCAN_NDAT2_ND63_Msk & ((value) << MCAN_NDAT2_ND63_Pos))
#define MCAN_NDAT2_Msk                        (0xFFFFFFFFUL)                                 /**< (MCAN_NDAT2) Register Mask  */

/* -------- MCAN_RXF0C : (MCAN Offset: 0xA0) (R/W 32) Receive FIFO 0 Configuration Register -------- */
#define MCAN_RXF0C_F0SA_Pos                   (2U)                                           /**< (MCAN_RXF0C) Receive FIFO 0 Start Address Position */
#define MCAN_RXF0C_F0SA_Msk                   (0x3FFFU << MCAN_RXF0C_F0SA_Pos)               /**< (MCAN_RXF0C) Receive FIFO 0 Start Address Mask */
#define MCAN_RXF0C_F0SA(value)                (MCAN_RXF0C_F0SA_Msk & ((value) << MCAN_RXF0C_F0SA_Pos))
#define MCAN_RXF0C_F0S_Pos                    (16U)                                          /**< (MCAN_RXF0C) Receive FIFO 0 Start Address Position */
#define MCAN_RXF0C_F0S_Msk                    (0x7FU << MCAN_RXF0C_F0S_Pos)                  /**< (MCAN_RXF0C) Receive FIFO 0 Start Address Mask */
#define MCAN_RXF0C_F0S(value)                 (MCAN_RXF0C_F0S_Msk & ((value) << MCAN_RXF0C_F0S_Pos))
#define MCAN_RXF0C_F0WM_Pos                   (24U)                                          /**< (MCAN_RXF0C) Receive FIFO 0 Watermark Position */
#define MCAN_RXF0C_F0WM_Msk                   (0x7FU << MCAN_RXF0C_F0WM_Pos)                 /**< (MCAN_RXF0C) Receive FIFO 0 Watermark Mask */
#define MCAN_RXF0C_F0WM(value)                (MCAN_RXF0C_F0WM_Msk & ((value) << MCAN_RXF0C_F0WM_Pos))
#define MCAN_RXF0C_F0OM_Pos                   (31U)                                          /**< (MCAN_RXF0C) FIFO 0 Operation Mode Position */
#define MCAN_RXF0C_F0OM_Msk                   (0x1U << MCAN_RXF0C_F0OM_Pos)                  /**< (MCAN_RXF0C) FIFO 0 Operation Mode Mask */
#define MCAN_RXF0C_F0OM(value)                (MCAN_RXF0C_F0OM_Msk & ((value) << MCAN_RXF0C_F0OM_Pos))
#define MCAN_RXF0C_Msk                        (0xFF7FFFFCUL)                                 /**< (MCAN_RXF0C) Register Mask  */

/* -------- MCAN_RXF0S : (MCAN Offset: 0xA4) (R/  32) Receive FIFO 0 Status Register -------- */
#define MCAN_RXF0S_F0FL_Pos                   (0U)                                           /**< (MCAN_RXF0S) Receive FIFO 0 Fill Level Position */
#define MCAN_RXF0S_F0FL_Msk                   (0x7FU << MCAN_RXF0S_F0FL_Pos)                 /**< (MCAN_RXF0S) Receive FIFO 0 Fill Level Mask */
#define MCAN_RXF0S_F0FL(value)                (MCAN_RXF0S_F0FL_Msk & ((value) << MCAN_RXF0S_F0FL_Pos))
#define MCAN_RXF0S_F0GI_Pos                   (8U)                                           /**< (MCAN_RXF0S) Receive FIFO 0 Get Index Position */
#define MCAN_RXF0S_F0GI_Msk                   (0x3FU << MCAN_RXF0S_F0GI_Pos)                 /**< (MCAN_RXF0S) Receive FIFO 0 Get Index Mask */
#define MCAN_RXF0S_F0GI(value)                (MCAN_RXF0S_F0GI_Msk & ((value) << MCAN_RXF0S_F0GI_Pos))
#define MCAN_RXF0S_F0PI_Pos                   (16U)                                          /**< (MCAN_RXF0S) Receive FIFO 0 Put Index Position */
#define MCAN_RXF0S_F0PI_Msk                   (0x3FU << MCAN_RXF0S_F0PI_Pos)                 /**< (MCAN_RXF0S) Receive FIFO 0 Put Index Mask */
#define MCAN_RXF0S_F0PI(value)                (MCAN_RXF0S_F0PI_Msk & ((value) << MCAN_RXF0S_F0PI_Pos))
#define MCAN_RXF0S_F0F_Pos                    (24U)                                          /**< (MCAN_RXF0S) Receive FIFO 0 Fill Level Position */
#define MCAN_RXF0S_F0F_Msk                    (0x1U << MCAN_RXF0S_F0F_Pos)                   /**< (MCAN_RXF0S) Receive FIFO 0 Fill Level Mask */
#define MCAN_RXF0S_F0F(value)                 (MCAN_RXF0S_F0F_Msk & ((value) << MCAN_RXF0S_F0F_Pos))
#define MCAN_RXF0S_RF0L_Pos                   (25U)                                          /**< (MCAN_RXF0S) Receive FIFO 0 Message Lost Position */
#define MCAN_RXF0S_RF0L_Msk                   (0x1U << MCAN_RXF0S_RF0L_Pos)                  /**< (MCAN_RXF0S) Receive FIFO 0 Message Lost Mask */
#define MCAN_RXF0S_RF0L(value)                (MCAN_RXF0S_RF0L_Msk & ((value) << MCAN_RXF0S_RF0L_Pos))
#define MCAN_RXF0S_Msk                        (0x033F3F7FUL)                                 /**< (MCAN_RXF0S) Register Mask  */

/* -------- MCAN_RXF0A : (MCAN Offset: 0xA8) (R/W 32) Receive FIFO 0 Acknowledge Register -------- */
#define MCAN_RXF0A_F0AI_Pos                   (0U)                                           /**< (MCAN_RXF0A) Receive FIFO 0 Acknowledge Index Position */
#define MCAN_RXF0A_F0AI_Msk                   (0x3FU << MCAN_RXF0A_F0AI_Pos)                 /**< (MCAN_RXF0A) Receive FIFO 0 Acknowledge Index Mask */
#define MCAN_RXF0A_F0AI(value)                (MCAN_RXF0A_F0AI_Msk & ((value) << MCAN_RXF0A_F0AI_Pos))
#define MCAN_RXF0A_Msk                        (0x0000003FUL)                                 /**< (MCAN_RXF0A) Register Mask  */

/* -------- MCAN_RXBC : (MCAN Offset: 0xAC) (R/W 32) Receive Rx Buffer Configuration Register -------- */
#define MCAN_RXBC_RBSA_Pos                    (2U)                                           /**< (MCAN_RXBC) Receive Buffer Start Address Position */
#define MCAN_RXBC_RBSA_Msk                    (0x3FFFU << MCAN_RXBC_RBSA_Pos)                /**< (MCAN_RXBC) Receive Buffer Start Address Mask */
#define MCAN_RXBC_RBSA(value)                 (MCAN_RXBC_RBSA_Msk & ((value) << MCAN_RXBC_RBSA_Pos))
#define MCAN_RXBC_Msk                         (0x0000FFFCUL)                                 /**< (MCAN_RXBC) Register Mask  */

/* -------- MCAN_RXF1C : (MCAN Offset: 0xB0) (R/W 32) Receive FIFO 1 Configuration Register -------- */
#define MCAN_RXF1C_F1SA_Pos                   (2U)                                           /**< (MCAN_RXF1C) Receive FIFO 1 Start Address Position */
#define MCAN_RXF1C_F1SA_Msk                   (0x3FFFU << MCAN_RXF1C_F1SA_Pos)               /**< (MCAN_RXF1C) Receive FIFO 1 Start Address Mask */
#define MCAN_RXF1C_F1SA(value)                (MCAN_RXF1C_F1SA_Msk & ((value) << MCAN_RXF1C_F1SA_Pos))
#define MCAN_RXF1C_F1S_Pos                    (16U)                                          /**< (MCAN_RXF1C) Receive FIFO 1 Start Address Position */
#define MCAN_RXF1C_F1S_Msk                    (0x7FU << MCAN_RXF1C_F1S_Pos)                  /**< (MCAN_RXF1C) Receive FIFO 1 Start Address Mask */
#define MCAN_RXF1C_F1S(value)                 (MCAN_RXF1C_F1S_Msk & ((value) << MCAN_RXF1C_F1S_Pos))
#define MCAN_RXF1C_F1WM_Pos                   (24U)                                          /**< (MCAN_RXF1C) Receive FIFO 1 Watermark Position */
#define MCAN_RXF1C_F1WM_Msk                   (0x7FU << MCAN_RXF1C_F1WM_Pos)                 /**< (MCAN_RXF1C) Receive FIFO 1 Watermark Mask */
#define MCAN_RXF1C_F1WM(value)                (MCAN_RXF1C_F1WM_Msk & ((value) << MCAN_RXF1C_F1WM_Pos))
#define MCAN_RXF1C_F1OM_Pos                   (31U)                                          /**< (MCAN_RXF1C) FIFO 1 Operation Mode Position */
#define MCAN_RXF1C_F1OM_Msk                   (0x1U << MCAN_RXF1C_F1OM_Pos)                  /**< (MCAN_RXF1C) FIFO 1 Operation Mode Mask */
#define MCAN_RXF1C_F1OM(value)                (MCAN_RXF1C_F1OM_Msk & ((value) << MCAN_RXF1C_F1OM_Pos))
#define MCAN_RXF1C_Msk                        (0xFF7FFFFCUL)                                 /**< (MCAN_RXF1C) Register Mask  */

/* -------- MCAN_RXF1S : (MCAN Offset: 0xB4) (R/  32) Receive FIFO 1 Status Register -------- */
#define MCAN_RXF1S_F1FL_Pos                   (0U)                                           /**< (MCAN_RXF1S) Receive FIFO 1 Fill Level Position */
#define MCAN_RXF1S_F1FL_Msk                   (0x7FU << MCAN_RXF1S_F1FL_Pos)                 /**< (MCAN_RXF1S) Receive FIFO 1 Fill Level Mask */
#define MCAN_RXF1S_F1FL(value)                (MCAN_RXF1S_F1FL_Msk & ((value) << MCAN_RXF1S_F1FL_Pos))
#define MCAN_RXF1S_F1GI_Pos                   (8U)                                           /**< (MCAN_RXF1S) Receive FIFO 1 Get Index Position */
#define MCAN_RXF1S_F1GI_Msk                   (0x3FU << MCAN_RXF1S_F1GI_Pos)                 /**< (MCAN_RXF1S) Receive FIFO 1 Get Index Mask */
#define MCAN_RXF1S_F1GI(value)                (MCAN_RXF1S_F1GI_Msk & ((value) << MCAN_RXF1S_F1GI_Pos))
#define MCAN_RXF1S_F1PI_Pos                   (16U)                                          /**< (MCAN_RXF1S) Receive FIFO 1 Put Index Position */
#define MCAN_RXF1S_F1PI_Msk                   (0x3FU << MCAN_RXF1S_F1PI_Pos)                 /**< (MCAN_RXF1S) Receive FIFO 1 Put Index Mask */
#define MCAN_RXF1S_F1PI(value)                (MCAN_RXF1S_F1PI_Msk & ((value) << MCAN_RXF1S_F1PI_Pos))
#define MCAN_RXF1S_F1F_Pos                    (24U)                                          /**< (MCAN_RXF1S) Receive FIFO 1 Fill Level Position */
#define MCAN_RXF1S_F1F_Msk                    (0x1U << MCAN_RXF1S_F1F_Pos)                   /**< (MCAN_RXF1S) Receive FIFO 1 Fill Level Mask */
#define MCAN_RXF1S_F1F(value)                 (MCAN_RXF1S_F1F_Msk & ((value) << MCAN_RXF1S_F1F_Pos))
#define MCAN_RXF1S_RF1L_Pos                   (25U)                                          /**< (MCAN_RXF1S) Receive FIFO 1 Message Lost Position */
#define MCAN_RXF1S_RF1L_Msk                   (0x1U << MCAN_RXF1S_RF1L_Pos)                  /**< (MCAN_RXF1S) Receive FIFO 1 Message Lost Mask */
#define MCAN_RXF1S_RF1L(value)                (MCAN_RXF1S_RF1L_Msk & ((value) << MCAN_RXF1S_RF1L_Pos))
#define MCAN_RXF1S_DMS_Pos                    (30U)                                          /**< (MCAN_RXF1S) Debug Message Status Position */
#define MCAN_RXF1S_DMS_Msk                    (0x3U << MCAN_RXF1S_DMS_Pos)                   /**< (MCAN_RXF1S) Debug Message Status Mask */
#define MCAN_RXF1S_DMS(value)                 (MCAN_RXF1S_DMS_Msk & ((value) << MCAN_RXF1S_DMS_Pos))
#define   MCAN_RXF1S_DMS_IDLE_Val             (0U)                                           /**< (MCAN_RXF1S) Idle state, wait for reception of debug messages, DMA request is cleared.  */
#define   MCAN_RXF1S_DMS_MSG_A_Val            (1U)                                           /**< (MCAN_RXF1S) Debug message A received.  */
#define   MCAN_RXF1S_DMS_MSG_AB_Val           (2U)                                           /**< (MCAN_RXF1S) Debug messages A, B received.  */
#define   MCAN_RXF1S_DMS_MSG_ABC_Val          (3U)                                           /**< (MCAN_RXF1S) Debug messages A, B, C received, DMA request is set.  */
#define MCAN_RXF1S_DMS_IDLE                   (MCAN_RXF1S_DMS_IDLE_Val << MCAN_RXF1S_DMS_Pos) /**< (MCAN_RXF1S) Idle state, wait for reception of debug messages, DMA request is cleared. Position  */
#define MCAN_RXF1S_DMS_MSG_A                  (MCAN_RXF1S_DMS_MSG_A_Val << MCAN_RXF1S_DMS_Pos) /**< (MCAN_RXF1S) Debug message A received. Position  */
#define MCAN_RXF1S_DMS_MSG_AB                 (MCAN_RXF1S_DMS_MSG_AB_Val << MCAN_RXF1S_DMS_Pos) /**< (MCAN_RXF1S) Debug messages A, B received. Position  */
#define MCAN_RXF1S_DMS_MSG_ABC                (MCAN_RXF1S_DMS_MSG_ABC_Val << MCAN_RXF1S_DMS_Pos) /**< (MCAN_RXF1S) Debug messages A, B, C received, DMA request is set. Position  */
#define MCAN_RXF1S_Msk                        (0xC33F3F7FUL)                                 /**< (MCAN_RXF1S) Register Mask  */

/* -------- MCAN_RXF1A : (MCAN Offset: 0xB8) (R/W 32) Receive FIFO 1 Acknowledge Register -------- */
#define MCAN_RXF1A_F1AI_Pos                   (0U)                                           /**< (MCAN_RXF1A) Receive FIFO 1 Acknowledge Index Position */
#define MCAN_RXF1A_F1AI_Msk                   (0x3FU << MCAN_RXF1A_F1AI_Pos)                 /**< (MCAN_RXF1A) Receive FIFO 1 Acknowledge Index Mask */
#define MCAN_RXF1A_F1AI(value)                (MCAN_RXF1A_F1AI_Msk & ((value) << MCAN_RXF1A_F1AI_Pos))
#define MCAN_RXF1A_Msk                        (0x0000003FUL)                                 /**< (MCAN_RXF1A) Register Mask  */

/* -------- MCAN_RXESC : (MCAN Offset: 0xBC) (R/W 32) Receive Buffer / FIFO Element Size Configuration Register -------- */
#define MCAN_RXESC_F0DS_Pos                   (0U)                                           /**< (MCAN_RXESC) Receive FIFO 0 Data Field Size Position */
#define MCAN_RXESC_F0DS_Msk                   (0x7U << MCAN_RXESC_F0DS_Pos)                  /**< (MCAN_RXESC) Receive FIFO 0 Data Field Size Mask */
#define MCAN_RXESC_F0DS(value)                (MCAN_RXESC_F0DS_Msk & ((value) << MCAN_RXESC_F0DS_Pos))
#define   MCAN_RXESC_F0DS_8_BYTE_Val          (0U)                                           /**< (MCAN_RXESC) 8-byte data field  */
#define   MCAN_RXESC_F0DS_12_BYTE_Val         (1U)                                           /**< (MCAN_RXESC) 12-byte data field  */
#define   MCAN_RXESC_F0DS_16_BYTE_Val         (2U)                                           /**< (MCAN_RXESC) 16-byte data field  */
#define   MCAN_RXESC_F0DS_20_BYTE_Val         (3U)                                           /**< (MCAN_RXESC) 20-byte data field  */
#define   MCAN_RXESC_F0DS_24_BYTE_Val         (4U)                                           /**< (MCAN_RXESC) 24-byte data field  */
#define   MCAN_RXESC_F0DS_32_BYTE_Val         (5U)                                           /**< (MCAN_RXESC) 32-byte data field  */
#define   MCAN_RXESC_F0DS_48_BYTE_Val         (6U)                                           /**< (MCAN_RXESC) 48-byte data field  */
#define   MCAN_RXESC_F0DS_64_BYTE_Val         (7U)                                           /**< (MCAN_RXESC) 64-byte data field  */
#define MCAN_RXESC_F0DS_8_BYTE                (MCAN_RXESC_F0DS_8_BYTE_Val << MCAN_RXESC_F0DS_Pos) /**< (MCAN_RXESC) 8-byte data field Position  */
#define MCAN_RXESC_F0DS_12_BYTE               (MCAN_RXESC_F0DS_12_BYTE_Val << MCAN_RXESC_F0DS_Pos) /**< (MCAN_RXESC) 12-byte data field Position  */
#define MCAN_RXESC_F0DS_16_BYTE               (MCAN_RXESC_F0DS_16_BYTE_Val << MCAN_RXESC_F0DS_Pos) /**< (MCAN_RXESC) 16-byte data field Position  */
#define MCAN_RXESC_F0DS_20_BYTE               (MCAN_RXESC_F0DS_20_BYTE_Val << MCAN_RXESC_F0DS_Pos) /**< (MCAN_RXESC) 20-byte data field Position  */
#define MCAN_RXESC_F0DS_24_BYTE               (MCAN_RXESC_F0DS_24_BYTE_Val << MCAN_RXESC_F0DS_Pos) /**< (MCAN_RXESC) 24-byte data field Position  */
#define MCAN_RXESC_F0DS_32_BYTE               (MCAN_RXESC_F0DS_32_BYTE_Val << MCAN_RXESC_F0DS_Pos) /**< (MCAN_RXESC) 32-byte data field Position  */
#define MCAN_RXESC_F0DS_48_BYTE               (MCAN_RXESC_F0DS_48_BYTE_Val << MCAN_RXESC_F0DS_Pos) /**< (MCAN_RXESC) 48-byte data field Position  */
#define MCAN_RXESC_F0DS_64_BYTE               (MCAN_RXESC_F0DS_64_BYTE_Val << MCAN_RXESC_F0DS_Pos) /**< (MCAN_RXESC) 64-byte data field Position  */
#define MCAN_RXESC_F1DS_Pos                   (4U)                                           /**< (MCAN_RXESC) Receive FIFO 1 Data Field Size Position */
#define MCAN_RXESC_F1DS_Msk                   (0x7U << MCAN_RXESC_F1DS_Pos)                  /**< (MCAN_RXESC) Receive FIFO 1 Data Field Size Mask */
#define MCAN_RXESC_F1DS(value)                (MCAN_RXESC_F1DS_Msk & ((value) << MCAN_RXESC_F1DS_Pos))
#define   MCAN_RXESC_F1DS_8_BYTE_Val          (0U)                                           /**< (MCAN_RXESC) 8-byte data field  */
#define   MCAN_RXESC_F1DS_12_BYTE_Val         (1U)                                           /**< (MCAN_RXESC) 12-byte data field  */
#define   MCAN_RXESC_F1DS_16_BYTE_Val         (2U)                                           /**< (MCAN_RXESC) 16-byte data field  */
#define   MCAN_RXESC_F1DS_20_BYTE_Val         (3U)                                           /**< (MCAN_RXESC) 20-byte data field  */
#define   MCAN_RXESC_F1DS_24_BYTE_Val         (4U)                                           /**< (MCAN_RXESC) 24-byte data field  */
#define   MCAN_RXESC_F1DS_32_BYTE_Val         (5U)                                           /**< (MCAN_RXESC) 32-byte data field  */
#define   MCAN_RXESC_F1DS_48_BYTE_Val         (6U)                                           /**< (MCAN_RXESC) 48-byte data field  */
#define   MCAN_RXESC_F1DS_64_BYTE_Val         (7U)                                           /**< (MCAN_RXESC) 64-byte data field  */
#define MCAN_RXESC_F1DS_8_BYTE                (MCAN_RXESC_F1DS_8_BYTE_Val << MCAN_RXESC_F1DS_Pos) /**< (MCAN_RXESC) 8-byte data field Position  */
#define MCAN_RXESC_F1DS_12_BYTE               (MCAN_RXESC_F1DS_12_BYTE_Val << MCAN_RXESC_F1DS_Pos) /**< (MCAN_RXESC) 12-byte data field Position  */
#define MCAN_RXESC_F1DS_16_BYTE               (MCAN_RXESC_F1DS_16_BYTE_Val << MCAN_RXESC_F1DS_Pos) /**< (MCAN_RXESC) 16-byte data field Position  */
#define MCAN_RXESC_F1DS_20_BYTE               (MCAN_RXESC_F1DS_20_BYTE_Val << MCAN_RXESC_F1DS_Pos) /**< (MCAN_RXESC) 20-byte data field Position  */
#define MCAN_RXESC_F1DS_24_BYTE               (MCAN_RXESC_F1DS_24_BYTE_Val << MCAN_RXESC_F1DS_Pos) /**< (MCAN_RXESC) 24-byte data field Position  */
#define MCAN_RXESC_F1DS_32_BYTE               (MCAN_RXESC_F1DS_32_BYTE_Val << MCAN_RXESC_F1DS_Pos) /**< (MCAN_RXESC) 32-byte data field Position  */
#define MCAN_RXESC_F1DS_48_BYTE               (MCAN_RXESC_F1DS_48_BYTE_Val << MCAN_RXESC_F1DS_Pos) /**< (MCAN_RXESC) 48-byte data field Position  */
#define MCAN_RXESC_F1DS_64_BYTE               (MCAN_RXESC_F1DS_64_BYTE_Val << MCAN_RXESC_F1DS_Pos) /**< (MCAN_RXESC) 64-byte data field Position  */
#define MCAN_RXESC_RBDS_Pos                   (8U)                                           /**< (MCAN_RXESC) Receive Buffer Data Field Size Position */
#define MCAN_RXESC_RBDS_Msk                   (0x7U << MCAN_RXESC_RBDS_Pos)                  /**< (MCAN_RXESC) Receive Buffer Data Field Size Mask */
#define MCAN_RXESC_RBDS(value)                (MCAN_RXESC_RBDS_Msk & ((value) << MCAN_RXESC_RBDS_Pos))
#define   MCAN_RXESC_RBDS_8_BYTE_Val          (0U)                                           /**< (MCAN_RXESC) 8-byte data field  */
#define   MCAN_RXESC_RBDS_12_BYTE_Val         (1U)                                           /**< (MCAN_RXESC) 12-byte data field  */
#define   MCAN_RXESC_RBDS_16_BYTE_Val         (2U)                                           /**< (MCAN_RXESC) 16-byte data field  */
#define   MCAN_RXESC_RBDS_20_BYTE_Val         (3U)                                           /**< (MCAN_RXESC) 20-byte data field  */
#define   MCAN_RXESC_RBDS_24_BYTE_Val         (4U)                                           /**< (MCAN_RXESC) 24-byte data field  */
#define   MCAN_RXESC_RBDS_32_BYTE_Val         (5U)                                           /**< (MCAN_RXESC) 32-byte data field  */
#define   MCAN_RXESC_RBDS_48_BYTE_Val         (6U)                                           /**< (MCAN_RXESC) 48-byte data field  */
#define   MCAN_RXESC_RBDS_64_BYTE_Val         (7U)                                           /**< (MCAN_RXESC) 64-byte data field  */
#define MCAN_RXESC_RBDS_8_BYTE                (MCAN_RXESC_RBDS_8_BYTE_Val << MCAN_RXESC_RBDS_Pos) /**< (MCAN_RXESC) 8-byte data field Position  */
#define MCAN_RXESC_RBDS_12_BYTE               (MCAN_RXESC_RBDS_12_BYTE_Val << MCAN_RXESC_RBDS_Pos) /**< (MCAN_RXESC) 12-byte data field Position  */
#define MCAN_RXESC_RBDS_16_BYTE               (MCAN_RXESC_RBDS_16_BYTE_Val << MCAN_RXESC_RBDS_Pos) /**< (MCAN_RXESC) 16-byte data field Position  */
#define MCAN_RXESC_RBDS_20_BYTE               (MCAN_RXESC_RBDS_20_BYTE_Val << MCAN_RXESC_RBDS_Pos) /**< (MCAN_RXESC) 20-byte data field Position  */
#define MCAN_RXESC_RBDS_24_BYTE               (MCAN_RXESC_RBDS_24_BYTE_Val << MCAN_RXESC_RBDS_Pos) /**< (MCAN_RXESC) 24-byte data field Position  */
#define MCAN_RXESC_RBDS_32_BYTE               (MCAN_RXESC_RBDS_32_BYTE_Val << MCAN_RXESC_RBDS_Pos) /**< (MCAN_RXESC) 32-byte data field Position  */
#define MCAN_RXESC_RBDS_48_BYTE               (MCAN_RXESC_RBDS_48_BYTE_Val << MCAN_RXESC_RBDS_Pos) /**< (MCAN_RXESC) 48-byte data field Position  */
#define MCAN_RXESC_RBDS_64_BYTE               (MCAN_RXESC_RBDS_64_BYTE_Val << MCAN_RXESC_RBDS_Pos) /**< (MCAN_RXESC) 64-byte data field Position  */
#define MCAN_RXESC_Msk                        (0x00000777UL)                                 /**< (MCAN_RXESC) Register Mask  */

/* -------- MCAN_TXBC : (MCAN Offset: 0xC0) (R/W 32) Transmit Buffer Configuration Register -------- */
#define MCAN_TXBC_TBSA_Pos                    (2U)                                           /**< (MCAN_TXBC) Tx Buffers Start Address Position */
#define MCAN_TXBC_TBSA_Msk                    (0x3FFFU << MCAN_TXBC_TBSA_Pos)                /**< (MCAN_TXBC) Tx Buffers Start Address Mask */
#define MCAN_TXBC_TBSA(value)                 (MCAN_TXBC_TBSA_Msk & ((value) << MCAN_TXBC_TBSA_Pos))
#define MCAN_TXBC_NDTB_Pos                    (16U)                                          /**< (MCAN_TXBC) Number of Dedicated Transmit Buffers Position */
#define MCAN_TXBC_NDTB_Msk                    (0x3FU << MCAN_TXBC_NDTB_Pos)                  /**< (MCAN_TXBC) Number of Dedicated Transmit Buffers Mask */
#define MCAN_TXBC_NDTB(value)                 (MCAN_TXBC_NDTB_Msk & ((value) << MCAN_TXBC_NDTB_Pos))
#define MCAN_TXBC_TFQS_Pos                    (24U)                                          /**< (MCAN_TXBC) Transmit FIFO/Queue Size Position */
#define MCAN_TXBC_TFQS_Msk                    (0x3FU << MCAN_TXBC_TFQS_Pos)                  /**< (MCAN_TXBC) Transmit FIFO/Queue Size Mask */
#define MCAN_TXBC_TFQS(value)                 (MCAN_TXBC_TFQS_Msk & ((value) << MCAN_TXBC_TFQS_Pos))
#define MCAN_TXBC_TFQM_Pos                    (30U)                                          /**< (MCAN_TXBC) Tx FIFO/Queue Mode Position */
#define MCAN_TXBC_TFQM_Msk                    (0x1U << MCAN_TXBC_TFQM_Pos)                   /**< (MCAN_TXBC) Tx FIFO/Queue Mode Mask */
#define MCAN_TXBC_TFQM(value)                 (MCAN_TXBC_TFQM_Msk & ((value) << MCAN_TXBC_TFQM_Pos))
#define MCAN_TXBC_Msk                         (0x7F3FFFFCUL)                                 /**< (MCAN_TXBC) Register Mask  */

/* -------- MCAN_TXFQS : (MCAN Offset: 0xC4) (R/  32) Transmit FIFO/Queue Status Register -------- */
#define MCAN_TXFQS_TFFL_Pos                   (0U)                                           /**< (MCAN_TXFQS) Tx FIFO Free Level Position */
#define MCAN_TXFQS_TFFL_Msk                   (0x3FU << MCAN_TXFQS_TFFL_Pos)                 /**< (MCAN_TXFQS) Tx FIFO Free Level Mask */
#define MCAN_TXFQS_TFFL(value)                (MCAN_TXFQS_TFFL_Msk & ((value) << MCAN_TXFQS_TFFL_Pos))
#define MCAN_TXFQS_TFGI_Pos                   (8U)                                           /**< (MCAN_TXFQS) Tx FIFO Get Index Position */
#define MCAN_TXFQS_TFGI_Msk                   (0x1FU << MCAN_TXFQS_TFGI_Pos)                 /**< (MCAN_TXFQS) Tx FIFO Get Index Mask */
#define MCAN_TXFQS_TFGI(value)                (MCAN_TXFQS_TFGI_Msk & ((value) << MCAN_TXFQS_TFGI_Pos))
#define MCAN_TXFQS_TFQPI_Pos                  (16U)                                          /**< (MCAN_TXFQS) Tx FIFO/Queue Put Index Position */
#define MCAN_TXFQS_TFQPI_Msk                  (0x1FU << MCAN_TXFQS_TFQPI_Pos)                /**< (MCAN_TXFQS) Tx FIFO/Queue Put Index Mask */
#define MCAN_TXFQS_TFQPI(value)               (MCAN_TXFQS_TFQPI_Msk & ((value) << MCAN_TXFQS_TFQPI_Pos))
#define MCAN_TXFQS_TFQF_Pos                   (21U)                                          /**< (MCAN_TXFQS) Tx FIFO/Queue Full Position */
#define MCAN_TXFQS_TFQF_Msk                   (0x1U << MCAN_TXFQS_TFQF_Pos)                  /**< (MCAN_TXFQS) Tx FIFO/Queue Full Mask */
#define MCAN_TXFQS_TFQF(value)                (MCAN_TXFQS_TFQF_Msk & ((value) << MCAN_TXFQS_TFQF_Pos))
#define MCAN_TXFQS_Msk                        (0x003F1F3FUL)                                 /**< (MCAN_TXFQS) Register Mask  */

/* -------- MCAN_TXESC : (MCAN Offset: 0xC8) (R/W 32) Transmit Buffer Element Size Configuration Register -------- */
#define MCAN_TXESC_TBDS_Pos                   (0U)                                           /**< (MCAN_TXESC) Tx Buffer Data Field Size Position */
#define MCAN_TXESC_TBDS_Msk                   (0x7U << MCAN_TXESC_TBDS_Pos)                  /**< (MCAN_TXESC) Tx Buffer Data Field Size Mask */
#define MCAN_TXESC_TBDS(value)                (MCAN_TXESC_TBDS_Msk & ((value) << MCAN_TXESC_TBDS_Pos))
#define   MCAN_TXESC_TBDS_8_BYTE_Val          (0U)                                           /**< (MCAN_TXESC) 8-byte data field  */
#define   MCAN_TXESC_TBDS_12_BYTE_Val         (1U)                                           /**< (MCAN_TXESC) 12-byte data field  */
#define   MCAN_TXESC_TBDS_16_BYTE_Val         (2U)                                           /**< (MCAN_TXESC) 16-byte data field  */
#define   MCAN_TXESC_TBDS_20_BYTE_Val         (3U)                                           /**< (MCAN_TXESC) 20-byte data field  */
#define   MCAN_TXESC_TBDS_24_BYTE_Val         (4U)                                           /**< (MCAN_TXESC) 24-byte data field  */
#define   MCAN_TXESC_TBDS_32_BYTE_Val         (5U)                                           /**< (MCAN_TXESC) 32-byte data field  */
#define   MCAN_TXESC_TBDS_48_BYTE_Val         (6U)                                           /**< (MCAN_TXESC) 48-byte data field  */
#define   MCAN_TXESC_TBDS_64_BYTE_Val         (7U)                                           /**< (MCAN_TXESC) 64-byte data field  */
#define MCAN_TXESC_TBDS_8_BYTE                (MCAN_TXESC_TBDS_8_BYTE_Val << MCAN_TXESC_TBDS_Pos) /**< (MCAN_TXESC) 8-byte data field Position  */
#define MCAN_TXESC_TBDS_12_BYTE               (MCAN_TXESC_TBDS_12_BYTE_Val << MCAN_TXESC_TBDS_Pos) /**< (MCAN_TXESC) 12-byte data field Position  */
#define MCAN_TXESC_TBDS_16_BYTE               (MCAN_TXESC_TBDS_16_BYTE_Val << MCAN_TXESC_TBDS_Pos) /**< (MCAN_TXESC) 16-byte data field Position  */
#define MCAN_TXESC_TBDS_20_BYTE               (MCAN_TXESC_TBDS_20_BYTE_Val << MCAN_TXESC_TBDS_Pos) /**< (MCAN_TXESC) 20-byte data field Position  */
#define MCAN_TXESC_TBDS_24_BYTE               (MCAN_TXESC_TBDS_24_BYTE_Val << MCAN_TXESC_TBDS_Pos) /**< (MCAN_TXESC) 24-byte data field Position  */
#define MCAN_TXESC_TBDS_32_BYTE               (MCAN_TXESC_TBDS_32_BYTE_Val << MCAN_TXESC_TBDS_Pos) /**< (MCAN_TXESC) 32-byte data field Position  */
#define MCAN_TXESC_TBDS_48_BYTE               (MCAN_TXESC_TBDS_48_BYTE_Val << MCAN_TXESC_TBDS_Pos) /**< (MCAN_TXESC) 48-byte data field Position  */
#define MCAN_TXESC_TBDS_64_BYTE               (MCAN_TXESC_TBDS_64_BYTE_Val << MCAN_TXESC_TBDS_Pos) /**< (MCAN_TXESC) 64-byte data field Position  */
#define MCAN_TXESC_Msk                        (0x00000007UL)                                 /**< (MCAN_TXESC) Register Mask  */

/* -------- MCAN_TXBRP : (MCAN Offset: 0xCC) (R/  32) Transmit Buffer Request Pending Register -------- */
#define MCAN_TXBRP_TRP0_Pos                   (0U)                                           /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 0 Position */
#define MCAN_TXBRP_TRP0_Msk                   (0x1U << MCAN_TXBRP_TRP0_Pos)                  /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 0 Mask */
#define MCAN_TXBRP_TRP0(value)                (MCAN_TXBRP_TRP0_Msk & ((value) << MCAN_TXBRP_TRP0_Pos))
#define MCAN_TXBRP_TRP1_Pos                   (1U)                                           /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 1 Position */
#define MCAN_TXBRP_TRP1_Msk                   (0x1U << MCAN_TXBRP_TRP1_Pos)                  /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 1 Mask */
#define MCAN_TXBRP_TRP1(value)                (MCAN_TXBRP_TRP1_Msk & ((value) << MCAN_TXBRP_TRP1_Pos))
#define MCAN_TXBRP_TRP2_Pos                   (2U)                                           /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 2 Position */
#define MCAN_TXBRP_TRP2_Msk                   (0x1U << MCAN_TXBRP_TRP2_Pos)                  /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 2 Mask */
#define MCAN_TXBRP_TRP2(value)                (MCAN_TXBRP_TRP2_Msk & ((value) << MCAN_TXBRP_TRP2_Pos))
#define MCAN_TXBRP_TRP3_Pos                   (3U)                                           /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 3 Position */
#define MCAN_TXBRP_TRP3_Msk                   (0x1U << MCAN_TXBRP_TRP3_Pos)                  /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 3 Mask */
#define MCAN_TXBRP_TRP3(value)                (MCAN_TXBRP_TRP3_Msk & ((value) << MCAN_TXBRP_TRP3_Pos))
#define MCAN_TXBRP_TRP4_Pos                   (4U)                                           /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 4 Position */
#define MCAN_TXBRP_TRP4_Msk                   (0x1U << MCAN_TXBRP_TRP4_Pos)                  /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 4 Mask */
#define MCAN_TXBRP_TRP4(value)                (MCAN_TXBRP_TRP4_Msk & ((value) << MCAN_TXBRP_TRP4_Pos))
#define MCAN_TXBRP_TRP5_Pos                   (5U)                                           /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 5 Position */
#define MCAN_TXBRP_TRP5_Msk                   (0x1U << MCAN_TXBRP_TRP5_Pos)                  /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 5 Mask */
#define MCAN_TXBRP_TRP5(value)                (MCAN_TXBRP_TRP5_Msk & ((value) << MCAN_TXBRP_TRP5_Pos))
#define MCAN_TXBRP_TRP6_Pos                   (6U)                                           /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 6 Position */
#define MCAN_TXBRP_TRP6_Msk                   (0x1U << MCAN_TXBRP_TRP6_Pos)                  /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 6 Mask */
#define MCAN_TXBRP_TRP6(value)                (MCAN_TXBRP_TRP6_Msk & ((value) << MCAN_TXBRP_TRP6_Pos))
#define MCAN_TXBRP_TRP7_Pos                   (7U)                                           /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 7 Position */
#define MCAN_TXBRP_TRP7_Msk                   (0x1U << MCAN_TXBRP_TRP7_Pos)                  /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 7 Mask */
#define MCAN_TXBRP_TRP7(value)                (MCAN_TXBRP_TRP7_Msk & ((value) << MCAN_TXBRP_TRP7_Pos))
#define MCAN_TXBRP_TRP8_Pos                   (8U)                                           /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 8 Position */
#define MCAN_TXBRP_TRP8_Msk                   (0x1U << MCAN_TXBRP_TRP8_Pos)                  /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 8 Mask */
#define MCAN_TXBRP_TRP8(value)                (MCAN_TXBRP_TRP8_Msk & ((value) << MCAN_TXBRP_TRP8_Pos))
#define MCAN_TXBRP_TRP9_Pos                   (9U)                                           /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 9 Position */
#define MCAN_TXBRP_TRP9_Msk                   (0x1U << MCAN_TXBRP_TRP9_Pos)                  /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 9 Mask */
#define MCAN_TXBRP_TRP9(value)                (MCAN_TXBRP_TRP9_Msk & ((value) << MCAN_TXBRP_TRP9_Pos))
#define MCAN_TXBRP_TRP10_Pos                  (10U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 10 Position */
#define MCAN_TXBRP_TRP10_Msk                  (0x1U << MCAN_TXBRP_TRP10_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 10 Mask */
#define MCAN_TXBRP_TRP10(value)               (MCAN_TXBRP_TRP10_Msk & ((value) << MCAN_TXBRP_TRP10_Pos))
#define MCAN_TXBRP_TRP11_Pos                  (11U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 11 Position */
#define MCAN_TXBRP_TRP11_Msk                  (0x1U << MCAN_TXBRP_TRP11_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 11 Mask */
#define MCAN_TXBRP_TRP11(value)               (MCAN_TXBRP_TRP11_Msk & ((value) << MCAN_TXBRP_TRP11_Pos))
#define MCAN_TXBRP_TRP12_Pos                  (12U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 12 Position */
#define MCAN_TXBRP_TRP12_Msk                  (0x1U << MCAN_TXBRP_TRP12_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 12 Mask */
#define MCAN_TXBRP_TRP12(value)               (MCAN_TXBRP_TRP12_Msk & ((value) << MCAN_TXBRP_TRP12_Pos))
#define MCAN_TXBRP_TRP13_Pos                  (13U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 13 Position */
#define MCAN_TXBRP_TRP13_Msk                  (0x1U << MCAN_TXBRP_TRP13_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 13 Mask */
#define MCAN_TXBRP_TRP13(value)               (MCAN_TXBRP_TRP13_Msk & ((value) << MCAN_TXBRP_TRP13_Pos))
#define MCAN_TXBRP_TRP14_Pos                  (14U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 14 Position */
#define MCAN_TXBRP_TRP14_Msk                  (0x1U << MCAN_TXBRP_TRP14_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 14 Mask */
#define MCAN_TXBRP_TRP14(value)               (MCAN_TXBRP_TRP14_Msk & ((value) << MCAN_TXBRP_TRP14_Pos))
#define MCAN_TXBRP_TRP15_Pos                  (15U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 15 Position */
#define MCAN_TXBRP_TRP15_Msk                  (0x1U << MCAN_TXBRP_TRP15_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 15 Mask */
#define MCAN_TXBRP_TRP15(value)               (MCAN_TXBRP_TRP15_Msk & ((value) << MCAN_TXBRP_TRP15_Pos))
#define MCAN_TXBRP_TRP16_Pos                  (16U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 16 Position */
#define MCAN_TXBRP_TRP16_Msk                  (0x1U << MCAN_TXBRP_TRP16_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 16 Mask */
#define MCAN_TXBRP_TRP16(value)               (MCAN_TXBRP_TRP16_Msk & ((value) << MCAN_TXBRP_TRP16_Pos))
#define MCAN_TXBRP_TRP17_Pos                  (17U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 17 Position */
#define MCAN_TXBRP_TRP17_Msk                  (0x1U << MCAN_TXBRP_TRP17_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 17 Mask */
#define MCAN_TXBRP_TRP17(value)               (MCAN_TXBRP_TRP17_Msk & ((value) << MCAN_TXBRP_TRP17_Pos))
#define MCAN_TXBRP_TRP18_Pos                  (18U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 18 Position */
#define MCAN_TXBRP_TRP18_Msk                  (0x1U << MCAN_TXBRP_TRP18_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 18 Mask */
#define MCAN_TXBRP_TRP18(value)               (MCAN_TXBRP_TRP18_Msk & ((value) << MCAN_TXBRP_TRP18_Pos))
#define MCAN_TXBRP_TRP19_Pos                  (19U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 19 Position */
#define MCAN_TXBRP_TRP19_Msk                  (0x1U << MCAN_TXBRP_TRP19_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 19 Mask */
#define MCAN_TXBRP_TRP19(value)               (MCAN_TXBRP_TRP19_Msk & ((value) << MCAN_TXBRP_TRP19_Pos))
#define MCAN_TXBRP_TRP20_Pos                  (20U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 20 Position */
#define MCAN_TXBRP_TRP20_Msk                  (0x1U << MCAN_TXBRP_TRP20_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 20 Mask */
#define MCAN_TXBRP_TRP20(value)               (MCAN_TXBRP_TRP20_Msk & ((value) << MCAN_TXBRP_TRP20_Pos))
#define MCAN_TXBRP_TRP21_Pos                  (21U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 21 Position */
#define MCAN_TXBRP_TRP21_Msk                  (0x1U << MCAN_TXBRP_TRP21_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 21 Mask */
#define MCAN_TXBRP_TRP21(value)               (MCAN_TXBRP_TRP21_Msk & ((value) << MCAN_TXBRP_TRP21_Pos))
#define MCAN_TXBRP_TRP22_Pos                  (22U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 22 Position */
#define MCAN_TXBRP_TRP22_Msk                  (0x1U << MCAN_TXBRP_TRP22_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 22 Mask */
#define MCAN_TXBRP_TRP22(value)               (MCAN_TXBRP_TRP22_Msk & ((value) << MCAN_TXBRP_TRP22_Pos))
#define MCAN_TXBRP_TRP23_Pos                  (23U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 23 Position */
#define MCAN_TXBRP_TRP23_Msk                  (0x1U << MCAN_TXBRP_TRP23_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 23 Mask */
#define MCAN_TXBRP_TRP23(value)               (MCAN_TXBRP_TRP23_Msk & ((value) << MCAN_TXBRP_TRP23_Pos))
#define MCAN_TXBRP_TRP24_Pos                  (24U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 24 Position */
#define MCAN_TXBRP_TRP24_Msk                  (0x1U << MCAN_TXBRP_TRP24_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 24 Mask */
#define MCAN_TXBRP_TRP24(value)               (MCAN_TXBRP_TRP24_Msk & ((value) << MCAN_TXBRP_TRP24_Pos))
#define MCAN_TXBRP_TRP25_Pos                  (25U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 25 Position */
#define MCAN_TXBRP_TRP25_Msk                  (0x1U << MCAN_TXBRP_TRP25_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 25 Mask */
#define MCAN_TXBRP_TRP25(value)               (MCAN_TXBRP_TRP25_Msk & ((value) << MCAN_TXBRP_TRP25_Pos))
#define MCAN_TXBRP_TRP26_Pos                  (26U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 26 Position */
#define MCAN_TXBRP_TRP26_Msk                  (0x1U << MCAN_TXBRP_TRP26_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 26 Mask */
#define MCAN_TXBRP_TRP26(value)               (MCAN_TXBRP_TRP26_Msk & ((value) << MCAN_TXBRP_TRP26_Pos))
#define MCAN_TXBRP_TRP27_Pos                  (27U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 27 Position */
#define MCAN_TXBRP_TRP27_Msk                  (0x1U << MCAN_TXBRP_TRP27_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 27 Mask */
#define MCAN_TXBRP_TRP27(value)               (MCAN_TXBRP_TRP27_Msk & ((value) << MCAN_TXBRP_TRP27_Pos))
#define MCAN_TXBRP_TRP28_Pos                  (28U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 28 Position */
#define MCAN_TXBRP_TRP28_Msk                  (0x1U << MCAN_TXBRP_TRP28_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 28 Mask */
#define MCAN_TXBRP_TRP28(value)               (MCAN_TXBRP_TRP28_Msk & ((value) << MCAN_TXBRP_TRP28_Pos))
#define MCAN_TXBRP_TRP29_Pos                  (29U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 29 Position */
#define MCAN_TXBRP_TRP29_Msk                  (0x1U << MCAN_TXBRP_TRP29_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 29 Mask */
#define MCAN_TXBRP_TRP29(value)               (MCAN_TXBRP_TRP29_Msk & ((value) << MCAN_TXBRP_TRP29_Pos))
#define MCAN_TXBRP_TRP30_Pos                  (30U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 30 Position */
#define MCAN_TXBRP_TRP30_Msk                  (0x1U << MCAN_TXBRP_TRP30_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 30 Mask */
#define MCAN_TXBRP_TRP30(value)               (MCAN_TXBRP_TRP30_Msk & ((value) << MCAN_TXBRP_TRP30_Pos))
#define MCAN_TXBRP_TRP31_Pos                  (31U)                                          /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 31 Position */
#define MCAN_TXBRP_TRP31_Msk                  (0x1U << MCAN_TXBRP_TRP31_Pos)                 /**< (MCAN_TXBRP) Transmission Request Pending for Buffer 31 Mask */
#define MCAN_TXBRP_TRP31(value)               (MCAN_TXBRP_TRP31_Msk & ((value) << MCAN_TXBRP_TRP31_Pos))
#define MCAN_TXBRP_Msk                        (0xFFFFFFFFUL)                                 /**< (MCAN_TXBRP) Register Mask  */

/* -------- MCAN_TXBAR : (MCAN Offset: 0xD0) (R/W 32) Transmit Buffer Add Request Register -------- */
#define MCAN_TXBAR_AR0_Pos                    (0U)                                           /**< (MCAN_TXBAR) Add Request for Transmit Buffer 0 Position */
#define MCAN_TXBAR_AR0_Msk                    (0x1U << MCAN_TXBAR_AR0_Pos)                   /**< (MCAN_TXBAR) Add Request for Transmit Buffer 0 Mask */
#define MCAN_TXBAR_AR0(value)                 (MCAN_TXBAR_AR0_Msk & ((value) << MCAN_TXBAR_AR0_Pos))
#define MCAN_TXBAR_AR1_Pos                    (1U)                                           /**< (MCAN_TXBAR) Add Request for Transmit Buffer 1 Position */
#define MCAN_TXBAR_AR1_Msk                    (0x1U << MCAN_TXBAR_AR1_Pos)                   /**< (MCAN_TXBAR) Add Request for Transmit Buffer 1 Mask */
#define MCAN_TXBAR_AR1(value)                 (MCAN_TXBAR_AR1_Msk & ((value) << MCAN_TXBAR_AR1_Pos))
#define MCAN_TXBAR_AR2_Pos                    (2U)                                           /**< (MCAN_TXBAR) Add Request for Transmit Buffer 2 Position */
#define MCAN_TXBAR_AR2_Msk                    (0x1U << MCAN_TXBAR_AR2_Pos)                   /**< (MCAN_TXBAR) Add Request for Transmit Buffer 2 Mask */
#define MCAN_TXBAR_AR2(value)                 (MCAN_TXBAR_AR2_Msk & ((value) << MCAN_TXBAR_AR2_Pos))
#define MCAN_TXBAR_AR3_Pos                    (3U)                                           /**< (MCAN_TXBAR) Add Request for Transmit Buffer 3 Position */
#define MCAN_TXBAR_AR3_Msk                    (0x1U << MCAN_TXBAR_AR3_Pos)                   /**< (MCAN_TXBAR) Add Request for Transmit Buffer 3 Mask */
#define MCAN_TXBAR_AR3(value)                 (MCAN_TXBAR_AR3_Msk & ((value) << MCAN_TXBAR_AR3_Pos))
#define MCAN_TXBAR_AR4_Pos                    (4U)                                           /**< (MCAN_TXBAR) Add Request for Transmit Buffer 4 Position */
#define MCAN_TXBAR_AR4_Msk                    (0x1U << MCAN_TXBAR_AR4_Pos)                   /**< (MCAN_TXBAR) Add Request for Transmit Buffer 4 Mask */
#define MCAN_TXBAR_AR4(value)                 (MCAN_TXBAR_AR4_Msk & ((value) << MCAN_TXBAR_AR4_Pos))
#define MCAN_TXBAR_AR5_Pos                    (5U)                                           /**< (MCAN_TXBAR) Add Request for Transmit Buffer 5 Position */
#define MCAN_TXBAR_AR5_Msk                    (0x1U << MCAN_TXBAR_AR5_Pos)                   /**< (MCAN_TXBAR) Add Request for Transmit Buffer 5 Mask */
#define MCAN_TXBAR_AR5(value)                 (MCAN_TXBAR_AR5_Msk & ((value) << MCAN_TXBAR_AR5_Pos))
#define MCAN_TXBAR_AR6_Pos                    (6U)                                           /**< (MCAN_TXBAR) Add Request for Transmit Buffer 6 Position */
#define MCAN_TXBAR_AR6_Msk                    (0x1U << MCAN_TXBAR_AR6_Pos)                   /**< (MCAN_TXBAR) Add Request for Transmit Buffer 6 Mask */
#define MCAN_TXBAR_AR6(value)                 (MCAN_TXBAR_AR6_Msk & ((value) << MCAN_TXBAR_AR6_Pos))
#define MCAN_TXBAR_AR7_Pos                    (7U)                                           /**< (MCAN_TXBAR) Add Request for Transmit Buffer 7 Position */
#define MCAN_TXBAR_AR7_Msk                    (0x1U << MCAN_TXBAR_AR7_Pos)                   /**< (MCAN_TXBAR) Add Request for Transmit Buffer 7 Mask */
#define MCAN_TXBAR_AR7(value)                 (MCAN_TXBAR_AR7_Msk & ((value) << MCAN_TXBAR_AR7_Pos))
#define MCAN_TXBAR_AR8_Pos                    (8U)                                           /**< (MCAN_TXBAR) Add Request for Transmit Buffer 8 Position */
#define MCAN_TXBAR_AR8_Msk                    (0x1U << MCAN_TXBAR_AR8_Pos)                   /**< (MCAN_TXBAR) Add Request for Transmit Buffer 8 Mask */
#define MCAN_TXBAR_AR8(value)                 (MCAN_TXBAR_AR8_Msk & ((value) << MCAN_TXBAR_AR8_Pos))
#define MCAN_TXBAR_AR9_Pos                    (9U)                                           /**< (MCAN_TXBAR) Add Request for Transmit Buffer 9 Position */
#define MCAN_TXBAR_AR9_Msk                    (0x1U << MCAN_TXBAR_AR9_Pos)                   /**< (MCAN_TXBAR) Add Request for Transmit Buffer 9 Mask */
#define MCAN_TXBAR_AR9(value)                 (MCAN_TXBAR_AR9_Msk & ((value) << MCAN_TXBAR_AR9_Pos))
#define MCAN_TXBAR_AR10_Pos                   (10U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 10 Position */
#define MCAN_TXBAR_AR10_Msk                   (0x1U << MCAN_TXBAR_AR10_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 10 Mask */
#define MCAN_TXBAR_AR10(value)                (MCAN_TXBAR_AR10_Msk & ((value) << MCAN_TXBAR_AR10_Pos))
#define MCAN_TXBAR_AR11_Pos                   (11U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 11 Position */
#define MCAN_TXBAR_AR11_Msk                   (0x1U << MCAN_TXBAR_AR11_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 11 Mask */
#define MCAN_TXBAR_AR11(value)                (MCAN_TXBAR_AR11_Msk & ((value) << MCAN_TXBAR_AR11_Pos))
#define MCAN_TXBAR_AR12_Pos                   (12U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 12 Position */
#define MCAN_TXBAR_AR12_Msk                   (0x1U << MCAN_TXBAR_AR12_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 12 Mask */
#define MCAN_TXBAR_AR12(value)                (MCAN_TXBAR_AR12_Msk & ((value) << MCAN_TXBAR_AR12_Pos))
#define MCAN_TXBAR_AR13_Pos                   (13U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 13 Position */
#define MCAN_TXBAR_AR13_Msk                   (0x1U << MCAN_TXBAR_AR13_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 13 Mask */
#define MCAN_TXBAR_AR13(value)                (MCAN_TXBAR_AR13_Msk & ((value) << MCAN_TXBAR_AR13_Pos))
#define MCAN_TXBAR_AR14_Pos                   (14U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 14 Position */
#define MCAN_TXBAR_AR14_Msk                   (0x1U << MCAN_TXBAR_AR14_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 14 Mask */
#define MCAN_TXBAR_AR14(value)                (MCAN_TXBAR_AR14_Msk & ((value) << MCAN_TXBAR_AR14_Pos))
#define MCAN_TXBAR_AR15_Pos                   (15U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 15 Position */
#define MCAN_TXBAR_AR15_Msk                   (0x1U << MCAN_TXBAR_AR15_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 15 Mask */
#define MCAN_TXBAR_AR15(value)                (MCAN_TXBAR_AR15_Msk & ((value) << MCAN_TXBAR_AR15_Pos))
#define MCAN_TXBAR_AR16_Pos                   (16U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 16 Position */
#define MCAN_TXBAR_AR16_Msk                   (0x1U << MCAN_TXBAR_AR16_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 16 Mask */
#define MCAN_TXBAR_AR16(value)                (MCAN_TXBAR_AR16_Msk & ((value) << MCAN_TXBAR_AR16_Pos))
#define MCAN_TXBAR_AR17_Pos                   (17U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 17 Position */
#define MCAN_TXBAR_AR17_Msk                   (0x1U << MCAN_TXBAR_AR17_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 17 Mask */
#define MCAN_TXBAR_AR17(value)                (MCAN_TXBAR_AR17_Msk & ((value) << MCAN_TXBAR_AR17_Pos))
#define MCAN_TXBAR_AR18_Pos                   (18U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 18 Position */
#define MCAN_TXBAR_AR18_Msk                   (0x1U << MCAN_TXBAR_AR18_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 18 Mask */
#define MCAN_TXBAR_AR18(value)                (MCAN_TXBAR_AR18_Msk & ((value) << MCAN_TXBAR_AR18_Pos))
#define MCAN_TXBAR_AR19_Pos                   (19U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 19 Position */
#define MCAN_TXBAR_AR19_Msk                   (0x1U << MCAN_TXBAR_AR19_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 19 Mask */
#define MCAN_TXBAR_AR19(value)                (MCAN_TXBAR_AR19_Msk & ((value) << MCAN_TXBAR_AR19_Pos))
#define MCAN_TXBAR_AR20_Pos                   (20U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 20 Position */
#define MCAN_TXBAR_AR20_Msk                   (0x1U << MCAN_TXBAR_AR20_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 20 Mask */
#define MCAN_TXBAR_AR20(value)                (MCAN_TXBAR_AR20_Msk & ((value) << MCAN_TXBAR_AR20_Pos))
#define MCAN_TXBAR_AR21_Pos                   (21U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 21 Position */
#define MCAN_TXBAR_AR21_Msk                   (0x1U << MCAN_TXBAR_AR21_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 21 Mask */
#define MCAN_TXBAR_AR21(value)                (MCAN_TXBAR_AR21_Msk & ((value) << MCAN_TXBAR_AR21_Pos))
#define MCAN_TXBAR_AR22_Pos                   (22U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 22 Position */
#define MCAN_TXBAR_AR22_Msk                   (0x1U << MCAN_TXBAR_AR22_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 22 Mask */
#define MCAN_TXBAR_AR22(value)                (MCAN_TXBAR_AR22_Msk & ((value) << MCAN_TXBAR_AR22_Pos))
#define MCAN_TXBAR_AR23_Pos                   (23U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 23 Position */
#define MCAN_TXBAR_AR23_Msk                   (0x1U << MCAN_TXBAR_AR23_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 23 Mask */
#define MCAN_TXBAR_AR23(value)                (MCAN_TXBAR_AR23_Msk & ((value) << MCAN_TXBAR_AR23_Pos))
#define MCAN_TXBAR_AR24_Pos                   (24U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 24 Position */
#define MCAN_TXBAR_AR24_Msk                   (0x1U << MCAN_TXBAR_AR24_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 24 Mask */
#define MCAN_TXBAR_AR24(value)                (MCAN_TXBAR_AR24_Msk & ((value) << MCAN_TXBAR_AR24_Pos))
#define MCAN_TXBAR_AR25_Pos                   (25U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 25 Position */
#define MCAN_TXBAR_AR25_Msk                   (0x1U << MCAN_TXBAR_AR25_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 25 Mask */
#define MCAN_TXBAR_AR25(value)                (MCAN_TXBAR_AR25_Msk & ((value) << MCAN_TXBAR_AR25_Pos))
#define MCAN_TXBAR_AR26_Pos                   (26U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 26 Position */
#define MCAN_TXBAR_AR26_Msk                   (0x1U << MCAN_TXBAR_AR26_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 26 Mask */
#define MCAN_TXBAR_AR26(value)                (MCAN_TXBAR_AR26_Msk & ((value) << MCAN_TXBAR_AR26_Pos))
#define MCAN_TXBAR_AR27_Pos                   (27U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 27 Position */
#define MCAN_TXBAR_AR27_Msk                   (0x1U << MCAN_TXBAR_AR27_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 27 Mask */
#define MCAN_TXBAR_AR27(value)                (MCAN_TXBAR_AR27_Msk & ((value) << MCAN_TXBAR_AR27_Pos))
#define MCAN_TXBAR_AR28_Pos                   (28U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 28 Position */
#define MCAN_TXBAR_AR28_Msk                   (0x1U << MCAN_TXBAR_AR28_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 28 Mask */
#define MCAN_TXBAR_AR28(value)                (MCAN_TXBAR_AR28_Msk & ((value) << MCAN_TXBAR_AR28_Pos))
#define MCAN_TXBAR_AR29_Pos                   (29U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 29 Position */
#define MCAN_TXBAR_AR29_Msk                   (0x1U << MCAN_TXBAR_AR29_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 29 Mask */
#define MCAN_TXBAR_AR29(value)                (MCAN_TXBAR_AR29_Msk & ((value) << MCAN_TXBAR_AR29_Pos))
#define MCAN_TXBAR_AR30_Pos                   (30U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 30 Position */
#define MCAN_TXBAR_AR30_Msk                   (0x1U << MCAN_TXBAR_AR30_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 30 Mask */
#define MCAN_TXBAR_AR30(value)                (MCAN_TXBAR_AR30_Msk & ((value) << MCAN_TXBAR_AR30_Pos))
#define MCAN_TXBAR_AR31_Pos                   (31U)                                          /**< (MCAN_TXBAR) Add Request for Transmit Buffer 31 Position */
#define MCAN_TXBAR_AR31_Msk                   (0x1U << MCAN_TXBAR_AR31_Pos)                  /**< (MCAN_TXBAR) Add Request for Transmit Buffer 31 Mask */
#define MCAN_TXBAR_AR31(value)                (MCAN_TXBAR_AR31_Msk & ((value) << MCAN_TXBAR_AR31_Pos))
#define MCAN_TXBAR_Msk                        (0xFFFFFFFFUL)                                 /**< (MCAN_TXBAR) Register Mask  */

/* -------- MCAN_TXBCR : (MCAN Offset: 0xD4) (R/W 32) Transmit Buffer Cancellation Request Register -------- */
#define MCAN_TXBCR_CR0_Pos                    (0U)                                           /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 0 Position */
#define MCAN_TXBCR_CR0_Msk                    (0x1U << MCAN_TXBCR_CR0_Pos)                   /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 0 Mask */
#define MCAN_TXBCR_CR0(value)                 (MCAN_TXBCR_CR0_Msk & ((value) << MCAN_TXBCR_CR0_Pos))
#define MCAN_TXBCR_CR1_Pos                    (1U)                                           /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 1 Position */
#define MCAN_TXBCR_CR1_Msk                    (0x1U << MCAN_TXBCR_CR1_Pos)                   /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 1 Mask */
#define MCAN_TXBCR_CR1(value)                 (MCAN_TXBCR_CR1_Msk & ((value) << MCAN_TXBCR_CR1_Pos))
#define MCAN_TXBCR_CR2_Pos                    (2U)                                           /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 2 Position */
#define MCAN_TXBCR_CR2_Msk                    (0x1U << MCAN_TXBCR_CR2_Pos)                   /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 2 Mask */
#define MCAN_TXBCR_CR2(value)                 (MCAN_TXBCR_CR2_Msk & ((value) << MCAN_TXBCR_CR2_Pos))
#define MCAN_TXBCR_CR3_Pos                    (3U)                                           /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 3 Position */
#define MCAN_TXBCR_CR3_Msk                    (0x1U << MCAN_TXBCR_CR3_Pos)                   /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 3 Mask */
#define MCAN_TXBCR_CR3(value)                 (MCAN_TXBCR_CR3_Msk & ((value) << MCAN_TXBCR_CR3_Pos))
#define MCAN_TXBCR_CR4_Pos                    (4U)                                           /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 4 Position */
#define MCAN_TXBCR_CR4_Msk                    (0x1U << MCAN_TXBCR_CR4_Pos)                   /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 4 Mask */
#define MCAN_TXBCR_CR4(value)                 (MCAN_TXBCR_CR4_Msk & ((value) << MCAN_TXBCR_CR4_Pos))
#define MCAN_TXBCR_CR5_Pos                    (5U)                                           /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 5 Position */
#define MCAN_TXBCR_CR5_Msk                    (0x1U << MCAN_TXBCR_CR5_Pos)                   /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 5 Mask */
#define MCAN_TXBCR_CR5(value)                 (MCAN_TXBCR_CR5_Msk & ((value) << MCAN_TXBCR_CR5_Pos))
#define MCAN_TXBCR_CR6_Pos                    (6U)                                           /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 6 Position */
#define MCAN_TXBCR_CR6_Msk                    (0x1U << MCAN_TXBCR_CR6_Pos)                   /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 6 Mask */
#define MCAN_TXBCR_CR6(value)                 (MCAN_TXBCR_CR6_Msk & ((value) << MCAN_TXBCR_CR6_Pos))
#define MCAN_TXBCR_CR7_Pos                    (7U)                                           /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 7 Position */
#define MCAN_TXBCR_CR7_Msk                    (0x1U << MCAN_TXBCR_CR7_Pos)                   /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 7 Mask */
#define MCAN_TXBCR_CR7(value)                 (MCAN_TXBCR_CR7_Msk & ((value) << MCAN_TXBCR_CR7_Pos))
#define MCAN_TXBCR_CR8_Pos                    (8U)                                           /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 8 Position */
#define MCAN_TXBCR_CR8_Msk                    (0x1U << MCAN_TXBCR_CR8_Pos)                   /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 8 Mask */
#define MCAN_TXBCR_CR8(value)                 (MCAN_TXBCR_CR8_Msk & ((value) << MCAN_TXBCR_CR8_Pos))
#define MCAN_TXBCR_CR9_Pos                    (9U)                                           /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 9 Position */
#define MCAN_TXBCR_CR9_Msk                    (0x1U << MCAN_TXBCR_CR9_Pos)                   /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 9 Mask */
#define MCAN_TXBCR_CR9(value)                 (MCAN_TXBCR_CR9_Msk & ((value) << MCAN_TXBCR_CR9_Pos))
#define MCAN_TXBCR_CR10_Pos                   (10U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 10 Position */
#define MCAN_TXBCR_CR10_Msk                   (0x1U << MCAN_TXBCR_CR10_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 10 Mask */
#define MCAN_TXBCR_CR10(value)                (MCAN_TXBCR_CR10_Msk & ((value) << MCAN_TXBCR_CR10_Pos))
#define MCAN_TXBCR_CR11_Pos                   (11U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 11 Position */
#define MCAN_TXBCR_CR11_Msk                   (0x1U << MCAN_TXBCR_CR11_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 11 Mask */
#define MCAN_TXBCR_CR11(value)                (MCAN_TXBCR_CR11_Msk & ((value) << MCAN_TXBCR_CR11_Pos))
#define MCAN_TXBCR_CR12_Pos                   (12U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 12 Position */
#define MCAN_TXBCR_CR12_Msk                   (0x1U << MCAN_TXBCR_CR12_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 12 Mask */
#define MCAN_TXBCR_CR12(value)                (MCAN_TXBCR_CR12_Msk & ((value) << MCAN_TXBCR_CR12_Pos))
#define MCAN_TXBCR_CR13_Pos                   (13U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 13 Position */
#define MCAN_TXBCR_CR13_Msk                   (0x1U << MCAN_TXBCR_CR13_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 13 Mask */
#define MCAN_TXBCR_CR13(value)                (MCAN_TXBCR_CR13_Msk & ((value) << MCAN_TXBCR_CR13_Pos))
#define MCAN_TXBCR_CR14_Pos                   (14U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 14 Position */
#define MCAN_TXBCR_CR14_Msk                   (0x1U << MCAN_TXBCR_CR14_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 14 Mask */
#define MCAN_TXBCR_CR14(value)                (MCAN_TXBCR_CR14_Msk & ((value) << MCAN_TXBCR_CR14_Pos))
#define MCAN_TXBCR_CR15_Pos                   (15U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 15 Position */
#define MCAN_TXBCR_CR15_Msk                   (0x1U << MCAN_TXBCR_CR15_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 15 Mask */
#define MCAN_TXBCR_CR15(value)                (MCAN_TXBCR_CR15_Msk & ((value) << MCAN_TXBCR_CR15_Pos))
#define MCAN_TXBCR_CR16_Pos                   (16U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 16 Position */
#define MCAN_TXBCR_CR16_Msk                   (0x1U << MCAN_TXBCR_CR16_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 16 Mask */
#define MCAN_TXBCR_CR16(value)                (MCAN_TXBCR_CR16_Msk & ((value) << MCAN_TXBCR_CR16_Pos))
#define MCAN_TXBCR_CR17_Pos                   (17U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 17 Position */
#define MCAN_TXBCR_CR17_Msk                   (0x1U << MCAN_TXBCR_CR17_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 17 Mask */
#define MCAN_TXBCR_CR17(value)                (MCAN_TXBCR_CR17_Msk & ((value) << MCAN_TXBCR_CR17_Pos))
#define MCAN_TXBCR_CR18_Pos                   (18U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 18 Position */
#define MCAN_TXBCR_CR18_Msk                   (0x1U << MCAN_TXBCR_CR18_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 18 Mask */
#define MCAN_TXBCR_CR18(value)                (MCAN_TXBCR_CR18_Msk & ((value) << MCAN_TXBCR_CR18_Pos))
#define MCAN_TXBCR_CR19_Pos                   (19U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 19 Position */
#define MCAN_TXBCR_CR19_Msk                   (0x1U << MCAN_TXBCR_CR19_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 19 Mask */
#define MCAN_TXBCR_CR19(value)                (MCAN_TXBCR_CR19_Msk & ((value) << MCAN_TXBCR_CR19_Pos))
#define MCAN_TXBCR_CR20_Pos                   (20U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 20 Position */
#define MCAN_TXBCR_CR20_Msk                   (0x1U << MCAN_TXBCR_CR20_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 20 Mask */
#define MCAN_TXBCR_CR20(value)                (MCAN_TXBCR_CR20_Msk & ((value) << MCAN_TXBCR_CR20_Pos))
#define MCAN_TXBCR_CR21_Pos                   (21U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 21 Position */
#define MCAN_TXBCR_CR21_Msk                   (0x1U << MCAN_TXBCR_CR21_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 21 Mask */
#define MCAN_TXBCR_CR21(value)                (MCAN_TXBCR_CR21_Msk & ((value) << MCAN_TXBCR_CR21_Pos))
#define MCAN_TXBCR_CR22_Pos                   (22U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 22 Position */
#define MCAN_TXBCR_CR22_Msk                   (0x1U << MCAN_TXBCR_CR22_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 22 Mask */
#define MCAN_TXBCR_CR22(value)                (MCAN_TXBCR_CR22_Msk & ((value) << MCAN_TXBCR_CR22_Pos))
#define MCAN_TXBCR_CR23_Pos                   (23U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 23 Position */
#define MCAN_TXBCR_CR23_Msk                   (0x1U << MCAN_TXBCR_CR23_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 23 Mask */
#define MCAN_TXBCR_CR23(value)                (MCAN_TXBCR_CR23_Msk & ((value) << MCAN_TXBCR_CR23_Pos))
#define MCAN_TXBCR_CR24_Pos                   (24U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 24 Position */
#define MCAN_TXBCR_CR24_Msk                   (0x1U << MCAN_TXBCR_CR24_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 24 Mask */
#define MCAN_TXBCR_CR24(value)                (MCAN_TXBCR_CR24_Msk & ((value) << MCAN_TXBCR_CR24_Pos))
#define MCAN_TXBCR_CR25_Pos                   (25U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 25 Position */
#define MCAN_TXBCR_CR25_Msk                   (0x1U << MCAN_TXBCR_CR25_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 25 Mask */
#define MCAN_TXBCR_CR25(value)                (MCAN_TXBCR_CR25_Msk & ((value) << MCAN_TXBCR_CR25_Pos))
#define MCAN_TXBCR_CR26_Pos                   (26U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 26 Position */
#define MCAN_TXBCR_CR26_Msk                   (0x1U << MCAN_TXBCR_CR26_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 26 Mask */
#define MCAN_TXBCR_CR26(value)                (MCAN_TXBCR_CR26_Msk & ((value) << MCAN_TXBCR_CR26_Pos))
#define MCAN_TXBCR_CR27_Pos                   (27U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 27 Position */
#define MCAN_TXBCR_CR27_Msk                   (0x1U << MCAN_TXBCR_CR27_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 27 Mask */
#define MCAN_TXBCR_CR27(value)                (MCAN_TXBCR_CR27_Msk & ((value) << MCAN_TXBCR_CR27_Pos))
#define MCAN_TXBCR_CR28_Pos                   (28U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 28 Position */
#define MCAN_TXBCR_CR28_Msk                   (0x1U << MCAN_TXBCR_CR28_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 28 Mask */
#define MCAN_TXBCR_CR28(value)                (MCAN_TXBCR_CR28_Msk & ((value) << MCAN_TXBCR_CR28_Pos))
#define MCAN_TXBCR_CR29_Pos                   (29U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 29 Position */
#define MCAN_TXBCR_CR29_Msk                   (0x1U << MCAN_TXBCR_CR29_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 29 Mask */
#define MCAN_TXBCR_CR29(value)                (MCAN_TXBCR_CR29_Msk & ((value) << MCAN_TXBCR_CR29_Pos))
#define MCAN_TXBCR_CR30_Pos                   (30U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 30 Position */
#define MCAN_TXBCR_CR30_Msk                   (0x1U << MCAN_TXBCR_CR30_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 30 Mask */
#define MCAN_TXBCR_CR30(value)                (MCAN_TXBCR_CR30_Msk & ((value) << MCAN_TXBCR_CR30_Pos))
#define MCAN_TXBCR_CR31_Pos                   (31U)                                          /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 31 Position */
#define MCAN_TXBCR_CR31_Msk                   (0x1U << MCAN_TXBCR_CR31_Pos)                  /**< (MCAN_TXBCR) Cancellation Request for Transmit Buffer 31 Mask */
#define MCAN_TXBCR_CR31(value)                (MCAN_TXBCR_CR31_Msk & ((value) << MCAN_TXBCR_CR31_Pos))
#define MCAN_TXBCR_Msk                        (0xFFFFFFFFUL)                                 /**< (MCAN_TXBCR) Register Mask  */

/* -------- MCAN_TXBTO : (MCAN Offset: 0xD8) (R/  32) Transmit Buffer Transmission Occurred Register -------- */
#define MCAN_TXBTO_TO0_Pos                    (0U)                                           /**< (MCAN_TXBTO) Transmission Occurred for Buffer 0 Position */
#define MCAN_TXBTO_TO0_Msk                    (0x1U << MCAN_TXBTO_TO0_Pos)                   /**< (MCAN_TXBTO) Transmission Occurred for Buffer 0 Mask */
#define MCAN_TXBTO_TO0(value)                 (MCAN_TXBTO_TO0_Msk & ((value) << MCAN_TXBTO_TO0_Pos))
#define MCAN_TXBTO_TO1_Pos                    (1U)                                           /**< (MCAN_TXBTO) Transmission Occurred for Buffer 1 Position */
#define MCAN_TXBTO_TO1_Msk                    (0x1U << MCAN_TXBTO_TO1_Pos)                   /**< (MCAN_TXBTO) Transmission Occurred for Buffer 1 Mask */
#define MCAN_TXBTO_TO1(value)                 (MCAN_TXBTO_TO1_Msk & ((value) << MCAN_TXBTO_TO1_Pos))
#define MCAN_TXBTO_TO2_Pos                    (2U)                                           /**< (MCAN_TXBTO) Transmission Occurred for Buffer 2 Position */
#define MCAN_TXBTO_TO2_Msk                    (0x1U << MCAN_TXBTO_TO2_Pos)                   /**< (MCAN_TXBTO) Transmission Occurred for Buffer 2 Mask */
#define MCAN_TXBTO_TO2(value)                 (MCAN_TXBTO_TO2_Msk & ((value) << MCAN_TXBTO_TO2_Pos))
#define MCAN_TXBTO_TO3_Pos                    (3U)                                           /**< (MCAN_TXBTO) Transmission Occurred for Buffer 3 Position */
#define MCAN_TXBTO_TO3_Msk                    (0x1U << MCAN_TXBTO_TO3_Pos)                   /**< (MCAN_TXBTO) Transmission Occurred for Buffer 3 Mask */
#define MCAN_TXBTO_TO3(value)                 (MCAN_TXBTO_TO3_Msk & ((value) << MCAN_TXBTO_TO3_Pos))
#define MCAN_TXBTO_TO4_Pos                    (4U)                                           /**< (MCAN_TXBTO) Transmission Occurred for Buffer 4 Position */
#define MCAN_TXBTO_TO4_Msk                    (0x1U << MCAN_TXBTO_TO4_Pos)                   /**< (MCAN_TXBTO) Transmission Occurred for Buffer 4 Mask */
#define MCAN_TXBTO_TO4(value)                 (MCAN_TXBTO_TO4_Msk & ((value) << MCAN_TXBTO_TO4_Pos))
#define MCAN_TXBTO_TO5_Pos                    (5U)                                           /**< (MCAN_TXBTO) Transmission Occurred for Buffer 5 Position */
#define MCAN_TXBTO_TO5_Msk                    (0x1U << MCAN_TXBTO_TO5_Pos)                   /**< (MCAN_TXBTO) Transmission Occurred for Buffer 5 Mask */
#define MCAN_TXBTO_TO5(value)                 (MCAN_TXBTO_TO5_Msk & ((value) << MCAN_TXBTO_TO5_Pos))
#define MCAN_TXBTO_TO6_Pos                    (6U)                                           /**< (MCAN_TXBTO) Transmission Occurred for Buffer 6 Position */
#define MCAN_TXBTO_TO6_Msk                    (0x1U << MCAN_TXBTO_TO6_Pos)                   /**< (MCAN_TXBTO) Transmission Occurred for Buffer 6 Mask */
#define MCAN_TXBTO_TO6(value)                 (MCAN_TXBTO_TO6_Msk & ((value) << MCAN_TXBTO_TO6_Pos))
#define MCAN_TXBTO_TO7_Pos                    (7U)                                           /**< (MCAN_TXBTO) Transmission Occurred for Buffer 7 Position */
#define MCAN_TXBTO_TO7_Msk                    (0x1U << MCAN_TXBTO_TO7_Pos)                   /**< (MCAN_TXBTO) Transmission Occurred for Buffer 7 Mask */
#define MCAN_TXBTO_TO7(value)                 (MCAN_TXBTO_TO7_Msk & ((value) << MCAN_TXBTO_TO7_Pos))
#define MCAN_TXBTO_TO8_Pos                    (8U)                                           /**< (MCAN_TXBTO) Transmission Occurred for Buffer 8 Position */
#define MCAN_TXBTO_TO8_Msk                    (0x1U << MCAN_TXBTO_TO8_Pos)                   /**< (MCAN_TXBTO) Transmission Occurred for Buffer 8 Mask */
#define MCAN_TXBTO_TO8(value)                 (MCAN_TXBTO_TO8_Msk & ((value) << MCAN_TXBTO_TO8_Pos))
#define MCAN_TXBTO_TO9_Pos                    (9U)                                           /**< (MCAN_TXBTO) Transmission Occurred for Buffer 9 Position */
#define MCAN_TXBTO_TO9_Msk                    (0x1U << MCAN_TXBTO_TO9_Pos)                   /**< (MCAN_TXBTO) Transmission Occurred for Buffer 9 Mask */
#define MCAN_TXBTO_TO9(value)                 (MCAN_TXBTO_TO9_Msk & ((value) << MCAN_TXBTO_TO9_Pos))
#define MCAN_TXBTO_TO10_Pos                   (10U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 10 Position */
#define MCAN_TXBTO_TO10_Msk                   (0x1U << MCAN_TXBTO_TO10_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 10 Mask */
#define MCAN_TXBTO_TO10(value)                (MCAN_TXBTO_TO10_Msk & ((value) << MCAN_TXBTO_TO10_Pos))
#define MCAN_TXBTO_TO11_Pos                   (11U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 11 Position */
#define MCAN_TXBTO_TO11_Msk                   (0x1U << MCAN_TXBTO_TO11_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 11 Mask */
#define MCAN_TXBTO_TO11(value)                (MCAN_TXBTO_TO11_Msk & ((value) << MCAN_TXBTO_TO11_Pos))
#define MCAN_TXBTO_TO12_Pos                   (12U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 12 Position */
#define MCAN_TXBTO_TO12_Msk                   (0x1U << MCAN_TXBTO_TO12_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 12 Mask */
#define MCAN_TXBTO_TO12(value)                (MCAN_TXBTO_TO12_Msk & ((value) << MCAN_TXBTO_TO12_Pos))
#define MCAN_TXBTO_TO13_Pos                   (13U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 13 Position */
#define MCAN_TXBTO_TO13_Msk                   (0x1U << MCAN_TXBTO_TO13_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 13 Mask */
#define MCAN_TXBTO_TO13(value)                (MCAN_TXBTO_TO13_Msk & ((value) << MCAN_TXBTO_TO13_Pos))
#define MCAN_TXBTO_TO14_Pos                   (14U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 14 Position */
#define MCAN_TXBTO_TO14_Msk                   (0x1U << MCAN_TXBTO_TO14_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 14 Mask */
#define MCAN_TXBTO_TO14(value)                (MCAN_TXBTO_TO14_Msk & ((value) << MCAN_TXBTO_TO14_Pos))
#define MCAN_TXBTO_TO15_Pos                   (15U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 15 Position */
#define MCAN_TXBTO_TO15_Msk                   (0x1U << MCAN_TXBTO_TO15_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 15 Mask */
#define MCAN_TXBTO_TO15(value)                (MCAN_TXBTO_TO15_Msk & ((value) << MCAN_TXBTO_TO15_Pos))
#define MCAN_TXBTO_TO16_Pos                   (16U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 16 Position */
#define MCAN_TXBTO_TO16_Msk                   (0x1U << MCAN_TXBTO_TO16_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 16 Mask */
#define MCAN_TXBTO_TO16(value)                (MCAN_TXBTO_TO16_Msk & ((value) << MCAN_TXBTO_TO16_Pos))
#define MCAN_TXBTO_TO17_Pos                   (17U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 17 Position */
#define MCAN_TXBTO_TO17_Msk                   (0x1U << MCAN_TXBTO_TO17_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 17 Mask */
#define MCAN_TXBTO_TO17(value)                (MCAN_TXBTO_TO17_Msk & ((value) << MCAN_TXBTO_TO17_Pos))
#define MCAN_TXBTO_TO18_Pos                   (18U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 18 Position */
#define MCAN_TXBTO_TO18_Msk                   (0x1U << MCAN_TXBTO_TO18_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 18 Mask */
#define MCAN_TXBTO_TO18(value)                (MCAN_TXBTO_TO18_Msk & ((value) << MCAN_TXBTO_TO18_Pos))
#define MCAN_TXBTO_TO19_Pos                   (19U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 19 Position */
#define MCAN_TXBTO_TO19_Msk                   (0x1U << MCAN_TXBTO_TO19_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 19 Mask */
#define MCAN_TXBTO_TO19(value)                (MCAN_TXBTO_TO19_Msk & ((value) << MCAN_TXBTO_TO19_Pos))
#define MCAN_TXBTO_TO20_Pos                   (20U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 20 Position */
#define MCAN_TXBTO_TO20_Msk                   (0x1U << MCAN_TXBTO_TO20_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 20 Mask */
#define MCAN_TXBTO_TO20(value)                (MCAN_TXBTO_TO20_Msk & ((value) << MCAN_TXBTO_TO20_Pos))
#define MCAN_TXBTO_TO21_Pos                   (21U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 21 Position */
#define MCAN_TXBTO_TO21_Msk                   (0x1U << MCAN_TXBTO_TO21_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 21 Mask */
#define MCAN_TXBTO_TO21(value)                (MCAN_TXBTO_TO21_Msk & ((value) << MCAN_TXBTO_TO21_Pos))
#define MCAN_TXBTO_TO22_Pos                   (22U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 22 Position */
#define MCAN_TXBTO_TO22_Msk                   (0x1U << MCAN_TXBTO_TO22_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 22 Mask */
#define MCAN_TXBTO_TO22(value)                (MCAN_TXBTO_TO22_Msk & ((value) << MCAN_TXBTO_TO22_Pos))
#define MCAN_TXBTO_TO23_Pos                   (23U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 23 Position */
#define MCAN_TXBTO_TO23_Msk                   (0x1U << MCAN_TXBTO_TO23_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 23 Mask */
#define MCAN_TXBTO_TO23(value)                (MCAN_TXBTO_TO23_Msk & ((value) << MCAN_TXBTO_TO23_Pos))
#define MCAN_TXBTO_TO24_Pos                   (24U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 24 Position */
#define MCAN_TXBTO_TO24_Msk                   (0x1U << MCAN_TXBTO_TO24_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 24 Mask */
#define MCAN_TXBTO_TO24(value)                (MCAN_TXBTO_TO24_Msk & ((value) << MCAN_TXBTO_TO24_Pos))
#define MCAN_TXBTO_TO25_Pos                   (25U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 25 Position */
#define MCAN_TXBTO_TO25_Msk                   (0x1U << MCAN_TXBTO_TO25_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 25 Mask */
#define MCAN_TXBTO_TO25(value)                (MCAN_TXBTO_TO25_Msk & ((value) << MCAN_TXBTO_TO25_Pos))
#define MCAN_TXBTO_TO26_Pos                   (26U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 26 Position */
#define MCAN_TXBTO_TO26_Msk                   (0x1U << MCAN_TXBTO_TO26_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 26 Mask */
#define MCAN_TXBTO_TO26(value)                (MCAN_TXBTO_TO26_Msk & ((value) << MCAN_TXBTO_TO26_Pos))
#define MCAN_TXBTO_TO27_Pos                   (27U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 27 Position */
#define MCAN_TXBTO_TO27_Msk                   (0x1U << MCAN_TXBTO_TO27_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 27 Mask */
#define MCAN_TXBTO_TO27(value)                (MCAN_TXBTO_TO27_Msk & ((value) << MCAN_TXBTO_TO27_Pos))
#define MCAN_TXBTO_TO28_Pos                   (28U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 28 Position */
#define MCAN_TXBTO_TO28_Msk                   (0x1U << MCAN_TXBTO_TO28_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 28 Mask */
#define MCAN_TXBTO_TO28(value)                (MCAN_TXBTO_TO28_Msk & ((value) << MCAN_TXBTO_TO28_Pos))
#define MCAN_TXBTO_TO29_Pos                   (29U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 29 Position */
#define MCAN_TXBTO_TO29_Msk                   (0x1U << MCAN_TXBTO_TO29_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 29 Mask */
#define MCAN_TXBTO_TO29(value)                (MCAN_TXBTO_TO29_Msk & ((value) << MCAN_TXBTO_TO29_Pos))
#define MCAN_TXBTO_TO30_Pos                   (30U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 30 Position */
#define MCAN_TXBTO_TO30_Msk                   (0x1U << MCAN_TXBTO_TO30_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 30 Mask */
#define MCAN_TXBTO_TO30(value)                (MCAN_TXBTO_TO30_Msk & ((value) << MCAN_TXBTO_TO30_Pos))
#define MCAN_TXBTO_TO31_Pos                   (31U)                                          /**< (MCAN_TXBTO) Transmission Occurred for Buffer 31 Position */
#define MCAN_TXBTO_TO31_Msk                   (0x1U << MCAN_TXBTO_TO31_Pos)                  /**< (MCAN_TXBTO) Transmission Occurred for Buffer 31 Mask */
#define MCAN_TXBTO_TO31(value)                (MCAN_TXBTO_TO31_Msk & ((value) << MCAN_TXBTO_TO31_Pos))
#define MCAN_TXBTO_Msk                        (0xFFFFFFFFUL)                                 /**< (MCAN_TXBTO) Register Mask  */

/* -------- MCAN_TXBCF : (MCAN Offset: 0xDC) (R/  32) Transmit Buffer Cancellation Finished Register -------- */
#define MCAN_TXBCF_CF0_Pos                    (0U)                                           /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 0 Position */
#define MCAN_TXBCF_CF0_Msk                    (0x1U << MCAN_TXBCF_CF0_Pos)                   /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 0 Mask */
#define MCAN_TXBCF_CF0(value)                 (MCAN_TXBCF_CF0_Msk & ((value) << MCAN_TXBCF_CF0_Pos))
#define MCAN_TXBCF_CF1_Pos                    (1U)                                           /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 1 Position */
#define MCAN_TXBCF_CF1_Msk                    (0x1U << MCAN_TXBCF_CF1_Pos)                   /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 1 Mask */
#define MCAN_TXBCF_CF1(value)                 (MCAN_TXBCF_CF1_Msk & ((value) << MCAN_TXBCF_CF1_Pos))
#define MCAN_TXBCF_CF2_Pos                    (2U)                                           /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 2 Position */
#define MCAN_TXBCF_CF2_Msk                    (0x1U << MCAN_TXBCF_CF2_Pos)                   /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 2 Mask */
#define MCAN_TXBCF_CF2(value)                 (MCAN_TXBCF_CF2_Msk & ((value) << MCAN_TXBCF_CF2_Pos))
#define MCAN_TXBCF_CF3_Pos                    (3U)                                           /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 3 Position */
#define MCAN_TXBCF_CF3_Msk                    (0x1U << MCAN_TXBCF_CF3_Pos)                   /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 3 Mask */
#define MCAN_TXBCF_CF3(value)                 (MCAN_TXBCF_CF3_Msk & ((value) << MCAN_TXBCF_CF3_Pos))
#define MCAN_TXBCF_CF4_Pos                    (4U)                                           /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 4 Position */
#define MCAN_TXBCF_CF4_Msk                    (0x1U << MCAN_TXBCF_CF4_Pos)                   /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 4 Mask */
#define MCAN_TXBCF_CF4(value)                 (MCAN_TXBCF_CF4_Msk & ((value) << MCAN_TXBCF_CF4_Pos))
#define MCAN_TXBCF_CF5_Pos                    (5U)                                           /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 5 Position */
#define MCAN_TXBCF_CF5_Msk                    (0x1U << MCAN_TXBCF_CF5_Pos)                   /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 5 Mask */
#define MCAN_TXBCF_CF5(value)                 (MCAN_TXBCF_CF5_Msk & ((value) << MCAN_TXBCF_CF5_Pos))
#define MCAN_TXBCF_CF6_Pos                    (6U)                                           /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 6 Position */
#define MCAN_TXBCF_CF6_Msk                    (0x1U << MCAN_TXBCF_CF6_Pos)                   /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 6 Mask */
#define MCAN_TXBCF_CF6(value)                 (MCAN_TXBCF_CF6_Msk & ((value) << MCAN_TXBCF_CF6_Pos))
#define MCAN_TXBCF_CF7_Pos                    (7U)                                           /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 7 Position */
#define MCAN_TXBCF_CF7_Msk                    (0x1U << MCAN_TXBCF_CF7_Pos)                   /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 7 Mask */
#define MCAN_TXBCF_CF7(value)                 (MCAN_TXBCF_CF7_Msk & ((value) << MCAN_TXBCF_CF7_Pos))
#define MCAN_TXBCF_CF8_Pos                    (8U)                                           /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 8 Position */
#define MCAN_TXBCF_CF8_Msk                    (0x1U << MCAN_TXBCF_CF8_Pos)                   /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 8 Mask */
#define MCAN_TXBCF_CF8(value)                 (MCAN_TXBCF_CF8_Msk & ((value) << MCAN_TXBCF_CF8_Pos))
#define MCAN_TXBCF_CF9_Pos                    (9U)                                           /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 9 Position */
#define MCAN_TXBCF_CF9_Msk                    (0x1U << MCAN_TXBCF_CF9_Pos)                   /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 9 Mask */
#define MCAN_TXBCF_CF9(value)                 (MCAN_TXBCF_CF9_Msk & ((value) << MCAN_TXBCF_CF9_Pos))
#define MCAN_TXBCF_CF10_Pos                   (10U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 10 Position */
#define MCAN_TXBCF_CF10_Msk                   (0x1U << MCAN_TXBCF_CF10_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 10 Mask */
#define MCAN_TXBCF_CF10(value)                (MCAN_TXBCF_CF10_Msk & ((value) << MCAN_TXBCF_CF10_Pos))
#define MCAN_TXBCF_CF11_Pos                   (11U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 11 Position */
#define MCAN_TXBCF_CF11_Msk                   (0x1U << MCAN_TXBCF_CF11_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 11 Mask */
#define MCAN_TXBCF_CF11(value)                (MCAN_TXBCF_CF11_Msk & ((value) << MCAN_TXBCF_CF11_Pos))
#define MCAN_TXBCF_CF12_Pos                   (12U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 12 Position */
#define MCAN_TXBCF_CF12_Msk                   (0x1U << MCAN_TXBCF_CF12_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 12 Mask */
#define MCAN_TXBCF_CF12(value)                (MCAN_TXBCF_CF12_Msk & ((value) << MCAN_TXBCF_CF12_Pos))
#define MCAN_TXBCF_CF13_Pos                   (13U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 13 Position */
#define MCAN_TXBCF_CF13_Msk                   (0x1U << MCAN_TXBCF_CF13_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 13 Mask */
#define MCAN_TXBCF_CF13(value)                (MCAN_TXBCF_CF13_Msk & ((value) << MCAN_TXBCF_CF13_Pos))
#define MCAN_TXBCF_CF14_Pos                   (14U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 14 Position */
#define MCAN_TXBCF_CF14_Msk                   (0x1U << MCAN_TXBCF_CF14_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 14 Mask */
#define MCAN_TXBCF_CF14(value)                (MCAN_TXBCF_CF14_Msk & ((value) << MCAN_TXBCF_CF14_Pos))
#define MCAN_TXBCF_CF15_Pos                   (15U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 15 Position */
#define MCAN_TXBCF_CF15_Msk                   (0x1U << MCAN_TXBCF_CF15_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 15 Mask */
#define MCAN_TXBCF_CF15(value)                (MCAN_TXBCF_CF15_Msk & ((value) << MCAN_TXBCF_CF15_Pos))
#define MCAN_TXBCF_CF16_Pos                   (16U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 16 Position */
#define MCAN_TXBCF_CF16_Msk                   (0x1U << MCAN_TXBCF_CF16_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 16 Mask */
#define MCAN_TXBCF_CF16(value)                (MCAN_TXBCF_CF16_Msk & ((value) << MCAN_TXBCF_CF16_Pos))
#define MCAN_TXBCF_CF17_Pos                   (17U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 17 Position */
#define MCAN_TXBCF_CF17_Msk                   (0x1U << MCAN_TXBCF_CF17_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 17 Mask */
#define MCAN_TXBCF_CF17(value)                (MCAN_TXBCF_CF17_Msk & ((value) << MCAN_TXBCF_CF17_Pos))
#define MCAN_TXBCF_CF18_Pos                   (18U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 18 Position */
#define MCAN_TXBCF_CF18_Msk                   (0x1U << MCAN_TXBCF_CF18_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 18 Mask */
#define MCAN_TXBCF_CF18(value)                (MCAN_TXBCF_CF18_Msk & ((value) << MCAN_TXBCF_CF18_Pos))
#define MCAN_TXBCF_CF19_Pos                   (19U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 19 Position */
#define MCAN_TXBCF_CF19_Msk                   (0x1U << MCAN_TXBCF_CF19_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 19 Mask */
#define MCAN_TXBCF_CF19(value)                (MCAN_TXBCF_CF19_Msk & ((value) << MCAN_TXBCF_CF19_Pos))
#define MCAN_TXBCF_CF20_Pos                   (20U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 20 Position */
#define MCAN_TXBCF_CF20_Msk                   (0x1U << MCAN_TXBCF_CF20_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 20 Mask */
#define MCAN_TXBCF_CF20(value)                (MCAN_TXBCF_CF20_Msk & ((value) << MCAN_TXBCF_CF20_Pos))
#define MCAN_TXBCF_CF21_Pos                   (21U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 21 Position */
#define MCAN_TXBCF_CF21_Msk                   (0x1U << MCAN_TXBCF_CF21_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 21 Mask */
#define MCAN_TXBCF_CF21(value)                (MCAN_TXBCF_CF21_Msk & ((value) << MCAN_TXBCF_CF21_Pos))
#define MCAN_TXBCF_CF22_Pos                   (22U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 22 Position */
#define MCAN_TXBCF_CF22_Msk                   (0x1U << MCAN_TXBCF_CF22_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 22 Mask */
#define MCAN_TXBCF_CF22(value)                (MCAN_TXBCF_CF22_Msk & ((value) << MCAN_TXBCF_CF22_Pos))
#define MCAN_TXBCF_CF23_Pos                   (23U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 23 Position */
#define MCAN_TXBCF_CF23_Msk                   (0x1U << MCAN_TXBCF_CF23_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 23 Mask */
#define MCAN_TXBCF_CF23(value)                (MCAN_TXBCF_CF23_Msk & ((value) << MCAN_TXBCF_CF23_Pos))
#define MCAN_TXBCF_CF24_Pos                   (24U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 24 Position */
#define MCAN_TXBCF_CF24_Msk                   (0x1U << MCAN_TXBCF_CF24_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 24 Mask */
#define MCAN_TXBCF_CF24(value)                (MCAN_TXBCF_CF24_Msk & ((value) << MCAN_TXBCF_CF24_Pos))
#define MCAN_TXBCF_CF25_Pos                   (25U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 25 Position */
#define MCAN_TXBCF_CF25_Msk                   (0x1U << MCAN_TXBCF_CF25_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 25 Mask */
#define MCAN_TXBCF_CF25(value)                (MCAN_TXBCF_CF25_Msk & ((value) << MCAN_TXBCF_CF25_Pos))
#define MCAN_TXBCF_CF26_Pos                   (26U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 26 Position */
#define MCAN_TXBCF_CF26_Msk                   (0x1U << MCAN_TXBCF_CF26_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 26 Mask */
#define MCAN_TXBCF_CF26(value)                (MCAN_TXBCF_CF26_Msk & ((value) << MCAN_TXBCF_CF26_Pos))
#define MCAN_TXBCF_CF27_Pos                   (27U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 27 Position */
#define MCAN_TXBCF_CF27_Msk                   (0x1U << MCAN_TXBCF_CF27_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 27 Mask */
#define MCAN_TXBCF_CF27(value)                (MCAN_TXBCF_CF27_Msk & ((value) << MCAN_TXBCF_CF27_Pos))
#define MCAN_TXBCF_CF28_Pos                   (28U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 28 Position */
#define MCAN_TXBCF_CF28_Msk                   (0x1U << MCAN_TXBCF_CF28_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 28 Mask */
#define MCAN_TXBCF_CF28(value)                (MCAN_TXBCF_CF28_Msk & ((value) << MCAN_TXBCF_CF28_Pos))
#define MCAN_TXBCF_CF29_Pos                   (29U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 29 Position */
#define MCAN_TXBCF_CF29_Msk                   (0x1U << MCAN_TXBCF_CF29_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 29 Mask */
#define MCAN_TXBCF_CF29(value)                (MCAN_TXBCF_CF29_Msk & ((value) << MCAN_TXBCF_CF29_Pos))
#define MCAN_TXBCF_CF30_Pos                   (30U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 30 Position */
#define MCAN_TXBCF_CF30_Msk                   (0x1U << MCAN_TXBCF_CF30_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 30 Mask */
#define MCAN_TXBCF_CF30(value)                (MCAN_TXBCF_CF30_Msk & ((value) << MCAN_TXBCF_CF30_Pos))
#define MCAN_TXBCF_CF31_Pos                   (31U)                                          /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 31 Position */
#define MCAN_TXBCF_CF31_Msk                   (0x1U << MCAN_TXBCF_CF31_Pos)                  /**< (MCAN_TXBCF) Cancellation Finished for Transmit Buffer 31 Mask */
#define MCAN_TXBCF_CF31(value)                (MCAN_TXBCF_CF31_Msk & ((value) << MCAN_TXBCF_CF31_Pos))
#define MCAN_TXBCF_Msk                        (0xFFFFFFFFUL)                                 /**< (MCAN_TXBCF) Register Mask  */

/* -------- MCAN_TXBTIE : (MCAN Offset: 0xE0) (R/W 32) Transmit Buffer Transmission Interrupt Enable Register -------- */
#define MCAN_TXBTIE_TIE0_Pos                  (0U)                                           /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 0 Position */
#define MCAN_TXBTIE_TIE0_Msk                  (0x1U << MCAN_TXBTIE_TIE0_Pos)                 /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 0 Mask */
#define MCAN_TXBTIE_TIE0(value)               (MCAN_TXBTIE_TIE0_Msk & ((value) << MCAN_TXBTIE_TIE0_Pos))
#define MCAN_TXBTIE_TIE1_Pos                  (1U)                                           /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 1 Position */
#define MCAN_TXBTIE_TIE1_Msk                  (0x1U << MCAN_TXBTIE_TIE1_Pos)                 /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 1 Mask */
#define MCAN_TXBTIE_TIE1(value)               (MCAN_TXBTIE_TIE1_Msk & ((value) << MCAN_TXBTIE_TIE1_Pos))
#define MCAN_TXBTIE_TIE2_Pos                  (2U)                                           /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 2 Position */
#define MCAN_TXBTIE_TIE2_Msk                  (0x1U << MCAN_TXBTIE_TIE2_Pos)                 /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 2 Mask */
#define MCAN_TXBTIE_TIE2(value)               (MCAN_TXBTIE_TIE2_Msk & ((value) << MCAN_TXBTIE_TIE2_Pos))
#define MCAN_TXBTIE_TIE3_Pos                  (3U)                                           /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 3 Position */
#define MCAN_TXBTIE_TIE3_Msk                  (0x1U << MCAN_TXBTIE_TIE3_Pos)                 /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 3 Mask */
#define MCAN_TXBTIE_TIE3(value)               (MCAN_TXBTIE_TIE3_Msk & ((value) << MCAN_TXBTIE_TIE3_Pos))
#define MCAN_TXBTIE_TIE4_Pos                  (4U)                                           /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 4 Position */
#define MCAN_TXBTIE_TIE4_Msk                  (0x1U << MCAN_TXBTIE_TIE4_Pos)                 /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 4 Mask */
#define MCAN_TXBTIE_TIE4(value)               (MCAN_TXBTIE_TIE4_Msk & ((value) << MCAN_TXBTIE_TIE4_Pos))
#define MCAN_TXBTIE_TIE5_Pos                  (5U)                                           /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 5 Position */
#define MCAN_TXBTIE_TIE5_Msk                  (0x1U << MCAN_TXBTIE_TIE5_Pos)                 /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 5 Mask */
#define MCAN_TXBTIE_TIE5(value)               (MCAN_TXBTIE_TIE5_Msk & ((value) << MCAN_TXBTIE_TIE5_Pos))
#define MCAN_TXBTIE_TIE6_Pos                  (6U)                                           /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 6 Position */
#define MCAN_TXBTIE_TIE6_Msk                  (0x1U << MCAN_TXBTIE_TIE6_Pos)                 /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 6 Mask */
#define MCAN_TXBTIE_TIE6(value)               (MCAN_TXBTIE_TIE6_Msk & ((value) << MCAN_TXBTIE_TIE6_Pos))
#define MCAN_TXBTIE_TIE7_Pos                  (7U)                                           /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 7 Position */
#define MCAN_TXBTIE_TIE7_Msk                  (0x1U << MCAN_TXBTIE_TIE7_Pos)                 /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 7 Mask */
#define MCAN_TXBTIE_TIE7(value)               (MCAN_TXBTIE_TIE7_Msk & ((value) << MCAN_TXBTIE_TIE7_Pos))
#define MCAN_TXBTIE_TIE8_Pos                  (8U)                                           /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 8 Position */
#define MCAN_TXBTIE_TIE8_Msk                  (0x1U << MCAN_TXBTIE_TIE8_Pos)                 /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 8 Mask */
#define MCAN_TXBTIE_TIE8(value)               (MCAN_TXBTIE_TIE8_Msk & ((value) << MCAN_TXBTIE_TIE8_Pos))
#define MCAN_TXBTIE_TIE9_Pos                  (9U)                                           /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 9 Position */
#define MCAN_TXBTIE_TIE9_Msk                  (0x1U << MCAN_TXBTIE_TIE9_Pos)                 /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 9 Mask */
#define MCAN_TXBTIE_TIE9(value)               (MCAN_TXBTIE_TIE9_Msk & ((value) << MCAN_TXBTIE_TIE9_Pos))
#define MCAN_TXBTIE_TIE10_Pos                 (10U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 10 Position */
#define MCAN_TXBTIE_TIE10_Msk                 (0x1U << MCAN_TXBTIE_TIE10_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 10 Mask */
#define MCAN_TXBTIE_TIE10(value)              (MCAN_TXBTIE_TIE10_Msk & ((value) << MCAN_TXBTIE_TIE10_Pos))
#define MCAN_TXBTIE_TIE11_Pos                 (11U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 11 Position */
#define MCAN_TXBTIE_TIE11_Msk                 (0x1U << MCAN_TXBTIE_TIE11_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 11 Mask */
#define MCAN_TXBTIE_TIE11(value)              (MCAN_TXBTIE_TIE11_Msk & ((value) << MCAN_TXBTIE_TIE11_Pos))
#define MCAN_TXBTIE_TIE12_Pos                 (12U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 12 Position */
#define MCAN_TXBTIE_TIE12_Msk                 (0x1U << MCAN_TXBTIE_TIE12_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 12 Mask */
#define MCAN_TXBTIE_TIE12(value)              (MCAN_TXBTIE_TIE12_Msk & ((value) << MCAN_TXBTIE_TIE12_Pos))
#define MCAN_TXBTIE_TIE13_Pos                 (13U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 13 Position */
#define MCAN_TXBTIE_TIE13_Msk                 (0x1U << MCAN_TXBTIE_TIE13_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 13 Mask */
#define MCAN_TXBTIE_TIE13(value)              (MCAN_TXBTIE_TIE13_Msk & ((value) << MCAN_TXBTIE_TIE13_Pos))
#define MCAN_TXBTIE_TIE14_Pos                 (14U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 14 Position */
#define MCAN_TXBTIE_TIE14_Msk                 (0x1U << MCAN_TXBTIE_TIE14_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 14 Mask */
#define MCAN_TXBTIE_TIE14(value)              (MCAN_TXBTIE_TIE14_Msk & ((value) << MCAN_TXBTIE_TIE14_Pos))
#define MCAN_TXBTIE_TIE15_Pos                 (15U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 15 Position */
#define MCAN_TXBTIE_TIE15_Msk                 (0x1U << MCAN_TXBTIE_TIE15_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 15 Mask */
#define MCAN_TXBTIE_TIE15(value)              (MCAN_TXBTIE_TIE15_Msk & ((value) << MCAN_TXBTIE_TIE15_Pos))
#define MCAN_TXBTIE_TIE16_Pos                 (16U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 16 Position */
#define MCAN_TXBTIE_TIE16_Msk                 (0x1U << MCAN_TXBTIE_TIE16_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 16 Mask */
#define MCAN_TXBTIE_TIE16(value)              (MCAN_TXBTIE_TIE16_Msk & ((value) << MCAN_TXBTIE_TIE16_Pos))
#define MCAN_TXBTIE_TIE17_Pos                 (17U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 17 Position */
#define MCAN_TXBTIE_TIE17_Msk                 (0x1U << MCAN_TXBTIE_TIE17_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 17 Mask */
#define MCAN_TXBTIE_TIE17(value)              (MCAN_TXBTIE_TIE17_Msk & ((value) << MCAN_TXBTIE_TIE17_Pos))
#define MCAN_TXBTIE_TIE18_Pos                 (18U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 18 Position */
#define MCAN_TXBTIE_TIE18_Msk                 (0x1U << MCAN_TXBTIE_TIE18_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 18 Mask */
#define MCAN_TXBTIE_TIE18(value)              (MCAN_TXBTIE_TIE18_Msk & ((value) << MCAN_TXBTIE_TIE18_Pos))
#define MCAN_TXBTIE_TIE19_Pos                 (19U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 19 Position */
#define MCAN_TXBTIE_TIE19_Msk                 (0x1U << MCAN_TXBTIE_TIE19_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 19 Mask */
#define MCAN_TXBTIE_TIE19(value)              (MCAN_TXBTIE_TIE19_Msk & ((value) << MCAN_TXBTIE_TIE19_Pos))
#define MCAN_TXBTIE_TIE20_Pos                 (20U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 20 Position */
#define MCAN_TXBTIE_TIE20_Msk                 (0x1U << MCAN_TXBTIE_TIE20_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 20 Mask */
#define MCAN_TXBTIE_TIE20(value)              (MCAN_TXBTIE_TIE20_Msk & ((value) << MCAN_TXBTIE_TIE20_Pos))
#define MCAN_TXBTIE_TIE21_Pos                 (21U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 21 Position */
#define MCAN_TXBTIE_TIE21_Msk                 (0x1U << MCAN_TXBTIE_TIE21_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 21 Mask */
#define MCAN_TXBTIE_TIE21(value)              (MCAN_TXBTIE_TIE21_Msk & ((value) << MCAN_TXBTIE_TIE21_Pos))
#define MCAN_TXBTIE_TIE22_Pos                 (22U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 22 Position */
#define MCAN_TXBTIE_TIE22_Msk                 (0x1U << MCAN_TXBTIE_TIE22_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 22 Mask */
#define MCAN_TXBTIE_TIE22(value)              (MCAN_TXBTIE_TIE22_Msk & ((value) << MCAN_TXBTIE_TIE22_Pos))
#define MCAN_TXBTIE_TIE23_Pos                 (23U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 23 Position */
#define MCAN_TXBTIE_TIE23_Msk                 (0x1U << MCAN_TXBTIE_TIE23_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 23 Mask */
#define MCAN_TXBTIE_TIE23(value)              (MCAN_TXBTIE_TIE23_Msk & ((value) << MCAN_TXBTIE_TIE23_Pos))
#define MCAN_TXBTIE_TIE24_Pos                 (24U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 24 Position */
#define MCAN_TXBTIE_TIE24_Msk                 (0x1U << MCAN_TXBTIE_TIE24_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 24 Mask */
#define MCAN_TXBTIE_TIE24(value)              (MCAN_TXBTIE_TIE24_Msk & ((value) << MCAN_TXBTIE_TIE24_Pos))
#define MCAN_TXBTIE_TIE25_Pos                 (25U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 25 Position */
#define MCAN_TXBTIE_TIE25_Msk                 (0x1U << MCAN_TXBTIE_TIE25_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 25 Mask */
#define MCAN_TXBTIE_TIE25(value)              (MCAN_TXBTIE_TIE25_Msk & ((value) << MCAN_TXBTIE_TIE25_Pos))
#define MCAN_TXBTIE_TIE26_Pos                 (26U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 26 Position */
#define MCAN_TXBTIE_TIE26_Msk                 (0x1U << MCAN_TXBTIE_TIE26_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 26 Mask */
#define MCAN_TXBTIE_TIE26(value)              (MCAN_TXBTIE_TIE26_Msk & ((value) << MCAN_TXBTIE_TIE26_Pos))
#define MCAN_TXBTIE_TIE27_Pos                 (27U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 27 Position */
#define MCAN_TXBTIE_TIE27_Msk                 (0x1U << MCAN_TXBTIE_TIE27_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 27 Mask */
#define MCAN_TXBTIE_TIE27(value)              (MCAN_TXBTIE_TIE27_Msk & ((value) << MCAN_TXBTIE_TIE27_Pos))
#define MCAN_TXBTIE_TIE28_Pos                 (28U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 28 Position */
#define MCAN_TXBTIE_TIE28_Msk                 (0x1U << MCAN_TXBTIE_TIE28_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 28 Mask */
#define MCAN_TXBTIE_TIE28(value)              (MCAN_TXBTIE_TIE28_Msk & ((value) << MCAN_TXBTIE_TIE28_Pos))
#define MCAN_TXBTIE_TIE29_Pos                 (29U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 29 Position */
#define MCAN_TXBTIE_TIE29_Msk                 (0x1U << MCAN_TXBTIE_TIE29_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 29 Mask */
#define MCAN_TXBTIE_TIE29(value)              (MCAN_TXBTIE_TIE29_Msk & ((value) << MCAN_TXBTIE_TIE29_Pos))
#define MCAN_TXBTIE_TIE30_Pos                 (30U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 30 Position */
#define MCAN_TXBTIE_TIE30_Msk                 (0x1U << MCAN_TXBTIE_TIE30_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 30 Mask */
#define MCAN_TXBTIE_TIE30(value)              (MCAN_TXBTIE_TIE30_Msk & ((value) << MCAN_TXBTIE_TIE30_Pos))
#define MCAN_TXBTIE_TIE31_Pos                 (31U)                                          /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 31 Position */
#define MCAN_TXBTIE_TIE31_Msk                 (0x1U << MCAN_TXBTIE_TIE31_Pos)                /**< (MCAN_TXBTIE) Transmission Interrupt Enable for Buffer 31 Mask */
#define MCAN_TXBTIE_TIE31(value)              (MCAN_TXBTIE_TIE31_Msk & ((value) << MCAN_TXBTIE_TIE31_Pos))
#define MCAN_TXBTIE_Msk                       (0xFFFFFFFFUL)                                 /**< (MCAN_TXBTIE) Register Mask  */

/* -------- MCAN_TXBCIE : (MCAN Offset: 0xE4) (R/W 32) Transmit Buffer Cancellation Finished Interrupt Enable Register -------- */
#define MCAN_TXBCIE_CFIE0_Pos                 (0U)                                           /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 0 Position */
#define MCAN_TXBCIE_CFIE0_Msk                 (0x1U << MCAN_TXBCIE_CFIE0_Pos)                /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 0 Mask */
#define MCAN_TXBCIE_CFIE0(value)              (MCAN_TXBCIE_CFIE0_Msk & ((value) << MCAN_TXBCIE_CFIE0_Pos))
#define MCAN_TXBCIE_CFIE1_Pos                 (1U)                                           /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 1 Position */
#define MCAN_TXBCIE_CFIE1_Msk                 (0x1U << MCAN_TXBCIE_CFIE1_Pos)                /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 1 Mask */
#define MCAN_TXBCIE_CFIE1(value)              (MCAN_TXBCIE_CFIE1_Msk & ((value) << MCAN_TXBCIE_CFIE1_Pos))
#define MCAN_TXBCIE_CFIE2_Pos                 (2U)                                           /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 2 Position */
#define MCAN_TXBCIE_CFIE2_Msk                 (0x1U << MCAN_TXBCIE_CFIE2_Pos)                /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 2 Mask */
#define MCAN_TXBCIE_CFIE2(value)              (MCAN_TXBCIE_CFIE2_Msk & ((value) << MCAN_TXBCIE_CFIE2_Pos))
#define MCAN_TXBCIE_CFIE3_Pos                 (3U)                                           /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 3 Position */
#define MCAN_TXBCIE_CFIE3_Msk                 (0x1U << MCAN_TXBCIE_CFIE3_Pos)                /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 3 Mask */
#define MCAN_TXBCIE_CFIE3(value)              (MCAN_TXBCIE_CFIE3_Msk & ((value) << MCAN_TXBCIE_CFIE3_Pos))
#define MCAN_TXBCIE_CFIE4_Pos                 (4U)                                           /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 4 Position */
#define MCAN_TXBCIE_CFIE4_Msk                 (0x1U << MCAN_TXBCIE_CFIE4_Pos)                /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 4 Mask */
#define MCAN_TXBCIE_CFIE4(value)              (MCAN_TXBCIE_CFIE4_Msk & ((value) << MCAN_TXBCIE_CFIE4_Pos))
#define MCAN_TXBCIE_CFIE5_Pos                 (5U)                                           /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 5 Position */
#define MCAN_TXBCIE_CFIE5_Msk                 (0x1U << MCAN_TXBCIE_CFIE5_Pos)                /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 5 Mask */
#define MCAN_TXBCIE_CFIE5(value)              (MCAN_TXBCIE_CFIE5_Msk & ((value) << MCAN_TXBCIE_CFIE5_Pos))
#define MCAN_TXBCIE_CFIE6_Pos                 (6U)                                           /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 6 Position */
#define MCAN_TXBCIE_CFIE6_Msk                 (0x1U << MCAN_TXBCIE_CFIE6_Pos)                /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 6 Mask */
#define MCAN_TXBCIE_CFIE6(value)              (MCAN_TXBCIE_CFIE6_Msk & ((value) << MCAN_TXBCIE_CFIE6_Pos))
#define MCAN_TXBCIE_CFIE7_Pos                 (7U)                                           /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 7 Position */
#define MCAN_TXBCIE_CFIE7_Msk                 (0x1U << MCAN_TXBCIE_CFIE7_Pos)                /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 7 Mask */
#define MCAN_TXBCIE_CFIE7(value)              (MCAN_TXBCIE_CFIE7_Msk & ((value) << MCAN_TXBCIE_CFIE7_Pos))
#define MCAN_TXBCIE_CFIE8_Pos                 (8U)                                           /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 8 Position */
#define MCAN_TXBCIE_CFIE8_Msk                 (0x1U << MCAN_TXBCIE_CFIE8_Pos)                /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 8 Mask */
#define MCAN_TXBCIE_CFIE8(value)              (MCAN_TXBCIE_CFIE8_Msk & ((value) << MCAN_TXBCIE_CFIE8_Pos))
#define MCAN_TXBCIE_CFIE9_Pos                 (9U)                                           /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 9 Position */
#define MCAN_TXBCIE_CFIE9_Msk                 (0x1U << MCAN_TXBCIE_CFIE9_Pos)                /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 9 Mask */
#define MCAN_TXBCIE_CFIE9(value)              (MCAN_TXBCIE_CFIE9_Msk & ((value) << MCAN_TXBCIE_CFIE9_Pos))
#define MCAN_TXBCIE_CFIE10_Pos                (10U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 10 Position */
#define MCAN_TXBCIE_CFIE10_Msk                (0x1U << MCAN_TXBCIE_CFIE10_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 10 Mask */
#define MCAN_TXBCIE_CFIE10(value)             (MCAN_TXBCIE_CFIE10_Msk & ((value) << MCAN_TXBCIE_CFIE10_Pos))
#define MCAN_TXBCIE_CFIE11_Pos                (11U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 11 Position */
#define MCAN_TXBCIE_CFIE11_Msk                (0x1U << MCAN_TXBCIE_CFIE11_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 11 Mask */
#define MCAN_TXBCIE_CFIE11(value)             (MCAN_TXBCIE_CFIE11_Msk & ((value) << MCAN_TXBCIE_CFIE11_Pos))
#define MCAN_TXBCIE_CFIE12_Pos                (12U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 12 Position */
#define MCAN_TXBCIE_CFIE12_Msk                (0x1U << MCAN_TXBCIE_CFIE12_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 12 Mask */
#define MCAN_TXBCIE_CFIE12(value)             (MCAN_TXBCIE_CFIE12_Msk & ((value) << MCAN_TXBCIE_CFIE12_Pos))
#define MCAN_TXBCIE_CFIE13_Pos                (13U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 13 Position */
#define MCAN_TXBCIE_CFIE13_Msk                (0x1U << MCAN_TXBCIE_CFIE13_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 13 Mask */
#define MCAN_TXBCIE_CFIE13(value)             (MCAN_TXBCIE_CFIE13_Msk & ((value) << MCAN_TXBCIE_CFIE13_Pos))
#define MCAN_TXBCIE_CFIE14_Pos                (14U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 14 Position */
#define MCAN_TXBCIE_CFIE14_Msk                (0x1U << MCAN_TXBCIE_CFIE14_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 14 Mask */
#define MCAN_TXBCIE_CFIE14(value)             (MCAN_TXBCIE_CFIE14_Msk & ((value) << MCAN_TXBCIE_CFIE14_Pos))
#define MCAN_TXBCIE_CFIE15_Pos                (15U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 15 Position */
#define MCAN_TXBCIE_CFIE15_Msk                (0x1U << MCAN_TXBCIE_CFIE15_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 15 Mask */
#define MCAN_TXBCIE_CFIE15(value)             (MCAN_TXBCIE_CFIE15_Msk & ((value) << MCAN_TXBCIE_CFIE15_Pos))
#define MCAN_TXBCIE_CFIE16_Pos                (16U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 16 Position */
#define MCAN_TXBCIE_CFIE16_Msk                (0x1U << MCAN_TXBCIE_CFIE16_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 16 Mask */
#define MCAN_TXBCIE_CFIE16(value)             (MCAN_TXBCIE_CFIE16_Msk & ((value) << MCAN_TXBCIE_CFIE16_Pos))
#define MCAN_TXBCIE_CFIE17_Pos                (17U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 17 Position */
#define MCAN_TXBCIE_CFIE17_Msk                (0x1U << MCAN_TXBCIE_CFIE17_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 17 Mask */
#define MCAN_TXBCIE_CFIE17(value)             (MCAN_TXBCIE_CFIE17_Msk & ((value) << MCAN_TXBCIE_CFIE17_Pos))
#define MCAN_TXBCIE_CFIE18_Pos                (18U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 18 Position */
#define MCAN_TXBCIE_CFIE18_Msk                (0x1U << MCAN_TXBCIE_CFIE18_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 18 Mask */
#define MCAN_TXBCIE_CFIE18(value)             (MCAN_TXBCIE_CFIE18_Msk & ((value) << MCAN_TXBCIE_CFIE18_Pos))
#define MCAN_TXBCIE_CFIE19_Pos                (19U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 19 Position */
#define MCAN_TXBCIE_CFIE19_Msk                (0x1U << MCAN_TXBCIE_CFIE19_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 19 Mask */
#define MCAN_TXBCIE_CFIE19(value)             (MCAN_TXBCIE_CFIE19_Msk & ((value) << MCAN_TXBCIE_CFIE19_Pos))
#define MCAN_TXBCIE_CFIE20_Pos                (20U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 20 Position */
#define MCAN_TXBCIE_CFIE20_Msk                (0x1U << MCAN_TXBCIE_CFIE20_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 20 Mask */
#define MCAN_TXBCIE_CFIE20(value)             (MCAN_TXBCIE_CFIE20_Msk & ((value) << MCAN_TXBCIE_CFIE20_Pos))
#define MCAN_TXBCIE_CFIE21_Pos                (21U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 21 Position */
#define MCAN_TXBCIE_CFIE21_Msk                (0x1U << MCAN_TXBCIE_CFIE21_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 21 Mask */
#define MCAN_TXBCIE_CFIE21(value)             (MCAN_TXBCIE_CFIE21_Msk & ((value) << MCAN_TXBCIE_CFIE21_Pos))
#define MCAN_TXBCIE_CFIE22_Pos                (22U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 22 Position */
#define MCAN_TXBCIE_CFIE22_Msk                (0x1U << MCAN_TXBCIE_CFIE22_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 22 Mask */
#define MCAN_TXBCIE_CFIE22(value)             (MCAN_TXBCIE_CFIE22_Msk & ((value) << MCAN_TXBCIE_CFIE22_Pos))
#define MCAN_TXBCIE_CFIE23_Pos                (23U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 23 Position */
#define MCAN_TXBCIE_CFIE23_Msk                (0x1U << MCAN_TXBCIE_CFIE23_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 23 Mask */
#define MCAN_TXBCIE_CFIE23(value)             (MCAN_TXBCIE_CFIE23_Msk & ((value) << MCAN_TXBCIE_CFIE23_Pos))
#define MCAN_TXBCIE_CFIE24_Pos                (24U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 24 Position */
#define MCAN_TXBCIE_CFIE24_Msk                (0x1U << MCAN_TXBCIE_CFIE24_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 24 Mask */
#define MCAN_TXBCIE_CFIE24(value)             (MCAN_TXBCIE_CFIE24_Msk & ((value) << MCAN_TXBCIE_CFIE24_Pos))
#define MCAN_TXBCIE_CFIE25_Pos                (25U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 25 Position */
#define MCAN_TXBCIE_CFIE25_Msk                (0x1U << MCAN_TXBCIE_CFIE25_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 25 Mask */
#define MCAN_TXBCIE_CFIE25(value)             (MCAN_TXBCIE_CFIE25_Msk & ((value) << MCAN_TXBCIE_CFIE25_Pos))
#define MCAN_TXBCIE_CFIE26_Pos                (26U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 26 Position */
#define MCAN_TXBCIE_CFIE26_Msk                (0x1U << MCAN_TXBCIE_CFIE26_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 26 Mask */
#define MCAN_TXBCIE_CFIE26(value)             (MCAN_TXBCIE_CFIE26_Msk & ((value) << MCAN_TXBCIE_CFIE26_Pos))
#define MCAN_TXBCIE_CFIE27_Pos                (27U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 27 Position */
#define MCAN_TXBCIE_CFIE27_Msk                (0x1U << MCAN_TXBCIE_CFIE27_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 27 Mask */
#define MCAN_TXBCIE_CFIE27(value)             (MCAN_TXBCIE_CFIE27_Msk & ((value) << MCAN_TXBCIE_CFIE27_Pos))
#define MCAN_TXBCIE_CFIE28_Pos                (28U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 28 Position */
#define MCAN_TXBCIE_CFIE28_Msk                (0x1U << MCAN_TXBCIE_CFIE28_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 28 Mask */
#define MCAN_TXBCIE_CFIE28(value)             (MCAN_TXBCIE_CFIE28_Msk & ((value) << MCAN_TXBCIE_CFIE28_Pos))
#define MCAN_TXBCIE_CFIE29_Pos                (29U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 29 Position */
#define MCAN_TXBCIE_CFIE29_Msk                (0x1U << MCAN_TXBCIE_CFIE29_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 29 Mask */
#define MCAN_TXBCIE_CFIE29(value)             (MCAN_TXBCIE_CFIE29_Msk & ((value) << MCAN_TXBCIE_CFIE29_Pos))
#define MCAN_TXBCIE_CFIE30_Pos                (30U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 30 Position */
#define MCAN_TXBCIE_CFIE30_Msk                (0x1U << MCAN_TXBCIE_CFIE30_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 30 Mask */
#define MCAN_TXBCIE_CFIE30(value)             (MCAN_TXBCIE_CFIE30_Msk & ((value) << MCAN_TXBCIE_CFIE30_Pos))
#define MCAN_TXBCIE_CFIE31_Pos                (31U)                                          /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 31 Position */
#define MCAN_TXBCIE_CFIE31_Msk                (0x1U << MCAN_TXBCIE_CFIE31_Pos)               /**< (MCAN_TXBCIE) Cancellation Finished Interrupt Enable for Transmit Buffer 31 Mask */
#define MCAN_TXBCIE_CFIE31(value)             (MCAN_TXBCIE_CFIE31_Msk & ((value) << MCAN_TXBCIE_CFIE31_Pos))
#define MCAN_TXBCIE_Msk                       (0xFFFFFFFFUL)                                 /**< (MCAN_TXBCIE) Register Mask  */

/* -------- MCAN_TXEFC : (MCAN Offset: 0xF0) (R/W 32) Transmit Event FIFO Configuration Register -------- */
#define MCAN_TXEFC_EFSA_Pos                   (2U)                                           /**< (MCAN_TXEFC) Event FIFO Start Address Position */
#define MCAN_TXEFC_EFSA_Msk                   (0x3FFFU << MCAN_TXEFC_EFSA_Pos)               /**< (MCAN_TXEFC) Event FIFO Start Address Mask */
#define MCAN_TXEFC_EFSA(value)                (MCAN_TXEFC_EFSA_Msk & ((value) << MCAN_TXEFC_EFSA_Pos))
#define MCAN_TXEFC_EFS_Pos                    (16U)                                          /**< (MCAN_TXEFC) Event FIFO Size Position */
#define MCAN_TXEFC_EFS_Msk                    (0x3FU << MCAN_TXEFC_EFS_Pos)                  /**< (MCAN_TXEFC) Event FIFO Size Mask */
#define MCAN_TXEFC_EFS(value)                 (MCAN_TXEFC_EFS_Msk & ((value) << MCAN_TXEFC_EFS_Pos))
#define MCAN_TXEFC_EFWM_Pos                   (24U)                                          /**< (MCAN_TXEFC) Event FIFO Watermark Position */
#define MCAN_TXEFC_EFWM_Msk                   (0x3FU << MCAN_TXEFC_EFWM_Pos)                 /**< (MCAN_TXEFC) Event FIFO Watermark Mask */
#define MCAN_TXEFC_EFWM(value)                (MCAN_TXEFC_EFWM_Msk & ((value) << MCAN_TXEFC_EFWM_Pos))
#define MCAN_TXEFC_Msk                        (0x3F3FFFFCUL)                                 /**< (MCAN_TXEFC) Register Mask  */

/* -------- MCAN_TXEFS : (MCAN Offset: 0xF4) (R/  32) Transmit Event FIFO Status Register -------- */
#define MCAN_TXEFS_EFFL_Pos                   (0U)                                           /**< (MCAN_TXEFS) Event FIFO Fill Level Position */
#define MCAN_TXEFS_EFFL_Msk                   (0x3FU << MCAN_TXEFS_EFFL_Pos)                 /**< (MCAN_TXEFS) Event FIFO Fill Level Mask */
#define MCAN_TXEFS_EFFL(value)                (MCAN_TXEFS_EFFL_Msk & ((value) << MCAN_TXEFS_EFFL_Pos))
#define MCAN_TXEFS_EFGI_Pos                   (8U)                                           /**< (MCAN_TXEFS) Event FIFO Get Index Position */
#define MCAN_TXEFS_EFGI_Msk                   (0x1FU << MCAN_TXEFS_EFGI_Pos)                 /**< (MCAN_TXEFS) Event FIFO Get Index Mask */
#define MCAN_TXEFS_EFGI(value)                (MCAN_TXEFS_EFGI_Msk & ((value) << MCAN_TXEFS_EFGI_Pos))
#define MCAN_TXEFS_EFPI_Pos                   (16U)                                          /**< (MCAN_TXEFS) Event FIFO Put Index Position */
#define MCAN_TXEFS_EFPI_Msk                   (0x1FU << MCAN_TXEFS_EFPI_Pos)                 /**< (MCAN_TXEFS) Event FIFO Put Index Mask */
#define MCAN_TXEFS_EFPI(value)                (MCAN_TXEFS_EFPI_Msk & ((value) << MCAN_TXEFS_EFPI_Pos))
#define MCAN_TXEFS_EFF_Pos                    (24U)                                          /**< (MCAN_TXEFS) Event FIFO Full Position */
#define MCAN_TXEFS_EFF_Msk                    (0x1U << MCAN_TXEFS_EFF_Pos)                   /**< (MCAN_TXEFS) Event FIFO Full Mask */
#define MCAN_TXEFS_EFF(value)                 (MCAN_TXEFS_EFF_Msk & ((value) << MCAN_TXEFS_EFF_Pos))
#define MCAN_TXEFS_TEFL_Pos                   (25U)                                          /**< (MCAN_TXEFS) Tx Event FIFO Element Lost Position */
#define MCAN_TXEFS_TEFL_Msk                   (0x1U << MCAN_TXEFS_TEFL_Pos)                  /**< (MCAN_TXEFS) Tx Event FIFO Element Lost Mask */
#define MCAN_TXEFS_TEFL(value)                (MCAN_TXEFS_TEFL_Msk & ((value) << MCAN_TXEFS_TEFL_Pos))
#define MCAN_TXEFS_Msk                        (0x031F1F3FUL)                                 /**< (MCAN_TXEFS) Register Mask  */

/* -------- MCAN_TXEFA : (MCAN Offset: 0xF8) (R/W 32) Transmit Event FIFO Acknowledge Register -------- */
#define MCAN_TXEFA_EFAI_Pos                   (0U)                                           /**< (MCAN_TXEFA) Event FIFO Acknowledge Index Position */
#define MCAN_TXEFA_EFAI_Msk                   (0x1FU << MCAN_TXEFA_EFAI_Pos)                 /**< (MCAN_TXEFA) Event FIFO Acknowledge Index Mask */
#define MCAN_TXEFA_EFAI(value)                (MCAN_TXEFA_EFAI_Msk & ((value) << MCAN_TXEFA_EFAI_Pos))
#define MCAN_TXEFA_Msk                        (0x0000001FUL)                                 /**< (MCAN_TXEFA) Register Mask  */

/** \brief MCAN register offsets definitions */
#define MCAN_CREL_OFFSET               (0x00)         /**< (MCAN_CREL) Core Release Register Offset */
#define MCAN_ENDN_OFFSET               (0x04)         /**< (MCAN_ENDN) Endian Register Offset */
#define MCAN_CUST_OFFSET               (0x08)         /**< (MCAN_CUST) Customer Register Offset */
#define MCAN_DBTP_OFFSET               (0x0C)         /**< (MCAN_DBTP) Data Bit Timing and Prescaler Register Offset */
#define MCAN_TEST_OFFSET               (0x10)         /**< (MCAN_TEST) Test Register Offset */
#define MCAN_RWD_OFFSET                (0x14)         /**< (MCAN_RWD) RAM Watchdog Register Offset */
#define MCAN_CCCR_OFFSET               (0x18)         /**< (MCAN_CCCR) CC Control Register Offset */
#define MCAN_NBTP_OFFSET               (0x1C)         /**< (MCAN_NBTP) Nominal Bit Timing and Prescaler Register Offset */
#define MCAN_TSCC_OFFSET               (0x20)         /**< (MCAN_TSCC) Timestamp Counter Configuration Register Offset */
#define MCAN_TSCV_OFFSET               (0x24)         /**< (MCAN_TSCV) Timestamp Counter Value Register Offset */
#define MCAN_TOCC_OFFSET               (0x28)         /**< (MCAN_TOCC) Timeout Counter Configuration Register Offset */
#define MCAN_TOCV_OFFSET               (0x2C)         /**< (MCAN_TOCV) Timeout Counter Value Register Offset */
#define MCAN_ECR_OFFSET                (0x40)         /**< (MCAN_ECR) Error Counter Register Offset */
#define MCAN_PSR_OFFSET                (0x44)         /**< (MCAN_PSR) Protocol Status Register Offset */
#define MCAN_TDCR_OFFSET               (0x48)         /**< (MCAN_TDCR) Transmit Delay Compensation Register Offset */
#define MCAN_IR_OFFSET                 (0x50)         /**< (MCAN_IR) Interrupt Register Offset */
#define MCAN_IE_OFFSET                 (0x54)         /**< (MCAN_IE) Interrupt Enable Register Offset */
#define MCAN_ILS_OFFSET                (0x58)         /**< (MCAN_ILS) Interrupt Line Select Register Offset */
#define MCAN_ILE_OFFSET                (0x5C)         /**< (MCAN_ILE) Interrupt Line Enable Register Offset */
#define MCAN_GFC_OFFSET                (0x80)         /**< (MCAN_GFC) Global Filter Configuration Register Offset */
#define MCAN_SIDFC_OFFSET              (0x84)         /**< (MCAN_SIDFC) Standard ID Filter Configuration Register Offset */
#define MCAN_XIDFC_OFFSET              (0x88)         /**< (MCAN_XIDFC) Extended ID Filter Configuration Register Offset */
#define MCAN_XIDAM_OFFSET              (0x90)         /**< (MCAN_XIDAM) Extended ID AND Mask Register Offset */
#define MCAN_HPMS_OFFSET               (0x94)         /**< (MCAN_HPMS) High Priority Message Status Register Offset */
#define MCAN_NDAT1_OFFSET              (0x98)         /**< (MCAN_NDAT1) New Data 1 Register Offset */
#define MCAN_NDAT2_OFFSET              (0x9C)         /**< (MCAN_NDAT2) New Data 2 Register Offset */
#define MCAN_RXF0C_OFFSET              (0xA0)         /**< (MCAN_RXF0C) Receive FIFO 0 Configuration Register Offset */
#define MCAN_RXF0S_OFFSET              (0xA4)         /**< (MCAN_RXF0S) Receive FIFO 0 Status Register Offset */
#define MCAN_RXF0A_OFFSET              (0xA8)         /**< (MCAN_RXF0A) Receive FIFO 0 Acknowledge Register Offset */
#define MCAN_RXBC_OFFSET               (0xAC)         /**< (MCAN_RXBC) Receive Rx Buffer Configuration Register Offset */
#define MCAN_RXF1C_OFFSET              (0xB0)         /**< (MCAN_RXF1C) Receive FIFO 1 Configuration Register Offset */
#define MCAN_RXF1S_OFFSET              (0xB4)         /**< (MCAN_RXF1S) Receive FIFO 1 Status Register Offset */
#define MCAN_RXF1A_OFFSET              (0xB8)         /**< (MCAN_RXF1A) Receive FIFO 1 Acknowledge Register Offset */
#define MCAN_RXESC_OFFSET              (0xBC)         /**< (MCAN_RXESC) Receive Buffer / FIFO Element Size Configuration Register Offset */
#define MCAN_TXBC_OFFSET               (0xC0)         /**< (MCAN_TXBC) Transmit Buffer Configuration Register Offset */
#define MCAN_TXFQS_OFFSET              (0xC4)         /**< (MCAN_TXFQS) Transmit FIFO/Queue Status Register Offset */
#define MCAN_TXESC_OFFSET              (0xC8)         /**< (MCAN_TXESC) Transmit Buffer Element Size Configuration Register Offset */
#define MCAN_TXBRP_OFFSET              (0xCC)         /**< (MCAN_TXBRP) Transmit Buffer Request Pending Register Offset */
#define MCAN_TXBAR_OFFSET              (0xD0)         /**< (MCAN_TXBAR) Transmit Buffer Add Request Register Offset */
#define MCAN_TXBCR_OFFSET              (0xD4)         /**< (MCAN_TXBCR) Transmit Buffer Cancellation Request Register Offset */
#define MCAN_TXBTO_OFFSET              (0xD8)         /**< (MCAN_TXBTO) Transmit Buffer Transmission Occurred Register Offset */
#define MCAN_TXBCF_OFFSET              (0xDC)         /**< (MCAN_TXBCF) Transmit Buffer Cancellation Finished Register Offset */
#define MCAN_TXBTIE_OFFSET             (0xE0)         /**< (MCAN_TXBTIE) Transmit Buffer Transmission Interrupt Enable Register Offset */
#define MCAN_TXBCIE_OFFSET             (0xE4)         /**< (MCAN_TXBCIE) Transmit Buffer Cancellation Finished Interrupt Enable Register Offset */
#define MCAN_TXEFC_OFFSET              (0xF0)         /**< (MCAN_TXEFC) Transmit Event FIFO Configuration Register Offset */
#define MCAN_TXEFS_OFFSET              (0xF4)         /**< (MCAN_TXEFS) Transmit Event FIFO Status Register Offset */
#define MCAN_TXEFA_OFFSET              (0xF8)         /**< (MCAN_TXEFA) Transmit Event FIFO Acknowledge Register Offset */

/** \brief MCAN register API structure */
typedef struct
{
  __I   uint32_t                       MCAN_CREL;       /**< Offset: 0x00 (R/   32) Core Release Register */
  __I   uint32_t                       MCAN_ENDN;       /**< Offset: 0x04 (R/   32) Endian Register */
  __IO  uint32_t                       MCAN_CUST;       /**< Offset: 0x08 (R/W  32) Customer Register */
  __IO  uint32_t                       MCAN_DBTP;       /**< Offset: 0x0c (R/W  32) Data Bit Timing and Prescaler Register */
  __IO  uint32_t                       MCAN_TEST;       /**< Offset: 0x10 (R/W  32) Test Register */
  __IO  uint32_t                       MCAN_RWD;        /**< Offset: 0x14 (R/W  32) RAM Watchdog Register */
  __IO  uint32_t                       MCAN_CCCR;       /**< Offset: 0x18 (R/W  32) CC Control Register */
  __IO  uint32_t                       MCAN_NBTP;       /**< Offset: 0x1c (R/W  32) Nominal Bit Timing and Prescaler Register */
  __IO  uint32_t                       MCAN_TSCC;       /**< Offset: 0x20 (R/W  32) Timestamp Counter Configuration Register */
  __IO  uint32_t                       MCAN_TSCV;       /**< Offset: 0x24 (R/W  32) Timestamp Counter Value Register */
  __IO  uint32_t                       MCAN_TOCC;       /**< Offset: 0x28 (R/W  32) Timeout Counter Configuration Register */
  __IO  uint32_t                       MCAN_TOCV;       /**< Offset: 0x2c (R/W  32) Timeout Counter Value Register */
  __I   uint8_t                        Reserved1[0x10];
  __I   uint32_t                       MCAN_ECR;        /**< Offset: 0x40 (R/   32) Error Counter Register */
  __I   uint32_t                       MCAN_PSR;        /**< Offset: 0x44 (R/   32) Protocol Status Register */
  __IO  uint32_t                       MCAN_TDCR;       /**< Offset: 0x48 (R/W  32) Transmit Delay Compensation Register */
  __I   uint8_t                        Reserved2[0x04];
  __IO  uint32_t                       MCAN_IR;         /**< Offset: 0x50 (R/W  32) Interrupt Register */
  __IO  uint32_t                       MCAN_IE;         /**< Offset: 0x54 (R/W  32) Interrupt Enable Register */
  __IO  uint32_t                       MCAN_ILS;        /**< Offset: 0x58 (R/W  32) Interrupt Line Select Register */
  __IO  uint32_t                       MCAN_ILE;        /**< Offset: 0x5c (R/W  32) Interrupt Line Enable Register */
  __I   uint8_t                        Reserved3[0x20];
  __IO  uint32_t                       MCAN_GFC;        /**< Offset: 0x80 (R/W  32) Global Filter Configuration Register */
  __IO  uint32_t                       MCAN_SIDFC;      /**< Offset: 0x84 (R/W  32) Standard ID Filter Configuration Register */
  __IO  uint32_t                       MCAN_XIDFC;      /**< Offset: 0x88 (R/W  32) Extended ID Filter Configuration Register */
  __I   uint8_t                        Reserved4[0x04];
  __IO  uint32_t                       MCAN_XIDAM;      /**< Offset: 0x90 (R/W  32) Extended ID AND Mask Register */
  __I   uint32_t                       MCAN_HPMS;       /**< Offset: 0x94 (R/   32) High Priority Message Status Register */
  __IO  uint32_t                       MCAN_NDAT1;      /**< Offset: 0x98 (R/W  32) New Data 1 Register */
  __IO  uint32_t                       MCAN_NDAT2;      /**< Offset: 0x9c (R/W  32) New Data 2 Register */
  __IO  uint32_t                       MCAN_RXF0C;      /**< Offset: 0xa0 (R/W  32) Receive FIFO 0 Configuration Register */
  __I   uint32_t                       MCAN_RXF0S;      /**< Offset: 0xa4 (R/   32) Receive FIFO 0 Status Register */
  __IO  uint32_t                       MCAN_RXF0A;      /**< Offset: 0xa8 (R/W  32) Receive FIFO 0 Acknowledge Register */
  __IO  uint32_t                       MCAN_RXBC;       /**< Offset: 0xac (R/W  32) Receive Rx Buffer Configuration Register */
  __IO  uint32_t                       MCAN_RXF1C;      /**< Offset: 0xb0 (R/W  32) Receive FIFO 1 Configuration Register */
  __I   uint32_t                       MCAN_RXF1S;      /**< Offset: 0xb4 (R/   32) Receive FIFO 1 Status Register */
  __IO  uint32_t                       MCAN_RXF1A;      /**< Offset: 0xb8 (R/W  32) Receive FIFO 1 Acknowledge Register */
  __IO  uint32_t                       MCAN_RXESC;      /**< Offset: 0xbc (R/W  32) Receive Buffer / FIFO Element Size Configuration Register */
  __IO  uint32_t                       MCAN_TXBC;       /**< Offset: 0xc0 (R/W  32) Transmit Buffer Configuration Register */
  __I   uint32_t                       MCAN_TXFQS;      /**< Offset: 0xc4 (R/   32) Transmit FIFO/Queue Status Register */
  __IO  uint32_t                       MCAN_TXESC;      /**< Offset: 0xc8 (R/W  32) Transmit Buffer Element Size Configuration Register */
  __I   uint32_t                       MCAN_TXBRP;      /**< Offset: 0xcc (R/   32) Transmit Buffer Request Pending Register */
  __IO  uint32_t                       MCAN_TXBAR;      /**< Offset: 0xd0 (R/W  32) Transmit Buffer Add Request Register */
  __IO  uint32_t                       MCAN_TXBCR;      /**< Offset: 0xd4 (R/W  32) Transmit Buffer Cancellation Request Register */
  __I   uint32_t                       MCAN_TXBTO;      /**< Offset: 0xd8 (R/   32) Transmit Buffer Transmission Occurred Register */
  __I   uint32_t                       MCAN_TXBCF;      /**< Offset: 0xdc (R/   32) Transmit Buffer Cancellation Finished Register */
  __IO  uint32_t                       MCAN_TXBTIE;     /**< Offset: 0xe0 (R/W  32) Transmit Buffer Transmission Interrupt Enable Register */
  __IO  uint32_t                       MCAN_TXBCIE;     /**< Offset: 0xe4 (R/W  32) Transmit Buffer Cancellation Finished Interrupt Enable Register */
  __I   uint8_t                        Reserved5[0x08];
  __IO  uint32_t                       MCAN_TXEFC;      /**< Offset: 0xf0 (R/W  32) Transmit Event FIFO Configuration Register */
  __I   uint32_t                       MCAN_TXEFS;      /**< Offset: 0xf4 (R/   32) Transmit Event FIFO Status Register */
  __IO  uint32_t                       MCAN_TXEFA;      /**< Offset: 0xf8 (R/W  32) Transmit Event FIFO Acknowledge Register */
} mcan_registers_t;
/** @}  end of Controller Area Network */

#endif /* SAME_SAME70_MCAN_MODULE_H */

