/**
 * \brief Header file for SAME/SAME70 ISI
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
#ifndef SAME_SAME70_ISI_MODULE_H
#define SAME_SAME70_ISI_MODULE_H

/** \addtogroup SAME_SAME70 Image Sensor Interface
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_ISI                                   */
/* ========================================================================== */

/* -------- ISI_CFG1 : (ISI Offset: 0x00) (R/W 32) ISI Configuration 1 Register -------- */
#define ISI_CFG1_HSYNC_POL_Pos                (2U)                                           /**< (ISI_CFG1) Horizontal Synchronization Polarity Position */
#define ISI_CFG1_HSYNC_POL_Msk                (0x1U << ISI_CFG1_HSYNC_POL_Pos)               /**< (ISI_CFG1) Horizontal Synchronization Polarity Mask */
#define ISI_CFG1_HSYNC_POL(value)             (ISI_CFG1_HSYNC_POL_Msk & ((value) << ISI_CFG1_HSYNC_POL_Pos))
#define ISI_CFG1_VSYNC_POL_Pos                (3U)                                           /**< (ISI_CFG1) Vertical Synchronization Polarity Position */
#define ISI_CFG1_VSYNC_POL_Msk                (0x1U << ISI_CFG1_VSYNC_POL_Pos)               /**< (ISI_CFG1) Vertical Synchronization Polarity Mask */
#define ISI_CFG1_VSYNC_POL(value)             (ISI_CFG1_VSYNC_POL_Msk & ((value) << ISI_CFG1_VSYNC_POL_Pos))
#define ISI_CFG1_PIXCLK_POL_Pos               (4U)                                           /**< (ISI_CFG1) Pixel Clock Polarity Position */
#define ISI_CFG1_PIXCLK_POL_Msk               (0x1U << ISI_CFG1_PIXCLK_POL_Pos)              /**< (ISI_CFG1) Pixel Clock Polarity Mask */
#define ISI_CFG1_PIXCLK_POL(value)            (ISI_CFG1_PIXCLK_POL_Msk & ((value) << ISI_CFG1_PIXCLK_POL_Pos))
#define ISI_CFG1_GRAYLE_Pos                   (5U)                                           /**< (ISI_CFG1) Grayscale Little Endian Position */
#define ISI_CFG1_GRAYLE_Msk                   (0x1U << ISI_CFG1_GRAYLE_Pos)                  /**< (ISI_CFG1) Grayscale Little Endian Mask */
#define ISI_CFG1_GRAYLE(value)                (ISI_CFG1_GRAYLE_Msk & ((value) << ISI_CFG1_GRAYLE_Pos))
#define ISI_CFG1_EMB_SYNC_Pos                 (6U)                                           /**< (ISI_CFG1) Embedded Synchronization Position */
#define ISI_CFG1_EMB_SYNC_Msk                 (0x1U << ISI_CFG1_EMB_SYNC_Pos)                /**< (ISI_CFG1) Embedded Synchronization Mask */
#define ISI_CFG1_EMB_SYNC(value)              (ISI_CFG1_EMB_SYNC_Msk & ((value) << ISI_CFG1_EMB_SYNC_Pos))
#define ISI_CFG1_CRC_SYNC_Pos                 (7U)                                           /**< (ISI_CFG1) Embedded Synchronization Correction Position */
#define ISI_CFG1_CRC_SYNC_Msk                 (0x1U << ISI_CFG1_CRC_SYNC_Pos)                /**< (ISI_CFG1) Embedded Synchronization Correction Mask */
#define ISI_CFG1_CRC_SYNC(value)              (ISI_CFG1_CRC_SYNC_Msk & ((value) << ISI_CFG1_CRC_SYNC_Pos))
#define ISI_CFG1_FRATE_Pos                    (8U)                                           /**< (ISI_CFG1) Frame Rate [0..7] Position */
#define ISI_CFG1_FRATE_Msk                    (0x7U << ISI_CFG1_FRATE_Pos)                   /**< (ISI_CFG1) Frame Rate [0..7] Mask */
#define ISI_CFG1_FRATE(value)                 (ISI_CFG1_FRATE_Msk & ((value) << ISI_CFG1_FRATE_Pos))
#define ISI_CFG1_DISCR_Pos                    (11U)                                          /**< (ISI_CFG1) Disable Codec Request Position */
#define ISI_CFG1_DISCR_Msk                    (0x1U << ISI_CFG1_DISCR_Pos)                   /**< (ISI_CFG1) Disable Codec Request Mask */
#define ISI_CFG1_DISCR(value)                 (ISI_CFG1_DISCR_Msk & ((value) << ISI_CFG1_DISCR_Pos))
#define ISI_CFG1_FULL_Pos                     (12U)                                          /**< (ISI_CFG1) Full Mode is Allowed Position */
#define ISI_CFG1_FULL_Msk                     (0x1U << ISI_CFG1_FULL_Pos)                    /**< (ISI_CFG1) Full Mode is Allowed Mask */
#define ISI_CFG1_FULL(value)                  (ISI_CFG1_FULL_Msk & ((value) << ISI_CFG1_FULL_Pos))
#define ISI_CFG1_THMASK_Pos                   (13U)                                          /**< (ISI_CFG1) Threshold Mask Position */
#define ISI_CFG1_THMASK_Msk                   (0x3U << ISI_CFG1_THMASK_Pos)                  /**< (ISI_CFG1) Threshold Mask Mask */
#define ISI_CFG1_THMASK(value)                (ISI_CFG1_THMASK_Msk & ((value) << ISI_CFG1_THMASK_Pos))
#define   ISI_CFG1_THMASK_BEATS_4_Val         (0U)                                           /**< (ISI_CFG1) Only 4 beats AHB burst allowed  */
#define   ISI_CFG1_THMASK_BEATS_8_Val         (1U)                                           /**< (ISI_CFG1) Only 4 and 8 beats AHB burst allowed  */
#define   ISI_CFG1_THMASK_BEATS_16_Val        (2U)                                           /**< (ISI_CFG1) 4, 8 and 16 beats AHB burst allowed  */
#define ISI_CFG1_THMASK_BEATS_4               (ISI_CFG1_THMASK_BEATS_4_Val << ISI_CFG1_THMASK_Pos) /**< (ISI_CFG1) Only 4 beats AHB burst allowed Position  */
#define ISI_CFG1_THMASK_BEATS_8               (ISI_CFG1_THMASK_BEATS_8_Val << ISI_CFG1_THMASK_Pos) /**< (ISI_CFG1) Only 4 and 8 beats AHB burst allowed Position  */
#define ISI_CFG1_THMASK_BEATS_16              (ISI_CFG1_THMASK_BEATS_16_Val << ISI_CFG1_THMASK_Pos) /**< (ISI_CFG1) 4, 8 and 16 beats AHB burst allowed Position  */
#define ISI_CFG1_SLD_Pos                      (16U)                                          /**< (ISI_CFG1) Start of Line Delay Position */
#define ISI_CFG1_SLD_Msk                      (0xFFU << ISI_CFG1_SLD_Pos)                    /**< (ISI_CFG1) Start of Line Delay Mask */
#define ISI_CFG1_SLD(value)                   (ISI_CFG1_SLD_Msk & ((value) << ISI_CFG1_SLD_Pos))
#define ISI_CFG1_SFD_Pos                      (24U)                                          /**< (ISI_CFG1) Start of Frame Delay Position */
#define ISI_CFG1_SFD_Msk                      (0xFFU << ISI_CFG1_SFD_Pos)                    /**< (ISI_CFG1) Start of Frame Delay Mask */
#define ISI_CFG1_SFD(value)                   (ISI_CFG1_SFD_Msk & ((value) << ISI_CFG1_SFD_Pos))
#define ISI_CFG1_Msk                          (0xFFFF7FFCUL)                                 /**< (ISI_CFG1) Register Mask  */

