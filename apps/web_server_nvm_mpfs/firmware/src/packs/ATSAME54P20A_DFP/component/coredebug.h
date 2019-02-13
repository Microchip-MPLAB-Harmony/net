/**
 * \brief Component description for CoreDebug
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
#ifndef _SAME54_CoreDebug_COMPONENT_H_
#define _SAME54_CoreDebug_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR CoreDebug                                    */
/* ************************************************************************** */

/* -------- CoreDebug_DHCSR : (CoreDebug Offset: 0x00) (R/W 32) Debug Halting Control and Status Register -------- */
#define CoreDebug_DHCSR_C_DEBUGEN_Pos         _U_(0)                                               /**< (CoreDebug_DHCSR)  Position */
#define CoreDebug_DHCSR_C_DEBUGEN_Msk         (_U_(0x1) << CoreDebug_DHCSR_C_DEBUGEN_Pos)          /**< (CoreDebug_DHCSR)  Mask */
#define CoreDebug_DHCSR_C_DEBUGEN(value)      (CoreDebug_DHCSR_C_DEBUGEN_Msk & ((value) << CoreDebug_DHCSR_C_DEBUGEN_Pos))
#define CoreDebug_DHCSR_C_HALT_Pos            _U_(1)                                               /**< (CoreDebug_DHCSR)  Position */
#define CoreDebug_DHCSR_C_HALT_Msk            (_U_(0x1) << CoreDebug_DHCSR_C_HALT_Pos)             /**< (CoreDebug_DHCSR)  Mask */
#define CoreDebug_DHCSR_C_HALT(value)         (CoreDebug_DHCSR_C_HALT_Msk & ((value) << CoreDebug_DHCSR_C_HALT_Pos))
#define CoreDebug_DHCSR_C_STEP_Pos            _U_(2)                                               /**< (CoreDebug_DHCSR)  Position */
#define CoreDebug_DHCSR_C_STEP_Msk            (_U_(0x1) << CoreDebug_DHCSR_C_STEP_Pos)             /**< (CoreDebug_DHCSR)  Mask */
#define CoreDebug_DHCSR_C_STEP(value)         (CoreDebug_DHCSR_C_STEP_Msk & ((value) << CoreDebug_DHCSR_C_STEP_Pos))
#define CoreDebug_DHCSR_C_MASKINTS_Pos        _U_(3)                                               /**< (CoreDebug_DHCSR)  Position */
#define CoreDebug_DHCSR_C_MASKINTS_Msk        (_U_(0x1) << CoreDebug_DHCSR_C_MASKINTS_Pos)         /**< (CoreDebug_DHCSR)  Mask */
#define CoreDebug_DHCSR_C_MASKINTS(value)     (CoreDebug_DHCSR_C_MASKINTS_Msk & ((value) << CoreDebug_DHCSR_C_MASKINTS_Pos))
#define CoreDebug_DHCSR_C_SNAPSTALL_Pos       _U_(5)                                               /**< (CoreDebug_DHCSR)  Position */
#define CoreDebug_DHCSR_C_SNAPSTALL_Msk       (_U_(0x1) << CoreDebug_DHCSR_C_SNAPSTALL_Pos)        /**< (CoreDebug_DHCSR)  Mask */
#define CoreDebug_DHCSR_C_SNAPSTALL(value)    (CoreDebug_DHCSR_C_SNAPSTALL_Msk & ((value) << CoreDebug_DHCSR_C_SNAPSTALL_Pos))
#define CoreDebug_DHCSR_S_REGRDY_Pos          _U_(16)                                              /**< (CoreDebug_DHCSR)  Position */
#define CoreDebug_DHCSR_S_REGRDY_Msk          (_U_(0x1) << CoreDebug_DHCSR_S_REGRDY_Pos)           /**< (CoreDebug_DHCSR)  Mask */
#define CoreDebug_DHCSR_S_REGRDY(value)       (CoreDebug_DHCSR_S_REGRDY_Msk & ((value) << CoreDebug_DHCSR_S_REGRDY_Pos))
#define CoreDebug_DHCSR_DBGKEY_Pos            _U_(16)                                              /**< (CoreDebug_DHCSR)  Position */
#define CoreDebug_DHCSR_DBGKEY_Msk            (_U_(0xFFFF) << CoreDebug_DHCSR_DBGKEY_Pos)          /**< (CoreDebug_DHCSR)  Mask */
#define CoreDebug_DHCSR_DBGKEY(value)         (CoreDebug_DHCSR_DBGKEY_Msk & ((value) << CoreDebug_DHCSR_DBGKEY_Pos))
#define CoreDebug_DHCSR_S_HALT_Pos            _U_(17)                                              /**< (CoreDebug_DHCSR)  Position */
#define CoreDebug_DHCSR_S_HALT_Msk            (_U_(0x1) << CoreDebug_DHCSR_S_HALT_Pos)             /**< (CoreDebug_DHCSR)  Mask */
#define CoreDebug_DHCSR_S_HALT(value)         (CoreDebug_DHCSR_S_HALT_Msk & ((value) << CoreDebug_DHCSR_S_HALT_Pos))
#define CoreDebug_DHCSR_S_SLEEP_Pos           _U_(18)                                              /**< (CoreDebug_DHCSR)  Position */
#define CoreDebug_DHCSR_S_SLEEP_Msk           (_U_(0x1) << CoreDebug_DHCSR_S_SLEEP_Pos)            /**< (CoreDebug_DHCSR)  Mask */
#define CoreDebug_DHCSR_S_SLEEP(value)        (CoreDebug_DHCSR_S_SLEEP_Msk & ((value) << CoreDebug_DHCSR_S_SLEEP_Pos))
#define CoreDebug_DHCSR_S_LOCKUP_Pos          _U_(19)                                              /**< (CoreDebug_DHCSR)  Position */
#define CoreDebug_DHCSR_S_LOCKUP_Msk          (_U_(0x1) << CoreDebug_DHCSR_S_LOCKUP_Pos)           /**< (CoreDebug_DHCSR)  Mask */
#define CoreDebug_DHCSR_S_LOCKUP(value)       (CoreDebug_DHCSR_S_LOCKUP_Msk & ((value) << CoreDebug_DHCSR_S_LOCKUP_Pos))
#define CoreDebug_DHCSR_S_RETIRE_ST_Pos       _U_(24)                                              /**< (CoreDebug_DHCSR)  Position */
#define CoreDebug_DHCSR_S_RETIRE_ST_Msk       (_U_(0x1) << CoreDebug_DHCSR_S_RETIRE_ST_Pos)        /**< (CoreDebug_DHCSR)  Mask */
#define CoreDebug_DHCSR_S_RETIRE_ST(value)    (CoreDebug_DHCSR_S_RETIRE_ST_Msk & ((value) << CoreDebug_DHCSR_S_RETIRE_ST_Pos))
#define CoreDebug_DHCSR_S_RESET_ST_Pos        _U_(25)                                              /**< (CoreDebug_DHCSR)  Position */
#define CoreDebug_DHCSR_S_RESET_ST_Msk        (_U_(0x1) << CoreDebug_DHCSR_S_RESET_ST_Pos)         /**< (CoreDebug_DHCSR)  Mask */
#define CoreDebug_DHCSR_S_RESET_ST(value)     (CoreDebug_DHCSR_S_RESET_ST_Msk & ((value) << CoreDebug_DHCSR_S_RESET_ST_Pos))
#define CoreDebug_DHCSR_Msk                   _U_(0xFFFF002F)                                      /**< (CoreDebug_DHCSR) Register Mask  */


