/**
 * \brief Component description for ETM
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip software and any derivatives
 * exclusively with Microchip products. It is your responsibility to comply with third party license
 * terms applicable to your use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY,
 * APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT
 * EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/* file generated from device description version 2018-12-05T04:45:25Z */
#ifndef _SAME54_ETM_COMPONENT_H_
#define _SAME54_ETM_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR ETM                                          */
/* ************************************************************************** */

/* -------- ETM_CR : (ETM Offset: 0x00) (R/W 32) ETM Main Control Register -------- */
#define ETM_CR_RESETVALUE                     _U_(0x411)                                           /**<  (ETM_CR) ETM Main Control Register  Reset Value */

#define ETM_CR_ETMPD_Pos                      _U_(0)                                               /**< (ETM_CR) ETM Power Down Position */
#define ETM_CR_ETMPD_Msk                      (_U_(0x1) << ETM_CR_ETMPD_Pos)                       /**< (ETM_CR) ETM Power Down Mask */
#define ETM_CR_ETMPD(value)                   (ETM_CR_ETMPD_Msk & ((value) << ETM_CR_ETMPD_Pos))  
#define ETM_CR_PORTSIZE_Pos                   _U_(4)                                               /**< (ETM_CR) Port Size bits 2:0 Position */
#define ETM_CR_PORTSIZE_Msk                   (_U_(0x7) << ETM_CR_PORTSIZE_Pos)                    /**< (ETM_CR) Port Size bits 2:0 Mask */
#define ETM_CR_PORTSIZE(value)                (ETM_CR_PORTSIZE_Msk & ((value) << ETM_CR_PORTSIZE_Pos))
#define ETM_CR_STALL_Pos                      _U_(7)                                               /**< (ETM_CR) Stall Processor Position */
#define ETM_CR_STALL_Msk                      (_U_(0x1) << ETM_CR_STALL_Pos)                       /**< (ETM_CR) Stall Processor Mask */
#define ETM_CR_STALL(value)                   (ETM_CR_STALL_Msk & ((value) << ETM_CR_STALL_Pos))  
#define ETM_CR_BROUT_Pos                      _U_(8)                                               /**< (ETM_CR) Branch Output Position */
#define ETM_CR_BROUT_Msk                      (_U_(0x1) << ETM_CR_BROUT_Pos)                       /**< (ETM_CR) Branch Output Mask */
#define ETM_CR_BROUT(value)                   (ETM_CR_BROUT_Msk & ((value) << ETM_CR_BROUT_Pos))  
#define ETM_CR_DBGRQ_Pos                      _U_(9)                                               /**< (ETM_CR) Debug Request Control Position */
#define ETM_CR_DBGRQ_Msk                      (_U_(0x1) << ETM_CR_DBGRQ_Pos)                       /**< (ETM_CR) Debug Request Control Mask */
#define ETM_CR_DBGRQ(value)                   (ETM_CR_DBGRQ_Msk & ((value) << ETM_CR_DBGRQ_Pos))  
#define ETM_CR_PROG_Pos                       _U_(10)                                              /**< (ETM_CR) ETM Programming Position */
#define ETM_CR_PROG_Msk                       (_U_(0x1) << ETM_CR_PROG_Pos)                        /**< (ETM_CR) ETM Programming Mask */
#define ETM_CR_PROG(value)                    (ETM_CR_PROG_Msk & ((value) << ETM_CR_PROG_Pos))    
#define ETM_CR_PORTSEL_Pos                    _U_(11)                                              /**< (ETM_CR) ETM Port Select Position */
#define ETM_CR_PORTSEL_Msk                    (_U_(0x1) << ETM_CR_PORTSEL_Pos)                     /**< (ETM_CR) ETM Port Select Mask */
#define ETM_CR_PORTSEL(value)                 (ETM_CR_PORTSEL_Msk & ((value) << ETM_CR_PORTSEL_Pos))
#define ETM_CR_PORTMODE2_Pos                  _U_(13)                                              /**< (ETM_CR) Port Mode bit 2 Position */
#define ETM_CR_PORTMODE2_Msk                  (_U_(0x1) << ETM_CR_PORTMODE2_Pos)                   /**< (ETM_CR) Port Mode bit 2 Mask */
#define ETM_CR_PORTMODE2(value)               (ETM_CR_PORTMODE2_Msk & ((value) << ETM_CR_PORTMODE2_Pos))
#define ETM_CR_PORTMODE_Pos                   _U_(16)                                              /**< (ETM_CR) Port Mode bits 1:0 Position */
#define ETM_CR_PORTMODE_Msk                   (_U_(0x3) << ETM_CR_PORTMODE_Pos)                    /**< (ETM_CR) Port Mode bits 1:0 Mask */
#define ETM_CR_PORTMODE(value)                (ETM_CR_PORTMODE_Msk & ((value) << ETM_CR_PORTMODE_Pos))
#define ETM_CR_PORTSIZE3_Pos                  _U_(21)                                              /**< (ETM_CR) Port Size bit 3 Position */
#define ETM_CR_PORTSIZE3_Msk                  (_U_(0x1) << ETM_CR_PORTSIZE3_Pos)                   /**< (ETM_CR) Port Size bit 3 Mask */
#define ETM_CR_PORTSIZE3(value)               (ETM_CR_PORTSIZE3_Msk & ((value) << ETM_CR_PORTSIZE3_Pos))
#define ETM_CR_TSEN_Pos                       _U_(28)                                              /**< (ETM_CR) TimeStamp Enable Position */
#define ETM_CR_TSEN_Msk                       (_U_(0x1) << ETM_CR_TSEN_Pos)                        /**< (ETM_CR) TimeStamp Enable Mask */
#define ETM_CR_TSEN(value)                    (ETM_CR_TSEN_Msk & ((value) << ETM_CR_TSEN_Pos))    
#define ETM_CR_Msk                            _U_(0x10232FF1)                                      /**< (ETM_CR) Register Mask  */