/* -------- ISI_CFG2 : (ISI Offset: 0x04) (R/W 32) ISI Configuration 2 Register -------- */
#define ISI_CFG2_IM_VSIZE_Pos                 (0U)                                           /**< (ISI_CFG2) Vertical Size of the Image Sensor [0..2047] Position */
#define ISI_CFG2_IM_VSIZE_Msk                 (0x7FFU << ISI_CFG2_IM_VSIZE_Pos)              /**< (ISI_CFG2) Vertical Size of the Image Sensor [0..2047] Mask */
#define ISI_CFG2_IM_VSIZE(value)              (ISI_CFG2_IM_VSIZE_Msk & ((value) << ISI_CFG2_IM_VSIZE_Pos))
#define ISI_CFG2_GS_MODE_Pos                  (11U)                                          /**< (ISI_CFG2) Grayscale Pixel Format Mode Position */
#define ISI_CFG2_GS_MODE_Msk                  (0x1U << ISI_CFG2_GS_MODE_Pos)                 /**< (ISI_CFG2) Grayscale Pixel Format Mode Mask */
#define ISI_CFG2_GS_MODE(value)               (ISI_CFG2_GS_MODE_Msk & ((value) << ISI_CFG2_GS_MODE_Pos))
#define ISI_CFG2_RGB_MODE_Pos                 (12U)                                          /**< (ISI_CFG2) RGB Input Mode Position */
#define ISI_CFG2_RGB_MODE_Msk                 (0x1U << ISI_CFG2_RGB_MODE_Pos)                /**< (ISI_CFG2) RGB Input Mode Mask */
#define ISI_CFG2_RGB_MODE(value)              (ISI_CFG2_RGB_MODE_Msk & ((value) << ISI_CFG2_RGB_MODE_Pos))
#define ISI_CFG2_GRAYSCALE_Pos                (13U)                                          /**< (ISI_CFG2) Grayscale Mode Format Enable Position */
#define ISI_CFG2_GRAYSCALE_Msk                (0x1U << ISI_CFG2_GRAYSCALE_Pos)               /**< (ISI_CFG2) Grayscale Mode Format Enable Mask */
#define ISI_CFG2_GRAYSCALE(value)             (ISI_CFG2_GRAYSCALE_Msk & ((value) << ISI_CFG2_GRAYSCALE_Pos))
#define ISI_CFG2_RGB_SWAP_Pos                 (14U)                                          /**< (ISI_CFG2) RGB Format Swap Mode Position */
#define ISI_CFG2_RGB_SWAP_Msk                 (0x1U << ISI_CFG2_RGB_SWAP_Pos)                /**< (ISI_CFG2) RGB Format Swap Mode Mask */
#define ISI_CFG2_RGB_SWAP(value)              (ISI_CFG2_RGB_SWAP_Msk & ((value) << ISI_CFG2_RGB_SWAP_Pos))
#define ISI_CFG2_COL_SPACE_Pos                (15U)                                          /**< (ISI_CFG2) Color Space for the Image Data Position */
#define ISI_CFG2_COL_SPACE_Msk                (0x1U << ISI_CFG2_COL_SPACE_Pos)               /**< (ISI_CFG2) Color Space for the Image Data Mask */
#define ISI_CFG2_COL_SPACE(value)             (ISI_CFG2_COL_SPACE_Msk & ((value) << ISI_CFG2_COL_SPACE_Pos))
#define ISI_CFG2_IM_HSIZE_Pos                 (16U)                                          /**< (ISI_CFG2) Horizontal Size of the Image Sensor [0..2047] Position */
#define ISI_CFG2_IM_HSIZE_Msk                 (0x7FFU << ISI_CFG2_IM_HSIZE_Pos)              /**< (ISI_CFG2) Horizontal Size of the Image Sensor [0..2047] Mask */
#define ISI_CFG2_IM_HSIZE(value)              (ISI_CFG2_IM_HSIZE_Msk & ((value) << ISI_CFG2_IM_HSIZE_Pos))
#define ISI_CFG2_YCC_SWAP_Pos                 (28U)                                          /**< (ISI_CFG2) YCrCb Format Swap Mode Position */
#define ISI_CFG2_YCC_SWAP_Msk                 (0x3U << ISI_CFG2_YCC_SWAP_Pos)                /**< (ISI_CFG2) YCrCb Format Swap Mode Mask */
#define ISI_CFG2_YCC_SWAP(value)              (ISI_CFG2_YCC_SWAP_Msk & ((value) << ISI_CFG2_YCC_SWAP_Pos))
#define   ISI_CFG2_YCC_SWAP_DEFAULT_Val       (0U)                                           /**< (ISI_CFG2) Byte 0 Cb(i)Byte 1 Y(i)Byte 2 Cr(i)Byte 3 Y(i+1)  */
#define   ISI_CFG2_YCC_SWAP_MODE1_Val         (1U)                                           /**< (ISI_CFG2) Byte 0 Cr(i)Byte 1 Y(i)Byte 2 Cb(i)Byte 3 Y(i+1)  */
#define   ISI_CFG2_YCC_SWAP_MODE2_Val         (2U)                                           /**< (ISI_CFG2) Byte 0 Y(i)Byte 1 Cb(i)Byte 2 Y(i+1)Byte 3 Cr(i)  */
#define   ISI_CFG2_YCC_SWAP_MODE3_Val         (3U)                                           /**< (ISI_CFG2) Byte 0 Y(i)Byte 1 Cr(i)Byte 2 Y(i+1)Byte 3 Cb(i)  */
#define ISI_CFG2_YCC_SWAP_DEFAULT             (ISI_CFG2_YCC_SWAP_DEFAULT_Val << ISI_CFG2_YCC_SWAP_Pos) /**< (ISI_CFG2) Byte 0 Cb(i)Byte 1 Y(i)Byte 2 Cr(i)Byte 3 Y(i+1) Position  */
#define ISI_CFG2_YCC_SWAP_MODE1               (ISI_CFG2_YCC_SWAP_MODE1_Val << ISI_CFG2_YCC_SWAP_Pos) /**< (ISI_CFG2) Byte 0 Cr(i)Byte 1 Y(i)Byte 2 Cb(i)Byte 3 Y(i+1) Position  */
#define ISI_CFG2_YCC_SWAP_MODE2               (ISI_CFG2_YCC_SWAP_MODE2_Val << ISI_CFG2_YCC_SWAP_Pos) /**< (ISI_CFG2) Byte 0 Y(i)Byte 1 Cb(i)Byte 2 Y(i+1)Byte 3 Cr(i) Position  */
#define ISI_CFG2_YCC_SWAP_MODE3               (ISI_CFG2_YCC_SWAP_MODE3_Val << ISI_CFG2_YCC_SWAP_Pos) /**< (ISI_CFG2) Byte 0 Y(i)Byte 1 Cr(i)Byte 2 Y(i+1)Byte 3 Cb(i) Position  */
#define ISI_CFG2_RGB_CFG_Pos                  (30U)                                          /**< (ISI_CFG2) RGB Pixel Mapping Configuration Position */
#define ISI_CFG2_RGB_CFG_Msk                  (0x3U << ISI_CFG2_RGB_CFG_Pos)                 /**< (ISI_CFG2) RGB Pixel Mapping Configuration Mask */
#define ISI_CFG2_RGB_CFG(value)               (ISI_CFG2_RGB_CFG_Msk & ((value) << ISI_CFG2_RGB_CFG_Pos))
#define   ISI_CFG2_RGB_CFG_DEFAULT_Val        (0U)                                           /**< (ISI_CFG2) Byte 0 R/G(MSB)Byte 1 G(LSB)/BByte 2 R/G(MSB)Byte 3 G(LSB)/B  */
#define   ISI_CFG2_RGB_CFG_MODE1_Val          (1U)                                           /**< (ISI_CFG2) Byte 0 B/G(MSB)Byte 1 G(LSB)/RByte 2 B/G(MSB)Byte 3 G(LSB)/R  */
#define   ISI_CFG2_RGB_CFG_MODE2_Val          (2U)                                           /**< (ISI_CFG2) Byte 0 G(LSB)/RByte 1 B/G(MSB)Byte 2 G(LSB)/RByte 3 B/G(MSB)  */
#define   ISI_CFG2_RGB_CFG_MODE3_Val          (3U)                                           /**< (ISI_CFG2) Byte 0 G(LSB)/BByte 1 R/G(MSB)Byte 2 G(LSB)/BByte 3 R/G(MSB)  */
#define ISI_CFG2_RGB_CFG_DEFAULT              (ISI_CFG2_RGB_CFG_DEFAULT_Val << ISI_CFG2_RGB_CFG_Pos) /**< (ISI_CFG2) Byte 0 R/G(MSB)Byte 1 G(LSB)/BByte 2 R/G(MSB)Byte 3 G(LSB)/B Position  */
#define ISI_CFG2_RGB_CFG_MODE1                (ISI_CFG2_RGB_CFG_MODE1_Val << ISI_CFG2_RGB_CFG_Pos) /**< (ISI_CFG2) Byte 0 B/G(MSB)Byte 1 G(LSB)/RByte 2 B/G(MSB)Byte 3 G(LSB)/R Position  */
#define ISI_CFG2_RGB_CFG_MODE2                (ISI_CFG2_RGB_CFG_MODE2_Val << ISI_CFG2_RGB_CFG_Pos) /**< (ISI_CFG2) Byte 0 G(LSB)/RByte 1 B/G(MSB)Byte 2 G(LSB)/RByte 3 B/G(MSB) Position  */
#define ISI_CFG2_RGB_CFG_MODE3                (ISI_CFG2_RGB_CFG_MODE3_Val << ISI_CFG2_RGB_CFG_Pos) /**< (ISI_CFG2) Byte 0 G(LSB)/BByte 1 R/G(MSB)Byte 2 G(LSB)/BByte 3 R/G(MSB) Position  */
#define ISI_CFG2_Msk                          (0xF7FFFFFFUL)                                 /**< (ISI_CFG2) Register Mask  */

/* -------- ISI_PSIZE : (ISI Offset: 0x08) (R/W 32) ISI Preview Size Register -------- */
#define ISI_PSIZE_PREV_VSIZE_Pos              (0U)                                           /**< (ISI_PSIZE) Vertical Size for the Preview Path Position */
#define ISI_PSIZE_PREV_VSIZE_Msk              (0x3FFU << ISI_PSIZE_PREV_VSIZE_Pos)           /**< (ISI_PSIZE) Vertical Size for the Preview Path Mask */
#define ISI_PSIZE_PREV_VSIZE(value)           (ISI_PSIZE_PREV_VSIZE_Msk & ((value) << ISI_PSIZE_PREV_VSIZE_Pos))
#define ISI_PSIZE_PREV_HSIZE_Pos              (16U)                                          /**< (ISI_PSIZE) Horizontal Size for the Preview Path Position */
#define ISI_PSIZE_PREV_HSIZE_Msk              (0x3FFU << ISI_PSIZE_PREV_HSIZE_Pos)           /**< (ISI_PSIZE) Horizontal Size for the Preview Path Mask */
#define ISI_PSIZE_PREV_HSIZE(value)           (ISI_PSIZE_PREV_HSIZE_Msk & ((value) << ISI_PSIZE_PREV_HSIZE_Pos))
#define ISI_PSIZE_Msk                         (0x03FF03FFUL)                                 /**< (ISI_PSIZE) Register Mask  */

/* -------- ISI_PDECF : (ISI Offset: 0x0C) (R/W 32) ISI Preview Decimation Factor Register -------- */
#define ISI_PDECF_DEC_FACTOR_Pos              (0U)                                           /**< (ISI_PDECF) Decimation Factor Position */
#define ISI_PDECF_DEC_FACTOR_Msk              (0xFFU << ISI_PDECF_DEC_FACTOR_Pos)            /**< (ISI_PDECF) Decimation Factor Mask */
#define ISI_PDECF_DEC_FACTOR(value)           (ISI_PDECF_DEC_FACTOR_Msk & ((value) << ISI_PDECF_DEC_FACTOR_Pos))
#define ISI_PDECF_Msk                         (0x000000FFUL)                                 /**< (ISI_PDECF) Register Mask  */

