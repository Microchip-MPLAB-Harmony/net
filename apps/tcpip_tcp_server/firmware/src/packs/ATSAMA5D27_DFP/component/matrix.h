/**
 * \brief Component description for MATRIX
 *
 * Copyright (c) 2020 Microchip Technology Inc. and its subsidiaries.
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

/* file generated from device description version 2020-03-18T12:53:15Z */
#ifndef _SAMA5D2_MATRIX_COMPONENT_H_
#define _SAMA5D2_MATRIX_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR MATRIX                                       */
/* ************************************************************************** */

/* -------- MATRIX_PRAS : (MATRIX Offset: 0x00) (R/W 32) Priority Register A for Slave 0 -------- */
#define MATRIX_PRAS_M0PR_Pos                  _U_(0)                                               /**< (MATRIX_PRAS) Master 0 Priority Position */
#define MATRIX_PRAS_M0PR_Msk                  (_U_(0x3) << MATRIX_PRAS_M0PR_Pos)                   /**< (MATRIX_PRAS) Master 0 Priority Mask */
#define MATRIX_PRAS_M0PR(value)               (MATRIX_PRAS_M0PR_Msk & ((value) << MATRIX_PRAS_M0PR_Pos))
#define MATRIX_PRAS_M1PR_Pos                  _U_(4)                                               /**< (MATRIX_PRAS) Master 1 Priority Position */
#define MATRIX_PRAS_M1PR_Msk                  (_U_(0x3) << MATRIX_PRAS_M1PR_Pos)                   /**< (MATRIX_PRAS) Master 1 Priority Mask */
#define MATRIX_PRAS_M1PR(value)               (MATRIX_PRAS_M1PR_Msk & ((value) << MATRIX_PRAS_M1PR_Pos))
#define MATRIX_PRAS_M2PR_Pos                  _U_(8)                                               /**< (MATRIX_PRAS) Master 2 Priority Position */
#define MATRIX_PRAS_M2PR_Msk                  (_U_(0x3) << MATRIX_PRAS_M2PR_Pos)                   /**< (MATRIX_PRAS) Master 2 Priority Mask */
#define MATRIX_PRAS_M2PR(value)               (MATRIX_PRAS_M2PR_Msk & ((value) << MATRIX_PRAS_M2PR_Pos))
#define MATRIX_PRAS_M3PR_Pos                  _U_(12)                                              /**< (MATRIX_PRAS) Master 3 Priority Position */
#define MATRIX_PRAS_M3PR_Msk                  (_U_(0x3) << MATRIX_PRAS_M3PR_Pos)                   /**< (MATRIX_PRAS) Master 3 Priority Mask */
#define MATRIX_PRAS_M3PR(value)               (MATRIX_PRAS_M3PR_Msk & ((value) << MATRIX_PRAS_M3PR_Pos))
#define MATRIX_PRAS_M4PR_Pos                  _U_(16)                                              /**< (MATRIX_PRAS) Master 4 Priority Position */
#define MATRIX_PRAS_M4PR_Msk                  (_U_(0x3) << MATRIX_PRAS_M4PR_Pos)                   /**< (MATRIX_PRAS) Master 4 Priority Mask */
#define MATRIX_PRAS_M4PR(value)               (MATRIX_PRAS_M4PR_Msk & ((value) << MATRIX_PRAS_M4PR_Pos))
#define MATRIX_PRAS_M5PR_Pos                  _U_(20)                                              /**< (MATRIX_PRAS) Master 5 Priority Position */
#define MATRIX_PRAS_M5PR_Msk                  (_U_(0x3) << MATRIX_PRAS_M5PR_Pos)                   /**< (MATRIX_PRAS) Master 5 Priority Mask */
#define MATRIX_PRAS_M5PR(value)               (MATRIX_PRAS_M5PR_Msk & ((value) << MATRIX_PRAS_M5PR_Pos))
#define MATRIX_PRAS_M6PR_Pos                  _U_(24)                                              /**< (MATRIX_PRAS) Master 6 Priority Position */
#define MATRIX_PRAS_M6PR_Msk                  (_U_(0x3) << MATRIX_PRAS_M6PR_Pos)                   /**< (MATRIX_PRAS) Master 6 Priority Mask */
#define MATRIX_PRAS_M6PR(value)               (MATRIX_PRAS_M6PR_Msk & ((value) << MATRIX_PRAS_M6PR_Pos))
#define MATRIX_PRAS_M7PR_Pos                  _U_(28)                                              /**< (MATRIX_PRAS) Master 7 Priority Position */
#define MATRIX_PRAS_M7PR_Msk                  (_U_(0x3) << MATRIX_PRAS_M7PR_Pos)                   /**< (MATRIX_PRAS) Master 7 Priority Mask */
#define MATRIX_PRAS_M7PR(value)               (MATRIX_PRAS_M7PR_Msk & ((value) << MATRIX_PRAS_M7PR_Pos))
#define MATRIX_PRAS_Msk                       _U_(0x33333333)                                      /**< (MATRIX_PRAS) Register Mask  */


/* -------- MATRIX_PRBS : (MATRIX Offset: 0x04) (R/W 32) Priority Register B for Slave 0 -------- */
#define MATRIX_PRBS_M8PR_Pos                  _U_(0)                                               /**< (MATRIX_PRBS) Master 8 Priority Position */
#define MATRIX_PRBS_M8PR_Msk                  (_U_(0x3) << MATRIX_PRBS_M8PR_Pos)                   /**< (MATRIX_PRBS) Master 8 Priority Mask */
#define MATRIX_PRBS_M8PR(value)               (MATRIX_PRBS_M8PR_Msk & ((value) << MATRIX_PRBS_M8PR_Pos))
#define MATRIX_PRBS_M9PR_Pos                  _U_(4)                                               /**< (MATRIX_PRBS) Master 9 Priority Position */
#define MATRIX_PRBS_M9PR_Msk                  (_U_(0x3) << MATRIX_PRBS_M9PR_Pos)                   /**< (MATRIX_PRBS) Master 9 Priority Mask */
#define MATRIX_PRBS_M9PR(value)               (MATRIX_PRBS_M9PR_Msk & ((value) << MATRIX_PRBS_M9PR_Pos))
#define MATRIX_PRBS_M10PR_Pos                 _U_(8)                                               /**< (MATRIX_PRBS) Master 10 Priority Position */
#define MATRIX_PRBS_M10PR_Msk                 (_U_(0x3) << MATRIX_PRBS_M10PR_Pos)                  /**< (MATRIX_PRBS) Master 10 Priority Mask */
#define MATRIX_PRBS_M10PR(value)              (MATRIX_PRBS_M10PR_Msk & ((value) << MATRIX_PRBS_M10PR_Pos))
#define MATRIX_PRBS_M11PR_Pos                 _U_(12)                                              /**< (MATRIX_PRBS) Master 11 Priority Position */
#define MATRIX_PRBS_M11PR_Msk                 (_U_(0x3) << MATRIX_PRBS_M11PR_Pos)                  /**< (MATRIX_PRBS) Master 11 Priority Mask */
#define MATRIX_PRBS_M11PR(value)              (MATRIX_PRBS_M11PR_Msk & ((value) << MATRIX_PRBS_M11PR_Pos))
#define MATRIX_PRBS_Msk                       _U_(0x00003333)                                      /**< (MATRIX_PRBS) Register Mask  */


/* -------- MATRIX_MCFG : (MATRIX Offset: 0x00) (R/W 32) Master Configuration Register -------- */
#define MATRIX_MCFG_ULBT_Pos                  _U_(0)                                               /**< (MATRIX_MCFG) Undefined Length Burst Type Position */
#define MATRIX_MCFG_ULBT_Msk                  (_U_(0x7) << MATRIX_MCFG_ULBT_Pos)                   /**< (MATRIX_MCFG) Undefined Length Burst Type Mask */
#define MATRIX_MCFG_ULBT(value)               (MATRIX_MCFG_ULBT_Msk & ((value) << MATRIX_MCFG_ULBT_Pos))
#define   MATRIX_MCFG_ULBT_UNLIMITED_Val      _U_(0x0)                                             /**< (MATRIX_MCFG) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1 Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave.  */
#define   MATRIX_MCFG_ULBT_SINGLE_Val         _U_(0x1)                                             /**< (MATRIX_MCFG) Single Access-The undefined length burst is treated as a succession of single accesses, allowing rearbitration at each beat of the INCR burst or bursts sequence.  */
#define   MATRIX_MCFG_ULBT_4_BEAT_Val         _U_(0x2)                                             /**< (MATRIX_MCFG) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing rearbitration every 4 beats.  */
#define   MATRIX_MCFG_ULBT_8_BEAT_Val         _U_(0x3)                                             /**< (MATRIX_MCFG) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing rearbitration every 8 beats.  */
#define   MATRIX_MCFG_ULBT_16_BEAT_Val        _U_(0x4)                                             /**< (MATRIX_MCFG) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing rearbitration every 16 beats.  */
#define   MATRIX_MCFG_ULBT_32_BEAT_Val        _U_(0x5)                                             /**< (MATRIX_MCFG) 32-beat Burst-The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing rearbitration every 32 beats.  */
#define   MATRIX_MCFG_ULBT_64_BEAT_Val        _U_(0x6)                                             /**< (MATRIX_MCFG) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing rearbitration every 64 beats.  */
#define   MATRIX_MCFG_ULBT_128_BEAT_Val       _U_(0x7)                                             /**< (MATRIX_MCFG) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing rearbitration every 128 beats.Unless duly needed, the ULBT should be left at its default 0 value for power saving.  */
#define MATRIX_MCFG_ULBT_UNLIMITED            (MATRIX_MCFG_ULBT_UNLIMITED_Val << MATRIX_MCFG_ULBT_Pos) /**< (MATRIX_MCFG) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1 Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. Position  */
#define MATRIX_MCFG_ULBT_SINGLE               (MATRIX_MCFG_ULBT_SINGLE_Val << MATRIX_MCFG_ULBT_Pos) /**< (MATRIX_MCFG) Single Access-The undefined length burst is treated as a succession of single accesses, allowing rearbitration at each beat of the INCR burst or bursts sequence. Position  */
#define MATRIX_MCFG_ULBT_4_BEAT               (MATRIX_MCFG_ULBT_4_BEAT_Val << MATRIX_MCFG_ULBT_Pos) /**< (MATRIX_MCFG) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing rearbitration every 4 beats. Position  */
#define MATRIX_MCFG_ULBT_8_BEAT               (MATRIX_MCFG_ULBT_8_BEAT_Val << MATRIX_MCFG_ULBT_Pos) /**< (MATRIX_MCFG) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing rearbitration every 8 beats. Position  */
#define MATRIX_MCFG_ULBT_16_BEAT              (MATRIX_MCFG_ULBT_16_BEAT_Val << MATRIX_MCFG_ULBT_Pos) /**< (MATRIX_MCFG) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing rearbitration every 16 beats. Position  */
#define MATRIX_MCFG_ULBT_32_BEAT              (MATRIX_MCFG_ULBT_32_BEAT_Val << MATRIX_MCFG_ULBT_Pos) /**< (MATRIX_MCFG) 32-beat Burst-The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing rearbitration every 32 beats. Position  */
#define MATRIX_MCFG_ULBT_64_BEAT              (MATRIX_MCFG_ULBT_64_BEAT_Val << MATRIX_MCFG_ULBT_Pos) /**< (MATRIX_MCFG) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing rearbitration every 64 beats. Position  */
#define MATRIX_MCFG_ULBT_128_BEAT             (MATRIX_MCFG_ULBT_128_BEAT_Val << MATRIX_MCFG_ULBT_Pos) /**< (MATRIX_MCFG) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing rearbitration every 128 beats.Unless duly needed, the ULBT should be left at its default 0 value for power saving. Position  */
#define MATRIX_MCFG_Msk                       _U_(0x00000007)                                      /**< (MATRIX_MCFG) Register Mask  */


