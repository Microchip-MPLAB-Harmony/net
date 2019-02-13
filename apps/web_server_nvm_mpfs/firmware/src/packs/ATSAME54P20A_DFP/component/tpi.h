/**
 * \brief Component description for TPI
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
#ifndef _SAME54_TPI_COMPONENT_H_
#define _SAME54_TPI_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR TPI                                          */
/* ************************************************************************** */

/* -------- TPI_SSPSR : (TPI Offset: 0x00) ( R/ 32) Supported Parallel Port Size Register -------- */
#define TPI_SSPSR_Msk                         _U_(0x00000000)                                      /**< (TPI_SSPSR) Register Mask  */


/* -------- TPI_CSPSR : (TPI Offset: 0x04) (R/W 32) Current Parallel Port Size Register -------- */
#define TPI_CSPSR_Msk                         _U_(0x00000000)                                      /**< (TPI_CSPSR) Register Mask  */


/* -------- TPI_ACPR : (TPI Offset: 0x10) (R/W 32) Asynchronous Clock Prescaler Register -------- */
#define TPI_ACPR_PRESCALER_Pos                _U_(0)                                               /**< (TPI_ACPR)  Position */
#define TPI_ACPR_PRESCALER_Msk                (_U_(0x1FFF) << TPI_ACPR_PRESCALER_Pos)              /**< (TPI_ACPR)  Mask */
#define TPI_ACPR_PRESCALER(value)             (TPI_ACPR_PRESCALER_Msk & ((value) << TPI_ACPR_PRESCALER_Pos))
#define TPI_ACPR_Msk                          _U_(0x00001FFF)                                      /**< (TPI_ACPR) Register Mask  */


/* -------- TPI_SPPR : (TPI Offset: 0xF0) (R/W 32) Selected Pin Protocol Register -------- */
#define TPI_SPPR_TXMODE_Pos                   _U_(0)                                               /**< (TPI_SPPR)  Position */
#define TPI_SPPR_TXMODE_Msk                   (_U_(0x3) << TPI_SPPR_TXMODE_Pos)                    /**< (TPI_SPPR)  Mask */
#define TPI_SPPR_TXMODE(value)                (TPI_SPPR_TXMODE_Msk & ((value) << TPI_SPPR_TXMODE_Pos))
#define TPI_SPPR_Msk                          _U_(0x00000003)                                      /**< (TPI_SPPR) Register Mask  */


/* -------- TPI_FFSR : (TPI Offset: 0x300) ( R/ 32) Formatter and Flush Status Register -------- */
#define TPI_FFSR_FlInProg_Pos                 _U_(0)                                               /**< (TPI_FFSR)  Position */
#define TPI_FFSR_FlInProg_Msk                 (_U_(0x1) << TPI_FFSR_FlInProg_Pos)                  /**< (TPI_FFSR)  Mask */
#define TPI_FFSR_FlInProg(value)              (TPI_FFSR_FlInProg_Msk & ((value) << TPI_FFSR_FlInProg_Pos))
#define TPI_FFSR_FtStopped_Pos                _U_(1)                                               /**< (TPI_FFSR)  Position */
#define TPI_FFSR_FtStopped_Msk                (_U_(0x1) << TPI_FFSR_FtStopped_Pos)                 /**< (TPI_FFSR)  Mask */
#define TPI_FFSR_FtStopped(value)             (TPI_FFSR_FtStopped_Msk & ((value) << TPI_FFSR_FtStopped_Pos))
#define TPI_FFSR_TCPresent_Pos                _U_(2)                                               /**< (TPI_FFSR)  Position */
#define TPI_FFSR_TCPresent_Msk                (_U_(0x1) << TPI_FFSR_TCPresent_Pos)                 /**< (TPI_FFSR)  Mask */
#define TPI_FFSR_TCPresent(value)             (TPI_FFSR_TCPresent_Msk & ((value) << TPI_FFSR_TCPresent_Pos))
#define TPI_FFSR_FtNonStop_Pos                _U_(3)                                               /**< (TPI_FFSR)  Position */
#define TPI_FFSR_FtNonStop_Msk                (_U_(0x1) << TPI_FFSR_FtNonStop_Pos)                 /**< (TPI_FFSR)  Mask */
#define TPI_FFSR_FtNonStop(value)             (TPI_FFSR_FtNonStop_Msk & ((value) << TPI_FFSR_FtNonStop_Pos))
#define TPI_FFSR_Msk                          _U_(0x0000000F)                                      /**< (TPI_FFSR) Register Mask  */