/* -------- ETM_CCR : (ETM Offset: 0x04) ( R/ 32) ETM Configuration Code Register -------- */
#define ETM_CCR_RESETVALUE                    _U_(0x8C802000)                                      /**<  (ETM_CCR) ETM Configuration Code Register  Reset Value */

#define ETM_CCR_Msk                           _U_(0x00000000)                                      /**< (ETM_CCR) Register Mask  */


/* -------- ETM_TRIGGER : (ETM Offset: 0x08) (R/W 32) ETM Trigger Event Register -------- */
#define ETM_TRIGGER_Msk                       _U_(0x00000000)                                      /**< (ETM_TRIGGER) Register Mask  */


/* -------- ETM_SR : (ETM Offset: 0x10) (R/W 32) ETM Status Register -------- */
#define ETM_SR_Msk                            _U_(0x00000000)                                      /**< (ETM_SR) Register Mask  */


/* -------- ETM_SCR : (ETM Offset: 0x14) ( R/ 32) ETM System Configuration Register -------- */
#define ETM_SCR_RESETVALUE                    _U_(0x20D09)                                         /**<  (ETM_SCR) ETM System Configuration Register  Reset Value */

#define ETM_SCR_Msk                           _U_(0x00000000)                                      /**< (ETM_SCR) Register Mask  */


/* -------- ETM_TEEVR : (ETM Offset: 0x20) (R/W 32) ETM TraceEnable Event Register -------- */
#define ETM_TEEVR_Msk                         _U_(0x00000000)                                      /**< (ETM_TEEVR) Register Mask  */


/* -------- ETM_TECR1 : (ETM Offset: 0x24) (R/W 32) ETM TraceEnable Control 1 Register -------- */
#define ETM_TECR1_Msk                         _U_(0x00000000)                                      /**< (ETM_TECR1) Register Mask  */


/* -------- ETM_FFLR : (ETM Offset: 0x28) (R/W 32) ETM FIFO Full Level Register -------- */
#define ETM_FFLR_Msk                          _U_(0x00000000)                                      /**< (ETM_FFLR) Register Mask  */


/* -------- ETM_CNTRLDVR1 : (ETM Offset: 0x140) (R/W 32) ETM Free-running Counter Reload Value -------- */
#define ETM_CNTRLDVR1_Msk                     _U_(0x00000000)                                      /**< (ETM_CNTRLDVR1) Register Mask  */


/* -------- ETM_SYNCFR : (ETM Offset: 0x1E0) ( R/ 32) ETM Synchronization Frequency Register -------- */
#define ETM_SYNCFR_RESETVALUE                 _U_(0x400)                                           /**<  (ETM_SYNCFR) ETM Synchronization Frequency Register  Reset Value */

#define ETM_SYNCFR_Msk                        _U_(0x00000000)                                      /**< (ETM_SYNCFR) Register Mask  */