/* -------- MATRIX_SCFG : (MATRIX Offset: 0x40) (R/W 32) Slave Configuration Register -------- */
#define MATRIX_SCFG_SLOT_CYCLE_Pos            _U_(0)                                               /**< (MATRIX_SCFG) Maximum Bus Grant Duration for Masters Position */
#define MATRIX_SCFG_SLOT_CYCLE_Msk            (_U_(0x1FF) << MATRIX_SCFG_SLOT_CYCLE_Pos)           /**< (MATRIX_SCFG) Maximum Bus Grant Duration for Masters Mask */
#define MATRIX_SCFG_SLOT_CYCLE(value)         (MATRIX_SCFG_SLOT_CYCLE_Msk & ((value) << MATRIX_SCFG_SLOT_CYCLE_Pos))
#define MATRIX_SCFG_DEFMSTR_TYPE_Pos          _U_(16)                                              /**< (MATRIX_SCFG) Default Master Type Position */
#define MATRIX_SCFG_DEFMSTR_TYPE_Msk          (_U_(0x3) << MATRIX_SCFG_DEFMSTR_TYPE_Pos)           /**< (MATRIX_SCFG) Default Master Type Mask */
#define MATRIX_SCFG_DEFMSTR_TYPE(value)       (MATRIX_SCFG_DEFMSTR_TYPE_Msk & ((value) << MATRIX_SCFG_DEFMSTR_TYPE_Pos))
#define   MATRIX_SCFG_DEFMSTR_TYPE_NONE_Val   _U_(0x0)                                             /**< (MATRIX_SCFG) No Default Master-At the end of the current slave access, if no other master request is pending, the slave is disconnected from all masters.This results in a one clock cycle latency for the first access of a burst transfer or for a single access.  */
#define   MATRIX_SCFG_DEFMSTR_TYPE_LAST_Val   _U_(0x1)                                             /**< (MATRIX_SCFG) Last Default Master-At the end of the current slave access, if no other master request is pending, the slave stays connected to the last master having accessed it.This results in not having one clock cycle latency when the last master tries to access the slave again.  */
#define   MATRIX_SCFG_DEFMSTR_TYPE_FIXED_Val  _U_(0x2)                                             /**< (MATRIX_SCFG) Fixed Default Master-At the end of the current slave access, if no other master request is pending, the slave connects to the fixed master the number that has been written in the FIXED_DEFMSTR field.This results in not having one clock cycle latency when the fixed master tries to access the slave again.  */
#define MATRIX_SCFG_DEFMSTR_TYPE_NONE         (MATRIX_SCFG_DEFMSTR_TYPE_NONE_Val << MATRIX_SCFG_DEFMSTR_TYPE_Pos) /**< (MATRIX_SCFG) No Default Master-At the end of the current slave access, if no other master request is pending, the slave is disconnected from all masters.This results in a one clock cycle latency for the first access of a burst transfer or for a single access. Position  */
#define MATRIX_SCFG_DEFMSTR_TYPE_LAST         (MATRIX_SCFG_DEFMSTR_TYPE_LAST_Val << MATRIX_SCFG_DEFMSTR_TYPE_Pos) /**< (MATRIX_SCFG) Last Default Master-At the end of the current slave access, if no other master request is pending, the slave stays connected to the last master having accessed it.This results in not having one clock cycle latency when the last master tries to access the slave again. Position  */
#define MATRIX_SCFG_DEFMSTR_TYPE_FIXED        (MATRIX_SCFG_DEFMSTR_TYPE_FIXED_Val << MATRIX_SCFG_DEFMSTR_TYPE_Pos) /**< (MATRIX_SCFG) Fixed Default Master-At the end of the current slave access, if no other master request is pending, the slave connects to the fixed master the number that has been written in the FIXED_DEFMSTR field.This results in not having one clock cycle latency when the fixed master tries to access the slave again. Position  */
#define MATRIX_SCFG_FIXED_DEFMSTR_Pos         _U_(18)                                              /**< (MATRIX_SCFG) Fixed Default Master Position */
#define MATRIX_SCFG_FIXED_DEFMSTR_Msk         (_U_(0xF) << MATRIX_SCFG_FIXED_DEFMSTR_Pos)          /**< (MATRIX_SCFG) Fixed Default Master Mask */
#define MATRIX_SCFG_FIXED_DEFMSTR(value)      (MATRIX_SCFG_FIXED_DEFMSTR_Msk & ((value) << MATRIX_SCFG_FIXED_DEFMSTR_Pos))
#define MATRIX_SCFG_Msk                       _U_(0x003F01FF)                                      /**< (MATRIX_SCFG) Register Mask  */


/* -------- MATRIX_MEIER : (MATRIX Offset: 0x150) ( /W 32) Master Error Interrupt Enable Register -------- */
#define MATRIX_MEIER_MERR0_Pos                _U_(0)                                               /**< (MATRIX_MEIER) Master 0 Access Error Position */
#define MATRIX_MEIER_MERR0_Msk                (_U_(0x1) << MATRIX_MEIER_MERR0_Pos)                 /**< (MATRIX_MEIER) Master 0 Access Error Mask */
#define MATRIX_MEIER_MERR0(value)             (MATRIX_MEIER_MERR0_Msk & ((value) << MATRIX_MEIER_MERR0_Pos))
#define MATRIX_MEIER_MERR1_Pos                _U_(1)                                               /**< (MATRIX_MEIER) Master 1 Access Error Position */
#define MATRIX_MEIER_MERR1_Msk                (_U_(0x1) << MATRIX_MEIER_MERR1_Pos)                 /**< (MATRIX_MEIER) Master 1 Access Error Mask */
#define MATRIX_MEIER_MERR1(value)             (MATRIX_MEIER_MERR1_Msk & ((value) << MATRIX_MEIER_MERR1_Pos))
#define MATRIX_MEIER_MERR2_Pos                _U_(2)                                               /**< (MATRIX_MEIER) Master 2 Access Error Position */
#define MATRIX_MEIER_MERR2_Msk                (_U_(0x1) << MATRIX_MEIER_MERR2_Pos)                 /**< (MATRIX_MEIER) Master 2 Access Error Mask */
#define MATRIX_MEIER_MERR2(value)             (MATRIX_MEIER_MERR2_Msk & ((value) << MATRIX_MEIER_MERR2_Pos))
#define MATRIX_MEIER_MERR3_Pos                _U_(3)                                               /**< (MATRIX_MEIER) Master 3 Access Error Position */
#define MATRIX_MEIER_MERR3_Msk                (_U_(0x1) << MATRIX_MEIER_MERR3_Pos)                 /**< (MATRIX_MEIER) Master 3 Access Error Mask */
#define MATRIX_MEIER_MERR3(value)             (MATRIX_MEIER_MERR3_Msk & ((value) << MATRIX_MEIER_MERR3_Pos))
#define MATRIX_MEIER_MERR4_Pos                _U_(4)                                               /**< (MATRIX_MEIER) Master 4 Access Error Position */
#define MATRIX_MEIER_MERR4_Msk                (_U_(0x1) << MATRIX_MEIER_MERR4_Pos)                 /**< (MATRIX_MEIER) Master 4 Access Error Mask */
#define MATRIX_MEIER_MERR4(value)             (MATRIX_MEIER_MERR4_Msk & ((value) << MATRIX_MEIER_MERR4_Pos))
#define MATRIX_MEIER_MERR5_Pos                _U_(5)                                               /**< (MATRIX_MEIER) Master 5 Access Error Position */
#define MATRIX_MEIER_MERR5_Msk                (_U_(0x1) << MATRIX_MEIER_MERR5_Pos)                 /**< (MATRIX_MEIER) Master 5 Access Error Mask */
#define MATRIX_MEIER_MERR5(value)             (MATRIX_MEIER_MERR5_Msk & ((value) << MATRIX_MEIER_MERR5_Pos))
#define MATRIX_MEIER_MERR6_Pos                _U_(6)                                               /**< (MATRIX_MEIER) Master 6 Access Error Position */
#define MATRIX_MEIER_MERR6_Msk                (_U_(0x1) << MATRIX_MEIER_MERR6_Pos)                 /**< (MATRIX_MEIER) Master 6 Access Error Mask */
#define MATRIX_MEIER_MERR6(value)             (MATRIX_MEIER_MERR6_Msk & ((value) << MATRIX_MEIER_MERR6_Pos))
#define MATRIX_MEIER_MERR7_Pos                _U_(7)                                               /**< (MATRIX_MEIER) Master 7 Access Error Position */
#define MATRIX_MEIER_MERR7_Msk                (_U_(0x1) << MATRIX_MEIER_MERR7_Pos)                 /**< (MATRIX_MEIER) Master 7 Access Error Mask */
#define MATRIX_MEIER_MERR7(value)             (MATRIX_MEIER_MERR7_Msk & ((value) << MATRIX_MEIER_MERR7_Pos))
#define MATRIX_MEIER_MERR8_Pos                _U_(8)                                               /**< (MATRIX_MEIER) Master 8 Access Error Position */
#define MATRIX_MEIER_MERR8_Msk                (_U_(0x1) << MATRIX_MEIER_MERR8_Pos)                 /**< (MATRIX_MEIER) Master 8 Access Error Mask */
#define MATRIX_MEIER_MERR8(value)             (MATRIX_MEIER_MERR8_Msk & ((value) << MATRIX_MEIER_MERR8_Pos))
#define MATRIX_MEIER_MERR9_Pos                _U_(9)                                               /**< (MATRIX_MEIER) Master 9 Access Error Position */
#define MATRIX_MEIER_MERR9_Msk                (_U_(0x1) << MATRIX_MEIER_MERR9_Pos)                 /**< (MATRIX_MEIER) Master 9 Access Error Mask */
#define MATRIX_MEIER_MERR9(value)             (MATRIX_MEIER_MERR9_Msk & ((value) << MATRIX_MEIER_MERR9_Pos))
#define MATRIX_MEIER_MERR10_Pos               _U_(10)                                              /**< (MATRIX_MEIER) Master 10 Access Error Position */
#define MATRIX_MEIER_MERR10_Msk               (_U_(0x1) << MATRIX_MEIER_MERR10_Pos)                /**< (MATRIX_MEIER) Master 10 Access Error Mask */
#define MATRIX_MEIER_MERR10(value)            (MATRIX_MEIER_MERR10_Msk & ((value) << MATRIX_MEIER_MERR10_Pos))
#define MATRIX_MEIER_MERR11_Pos               _U_(11)                                              /**< (MATRIX_MEIER) Master 11 Access Error Position */
#define MATRIX_MEIER_MERR11_Msk               (_U_(0x1) << MATRIX_MEIER_MERR11_Pos)                /**< (MATRIX_MEIER) Master 11 Access Error Mask */
#define MATRIX_MEIER_MERR11(value)            (MATRIX_MEIER_MERR11_Msk & ((value) << MATRIX_MEIER_MERR11_Pos))
#define MATRIX_MEIER_Msk                      _U_(0x00000FFF)                                      /**< (MATRIX_MEIER) Register Mask  */

