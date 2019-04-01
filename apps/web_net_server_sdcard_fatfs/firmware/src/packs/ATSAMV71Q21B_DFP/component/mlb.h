/**
 * \brief Component description for MLB
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

/* file generated from device description version 2019-01-18T21:21:15Z */
#ifndef _SAMV71_MLB_COMPONENT_H_
#define _SAMV71_MLB_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR MLB                                          */
/* ************************************************************************** */

/* -------- MLB_MLBC0 : (MLB Offset: 0x00) (R/W 32) MediaLB Control 0 Register -------- */
#define MLB_MLBC0_MLBEN_Pos                   _U_(0)                                               /**< (MLB_MLBC0) MediaLB Enable Position */
#define MLB_MLBC0_MLBEN_Msk                   (_U_(0x1) << MLB_MLBC0_MLBEN_Pos)                    /**< (MLB_MLBC0) MediaLB Enable Mask */
#define MLB_MLBC0_MLBEN(value)                (MLB_MLBC0_MLBEN_Msk & ((value) << MLB_MLBC0_MLBEN_Pos))
#define MLB_MLBC0_MLBCLK_Pos                  _U_(2)                                               /**< (MLB_MLBC0) MLBCLK (MediaLB clock) Speed Select Position */
#define MLB_MLBC0_MLBCLK_Msk                  (_U_(0x7) << MLB_MLBC0_MLBCLK_Pos)                   /**< (MLB_MLBC0) MLBCLK (MediaLB clock) Speed Select Mask */
#define MLB_MLBC0_MLBCLK(value)               (MLB_MLBC0_MLBCLK_Msk & ((value) << MLB_MLBC0_MLBCLK_Pos))
#define   MLB_MLBC0_MLBCLK_256_FS_Val         _U_(0x0)                                             /**< (MLB_MLBC0) 256xFs (for MLBPEN = 0)  */
#define   MLB_MLBC0_MLBCLK_512_FS_Val         _U_(0x1)                                             /**< (MLB_MLBC0) 512xFs (for MLBPEN = 0)  */
#define   MLB_MLBC0_MLBCLK_1024_FS_Val        _U_(0x2)                                             /**< (MLB_MLBC0) 1024xFs (for MLBPEN = 0)  */
#define MLB_MLBC0_MLBCLK_256_FS               (MLB_MLBC0_MLBCLK_256_FS_Val << MLB_MLBC0_MLBCLK_Pos) /**< (MLB_MLBC0) 256xFs (for MLBPEN = 0) Position  */
#define MLB_MLBC0_MLBCLK_512_FS               (MLB_MLBC0_MLBCLK_512_FS_Val << MLB_MLBC0_MLBCLK_Pos) /**< (MLB_MLBC0) 512xFs (for MLBPEN = 0) Position  */
#define MLB_MLBC0_MLBCLK_1024_FS              (MLB_MLBC0_MLBCLK_1024_FS_Val << MLB_MLBC0_MLBCLK_Pos) /**< (MLB_MLBC0) 1024xFs (for MLBPEN = 0) Position  */
#define MLB_MLBC0_ZERO_Pos                    _U_(5)                                               /**< (MLB_MLBC0) Must be Written to 0 Position */
#define MLB_MLBC0_ZERO_Msk                    (_U_(0x1) << MLB_MLBC0_ZERO_Pos)                     /**< (MLB_MLBC0) Must be Written to 0 Mask */
#define MLB_MLBC0_ZERO(value)                 (MLB_MLBC0_ZERO_Msk & ((value) << MLB_MLBC0_ZERO_Pos))
#define MLB_MLBC0_MLBLK_Pos                   _U_(7)                                               /**< (MLB_MLBC0) MediaLB Lock Status (read-only) Position */
#define MLB_MLBC0_MLBLK_Msk                   (_U_(0x1) << MLB_MLBC0_MLBLK_Pos)                    /**< (MLB_MLBC0) MediaLB Lock Status (read-only) Mask */
#define MLB_MLBC0_MLBLK(value)                (MLB_MLBC0_MLBLK_Msk & ((value) << MLB_MLBC0_MLBLK_Pos))
#define MLB_MLBC0_ASYRETRY_Pos                _U_(12)                                              /**< (MLB_MLBC0) Asynchronous Tx Packet Retry Position */
#define MLB_MLBC0_ASYRETRY_Msk                (_U_(0x1) << MLB_MLBC0_ASYRETRY_Pos)                 /**< (MLB_MLBC0) Asynchronous Tx Packet Retry Mask */
#define MLB_MLBC0_ASYRETRY(value)             (MLB_MLBC0_ASYRETRY_Msk & ((value) << MLB_MLBC0_ASYRETRY_Pos))
#define MLB_MLBC0_CTLRETRY_Pos                _U_(14)                                              /**< (MLB_MLBC0) Control Tx Packet Retry Position */
#define MLB_MLBC0_CTLRETRY_Msk                (_U_(0x1) << MLB_MLBC0_CTLRETRY_Pos)                 /**< (MLB_MLBC0) Control Tx Packet Retry Mask */
#define MLB_MLBC0_CTLRETRY(value)             (MLB_MLBC0_CTLRETRY_Msk & ((value) << MLB_MLBC0_CTLRETRY_Pos))
#define MLB_MLBC0_FCNT_Pos                    _U_(15)                                              /**< (MLB_MLBC0) The number of frames per sub-buffer for synchronous channels Position */
#define MLB_MLBC0_FCNT_Msk                    (_U_(0x7) << MLB_MLBC0_FCNT_Pos)                     /**< (MLB_MLBC0) The number of frames per sub-buffer for synchronous channels Mask */
#define MLB_MLBC0_FCNT(value)                 (MLB_MLBC0_FCNT_Msk & ((value) << MLB_MLBC0_FCNT_Pos))
#define   MLB_MLBC0_FCNT_1_FRAME_Val          _U_(0x0)                                             /**< (MLB_MLBC0) 1 frame per sub-buffer (Operation is the same as Standard mode.)  */
#define   MLB_MLBC0_FCNT_2_FRAMES_Val         _U_(0x1)                                             /**< (MLB_MLBC0) 2 frames per sub-buffer  */
#define   MLB_MLBC0_FCNT_4_FRAMES_Val         _U_(0x2)                                             /**< (MLB_MLBC0) 4 frames per sub-buffer  */
#define   MLB_MLBC0_FCNT_8_FRAMES_Val         _U_(0x3)                                             /**< (MLB_MLBC0) 8 frames per sub-buffer  */
#define   MLB_MLBC0_FCNT_16_FRAMES_Val        _U_(0x4)                                             /**< (MLB_MLBC0) 16 frames per sub-buffer  */
#define   MLB_MLBC0_FCNT_32_FRAMES_Val        _U_(0x5)                                             /**< (MLB_MLBC0) 32 frames per sub-buffer  */
#define   MLB_MLBC0_FCNT_64_FRAMES_Val        _U_(0x6)                                             /**< (MLB_MLBC0) 64 frames per sub-buffer  */
#define MLB_MLBC0_FCNT_1_FRAME                (MLB_MLBC0_FCNT_1_FRAME_Val << MLB_MLBC0_FCNT_Pos)   /**< (MLB_MLBC0) 1 frame per sub-buffer (Operation is the same as Standard mode.) Position  */
#define MLB_MLBC0_FCNT_2_FRAMES               (MLB_MLBC0_FCNT_2_FRAMES_Val << MLB_MLBC0_FCNT_Pos)  /**< (MLB_MLBC0) 2 frames per sub-buffer Position  */
#define MLB_MLBC0_FCNT_4_FRAMES               (MLB_MLBC0_FCNT_4_FRAMES_Val << MLB_MLBC0_FCNT_Pos)  /**< (MLB_MLBC0) 4 frames per sub-buffer Position  */
#define MLB_MLBC0_FCNT_8_FRAMES               (MLB_MLBC0_FCNT_8_FRAMES_Val << MLB_MLBC0_FCNT_Pos)  /**< (MLB_MLBC0) 8 frames per sub-buffer Position  */
#define MLB_MLBC0_FCNT_16_FRAMES              (MLB_MLBC0_FCNT_16_FRAMES_Val << MLB_MLBC0_FCNT_Pos) /**< (MLB_MLBC0) 16 frames per sub-buffer Position  */
#define MLB_MLBC0_FCNT_32_FRAMES              (MLB_MLBC0_FCNT_32_FRAMES_Val << MLB_MLBC0_FCNT_Pos) /**< (MLB_MLBC0) 32 frames per sub-buffer Position  */
#define MLB_MLBC0_FCNT_64_FRAMES              (MLB_MLBC0_FCNT_64_FRAMES_Val << MLB_MLBC0_FCNT_Pos) /**< (MLB_MLBC0) 64 frames per sub-buffer Position  */
#define MLB_MLBC0_Msk                         _U_(0x0003D0BD)                                      /**< (MLB_MLBC0) Register Mask  */


