/**
 * \brief Component description for DWT
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
#ifndef _SAME54_DWT_COMPONENT_H_
#define _SAME54_DWT_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR DWT                                          */
/* ************************************************************************** */

/* -------- DWT_CTRL : (DWT Offset: 0x00) (R/W 32) Control Register -------- */
#define DWT_CTRL_CYCCNTENA_Pos                _U_(0)                                               /**< (DWT_CTRL)  Position */
#define DWT_CTRL_CYCCNTENA_Msk                (_U_(0x1) << DWT_CTRL_CYCCNTENA_Pos)                 /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_CYCCNTENA(value)             (DWT_CTRL_CYCCNTENA_Msk & ((value) << DWT_CTRL_CYCCNTENA_Pos))
#define DWT_CTRL_POSTPRESET_Pos               _U_(1)                                               /**< (DWT_CTRL)  Position */
#define DWT_CTRL_POSTPRESET_Msk               (_U_(0xF) << DWT_CTRL_POSTPRESET_Pos)                /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_POSTPRESET(value)            (DWT_CTRL_POSTPRESET_Msk & ((value) << DWT_CTRL_POSTPRESET_Pos))
#define DWT_CTRL_POSTINIT_Pos                 _U_(5)                                               /**< (DWT_CTRL)  Position */
#define DWT_CTRL_POSTINIT_Msk                 (_U_(0xF) << DWT_CTRL_POSTINIT_Pos)                  /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_POSTINIT(value)              (DWT_CTRL_POSTINIT_Msk & ((value) << DWT_CTRL_POSTINIT_Pos))
#define DWT_CTRL_CYCTAP_Pos                   _U_(9)                                               /**< (DWT_CTRL)  Position */
#define DWT_CTRL_CYCTAP_Msk                   (_U_(0x1) << DWT_CTRL_CYCTAP_Pos)                    /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_CYCTAP(value)                (DWT_CTRL_CYCTAP_Msk & ((value) << DWT_CTRL_CYCTAP_Pos))
#define DWT_CTRL_SYNCTAP_Pos                  _U_(10)                                              /**< (DWT_CTRL)  Position */
#define DWT_CTRL_SYNCTAP_Msk                  (_U_(0x3) << DWT_CTRL_SYNCTAP_Pos)                   /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_SYNCTAP(value)               (DWT_CTRL_SYNCTAP_Msk & ((value) << DWT_CTRL_SYNCTAP_Pos))
#define DWT_CTRL_PCSAMPLENA_Pos               _U_(12)                                              /**< (DWT_CTRL)  Position */
#define DWT_CTRL_PCSAMPLENA_Msk               (_U_(0x1) << DWT_CTRL_PCSAMPLENA_Pos)                /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_PCSAMPLENA(value)            (DWT_CTRL_PCSAMPLENA_Msk & ((value) << DWT_CTRL_PCSAMPLENA_Pos))
#define DWT_CTRL_EXCTRCENA_Pos                _U_(16)                                              /**< (DWT_CTRL)  Position */
#define DWT_CTRL_EXCTRCENA_Msk                (_U_(0x1) << DWT_CTRL_EXCTRCENA_Pos)                 /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_EXCTRCENA(value)             (DWT_CTRL_EXCTRCENA_Msk & ((value) << DWT_CTRL_EXCTRCENA_Pos))
#define DWT_CTRL_CPIEVTENA_Pos                _U_(17)                                              /**< (DWT_CTRL)  Position */
#define DWT_CTRL_CPIEVTENA_Msk                (_U_(0x1) << DWT_CTRL_CPIEVTENA_Pos)                 /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_CPIEVTENA(value)             (DWT_CTRL_CPIEVTENA_Msk & ((value) << DWT_CTRL_CPIEVTENA_Pos))
#define DWT_CTRL_EXCEVTENA_Pos                _U_(18)                                              /**< (DWT_CTRL)  Position */
#define DWT_CTRL_EXCEVTENA_Msk                (_U_(0x1) << DWT_CTRL_EXCEVTENA_Pos)                 /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_EXCEVTENA(value)             (DWT_CTRL_EXCEVTENA_Msk & ((value) << DWT_CTRL_EXCEVTENA_Pos))
#define DWT_CTRL_SLEEPEVTENA_Pos              _U_(19)                                              /**< (DWT_CTRL)  Position */
#define DWT_CTRL_SLEEPEVTENA_Msk              (_U_(0x1) << DWT_CTRL_SLEEPEVTENA_Pos)               /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_SLEEPEVTENA(value)           (DWT_CTRL_SLEEPEVTENA_Msk & ((value) << DWT_CTRL_SLEEPEVTENA_Pos))
#define DWT_CTRL_LSUEVTENA_Pos                _U_(20)                                              /**< (DWT_CTRL)  Position */
#define DWT_CTRL_LSUEVTENA_Msk                (_U_(0x1) << DWT_CTRL_LSUEVTENA_Pos)                 /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_LSUEVTENA(value)             (DWT_CTRL_LSUEVTENA_Msk & ((value) << DWT_CTRL_LSUEVTENA_Pos))
#define DWT_CTRL_FOLDEVTENA_Pos               _U_(21)                                              /**< (DWT_CTRL)  Position */
#define DWT_CTRL_FOLDEVTENA_Msk               (_U_(0x1) << DWT_CTRL_FOLDEVTENA_Pos)                /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_FOLDEVTENA(value)            (DWT_CTRL_FOLDEVTENA_Msk & ((value) << DWT_CTRL_FOLDEVTENA_Pos))
#define DWT_CTRL_CYCEVTENA_Pos                _U_(22)                                              /**< (DWT_CTRL)  Position */
#define DWT_CTRL_CYCEVTENA_Msk                (_U_(0x1) << DWT_CTRL_CYCEVTENA_Pos)                 /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_CYCEVTENA(value)             (DWT_CTRL_CYCEVTENA_Msk & ((value) << DWT_CTRL_CYCEVTENA_Pos))
#define DWT_CTRL_NOPRFCNT_Pos                 _U_(24)                                              /**< (DWT_CTRL)  Position */
#define DWT_CTRL_NOPRFCNT_Msk                 (_U_(0x1) << DWT_CTRL_NOPRFCNT_Pos)                  /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_NOPRFCNT(value)              (DWT_CTRL_NOPRFCNT_Msk & ((value) << DWT_CTRL_NOPRFCNT_Pos))
#define DWT_CTRL_NOCYCCNT_Pos                 _U_(25)                                              /**< (DWT_CTRL)  Position */
#define DWT_CTRL_NOCYCCNT_Msk                 (_U_(0x1) << DWT_CTRL_NOCYCCNT_Pos)                  /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_NOCYCCNT(value)              (DWT_CTRL_NOCYCCNT_Msk & ((value) << DWT_CTRL_NOCYCCNT_Pos))
#define DWT_CTRL_NOEXTTRIG_Pos                _U_(26)                                              /**< (DWT_CTRL)  Position */
#define DWT_CTRL_NOEXTTRIG_Msk                (_U_(0x1) << DWT_CTRL_NOEXTTRIG_Pos)                 /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_NOEXTTRIG(value)             (DWT_CTRL_NOEXTTRIG_Msk & ((value) << DWT_CTRL_NOEXTTRIG_Pos))
#define DWT_CTRL_NOTRCPKT_Pos                 _U_(27)                                              /**< (DWT_CTRL)  Position */
#define DWT_CTRL_NOTRCPKT_Msk                 (_U_(0x1) << DWT_CTRL_NOTRCPKT_Pos)                  /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_NOTRCPKT(value)              (DWT_CTRL_NOTRCPKT_Msk & ((value) << DWT_CTRL_NOTRCPKT_Pos))
#define DWT_CTRL_NUMCOMP_Pos                  _U_(28)                                              /**< (DWT_CTRL)  Position */
#define DWT_CTRL_NUMCOMP_Msk                  (_U_(0xF) << DWT_CTRL_NUMCOMP_Pos)                   /**< (DWT_CTRL)  Mask */
#define DWT_CTRL_NUMCOMP(value)               (DWT_CTRL_NUMCOMP_Msk & ((value) << DWT_CTRL_NUMCOMP_Pos))
#define DWT_CTRL_Msk                          _U_(0xFF7F1FFF)                                      /**< (DWT_CTRL) Register Mask  */