/* -------- TPI_FFCR : (TPI Offset: 0x304) (R/W 32) Formatter and Flush Control Register -------- */
#define TPI_FFCR_EnFCont_Pos                  _U_(1)                                               /**< (TPI_FFCR)  Position */
#define TPI_FFCR_EnFCont_Msk                  (_U_(0x1) << TPI_FFCR_EnFCont_Pos)                   /**< (TPI_FFCR)  Mask */
#define TPI_FFCR_EnFCont(value)               (TPI_FFCR_EnFCont_Msk & ((value) << TPI_FFCR_EnFCont_Pos))
#define TPI_FFCR_TrigIn_Pos                   _U_(8)                                               /**< (TPI_FFCR)  Position */
#define TPI_FFCR_TrigIn_Msk                   (_U_(0x1) << TPI_FFCR_TrigIn_Pos)                    /**< (TPI_FFCR)  Mask */
#define TPI_FFCR_TrigIn(value)                (TPI_FFCR_TrigIn_Msk & ((value) << TPI_FFCR_TrigIn_Pos))
#define TPI_FFCR_Msk                          _U_(0x00000102)                                      /**< (TPI_FFCR) Register Mask  */


/* -------- TPI_FSCR : (TPI Offset: 0x308) ( R/ 32) Formatter Synchronization Counter Register -------- */
#define TPI_FSCR_Msk                          _U_(0x00000000)                                      /**< (TPI_FSCR) Register Mask  */


/* -------- TPI_TRIGGER : (TPI Offset: 0xEE8) ( R/ 32) TRIGGER -------- */
#define TPI_TRIGGER_TRIGGER_Pos               _U_(0)                                               /**< (TPI_TRIGGER)  Position */
#define TPI_TRIGGER_TRIGGER_Msk               (_U_(0x1) << TPI_TRIGGER_TRIGGER_Pos)                /**< (TPI_TRIGGER)  Mask */
#define TPI_TRIGGER_TRIGGER(value)            (TPI_TRIGGER_TRIGGER_Msk & ((value) << TPI_TRIGGER_TRIGGER_Pos))
#define TPI_TRIGGER_Msk                       _U_(0x00000001)                                      /**< (TPI_TRIGGER) Register Mask  */


