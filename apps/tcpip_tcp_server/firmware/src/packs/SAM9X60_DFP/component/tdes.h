/**
 * \brief Component description for TDES
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

/* file generated from device description version 2019-08-22T13:04:26Z */
#ifndef _SAM9X_TDES_COMPONENT_H_
#define _SAM9X_TDES_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR TDES                                         */
/* ************************************************************************** */

/* -------- TDES_CR : (TDES Offset: 0x00) ( /W 32) Control Register -------- */
#define TDES_CR_START_Pos                     _U_(0)                                               /**< (TDES_CR) Start Processing Position */
#define TDES_CR_START_Msk                     (_U_(0x1) << TDES_CR_START_Pos)                      /**< (TDES_CR) Start Processing Mask */
#define TDES_CR_START(value)                  (TDES_CR_START_Msk & ((value) << TDES_CR_START_Pos))
#define TDES_CR_SWRST_Pos                     _U_(8)                                               /**< (TDES_CR) Software Reset Position */
#define TDES_CR_SWRST_Msk                     (_U_(0x1) << TDES_CR_SWRST_Pos)                      /**< (TDES_CR) Software Reset Mask */
#define TDES_CR_SWRST(value)                  (TDES_CR_SWRST_Msk & ((value) << TDES_CR_SWRST_Pos))
#define TDES_CR_LOADSEED_Pos                  _U_(16)                                              /**< (TDES_CR) Loadseed Position */
#define TDES_CR_LOADSEED_Msk                  (_U_(0x1) << TDES_CR_LOADSEED_Pos)                   /**< (TDES_CR) Loadseed Mask */
#define TDES_CR_LOADSEED(value)               (TDES_CR_LOADSEED_Msk & ((value) << TDES_CR_LOADSEED_Pos))
#define TDES_CR_UNLOCK_Pos                    _U_(24)                                              /**< (TDES_CR) Unlock Processing Position */
#define TDES_CR_UNLOCK_Msk                    (_U_(0x1) << TDES_CR_UNLOCK_Pos)                     /**< (TDES_CR) Unlock Processing Mask */
#define TDES_CR_UNLOCK(value)                 (TDES_CR_UNLOCK_Msk & ((value) << TDES_CR_UNLOCK_Pos))
#define TDES_CR_Msk                           _U_(0x01010101)                                      /**< (TDES_CR) Register Mask  */