/* -------- CoreDebug_DCRSR : (CoreDebug Offset: 0x04) ( /W 32) Debug Core Register Selector Register -------- */
#define CoreDebug_DCRSR_REGSEL_Pos            _U_(0)                                               /**< (CoreDebug_DCRSR)  Position */
#define CoreDebug_DCRSR_REGSEL_Msk            (_U_(0x1F) << CoreDebug_DCRSR_REGSEL_Pos)            /**< (CoreDebug_DCRSR)  Mask */
#define CoreDebug_DCRSR_REGSEL(value)         (CoreDebug_DCRSR_REGSEL_Msk & ((value) << CoreDebug_DCRSR_REGSEL_Pos))
#define CoreDebug_DCRSR_REGWnR_Pos            _U_(16)                                              /**< (CoreDebug_DCRSR)  Position */
#define CoreDebug_DCRSR_REGWnR_Msk            (_U_(0x1) << CoreDebug_DCRSR_REGWnR_Pos)             /**< (CoreDebug_DCRSR)  Mask */
#define CoreDebug_DCRSR_REGWnR(value)         (CoreDebug_DCRSR_REGWnR_Msk & ((value) << CoreDebug_DCRSR_REGWnR_Pos))
#define CoreDebug_DCRSR_Msk                   _U_(0x0001001F)                                      /**< (CoreDebug_DCRSR) Register Mask  */


/* -------- CoreDebug_DCRDR : (CoreDebug Offset: 0x08) (R/W 32) Debug Core Register Data Register -------- */
#define CoreDebug_DCRDR_Msk                   _U_(0x00000000)                                      /**< (CoreDebug_DCRDR) Register Mask  */