/* -------- ISI_Y2R_SET0 : (ISI Offset: 0x10) (R/W 32) ISI Color Space Conversion YCrCb To RGB Set 0 Register -------- */
#define ISI_Y2R_SET0_C0_Pos                   (0U)                                           /**< (ISI_Y2R_SET0) Color Space Conversion Matrix Coefficient C0 Position */
#define ISI_Y2R_SET0_C0_Msk                   (0xFFU << ISI_Y2R_SET0_C0_Pos)                 /**< (ISI_Y2R_SET0) Color Space Conversion Matrix Coefficient C0 Mask */
#define ISI_Y2R_SET0_C0(value)                (ISI_Y2R_SET0_C0_Msk & ((value) << ISI_Y2R_SET0_C0_Pos))
#define ISI_Y2R_SET0_C1_Pos                   (8U)                                           /**< (ISI_Y2R_SET0) Color Space Conversion Matrix Coefficient C1 Position */
#define ISI_Y2R_SET0_C1_Msk                   (0xFFU << ISI_Y2R_SET0_C1_Pos)                 /**< (ISI_Y2R_SET0) Color Space Conversion Matrix Coefficient C1 Mask */
#define ISI_Y2R_SET0_C1(value)                (ISI_Y2R_SET0_C1_Msk & ((value) << ISI_Y2R_SET0_C1_Pos))
#define ISI_Y2R_SET0_C2_Pos                   (16U)                                          /**< (ISI_Y2R_SET0) Color Space Conversion Matrix Coefficient C2 Position */
#define ISI_Y2R_SET0_C2_Msk                   (0xFFU << ISI_Y2R_SET0_C2_Pos)                 /**< (ISI_Y2R_SET0) Color Space Conversion Matrix Coefficient C2 Mask */
#define ISI_Y2R_SET0_C2(value)                (ISI_Y2R_SET0_C2_Msk & ((value) << ISI_Y2R_SET0_C2_Pos))
#define ISI_Y2R_SET0_C3_Pos                   (24U)                                          /**< (ISI_Y2R_SET0) Color Space Conversion Matrix Coefficient C3 Position */
#define ISI_Y2R_SET0_C3_Msk                   (0xFFU << ISI_Y2R_SET0_C3_Pos)                 /**< (ISI_Y2R_SET0) Color Space Conversion Matrix Coefficient C3 Mask */
#define ISI_Y2R_SET0_C3(value)                (ISI_Y2R_SET0_C3_Msk & ((value) << ISI_Y2R_SET0_C3_Pos))
#define ISI_Y2R_SET0_Msk                      (0xFFFFFFFFUL)                                 /**< (ISI_Y2R_SET0) Register Mask  */

/* -------- ISI_Y2R_SET1 : (ISI Offset: 0x14) (R/W 32) ISI Color Space Conversion YCrCb To RGB Set 1 Register -------- */
#define ISI_Y2R_SET1_C4_Pos                   (0U)                                           /**< (ISI_Y2R_SET1) Color Space Conversion Matrix Coefficient C4 Position */
#define ISI_Y2R_SET1_C4_Msk                   (0x1FFU << ISI_Y2R_SET1_C4_Pos)                /**< (ISI_Y2R_SET1) Color Space Conversion Matrix Coefficient C4 Mask */
#define ISI_Y2R_SET1_C4(value)                (ISI_Y2R_SET1_C4_Msk & ((value) << ISI_Y2R_SET1_C4_Pos))
#define ISI_Y2R_SET1_Yoff_Pos                 (12U)                                          /**< (ISI_Y2R_SET1) Color Space Conversion Luminance Default Offset Position */
#define ISI_Y2R_SET1_Yoff_Msk                 (0x1U << ISI_Y2R_SET1_Yoff_Pos)                /**< (ISI_Y2R_SET1) Color Space Conversion Luminance Default Offset Mask */
#define ISI_Y2R_SET1_Yoff(value)              (ISI_Y2R_SET1_Yoff_Msk & ((value) << ISI_Y2R_SET1_Yoff_Pos))
#define ISI_Y2R_SET1_Croff_Pos                (13U)                                          /**< (ISI_Y2R_SET1) Color Space Conversion Red Chrominance Default Offset Position */
#define ISI_Y2R_SET1_Croff_Msk                (0x1U << ISI_Y2R_SET1_Croff_Pos)               /**< (ISI_Y2R_SET1) Color Space Conversion Red Chrominance Default Offset Mask */
#define ISI_Y2R_SET1_Croff(value)             (ISI_Y2R_SET1_Croff_Msk & ((value) << ISI_Y2R_SET1_Croff_Pos))
#define ISI_Y2R_SET1_Cboff_Pos                (14U)                                          /**< (ISI_Y2R_SET1) Color Space Conversion Blue Chrominance Default Offset Position */
#define ISI_Y2R_SET1_Cboff_Msk                (0x1U << ISI_Y2R_SET1_Cboff_Pos)               /**< (ISI_Y2R_SET1) Color Space Conversion Blue Chrominance Default Offset Mask */
#define ISI_Y2R_SET1_Cboff(value)             (ISI_Y2R_SET1_Cboff_Msk & ((value) << ISI_Y2R_SET1_Cboff_Pos))
#define ISI_Y2R_SET1_Msk                      (0x000071FFUL)                                 /**< (ISI_Y2R_SET1) Register Mask  */

/* -------- ISI_R2Y_SET0 : (ISI Offset: 0x18) (R/W 32) ISI Color Space Conversion RGB To YCrCb Set 0 Register -------- */
#define ISI_R2Y_SET0_C0_Pos                   (0U)                                           /**< (ISI_R2Y_SET0) Color Space Conversion Matrix Coefficient C0 Position */
#define ISI_R2Y_SET0_C0_Msk                   (0x7FU << ISI_R2Y_SET0_C0_Pos)                 /**< (ISI_R2Y_SET0) Color Space Conversion Matrix Coefficient C0 Mask */
#define ISI_R2Y_SET0_C0(value)                (ISI_R2Y_SET0_C0_Msk & ((value) << ISI_R2Y_SET0_C0_Pos))
#define ISI_R2Y_SET0_C1_Pos                   (8U)                                           /**< (ISI_R2Y_SET0) Color Space Conversion Matrix Coefficient C1 Position */
#define ISI_R2Y_SET0_C1_Msk                   (0x7FU << ISI_R2Y_SET0_C1_Pos)                 /**< (ISI_R2Y_SET0) Color Space Conversion Matrix Coefficient C1 Mask */
#define ISI_R2Y_SET0_C1(value)                (ISI_R2Y_SET0_C1_Msk & ((value) << ISI_R2Y_SET0_C1_Pos))
#define ISI_R2Y_SET0_C2_Pos                   (16U)                                          /**< (ISI_R2Y_SET0) Color Space Conversion Matrix Coefficient C2 Position */
#define ISI_R2Y_SET0_C2_Msk                   (0x7FU << ISI_R2Y_SET0_C2_Pos)                 /**< (ISI_R2Y_SET0) Color Space Conversion Matrix Coefficient C2 Mask */
#define ISI_R2Y_SET0_C2(value)                (ISI_R2Y_SET0_C2_Msk & ((value) << ISI_R2Y_SET0_C2_Pos))
#define ISI_R2Y_SET0_Roff_Pos                 (24U)                                          /**< (ISI_R2Y_SET0) Color Space Conversion Red Component Offset Position */
#define ISI_R2Y_SET0_Roff_Msk                 (0x1U << ISI_R2Y_SET0_Roff_Pos)                /**< (ISI_R2Y_SET0) Color Space Conversion Red Component Offset Mask */
#define ISI_R2Y_SET0_Roff(value)              (ISI_R2Y_SET0_Roff_Msk & ((value) << ISI_R2Y_SET0_Roff_Pos))
#define ISI_R2Y_SET0_Msk                      (0x017F7F7FUL)                                 /**< (ISI_R2Y_SET0) Register Mask  */

/* -------- ISI_R2Y_SET1 : (ISI Offset: 0x1C) (R/W 32) ISI Color Space Conversion RGB To YCrCb Set 1 Register -------- */
#define ISI_R2Y_SET1_C3_Pos                   (0U)                                           /**< (ISI_R2Y_SET1) Color Space Conversion Matrix Coefficient C3 Position */
#define ISI_R2Y_SET1_C3_Msk                   (0x7FU << ISI_R2Y_SET1_C3_Pos)                 /**< (ISI_R2Y_SET1) Color Space Conversion Matrix Coefficient C3 Mask */
#define ISI_R2Y_SET1_C3(value)                (ISI_R2Y_SET1_C3_Msk & ((value) << ISI_R2Y_SET1_C3_Pos))
#define ISI_R2Y_SET1_C4_Pos                   (8U)                                           /**< (ISI_R2Y_SET1) Color Space Conversion Matrix Coefficient C4 Position */
#define ISI_R2Y_SET1_C4_Msk                   (0x7FU << ISI_R2Y_SET1_C4_Pos)                 /**< (ISI_R2Y_SET1) Color Space Conversion Matrix Coefficient C4 Mask */
#define ISI_R2Y_SET1_C4(value)                (ISI_R2Y_SET1_C4_Msk & ((value) << ISI_R2Y_SET1_C4_Pos))
#define ISI_R2Y_SET1_C5_Pos                   (16U)                                          /**< (ISI_R2Y_SET1) Color Space Conversion Matrix Coefficient C5 Position */
#define ISI_R2Y_SET1_C5_Msk                   (0x7FU << ISI_R2Y_SET1_C5_Pos)                 /**< (ISI_R2Y_SET1) Color Space Conversion Matrix Coefficient C5 Mask */
#define ISI_R2Y_SET1_C5(value)                (ISI_R2Y_SET1_C5_Msk & ((value) << ISI_R2Y_SET1_C5_Pos))
#define ISI_R2Y_SET1_Goff_Pos                 (24U)                                          /**< (ISI_R2Y_SET1) Color Space Conversion Green Component Offset Position */
#define ISI_R2Y_SET1_Goff_Msk                 (0x1U << ISI_R2Y_SET1_Goff_Pos)                /**< (ISI_R2Y_SET1) Color Space Conversion Green Component Offset Mask */
#define ISI_R2Y_SET1_Goff(value)              (ISI_R2Y_SET1_Goff_Msk & ((value) << ISI_R2Y_SET1_Goff_Pos))
#define ISI_R2Y_SET1_Msk                      (0x017F7F7FUL)                                 /**< (ISI_R2Y_SET1) Register Mask  */

