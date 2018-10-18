/**
 * \brief Header file for SAME/SAME70 RSTC
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
#ifndef SAME_SAME70_RSTC_MODULE_H
#define SAME_SAME70_RSTC_MODULE_H

/** \addtogroup SAME_SAME70 Reset Controller
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_RSTC                                  */
/* ========================================================================== */

/* -------- RSTC_CR : (RSTC Offset: 0x00) ( /W 32) Control Register -------- */
#define RSTC_CR_PROCRST_Pos                   (0U)                                           /**< (RSTC_CR) Processor Reset Position */
#define RSTC_CR_PROCRST_Msk                   (0x1U << RSTC_CR_PROCRST_Pos)                  /**< (RSTC_CR) Processor Reset Mask */
#define RSTC_CR_PROCRST(value)                (RSTC_CR_PROCRST_Msk & ((value) << RSTC_CR_PROCRST_Pos))
#define RSTC_CR_EXTRST_Pos                    (3U)                                           /**< (RSTC_CR) External Reset Position */
#define RSTC_CR_EXTRST_Msk                    (0x1U << RSTC_CR_EXTRST_Pos)                   /**< (RSTC_CR) External Reset Mask */
#define RSTC_CR_EXTRST(value)                 (RSTC_CR_EXTRST_Msk & ((value) << RSTC_CR_EXTRST_Pos))
#define RSTC_CR_KEY_Pos                       (24U)                                          /**< (RSTC_CR) System Reset Key Position */
#define RSTC_CR_KEY_Msk                       (0xFFU << RSTC_CR_KEY_Pos)                     /**< (RSTC_CR) System Reset Key Mask */
#define RSTC_CR_KEY(value)                    (RSTC_CR_KEY_Msk & ((value) << RSTC_CR_KEY_Pos))
#define   RSTC_CR_KEY_PASSWD_Val              (165U)                                         /**< (RSTC_CR) Writing any other value in this field aborts the write operation.  */
#define RSTC_CR_KEY_PASSWD                    (RSTC_CR_KEY_PASSWD_Val << RSTC_CR_KEY_Pos)    /**< (RSTC_CR) Writing any other value in this field aborts the write operation. Position  */
#define RSTC_CR_Msk                           (0xFF000009UL)                                 /**< (RSTC_CR) Register Mask  */

/* -------- RSTC_SR : (RSTC Offset: 0x04) (R/  32) Status Register -------- */
#define RSTC_SR_URSTS_Pos                     (0U)                                           /**< (RSTC_SR) User Reset Status Position */
#define RSTC_SR_URSTS_Msk                     (0x1U << RSTC_SR_URSTS_Pos)                    /**< (RSTC_SR) User Reset Status Mask */
#define RSTC_SR_URSTS(value)                  (RSTC_SR_URSTS_Msk & ((value) << RSTC_SR_URSTS_Pos))
#define RSTC_SR_RSTTYP_Pos                    (8U)                                           /**< (RSTC_SR) Reset Type Position */
#define RSTC_SR_RSTTYP_Msk                    (0x7U << RSTC_SR_RSTTYP_Pos)                   /**< (RSTC_SR) Reset Type Mask */
#define RSTC_SR_RSTTYP(value)                 (RSTC_SR_RSTTYP_Msk & ((value) << RSTC_SR_RSTTYP_Pos))
#define   RSTC_SR_RSTTYP_GENERAL_RST_Val      (0U)                                           /**< (RSTC_SR) First power-up reset  */
#define   RSTC_SR_RSTTYP_BACKUP_RST_Val       (1U)                                           /**< (RSTC_SR) Return from Backup Mode  */
#define   RSTC_SR_RSTTYP_WDT_RST_Val          (2U)                                           /**< (RSTC_SR) Watchdog fault occurred  */
#define   RSTC_SR_RSTTYP_SOFT_RST_Val         (3U)                                           /**< (RSTC_SR) Processor reset required by the software  */
#define   RSTC_SR_RSTTYP_USER_RST_Val         (4U)                                           /**< (RSTC_SR) NRST pin detected low  */
#define RSTC_SR_RSTTYP_GENERAL_RST            (RSTC_SR_RSTTYP_GENERAL_RST_Val << RSTC_SR_RSTTYP_Pos) /**< (RSTC_SR) First power-up reset Position  */
#define RSTC_SR_RSTTYP_BACKUP_RST             (RSTC_SR_RSTTYP_BACKUP_RST_Val << RSTC_SR_RSTTYP_Pos) /**< (RSTC_SR) Return from Backup Mode Position  */
#define RSTC_SR_RSTTYP_WDT_RST                (RSTC_SR_RSTTYP_WDT_RST_Val << RSTC_SR_RSTTYP_Pos) /**< (RSTC_SR) Watchdog fault occurred Position  */
#define RSTC_SR_RSTTYP_SOFT_RST               (RSTC_SR_RSTTYP_SOFT_RST_Val << RSTC_SR_RSTTYP_Pos) /**< (RSTC_SR) Processor reset required by the software Position  */
#define RSTC_SR_RSTTYP_USER_RST               (RSTC_SR_RSTTYP_USER_RST_Val << RSTC_SR_RSTTYP_Pos) /**< (RSTC_SR) NRST pin detected low Position  */
#define RSTC_SR_NRSTL_Pos                     (16U)                                          /**< (RSTC_SR) NRST Pin Level Position */
#define RSTC_SR_NRSTL_Msk                     (0x1U << RSTC_SR_NRSTL_Pos)                    /**< (RSTC_SR) NRST Pin Level Mask */
#define RSTC_SR_NRSTL(value)                  (RSTC_SR_NRSTL_Msk & ((value) << RSTC_SR_NRSTL_Pos))
#define RSTC_SR_SRCMP_Pos                     (17U)                                          /**< (RSTC_SR) Software Reset Command in Progress Position */
#define RSTC_SR_SRCMP_Msk                     (0x1U << RSTC_SR_SRCMP_Pos)                    /**< (RSTC_SR) Software Reset Command in Progress Mask */
#define RSTC_SR_SRCMP(value)                  (RSTC_SR_SRCMP_Msk & ((value) << RSTC_SR_SRCMP_Pos))
#define RSTC_SR_Msk                           (0x00030701UL)                                 /**< (RSTC_SR) Register Mask  */