/* -------- MLB_MS0 : (MLB Offset: 0x0C) (R/W 32) MediaLB Channel Status 0 Register -------- */
#define MLB_MS0_MCS_Pos                       _U_(0)                                               /**< (MLB_MS0) MediaLB Channel Status [31:0] (cleared by writing a 0) Position */
#define MLB_MS0_MCS_Msk                       (_U_(0xFFFFFFFF) << MLB_MS0_MCS_Pos)                 /**< (MLB_MS0) MediaLB Channel Status [31:0] (cleared by writing a 0) Mask */
#define MLB_MS0_MCS(value)                    (MLB_MS0_MCS_Msk & ((value) << MLB_MS0_MCS_Pos))    
#define MLB_MS0_Msk                           _U_(0xFFFFFFFF)                                      /**< (MLB_MS0) Register Mask  */


/* -------- MLB_MS1 : (MLB Offset: 0x14) (R/W 32) MediaLB Channel Status1 Register -------- */
#define MLB_MS1_MCS_Pos                       _U_(0)                                               /**< (MLB_MS1) MediaLB Channel Status [63:32] (cleared by writing a 0) Position */
#define MLB_MS1_MCS_Msk                       (_U_(0xFFFFFFFF) << MLB_MS1_MCS_Pos)                 /**< (MLB_MS1) MediaLB Channel Status [63:32] (cleared by writing a 0) Mask */
#define MLB_MS1_MCS(value)                    (MLB_MS1_MCS_Msk & ((value) << MLB_MS1_MCS_Pos))    
#define MLB_MS1_Msk                           _U_(0xFFFFFFFF)                                      /**< (MLB_MS1) Register Mask  */


