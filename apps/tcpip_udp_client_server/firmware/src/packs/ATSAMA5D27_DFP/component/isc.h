/**
 * \brief Component description for ISC
 *
 * Copyright (c) 2019 Microchip Technology Inc. and its subsidiaries.
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

/* file generated from device description version 2019-04-25T17:28:51Z */
#ifndef _SAMA5D2_ISC_COMPONENT_H_
#define _SAMA5D2_ISC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR ISC                                          */
/* ************************************************************************** */

/* -------- ISC_DAD : (ISC Offset: 0x00) (R/W 32) DMA Address 0 Register -------- */
#define ISC_DAD_AD0_Pos                       _U_(0)                                               /**< (ISC_DAD) Channel 0 Address Position */
#define ISC_DAD_AD0_Msk                       (_U_(0xFFFFFFFF) << ISC_DAD_AD0_Pos)                 /**< (ISC_DAD) Channel 0 Address Mask */
#define ISC_DAD_AD0(value)                    (ISC_DAD_AD0_Msk & ((value) << ISC_DAD_AD0_Pos))    
#define ISC_DAD_Msk                           _U_(0xFFFFFFFF)                                      /**< (ISC_DAD) Register Mask  */


/* -------- ISC_DST : (ISC Offset: 0x04) (R/W 32) DMA Stride 0 Register -------- */
#define ISC_DST_ST0_Pos                       _U_(0)                                               /**< (ISC_DST) Channel 0 Stride Position */
#define ISC_DST_ST0_Msk                       (_U_(0xFFFFFFFF) << ISC_DST_ST0_Pos)                 /**< (ISC_DST) Channel 0 Stride Mask */
#define ISC_DST_ST0(value)                    (ISC_DST_ST0_Msk & ((value) << ISC_DST_ST0_Pos))    
#define ISC_DST_Msk                           _U_(0xFFFFFFFF)                                      /**< (ISC_DST) Register Mask  */


/* -------- ISC_CTRLEN : (ISC Offset: 0x00) ( /W 32) Control Enable Register -------- */
#define ISC_CTRLEN_CAPTURE_Pos                _U_(0)                                               /**< (ISC_CTRLEN) Capture Input Stream Command Position */
#define ISC_CTRLEN_CAPTURE_Msk                (_U_(0x1) << ISC_CTRLEN_CAPTURE_Pos)                 /**< (ISC_CTRLEN) Capture Input Stream Command Mask */
#define ISC_CTRLEN_CAPTURE(value)             (ISC_CTRLEN_CAPTURE_Msk & ((value) << ISC_CTRLEN_CAPTURE_Pos))
#define ISC_CTRLEN_UPPRO_Pos                  _U_(1)                                               /**< (ISC_CTRLEN) Update Profile Position */
#define ISC_CTRLEN_UPPRO_Msk                  (_U_(0x1) << ISC_CTRLEN_UPPRO_Pos)                   /**< (ISC_CTRLEN) Update Profile Mask */
#define ISC_CTRLEN_UPPRO(value)               (ISC_CTRLEN_UPPRO_Msk & ((value) << ISC_CTRLEN_UPPRO_Pos))
#define ISC_CTRLEN_HISREQ_Pos                 _U_(2)                                               /**< (ISC_CTRLEN) Histogram Request Position */
#define ISC_CTRLEN_HISREQ_Msk                 (_U_(0x1) << ISC_CTRLEN_HISREQ_Pos)                  /**< (ISC_CTRLEN) Histogram Request Mask */
#define ISC_CTRLEN_HISREQ(value)              (ISC_CTRLEN_HISREQ_Msk & ((value) << ISC_CTRLEN_HISREQ_Pos))
#define ISC_CTRLEN_HISCLR_Pos                 _U_(3)                                               /**< (ISC_CTRLEN) Histogram Clear Position */
#define ISC_CTRLEN_HISCLR_Msk                 (_U_(0x1) << ISC_CTRLEN_HISCLR_Pos)                  /**< (ISC_CTRLEN) Histogram Clear Mask */
#define ISC_CTRLEN_HISCLR(value)              (ISC_CTRLEN_HISCLR_Msk & ((value) << ISC_CTRLEN_HISCLR_Pos))
#define ISC_CTRLEN_Msk                        _U_(0x0000000F)                                      /**< (ISC_CTRLEN) Register Mask  */


/* -------- ISC_CTRLDIS : (ISC Offset: 0x04) ( /W 32) Control Disable Register -------- */
#define ISC_CTRLDIS_DISABLE_Pos               _U_(0)                                               /**< (ISC_CTRLDIS) Capture Disable Position */
#define ISC_CTRLDIS_DISABLE_Msk               (_U_(0x1) << ISC_CTRLDIS_DISABLE_Pos)                /**< (ISC_CTRLDIS) Capture Disable Mask */
#define ISC_CTRLDIS_DISABLE(value)            (ISC_CTRLDIS_DISABLE_Msk & ((value) << ISC_CTRLDIS_DISABLE_Pos))
#define ISC_CTRLDIS_SWRST_Pos                 _U_(8)                                               /**< (ISC_CTRLDIS) Software Reset Position */
#define ISC_CTRLDIS_SWRST_Msk                 (_U_(0x1) << ISC_CTRLDIS_SWRST_Pos)                  /**< (ISC_CTRLDIS) Software Reset Mask */
#define ISC_CTRLDIS_SWRST(value)              (ISC_CTRLDIS_SWRST_Msk & ((value) << ISC_CTRLDIS_SWRST_Pos))
#define ISC_CTRLDIS_Msk                       _U_(0x00000101)                                      /**< (ISC_CTRLDIS) Register Mask  */


/* -------- ISC_CTRLSR : (ISC Offset: 0x08) ( R/ 32) Control Status Register -------- */
#define ISC_CTRLSR_CAPTURE_Pos                _U_(0)                                               /**< (ISC_CTRLSR) Capture pending Position */
#define ISC_CTRLSR_CAPTURE_Msk                (_U_(0x1) << ISC_CTRLSR_CAPTURE_Pos)                 /**< (ISC_CTRLSR) Capture pending Mask */
#define ISC_CTRLSR_CAPTURE(value)             (ISC_CTRLSR_CAPTURE_Msk & ((value) << ISC_CTRLSR_CAPTURE_Pos))
#define ISC_CTRLSR_UPPRO_Pos                  _U_(1)                                               /**< (ISC_CTRLSR) Profile Update Pending Position */
#define ISC_CTRLSR_UPPRO_Msk                  (_U_(0x1) << ISC_CTRLSR_UPPRO_Pos)                   /**< (ISC_CTRLSR) Profile Update Pending Mask */
#define ISC_CTRLSR_UPPRO(value)               (ISC_CTRLSR_UPPRO_Msk & ((value) << ISC_CTRLSR_UPPRO_Pos))
#define ISC_CTRLSR_HISREQ_Pos                 _U_(2)                                               /**< (ISC_CTRLSR) Histogram Request Pending Position */
#define ISC_CTRLSR_HISREQ_Msk                 (_U_(0x1) << ISC_CTRLSR_HISREQ_Pos)                  /**< (ISC_CTRLSR) Histogram Request Pending Mask */
#define ISC_CTRLSR_HISREQ(value)              (ISC_CTRLSR_HISREQ_Msk & ((value) << ISC_CTRLSR_HISREQ_Pos))
#define ISC_CTRLSR_FIELD_Pos                  _U_(4)                                               /**< (ISC_CTRLSR) Field Status (only relevant when the video stream is interlaced) Position */
#define ISC_CTRLSR_FIELD_Msk                  (_U_(0x1) << ISC_CTRLSR_FIELD_Pos)                   /**< (ISC_CTRLSR) Field Status (only relevant when the video stream is interlaced) Mask */
#define ISC_CTRLSR_FIELD(value)               (ISC_CTRLSR_FIELD_Msk & ((value) << ISC_CTRLSR_FIELD_Pos))
#define ISC_CTRLSR_SIP_Pos                    _U_(31)                                              /**< (ISC_CTRLSR) Synchronization In Progress Position */
#define ISC_CTRLSR_SIP_Msk                    (_U_(0x1) << ISC_CTRLSR_SIP_Pos)                     /**< (ISC_CTRLSR) Synchronization In Progress Mask */
#define ISC_CTRLSR_SIP(value)                 (ISC_CTRLSR_SIP_Msk & ((value) << ISC_CTRLSR_SIP_Pos))
#define ISC_CTRLSR_Msk                        _U_(0x80000017)                                      /**< (ISC_CTRLSR) Register Mask  */


/* -------- ISC_PFE_CFG0 : (ISC Offset: 0x0C) (R/W 32) Parallel Front End Configuration 0 Register -------- */
#define ISC_PFE_CFG0_HPOL_Pos                 _U_(0)                                               /**< (ISC_PFE_CFG0) Horizontal Synchronization Polarity Position */
#define ISC_PFE_CFG0_HPOL_Msk                 (_U_(0x1) << ISC_PFE_CFG0_HPOL_Pos)                  /**< (ISC_PFE_CFG0) Horizontal Synchronization Polarity Mask */
#define ISC_PFE_CFG0_HPOL(value)              (ISC_PFE_CFG0_HPOL_Msk & ((value) << ISC_PFE_CFG0_HPOL_Pos))
#define ISC_PFE_CFG0_VPOL_Pos                 _U_(1)                                               /**< (ISC_PFE_CFG0) Vertical Synchronization Polarity Position */
#define ISC_PFE_CFG0_VPOL_Msk                 (_U_(0x1) << ISC_PFE_CFG0_VPOL_Pos)                  /**< (ISC_PFE_CFG0) Vertical Synchronization Polarity Mask */
#define ISC_PFE_CFG0_VPOL(value)              (ISC_PFE_CFG0_VPOL_Msk & ((value) << ISC_PFE_CFG0_VPOL_Pos))
#define ISC_PFE_CFG0_PPOL_Pos                 _U_(2)                                               /**< (ISC_PFE_CFG0) Pixel Clock Polarity Position */
#define ISC_PFE_CFG0_PPOL_Msk                 (_U_(0x1) << ISC_PFE_CFG0_PPOL_Pos)                  /**< (ISC_PFE_CFG0) Pixel Clock Polarity Mask */
#define ISC_PFE_CFG0_PPOL(value)              (ISC_PFE_CFG0_PPOL_Msk & ((value) << ISC_PFE_CFG0_PPOL_Pos))
#define ISC_PFE_CFG0_FPOL_Pos                 _U_(3)                                               /**< (ISC_PFE_CFG0) Field Polarity Position */
#define ISC_PFE_CFG0_FPOL_Msk                 (_U_(0x1) << ISC_PFE_CFG0_FPOL_Pos)                  /**< (ISC_PFE_CFG0) Field Polarity Mask */
#define ISC_PFE_CFG0_FPOL(value)              (ISC_PFE_CFG0_FPOL_Msk & ((value) << ISC_PFE_CFG0_FPOL_Pos))
#define ISC_PFE_CFG0_MODE_Pos                 _U_(4)                                               /**< (ISC_PFE_CFG0) Parallel Front End Mode Position */
#define ISC_PFE_CFG0_MODE_Msk                 (_U_(0x7) << ISC_PFE_CFG0_MODE_Pos)                  /**< (ISC_PFE_CFG0) Parallel Front End Mode Mask */
#define ISC_PFE_CFG0_MODE(value)              (ISC_PFE_CFG0_MODE_Msk & ((value) << ISC_PFE_CFG0_MODE_Pos))
#define   ISC_PFE_CFG0_MODE_PROGRESSIVE_Val   _U_(0x0)                                             /**< (ISC_PFE_CFG0) Video source is progressive.  */
#define   ISC_PFE_CFG0_MODE_DF_TOP_Val        _U_(0x1)                                             /**< (ISC_PFE_CFG0) Video source is interlaced, two fields are captured starting with top field.  */
#define   ISC_PFE_CFG0_MODE_DF_BOTTOM_Val     _U_(0x2)                                             /**< (ISC_PFE_CFG0) Video source is interlaced, two fields are captured starting with bottom field.  */
#define   ISC_PFE_CFG0_MODE_DF_IMMEDIATE_Val  _U_(0x3)                                             /**< (ISC_PFE_CFG0) Video source is interlaced, two fields are captured immediately.  */
#define   ISC_PFE_CFG0_MODE_SF_TOP_Val        _U_(0x4)                                             /**< (ISC_PFE_CFG0) Video source is interlaced, one field is captured starting with the top field.  */
#define   ISC_PFE_CFG0_MODE_SF_BOTTOM_Val     _U_(0x5)                                             /**< (ISC_PFE_CFG0) Video source is interlaced, one field is captured starting with the bottom field.  */
#define   ISC_PFE_CFG0_MODE_SF_IMMEDIATE_Val  _U_(0x6)                                             /**< (ISC_PFE_CFG0) Video source is interlaced, one field is captured starting immediately.  */
#define ISC_PFE_CFG0_MODE_PROGRESSIVE         (ISC_PFE_CFG0_MODE_PROGRESSIVE_Val << ISC_PFE_CFG0_MODE_Pos) /**< (ISC_PFE_CFG0) Video source is progressive. Position  */
#define ISC_PFE_CFG0_MODE_DF_TOP              (ISC_PFE_CFG0_MODE_DF_TOP_Val << ISC_PFE_CFG0_MODE_Pos) /**< (ISC_PFE_CFG0) Video source is interlaced, two fields are captured starting with top field. Position  */
#define ISC_PFE_CFG0_MODE_DF_BOTTOM           (ISC_PFE_CFG0_MODE_DF_BOTTOM_Val << ISC_PFE_CFG0_MODE_Pos) /**< (ISC_PFE_CFG0) Video source is interlaced, two fields are captured starting with bottom field. Position  */
#define ISC_PFE_CFG0_MODE_DF_IMMEDIATE        (ISC_PFE_CFG0_MODE_DF_IMMEDIATE_Val << ISC_PFE_CFG0_MODE_Pos) /**< (ISC_PFE_CFG0) Video source is interlaced, two fields are captured immediately. Position  */
#define ISC_PFE_CFG0_MODE_SF_TOP              (ISC_PFE_CFG0_MODE_SF_TOP_Val << ISC_PFE_CFG0_MODE_Pos) /**< (ISC_PFE_CFG0) Video source is interlaced, one field is captured starting with the top field. Position  */
#define ISC_PFE_CFG0_MODE_SF_BOTTOM           (ISC_PFE_CFG0_MODE_SF_BOTTOM_Val << ISC_PFE_CFG0_MODE_Pos) /**< (ISC_PFE_CFG0) Video source is interlaced, one field is captured starting with the bottom field. Position  */
#define ISC_PFE_CFG0_MODE_SF_IMMEDIATE        (ISC_PFE_CFG0_MODE_SF_IMMEDIATE_Val << ISC_PFE_CFG0_MODE_Pos) /**< (ISC_PFE_CFG0) Video source is interlaced, one field is captured starting immediately. Position  */
#define ISC_PFE_CFG0_CONT_Pos                 _U_(7)                                               /**< (ISC_PFE_CFG0) Continuous Acquisition Position */
#define ISC_PFE_CFG0_CONT_Msk                 (_U_(0x1) << ISC_PFE_CFG0_CONT_Pos)                  /**< (ISC_PFE_CFG0) Continuous Acquisition Mask */
#define ISC_PFE_CFG0_CONT(value)              (ISC_PFE_CFG0_CONT_Msk & ((value) << ISC_PFE_CFG0_CONT_Pos))
#define ISC_PFE_CFG0_GATED_Pos                _U_(8)                                               /**< (ISC_PFE_CFG0) Gated input clock Position */
#define ISC_PFE_CFG0_GATED_Msk                (_U_(0x1) << ISC_PFE_CFG0_GATED_Pos)                 /**< (ISC_PFE_CFG0) Gated input clock Mask */
#define ISC_PFE_CFG0_GATED(value)             (ISC_PFE_CFG0_GATED_Msk & ((value) << ISC_PFE_CFG0_GATED_Pos))
#define ISC_PFE_CFG0_CCIR656_Pos              _U_(9)                                               /**< (ISC_PFE_CFG0) CCIR656 input mode Position */
#define ISC_PFE_CFG0_CCIR656_Msk              (_U_(0x1) << ISC_PFE_CFG0_CCIR656_Pos)               /**< (ISC_PFE_CFG0) CCIR656 input mode Mask */
#define ISC_PFE_CFG0_CCIR656(value)           (ISC_PFE_CFG0_CCIR656_Msk & ((value) << ISC_PFE_CFG0_CCIR656_Pos))
#define ISC_PFE_CFG0_CCIR_CRC_Pos             _U_(10)                                              /**< (ISC_PFE_CFG0) CCIR656 CRC Decoder Position */
#define ISC_PFE_CFG0_CCIR_CRC_Msk             (_U_(0x1) << ISC_PFE_CFG0_CCIR_CRC_Pos)              /**< (ISC_PFE_CFG0) CCIR656 CRC Decoder Mask */
#define ISC_PFE_CFG0_CCIR_CRC(value)          (ISC_PFE_CFG0_CCIR_CRC_Msk & ((value) << ISC_PFE_CFG0_CCIR_CRC_Pos))
#define ISC_PFE_CFG0_CCIR10_8N_Pos            _U_(11)                                              /**< (ISC_PFE_CFG0) CCIR 10 bits or 8 bits Position */
#define ISC_PFE_CFG0_CCIR10_8N_Msk            (_U_(0x1) << ISC_PFE_CFG0_CCIR10_8N_Pos)             /**< (ISC_PFE_CFG0) CCIR 10 bits or 8 bits Mask */
#define ISC_PFE_CFG0_CCIR10_8N(value)         (ISC_PFE_CFG0_CCIR10_8N_Msk & ((value) << ISC_PFE_CFG0_CCIR10_8N_Pos))
#define ISC_PFE_CFG0_COLEN_Pos                _U_(12)                                              /**< (ISC_PFE_CFG0) Column Cropping Enable Position */
#define ISC_PFE_CFG0_COLEN_Msk                (_U_(0x1) << ISC_PFE_CFG0_COLEN_Pos)                 /**< (ISC_PFE_CFG0) Column Cropping Enable Mask */
#define ISC_PFE_CFG0_COLEN(value)             (ISC_PFE_CFG0_COLEN_Msk & ((value) << ISC_PFE_CFG0_COLEN_Pos))
#define ISC_PFE_CFG0_ROWEN_Pos                _U_(13)                                              /**< (ISC_PFE_CFG0) Row Cropping Enable Position */
#define ISC_PFE_CFG0_ROWEN_Msk                (_U_(0x1) << ISC_PFE_CFG0_ROWEN_Pos)                 /**< (ISC_PFE_CFG0) Row Cropping Enable Mask */
#define ISC_PFE_CFG0_ROWEN(value)             (ISC_PFE_CFG0_ROWEN_Msk & ((value) << ISC_PFE_CFG0_ROWEN_Pos))
#define ISC_PFE_CFG0_SKIPCNT_Pos              _U_(16)                                              /**< (ISC_PFE_CFG0) Frame Skipping Counter Position */
#define ISC_PFE_CFG0_SKIPCNT_Msk              (_U_(0xFF) << ISC_PFE_CFG0_SKIPCNT_Pos)              /**< (ISC_PFE_CFG0) Frame Skipping Counter Mask */
#define ISC_PFE_CFG0_SKIPCNT(value)           (ISC_PFE_CFG0_SKIPCNT_Msk & ((value) << ISC_PFE_CFG0_SKIPCNT_Pos))
#define ISC_PFE_CFG0_CCIR_REP_Pos             _U_(27)                                              /**< (ISC_PFE_CFG0) CCIR Replication Position */
#define ISC_PFE_CFG0_CCIR_REP_Msk             (_U_(0x1) << ISC_PFE_CFG0_CCIR_REP_Pos)              /**< (ISC_PFE_CFG0) CCIR Replication Mask */
#define ISC_PFE_CFG0_CCIR_REP(value)          (ISC_PFE_CFG0_CCIR_REP_Msk & ((value) << ISC_PFE_CFG0_CCIR_REP_Pos))
#define ISC_PFE_CFG0_BPS_Pos                  _U_(28)                                              /**< (ISC_PFE_CFG0) Bits Per Sample Position */
#define ISC_PFE_CFG0_BPS_Msk                  (_U_(0x7) << ISC_PFE_CFG0_BPS_Pos)                   /**< (ISC_PFE_CFG0) Bits Per Sample Mask */
#define ISC_PFE_CFG0_BPS(value)               (ISC_PFE_CFG0_BPS_Msk & ((value) << ISC_PFE_CFG0_BPS_Pos))
#define   ISC_PFE_CFG0_BPS_TWELVE_Val         _U_(0x0)                                             /**< (ISC_PFE_CFG0) 12-bit input  */
#define   ISC_PFE_CFG0_BPS_ELEVEN_Val         _U_(0x1)                                             /**< (ISC_PFE_CFG0) 11-bit input  */
#define   ISC_PFE_CFG0_BPS_TEN_Val            _U_(0x2)                                             /**< (ISC_PFE_CFG0) 10-bit input  */
#define   ISC_PFE_CFG0_BPS_NINE_Val           _U_(0x3)                                             /**< (ISC_PFE_CFG0) 9-bit input  */
#define   ISC_PFE_CFG0_BPS_EIGHT_Val          _U_(0x4)                                             /**< (ISC_PFE_CFG0) 8-bit input  */
#define ISC_PFE_CFG0_BPS_TWELVE               (ISC_PFE_CFG0_BPS_TWELVE_Val << ISC_PFE_CFG0_BPS_Pos) /**< (ISC_PFE_CFG0) 12-bit input Position  */
#define ISC_PFE_CFG0_BPS_ELEVEN               (ISC_PFE_CFG0_BPS_ELEVEN_Val << ISC_PFE_CFG0_BPS_Pos) /**< (ISC_PFE_CFG0) 11-bit input Position  */
#define ISC_PFE_CFG0_BPS_TEN                  (ISC_PFE_CFG0_BPS_TEN_Val << ISC_PFE_CFG0_BPS_Pos)   /**< (ISC_PFE_CFG0) 10-bit input Position  */
#define ISC_PFE_CFG0_BPS_NINE                 (ISC_PFE_CFG0_BPS_NINE_Val << ISC_PFE_CFG0_BPS_Pos)  /**< (ISC_PFE_CFG0) 9-bit input Position  */
#define ISC_PFE_CFG0_BPS_EIGHT                (ISC_PFE_CFG0_BPS_EIGHT_Val << ISC_PFE_CFG0_BPS_Pos) /**< (ISC_PFE_CFG0) 8-bit input Position  */
#define ISC_PFE_CFG0_REP_Pos                  _U_(31)                                              /**< (ISC_PFE_CFG0) Up Multiply with Replication Position */
#define ISC_PFE_CFG0_REP_Msk                  (_U_(0x1) << ISC_PFE_CFG0_REP_Pos)                   /**< (ISC_PFE_CFG0) Up Multiply with Replication Mask */
#define ISC_PFE_CFG0_REP(value)               (ISC_PFE_CFG0_REP_Msk & ((value) << ISC_PFE_CFG0_REP_Pos))
#define ISC_PFE_CFG0_Msk                      _U_(0xF8FF3FFF)                                      /**< (ISC_PFE_CFG0) Register Mask  */