/* -------- TDES_MR : (TDES Offset: 0x04) (R/W 32) Mode Register -------- */
#define TDES_MR_CIPHER_Pos                    _U_(0)                                               /**< (TDES_MR) Processing Mode Position */
#define TDES_MR_CIPHER_Msk                    (_U_(0x1) << TDES_MR_CIPHER_Pos)                     /**< (TDES_MR) Processing Mode Mask */
#define TDES_MR_CIPHER(value)                 (TDES_MR_CIPHER_Msk & ((value) << TDES_MR_CIPHER_Pos))
#define   TDES_MR_CIPHER_DECRYPT_Val          _U_(0x0)                                             /**< (TDES_MR) Decrypts data.  */
#define   TDES_MR_CIPHER_ENCRYPT_Val          _U_(0x1)                                             /**< (TDES_MR) Encrypts data.  */
#define TDES_MR_CIPHER_DECRYPT                (TDES_MR_CIPHER_DECRYPT_Val << TDES_MR_CIPHER_Pos)   /**< (TDES_MR) Decrypts data. Position  */
#define TDES_MR_CIPHER_ENCRYPT                (TDES_MR_CIPHER_ENCRYPT_Val << TDES_MR_CIPHER_Pos)   /**< (TDES_MR) Encrypts data. Position  */
#define TDES_MR_TDESMOD_Pos                   _U_(1)                                               /**< (TDES_MR) ALGORITHM Mode Position */
#define TDES_MR_TDESMOD_Msk                   (_U_(0x3) << TDES_MR_TDESMOD_Pos)                    /**< (TDES_MR) ALGORITHM Mode Mask */
#define TDES_MR_TDESMOD(value)                (TDES_MR_TDESMOD_Msk & ((value) << TDES_MR_TDESMOD_Pos))
#define   TDES_MR_TDESMOD_SINGLE_DES_Val      _U_(0x0)                                             /**< (TDES_MR) Single DES processing using TDES_KEY1WRy.  */
#define   TDES_MR_TDESMOD_TRIPLE_DES_Val      _U_(0x1)                                             /**< (TDES_MR) Triple DES processing using TDES_KEY1WRy, TDES_KEY2WRy and TDES_KEY3WRy.  */
#define   TDES_MR_TDESMOD_XTEA_Val            _U_(0x2)                                             /**< (TDES_MR) XTEA processing using TDES_KEY1WRy and TDES_KEY2WRy.  */
#define TDES_MR_TDESMOD_SINGLE_DES            (TDES_MR_TDESMOD_SINGLE_DES_Val << TDES_MR_TDESMOD_Pos) /**< (TDES_MR) Single DES processing using TDES_KEY1WRy. Position  */
#define TDES_MR_TDESMOD_TRIPLE_DES            (TDES_MR_TDESMOD_TRIPLE_DES_Val << TDES_MR_TDESMOD_Pos) /**< (TDES_MR) Triple DES processing using TDES_KEY1WRy, TDES_KEY2WRy and TDES_KEY3WRy. Position  */
#define TDES_MR_TDESMOD_XTEA                  (TDES_MR_TDESMOD_XTEA_Val << TDES_MR_TDESMOD_Pos)    /**< (TDES_MR) XTEA processing using TDES_KEY1WRy and TDES_KEY2WRy. Position  */
#define TDES_MR_KEYMOD_Pos                    _U_(4)                                               /**< (TDES_MR) Key Mode Position */
#define TDES_MR_KEYMOD_Msk                    (_U_(0x1) << TDES_MR_KEYMOD_Pos)                     /**< (TDES_MR) Key Mode Mask */
#define TDES_MR_KEYMOD(value)                 (TDES_MR_KEYMOD_Msk & ((value) << TDES_MR_KEYMOD_Pos))
#define TDES_MR_PKWO_Pos                      _U_(6)                                               /**< (TDES_MR) Private Key Write Once Position */
#define TDES_MR_PKWO_Msk                      (_U_(0x1) << TDES_MR_PKWO_Pos)                       /**< (TDES_MR) Private Key Write Once Mask */
#define TDES_MR_PKWO(value)                   (TDES_MR_PKWO_Msk & ((value) << TDES_MR_PKWO_Pos))  
#define TDES_MR_PKRS_Pos                      _U_(7)                                               /**< (TDES_MR) Private Key Internal Register Select Position */
#define TDES_MR_PKRS_Msk                      (_U_(0x1) << TDES_MR_PKRS_Pos)                       /**< (TDES_MR) Private Key Internal Register Select Mask */
#define TDES_MR_PKRS(value)                   (TDES_MR_PKRS_Msk & ((value) << TDES_MR_PKRS_Pos))  
#define TDES_MR_SMOD_Pos                      _U_(8)                                               /**< (TDES_MR) Start Mode Position */
#define TDES_MR_SMOD_Msk                      (_U_(0x3) << TDES_MR_SMOD_Pos)                       /**< (TDES_MR) Start Mode Mask */
#define TDES_MR_SMOD(value)                   (TDES_MR_SMOD_Msk & ((value) << TDES_MR_SMOD_Pos))  
#define   TDES_MR_SMOD_MANUAL_START_Val       _U_(0x0)                                             /**< (TDES_MR) Manual Mode  */
#define   TDES_MR_SMOD_AUTO_START_Val         _U_(0x1)                                             /**< (TDES_MR) Auto Mode  */
#define   TDES_MR_SMOD_IDATAR0_START_Val      _U_(0x2)                                             /**< (TDES_MR) TDES_IDATAR0 accesses only Auto mode  */
#define TDES_MR_SMOD_MANUAL_START             (TDES_MR_SMOD_MANUAL_START_Val << TDES_MR_SMOD_Pos)  /**< (TDES_MR) Manual Mode Position  */
#define TDES_MR_SMOD_AUTO_START               (TDES_MR_SMOD_AUTO_START_Val << TDES_MR_SMOD_Pos)    /**< (TDES_MR) Auto Mode Position  */
#define TDES_MR_SMOD_IDATAR0_START            (TDES_MR_SMOD_IDATAR0_START_Val << TDES_MR_SMOD_Pos) /**< (TDES_MR) TDES_IDATAR0 accesses only Auto mode Position  */
#define TDES_MR_OPMOD_Pos                     _U_(12)                                              /**< (TDES_MR) Operating Mode Position */
#define TDES_MR_OPMOD_Msk                     (_U_(0x3) << TDES_MR_OPMOD_Pos)                      /**< (TDES_MR) Operating Mode Mask */
#define TDES_MR_OPMOD(value)                  (TDES_MR_OPMOD_Msk & ((value) << TDES_MR_OPMOD_Pos))
#define   TDES_MR_OPMOD_ECB_Val               _U_(0x0)                                             /**< (TDES_MR) Electronic Code Book mode  */
#define   TDES_MR_OPMOD_CBC_Val               _U_(0x1)                                             /**< (TDES_MR) Cipher Block Chaining mode  */
#define   TDES_MR_OPMOD_OFB_Val               _U_(0x2)                                             /**< (TDES_MR) Output Feedback mode  */
#define   TDES_MR_OPMOD_CFB_Val               _U_(0x3)                                             /**< (TDES_MR) Cipher Feedback mode  */
#define TDES_MR_OPMOD_ECB                     (TDES_MR_OPMOD_ECB_Val << TDES_MR_OPMOD_Pos)         /**< (TDES_MR) Electronic Code Book mode Position  */
#define TDES_MR_OPMOD_CBC                     (TDES_MR_OPMOD_CBC_Val << TDES_MR_OPMOD_Pos)         /**< (TDES_MR) Cipher Block Chaining mode Position  */
#define TDES_MR_OPMOD_OFB                     (TDES_MR_OPMOD_OFB_Val << TDES_MR_OPMOD_Pos)         /**< (TDES_MR) Output Feedback mode Position  */
#define TDES_MR_OPMOD_CFB                     (TDES_MR_OPMOD_CFB_Val << TDES_MR_OPMOD_Pos)         /**< (TDES_MR) Cipher Feedback mode Position  */
#define TDES_MR_LOD_Pos                       _U_(15)                                              /**< (TDES_MR) Last Output Data Mode Position */
#define TDES_MR_LOD_Msk                       (_U_(0x1) << TDES_MR_LOD_Pos)                        /**< (TDES_MR) Last Output Data Mode Mask */
#define TDES_MR_LOD(value)                    (TDES_MR_LOD_Msk & ((value) << TDES_MR_LOD_Pos))    
#define TDES_MR_CFBS_Pos                      _U_(16)                                              /**< (TDES_MR) Cipher Feedback Data Size Position */
#define TDES_MR_CFBS_Msk                      (_U_(0x3) << TDES_MR_CFBS_Pos)                       /**< (TDES_MR) Cipher Feedback Data Size Mask */
#define TDES_MR_CFBS(value)                   (TDES_MR_CFBS_Msk & ((value) << TDES_MR_CFBS_Pos))  
#define   TDES_MR_CFBS_SIZE_64BIT_Val         _U_(0x0)                                             /**< (TDES_MR) 64-bit  */
#define   TDES_MR_CFBS_SIZE_32BIT_Val         _U_(0x1)                                             /**< (TDES_MR) 32-bit  */
#define   TDES_MR_CFBS_SIZE_16BIT_Val         _U_(0x2)                                             /**< (TDES_MR) 16-bit  */
#define   TDES_MR_CFBS_SIZE_8BIT_Val          _U_(0x3)                                             /**< (TDES_MR) 8-bit  */
#define TDES_MR_CFBS_SIZE_64BIT               (TDES_MR_CFBS_SIZE_64BIT_Val << TDES_MR_CFBS_Pos)    /**< (TDES_MR) 64-bit Position  */
#define TDES_MR_CFBS_SIZE_32BIT               (TDES_MR_CFBS_SIZE_32BIT_Val << TDES_MR_CFBS_Pos)    /**< (TDES_MR) 32-bit Position  */
#define TDES_MR_CFBS_SIZE_16BIT               (TDES_MR_CFBS_SIZE_16BIT_Val << TDES_MR_CFBS_Pos)    /**< (TDES_MR) 16-bit Position  */
#define TDES_MR_CFBS_SIZE_8BIT                (TDES_MR_CFBS_SIZE_8BIT_Val << TDES_MR_CFBS_Pos)     /**< (TDES_MR) 8-bit Position  */
#define TDES_MR_CKEY_Pos                      _U_(20)                                              /**< (TDES_MR) Countermeasure Key Position */
#define TDES_MR_CKEY_Msk                      (_U_(0xF) << TDES_MR_CKEY_Pos)                       /**< (TDES_MR) Countermeasure Key Mask */
#define TDES_MR_CKEY(value)                   (TDES_MR_CKEY_Msk & ((value) << TDES_MR_CKEY_Pos))  
#define TDES_MR_CMTYP1_Pos                    _U_(24)                                              /**< (TDES_MR) Countermeasure Type 1 Position */
#define TDES_MR_CMTYP1_Msk                    (_U_(0x1) << TDES_MR_CMTYP1_Pos)                     /**< (TDES_MR) Countermeasure Type 1 Mask */
#define TDES_MR_CMTYP1(value)                 (TDES_MR_CMTYP1_Msk & ((value) << TDES_MR_CMTYP1_Pos))
#define   TDES_MR_CMTYP1_NO_PAUSE_Val         _U_(0x0)                                             /**< (TDES_MR) Countermeasure type 1 is disabled.  */
#define   TDES_MR_CMTYP1_PAUSE_Val            _U_(0x1)                                             /**< (TDES_MR) Countermeasure type 1 is enabled.  */
#define TDES_MR_CMTYP1_NO_PAUSE               (TDES_MR_CMTYP1_NO_PAUSE_Val << TDES_MR_CMTYP1_Pos)  /**< (TDES_MR) Countermeasure type 1 is disabled. Position  */
#define TDES_MR_CMTYP1_PAUSE                  (TDES_MR_CMTYP1_PAUSE_Val << TDES_MR_CMTYP1_Pos)     /**< (TDES_MR) Countermeasure type 1 is enabled. Position  */
#define TDES_MR_CMTYP2_Pos                    _U_(25)                                              /**< (TDES_MR) Countermeasure Type 2 Position */
#define TDES_MR_CMTYP2_Msk                    (_U_(0x1) << TDES_MR_CMTYP2_Pos)                     /**< (TDES_MR) Countermeasure Type 2 Mask */
#define TDES_MR_CMTYP2(value)                 (TDES_MR_CMTYP2_Msk & ((value) << TDES_MR_CMTYP2_Pos))
#define   TDES_MR_CMTYP2_NO_DUMMY_Val         _U_(0x0)                                             /**< (TDES_MR) Countermeasure type 2 is disabled.  */
#define   TDES_MR_CMTYP2_DUMMY_Val            _U_(0x1)                                             /**< (TDES_MR) Countermeasure type 2 is enabled.  */
#define TDES_MR_CMTYP2_NO_DUMMY               (TDES_MR_CMTYP2_NO_DUMMY_Val << TDES_MR_CMTYP2_Pos)  /**< (TDES_MR) Countermeasure type 2 is disabled. Position  */
#define TDES_MR_CMTYP2_DUMMY                  (TDES_MR_CMTYP2_DUMMY_Val << TDES_MR_CMTYP2_Pos)     /**< (TDES_MR) Countermeasure type 2 is enabled. Position  */
#define TDES_MR_CMTYP3_Pos                    _U_(26)                                              /**< (TDES_MR) Countermeasure Type 3 Position */
#define TDES_MR_CMTYP3_Msk                    (_U_(0x1) << TDES_MR_CMTYP3_Pos)                     /**< (TDES_MR) Countermeasure Type 3 Mask */
#define TDES_MR_CMTYP3(value)                 (TDES_MR_CMTYP3_Msk & ((value) << TDES_MR_CMTYP3_Pos))
#define   TDES_MR_CMTYP3_NO_RESTART_Val       _U_(0x0)                                             /**< (TDES_MR) Countermeasure type 3 is disabled.  */
#define   TDES_MR_CMTYP3_RESTART_Val          _U_(0x1)                                             /**< (TDES_MR) Countermeasure type 3 is enabled.  */
#define TDES_MR_CMTYP3_NO_RESTART             (TDES_MR_CMTYP3_NO_RESTART_Val << TDES_MR_CMTYP3_Pos) /**< (TDES_MR) Countermeasure type 3 is disabled. Position  */
#define TDES_MR_CMTYP3_RESTART                (TDES_MR_CMTYP3_RESTART_Val << TDES_MR_CMTYP3_Pos)   /**< (TDES_MR) Countermeasure type 3 is enabled. Position  */
#define TDES_MR_CMTYP4_Pos                    _U_(27)                                              /**< (TDES_MR) Countermeasure Type 4 Position */
#define TDES_MR_CMTYP4_Msk                    (_U_(0x1) << TDES_MR_CMTYP4_Pos)                     /**< (TDES_MR) Countermeasure Type 4 Mask */
#define TDES_MR_CMTYP4(value)                 (TDES_MR_CMTYP4_Msk & ((value) << TDES_MR_CMTYP4_Pos))
#define   TDES_MR_CMTYP4_NO_IDLECURRENT_Val   _U_(0x0)                                             /**< (TDES_MR) Countermeasure type 4 is disabled.  */
#define   TDES_MR_CMTYP4_IDLECURRENT_Val      _U_(0x1)                                             /**< (TDES_MR) Countermeasure type 4 is enabled.  */
#define TDES_MR_CMTYP4_NO_IDLECURRENT         (TDES_MR_CMTYP4_NO_IDLECURRENT_Val << TDES_MR_CMTYP4_Pos) /**< (TDES_MR) Countermeasure type 4 is disabled. Position  */
#define TDES_MR_CMTYP4_IDLECURRENT            (TDES_MR_CMTYP4_IDLECURRENT_Val << TDES_MR_CMTYP4_Pos) /**< (TDES_MR) Countermeasure type 4 is enabled. Position  */
#define TDES_MR_CMTYP5_Pos                    _U_(28)                                              /**< (TDES_MR) Countermeasure Type 5 Position */
#define TDES_MR_CMTYP5_Msk                    (_U_(0x1) << TDES_MR_CMTYP5_Pos)                     /**< (TDES_MR) Countermeasure Type 5 Mask */
#define TDES_MR_CMTYP5(value)                 (TDES_MR_CMTYP5_Msk & ((value) << TDES_MR_CMTYP5_Pos))
#define   TDES_MR_CMTYP5_NO_ADDACCESS_Val     _U_(0x0)                                             /**< (TDES_MR) Countermeasure type 5 is disabled.  */
#define   TDES_MR_CMTYP5_ADDACCESS_Val        _U_(0x1)                                             /**< (TDES_MR) Countermeasure type 5 is enabled.  */
#define TDES_MR_CMTYP5_NO_ADDACCESS           (TDES_MR_CMTYP5_NO_ADDACCESS_Val << TDES_MR_CMTYP5_Pos) /**< (TDES_MR) Countermeasure type 5 is disabled. Position  */
#define TDES_MR_CMTYP5_ADDACCESS              (TDES_MR_CMTYP5_ADDACCESS_Val << TDES_MR_CMTYP5_Pos) /**< (TDES_MR) Countermeasure type 5 is enabled. Position  */
#define TDES_MR_CMTYP6_Pos                    _U_(29)                                              /**< (TDES_MR) Countermeasure Type 6 Position */
#define TDES_MR_CMTYP6_Msk                    (_U_(0x1) << TDES_MR_CMTYP6_Pos)                     /**< (TDES_MR) Countermeasure Type 6 Mask */
#define TDES_MR_CMTYP6(value)                 (TDES_MR_CMTYP6_Msk & ((value) << TDES_MR_CMTYP6_Pos))
#define   TDES_MR_CMTYP6_NO_UNIFORM_Val       _U_(0x0)                                             /**< (TDES_MR) Countermeasure type 6 is disabled.  */
#define   TDES_MR_CMTYP6_UNIFORM_Val          _U_(0x1)                                             /**< (TDES_MR) Countermeasure type 6 is enabled.  */
#define TDES_MR_CMTYP6_NO_UNIFORM             (TDES_MR_CMTYP6_NO_UNIFORM_Val << TDES_MR_CMTYP6_Pos) /**< (TDES_MR) Countermeasure type 6 is disabled. Position  */
#define TDES_MR_CMTYP6_UNIFORM                (TDES_MR_CMTYP6_UNIFORM_Val << TDES_MR_CMTYP6_Pos)   /**< (TDES_MR) Countermeasure type 6 is enabled. Position  */
#define TDES_MR_CMTYP7_Pos                    _U_(30)                                              /**< (TDES_MR) Countermeasure Type 7 Position */
#define TDES_MR_CMTYP7_Msk                    (_U_(0x1) << TDES_MR_CMTYP7_Pos)                     /**< (TDES_MR) Countermeasure Type 7 Mask */
#define TDES_MR_CMTYP7(value)                 (TDES_MR_CMTYP7_Msk & ((value) << TDES_MR_CMTYP7_Pos))
#define   TDES_MR_CMTYP7_NO_STARTDELAY_Val    _U_(0x0)                                             /**< (TDES_MR) Countermeasure type 7 is disabled.  */
#define   TDES_MR_CMTYP7_STARTDELAY_Val       _U_(0x1)                                             /**< (TDES_MR) Countermeasure type 7 is enabled.  */
#define TDES_MR_CMTYP7_NO_STARTDELAY          (TDES_MR_CMTYP7_NO_STARTDELAY_Val << TDES_MR_CMTYP7_Pos) /**< (TDES_MR) Countermeasure type 7 is disabled. Position  */
#define TDES_MR_CMTYP7_STARTDELAY             (TDES_MR_CMTYP7_STARTDELAY_Val << TDES_MR_CMTYP7_Pos) /**< (TDES_MR) Countermeasure type 7 is enabled. Position  */
#define TDES_MR_TAMPCLR_Pos                   _U_(31)                                              /**< (TDES_MR) Tamper Pin Clear Key Enable Position */
#define TDES_MR_TAMPCLR_Msk                   (_U_(0x1) << TDES_MR_TAMPCLR_Pos)                    /**< (TDES_MR) Tamper Pin Clear Key Enable Mask */
#define TDES_MR_TAMPCLR(value)                (TDES_MR_TAMPCLR_Msk & ((value) << TDES_MR_TAMPCLR_Pos))
#define TDES_MR_Msk                           _U_(0xFFF3B3D7)                                      /**< (TDES_MR) Register Mask  */