/* -------- TPI_FIFO0 : (TPI Offset: 0xEEC) ( R/ 32) Integration ETM Data -------- */
#define TPI_FIFO0_ETM0_Pos                    _U_(0)                                               /**< (TPI_FIFO0)  Position */
#define TPI_FIFO0_ETM0_Msk                    (_U_(0xFF) << TPI_FIFO0_ETM0_Pos)                    /**< (TPI_FIFO0)  Mask */
#define TPI_FIFO0_ETM0(value)                 (TPI_FIFO0_ETM0_Msk & ((value) << TPI_FIFO0_ETM0_Pos))
#define TPI_FIFO0_ETM1_Pos                    _U_(8)                                               /**< (TPI_FIFO0)  Position */
#define TPI_FIFO0_ETM1_Msk                    (_U_(0xFF) << TPI_FIFO0_ETM1_Pos)                    /**< (TPI_FIFO0)  Mask */
#define TPI_FIFO0_ETM1(value)                 (TPI_FIFO0_ETM1_Msk & ((value) << TPI_FIFO0_ETM1_Pos))
#define TPI_FIFO0_ETM2_Pos                    _U_(16)                                              /**< (TPI_FIFO0)  Position */
#define TPI_FIFO0_ETM2_Msk                    (_U_(0xFF) << TPI_FIFO0_ETM2_Pos)                    /**< (TPI_FIFO0)  Mask */
#define TPI_FIFO0_ETM2(value)                 (TPI_FIFO0_ETM2_Msk & ((value) << TPI_FIFO0_ETM2_Pos))
#define TPI_FIFO0_ETM_bytecount_Pos           _U_(24)                                              /**< (TPI_FIFO0)  Position */
#define TPI_FIFO0_ETM_bytecount_Msk           (_U_(0x3) << TPI_FIFO0_ETM_bytecount_Pos)            /**< (TPI_FIFO0)  Mask */
#define TPI_FIFO0_ETM_bytecount(value)        (TPI_FIFO0_ETM_bytecount_Msk & ((value) << TPI_FIFO0_ETM_bytecount_Pos))
#define TPI_FIFO0_ETM_ATVALID_Pos             _U_(26)                                              /**< (TPI_FIFO0)  Position */
#define TPI_FIFO0_ETM_ATVALID_Msk             (_U_(0x3) << TPI_FIFO0_ETM_ATVALID_Pos)              /**< (TPI_FIFO0)  Mask */
#define TPI_FIFO0_ETM_ATVALID(value)          (TPI_FIFO0_ETM_ATVALID_Msk & ((value) << TPI_FIFO0_ETM_ATVALID_Pos))
#define TPI_FIFO0_ITM_bytecount_Pos           _U_(27)                                              /**< (TPI_FIFO0)  Position */
#define TPI_FIFO0_ITM_bytecount_Msk           (_U_(0x3) << TPI_FIFO0_ITM_bytecount_Pos)            /**< (TPI_FIFO0)  Mask */
#define TPI_FIFO0_ITM_bytecount(value)        (TPI_FIFO0_ITM_bytecount_Msk & ((value) << TPI_FIFO0_ITM_bytecount_Pos))
#define TPI_FIFO0_ITM_ATVALID_Pos             _U_(29)                                              /**< (TPI_FIFO0)  Position */
#define TPI_FIFO0_ITM_ATVALID_Msk             (_U_(0x3) << TPI_FIFO0_ITM_ATVALID_Pos)              /**< (TPI_FIFO0)  Mask */
#define TPI_FIFO0_ITM_ATVALID(value)          (TPI_FIFO0_ITM_ATVALID_Msk & ((value) << TPI_FIFO0_ITM_ATVALID_Pos))
#define TPI_FIFO0_Msk                         _U_(0x7FFFFFFF)                                      /**< (TPI_FIFO0) Register Mask  */


/* -------- TPI_ITATBCTR2 : (TPI Offset: 0xEF0) ( R/ 32) ITATBCTR2 -------- */
#define TPI_ITATBCTR2_ATREADY_Pos             _U_(0)                                               /**< (TPI_ITATBCTR2)  Position */
#define TPI_ITATBCTR2_ATREADY_Msk             (_U_(0x1) << TPI_ITATBCTR2_ATREADY_Pos)              /**< (TPI_ITATBCTR2)  Mask */
#define TPI_ITATBCTR2_ATREADY(value)          (TPI_ITATBCTR2_ATREADY_Msk & ((value) << TPI_ITATBCTR2_ATREADY_Pos))
#define TPI_ITATBCTR2_Msk                     _U_(0x00000001)                                      /**< (TPI_ITATBCTR2) Register Mask  */