/* -------- ETM_IDR : (ETM Offset: 0x1E4) ( R/ 32) ETM ID Register -------- */
#define ETM_IDR_RESETVALUE                    _U_(0x4114F250)                                      /**<  (ETM_IDR) ETM ID Register  Reset Value */

#define ETM_IDR_Msk                           _U_(0x00000000)                                      /**< (ETM_IDR) Register Mask  */


/* -------- ETM_CCER : (ETM Offset: 0x1E8) ( R/ 32) ETM Configuration Code Extension Register -------- */
#define ETM_CCER_RESETVALUE                   _U_(0x18541800)                                      /**<  (ETM_CCER) ETM Configuration Code Extension Register  Reset Value */

#define ETM_CCER_Msk                          _U_(0x00000000)                                      /**< (ETM_CCER) Register Mask  */


/* -------- ETM_TESSEICR : (ETM Offset: 0x1F0) (R/W 32) ETM TraceEnable Start/Stop EmbeddedICE Control Register -------- */
#define ETM_TESSEICR_Msk                      _U_(0x00000000)                                      /**< (ETM_TESSEICR) Register Mask  */


/* -------- ETM_TSEVT : (ETM Offset: 0x1F8) (R/W 32) ETM TimeStamp Event Register -------- */
#define ETM_TSEVT_Msk                         _U_(0x00000000)                                      /**< (ETM_TSEVT) Register Mask  */


/* -------- ETM_TRACEIDR : (ETM Offset: 0x200) (R/W 32) ETM CoreSight Trace ID Register -------- */
#define ETM_TRACEIDR_RESETVALUE               _U_(0x00)                                            /**<  (ETM_TRACEIDR) ETM CoreSight Trace ID Register  Reset Value */

#define ETM_TRACEIDR_Msk                      _U_(0x00000000)                                      /**< (ETM_TRACEIDR) Register Mask  */


/* -------- ETM_IDR2 : (ETM Offset: 0x208) ( R/ 32) ETM ID Register 2 -------- */
#define ETM_IDR2_RESETVALUE                   _U_(0x00)                                            /**<  (ETM_IDR2) ETM ID Register 2  Reset Value */

#define ETM_IDR2_Msk                          _U_(0x00000000)                                      /**< (ETM_IDR2) Register Mask  */


/* -------- ETM_PDSR : (ETM Offset: 0x314) ( R/ 32) ETM Device Power-Down Status Register -------- */
#define ETM_PDSR_RESETVALUE                   _U_(0x01)                                            /**<  (ETM_PDSR) ETM Device Power-Down Status Register  Reset Value */

#define ETM_PDSR_Msk                          _U_(0x00000000)                                      /**< (ETM_PDSR) Register Mask  */


/* -------- ETM_ITMISCIN : (ETM Offset: 0xEE0) ( R/ 32) ETM Integration Test Miscellaneous Inputs -------- */
#define ETM_ITMISCIN_Msk                      _U_(0x00000000)                                      /**< (ETM_ITMISCIN) Register Mask  */


/* -------- ETM_ITTRIGOUT : (ETM Offset: 0xEE8) ( /W 32) ETM Integration Test Trigger Out -------- */
#define ETM_ITTRIGOUT_Msk                     _U_(0x00000000)                                      /**< (ETM_ITTRIGOUT) Register Mask  */


/* -------- ETM_ITATBCTR2 : (ETM Offset: 0xEF0) ( R/ 32) ETM Integration Test ATB Control 2 -------- */
#define ETM_ITATBCTR2_Msk                     _U_(0x00000000)                                      /**< (ETM_ITATBCTR2) Register Mask  */


/* -------- ETM_ITATBCTR0 : (ETM Offset: 0xEF8) ( /W 32) ETM Integration Test ATB Control 0 -------- */
#define ETM_ITATBCTR0_Msk                     _U_(0x00000000)                                      /**< (ETM_ITATBCTR0) Register Mask  */


/* -------- ETM_ITCTRL : (ETM Offset: 0xF00) (R/W 32) ETM Integration Mode Control Register -------- */
#define ETM_ITCTRL_RESETVALUE                 _U_(0x00)                                            /**<  (ETM_ITCTRL) ETM Integration Mode Control Register  Reset Value */

