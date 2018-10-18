/**
 * \brief Header file for SAME/SAME70 EFC
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
#ifndef SAME_SAME70_EFC_MODULE_H
#define SAME_SAME70_EFC_MODULE_H

/** \addtogroup SAME_SAME70 Embedded Flash Controller
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_EFC                                   */
/* ========================================================================== */

/* -------- EEFC_FMR : (EFC Offset: 0x00) (R/W 32) EEFC Flash Mode Register -------- */
#define EEFC_FMR_FRDY_Pos                     (0U)                                           /**< (EEFC_FMR) Flash Ready Interrupt Enable Position */
#define EEFC_FMR_FRDY_Msk                     (0x1U << EEFC_FMR_FRDY_Pos)                    /**< (EEFC_FMR) Flash Ready Interrupt Enable Mask */
#define EEFC_FMR_FRDY(value)                  (EEFC_FMR_FRDY_Msk & ((value) << EEFC_FMR_FRDY_Pos))
#define EEFC_FMR_FWS_Pos                      (8U)                                           /**< (EEFC_FMR) Flash Wait State Position */
#define EEFC_FMR_FWS_Msk                      (0xFU << EEFC_FMR_FWS_Pos)                     /**< (EEFC_FMR) Flash Wait State Mask */
#define EEFC_FMR_FWS(value)                   (EEFC_FMR_FWS_Msk & ((value) << EEFC_FMR_FWS_Pos))
#define EEFC_FMR_SCOD_Pos                     (16U)                                          /**< (EEFC_FMR) Sequential Code Optimization Disable Position */
#define EEFC_FMR_SCOD_Msk                     (0x1U << EEFC_FMR_SCOD_Pos)                    /**< (EEFC_FMR) Sequential Code Optimization Disable Mask */
#define EEFC_FMR_SCOD(value)                  (EEFC_FMR_SCOD_Msk & ((value) << EEFC_FMR_SCOD_Pos))
#define EEFC_FMR_CLOE_Pos                     (26U)                                          /**< (EEFC_FMR) Code Loop Optimization Enable Position */
#define EEFC_FMR_CLOE_Msk                     (0x1U << EEFC_FMR_CLOE_Pos)                    /**< (EEFC_FMR) Code Loop Optimization Enable Mask */
#define EEFC_FMR_CLOE(value)                  (EEFC_FMR_CLOE_Msk & ((value) << EEFC_FMR_CLOE_Pos))
#define EEFC_FMR_Msk                          (0x04010F01UL)                                 /**< (EEFC_FMR) Register Mask  */