/* -------- ISI_R2Y_SET2 : (ISI Offset: 0x20) (R/W 32) ISI Color Space Conversion RGB To YCrCb Set 2 Register -------- */
#define ISI_R2Y_SET2_C6_Pos                   (0U)                                           /**< (ISI_R2Y_SET2) Color Space Conversion Matrix Coefficient C6 Position */
#define ISI_R2Y_SET2_C6_Msk                   (0x7FU << ISI_R2Y_SET2_C6_Pos)                 /**< (ISI_R2Y_SET2) Color Space Conversion Matrix Coefficient C6 Mask */
#define ISI_R2Y_SET2_C6(value)                (ISI_R2Y_SET2_C6_Msk & ((value) << ISI_R2Y_SET2_C6_Pos))
#define ISI_R2Y_SET2_C7_Pos                   (8U)                                           /**< (ISI_R2Y_SET2) Color Space Conversion Matrix Coefficient C7 Position */
#define ISI_R2Y_SET2_C7_Msk                   (0x7FU << ISI_R2Y_SET2_C7_Pos)                 /**< (ISI_R2Y_SET2) Color Space Conversion Matrix Coefficient C7 Mask */
#define ISI_R2Y_SET2_C7(value)                (ISI_R2Y_SET2_C7_Msk & ((value) << ISI_R2Y_SET2_C7_Pos))
#define ISI_R2Y_SET2_C8_Pos                   (16U)                                          /**< (ISI_R2Y_SET2) Color Space Conversion Matrix Coefficient C8 Position */
#define ISI_R2Y_SET2_C8_Msk                   (0x7FU << ISI_R2Y_SET2_C8_Pos)                 /**< (ISI_R2Y_SET2) Color Space Conversion Matrix Coefficient C8 Mask */
#define ISI_R2Y_SET2_C8(value)                (ISI_R2Y_SET2_C8_Msk & ((value) << ISI_R2Y_SET2_C8_Pos))
#define ISI_R2Y_SET2_Boff_Pos                 (24U)                                          /**< (ISI_R2Y_SET2) Color Space Conversion Blue Component Offset Position */
#define ISI_R2Y_SET2_Boff_Msk                 (0x1U << ISI_R2Y_SET2_Boff_Pos)                /**< (ISI_R2Y_SET2) Color Space Conversion Blue Component Offset Mask */
#define ISI_R2Y_SET2_Boff(value)              (ISI_R2Y_SET2_Boff_Msk & ((value) << ISI_R2Y_SET2_Boff_Pos))
#define ISI_R2Y_SET2_Msk                      (0x017F7F7FUL)                                 /**< (ISI_R2Y_SET2) Register Mask  */

/* -------- ISI_CR : (ISI Offset: 0x24) ( /W 32) ISI Control Register -------- */
#define ISI_CR_ISI_EN_Pos                     (0U)                                           /**< (ISI_CR) ISI Module Enable Request Position */
#define ISI_CR_ISI_EN_Msk                     (0x1U << ISI_CR_ISI_EN_Pos)                    /**< (ISI_CR) ISI Module Enable Request Mask */
#define ISI_CR_ISI_EN(value)                  (ISI_CR_ISI_EN_Msk & ((value) << ISI_CR_ISI_EN_Pos))
#define ISI_CR_ISI_DIS_Pos                    (1U)                                           /**< (ISI_CR) ISI Module Disable Request Position */
#define ISI_CR_ISI_DIS_Msk                    (0x1U << ISI_CR_ISI_DIS_Pos)                   /**< (ISI_CR) ISI Module Disable Request Mask */
#define ISI_CR_ISI_DIS(value)                 (ISI_CR_ISI_DIS_Msk & ((value) << ISI_CR_ISI_DIS_Pos))
#define ISI_CR_ISI_SRST_Pos                   (2U)                                           /**< (ISI_CR) ISI Software Reset Request Position */
#define ISI_CR_ISI_SRST_Msk                   (0x1U << ISI_CR_ISI_SRST_Pos)                  /**< (ISI_CR) ISI Software Reset Request Mask */
#define ISI_CR_ISI_SRST(value)                (ISI_CR_ISI_SRST_Msk & ((value) << ISI_CR_ISI_SRST_Pos))
#define ISI_CR_ISI_CDC_Pos                    (8U)                                           /**< (ISI_CR) ISI Codec Request Position */
#define ISI_CR_ISI_CDC_Msk                    (0x1U << ISI_CR_ISI_CDC_Pos)                   /**< (ISI_CR) ISI Codec Request Mask */
#define ISI_CR_ISI_CDC(value)                 (ISI_CR_ISI_CDC_Msk & ((value) << ISI_CR_ISI_CDC_Pos))
#define ISI_CR_Msk                            (0x00000107UL)                                 /**< (ISI_CR) Register Mask  */

/* -------- ISI_SR : (ISI Offset: 0x28) (R/  32) ISI Status Register -------- */
#define ISI_SR_ENABLE_Pos                     (0U)                                           /**< (ISI_SR) Module Enable Position */
#define ISI_SR_ENABLE_Msk                     (0x1U << ISI_SR_ENABLE_Pos)                    /**< (ISI_SR) Module Enable Mask */
#define ISI_SR_ENABLE(value)                  (ISI_SR_ENABLE_Msk & ((value) << ISI_SR_ENABLE_Pos))
#define ISI_SR_DIS_DONE_Pos                   (1U)                                           /**< (ISI_SR) Module Disable Request has Terminated (cleared on read) Position */
#define ISI_SR_DIS_DONE_Msk                   (0x1U << ISI_SR_DIS_DONE_Pos)                  /**< (ISI_SR) Module Disable Request has Terminated (cleared on read) Mask */
#define ISI_SR_DIS_DONE(value)                (ISI_SR_DIS_DONE_Msk & ((value) << ISI_SR_DIS_DONE_Pos))
#define ISI_SR_SRST_Pos                       (2U)                                           /**< (ISI_SR) Module Software Reset Request has Terminated (cleared on read) Position */
#define ISI_SR_SRST_Msk                       (0x1U << ISI_SR_SRST_Pos)                      /**< (ISI_SR) Module Software Reset Request has Terminated (cleared on read) Mask */
#define ISI_SR_SRST(value)                    (ISI_SR_SRST_Msk & ((value) << ISI_SR_SRST_Pos))
#define ISI_SR_CDC_PND_Pos                    (8U)                                           /**< (ISI_SR) Pending Codec Request Position */
#define ISI_SR_CDC_PND_Msk                    (0x1U << ISI_SR_CDC_PND_Pos)                   /**< (ISI_SR) Pending Codec Request Mask */
#define ISI_SR_CDC_PND(value)                 (ISI_SR_CDC_PND_Msk & ((value) << ISI_SR_CDC_PND_Pos))
#define ISI_SR_VSYNC_Pos                      (10U)                                          /**< (ISI_SR) Vertical Synchronization (cleared on read) Position */
#define ISI_SR_VSYNC_Msk                      (0x1U << ISI_SR_VSYNC_Pos)                     /**< (ISI_SR) Vertical Synchronization (cleared on read) Mask */
#define ISI_SR_VSYNC(value)                   (ISI_SR_VSYNC_Msk & ((value) << ISI_SR_VSYNC_Pos))
#define ISI_SR_PXFR_DONE_Pos                  (16U)                                          /**< (ISI_SR) Preview DMA Transfer has Terminated (cleared on read) Position */
#define ISI_SR_PXFR_DONE_Msk                  (0x1U << ISI_SR_PXFR_DONE_Pos)                 /**< (ISI_SR) Preview DMA Transfer has Terminated (cleared on read) Mask */
#define ISI_SR_PXFR_DONE(value)               (ISI_SR_PXFR_DONE_Msk & ((value) << ISI_SR_PXFR_DONE_Pos))
#define ISI_SR_CXFR_DONE_Pos                  (17U)                                          /**< (ISI_SR) Codec DMA Transfer has Terminated (cleared on read) Position */
#define ISI_SR_CXFR_DONE_Msk                  (0x1U << ISI_SR_CXFR_DONE_Pos)                 /**< (ISI_SR) Codec DMA Transfer has Terminated (cleared on read) Mask */
#define ISI_SR_CXFR_DONE(value)               (ISI_SR_CXFR_DONE_Msk & ((value) << ISI_SR_CXFR_DONE_Pos))
#define ISI_SR_SIP_Pos                        (19U)                                          /**< (ISI_SR) Synchronization in Progress Position */
#define ISI_SR_SIP_Msk                        (0x1U << ISI_SR_SIP_Pos)                       /**< (ISI_SR) Synchronization in Progress Mask */
#define ISI_SR_SIP(value)                     (ISI_SR_SIP_Msk & ((value) << ISI_SR_SIP_Pos))
#define ISI_SR_P_OVR_Pos                      (24U)                                          /**< (ISI_SR) Preview Datapath Overflow (cleared on read) Position */
#define ISI_SR_P_OVR_Msk                      (0x1U << ISI_SR_P_OVR_Pos)                     /**< (ISI_SR) Preview Datapath Overflow (cleared on read) Mask */
#define ISI_SR_P_OVR(value)                   (ISI_SR_P_OVR_Msk & ((value) << ISI_SR_P_OVR_Pos))
#define ISI_SR_C_OVR_Pos                      (25U)                                          /**< (ISI_SR) Codec Datapath Overflow (cleared on read) Position */
#define ISI_SR_C_OVR_Msk                      (0x1U << ISI_SR_C_OVR_Pos)                     /**< (ISI_SR) Codec Datapath Overflow (cleared on read) Mask */
#define ISI_SR_C_OVR(value)                   (ISI_SR_C_OVR_Msk & ((value) << ISI_SR_C_OVR_Pos))
#define ISI_SR_CRC_ERR_Pos                    (26U)                                          /**< (ISI_SR) CRC Synchronization Error (cleared on read) Position */
#define ISI_SR_CRC_ERR_Msk                    (0x1U << ISI_SR_CRC_ERR_Pos)                   /**< (ISI_SR) CRC Synchronization Error (cleared on read) Mask */
#define ISI_SR_CRC_ERR(value)                 (ISI_SR_CRC_ERR_Msk & ((value) << ISI_SR_CRC_ERR_Pos))
#define ISI_SR_FR_OVR_Pos                     (27U)                                          /**< (ISI_SR) Frame Rate Overrun (cleared on read) Position */
#define ISI_SR_FR_OVR_Msk                     (0x1U << ISI_SR_FR_OVR_Pos)                    /**< (ISI_SR) Frame Rate Overrun (cleared on read) Mask */
#define ISI_SR_FR_OVR(value)                  (ISI_SR_FR_OVR_Msk & ((value) << ISI_SR_FR_OVR_Pos))
#define ISI_SR_Msk                            (0x0F0B0507UL)                                 /**< (ISI_SR) Register Mask  */