/* -------- DWT_CYCCNT : (DWT Offset: 0x04) (R/W 32) Cycle Count Register -------- */
#define DWT_CYCCNT_Msk                        _U_(0x00000000)                                      /**< (DWT_CYCCNT) Register Mask  */


/* -------- DWT_CPICNT : (DWT Offset: 0x08) (R/W 32) CPI Count Register -------- */
#define DWT_CPICNT_CPICNT_Pos                 _U_(0)                                               /**< (DWT_CPICNT)  Position */
#define DWT_CPICNT_CPICNT_Msk                 (_U_(0xFF) << DWT_CPICNT_CPICNT_Pos)                 /**< (DWT_CPICNT)  Mask */
#define DWT_CPICNT_CPICNT(value)              (DWT_CPICNT_CPICNT_Msk & ((value) << DWT_CPICNT_CPICNT_Pos))
#define DWT_CPICNT_Msk                        _U_(0x000000FF)                                      /**< (DWT_CPICNT) Register Mask  */


/* -------- DWT_EXCCNT : (DWT Offset: 0x0C) (R/W 32) Exception Overhead Count Register -------- */
#define DWT_EXCCNT_EXCCNT_Pos                 _U_(0)                                               /**< (DWT_EXCCNT)  Position */
#define DWT_EXCCNT_EXCCNT_Msk                 (_U_(0xFF) << DWT_EXCCNT_EXCCNT_Pos)                 /**< (DWT_EXCCNT)  Mask */
#define DWT_EXCCNT_EXCCNT(value)              (DWT_EXCCNT_EXCCNT_Msk & ((value) << DWT_EXCCNT_EXCCNT_Pos))
#define DWT_EXCCNT_Msk                        _U_(0x000000FF)                                      /**< (DWT_EXCCNT) Register Mask  */


/* -------- DWT_SLEEPCNT : (DWT Offset: 0x10) (R/W 32) Sleep Count Register -------- */
#define DWT_SLEEPCNT_SLEEPCNT_Pos             _U_(0)                                               /**< (DWT_SLEEPCNT)  Position */
#define DWT_SLEEPCNT_SLEEPCNT_Msk             (_U_(0xFF) << DWT_SLEEPCNT_SLEEPCNT_Pos)             /**< (DWT_SLEEPCNT)  Mask */
#define DWT_SLEEPCNT_SLEEPCNT(value)          (DWT_SLEEPCNT_SLEEPCNT_Msk & ((value) << DWT_SLEEPCNT_SLEEPCNT_Pos))
#define DWT_SLEEPCNT_Msk                      _U_(0x000000FF)                                      /**< (DWT_SLEEPCNT) Register Mask  */