#define TDES_MR_CMTYP_Pos                     _U_(24)                                              /**< (TDES_MR Position) Countermeasure Type x */
#define TDES_MR_CMTYP_Msk                     (_U_(0x7F) << TDES_MR_CMTYP_Pos)                     /**< (TDES_MR Mask) CMTYP */
#define TDES_MR_CMTYP(value)                  (TDES_MR_CMTYP_Msk & ((value) << TDES_MR_CMTYP_Pos)) 

/* -------- TDES_IER : (TDES Offset: 0x10) ( /W 32) Interrupt Enable Register -------- */
#define TDES_IER_DATRDY_Pos                   _U_(0)                                               /**< (TDES_IER) Data Ready Interrupt Enable Position */
#define TDES_IER_DATRDY_Msk                   (_U_(0x1) << TDES_IER_DATRDY_Pos)                    /**< (TDES_IER) Data Ready Interrupt Enable Mask */
#define TDES_IER_DATRDY(value)                (TDES_IER_DATRDY_Msk & ((value) << TDES_IER_DATRDY_Pos))
#define TDES_IER_URAD_Pos                     _U_(8)                                               /**< (TDES_IER) Unspecified Register Access Detection Interrupt Enable Position */
#define TDES_IER_URAD_Msk                     (_U_(0x1) << TDES_IER_URAD_Pos)                      /**< (TDES_IER) Unspecified Register Access Detection Interrupt Enable Mask */
#define TDES_IER_URAD(value)                  (TDES_IER_URAD_Msk & ((value) << TDES_IER_URAD_Pos))
#define TDES_IER_SECE_Pos                     _U_(16)                                              /**< (TDES_IER) Security and/or Safety Event Interrupt Enable Position */
#define TDES_IER_SECE_Msk                     (_U_(0x1) << TDES_IER_SECE_Pos)                      /**< (TDES_IER) Security and/or Safety Event Interrupt Enable Mask */
#define TDES_IER_SECE(value)                  (TDES_IER_SECE_Msk & ((value) << TDES_IER_SECE_Pos))
#define TDES_IER_Msk                          _U_(0x00010101)                                      /**< (TDES_IER) Register Mask  */