#define ISC_PFE_CFG0_CCIR_Pos                 _U_(9)                                               /**< (ISC_PFE_CFG0 Position) CCIR656 input mode */
#define ISC_PFE_CFG0_CCIR_Msk                 (_U_(0x1) << ISC_PFE_CFG0_CCIR_Pos)                  /**< (ISC_PFE_CFG0 Mask) CCIR */
#define ISC_PFE_CFG0_CCIR(value)              (ISC_PFE_CFG0_CCIR_Msk & ((value) << ISC_PFE_CFG0_CCIR_Pos)) 

/* -------- ISC_PFE_CFG1 : (ISC Offset: 0x10) (R/W 32) Parallel Front End Configuration 1 Register -------- */
#define ISC_PFE_CFG1_COLMIN_Pos               _U_(0)                                               /**< (ISC_PFE_CFG1) Column Minimum Limit Position */
#define ISC_PFE_CFG1_COLMIN_Msk               (_U_(0xFFFF) << ISC_PFE_CFG1_COLMIN_Pos)             /**< (ISC_PFE_CFG1) Column Minimum Limit Mask */
#define ISC_PFE_CFG1_COLMIN(value)            (ISC_PFE_CFG1_COLMIN_Msk & ((value) << ISC_PFE_CFG1_COLMIN_Pos))
#define ISC_PFE_CFG1_COLMAX_Pos               _U_(16)                                              /**< (ISC_PFE_CFG1) Column Maximum Limit Position */
#define ISC_PFE_CFG1_COLMAX_Msk               (_U_(0xFFFF) << ISC_PFE_CFG1_COLMAX_Pos)             /**< (ISC_PFE_CFG1) Column Maximum Limit Mask */
#define ISC_PFE_CFG1_COLMAX(value)            (ISC_PFE_CFG1_COLMAX_Msk & ((value) << ISC_PFE_CFG1_COLMAX_Pos))
#define ISC_PFE_CFG1_Msk                      _U_(0xFFFFFFFF)                                      /**< (ISC_PFE_CFG1) Register Mask  */


/* -------- ISC_PFE_CFG2 : (ISC Offset: 0x14) (R/W 32) Parallel Front End Configuration 2 Register -------- */
#define ISC_PFE_CFG2_ROWMIN_Pos               _U_(0)                                               /**< (ISC_PFE_CFG2) Row Minimum Limit Position */
#define ISC_PFE_CFG2_ROWMIN_Msk               (_U_(0xFFFF) << ISC_PFE_CFG2_ROWMIN_Pos)             /**< (ISC_PFE_CFG2) Row Minimum Limit Mask */
#define ISC_PFE_CFG2_ROWMIN(value)            (ISC_PFE_CFG2_ROWMIN_Msk & ((value) << ISC_PFE_CFG2_ROWMIN_Pos))
#define ISC_PFE_CFG2_ROWMAX_Pos               _U_(16)                                              /**< (ISC_PFE_CFG2) Row Maximum Limit Position */
#define ISC_PFE_CFG2_ROWMAX_Msk               (_U_(0xFFFF) << ISC_PFE_CFG2_ROWMAX_Pos)             /**< (ISC_PFE_CFG2) Row Maximum Limit Mask */
#define ISC_PFE_CFG2_ROWMAX(value)            (ISC_PFE_CFG2_ROWMAX_Msk & ((value) << ISC_PFE_CFG2_ROWMAX_Pos))
#define ISC_PFE_CFG2_Msk                      _U_(0xFFFFFFFF)                                      /**< (ISC_PFE_CFG2) Register Mask  */


/* -------- ISC_CLKEN : (ISC Offset: 0x18) ( /W 32) Clock Enable Register -------- */
#define ISC_CLKEN_ICEN_Pos                    _U_(0)                                               /**< (ISC_CLKEN) ISP Clock Enable Position */
#define ISC_CLKEN_ICEN_Msk                    (_U_(0x1) << ISC_CLKEN_ICEN_Pos)                     /**< (ISC_CLKEN) ISP Clock Enable Mask */
#define ISC_CLKEN_ICEN(value)                 (ISC_CLKEN_ICEN_Msk & ((value) << ISC_CLKEN_ICEN_Pos))
#define ISC_CLKEN_MCEN_Pos                    _U_(1)                                               /**< (ISC_CLKEN) Master Clock Enable Position */
#define ISC_CLKEN_MCEN_Msk                    (_U_(0x1) << ISC_CLKEN_MCEN_Pos)                     /**< (ISC_CLKEN) Master Clock Enable Mask */
#define ISC_CLKEN_MCEN(value)                 (ISC_CLKEN_MCEN_Msk & ((value) << ISC_CLKEN_MCEN_Pos))
#define ISC_CLKEN_Msk                         _U_(0x00000003)                                      /**< (ISC_CLKEN) Register Mask  */


/* -------- ISC_CLKDIS : (ISC Offset: 0x1C) ( /W 32) Clock Disable Register -------- */
#define ISC_CLKDIS_ICDIS_Pos                  _U_(0)                                               /**< (ISC_CLKDIS) ISP Clock Disable Position */
#define ISC_CLKDIS_ICDIS_Msk                  (_U_(0x1) << ISC_CLKDIS_ICDIS_Pos)                   /**< (ISC_CLKDIS) ISP Clock Disable Mask */
#define ISC_CLKDIS_ICDIS(value)               (ISC_CLKDIS_ICDIS_Msk & ((value) << ISC_CLKDIS_ICDIS_Pos))
#define ISC_CLKDIS_MCDIS_Pos                  _U_(1)                                               /**< (ISC_CLKDIS) Master Clock Disable Position */
#define ISC_CLKDIS_MCDIS_Msk                  (_U_(0x1) << ISC_CLKDIS_MCDIS_Pos)                   /**< (ISC_CLKDIS) Master Clock Disable Mask */
#define ISC_CLKDIS_MCDIS(value)               (ISC_CLKDIS_MCDIS_Msk & ((value) << ISC_CLKDIS_MCDIS_Pos))
#define ISC_CLKDIS_ICSWRST_Pos                _U_(8)                                               /**< (ISC_CLKDIS) ISP Clock Software Reset Position */
#define ISC_CLKDIS_ICSWRST_Msk                (_U_(0x1) << ISC_CLKDIS_ICSWRST_Pos)                 /**< (ISC_CLKDIS) ISP Clock Software Reset Mask */
#define ISC_CLKDIS_ICSWRST(value)             (ISC_CLKDIS_ICSWRST_Msk & ((value) << ISC_CLKDIS_ICSWRST_Pos))
#define ISC_CLKDIS_MCSWRST_Pos                _U_(9)                                               /**< (ISC_CLKDIS) Master Clock Software Reset Position */
#define ISC_CLKDIS_MCSWRST_Msk                (_U_(0x1) << ISC_CLKDIS_MCSWRST_Pos)                 /**< (ISC_CLKDIS) Master Clock Software Reset Mask */
#define ISC_CLKDIS_MCSWRST(value)             (ISC_CLKDIS_MCSWRST_Msk & ((value) << ISC_CLKDIS_MCSWRST_Pos))
#define ISC_CLKDIS_Msk                        _U_(0x00000303)                                      /**< (ISC_CLKDIS) Register Mask  */


/* -------- ISC_CLKSR : (ISC Offset: 0x20) ( R/ 32) Clock Status Register -------- */
#define ISC_CLKSR_ICSR_Pos                    _U_(0)                                               /**< (ISC_CLKSR) ISP Clock Status Register Position */
#define ISC_CLKSR_ICSR_Msk                    (_U_(0x1) << ISC_CLKSR_ICSR_Pos)                     /**< (ISC_CLKSR) ISP Clock Status Register Mask */
#define ISC_CLKSR_ICSR(value)                 (ISC_CLKSR_ICSR_Msk & ((value) << ISC_CLKSR_ICSR_Pos))
#define ISC_CLKSR_MCSR_Pos                    _U_(1)                                               /**< (ISC_CLKSR) Master Clock Status Register Position */
#define ISC_CLKSR_MCSR_Msk                    (_U_(0x1) << ISC_CLKSR_MCSR_Pos)                     /**< (ISC_CLKSR) Master Clock Status Register Mask */
#define ISC_CLKSR_MCSR(value)                 (ISC_CLKSR_MCSR_Msk & ((value) << ISC_CLKSR_MCSR_Pos))
#define ISC_CLKSR_SIP_Pos                     _U_(31)                                              /**< (ISC_CLKSR) Synchronization In Progress Position */
#define ISC_CLKSR_SIP_Msk                     (_U_(0x1) << ISC_CLKSR_SIP_Pos)                      /**< (ISC_CLKSR) Synchronization In Progress Mask */
#define ISC_CLKSR_SIP(value)                  (ISC_CLKSR_SIP_Msk & ((value) << ISC_CLKSR_SIP_Pos))
#define ISC_CLKSR_Msk                         _U_(0x80000003)                                      /**< (ISC_CLKSR) Register Mask  */


/* -------- ISC_CLKCFG : (ISC Offset: 0x24) (R/W 32) Clock Configuration Register -------- */
#define ISC_CLKCFG_ICDIV_Pos                  _U_(0)                                               /**< (ISC_CLKCFG) ISP Clock Divider Position */
#define ISC_CLKCFG_ICDIV_Msk                  (_U_(0xFF) << ISC_CLKCFG_ICDIV_Pos)                  /**< (ISC_CLKCFG) ISP Clock Divider Mask */
#define ISC_CLKCFG_ICDIV(value)               (ISC_CLKCFG_ICDIV_Msk & ((value) << ISC_CLKCFG_ICDIV_Pos))
#define ISC_CLKCFG_ICSEL_Pos                  _U_(8)                                               /**< (ISC_CLKCFG) ISP Clock Selection Position */
#define ISC_CLKCFG_ICSEL_Msk                  (_U_(0x1) << ISC_CLKCFG_ICSEL_Pos)                   /**< (ISC_CLKCFG) ISP Clock Selection Mask */
#define ISC_CLKCFG_ICSEL(value)               (ISC_CLKCFG_ICSEL_Msk & ((value) << ISC_CLKCFG_ICSEL_Pos))
#define ISC_CLKCFG_MCDIV_Pos                  _U_(16)                                              /**< (ISC_CLKCFG) Master Clock Divider Position */
#define ISC_CLKCFG_MCDIV_Msk                  (_U_(0xFF) << ISC_CLKCFG_MCDIV_Pos)                  /**< (ISC_CLKCFG) Master Clock Divider Mask */
#define ISC_CLKCFG_MCDIV(value)               (ISC_CLKCFG_MCDIV_Msk & ((value) << ISC_CLKCFG_MCDIV_Pos))
#define ISC_CLKCFG_MCSEL_Pos                  _U_(24)                                              /**< (ISC_CLKCFG) Master Clock Reference Clock Selection Position */
#define ISC_CLKCFG_MCSEL_Msk                  (_U_(0x3) << ISC_CLKCFG_MCSEL_Pos)                   /**< (ISC_CLKCFG) Master Clock Reference Clock Selection Mask */
#define ISC_CLKCFG_MCSEL(value)               (ISC_CLKCFG_MCSEL_Msk & ((value) << ISC_CLKCFG_MCSEL_Pos))
#define ISC_CLKCFG_Msk                        _U_(0x03FF01FF)                                      /**< (ISC_CLKCFG) Register Mask  */


/* -------- ISC_INTEN : (ISC Offset: 0x28) ( /W 32) Interrupt Enable Register -------- */
#define ISC_INTEN_VD_Pos                      _U_(0)                                               /**< (ISC_INTEN) Vertical Synchronization Detection Interrupt Enable Position */
#define ISC_INTEN_VD_Msk                      (_U_(0x1) << ISC_INTEN_VD_Pos)                       /**< (ISC_INTEN) Vertical Synchronization Detection Interrupt Enable Mask */
#define ISC_INTEN_VD(value)                   (ISC_INTEN_VD_Msk & ((value) << ISC_INTEN_VD_Pos))  
#define ISC_INTEN_HD_Pos                      _U_(1)                                               /**< (ISC_INTEN) Horizontal Synchronization Detection Interrupt Enable Position */
#define ISC_INTEN_HD_Msk                      (_U_(0x1) << ISC_INTEN_HD_Pos)                       /**< (ISC_INTEN) Horizontal Synchronization Detection Interrupt Enable Mask */
#define ISC_INTEN_HD(value)                   (ISC_INTEN_HD_Msk & ((value) << ISC_INTEN_HD_Pos))  
#define ISC_INTEN_SWRST_Pos                   _U_(4)                                               /**< (ISC_INTEN) Software Reset Completed Interrupt Enable Position */
#define ISC_INTEN_SWRST_Msk                   (_U_(0x1) << ISC_INTEN_SWRST_Pos)                    /**< (ISC_INTEN) Software Reset Completed Interrupt Enable Mask */
#define ISC_INTEN_SWRST(value)                (ISC_INTEN_SWRST_Msk & ((value) << ISC_INTEN_SWRST_Pos))
#define ISC_INTEN_DIS_Pos                     _U_(5)                                               /**< (ISC_INTEN) Disable Completed Interrupt Enable Position */
#define ISC_INTEN_DIS_Msk                     (_U_(0x1) << ISC_INTEN_DIS_Pos)                      /**< (ISC_INTEN) Disable Completed Interrupt Enable Mask */
#define ISC_INTEN_DIS(value)                  (ISC_INTEN_DIS_Msk & ((value) << ISC_INTEN_DIS_Pos))
#define ISC_INTEN_DDONE_Pos                   _U_(8)                                               /**< (ISC_INTEN) DMA Done Interrupt Enable Position */
#define ISC_INTEN_DDONE_Msk                   (_U_(0x1) << ISC_INTEN_DDONE_Pos)                    /**< (ISC_INTEN) DMA Done Interrupt Enable Mask */
#define ISC_INTEN_DDONE(value)                (ISC_INTEN_DDONE_Msk & ((value) << ISC_INTEN_DDONE_Pos))
#define ISC_INTEN_LDONE_Pos                   _U_(9)                                               /**< (ISC_INTEN) DMA List Done Interrupt Enable Position */
#define ISC_INTEN_LDONE_Msk                   (_U_(0x1) << ISC_INTEN_LDONE_Pos)                    /**< (ISC_INTEN) DMA List Done Interrupt Enable Mask */
#define ISC_INTEN_LDONE(value)                (ISC_INTEN_LDONE_Msk & ((value) << ISC_INTEN_LDONE_Pos))
#define ISC_INTEN_HISDONE_Pos                 _U_(12)                                              /**< (ISC_INTEN) Histogram Completed Interrupt Enable Position */
#define ISC_INTEN_HISDONE_Msk                 (_U_(0x1) << ISC_INTEN_HISDONE_Pos)                  /**< (ISC_INTEN) Histogram Completed Interrupt Enable Mask */
#define ISC_INTEN_HISDONE(value)              (ISC_INTEN_HISDONE_Msk & ((value) << ISC_INTEN_HISDONE_Pos))
#define ISC_INTEN_HISCLR_Pos                  _U_(13)                                              /**< (ISC_INTEN) Histogram Clear Interrupt Enable Position */
#define ISC_INTEN_HISCLR_Msk                  (_U_(0x1) << ISC_INTEN_HISCLR_Pos)                   /**< (ISC_INTEN) Histogram Clear Interrupt Enable Mask */
#define ISC_INTEN_HISCLR(value)               (ISC_INTEN_HISCLR_Msk & ((value) << ISC_INTEN_HISCLR_Pos))
#define ISC_INTEN_WERR_Pos                    _U_(16)                                              /**< (ISC_INTEN) Write Channel Error Interrupt Enable Position */
#define ISC_INTEN_WERR_Msk                    (_U_(0x1) << ISC_INTEN_WERR_Pos)                     /**< (ISC_INTEN) Write Channel Error Interrupt Enable Mask */
#define ISC_INTEN_WERR(value)                 (ISC_INTEN_WERR_Msk & ((value) << ISC_INTEN_WERR_Pos))
#define ISC_INTEN_RERR_Pos                    _U_(20)                                              /**< (ISC_INTEN) Read Channel Error Interrupt Enable Position */
#define ISC_INTEN_RERR_Msk                    (_U_(0x1) << ISC_INTEN_RERR_Pos)                     /**< (ISC_INTEN) Read Channel Error Interrupt Enable Mask */
#define ISC_INTEN_RERR(value)                 (ISC_INTEN_RERR_Msk & ((value) << ISC_INTEN_RERR_Pos))
#define ISC_INTEN_VFPOV_Pos                   _U_(24)                                              /**< (ISC_INTEN) Vertical Front Porch Overflow Interrupt Enable Position */
#define ISC_INTEN_VFPOV_Msk                   (_U_(0x1) << ISC_INTEN_VFPOV_Pos)                    /**< (ISC_INTEN) Vertical Front Porch Overflow Interrupt Enable Mask */
#define ISC_INTEN_VFPOV(value)                (ISC_INTEN_VFPOV_Msk & ((value) << ISC_INTEN_VFPOV_Pos))
#define ISC_INTEN_DAOV_Pos                    _U_(25)                                              /**< (ISC_INTEN) Data Overflow Interrupt Enable Position */
#define ISC_INTEN_DAOV_Msk                    (_U_(0x1) << ISC_INTEN_DAOV_Pos)                     /**< (ISC_INTEN) Data Overflow Interrupt Enable Mask */
#define ISC_INTEN_DAOV(value)                 (ISC_INTEN_DAOV_Msk & ((value) << ISC_INTEN_DAOV_Pos))
#define ISC_INTEN_VDTO_Pos                    _U_(26)                                              /**< (ISC_INTEN) Vertical Synchronization Timeout Interrupt Enable Position */
#define ISC_INTEN_VDTO_Msk                    (_U_(0x1) << ISC_INTEN_VDTO_Pos)                     /**< (ISC_INTEN) Vertical Synchronization Timeout Interrupt Enable Mask */
#define ISC_INTEN_VDTO(value)                 (ISC_INTEN_VDTO_Msk & ((value) << ISC_INTEN_VDTO_Pos))
#define ISC_INTEN_HDTO_Pos                    _U_(27)                                              /**< (ISC_INTEN) Horizontal Synchronization Timeout Interrupt Enable Position */
#define ISC_INTEN_HDTO_Msk                    (_U_(0x1) << ISC_INTEN_HDTO_Pos)                     /**< (ISC_INTEN) Horizontal Synchronization Timeout Interrupt Enable Mask */
#define ISC_INTEN_HDTO(value)                 (ISC_INTEN_HDTO_Msk & ((value) << ISC_INTEN_HDTO_Pos))
#define ISC_INTEN_CCIRERR_Pos                 _U_(28)                                              /**< (ISC_INTEN) CCIR Decoder Error Interrupt Enable Position */
#define ISC_INTEN_CCIRERR_Msk                 (_U_(0x1) << ISC_INTEN_CCIRERR_Pos)                  /**< (ISC_INTEN) CCIR Decoder Error Interrupt Enable Mask */
#define ISC_INTEN_CCIRERR(value)              (ISC_INTEN_CCIRERR_Msk & ((value) << ISC_INTEN_CCIRERR_Pos))
#define ISC_INTEN_Msk                         _U_(0x1F113333)                                      /**< (ISC_INTEN) Register Mask  */