/* -------- DWT_LSUCNT : (DWT Offset: 0x14) (R/W 32) LSU Count Register -------- */
#define DWT_LSUCNT_LSUCNT_Pos                 _U_(0)                                               /**< (DWT_LSUCNT)  Position */
#define DWT_LSUCNT_LSUCNT_Msk                 (_U_(0xFF) << DWT_LSUCNT_LSUCNT_Pos)                 /**< (DWT_LSUCNT)  Mask */
#define DWT_LSUCNT_LSUCNT(value)              (DWT_LSUCNT_LSUCNT_Msk & ((value) << DWT_LSUCNT_LSUCNT_Pos))
#define DWT_LSUCNT_Msk                        _U_(0x000000FF)                                      /**< (DWT_LSUCNT) Register Mask  */


/* -------- DWT_FOLDCNT : (DWT Offset: 0x18) (R/W 32) Folded-instruction Count Register -------- */
#define DWT_FOLDCNT_FOLDCNT_Pos               _U_(0)                                               /**< (DWT_FOLDCNT)  Position */
#define DWT_FOLDCNT_FOLDCNT_Msk               (_U_(0xFF) << DWT_FOLDCNT_FOLDCNT_Pos)               /**< (DWT_FOLDCNT)  Mask */
#define DWT_FOLDCNT_FOLDCNT(value)            (DWT_FOLDCNT_FOLDCNT_Msk & ((value) << DWT_FOLDCNT_FOLDCNT_Pos))
#define DWT_FOLDCNT_Msk                       _U_(0x000000FF)                                      /**< (DWT_FOLDCNT) Register Mask  */


/* -------- DWT_PCSR : (DWT Offset: 0x1C) ( R/ 32) Program Counter Sample Register -------- */
#define DWT_PCSR_Msk                          _U_(0x00000000)                                      /**< (DWT_PCSR) Register Mask  */


/* -------- DWT_COMP0 : (DWT Offset: 0x20) (R/W 32) Comparator Register 0 -------- */
#define DWT_COMP0_Msk                         _U_(0x00000000)                                      /**< (DWT_COMP0) Register Mask  */


/* -------- DWT_MASK0 : (DWT Offset: 0x24) (R/W 32) Mask Register 0 -------- */
#define DWT_MASK0_MASK_Pos                    _U_(0)                                               /**< (DWT_MASK0)  Position */
#define DWT_MASK0_MASK_Msk                    (_U_(0x1F) << DWT_MASK0_MASK_Pos)                    /**< (DWT_MASK0)  Mask */
#define DWT_MASK0_MASK(value)                 (DWT_MASK0_MASK_Msk & ((value) << DWT_MASK0_MASK_Pos))
#define DWT_MASK0_Msk                         _U_(0x0000001F)                                      /**< (DWT_MASK0) Register Mask  */