#define MATRIX_MEIER_MERR_Pos                 _U_(0)                                               /**< (MATRIX_MEIER Position) Master xx Access Error */
#define MATRIX_MEIER_MERR_Msk                 (_U_(0xFFF) << MATRIX_MEIER_MERR_Pos)                /**< (MATRIX_MEIER Mask) MERR */
#define MATRIX_MEIER_MERR(value)              (MATRIX_MEIER_MERR_Msk & ((value) << MATRIX_MEIER_MERR_Pos)) 

/* -------- MATRIX_MEIDR : (MATRIX Offset: 0x154) ( /W 32) Master Error Interrupt Disable Register -------- */
#define MATRIX_MEIDR_MERR0_Pos                _U_(0)                                               /**< (MATRIX_MEIDR) Master 0 Access Error Position */
#define MATRIX_MEIDR_MERR0_Msk                (_U_(0x1) << MATRIX_MEIDR_MERR0_Pos)                 /**< (MATRIX_MEIDR) Master 0 Access Error Mask */
#define MATRIX_MEIDR_MERR0(value)             (MATRIX_MEIDR_MERR0_Msk & ((value) << MATRIX_MEIDR_MERR0_Pos))
#define MATRIX_MEIDR_MERR1_Pos                _U_(1)                                               /**< (MATRIX_MEIDR) Master 1 Access Error Position */
#define MATRIX_MEIDR_MERR1_Msk                (_U_(0x1) << MATRIX_MEIDR_MERR1_Pos)                 /**< (MATRIX_MEIDR) Master 1 Access Error Mask */
#define MATRIX_MEIDR_MERR1(value)             (MATRIX_MEIDR_MERR1_Msk & ((value) << MATRIX_MEIDR_MERR1_Pos))
#define MATRIX_MEIDR_MERR2_Pos                _U_(2)                                               /**< (MATRIX_MEIDR) Master 2 Access Error Position */
#define MATRIX_MEIDR_MERR2_Msk                (_U_(0x1) << MATRIX_MEIDR_MERR2_Pos)                 /**< (MATRIX_MEIDR) Master 2 Access Error Mask */
#define MATRIX_MEIDR_MERR2(value)             (MATRIX_MEIDR_MERR2_Msk & ((value) << MATRIX_MEIDR_MERR2_Pos))
#define MATRIX_MEIDR_MERR3_Pos                _U_(3)                                               /**< (MATRIX_MEIDR) Master 3 Access Error Position */
#define MATRIX_MEIDR_MERR3_Msk                (_U_(0x1) << MATRIX_MEIDR_MERR3_Pos)                 /**< (MATRIX_MEIDR) Master 3 Access Error Mask */
#define MATRIX_MEIDR_MERR3(value)             (MATRIX_MEIDR_MERR3_Msk & ((value) << MATRIX_MEIDR_MERR3_Pos))
#define MATRIX_MEIDR_MERR4_Pos                _U_(4)                                               /**< (MATRIX_MEIDR) Master 4 Access Error Position */
#define MATRIX_MEIDR_MERR4_Msk                (_U_(0x1) << MATRIX_MEIDR_MERR4_Pos)                 /**< (MATRIX_MEIDR) Master 4 Access Error Mask */
#define MATRIX_MEIDR_MERR4(value)             (MATRIX_MEIDR_MERR4_Msk & ((value) << MATRIX_MEIDR_MERR4_Pos))
#define MATRIX_MEIDR_MERR5_Pos                _U_(5)                                               /**< (MATRIX_MEIDR) Master 5 Access Error Position */
#define MATRIX_MEIDR_MERR5_Msk                (_U_(0x1) << MATRIX_MEIDR_MERR5_Pos)                 /**< (MATRIX_MEIDR) Master 5 Access Error Mask */
#define MATRIX_MEIDR_MERR5(value)             (MATRIX_MEIDR_MERR5_Msk & ((value) << MATRIX_MEIDR_MERR5_Pos))
#define MATRIX_MEIDR_MERR6_Pos                _U_(6)                                               /**< (MATRIX_MEIDR) Master 6 Access Error Position */
#define MATRIX_MEIDR_MERR6_Msk                (_U_(0x1) << MATRIX_MEIDR_MERR6_Pos)                 /**< (MATRIX_MEIDR) Master 6 Access Error Mask */
#define MATRIX_MEIDR_MERR6(value)             (MATRIX_MEIDR_MERR6_Msk & ((value) << MATRIX_MEIDR_MERR6_Pos))
#define MATRIX_MEIDR_MERR7_Pos                _U_(7)                                               /**< (MATRIX_MEIDR) Master 7 Access Error Position */
#define MATRIX_MEIDR_MERR7_Msk                (_U_(0x1) << MATRIX_MEIDR_MERR7_Pos)                 /**< (MATRIX_MEIDR) Master 7 Access Error Mask */
#define MATRIX_MEIDR_MERR7(value)             (MATRIX_MEIDR_MERR7_Msk & ((value) << MATRIX_MEIDR_MERR7_Pos))
#define MATRIX_MEIDR_MERR8_Pos                _U_(8)                                               /**< (MATRIX_MEIDR) Master 8 Access Error Position */
#define MATRIX_MEIDR_MERR8_Msk                (_U_(0x1) << MATRIX_MEIDR_MERR8_Pos)                 /**< (MATRIX_MEIDR) Master 8 Access Error Mask */
#define MATRIX_MEIDR_MERR8(value)             (MATRIX_MEIDR_MERR8_Msk & ((value) << MATRIX_MEIDR_MERR8_Pos))
#define MATRIX_MEIDR_MERR9_Pos                _U_(9)                                               /**< (MATRIX_MEIDR) Master 9 Access Error Position */
#define MATRIX_MEIDR_MERR9_Msk                (_U_(0x1) << MATRIX_MEIDR_MERR9_Pos)                 /**< (MATRIX_MEIDR) Master 9 Access Error Mask */
#define MATRIX_MEIDR_MERR9(value)             (MATRIX_MEIDR_MERR9_Msk & ((value) << MATRIX_MEIDR_MERR9_Pos))
#define MATRIX_MEIDR_MERR10_Pos               _U_(10)                                              /**< (MATRIX_MEIDR) Master 10 Access Error Position */
#define MATRIX_MEIDR_MERR10_Msk               (_U_(0x1) << MATRIX_MEIDR_MERR10_Pos)                /**< (MATRIX_MEIDR) Master 10 Access Error Mask */
#define MATRIX_MEIDR_MERR10(value)            (MATRIX_MEIDR_MERR10_Msk & ((value) << MATRIX_MEIDR_MERR10_Pos))
#define MATRIX_MEIDR_MERR11_Pos               _U_(11)                                              /**< (MATRIX_MEIDR) Master 11 Access Error Position */
#define MATRIX_MEIDR_MERR11_Msk               (_U_(0x1) << MATRIX_MEIDR_MERR11_Pos)                /**< (MATRIX_MEIDR) Master 11 Access Error Mask */
#define MATRIX_MEIDR_MERR11(value)            (MATRIX_MEIDR_MERR11_Msk & ((value) << MATRIX_MEIDR_MERR11_Pos))
#define MATRIX_MEIDR_Msk                      _U_(0x00000FFF)                                      /**< (MATRIX_MEIDR) Register Mask  */

#define MATRIX_MEIDR_MERR_Pos                 _U_(0)                                               /**< (MATRIX_MEIDR Position) Master xx Access Error */
#define MATRIX_MEIDR_MERR_Msk                 (_U_(0xFFF) << MATRIX_MEIDR_MERR_Pos)                /**< (MATRIX_MEIDR Mask) MERR */
#define MATRIX_MEIDR_MERR(value)              (MATRIX_MEIDR_MERR_Msk & ((value) << MATRIX_MEIDR_MERR_Pos)) 