/* -------- TDES_IDR : (TDES Offset: 0x14) ( /W 32) Interrupt Disable Register -------- */
#define TDES_IDR_DATRDY_Pos                   _U_(0)                                               /**< (TDES_IDR) Data Ready Interrupt Disable Position */
#define TDES_IDR_DATRDY_Msk                   (_U_(0x1) << TDES_IDR_DATRDY_Pos)                    /**< (TDES_IDR) Data Ready Interrupt Disable Mask */
#define TDES_IDR_DATRDY(value)                (TDES_IDR_DATRDY_Msk & ((value) << TDES_IDR_DATRDY_Pos))
#define TDES_IDR_URAD_Pos                     _U_(8)                                               /**< (TDES_IDR) Unspecified Register Access Detection Interrupt Disable Position */
#define TDES_IDR_URAD_Msk                     (_U_(0x1) << TDES_IDR_URAD_Pos)                      /**< (TDES_IDR) Unspecified Register Access Detection Interrupt Disable Mask */
#define TDES_IDR_URAD(value)                  (TDES_IDR_URAD_Msk & ((value) << TDES_IDR_URAD_Pos))
#define TDES_IDR_SECE_Pos                     _U_(16)                                              /**< (TDES_IDR) Security and/or Safety Event Interrupt Disable Position */
#define TDES_IDR_SECE_Msk                     (_U_(0x1) << TDES_IDR_SECE_Pos)                      /**< (TDES_IDR) Security and/or Safety Event Interrupt Disable Mask */
#define TDES_IDR_SECE(value)                  (TDES_IDR_SECE_Msk & ((value) << TDES_IDR_SECE_Pos))
#define TDES_IDR_Msk                          _U_(0x00010101)                                      /**< (TDES_IDR) Register Mask  */


/* -------- TDES_IMR : (TDES Offset: 0x18) ( R/ 32) Interrupt Mask Register -------- */
#define TDES_IMR_DATRDY_Pos                   _U_(0)                                               /**< (TDES_IMR) Data Ready Interrupt Mask Position */
#define TDES_IMR_DATRDY_Msk                   (_U_(0x1) << TDES_IMR_DATRDY_Pos)                    /**< (TDES_IMR) Data Ready Interrupt Mask Mask */
#define TDES_IMR_DATRDY(value)                (TDES_IMR_DATRDY_Msk & ((value) << TDES_IMR_DATRDY_Pos))
#define TDES_IMR_URAD_Pos                     _U_(8)                                               /**< (TDES_IMR) Unspecified Register Access Detection Interrupt Mask Position */
#define TDES_IMR_URAD_Msk                     (_U_(0x1) << TDES_IMR_URAD_Pos)                      /**< (TDES_IMR) Unspecified Register Access Detection Interrupt Mask Mask */
#define TDES_IMR_URAD(value)                  (TDES_IMR_URAD_Msk & ((value) << TDES_IMR_URAD_Pos))
#define TDES_IMR_SECE_Pos                     _U_(16)                                              /**< (TDES_IMR) Security and/or Safety Event Interrupt Mask Position */
#define TDES_IMR_SECE_Msk                     (_U_(0x1) << TDES_IMR_SECE_Pos)                      /**< (TDES_IMR) Security and/or Safety Event Interrupt Mask Mask */
#define TDES_IMR_SECE(value)                  (TDES_IMR_SECE_Msk & ((value) << TDES_IMR_SECE_Pos))
#define TDES_IMR_Msk                          _U_(0x00010101)                                      /**< (TDES_IMR) Register Mask  */