/* -------- TPI_ITATBCTR0 : (TPI Offset: 0xEF8) ( R/ 32) ITATBCTR0 -------- */
#define TPI_ITATBCTR0_ATREADY_Pos             _U_(0)                                               /**< (TPI_ITATBCTR0)  Position */
#define TPI_ITATBCTR0_ATREADY_Msk             (_U_(0x1) << TPI_ITATBCTR0_ATREADY_Pos)              /**< (TPI_ITATBCTR0)  Mask */
#define TPI_ITATBCTR0_ATREADY(value)          (TPI_ITATBCTR0_ATREADY_Msk & ((value) << TPI_ITATBCTR0_ATREADY_Pos))
#define TPI_ITATBCTR0_Msk                     _U_(0x00000001)                                      /**< (TPI_ITATBCTR0) Register Mask  */


/* -------- TPI_FIFO1 : (TPI Offset: 0xEFC) ( R/ 32) Integration ITM Data -------- */
#define TPI_FIFO1_ITM0_Pos                    _U_(0)                                               /**< (TPI_FIFO1)  Position */
#define TPI_FIFO1_ITM0_Msk                    (_U_(0xFF) << TPI_FIFO1_ITM0_Pos)                    /**< (TPI_FIFO1)  Mask */
#define TPI_FIFO1_ITM0(value)                 (TPI_FIFO1_ITM0_Msk & ((value) << TPI_FIFO1_ITM0_Pos))
#define TPI_FIFO1_ITM1_Pos                    _U_(8)                                               /**< (TPI_FIFO1)  Position */
#define TPI_FIFO1_ITM1_Msk                    (_U_(0xFF) << TPI_FIFO1_ITM1_Pos)                    /**< (TPI_FIFO1)  Mask */
#define TPI_FIFO1_ITM1(value)                 (TPI_FIFO1_ITM1_Msk & ((value) << TPI_FIFO1_ITM1_Pos))
#define TPI_FIFO1_ITM2_Pos                    _U_(16)                                              /**< (TPI_FIFO1)  Position */
#define TPI_FIFO1_ITM2_Msk                    (_U_(0xFF) << TPI_FIFO1_ITM2_Pos)                    /**< (TPI_FIFO1)  Mask */
#define TPI_FIFO1_ITM2(value)                 (TPI_FIFO1_ITM2_Msk & ((value) << TPI_FIFO1_ITM2_Pos))
#define TPI_FIFO1_ETM_bytecount_Pos           _U_(24)                                              /**< (TPI_FIFO1)  Position */
#define TPI_FIFO1_ETM_bytecount_Msk           (_U_(0x3) << TPI_FIFO1_ETM_bytecount_Pos)            /**< (TPI_FIFO1)  Mask */
#define TPI_FIFO1_ETM_bytecount(value)        (TPI_FIFO1_ETM_bytecount_Msk & ((value) << TPI_FIFO1_ETM_bytecount_Pos))
#define TPI_FIFO1_ETM_ATVALID_Pos             _U_(26)                                              /**< (TPI_FIFO1)  Position */
#define TPI_FIFO1_ETM_ATVALID_Msk             (_U_(0x3) << TPI_FIFO1_ETM_ATVALID_Pos)              /**< (TPI_FIFO1)  Mask */
#define TPI_FIFO1_ETM_ATVALID(value)          (TPI_FIFO1_ETM_ATVALID_Msk & ((value) << TPI_FIFO1_ETM_ATVALID_Pos))
#define TPI_FIFO1_ITM_bytecount_Pos           _U_(27)                                              /**< (TPI_FIFO1)  Position */
#define TPI_FIFO1_ITM_bytecount_Msk           (_U_(0x3) << TPI_FIFO1_ITM_bytecount_Pos)            /**< (TPI_FIFO1)  Mask */
#define TPI_FIFO1_ITM_bytecount(value)        (TPI_FIFO1_ITM_bytecount_Msk & ((value) << TPI_FIFO1_ITM_bytecount_Pos))
#define TPI_FIFO1_ITM_ATVALID_Pos             _U_(29)                                              /**< (TPI_FIFO1)  Position */
#define TPI_FIFO1_ITM_ATVALID_Msk             (_U_(0x3) << TPI_FIFO1_ITM_ATVALID_Pos)              /**< (TPI_FIFO1)  Mask */
#define TPI_FIFO1_ITM_ATVALID(value)          (TPI_FIFO1_ITM_ATVALID_Msk & ((value) << TPI_FIFO1_ITM_ATVALID_Pos))
#define TPI_FIFO1_Msk                         _U_(0x7FFFFFFF)                                      /**< (TPI_FIFO1) Register Mask  */


