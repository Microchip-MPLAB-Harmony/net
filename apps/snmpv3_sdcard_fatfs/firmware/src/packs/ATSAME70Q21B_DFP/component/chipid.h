/**
 * \brief Header file for SAME/SAME70 CHIPID
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
#ifndef SAME_SAME70_CHIPID_MODULE_H
#define SAME_SAME70_CHIPID_MODULE_H

/** \addtogroup SAME_SAME70 Chip Identifier
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_CHIPID                                */
/* ========================================================================== */

/* -------- CHIPID_CIDR : (CHIPID Offset: 0x00) (R/  32) Chip ID Register -------- */
#define CHIPID_CIDR_VERSION_Pos               (0U)                                           /**< (CHIPID_CIDR) Version of the Device Position */
#define CHIPID_CIDR_VERSION_Msk               (0x1FU << CHIPID_CIDR_VERSION_Pos)             /**< (CHIPID_CIDR) Version of the Device Mask */
#define CHIPID_CIDR_VERSION(value)            (CHIPID_CIDR_VERSION_Msk & ((value) << CHIPID_CIDR_VERSION_Pos))
#define CHIPID_CIDR_EPROC_Pos                 (5U)                                           /**< (CHIPID_CIDR) Embedded Processor Position */
#define CHIPID_CIDR_EPROC_Msk                 (0x7U << CHIPID_CIDR_EPROC_Pos)                /**< (CHIPID_CIDR) Embedded Processor Mask */
#define CHIPID_CIDR_EPROC(value)              (CHIPID_CIDR_EPROC_Msk & ((value) << CHIPID_CIDR_EPROC_Pos))
#define   CHIPID_CIDR_EPROC_SAMx7_Val         (0U)                                           /**< (CHIPID_CIDR) Cortex-M7  */
#define   CHIPID_CIDR_EPROC_ARM946ES_Val      (1U)                                           /**< (CHIPID_CIDR) ARM946ES  */
#define   CHIPID_CIDR_EPROC_ARM7TDMI_Val      (2U)                                           /**< (CHIPID_CIDR) ARM7TDMI  */
#define   CHIPID_CIDR_EPROC_CM3_Val           (3U)                                           /**< (CHIPID_CIDR) Cortex-M3  */
#define   CHIPID_CIDR_EPROC_ARM920T_Val       (4U)                                           /**< (CHIPID_CIDR) ARM920T  */
#define   CHIPID_CIDR_EPROC_ARM926EJS_Val     (5U)                                           /**< (CHIPID_CIDR) ARM926EJS  */
#define   CHIPID_CIDR_EPROC_CA5_Val           (6U)                                           /**< (CHIPID_CIDR) Cortex-A5  */
#define   CHIPID_CIDR_EPROC_CM4_Val           (7U)                                           /**< (CHIPID_CIDR) Cortex-M4  */
#define CHIPID_CIDR_EPROC_SAMx7               (CHIPID_CIDR_EPROC_SAMx7_Val << CHIPID_CIDR_EPROC_Pos) /**< (CHIPID_CIDR) Cortex-M7 Position  */
#define CHIPID_CIDR_EPROC_ARM946ES            (CHIPID_CIDR_EPROC_ARM946ES_Val << CHIPID_CIDR_EPROC_Pos) /**< (CHIPID_CIDR) ARM946ES Position  */
#define CHIPID_CIDR_EPROC_ARM7TDMI            (CHIPID_CIDR_EPROC_ARM7TDMI_Val << CHIPID_CIDR_EPROC_Pos) /**< (CHIPID_CIDR) ARM7TDMI Position  */
#define CHIPID_CIDR_EPROC_CM3                 (CHIPID_CIDR_EPROC_CM3_Val << CHIPID_CIDR_EPROC_Pos) /**< (CHIPID_CIDR) Cortex-M3 Position  */
#define CHIPID_CIDR_EPROC_ARM920T             (CHIPID_CIDR_EPROC_ARM920T_Val << CHIPID_CIDR_EPROC_Pos) /**< (CHIPID_CIDR) ARM920T Position  */
#define CHIPID_CIDR_EPROC_ARM926EJS           (CHIPID_CIDR_EPROC_ARM926EJS_Val << CHIPID_CIDR_EPROC_Pos) /**< (CHIPID_CIDR) ARM926EJS Position  */
#define CHIPID_CIDR_EPROC_CA5                 (CHIPID_CIDR_EPROC_CA5_Val << CHIPID_CIDR_EPROC_Pos) /**< (CHIPID_CIDR) Cortex-A5 Position  */
#define CHIPID_CIDR_EPROC_CM4                 (CHIPID_CIDR_EPROC_CM4_Val << CHIPID_CIDR_EPROC_Pos) /**< (CHIPID_CIDR) Cortex-M4 Position  */
#define CHIPID_CIDR_NVPSIZ_Pos                (8U)                                           /**< (CHIPID_CIDR) Nonvolatile Program Memory Size Position */
#define CHIPID_CIDR_NVPSIZ_Msk                (0xFU << CHIPID_CIDR_NVPSIZ_Pos)               /**< (CHIPID_CIDR) Nonvolatile Program Memory Size Mask */
#define CHIPID_CIDR_NVPSIZ(value)             (CHIPID_CIDR_NVPSIZ_Msk & ((value) << CHIPID_CIDR_NVPSIZ_Pos))
#define   CHIPID_CIDR_NVPSIZ_NONE_Val         (0U)                                           /**< (CHIPID_CIDR) None  */
#define   CHIPID_CIDR_NVPSIZ_8K_Val           (1U)                                           /**< (CHIPID_CIDR) 8 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_16K_Val          (2U)                                           /**< (CHIPID_CIDR) 16 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_32K_Val          (3U)                                           /**< (CHIPID_CIDR) 32 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_64K_Val          (5U)                                           /**< (CHIPID_CIDR) 64 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_128K_Val         (7U)                                           /**< (CHIPID_CIDR) 128 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_160K_Val         (8U)                                           /**< (CHIPID_CIDR) 160 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_256K_Val         (9U)                                           /**< (CHIPID_CIDR) 256 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_512K_Val         (10U)                                          /**< (CHIPID_CIDR) 512 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_1024K_Val        (12U)                                          /**< (CHIPID_CIDR) 1024 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ_2048K_Val        (14U)                                          /**< (CHIPID_CIDR) 2048 Kbytes  */
#define CHIPID_CIDR_NVPSIZ_NONE               (CHIPID_CIDR_NVPSIZ_NONE_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) None Position  */
#define CHIPID_CIDR_NVPSIZ_8K                 (CHIPID_CIDR_NVPSIZ_8K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 8 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_16K                (CHIPID_CIDR_NVPSIZ_16K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 16 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_32K                (CHIPID_CIDR_NVPSIZ_32K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 32 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_64K                (CHIPID_CIDR_NVPSIZ_64K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 64 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_128K               (CHIPID_CIDR_NVPSIZ_128K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 128 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_160K               (CHIPID_CIDR_NVPSIZ_160K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 160 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_256K               (CHIPID_CIDR_NVPSIZ_256K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 256 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_512K               (CHIPID_CIDR_NVPSIZ_512K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 512 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_1024K              (CHIPID_CIDR_NVPSIZ_1024K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 1024 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ_2048K              (CHIPID_CIDR_NVPSIZ_2048K_Val << CHIPID_CIDR_NVPSIZ_Pos) /**< (CHIPID_CIDR) 2048 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_Pos               (12U)                                          /**< (CHIPID_CIDR) Second Nonvolatile Program Memory Size Position */
#define CHIPID_CIDR_NVPSIZ2_Msk               (0xFU << CHIPID_CIDR_NVPSIZ2_Pos)              /**< (CHIPID_CIDR) Second Nonvolatile Program Memory Size Mask */
#define CHIPID_CIDR_NVPSIZ2(value)            (CHIPID_CIDR_NVPSIZ2_Msk & ((value) << CHIPID_CIDR_NVPSIZ2_Pos))
#define   CHIPID_CIDR_NVPSIZ2_NONE_Val        (0U)                                           /**< (CHIPID_CIDR) None  */
#define   CHIPID_CIDR_NVPSIZ2_8K_Val          (1U)                                           /**< (CHIPID_CIDR) 8 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ2_16K_Val         (2U)                                           /**< (CHIPID_CIDR) 16 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ2_32K_Val         (3U)                                           /**< (CHIPID_CIDR) 32 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ2_64K_Val         (5U)                                           /**< (CHIPID_CIDR) 64 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ2_128K_Val        (7U)                                           /**< (CHIPID_CIDR) 128 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ2_256K_Val        (9U)                                           /**< (CHIPID_CIDR) 256 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ2_512K_Val        (10U)                                          /**< (CHIPID_CIDR) 512 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ2_1024K_Val       (12U)                                          /**< (CHIPID_CIDR) 1024 Kbytes  */
#define   CHIPID_CIDR_NVPSIZ2_2048K_Val       (14U)                                          /**< (CHIPID_CIDR) 2048 Kbytes  */
#define CHIPID_CIDR_NVPSIZ2_NONE              (CHIPID_CIDR_NVPSIZ2_NONE_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) None Position  */
#define CHIPID_CIDR_NVPSIZ2_8K                (CHIPID_CIDR_NVPSIZ2_8K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 8 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_16K               (CHIPID_CIDR_NVPSIZ2_16K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 16 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_32K               (CHIPID_CIDR_NVPSIZ2_32K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 32 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_64K               (CHIPID_CIDR_NVPSIZ2_64K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 64 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_128K              (CHIPID_CIDR_NVPSIZ2_128K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 128 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_256K              (CHIPID_CIDR_NVPSIZ2_256K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 256 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_512K              (CHIPID_CIDR_NVPSIZ2_512K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 512 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_1024K             (CHIPID_CIDR_NVPSIZ2_1024K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 1024 Kbytes Position  */
#define CHIPID_CIDR_NVPSIZ2_2048K             (CHIPID_CIDR_NVPSIZ2_2048K_Val << CHIPID_CIDR_NVPSIZ2_Pos) /**< (CHIPID_CIDR) 2048 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_Pos               (16U)                                          /**< (CHIPID_CIDR) Internal SRAM Size Position */
#define CHIPID_CIDR_SRAMSIZ_Msk               (0xFU << CHIPID_CIDR_SRAMSIZ_Pos)              /**< (CHIPID_CIDR) Internal SRAM Size Mask */
#define CHIPID_CIDR_SRAMSIZ(value)            (CHIPID_CIDR_SRAMSIZ_Msk & ((value) << CHIPID_CIDR_SRAMSIZ_Pos))
#define   CHIPID_CIDR_SRAMSIZ_48K_Val         (0U)                                           /**< (CHIPID_CIDR) 48 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_192K_Val        (1U)                                           /**< (CHIPID_CIDR) 192 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_384K_Val        (2U)                                           /**< (CHIPID_CIDR) 384 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_6K_Val          (3U)                                           /**< (CHIPID_CIDR) 6 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_24K_Val         (4U)                                           /**< (CHIPID_CIDR) 24 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_4K_Val          (5U)                                           /**< (CHIPID_CIDR) 4 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_80K_Val         (6U)                                           /**< (CHIPID_CIDR) 80 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_160K_Val        (7U)                                           /**< (CHIPID_CIDR) 160 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_8K_Val          (8U)                                           /**< (CHIPID_CIDR) 8 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_16K_Val         (9U)                                           /**< (CHIPID_CIDR) 16 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_32K_Val         (10U)                                          /**< (CHIPID_CIDR) 32 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_64K_Val         (11U)                                          /**< (CHIPID_CIDR) 64 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_128K_Val        (12U)                                          /**< (CHIPID_CIDR) 128 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_256K_Val        (13U)                                          /**< (CHIPID_CIDR) 256 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_96K_Val         (14U)                                          /**< (CHIPID_CIDR) 96 Kbytes  */
#define   CHIPID_CIDR_SRAMSIZ_512K_Val        (15U)                                          /**< (CHIPID_CIDR) 512 Kbytes  */
#define CHIPID_CIDR_SRAMSIZ_48K               (CHIPID_CIDR_SRAMSIZ_48K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 48 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_192K              (CHIPID_CIDR_SRAMSIZ_192K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 192 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_384K              (CHIPID_CIDR_SRAMSIZ_384K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 384 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_6K                (CHIPID_CIDR_SRAMSIZ_6K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 6 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_24K               (CHIPID_CIDR_SRAMSIZ_24K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 24 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_4K                (CHIPID_CIDR_SRAMSIZ_4K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 4 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_80K               (CHIPID_CIDR_SRAMSIZ_80K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 80 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_160K              (CHIPID_CIDR_SRAMSIZ_160K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 160 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_8K                (CHIPID_CIDR_SRAMSIZ_8K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 8 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_16K               (CHIPID_CIDR_SRAMSIZ_16K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 16 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_32K               (CHIPID_CIDR_SRAMSIZ_32K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 32 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_64K               (CHIPID_CIDR_SRAMSIZ_64K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 64 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_128K              (CHIPID_CIDR_SRAMSIZ_128K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 128 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_256K              (CHIPID_CIDR_SRAMSIZ_256K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 256 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_96K               (CHIPID_CIDR_SRAMSIZ_96K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 96 Kbytes Position  */
#define CHIPID_CIDR_SRAMSIZ_512K              (CHIPID_CIDR_SRAMSIZ_512K_Val << CHIPID_CIDR_SRAMSIZ_Pos) /**< (CHIPID_CIDR) 512 Kbytes Position  */
#define CHIPID_CIDR_ARCH_Pos                  (20U)                                          /**< (CHIPID_CIDR) Architecture Identifier Position */
#define CHIPID_CIDR_ARCH_Msk                  (0xFFU << CHIPID_CIDR_ARCH_Pos)                /**< (CHIPID_CIDR) Architecture Identifier Mask */
#define CHIPID_CIDR_ARCH(value)               (CHIPID_CIDR_ARCH_Msk & ((value) << CHIPID_CIDR_ARCH_Pos))
#define   CHIPID_CIDR_ARCH_SAME70_Val         (16U)                                          /**< (CHIPID_CIDR) SAM E70  */
#define   CHIPID_CIDR_ARCH_SAMS70_Val         (17U)                                          /**< (CHIPID_CIDR) SAM S70  */
#define   CHIPID_CIDR_ARCH_SAMV71_Val         (18U)                                          /**< (CHIPID_CIDR) SAM V71  */
#define   CHIPID_CIDR_ARCH_SAMV70_Val         (19U)                                          /**< (CHIPID_CIDR) SAM V70  */
#define CHIPID_CIDR_ARCH_SAME70               (CHIPID_CIDR_ARCH_SAME70_Val << CHIPID_CIDR_ARCH_Pos) /**< (CHIPID_CIDR) SAM E70 Position  */
#define CHIPID_CIDR_ARCH_SAMS70               (CHIPID_CIDR_ARCH_SAMS70_Val << CHIPID_CIDR_ARCH_Pos) /**< (CHIPID_CIDR) SAM S70 Position  */
#define CHIPID_CIDR_ARCH_SAMV71               (CHIPID_CIDR_ARCH_SAMV71_Val << CHIPID_CIDR_ARCH_Pos) /**< (CHIPID_CIDR) SAM V71 Position  */
#define CHIPID_CIDR_ARCH_SAMV70               (CHIPID_CIDR_ARCH_SAMV70_Val << CHIPID_CIDR_ARCH_Pos) /**< (CHIPID_CIDR) SAM V70 Position  */
#define CHIPID_CIDR_NVPTYP_Pos                (28U)                                          /**< (CHIPID_CIDR) Nonvolatile Program Memory Type Position */
#define CHIPID_CIDR_NVPTYP_Msk                (0x7U << CHIPID_CIDR_NVPTYP_Pos)               /**< (CHIPID_CIDR) Nonvolatile Program Memory Type Mask */
#define CHIPID_CIDR_NVPTYP(value)             (CHIPID_CIDR_NVPTYP_Msk & ((value) << CHIPID_CIDR_NVPTYP_Pos))
#define   CHIPID_CIDR_NVPTYP_ROM_Val          (0U)                                           /**< (CHIPID_CIDR) ROM  */
#define   CHIPID_CIDR_NVPTYP_ROMLESS_Val      (1U)                                           /**< (CHIPID_CIDR) ROMless or on-chip Flash  */
#define   CHIPID_CIDR_NVPTYP_FLASH_Val        (2U)                                           /**< (CHIPID_CIDR) Embedded Flash Memory  */
#define   CHIPID_CIDR_NVPTYP_ROM_FLASH_Val    (3U)                                           /**< (CHIPID_CIDR) ROM and Embedded Flash Memory- NVPSIZ is ROM size- NVPSIZ2 is Flash size  */
#define   CHIPID_CIDR_NVPTYP_SRAM_Val         (4U)                                           /**< (CHIPID_CIDR) SRAM emulating ROM  */
#define CHIPID_CIDR_NVPTYP_ROM                (CHIPID_CIDR_NVPTYP_ROM_Val << CHIPID_CIDR_NVPTYP_Pos) /**< (CHIPID_CIDR) ROM Position  */
#define CHIPID_CIDR_NVPTYP_ROMLESS            (CHIPID_CIDR_NVPTYP_ROMLESS_Val << CHIPID_CIDR_NVPTYP_Pos) /**< (CHIPID_CIDR) ROMless or on-chip Flash Position  */
#define CHIPID_CIDR_NVPTYP_FLASH              (CHIPID_CIDR_NVPTYP_FLASH_Val << CHIPID_CIDR_NVPTYP_Pos) /**< (CHIPID_CIDR) Embedded Flash Memory Position  */
#define CHIPID_CIDR_NVPTYP_ROM_FLASH          (CHIPID_CIDR_NVPTYP_ROM_FLASH_Val << CHIPID_CIDR_NVPTYP_Pos) /**< (CHIPID_CIDR) ROM and Embedded Flash Memory- NVPSIZ is ROM size- NVPSIZ2 is Flash size Position  */
#define CHIPID_CIDR_NVPTYP_SRAM               (CHIPID_CIDR_NVPTYP_SRAM_Val << CHIPID_CIDR_NVPTYP_Pos) /**< (CHIPID_CIDR) SRAM emulating ROM Position  */
#define CHIPID_CIDR_EXT_Pos                   (31U)                                          /**< (CHIPID_CIDR) Extension Flag Position */
#define CHIPID_CIDR_EXT_Msk                   (0x1U << CHIPID_CIDR_EXT_Pos)                  /**< (CHIPID_CIDR) Extension Flag Mask */
#define CHIPID_CIDR_EXT(value)                (CHIPID_CIDR_EXT_Msk & ((value) << CHIPID_CIDR_EXT_Pos))
#define CHIPID_CIDR_Msk                       (0xFFFFFFFFUL)                                 /**< (CHIPID_CIDR) Register Mask  */