/* -------- MATRIX_MEIMR : (MATRIX Offset: 0x158) ( R/ 32) Master Error Interrupt Mask Register -------- */
#define MATRIX_MEIMR_MERR0_Pos                _U_(0)                                               /**< (MATRIX_MEIMR) Master 0 Access Error Position */
#define MATRIX_MEIMR_MERR0_Msk                (_U_(0x1) << MATRIX_MEIMR_MERR0_Pos)                 /**< (MATRIX_MEIMR) Master 0 Access Error Mask */
#define MATRIX_MEIMR_MERR0(value)             (MATRIX_MEIMR_MERR0_Msk & ((value) << MATRIX_MEIMR_MERR0_Pos))
#define MATRIX_MEIMR_MERR1_Pos                _U_(1)                                               /**< (MATRIX_MEIMR) Master 1 Access Error Position */
#define MATRIX_MEIMR_MERR1_Msk                (_U_(0x1) << MATRIX_MEIMR_MERR1_Pos)                 /**< (MATRIX_MEIMR) Master 1 Access Error Mask */
#define MATRIX_MEIMR_MERR1(value)             (MATRIX_MEIMR_MERR1_Msk & ((value) << MATRIX_MEIMR_MERR1_Pos))
#define MATRIX_MEIMR_MERR2_Pos                _U_(2)                                               /**< (MATRIX_MEIMR) Master 2 Access Error Position */
#define MATRIX_MEIMR_MERR2_Msk                (_U_(0x1) << MATRIX_MEIMR_MERR2_Pos)                 /**< (MATRIX_MEIMR) Master 2 Access Error Mask */
#define MATRIX_MEIMR_MERR2(value)             (MATRIX_MEIMR_MERR2_Msk & ((value) << MATRIX_MEIMR_MERR2_Pos))
#define MATRIX_MEIMR_MERR3_Pos                _U_(3)                                               /**< (MATRIX_MEIMR) Master 3 Access Error Position */
#define MATRIX_MEIMR_MERR3_Msk                (_U_(0x1) << MATRIX_MEIMR_MERR3_Pos)                 /**< (MATRIX_MEIMR) Master 3 Access Error Mask */
#define MATRIX_MEIMR_MERR3(value)             (MATRIX_MEIMR_MERR3_Msk & ((value) << MATRIX_MEIMR_MERR3_Pos))
#define MATRIX_MEIMR_MERR4_Pos                _U_(4)                                               /**< (MATRIX_MEIMR) Master 4 Access Error Position */
#define MATRIX_MEIMR_MERR4_Msk                (_U_(0x1) << MATRIX_MEIMR_MERR4_Pos)                 /**< (MATRIX_MEIMR) Master 4 Access Error Mask */
#define MATRIX_MEIMR_MERR4(value)             (MATRIX_MEIMR_MERR4_Msk & ((value) << MATRIX_MEIMR_MERR4_Pos))
#define MATRIX_MEIMR_MERR5_Pos                _U_(5)                                               /**< (MATRIX_MEIMR) Master 5 Access Error Position */
#define MATRIX_MEIMR_MERR5_Msk                (_U_(0x1) << MATRIX_MEIMR_MERR5_Pos)                 /**< (MATRIX_MEIMR) Master 5 Access Error Mask */
#define MATRIX_MEIMR_MERR5(value)             (MATRIX_MEIMR_MERR5_Msk & ((value) << MATRIX_MEIMR_MERR5_Pos))
#define MATRIX_MEIMR_MERR6_Pos                _U_(6)                                               /**< (MATRIX_MEIMR) Master 6 Access Error Position */
#define MATRIX_MEIMR_MERR6_Msk                (_U_(0x1) << MATRIX_MEIMR_MERR6_Pos)                 /**< (MATRIX_MEIMR) Master 6 Access Error Mask */
#define MATRIX_MEIMR_MERR6(value)             (MATRIX_MEIMR_MERR6_Msk & ((value) << MATRIX_MEIMR_MERR6_Pos))
#define MATRIX_MEIMR_MERR7_Pos                _U_(7)                                               /**< (MATRIX_MEIMR) Master 7 Access Error Position */
#define MATRIX_MEIMR_MERR7_Msk                (_U_(0x1) << MATRIX_MEIMR_MERR7_Pos)                 /**< (MATRIX_MEIMR) Master 7 Access Error Mask */
#define MATRIX_MEIMR_MERR7(value)             (MATRIX_MEIMR_MERR7_Msk & ((value) << MATRIX_MEIMR_MERR7_Pos))
#define MATRIX_MEIMR_MERR8_Pos                _U_(8)                                               /**< (MATRIX_MEIMR) Master 8 Access Error Position */
#define MATRIX_MEIMR_MERR8_Msk                (_U_(0x1) << MATRIX_MEIMR_MERR8_Pos)                 /**< (MATRIX_MEIMR) Master 8 Access Error Mask */
#define MATRIX_MEIMR_MERR8(value)             (MATRIX_MEIMR_MERR8_Msk & ((value) << MATRIX_MEIMR_MERR8_Pos))
#define MATRIX_MEIMR_MERR9_Pos                _U_(9)                                               /**< (MATRIX_MEIMR) Master 9 Access Error Position */
#define MATRIX_MEIMR_MERR9_Msk                (_U_(0x1) << MATRIX_MEIMR_MERR9_Pos)                 /**< (MATRIX_MEIMR) Master 9 Access Error Mask */
#define MATRIX_MEIMR_MERR9(value)             (MATRIX_MEIMR_MERR9_Msk & ((value) << MATRIX_MEIMR_MERR9_Pos))
#define MATRIX_MEIMR_MERR10_Pos               _U_(10)                                              /**< (MATRIX_MEIMR) Master 10 Access Error Position */
#define MATRIX_MEIMR_MERR10_Msk               (_U_(0x1) << MATRIX_MEIMR_MERR10_Pos)                /**< (MATRIX_MEIMR) Master 10 Access Error Mask */
#define MATRIX_MEIMR_MERR10(value)            (MATRIX_MEIMR_MERR10_Msk & ((value) << MATRIX_MEIMR_MERR10_Pos))
#define MATRIX_MEIMR_MERR11_Pos               _U_(11)                                              /**< (MATRIX_MEIMR) Master 11 Access Error Position */
#define MATRIX_MEIMR_MERR11_Msk               (_U_(0x1) << MATRIX_MEIMR_MERR11_Pos)                /**< (MATRIX_MEIMR) Master 11 Access Error Mask */
#define MATRIX_MEIMR_MERR11(value)            (MATRIX_MEIMR_MERR11_Msk & ((value) << MATRIX_MEIMR_MERR11_Pos))
#define MATRIX_MEIMR_Msk                      _U_(0x00000FFF)                                      /**< (MATRIX_MEIMR) Register Mask  */

#define MATRIX_MEIMR_MERR_Pos                 _U_(0)                                               /**< (MATRIX_MEIMR Position) Master xx Access Error */
#define MATRIX_MEIMR_MERR_Msk                 (_U_(0xFFF) << MATRIX_MEIMR_MERR_Pos)                /**< (MATRIX_MEIMR Mask) MERR */
#define MATRIX_MEIMR_MERR(value)              (MATRIX_MEIMR_MERR_Msk & ((value) << MATRIX_MEIMR_MERR_Pos)) 

/* -------- MATRIX_MESR : (MATRIX Offset: 0x15C) ( R/ 32) Master Error Status Register -------- */
#define MATRIX_MESR_MERR0_Pos                 _U_(0)                                               /**< (MATRIX_MESR) Master 0 Access Error Position */
#define MATRIX_MESR_MERR0_Msk                 (_U_(0x1) << MATRIX_MESR_MERR0_Pos)                  /**< (MATRIX_MESR) Master 0 Access Error Mask */
#define MATRIX_MESR_MERR0(value)              (MATRIX_MESR_MERR0_Msk & ((value) << MATRIX_MESR_MERR0_Pos))
#define MATRIX_MESR_MERR1_Pos                 _U_(1)                                               /**< (MATRIX_MESR) Master 1 Access Error Position */
#define MATRIX_MESR_MERR1_Msk                 (_U_(0x1) << MATRIX_MESR_MERR1_Pos)                  /**< (MATRIX_MESR) Master 1 Access Error Mask */
#define MATRIX_MESR_MERR1(value)              (MATRIX_MESR_MERR1_Msk & ((value) << MATRIX_MESR_MERR1_Pos))
#define MATRIX_MESR_MERR2_Pos                 _U_(2)                                               /**< (MATRIX_MESR) Master 2 Access Error Position */
#define MATRIX_MESR_MERR2_Msk                 (_U_(0x1) << MATRIX_MESR_MERR2_Pos)                  /**< (MATRIX_MESR) Master 2 Access Error Mask */
#define MATRIX_MESR_MERR2(value)              (MATRIX_MESR_MERR2_Msk & ((value) << MATRIX_MESR_MERR2_Pos))
#define MATRIX_MESR_MERR3_Pos                 _U_(3)                                               /**< (MATRIX_MESR) Master 3 Access Error Position */
#define MATRIX_MESR_MERR3_Msk                 (_U_(0x1) << MATRIX_MESR_MERR3_Pos)                  /**< (MATRIX_MESR) Master 3 Access Error Mask */
#define MATRIX_MESR_MERR3(value)              (MATRIX_MESR_MERR3_Msk & ((value) << MATRIX_MESR_MERR3_Pos))
#define MATRIX_MESR_MERR4_Pos                 _U_(4)                                               /**< (MATRIX_MESR) Master 4 Access Error Position */
#define MATRIX_MESR_MERR4_Msk                 (_U_(0x1) << MATRIX_MESR_MERR4_Pos)                  /**< (MATRIX_MESR) Master 4 Access Error Mask */
#define MATRIX_MESR_MERR4(value)              (MATRIX_MESR_MERR4_Msk & ((value) << MATRIX_MESR_MERR4_Pos))
#define MATRIX_MESR_MERR5_Pos                 _U_(5)                                               /**< (MATRIX_MESR) Master 5 Access Error Position */
#define MATRIX_MESR_MERR5_Msk                 (_U_(0x1) << MATRIX_MESR_MERR5_Pos)                  /**< (MATRIX_MESR) Master 5 Access Error Mask */
#define MATRIX_MESR_MERR5(value)              (MATRIX_MESR_MERR5_Msk & ((value) << MATRIX_MESR_MERR5_Pos))
#define MATRIX_MESR_MERR6_Pos                 _U_(6)                                               /**< (MATRIX_MESR) Master 6 Access Error Position */
#define MATRIX_MESR_MERR6_Msk                 (_U_(0x1) << MATRIX_MESR_MERR6_Pos)                  /**< (MATRIX_MESR) Master 6 Access Error Mask */
#define MATRIX_MESR_MERR6(value)              (MATRIX_MESR_MERR6_Msk & ((value) << MATRIX_MESR_MERR6_Pos))
#define MATRIX_MESR_MERR7_Pos                 _U_(7)                                               /**< (MATRIX_MESR) Master 7 Access Error Position */
#define MATRIX_MESR_MERR7_Msk                 (_U_(0x1) << MATRIX_MESR_MERR7_Pos)                  /**< (MATRIX_MESR) Master 7 Access Error Mask */
#define MATRIX_MESR_MERR7(value)              (MATRIX_MESR_MERR7_Msk & ((value) << MATRIX_MESR_MERR7_Pos))
#define MATRIX_MESR_MERR8_Pos                 _U_(8)                                               /**< (MATRIX_MESR) Master 8 Access Error Position */
#define MATRIX_MESR_MERR8_Msk                 (_U_(0x1) << MATRIX_MESR_MERR8_Pos)                  /**< (MATRIX_MESR) Master 8 Access Error Mask */
#define MATRIX_MESR_MERR8(value)              (MATRIX_MESR_MERR8_Msk & ((value) << MATRIX_MESR_MERR8_Pos))
#define MATRIX_MESR_MERR9_Pos                 _U_(9)                                               /**< (MATRIX_MESR) Master 9 Access Error Position */
#define MATRIX_MESR_MERR9_Msk                 (_U_(0x1) << MATRIX_MESR_MERR9_Pos)                  /**< (MATRIX_MESR) Master 9 Access Error Mask */
#define MATRIX_MESR_MERR9(value)              (MATRIX_MESR_MERR9_Msk & ((value) << MATRIX_MESR_MERR9_Pos))
#define MATRIX_MESR_MERR10_Pos                _U_(10)                                              /**< (MATRIX_MESR) Master 10 Access Error Position */
#define MATRIX_MESR_MERR10_Msk                (_U_(0x1) << MATRIX_MESR_MERR10_Pos)                 /**< (MATRIX_MESR) Master 10 Access Error Mask */
#define MATRIX_MESR_MERR10(value)             (MATRIX_MESR_MERR10_Msk & ((value) << MATRIX_MESR_MERR10_Pos))
#define MATRIX_MESR_MERR11_Pos                _U_(11)                                              /**< (MATRIX_MESR) Master 11 Access Error Position */
#define MATRIX_MESR_MERR11_Msk                (_U_(0x1) << MATRIX_MESR_MERR11_Pos)                 /**< (MATRIX_MESR) Master 11 Access Error Mask */
#define MATRIX_MESR_MERR11(value)             (MATRIX_MESR_MERR11_Msk & ((value) << MATRIX_MESR_MERR11_Pos))
#define MATRIX_MESR_Msk                       _U_(0x00000FFF)                                      /**< (MATRIX_MESR) Register Mask  */