/* -------- MLB_MSS : (MLB Offset: 0x20) (R/W 32) MediaLB System Status Register -------- */
#define MLB_MSS_RSTSYSCMD_Pos                 _U_(0)                                               /**< (MLB_MSS) Reset System Command Detected in the System Quadlet (cleared by writing a 0) Position */
#define MLB_MSS_RSTSYSCMD_Msk                 (_U_(0x1) << MLB_MSS_RSTSYSCMD_Pos)                  /**< (MLB_MSS) Reset System Command Detected in the System Quadlet (cleared by writing a 0) Mask */
#define MLB_MSS_RSTSYSCMD(value)              (MLB_MSS_RSTSYSCMD_Msk & ((value) << MLB_MSS_RSTSYSCMD_Pos))
#define MLB_MSS_LKSYSCMD_Pos                  _U_(1)                                               /**< (MLB_MSS) Network Lock System Command Detected in the System Quadlet (cleared by writing a 0) Position */
#define MLB_MSS_LKSYSCMD_Msk                  (_U_(0x1) << MLB_MSS_LKSYSCMD_Pos)                   /**< (MLB_MSS) Network Lock System Command Detected in the System Quadlet (cleared by writing a 0) Mask */
#define MLB_MSS_LKSYSCMD(value)               (MLB_MSS_LKSYSCMD_Msk & ((value) << MLB_MSS_LKSYSCMD_Pos))
#define MLB_MSS_ULKSYSCMD_Pos                 _U_(2)                                               /**< (MLB_MSS) Network Unlock System Command Detected in the System Quadlet (cleared by writing a 0) Position */
#define MLB_MSS_ULKSYSCMD_Msk                 (_U_(0x1) << MLB_MSS_ULKSYSCMD_Pos)                  /**< (MLB_MSS) Network Unlock System Command Detected in the System Quadlet (cleared by writing a 0) Mask */
#define MLB_MSS_ULKSYSCMD(value)              (MLB_MSS_ULKSYSCMD_Msk & ((value) << MLB_MSS_ULKSYSCMD_Pos))
#define MLB_MSS_CSSYSCMD_Pos                  _U_(3)                                               /**< (MLB_MSS) Channel Scan System Command Detected in the System Quadlet (cleared by writing a 0) Position */
#define MLB_MSS_CSSYSCMD_Msk                  (_U_(0x1) << MLB_MSS_CSSYSCMD_Pos)                   /**< (MLB_MSS) Channel Scan System Command Detected in the System Quadlet (cleared by writing a 0) Mask */
#define MLB_MSS_CSSYSCMD(value)               (MLB_MSS_CSSYSCMD_Msk & ((value) << MLB_MSS_CSSYSCMD_Pos))
#define MLB_MSS_SWSYSCMD_Pos                  _U_(4)                                               /**< (MLB_MSS) Software System Command Detected in the System Quadlet (cleared by writing a 0) Position */
#define MLB_MSS_SWSYSCMD_Msk                  (_U_(0x1) << MLB_MSS_SWSYSCMD_Pos)                   /**< (MLB_MSS) Software System Command Detected in the System Quadlet (cleared by writing a 0) Mask */
#define MLB_MSS_SWSYSCMD(value)               (MLB_MSS_SWSYSCMD_Msk & ((value) << MLB_MSS_SWSYSCMD_Pos))
#define MLB_MSS_SERVREQ_Pos                   _U_(5)                                               /**< (MLB_MSS) Service Request Enabled Position */
#define MLB_MSS_SERVREQ_Msk                   (_U_(0x1) << MLB_MSS_SERVREQ_Pos)                    /**< (MLB_MSS) Service Request Enabled Mask */
#define MLB_MSS_SERVREQ(value)                (MLB_MSS_SERVREQ_Msk & ((value) << MLB_MSS_SERVREQ_Pos))
#define MLB_MSS_Msk                           _U_(0x0000003F)                                      /**< (MLB_MSS) Register Mask  */


/* -------- MLB_MSD : (MLB Offset: 0x24) ( R/ 32) MediaLB System Data Register -------- */
#define MLB_MSD_SD0_Pos                       _U_(0)                                               /**< (MLB_MSD) System Data (Byte 0) Position */
#define MLB_MSD_SD0_Msk                       (_U_(0xFF) << MLB_MSD_SD0_Pos)                       /**< (MLB_MSD) System Data (Byte 0) Mask */
#define MLB_MSD_SD0(value)                    (MLB_MSD_SD0_Msk & ((value) << MLB_MSD_SD0_Pos))    
#define MLB_MSD_SD1_Pos                       _U_(8)                                               /**< (MLB_MSD) System Data (Byte 1) Position */
#define MLB_MSD_SD1_Msk                       (_U_(0xFF) << MLB_MSD_SD1_Pos)                       /**< (MLB_MSD) System Data (Byte 1) Mask */
#define MLB_MSD_SD1(value)                    (MLB_MSD_SD1_Msk & ((value) << MLB_MSD_SD1_Pos))    
#define MLB_MSD_SD2_Pos                       _U_(16)                                              /**< (MLB_MSD) System Data (Byte 2) Position */
#define MLB_MSD_SD2_Msk                       (_U_(0xFF) << MLB_MSD_SD2_Pos)                       /**< (MLB_MSD) System Data (Byte 2) Mask */
#define MLB_MSD_SD2(value)                    (MLB_MSD_SD2_Msk & ((value) << MLB_MSD_SD2_Pos))    
#define MLB_MSD_SD3_Pos                       _U_(24)                                              /**< (MLB_MSD) System Data (Byte 3) Position */
#define MLB_MSD_SD3_Msk                       (_U_(0xFF) << MLB_MSD_SD3_Pos)                       /**< (MLB_MSD) System Data (Byte 3) Mask */
#define MLB_MSD_SD3(value)                    (MLB_MSD_SD3_Msk & ((value) << MLB_MSD_SD3_Pos))    
#define MLB_MSD_Msk                           _U_(0xFFFFFFFF)                                      /**< (MLB_MSD) Register Mask  */