/* -------- RSTC_MR : (RSTC Offset: 0x08) (R/W 32) Mode Register -------- */
#define RSTC_MR_URSTEN_Pos                    (0U)                                           /**< (RSTC_MR) User Reset Enable Position */
#define RSTC_MR_URSTEN_Msk                    (0x1U << RSTC_MR_URSTEN_Pos)                   /**< (RSTC_MR) User Reset Enable Mask */
#define RSTC_MR_URSTEN(value)                 (RSTC_MR_URSTEN_Msk & ((value) << RSTC_MR_URSTEN_Pos))
#define RSTC_MR_URSTIEN_Pos                   (4U)                                           /**< (RSTC_MR) User Reset Interrupt Enable Position */
#define RSTC_MR_URSTIEN_Msk                   (0x1U << RSTC_MR_URSTIEN_Pos)                  /**< (RSTC_MR) User Reset Interrupt Enable Mask */
#define RSTC_MR_URSTIEN(value)                (RSTC_MR_URSTIEN_Msk & ((value) << RSTC_MR_URSTIEN_Pos))
#define RSTC_MR_ERSTL_Pos                     (8U)                                           /**< (RSTC_MR) External Reset Length Position */
#define RSTC_MR_ERSTL_Msk                     (0xFU << RSTC_MR_ERSTL_Pos)                    /**< (RSTC_MR) External Reset Length Mask */
#define RSTC_MR_ERSTL(value)                  (RSTC_MR_ERSTL_Msk & ((value) << RSTC_MR_ERSTL_Pos))
#define RSTC_MR_KEY_Pos                       (24U)                                          /**< (RSTC_MR) Write Access Password Position */
#define RSTC_MR_KEY_Msk                       (0xFFU << RSTC_MR_KEY_Pos)                     /**< (RSTC_MR) Write Access Password Mask */
#define RSTC_MR_KEY(value)                    (RSTC_MR_KEY_Msk & ((value) << RSTC_MR_KEY_Pos))
#define   RSTC_MR_KEY_PASSWD_Val              (165U)                                         /**< (RSTC_MR) Writing any other value in this field aborts the write operation.Always reads as 0.  */
#define RSTC_MR_KEY_PASSWD                    (RSTC_MR_KEY_PASSWD_Val << RSTC_MR_KEY_Pos)    /**< (RSTC_MR) Writing any other value in this field aborts the write operation.Always reads as 0. Position  */
#define RSTC_MR_Msk                           (0xFF000F11UL)                                 /**< (RSTC_MR) Register Mask  */

/** \brief RSTC register offsets definitions */
#define RSTC_CR_OFFSET                 (0x00)         /**< (RSTC_CR) Control Register Offset */
#define RSTC_SR_OFFSET                 (0x04)         /**< (RSTC_SR) Status Register Offset */
#define RSTC_MR_OFFSET                 (0x08)         /**< (RSTC_MR) Mode Register Offset */

/** \brief RSTC register API structure */
typedef struct
{
  __O   uint32_t                       RSTC_CR;         /**< Offset: 0x00 ( /W  32) Control Register */
  __I   uint32_t                       RSTC_SR;         /**< Offset: 0x04 (R/   32) Status Register */
  __IO  uint32_t                       RSTC_MR;         /**< Offset: 0x08 (R/W  32) Mode Register */
} rstc_registers_t;
/** @}  end of Reset Controller */

#endif /* SAME_SAME70_RSTC_MODULE_H */