/* -------- ISC_INTDIS : (ISC Offset: 0x2C) ( /W 32) Interrupt Disable Register -------- */
#define ISC_INTDIS_VD_Pos                     _U_(0)                                               /**< (ISC_INTDIS) Vertical Synchronization Detection Interrupt Disable Position */
#define ISC_INTDIS_VD_Msk                     (_U_(0x1) << ISC_INTDIS_VD_Pos)                      /**< (ISC_INTDIS) Vertical Synchronization Detection Interrupt Disable Mask */
#define ISC_INTDIS_VD(value)                  (ISC_INTDIS_VD_Msk & ((value) << ISC_INTDIS_VD_Pos))
#define ISC_INTDIS_HD_Pos                     _U_(1)                                               /**< (ISC_INTDIS) Horizontal Synchronization Detection Interrupt Disable Position */
#define ISC_INTDIS_HD_Msk                     (_U_(0x1) << ISC_INTDIS_HD_Pos)                      /**< (ISC_INTDIS) Horizontal Synchronization Detection Interrupt Disable Mask */
#define ISC_INTDIS_HD(value)                  (ISC_INTDIS_HD_Msk & ((value) << ISC_INTDIS_HD_Pos))
#define ISC_INTDIS_SWRST_Pos                  _U_(4)                                               /**< (ISC_INTDIS) Software Reset Completed Interrupt Disable Position */
#define ISC_INTDIS_SWRST_Msk                  (_U_(0x1) << ISC_INTDIS_SWRST_Pos)                   /**< (ISC_INTDIS) Software Reset Completed Interrupt Disable Mask */
#define ISC_INTDIS_SWRST(value)               (ISC_INTDIS_SWRST_Msk & ((value) << ISC_INTDIS_SWRST_Pos))
#define ISC_INTDIS_DIS_Pos                    _U_(5)                                               /**< (ISC_INTDIS) Disable Completed Interrupt Disable Position */
#define ISC_INTDIS_DIS_Msk                    (_U_(0x1) << ISC_INTDIS_DIS_Pos)                     /**< (ISC_INTDIS) Disable Completed Interrupt Disable Mask */
#define ISC_INTDIS_DIS(value)                 (ISC_INTDIS_DIS_Msk & ((value) << ISC_INTDIS_DIS_Pos))
#define ISC_INTDIS_DDONE_Pos                  _U_(8)                                               /**< (ISC_INTDIS) DMA Done Interrupt Disable Position */
#define ISC_INTDIS_DDONE_Msk                  (_U_(0x1) << ISC_INTDIS_DDONE_Pos)                   /**< (ISC_INTDIS) DMA Done Interrupt Disable Mask */
#define ISC_INTDIS_DDONE(value)               (ISC_INTDIS_DDONE_Msk & ((value) << ISC_INTDIS_DDONE_Pos))
#define ISC_INTDIS_LDONE_Pos                  _U_(9)                                               /**< (ISC_INTDIS) DMA List Done Interrupt Disable Position */
#define ISC_INTDIS_LDONE_Msk                  (_U_(0x1) << ISC_INTDIS_LDONE_Pos)                   /**< (ISC_INTDIS) DMA List Done Interrupt Disable Mask */
#define ISC_INTDIS_LDONE(value)               (ISC_INTDIS_LDONE_Msk & ((value) << ISC_INTDIS_LDONE_Pos))
#define ISC_INTDIS_HISDONE_Pos                _U_(12)                                              /**< (ISC_INTDIS) Histogram Completed Interrupt Disable Position */
#define ISC_INTDIS_HISDONE_Msk                (_U_(0x1) << ISC_INTDIS_HISDONE_Pos)                 /**< (ISC_INTDIS) Histogram Completed Interrupt Disable Mask */
#define ISC_INTDIS_HISDONE(value)             (ISC_INTDIS_HISDONE_Msk & ((value) << ISC_INTDIS_HISDONE_Pos))
#define ISC_INTDIS_HISCLR_Pos                 _U_(13)                                              /**< (ISC_INTDIS) Histogram Clear Interrupt Disable Position */
#define ISC_INTDIS_HISCLR_Msk                 (_U_(0x1) << ISC_INTDIS_HISCLR_Pos)                  /**< (ISC_INTDIS) Histogram Clear Interrupt Disable Mask */
#define ISC_INTDIS_HISCLR(value)              (ISC_INTDIS_HISCLR_Msk & ((value) << ISC_INTDIS_HISCLR_Pos))
#define ISC_INTDIS_WERR_Pos                   _U_(16)                                              /**< (ISC_INTDIS) Write Channel Error Interrupt Disable Position */
#define ISC_INTDIS_WERR_Msk                   (_U_(0x1) << ISC_INTDIS_WERR_Pos)                    /**< (ISC_INTDIS) Write Channel Error Interrupt Disable Mask */
#define ISC_INTDIS_WERR(value)                (ISC_INTDIS_WERR_Msk & ((value) << ISC_INTDIS_WERR_Pos))
#define ISC_INTDIS_RERR_Pos                   _U_(20)                                              /**< (ISC_INTDIS) Read Channel Error Interrupt Disable Position */
#define ISC_INTDIS_RERR_Msk                   (_U_(0x1) << ISC_INTDIS_RERR_Pos)                    /**< (ISC_INTDIS) Read Channel Error Interrupt Disable Mask */
#define ISC_INTDIS_RERR(value)                (ISC_INTDIS_RERR_Msk & ((value) << ISC_INTDIS_RERR_Pos))
#define ISC_INTDIS_VFPOV_Pos                  _U_(24)                                              /**< (ISC_INTDIS) Vertical Front Porch Overflow Interrupt Disable Position */
#define ISC_INTDIS_VFPOV_Msk                  (_U_(0x1) << ISC_INTDIS_VFPOV_Pos)                   /**< (ISC_INTDIS) Vertical Front Porch Overflow Interrupt Disable Mask */
#define ISC_INTDIS_VFPOV(value)               (ISC_INTDIS_VFPOV_Msk & ((value) << ISC_INTDIS_VFPOV_Pos))
#define ISC_INTDIS_DAOV_Pos                   _U_(25)                                              /**< (ISC_INTDIS) Data Overflow Interrupt Disable Position */
#define ISC_INTDIS_DAOV_Msk                   (_U_(0x1) << ISC_INTDIS_DAOV_Pos)                    /**< (ISC_INTDIS) Data Overflow Interrupt Disable Mask */
#define ISC_INTDIS_DAOV(value)                (ISC_INTDIS_DAOV_Msk & ((value) << ISC_INTDIS_DAOV_Pos))
#define ISC_INTDIS_VDTO_Pos                   _U_(26)                                              /**< (ISC_INTDIS) Vertical Synchronization Timeout Interrupt Disable Position */
#define ISC_INTDIS_VDTO_Msk                   (_U_(0x1) << ISC_INTDIS_VDTO_Pos)                    /**< (ISC_INTDIS) Vertical Synchronization Timeout Interrupt Disable Mask */
#define ISC_INTDIS_VDTO(value)                (ISC_INTDIS_VDTO_Msk & ((value) << ISC_INTDIS_VDTO_Pos))
#define ISC_INTDIS_HDTO_Pos                   _U_(27)                                              /**< (ISC_INTDIS) Horizontal Synchronization Timeout Interrupt Disable Position */
#define ISC_INTDIS_HDTO_Msk                   (_U_(0x1) << ISC_INTDIS_HDTO_Pos)                    /**< (ISC_INTDIS) Horizontal Synchronization Timeout Interrupt Disable Mask */
#define ISC_INTDIS_HDTO(value)                (ISC_INTDIS_HDTO_Msk & ((value) << ISC_INTDIS_HDTO_Pos))
#define ISC_INTDIS_CCIRERR_Pos                _U_(28)                                              /**< (ISC_INTDIS) CCIR Decoder Error Interrupt Disable Position */
#define ISC_INTDIS_CCIRERR_Msk                (_U_(0x1) << ISC_INTDIS_CCIRERR_Pos)                 /**< (ISC_INTDIS) CCIR Decoder Error Interrupt Disable Mask */
#define ISC_INTDIS_CCIRERR(value)             (ISC_INTDIS_CCIRERR_Msk & ((value) << ISC_INTDIS_CCIRERR_Pos))
#define ISC_INTDIS_Msk                        _U_(0x1F113333)                                      /**< (ISC_INTDIS) Register Mask  */


/* -------- ISC_INTMASK : (ISC Offset: 0x30) ( R/ 32) Interrupt Mask Register -------- */
#define ISC_INTMASK_VD_Pos                    _U_(0)                                               /**< (ISC_INTMASK) Vertical Synchronization Detection Interrupt Mask Position */
#define ISC_INTMASK_VD_Msk                    (_U_(0x1) << ISC_INTMASK_VD_Pos)                     /**< (ISC_INTMASK) Vertical Synchronization Detection Interrupt Mask Mask */
#define ISC_INTMASK_VD(value)                 (ISC_INTMASK_VD_Msk & ((value) << ISC_INTMASK_VD_Pos))
#define ISC_INTMASK_HD_Pos                    _U_(1)                                               /**< (ISC_INTMASK) Horizontal Synchronization Detection Interrupt Mask Position */
#define ISC_INTMASK_HD_Msk                    (_U_(0x1) << ISC_INTMASK_HD_Pos)                     /**< (ISC_INTMASK) Horizontal Synchronization Detection Interrupt Mask Mask */
#define ISC_INTMASK_HD(value)                 (ISC_INTMASK_HD_Msk & ((value) << ISC_INTMASK_HD_Pos))
#define ISC_INTMASK_SWRST_Pos                 _U_(4)                                               /**< (ISC_INTMASK) Software Reset Completed Interrupt Mask Position */
#define ISC_INTMASK_SWRST_Msk                 (_U_(0x1) << ISC_INTMASK_SWRST_Pos)                  /**< (ISC_INTMASK) Software Reset Completed Interrupt Mask Mask */
#define ISC_INTMASK_SWRST(value)              (ISC_INTMASK_SWRST_Msk & ((value) << ISC_INTMASK_SWRST_Pos))
#define ISC_INTMASK_DIS_Pos                   _U_(5)                                               /**< (ISC_INTMASK) Disable Completed Interrupt Mask Position */
#define ISC_INTMASK_DIS_Msk                   (_U_(0x1) << ISC_INTMASK_DIS_Pos)                    /**< (ISC_INTMASK) Disable Completed Interrupt Mask Mask */
#define ISC_INTMASK_DIS(value)                (ISC_INTMASK_DIS_Msk & ((value) << ISC_INTMASK_DIS_Pos))
#define ISC_INTMASK_DDONE_Pos                 _U_(8)                                               /**< (ISC_INTMASK) DMA Done Interrupt Mask Position */
#define ISC_INTMASK_DDONE_Msk                 (_U_(0x1) << ISC_INTMASK_DDONE_Pos)                  /**< (ISC_INTMASK) DMA Done Interrupt Mask Mask */
#define ISC_INTMASK_DDONE(value)              (ISC_INTMASK_DDONE_Msk & ((value) << ISC_INTMASK_DDONE_Pos))
#define ISC_INTMASK_LDONE_Pos                 _U_(9)                                               /**< (ISC_INTMASK) DMA List Done Interrupt Mask Position */
#define ISC_INTMASK_LDONE_Msk                 (_U_(0x1) << ISC_INTMASK_LDONE_Pos)                  /**< (ISC_INTMASK) DMA List Done Interrupt Mask Mask */
#define ISC_INTMASK_LDONE(value)              (ISC_INTMASK_LDONE_Msk & ((value) << ISC_INTMASK_LDONE_Pos))
#define ISC_INTMASK_HISDONE_Pos               _U_(12)                                              /**< (ISC_INTMASK) Histogram Completed Interrupt Mask Position */
#define ISC_INTMASK_HISDONE_Msk               (_U_(0x1) << ISC_INTMASK_HISDONE_Pos)                /**< (ISC_INTMASK) Histogram Completed Interrupt Mask Mask */
#define ISC_INTMASK_HISDONE(value)            (ISC_INTMASK_HISDONE_Msk & ((value) << ISC_INTMASK_HISDONE_Pos))
#define ISC_INTMASK_HISCLR_Pos                _U_(13)                                              /**< (ISC_INTMASK) Histogram Clear Interrupt Mask Position */
#define ISC_INTMASK_HISCLR_Msk                (_U_(0x1) << ISC_INTMASK_HISCLR_Pos)                 /**< (ISC_INTMASK) Histogram Clear Interrupt Mask Mask */
#define ISC_INTMASK_HISCLR(value)             (ISC_INTMASK_HISCLR_Msk & ((value) << ISC_INTMASK_HISCLR_Pos))
#define ISC_INTMASK_WERR_Pos                  _U_(16)                                              /**< (ISC_INTMASK) Write Channel Error Interrupt Mask Position */
#define ISC_INTMASK_WERR_Msk                  (_U_(0x1) << ISC_INTMASK_WERR_Pos)                   /**< (ISC_INTMASK) Write Channel Error Interrupt Mask Mask */
#define ISC_INTMASK_WERR(value)               (ISC_INTMASK_WERR_Msk & ((value) << ISC_INTMASK_WERR_Pos))
#define ISC_INTMASK_RERR_Pos                  _U_(20)                                              /**< (ISC_INTMASK) Read Channel Error Interrupt Mask Position */
#define ISC_INTMASK_RERR_Msk                  (_U_(0x1) << ISC_INTMASK_RERR_Pos)                   /**< (ISC_INTMASK) Read Channel Error Interrupt Mask Mask */
#define ISC_INTMASK_RERR(value)               (ISC_INTMASK_RERR_Msk & ((value) << ISC_INTMASK_RERR_Pos))
#define ISC_INTMASK_VFPOV_Pos                 _U_(24)                                              /**< (ISC_INTMASK) Vertical Front Porch Overflow Interrupt Mask Position */
#define ISC_INTMASK_VFPOV_Msk                 (_U_(0x1) << ISC_INTMASK_VFPOV_Pos)                  /**< (ISC_INTMASK) Vertical Front Porch Overflow Interrupt Mask Mask */
#define ISC_INTMASK_VFPOV(value)              (ISC_INTMASK_VFPOV_Msk & ((value) << ISC_INTMASK_VFPOV_Pos))
#define ISC_INTMASK_DAOV_Pos                  _U_(25)                                              /**< (ISC_INTMASK) Data Overflow Interrupt Mask Position */
#define ISC_INTMASK_DAOV_Msk                  (_U_(0x1) << ISC_INTMASK_DAOV_Pos)                   /**< (ISC_INTMASK) Data Overflow Interrupt Mask Mask */
#define ISC_INTMASK_DAOV(value)               (ISC_INTMASK_DAOV_Msk & ((value) << ISC_INTMASK_DAOV_Pos))
#define ISC_INTMASK_VDTO_Pos                  _U_(26)                                              /**< (ISC_INTMASK) Vertical Synchronization Timeout Interrupt Mask Position */
#define ISC_INTMASK_VDTO_Msk                  (_U_(0x1) << ISC_INTMASK_VDTO_Pos)                   /**< (ISC_INTMASK) Vertical Synchronization Timeout Interrupt Mask Mask */
#define ISC_INTMASK_VDTO(value)               (ISC_INTMASK_VDTO_Msk & ((value) << ISC_INTMASK_VDTO_Pos))
#define ISC_INTMASK_HDTO_Pos                  _U_(27)                                              /**< (ISC_INTMASK) Horizontal Synchronization Timeout Interrupt Mask Position */
#define ISC_INTMASK_HDTO_Msk                  (_U_(0x1) << ISC_INTMASK_HDTO_Pos)                   /**< (ISC_INTMASK) Horizontal Synchronization Timeout Interrupt Mask Mask */
#define ISC_INTMASK_HDTO(value)               (ISC_INTMASK_HDTO_Msk & ((value) << ISC_INTMASK_HDTO_Pos))
#define ISC_INTMASK_CCIRERR_Pos               _U_(28)                                              /**< (ISC_INTMASK) CCIR Decoder Error Interrupt Mask Position */
#define ISC_INTMASK_CCIRERR_Msk               (_U_(0x1) << ISC_INTMASK_CCIRERR_Pos)                /**< (ISC_INTMASK) CCIR Decoder Error Interrupt Mask Mask */
#define ISC_INTMASK_CCIRERR(value)            (ISC_INTMASK_CCIRERR_Msk & ((value) << ISC_INTMASK_CCIRERR_Pos))
#define ISC_INTMASK_Msk                       _U_(0x1F113333)                                      /**< (ISC_INTMASK) Register Mask  */


/* -------- ISC_INTSR : (ISC Offset: 0x34) ( R/ 32) Interrupt Status Register -------- */
#define ISC_INTSR_VD_Pos                      _U_(0)                                               /**< (ISC_INTSR) Vertical Synchronization Detected Interrupt Position */
#define ISC_INTSR_VD_Msk                      (_U_(0x1) << ISC_INTSR_VD_Pos)                       /**< (ISC_INTSR) Vertical Synchronization Detected Interrupt Mask */
#define ISC_INTSR_VD(value)                   (ISC_INTSR_VD_Msk & ((value) << ISC_INTSR_VD_Pos))  
#define ISC_INTSR_HD_Pos                      _U_(1)                                               /**< (ISC_INTSR) Horizontal Synchronization Detected Interrupt Position */
#define ISC_INTSR_HD_Msk                      (_U_(0x1) << ISC_INTSR_HD_Pos)                       /**< (ISC_INTSR) Horizontal Synchronization Detected Interrupt Mask */
#define ISC_INTSR_HD(value)                   (ISC_INTSR_HD_Msk & ((value) << ISC_INTSR_HD_Pos))  
#define ISC_INTSR_SWRST_Pos                   _U_(4)                                               /**< (ISC_INTSR) Software Reset Completed Interrupt Position */
#define ISC_INTSR_SWRST_Msk                   (_U_(0x1) << ISC_INTSR_SWRST_Pos)                    /**< (ISC_INTSR) Software Reset Completed Interrupt Mask */
#define ISC_INTSR_SWRST(value)                (ISC_INTSR_SWRST_Msk & ((value) << ISC_INTSR_SWRST_Pos))
#define ISC_INTSR_DIS_Pos                     _U_(5)                                               /**< (ISC_INTSR) Disable Completed Interrupt Position */
#define ISC_INTSR_DIS_Msk                     (_U_(0x1) << ISC_INTSR_DIS_Pos)                      /**< (ISC_INTSR) Disable Completed Interrupt Mask */
#define ISC_INTSR_DIS(value)                  (ISC_INTSR_DIS_Msk & ((value) << ISC_INTSR_DIS_Pos))
#define ISC_INTSR_DDONE_Pos                   _U_(8)                                               /**< (ISC_INTSR) DMA Done Interrupt Position */
#define ISC_INTSR_DDONE_Msk                   (_U_(0x1) << ISC_INTSR_DDONE_Pos)                    /**< (ISC_INTSR) DMA Done Interrupt Mask */
#define ISC_INTSR_DDONE(value)                (ISC_INTSR_DDONE_Msk & ((value) << ISC_INTSR_DDONE_Pos))
#define ISC_INTSR_LDONE_Pos                   _U_(9)                                               /**< (ISC_INTSR) DMA List Done Interrupt Position */
#define ISC_INTSR_LDONE_Msk                   (_U_(0x1) << ISC_INTSR_LDONE_Pos)                    /**< (ISC_INTSR) DMA List Done Interrupt Mask */
#define ISC_INTSR_LDONE(value)                (ISC_INTSR_LDONE_Msk & ((value) << ISC_INTSR_LDONE_Pos))
#define ISC_INTSR_HISDONE_Pos                 _U_(12)                                              /**< (ISC_INTSR) Histogram Completed Interrupt Position */
#define ISC_INTSR_HISDONE_Msk                 (_U_(0x1) << ISC_INTSR_HISDONE_Pos)                  /**< (ISC_INTSR) Histogram Completed Interrupt Mask */
#define ISC_INTSR_HISDONE(value)              (ISC_INTSR_HISDONE_Msk & ((value) << ISC_INTSR_HISDONE_Pos))
#define ISC_INTSR_HISCLR_Pos                  _U_(13)                                              /**< (ISC_INTSR) Histogram Clear Interrupt Position */
#define ISC_INTSR_HISCLR_Msk                  (_U_(0x1) << ISC_INTSR_HISCLR_Pos)                   /**< (ISC_INTSR) Histogram Clear Interrupt Mask */
#define ISC_INTSR_HISCLR(value)               (ISC_INTSR_HISCLR_Msk & ((value) << ISC_INTSR_HISCLR_Pos))
#define ISC_INTSR_WERR_Pos                    _U_(16)                                              /**< (ISC_INTSR) Write Channel Error Interrupt Position */
#define ISC_INTSR_WERR_Msk                    (_U_(0x1) << ISC_INTSR_WERR_Pos)                     /**< (ISC_INTSR) Write Channel Error Interrupt Mask */
#define ISC_INTSR_WERR(value)                 (ISC_INTSR_WERR_Msk & ((value) << ISC_INTSR_WERR_Pos))
#define ISC_INTSR_WERRID_Pos                  _U_(17)                                              /**< (ISC_INTSR) Write Channel Error Identifier Position */
#define ISC_INTSR_WERRID_Msk                  (_U_(0x3) << ISC_INTSR_WERRID_Pos)                   /**< (ISC_INTSR) Write Channel Error Identifier Mask */
#define ISC_INTSR_WERRID(value)               (ISC_INTSR_WERRID_Msk & ((value) << ISC_INTSR_WERRID_Pos))
#define   ISC_INTSR_WERRID_CH0_Val            _U_(0x0)                                             /**< (ISC_INTSR) An error occurred for Channel 0 (RAW/RGB/Y)  */
#define   ISC_INTSR_WERRID_CH1_Val            _U_(0x1)                                             /**< (ISC_INTSR) An error occurred for Channel 1 (CbCr/Cb)  */
#define   ISC_INTSR_WERRID_CH2_Val            _U_(0x2)                                             /**< (ISC_INTSR) An error occurred for Channel 2 (Cr)  */
#define   ISC_INTSR_WERRID_WB_Val             _U_(0x3)                                             /**< (ISC_INTSR) Write back channel error  */
#define ISC_INTSR_WERRID_CH0                  (ISC_INTSR_WERRID_CH0_Val << ISC_INTSR_WERRID_Pos)   /**< (ISC_INTSR) An error occurred for Channel 0 (RAW/RGB/Y) Position  */
#define ISC_INTSR_WERRID_CH1                  (ISC_INTSR_WERRID_CH1_Val << ISC_INTSR_WERRID_Pos)   /**< (ISC_INTSR) An error occurred for Channel 1 (CbCr/Cb) Position  */
#define ISC_INTSR_WERRID_CH2                  (ISC_INTSR_WERRID_CH2_Val << ISC_INTSR_WERRID_Pos)   /**< (ISC_INTSR) An error occurred for Channel 2 (Cr) Position  */
#define ISC_INTSR_WERRID_WB                   (ISC_INTSR_WERRID_WB_Val << ISC_INTSR_WERRID_Pos)    /**< (ISC_INTSR) Write back channel error Position  */
#define ISC_INTSR_RERR_Pos                    _U_(20)                                              /**< (ISC_INTSR) Read Channel Error Interrupt Position */
#define ISC_INTSR_RERR_Msk                    (_U_(0x1) << ISC_INTSR_RERR_Pos)                     /**< (ISC_INTSR) Read Channel Error Interrupt Mask */
#define ISC_INTSR_RERR(value)                 (ISC_INTSR_RERR_Msk & ((value) << ISC_INTSR_RERR_Pos))
#define ISC_INTSR_VFPOV_Pos                   _U_(24)                                              /**< (ISC_INTSR) Vertical Front Porch Overflow Interrupt Position */
#define ISC_INTSR_VFPOV_Msk                   (_U_(0x1) << ISC_INTSR_VFPOV_Pos)                    /**< (ISC_INTSR) Vertical Front Porch Overflow Interrupt Mask */
#define ISC_INTSR_VFPOV(value)                (ISC_INTSR_VFPOV_Msk & ((value) << ISC_INTSR_VFPOV_Pos))
#define ISC_INTSR_DAOV_Pos                    _U_(25)                                              /**< (ISC_INTSR) Data Overflow Interrupt Position */
#define ISC_INTSR_DAOV_Msk                    (_U_(0x1) << ISC_INTSR_DAOV_Pos)                     /**< (ISC_INTSR) Data Overflow Interrupt Mask */
#define ISC_INTSR_DAOV(value)                 (ISC_INTSR_DAOV_Msk & ((value) << ISC_INTSR_DAOV_Pos))
#define ISC_INTSR_VDTO_Pos                    _U_(26)                                              /**< (ISC_INTSR) Vertical Synchronization Timeout Interrupt Position */
#define ISC_INTSR_VDTO_Msk                    (_U_(0x1) << ISC_INTSR_VDTO_Pos)                     /**< (ISC_INTSR) Vertical Synchronization Timeout Interrupt Mask */
#define ISC_INTSR_VDTO(value)                 (ISC_INTSR_VDTO_Msk & ((value) << ISC_INTSR_VDTO_Pos))
#define ISC_INTSR_HDTO_Pos                    _U_(27)                                              /**< (ISC_INTSR) Horizontal Synchronization Timeout Interrupt Position */
#define ISC_INTSR_HDTO_Msk                    (_U_(0x1) << ISC_INTSR_HDTO_Pos)                     /**< (ISC_INTSR) Horizontal Synchronization Timeout Interrupt Mask */
#define ISC_INTSR_HDTO(value)                 (ISC_INTSR_HDTO_Msk & ((value) << ISC_INTSR_HDTO_Pos))
#define ISC_INTSR_CCIRERR_Pos                 _U_(28)                                              /**< (ISC_INTSR) CCIR Decoder Error Interrupt Position */
#define ISC_INTSR_CCIRERR_Msk                 (_U_(0x1) << ISC_INTSR_CCIRERR_Pos)                  /**< (ISC_INTSR) CCIR Decoder Error Interrupt Mask */
#define ISC_INTSR_CCIRERR(value)              (ISC_INTSR_CCIRERR_Msk & ((value) << ISC_INTSR_CCIRERR_Pos))
#define ISC_INTSR_Msk                         _U_(0x1F173333)                                      /**< (ISC_INTSR) Register Mask  */