/* -------- MLB_MIEN : (MLB Offset: 0x2C) (R/W 32) MediaLB Interrupt Enable Register -------- */
#define MLB_MIEN_ISOC_PE_Pos                  _U_(0)                                               /**< (MLB_MIEN) Isochronous Rx Protocol Error Enable Position */
#define MLB_MIEN_ISOC_PE_Msk                  (_U_(0x1) << MLB_MIEN_ISOC_PE_Pos)                   /**< (MLB_MIEN) Isochronous Rx Protocol Error Enable Mask */
#define MLB_MIEN_ISOC_PE(value)               (MLB_MIEN_ISOC_PE_Msk & ((value) << MLB_MIEN_ISOC_PE_Pos))
#define MLB_MIEN_ISOC_BUFO_Pos                _U_(1)                                               /**< (MLB_MIEN) Isochronous Rx Buffer Overflow Enable Position */
#define MLB_MIEN_ISOC_BUFO_Msk                (_U_(0x1) << MLB_MIEN_ISOC_BUFO_Pos)                 /**< (MLB_MIEN) Isochronous Rx Buffer Overflow Enable Mask */
#define MLB_MIEN_ISOC_BUFO(value)             (MLB_MIEN_ISOC_BUFO_Msk & ((value) << MLB_MIEN_ISOC_BUFO_Pos))
#define MLB_MIEN_SYNC_PE_Pos                  _U_(16)                                              /**< (MLB_MIEN) Synchronous Protocol Error Enable Position */
#define MLB_MIEN_SYNC_PE_Msk                  (_U_(0x1) << MLB_MIEN_SYNC_PE_Pos)                   /**< (MLB_MIEN) Synchronous Protocol Error Enable Mask */
#define MLB_MIEN_SYNC_PE(value)               (MLB_MIEN_SYNC_PE_Msk & ((value) << MLB_MIEN_SYNC_PE_Pos))
#define MLB_MIEN_ARX_DONE_Pos                 _U_(17)                                              /**< (MLB_MIEN) Asynchronous Rx Done Enable Position */
#define MLB_MIEN_ARX_DONE_Msk                 (_U_(0x1) << MLB_MIEN_ARX_DONE_Pos)                  /**< (MLB_MIEN) Asynchronous Rx Done Enable Mask */
#define MLB_MIEN_ARX_DONE(value)              (MLB_MIEN_ARX_DONE_Msk & ((value) << MLB_MIEN_ARX_DONE_Pos))
#define MLB_MIEN_ARX_PE_Pos                   _U_(18)                                              /**< (MLB_MIEN) Asynchronous Rx Protocol Error Enable Position */
#define MLB_MIEN_ARX_PE_Msk                   (_U_(0x1) << MLB_MIEN_ARX_PE_Pos)                    /**< (MLB_MIEN) Asynchronous Rx Protocol Error Enable Mask */
#define MLB_MIEN_ARX_PE(value)                (MLB_MIEN_ARX_PE_Msk & ((value) << MLB_MIEN_ARX_PE_Pos))
#define MLB_MIEN_ARX_BREAK_Pos                _U_(19)                                              /**< (MLB_MIEN) Asynchronous Rx Break Enable Position */
#define MLB_MIEN_ARX_BREAK_Msk                (_U_(0x1) << MLB_MIEN_ARX_BREAK_Pos)                 /**< (MLB_MIEN) Asynchronous Rx Break Enable Mask */
#define MLB_MIEN_ARX_BREAK(value)             (MLB_MIEN_ARX_BREAK_Msk & ((value) << MLB_MIEN_ARX_BREAK_Pos))
#define MLB_MIEN_ATX_DONE_Pos                 _U_(20)                                              /**< (MLB_MIEN) Asynchronous Tx Packet Done Enable Position */
#define MLB_MIEN_ATX_DONE_Msk                 (_U_(0x1) << MLB_MIEN_ATX_DONE_Pos)                  /**< (MLB_MIEN) Asynchronous Tx Packet Done Enable Mask */
#define MLB_MIEN_ATX_DONE(value)              (MLB_MIEN_ATX_DONE_Msk & ((value) << MLB_MIEN_ATX_DONE_Pos))
#define MLB_MIEN_ATX_PE_Pos                   _U_(21)                                              /**< (MLB_MIEN) Asynchronous Tx Protocol Error Enable Position */
#define MLB_MIEN_ATX_PE_Msk                   (_U_(0x1) << MLB_MIEN_ATX_PE_Pos)                    /**< (MLB_MIEN) Asynchronous Tx Protocol Error Enable Mask */
#define MLB_MIEN_ATX_PE(value)                (MLB_MIEN_ATX_PE_Msk & ((value) << MLB_MIEN_ATX_PE_Pos))
#define MLB_MIEN_ATX_BREAK_Pos                _U_(22)                                              /**< (MLB_MIEN) Asynchronous Tx Break Enable Position */
#define MLB_MIEN_ATX_BREAK_Msk                (_U_(0x1) << MLB_MIEN_ATX_BREAK_Pos)                 /**< (MLB_MIEN) Asynchronous Tx Break Enable Mask */
#define MLB_MIEN_ATX_BREAK(value)             (MLB_MIEN_ATX_BREAK_Msk & ((value) << MLB_MIEN_ATX_BREAK_Pos))
#define MLB_MIEN_CRX_DONE_Pos                 _U_(24)                                              /**< (MLB_MIEN) Control Rx Packet Done Enable Position */
#define MLB_MIEN_CRX_DONE_Msk                 (_U_(0x1) << MLB_MIEN_CRX_DONE_Pos)                  /**< (MLB_MIEN) Control Rx Packet Done Enable Mask */
#define MLB_MIEN_CRX_DONE(value)              (MLB_MIEN_CRX_DONE_Msk & ((value) << MLB_MIEN_CRX_DONE_Pos))
#define MLB_MIEN_CRX_PE_Pos                   _U_(25)                                              /**< (MLB_MIEN) Control Rx Protocol Error Enable Position */
#define MLB_MIEN_CRX_PE_Msk                   (_U_(0x1) << MLB_MIEN_CRX_PE_Pos)                    /**< (MLB_MIEN) Control Rx Protocol Error Enable Mask */
#define MLB_MIEN_CRX_PE(value)                (MLB_MIEN_CRX_PE_Msk & ((value) << MLB_MIEN_CRX_PE_Pos))
#define MLB_MIEN_CRX_BREAK_Pos                _U_(26)                                              /**< (MLB_MIEN) Control Rx Break Enable Position */
#define MLB_MIEN_CRX_BREAK_Msk                (_U_(0x1) << MLB_MIEN_CRX_BREAK_Pos)                 /**< (MLB_MIEN) Control Rx Break Enable Mask */
#define MLB_MIEN_CRX_BREAK(value)             (MLB_MIEN_CRX_BREAK_Msk & ((value) << MLB_MIEN_CRX_BREAK_Pos))
#define MLB_MIEN_CTX_DONE_Pos                 _U_(27)                                              /**< (MLB_MIEN) Control Tx Packet Done Enable Position */
#define MLB_MIEN_CTX_DONE_Msk                 (_U_(0x1) << MLB_MIEN_CTX_DONE_Pos)                  /**< (MLB_MIEN) Control Tx Packet Done Enable Mask */
#define MLB_MIEN_CTX_DONE(value)              (MLB_MIEN_CTX_DONE_Msk & ((value) << MLB_MIEN_CTX_DONE_Pos))
#define MLB_MIEN_CTX_PE_Pos                   _U_(28)                                              /**< (MLB_MIEN) Control Tx Protocol Error Enable Position */
#define MLB_MIEN_CTX_PE_Msk                   (_U_(0x1) << MLB_MIEN_CTX_PE_Pos)                    /**< (MLB_MIEN) Control Tx Protocol Error Enable Mask */
#define MLB_MIEN_CTX_PE(value)                (MLB_MIEN_CTX_PE_Msk & ((value) << MLB_MIEN_CTX_PE_Pos))
#define MLB_MIEN_CTX_BREAK_Pos                _U_(29)                                              /**< (MLB_MIEN) Control Tx Break Enable Position */
#define MLB_MIEN_CTX_BREAK_Msk                (_U_(0x1) << MLB_MIEN_CTX_BREAK_Pos)                 /**< (MLB_MIEN) Control Tx Break Enable Mask */
#define MLB_MIEN_CTX_BREAK(value)             (MLB_MIEN_CTX_BREAK_Msk & ((value) << MLB_MIEN_CTX_BREAK_Pos))
#define MLB_MIEN_Msk                          _U_(0x3F7F0003)                                      /**< (MLB_MIEN) Register Mask  */