/* -------- ISI_IER : (ISI Offset: 0x2C) ( /W 32) ISI Interrupt Enable Register -------- */
#define ISI_IER_DIS_DONE_Pos                  (1U)                                           /**< (ISI_IER) Disable Done Interrupt Enable Position */
#define ISI_IER_DIS_DONE_Msk                  (0x1U << ISI_IER_DIS_DONE_Pos)                 /**< (ISI_IER) Disable Done Interrupt Enable Mask */
#define ISI_IER_DIS_DONE(value)               (ISI_IER_DIS_DONE_Msk & ((value) << ISI_IER_DIS_DONE_Pos))
#define ISI_IER_SRST_Pos                      (2U)                                           /**< (ISI_IER) Software Reset Interrupt Enable Position */
#define ISI_IER_SRST_Msk                      (0x1U << ISI_IER_SRST_Pos)                     /**< (ISI_IER) Software Reset Interrupt Enable Mask */
#define ISI_IER_SRST(value)                   (ISI_IER_SRST_Msk & ((value) << ISI_IER_SRST_Pos))
#define ISI_IER_VSYNC_Pos                     (10U)                                          /**< (ISI_IER) Vertical Synchronization Interrupt Enable Position */
#define ISI_IER_VSYNC_Msk                     (0x1U << ISI_IER_VSYNC_Pos)                    /**< (ISI_IER) Vertical Synchronization Interrupt Enable Mask */
#define ISI_IER_VSYNC(value)                  (ISI_IER_VSYNC_Msk & ((value) << ISI_IER_VSYNC_Pos))
#define ISI_IER_PXFR_DONE_Pos                 (16U)                                          /**< (ISI_IER) Preview DMA Transfer Done Interrupt Enable Position */
#define ISI_IER_PXFR_DONE_Msk                 (0x1U << ISI_IER_PXFR_DONE_Pos)                /**< (ISI_IER) Preview DMA Transfer Done Interrupt Enable Mask */
#define ISI_IER_PXFR_DONE(value)              (ISI_IER_PXFR_DONE_Msk & ((value) << ISI_IER_PXFR_DONE_Pos))
#define ISI_IER_CXFR_DONE_Pos                 (17U)                                          /**< (ISI_IER) Codec DMA Transfer Done Interrupt Enable Position */
#define ISI_IER_CXFR_DONE_Msk                 (0x1U << ISI_IER_CXFR_DONE_Pos)                /**< (ISI_IER) Codec DMA Transfer Done Interrupt Enable Mask */
#define ISI_IER_CXFR_DONE(value)              (ISI_IER_CXFR_DONE_Msk & ((value) << ISI_IER_CXFR_DONE_Pos))
#define ISI_IER_P_OVR_Pos                     (24U)                                          /**< (ISI_IER) Preview Datapath Overflow Interrupt Enable Position */
#define ISI_IER_P_OVR_Msk                     (0x1U << ISI_IER_P_OVR_Pos)                    /**< (ISI_IER) Preview Datapath Overflow Interrupt Enable Mask */
#define ISI_IER_P_OVR(value)                  (ISI_IER_P_OVR_Msk & ((value) << ISI_IER_P_OVR_Pos))
#define ISI_IER_C_OVR_Pos                     (25U)                                          /**< (ISI_IER) Codec Datapath Overflow Interrupt Enable Position */
#define ISI_IER_C_OVR_Msk                     (0x1U << ISI_IER_C_OVR_Pos)                    /**< (ISI_IER) Codec Datapath Overflow Interrupt Enable Mask */
#define ISI_IER_C_OVR(value)                  (ISI_IER_C_OVR_Msk & ((value) << ISI_IER_C_OVR_Pos))
#define ISI_IER_CRC_ERR_Pos                   (26U)                                          /**< (ISI_IER) Embedded Synchronization CRC Error Interrupt Enable Position */
#define ISI_IER_CRC_ERR_Msk                   (0x1U << ISI_IER_CRC_ERR_Pos)                  /**< (ISI_IER) Embedded Synchronization CRC Error Interrupt Enable Mask */
#define ISI_IER_CRC_ERR(value)                (ISI_IER_CRC_ERR_Msk & ((value) << ISI_IER_CRC_ERR_Pos))
#define ISI_IER_FR_OVR_Pos                    (27U)                                          /**< (ISI_IER) Frame Rate Overflow Interrupt Enable Position */
#define ISI_IER_FR_OVR_Msk                    (0x1U << ISI_IER_FR_OVR_Pos)                   /**< (ISI_IER) Frame Rate Overflow Interrupt Enable Mask */
#define ISI_IER_FR_OVR(value)                 (ISI_IER_FR_OVR_Msk & ((value) << ISI_IER_FR_OVR_Pos))
#define ISI_IER_Msk                           (0x0F030406UL)                                 /**< (ISI_IER) Register Mask  */

/* -------- ISI_IDR : (ISI Offset: 0x30) ( /W 32) ISI Interrupt Disable Register -------- */
#define ISI_IDR_DIS_DONE_Pos                  (1U)                                           /**< (ISI_IDR) Disable Done Interrupt Disable Position */
#define ISI_IDR_DIS_DONE_Msk                  (0x1U << ISI_IDR_DIS_DONE_Pos)                 /**< (ISI_IDR) Disable Done Interrupt Disable Mask */
#define ISI_IDR_DIS_DONE(value)               (ISI_IDR_DIS_DONE_Msk & ((value) << ISI_IDR_DIS_DONE_Pos))
#define ISI_IDR_SRST_Pos                      (2U)                                           /**< (ISI_IDR) Software Reset Interrupt Disable Position */
#define ISI_IDR_SRST_Msk                      (0x1U << ISI_IDR_SRST_Pos)                     /**< (ISI_IDR) Software Reset Interrupt Disable Mask */
#define ISI_IDR_SRST(value)                   (ISI_IDR_SRST_Msk & ((value) << ISI_IDR_SRST_Pos))
#define ISI_IDR_VSYNC_Pos                     (10U)                                          /**< (ISI_IDR) Vertical Synchronization Interrupt Disable Position */
#define ISI_IDR_VSYNC_Msk                     (0x1U << ISI_IDR_VSYNC_Pos)                    /**< (ISI_IDR) Vertical Synchronization Interrupt Disable Mask */
#define ISI_IDR_VSYNC(value)                  (ISI_IDR_VSYNC_Msk & ((value) << ISI_IDR_VSYNC_Pos))
#define ISI_IDR_PXFR_DONE_Pos                 (16U)                                          /**< (ISI_IDR) Preview DMA Transfer Done Interrupt Disable Position */
#define ISI_IDR_PXFR_DONE_Msk                 (0x1U << ISI_IDR_PXFR_DONE_Pos)                /**< (ISI_IDR) Preview DMA Transfer Done Interrupt Disable Mask */
#define ISI_IDR_PXFR_DONE(value)              (ISI_IDR_PXFR_DONE_Msk & ((value) << ISI_IDR_PXFR_DONE_Pos))
#define ISI_IDR_CXFR_DONE_Pos                 (17U)                                          /**< (ISI_IDR) Codec DMA Transfer Done Interrupt Disable Position */
#define ISI_IDR_CXFR_DONE_Msk                 (0x1U << ISI_IDR_CXFR_DONE_Pos)                /**< (ISI_IDR) Codec DMA Transfer Done Interrupt Disable Mask */
#define ISI_IDR_CXFR_DONE(value)              (ISI_IDR_CXFR_DONE_Msk & ((value) << ISI_IDR_CXFR_DONE_Pos))
#define ISI_IDR_P_OVR_Pos                     (24U)                                          /**< (ISI_IDR) Preview Datapath Overflow Interrupt Disable Position */
#define ISI_IDR_P_OVR_Msk                     (0x1U << ISI_IDR_P_OVR_Pos)                    /**< (ISI_IDR) Preview Datapath Overflow Interrupt Disable Mask */
#define ISI_IDR_P_OVR(value)                  (ISI_IDR_P_OVR_Msk & ((value) << ISI_IDR_P_OVR_Pos))
#define ISI_IDR_C_OVR_Pos                     (25U)                                          /**< (ISI_IDR) Codec Datapath Overflow Interrupt Disable Position */
#define ISI_IDR_C_OVR_Msk                     (0x1U << ISI_IDR_C_OVR_Pos)                    /**< (ISI_IDR) Codec Datapath Overflow Interrupt Disable Mask */
#define ISI_IDR_C_OVR(value)                  (ISI_IDR_C_OVR_Msk & ((value) << ISI_IDR_C_OVR_Pos))
#define ISI_IDR_CRC_ERR_Pos                   (26U)                                          /**< (ISI_IDR) Embedded Synchronization CRC Error Interrupt Disable Position */
#define ISI_IDR_CRC_ERR_Msk                   (0x1U << ISI_IDR_CRC_ERR_Pos)                  /**< (ISI_IDR) Embedded Synchronization CRC Error Interrupt Disable Mask */
#define ISI_IDR_CRC_ERR(value)                (ISI_IDR_CRC_ERR_Msk & ((value) << ISI_IDR_CRC_ERR_Pos))
#define ISI_IDR_FR_OVR_Pos                    (27U)                                          /**< (ISI_IDR) Frame Rate Overflow Interrupt Disable Position */
#define ISI_IDR_FR_OVR_Msk                    (0x1U << ISI_IDR_FR_OVR_Pos)                   /**< (ISI_IDR) Frame Rate Overflow Interrupt Disable Mask */
#define ISI_IDR_FR_OVR(value)                 (ISI_IDR_FR_OVR_Msk & ((value) << ISI_IDR_FR_OVR_Pos))
#define ISI_IDR_Msk                           (0x0F030406UL)                                 /**< (ISI_IDR) Register Mask  */