#define MATRIX_MESR_MERR_Pos                  _U_(0)                                               /**< (MATRIX_MESR Position) Master xx Access Error */
#define MATRIX_MESR_MERR_Msk                  (_U_(0xFFF) << MATRIX_MESR_MERR_Pos)                 /**< (MATRIX_MESR Mask) MERR */
#define MATRIX_MESR_MERR(value)               (MATRIX_MESR_MERR_Msk & ((value) << MATRIX_MESR_MERR_Pos)) 

/* -------- MATRIX_MEAR : (MATRIX Offset: 0x160) ( R/ 32) Master 0 Error Address Register -------- */
#define MATRIX_MEAR_ERRADD_Pos                _U_(0)                                               /**< (MATRIX_MEAR) Master Error Address Position */
#define MATRIX_MEAR_ERRADD_Msk                (_U_(0xFFFFFFFF) << MATRIX_MEAR_ERRADD_Pos)          /**< (MATRIX_MEAR) Master Error Address Mask */
#define MATRIX_MEAR_ERRADD(value)             (MATRIX_MEAR_ERRADD_Msk & ((value) << MATRIX_MEAR_ERRADD_Pos))
#define MATRIX_MEAR_Msk                       _U_(0xFFFFFFFF)                                      /**< (MATRIX_MEAR) Register Mask  */


/* -------- MATRIX_WPMR : (MATRIX Offset: 0x1E4) (R/W 32) Write Protection Mode Register -------- */
#define MATRIX_WPMR_WPEN_Pos                  _U_(0)                                               /**< (MATRIX_WPMR) Write Protection Enable Position */
#define MATRIX_WPMR_WPEN_Msk                  (_U_(0x1) << MATRIX_WPMR_WPEN_Pos)                   /**< (MATRIX_WPMR) Write Protection Enable Mask */
#define MATRIX_WPMR_WPEN(value)               (MATRIX_WPMR_WPEN_Msk & ((value) << MATRIX_WPMR_WPEN_Pos))
#define MATRIX_WPMR_WPKEY_Pos                 _U_(8)                                               /**< (MATRIX_WPMR) Write Protection Key (Write-only) Position */
#define MATRIX_WPMR_WPKEY_Msk                 (_U_(0xFFFFFF) << MATRIX_WPMR_WPKEY_Pos)             /**< (MATRIX_WPMR) Write Protection Key (Write-only) Mask */
#define MATRIX_WPMR_WPKEY(value)              (MATRIX_WPMR_WPKEY_Msk & ((value) << MATRIX_WPMR_WPKEY_Pos))
#define   MATRIX_WPMR_WPKEY_PASSWD_Val        _U_(0x4D4154)                                        /**< (MATRIX_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0.  */
#define MATRIX_WPMR_WPKEY_PASSWD              (MATRIX_WPMR_WPKEY_PASSWD_Val << MATRIX_WPMR_WPKEY_Pos) /**< (MATRIX_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0. Position  */
#define MATRIX_WPMR_Msk                       _U_(0xFFFFFF01)                                      /**< (MATRIX_WPMR) Register Mask  */


/* -------- MATRIX_WPSR : (MATRIX Offset: 0x1E8) ( R/ 32) Write Protection Status Register -------- */
#define MATRIX_WPSR_WPVS_Pos                  _U_(0)                                               /**< (MATRIX_WPSR) Write Protection Violation Status Position */
#define MATRIX_WPSR_WPVS_Msk                  (_U_(0x1) << MATRIX_WPSR_WPVS_Pos)                   /**< (MATRIX_WPSR) Write Protection Violation Status Mask */
#define MATRIX_WPSR_WPVS(value)               (MATRIX_WPSR_WPVS_Msk & ((value) << MATRIX_WPSR_WPVS_Pos))
#define MATRIX_WPSR_WPVSRC_Pos                _U_(8)                                               /**< (MATRIX_WPSR) Write Protection Violation Source Position */
#define MATRIX_WPSR_WPVSRC_Msk                (_U_(0xFFFF) << MATRIX_WPSR_WPVSRC_Pos)              /**< (MATRIX_WPSR) Write Protection Violation Source Mask */
#define MATRIX_WPSR_WPVSRC(value)             (MATRIX_WPSR_WPVSRC_Msk & ((value) << MATRIX_WPSR_WPVSRC_Pos))
#define MATRIX_WPSR_Msk                       _U_(0x00FFFF01)                                      /**< (MATRIX_WPSR) Register Mask  */