/* -------- DWT_FUNCTION0 : (DWT Offset: 0x28) (R/W 32) Function Register 0 -------- */
#define DWT_FUNCTION0_FUNCTION_Pos            _U_(0)                                               /**< (DWT_FUNCTION0)  Position */
#define DWT_FUNCTION0_FUNCTION_Msk            (_U_(0xF) << DWT_FUNCTION0_FUNCTION_Pos)             /**< (DWT_FUNCTION0)  Mask */
#define DWT_FUNCTION0_FUNCTION(value)         (DWT_FUNCTION0_FUNCTION_Msk & ((value) << DWT_FUNCTION0_FUNCTION_Pos))
#define DWT_FUNCTION0_EMITRANGE_Pos           _U_(5)                                               /**< (DWT_FUNCTION0)  Position */
#define DWT_FUNCTION0_EMITRANGE_Msk           (_U_(0x1) << DWT_FUNCTION0_EMITRANGE_Pos)            /**< (DWT_FUNCTION0)  Mask */
#define DWT_FUNCTION0_EMITRANGE(value)        (DWT_FUNCTION0_EMITRANGE_Msk & ((value) << DWT_FUNCTION0_EMITRANGE_Pos))
#define DWT_FUNCTION0_CYCMATCH_Pos            _U_(7)                                               /**< (DWT_FUNCTION0)  Position */
#define DWT_FUNCTION0_CYCMATCH_Msk            (_U_(0x1) << DWT_FUNCTION0_CYCMATCH_Pos)             /**< (DWT_FUNCTION0)  Mask */
#define DWT_FUNCTION0_CYCMATCH(value)         (DWT_FUNCTION0_CYCMATCH_Msk & ((value) << DWT_FUNCTION0_CYCMATCH_Pos))
#define DWT_FUNCTION0_DATAVMATCH_Pos          _U_(8)                                               /**< (DWT_FUNCTION0)  Position */
#define DWT_FUNCTION0_DATAVMATCH_Msk          (_U_(0x1) << DWT_FUNCTION0_DATAVMATCH_Pos)           /**< (DWT_FUNCTION0)  Mask */
#define DWT_FUNCTION0_DATAVMATCH(value)       (DWT_FUNCTION0_DATAVMATCH_Msk & ((value) << DWT_FUNCTION0_DATAVMATCH_Pos))
#define DWT_FUNCTION0_LNK1ENA_Pos             _U_(9)                                               /**< (DWT_FUNCTION0)  Position */
#define DWT_FUNCTION0_LNK1ENA_Msk             (_U_(0x1) << DWT_FUNCTION0_LNK1ENA_Pos)              /**< (DWT_FUNCTION0)  Mask */
#define DWT_FUNCTION0_LNK1ENA(value)          (DWT_FUNCTION0_LNK1ENA_Msk & ((value) << DWT_FUNCTION0_LNK1ENA_Pos))
#define DWT_FUNCTION0_DATAVSIZE_Pos           _U_(10)                                              /**< (DWT_FUNCTION0)  Position */
#define DWT_FUNCTION0_DATAVSIZE_Msk           (_U_(0x3) << DWT_FUNCTION0_DATAVSIZE_Pos)            /**< (DWT_FUNCTION0)  Mask */
#define DWT_FUNCTION0_DATAVSIZE(value)        (DWT_FUNCTION0_DATAVSIZE_Msk & ((value) << DWT_FUNCTION0_DATAVSIZE_Pos))
#define DWT_FUNCTION0_DATAVADDR0_Pos          _U_(12)                                              /**< (DWT_FUNCTION0)  Position */
#define DWT_FUNCTION0_DATAVADDR0_Msk          (_U_(0xF) << DWT_FUNCTION0_DATAVADDR0_Pos)           /**< (DWT_FUNCTION0)  Mask */
#define DWT_FUNCTION0_DATAVADDR0(value)       (DWT_FUNCTION0_DATAVADDR0_Msk & ((value) << DWT_FUNCTION0_DATAVADDR0_Pos))
#define DWT_FUNCTION0_DATAVADDR1_Pos          _U_(16)                                              /**< (DWT_FUNCTION0)  Position */
#define DWT_FUNCTION0_DATAVADDR1_Msk          (_U_(0xF) << DWT_FUNCTION0_DATAVADDR1_Pos)           /**< (DWT_FUNCTION0)  Mask */
#define DWT_FUNCTION0_DATAVADDR1(value)       (DWT_FUNCTION0_DATAVADDR1_Msk & ((value) << DWT_FUNCTION0_DATAVADDR1_Pos))
#define DWT_FUNCTION0_MATCHED_Pos             _U_(24)                                              /**< (DWT_FUNCTION0)  Position */
#define DWT_FUNCTION0_MATCHED_Msk             (_U_(0x1) << DWT_FUNCTION0_MATCHED_Pos)              /**< (DWT_FUNCTION0)  Mask */
#define DWT_FUNCTION0_MATCHED(value)          (DWT_FUNCTION0_MATCHED_Msk & ((value) << DWT_FUNCTION0_MATCHED_Pos))
#define DWT_FUNCTION0_Msk                     _U_(0x010FFFAF)                                      /**< (DWT_FUNCTION0) Register Mask  */


/* -------- DWT_COMP1 : (DWT Offset: 0x30) (R/W 32) Comparator Register 1 -------- */
#define DWT_COMP1_Msk                         _U_(0x00000000)                                      /**< (DWT_COMP1) Register Mask  */


/* -------- DWT_MASK1 : (DWT Offset: 0x34) (R/W 32) Mask Register 1 -------- */
#define DWT_MASK1_MASK_Pos                    _U_(0)                                               /**< (DWT_MASK1)  Position */
#define DWT_MASK1_MASK_Msk                    (_U_(0x1F) << DWT_MASK1_MASK_Pos)                    /**< (DWT_MASK1)  Mask */
#define DWT_MASK1_MASK(value)                 (DWT_MASK1_MASK_Msk & ((value) << DWT_MASK1_MASK_Pos))
#define DWT_MASK1_Msk                         _U_(0x0000001F)                                      /**< (DWT_MASK1) Register Mask  */