/* -------- CHIPID_EXID : (CHIPID Offset: 0x04) (R/  32) Chip ID Extension Register -------- */
#define CHIPID_EXID_EXID_Pos                  (0U)                                           /**< (CHIPID_EXID) Chip ID Extension Position */
#define CHIPID_EXID_EXID_Msk                  (0xFFFFFFFFU << CHIPID_EXID_EXID_Pos)          /**< (CHIPID_EXID) Chip ID Extension Mask */
#define CHIPID_EXID_EXID(value)               (CHIPID_EXID_EXID_Msk & ((value) << CHIPID_EXID_EXID_Pos))
#define CHIPID_EXID_Msk                       (0xFFFFFFFFUL)                                 /**< (CHIPID_EXID) Register Mask  */

/** \brief CHIPID register offsets definitions */
#define CHIPID_CIDR_OFFSET             (0x00)         /**< (CHIPID_CIDR) Chip ID Register Offset */
#define CHIPID_EXID_OFFSET             (0x04)         /**< (CHIPID_EXID) Chip ID Extension Register Offset */

/** \brief CHIPID register API structure */
typedef struct
{
  __I   uint32_t                       CHIPID_CIDR;     /**< Offset: 0x00 (R/   32) Chip ID Register */
  __I   uint32_t                       CHIPID_EXID;     /**< Offset: 0x04 (R/   32) Chip ID Extension Register */
} chipid_registers_t;
/** @}  end of Chip Identifier */

#endif /* SAME_SAME70_CHIPID_MODULE_H */