#define ETM_ITCTRL_INTEGRATION_Pos            _U_(0)                                               /**< (ETM_ITCTRL)  Position */
#define ETM_ITCTRL_INTEGRATION_Msk            (_U_(0x1) << ETM_ITCTRL_INTEGRATION_Pos)             /**< (ETM_ITCTRL)  Mask */
#define ETM_ITCTRL_INTEGRATION(value)         (ETM_ITCTRL_INTEGRATION_Msk & ((value) << ETM_ITCTRL_INTEGRATION_Pos))
#define ETM_ITCTRL_Msk                        _U_(0x00000001)                                      /**< (ETM_ITCTRL) Register Mask  */


/* -------- ETM_CLAIMSET : (ETM Offset: 0xFA0) (R/W 32) ETM Claim Tag Set Register -------- */
#define ETM_CLAIMSET_Msk                      _U_(0x00000000)                                      /**< (ETM_CLAIMSET) Register Mask  */


/* -------- ETM_CLAIMCLR : (ETM Offset: 0xFA4) (R/W 32) ETM Claim Tag Clear Register -------- */
#define ETM_CLAIMCLR_Msk                      _U_(0x00000000)                                      /**< (ETM_CLAIMCLR) Register Mask  */


/* -------- ETM_LAR : (ETM Offset: 0xFB0) ( /W 32) ETM Lock Access Register -------- */
#define ETM_LAR_Msk                           _U_(0x00000000)                                      /**< (ETM_LAR) Register Mask  */


/* -------- ETM_LSR : (ETM Offset: 0xFB4) ( R/ 32) ETM Lock Status Register -------- */
#define ETM_LSR_Present_Pos                   _U_(0)                                               /**< (ETM_LSR)  Position */
#define ETM_LSR_Present_Msk                   (_U_(0x1) << ETM_LSR_Present_Pos)                    /**< (ETM_LSR)  Mask */
#define ETM_LSR_Present(value)                (ETM_LSR_Present_Msk & ((value) << ETM_LSR_Present_Pos))
#define ETM_LSR_Access_Pos                    _U_(1)                                               /**< (ETM_LSR)  Position */
#define ETM_LSR_Access_Msk                    (_U_(0x1) << ETM_LSR_Access_Pos)                     /**< (ETM_LSR)  Mask */
#define ETM_LSR_Access(value)                 (ETM_LSR_Access_Msk & ((value) << ETM_LSR_Access_Pos))
#define ETM_LSR_ByteAcc_Pos                   _U_(2)                                               /**< (ETM_LSR)  Position */
#define ETM_LSR_ByteAcc_Msk                   (_U_(0x1) << ETM_LSR_ByteAcc_Pos)                    /**< (ETM_LSR)  Mask */
#define ETM_LSR_ByteAcc(value)                (ETM_LSR_ByteAcc_Msk & ((value) << ETM_LSR_ByteAcc_Pos))
#define ETM_LSR_Msk                           _U_(0x00000007)                                      /**< (ETM_LSR) Register Mask  */


/* -------- ETM_AUTHSTATUS : (ETM Offset: 0xFB8) ( R/ 32) ETM Authentication Status Register -------- */
#define ETM_AUTHSTATUS_Msk                    _U_(0x00000000)                                      /**< (ETM_AUTHSTATUS) Register Mask  */


/* -------- ETM_DEVTYPE : (ETM Offset: 0xFCC) ( R/ 32) ETM CoreSight Device Type Register -------- */
#define ETM_DEVTYPE_RESETVALUE                _U_(0x13)                                            /**<  (ETM_DEVTYPE) ETM CoreSight Device Type Register  Reset Value */

#define ETM_DEVTYPE_Msk                       _U_(0x00000000)                                      /**< (ETM_DEVTYPE) Register Mask  */


/* -------- ETM_PIDR4 : (ETM Offset: 0xFD0) ( R/ 32) ETM Peripheral Identification Register #4 -------- */
#define ETM_PIDR4_RESETVALUE                  _U_(0x04)                                            /**<  (ETM_PIDR4) ETM Peripheral Identification Register #4  Reset Value */

#define ETM_PIDR4_Msk                         _U_(0x00000000)                                      /**< (ETM_PIDR4) Register Mask  */


/* -------- ETM_PIDR5 : (ETM Offset: 0xFD4) ( R/ 32) ETM Peripheral Identification Register #5 -------- */
#define ETM_PIDR5_RESETVALUE                  _U_(0x00)                                            /**<  (ETM_PIDR5) ETM Peripheral Identification Register #5  Reset Value */