/* -------- EEFC_FCR : (EFC Offset: 0x04) ( /W 32) EEFC Flash Command Register -------- */
#define EEFC_FCR_FCMD_Pos                     (0U)                                           /**< (EEFC_FCR) Flash Command Position */
#define EEFC_FCR_FCMD_Msk                     (0xFFU << EEFC_FCR_FCMD_Pos)                   /**< (EEFC_FCR) Flash Command Mask */
#define EEFC_FCR_FCMD(value)                  (EEFC_FCR_FCMD_Msk & ((value) << EEFC_FCR_FCMD_Pos))
#define   EEFC_FCR_FCMD_GETD_Val              (0U)                                           /**< (EEFC_FCR) Get Flash descriptor  */
#define   EEFC_FCR_FCMD_WP_Val                (1U)                                           /**< (EEFC_FCR) Write page  */
#define   EEFC_FCR_FCMD_WPL_Val               (2U)                                           /**< (EEFC_FCR) Write page and lock  */
#define   EEFC_FCR_FCMD_EWP_Val               (3U)                                           /**< (EEFC_FCR) Erase page and write page  */
#define   EEFC_FCR_FCMD_EWPL_Val              (4U)                                           /**< (EEFC_FCR) Erase page and write page then lock  */
#define   EEFC_FCR_FCMD_EA_Val                (5U)                                           /**< (EEFC_FCR) Erase all  */
#define   EEFC_FCR_FCMD_EPA_Val               (7U)                                           /**< (EEFC_FCR) Erase pages  */
#define   EEFC_FCR_FCMD_SLB_Val               (8U)                                           /**< (EEFC_FCR) Set lock bit  */
#define   EEFC_FCR_FCMD_CLB_Val               (9U)                                           /**< (EEFC_FCR) Clear lock bit  */
#define   EEFC_FCR_FCMD_GLB_Val               (10U)                                          /**< (EEFC_FCR) Get lock bit  */
#define   EEFC_FCR_FCMD_SGPB_Val              (11U)                                          /**< (EEFC_FCR) Set GPNVM bit  */
#define   EEFC_FCR_FCMD_CGPB_Val              (12U)                                          /**< (EEFC_FCR) Clear GPNVM bit  */
#define   EEFC_FCR_FCMD_GGPB_Val              (13U)                                          /**< (EEFC_FCR) Get GPNVM bit  */
#define   EEFC_FCR_FCMD_STUI_Val              (14U)                                          /**< (EEFC_FCR) Start read unique identifier  */
#define   EEFC_FCR_FCMD_SPUI_Val              (15U)                                          /**< (EEFC_FCR) Stop read unique identifier  */
#define   EEFC_FCR_FCMD_GCALB_Val             (16U)                                          /**< (EEFC_FCR) Get CALIB bit  */
#define   EEFC_FCR_FCMD_ES_Val                (17U)                                          /**< (EEFC_FCR) Erase sector  */
#define   EEFC_FCR_FCMD_WUS_Val               (18U)                                          /**< (EEFC_FCR) Write user signature  */
#define   EEFC_FCR_FCMD_EUS_Val               (19U)                                          /**< (EEFC_FCR) Erase user signature  */
#define   EEFC_FCR_FCMD_STUS_Val              (20U)                                          /**< (EEFC_FCR) Start read user signature  */
#define   EEFC_FCR_FCMD_SPUS_Val              (21U)                                          /**< (EEFC_FCR) Stop read user signature  */
#define EEFC_FCR_FCMD_GETD                    (EEFC_FCR_FCMD_GETD_Val << EEFC_FCR_FCMD_Pos)  /**< (EEFC_FCR) Get Flash descriptor Position  */
#define EEFC_FCR_FCMD_WP                      (EEFC_FCR_FCMD_WP_Val << EEFC_FCR_FCMD_Pos)    /**< (EEFC_FCR) Write page Position  */
#define EEFC_FCR_FCMD_WPL                     (EEFC_FCR_FCMD_WPL_Val << EEFC_FCR_FCMD_Pos)   /**< (EEFC_FCR) Write page and lock Position  */
#define EEFC_FCR_FCMD_EWP                     (EEFC_FCR_FCMD_EWP_Val << EEFC_FCR_FCMD_Pos)   /**< (EEFC_FCR) Erase page and write page Position  */
#define EEFC_FCR_FCMD_EWPL                    (EEFC_FCR_FCMD_EWPL_Val << EEFC_FCR_FCMD_Pos)  /**< (EEFC_FCR) Erase page and write page then lock Position  */
#define EEFC_FCR_FCMD_EA                      (EEFC_FCR_FCMD_EA_Val << EEFC_FCR_FCMD_Pos)    /**< (EEFC_FCR) Erase all Position  */
#define EEFC_FCR_FCMD_EPA                     (EEFC_FCR_FCMD_EPA_Val << EEFC_FCR_FCMD_Pos)   /**< (EEFC_FCR) Erase pages Position  */
#define EEFC_FCR_FCMD_SLB                     (EEFC_FCR_FCMD_SLB_Val << EEFC_FCR_FCMD_Pos)   /**< (EEFC_FCR) Set lock bit Position  */
#define EEFC_FCR_FCMD_CLB                     (EEFC_FCR_FCMD_CLB_Val << EEFC_FCR_FCMD_Pos)   /**< (EEFC_FCR) Clear lock bit Position  */
#define EEFC_FCR_FCMD_GLB                     (EEFC_FCR_FCMD_GLB_Val << EEFC_FCR_FCMD_Pos)   /**< (EEFC_FCR) Get lock bit Position  */
#define EEFC_FCR_FCMD_SGPB                    (EEFC_FCR_FCMD_SGPB_Val << EEFC_FCR_FCMD_Pos)  /**< (EEFC_FCR) Set GPNVM bit Position  */
#define EEFC_FCR_FCMD_CGPB                    (EEFC_FCR_FCMD_CGPB_Val << EEFC_FCR_FCMD_Pos)  /**< (EEFC_FCR) Clear GPNVM bit Position  */
#define EEFC_FCR_FCMD_GGPB                    (EEFC_FCR_FCMD_GGPB_Val << EEFC_FCR_FCMD_Pos)  /**< (EEFC_FCR) Get GPNVM bit Position  */
#define EEFC_FCR_FCMD_STUI                    (EEFC_FCR_FCMD_STUI_Val << EEFC_FCR_FCMD_Pos)  /**< (EEFC_FCR) Start read unique identifier Position  */
#define EEFC_FCR_FCMD_SPUI                    (EEFC_FCR_FCMD_SPUI_Val << EEFC_FCR_FCMD_Pos)  /**< (EEFC_FCR) Stop read unique identifier Position  */
#define EEFC_FCR_FCMD_GCALB                   (EEFC_FCR_FCMD_GCALB_Val << EEFC_FCR_FCMD_Pos) /**< (EEFC_FCR) Get CALIB bit Position  */
#define EEFC_FCR_FCMD_ES                      (EEFC_FCR_FCMD_ES_Val << EEFC_FCR_FCMD_Pos)    /**< (EEFC_FCR) Erase sector Position  */
#define EEFC_FCR_FCMD_WUS                     (EEFC_FCR_FCMD_WUS_Val << EEFC_FCR_FCMD_Pos)   /**< (EEFC_FCR) Write user signature Position  */
#define EEFC_FCR_FCMD_EUS                     (EEFC_FCR_FCMD_EUS_Val << EEFC_FCR_FCMD_Pos)   /**< (EEFC_FCR) Erase user signature Position  */
#define EEFC_FCR_FCMD_STUS                    (EEFC_FCR_FCMD_STUS_Val << EEFC_FCR_FCMD_Pos)  /**< (EEFC_FCR) Start read user signature Position  */
#define EEFC_FCR_FCMD_SPUS                    (EEFC_FCR_FCMD_SPUS_Val << EEFC_FCR_FCMD_Pos)  /**< (EEFC_FCR) Stop read user signature Position  */
#define EEFC_FCR_FARG_Pos                     (8U)                                           /**< (EEFC_FCR) Flash Command Argument Position */
#define EEFC_FCR_FARG_Msk                     (0xFFFFU << EEFC_FCR_FARG_Pos)                 /**< (EEFC_FCR) Flash Command Argument Mask */
#define EEFC_FCR_FARG(value)                  (EEFC_FCR_FARG_Msk & ((value) << EEFC_FCR_FARG_Pos))
#define EEFC_FCR_FKEY_Pos                     (24U)                                          /**< (EEFC_FCR) Flash Writing Protection Key Position */
#define EEFC_FCR_FKEY_Msk                     (0xFFU << EEFC_FCR_FKEY_Pos)                   /**< (EEFC_FCR) Flash Writing Protection Key Mask */
#define EEFC_FCR_FKEY(value)                  (EEFC_FCR_FKEY_Msk & ((value) << EEFC_FCR_FKEY_Pos))
#define   EEFC_FCR_FKEY_PASSWD_Val            (90U)                                          /**< (EEFC_FCR) The 0x5A value enables the command defined by the bits of the register. If the field is written with a different value, the write is not performed and no action is started.  */
#define EEFC_FCR_FKEY_PASSWD                  (EEFC_FCR_FKEY_PASSWD_Val << EEFC_FCR_FKEY_Pos) /**< (EEFC_FCR) The 0x5A value enables the command defined by the bits of the register. If the field is written with a different value, the write is not performed and no action is started. Position  */
#define EEFC_FCR_Msk                          (0xFFFFFFFFUL)                                 /**< (EEFC_FCR) Register Mask  */