/* -------- ISC_WB_CTRL : (ISC Offset: 0x58) (R/W 32) White Balance Control Register -------- */
#define ISC_WB_CTRL_ENABLE_Pos                _U_(0)                                               /**< (ISC_WB_CTRL) White Balance Enable Position */
#define ISC_WB_CTRL_ENABLE_Msk                (_U_(0x1) << ISC_WB_CTRL_ENABLE_Pos)                 /**< (ISC_WB_CTRL) White Balance Enable Mask */
#define ISC_WB_CTRL_ENABLE(value)             (ISC_WB_CTRL_ENABLE_Msk & ((value) << ISC_WB_CTRL_ENABLE_Pos))
#define ISC_WB_CTRL_Msk                       _U_(0x00000001)                                      /**< (ISC_WB_CTRL) Register Mask  */


/* -------- ISC_WB_CFG : (ISC Offset: 0x5C) (R/W 32) White Balance Configuration Register -------- */
#define ISC_WB_CFG_BAYCFG_Pos                 _U_(0)                                               /**< (ISC_WB_CFG) White Balance Bayer Configuration (Pixel Color Pattern) Position */
#define ISC_WB_CFG_BAYCFG_Msk                 (_U_(0x3) << ISC_WB_CFG_BAYCFG_Pos)                  /**< (ISC_WB_CFG) White Balance Bayer Configuration (Pixel Color Pattern) Mask */
#define ISC_WB_CFG_BAYCFG(value)              (ISC_WB_CFG_BAYCFG_Msk & ((value) << ISC_WB_CFG_BAYCFG_Pos))
#define   ISC_WB_CFG_BAYCFG_GRGR_Val          _U_(0x0)                                             /**< (ISC_WB_CFG) Starting Row configuration is G R G R (Red Row)  */
#define   ISC_WB_CFG_BAYCFG_RGRG_Val          _U_(0x1)                                             /**< (ISC_WB_CFG) Starting Row configuration is R G R G (Red Row)  */
#define   ISC_WB_CFG_BAYCFG_GBGB_Val          _U_(0x2)                                             /**< (ISC_WB_CFG) Starting Row configuration is G B G B (Blue Row)  */
#define   ISC_WB_CFG_BAYCFG_BGBG_Val          _U_(0x3)                                             /**< (ISC_WB_CFG) Starting Row configuration is B G B G (Blue Row)  */
#define ISC_WB_CFG_BAYCFG_GRGR                (ISC_WB_CFG_BAYCFG_GRGR_Val << ISC_WB_CFG_BAYCFG_Pos) /**< (ISC_WB_CFG) Starting Row configuration is G R G R (Red Row) Position  */
#define ISC_WB_CFG_BAYCFG_RGRG                (ISC_WB_CFG_BAYCFG_RGRG_Val << ISC_WB_CFG_BAYCFG_Pos) /**< (ISC_WB_CFG) Starting Row configuration is R G R G (Red Row) Position  */
#define ISC_WB_CFG_BAYCFG_GBGB                (ISC_WB_CFG_BAYCFG_GBGB_Val << ISC_WB_CFG_BAYCFG_Pos) /**< (ISC_WB_CFG) Starting Row configuration is G B G B (Blue Row) Position  */
#define ISC_WB_CFG_BAYCFG_BGBG                (ISC_WB_CFG_BAYCFG_BGBG_Val << ISC_WB_CFG_BAYCFG_Pos) /**< (ISC_WB_CFG) Starting Row configuration is B G B G (Blue Row) Position  */
#define ISC_WB_CFG_Msk                        _U_(0x00000003)                                      /**< (ISC_WB_CFG) Register Mask  */


/* -------- ISC_WB_O_RGR : (ISC Offset: 0x60) (R/W 32) White Balance Offset for R, GR Register -------- */
#define ISC_WB_O_RGR_ROFST_Pos                _U_(0)                                               /**< (ISC_WB_O_RGR) Offset Red Component (signed 13 bits 1:12:0) Position */
#define ISC_WB_O_RGR_ROFST_Msk                (_U_(0x1FFF) << ISC_WB_O_RGR_ROFST_Pos)              /**< (ISC_WB_O_RGR) Offset Red Component (signed 13 bits 1:12:0) Mask */
#define ISC_WB_O_RGR_ROFST(value)             (ISC_WB_O_RGR_ROFST_Msk & ((value) << ISC_WB_O_RGR_ROFST_Pos))
#define ISC_WB_O_RGR_GROFST_Pos               _U_(16)                                              /**< (ISC_WB_O_RGR) Offset Green Component for Red Row (signed 13 bits 1:12:0) Position */
#define ISC_WB_O_RGR_GROFST_Msk               (_U_(0x1FFF) << ISC_WB_O_RGR_GROFST_Pos)             /**< (ISC_WB_O_RGR) Offset Green Component for Red Row (signed 13 bits 1:12:0) Mask */
#define ISC_WB_O_RGR_GROFST(value)            (ISC_WB_O_RGR_GROFST_Msk & ((value) << ISC_WB_O_RGR_GROFST_Pos))
#define ISC_WB_O_RGR_Msk                      _U_(0x1FFF1FFF)                                      /**< (ISC_WB_O_RGR) Register Mask  */


/* -------- ISC_WB_O_BGB : (ISC Offset: 0x64) (R/W 32) White Balance Offset for B, GB Register -------- */
#define ISC_WB_O_BGB_BOFST_Pos                _U_(0)                                               /**< (ISC_WB_O_BGB) Offset Blue Component (signed 13 bits, 1:12:0) Position */
#define ISC_WB_O_BGB_BOFST_Msk                (_U_(0x1FFF) << ISC_WB_O_BGB_BOFST_Pos)              /**< (ISC_WB_O_BGB) Offset Blue Component (signed 13 bits, 1:12:0) Mask */
#define ISC_WB_O_BGB_BOFST(value)             (ISC_WB_O_BGB_BOFST_Msk & ((value) << ISC_WB_O_BGB_BOFST_Pos))
#define ISC_WB_O_BGB_GBOFST_Pos               _U_(16)                                              /**< (ISC_WB_O_BGB) Offset Green Component for Blue Row (signed 13 bits, 1:12:0) Position */
#define ISC_WB_O_BGB_GBOFST_Msk               (_U_(0x1FFF) << ISC_WB_O_BGB_GBOFST_Pos)             /**< (ISC_WB_O_BGB) Offset Green Component for Blue Row (signed 13 bits, 1:12:0) Mask */
#define ISC_WB_O_BGB_GBOFST(value)            (ISC_WB_O_BGB_GBOFST_Msk & ((value) << ISC_WB_O_BGB_GBOFST_Pos))
#define ISC_WB_O_BGB_Msk                      _U_(0x1FFF1FFF)                                      /**< (ISC_WB_O_BGB) Register Mask  */


/* -------- ISC_WB_G_RGR : (ISC Offset: 0x68) (R/W 32) White Balance Gain for R, GR Register -------- */
#define ISC_WB_G_RGR_RGAIN_Pos                _U_(0)                                               /**< (ISC_WB_G_RGR) Red Component Gain (unsigned 13 bits, 0:4:9) Position */
#define ISC_WB_G_RGR_RGAIN_Msk                (_U_(0x1FFF) << ISC_WB_G_RGR_RGAIN_Pos)              /**< (ISC_WB_G_RGR) Red Component Gain (unsigned 13 bits, 0:4:9) Mask */
#define ISC_WB_G_RGR_RGAIN(value)             (ISC_WB_G_RGR_RGAIN_Msk & ((value) << ISC_WB_G_RGR_RGAIN_Pos))
#define ISC_WB_G_RGR_GRGAIN_Pos               _U_(16)                                              /**< (ISC_WB_G_RGR) Green Component (Red row) Gain (unsigned 13 bits, 0:4:9) Position */
#define ISC_WB_G_RGR_GRGAIN_Msk               (_U_(0x1FFF) << ISC_WB_G_RGR_GRGAIN_Pos)             /**< (ISC_WB_G_RGR) Green Component (Red row) Gain (unsigned 13 bits, 0:4:9) Mask */
#define ISC_WB_G_RGR_GRGAIN(value)            (ISC_WB_G_RGR_GRGAIN_Msk & ((value) << ISC_WB_G_RGR_GRGAIN_Pos))
#define ISC_WB_G_RGR_Msk                      _U_(0x1FFF1FFF)                                      /**< (ISC_WB_G_RGR) Register Mask  */


/* -------- ISC_WB_G_BGB : (ISC Offset: 0x6C) (R/W 32) White Balance Gain for B, GB Register -------- */
#define ISC_WB_G_BGB_BGAIN_Pos                _U_(0)                                               /**< (ISC_WB_G_BGB) Blue Component Gain (unsigned 13 bits, 0:4:9) Position */
#define ISC_WB_G_BGB_BGAIN_Msk                (_U_(0x1FFF) << ISC_WB_G_BGB_BGAIN_Pos)              /**< (ISC_WB_G_BGB) Blue Component Gain (unsigned 13 bits, 0:4:9) Mask */
#define ISC_WB_G_BGB_BGAIN(value)             (ISC_WB_G_BGB_BGAIN_Msk & ((value) << ISC_WB_G_BGB_BGAIN_Pos))
#define ISC_WB_G_BGB_GBGAIN_Pos               _U_(16)                                              /**< (ISC_WB_G_BGB) Green Component (Blue row) Gain (unsigned 13 bits, 0:4:9) Position */
#define ISC_WB_G_BGB_GBGAIN_Msk               (_U_(0x1FFF) << ISC_WB_G_BGB_GBGAIN_Pos)             /**< (ISC_WB_G_BGB) Green Component (Blue row) Gain (unsigned 13 bits, 0:4:9) Mask */
#define ISC_WB_G_BGB_GBGAIN(value)            (ISC_WB_G_BGB_GBGAIN_Msk & ((value) << ISC_WB_G_BGB_GBGAIN_Pos))
#define ISC_WB_G_BGB_Msk                      _U_(0x1FFF1FFF)                                      /**< (ISC_WB_G_BGB) Register Mask  */


/* -------- ISC_CFA_CTRL : (ISC Offset: 0x70) (R/W 32) Color Filter Array Control Register -------- */
#define ISC_CFA_CTRL_ENABLE_Pos               _U_(0)                                               /**< (ISC_CFA_CTRL) Color Filter Array Interpolation Enable Position */
#define ISC_CFA_CTRL_ENABLE_Msk               (_U_(0x1) << ISC_CFA_CTRL_ENABLE_Pos)                /**< (ISC_CFA_CTRL) Color Filter Array Interpolation Enable Mask */
#define ISC_CFA_CTRL_ENABLE(value)            (ISC_CFA_CTRL_ENABLE_Msk & ((value) << ISC_CFA_CTRL_ENABLE_Pos))
#define ISC_CFA_CTRL_Msk                      _U_(0x00000001)                                      /**< (ISC_CFA_CTRL) Register Mask  */


/* -------- ISC_CFA_CFG : (ISC Offset: 0x74) (R/W 32) Color Filter Array Configuration Register -------- */
#define ISC_CFA_CFG_BAYCFG_Pos                _U_(0)                                               /**< (ISC_CFA_CFG) Color Filter Array Pattern Position */
#define ISC_CFA_CFG_BAYCFG_Msk                (_U_(0x3) << ISC_CFA_CFG_BAYCFG_Pos)                 /**< (ISC_CFA_CFG) Color Filter Array Pattern Mask */
#define ISC_CFA_CFG_BAYCFG(value)             (ISC_CFA_CFG_BAYCFG_Msk & ((value) << ISC_CFA_CFG_BAYCFG_Pos))
#define   ISC_CFA_CFG_BAYCFG_GRGR_Val         _U_(0x0)                                             /**< (ISC_CFA_CFG) Starting row configuration is G R G R (red row)  */
#define   ISC_CFA_CFG_BAYCFG_RGRG_Val         _U_(0x1)                                             /**< (ISC_CFA_CFG) Starting row configuration is R G R G (red row  */
#define   ISC_CFA_CFG_BAYCFG_GBGB_Val         _U_(0x2)                                             /**< (ISC_CFA_CFG) Starting row configuration is G B G B (blue row)  */
#define   ISC_CFA_CFG_BAYCFG_BGBG_Val         _U_(0x3)                                             /**< (ISC_CFA_CFG) Starting row configuration is B G B G (blue row)  */
#define ISC_CFA_CFG_BAYCFG_GRGR               (ISC_CFA_CFG_BAYCFG_GRGR_Val << ISC_CFA_CFG_BAYCFG_Pos) /**< (ISC_CFA_CFG) Starting row configuration is G R G R (red row) Position  */
#define ISC_CFA_CFG_BAYCFG_RGRG               (ISC_CFA_CFG_BAYCFG_RGRG_Val << ISC_CFA_CFG_BAYCFG_Pos) /**< (ISC_CFA_CFG) Starting row configuration is R G R G (red row Position  */
#define ISC_CFA_CFG_BAYCFG_GBGB               (ISC_CFA_CFG_BAYCFG_GBGB_Val << ISC_CFA_CFG_BAYCFG_Pos) /**< (ISC_CFA_CFG) Starting row configuration is G B G B (blue row) Position  */
#define ISC_CFA_CFG_BAYCFG_BGBG               (ISC_CFA_CFG_BAYCFG_BGBG_Val << ISC_CFA_CFG_BAYCFG_Pos) /**< (ISC_CFA_CFG) Starting row configuration is B G B G (blue row) Position  */
#define ISC_CFA_CFG_EITPOL_Pos                _U_(4)                                               /**< (ISC_CFA_CFG) Edge Interpolation Position */
#define ISC_CFA_CFG_EITPOL_Msk                (_U_(0x1) << ISC_CFA_CFG_EITPOL_Pos)                 /**< (ISC_CFA_CFG) Edge Interpolation Mask */
#define ISC_CFA_CFG_EITPOL(value)             (ISC_CFA_CFG_EITPOL_Msk & ((value) << ISC_CFA_CFG_EITPOL_Pos))
#define ISC_CFA_CFG_Msk                       _U_(0x00000013)                                      /**< (ISC_CFA_CFG) Register Mask  */


/* -------- ISC_CC_CTRL : (ISC Offset: 0x78) (R/W 32) Color Correction Control Register -------- */
#define ISC_CC_CTRL_ENABLE_Pos                _U_(0)                                               /**< (ISC_CC_CTRL) Color Correction Enable Position */
#define ISC_CC_CTRL_ENABLE_Msk                (_U_(0x1) << ISC_CC_CTRL_ENABLE_Pos)                 /**< (ISC_CC_CTRL) Color Correction Enable Mask */
#define ISC_CC_CTRL_ENABLE(value)             (ISC_CC_CTRL_ENABLE_Msk & ((value) << ISC_CC_CTRL_ENABLE_Pos))
#define ISC_CC_CTRL_Msk                       _U_(0x00000001)                                      /**< (ISC_CC_CTRL) Register Mask  */


/* -------- ISC_CC_RR_RG : (ISC Offset: 0x7C) (R/W 32) Color Correction RR RG Register -------- */
#define ISC_CC_RR_RG_RRGAIN_Pos               _U_(0)                                               /**< (ISC_CC_RR_RG) Red Gain for Red Component (signed 12 bits, 1:3:8) Position */
#define ISC_CC_RR_RG_RRGAIN_Msk               (_U_(0xFFF) << ISC_CC_RR_RG_RRGAIN_Pos)              /**< (ISC_CC_RR_RG) Red Gain for Red Component (signed 12 bits, 1:3:8) Mask */
#define ISC_CC_RR_RG_RRGAIN(value)            (ISC_CC_RR_RG_RRGAIN_Msk & ((value) << ISC_CC_RR_RG_RRGAIN_Pos))
#define ISC_CC_RR_RG_RGGAIN_Pos               _U_(16)                                              /**< (ISC_CC_RR_RG) Green Gain for Red Component (signed 12 bits, 1:3:8) Position */
#define ISC_CC_RR_RG_RGGAIN_Msk               (_U_(0xFFF) << ISC_CC_RR_RG_RGGAIN_Pos)              /**< (ISC_CC_RR_RG) Green Gain for Red Component (signed 12 bits, 1:3:8) Mask */
#define ISC_CC_RR_RG_RGGAIN(value)            (ISC_CC_RR_RG_RGGAIN_Msk & ((value) << ISC_CC_RR_RG_RGGAIN_Pos))
#define ISC_CC_RR_RG_Msk                      _U_(0x0FFF0FFF)                                      /**< (ISC_CC_RR_RG) Register Mask  */


/* -------- ISC_CC_RB_OR : (ISC Offset: 0x80) (R/W 32) Color Correction RB OR Register -------- */
#define ISC_CC_RB_OR_RBGAIN_Pos               _U_(0)                                               /**< (ISC_CC_RB_OR) Blue Gain for Red Component (signed 12 bits, 1:3:8) Position */
#define ISC_CC_RB_OR_RBGAIN_Msk               (_U_(0xFFF) << ISC_CC_RB_OR_RBGAIN_Pos)              /**< (ISC_CC_RB_OR) Blue Gain for Red Component (signed 12 bits, 1:3:8) Mask */
#define ISC_CC_RB_OR_RBGAIN(value)            (ISC_CC_RB_OR_RBGAIN_Msk & ((value) << ISC_CC_RB_OR_RBGAIN_Pos))
#define ISC_CC_RB_OR_ROFST_Pos                _U_(16)                                              /**< (ISC_CC_RB_OR) Red Component Offset (signed 13 bits, 1:12:0) Position */
#define ISC_CC_RB_OR_ROFST_Msk                (_U_(0x1FFF) << ISC_CC_RB_OR_ROFST_Pos)              /**< (ISC_CC_RB_OR) Red Component Offset (signed 13 bits, 1:12:0) Mask */
#define ISC_CC_RB_OR_ROFST(value)             (ISC_CC_RB_OR_ROFST_Msk & ((value) << ISC_CC_RB_OR_ROFST_Pos))
#define ISC_CC_RB_OR_Msk                      _U_(0x1FFF0FFF)                                      /**< (ISC_CC_RB_OR) Register Mask  */