#define ETM_PIDR5_Msk                         _U_(0x00000000)                                      /**< (ETM_PIDR5) Register Mask  */


/* -------- ETM_PIDR6 : (ETM Offset: 0xFD8) ( R/ 32) ETM Peripheral Identification Register #6 -------- */
#define ETM_PIDR6_RESETVALUE                  _U_(0x00)                                            /**<  (ETM_PIDR6) ETM Peripheral Identification Register #6  Reset Value */

#define ETM_PIDR6_Msk                         _U_(0x00000000)                                      /**< (ETM_PIDR6) Register Mask  */


/* -------- ETM_PIDR7 : (ETM Offset: 0xFDC) ( R/ 32) ETM Peripheral Identification Register #7 -------- */
#define ETM_PIDR7_RESETVALUE                  _U_(0x00)                                            /**<  (ETM_PIDR7) ETM Peripheral Identification Register #7  Reset Value */

#define ETM_PIDR7_Msk                         _U_(0x00000000)                                      /**< (ETM_PIDR7) Register Mask  */


/* -------- ETM_PIDR0 : (ETM Offset: 0xFE0) ( R/ 32) ETM Peripheral Identification Register #0 -------- */
#define ETM_PIDR0_RESETVALUE                  _U_(0x25)                                            /**<  (ETM_PIDR0) ETM Peripheral Identification Register #0  Reset Value */

#define ETM_PIDR0_Msk                         _U_(0x00000000)                                      /**< (ETM_PIDR0) Register Mask  */


/* -------- ETM_PIDR1 : (ETM Offset: 0xFE4) ( R/ 32) ETM Peripheral Identification Register #1 -------- */
#define ETM_PIDR1_RESETVALUE                  _U_(0xB9)                                            /**<  (ETM_PIDR1) ETM Peripheral Identification Register #1  Reset Value */

#define ETM_PIDR1_Msk                         _U_(0x00000000)                                      /**< (ETM_PIDR1) Register Mask  */


/* -------- ETM_PIDR2 : (ETM Offset: 0xFE8) ( R/ 32) ETM Peripheral Identification Register #2 -------- */
#define ETM_PIDR2_RESETVALUE                  _U_(0x0B)                                            /**<  (ETM_PIDR2) ETM Peripheral Identification Register #2  Reset Value */

#define ETM_PIDR2_Msk                         _U_(0x00000000)                                      /**< (ETM_PIDR2) Register Mask  */


/* -------- ETM_PIDR3 : (ETM Offset: 0xFEC) ( R/ 32) ETM Peripheral Identification Register #3 -------- */
#define ETM_PIDR3_RESETVALUE                  _U_(0x00)                                            /**<  (ETM_PIDR3) ETM Peripheral Identification Register #3  Reset Value */

#define ETM_PIDR3_Msk                         _U_(0x00000000)                                      /**< (ETM_PIDR3) Register Mask  */


/* -------- ETM_CIDR0 : (ETM Offset: 0xFF0) ( R/ 32) ETM Component  Identification Register #0 -------- */
#define ETM_CIDR0_RESETVALUE                  _U_(0x0D)                                            /**<  (ETM_CIDR0) ETM Component  Identification Register #0  Reset Value */

#define ETM_CIDR0_Msk                         _U_(0x00000000)                                      /**< (ETM_CIDR0) Register Mask  */


/* -------- ETM_CIDR1 : (ETM Offset: 0xFF4) ( R/ 32) ETM Component  Identification Register #1 -------- */
#define ETM_CIDR1_RESETVALUE                  _U_(0x90)                                            /**<  (ETM_CIDR1) ETM Component  Identification Register #1  Reset Value */

#define ETM_CIDR1_Msk                         _U_(0x00000000)                                      /**< (ETM_CIDR1) Register Mask  */


/* -------- ETM_CIDR2 : (ETM Offset: 0xFF8) ( R/ 32) ETM Component  Identification Register #2 -------- */
#define ETM_CIDR2_RESETVALUE                  _U_(0x05)                                            /**<  (ETM_CIDR2) ETM Component  Identification Register #2  Reset Value */

#define ETM_CIDR2_Msk                         _U_(0x00000000)                                      /**< (ETM_CIDR2) Register Mask  */


/* -------- ETM_CIDR3 : (ETM Offset: 0xFFC) ( R/ 32) ETM Component  Identification Register #3 -------- */
#define ETM_CIDR3_RESETVALUE                  _U_(0xB1)                                            /**<  (ETM_CIDR3) ETM Component  Identification Register #3  Reset Value */