/* -------- TDES_ISR : (TDES Offset: 0x1C) ( R/ 32) Interrupt Status Register -------- */
#define TDES_ISR_DATRDY_Pos                   _U_(0)                                               /**< (TDES_ISR) Data Ready (cleared by setting TDES_CR.START or TDES_CR.SWRST, or by reading TDES_ODATARx) Position */
#define TDES_ISR_DATRDY_Msk                   (_U_(0x1) << TDES_ISR_DATRDY_Pos)                    /**< (TDES_ISR) Data Ready (cleared by setting TDES_CR.START or TDES_CR.SWRST, or by reading TDES_ODATARx) Mask */
#define TDES_ISR_DATRDY(value)                (TDES_ISR_DATRDY_Msk & ((value) << TDES_ISR_DATRDY_Pos))
#define TDES_ISR_URAD_Pos                     _U_(8)                                               /**< (TDES_ISR) Unspecified Register Access Detection Status (cleared by setting TDES_CR.SWRST) Position */
#define TDES_ISR_URAD_Msk                     (_U_(0x1) << TDES_ISR_URAD_Pos)                      /**< (TDES_ISR) Unspecified Register Access Detection Status (cleared by setting TDES_CR.SWRST) Mask */
#define TDES_ISR_URAD(value)                  (TDES_ISR_URAD_Msk & ((value) << TDES_ISR_URAD_Pos))
#define TDES_ISR_URAT_Pos                     _U_(12)                                              /**< (TDES_ISR) Unspecified Register Access (cleared by setting TDES_CR.SWRST) Position */
#define TDES_ISR_URAT_Msk                     (_U_(0x3) << TDES_ISR_URAT_Pos)                      /**< (TDES_ISR) Unspecified Register Access (cleared by setting TDES_CR.SWRST) Mask */
#define TDES_ISR_URAT(value)                  (TDES_ISR_URAT_Msk & ((value) << TDES_ISR_URAT_Pos))
#define   TDES_ISR_URAT_IDR_WR_PROCESSING_Val _U_(0x0)                                             /**< (TDES_ISR) TDES_IDATAR written during data processing when SMOD = 0x2 mode.  */
#define   TDES_ISR_URAT_ODR_RD_PROCESSING_Val _U_(0x1)                                             /**< (TDES_ISR) TDES_ODATAR read during data processing.  */
#define   TDES_ISR_URAT_MR_WR_PROCESSING_Val  _U_(0x2)                                             /**< (TDES_ISR) TDES_MR written during data processing.  */
#define   TDES_ISR_URAT_WOR_RD_ACCESS_Val     _U_(0x3)                                             /**< (TDES_ISR) Write-only register read access.  */
#define TDES_ISR_URAT_IDR_WR_PROCESSING       (TDES_ISR_URAT_IDR_WR_PROCESSING_Val << TDES_ISR_URAT_Pos) /**< (TDES_ISR) TDES_IDATAR written during data processing when SMOD = 0x2 mode. Position  */
#define TDES_ISR_URAT_ODR_RD_PROCESSING       (TDES_ISR_URAT_ODR_RD_PROCESSING_Val << TDES_ISR_URAT_Pos) /**< (TDES_ISR) TDES_ODATAR read during data processing. Position  */
#define TDES_ISR_URAT_MR_WR_PROCESSING        (TDES_ISR_URAT_MR_WR_PROCESSING_Val << TDES_ISR_URAT_Pos) /**< (TDES_ISR) TDES_MR written during data processing. Position  */
#define TDES_ISR_URAT_WOR_RD_ACCESS           (TDES_ISR_URAT_WOR_RD_ACCESS_Val << TDES_ISR_URAT_Pos) /**< (TDES_ISR) Write-only register read access. Position  */
#define TDES_ISR_SECE_Pos                     _U_(16)                                              /**< (TDES_ISR) Security and/or Safety Event Interrupt Mask Position */
#define TDES_ISR_SECE_Msk                     (_U_(0x1) << TDES_ISR_SECE_Pos)                      /**< (TDES_ISR) Security and/or Safety Event Interrupt Mask Mask */
#define TDES_ISR_SECE(value)                  (TDES_ISR_SECE_Msk & ((value) << TDES_ISR_SECE_Pos))
#define TDES_ISR_Msk                          _U_(0x00013101)                                      /**< (TDES_ISR) Register Mask  */


/* -------- TDES_KEY1WR : (TDES Offset: 0x20) ( /W 32) Key 1 Word Register -------- */
#define TDES_KEY1WR_KEY1W_Pos                 _U_(0)                                               /**< (TDES_KEY1WR) Key 1 Word Position */
#define TDES_KEY1WR_KEY1W_Msk                 (_U_(0xFFFFFFFF) << TDES_KEY1WR_KEY1W_Pos)           /**< (TDES_KEY1WR) Key 1 Word Mask */
#define TDES_KEY1WR_KEY1W(value)              (TDES_KEY1WR_KEY1W_Msk & ((value) << TDES_KEY1WR_KEY1W_Pos))
#define TDES_KEY1WR_Msk                       _U_(0xFFFFFFFF)                                      /**< (TDES_KEY1WR) Register Mask  */


/* -------- TDES_KEY2WR : (TDES Offset: 0x28) ( /W 32) Key 2 Word Register -------- */
#define TDES_KEY2WR_KEY2W_Pos                 _U_(0)                                               /**< (TDES_KEY2WR) Key 2 Word Position */
#define TDES_KEY2WR_KEY2W_Msk                 (_U_(0xFFFFFFFF) << TDES_KEY2WR_KEY2W_Pos)           /**< (TDES_KEY2WR) Key 2 Word Mask */
#define TDES_KEY2WR_KEY2W(value)              (TDES_KEY2WR_KEY2W_Msk & ((value) << TDES_KEY2WR_KEY2W_Pos))
#define TDES_KEY2WR_Msk                       _U_(0xFFFFFFFF)                                      /**< (TDES_KEY2WR) Register Mask  */


/* -------- TDES_KEY3WR : (TDES Offset: 0x30) ( /W 32) Key 3 Word Register -------- */
#define TDES_KEY3WR_KEY3W_Pos                 _U_(0)                                               /**< (TDES_KEY3WR) Key 3 Word Position */
#define TDES_KEY3WR_KEY3W_Msk                 (_U_(0xFFFFFFFF) << TDES_KEY3WR_KEY3W_Pos)           /**< (TDES_KEY3WR) Key 3 Word Mask */
#define TDES_KEY3WR_KEY3W(value)              (TDES_KEY3WR_KEY3W_Msk & ((value) << TDES_KEY3WR_KEY3W_Pos))
#define TDES_KEY3WR_Msk                       _U_(0xFFFFFFFF)                                      /**< (TDES_KEY3WR) Register Mask  */


/* -------- TDES_IDATAR : (TDES Offset: 0x40) ( /W 32) Input Data Register -------- */
#define TDES_IDATAR_IDATA_Pos                 _U_(0)                                               /**< (TDES_IDATAR) Input Data Position */
#define TDES_IDATAR_IDATA_Msk                 (_U_(0xFFFFFFFF) << TDES_IDATAR_IDATA_Pos)           /**< (TDES_IDATAR) Input Data Mask */
#define TDES_IDATAR_IDATA(value)              (TDES_IDATAR_IDATA_Msk & ((value) << TDES_IDATAR_IDATA_Pos))
#define TDES_IDATAR_Msk                       _U_(0xFFFFFFFF)                                      /**< (TDES_IDATAR) Register Mask  */


/* -------- TDES_ODATAR : (TDES Offset: 0x50) ( R/ 32) Output Data Register -------- */
#define TDES_ODATAR_ODATA_Pos                 _U_(0)                                               /**< (TDES_ODATAR) Output Data Position */
#define TDES_ODATAR_ODATA_Msk                 (_U_(0xFFFFFFFF) << TDES_ODATAR_ODATA_Pos)           /**< (TDES_ODATAR) Output Data Mask */
#define TDES_ODATAR_ODATA(value)              (TDES_ODATAR_ODATA_Msk & ((value) << TDES_ODATAR_ODATA_Pos))
#define TDES_ODATAR_Msk                       _U_(0xFFFFFFFF)                                      /**< (TDES_ODATAR) Register Mask  */


/* -------- TDES_IVR : (TDES Offset: 0x60) ( /W 32) Initialization Vector Register -------- */
#define TDES_IVR_IV_Pos                       _U_(0)                                               /**< (TDES_IVR) Initialization Vector Position */
#define TDES_IVR_IV_Msk                       (_U_(0xFFFFFFFF) << TDES_IVR_IV_Pos)                 /**< (TDES_IVR) Initialization Vector Mask */
#define TDES_IVR_IV(value)                    (TDES_IVR_IV_Msk & ((value) << TDES_IVR_IV_Pos))    
#define TDES_IVR_Msk                          _U_(0xFFFFFFFF)                                      /**< (TDES_IVR) Register Mask  */