/* -------- DWT_FUNCTION1 : (DWT Offset: 0x38) (R/W 32) Function Register 1 -------- */
#define DWT_FUNCTION1_FUNCTION_Pos            _U_(0)                                               /**< (DWT_FUNCTION1)  Position */
#define DWT_FUNCTION1_FUNCTION_Msk            (_U_(0xF) << DWT_FUNCTION1_FUNCTION_Pos)             /**< (DWT_FUNCTION1)  Mask */
#define DWT_FUNCTION1_FUNCTION(value)         (DWT_FUNCTION1_FUNCTION_Msk & ((value) << DWT_FUNCTION1_FUNCTION_Pos))
#define DWT_FUNCTION1_EMITRANGE_Pos           _U_(5)                                               /**< (DWT_FUNCTION1)  Position */
#define DWT_FUNCTION1_EMITRANGE_Msk           (_U_(0x1) << DWT_FUNCTION1_EMITRANGE_Pos)            /**< (DWT_FUNCTION1)  Mask */
#define DWT_FUNCTION1_EMITRANGE(value)        (DWT_FUNCTION1_EMITRANGE_Msk & ((value) << DWT_FUNCTION1_EMITRANGE_Pos))
#define DWT_FUNCTION1_CYCMATCH_Pos            _U_(7)                                               /**< (DWT_FUNCTION1)  Position */
#define DWT_FUNCTION1_CYCMATCH_Msk            (_U_(0x1) << DWT_FUNCTION1_CYCMATCH_Pos)             /**< (DWT_FUNCTION1)  Mask */
#define DWT_FUNCTION1_CYCMATCH(value)         (DWT_FUNCTION1_CYCMATCH_Msk & ((value) << DWT_FUNCTION1_CYCMATCH_Pos))
#define DWT_FUNCTION1_DATAVMATCH_Pos          _U_(8)                                               /**< (DWT_FUNCTION1)  Position */
#define DWT_FUNCTION1_DATAVMATCH_Msk          (_U_(0x1) << DWT_FUNCTION1_DATAVMATCH_Pos)           /**< (DWT_FUNCTION1)  Mask */
#define DWT_FUNCTION1_DATAVMATCH(value)       (DWT_FUNCTION1_DATAVMATCH_Msk & ((value) << DWT_FUNCTION1_DATAVMATCH_Pos))
#define DWT_FUNCTION1_LNK1ENA_Pos             _U_(9)                                               /**< (DWT_FUNCTION1)  Position */
#define DWT_FUNCTION1_LNK1ENA_Msk             (_U_(0x1) << DWT_FUNCTION1_LNK1ENA_Pos)              /**< (DWT_FUNCTION1)  Mask */
#define DWT_FUNCTION1_LNK1ENA(value)          (DWT_FUNCTION1_LNK1ENA_Msk & ((value) << DWT_FUNCTION1_LNK1ENA_Pos))
#define DWT_FUNCTION1_DATAVSIZE_Pos           _U_(10)                                              /**< (DWT_FUNCTION1)  Position */
#define DWT_FUNCTION1_DATAVSIZE_Msk           (_U_(0x3) << DWT_FUNCTION1_DATAVSIZE_Pos)            /**< (DWT_FUNCTION1)  Mask */
#define DWT_FUNCTION1_DATAVSIZE(value)        (DWT_FUNCTION1_DATAVSIZE_Msk & ((value) << DWT_FUNCTION1_DATAVSIZE_Pos))
#define DWT_FUNCTION1_DATAVADDR0_Pos          _U_(12)                                              /**< (DWT_FUNCTION1)  Position */
#define DWT_FUNCTION1_DATAVADDR0_Msk          (_U_(0xF) << DWT_FUNCTION1_DATAVADDR0_Pos)           /**< (DWT_FUNCTION1)  Mask */
#define DWT_FUNCTION1_DATAVADDR0(value)       (DWT_FUNCTION1_DATAVADDR0_Msk & ((value) << DWT_FUNCTION1_DATAVADDR0_Pos))
#define DWT_FUNCTION1_DATAVADDR1_Pos          _U_(16)                                              /**< (DWT_FUNCTION1)  Position */
#define DWT_FUNCTION1_DATAVADDR1_Msk          (_U_(0xF) << DWT_FUNCTION1_DATAVADDR1_Pos)           /**< (DWT_FUNCTION1)  Mask */
#define DWT_FUNCTION1_DATAVADDR1(value)       (DWT_FUNCTION1_DATAVADDR1_Msk & ((value) << DWT_FUNCTION1_DATAVADDR1_Pos))
#define DWT_FUNCTION1_MATCHED_Pos             _U_(24)                                              /**< (DWT_FUNCTION1)  Position */
#define DWT_FUNCTION1_MATCHED_Msk             (_U_(0x1) << DWT_FUNCTION1_MATCHED_Pos)              /**< (DWT_FUNCTION1)  Mask */
#define DWT_FUNCTION1_MATCHED(value)          (DWT_FUNCTION1_MATCHED_Msk & ((value) << DWT_FUNCTION1_MATCHED_Pos))
#define DWT_FUNCTION1_Msk                     _U_(0x010FFFAF)                                      /**< (DWT_FUNCTION1) Register Mask  */


/* -------- DWT_COMP2 : (DWT Offset: 0x40) (R/W 32) Comparator Register 2 -------- */
#define DWT_COMP2_Msk                         _U_(0x00000000)                                      /**< (DWT_COMP2) Register Mask  */


/* -------- DWT_MASK2 : (DWT Offset: 0x44) (R/W 32) Mask Register 2 -------- */
#define DWT_MASK2_MASK_Pos                    _U_(0)                                               /**< (DWT_MASK2)  Position */
#define DWT_MASK2_MASK_Msk                    (_U_(0x1F) << DWT_MASK2_MASK_Pos)                    /**< (DWT_MASK2)  Mask */
#define DWT_MASK2_MASK(value)                 (DWT_MASK2_MASK_Msk & ((value) << DWT_MASK2_MASK_Pos))
#define DWT_MASK2_Msk                         _U_(0x0000001F)                                      /**< (DWT_MASK2) Register Mask  */