/* -------- MLB_MLBC1 : (MLB Offset: 0x3C) (R/W 32) MediaLB Control 1 Register -------- */
#define MLB_MLBC1_LOCK_Pos                    _U_(6)                                               /**< (MLB_MLBC1) MediaLB Lock Error Status (cleared by writing a 0) Position */
#define MLB_MLBC1_LOCK_Msk                    (_U_(0x1) << MLB_MLBC1_LOCK_Pos)                     /**< (MLB_MLBC1) MediaLB Lock Error Status (cleared by writing a 0) Mask */
#define MLB_MLBC1_LOCK(value)                 (MLB_MLBC1_LOCK_Msk & ((value) << MLB_MLBC1_LOCK_Pos))
#define MLB_MLBC1_CLKM_Pos                    _U_(7)                                               /**< (MLB_MLBC1) MediaLB Clock Missing Status (cleared by writing a 0) Position */
#define MLB_MLBC1_CLKM_Msk                    (_U_(0x1) << MLB_MLBC1_CLKM_Pos)                     /**< (MLB_MLBC1) MediaLB Clock Missing Status (cleared by writing a 0) Mask */
#define MLB_MLBC1_CLKM(value)                 (MLB_MLBC1_CLKM_Msk & ((value) << MLB_MLBC1_CLKM_Pos))
#define MLB_MLBC1_NDA_Pos                     _U_(8)                                               /**< (MLB_MLBC1) Node Device Address Position */
#define MLB_MLBC1_NDA_Msk                     (_U_(0xFF) << MLB_MLBC1_NDA_Pos)                     /**< (MLB_MLBC1) Node Device Address Mask */
#define MLB_MLBC1_NDA(value)                  (MLB_MLBC1_NDA_Msk & ((value) << MLB_MLBC1_NDA_Pos))
#define MLB_MLBC1_Msk                         _U_(0x0000FFC0)                                      /**< (MLB_MLBC1) Register Mask  */


/* -------- MLB_HCTL : (MLB Offset: 0x80) (R/W 32) HBI Control Register -------- */
#define MLB_HCTL_RST0_Pos                     _U_(0)                                               /**< (MLB_HCTL) Address Generation Unit 0 Software Reset Position */
#define MLB_HCTL_RST0_Msk                     (_U_(0x1) << MLB_HCTL_RST0_Pos)                      /**< (MLB_HCTL) Address Generation Unit 0 Software Reset Mask */
#define MLB_HCTL_RST0(value)                  (MLB_HCTL_RST0_Msk & ((value) << MLB_HCTL_RST0_Pos))
#define MLB_HCTL_RST1_Pos                     _U_(1)                                               /**< (MLB_HCTL) Address Generation Unit 1 Software Reset Position */
#define MLB_HCTL_RST1_Msk                     (_U_(0x1) << MLB_HCTL_RST1_Pos)                      /**< (MLB_HCTL) Address Generation Unit 1 Software Reset Mask */
#define MLB_HCTL_RST1(value)                  (MLB_HCTL_RST1_Msk & ((value) << MLB_HCTL_RST1_Pos))
#define MLB_HCTL_EN_Pos                       _U_(15)                                              /**< (MLB_HCTL) HBI Enable Position */
#define MLB_HCTL_EN_Msk                       (_U_(0x1) << MLB_HCTL_EN_Pos)                        /**< (MLB_HCTL) HBI Enable Mask */
#define MLB_HCTL_EN(value)                    (MLB_HCTL_EN_Msk & ((value) << MLB_HCTL_EN_Pos))    
#define MLB_HCTL_Msk                          _U_(0x00008003)                                      /**< (MLB_HCTL) Register Mask  */

#define MLB_HCTL_RST_Pos                      _U_(0)                                               /**< (MLB_HCTL Position) Address Generation Unit x Software Reset */
#define MLB_HCTL_RST_Msk                      (_U_(0x3) << MLB_HCTL_RST_Pos)                       /**< (MLB_HCTL Mask) RST */
#define MLB_HCTL_RST(value)                   (MLB_HCTL_RST_Msk & ((value) << MLB_HCTL_RST_Pos))   