/* -------- TDES_XTEA_RNDR : (TDES Offset: 0x70) (R/W 32) XTEA Rounds Register -------- */
#define TDES_XTEA_RNDR_XTEA_RNDS_Pos          _U_(0)                                               /**< (TDES_XTEA_RNDR) Number of Rounds Position */
#define TDES_XTEA_RNDR_XTEA_RNDS_Msk          (_U_(0x3F) << TDES_XTEA_RNDR_XTEA_RNDS_Pos)          /**< (TDES_XTEA_RNDR) Number of Rounds Mask */
#define TDES_XTEA_RNDR_XTEA_RNDS(value)       (TDES_XTEA_RNDR_XTEA_RNDS_Msk & ((value) << TDES_XTEA_RNDR_XTEA_RNDS_Pos))
#define TDES_XTEA_RNDR_Msk                    _U_(0x0000003F)                                      /**< (TDES_XTEA_RNDR) Register Mask  */


/* -------- TDES_WPMR : (TDES Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define TDES_WPMR_WPEN_Pos                    _U_(0)                                               /**< (TDES_WPMR) Write Protection Enable Position */
#define TDES_WPMR_WPEN_Msk                    (_U_(0x1) << TDES_WPMR_WPEN_Pos)                     /**< (TDES_WPMR) Write Protection Enable Mask */
#define TDES_WPMR_WPEN(value)                 (TDES_WPMR_WPEN_Msk & ((value) << TDES_WPMR_WPEN_Pos))
#define TDES_WPMR_WPITEN_Pos                  _U_(1)                                               /**< (TDES_WPMR) Write Protection Interrupt Enable Position */
#define TDES_WPMR_WPITEN_Msk                  (_U_(0x1) << TDES_WPMR_WPITEN_Pos)                   /**< (TDES_WPMR) Write Protection Interrupt Enable Mask */
#define TDES_WPMR_WPITEN(value)               (TDES_WPMR_WPITEN_Msk & ((value) << TDES_WPMR_WPITEN_Pos))
#define TDES_WPMR_WPCREN_Pos                  _U_(2)                                               /**< (TDES_WPMR) Write Protection Control Enable Position */
#define TDES_WPMR_WPCREN_Msk                  (_U_(0x1) << TDES_WPMR_WPCREN_Pos)                   /**< (TDES_WPMR) Write Protection Control Enable Mask */
#define TDES_WPMR_WPCREN(value)               (TDES_WPMR_WPCREN_Msk & ((value) << TDES_WPMR_WPCREN_Pos))
#define TDES_WPMR_FIRSTE_Pos                  _U_(4)                                               /**< (TDES_WPMR) First Error Report Enable Position */
#define TDES_WPMR_FIRSTE_Msk                  (_U_(0x1) << TDES_WPMR_FIRSTE_Pos)                   /**< (TDES_WPMR) First Error Report Enable Mask */
#define TDES_WPMR_FIRSTE(value)               (TDES_WPMR_FIRSTE_Msk & ((value) << TDES_WPMR_FIRSTE_Pos))
#define TDES_WPMR_ACTION_Pos                  _U_(5)                                               /**< (TDES_WPMR) Action on Abnormal Event Detection Position */
#define TDES_WPMR_ACTION_Msk                  (_U_(0x7) << TDES_WPMR_ACTION_Pos)                   /**< (TDES_WPMR) Action on Abnormal Event Detection Mask */
#define TDES_WPMR_ACTION(value)               (TDES_WPMR_ACTION_Msk & ((value) << TDES_WPMR_ACTION_Pos))
#define   TDES_WPMR_ACTION_REPORT_ONLY_Val    _U_(0x0)                                             /**< (TDES_WPMR) No action (stop or clear key) is performed when one of PKRPVS, WPVS, CGD, SEQE, or SWE flags are set.  */
#define   TDES_WPMR_ACTION_LOCK_PKRPVS_WPVS_SWE_Val _U_(0x1)                                             /**< (TDES_WPMR) If a processing is in progress when the TDES_WPSR.PKRPVS/WPVS/SWE event detection occurs, the current processing is ended normally but no other processing is started while a TDES_CR.UNLOCK command is issued.  */
#define   TDES_WPMR_ACTION_LOCK_CGD_SEQE_Val  _U_(0x2)                                             /**< (TDES_WPMR) If a processing is in progress when the TDES_WPSR.CGD/SEQE event detection occurs, the current processing is ended normally but no other processing is started while a TDES_CR.UNLOCK command is issued.  */
#define   TDES_WPMR_ACTION_LOCK_ANY_EV_Val    _U_(0x3)                                             /**< (TDES_WPMR) If a processing is in progress when the TDES_WPSR.PKRPVS/WPVS/CGD/SEQE/SWE events detection occurs, the current processing is ended normally but no other processing is started while a TDES_CR.UNLOCK command is issued.  */
#define   TDES_WPMR_ACTION_CLEAR_PKRPVS_WPVS_SWE_Val _U_(0x4)                                             /**< (TDES_WPMR) If a processing is in progress when the TDES_WPSR.PKRPVS/WPVS/SWE events detection occurs, the current processing is ended normally but no other processing is started while a TDES_CR.UNLOCK command is issued. Moreover, TDES_KEYxWRy are immediately cleared.  */
#define   TDES_WPMR_ACTION_CLEAR_CGD_SEQE_Val _U_(0x5)                                             /**< (TDES_WPMR) If a processing is in progress when the TDES_WPSR.CGD/SEQE events detection occurs, the current processing is ended normally but no other processing is started while a TDES_CR.UNLOCK command is issued. Moreover, TDES_KEYxWRy are immediately cleared.  */
#define   TDES_WPMR_ACTION_CLEAR_ANY_EV_Val   _U_(0x6)                                             /**< (TDES_WPMR) If a processing is in progress when the TDES_WPSR.PKRPVS/WPVS/CGD/SEQE/SWE events detection occurs, the current processing is ended normally but no other processing is started while a TDES_CR.UNLOCK command is issued. Moreover, TDES_KEYxWRy are immediately cleared.  */
#define TDES_WPMR_ACTION_REPORT_ONLY          (TDES_WPMR_ACTION_REPORT_ONLY_Val << TDES_WPMR_ACTION_Pos) /**< (TDES_WPMR) No action (stop or clear key) is performed when one of PKRPVS, WPVS, CGD, SEQE, or SWE flags are set. Position  */
#define TDES_WPMR_ACTION_LOCK_PKRPVS_WPVS_SWE (TDES_WPMR_ACTION_LOCK_PKRPVS_WPVS_SWE_Val << TDES_WPMR_ACTION_Pos) /**< (TDES_WPMR) If a processing is in progress when the TDES_WPSR.PKRPVS/WPVS/SWE event detection occurs, the current processing is ended normally but no other processing is started while a TDES_CR.UNLOCK command is issued. Position  */
#define TDES_WPMR_ACTION_LOCK_CGD_SEQE        (TDES_WPMR_ACTION_LOCK_CGD_SEQE_Val << TDES_WPMR_ACTION_Pos) /**< (TDES_WPMR) If a processing is in progress when the TDES_WPSR.CGD/SEQE event detection occurs, the current processing is ended normally but no other processing is started while a TDES_CR.UNLOCK command is issued. Position  */
#define TDES_WPMR_ACTION_LOCK_ANY_EV          (TDES_WPMR_ACTION_LOCK_ANY_EV_Val << TDES_WPMR_ACTION_Pos) /**< (TDES_WPMR) If a processing is in progress when the TDES_WPSR.PKRPVS/WPVS/CGD/SEQE/SWE events detection occurs, the current processing is ended normally but no other processing is started while a TDES_CR.UNLOCK command is issued. Position  */
#define TDES_WPMR_ACTION_CLEAR_PKRPVS_WPVS_SWE (TDES_WPMR_ACTION_CLEAR_PKRPVS_WPVS_SWE_Val << TDES_WPMR_ACTION_Pos) /**< (TDES_WPMR) If a processing is in progress when the TDES_WPSR.PKRPVS/WPVS/SWE events detection occurs, the current processing is ended normally but no other processing is started while a TDES_CR.UNLOCK command is issued. Moreover, TDES_KEYxWRy are immediately cleared. Position  */
#define TDES_WPMR_ACTION_CLEAR_CGD_SEQE       (TDES_WPMR_ACTION_CLEAR_CGD_SEQE_Val << TDES_WPMR_ACTION_Pos) /**< (TDES_WPMR) If a processing is in progress when the TDES_WPSR.CGD/SEQE events detection occurs, the current processing is ended normally but no other processing is started while a TDES_CR.UNLOCK command is issued. Moreover, TDES_KEYxWRy are immediately cleared. Position  */
#define TDES_WPMR_ACTION_CLEAR_ANY_EV         (TDES_WPMR_ACTION_CLEAR_ANY_EV_Val << TDES_WPMR_ACTION_Pos) /**< (TDES_WPMR) If a processing is in progress when the TDES_WPSR.PKRPVS/WPVS/CGD/SEQE/SWE events detection occurs, the current processing is ended normally but no other processing is started while a TDES_CR.UNLOCK command is issued. Moreover, TDES_KEYxWRy are immediately cleared. Position  */
#define TDES_WPMR_WPKEY_Pos                   _U_(8)                                               /**< (TDES_WPMR) Write Protection Key Position */
#define TDES_WPMR_WPKEY_Msk                   (_U_(0xFFFFFF) << TDES_WPMR_WPKEY_Pos)               /**< (TDES_WPMR) Write Protection Key Mask */
#define TDES_WPMR_WPKEY(value)                (TDES_WPMR_WPKEY_Msk & ((value) << TDES_WPMR_WPKEY_Pos))
#define   TDES_WPMR_WPKEY_PASSWD_Val          _U_(0x444553)                                        /**< (TDES_WPMR) Writing any other value in this field aborts the write operation of bits WPEN, WPITEN and WPCREN. Always reads as 0.  */
#define TDES_WPMR_WPKEY_PASSWD                (TDES_WPMR_WPKEY_PASSWD_Val << TDES_WPMR_WPKEY_Pos)  /**< (TDES_WPMR) Writing any other value in this field aborts the write operation of bits WPEN, WPITEN and WPCREN. Always reads as 0. Position  */
#define TDES_WPMR_Msk                         _U_(0xFFFFFFF7)                                      /**< (TDES_WPMR) Register Mask  */