/* -------- CoreDebug_DEMCR : (CoreDebug Offset: 0x0C) (R/W 32) Debug Exception and Monitor Control Register -------- */
#define CoreDebug_DEMCR_VC_CORERESET_Pos      _U_(0)                                               /**< (CoreDebug_DEMCR)  Position */
#define CoreDebug_DEMCR_VC_CORERESET_Msk      (_U_(0x1) << CoreDebug_DEMCR_VC_CORERESET_Pos)       /**< (CoreDebug_DEMCR)  Mask */
#define CoreDebug_DEMCR_VC_CORERESET(value)   (CoreDebug_DEMCR_VC_CORERESET_Msk & ((value) << CoreDebug_DEMCR_VC_CORERESET_Pos))
#define CoreDebug_DEMCR_VC_MMERR_Pos          _U_(4)                                               /**< (CoreDebug_DEMCR)  Position */
#define CoreDebug_DEMCR_VC_MMERR_Msk          (_U_(0x1) << CoreDebug_DEMCR_VC_MMERR_Pos)           /**< (CoreDebug_DEMCR)  Mask */
#define CoreDebug_DEMCR_VC_MMERR(value)       (CoreDebug_DEMCR_VC_MMERR_Msk & ((value) << CoreDebug_DEMCR_VC_MMERR_Pos))
#define CoreDebug_DEMCR_VC_NOCPERR_Pos        _U_(5)                                               /**< (CoreDebug_DEMCR)  Position */
#define CoreDebug_DEMCR_VC_NOCPERR_Msk        (_U_(0x1) << CoreDebug_DEMCR_VC_NOCPERR_Pos)         /**< (CoreDebug_DEMCR)  Mask */
#define CoreDebug_DEMCR_VC_NOCPERR(value)     (CoreDebug_DEMCR_VC_NOCPERR_Msk & ((value) << CoreDebug_DEMCR_VC_NOCPERR_Pos))
#define CoreDebug_DEMCR_VC_CHKERR_Pos         _U_(6)                                               /**< (CoreDebug_DEMCR)  Position */
#define CoreDebug_DEMCR_VC_CHKERR_Msk         (_U_(0x1) << CoreDebug_DEMCR_VC_CHKERR_Pos)          /**< (CoreDebug_DEMCR)  Mask */
#define CoreDebug_DEMCR_VC_CHKERR(value)      (CoreDebug_DEMCR_VC_CHKERR_Msk & ((value) << CoreDebug_DEMCR_VC_CHKERR_Pos))
#define CoreDebug_DEMCR_VC_STATERR_Pos        _U_(7)                                               /**< (CoreDebug_DEMCR)  Position */
#define CoreDebug_DEMCR_VC_STATERR_Msk        (_U_(0x1) << CoreDebug_DEMCR_VC_STATERR_Pos)         /**< (CoreDebug_DEMCR)  Mask */
#define CoreDebug_DEMCR_VC_STATERR(value)     (CoreDebug_DEMCR_VC_STATERR_Msk & ((value) << CoreDebug_DEMCR_VC_STATERR_Pos))
#define CoreDebug_DEMCR_VC_BUSERR_Pos         _U_(8)                                               /**< (CoreDebug_DEMCR)  Position */
#define CoreDebug_DEMCR_VC_BUSERR_Msk         (_U_(0x1) << CoreDebug_DEMCR_VC_BUSERR_Pos)          /**< (CoreDebug_DEMCR)  Mask */
#define CoreDebug_DEMCR_VC_BUSERR(value)      (CoreDebug_DEMCR_VC_BUSERR_Msk & ((value) << CoreDebug_DEMCR_VC_BUSERR_Pos))
#define CoreDebug_DEMCR_VC_INTERR_Pos         _U_(9)                                               /**< (CoreDebug_DEMCR)  Position */
#define CoreDebug_DEMCR_VC_INTERR_Msk         (_U_(0x1) << CoreDebug_DEMCR_VC_INTERR_Pos)          /**< (CoreDebug_DEMCR)  Mask */
#define CoreDebug_DEMCR_VC_INTERR(value)      (CoreDebug_DEMCR_VC_INTERR_Msk & ((value) << CoreDebug_DEMCR_VC_INTERR_Pos))
#define CoreDebug_DEMCR_VC_HARDERR_Pos        _U_(10)                                              /**< (CoreDebug_DEMCR)  Position */
#define CoreDebug_DEMCR_VC_HARDERR_Msk        (_U_(0x1) << CoreDebug_DEMCR_VC_HARDERR_Pos)         /**< (CoreDebug_DEMCR)  Mask */
#define CoreDebug_DEMCR_VC_HARDERR(value)     (CoreDebug_DEMCR_VC_HARDERR_Msk & ((value) << CoreDebug_DEMCR_VC_HARDERR_Pos))
#define CoreDebug_DEMCR_MON_EN_Pos            _U_(16)                                              /**< (CoreDebug_DEMCR)  Position */
#define CoreDebug_DEMCR_MON_EN_Msk            (_U_(0x1) << CoreDebug_DEMCR_MON_EN_Pos)             /**< (CoreDebug_DEMCR)  Mask */
#define CoreDebug_DEMCR_MON_EN(value)         (CoreDebug_DEMCR_MON_EN_Msk & ((value) << CoreDebug_DEMCR_MON_EN_Pos))
#define CoreDebug_DEMCR_MON_PEND_Pos          _U_(17)                                              /**< (CoreDebug_DEMCR)  Position */
#define CoreDebug_DEMCR_MON_PEND_Msk          (_U_(0x1) << CoreDebug_DEMCR_MON_PEND_Pos)           /**< (CoreDebug_DEMCR)  Mask */
#define CoreDebug_DEMCR_MON_PEND(value)       (CoreDebug_DEMCR_MON_PEND_Msk & ((value) << CoreDebug_DEMCR_MON_PEND_Pos))
#define CoreDebug_DEMCR_MON_STEP_Pos          _U_(18)                                              /**< (CoreDebug_DEMCR)  Position */
#define CoreDebug_DEMCR_MON_STEP_Msk          (_U_(0x1) << CoreDebug_DEMCR_MON_STEP_Pos)           /**< (CoreDebug_DEMCR)  Mask */
#define CoreDebug_DEMCR_MON_STEP(value)       (CoreDebug_DEMCR_MON_STEP_Msk & ((value) << CoreDebug_DEMCR_MON_STEP_Pos))
#define CoreDebug_DEMCR_MON_REQ_Pos           _U_(19)                                              /**< (CoreDebug_DEMCR)  Position */
#define CoreDebug_DEMCR_MON_REQ_Msk           (_U_(0x1) << CoreDebug_DEMCR_MON_REQ_Pos)            /**< (CoreDebug_DEMCR)  Mask */
#define CoreDebug_DEMCR_MON_REQ(value)        (CoreDebug_DEMCR_MON_REQ_Msk & ((value) << CoreDebug_DEMCR_MON_REQ_Pos))
#define CoreDebug_DEMCR_TRCENA_Pos            _U_(24)                                              /**< (CoreDebug_DEMCR)  Position */
#define CoreDebug_DEMCR_TRCENA_Msk            (_U_(0x1) << CoreDebug_DEMCR_TRCENA_Pos)             /**< (CoreDebug_DEMCR)  Mask */
#define CoreDebug_DEMCR_TRCENA(value)         (CoreDebug_DEMCR_TRCENA_Msk & ((value) << CoreDebug_DEMCR_TRCENA_Pos))
#define CoreDebug_DEMCR_Msk                   _U_(0x010F07F1)                                      /**< (CoreDebug_DEMCR) Register Mask  */