/* -------- EEFC_FSR : (EFC Offset: 0x08) (R/  32) EEFC Flash Status Register -------- */
#define EEFC_FSR_FRDY_Pos                     (0U)                                           /**< (EEFC_FSR) Flash Ready Status (cleared when Flash is busy) Position */
#define EEFC_FSR_FRDY_Msk                     (0x1U << EEFC_FSR_FRDY_Pos)                    /**< (EEFC_FSR) Flash Ready Status (cleared when Flash is busy) Mask */
#define EEFC_FSR_FRDY(value)                  (EEFC_FSR_FRDY_Msk & ((value) << EEFC_FSR_FRDY_Pos))
#define EEFC_FSR_FCMDE_Pos                    (1U)                                           /**< (EEFC_FSR) Flash Command Error Status (cleared on read or by writing EEFC_FCR) Position */
#define EEFC_FSR_FCMDE_Msk                    (0x1U << EEFC_FSR_FCMDE_Pos)                   /**< (EEFC_FSR) Flash Command Error Status (cleared on read or by writing EEFC_FCR) Mask */
#define EEFC_FSR_FCMDE(value)                 (EEFC_FSR_FCMDE_Msk & ((value) << EEFC_FSR_FCMDE_Pos))
#define EEFC_FSR_FLOCKE_Pos                   (2U)                                           /**< (EEFC_FSR) Flash Lock Error Status (cleared on read) Position */
#define EEFC_FSR_FLOCKE_Msk                   (0x1U << EEFC_FSR_FLOCKE_Pos)                  /**< (EEFC_FSR) Flash Lock Error Status (cleared on read) Mask */
#define EEFC_FSR_FLOCKE(value)                (EEFC_FSR_FLOCKE_Msk & ((value) << EEFC_FSR_FLOCKE_Pos))
#define EEFC_FSR_FLERR_Pos                    (3U)                                           /**< (EEFC_FSR) Flash Error Status (cleared when a programming operation starts) Position */
#define EEFC_FSR_FLERR_Msk                    (0x1U << EEFC_FSR_FLERR_Pos)                   /**< (EEFC_FSR) Flash Error Status (cleared when a programming operation starts) Mask */
#define EEFC_FSR_FLERR(value)                 (EEFC_FSR_FLERR_Msk & ((value) << EEFC_FSR_FLERR_Pos))
#define EEFC_FSR_UECCELSB_Pos                 (16U)                                          /**< (EEFC_FSR) Unique ECC Error on LSB Part of the Memory Flash Data Bus (cleared on read) Position */
#define EEFC_FSR_UECCELSB_Msk                 (0x1U << EEFC_FSR_UECCELSB_Pos)                /**< (EEFC_FSR) Unique ECC Error on LSB Part of the Memory Flash Data Bus (cleared on read) Mask */
#define EEFC_FSR_UECCELSB(value)              (EEFC_FSR_UECCELSB_Msk & ((value) << EEFC_FSR_UECCELSB_Pos))
#define EEFC_FSR_MECCELSB_Pos                 (17U)                                          /**< (EEFC_FSR) Multiple ECC Error on LSB Part of the Memory Flash Data Bus (cleared on read) Position */
#define EEFC_FSR_MECCELSB_Msk                 (0x1U << EEFC_FSR_MECCELSB_Pos)                /**< (EEFC_FSR) Multiple ECC Error on LSB Part of the Memory Flash Data Bus (cleared on read) Mask */
#define EEFC_FSR_MECCELSB(value)              (EEFC_FSR_MECCELSB_Msk & ((value) << EEFC_FSR_MECCELSB_Pos))
#define EEFC_FSR_UECCEMSB_Pos                 (18U)                                          /**< (EEFC_FSR) Unique ECC Error on MSB Part of the Memory Flash Data Bus (cleared on read) Position */
#define EEFC_FSR_UECCEMSB_Msk                 (0x1U << EEFC_FSR_UECCEMSB_Pos)                /**< (EEFC_FSR) Unique ECC Error on MSB Part of the Memory Flash Data Bus (cleared on read) Mask */
#define EEFC_FSR_UECCEMSB(value)              (EEFC_FSR_UECCEMSB_Msk & ((value) << EEFC_FSR_UECCEMSB_Pos))
#define EEFC_FSR_MECCEMSB_Pos                 (19U)                                          /**< (EEFC_FSR) Multiple ECC Error on MSB Part of the Memory Flash Data Bus (cleared on read) Position */
#define EEFC_FSR_MECCEMSB_Msk                 (0x1U << EEFC_FSR_MECCEMSB_Pos)                /**< (EEFC_FSR) Multiple ECC Error on MSB Part of the Memory Flash Data Bus (cleared on read) Mask */
#define EEFC_FSR_MECCEMSB(value)              (EEFC_FSR_MECCEMSB_Msk & ((value) << EEFC_FSR_MECCEMSB_Pos))
#define EEFC_FSR_Msk                          (0x000F000FUL)                                 /**< (EEFC_FSR) Register Mask  */