/* -------- ISC_CC_GR_GG : (ISC Offset: 0x84) (R/W 32) Color Correction GR GG Register -------- */
#define ISC_CC_GR_GG_GRGAIN_Pos               _U_(0)                                               /**< (ISC_CC_GR_GG) Red Gain for Green Component (signed 12 bits, 1:3:8) Position */
#define ISC_CC_GR_GG_GRGAIN_Msk               (_U_(0xFFF) << ISC_CC_GR_GG_GRGAIN_Pos)              /**< (ISC_CC_GR_GG) Red Gain for Green Component (signed 12 bits, 1:3:8) Mask */
#define ISC_CC_GR_GG_GRGAIN(value)            (ISC_CC_GR_GG_GRGAIN_Msk & ((value) << ISC_CC_GR_GG_GRGAIN_Pos))
#define ISC_CC_GR_GG_GGGAIN_Pos               _U_(16)                                              /**< (ISC_CC_GR_GG) Green Gain for Green Component (signed 12 bits, 1:3:8) Position */
#define ISC_CC_GR_GG_GGGAIN_Msk               (_U_(0xFFF) << ISC_CC_GR_GG_GGGAIN_Pos)              /**< (ISC_CC_GR_GG) Green Gain for Green Component (signed 12 bits, 1:3:8) Mask */
#define ISC_CC_GR_GG_GGGAIN(value)            (ISC_CC_GR_GG_GGGAIN_Msk & ((value) << ISC_CC_GR_GG_GGGAIN_Pos))
#define ISC_CC_GR_GG_Msk                      _U_(0x0FFF0FFF)                                      /**< (ISC_CC_GR_GG) Register Mask  */


/* -------- ISC_CC_GB_OG : (ISC Offset: 0x88) (R/W 32) Color Correction GB OG Register -------- */
#define ISC_CC_GB_OG_GBGAIN_Pos               _U_(0)                                               /**< (ISC_CC_GB_OG) Blue Gain for Green Component (signed 12 bits, 1:3:8) Position */
#define ISC_CC_GB_OG_GBGAIN_Msk               (_U_(0xFFF) << ISC_CC_GB_OG_GBGAIN_Pos)              /**< (ISC_CC_GB_OG) Blue Gain for Green Component (signed 12 bits, 1:3:8) Mask */
#define ISC_CC_GB_OG_GBGAIN(value)            (ISC_CC_GB_OG_GBGAIN_Msk & ((value) << ISC_CC_GB_OG_GBGAIN_Pos))
#define ISC_CC_GB_OG_ROFST_Pos                _U_(16)                                              /**< (ISC_CC_GB_OG) Green Component Offset (signed 13 bits, 1:12:0) Position */
#define ISC_CC_GB_OG_ROFST_Msk                (_U_(0x1FFF) << ISC_CC_GB_OG_ROFST_Pos)              /**< (ISC_CC_GB_OG) Green Component Offset (signed 13 bits, 1:12:0) Mask */
#define ISC_CC_GB_OG_ROFST(value)             (ISC_CC_GB_OG_ROFST_Msk & ((value) << ISC_CC_GB_OG_ROFST_Pos))
#define ISC_CC_GB_OG_Msk                      _U_(0x1FFF0FFF)                                      /**< (ISC_CC_GB_OG) Register Mask  */


/* -------- ISC_CC_BR_BG : (ISC Offset: 0x8C) (R/W 32) Color Correction BR BG Register -------- */
#define ISC_CC_BR_BG_BRGAIN_Pos               _U_(0)                                               /**< (ISC_CC_BR_BG) Red Gain for Blue Component (signed 12 bits, 1:3:8) Position */
#define ISC_CC_BR_BG_BRGAIN_Msk               (_U_(0xFFF) << ISC_CC_BR_BG_BRGAIN_Pos)              /**< (ISC_CC_BR_BG) Red Gain for Blue Component (signed 12 bits, 1:3:8) Mask */
#define ISC_CC_BR_BG_BRGAIN(value)            (ISC_CC_BR_BG_BRGAIN_Msk & ((value) << ISC_CC_BR_BG_BRGAIN_Pos))
#define ISC_CC_BR_BG_BGGAIN_Pos               _U_(16)                                              /**< (ISC_CC_BR_BG) Green Gain for Blue Component (signed 12 bits, 1:3:8) Position */
#define ISC_CC_BR_BG_BGGAIN_Msk               (_U_(0xFFF) << ISC_CC_BR_BG_BGGAIN_Pos)              /**< (ISC_CC_BR_BG) Green Gain for Blue Component (signed 12 bits, 1:3:8) Mask */
#define ISC_CC_BR_BG_BGGAIN(value)            (ISC_CC_BR_BG_BGGAIN_Msk & ((value) << ISC_CC_BR_BG_BGGAIN_Pos))
#define ISC_CC_BR_BG_Msk                      _U_(0x0FFF0FFF)                                      /**< (ISC_CC_BR_BG) Register Mask  */


/* -------- ISC_CC_BB_OB : (ISC Offset: 0x90) (R/W 32) Color Correction BB OB Register -------- */
#define ISC_CC_BB_OB_BBGAIN_Pos               _U_(0)                                               /**< (ISC_CC_BB_OB) Blue Gain for Blue Component (signed 12 bits, 1:3:8) Position */
#define ISC_CC_BB_OB_BBGAIN_Msk               (_U_(0xFFF) << ISC_CC_BB_OB_BBGAIN_Pos)              /**< (ISC_CC_BB_OB) Blue Gain for Blue Component (signed 12 bits, 1:3:8) Mask */
#define ISC_CC_BB_OB_BBGAIN(value)            (ISC_CC_BB_OB_BBGAIN_Msk & ((value) << ISC_CC_BB_OB_BBGAIN_Pos))
#define ISC_CC_BB_OB_BOFST_Pos                _U_(16)                                              /**< (ISC_CC_BB_OB) Blue Component Offset (signed 13 bits, 1:12:0) Position */
#define ISC_CC_BB_OB_BOFST_Msk                (_U_(0x1FFF) << ISC_CC_BB_OB_BOFST_Pos)              /**< (ISC_CC_BB_OB) Blue Component Offset (signed 13 bits, 1:12:0) Mask */
#define ISC_CC_BB_OB_BOFST(value)             (ISC_CC_BB_OB_BOFST_Msk & ((value) << ISC_CC_BB_OB_BOFST_Pos))
#define ISC_CC_BB_OB_Msk                      _U_(0x1FFF0FFF)                                      /**< (ISC_CC_BB_OB) Register Mask  */


/* -------- ISC_GAM_CTRL : (ISC Offset: 0x94) (R/W 32) Gamma Correction Control Register -------- */
#define ISC_GAM_CTRL_ENABLE_Pos               _U_(0)                                               /**< (ISC_GAM_CTRL) Gamma Correction Enable Position */
#define ISC_GAM_CTRL_ENABLE_Msk               (_U_(0x1) << ISC_GAM_CTRL_ENABLE_Pos)                /**< (ISC_GAM_CTRL) Gamma Correction Enable Mask */
#define ISC_GAM_CTRL_ENABLE(value)            (ISC_GAM_CTRL_ENABLE_Msk & ((value) << ISC_GAM_CTRL_ENABLE_Pos))
#define ISC_GAM_CTRL_BENABLE_Pos              _U_(1)                                               /**< (ISC_GAM_CTRL) Gamma Correction Enable for B Channel Position */
#define ISC_GAM_CTRL_BENABLE_Msk              (_U_(0x1) << ISC_GAM_CTRL_BENABLE_Pos)               /**< (ISC_GAM_CTRL) Gamma Correction Enable for B Channel Mask */
#define ISC_GAM_CTRL_BENABLE(value)           (ISC_GAM_CTRL_BENABLE_Msk & ((value) << ISC_GAM_CTRL_BENABLE_Pos))
#define ISC_GAM_CTRL_GENABLE_Pos              _U_(2)                                               /**< (ISC_GAM_CTRL) Gamma Correction Enable for G Channel Position */
#define ISC_GAM_CTRL_GENABLE_Msk              (_U_(0x1) << ISC_GAM_CTRL_GENABLE_Pos)               /**< (ISC_GAM_CTRL) Gamma Correction Enable for G Channel Mask */
#define ISC_GAM_CTRL_GENABLE(value)           (ISC_GAM_CTRL_GENABLE_Msk & ((value) << ISC_GAM_CTRL_GENABLE_Pos))
#define ISC_GAM_CTRL_RENABLE_Pos              _U_(3)                                               /**< (ISC_GAM_CTRL) Gamma Correction Enable for R Channel Position */
#define ISC_GAM_CTRL_RENABLE_Msk              (_U_(0x1) << ISC_GAM_CTRL_RENABLE_Pos)               /**< (ISC_GAM_CTRL) Gamma Correction Enable for R Channel Mask */
#define ISC_GAM_CTRL_RENABLE(value)           (ISC_GAM_CTRL_RENABLE_Msk & ((value) << ISC_GAM_CTRL_RENABLE_Pos))
#define ISC_GAM_CTRL_Msk                      _U_(0x0000000F)                                      /**< (ISC_GAM_CTRL) Register Mask  */


/* -------- ISC_GAM_BENTRY : (ISC Offset: 0x98) (R/W 32) Gamma Correction Blue Entry -------- */
#define ISC_GAM_BENTRY_BSLOPE_Pos             _U_(0)                                               /**< (ISC_GAM_BENTRY) Blue Color Slope for Piecewise Interpolation (signed 10 bits 1:3:6) Position */
#define ISC_GAM_BENTRY_BSLOPE_Msk             (_U_(0x3FF) << ISC_GAM_BENTRY_BSLOPE_Pos)            /**< (ISC_GAM_BENTRY) Blue Color Slope for Piecewise Interpolation (signed 10 bits 1:3:6) Mask */
#define ISC_GAM_BENTRY_BSLOPE(value)          (ISC_GAM_BENTRY_BSLOPE_Msk & ((value) << ISC_GAM_BENTRY_BSLOPE_Pos))
#define ISC_GAM_BENTRY_BCONSTANT_Pos          _U_(16)                                              /**< (ISC_GAM_BENTRY) Blue Color Constant for Piecewise Interpolation (unsigned 10 bits 0:10:0) Position */
#define ISC_GAM_BENTRY_BCONSTANT_Msk          (_U_(0x3FF) << ISC_GAM_BENTRY_BCONSTANT_Pos)         /**< (ISC_GAM_BENTRY) Blue Color Constant for Piecewise Interpolation (unsigned 10 bits 0:10:0) Mask */
#define ISC_GAM_BENTRY_BCONSTANT(value)       (ISC_GAM_BENTRY_BCONSTANT_Msk & ((value) << ISC_GAM_BENTRY_BCONSTANT_Pos))
#define ISC_GAM_BENTRY_Msk                    _U_(0x03FF03FF)                                      /**< (ISC_GAM_BENTRY) Register Mask  */


/* -------- ISC_GAM_GENTRY : (ISC Offset: 0x198) (R/W 32) Gamma Correction Green Entry -------- */
#define ISC_GAM_GENTRY_GSLOPE_Pos             _U_(0)                                               /**< (ISC_GAM_GENTRY) Green Color Slope for Piecewise Interpolation (signed 10 bits 1:3:6) Position */
#define ISC_GAM_GENTRY_GSLOPE_Msk             (_U_(0x3FF) << ISC_GAM_GENTRY_GSLOPE_Pos)            /**< (ISC_GAM_GENTRY) Green Color Slope for Piecewise Interpolation (signed 10 bits 1:3:6) Mask */
#define ISC_GAM_GENTRY_GSLOPE(value)          (ISC_GAM_GENTRY_GSLOPE_Msk & ((value) << ISC_GAM_GENTRY_GSLOPE_Pos))
#define ISC_GAM_GENTRY_GCONSTANT_Pos          _U_(16)                                              /**< (ISC_GAM_GENTRY) Green Color Constant for Piecewise Interpolation (unsigned 10 bits 0:10:0) Position */
#define ISC_GAM_GENTRY_GCONSTANT_Msk          (_U_(0x3FF) << ISC_GAM_GENTRY_GCONSTANT_Pos)         /**< (ISC_GAM_GENTRY) Green Color Constant for Piecewise Interpolation (unsigned 10 bits 0:10:0) Mask */
#define ISC_GAM_GENTRY_GCONSTANT(value)       (ISC_GAM_GENTRY_GCONSTANT_Msk & ((value) << ISC_GAM_GENTRY_GCONSTANT_Pos))
#define ISC_GAM_GENTRY_Msk                    _U_(0x03FF03FF)                                      /**< (ISC_GAM_GENTRY) Register Mask  */


/* -------- ISC_GAM_RENTRY : (ISC Offset: 0x298) (R/W 32) Gamma Correction Red Entry -------- */
#define ISC_GAM_RENTRY_RSLOPE_Pos             _U_(0)                                               /**< (ISC_GAM_RENTRY) Red Color Slope for Piecewise Interpolation (signed 10 bits 1:3:6) Position */
#define ISC_GAM_RENTRY_RSLOPE_Msk             (_U_(0x3FF) << ISC_GAM_RENTRY_RSLOPE_Pos)            /**< (ISC_GAM_RENTRY) Red Color Slope for Piecewise Interpolation (signed 10 bits 1:3:6) Mask */
#define ISC_GAM_RENTRY_RSLOPE(value)          (ISC_GAM_RENTRY_RSLOPE_Msk & ((value) << ISC_GAM_RENTRY_RSLOPE_Pos))
#define ISC_GAM_RENTRY_RCONSTANT_Pos          _U_(16)                                              /**< (ISC_GAM_RENTRY) Red Color Constant for Piecewise Interpolation (unsigned 10 bits 0:10:0) Position */
#define ISC_GAM_RENTRY_RCONSTANT_Msk          (_U_(0x3FF) << ISC_GAM_RENTRY_RCONSTANT_Pos)         /**< (ISC_GAM_RENTRY) Red Color Constant for Piecewise Interpolation (unsigned 10 bits 0:10:0) Mask */
#define ISC_GAM_RENTRY_RCONSTANT(value)       (ISC_GAM_RENTRY_RCONSTANT_Msk & ((value) << ISC_GAM_RENTRY_RCONSTANT_Pos))
#define ISC_GAM_RENTRY_Msk                    _U_(0x03FF03FF)                                      /**< (ISC_GAM_RENTRY) Register Mask  */


/* -------- ISC_CSC_CTRL : (ISC Offset: 0x398) (R/W 32) Color Space Conversion Control Register -------- */
#define ISC_CSC_CTRL_ENABLE_Pos               _U_(0)                                               /**< (ISC_CSC_CTRL) RGB to YCbCr Color Space Conversion Enable Position */
#define ISC_CSC_CTRL_ENABLE_Msk               (_U_(0x1) << ISC_CSC_CTRL_ENABLE_Pos)                /**< (ISC_CSC_CTRL) RGB to YCbCr Color Space Conversion Enable Mask */
#define ISC_CSC_CTRL_ENABLE(value)            (ISC_CSC_CTRL_ENABLE_Msk & ((value) << ISC_CSC_CTRL_ENABLE_Pos))
#define ISC_CSC_CTRL_Msk                      _U_(0x00000001)                                      /**< (ISC_CSC_CTRL) Register Mask  */


/* -------- ISC_CSC_YR_YG : (ISC Offset: 0x39C) (R/W 32) Color Space Conversion YR, YG Register -------- */
#define ISC_CSC_YR_YG_YRGAIN_Pos              _U_(0)                                               /**< (ISC_CSC_YR_YG) Reg Gain for Luminance (signed 12 bits 1:3:8) Position */
#define ISC_CSC_YR_YG_YRGAIN_Msk              (_U_(0xFFF) << ISC_CSC_YR_YG_YRGAIN_Pos)             /**< (ISC_CSC_YR_YG) Reg Gain for Luminance (signed 12 bits 1:3:8) Mask */
#define ISC_CSC_YR_YG_YRGAIN(value)           (ISC_CSC_YR_YG_YRGAIN_Msk & ((value) << ISC_CSC_YR_YG_YRGAIN_Pos))
#define ISC_CSC_YR_YG_YGGAIN_Pos              _U_(16)                                              /**< (ISC_CSC_YR_YG) Green Gain for Luminance (signed 12 bits 1:3:8) Position */
#define ISC_CSC_YR_YG_YGGAIN_Msk              (_U_(0xFFF) << ISC_CSC_YR_YG_YGGAIN_Pos)             /**< (ISC_CSC_YR_YG) Green Gain for Luminance (signed 12 bits 1:3:8) Mask */
#define ISC_CSC_YR_YG_YGGAIN(value)           (ISC_CSC_YR_YG_YGGAIN_Msk & ((value) << ISC_CSC_YR_YG_YGGAIN_Pos))
#define ISC_CSC_YR_YG_Msk                     _U_(0x0FFF0FFF)                                      /**< (ISC_CSC_YR_YG) Register Mask  */


/* -------- ISC_CSC_YB_OY : (ISC Offset: 0x3A0) (R/W 32) Color Space Conversion YB, OY Register -------- */
#define ISC_CSC_YB_OY_YBGAIN_Pos              _U_(0)                                               /**< (ISC_CSC_YB_OY) Blue Gain for Luminance Component (12 bits signed 1:3:8) Position */
#define ISC_CSC_YB_OY_YBGAIN_Msk              (_U_(0xFFF) << ISC_CSC_YB_OY_YBGAIN_Pos)             /**< (ISC_CSC_YB_OY) Blue Gain for Luminance Component (12 bits signed 1:3:8) Mask */
#define ISC_CSC_YB_OY_YBGAIN(value)           (ISC_CSC_YB_OY_YBGAIN_Msk & ((value) << ISC_CSC_YB_OY_YBGAIN_Pos))
#define ISC_CSC_YB_OY_YOFST_Pos               _U_(16)                                              /**< (ISC_CSC_YB_OY) Luminance Offset (11 bits signed 1:10:0) Position */
#define ISC_CSC_YB_OY_YOFST_Msk               (_U_(0x7FF) << ISC_CSC_YB_OY_YOFST_Pos)              /**< (ISC_CSC_YB_OY) Luminance Offset (11 bits signed 1:10:0) Mask */
#define ISC_CSC_YB_OY_YOFST(value)            (ISC_CSC_YB_OY_YOFST_Msk & ((value) << ISC_CSC_YB_OY_YOFST_Pos))
#define ISC_CSC_YB_OY_Msk                     _U_(0x07FF0FFF)                                      /**< (ISC_CSC_YB_OY) Register Mask  */


/* -------- ISC_CSC_CBR_CBG : (ISC Offset: 0x3A4) (R/W 32) Color Space Conversion CBR CBG Register -------- */
#define ISC_CSC_CBR_CBG_CBRGAIN_Pos           _U_(0)                                               /**< (ISC_CSC_CBR_CBG) Red Gain for Blue Chrominance (signed 12 bits, 1:3:8) Position */
#define ISC_CSC_CBR_CBG_CBRGAIN_Msk           (_U_(0xFFF) << ISC_CSC_CBR_CBG_CBRGAIN_Pos)          /**< (ISC_CSC_CBR_CBG) Red Gain for Blue Chrominance (signed 12 bits, 1:3:8) Mask */
#define ISC_CSC_CBR_CBG_CBRGAIN(value)        (ISC_CSC_CBR_CBG_CBRGAIN_Msk & ((value) << ISC_CSC_CBR_CBG_CBRGAIN_Pos))
#define ISC_CSC_CBR_CBG_CBGGAIN_Pos           _U_(16)                                              /**< (ISC_CSC_CBR_CBG) Green Gain for Blue Chrominance (signed 12 bits 1:3:8) Position */
#define ISC_CSC_CBR_CBG_CBGGAIN_Msk           (_U_(0xFFF) << ISC_CSC_CBR_CBG_CBGGAIN_Pos)          /**< (ISC_CSC_CBR_CBG) Green Gain for Blue Chrominance (signed 12 bits 1:3:8) Mask */
#define ISC_CSC_CBR_CBG_CBGGAIN(value)        (ISC_CSC_CBR_CBG_CBGGAIN_Msk & ((value) << ISC_CSC_CBR_CBG_CBGGAIN_Pos))
#define ISC_CSC_CBR_CBG_Msk                   _U_(0x0FFF0FFF)                                      /**< (ISC_CSC_CBR_CBG) Register Mask  */