/** \brief CoreDebug register offsets definitions */
#define CoreDebug_DHCSR_REG_OFST       (0x00)              /**< (CoreDebug_DHCSR) Debug Halting Control and Status Register Offset */
#define CoreDebug_DCRSR_REG_OFST       (0x04)              /**< (CoreDebug_DCRSR) Debug Core Register Selector Register Offset */
#define CoreDebug_DCRDR_REG_OFST       (0x08)              /**< (CoreDebug_DCRDR) Debug Core Register Data Register Offset */
#define CoreDebug_DEMCR_REG_OFST       (0x0C)              /**< (CoreDebug_DEMCR) Debug Exception and Monitor Control Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief CoreDebug register API structure */
typedef struct
{  /* Core Debug Register */
  __IO  uint32_t                       CoreDebug_DHCSR;    /**< Offset: 0x00 (R/W  32) Debug Halting Control and Status Register */
  __O   uint32_t                       CoreDebug_DCRSR;    /**< Offset: 0x04 ( /W  32) Debug Core Register Selector Register */
  __IO  uint32_t                       CoreDebug_DCRDR;    /**< Offset: 0x08 (R/W  32) Debug Core Register Data Register */
  __IO  uint32_t                       CoreDebug_DEMCR;    /**< Offset: 0x0C (R/W  32) Debug Exception and Monitor Control Register */
} coredebug_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_CoreDebug_COMPONENT_H_ */