/* -------- DWT_FUNCTION2 : (DWT Offset: 0x48) (R/W 32) Function Register 2 -------- */
#define DWT_FUNCTION2_FUNCTION_Pos            _U_(0)                                               /**< (DWT_FUNCTION2)  Position */
#define DWT_FUNCTION2_FUNCTION_Msk            (_U_(0xF) << DWT_FUNCTION2_FUNCTION_Pos)             /**< (DWT_FUNCTION2)  Mask */
#define DWT_FUNCTION2_FUNCTION(value)         (DWT_FUNCTION2_FUNCTION_Msk & ((value) << DWT_FUNCTION2_FUNCTION_Pos))
#define DWT_FUNCTION2_EMITRANGE_Pos           _U_(5)                                               /**< (DWT_FUNCTION2)  Position */
#define DWT_FUNCTION2_EMITRANGE_Msk           (_U_(0x1) << DWT_FUNCTION2_EMITRANGE_Pos)            /**< (DWT_FUNCTION2)  Mask */
#define DWT_FUNCTION2_EMITRANGE(value)        (DWT_FUNCTION2_EMITRANGE_Msk & ((value) << DWT_FUNCTION2_EMITRANGE_Pos))
#define DWT_FUNCTION2_CYCMATCH_Pos            _U_(7)                                               /**< (DWT_FUNCTION2)  Position */
#define DWT_FUNCTION2_CYCMATCH_Msk            (_U_(0x1) << DWT_FUNCTION2_CYCMATCH_Pos)             /**< (DWT_FUNCTION2)  Mask */
#define DWT_FUNCTION2_CYCMATCH(value)         (DWT_FUNCTION2_CYCMATCH_Msk & ((value) << DWT_FUNCTION2_CYCMATCH_Pos))
#define DWT_FUNCTION2_DATAVMATCH_Pos          _U_(8)                                               /**< (DWT_FUNCTION2)  Position */
#define DWT_FUNCTION2_DATAVMATCH_Msk          (_U_(0x1) << DWT_FUNCTION2_DATAVMATCH_Pos)           /**< (DWT_FUNCTION2)  Mask */
#define DWT_FUNCTION2_DATAVMATCH(value)       (DWT_FUNCTION2_DATAVMATCH_Msk & ((value) << DWT_FUNCTION2_DATAVMATCH_Pos))
#define DWT_FUNCTION2_LNK1ENA_Pos             _U_(9)                                               /**< (DWT_FUNCTION2)  Position */
#define DWT_FUNCTION2_LNK1ENA_Msk             (_U_(0x1) << DWT_FUNCTION2_LNK1ENA_Pos)              /**< (DWT_FUNCTION2)  Mask */
#define DWT_FUNCTION2_LNK1ENA(value)          (DWT_FUNCTION2_LNK1ENA_Msk & ((value) << DWT_FUNCTION2_LNK1ENA_Pos))
#define DWT_FUNCTION2_DATAVSIZE_Pos           _U_(10)                                              /**< (DWT_FUNCTION2)  Position */
#define DWT_FUNCTION2_DATAVSIZE_Msk           (_U_(0x3) << DWT_FUNCTION2_DATAVSIZE_Pos)            /**< (DWT_FUNCTION2)  Mask */
#define DWT_FUNCTION2_DATAVSIZE(value)        (DWT_FUNCTION2_DATAVSIZE_Msk & ((value) << DWT_FUNCTION2_DATAVSIZE_Pos))
#define DWT_FUNCTION2_DATAVADDR0_Pos          _U_(12)                                              /**< (DWT_FUNCTION2)  Position */
#define DWT_FUNCTION2_DATAVADDR0_Msk          (_U_(0xF) << DWT_FUNCTION2_DATAVADDR0_Pos)           /**< (DWT_FUNCTION2)  Mask */
#define DWT_FUNCTION2_DATAVADDR0(value)       (DWT_FUNCTION2_DATAVADDR0_Msk & ((value) << DWT_FUNCTION2_DATAVADDR0_Pos))
#define DWT_FUNCTION2_DATAVADDR1_Pos          _U_(16)                                              /**< (DWT_FUNCTION2)  Position */
#define DWT_FUNCTION2_DATAVADDR1_Msk          (_U_(0xF) << DWT_FUNCTION2_DATAVADDR1_Pos)           /**< (DWT_FUNCTION2)  Mask */
#define DWT_FUNCTION2_DATAVADDR1(value)       (DWT_FUNCTION2_DATAVADDR1_Msk & ((value) << DWT_FUNCTION2_DATAVADDR1_Pos))
#define DWT_FUNCTION2_MATCHED_Pos             _U_(24)                                              /**< (DWT_FUNCTION2)  Position */
#define DWT_FUNCTION2_MATCHED_Msk             (_U_(0x1) << DWT_FUNCTION2_MATCHED_Pos)              /**< (DWT_FUNCTION2)  Mask */
#define DWT_FUNCTION2_MATCHED(value)          (DWT_FUNCTION2_MATCHED_Msk & ((value) << DWT_FUNCTION2_MATCHED_Pos))
#define DWT_FUNCTION2_Msk                     _U_(0x010FFFAF)                                      /**< (DWT_FUNCTION2) Register Mask  */


/* -------- DWT_COMP3 : (DWT Offset: 0x50) (R/W 32) Comparator Register 3 -------- */
#define DWT_COMP3_Msk                         _U_(0x00000000)                                      /**< (DWT_COMP3) Register Mask  */


/* -------- DWT_MASK3 : (DWT Offset: 0x54) (R/W 32) Mask Register 3 -------- */
#define DWT_MASK3_MASK_Pos                    _U_(0)                                               /**< (DWT_MASK3)  Position */
#define DWT_MASK3_MASK_Msk                    (_U_(0x1F) << DWT_MASK3_MASK_Pos)                    /**< (DWT_MASK3)  Mask */
#define DWT_MASK3_MASK(value)                 (DWT_MASK3_MASK_Msk & ((value) << DWT_MASK3_MASK_Pos))
#define DWT_MASK3_Msk                         _U_(0x0000001F)                                      /**< (DWT_MASK3) Register Mask  */