/* -------- ISC_CSC_CBB_OCB : (ISC Offset: 0x3A8) (R/W 32) Color Space Conversion CBB OCB Register -------- */
#define ISC_CSC_CBB_OCB_CBBGAIN_Pos           _U_(0)                                               /**< (ISC_CSC_CBB_OCB) Blue Gain for Blue Chrominance (signed 12 bits 1:3:8) Position */
#define ISC_CSC_CBB_OCB_CBBGAIN_Msk           (_U_(0xFFF) << ISC_CSC_CBB_OCB_CBBGAIN_Pos)          /**< (ISC_CSC_CBB_OCB) Blue Gain for Blue Chrominance (signed 12 bits 1:3:8) Mask */
#define ISC_CSC_CBB_OCB_CBBGAIN(value)        (ISC_CSC_CBB_OCB_CBBGAIN_Msk & ((value) << ISC_CSC_CBB_OCB_CBBGAIN_Pos))
#define ISC_CSC_CBB_OCB_CBOFST_Pos            _U_(16)                                              /**< (ISC_CSC_CBB_OCB) Blue Chrominance Offset (signed 11 bits 1:10:0) Position */
#define ISC_CSC_CBB_OCB_CBOFST_Msk            (_U_(0x7FF) << ISC_CSC_CBB_OCB_CBOFST_Pos)           /**< (ISC_CSC_CBB_OCB) Blue Chrominance Offset (signed 11 bits 1:10:0) Mask */
#define ISC_CSC_CBB_OCB_CBOFST(value)         (ISC_CSC_CBB_OCB_CBOFST_Msk & ((value) << ISC_CSC_CBB_OCB_CBOFST_Pos))
#define ISC_CSC_CBB_OCB_Msk                   _U_(0x07FF0FFF)                                      /**< (ISC_CSC_CBB_OCB) Register Mask  */


/* -------- ISC_CSC_CRR_CRG : (ISC Offset: 0x3AC) (R/W 32) Color Space Conversion CRR CRG Register -------- */
#define ISC_CSC_CRR_CRG_CRRGAIN_Pos           _U_(0)                                               /**< (ISC_CSC_CRR_CRG) Red Gain for Red Chrominance (signed 12 bits 1:3:8) Position */
#define ISC_CSC_CRR_CRG_CRRGAIN_Msk           (_U_(0xFFF) << ISC_CSC_CRR_CRG_CRRGAIN_Pos)          /**< (ISC_CSC_CRR_CRG) Red Gain for Red Chrominance (signed 12 bits 1:3:8) Mask */
#define ISC_CSC_CRR_CRG_CRRGAIN(value)        (ISC_CSC_CRR_CRG_CRRGAIN_Msk & ((value) << ISC_CSC_CRR_CRG_CRRGAIN_Pos))
#define ISC_CSC_CRR_CRG_CRGGAIN_Pos           _U_(16)                                              /**< (ISC_CSC_CRR_CRG) Green Gain for Red Chrominance (signed 12 bits 1:3:8) Position */
#define ISC_CSC_CRR_CRG_CRGGAIN_Msk           (_U_(0xFFF) << ISC_CSC_CRR_CRG_CRGGAIN_Pos)          /**< (ISC_CSC_CRR_CRG) Green Gain for Red Chrominance (signed 12 bits 1:3:8) Mask */
#define ISC_CSC_CRR_CRG_CRGGAIN(value)        (ISC_CSC_CRR_CRG_CRGGAIN_Msk & ((value) << ISC_CSC_CRR_CRG_CRGGAIN_Pos))
#define ISC_CSC_CRR_CRG_Msk                   _U_(0x0FFF0FFF)                                      /**< (ISC_CSC_CRR_CRG) Register Mask  */


/* -------- ISC_CSC_CRB_OCR : (ISC Offset: 0x3B0) (R/W 32) Color Space Conversion CRB OCR Register -------- */
#define ISC_CSC_CRB_OCR_CRBGAIN_Pos           _U_(0)                                               /**< (ISC_CSC_CRB_OCR) Blue Gain for Red Chrominance (signed 12 bits 1:3:8) Position */
#define ISC_CSC_CRB_OCR_CRBGAIN_Msk           (_U_(0xFFF) << ISC_CSC_CRB_OCR_CRBGAIN_Pos)          /**< (ISC_CSC_CRB_OCR) Blue Gain for Red Chrominance (signed 12 bits 1:3:8) Mask */
#define ISC_CSC_CRB_OCR_CRBGAIN(value)        (ISC_CSC_CRB_OCR_CRBGAIN_Msk & ((value) << ISC_CSC_CRB_OCR_CRBGAIN_Pos))
#define ISC_CSC_CRB_OCR_CROFST_Pos            _U_(16)                                              /**< (ISC_CSC_CRB_OCR) Red Chrominance Offset (signed 11 bits 1:10:0) Position */
#define ISC_CSC_CRB_OCR_CROFST_Msk            (_U_(0x7FF) << ISC_CSC_CRB_OCR_CROFST_Pos)           /**< (ISC_CSC_CRB_OCR) Red Chrominance Offset (signed 11 bits 1:10:0) Mask */
#define ISC_CSC_CRB_OCR_CROFST(value)         (ISC_CSC_CRB_OCR_CROFST_Msk & ((value) << ISC_CSC_CRB_OCR_CROFST_Pos))
#define ISC_CSC_CRB_OCR_Msk                   _U_(0x07FF0FFF)                                      /**< (ISC_CSC_CRB_OCR) Register Mask  */


/* -------- ISC_CBC_CTRL : (ISC Offset: 0x3B4) (R/W 32) Contrast and Brightness Control Register -------- */
#define ISC_CBC_CTRL_ENABLE_Pos               _U_(0)                                               /**< (ISC_CBC_CTRL) Contrast and Brightness Control Enable Position */
#define ISC_CBC_CTRL_ENABLE_Msk               (_U_(0x1) << ISC_CBC_CTRL_ENABLE_Pos)                /**< (ISC_CBC_CTRL) Contrast and Brightness Control Enable Mask */
#define ISC_CBC_CTRL_ENABLE(value)            (ISC_CBC_CTRL_ENABLE_Msk & ((value) << ISC_CBC_CTRL_ENABLE_Pos))
#define ISC_CBC_CTRL_Msk                      _U_(0x00000001)                                      /**< (ISC_CBC_CTRL) Register Mask  */


/* -------- ISC_CBC_CFG : (ISC Offset: 0x3B8) (R/W 32) Contrast and Brightness Configuration Register -------- */
#define ISC_CBC_CFG_CCIR_Pos                  _U_(0)                                               /**< (ISC_CBC_CFG) CCIR656 Stream Enable Position */
#define ISC_CBC_CFG_CCIR_Msk                  (_U_(0x1) << ISC_CBC_CFG_CCIR_Pos)                   /**< (ISC_CBC_CFG) CCIR656 Stream Enable Mask */
#define ISC_CBC_CFG_CCIR(value)               (ISC_CBC_CFG_CCIR_Msk & ((value) << ISC_CBC_CFG_CCIR_Pos))
#define ISC_CBC_CFG_CCIRMODE_Pos              _U_(1)                                               /**< (ISC_CBC_CFG) CCIR656 Byte Ordering Position */
#define ISC_CBC_CFG_CCIRMODE_Msk              (_U_(0x3) << ISC_CBC_CFG_CCIRMODE_Pos)               /**< (ISC_CBC_CFG) CCIR656 Byte Ordering Mask */
#define ISC_CBC_CFG_CCIRMODE(value)           (ISC_CBC_CFG_CCIRMODE_Msk & ((value) << ISC_CBC_CFG_CCIRMODE_Pos))
#define   ISC_CBC_CFG_CCIRMODE_CBY_Val        _U_(0x0)                                             /**< (ISC_CBC_CFG) Byte ordering Cb0, Y0, Cr0, Y1  */
#define   ISC_CBC_CFG_CCIRMODE_CRY_Val        _U_(0x1)                                             /**< (ISC_CBC_CFG) Byte ordering Cr0, Y0, Cb0, Y1  */
#define   ISC_CBC_CFG_CCIRMODE_YCB_Val        _U_(0x2)                                             /**< (ISC_CBC_CFG) Byte ordering Y0, Cb0, Y1, Cr0  */
#define   ISC_CBC_CFG_CCIRMODE_YCR_Val        _U_(0x3)                                             /**< (ISC_CBC_CFG) Byte ordering Y0, Cr0, Y1, Cb0  */
#define ISC_CBC_CFG_CCIRMODE_CBY              (ISC_CBC_CFG_CCIRMODE_CBY_Val << ISC_CBC_CFG_CCIRMODE_Pos) /**< (ISC_CBC_CFG) Byte ordering Cb0, Y0, Cr0, Y1 Position  */
#define ISC_CBC_CFG_CCIRMODE_CRY              (ISC_CBC_CFG_CCIRMODE_CRY_Val << ISC_CBC_CFG_CCIRMODE_Pos) /**< (ISC_CBC_CFG) Byte ordering Cr0, Y0, Cb0, Y1 Position  */
#define ISC_CBC_CFG_CCIRMODE_YCB              (ISC_CBC_CFG_CCIRMODE_YCB_Val << ISC_CBC_CFG_CCIRMODE_Pos) /**< (ISC_CBC_CFG) Byte ordering Y0, Cb0, Y1, Cr0 Position  */
#define ISC_CBC_CFG_CCIRMODE_YCR              (ISC_CBC_CFG_CCIRMODE_YCR_Val << ISC_CBC_CFG_CCIRMODE_Pos) /**< (ISC_CBC_CFG) Byte ordering Y0, Cr0, Y1, Cb0 Position  */
#define ISC_CBC_CFG_Msk                       _U_(0x00000007)                                      /**< (ISC_CBC_CFG) Register Mask  */


/* -------- ISC_CBC_BRIGHT : (ISC Offset: 0x3BC) (R/W 32) Contrast and Brightness, Brightness Register -------- */
#define ISC_CBC_BRIGHT_BRIGHT_Pos             _U_(0)                                               /**< (ISC_CBC_BRIGHT) Brightness Control (signed 11 bits 1:10:0) Position */
#define ISC_CBC_BRIGHT_BRIGHT_Msk             (_U_(0x7FF) << ISC_CBC_BRIGHT_BRIGHT_Pos)            /**< (ISC_CBC_BRIGHT) Brightness Control (signed 11 bits 1:10:0) Mask */
#define ISC_CBC_BRIGHT_BRIGHT(value)          (ISC_CBC_BRIGHT_BRIGHT_Msk & ((value) << ISC_CBC_BRIGHT_BRIGHT_Pos))
#define ISC_CBC_BRIGHT_Msk                    _U_(0x000007FF)                                      /**< (ISC_CBC_BRIGHT) Register Mask  */


/* -------- ISC_CBC_CONTRAST : (ISC Offset: 0x3C0) (R/W 32) Contrast and Brightness, Contrast Register -------- */
#define ISC_CBC_CONTRAST_CONTRAST_Pos         _U_(0)                                               /**< (ISC_CBC_CONTRAST) Contrast (unsigned 12 bits 0:4:8) Position */
#define ISC_CBC_CONTRAST_CONTRAST_Msk         (_U_(0xFFF) << ISC_CBC_CONTRAST_CONTRAST_Pos)        /**< (ISC_CBC_CONTRAST) Contrast (unsigned 12 bits 0:4:8) Mask */
#define ISC_CBC_CONTRAST_CONTRAST(value)      (ISC_CBC_CONTRAST_CONTRAST_Msk & ((value) << ISC_CBC_CONTRAST_CONTRAST_Pos))
#define ISC_CBC_CONTRAST_Msk                  _U_(0x00000FFF)                                      /**< (ISC_CBC_CONTRAST) Register Mask  */


/* -------- ISC_SUB422_CTRL : (ISC Offset: 0x3C4) (R/W 32) Subsampling 4:4:4 to 4:2:2 Control Register -------- */
#define ISC_SUB422_CTRL_ENABLE_Pos            _U_(0)                                               /**< (ISC_SUB422_CTRL) 4:4:4 to 4:2:2 Chrominance Horizontal Subsampling Filter Enable Position */
#define ISC_SUB422_CTRL_ENABLE_Msk            (_U_(0x1) << ISC_SUB422_CTRL_ENABLE_Pos)             /**< (ISC_SUB422_CTRL) 4:4:4 to 4:2:2 Chrominance Horizontal Subsampling Filter Enable Mask */
#define ISC_SUB422_CTRL_ENABLE(value)         (ISC_SUB422_CTRL_ENABLE_Msk & ((value) << ISC_SUB422_CTRL_ENABLE_Pos))
#define ISC_SUB422_CTRL_Msk                   _U_(0x00000001)                                      /**< (ISC_SUB422_CTRL) Register Mask  */


/* -------- ISC_SUB422_CFG : (ISC Offset: 0x3C8) (R/W 32) Subsampling 4:4:4 to 4:2:2 Configuration Register -------- */
#define ISC_SUB422_CFG_CCIR_Pos               _U_(0)                                               /**< (ISC_SUB422_CFG) CCIR656 Input Stream Position */
#define ISC_SUB422_CFG_CCIR_Msk               (_U_(0x1) << ISC_SUB422_CFG_CCIR_Pos)                /**< (ISC_SUB422_CFG) CCIR656 Input Stream Mask */
#define ISC_SUB422_CFG_CCIR(value)            (ISC_SUB422_CFG_CCIR_Msk & ((value) << ISC_SUB422_CFG_CCIR_Pos))
#define ISC_SUB422_CFG_CCIRMODE_Pos           _U_(1)                                               /**< (ISC_SUB422_CFG) CCIR656 Byte Ordering Position */
#define ISC_SUB422_CFG_CCIRMODE_Msk           (_U_(0x3) << ISC_SUB422_CFG_CCIRMODE_Pos)            /**< (ISC_SUB422_CFG) CCIR656 Byte Ordering Mask */
#define ISC_SUB422_CFG_CCIRMODE(value)        (ISC_SUB422_CFG_CCIRMODE_Msk & ((value) << ISC_SUB422_CFG_CCIRMODE_Pos))
#define   ISC_SUB422_CFG_CCIRMODE_CBY_Val     _U_(0x0)                                             /**< (ISC_SUB422_CFG) Byte ordering Cb0, Y0, Cr0, Y1  */
#define   ISC_SUB422_CFG_CCIRMODE_CRY_Val     _U_(0x1)                                             /**< (ISC_SUB422_CFG) Byte ordering Cr0, Y0, Cb0, Y1  */
#define   ISC_SUB422_CFG_CCIRMODE_YCB_Val     _U_(0x2)                                             /**< (ISC_SUB422_CFG) Byte ordering Y0, Cb0, Y1, Cr0  */
#define   ISC_SUB422_CFG_CCIRMODE_YCR_Val     _U_(0x3)                                             /**< (ISC_SUB422_CFG) Byte ordering Y0, Cr0, Y1, Cb0  */
#define ISC_SUB422_CFG_CCIRMODE_CBY           (ISC_SUB422_CFG_CCIRMODE_CBY_Val << ISC_SUB422_CFG_CCIRMODE_Pos) /**< (ISC_SUB422_CFG) Byte ordering Cb0, Y0, Cr0, Y1 Position  */
#define ISC_SUB422_CFG_CCIRMODE_CRY           (ISC_SUB422_CFG_CCIRMODE_CRY_Val << ISC_SUB422_CFG_CCIRMODE_Pos) /**< (ISC_SUB422_CFG) Byte ordering Cr0, Y0, Cb0, Y1 Position  */
#define ISC_SUB422_CFG_CCIRMODE_YCB           (ISC_SUB422_CFG_CCIRMODE_YCB_Val << ISC_SUB422_CFG_CCIRMODE_Pos) /**< (ISC_SUB422_CFG) Byte ordering Y0, Cb0, Y1, Cr0 Position  */
#define ISC_SUB422_CFG_CCIRMODE_YCR           (ISC_SUB422_CFG_CCIRMODE_YCR_Val << ISC_SUB422_CFG_CCIRMODE_Pos) /**< (ISC_SUB422_CFG) Byte ordering Y0, Cr0, Y1, Cb0 Position  */
#define ISC_SUB422_CFG_FILTER_Pos             _U_(4)                                               /**< (ISC_SUB422_CFG) Low Pass Filter Selection Position */
#define ISC_SUB422_CFG_FILTER_Msk             (_U_(0x3) << ISC_SUB422_CFG_FILTER_Pos)              /**< (ISC_SUB422_CFG) Low Pass Filter Selection Mask */
#define ISC_SUB422_CFG_FILTER(value)          (ISC_SUB422_CFG_FILTER_Msk & ((value) << ISC_SUB422_CFG_FILTER_Pos))
#define   ISC_SUB422_CFG_FILTER_FILT0CO_Val   _U_(0x0)                                             /**< (ISC_SUB422_CFG) Cosited, {1}  */
#define   ISC_SUB422_CFG_FILTER_FILT1CE_Val   _U_(0x1)                                             /**< (ISC_SUB422_CFG) Centered {1, 1}  */
#define   ISC_SUB422_CFG_FILTER_FILT2CO_Val   _U_(0x2)                                             /**< (ISC_SUB422_CFG) Cosited {1,2,1}  */
#define   ISC_SUB422_CFG_FILTER_FILT3CE_Val   _U_(0x3)                                             /**< (ISC_SUB422_CFG) Centered {1, 3, 3, 1}  */
#define ISC_SUB422_CFG_FILTER_FILT0CO         (ISC_SUB422_CFG_FILTER_FILT0CO_Val << ISC_SUB422_CFG_FILTER_Pos) /**< (ISC_SUB422_CFG) Cosited, {1} Position  */
#define ISC_SUB422_CFG_FILTER_FILT1CE         (ISC_SUB422_CFG_FILTER_FILT1CE_Val << ISC_SUB422_CFG_FILTER_Pos) /**< (ISC_SUB422_CFG) Centered {1, 1} Position  */
#define ISC_SUB422_CFG_FILTER_FILT2CO         (ISC_SUB422_CFG_FILTER_FILT2CO_Val << ISC_SUB422_CFG_FILTER_Pos) /**< (ISC_SUB422_CFG) Cosited {1,2,1} Position  */
#define ISC_SUB422_CFG_FILTER_FILT3CE         (ISC_SUB422_CFG_FILTER_FILT3CE_Val << ISC_SUB422_CFG_FILTER_Pos) /**< (ISC_SUB422_CFG) Centered {1, 3, 3, 1} Position  */
#define ISC_SUB422_CFG_Msk                    _U_(0x00000037)                                      /**< (ISC_SUB422_CFG) Register Mask  */


/* -------- ISC_SUB420_CTRL : (ISC Offset: 0x3CC) (R/W 32) Subsampling 4:2:2 to 4:2:0 Control Register -------- */
#define ISC_SUB420_CTRL_ENABLE_Pos            _U_(0)                                               /**< (ISC_SUB420_CTRL) 4:2:2 to 4:2:0 Vertical Subsampling Filter Enable (Center Aligned) Position */
#define ISC_SUB420_CTRL_ENABLE_Msk            (_U_(0x1) << ISC_SUB420_CTRL_ENABLE_Pos)             /**< (ISC_SUB420_CTRL) 4:2:2 to 4:2:0 Vertical Subsampling Filter Enable (Center Aligned) Mask */
#define ISC_SUB420_CTRL_ENABLE(value)         (ISC_SUB420_CTRL_ENABLE_Msk & ((value) << ISC_SUB420_CTRL_ENABLE_Pos))
#define ISC_SUB420_CTRL_FILTER_Pos            _U_(4)                                               /**< (ISC_SUB420_CTRL) Interlaced or Progressive Chrominance Filter Position */
#define ISC_SUB420_CTRL_FILTER_Msk            (_U_(0x1) << ISC_SUB420_CTRL_FILTER_Pos)             /**< (ISC_SUB420_CTRL) Interlaced or Progressive Chrominance Filter Mask */
#define ISC_SUB420_CTRL_FILTER(value)         (ISC_SUB420_CTRL_FILTER_Msk & ((value) << ISC_SUB420_CTRL_FILTER_Pos))
#define ISC_SUB420_CTRL_Msk                   _U_(0x00000011)                                      /**< (ISC_SUB420_CTRL) Register Mask  */