/* -------- ISI_IMR : (ISI Offset: 0x34) (R/  32) ISI Interrupt Mask Register -------- */
#define ISI_IMR_DIS_DONE_Pos                  (1U)                                           /**< (ISI_IMR) Module Disable Operation Completed Position */
#define ISI_IMR_DIS_DONE_Msk                  (0x1U << ISI_IMR_DIS_DONE_Pos)                 /**< (ISI_IMR) Module Disable Operation Completed Mask */
#define ISI_IMR_DIS_DONE(value)               (ISI_IMR_DIS_DONE_Msk & ((value) << ISI_IMR_DIS_DONE_Pos))
#define ISI_IMR_SRST_Pos                      (2U)                                           /**< (ISI_IMR) Software Reset Completed Position */
#define ISI_IMR_SRST_Msk                      (0x1U << ISI_IMR_SRST_Pos)                     /**< (ISI_IMR) Software Reset Completed Mask */
#define ISI_IMR_SRST(value)                   (ISI_IMR_SRST_Msk & ((value) << ISI_IMR_SRST_Pos))
#define ISI_IMR_VSYNC_Pos                     (10U)                                          /**< (ISI_IMR) Vertical Synchronization Position */
#define ISI_IMR_VSYNC_Msk                     (0x1U << ISI_IMR_VSYNC_Pos)                    /**< (ISI_IMR) Vertical Synchronization Mask */
#define ISI_IMR_VSYNC(value)                  (ISI_IMR_VSYNC_Msk & ((value) << ISI_IMR_VSYNC_Pos))
#define ISI_IMR_PXFR_DONE_Pos                 (16U)                                          /**< (ISI_IMR) Preview DMA Transfer Completed Position */
#define ISI_IMR_PXFR_DONE_Msk                 (0x1U << ISI_IMR_PXFR_DONE_Pos)                /**< (ISI_IMR) Preview DMA Transfer Completed Mask */
#define ISI_IMR_PXFR_DONE(value)              (ISI_IMR_PXFR_DONE_Msk & ((value) << ISI_IMR_PXFR_DONE_Pos))
#define ISI_IMR_CXFR_DONE_Pos                 (17U)                                          /**< (ISI_IMR) Codec DMA Transfer Completed Position */
#define ISI_IMR_CXFR_DONE_Msk                 (0x1U << ISI_IMR_CXFR_DONE_Pos)                /**< (ISI_IMR) Codec DMA Transfer Completed Mask */
#define ISI_IMR_CXFR_DONE(value)              (ISI_IMR_CXFR_DONE_Msk & ((value) << ISI_IMR_CXFR_DONE_Pos))
#define ISI_IMR_P_OVR_Pos                     (24U)                                          /**< (ISI_IMR) Preview FIFO Overflow Position */
#define ISI_IMR_P_OVR_Msk                     (0x1U << ISI_IMR_P_OVR_Pos)                    /**< (ISI_IMR) Preview FIFO Overflow Mask */
#define ISI_IMR_P_OVR(value)                  (ISI_IMR_P_OVR_Msk & ((value) << ISI_IMR_P_OVR_Pos))
#define ISI_IMR_C_OVR_Pos                     (25U)                                          /**< (ISI_IMR) Codec FIFO Overflow Position */
#define ISI_IMR_C_OVR_Msk                     (0x1U << ISI_IMR_C_OVR_Pos)                    /**< (ISI_IMR) Codec FIFO Overflow Mask */
#define ISI_IMR_C_OVR(value)                  (ISI_IMR_C_OVR_Msk & ((value) << ISI_IMR_C_OVR_Pos))
#define ISI_IMR_CRC_ERR_Pos                   (26U)                                          /**< (ISI_IMR) CRC Synchronization Error Position */
#define ISI_IMR_CRC_ERR_Msk                   (0x1U << ISI_IMR_CRC_ERR_Pos)                  /**< (ISI_IMR) CRC Synchronization Error Mask */
#define ISI_IMR_CRC_ERR(value)                (ISI_IMR_CRC_ERR_Msk & ((value) << ISI_IMR_CRC_ERR_Pos))
#define ISI_IMR_FR_OVR_Pos                    (27U)                                          /**< (ISI_IMR) Frame Rate Overrun Position */
#define ISI_IMR_FR_OVR_Msk                    (0x1U << ISI_IMR_FR_OVR_Pos)                   /**< (ISI_IMR) Frame Rate Overrun Mask */
#define ISI_IMR_FR_OVR(value)                 (ISI_IMR_FR_OVR_Msk & ((value) << ISI_IMR_FR_OVR_Pos))
#define ISI_IMR_Msk                           (0x0F030406UL)                                 /**< (ISI_IMR) Register Mask  */

/* -------- ISI_DMA_CHER : (ISI Offset: 0x38) ( /W 32) DMA Channel Enable Register -------- */
#define ISI_DMA_CHER_P_CH_EN_Pos              (0U)                                           /**< (ISI_DMA_CHER) Preview Channel Enable Position */
#define ISI_DMA_CHER_P_CH_EN_Msk              (0x1U << ISI_DMA_CHER_P_CH_EN_Pos)             /**< (ISI_DMA_CHER) Preview Channel Enable Mask */
#define ISI_DMA_CHER_P_CH_EN(value)           (ISI_DMA_CHER_P_CH_EN_Msk & ((value) << ISI_DMA_CHER_P_CH_EN_Pos))
#define ISI_DMA_CHER_C_CH_EN_Pos              (1U)                                           /**< (ISI_DMA_CHER) Codec Channel Enable Position */
#define ISI_DMA_CHER_C_CH_EN_Msk              (0x1U << ISI_DMA_CHER_C_CH_EN_Pos)             /**< (ISI_DMA_CHER) Codec Channel Enable Mask */
#define ISI_DMA_CHER_C_CH_EN(value)           (ISI_DMA_CHER_C_CH_EN_Msk & ((value) << ISI_DMA_CHER_C_CH_EN_Pos))
#define ISI_DMA_CHER_Msk                      (0x00000003UL)                                 /**< (ISI_DMA_CHER) Register Mask  */

/* -------- ISI_DMA_CHDR : (ISI Offset: 0x3C) ( /W 32) DMA Channel Disable Register -------- */
#define ISI_DMA_CHDR_P_CH_DIS_Pos             (0U)                                           /**< (ISI_DMA_CHDR) Preview Channel Disable Request Position */
#define ISI_DMA_CHDR_P_CH_DIS_Msk             (0x1U << ISI_DMA_CHDR_P_CH_DIS_Pos)            /**< (ISI_DMA_CHDR) Preview Channel Disable Request Mask */
#define ISI_DMA_CHDR_P_CH_DIS(value)          (ISI_DMA_CHDR_P_CH_DIS_Msk & ((value) << ISI_DMA_CHDR_P_CH_DIS_Pos))
#define ISI_DMA_CHDR_C_CH_DIS_Pos             (1U)                                           /**< (ISI_DMA_CHDR) Codec Channel Disable Request Position */
#define ISI_DMA_CHDR_C_CH_DIS_Msk             (0x1U << ISI_DMA_CHDR_C_CH_DIS_Pos)            /**< (ISI_DMA_CHDR) Codec Channel Disable Request Mask */
#define ISI_DMA_CHDR_C_CH_DIS(value)          (ISI_DMA_CHDR_C_CH_DIS_Msk & ((value) << ISI_DMA_CHDR_C_CH_DIS_Pos))
#define ISI_DMA_CHDR_Msk                      (0x00000003UL)                                 /**< (ISI_DMA_CHDR) Register Mask  */

/* -------- ISI_DMA_CHSR : (ISI Offset: 0x40) (R/  32) DMA Channel Status Register -------- */
#define ISI_DMA_CHSR_P_CH_S_Pos               (0U)                                           /**< (ISI_DMA_CHSR) Preview DMA Channel Status Position */
#define ISI_DMA_CHSR_P_CH_S_Msk               (0x1U << ISI_DMA_CHSR_P_CH_S_Pos)              /**< (ISI_DMA_CHSR) Preview DMA Channel Status Mask */
#define ISI_DMA_CHSR_P_CH_S(value)            (ISI_DMA_CHSR_P_CH_S_Msk & ((value) << ISI_DMA_CHSR_P_CH_S_Pos))
#define ISI_DMA_CHSR_C_CH_S_Pos               (1U)                                           /**< (ISI_DMA_CHSR) Code DMA Channel Status Position */
#define ISI_DMA_CHSR_C_CH_S_Msk               (0x1U << ISI_DMA_CHSR_C_CH_S_Pos)              /**< (ISI_DMA_CHSR) Code DMA Channel Status Mask */
#define ISI_DMA_CHSR_C_CH_S(value)            (ISI_DMA_CHSR_C_CH_S_Msk & ((value) << ISI_DMA_CHSR_C_CH_S_Pos))
#define ISI_DMA_CHSR_Msk                      (0x00000003UL)                                 /**< (ISI_DMA_CHSR) Register Mask  */