/* -------- MATRIX_SSR : (MATRIX Offset: 0x200) (R/W 32) Security Slave 0 Register -------- */
#define MATRIX_SSR_LANSECH0_Pos               _U_(0)                                               /**< (MATRIX_SSR) Low Area Non-secured in HSELx Security Region Position */
#define MATRIX_SSR_LANSECH0_Msk               (_U_(0x1) << MATRIX_SSR_LANSECH0_Pos)                /**< (MATRIX_SSR) Low Area Non-secured in HSELx Security Region Mask */
#define MATRIX_SSR_LANSECH0(value)            (MATRIX_SSR_LANSECH0_Msk & ((value) << MATRIX_SSR_LANSECH0_Pos))
#define MATRIX_SSR_LANSECH1_Pos               _U_(1)                                               /**< (MATRIX_SSR) Low Area Non-secured in HSELx Security Region Position */
#define MATRIX_SSR_LANSECH1_Msk               (_U_(0x1) << MATRIX_SSR_LANSECH1_Pos)                /**< (MATRIX_SSR) Low Area Non-secured in HSELx Security Region Mask */
#define MATRIX_SSR_LANSECH1(value)            (MATRIX_SSR_LANSECH1_Msk & ((value) << MATRIX_SSR_LANSECH1_Pos))
#define MATRIX_SSR_LANSECH2_Pos               _U_(2)                                               /**< (MATRIX_SSR) Low Area Non-secured in HSELx Security Region Position */
#define MATRIX_SSR_LANSECH2_Msk               (_U_(0x1) << MATRIX_SSR_LANSECH2_Pos)                /**< (MATRIX_SSR) Low Area Non-secured in HSELx Security Region Mask */
#define MATRIX_SSR_LANSECH2(value)            (MATRIX_SSR_LANSECH2_Msk & ((value) << MATRIX_SSR_LANSECH2_Pos))
#define MATRIX_SSR_LANSECH3_Pos               _U_(3)                                               /**< (MATRIX_SSR) Low Area Non-secured in HSELx Security Region Position */
#define MATRIX_SSR_LANSECH3_Msk               (_U_(0x1) << MATRIX_SSR_LANSECH3_Pos)                /**< (MATRIX_SSR) Low Area Non-secured in HSELx Security Region Mask */
#define MATRIX_SSR_LANSECH3(value)            (MATRIX_SSR_LANSECH3_Msk & ((value) << MATRIX_SSR_LANSECH3_Pos))
#define MATRIX_SSR_LANSECH4_Pos               _U_(4)                                               /**< (MATRIX_SSR) Low Area Non-secured in HSELx Security Region Position */
#define MATRIX_SSR_LANSECH4_Msk               (_U_(0x1) << MATRIX_SSR_LANSECH4_Pos)                /**< (MATRIX_SSR) Low Area Non-secured in HSELx Security Region Mask */
#define MATRIX_SSR_LANSECH4(value)            (MATRIX_SSR_LANSECH4_Msk & ((value) << MATRIX_SSR_LANSECH4_Pos))
#define MATRIX_SSR_LANSECH5_Pos               _U_(5)                                               /**< (MATRIX_SSR) Low Area Non-secured in HSELx Security Region Position */
#define MATRIX_SSR_LANSECH5_Msk               (_U_(0x1) << MATRIX_SSR_LANSECH5_Pos)                /**< (MATRIX_SSR) Low Area Non-secured in HSELx Security Region Mask */
#define MATRIX_SSR_LANSECH5(value)            (MATRIX_SSR_LANSECH5_Msk & ((value) << MATRIX_SSR_LANSECH5_Pos))
#define MATRIX_SSR_LANSECH6_Pos               _U_(6)                                               /**< (MATRIX_SSR) Low Area Non-secured in HSELx Security Region Position */
#define MATRIX_SSR_LANSECH6_Msk               (_U_(0x1) << MATRIX_SSR_LANSECH6_Pos)                /**< (MATRIX_SSR) Low Area Non-secured in HSELx Security Region Mask */
#define MATRIX_SSR_LANSECH6(value)            (MATRIX_SSR_LANSECH6_Msk & ((value) << MATRIX_SSR_LANSECH6_Pos))
#define MATRIX_SSR_LANSECH7_Pos               _U_(7)                                               /**< (MATRIX_SSR) Low Area Non-secured in HSELx Security Region Position */
#define MATRIX_SSR_LANSECH7_Msk               (_U_(0x1) << MATRIX_SSR_LANSECH7_Pos)                /**< (MATRIX_SSR) Low Area Non-secured in HSELx Security Region Mask */
#define MATRIX_SSR_LANSECH7(value)            (MATRIX_SSR_LANSECH7_Msk & ((value) << MATRIX_SSR_LANSECH7_Pos))
#define MATRIX_SSR_RDNSECH0_Pos               _U_(8)                                               /**< (MATRIX_SSR) Read Non-secured for HSELx Security Region Position */
#define MATRIX_SSR_RDNSECH0_Msk               (_U_(0x1) << MATRIX_SSR_RDNSECH0_Pos)                /**< (MATRIX_SSR) Read Non-secured for HSELx Security Region Mask */
#define MATRIX_SSR_RDNSECH0(value)            (MATRIX_SSR_RDNSECH0_Msk & ((value) << MATRIX_SSR_RDNSECH0_Pos))
#define MATRIX_SSR_RDNSECH1_Pos               _U_(9)                                               /**< (MATRIX_SSR) Read Non-secured for HSELx Security Region Position */
#define MATRIX_SSR_RDNSECH1_Msk               (_U_(0x1) << MATRIX_SSR_RDNSECH1_Pos)                /**< (MATRIX_SSR) Read Non-secured for HSELx Security Region Mask */
#define MATRIX_SSR_RDNSECH1(value)            (MATRIX_SSR_RDNSECH1_Msk & ((value) << MATRIX_SSR_RDNSECH1_Pos))
#define MATRIX_SSR_RDNSECH2_Pos               _U_(10)                                              /**< (MATRIX_SSR) Read Non-secured for HSELx Security Region Position */
#define MATRIX_SSR_RDNSECH2_Msk               (_U_(0x1) << MATRIX_SSR_RDNSECH2_Pos)                /**< (MATRIX_SSR) Read Non-secured for HSELx Security Region Mask */
#define MATRIX_SSR_RDNSECH2(value)            (MATRIX_SSR_RDNSECH2_Msk & ((value) << MATRIX_SSR_RDNSECH2_Pos))
#define MATRIX_SSR_RDNSECH3_Pos               _U_(11)                                              /**< (MATRIX_SSR) Read Non-secured for HSELx Security Region Position */
#define MATRIX_SSR_RDNSECH3_Msk               (_U_(0x1) << MATRIX_SSR_RDNSECH3_Pos)                /**< (MATRIX_SSR) Read Non-secured for HSELx Security Region Mask */
#define MATRIX_SSR_RDNSECH3(value)            (MATRIX_SSR_RDNSECH3_Msk & ((value) << MATRIX_SSR_RDNSECH3_Pos))
#define MATRIX_SSR_RDNSECH4_Pos               _U_(12)                                              /**< (MATRIX_SSR) Read Non-secured for HSELx Security Region Position */
#define MATRIX_SSR_RDNSECH4_Msk               (_U_(0x1) << MATRIX_SSR_RDNSECH4_Pos)                /**< (MATRIX_SSR) Read Non-secured for HSELx Security Region Mask */
#define MATRIX_SSR_RDNSECH4(value)            (MATRIX_SSR_RDNSECH4_Msk & ((value) << MATRIX_SSR_RDNSECH4_Pos))
#define MATRIX_SSR_RDNSECH5_Pos               _U_(13)                                              /**< (MATRIX_SSR) Read Non-secured for HSELx Security Region Position */
#define MATRIX_SSR_RDNSECH5_Msk               (_U_(0x1) << MATRIX_SSR_RDNSECH5_Pos)                /**< (MATRIX_SSR) Read Non-secured for HSELx Security Region Mask */
#define MATRIX_SSR_RDNSECH5(value)            (MATRIX_SSR_RDNSECH5_Msk & ((value) << MATRIX_SSR_RDNSECH5_Pos))
#define MATRIX_SSR_RDNSECH6_Pos               _U_(14)                                              /**< (MATRIX_SSR) Read Non-secured for HSELx Security Region Position */
#define MATRIX_SSR_RDNSECH6_Msk               (_U_(0x1) << MATRIX_SSR_RDNSECH6_Pos)                /**< (MATRIX_SSR) Read Non-secured for HSELx Security Region Mask */
#define MATRIX_SSR_RDNSECH6(value)            (MATRIX_SSR_RDNSECH6_Msk & ((value) << MATRIX_SSR_RDNSECH6_Pos))
#define MATRIX_SSR_RDNSECH7_Pos               _U_(15)                                              /**< (MATRIX_SSR) Read Non-secured for HSELx Security Region Position */
#define MATRIX_SSR_RDNSECH7_Msk               (_U_(0x1) << MATRIX_SSR_RDNSECH7_Pos)                /**< (MATRIX_SSR) Read Non-secured for HSELx Security Region Mask */
#define MATRIX_SSR_RDNSECH7(value)            (MATRIX_SSR_RDNSECH7_Msk & ((value) << MATRIX_SSR_RDNSECH7_Pos))
#define MATRIX_SSR_WRNSECH0_Pos               _U_(16)                                              /**< (MATRIX_SSR) Write Non-secured for HSELx Security Region Position */
#define MATRIX_SSR_WRNSECH0_Msk               (_U_(0x1) << MATRIX_SSR_WRNSECH0_Pos)                /**< (MATRIX_SSR) Write Non-secured for HSELx Security Region Mask */
#define MATRIX_SSR_WRNSECH0(value)            (MATRIX_SSR_WRNSECH0_Msk & ((value) << MATRIX_SSR_WRNSECH0_Pos))
#define MATRIX_SSR_WRNSECH1_Pos               _U_(17)                                              /**< (MATRIX_SSR) Write Non-secured for HSELx Security Region Position */
#define MATRIX_SSR_WRNSECH1_Msk               (_U_(0x1) << MATRIX_SSR_WRNSECH1_Pos)                /**< (MATRIX_SSR) Write Non-secured for HSELx Security Region Mask */
#define MATRIX_SSR_WRNSECH1(value)            (MATRIX_SSR_WRNSECH1_Msk & ((value) << MATRIX_SSR_WRNSECH1_Pos))
#define MATRIX_SSR_WRNSECH2_Pos               _U_(18)                                              /**< (MATRIX_SSR) Write Non-secured for HSELx Security Region Position */
#define MATRIX_SSR_WRNSECH2_Msk               (_U_(0x1) << MATRIX_SSR_WRNSECH2_Pos)                /**< (MATRIX_SSR) Write Non-secured for HSELx Security Region Mask */
#define MATRIX_SSR_WRNSECH2(value)            (MATRIX_SSR_WRNSECH2_Msk & ((value) << MATRIX_SSR_WRNSECH2_Pos))
#define MATRIX_SSR_WRNSECH3_Pos               _U_(19)                                              /**< (MATRIX_SSR) Write Non-secured for HSELx Security Region Position */
#define MATRIX_SSR_WRNSECH3_Msk               (_U_(0x1) << MATRIX_SSR_WRNSECH3_Pos)                /**< (MATRIX_SSR) Write Non-secured for HSELx Security Region Mask */
#define MATRIX_SSR_WRNSECH3(value)            (MATRIX_SSR_WRNSECH3_Msk & ((value) << MATRIX_SSR_WRNSECH3_Pos))
#define MATRIX_SSR_WRNSECH4_Pos               _U_(20)                                              /**< (MATRIX_SSR) Write Non-secured for HSELx Security Region Position */
#define MATRIX_SSR_WRNSECH4_Msk               (_U_(0x1) << MATRIX_SSR_WRNSECH4_Pos)                /**< (MATRIX_SSR) Write Non-secured for HSELx Security Region Mask */
#define MATRIX_SSR_WRNSECH4(value)            (MATRIX_SSR_WRNSECH4_Msk & ((value) << MATRIX_SSR_WRNSECH4_Pos))
#define MATRIX_SSR_WRNSECH5_Pos               _U_(21)                                              /**< (MATRIX_SSR) Write Non-secured for HSELx Security Region Position */
#define MATRIX_SSR_WRNSECH5_Msk               (_U_(0x1) << MATRIX_SSR_WRNSECH5_Pos)                /**< (MATRIX_SSR) Write Non-secured for HSELx Security Region Mask */
#define MATRIX_SSR_WRNSECH5(value)            (MATRIX_SSR_WRNSECH5_Msk & ((value) << MATRIX_SSR_WRNSECH5_Pos))
#define MATRIX_SSR_WRNSECH6_Pos               _U_(22)                                              /**< (MATRIX_SSR) Write Non-secured for HSELx Security Region Position */
#define MATRIX_SSR_WRNSECH6_Msk               (_U_(0x1) << MATRIX_SSR_WRNSECH6_Pos)                /**< (MATRIX_SSR) Write Non-secured for HSELx Security Region Mask */
#define MATRIX_SSR_WRNSECH6(value)            (MATRIX_SSR_WRNSECH6_Msk & ((value) << MATRIX_SSR_WRNSECH6_Pos))
#define MATRIX_SSR_WRNSECH7_Pos               _U_(23)                                              /**< (MATRIX_SSR) Write Non-secured for HSELx Security Region Position */
#define MATRIX_SSR_WRNSECH7_Msk               (_U_(0x1) << MATRIX_SSR_WRNSECH7_Pos)                /**< (MATRIX_SSR) Write Non-secured for HSELx Security Region Mask */
#define MATRIX_SSR_WRNSECH7(value)            (MATRIX_SSR_WRNSECH7_Msk & ((value) << MATRIX_SSR_WRNSECH7_Pos))
#define MATRIX_SSR_Msk                        _U_(0x00FFFFFF)                                      /**< (MATRIX_SSR) Register Mask  */

#define MATRIX_SSR_LANSECH_Pos                _U_(0)                                               /**< (MATRIX_SSR Position) Low Area Non-secured in HSELx Security Region */
#define MATRIX_SSR_LANSECH_Msk                (_U_(0xFF) << MATRIX_SSR_LANSECH_Pos)                /**< (MATRIX_SSR Mask) LANSECH */
#define MATRIX_SSR_LANSECH(value)             (MATRIX_SSR_LANSECH_Msk & ((value) << MATRIX_SSR_LANSECH_Pos)) 
#define MATRIX_SSR_RDNSECH_Pos                _U_(8)                                               /**< (MATRIX_SSR Position) Read Non-secured for HSELx Security Region */
#define MATRIX_SSR_RDNSECH_Msk                (_U_(0xFF) << MATRIX_SSR_RDNSECH_Pos)                /**< (MATRIX_SSR Mask) RDNSECH */
#define MATRIX_SSR_RDNSECH(value)             (MATRIX_SSR_RDNSECH_Msk & ((value) << MATRIX_SSR_RDNSECH_Pos)) 
#define MATRIX_SSR_WRNSECH_Pos                _U_(16)                                              /**< (MATRIX_SSR Position) Write Non-secured for HSELx Security Region */
#define MATRIX_SSR_WRNSECH_Msk                (_U_(0xFF) << MATRIX_SSR_WRNSECH_Pos)                /**< (MATRIX_SSR Mask) WRNSECH */
#define MATRIX_SSR_WRNSECH(value)             (MATRIX_SSR_WRNSECH_Msk & ((value) << MATRIX_SSR_WRNSECH_Pos)) 