/* -------- TPI_ITCTRL : (TPI Offset: 0xF00) (R/W 32) Integration Mode Control -------- */
#define TPI_ITCTRL_Mode_Pos                   _U_(0)                                               /**< (TPI_ITCTRL)  Position */
#define TPI_ITCTRL_Mode_Msk                   (_U_(0x1) << TPI_ITCTRL_Mode_Pos)                    /**< (TPI_ITCTRL)  Mask */
#define TPI_ITCTRL_Mode(value)                (TPI_ITCTRL_Mode_Msk & ((value) << TPI_ITCTRL_Mode_Pos))
#define TPI_ITCTRL_Msk                        _U_(0x00000001)                                      /**< (TPI_ITCTRL) Register Mask  */


/* -------- TPI_CLAIMSET : (TPI Offset: 0xFA0) (R/W 32) Claim tag set -------- */
#define TPI_CLAIMSET_Msk                      _U_(0x00000000)                                      /**< (TPI_CLAIMSET) Register Mask  */


/* -------- TPI_CLAIMCLR : (TPI Offset: 0xFA4) (R/W 32) Claim tag clear -------- */
#define TPI_CLAIMCLR_Msk                      _U_(0x00000000)                                      /**< (TPI_CLAIMCLR) Register Mask  */


/* -------- TPI_DEVID : (TPI Offset: 0xFC8) ( R/ 32) TPIU_DEVID -------- */
#define TPI_DEVID_NrTraceInput_Pos            _U_(0)                                               /**< (TPI_DEVID)  Position */
#define TPI_DEVID_NrTraceInput_Msk            (_U_(0x1) << TPI_DEVID_NrTraceInput_Pos)             /**< (TPI_DEVID)  Mask */
#define TPI_DEVID_NrTraceInput(value)         (TPI_DEVID_NrTraceInput_Msk & ((value) << TPI_DEVID_NrTraceInput_Pos))
#define TPI_DEVID_AsynClkIn_Pos               _U_(5)                                               /**< (TPI_DEVID)  Position */
#define TPI_DEVID_AsynClkIn_Msk               (_U_(0x1) << TPI_DEVID_AsynClkIn_Pos)                /**< (TPI_DEVID)  Mask */
#define TPI_DEVID_AsynClkIn(value)            (TPI_DEVID_AsynClkIn_Msk & ((value) << TPI_DEVID_AsynClkIn_Pos))
#define TPI_DEVID_MinBufSz_Pos                _U_(6)                                               /**< (TPI_DEVID)  Position */
#define TPI_DEVID_MinBufSz_Msk                (_U_(0x7) << TPI_DEVID_MinBufSz_Pos)                 /**< (TPI_DEVID)  Mask */
#define TPI_DEVID_MinBufSz(value)             (TPI_DEVID_MinBufSz_Msk & ((value) << TPI_DEVID_MinBufSz_Pos))
#define TPI_DEVID_PTINVALID_Pos               _U_(9)                                               /**< (TPI_DEVID)  Position */
#define TPI_DEVID_PTINVALID_Msk               (_U_(0x1) << TPI_DEVID_PTINVALID_Pos)                /**< (TPI_DEVID)  Mask */
#define TPI_DEVID_PTINVALID(value)            (TPI_DEVID_PTINVALID_Msk & ((value) << TPI_DEVID_PTINVALID_Pos))
#define TPI_DEVID_MANCVALID_Pos               _U_(10)                                              /**< (TPI_DEVID)  Position */
#define TPI_DEVID_MANCVALID_Msk               (_U_(0x1) << TPI_DEVID_MANCVALID_Pos)                /**< (TPI_DEVID)  Mask */
#define TPI_DEVID_MANCVALID(value)            (TPI_DEVID_MANCVALID_Msk & ((value) << TPI_DEVID_MANCVALID_Pos))
#define TPI_DEVID_NRZVALID_Pos                _U_(11)                                              /**< (TPI_DEVID)  Position */
#define TPI_DEVID_NRZVALID_Msk                (_U_(0x1) << TPI_DEVID_NRZVALID_Pos)                 /**< (TPI_DEVID)  Mask */
#define TPI_DEVID_NRZVALID(value)             (TPI_DEVID_NRZVALID_Msk & ((value) << TPI_DEVID_NRZVALID_Pos))
#define TPI_DEVID_Msk                         _U_(0x00000FE1)                                      /**< (TPI_DEVID) Register Mask  */