/* -------- ISI_DMA_P_ADDR : (ISI Offset: 0x44) (R/W 32) DMA Preview Base Address Register -------- */
#define ISI_DMA_P_ADDR_P_ADDR_Pos             (2U)                                           /**< (ISI_DMA_P_ADDR) Preview Image Base Address Position */
#define ISI_DMA_P_ADDR_P_ADDR_Msk             (0x3FFFFFFFU << ISI_DMA_P_ADDR_P_ADDR_Pos)     /**< (ISI_DMA_P_ADDR) Preview Image Base Address Mask */
#define ISI_DMA_P_ADDR_P_ADDR(value)          (ISI_DMA_P_ADDR_P_ADDR_Msk & ((value) << ISI_DMA_P_ADDR_P_ADDR_Pos))
#define ISI_DMA_P_ADDR_Msk                    (0xFFFFFFFCUL)                                 /**< (ISI_DMA_P_ADDR) Register Mask  */

/* -------- ISI_DMA_P_CTRL : (ISI Offset: 0x48) (R/W 32) DMA Preview Control Register -------- */
#define ISI_DMA_P_CTRL_P_FETCH_Pos            (0U)                                           /**< (ISI_DMA_P_CTRL) Descriptor Fetch Control Bit Position */
#define ISI_DMA_P_CTRL_P_FETCH_Msk            (0x1U << ISI_DMA_P_CTRL_P_FETCH_Pos)           /**< (ISI_DMA_P_CTRL) Descriptor Fetch Control Bit Mask */
#define ISI_DMA_P_CTRL_P_FETCH(value)         (ISI_DMA_P_CTRL_P_FETCH_Msk & ((value) << ISI_DMA_P_CTRL_P_FETCH_Pos))
#define ISI_DMA_P_CTRL_P_WB_Pos               (1U)                                           /**< (ISI_DMA_P_CTRL) Descriptor Writeback Control Bit Position */
#define ISI_DMA_P_CTRL_P_WB_Msk               (0x1U << ISI_DMA_P_CTRL_P_WB_Pos)              /**< (ISI_DMA_P_CTRL) Descriptor Writeback Control Bit Mask */
#define ISI_DMA_P_CTRL_P_WB(value)            (ISI_DMA_P_CTRL_P_WB_Msk & ((value) << ISI_DMA_P_CTRL_P_WB_Pos))
#define ISI_DMA_P_CTRL_P_IEN_Pos              (2U)                                           /**< (ISI_DMA_P_CTRL) Transfer Done Flag Control Position */
#define ISI_DMA_P_CTRL_P_IEN_Msk              (0x1U << ISI_DMA_P_CTRL_P_IEN_Pos)             /**< (ISI_DMA_P_CTRL) Transfer Done Flag Control Mask */
#define ISI_DMA_P_CTRL_P_IEN(value)           (ISI_DMA_P_CTRL_P_IEN_Msk & ((value) << ISI_DMA_P_CTRL_P_IEN_Pos))
#define ISI_DMA_P_CTRL_P_DONE_Pos             (3U)                                           /**< (ISI_DMA_P_CTRL) Preview Transfer Done Position */
#define ISI_DMA_P_CTRL_P_DONE_Msk             (0x1U << ISI_DMA_P_CTRL_P_DONE_Pos)            /**< (ISI_DMA_P_CTRL) Preview Transfer Done Mask */
#define ISI_DMA_P_CTRL_P_DONE(value)          (ISI_DMA_P_CTRL_P_DONE_Msk & ((value) << ISI_DMA_P_CTRL_P_DONE_Pos))
#define ISI_DMA_P_CTRL_Msk                    (0x0000000FUL)                                 /**< (ISI_DMA_P_CTRL) Register Mask  */

/* -------- ISI_DMA_P_DSCR : (ISI Offset: 0x4C) (R/W 32) DMA Preview Descriptor Address Register -------- */
#define ISI_DMA_P_DSCR_P_DSCR_Pos             (2U)                                           /**< (ISI_DMA_P_DSCR) Preview Descriptor Base Address Position */
#define ISI_DMA_P_DSCR_P_DSCR_Msk             (0x3FFFFFFFU << ISI_DMA_P_DSCR_P_DSCR_Pos)     /**< (ISI_DMA_P_DSCR) Preview Descriptor Base Address Mask */
#define ISI_DMA_P_DSCR_P_DSCR(value)          (ISI_DMA_P_DSCR_P_DSCR_Msk & ((value) << ISI_DMA_P_DSCR_P_DSCR_Pos))
#define ISI_DMA_P_DSCR_Msk                    (0xFFFFFFFCUL)                                 /**< (ISI_DMA_P_DSCR) Register Mask  */

/* -------- ISI_DMA_C_ADDR : (ISI Offset: 0x50) (R/W 32) DMA Codec Base Address Register -------- */
#define ISI_DMA_C_ADDR_C_ADDR_Pos             (2U)                                           /**< (ISI_DMA_C_ADDR) Codec Image Base Address Position */
#define ISI_DMA_C_ADDR_C_ADDR_Msk             (0x3FFFFFFFU << ISI_DMA_C_ADDR_C_ADDR_Pos)     /**< (ISI_DMA_C_ADDR) Codec Image Base Address Mask */
#define ISI_DMA_C_ADDR_C_ADDR(value)          (ISI_DMA_C_ADDR_C_ADDR_Msk & ((value) << ISI_DMA_C_ADDR_C_ADDR_Pos))
#define ISI_DMA_C_ADDR_Msk                    (0xFFFFFFFCUL)                                 /**< (ISI_DMA_C_ADDR) Register Mask  */

/* -------- ISI_DMA_C_CTRL : (ISI Offset: 0x54) (R/W 32) DMA Codec Control Register -------- */
#define ISI_DMA_C_CTRL_C_FETCH_Pos            (0U)                                           /**< (ISI_DMA_C_CTRL) Descriptor Fetch Control Bit Position */
#define ISI_DMA_C_CTRL_C_FETCH_Msk            (0x1U << ISI_DMA_C_CTRL_C_FETCH_Pos)           /**< (ISI_DMA_C_CTRL) Descriptor Fetch Control Bit Mask */
#define ISI_DMA_C_CTRL_C_FETCH(value)         (ISI_DMA_C_CTRL_C_FETCH_Msk & ((value) << ISI_DMA_C_CTRL_C_FETCH_Pos))
#define ISI_DMA_C_CTRL_C_WB_Pos               (1U)                                           /**< (ISI_DMA_C_CTRL) Descriptor Writeback Control Bit Position */
#define ISI_DMA_C_CTRL_C_WB_Msk               (0x1U << ISI_DMA_C_CTRL_C_WB_Pos)              /**< (ISI_DMA_C_CTRL) Descriptor Writeback Control Bit Mask */
#define ISI_DMA_C_CTRL_C_WB(value)            (ISI_DMA_C_CTRL_C_WB_Msk & ((value) << ISI_DMA_C_CTRL_C_WB_Pos))
#define ISI_DMA_C_CTRL_C_IEN_Pos              (2U)                                           /**< (ISI_DMA_C_CTRL) Transfer Done Flag Control Position */
#define ISI_DMA_C_CTRL_C_IEN_Msk              (0x1U << ISI_DMA_C_CTRL_C_IEN_Pos)             /**< (ISI_DMA_C_CTRL) Transfer Done Flag Control Mask */
#define ISI_DMA_C_CTRL_C_IEN(value)           (ISI_DMA_C_CTRL_C_IEN_Msk & ((value) << ISI_DMA_C_CTRL_C_IEN_Pos))
#define ISI_DMA_C_CTRL_C_DONE_Pos             (3U)                                           /**< (ISI_DMA_C_CTRL) Codec Transfer Done Position */
#define ISI_DMA_C_CTRL_C_DONE_Msk             (0x1U << ISI_DMA_C_CTRL_C_DONE_Pos)            /**< (ISI_DMA_C_CTRL) Codec Transfer Done Mask */
#define ISI_DMA_C_CTRL_C_DONE(value)          (ISI_DMA_C_CTRL_C_DONE_Msk & ((value) << ISI_DMA_C_CTRL_C_DONE_Pos))
#define ISI_DMA_C_CTRL_Msk                    (0x0000000FUL)                                 /**< (ISI_DMA_C_CTRL) Register Mask  */

/* -------- ISI_DMA_C_DSCR : (ISI Offset: 0x58) (R/W 32) DMA Codec Descriptor Address Register -------- */
#define ISI_DMA_C_DSCR_C_DSCR_Pos             (2U)                                           /**< (ISI_DMA_C_DSCR) Codec Descriptor Base Address Position */
#define ISI_DMA_C_DSCR_C_DSCR_Msk             (0x3FFFFFFFU << ISI_DMA_C_DSCR_C_DSCR_Pos)     /**< (ISI_DMA_C_DSCR) Codec Descriptor Base Address Mask */
#define ISI_DMA_C_DSCR_C_DSCR(value)          (ISI_DMA_C_DSCR_C_DSCR_Msk & ((value) << ISI_DMA_C_DSCR_C_DSCR_Pos))
#define ISI_DMA_C_DSCR_Msk                    (0xFFFFFFFCUL)                                 /**< (ISI_DMA_C_DSCR) Register Mask  */