/* -------- TDES_WPSR : (TDES Offset: 0xE8) ( R/ 32) Write Protection Status Register -------- */
#define TDES_WPSR_WPVS_Pos                    _U_(0)                                               /**< (TDES_WPSR) Write Protection Violation Status (cleared on read) Position */
#define TDES_WPSR_WPVS_Msk                    (_U_(0x1) << TDES_WPSR_WPVS_Pos)                     /**< (TDES_WPSR) Write Protection Violation Status (cleared on read) Mask */
#define TDES_WPSR_WPVS(value)                 (TDES_WPSR_WPVS_Msk & ((value) << TDES_WPSR_WPVS_Pos))
#define TDES_WPSR_CGD_Pos                     _U_(1)                                               /**< (TDES_WPSR) Clock Glitch Detected (cleared on read) Position */
#define TDES_WPSR_CGD_Msk                     (_U_(0x1) << TDES_WPSR_CGD_Pos)                      /**< (TDES_WPSR) Clock Glitch Detected (cleared on read) Mask */
#define TDES_WPSR_CGD(value)                  (TDES_WPSR_CGD_Msk & ((value) << TDES_WPSR_CGD_Pos))
#define TDES_WPSR_SEQE_Pos                    _U_(2)                                               /**< (TDES_WPSR) Internal Sequencer Error (cleared on read) Position */
#define TDES_WPSR_SEQE_Msk                    (_U_(0x1) << TDES_WPSR_SEQE_Pos)                     /**< (TDES_WPSR) Internal Sequencer Error (cleared on read) Mask */
#define TDES_WPSR_SEQE(value)                 (TDES_WPSR_SEQE_Msk & ((value) << TDES_WPSR_SEQE_Pos))
#define TDES_WPSR_SWE_Pos                     _U_(3)                                               /**< (TDES_WPSR) Software Control Error (cleared on read) Position */
#define TDES_WPSR_SWE_Msk                     (_U_(0x1) << TDES_WPSR_SWE_Pos)                      /**< (TDES_WPSR) Software Control Error (cleared on read) Mask */
#define TDES_WPSR_SWE(value)                  (TDES_WPSR_SWE_Msk & ((value) << TDES_WPSR_SWE_Pos))
#define TDES_WPSR_PKRPVS_Pos                  _U_(4)                                               /**< (TDES_WPSR) Private Key Register Protection Violation Status (cleared on read) Position */
#define TDES_WPSR_PKRPVS_Msk                  (_U_(0x1) << TDES_WPSR_PKRPVS_Pos)                   /**< (TDES_WPSR) Private Key Register Protection Violation Status (cleared on read) Mask */
#define TDES_WPSR_PKRPVS(value)               (TDES_WPSR_PKRPVS_Msk & ((value) << TDES_WPSR_PKRPVS_Pos))
#define TDES_WPSR_WPVSRC_Pos                  _U_(8)                                               /**< (TDES_WPSR) Write Protection Violation Source (cleared on read) Position */
#define TDES_WPSR_WPVSRC_Msk                  (_U_(0xFFFF) << TDES_WPSR_WPVSRC_Pos)                /**< (TDES_WPSR) Write Protection Violation Source (cleared on read) Mask */
#define TDES_WPSR_WPVSRC(value)               (TDES_WPSR_WPVSRC_Msk & ((value) << TDES_WPSR_WPVSRC_Pos))
#define TDES_WPSR_SWETYP_Pos                  _U_(24)                                              /**< (TDES_WPSR) Software Error Type (cleared on read) Position */
#define TDES_WPSR_SWETYP_Msk                  (_U_(0xF) << TDES_WPSR_SWETYP_Pos)                   /**< (TDES_WPSR) Software Error Type (cleared on read) Mask */
#define TDES_WPSR_SWETYP(value)               (TDES_WPSR_SWETYP_Msk & ((value) << TDES_WPSR_SWETYP_Pos))
#define   TDES_WPSR_SWETYP_READ_WO_Val        _U_(0x0)                                             /**< (TDES_WPSR) A write-only register has been read (Warning).  */
#define   TDES_WPSR_SWETYP_WRITE_RO_Val       _U_(0x1)                                             /**< (TDES_WPSR) TDES is enabled and a write access has been performed on a read-only register (Warning).  */
#define   TDES_WPSR_SWETYP_UNDEF_RW_Val       _U_(0x2)                                             /**< (TDES_WPSR) Access to an undefined address (Warning).  */
#define   TDES_WPSR_SWETYP_CTRL_START_Val     _U_(0x3)                                             /**< (TDES_WPSR) Abnormal use of TDES_CR.START command when DMA access is configured.  */
#define   TDES_WPSR_SWETYP_WEIRD_ACTION_Val   _U_(0x4)                                             /**< (TDES_WPSR) A key write, init value write, output data read, Mode register write, private key bus access or XTEA round register has been performed while a current processing is in progress (abnormal).  */
#define   TDES_WPSR_SWETYP_INCOMPLETE_KEY_Val _U_(0x5)                                             /**< (TDES_WPSR) A tentative of start is required while the keys are not fully loaded into TDES_KEYxWRy.  */
#define TDES_WPSR_SWETYP_READ_WO              (TDES_WPSR_SWETYP_READ_WO_Val << TDES_WPSR_SWETYP_Pos) /**< (TDES_WPSR) A write-only register has been read (Warning). Position  */
#define TDES_WPSR_SWETYP_WRITE_RO             (TDES_WPSR_SWETYP_WRITE_RO_Val << TDES_WPSR_SWETYP_Pos) /**< (TDES_WPSR) TDES is enabled and a write access has been performed on a read-only register (Warning). Position  */
#define TDES_WPSR_SWETYP_UNDEF_RW             (TDES_WPSR_SWETYP_UNDEF_RW_Val << TDES_WPSR_SWETYP_Pos) /**< (TDES_WPSR) Access to an undefined address (Warning). Position  */
#define TDES_WPSR_SWETYP_CTRL_START           (TDES_WPSR_SWETYP_CTRL_START_Val << TDES_WPSR_SWETYP_Pos) /**< (TDES_WPSR) Abnormal use of TDES_CR.START command when DMA access is configured. Position  */
#define TDES_WPSR_SWETYP_WEIRD_ACTION         (TDES_WPSR_SWETYP_WEIRD_ACTION_Val << TDES_WPSR_SWETYP_Pos) /**< (TDES_WPSR) A key write, init value write, output data read, Mode register write, private key bus access or XTEA round register has been performed while a current processing is in progress (abnormal). Position  */
#define TDES_WPSR_SWETYP_INCOMPLETE_KEY       (TDES_WPSR_SWETYP_INCOMPLETE_KEY_Val << TDES_WPSR_SWETYP_Pos) /**< (TDES_WPSR) A tentative of start is required while the keys are not fully loaded into TDES_KEYxWRy. Position  */
#define TDES_WPSR_ECLASS_Pos                  _U_(31)                                              /**< (TDES_WPSR) Software Error Class (cleared on read) Position */
#define TDES_WPSR_ECLASS_Msk                  (_U_(0x1) << TDES_WPSR_ECLASS_Pos)                   /**< (TDES_WPSR) Software Error Class (cleared on read) Mask */
#define TDES_WPSR_ECLASS(value)               (TDES_WPSR_ECLASS_Msk & ((value) << TDES_WPSR_ECLASS_Pos))
#define   TDES_WPSR_ECLASS_WARNING_Val        _U_(0x0)                                             /**< (TDES_WPSR) An abnormal access that does not affect system functionality.  */
#define   TDES_WPSR_ECLASS_ERROR_Val          _U_(0x1)                                             /**< (TDES_WPSR) An access is performed into key, input data, control registers while the TDES is performing an encryption/decryption or a start is request by software or DMA while the key is not fully configured.  */
#define TDES_WPSR_ECLASS_WARNING              (TDES_WPSR_ECLASS_WARNING_Val << TDES_WPSR_ECLASS_Pos) /**< (TDES_WPSR) An abnormal access that does not affect system functionality. Position  */
#define TDES_WPSR_ECLASS_ERROR                (TDES_WPSR_ECLASS_ERROR_Val << TDES_WPSR_ECLASS_Pos) /**< (TDES_WPSR) An access is performed into key, input data, control registers while the TDES is performing an encryption/decryption or a start is request by software or DMA while the key is not fully configured. Position  */
#define TDES_WPSR_Msk                         _U_(0x8FFFFF1F)                                      /**< (TDES_WPSR) Register Mask  */