/* -------- MLB_HCMR : (MLB Offset: 0x88) (R/W 32) HBI Channel Mask 0 Register 0 -------- */
#define MLB_HCMR_CHM_Pos                      _U_(0)                                               /**< (MLB_HCMR) Bitwise Channel Mask Bit [31:0] Position */
#define MLB_HCMR_CHM_Msk                      (_U_(0xFFFFFFFF) << MLB_HCMR_CHM_Pos)                /**< (MLB_HCMR) Bitwise Channel Mask Bit [31:0] Mask */
#define MLB_HCMR_CHM(value)                   (MLB_HCMR_CHM_Msk & ((value) << MLB_HCMR_CHM_Pos))  
#define MLB_HCMR_Msk                          _U_(0xFFFFFFFF)                                      /**< (MLB_HCMR) Register Mask  */


/* -------- MLB_HCER : (MLB Offset: 0x90) ( R/ 32) HBI Channel Error 0 Register 0 -------- */
#define MLB_HCER_CERR_Pos                     _U_(0)                                               /**< (MLB_HCER) Bitwise Channel Error Bit [31:0] Position */
#define MLB_HCER_CERR_Msk                     (_U_(0xFFFFFFFF) << MLB_HCER_CERR_Pos)               /**< (MLB_HCER) Bitwise Channel Error Bit [31:0] Mask */
#define MLB_HCER_CERR(value)                  (MLB_HCER_CERR_Msk & ((value) << MLB_HCER_CERR_Pos))
#define MLB_HCER_Msk                          _U_(0xFFFFFFFF)                                      /**< (MLB_HCER) Register Mask  */


/* -------- MLB_HCBR : (MLB Offset: 0x98) ( R/ 32) HBI Channel Busy 0 Register 0 -------- */
#define MLB_HCBR_CHB_Pos                      _U_(0)                                               /**< (MLB_HCBR) Bitwise Channel Busy Bit [31:0] Position */
#define MLB_HCBR_CHB_Msk                      (_U_(0xFFFFFFFF) << MLB_HCBR_CHB_Pos)                /**< (MLB_HCBR) Bitwise Channel Busy Bit [31:0] Mask */
#define MLB_HCBR_CHB(value)                   (MLB_HCBR_CHB_Msk & ((value) << MLB_HCBR_CHB_Pos))  
#define MLB_HCBR_Msk                          _U_(0xFFFFFFFF)                                      /**< (MLB_HCBR) Register Mask  */


/* -------- MLB_MDAT : (MLB Offset: 0xC0) (R/W 32) MIF Data 0 Register 0 -------- */
#define MLB_MDAT_DATA_Pos                     _U_(0)                                               /**< (MLB_MDAT) CRT or DBR Data Position */
#define MLB_MDAT_DATA_Msk                     (_U_(0xFFFFFFFF) << MLB_MDAT_DATA_Pos)               /**< (MLB_MDAT) CRT or DBR Data Mask */
#define MLB_MDAT_DATA(value)                  (MLB_MDAT_DATA_Msk & ((value) << MLB_MDAT_DATA_Pos))
#define MLB_MDAT_Msk                          _U_(0xFFFFFFFF)                                      /**< (MLB_MDAT) Register Mask  */


/* -------- MLB_MDWE : (MLB Offset: 0xD0) (R/W 32) MIF Data Write Enable 0 Register 0 -------- */
#define MLB_MDWE_MASK_Pos                     _U_(0)                                               /**< (MLB_MDWE) Bitwise Write Enable for CTR Data - bits[31:0] Position */
#define MLB_MDWE_MASK_Msk                     (_U_(0xFFFFFFFF) << MLB_MDWE_MASK_Pos)               /**< (MLB_MDWE) Bitwise Write Enable for CTR Data - bits[31:0] Mask */
#define MLB_MDWE_MASK(value)                  (MLB_MDWE_MASK_Msk & ((value) << MLB_MDWE_MASK_Pos))
#define MLB_MDWE_Msk                          _U_(0xFFFFFFFF)                                      /**< (MLB_MDWE) Register Mask  */


/* -------- MLB_MCTL : (MLB Offset: 0xE0) (R/W 32) MIF Control Register -------- */
#define MLB_MCTL_XCMP_Pos                     _U_(0)                                               /**< (MLB_MCTL) Transfer Complete (Write 0 to Clear) Position */
#define MLB_MCTL_XCMP_Msk                     (_U_(0x1) << MLB_MCTL_XCMP_Pos)                      /**< (MLB_MCTL) Transfer Complete (Write 0 to Clear) Mask */
#define MLB_MCTL_XCMP(value)                  (MLB_MCTL_XCMP_Msk & ((value) << MLB_MCTL_XCMP_Pos))
#define MLB_MCTL_Msk                          _U_(0x00000001)                                      /**< (MLB_MCTL) Register Mask  */