/* -------- MATRIX_SASSR : (MATRIX Offset: 0x240) (R/W 32) Security Areas Split Slave 0 Register -------- */
#define MATRIX_SASSR_SASPLIT0_Pos             _U_(0)                                               /**< (MATRIX_SASSR) Security Areas Split for HSELx Security Region Position */
#define MATRIX_SASSR_SASPLIT0_Msk             (_U_(0xF) << MATRIX_SASSR_SASPLIT0_Pos)              /**< (MATRIX_SASSR) Security Areas Split for HSELx Security Region Mask */
#define MATRIX_SASSR_SASPLIT0(value)          (MATRIX_SASSR_SASPLIT0_Msk & ((value) << MATRIX_SASSR_SASPLIT0_Pos))
#define MATRIX_SASSR_SASPLIT1_Pos             _U_(4)                                               /**< (MATRIX_SASSR) Security Areas Split for HSELx Security Region Position */
#define MATRIX_SASSR_SASPLIT1_Msk             (_U_(0xF) << MATRIX_SASSR_SASPLIT1_Pos)              /**< (MATRIX_SASSR) Security Areas Split for HSELx Security Region Mask */
#define MATRIX_SASSR_SASPLIT1(value)          (MATRIX_SASSR_SASPLIT1_Msk & ((value) << MATRIX_SASSR_SASPLIT1_Pos))
#define MATRIX_SASSR_SASPLIT2_Pos             _U_(8)                                               /**< (MATRIX_SASSR) Security Areas Split for HSELx Security Region Position */
#define MATRIX_SASSR_SASPLIT2_Msk             (_U_(0xF) << MATRIX_SASSR_SASPLIT2_Pos)              /**< (MATRIX_SASSR) Security Areas Split for HSELx Security Region Mask */
#define MATRIX_SASSR_SASPLIT2(value)          (MATRIX_SASSR_SASPLIT2_Msk & ((value) << MATRIX_SASSR_SASPLIT2_Pos))
#define MATRIX_SASSR_SASPLIT3_Pos             _U_(12)                                              /**< (MATRIX_SASSR) Security Areas Split for HSELx Security Region Position */
#define MATRIX_SASSR_SASPLIT3_Msk             (_U_(0xF) << MATRIX_SASSR_SASPLIT3_Pos)              /**< (MATRIX_SASSR) Security Areas Split for HSELx Security Region Mask */
#define MATRIX_SASSR_SASPLIT3(value)          (MATRIX_SASSR_SASPLIT3_Msk & ((value) << MATRIX_SASSR_SASPLIT3_Pos))
#define MATRIX_SASSR_SASPLIT4_Pos             _U_(16)                                              /**< (MATRIX_SASSR) Security Areas Split for HSELx Security Region Position */
#define MATRIX_SASSR_SASPLIT4_Msk             (_U_(0xF) << MATRIX_SASSR_SASPLIT4_Pos)              /**< (MATRIX_SASSR) Security Areas Split for HSELx Security Region Mask */
#define MATRIX_SASSR_SASPLIT4(value)          (MATRIX_SASSR_SASPLIT4_Msk & ((value) << MATRIX_SASSR_SASPLIT4_Pos))
#define MATRIX_SASSR_SASPLIT5_Pos             _U_(20)                                              /**< (MATRIX_SASSR) Security Areas Split for HSELx Security Region Position */
#define MATRIX_SASSR_SASPLIT5_Msk             (_U_(0xF) << MATRIX_SASSR_SASPLIT5_Pos)              /**< (MATRIX_SASSR) Security Areas Split for HSELx Security Region Mask */
#define MATRIX_SASSR_SASPLIT5(value)          (MATRIX_SASSR_SASPLIT5_Msk & ((value) << MATRIX_SASSR_SASPLIT5_Pos))
#define MATRIX_SASSR_SASPLIT6_Pos             _U_(24)                                              /**< (MATRIX_SASSR) Security Areas Split for HSELx Security Region Position */
#define MATRIX_SASSR_SASPLIT6_Msk             (_U_(0xF) << MATRIX_SASSR_SASPLIT6_Pos)              /**< (MATRIX_SASSR) Security Areas Split for HSELx Security Region Mask */
#define MATRIX_SASSR_SASPLIT6(value)          (MATRIX_SASSR_SASPLIT6_Msk & ((value) << MATRIX_SASSR_SASPLIT6_Pos))
#define MATRIX_SASSR_SASPLIT7_Pos             _U_(28)                                              /**< (MATRIX_SASSR) Security Areas Split for HSELx Security Region Position */
#define MATRIX_SASSR_SASPLIT7_Msk             (_U_(0xF) << MATRIX_SASSR_SASPLIT7_Pos)              /**< (MATRIX_SASSR) Security Areas Split for HSELx Security Region Mask */
#define MATRIX_SASSR_SASPLIT7(value)          (MATRIX_SASSR_SASPLIT7_Msk & ((value) << MATRIX_SASSR_SASPLIT7_Pos))
#define MATRIX_SASSR_Msk                      _U_(0xFFFFFFFF)                                      /**< (MATRIX_SASSR) Register Mask  */


/* -------- MATRIX_SRTSR : (MATRIX Offset: 0x284) (R/W 32) Security Region Top Slave 1 Register -------- */
#define MATRIX_SRTSR_SRTOP0_Pos               _U_(0)                                               /**< (MATRIX_SRTSR) HSELx Security Region Top Position */
#define MATRIX_SRTSR_SRTOP0_Msk               (_U_(0xF) << MATRIX_SRTSR_SRTOP0_Pos)                /**< (MATRIX_SRTSR) HSELx Security Region Top Mask */
#define MATRIX_SRTSR_SRTOP0(value)            (MATRIX_SRTSR_SRTOP0_Msk & ((value) << MATRIX_SRTSR_SRTOP0_Pos))
#define MATRIX_SRTSR_SRTOP1_Pos               _U_(4)                                               /**< (MATRIX_SRTSR) HSELx Security Region Top Position */
#define MATRIX_SRTSR_SRTOP1_Msk               (_U_(0xF) << MATRIX_SRTSR_SRTOP1_Pos)                /**< (MATRIX_SRTSR) HSELx Security Region Top Mask */
#define MATRIX_SRTSR_SRTOP1(value)            (MATRIX_SRTSR_SRTOP1_Msk & ((value) << MATRIX_SRTSR_SRTOP1_Pos))
#define MATRIX_SRTSR_SRTOP2_Pos               _U_(8)                                               /**< (MATRIX_SRTSR) HSELx Security Region Top Position */
#define MATRIX_SRTSR_SRTOP2_Msk               (_U_(0xF) << MATRIX_SRTSR_SRTOP2_Pos)                /**< (MATRIX_SRTSR) HSELx Security Region Top Mask */
#define MATRIX_SRTSR_SRTOP2(value)            (MATRIX_SRTSR_SRTOP2_Msk & ((value) << MATRIX_SRTSR_SRTOP2_Pos))
#define MATRIX_SRTSR_SRTOP3_Pos               _U_(12)                                              /**< (MATRIX_SRTSR) HSELx Security Region Top Position */
#define MATRIX_SRTSR_SRTOP3_Msk               (_U_(0xF) << MATRIX_SRTSR_SRTOP3_Pos)                /**< (MATRIX_SRTSR) HSELx Security Region Top Mask */
#define MATRIX_SRTSR_SRTOP3(value)            (MATRIX_SRTSR_SRTOP3_Msk & ((value) << MATRIX_SRTSR_SRTOP3_Pos))
#define MATRIX_SRTSR_SRTOP4_Pos               _U_(16)                                              /**< (MATRIX_SRTSR) HSELx Security Region Top Position */
#define MATRIX_SRTSR_SRTOP4_Msk               (_U_(0xF) << MATRIX_SRTSR_SRTOP4_Pos)                /**< (MATRIX_SRTSR) HSELx Security Region Top Mask */
#define MATRIX_SRTSR_SRTOP4(value)            (MATRIX_SRTSR_SRTOP4_Msk & ((value) << MATRIX_SRTSR_SRTOP4_Pos))
#define MATRIX_SRTSR_SRTOP5_Pos               _U_(20)                                              /**< (MATRIX_SRTSR) HSELx Security Region Top Position */
#define MATRIX_SRTSR_SRTOP5_Msk               (_U_(0xF) << MATRIX_SRTSR_SRTOP5_Pos)                /**< (MATRIX_SRTSR) HSELx Security Region Top Mask */
#define MATRIX_SRTSR_SRTOP5(value)            (MATRIX_SRTSR_SRTOP5_Msk & ((value) << MATRIX_SRTSR_SRTOP5_Pos))
#define MATRIX_SRTSR_SRTOP6_Pos               _U_(24)                                              /**< (MATRIX_SRTSR) HSELx Security Region Top Position */
#define MATRIX_SRTSR_SRTOP6_Msk               (_U_(0xF) << MATRIX_SRTSR_SRTOP6_Pos)                /**< (MATRIX_SRTSR) HSELx Security Region Top Mask */
#define MATRIX_SRTSR_SRTOP6(value)            (MATRIX_SRTSR_SRTOP6_Msk & ((value) << MATRIX_SRTSR_SRTOP6_Pos))
#define MATRIX_SRTSR_SRTOP7_Pos               _U_(28)                                              /**< (MATRIX_SRTSR) HSELx Security Region Top Position */
#define MATRIX_SRTSR_SRTOP7_Msk               (_U_(0xF) << MATRIX_SRTSR_SRTOP7_Pos)                /**< (MATRIX_SRTSR) HSELx Security Region Top Mask */
#define MATRIX_SRTSR_SRTOP7(value)            (MATRIX_SRTSR_SRTOP7_Msk & ((value) << MATRIX_SRTSR_SRTOP7_Pos))
#define MATRIX_SRTSR_Msk                      _U_(0xFFFFFFFF)                                      /**< (MATRIX_SRTSR) Register Mask  */