#define ETM_CIDR3_Msk                         _U_(0x00000000)                                      /**< (ETM_CIDR3) Register Mask  */


/** \brief ETM register offsets definitions */
#define ETM_CR_REG_OFST                (0x00)              /**< (ETM_CR) ETM Main Control Register Offset */
#define ETM_CCR_REG_OFST               (0x04)              /**< (ETM_CCR) ETM Configuration Code Register Offset */
#define ETM_TRIGGER_REG_OFST           (0x08)              /**< (ETM_TRIGGER) ETM Trigger Event Register Offset */
#define ETM_SR_REG_OFST                (0x10)              /**< (ETM_SR) ETM Status Register Offset */
#define ETM_SCR_REG_OFST               (0x14)              /**< (ETM_SCR) ETM System Configuration Register Offset */
#define ETM_TEEVR_REG_OFST             (0x20)              /**< (ETM_TEEVR) ETM TraceEnable Event Register Offset */
#define ETM_TECR1_REG_OFST             (0x24)              /**< (ETM_TECR1) ETM TraceEnable Control 1 Register Offset */
#define ETM_FFLR_REG_OFST              (0x28)              /**< (ETM_FFLR) ETM FIFO Full Level Register Offset */
#define ETM_CNTRLDVR1_REG_OFST         (0x140)             /**< (ETM_CNTRLDVR1) ETM Free-running Counter Reload Value Offset */
#define ETM_SYNCFR_REG_OFST            (0x1E0)             /**< (ETM_SYNCFR) ETM Synchronization Frequency Register Offset */
#define ETM_IDR_REG_OFST               (0x1E4)             /**< (ETM_IDR) ETM ID Register Offset */
#define ETM_CCER_REG_OFST              (0x1E8)             /**< (ETM_CCER) ETM Configuration Code Extension Register Offset */
#define ETM_TESSEICR_REG_OFST          (0x1F0)             /**< (ETM_TESSEICR) ETM TraceEnable Start/Stop EmbeddedICE Control Register Offset */
#define ETM_TSEVT_REG_OFST             (0x1F8)             /**< (ETM_TSEVT) ETM TimeStamp Event Register Offset */
#define ETM_TRACEIDR_REG_OFST          (0x200)             /**< (ETM_TRACEIDR) ETM CoreSight Trace ID Register Offset */
#define ETM_IDR2_REG_OFST              (0x208)             /**< (ETM_IDR2) ETM ID Register 2 Offset */
#define ETM_PDSR_REG_OFST              (0x314)             /**< (ETM_PDSR) ETM Device Power-Down Status Register Offset */
#define ETM_ITMISCIN_REG_OFST          (0xEE0)             /**< (ETM_ITMISCIN) ETM Integration Test Miscellaneous Inputs Offset */
#define ETM_ITTRIGOUT_REG_OFST         (0xEE8)             /**< (ETM_ITTRIGOUT) ETM Integration Test Trigger Out Offset */
#define ETM_ITATBCTR2_REG_OFST         (0xEF0)             /**< (ETM_ITATBCTR2) ETM Integration Test ATB Control 2 Offset */
#define ETM_ITATBCTR0_REG_OFST         (0xEF8)             /**< (ETM_ITATBCTR0) ETM Integration Test ATB Control 0 Offset */
#define ETM_ITCTRL_REG_OFST            (0xF00)             /**< (ETM_ITCTRL) ETM Integration Mode Control Register Offset */
#define ETM_CLAIMSET_REG_OFST          (0xFA0)             /**< (ETM_CLAIMSET) ETM Claim Tag Set Register Offset */
#define ETM_CLAIMCLR_REG_OFST          (0xFA4)             /**< (ETM_CLAIMCLR) ETM Claim Tag Clear Register Offset */
#define ETM_LAR_REG_OFST               (0xFB0)             /**< (ETM_LAR) ETM Lock Access Register Offset */
#define ETM_LSR_REG_OFST               (0xFB4)             /**< (ETM_LSR) ETM Lock Status Register Offset */
#define ETM_AUTHSTATUS_REG_OFST        (0xFB8)             /**< (ETM_AUTHSTATUS) ETM Authentication Status Register Offset */
#define ETM_DEVTYPE_REG_OFST           (0xFCC)             /**< (ETM_DEVTYPE) ETM CoreSight Device Type Register Offset */
#define ETM_PIDR4_REG_OFST             (0xFD0)             /**< (ETM_PIDR4) ETM Peripheral Identification Register #4 Offset */
#define ETM_PIDR5_REG_OFST             (0xFD4)             /**< (ETM_PIDR5) ETM Peripheral Identification Register #5 Offset */
#define ETM_PIDR6_REG_OFST             (0xFD8)             /**< (ETM_PIDR6) ETM Peripheral Identification Register #6 Offset */
#define ETM_PIDR7_REG_OFST             (0xFDC)             /**< (ETM_PIDR7) ETM Peripheral Identification Register #7 Offset */
#define ETM_PIDR0_REG_OFST             (0xFE0)             /**< (ETM_PIDR0) ETM Peripheral Identification Register #0 Offset */
#define ETM_PIDR1_REG_OFST             (0xFE4)             /**< (ETM_PIDR1) ETM Peripheral Identification Register #1 Offset */
#define ETM_PIDR2_REG_OFST             (0xFE8)             /**< (ETM_PIDR2) ETM Peripheral Identification Register #2 Offset */
#define ETM_PIDR3_REG_OFST             (0xFEC)             /**< (ETM_PIDR3) ETM Peripheral Identification Register #3 Offset */
#define ETM_CIDR0_REG_OFST             (0xFF0)             /**< (ETM_CIDR0) ETM Component  Identification Register #0 Offset */
#define ETM_CIDR1_REG_OFST             (0xFF4)             /**< (ETM_CIDR1) ETM Component  Identification Register #1 Offset */
#define ETM_CIDR2_REG_OFST             (0xFF8)             /**< (ETM_CIDR2) ETM Component  Identification Register #2 Offset */
#define ETM_CIDR3_REG_OFST             (0xFFC)             /**< (ETM_CIDR3) ETM Component  Identification Register #3 Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief ETM register API structure */
typedef struct
{  /* Embedded Trace Macrocell */
  __IO  uint32_t                       ETM_CR;             /**< Offset: 0x00 (R/W  32) ETM Main Control Register */
  __I   uint32_t                       ETM_CCR;            /**< Offset: 0x04 (R/   32) ETM Configuration Code Register */
  __IO  uint32_t                       ETM_TRIGGER;        /**< Offset: 0x08 (R/W  32) ETM Trigger Event Register */
  __I   uint8_t                        Reserved1[0x04];
  __IO  uint32_t                       ETM_SR;             /**< Offset: 0x10 (R/W  32) ETM Status Register */
  __I   uint32_t                       ETM_SCR;            /**< Offset: 0x14 (R/   32) ETM System Configuration Register */
  __I   uint8_t                        Reserved2[0x08];
  __IO  uint32_t                       ETM_TEEVR;          /**< Offset: 0x20 (R/W  32) ETM TraceEnable Event Register */
  __IO  uint32_t                       ETM_TECR1;          /**< Offset: 0x24 (R/W  32) ETM TraceEnable Control 1 Register */
  __IO  uint32_t                       ETM_FFLR;           /**< Offset: 0x28 (R/W  32) ETM FIFO Full Level Register */
  __I   uint8_t                        Reserved3[0x114];
  __IO  uint32_t                       ETM_CNTRLDVR1;      /**< Offset: 0x140 (R/W  32) ETM Free-running Counter Reload Value */
  __I   uint8_t                        Reserved4[0x9C];
  __I   uint32_t                       ETM_SYNCFR;         /**< Offset: 0x1E0 (R/   32) ETM Synchronization Frequency Register */
  __I   uint32_t                       ETM_IDR;            /**< Offset: 0x1E4 (R/   32) ETM ID Register */
  __I   uint32_t                       ETM_CCER;           /**< Offset: 0x1E8 (R/   32) ETM Configuration Code Extension Register */
  __I   uint8_t                        Reserved5[0x04];
  __IO  uint32_t                       ETM_TESSEICR;       /**< Offset: 0x1F0 (R/W  32) ETM TraceEnable Start/Stop EmbeddedICE Control Register */
  __I   uint8_t                        Reserved6[0x04];
  __IO  uint32_t                       ETM_TSEVT;          /**< Offset: 0x1F8 (R/W  32) ETM TimeStamp Event Register */
  __I   uint8_t                        Reserved7[0x04];
  __IO  uint32_t                       ETM_TRACEIDR;       /**< Offset: 0x200 (R/W  32) ETM CoreSight Trace ID Register */
  __I   uint8_t                        Reserved8[0x04];
  __I   uint32_t                       ETM_IDR2;           /**< Offset: 0x208 (R/   32) ETM ID Register 2 */
  __I   uint8_t                        Reserved9[0x108];
  __I   uint32_t                       ETM_PDSR;           /**< Offset: 0x314 (R/   32) ETM Device Power-Down Status Register */
  __I   uint8_t                        Reserved10[0xBC8];
  __I   uint32_t                       ETM_ITMISCIN;       /**< Offset: 0xEE0 (R/   32) ETM Integration Test Miscellaneous Inputs */
  __I   uint8_t                        Reserved11[0x04];
  __O   uint32_t                       ETM_ITTRIGOUT;      /**< Offset: 0xEE8 ( /W  32) ETM Integration Test Trigger Out */
  __I   uint8_t                        Reserved12[0x04];
  __I   uint32_t                       ETM_ITATBCTR2;      /**< Offset: 0xEF0 (R/   32) ETM Integration Test ATB Control 2 */
  __I   uint8_t                        Reserved13[0x04];
  __O   uint32_t                       ETM_ITATBCTR0;      /**< Offset: 0xEF8 ( /W  32) ETM Integration Test ATB Control 0 */
  __I   uint8_t                        Reserved14[0x04];
  __IO  uint32_t                       ETM_ITCTRL;         /**< Offset: 0xF00 (R/W  32) ETM Integration Mode Control Register */
  __I   uint8_t                        Reserved15[0x9C];
  __IO  uint32_t                       ETM_CLAIMSET;       /**< Offset: 0xFA0 (R/W  32) ETM Claim Tag Set Register */
  __IO  uint32_t                       ETM_CLAIMCLR;       /**< Offset: 0xFA4 (R/W  32) ETM Claim Tag Clear Register */
  __I   uint8_t                        Reserved16[0x08];
  __O   uint32_t                       ETM_LAR;            /**< Offset: 0xFB0 ( /W  32) ETM Lock Access Register */
  __I   uint32_t                       ETM_LSR;            /**< Offset: 0xFB4 (R/   32) ETM Lock Status Register */
  __I   uint32_t                       ETM_AUTHSTATUS;     /**< Offset: 0xFB8 (R/   32) ETM Authentication Status Register */
  __I   uint8_t                        Reserved17[0x10];
  __I   uint32_t                       ETM_DEVTYPE;        /**< Offset: 0xFCC (R/   32) ETM CoreSight Device Type Register */
  __I   uint32_t                       ETM_PIDR4;          /**< Offset: 0xFD0 (R/   32) ETM Peripheral Identification Register #4 */
  __I   uint32_t                       ETM_PIDR5;          /**< Offset: 0xFD4 (R/   32) ETM Peripheral Identification Register #5 */
  __I   uint32_t                       ETM_PIDR6;          /**< Offset: 0xFD8 (R/   32) ETM Peripheral Identification Register #6 */
  __I   uint32_t                       ETM_PIDR7;          /**< Offset: 0xFDC (R/   32) ETM Peripheral Identification Register #7 */
  __I   uint32_t                       ETM_PIDR0;          /**< Offset: 0xFE0 (R/   32) ETM Peripheral Identification Register #0 */
  __I   uint32_t                       ETM_PIDR1;          /**< Offset: 0xFE4 (R/   32) ETM Peripheral Identification Register #1 */
  __I   uint32_t                       ETM_PIDR2;          /**< Offset: 0xFE8 (R/   32) ETM Peripheral Identification Register #2 */
  __I   uint32_t                       ETM_PIDR3;          /**< Offset: 0xFEC (R/   32) ETM Peripheral Identification Register #3 */
  __I   uint32_t                       ETM_CIDR0;          /**< Offset: 0xFF0 (R/   32) ETM Component  Identification Register #0 */
  __I   uint32_t                       ETM_CIDR1;          /**< Offset: 0xFF4 (R/   32) ETM Component  Identification Register #1 */
  __I   uint32_t                       ETM_CIDR2;          /**< Offset: 0xFF8 (R/   32) ETM Component  Identification Register #2 */
  __I   uint32_t                       ETM_CIDR3;          /**< Offset: 0xFFC (R/   32) ETM Component  Identification Register #3 */
} etm_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_ETM_COMPONENT_H_ */
