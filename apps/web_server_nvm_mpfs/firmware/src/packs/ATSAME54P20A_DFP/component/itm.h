/**
 * \brief Component description for ITM
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
#ifndef _SAME54_ITM_COMPONENT_H_
#define _SAME54_ITM_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR ITM                                          */
/* ************************************************************************** */

/* -------- ITM_PORT : (ITM Offset: 0x00) ( /W 32) ITM Stimulus Port Registers -------- */
#define ITM_PORT_Msk                          _U_(0x00000000)                                      /**< (ITM_PORT) Register Mask  */

/* WORD mode */
#define ITM_PORT_WORD_PORT_Pos                _U_(0)                                               /**< (ITM_PORT)  Position */
#define ITM_PORT_WORD_PORT_Msk                (_U_(0xFFFFFFFF) << ITM_PORT_WORD_PORT_Pos)          /**< (ITM_PORT)  Mask */
#define ITM_PORT_WORD_PORT(value)             (ITM_PORT_WORD_PORT_Msk & ((value) << ITM_PORT_WORD_PORT_Pos))
#define ITM_PORT_WORD_Msk                     _U_(0xFFFFFFFF)                                       /**< (ITM_PORT_WORD) Register Mask  */

/* BYTE mode */
#define ITM_PORT_BYTE_PORT_Pos                _U_(0)                                               /**< (ITM_PORT)  Position */
#define ITM_PORT_BYTE_PORT_Msk                (_U_(0xFF) << ITM_PORT_BYTE_PORT_Pos)                /**< (ITM_PORT)  Mask */
#define ITM_PORT_BYTE_PORT(value)             (ITM_PORT_BYTE_PORT_Msk & ((value) << ITM_PORT_BYTE_PORT_Pos))
#define ITM_PORT_BYTE_Msk                     _U_(0x000000FF)                                       /**< (ITM_PORT_BYTE) Register Mask  */

/* HWORD mode */
#define ITM_PORT_HWORD_PORT_Pos               _U_(0)                                               /**< (ITM_PORT)  Position */
#define ITM_PORT_HWORD_PORT_Msk               (_U_(0xFFFF) << ITM_PORT_HWORD_PORT_Pos)             /**< (ITM_PORT)  Mask */
#define ITM_PORT_HWORD_PORT(value)            (ITM_PORT_HWORD_PORT_Msk & ((value) << ITM_PORT_HWORD_PORT_Pos))
#define ITM_PORT_HWORD_Msk                    _U_(0x0000FFFF)                                       /**< (ITM_PORT_HWORD) Register Mask  */


/* -------- ITM_TER : (ITM Offset: 0xE00) (R/W 32) ITM Trace Enable Register -------- */
#define ITM_TER_Msk                           _U_(0x00000000)                                      /**< (ITM_TER) Register Mask  */


/* -------- ITM_TPR : (ITM Offset: 0xE40) (R/W 32) ITM Trace Privilege Register -------- */
#define ITM_TPR_PRIVMASK_Pos                  _U_(0)                                               /**< (ITM_TPR)  Position */
#define ITM_TPR_PRIVMASK_Msk                  (_U_(0xF) << ITM_TPR_PRIVMASK_Pos)                   /**< (ITM_TPR)  Mask */
#define ITM_TPR_PRIVMASK(value)               (ITM_TPR_PRIVMASK_Msk & ((value) << ITM_TPR_PRIVMASK_Pos))
#define ITM_TPR_Msk                           _U_(0x0000000F)                                      /**< (ITM_TPR) Register Mask  */