/* -------- DWT_FUNCTION3 : (DWT Offset: 0x58) (R/W 32) Function Register 3 -------- */
#define DWT_FUNCTION3_FUNCTION_Pos            _U_(0)                                               /**< (DWT_FUNCTION3)  Position */
#define DWT_FUNCTION3_FUNCTION_Msk            (_U_(0xF) << DWT_FUNCTION3_FUNCTION_Pos)             /**< (DWT_FUNCTION3)  Mask */
#define DWT_FUNCTION3_FUNCTION(value)         (DWT_FUNCTION3_FUNCTION_Msk & ((value) << DWT_FUNCTION3_FUNCTION_Pos))
#define DWT_FUNCTION3_EMITRANGE_Pos           _U_(5)                                               /**< (DWT_FUNCTION3)  Position */
#define DWT_FUNCTION3_EMITRANGE_Msk           (_U_(0x1) << DWT_FUNCTION3_EMITRANGE_Pos)            /**< (DWT_FUNCTION3)  Mask */
#define DWT_FUNCTION3_EMITRANGE(value)        (DWT_FUNCTION3_EMITRANGE_Msk & ((value) << DWT_FUNCTION3_EMITRANGE_Pos))
#define DWT_FUNCTION3_CYCMATCH_Pos            _U_(7)                                               /**< (DWT_FUNCTION3)  Position */
#define DWT_FUNCTION3_CYCMATCH_Msk            (_U_(0x1) << DWT_FUNCTION3_CYCMATCH_Pos)             /**< (DWT_FUNCTION3)  Mask */
#define DWT_FUNCTION3_CYCMATCH(value)         (DWT_FUNCTION3_CYCMATCH_Msk & ((value) << DWT_FUNCTION3_CYCMATCH_Pos))
#define DWT_FUNCTION3_DATAVMATCH_Pos          _U_(8)                                               /**< (DWT_FUNCTION3)  Position */
#define DWT_FUNCTION3_DATAVMATCH_Msk          (_U_(0x1) << DWT_FUNCTION3_DATAVMATCH_Pos)           /**< (DWT_FUNCTION3)  Mask */
#define DWT_FUNCTION3_DATAVMATCH(value)       (DWT_FUNCTION3_DATAVMATCH_Msk & ((value) << DWT_FUNCTION3_DATAVMATCH_Pos))
#define DWT_FUNCTION3_LNK1ENA_Pos             _U_(9)                                               /**< (DWT_FUNCTION3)  Position */
#define DWT_FUNCTION3_LNK1ENA_Msk             (_U_(0x1) << DWT_FUNCTION3_LNK1ENA_Pos)              /**< (DWT_FUNCTION3)  Mask */
#define DWT_FUNCTION3_LNK1ENA(value)          (DWT_FUNCTION3_LNK1ENA_Msk & ((value) << DWT_FUNCTION3_LNK1ENA_Pos))
#define DWT_FUNCTION3_DATAVSIZE_Pos           _U_(10)                                              /**< (DWT_FUNCTION3)  Position */
#define DWT_FUNCTION3_DATAVSIZE_Msk           (_U_(0x3) << DWT_FUNCTION3_DATAVSIZE_Pos)            /**< (DWT_FUNCTION3)  Mask */
#define DWT_FUNCTION3_DATAVSIZE(value)        (DWT_FUNCTION3_DATAVSIZE_Msk & ((value) << DWT_FUNCTION3_DATAVSIZE_Pos))
#define DWT_FUNCTION3_DATAVADDR0_Pos          _U_(12)                                              /**< (DWT_FUNCTION3)  Position */
#define DWT_FUNCTION3_DATAVADDR0_Msk          (_U_(0xF) << DWT_FUNCTION3_DATAVADDR0_Pos)           /**< (DWT_FUNCTION3)  Mask */
#define DWT_FUNCTION3_DATAVADDR0(value)       (DWT_FUNCTION3_DATAVADDR0_Msk & ((value) << DWT_FUNCTION3_DATAVADDR0_Pos))
#define DWT_FUNCTION3_DATAVADDR1_Pos          _U_(16)                                              /**< (DWT_FUNCTION3)  Position */
#define DWT_FUNCTION3_DATAVADDR1_Msk          (_U_(0xF) << DWT_FUNCTION3_DATAVADDR1_Pos)           /**< (DWT_FUNCTION3)  Mask */
#define DWT_FUNCTION3_DATAVADDR1(value)       (DWT_FUNCTION3_DATAVADDR1_Msk & ((value) << DWT_FUNCTION3_DATAVADDR1_Pos))
#define DWT_FUNCTION3_MATCHED_Pos             _U_(24)                                              /**< (DWT_FUNCTION3)  Position */
#define DWT_FUNCTION3_MATCHED_Msk             (_U_(0x1) << DWT_FUNCTION3_MATCHED_Pos)              /**< (DWT_FUNCTION3)  Mask */
#define DWT_FUNCTION3_MATCHED(value)          (DWT_FUNCTION3_MATCHED_Msk & ((value) << DWT_FUNCTION3_MATCHED_Pos))
#define DWT_FUNCTION3_Msk                     _U_(0x010FFFAF)                                      /**< (DWT_FUNCTION3) Register Mask  */