/* -------- ISI_WPMR : (ISI Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define ISI_WPMR_WPEN_Pos                     (0U)                                           /**< (ISI_WPMR) Write Protection Enable Position */
#define ISI_WPMR_WPEN_Msk                     (0x1U << ISI_WPMR_WPEN_Pos)                    /**< (ISI_WPMR) Write Protection Enable Mask */
#define ISI_WPMR_WPEN(value)                  (ISI_WPMR_WPEN_Msk & ((value) << ISI_WPMR_WPEN_Pos))
#define ISI_WPMR_WPKEY_Pos                    (8U)                                           /**< (ISI_WPMR) Write Protection Key Password Position */
#define ISI_WPMR_WPKEY_Msk                    (0xFFFFFFU << ISI_WPMR_WPKEY_Pos)              /**< (ISI_WPMR) Write Protection Key Password Mask */
#define ISI_WPMR_WPKEY(value)                 (ISI_WPMR_WPKEY_Msk & ((value) << ISI_WPMR_WPKEY_Pos))
#define   ISI_WPMR_WPKEY_PASSWD_Val           (4805449U)                                     /**< (ISI_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0.  */
#define ISI_WPMR_WPKEY_PASSWD                 (ISI_WPMR_WPKEY_PASSWD_Val << ISI_WPMR_WPKEY_Pos) /**< (ISI_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0. Position  */
#define ISI_WPMR_Msk                          (0xFFFFFF01UL)                                 /**< (ISI_WPMR) Register Mask  */

/* -------- ISI_WPSR : (ISI Offset: 0xE8) (R/  32) Write Protection Status Register -------- */
#define ISI_WPSR_WPVS_Pos                     (0U)                                           /**< (ISI_WPSR) Write Protection Violation Status Position */
#define ISI_WPSR_WPVS_Msk                     (0x1U << ISI_WPSR_WPVS_Pos)                    /**< (ISI_WPSR) Write Protection Violation Status Mask */
#define ISI_WPSR_WPVS(value)                  (ISI_WPSR_WPVS_Msk & ((value) << ISI_WPSR_WPVS_Pos))
#define ISI_WPSR_WPVSRC_Pos                   (8U)                                           /**< (ISI_WPSR) Write Protection Violation Source Position */
#define ISI_WPSR_WPVSRC_Msk                   (0xFFFFU << ISI_WPSR_WPVSRC_Pos)               /**< (ISI_WPSR) Write Protection Violation Source Mask */
#define ISI_WPSR_WPVSRC(value)                (ISI_WPSR_WPVSRC_Msk & ((value) << ISI_WPSR_WPVSRC_Pos))
#define ISI_WPSR_Msk                          (0x00FFFF01UL)                                 /**< (ISI_WPSR) Register Mask  */

/** \brief ISI register offsets definitions */
#define ISI_CFG1_OFFSET                (0x00)         /**< (ISI_CFG1) ISI Configuration 1 Register Offset */
#define ISI_CFG2_OFFSET                (0x04)         /**< (ISI_CFG2) ISI Configuration 2 Register Offset */
#define ISI_PSIZE_OFFSET               (0x08)         /**< (ISI_PSIZE) ISI Preview Size Register Offset */
#define ISI_PDECF_OFFSET               (0x0C)         /**< (ISI_PDECF) ISI Preview Decimation Factor Register Offset */
#define ISI_Y2R_SET0_OFFSET            (0x10)         /**< (ISI_Y2R_SET0) ISI Color Space Conversion YCrCb To RGB Set 0 Register Offset */
#define ISI_Y2R_SET1_OFFSET            (0x14)         /**< (ISI_Y2R_SET1) ISI Color Space Conversion YCrCb To RGB Set 1 Register Offset */
#define ISI_R2Y_SET0_OFFSET            (0x18)         /**< (ISI_R2Y_SET0) ISI Color Space Conversion RGB To YCrCb Set 0 Register Offset */
#define ISI_R2Y_SET1_OFFSET            (0x1C)         /**< (ISI_R2Y_SET1) ISI Color Space Conversion RGB To YCrCb Set 1 Register Offset */
#define ISI_R2Y_SET2_OFFSET            (0x20)         /**< (ISI_R2Y_SET2) ISI Color Space Conversion RGB To YCrCb Set 2 Register Offset */
#define ISI_CR_OFFSET                  (0x24)         /**< (ISI_CR) ISI Control Register Offset */
#define ISI_SR_OFFSET                  (0x28)         /**< (ISI_SR) ISI Status Register Offset */
#define ISI_IER_OFFSET                 (0x2C)         /**< (ISI_IER) ISI Interrupt Enable Register Offset */
#define ISI_IDR_OFFSET                 (0x30)         /**< (ISI_IDR) ISI Interrupt Disable Register Offset */
#define ISI_IMR_OFFSET                 (0x34)         /**< (ISI_IMR) ISI Interrupt Mask Register Offset */
#define ISI_DMA_CHER_OFFSET            (0x38)         /**< (ISI_DMA_CHER) DMA Channel Enable Register Offset */
#define ISI_DMA_CHDR_OFFSET            (0x3C)         /**< (ISI_DMA_CHDR) DMA Channel Disable Register Offset */
#define ISI_DMA_CHSR_OFFSET            (0x40)         /**< (ISI_DMA_CHSR) DMA Channel Status Register Offset */
#define ISI_DMA_P_ADDR_OFFSET          (0x44)         /**< (ISI_DMA_P_ADDR) DMA Preview Base Address Register Offset */
#define ISI_DMA_P_CTRL_OFFSET          (0x48)         /**< (ISI_DMA_P_CTRL) DMA Preview Control Register Offset */
#define ISI_DMA_P_DSCR_OFFSET          (0x4C)         /**< (ISI_DMA_P_DSCR) DMA Preview Descriptor Address Register Offset */
#define ISI_DMA_C_ADDR_OFFSET          (0x50)         /**< (ISI_DMA_C_ADDR) DMA Codec Base Address Register Offset */
#define ISI_DMA_C_CTRL_OFFSET          (0x54)         /**< (ISI_DMA_C_CTRL) DMA Codec Control Register Offset */
#define ISI_DMA_C_DSCR_OFFSET          (0x58)         /**< (ISI_DMA_C_DSCR) DMA Codec Descriptor Address Register Offset */
#define ISI_WPMR_OFFSET                (0xE4)         /**< (ISI_WPMR) Write Protection Mode Register Offset */
#define ISI_WPSR_OFFSET                (0xE8)         /**< (ISI_WPSR) Write Protection Status Register Offset */

/** \brief ISI register API structure */
typedef struct
{
  __IO  uint32_t                       ISI_CFG1;        /**< Offset: 0x00 (R/W  32) ISI Configuration 1 Register */
  __IO  uint32_t                       ISI_CFG2;        /**< Offset: 0x04 (R/W  32) ISI Configuration 2 Register */
  __IO  uint32_t                       ISI_PSIZE;       /**< Offset: 0x08 (R/W  32) ISI Preview Size Register */
  __IO  uint32_t                       ISI_PDECF;       /**< Offset: 0x0c (R/W  32) ISI Preview Decimation Factor Register */
  __IO  uint32_t                       ISI_Y2R_SET0;    /**< Offset: 0x10 (R/W  32) ISI Color Space Conversion YCrCb To RGB Set 0 Register */
  __IO  uint32_t                       ISI_Y2R_SET1;    /**< Offset: 0x14 (R/W  32) ISI Color Space Conversion YCrCb To RGB Set 1 Register */
  __IO  uint32_t                       ISI_R2Y_SET0;    /**< Offset: 0x18 (R/W  32) ISI Color Space Conversion RGB To YCrCb Set 0 Register */
  __IO  uint32_t                       ISI_R2Y_SET1;    /**< Offset: 0x1c (R/W  32) ISI Color Space Conversion RGB To YCrCb Set 1 Register */
  __IO  uint32_t                       ISI_R2Y_SET2;    /**< Offset: 0x20 (R/W  32) ISI Color Space Conversion RGB To YCrCb Set 2 Register */
  __O   uint32_t                       ISI_CR;          /**< Offset: 0x24 ( /W  32) ISI Control Register */
  __I   uint32_t                       ISI_SR;          /**< Offset: 0x28 (R/   32) ISI Status Register */
  __O   uint32_t                       ISI_IER;         /**< Offset: 0x2c ( /W  32) ISI Interrupt Enable Register */
  __O   uint32_t                       ISI_IDR;         /**< Offset: 0x30 ( /W  32) ISI Interrupt Disable Register */
  __I   uint32_t                       ISI_IMR;         /**< Offset: 0x34 (R/   32) ISI Interrupt Mask Register */
  __O   uint32_t                       ISI_DMA_CHER;    /**< Offset: 0x38 ( /W  32) DMA Channel Enable Register */
  __O   uint32_t                       ISI_DMA_CHDR;    /**< Offset: 0x3c ( /W  32) DMA Channel Disable Register */
  __I   uint32_t                       ISI_DMA_CHSR;    /**< Offset: 0x40 (R/   32) DMA Channel Status Register */
  __IO  uint32_t                       ISI_DMA_P_ADDR;  /**< Offset: 0x44 (R/W  32) DMA Preview Base Address Register */
  __IO  uint32_t                       ISI_DMA_P_CTRL;  /**< Offset: 0x48 (R/W  32) DMA Preview Control Register */
  __IO  uint32_t                       ISI_DMA_P_DSCR;  /**< Offset: 0x4c (R/W  32) DMA Preview Descriptor Address Register */
  __IO  uint32_t                       ISI_DMA_C_ADDR;  /**< Offset: 0x50 (R/W  32) DMA Codec Base Address Register */
  __IO  uint32_t                       ISI_DMA_C_CTRL;  /**< Offset: 0x54 (R/W  32) DMA Codec Control Register */
  __IO  uint32_t                       ISI_DMA_C_DSCR;  /**< Offset: 0x58 (R/W  32) DMA Codec Descriptor Address Register */
  __I   uint8_t                        Reserved1[0x88];
  __IO  uint32_t                       ISI_WPMR;        /**< Offset: 0xe4 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       ISI_WPSR;        /**< Offset: 0xe8 (R/   32) Write Protection Status Register */
} isi_registers_t;
/** @}  end of Image Sensor Interface */

#endif /* SAME_SAME70_ISI_MODULE_H */