/* -------- ITM_TCR : (ITM Offset: 0xE80) (R/W 32) ITM Trace Control Register -------- */
#define ITM_TCR_ITMENA_Pos                    _U_(0)                                               /**< (ITM_TCR)  Position */
#define ITM_TCR_ITMENA_Msk                    (_U_(0x1) << ITM_TCR_ITMENA_Pos)                     /**< (ITM_TCR)  Mask */
#define ITM_TCR_ITMENA(value)                 (ITM_TCR_ITMENA_Msk & ((value) << ITM_TCR_ITMENA_Pos))
#define ITM_TCR_TSENA_Pos                     _U_(1)                                               /**< (ITM_TCR)  Position */
#define ITM_TCR_TSENA_Msk                     (_U_(0x1) << ITM_TCR_TSENA_Pos)                      /**< (ITM_TCR)  Mask */
#define ITM_TCR_TSENA(value)                  (ITM_TCR_TSENA_Msk & ((value) << ITM_TCR_TSENA_Pos))
#define ITM_TCR_SYNCENA_Pos                   _U_(2)                                               /**< (ITM_TCR)  Position */
#define ITM_TCR_SYNCENA_Msk                   (_U_(0x1) << ITM_TCR_SYNCENA_Pos)                    /**< (ITM_TCR)  Mask */
#define ITM_TCR_SYNCENA(value)                (ITM_TCR_SYNCENA_Msk & ((value) << ITM_TCR_SYNCENA_Pos))
#define ITM_TCR_DWTENA_Pos                    _U_(3)                                               /**< (ITM_TCR)  Position */
#define ITM_TCR_DWTENA_Msk                    (_U_(0x1) << ITM_TCR_DWTENA_Pos)                     /**< (ITM_TCR)  Mask */
#define ITM_TCR_DWTENA(value)                 (ITM_TCR_DWTENA_Msk & ((value) << ITM_TCR_DWTENA_Pos))
#define ITM_TCR_SWOENA_Pos                    _U_(4)                                               /**< (ITM_TCR)  Position */
#define ITM_TCR_SWOENA_Msk                    (_U_(0x1) << ITM_TCR_SWOENA_Pos)                     /**< (ITM_TCR)  Mask */
#define ITM_TCR_SWOENA(value)                 (ITM_TCR_SWOENA_Msk & ((value) << ITM_TCR_SWOENA_Pos))
#define ITM_TCR_STALLENA_Pos                  _U_(5)                                               /**< (ITM_TCR)  Position */
#define ITM_TCR_STALLENA_Msk                  (_U_(0x1) << ITM_TCR_STALLENA_Pos)                   /**< (ITM_TCR)  Mask */
#define ITM_TCR_STALLENA(value)               (ITM_TCR_STALLENA_Msk & ((value) << ITM_TCR_STALLENA_Pos))
#define ITM_TCR_TSPrescale_Pos                _U_(8)                                               /**< (ITM_TCR)  Position */
#define ITM_TCR_TSPrescale_Msk                (_U_(0x3) << ITM_TCR_TSPrescale_Pos)                 /**< (ITM_TCR)  Mask */
#define ITM_TCR_TSPrescale(value)             (ITM_TCR_TSPrescale_Msk & ((value) << ITM_TCR_TSPrescale_Pos))
#define ITM_TCR_GTSFREQ_Pos                   _U_(10)                                              /**< (ITM_TCR)  Position */
#define ITM_TCR_GTSFREQ_Msk                   (_U_(0x3) << ITM_TCR_GTSFREQ_Pos)                    /**< (ITM_TCR)  Mask */
#define ITM_TCR_GTSFREQ(value)                (ITM_TCR_GTSFREQ_Msk & ((value) << ITM_TCR_GTSFREQ_Pos))
#define ITM_TCR_TraceBusID_Pos                _U_(16)                                              /**< (ITM_TCR)  Position */
#define ITM_TCR_TraceBusID_Msk                (_U_(0x7F) << ITM_TCR_TraceBusID_Pos)                /**< (ITM_TCR)  Mask */
#define ITM_TCR_TraceBusID(value)             (ITM_TCR_TraceBusID_Msk & ((value) << ITM_TCR_TraceBusID_Pos))
#define ITM_TCR_BUSY_Pos                      _U_(23)                                              /**< (ITM_TCR)  Position */
#define ITM_TCR_BUSY_Msk                      (_U_(0x1) << ITM_TCR_BUSY_Pos)                       /**< (ITM_TCR)  Mask */
#define ITM_TCR_BUSY(value)                   (ITM_TCR_BUSY_Msk & ((value) << ITM_TCR_BUSY_Pos))  
#define ITM_TCR_Msk                           _U_(0x00FF0F3F)                                      /**< (ITM_TCR) Register Mask  */