/* -------- MATRIX_SPSELR : (MATRIX Offset: 0x2C0) (R/W 32) Security Peripheral Select 1 Register -------- */
#define MATRIX_SPSELR_NSECP0_Pos              _U_(0)                                               /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP0_Msk              (_U_(0x1) << MATRIX_SPSELR_NSECP0_Pos)               /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP0(value)           (MATRIX_SPSELR_NSECP0_Msk & ((value) << MATRIX_SPSELR_NSECP0_Pos))
#define MATRIX_SPSELR_NSECP1_Pos              _U_(1)                                               /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP1_Msk              (_U_(0x1) << MATRIX_SPSELR_NSECP1_Pos)               /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP1(value)           (MATRIX_SPSELR_NSECP1_Msk & ((value) << MATRIX_SPSELR_NSECP1_Pos))
#define MATRIX_SPSELR_NSECP2_Pos              _U_(2)                                               /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP2_Msk              (_U_(0x1) << MATRIX_SPSELR_NSECP2_Pos)               /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP2(value)           (MATRIX_SPSELR_NSECP2_Msk & ((value) << MATRIX_SPSELR_NSECP2_Pos))
#define MATRIX_SPSELR_NSECP3_Pos              _U_(3)                                               /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP3_Msk              (_U_(0x1) << MATRIX_SPSELR_NSECP3_Pos)               /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP3(value)           (MATRIX_SPSELR_NSECP3_Msk & ((value) << MATRIX_SPSELR_NSECP3_Pos))
#define MATRIX_SPSELR_NSECP4_Pos              _U_(4)                                               /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP4_Msk              (_U_(0x1) << MATRIX_SPSELR_NSECP4_Pos)               /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP4(value)           (MATRIX_SPSELR_NSECP4_Msk & ((value) << MATRIX_SPSELR_NSECP4_Pos))
#define MATRIX_SPSELR_NSECP5_Pos              _U_(5)                                               /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP5_Msk              (_U_(0x1) << MATRIX_SPSELR_NSECP5_Pos)               /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP5(value)           (MATRIX_SPSELR_NSECP5_Msk & ((value) << MATRIX_SPSELR_NSECP5_Pos))
#define MATRIX_SPSELR_NSECP6_Pos              _U_(6)                                               /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP6_Msk              (_U_(0x1) << MATRIX_SPSELR_NSECP6_Pos)               /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP6(value)           (MATRIX_SPSELR_NSECP6_Msk & ((value) << MATRIX_SPSELR_NSECP6_Pos))
#define MATRIX_SPSELR_NSECP7_Pos              _U_(7)                                               /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP7_Msk              (_U_(0x1) << MATRIX_SPSELR_NSECP7_Pos)               /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP7(value)           (MATRIX_SPSELR_NSECP7_Msk & ((value) << MATRIX_SPSELR_NSECP7_Pos))
#define MATRIX_SPSELR_NSECP8_Pos              _U_(8)                                               /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP8_Msk              (_U_(0x1) << MATRIX_SPSELR_NSECP8_Pos)               /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP8(value)           (MATRIX_SPSELR_NSECP8_Msk & ((value) << MATRIX_SPSELR_NSECP8_Pos))
#define MATRIX_SPSELR_NSECP9_Pos              _U_(9)                                               /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP9_Msk              (_U_(0x1) << MATRIX_SPSELR_NSECP9_Pos)               /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP9(value)           (MATRIX_SPSELR_NSECP9_Msk & ((value) << MATRIX_SPSELR_NSECP9_Pos))
#define MATRIX_SPSELR_NSECP10_Pos             _U_(10)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP10_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP10_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP10(value)          (MATRIX_SPSELR_NSECP10_Msk & ((value) << MATRIX_SPSELR_NSECP10_Pos))
#define MATRIX_SPSELR_NSECP11_Pos             _U_(11)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP11_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP11_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP11(value)          (MATRIX_SPSELR_NSECP11_Msk & ((value) << MATRIX_SPSELR_NSECP11_Pos))
#define MATRIX_SPSELR_NSECP12_Pos             _U_(12)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP12_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP12_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP12(value)          (MATRIX_SPSELR_NSECP12_Msk & ((value) << MATRIX_SPSELR_NSECP12_Pos))
#define MATRIX_SPSELR_NSECP13_Pos             _U_(13)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP13_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP13_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP13(value)          (MATRIX_SPSELR_NSECP13_Msk & ((value) << MATRIX_SPSELR_NSECP13_Pos))
#define MATRIX_SPSELR_NSECP14_Pos             _U_(14)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP14_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP14_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP14(value)          (MATRIX_SPSELR_NSECP14_Msk & ((value) << MATRIX_SPSELR_NSECP14_Pos))
#define MATRIX_SPSELR_NSECP15_Pos             _U_(15)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP15_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP15_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP15(value)          (MATRIX_SPSELR_NSECP15_Msk & ((value) << MATRIX_SPSELR_NSECP15_Pos))
#define MATRIX_SPSELR_NSECP16_Pos             _U_(16)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP16_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP16_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP16(value)          (MATRIX_SPSELR_NSECP16_Msk & ((value) << MATRIX_SPSELR_NSECP16_Pos))
#define MATRIX_SPSELR_NSECP17_Pos             _U_(17)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP17_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP17_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP17(value)          (MATRIX_SPSELR_NSECP17_Msk & ((value) << MATRIX_SPSELR_NSECP17_Pos))
#define MATRIX_SPSELR_NSECP18_Pos             _U_(18)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP18_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP18_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP18(value)          (MATRIX_SPSELR_NSECP18_Msk & ((value) << MATRIX_SPSELR_NSECP18_Pos))
#define MATRIX_SPSELR_NSECP19_Pos             _U_(19)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP19_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP19_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP19(value)          (MATRIX_SPSELR_NSECP19_Msk & ((value) << MATRIX_SPSELR_NSECP19_Pos))
#define MATRIX_SPSELR_NSECP20_Pos             _U_(20)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP20_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP20_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP20(value)          (MATRIX_SPSELR_NSECP20_Msk & ((value) << MATRIX_SPSELR_NSECP20_Pos))
#define MATRIX_SPSELR_NSECP21_Pos             _U_(21)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP21_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP21_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP21(value)          (MATRIX_SPSELR_NSECP21_Msk & ((value) << MATRIX_SPSELR_NSECP21_Pos))
#define MATRIX_SPSELR_NSECP22_Pos             _U_(22)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP22_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP22_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP22(value)          (MATRIX_SPSELR_NSECP22_Msk & ((value) << MATRIX_SPSELR_NSECP22_Pos))
#define MATRIX_SPSELR_NSECP23_Pos             _U_(23)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP23_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP23_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP23(value)          (MATRIX_SPSELR_NSECP23_Msk & ((value) << MATRIX_SPSELR_NSECP23_Pos))
#define MATRIX_SPSELR_NSECP24_Pos             _U_(24)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP24_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP24_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP24(value)          (MATRIX_SPSELR_NSECP24_Msk & ((value) << MATRIX_SPSELR_NSECP24_Pos))
#define MATRIX_SPSELR_NSECP25_Pos             _U_(25)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP25_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP25_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP25(value)          (MATRIX_SPSELR_NSECP25_Msk & ((value) << MATRIX_SPSELR_NSECP25_Pos))
#define MATRIX_SPSELR_NSECP26_Pos             _U_(26)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP26_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP26_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP26(value)          (MATRIX_SPSELR_NSECP26_Msk & ((value) << MATRIX_SPSELR_NSECP26_Pos))
#define MATRIX_SPSELR_NSECP27_Pos             _U_(27)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP27_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP27_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP27(value)          (MATRIX_SPSELR_NSECP27_Msk & ((value) << MATRIX_SPSELR_NSECP27_Pos))
#define MATRIX_SPSELR_NSECP28_Pos             _U_(28)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP28_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP28_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP28(value)          (MATRIX_SPSELR_NSECP28_Msk & ((value) << MATRIX_SPSELR_NSECP28_Pos))
#define MATRIX_SPSELR_NSECP29_Pos             _U_(29)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP29_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP29_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP29(value)          (MATRIX_SPSELR_NSECP29_Msk & ((value) << MATRIX_SPSELR_NSECP29_Pos))
#define MATRIX_SPSELR_NSECP30_Pos             _U_(30)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP30_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP30_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP30(value)          (MATRIX_SPSELR_NSECP30_Msk & ((value) << MATRIX_SPSELR_NSECP30_Pos))
#define MATRIX_SPSELR_NSECP31_Pos             _U_(31)                                              /**< (MATRIX_SPSELR) Non-secured Peripheral Position */
#define MATRIX_SPSELR_NSECP31_Msk             (_U_(0x1) << MATRIX_SPSELR_NSECP31_Pos)              /**< (MATRIX_SPSELR) Non-secured Peripheral Mask */
#define MATRIX_SPSELR_NSECP31(value)          (MATRIX_SPSELR_NSECP31_Msk & ((value) << MATRIX_SPSELR_NSECP31_Pos))
#define MATRIX_SPSELR_Msk                     _U_(0xFFFFFFFF)                                      /**< (MATRIX_SPSELR) Register Mask  */

#define MATRIX_SPSELR_NSECP_Pos               _U_(0)                                               /**< (MATRIX_SPSELR Position) Non-secured Peripheral */
#define MATRIX_SPSELR_NSECP_Msk               (_U_(0xFFFFFFFF) << MATRIX_SPSELR_NSECP_Pos)         /**< (MATRIX_SPSELR Mask) NSECP */
#define MATRIX_SPSELR_NSECP(value)            (MATRIX_SPSELR_NSECP_Msk & ((value) << MATRIX_SPSELR_NSECP_Pos)) 

/** \brief MATRIX register offsets definitions */
#define MATRIX_PRAS_REG_OFST           (0x00)              /**< (MATRIX_PRAS) Priority Register A for Slave 0 Offset */
#define MATRIX_PRBS_REG_OFST           (0x04)              /**< (MATRIX_PRBS) Priority Register B for Slave 0 Offset */
#define MATRIX_MCFG_REG_OFST           (0x00)              /**< (MATRIX_MCFG) Master Configuration Register Offset */
#define MATRIX_SCFG_REG_OFST           (0x40)              /**< (MATRIX_SCFG) Slave Configuration Register Offset */
#define MATRIX_MEIER_REG_OFST          (0x150)             /**< (MATRIX_MEIER) Master Error Interrupt Enable Register Offset */
#define MATRIX_MEIDR_REG_OFST          (0x154)             /**< (MATRIX_MEIDR) Master Error Interrupt Disable Register Offset */
#define MATRIX_MEIMR_REG_OFST          (0x158)             /**< (MATRIX_MEIMR) Master Error Interrupt Mask Register Offset */
#define MATRIX_MESR_REG_OFST           (0x15C)             /**< (MATRIX_MESR) Master Error Status Register Offset */
#define MATRIX_MEAR_REG_OFST           (0x160)             /**< (MATRIX_MEAR) Master 0 Error Address Register Offset */
#define MATRIX_WPMR_REG_OFST           (0x1E4)             /**< (MATRIX_WPMR) Write Protection Mode Register Offset */
#define MATRIX_WPSR_REG_OFST           (0x1E8)             /**< (MATRIX_WPSR) Write Protection Status Register Offset */
#define MATRIX_SSR_REG_OFST            (0x200)             /**< (MATRIX_SSR) Security Slave 0 Register Offset */
#define MATRIX_SASSR_REG_OFST          (0x240)             /**< (MATRIX_SASSR) Security Areas Split Slave 0 Register Offset */
#define MATRIX_SRTSR_REG_OFST          (0x284)             /**< (MATRIX_SRTSR) Security Region Top Slave 1 Register Offset */
#define MATRIX_SPSELR_REG_OFST         (0x2C0)             /**< (MATRIX_SPSELR) Security Peripheral Select 1 Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief MATRIX_PR register API structure */
typedef struct
{
  __IO  uint32_t                       MATRIX_PRAS;        /**< Offset: 0x00 (R/W  32) Priority Register A for Slave 0 */
  __IO  uint32_t                       MATRIX_PRBS;        /**< Offset: 0x04 (R/W  32) Priority Register B for Slave 0 */
} matrix_pr_registers_t;

#define MATRIX_PR_NUMBER _U_(15)

/** \brief MATRIX register API structure */
typedef struct
{
  __IO  uint32_t                       MATRIX_MCFG[12];    /**< Offset: 0x00 (R/W  32) Master Configuration Register */
  __I   uint8_t                        Reserved1[0x10];
  __IO  uint32_t                       MATRIX_SCFG[15];    /**< Offset: 0x40 (R/W  32) Slave Configuration Register */
  __I   uint8_t                        Reserved2[0x04];
        matrix_pr_registers_t          MATRIX_PR[MATRIX_PR_NUMBER]; /**< Offset: 0x80  */
  __I   uint8_t                        Reserved3[0x58];
  __O   uint32_t                       MATRIX_MEIER;       /**< Offset: 0x150 ( /W  32) Master Error Interrupt Enable Register */
  __O   uint32_t                       MATRIX_MEIDR;       /**< Offset: 0x154 ( /W  32) Master Error Interrupt Disable Register */
  __I   uint32_t                       MATRIX_MEIMR;       /**< Offset: 0x158 (R/   32) Master Error Interrupt Mask Register */
  __I   uint32_t                       MATRIX_MESR;        /**< Offset: 0x15C (R/   32) Master Error Status Register */
  __I   uint32_t                       MATRIX_MEAR[12];    /**< Offset: 0x160 (R/   32) Master 0 Error Address Register */
  __I   uint8_t                        Reserved4[0x54];
  __IO  uint32_t                       MATRIX_WPMR;        /**< Offset: 0x1E4 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       MATRIX_WPSR;        /**< Offset: 0x1E8 (R/   32) Write Protection Status Register */
  __I   uint8_t                        Reserved5[0x14];
  __IO  uint32_t                       MATRIX_SSR[15];     /**< Offset: 0x200 (R/W  32) Security Slave 0 Register */
  __I   uint8_t                        Reserved6[0x04];
  __IO  uint32_t                       MATRIX_SASSR[15];   /**< Offset: 0x240 (R/W  32) Security Areas Split Slave 0 Register */
  __I   uint8_t                        Reserved7[0x08];
  __IO  uint32_t                       MATRIX_SRTSR[14];   /**< Offset: 0x284 (R/W  32) Security Region Top Slave 1 Register */
  __I   uint8_t                        Reserved8[0x04];
  __IO  uint32_t                       MATRIX_SPSELR[3];   /**< Offset: 0x2C0 (R/W  32) Security Peripheral Select 1 Register */
} matrix_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAMA5D2_MATRIX_COMPONENT_H_ */