/* -------- ISC_RLP_CFG : (ISC Offset: 0x3D0) (R/W 32) Rounding, Limiting and Packing Configuration Register -------- */
#define ISC_RLP_CFG_MODE_Pos                  _U_(0)                                               /**< (ISC_RLP_CFG) Rounding, Limiting and Packing Mode Position */
#define ISC_RLP_CFG_MODE_Msk                  (_U_(0xF) << ISC_RLP_CFG_MODE_Pos)                   /**< (ISC_RLP_CFG) Rounding, Limiting and Packing Mode Mask */
#define ISC_RLP_CFG_MODE(value)               (ISC_RLP_CFG_MODE_Msk & ((value) << ISC_RLP_CFG_MODE_Pos))
#define   ISC_RLP_CFG_MODE_DAT8_Val           _U_(0x0)                                             /**< (ISC_RLP_CFG) 8-bit data  */
#define   ISC_RLP_CFG_MODE_DAT9_Val           _U_(0x1)                                             /**< (ISC_RLP_CFG) 9-bit data  */
#define   ISC_RLP_CFG_MODE_DAT10_Val          _U_(0x2)                                             /**< (ISC_RLP_CFG) 10-bit data  */
#define   ISC_RLP_CFG_MODE_DAT11_Val          _U_(0x3)                                             /**< (ISC_RLP_CFG) 11-bit data  */
#define   ISC_RLP_CFG_MODE_DAT12_Val          _U_(0x4)                                             /**< (ISC_RLP_CFG) 12-bit data  */
#define   ISC_RLP_CFG_MODE_DATY8_Val          _U_(0x5)                                             /**< (ISC_RLP_CFG) 8-bit luminance only  */
#define   ISC_RLP_CFG_MODE_DATY10_Val         _U_(0x6)                                             /**< (ISC_RLP_CFG) 10-bit luminance only  */
#define   ISC_RLP_CFG_MODE_ARGB444_Val        _U_(0x7)                                             /**< (ISC_RLP_CFG) 12-bit RGB+4-bit Alpha (MSB)  */
#define   ISC_RLP_CFG_MODE_ARGB555_Val        _U_(0x8)                                             /**< (ISC_RLP_CFG) 15-bit RGB+1-bit Alpha (MSB)  */
#define   ISC_RLP_CFG_MODE_RGB565_Val         _U_(0x9)                                             /**< (ISC_RLP_CFG) 16-bit RGB  */
#define   ISC_RLP_CFG_MODE_ARGB32_Val         _U_(0xA)                                             /**< (ISC_RLP_CFG) 24-bits RGB mode+8-bit Alpha  */
#define   ISC_RLP_CFG_MODE_YYCC_Val           _U_(0xB)                                             /**< (ISC_RLP_CFG) YCbCr mode (full range, [0-255])  */
#define   ISC_RLP_CFG_MODE_YYCC_LIMITED_Val   _U_(0xC)                                             /**< (ISC_RLP_CFG) YCbCr mode (limited range)  */
#define ISC_RLP_CFG_MODE_DAT8                 (ISC_RLP_CFG_MODE_DAT8_Val << ISC_RLP_CFG_MODE_Pos)  /**< (ISC_RLP_CFG) 8-bit data Position  */
#define ISC_RLP_CFG_MODE_DAT9                 (ISC_RLP_CFG_MODE_DAT9_Val << ISC_RLP_CFG_MODE_Pos)  /**< (ISC_RLP_CFG) 9-bit data Position  */
#define ISC_RLP_CFG_MODE_DAT10                (ISC_RLP_CFG_MODE_DAT10_Val << ISC_RLP_CFG_MODE_Pos) /**< (ISC_RLP_CFG) 10-bit data Position  */
#define ISC_RLP_CFG_MODE_DAT11                (ISC_RLP_CFG_MODE_DAT11_Val << ISC_RLP_CFG_MODE_Pos) /**< (ISC_RLP_CFG) 11-bit data Position  */
#define ISC_RLP_CFG_MODE_DAT12                (ISC_RLP_CFG_MODE_DAT12_Val << ISC_RLP_CFG_MODE_Pos) /**< (ISC_RLP_CFG) 12-bit data Position  */
#define ISC_RLP_CFG_MODE_DATY8                (ISC_RLP_CFG_MODE_DATY8_Val << ISC_RLP_CFG_MODE_Pos) /**< (ISC_RLP_CFG) 8-bit luminance only Position  */
#define ISC_RLP_CFG_MODE_DATY10               (ISC_RLP_CFG_MODE_DATY10_Val << ISC_RLP_CFG_MODE_Pos) /**< (ISC_RLP_CFG) 10-bit luminance only Position  */
#define ISC_RLP_CFG_MODE_ARGB444              (ISC_RLP_CFG_MODE_ARGB444_Val << ISC_RLP_CFG_MODE_Pos) /**< (ISC_RLP_CFG) 12-bit RGB+4-bit Alpha (MSB) Position  */
#define ISC_RLP_CFG_MODE_ARGB555              (ISC_RLP_CFG_MODE_ARGB555_Val << ISC_RLP_CFG_MODE_Pos) /**< (ISC_RLP_CFG) 15-bit RGB+1-bit Alpha (MSB) Position  */
#define ISC_RLP_CFG_MODE_RGB565               (ISC_RLP_CFG_MODE_RGB565_Val << ISC_RLP_CFG_MODE_Pos) /**< (ISC_RLP_CFG) 16-bit RGB Position  */
#define ISC_RLP_CFG_MODE_ARGB32               (ISC_RLP_CFG_MODE_ARGB32_Val << ISC_RLP_CFG_MODE_Pos) /**< (ISC_RLP_CFG) 24-bits RGB mode+8-bit Alpha Position  */
#define ISC_RLP_CFG_MODE_YYCC                 (ISC_RLP_CFG_MODE_YYCC_Val << ISC_RLP_CFG_MODE_Pos)  /**< (ISC_RLP_CFG) YCbCr mode (full range, [0-255]) Position  */
#define ISC_RLP_CFG_MODE_YYCC_LIMITED         (ISC_RLP_CFG_MODE_YYCC_LIMITED_Val << ISC_RLP_CFG_MODE_Pos) /**< (ISC_RLP_CFG) YCbCr mode (limited range) Position  */
#define ISC_RLP_CFG_ALPHA_Pos                 _U_(8)                                               /**< (ISC_RLP_CFG) Alpha Value for Alpha-enabled RGB Mode Position */
#define ISC_RLP_CFG_ALPHA_Msk                 (_U_(0xFF) << ISC_RLP_CFG_ALPHA_Pos)                 /**< (ISC_RLP_CFG) Alpha Value for Alpha-enabled RGB Mode Mask */
#define ISC_RLP_CFG_ALPHA(value)              (ISC_RLP_CFG_ALPHA_Msk & ((value) << ISC_RLP_CFG_ALPHA_Pos))
#define ISC_RLP_CFG_Msk                       _U_(0x0000FF0F)                                      /**< (ISC_RLP_CFG) Register Mask  */


/* -------- ISC_HIS_CTRL : (ISC Offset: 0x3D4) (R/W 32) Histogram Control Register -------- */
#define ISC_HIS_CTRL_ENABLE_Pos               _U_(0)                                               /**< (ISC_HIS_CTRL) Histogram Sub Module Enable Position */
#define ISC_HIS_CTRL_ENABLE_Msk               (_U_(0x1) << ISC_HIS_CTRL_ENABLE_Pos)                /**< (ISC_HIS_CTRL) Histogram Sub Module Enable Mask */
#define ISC_HIS_CTRL_ENABLE(value)            (ISC_HIS_CTRL_ENABLE_Msk & ((value) << ISC_HIS_CTRL_ENABLE_Pos))
#define ISC_HIS_CTRL_Msk                      _U_(0x00000001)                                      /**< (ISC_HIS_CTRL) Register Mask  */


/* -------- ISC_HIS_CFG : (ISC Offset: 0x3D8) (R/W 32) Histogram Configuration Register -------- */
#define ISC_HIS_CFG_MODE_Pos                  _U_(0)                                               /**< (ISC_HIS_CFG) Histogram Operating Mode Position */
#define ISC_HIS_CFG_MODE_Msk                  (_U_(0x7) << ISC_HIS_CFG_MODE_Pos)                   /**< (ISC_HIS_CFG) Histogram Operating Mode Mask */
#define ISC_HIS_CFG_MODE(value)               (ISC_HIS_CFG_MODE_Msk & ((value) << ISC_HIS_CFG_MODE_Pos))
#define   ISC_HIS_CFG_MODE_Gr_Val             _U_(0x0)                                             /**< (ISC_HIS_CFG) Gr sampling  */
#define   ISC_HIS_CFG_MODE_R_Val              _U_(0x1)                                             /**< (ISC_HIS_CFG) R sampling  */
#define   ISC_HIS_CFG_MODE_Gb_Val             _U_(0x2)                                             /**< (ISC_HIS_CFG) Gb sampling  */
#define   ISC_HIS_CFG_MODE_B_Val              _U_(0x3)                                             /**< (ISC_HIS_CFG) B sampling  */
#define   ISC_HIS_CFG_MODE_Y_Val              _U_(0x4)                                             /**< (ISC_HIS_CFG) Luminance-only mode  */
#define   ISC_HIS_CFG_MODE_RAW_Val            _U_(0x5)                                             /**< (ISC_HIS_CFG) Raw sampling  */
#define   ISC_HIS_CFG_MODE_YCCIR656_Val       _U_(0x6)                                             /**< (ISC_HIS_CFG) Luminance only with CCIR656 10-bit or 8-bit mode  */
#define ISC_HIS_CFG_MODE_Gr                   (ISC_HIS_CFG_MODE_Gr_Val << ISC_HIS_CFG_MODE_Pos)    /**< (ISC_HIS_CFG) Gr sampling Position  */
#define ISC_HIS_CFG_MODE_R                    (ISC_HIS_CFG_MODE_R_Val << ISC_HIS_CFG_MODE_Pos)     /**< (ISC_HIS_CFG) R sampling Position  */
#define ISC_HIS_CFG_MODE_Gb                   (ISC_HIS_CFG_MODE_Gb_Val << ISC_HIS_CFG_MODE_Pos)    /**< (ISC_HIS_CFG) Gb sampling Position  */
#define ISC_HIS_CFG_MODE_B                    (ISC_HIS_CFG_MODE_B_Val << ISC_HIS_CFG_MODE_Pos)     /**< (ISC_HIS_CFG) B sampling Position  */
#define ISC_HIS_CFG_MODE_Y                    (ISC_HIS_CFG_MODE_Y_Val << ISC_HIS_CFG_MODE_Pos)     /**< (ISC_HIS_CFG) Luminance-only mode Position  */
#define ISC_HIS_CFG_MODE_RAW                  (ISC_HIS_CFG_MODE_RAW_Val << ISC_HIS_CFG_MODE_Pos)   /**< (ISC_HIS_CFG) Raw sampling Position  */
#define ISC_HIS_CFG_MODE_YCCIR656             (ISC_HIS_CFG_MODE_YCCIR656_Val << ISC_HIS_CFG_MODE_Pos) /**< (ISC_HIS_CFG) Luminance only with CCIR656 10-bit or 8-bit mode Position  */
#define ISC_HIS_CFG_BAYSEL_Pos                _U_(4)                                               /**< (ISC_HIS_CFG) Bayer Color Component Selection Position */
#define ISC_HIS_CFG_BAYSEL_Msk                (_U_(0x3) << ISC_HIS_CFG_BAYSEL_Pos)                 /**< (ISC_HIS_CFG) Bayer Color Component Selection Mask */
#define ISC_HIS_CFG_BAYSEL(value)             (ISC_HIS_CFG_BAYSEL_Msk & ((value) << ISC_HIS_CFG_BAYSEL_Pos))
#define   ISC_HIS_CFG_BAYSEL_GRGR_Val         _U_(0x0)                                             /**< (ISC_HIS_CFG) Starting row configuration is G R G R (red row)  */
#define   ISC_HIS_CFG_BAYSEL_RGRG_Val         _U_(0x1)                                             /**< (ISC_HIS_CFG) Starting row configuration is R G R G (red row)  */
#define   ISC_HIS_CFG_BAYSEL_GBGB_Val         _U_(0x2)                                             /**< (ISC_HIS_CFG) Starting row configuration is G B G B (blue row  */
#define   ISC_HIS_CFG_BAYSEL_BGBG_Val         _U_(0x3)                                             /**< (ISC_HIS_CFG) Starting row configuration is B G B G (blue row)  */
#define ISC_HIS_CFG_BAYSEL_GRGR               (ISC_HIS_CFG_BAYSEL_GRGR_Val << ISC_HIS_CFG_BAYSEL_Pos) /**< (ISC_HIS_CFG) Starting row configuration is G R G R (red row) Position  */
#define ISC_HIS_CFG_BAYSEL_RGRG               (ISC_HIS_CFG_BAYSEL_RGRG_Val << ISC_HIS_CFG_BAYSEL_Pos) /**< (ISC_HIS_CFG) Starting row configuration is R G R G (red row) Position  */
#define ISC_HIS_CFG_BAYSEL_GBGB               (ISC_HIS_CFG_BAYSEL_GBGB_Val << ISC_HIS_CFG_BAYSEL_Pos) /**< (ISC_HIS_CFG) Starting row configuration is G B G B (blue row Position  */
#define ISC_HIS_CFG_BAYSEL_BGBG               (ISC_HIS_CFG_BAYSEL_BGBG_Val << ISC_HIS_CFG_BAYSEL_Pos) /**< (ISC_HIS_CFG) Starting row configuration is B G B G (blue row) Position  */
#define ISC_HIS_CFG_RAR_Pos                   _U_(8)                                               /**< (ISC_HIS_CFG) Histogram Reset After Read Position */
#define ISC_HIS_CFG_RAR_Msk                   (_U_(0x1) << ISC_HIS_CFG_RAR_Pos)                    /**< (ISC_HIS_CFG) Histogram Reset After Read Mask */
#define ISC_HIS_CFG_RAR(value)                (ISC_HIS_CFG_RAR_Msk & ((value) << ISC_HIS_CFG_RAR_Pos))
#define ISC_HIS_CFG_Msk                       _U_(0x00000137)                                      /**< (ISC_HIS_CFG) Register Mask  */


/* -------- ISC_DCFG : (ISC Offset: 0x3E0) (R/W 32) DMA Configuration Register -------- */
#define ISC_DCFG_IMODE_Pos                    _U_(0)                                               /**< (ISC_DCFG) DMA Input Mode Selection Position */
#define ISC_DCFG_IMODE_Msk                    (_U_(0x7) << ISC_DCFG_IMODE_Pos)                     /**< (ISC_DCFG) DMA Input Mode Selection Mask */
#define ISC_DCFG_IMODE(value)                 (ISC_DCFG_IMODE_Msk & ((value) << ISC_DCFG_IMODE_Pos))
#define   ISC_DCFG_IMODE_PACKED8_Val          _U_(0x0)                                             /**< (ISC_DCFG) 8 bits, single channel packed  */
#define   ISC_DCFG_IMODE_PACKED16_Val         _U_(0x1)                                             /**< (ISC_DCFG) 16 bits, single channel packed  */
#define   ISC_DCFG_IMODE_PACKED32_Val         _U_(0x2)                                             /**< (ISC_DCFG) 32 bits, single channel packed  */
#define   ISC_DCFG_IMODE_YC422SP_Val          _U_(0x3)                                             /**< (ISC_DCFG) 32 bits, dual channel  */
#define   ISC_DCFG_IMODE_YC422P_Val           _U_(0x4)                                             /**< (ISC_DCFG) 32 bits, triple channel  */
#define   ISC_DCFG_IMODE_YC420SP_Val          _U_(0x5)                                             /**< (ISC_DCFG) 32 bits, dual channel  */
#define   ISC_DCFG_IMODE_YC420P_Val           _U_(0x6)                                             /**< (ISC_DCFG) 32 bits, triple channel  */
#define ISC_DCFG_IMODE_PACKED8                (ISC_DCFG_IMODE_PACKED8_Val << ISC_DCFG_IMODE_Pos)   /**< (ISC_DCFG) 8 bits, single channel packed Position  */
#define ISC_DCFG_IMODE_PACKED16               (ISC_DCFG_IMODE_PACKED16_Val << ISC_DCFG_IMODE_Pos)  /**< (ISC_DCFG) 16 bits, single channel packed Position  */
#define ISC_DCFG_IMODE_PACKED32               (ISC_DCFG_IMODE_PACKED32_Val << ISC_DCFG_IMODE_Pos)  /**< (ISC_DCFG) 32 bits, single channel packed Position  */
#define ISC_DCFG_IMODE_YC422SP                (ISC_DCFG_IMODE_YC422SP_Val << ISC_DCFG_IMODE_Pos)   /**< (ISC_DCFG) 32 bits, dual channel Position  */
#define ISC_DCFG_IMODE_YC422P                 (ISC_DCFG_IMODE_YC422P_Val << ISC_DCFG_IMODE_Pos)    /**< (ISC_DCFG) 32 bits, triple channel Position  */
#define ISC_DCFG_IMODE_YC420SP                (ISC_DCFG_IMODE_YC420SP_Val << ISC_DCFG_IMODE_Pos)   /**< (ISC_DCFG) 32 bits, dual channel Position  */
#define ISC_DCFG_IMODE_YC420P                 (ISC_DCFG_IMODE_YC420P_Val << ISC_DCFG_IMODE_Pos)    /**< (ISC_DCFG) 32 bits, triple channel Position  */
#define ISC_DCFG_YMBSIZE_Pos                  _U_(4)                                               /**< (ISC_DCFG) DMA Memory Burst Size Y channel Position */
#define ISC_DCFG_YMBSIZE_Msk                  (_U_(0x3) << ISC_DCFG_YMBSIZE_Pos)                   /**< (ISC_DCFG) DMA Memory Burst Size Y channel Mask */
#define ISC_DCFG_YMBSIZE(value)               (ISC_DCFG_YMBSIZE_Msk & ((value) << ISC_DCFG_YMBSIZE_Pos))
#define   ISC_DCFG_YMBSIZE_SINGLE_Val         _U_(0x0)                                             /**< (ISC_DCFG) DMA single access  */
#define   ISC_DCFG_YMBSIZE_BEATS4_Val         _U_(0x1)                                             /**< (ISC_DCFG) 4-beat burst access  */
#define   ISC_DCFG_YMBSIZE_BEATS8_Val         _U_(0x2)                                             /**< (ISC_DCFG) 8-beat burst access  */
#define   ISC_DCFG_YMBSIZE_BEATS16_Val        _U_(0x3)                                             /**< (ISC_DCFG) 16-beat burst access  */
#define ISC_DCFG_YMBSIZE_SINGLE               (ISC_DCFG_YMBSIZE_SINGLE_Val << ISC_DCFG_YMBSIZE_Pos) /**< (ISC_DCFG) DMA single access Position  */
#define ISC_DCFG_YMBSIZE_BEATS4               (ISC_DCFG_YMBSIZE_BEATS4_Val << ISC_DCFG_YMBSIZE_Pos) /**< (ISC_DCFG) 4-beat burst access Position  */
#define ISC_DCFG_YMBSIZE_BEATS8               (ISC_DCFG_YMBSIZE_BEATS8_Val << ISC_DCFG_YMBSIZE_Pos) /**< (ISC_DCFG) 8-beat burst access Position  */
#define ISC_DCFG_YMBSIZE_BEATS16              (ISC_DCFG_YMBSIZE_BEATS16_Val << ISC_DCFG_YMBSIZE_Pos) /**< (ISC_DCFG) 16-beat burst access Position  */
#define ISC_DCFG_CMBSIZE_Pos                  _U_(8)                                               /**< (ISC_DCFG) DMA Memory Burst Size C channel Position */
#define ISC_DCFG_CMBSIZE_Msk                  (_U_(0x3) << ISC_DCFG_CMBSIZE_Pos)                   /**< (ISC_DCFG) DMA Memory Burst Size C channel Mask */
#define ISC_DCFG_CMBSIZE(value)               (ISC_DCFG_CMBSIZE_Msk & ((value) << ISC_DCFG_CMBSIZE_Pos))
#define   ISC_DCFG_CMBSIZE_SINGLE_Val         _U_(0x0)                                             /**< (ISC_DCFG) DMA single access  */
#define   ISC_DCFG_CMBSIZE_BEATS4_Val         _U_(0x1)                                             /**< (ISC_DCFG) 4-beat burst access  */
#define   ISC_DCFG_CMBSIZE_BEATS8_Val         _U_(0x2)                                             /**< (ISC_DCFG) 8-beat burst access  */
#define   ISC_DCFG_CMBSIZE_BEATS16_Val        _U_(0x3)                                             /**< (ISC_DCFG) 16-beat burst access  */
#define ISC_DCFG_CMBSIZE_SINGLE               (ISC_DCFG_CMBSIZE_SINGLE_Val << ISC_DCFG_CMBSIZE_Pos) /**< (ISC_DCFG) DMA single access Position  */
#define ISC_DCFG_CMBSIZE_BEATS4               (ISC_DCFG_CMBSIZE_BEATS4_Val << ISC_DCFG_CMBSIZE_Pos) /**< (ISC_DCFG) 4-beat burst access Position  */
#define ISC_DCFG_CMBSIZE_BEATS8               (ISC_DCFG_CMBSIZE_BEATS8_Val << ISC_DCFG_CMBSIZE_Pos) /**< (ISC_DCFG) 8-beat burst access Position  */
#define ISC_DCFG_CMBSIZE_BEATS16              (ISC_DCFG_CMBSIZE_BEATS16_Val << ISC_DCFG_CMBSIZE_Pos) /**< (ISC_DCFG) 16-beat burst access Position  */
#define ISC_DCFG_Msk                          _U_(0x00000337)                                      /**< (ISC_DCFG) Register Mask  */