/** \brief TDES register offsets definitions */
#define TDES_CR_REG_OFST               (0x00)              /**< (TDES_CR) Control Register Offset */
#define TDES_MR_REG_OFST               (0x04)              /**< (TDES_MR) Mode Register Offset */
#define TDES_IER_REG_OFST              (0x10)              /**< (TDES_IER) Interrupt Enable Register Offset */
#define TDES_IDR_REG_OFST              (0x14)              /**< (TDES_IDR) Interrupt Disable Register Offset */
#define TDES_IMR_REG_OFST              (0x18)              /**< (TDES_IMR) Interrupt Mask Register Offset */
#define TDES_ISR_REG_OFST              (0x1C)              /**< (TDES_ISR) Interrupt Status Register Offset */
#define TDES_KEY1WR_REG_OFST           (0x20)              /**< (TDES_KEY1WR) Key 1 Word Register Offset */
#define TDES_KEY2WR_REG_OFST           (0x28)              /**< (TDES_KEY2WR) Key 2 Word Register Offset */
#define TDES_KEY3WR_REG_OFST           (0x30)              /**< (TDES_KEY3WR) Key 3 Word Register Offset */
#define TDES_IDATAR_REG_OFST           (0x40)              /**< (TDES_IDATAR) Input Data Register Offset */
#define TDES_ODATAR_REG_OFST           (0x50)              /**< (TDES_ODATAR) Output Data Register Offset */
#define TDES_IVR_REG_OFST              (0x60)              /**< (TDES_IVR) Initialization Vector Register Offset */
#define TDES_XTEA_RNDR_REG_OFST        (0x70)              /**< (TDES_XTEA_RNDR) XTEA Rounds Register Offset */
#define TDES_WPMR_REG_OFST             (0xE4)              /**< (TDES_WPMR) Write Protection Mode Register Offset */
#define TDES_WPSR_REG_OFST             (0xE8)              /**< (TDES_WPSR) Write Protection Status Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief TDES register API structure */
typedef struct
{
  __O   uint32_t                       TDES_CR;            /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       TDES_MR;            /**< Offset: 0x04 (R/W  32) Mode Register */
  __I   uint8_t                        Reserved1[0x08];
  __O   uint32_t                       TDES_IER;           /**< Offset: 0x10 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       TDES_IDR;           /**< Offset: 0x14 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       TDES_IMR;           /**< Offset: 0x18 (R/   32) Interrupt Mask Register */
  __I   uint32_t                       TDES_ISR;           /**< Offset: 0x1C (R/   32) Interrupt Status Register */
  __O   uint32_t                       TDES_KEY1WR[2];     /**< Offset: 0x20 ( /W  32) Key 1 Word Register */
  __O   uint32_t                       TDES_KEY2WR[2];     /**< Offset: 0x28 ( /W  32) Key 2 Word Register */
  __O   uint32_t                       TDES_KEY3WR[2];     /**< Offset: 0x30 ( /W  32) Key 3 Word Register */
  __I   uint8_t                        Reserved2[0x08];
  __O   uint32_t                       TDES_IDATAR[2];     /**< Offset: 0x40 ( /W  32) Input Data Register */
  __I   uint8_t                        Reserved3[0x08];
  __I   uint32_t                       TDES_ODATAR[2];     /**< Offset: 0x50 (R/   32) Output Data Register */
  __I   uint8_t                        Reserved4[0x08];
  __O   uint32_t                       TDES_IVR[2];        /**< Offset: 0x60 ( /W  32) Initialization Vector Register */
  __I   uint8_t                        Reserved5[0x08];
  __IO  uint32_t                       TDES_XTEA_RNDR;     /**< Offset: 0x70 (R/W  32) XTEA Rounds Register */
  __I   uint8_t                        Reserved6[0x70];
  __IO  uint32_t                       TDES_WPMR;          /**< Offset: 0xE4 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       TDES_WPSR;          /**< Offset: 0xE8 (R/   32) Write Protection Status Register */
} tdes_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAM9X_TDES_COMPONENT_H_ */