/* -------- MLB_MADR : (MLB Offset: 0xE4) (R/W 32) MIF Address Register -------- */
#define MLB_MADR_ADDR_Pos                     _U_(0)                                               /**< (MLB_MADR) CTR or DBR Address Position */
#define MLB_MADR_ADDR_Msk                     (_U_(0x3FFF) << MLB_MADR_ADDR_Pos)                   /**< (MLB_MADR) CTR or DBR Address Mask */
#define MLB_MADR_ADDR(value)                  (MLB_MADR_ADDR_Msk & ((value) << MLB_MADR_ADDR_Pos))
#define MLB_MADR_TB_Pos                       _U_(30)                                              /**< (MLB_MADR) Target Location Bit Position */
#define MLB_MADR_TB_Msk                       (_U_(0x1) << MLB_MADR_TB_Pos)                        /**< (MLB_MADR) Target Location Bit Mask */
#define MLB_MADR_TB(value)                    (MLB_MADR_TB_Msk & ((value) << MLB_MADR_TB_Pos))    
#define   MLB_MADR_TB_CTR_Val                 _U_(0x0)                                             /**< (MLB_MADR) Selects CTR  */
#define   MLB_MADR_TB_DBR_Val                 _U_(0x1)                                             /**< (MLB_MADR) Selects DBR  */
#define MLB_MADR_TB_CTR                       (MLB_MADR_TB_CTR_Val << MLB_MADR_TB_Pos)             /**< (MLB_MADR) Selects CTR Position  */
#define MLB_MADR_TB_DBR                       (MLB_MADR_TB_DBR_Val << MLB_MADR_TB_Pos)             /**< (MLB_MADR) Selects DBR Position  */
#define MLB_MADR_WNR_Pos                      _U_(31)                                              /**< (MLB_MADR) Write-Not-Read Selection Position */
#define MLB_MADR_WNR_Msk                      (_U_(0x1) << MLB_MADR_WNR_Pos)                       /**< (MLB_MADR) Write-Not-Read Selection Mask */
#define MLB_MADR_WNR(value)                   (MLB_MADR_WNR_Msk & ((value) << MLB_MADR_WNR_Pos))  
#define MLB_MADR_Msk                          _U_(0xC0003FFF)                                      /**< (MLB_MADR) Register Mask  */


/* -------- MLB_ACTL : (MLB Offset: 0x3C0) (R/W 32) AHB Control Register -------- */
#define MLB_ACTL_SCE_Pos                      _U_(0)                                               /**< (MLB_ACTL) Software Clear Enable Position */
#define MLB_ACTL_SCE_Msk                      (_U_(0x1) << MLB_ACTL_SCE_Pos)                       /**< (MLB_ACTL) Software Clear Enable Mask */
#define MLB_ACTL_SCE(value)                   (MLB_ACTL_SCE_Msk & ((value) << MLB_ACTL_SCE_Pos))  
#define MLB_ACTL_SMX_Pos                      _U_(1)                                               /**< (MLB_ACTL) AHB Interrupt Mux Enable Position */
#define MLB_ACTL_SMX_Msk                      (_U_(0x1) << MLB_ACTL_SMX_Pos)                       /**< (MLB_ACTL) AHB Interrupt Mux Enable Mask */
#define MLB_ACTL_SMX(value)                   (MLB_ACTL_SMX_Msk & ((value) << MLB_ACTL_SMX_Pos))  
#define MLB_ACTL_DMA_MODE_Pos                 _U_(2)                                               /**< (MLB_ACTL) DMA Mode Position */
#define MLB_ACTL_DMA_MODE_Msk                 (_U_(0x1) << MLB_ACTL_DMA_MODE_Pos)                  /**< (MLB_ACTL) DMA Mode Mask */
#define MLB_ACTL_DMA_MODE(value)              (MLB_ACTL_DMA_MODE_Msk & ((value) << MLB_ACTL_DMA_MODE_Pos))
#define MLB_ACTL_MPB_Pos                      _U_(4)                                               /**< (MLB_ACTL) DMA Packet Buffering Mode Position */
#define MLB_ACTL_MPB_Msk                      (_U_(0x1) << MLB_ACTL_MPB_Pos)                       /**< (MLB_ACTL) DMA Packet Buffering Mode Mask */
#define MLB_ACTL_MPB(value)                   (MLB_ACTL_MPB_Msk & ((value) << MLB_ACTL_MPB_Pos))  
#define   MLB_ACTL_MPB_SINGLE_PACKET_Val      _U_(0x0)                                             /**< (MLB_ACTL) Single-packet mode  */
#define   MLB_ACTL_MPB_MULTIPLE_PACKET_Val    _U_(0x1)                                             /**< (MLB_ACTL) Multiple-packet mode  */
#define MLB_ACTL_MPB_SINGLE_PACKET            (MLB_ACTL_MPB_SINGLE_PACKET_Val << MLB_ACTL_MPB_Pos) /**< (MLB_ACTL) Single-packet mode Position  */
#define MLB_ACTL_MPB_MULTIPLE_PACKET          (MLB_ACTL_MPB_MULTIPLE_PACKET_Val << MLB_ACTL_MPB_Pos) /**< (MLB_ACTL) Multiple-packet mode Position  */
#define MLB_ACTL_Msk                          _U_(0x00000017)                                      /**< (MLB_ACTL) Register Mask  */


/* -------- MLB_ACSR : (MLB Offset: 0x3D0) (R/W 32) AHB Channel Status 0 Register 0 -------- */
#define MLB_ACSR_CHS_Pos                      _U_(0)                                               /**< (MLB_ACSR) Interrupt Status for Logical Channels [31:0] (cleared by writing a 1) Position */
#define MLB_ACSR_CHS_Msk                      (_U_(0xFFFFFFFF) << MLB_ACSR_CHS_Pos)                /**< (MLB_ACSR) Interrupt Status for Logical Channels [31:0] (cleared by writing a 1) Mask */
#define MLB_ACSR_CHS(value)                   (MLB_ACSR_CHS_Msk & ((value) << MLB_ACSR_CHS_Pos))  
#define MLB_ACSR_Msk                          _U_(0xFFFFFFFF)                                      /**< (MLB_ACSR) Register Mask  */


/* -------- MLB_ACMR : (MLB Offset: 0x3D8) (R/W 32) AHB Channel Mask 0 Register 0 -------- */
#define MLB_ACMR_CHM_Pos                      _U_(0)                                               /**< (MLB_ACMR) Bitwise Channel Mask Bits 31 to 0 Position */
#define MLB_ACMR_CHM_Msk                      (_U_(0xFFFFFFFF) << MLB_ACMR_CHM_Pos)                /**< (MLB_ACMR) Bitwise Channel Mask Bits 31 to 0 Mask */
#define MLB_ACMR_CHM(value)                   (MLB_ACMR_CHM_Msk & ((value) << MLB_ACMR_CHM_Pos))  
#define MLB_ACMR_Msk                          _U_(0xFFFFFFFF)                                      /**< (MLB_ACMR) Register Mask  */