/* -------- ISC_DCTRL : (ISC Offset: 0x3E4) (R/W 32) DMA Control Register -------- */
#define ISC_DCTRL_DE_Pos                      _U_(0)                                               /**< (ISC_DCTRL) Descriptor Enable Position */
#define ISC_DCTRL_DE_Msk                      (_U_(0x1) << ISC_DCTRL_DE_Pos)                       /**< (ISC_DCTRL) Descriptor Enable Mask */
#define ISC_DCTRL_DE(value)                   (ISC_DCTRL_DE_Msk & ((value) << ISC_DCTRL_DE_Pos))  
#define ISC_DCTRL_DVIEW_Pos                   _U_(1)                                               /**< (ISC_DCTRL) Descriptor View Position */
#define ISC_DCTRL_DVIEW_Msk                   (_U_(0x3) << ISC_DCTRL_DVIEW_Pos)                    /**< (ISC_DCTRL) Descriptor View Mask */
#define ISC_DCTRL_DVIEW(value)                (ISC_DCTRL_DVIEW_Msk & ((value) << ISC_DCTRL_DVIEW_Pos))
#define   ISC_DCTRL_DVIEW_PACKED_Val          _U_(0x0)                                             /**< (ISC_DCTRL) Address {0} Stride {0} are updated  */
#define   ISC_DCTRL_DVIEW_SEMIPLANAR_Val      _U_(0x1)                                             /**< (ISC_DCTRL) Address {0,1} Stride {0,1} are updated  */
#define   ISC_DCTRL_DVIEW_PLANAR_Val          _U_(0x2)                                             /**< (ISC_DCTRL) Address {0,1,2} Stride {0,1,2} are updated  */
#define ISC_DCTRL_DVIEW_PACKED                (ISC_DCTRL_DVIEW_PACKED_Val << ISC_DCTRL_DVIEW_Pos)  /**< (ISC_DCTRL) Address {0} Stride {0} are updated Position  */
#define ISC_DCTRL_DVIEW_SEMIPLANAR            (ISC_DCTRL_DVIEW_SEMIPLANAR_Val << ISC_DCTRL_DVIEW_Pos) /**< (ISC_DCTRL) Address {0,1} Stride {0,1} are updated Position  */
#define ISC_DCTRL_DVIEW_PLANAR                (ISC_DCTRL_DVIEW_PLANAR_Val << ISC_DCTRL_DVIEW_Pos)  /**< (ISC_DCTRL) Address {0,1,2} Stride {0,1,2} are updated Position  */
#define ISC_DCTRL_IE_Pos                      _U_(4)                                               /**< (ISC_DCTRL) Interrupt Enable Position */
#define ISC_DCTRL_IE_Msk                      (_U_(0x1) << ISC_DCTRL_IE_Pos)                       /**< (ISC_DCTRL) Interrupt Enable Mask */
#define ISC_DCTRL_IE(value)                   (ISC_DCTRL_IE_Msk & ((value) << ISC_DCTRL_IE_Pos))  
#define ISC_DCTRL_WB_Pos                      _U_(5)                                               /**< (ISC_DCTRL) Write Back Operation Enable Position */
#define ISC_DCTRL_WB_Msk                      (_U_(0x1) << ISC_DCTRL_WB_Pos)                       /**< (ISC_DCTRL) Write Back Operation Enable Mask */
#define ISC_DCTRL_WB(value)                   (ISC_DCTRL_WB_Msk & ((value) << ISC_DCTRL_WB_Pos))  
#define ISC_DCTRL_FIELD_Pos                   _U_(6)                                               /**< (ISC_DCTRL) Value of Captured Frame Field Signal(1)(2) Position */
#define ISC_DCTRL_FIELD_Msk                   (_U_(0x1) << ISC_DCTRL_FIELD_Pos)                    /**< (ISC_DCTRL) Value of Captured Frame Field Signal(1)(2) Mask */
#define ISC_DCTRL_FIELD(value)                (ISC_DCTRL_FIELD_Msk & ((value) << ISC_DCTRL_FIELD_Pos))
#define ISC_DCTRL_DONE_Pos                    _U_(7)                                               /**< (ISC_DCTRL) Descriptor Processing Status(2) Position */
#define ISC_DCTRL_DONE_Msk                    (_U_(0x1) << ISC_DCTRL_DONE_Pos)                     /**< (ISC_DCTRL) Descriptor Processing Status(2) Mask */
#define ISC_DCTRL_DONE(value)                 (ISC_DCTRL_DONE_Msk & ((value) << ISC_DCTRL_DONE_Pos))
#define ISC_DCTRL_Msk                         _U_(0x000000F7)                                      /**< (ISC_DCTRL) Register Mask  */


/* -------- ISC_DNDA : (ISC Offset: 0x3E8) (R/W 32) DMA Descriptor Address Register -------- */
#define ISC_DNDA_NDA_Pos                      _U_(2)                                               /**< (ISC_DNDA) Next Descriptor Address Register Position */
#define ISC_DNDA_NDA_Msk                      (_U_(0x3FFFFFFF) << ISC_DNDA_NDA_Pos)                /**< (ISC_DNDA) Next Descriptor Address Register Mask */
#define ISC_DNDA_NDA(value)                   (ISC_DNDA_NDA_Msk & ((value) << ISC_DNDA_NDA_Pos))  
#define ISC_DNDA_Msk                          _U_(0xFFFFFFFC)                                      /**< (ISC_DNDA) Register Mask  */


/* -------- ISC_HIS_ENTRY : (ISC Offset: 0x410) ( R/ 32) Histogram Entry -------- */
#define ISC_HIS_ENTRY_COUNT_Pos               _U_(0)                                               /**< (ISC_HIS_ENTRY) Entry Counter Position */
#define ISC_HIS_ENTRY_COUNT_Msk               (_U_(0xFFFFF) << ISC_HIS_ENTRY_COUNT_Pos)            /**< (ISC_HIS_ENTRY) Entry Counter Mask */
#define ISC_HIS_ENTRY_COUNT(value)            (ISC_HIS_ENTRY_COUNT_Msk & ((value) << ISC_HIS_ENTRY_COUNT_Pos))
#define ISC_HIS_ENTRY_Msk                     _U_(0x000FFFFF)                                      /**< (ISC_HIS_ENTRY) Register Mask  */


/** \brief ISC register offsets definitions */
#define ISC_DAD_REG_OFST               (0x00)              /**< (ISC_DAD) DMA Address 0 Register Offset */
#define ISC_DST_REG_OFST               (0x04)              /**< (ISC_DST) DMA Stride 0 Register Offset */
#define ISC_CTRLEN_REG_OFST            (0x00)              /**< (ISC_CTRLEN) Control Enable Register Offset */
#define ISC_CTRLDIS_REG_OFST           (0x04)              /**< (ISC_CTRLDIS) Control Disable Register Offset */
#define ISC_CTRLSR_REG_OFST            (0x08)              /**< (ISC_CTRLSR) Control Status Register Offset */
#define ISC_PFE_CFG0_REG_OFST          (0x0C)              /**< (ISC_PFE_CFG0) Parallel Front End Configuration 0 Register Offset */
#define ISC_PFE_CFG1_REG_OFST          (0x10)              /**< (ISC_PFE_CFG1) Parallel Front End Configuration 1 Register Offset */
#define ISC_PFE_CFG2_REG_OFST          (0x14)              /**< (ISC_PFE_CFG2) Parallel Front End Configuration 2 Register Offset */
#define ISC_CLKEN_REG_OFST             (0x18)              /**< (ISC_CLKEN) Clock Enable Register Offset */
#define ISC_CLKDIS_REG_OFST            (0x1C)              /**< (ISC_CLKDIS) Clock Disable Register Offset */
#define ISC_CLKSR_REG_OFST             (0x20)              /**< (ISC_CLKSR) Clock Status Register Offset */
#define ISC_CLKCFG_REG_OFST            (0x24)              /**< (ISC_CLKCFG) Clock Configuration Register Offset */
#define ISC_INTEN_REG_OFST             (0x28)              /**< (ISC_INTEN) Interrupt Enable Register Offset */
#define ISC_INTDIS_REG_OFST            (0x2C)              /**< (ISC_INTDIS) Interrupt Disable Register Offset */
#define ISC_INTMASK_REG_OFST           (0x30)              /**< (ISC_INTMASK) Interrupt Mask Register Offset */
#define ISC_INTSR_REG_OFST             (0x34)              /**< (ISC_INTSR) Interrupt Status Register Offset */
#define ISC_WB_CTRL_REG_OFST           (0x58)              /**< (ISC_WB_CTRL) White Balance Control Register Offset */
#define ISC_WB_CFG_REG_OFST            (0x5C)              /**< (ISC_WB_CFG) White Balance Configuration Register Offset */
#define ISC_WB_O_RGR_REG_OFST          (0x60)              /**< (ISC_WB_O_RGR) White Balance Offset for R, GR Register Offset */
#define ISC_WB_O_BGB_REG_OFST          (0x64)              /**< (ISC_WB_O_BGB) White Balance Offset for B, GB Register Offset */
#define ISC_WB_G_RGR_REG_OFST          (0x68)              /**< (ISC_WB_G_RGR) White Balance Gain for R, GR Register Offset */
#define ISC_WB_G_BGB_REG_OFST          (0x6C)              /**< (ISC_WB_G_BGB) White Balance Gain for B, GB Register Offset */
#define ISC_CFA_CTRL_REG_OFST          (0x70)              /**< (ISC_CFA_CTRL) Color Filter Array Control Register Offset */
#define ISC_CFA_CFG_REG_OFST           (0x74)              /**< (ISC_CFA_CFG) Color Filter Array Configuration Register Offset */
#define ISC_CC_CTRL_REG_OFST           (0x78)              /**< (ISC_CC_CTRL) Color Correction Control Register Offset */
#define ISC_CC_RR_RG_REG_OFST          (0x7C)              /**< (ISC_CC_RR_RG) Color Correction RR RG Register Offset */
#define ISC_CC_RB_OR_REG_OFST          (0x80)              /**< (ISC_CC_RB_OR) Color Correction RB OR Register Offset */
#define ISC_CC_GR_GG_REG_OFST          (0x84)              /**< (ISC_CC_GR_GG) Color Correction GR GG Register Offset */
#define ISC_CC_GB_OG_REG_OFST          (0x88)              /**< (ISC_CC_GB_OG) Color Correction GB OG Register Offset */
#define ISC_CC_BR_BG_REG_OFST          (0x8C)              /**< (ISC_CC_BR_BG) Color Correction BR BG Register Offset */
#define ISC_CC_BB_OB_REG_OFST          (0x90)              /**< (ISC_CC_BB_OB) Color Correction BB OB Register Offset */
#define ISC_GAM_CTRL_REG_OFST          (0x94)              /**< (ISC_GAM_CTRL) Gamma Correction Control Register Offset */
#define ISC_GAM_BENTRY_REG_OFST        (0x98)              /**< (ISC_GAM_BENTRY) Gamma Correction Blue Entry Offset */
#define ISC_GAM_GENTRY_REG_OFST        (0x198)             /**< (ISC_GAM_GENTRY) Gamma Correction Green Entry Offset */
#define ISC_GAM_RENTRY_REG_OFST        (0x298)             /**< (ISC_GAM_RENTRY) Gamma Correction Red Entry Offset */
#define ISC_CSC_CTRL_REG_OFST          (0x398)             /**< (ISC_CSC_CTRL) Color Space Conversion Control Register Offset */
#define ISC_CSC_YR_YG_REG_OFST         (0x39C)             /**< (ISC_CSC_YR_YG) Color Space Conversion YR, YG Register Offset */
#define ISC_CSC_YB_OY_REG_OFST         (0x3A0)             /**< (ISC_CSC_YB_OY) Color Space Conversion YB, OY Register Offset */
#define ISC_CSC_CBR_CBG_REG_OFST       (0x3A4)             /**< (ISC_CSC_CBR_CBG) Color Space Conversion CBR CBG Register Offset */
#define ISC_CSC_CBB_OCB_REG_OFST       (0x3A8)             /**< (ISC_CSC_CBB_OCB) Color Space Conversion CBB OCB Register Offset */
#define ISC_CSC_CRR_CRG_REG_OFST       (0x3AC)             /**< (ISC_CSC_CRR_CRG) Color Space Conversion CRR CRG Register Offset */
#define ISC_CSC_CRB_OCR_REG_OFST       (0x3B0)             /**< (ISC_CSC_CRB_OCR) Color Space Conversion CRB OCR Register Offset */
#define ISC_CBC_CTRL_REG_OFST          (0x3B4)             /**< (ISC_CBC_CTRL) Contrast and Brightness Control Register Offset */
#define ISC_CBC_CFG_REG_OFST           (0x3B8)             /**< (ISC_CBC_CFG) Contrast and Brightness Configuration Register Offset */
#define ISC_CBC_BRIGHT_REG_OFST        (0x3BC)             /**< (ISC_CBC_BRIGHT) Contrast and Brightness, Brightness Register Offset */
#define ISC_CBC_CONTRAST_REG_OFST      (0x3C0)             /**< (ISC_CBC_CONTRAST) Contrast and Brightness, Contrast Register Offset */
#define ISC_SUB422_CTRL_REG_OFST       (0x3C4)             /**< (ISC_SUB422_CTRL) Subsampling 4:4:4 to 4:2:2 Control Register Offset */
#define ISC_SUB422_CFG_REG_OFST        (0x3C8)             /**< (ISC_SUB422_CFG) Subsampling 4:4:4 to 4:2:2 Configuration Register Offset */
#define ISC_SUB420_CTRL_REG_OFST       (0x3CC)             /**< (ISC_SUB420_CTRL) Subsampling 4:2:2 to 4:2:0 Control Register Offset */
#define ISC_RLP_CFG_REG_OFST           (0x3D0)             /**< (ISC_RLP_CFG) Rounding, Limiting and Packing Configuration Register Offset */
#define ISC_HIS_CTRL_REG_OFST          (0x3D4)             /**< (ISC_HIS_CTRL) Histogram Control Register Offset */
#define ISC_HIS_CFG_REG_OFST           (0x3D8)             /**< (ISC_HIS_CFG) Histogram Configuration Register Offset */
#define ISC_DCFG_REG_OFST              (0x3E0)             /**< (ISC_DCFG) DMA Configuration Register Offset */
#define ISC_DCTRL_REG_OFST             (0x3E4)             /**< (ISC_DCTRL) DMA Control Register Offset */
#define ISC_DNDA_REG_OFST              (0x3E8)             /**< (ISC_DNDA) DMA Descriptor Address Register Offset */
#define ISC_HIS_ENTRY_REG_OFST         (0x410)             /**< (ISC_HIS_ENTRY) Histogram Entry Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief ISC_SUB0 register API structure */
typedef struct
{
  __IO  uint32_t                       ISC_DAD;            /**< Offset: 0x00 (R/W  32) DMA Address 0 Register */
  __IO  uint32_t                       ISC_DST;            /**< Offset: 0x04 (R/W  32) DMA Stride 0 Register */
} isc_sub0_registers_t;

#define ISC_SUB0_NUMBER _U_(3)

/** \brief ISC register API structure */
typedef struct
{
  __O   uint32_t                       ISC_CTRLEN;         /**< Offset: 0x00 ( /W  32) Control Enable Register */
  __O   uint32_t                       ISC_CTRLDIS;        /**< Offset: 0x04 ( /W  32) Control Disable Register */
  __I   uint32_t                       ISC_CTRLSR;         /**< Offset: 0x08 (R/   32) Control Status Register */
  __IO  uint32_t                       ISC_PFE_CFG0;       /**< Offset: 0x0C (R/W  32) Parallel Front End Configuration 0 Register */
  __IO  uint32_t                       ISC_PFE_CFG1;       /**< Offset: 0x10 (R/W  32) Parallel Front End Configuration 1 Register */
  __IO  uint32_t                       ISC_PFE_CFG2;       /**< Offset: 0x14 (R/W  32) Parallel Front End Configuration 2 Register */
  __O   uint32_t                       ISC_CLKEN;          /**< Offset: 0x18 ( /W  32) Clock Enable Register */
  __O   uint32_t                       ISC_CLKDIS;         /**< Offset: 0x1C ( /W  32) Clock Disable Register */
  __I   uint32_t                       ISC_CLKSR;          /**< Offset: 0x20 (R/   32) Clock Status Register */
  __IO  uint32_t                       ISC_CLKCFG;         /**< Offset: 0x24 (R/W  32) Clock Configuration Register */
  __O   uint32_t                       ISC_INTEN;          /**< Offset: 0x28 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       ISC_INTDIS;         /**< Offset: 0x2C ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       ISC_INTMASK;        /**< Offset: 0x30 (R/   32) Interrupt Mask Register */
  __I   uint32_t                       ISC_INTSR;          /**< Offset: 0x34 (R/   32) Interrupt Status Register */
  __I   uint8_t                        Reserved1[0x20];
  __IO  uint32_t                       ISC_WB_CTRL;        /**< Offset: 0x58 (R/W  32) White Balance Control Register */
  __IO  uint32_t                       ISC_WB_CFG;         /**< Offset: 0x5C (R/W  32) White Balance Configuration Register */
  __IO  uint32_t                       ISC_WB_O_RGR;       /**< Offset: 0x60 (R/W  32) White Balance Offset for R, GR Register */
  __IO  uint32_t                       ISC_WB_O_BGB;       /**< Offset: 0x64 (R/W  32) White Balance Offset for B, GB Register */
  __IO  uint32_t                       ISC_WB_G_RGR;       /**< Offset: 0x68 (R/W  32) White Balance Gain for R, GR Register */
  __IO  uint32_t                       ISC_WB_G_BGB;       /**< Offset: 0x6C (R/W  32) White Balance Gain for B, GB Register */
  __IO  uint32_t                       ISC_CFA_CTRL;       /**< Offset: 0x70 (R/W  32) Color Filter Array Control Register */
  __IO  uint32_t                       ISC_CFA_CFG;        /**< Offset: 0x74 (R/W  32) Color Filter Array Configuration Register */
  __IO  uint32_t                       ISC_CC_CTRL;        /**< Offset: 0x78 (R/W  32) Color Correction Control Register */
  __IO  uint32_t                       ISC_CC_RR_RG;       /**< Offset: 0x7C (R/W  32) Color Correction RR RG Register */
  __IO  uint32_t                       ISC_CC_RB_OR;       /**< Offset: 0x80 (R/W  32) Color Correction RB OR Register */
  __IO  uint32_t                       ISC_CC_GR_GG;       /**< Offset: 0x84 (R/W  32) Color Correction GR GG Register */
  __IO  uint32_t                       ISC_CC_GB_OG;       /**< Offset: 0x88 (R/W  32) Color Correction GB OG Register */
  __IO  uint32_t                       ISC_CC_BR_BG;       /**< Offset: 0x8C (R/W  32) Color Correction BR BG Register */
  __IO  uint32_t                       ISC_CC_BB_OB;       /**< Offset: 0x90 (R/W  32) Color Correction BB OB Register */
  __IO  uint32_t                       ISC_GAM_CTRL;       /**< Offset: 0x94 (R/W  32) Gamma Correction Control Register */
  __IO  uint32_t                       ISC_GAM_BENTRY[64]; /**< Offset: 0x98 (R/W  32) Gamma Correction Blue Entry */
  __IO  uint32_t                       ISC_GAM_GENTRY[64]; /**< Offset: 0x198 (R/W  32) Gamma Correction Green Entry */
  __IO  uint32_t                       ISC_GAM_RENTRY[64]; /**< Offset: 0x298 (R/W  32) Gamma Correction Red Entry */
  __IO  uint32_t                       ISC_CSC_CTRL;       /**< Offset: 0x398 (R/W  32) Color Space Conversion Control Register */
  __IO  uint32_t                       ISC_CSC_YR_YG;      /**< Offset: 0x39C (R/W  32) Color Space Conversion YR, YG Register */
  __IO  uint32_t                       ISC_CSC_YB_OY;      /**< Offset: 0x3A0 (R/W  32) Color Space Conversion YB, OY Register */
  __IO  uint32_t                       ISC_CSC_CBR_CBG;    /**< Offset: 0x3A4 (R/W  32) Color Space Conversion CBR CBG Register */
  __IO  uint32_t                       ISC_CSC_CBB_OCB;    /**< Offset: 0x3A8 (R/W  32) Color Space Conversion CBB OCB Register */
  __IO  uint32_t                       ISC_CSC_CRR_CRG;    /**< Offset: 0x3AC (R/W  32) Color Space Conversion CRR CRG Register */
  __IO  uint32_t                       ISC_CSC_CRB_OCR;    /**< Offset: 0x3B0 (R/W  32) Color Space Conversion CRB OCR Register */
  __IO  uint32_t                       ISC_CBC_CTRL;       /**< Offset: 0x3B4 (R/W  32) Contrast and Brightness Control Register */
  __IO  uint32_t                       ISC_CBC_CFG;        /**< Offset: 0x3B8 (R/W  32) Contrast and Brightness Configuration Register */
  __IO  uint32_t                       ISC_CBC_BRIGHT;     /**< Offset: 0x3BC (R/W  32) Contrast and Brightness, Brightness Register */
  __IO  uint32_t                       ISC_CBC_CONTRAST;   /**< Offset: 0x3C0 (R/W  32) Contrast and Brightness, Contrast Register */
  __IO  uint32_t                       ISC_SUB422_CTRL;    /**< Offset: 0x3C4 (R/W  32) Subsampling 4:4:4 to 4:2:2 Control Register */
  __IO  uint32_t                       ISC_SUB422_CFG;     /**< Offset: 0x3C8 (R/W  32) Subsampling 4:4:4 to 4:2:2 Configuration Register */
  __IO  uint32_t                       ISC_SUB420_CTRL;    /**< Offset: 0x3CC (R/W  32) Subsampling 4:2:2 to 4:2:0 Control Register */
  __IO  uint32_t                       ISC_RLP_CFG;        /**< Offset: 0x3D0 (R/W  32) Rounding, Limiting and Packing Configuration Register */
  __IO  uint32_t                       ISC_HIS_CTRL;       /**< Offset: 0x3D4 (R/W  32) Histogram Control Register */
  __IO  uint32_t                       ISC_HIS_CFG;        /**< Offset: 0x3D8 (R/W  32) Histogram Configuration Register */
  __I   uint8_t                        Reserved2[0x04];
  __IO  uint32_t                       ISC_DCFG;           /**< Offset: 0x3E0 (R/W  32) DMA Configuration Register */
  __IO  uint32_t                       ISC_DCTRL;          /**< Offset: 0x3E4 (R/W  32) DMA Control Register */
  __IO  uint32_t                       ISC_DNDA;           /**< Offset: 0x3E8 (R/W  32) DMA Descriptor Address Register */
        isc_sub0_registers_t           ISC_SUB0[ISC_SUB0_NUMBER]; /**< Offset: 0x3EC  */
  __I   uint8_t                        Reserved3[0x0C];
  __I   uint32_t                       ISC_HIS_ENTRY[512]; /**< Offset: 0x410 (R/   32) Histogram Entry */
} isc_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAMA5D2_ISC_COMPONENT_H_ */