/* -------- EEFC_FRR : (EFC Offset: 0x0C) (R/  32) EEFC Flash Result Register -------- */
#define EEFC_FRR_FVALUE_Pos                   (0U)                                           /**< (EEFC_FRR) Flash Result Value Position */
#define EEFC_FRR_FVALUE_Msk                   (0xFFFFFFFFU << EEFC_FRR_FVALUE_Pos)           /**< (EEFC_FRR) Flash Result Value Mask */
#define EEFC_FRR_FVALUE(value)                (EEFC_FRR_FVALUE_Msk & ((value) << EEFC_FRR_FVALUE_Pos))
#define EEFC_FRR_Msk                          (0xFFFFFFFFUL)                                 /**< (EEFC_FRR) Register Mask  */

/* -------- EEFC_WPMR : (EFC Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define EEFC_WPMR_WPEN_Pos                    (0U)                                           /**< (EEFC_WPMR) Write Protection Enable Position */
#define EEFC_WPMR_WPEN_Msk                    (0x1U << EEFC_WPMR_WPEN_Pos)                   /**< (EEFC_WPMR) Write Protection Enable Mask */
#define EEFC_WPMR_WPEN(value)                 (EEFC_WPMR_WPEN_Msk & ((value) << EEFC_WPMR_WPEN_Pos))
#define EEFC_WPMR_WPKEY_Pos                   (8U)                                           /**< (EEFC_WPMR) Write Protection Key Position */
#define EEFC_WPMR_WPKEY_Msk                   (0xFFFFFFU << EEFC_WPMR_WPKEY_Pos)             /**< (EEFC_WPMR) Write Protection Key Mask */
#define EEFC_WPMR_WPKEY(value)                (EEFC_WPMR_WPKEY_Msk & ((value) << EEFC_WPMR_WPKEY_Pos))
#define   EEFC_WPMR_WPKEY_PASSWD_Val          (4539971U)                                     /**< (EEFC_WPMR) Writing any other value in this field aborts the write operation.Always reads as 0.  */
#define EEFC_WPMR_WPKEY_PASSWD                (EEFC_WPMR_WPKEY_PASSWD_Val << EEFC_WPMR_WPKEY_Pos) /**< (EEFC_WPMR) Writing any other value in this field aborts the write operation.Always reads as 0. Position  */
#define EEFC_WPMR_Msk                         (0xFFFFFF01UL)                                 /**< (EEFC_WPMR) Register Mask  */