/** \brief MLB register offsets definitions */
#define MLB_MLBC0_REG_OFST             (0x00)              /**< (MLB_MLBC0) MediaLB Control 0 Register Offset */
#define MLB_MS0_REG_OFST               (0x0C)              /**< (MLB_MS0) MediaLB Channel Status 0 Register Offset */
#define MLB_MS1_REG_OFST               (0x14)              /**< (MLB_MS1) MediaLB Channel Status1 Register Offset */
#define MLB_MSS_REG_OFST               (0x20)              /**< (MLB_MSS) MediaLB System Status Register Offset */
#define MLB_MSD_REG_OFST               (0x24)              /**< (MLB_MSD) MediaLB System Data Register Offset */
#define MLB_MIEN_REG_OFST              (0x2C)              /**< (MLB_MIEN) MediaLB Interrupt Enable Register Offset */
#define MLB_MLBC1_REG_OFST             (0x3C)              /**< (MLB_MLBC1) MediaLB Control 1 Register Offset */
#define MLB_HCTL_REG_OFST              (0x80)              /**< (MLB_HCTL) HBI Control Register Offset */
#define MLB_HCMR_REG_OFST              (0x88)              /**< (MLB_HCMR) HBI Channel Mask 0 Register 0 Offset */
#define MLB_HCER_REG_OFST              (0x90)              /**< (MLB_HCER) HBI Channel Error 0 Register 0 Offset */
#define MLB_HCBR_REG_OFST              (0x98)              /**< (MLB_HCBR) HBI Channel Busy 0 Register 0 Offset */
#define MLB_MDAT_REG_OFST              (0xC0)              /**< (MLB_MDAT) MIF Data 0 Register 0 Offset */
#define MLB_MDWE_REG_OFST              (0xD0)              /**< (MLB_MDWE) MIF Data Write Enable 0 Register 0 Offset */
#define MLB_MCTL_REG_OFST              (0xE0)              /**< (MLB_MCTL) MIF Control Register Offset */
#define MLB_MADR_REG_OFST              (0xE4)              /**< (MLB_MADR) MIF Address Register Offset */
#define MLB_ACTL_REG_OFST              (0x3C0)             /**< (MLB_ACTL) AHB Control Register Offset */
#define MLB_ACSR_REG_OFST              (0x3D0)             /**< (MLB_ACSR) AHB Channel Status 0 Register 0 Offset */
#define MLB_ACMR_REG_OFST              (0x3D8)             /**< (MLB_ACMR) AHB Channel Mask 0 Register 0 Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief MLB register API structure */
typedef struct
{
  __IO  uint32_t                       MLB_MLBC0;          /**< Offset: 0x00 (R/W  32) MediaLB Control 0 Register */
  __I   uint8_t                        Reserved1[0x08];
  __IO  uint32_t                       MLB_MS0;            /**< Offset: 0x0C (R/W  32) MediaLB Channel Status 0 Register */
  __I   uint8_t                        Reserved2[0x04];
  __IO  uint32_t                       MLB_MS1;            /**< Offset: 0x14 (R/W  32) MediaLB Channel Status1 Register */
  __I   uint8_t                        Reserved3[0x08];
  __IO  uint32_t                       MLB_MSS;            /**< Offset: 0x20 (R/W  32) MediaLB System Status Register */
  __I   uint32_t                       MLB_MSD;            /**< Offset: 0x24 (R/   32) MediaLB System Data Register */
  __I   uint8_t                        Reserved4[0x04];
  __IO  uint32_t                       MLB_MIEN;           /**< Offset: 0x2C (R/W  32) MediaLB Interrupt Enable Register */
  __I   uint8_t                        Reserved5[0x0C];
  __IO  uint32_t                       MLB_MLBC1;          /**< Offset: 0x3C (R/W  32) MediaLB Control 1 Register */
  __I   uint8_t                        Reserved6[0x40];
  __IO  uint32_t                       MLB_HCTL;           /**< Offset: 0x80 (R/W  32) HBI Control Register */
  __I   uint8_t                        Reserved7[0x04];
  __IO  uint32_t                       MLB_HCMR[2];        /**< Offset: 0x88 (R/W  32) HBI Channel Mask 0 Register 0 */
  __I   uint32_t                       MLB_HCER[2];        /**< Offset: 0x90 (R/   32) HBI Channel Error 0 Register 0 */
  __I   uint32_t                       MLB_HCBR[2];        /**< Offset: 0x98 (R/   32) HBI Channel Busy 0 Register 0 */
  __I   uint8_t                        Reserved8[0x20];
  __IO  uint32_t                       MLB_MDAT[4];        /**< Offset: 0xC0 (R/W  32) MIF Data 0 Register 0 */
  __IO  uint32_t                       MLB_MDWE[4];        /**< Offset: 0xD0 (R/W  32) MIF Data Write Enable 0 Register 0 */
  __IO  uint32_t                       MLB_MCTL;           /**< Offset: 0xE0 (R/W  32) MIF Control Register */
  __IO  uint32_t                       MLB_MADR;           /**< Offset: 0xE4 (R/W  32) MIF Address Register */
  __I   uint8_t                        Reserved9[0x2D8];
  __IO  uint32_t                       MLB_ACTL;           /**< Offset: 0x3C0 (R/W  32) AHB Control Register */
  __I   uint8_t                        Reserved10[0x0C];
  __IO  uint32_t                       MLB_ACSR[2];        /**< Offset: 0x3D0 (R/W  32) AHB Channel Status 0 Register 0 */
  __IO  uint32_t                       MLB_ACMR[2];        /**< Offset: 0x3D8 (R/W  32) AHB Channel Mask 0 Register 0 */
} mlb_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAMV71_MLB_COMPONENT_H_ */