/* -------- TPI_DEVTYPE : (TPI Offset: 0xFCC) ( R/ 32) TPIU_DEVTYPE -------- */
#define TPI_DEVTYPE_SubType_Pos               _U_(0)                                               /**< (TPI_DEVTYPE)  Position */
#define TPI_DEVTYPE_SubType_Msk               (_U_(0xF) << TPI_DEVTYPE_SubType_Pos)                /**< (TPI_DEVTYPE)  Mask */
#define TPI_DEVTYPE_SubType(value)            (TPI_DEVTYPE_SubType_Msk & ((value) << TPI_DEVTYPE_SubType_Pos))
#define TPI_DEVTYPE_MajorType_Pos             _U_(4)                                               /**< (TPI_DEVTYPE)  Position */
#define TPI_DEVTYPE_MajorType_Msk             (_U_(0xF) << TPI_DEVTYPE_MajorType_Pos)              /**< (TPI_DEVTYPE)  Mask */
#define TPI_DEVTYPE_MajorType(value)          (TPI_DEVTYPE_MajorType_Msk & ((value) << TPI_DEVTYPE_MajorType_Pos))
#define TPI_DEVTYPE_Msk                       _U_(0x000000FF)                                      /**< (TPI_DEVTYPE) Register Mask  */


/** \brief TPI register offsets definitions */
#define TPI_SSPSR_REG_OFST             (0x00)              /**< (TPI_SSPSR) Supported Parallel Port Size Register Offset */
#define TPI_CSPSR_REG_OFST             (0x04)              /**< (TPI_CSPSR) Current Parallel Port Size Register Offset */
#define TPI_ACPR_REG_OFST              (0x10)              /**< (TPI_ACPR) Asynchronous Clock Prescaler Register Offset */
#define TPI_SPPR_REG_OFST              (0xF0)              /**< (TPI_SPPR) Selected Pin Protocol Register Offset */
#define TPI_FFSR_REG_OFST              (0x300)             /**< (TPI_FFSR) Formatter and Flush Status Register Offset */
#define TPI_FFCR_REG_OFST              (0x304)             /**< (TPI_FFCR) Formatter and Flush Control Register Offset */
#define TPI_FSCR_REG_OFST              (0x308)             /**< (TPI_FSCR) Formatter Synchronization Counter Register Offset */
#define TPI_TRIGGER_REG_OFST           (0xEE8)             /**< (TPI_TRIGGER) TRIGGER Offset */
#define TPI_FIFO0_REG_OFST             (0xEEC)             /**< (TPI_FIFO0) Integration ETM Data Offset */
#define TPI_ITATBCTR2_REG_OFST         (0xEF0)             /**< (TPI_ITATBCTR2) ITATBCTR2 Offset */
#define TPI_ITATBCTR0_REG_OFST         (0xEF8)             /**< (TPI_ITATBCTR0) ITATBCTR0 Offset */
#define TPI_FIFO1_REG_OFST             (0xEFC)             /**< (TPI_FIFO1) Integration ITM Data Offset */
#define TPI_ITCTRL_REG_OFST            (0xF00)             /**< (TPI_ITCTRL) Integration Mode Control Offset */
#define TPI_CLAIMSET_REG_OFST          (0xFA0)             /**< (TPI_CLAIMSET) Claim tag set Offset */
#define TPI_CLAIMCLR_REG_OFST          (0xFA4)             /**< (TPI_CLAIMCLR) Claim tag clear Offset */
#define TPI_DEVID_REG_OFST             (0xFC8)             /**< (TPI_DEVID) TPIU_DEVID Offset */
#define TPI_DEVTYPE_REG_OFST           (0xFCC)             /**< (TPI_DEVTYPE) TPIU_DEVTYPE Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief TPI register API structure */
typedef struct
{  /* Trace Port Interface Register */
  __I   uint32_t                       TPI_SSPSR;          /**< Offset: 0x00 (R/   32) Supported Parallel Port Size Register */
  __IO  uint32_t                       TPI_CSPSR;          /**< Offset: 0x04 (R/W  32) Current Parallel Port Size Register */
  __I   uint8_t                        Reserved1[0x08];
  __IO  uint32_t                       TPI_ACPR;           /**< Offset: 0x10 (R/W  32) Asynchronous Clock Prescaler Register */
  __I   uint8_t                        Reserved2[0xDC];
  __IO  uint32_t                       TPI_SPPR;           /**< Offset: 0xF0 (R/W  32) Selected Pin Protocol Register */
  __I   uint8_t                        Reserved3[0x20C];
  __I   uint32_t                       TPI_FFSR;           /**< Offset: 0x300 (R/   32) Formatter and Flush Status Register */
  __IO  uint32_t                       TPI_FFCR;           /**< Offset: 0x304 (R/W  32) Formatter and Flush Control Register */
  __I   uint32_t                       TPI_FSCR;           /**< Offset: 0x308 (R/   32) Formatter Synchronization Counter Register */
  __I   uint8_t                        Reserved4[0xBDC];
  __I   uint32_t                       TPI_TRIGGER;        /**< Offset: 0xEE8 (R/   32) TRIGGER */
  __I   uint32_t                       TPI_FIFO0;          /**< Offset: 0xEEC (R/   32) Integration ETM Data */
  __I   uint32_t                       TPI_ITATBCTR2;      /**< Offset: 0xEF0 (R/   32) ITATBCTR2 */
  __I   uint8_t                        Reserved5[0x04];
  __I   uint32_t                       TPI_ITATBCTR0;      /**< Offset: 0xEF8 (R/   32) ITATBCTR0 */
  __I   uint32_t                       TPI_FIFO1;          /**< Offset: 0xEFC (R/   32) Integration ITM Data */
  __IO  uint32_t                       TPI_ITCTRL;         /**< Offset: 0xF00 (R/W  32) Integration Mode Control */
  __I   uint8_t                        Reserved6[0x9C];
  __IO  uint32_t                       TPI_CLAIMSET;       /**< Offset: 0xFA0 (R/W  32) Claim tag set */
  __IO  uint32_t                       TPI_CLAIMCLR;       /**< Offset: 0xFA4 (R/W  32) Claim tag clear */
  __I   uint8_t                        Reserved7[0x20];
  __I   uint32_t                       TPI_DEVID;          /**< Offset: 0xFC8 (R/   32) TPIU_DEVID */
  __I   uint32_t                       TPI_DEVTYPE;        /**< Offset: 0xFCC (R/   32) TPIU_DEVTYPE */
} tpi_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_TPI_COMPONENT_H_ */