/* -------- ITM_IWR : (ITM Offset: 0xEF8) ( /W 32) ITM Integration Write Register -------- */
#define ITM_IWR_ATVALIDM_Pos                  _U_(0)                                               /**< (ITM_IWR)  Position */
#define ITM_IWR_ATVALIDM_Msk                  (_U_(0x1) << ITM_IWR_ATVALIDM_Pos)                   /**< (ITM_IWR)  Mask */
#define ITM_IWR_ATVALIDM(value)               (ITM_IWR_ATVALIDM_Msk & ((value) << ITM_IWR_ATVALIDM_Pos))
#define ITM_IWR_Msk                           _U_(0x00000001)                                      /**< (ITM_IWR) Register Mask  */


/* -------- ITM_IRR : (ITM Offset: 0xEFC) ( R/ 32) ITM Integration Read Register -------- */
#define ITM_IRR_ATREADYM_Pos                  _U_(0)                                               /**< (ITM_IRR)  Position */
#define ITM_IRR_ATREADYM_Msk                  (_U_(0x1) << ITM_IRR_ATREADYM_Pos)                   /**< (ITM_IRR)  Mask */
#define ITM_IRR_ATREADYM(value)               (ITM_IRR_ATREADYM_Msk & ((value) << ITM_IRR_ATREADYM_Pos))
#define ITM_IRR_Msk                           _U_(0x00000001)                                      /**< (ITM_IRR) Register Mask  */


/** \brief ITM register offsets definitions */
#define ITM_PORT_REG_OFST              (0x00)              /**< (ITM_PORT) ITM Stimulus Port Registers Offset */
#define ITM_TER_REG_OFST               (0xE00)             /**< (ITM_TER) ITM Trace Enable Register Offset */
#define ITM_TPR_REG_OFST               (0xE40)             /**< (ITM_TPR) ITM Trace Privilege Register Offset */
#define ITM_TCR_REG_OFST               (0xE80)             /**< (ITM_TCR) ITM Trace Control Register Offset */
#define ITM_IWR_REG_OFST               (0xEF8)             /**< (ITM_IWR) ITM Integration Write Register Offset */
#define ITM_IRR_REG_OFST               (0xEFC)             /**< (ITM_IRR) ITM Integration Read Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief ITM register API structure */
typedef struct
{  /* Instrumentation Trace Macrocell */
  __O   uint32_t                       ITM_PORT[32];       /**< Offset: 0x00 ( /W  32) ITM Stimulus Port Registers */
  __I   uint8_t                        Reserved1[0xD80];
  __IO  uint32_t                       ITM_TER;            /**< Offset: 0xE00 (R/W  32) ITM Trace Enable Register */
  __I   uint8_t                        Reserved2[0x3C];
  __IO  uint32_t                       ITM_TPR;            /**< Offset: 0xE40 (R/W  32) ITM Trace Privilege Register */
  __I   uint8_t                        Reserved3[0x3C];
  __IO  uint32_t                       ITM_TCR;            /**< Offset: 0xE80 (R/W  32) ITM Trace Control Register */
  __I   uint8_t                        Reserved4[0x74];
  __O   uint32_t                       ITM_IWR;            /**< Offset: 0xEF8 ( /W  32) ITM Integration Write Register */
  __I   uint32_t                       ITM_IRR;            /**< Offset: 0xEFC (R/   32) ITM Integration Read Register */
} itm_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_ITM_COMPONENT_H_ */