/** \brief DWT register offsets definitions */
#define DWT_CTRL_REG_OFST              (0x00)              /**< (DWT_CTRL) Control Register Offset */
#define DWT_CYCCNT_REG_OFST            (0x04)              /**< (DWT_CYCCNT) Cycle Count Register Offset */
#define DWT_CPICNT_REG_OFST            (0x08)              /**< (DWT_CPICNT) CPI Count Register Offset */
#define DWT_EXCCNT_REG_OFST            (0x0C)              /**< (DWT_EXCCNT) Exception Overhead Count Register Offset */
#define DWT_SLEEPCNT_REG_OFST          (0x10)              /**< (DWT_SLEEPCNT) Sleep Count Register Offset */
#define DWT_LSUCNT_REG_OFST            (0x14)              /**< (DWT_LSUCNT) LSU Count Register Offset */
#define DWT_FOLDCNT_REG_OFST           (0x18)              /**< (DWT_FOLDCNT) Folded-instruction Count Register Offset */
#define DWT_PCSR_REG_OFST              (0x1C)              /**< (DWT_PCSR) Program Counter Sample Register Offset */
#define DWT_COMP0_REG_OFST             (0x20)              /**< (DWT_COMP0) Comparator Register 0 Offset */
#define DWT_MASK0_REG_OFST             (0x24)              /**< (DWT_MASK0) Mask Register 0 Offset */
#define DWT_FUNCTION0_REG_OFST         (0x28)              /**< (DWT_FUNCTION0) Function Register 0 Offset */
#define DWT_COMP1_REG_OFST             (0x30)              /**< (DWT_COMP1) Comparator Register 1 Offset */
#define DWT_MASK1_REG_OFST             (0x34)              /**< (DWT_MASK1) Mask Register 1 Offset */
#define DWT_FUNCTION1_REG_OFST         (0x38)              /**< (DWT_FUNCTION1) Function Register 1 Offset */
#define DWT_COMP2_REG_OFST             (0x40)              /**< (DWT_COMP2) Comparator Register 2 Offset */
#define DWT_MASK2_REG_OFST             (0x44)              /**< (DWT_MASK2) Mask Register 2 Offset */
#define DWT_FUNCTION2_REG_OFST         (0x48)              /**< (DWT_FUNCTION2) Function Register 2 Offset */
#define DWT_COMP3_REG_OFST             (0x50)              /**< (DWT_COMP3) Comparator Register 3 Offset */
#define DWT_MASK3_REG_OFST             (0x54)              /**< (DWT_MASK3) Mask Register 3 Offset */
#define DWT_FUNCTION3_REG_OFST         (0x58)              /**< (DWT_FUNCTION3) Function Register 3 Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief DWT register API structure */
typedef struct
{  /* Data Watchpoint and Trace Register */
  __IO  uint32_t                       DWT_CTRL;           /**< Offset: 0x00 (R/W  32) Control Register */
  __IO  uint32_t                       DWT_CYCCNT;         /**< Offset: 0x04 (R/W  32) Cycle Count Register */
  __IO  uint32_t                       DWT_CPICNT;         /**< Offset: 0x08 (R/W  32) CPI Count Register */
  __IO  uint32_t                       DWT_EXCCNT;         /**< Offset: 0x0C (R/W  32) Exception Overhead Count Register */
  __IO  uint32_t                       DWT_SLEEPCNT;       /**< Offset: 0x10 (R/W  32) Sleep Count Register */
  __IO  uint32_t                       DWT_LSUCNT;         /**< Offset: 0x14 (R/W  32) LSU Count Register */
  __IO  uint32_t                       DWT_FOLDCNT;        /**< Offset: 0x18 (R/W  32) Folded-instruction Count Register */
  __I   uint32_t                       DWT_PCSR;           /**< Offset: 0x1C (R/   32) Program Counter Sample Register */
  __IO  uint32_t                       DWT_COMP0;          /**< Offset: 0x20 (R/W  32) Comparator Register 0 */
  __IO  uint32_t                       DWT_MASK0;          /**< Offset: 0x24 (R/W  32) Mask Register 0 */
  __IO  uint32_t                       DWT_FUNCTION0;      /**< Offset: 0x28 (R/W  32) Function Register 0 */
  __I   uint8_t                        Reserved1[0x04];
  __IO  uint32_t                       DWT_COMP1;          /**< Offset: 0x30 (R/W  32) Comparator Register 1 */
  __IO  uint32_t                       DWT_MASK1;          /**< Offset: 0x34 (R/W  32) Mask Register 1 */
  __IO  uint32_t                       DWT_FUNCTION1;      /**< Offset: 0x38 (R/W  32) Function Register 1 */
  __I   uint8_t                        Reserved2[0x04];
  __IO  uint32_t                       DWT_COMP2;          /**< Offset: 0x40 (R/W  32) Comparator Register 2 */
  __IO  uint32_t                       DWT_MASK2;          /**< Offset: 0x44 (R/W  32) Mask Register 2 */
  __IO  uint32_t                       DWT_FUNCTION2;      /**< Offset: 0x48 (R/W  32) Function Register 2 */
  __I   uint8_t                        Reserved3[0x04];
  __IO  uint32_t                       DWT_COMP3;          /**< Offset: 0x50 (R/W  32) Comparator Register 3 */
  __IO  uint32_t                       DWT_MASK3;          /**< Offset: 0x54 (R/W  32) Mask Register 3 */
  __IO  uint32_t                       DWT_FUNCTION3;      /**< Offset: 0x58 (R/W  32) Function Register 3 */
} dwt_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_DWT_COMPONENT_H_ */