/** \brief EFC register offsets definitions */
#define EEFC_FMR_OFFSET                (0x00)         /**< (EEFC_FMR) EEFC Flash Mode Register Offset */
#define EEFC_FCR_OFFSET                (0x04)         /**< (EEFC_FCR) EEFC Flash Command Register Offset */
#define EEFC_FSR_OFFSET                (0x08)         /**< (EEFC_FSR) EEFC Flash Status Register Offset */
#define EEFC_FRR_OFFSET                (0x0C)         /**< (EEFC_FRR) EEFC Flash Result Register Offset */
#define EEFC_WPMR_OFFSET               (0xE4)         /**< (EEFC_WPMR) Write Protection Mode Register Offset */

/** \brief EFC register API structure */
typedef struct
{
  __IO  uint32_t                       EEFC_FMR;        /**< Offset: 0x00 (R/W  32) EEFC Flash Mode Register */
  __O   uint32_t                       EEFC_FCR;        /**< Offset: 0x04 ( /W  32) EEFC Flash Command Register */
  __I   uint32_t                       EEFC_FSR;        /**< Offset: 0x08 (R/   32) EEFC Flash Status Register */
  __I   uint32_t                       EEFC_FRR;        /**< Offset: 0x0c (R/   32) EEFC Flash Result Register */
  __I   uint8_t                        Reserved1[0xD4];
  __IO  uint32_t                       EEFC_WPMR;       /**< Offset: 0xe4 (R/W  32) Write Protection Mode Register */
} efc_registers_t;
/** @}  end of Embedded Flash Controller */

#endif /* SAME_SAME70_EFC_MODULE_H */

