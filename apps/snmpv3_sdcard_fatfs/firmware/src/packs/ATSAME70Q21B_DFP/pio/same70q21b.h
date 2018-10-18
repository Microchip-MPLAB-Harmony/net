/**
 * \brief Peripheral I/O description for /SAME70Q21B
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
#ifndef SAME70Q21B_GPIO_H
#define SAME70Q21B_GPIO_H

/* ========== Peripheral I/O pin numbers ========== */
#define PIN_PA0                     (  0)  /**< Pin Number for PA0 */
#define PIN_PA1                     (  1)  /**< Pin Number for PA1 */
#define PIN_PA2                     (  2)  /**< Pin Number for PA2 */
#define PIN_PA3                     (  3)  /**< Pin Number for PA3 */
#define PIN_PA4                     (  4)  /**< Pin Number for PA4 */
#define PIN_PA5                     (  5)  /**< Pin Number for PA5 */
#define PIN_PA6                     (  6)  /**< Pin Number for PA6 */
#define PIN_PA7                     (  7)  /**< Pin Number for PA7 */
#define PIN_PA8                     (  8)  /**< Pin Number for PA8 */
#define PIN_PA9                     (  9)  /**< Pin Number for PA9 */
#define PIN_PA10                    ( 10)  /**< Pin Number for PA10 */
#define PIN_PA11                    ( 11)  /**< Pin Number for PA11 */
#define PIN_PA12                    ( 12)  /**< Pin Number for PA12 */
#define PIN_PA13                    ( 13)  /**< Pin Number for PA13 */
#define PIN_PA14                    ( 14)  /**< Pin Number for PA14 */
#define PIN_PA15                    ( 15)  /**< Pin Number for PA15 */
#define PIN_PA16                    ( 16)  /**< Pin Number for PA16 */
#define PIN_PA17                    ( 17)  /**< Pin Number for PA17 */
#define PIN_PA18                    ( 18)  /**< Pin Number for PA18 */
#define PIN_PA19                    ( 19)  /**< Pin Number for PA19 */
#define PIN_PA20                    ( 20)  /**< Pin Number for PA20 */
#define PIN_PA21                    ( 21)  /**< Pin Number for PA21 */
#define PIN_PA22                    ( 22)  /**< Pin Number for PA22 */
#define PIN_PA23                    ( 23)  /**< Pin Number for PA23 */
#define PIN_PA24                    ( 24)  /**< Pin Number for PA24 */
#define PIN_PA25                    ( 25)  /**< Pin Number for PA25 */
#define PIN_PA26                    ( 26)  /**< Pin Number for PA26 */
#define PIN_PA27                    ( 27)  /**< Pin Number for PA27 */
#define PIN_PA28                    ( 28)  /**< Pin Number for PA28 */
#define PIN_PA29                    ( 29)  /**< Pin Number for PA29 */
#define PIN_PA30                    ( 30)  /**< Pin Number for PA30 */
#define PIN_PA31                    ( 31)  /**< Pin Number for PA31 */
#define PIN_PB0                     ( 32)  /**< Pin Number for PB0 */
#define PIN_PB1                     ( 33)  /**< Pin Number for PB1 */
#define PIN_PB2                     ( 34)  /**< Pin Number for PB2 */
#define PIN_PB3                     ( 35)  /**< Pin Number for PB3 */
#define PIN_PB4                     ( 36)  /**< Pin Number for PB4 */
#define PIN_PB5                     ( 37)  /**< Pin Number for PB5 */
#define PIN_PB6                     ( 38)  /**< Pin Number for PB6 */
#define PIN_PB7                     ( 39)  /**< Pin Number for PB7 */
#define PIN_PB8                     ( 40)  /**< Pin Number for PB8 */
#define PIN_PB9                     ( 41)  /**< Pin Number for PB9 */
#define PIN_PB12                    ( 44)  /**< Pin Number for PB12 */
#define PIN_PB13                    ( 45)  /**< Pin Number for PB13 */
#define PIN_PC0                     ( 64)  /**< Pin Number for PC0 */
#define PIN_PC1                     ( 65)  /**< Pin Number for PC1 */
#define PIN_PC2                     ( 66)  /**< Pin Number for PC2 */
#define PIN_PC3                     ( 67)  /**< Pin Number for PC3 */
#define PIN_PC4                     ( 68)  /**< Pin Number for PC4 */
#define PIN_PC5                     ( 69)  /**< Pin Number for PC5 */
#define PIN_PC6                     ( 70)  /**< Pin Number for PC6 */
#define PIN_PC7                     ( 71)  /**< Pin Number for PC7 */
#define PIN_PC8                     ( 72)  /**< Pin Number for PC8 */
#define PIN_PC9                     ( 73)  /**< Pin Number for PC9 */
#define PIN_PC10                    ( 74)  /**< Pin Number for PC10 */
#define PIN_PC11                    ( 75)  /**< Pin Number for PC11 */
#define PIN_PC12                    ( 76)  /**< Pin Number for PC12 */
#define PIN_PC13                    ( 77)  /**< Pin Number for PC13 */
#define PIN_PC14                    ( 78)  /**< Pin Number for PC14 */
#define PIN_PC15                    ( 79)  /**< Pin Number for PC15 */
#define PIN_PC16                    ( 80)  /**< Pin Number for PC16 */
#define PIN_PC17                    ( 81)  /**< Pin Number for PC17 */
#define PIN_PC18                    ( 82)  /**< Pin Number for PC18 */
#define PIN_PC19                    ( 83)  /**< Pin Number for PC19 */
#define PIN_PC20                    ( 84)  /**< Pin Number for PC20 */
#define PIN_PC21                    ( 85)  /**< Pin Number for PC21 */
#define PIN_PC22                    ( 86)  /**< Pin Number for PC22 */
#define PIN_PC23                    ( 87)  /**< Pin Number for PC23 */
#define PIN_PC24                    ( 88)  /**< Pin Number for PC24 */
#define PIN_PC25                    ( 89)  /**< Pin Number for PC25 */
#define PIN_PC26                    ( 90)  /**< Pin Number for PC26 */
#define PIN_PC27                    ( 91)  /**< Pin Number for PC27 */
#define PIN_PC28                    ( 92)  /**< Pin Number for PC28 */
#define PIN_PC29                    ( 93)  /**< Pin Number for PC29 */
#define PIN_PC30                    ( 94)  /**< Pin Number for PC30 */
#define PIN_PC31                    ( 95)  /**< Pin Number for PC31 */
#define PIN_PD0                     ( 96)  /**< Pin Number for PD0 */
#define PIN_PD1                     ( 97)  /**< Pin Number for PD1 */
#define PIN_PD2                     ( 98)  /**< Pin Number for PD2 */
#define PIN_PD3                     ( 99)  /**< Pin Number for PD3 */
#define PIN_PD4                     (100)  /**< Pin Number for PD4 */
#define PIN_PD5                     (101)  /**< Pin Number for PD5 */
#define PIN_PD6                     (102)  /**< Pin Number for PD6 */
#define PIN_PD7                     (103)  /**< Pin Number for PD7 */
#define PIN_PD8                     (104)  /**< Pin Number for PD8 */
#define PIN_PD9                     (105)  /**< Pin Number for PD9 */
#define PIN_PD10                    (106)  /**< Pin Number for PD10 */
#define PIN_PD11                    (107)  /**< Pin Number for PD11 */
#define PIN_PD12                    (108)  /**< Pin Number for PD12 */
#define PIN_PD13                    (109)  /**< Pin Number for PD13 */
#define PIN_PD14                    (110)  /**< Pin Number for PD14 */
#define PIN_PD15                    (111)  /**< Pin Number for PD15 */
#define PIN_PD16                    (112)  /**< Pin Number for PD16 */
#define PIN_PD17                    (113)  /**< Pin Number for PD17 */
#define PIN_PD18                    (114)  /**< Pin Number for PD18 */
#define PIN_PD19                    (115)  /**< Pin Number for PD19 */
#define PIN_PD20                    (116)  /**< Pin Number for PD20 */
#define PIN_PD21                    (117)  /**< Pin Number for PD21 */
#define PIN_PD22                    (118)  /**< Pin Number for PD22 */
#define PIN_PD23                    (119)  /**< Pin Number for PD23 */
#define PIN_PD24                    (120)  /**< Pin Number for PD24 */
#define PIN_PD25                    (121)  /**< Pin Number for PD25 */
#define PIN_PD26                    (122)  /**< Pin Number for PD26 */
#define PIN_PD27                    (123)  /**< Pin Number for PD27 */
#define PIN_PD28                    (124)  /**< Pin Number for PD28 */
#define PIN_PD29                    (125)  /**< Pin Number for PD29 */
#define PIN_PD30                    (126)  /**< Pin Number for PD30 */
#define PIN_PD31                    (127)  /**< Pin Number for PD31 */
#define PIN_PE0                     (128)  /**< Pin Number for PE0 */
#define PIN_PE1                     (129)  /**< Pin Number for PE1 */
#define PIN_PE2                     (130)  /**< Pin Number for PE2 */
#define PIN_PE3                     (131)  /**< Pin Number for PE3 */
#define PIN_PE4                     (132)  /**< Pin Number for PE4 */
#define PIN_PE5                     (133)  /**< Pin Number for PE5 */

/* ========== Peripheral I/O masks ========== */
#define PIO_PA0                     (1UL << 0)  /**< PIO mask for PA0 */
#define PIO_PA1                     (1UL << 1)  /**< PIO mask for PA1 */
#define PIO_PA2                     (1UL << 2)  /**< PIO mask for PA2 */
#define PIO_PA3                     (1UL << 3)  /**< PIO mask for PA3 */
#define PIO_PA4                     (1UL << 4)  /**< PIO mask for PA4 */
#define PIO_PA5                     (1UL << 5)  /**< PIO mask for PA5 */
#define PIO_PA6                     (1UL << 6)  /**< PIO mask for PA6 */
#define PIO_PA7                     (1UL << 7)  /**< PIO mask for PA7 */
#define PIO_PA8                     (1UL << 8)  /**< PIO mask for PA8 */
#define PIO_PA9                     (1UL << 9)  /**< PIO mask for PA9 */
#define PIO_PA10                    (1UL << 10)  /**< PIO mask for PA10 */
#define PIO_PA11                    (1UL << 11)  /**< PIO mask for PA11 */
#define PIO_PA12                    (1UL << 12)  /**< PIO mask for PA12 */
#define PIO_PA13                    (1UL << 13)  /**< PIO mask for PA13 */
#define PIO_PA14                    (1UL << 14)  /**< PIO mask for PA14 */
#define PIO_PA15                    (1UL << 15)  /**< PIO mask for PA15 */
#define PIO_PA16                    (1UL << 16)  /**< PIO mask for PA16 */
#define PIO_PA17                    (1UL << 17)  /**< PIO mask for PA17 */
#define PIO_PA18                    (1UL << 18)  /**< PIO mask for PA18 */
#define PIO_PA19                    (1UL << 19)  /**< PIO mask for PA19 */
#define PIO_PA20                    (1UL << 20)  /**< PIO mask for PA20 */
#define PIO_PA21                    (1UL << 21)  /**< PIO mask for PA21 */
#define PIO_PA22                    (1UL << 22)  /**< PIO mask for PA22 */
#define PIO_PA23                    (1UL << 23)  /**< PIO mask for PA23 */
#define PIO_PA24                    (1UL << 24)  /**< PIO mask for PA24 */
#define PIO_PA25                    (1UL << 25)  /**< PIO mask for PA25 */
#define PIO_PA26                    (1UL << 26)  /**< PIO mask for PA26 */
#define PIO_PA27                    (1UL << 27)  /**< PIO mask for PA27 */
#define PIO_PA28                    (1UL << 28)  /**< PIO mask for PA28 */
#define PIO_PA29                    (1UL << 29)  /**< PIO mask for PA29 */
#define PIO_PA30                    (1UL << 30)  /**< PIO mask for PA30 */
#define PIO_PA31                    (1UL << 31)  /**< PIO mask for PA31 */
#define PIO_PB0                     (1UL << 0)  /**< PIO mask for PB0 */
#define PIO_PB1                     (1UL << 1)  /**< PIO mask for PB1 */
#define PIO_PB2                     (1UL << 2)  /**< PIO mask for PB2 */
#define PIO_PB3                     (1UL << 3)  /**< PIO mask for PB3 */
#define PIO_PB4                     (1UL << 4)  /**< PIO mask for PB4 */
#define PIO_PB5                     (1UL << 5)  /**< PIO mask for PB5 */
#define PIO_PB6                     (1UL << 6)  /**< PIO mask for PB6 */
#define PIO_PB7                     (1UL << 7)  /**< PIO mask for PB7 */
#define PIO_PB8                     (1UL << 8)  /**< PIO mask for PB8 */
#define PIO_PB9                     (1UL << 9)  /**< PIO mask for PB9 */
#define PIO_PB12                    (1UL << 12)  /**< PIO mask for PB12 */
#define PIO_PB13                    (1UL << 13)  /**< PIO mask for PB13 */
#define PIO_PC0                     (1UL << 0)  /**< PIO mask for PC0 */
#define PIO_PC1                     (1UL << 1)  /**< PIO mask for PC1 */
#define PIO_PC2                     (1UL << 2)  /**< PIO mask for PC2 */
#define PIO_PC3                     (1UL << 3)  /**< PIO mask for PC3 */
#define PIO_PC4                     (1UL << 4)  /**< PIO mask for PC4 */
#define PIO_PC5                     (1UL << 5)  /**< PIO mask for PC5 */
#define PIO_PC6                     (1UL << 6)  /**< PIO mask for PC6 */
#define PIO_PC7                     (1UL << 7)  /**< PIO mask for PC7 */
#define PIO_PC8                     (1UL << 8)  /**< PIO mask for PC8 */
#define PIO_PC9                     (1UL << 9)  /**< PIO mask for PC9 */
#define PIO_PC10                    (1UL << 10)  /**< PIO mask for PC10 */
#define PIO_PC11                    (1UL << 11)  /**< PIO mask for PC11 */
#define PIO_PC12                    (1UL << 12)  /**< PIO mask for PC12 */
#define PIO_PC13                    (1UL << 13)  /**< PIO mask for PC13 */
#define PIO_PC14                    (1UL << 14)  /**< PIO mask for PC14 */
#define PIO_PC15                    (1UL << 15)  /**< PIO mask for PC15 */
#define PIO_PC16                    (1UL << 16)  /**< PIO mask for PC16 */
#define PIO_PC17                    (1UL << 17)  /**< PIO mask for PC17 */
#define PIO_PC18                    (1UL << 18)  /**< PIO mask for PC18 */
#define PIO_PC19                    (1UL << 19)  /**< PIO mask for PC19 */
#define PIO_PC20                    (1UL << 20)  /**< PIO mask for PC20 */
#define PIO_PC21                    (1UL << 21)  /**< PIO mask for PC21 */
#define PIO_PC22                    (1UL << 22)  /**< PIO mask for PC22 */
#define PIO_PC23                    (1UL << 23)  /**< PIO mask for PC23 */
#define PIO_PC24                    (1UL << 24)  /**< PIO mask for PC24 */
#define PIO_PC25                    (1UL << 25)  /**< PIO mask for PC25 */
#define PIO_PC26                    (1UL << 26)  /**< PIO mask for PC26 */
#define PIO_PC27                    (1UL << 27)  /**< PIO mask for PC27 */
#define PIO_PC28                    (1UL << 28)  /**< PIO mask for PC28 */
#define PIO_PC29                    (1UL << 29)  /**< PIO mask for PC29 */
#define PIO_PC30                    (1UL << 30)  /**< PIO mask for PC30 */
#define PIO_PC31                    (1UL << 31)  /**< PIO mask for PC31 */
#define PIO_PD0                     (1UL << 0)  /**< PIO mask for PD0 */
#define PIO_PD1                     (1UL << 1)  /**< PIO mask for PD1 */
#define PIO_PD2                     (1UL << 2)  /**< PIO mask for PD2 */
#define PIO_PD3                     (1UL << 3)  /**< PIO mask for PD3 */
#define PIO_PD4                     (1UL << 4)  /**< PIO mask for PD4 */
#define PIO_PD5                     (1UL << 5)  /**< PIO mask for PD5 */
#define PIO_PD6                     (1UL << 6)  /**< PIO mask for PD6 */
#define PIO_PD7                     (1UL << 7)  /**< PIO mask for PD7 */
#define PIO_PD8                     (1UL << 8)  /**< PIO mask for PD8 */
#define PIO_PD9                     (1UL << 9)  /**< PIO mask for PD9 */
#define PIO_PD10                    (1UL << 10)  /**< PIO mask for PD10 */
#define PIO_PD11                    (1UL << 11)  /**< PIO mask for PD11 */
#define PIO_PD12                    (1UL << 12)  /**< PIO mask for PD12 */
#define PIO_PD13                    (1UL << 13)  /**< PIO mask for PD13 */
#define PIO_PD14                    (1UL << 14)  /**< PIO mask for PD14 */
#define PIO_PD15                    (1UL << 15)  /**< PIO mask for PD15 */
#define PIO_PD16                    (1UL << 16)  /**< PIO mask for PD16 */
#define PIO_PD17                    (1UL << 17)  /**< PIO mask for PD17 */
#define PIO_PD18                    (1UL << 18)  /**< PIO mask for PD18 */
#define PIO_PD19                    (1UL << 19)  /**< PIO mask for PD19 */
#define PIO_PD20                    (1UL << 20)  /**< PIO mask for PD20 */
#define PIO_PD21                    (1UL << 21)  /**< PIO mask for PD21 */
#define PIO_PD22                    (1UL << 22)  /**< PIO mask for PD22 */
#define PIO_PD23                    (1UL << 23)  /**< PIO mask for PD23 */
#define PIO_PD24                    (1UL << 24)  /**< PIO mask for PD24 */
#define PIO_PD25                    (1UL << 25)  /**< PIO mask for PD25 */
#define PIO_PD26                    (1UL << 26)  /**< PIO mask for PD26 */
#define PIO_PD27                    (1UL << 27)  /**< PIO mask for PD27 */
#define PIO_PD28                    (1UL << 28)  /**< PIO mask for PD28 */
#define PIO_PD29                    (1UL << 29)  /**< PIO mask for PD29 */
#define PIO_PD30                    (1UL << 30)  /**< PIO mask for PD30 */
#define PIO_PD31                    (1UL << 31)  /**< PIO mask for PD31 */
#define PIO_PE0                     (1UL << 0)  /**< PIO mask for PE0 */
#define PIO_PE1                     (1UL << 1)  /**< PIO mask for PE1 */
#define PIO_PE2                     (1UL << 2)  /**< PIO mask for PE2 */
#define PIO_PE3                     (1UL << 3)  /**< PIO mask for PE3 */
#define PIO_PE4                     (1UL << 4)  /**< PIO mask for PE4 */
#define PIO_PE5                     (1UL << 5)  /**< PIO mask for PE5 */

/* ========== PIO definition for AFEC0 peripheral ========== */
#define PIN_PA8B_AFEC0_ADTRG                       (8L)         /**< AFEC0 signal: AFEC0_ADTRG on PA8 mux B*/
#define MUX_PA8B_AFEC0_ADTRG                       (1L)         /**< AFEC0 signal line function value: AFEC0_ADTRG */
#define PIO_PA8B_AFEC0_ADTRG                       (1UL << 8)   /**< AFEC0 signal: AFEC0ADTRG */
#define PIN_PD30X1_AFEC0_AD0                       (126L)       /**< AFEC0 signal: AFEC0_AD0 on PD30 mux X1*/
#define PIO_PD30X1_AFEC0_AD0                       (1UL << 30)  /**< AFEC0 signal: AFEC0AD0 */
#define PIN_PA21X1_AFEC0_AD1                       (21L)        /**< AFEC0 signal: AFEC0_AD1 on PA21 mux X1*/
#define PIO_PA21X1_AFEC0_AD1                       (1UL << 21)  /**< AFEC0 signal: AFEC0AD1 */
#define PIN_PB3X1_AFEC0_AD2                        (35L)        /**< AFEC0 signal: AFEC0_AD2 on PB3 mux X1*/
#define PIO_PB3X1_AFEC0_AD2                        (1UL << 3)   /**< AFEC0 signal: AFEC0AD2 */
#define PIN_PE5X1_AFEC0_AD3                        (133L)       /**< AFEC0 signal: AFEC0_AD3 on PE5 mux X1*/
#define PIO_PE5X1_AFEC0_AD3                        (1UL << 5)   /**< AFEC0 signal: AFEC0AD3 */
#define PIN_PE4X1_AFEC0_AD4                        (132L)       /**< AFEC0 signal: AFEC0_AD4 on PE4 mux X1*/
#define PIO_PE4X1_AFEC0_AD4                        (1UL << 4)   /**< AFEC0 signal: AFEC0AD4 */
#define PIN_PB2X1_AFEC0_AD5                        (34L)        /**< AFEC0 signal: AFEC0_AD5 on PB2 mux X1*/
#define PIO_PB2X1_AFEC0_AD5                        (1UL << 2)   /**< AFEC0 signal: AFEC0AD5 */
#define PIN_PA17X1_AFEC0_AD6                       (17L)        /**< AFEC0 signal: AFEC0_AD6 on PA17 mux X1*/
#define PIO_PA17X1_AFEC0_AD6                       (1UL << 17)  /**< AFEC0 signal: AFEC0AD6 */
#define PIN_PA18X1_AFEC0_AD7                       (18L)        /**< AFEC0 signal: AFEC0_AD7 on PA18 mux X1*/
#define PIO_PA18X1_AFEC0_AD7                       (1UL << 18)  /**< AFEC0 signal: AFEC0AD7 */
#define PIN_PA19X1_AFEC0_AD8                       (19L)        /**< AFEC0 signal: AFEC0_AD8 on PA19 mux X1*/
#define PIO_PA19X1_AFEC0_AD8                       (1UL << 19)  /**< AFEC0 signal: AFEC0AD8 */
#define PIN_PA20X1_AFEC0_AD9                       (20L)        /**< AFEC0 signal: AFEC0_AD9 on PA20 mux X1*/
#define PIO_PA20X1_AFEC0_AD9                       (1UL << 20)  /**< AFEC0 signal: AFEC0AD9 */
#define PIN_PB0X1_AFEC0_AD10                       (32L)        /**< AFEC0 signal: AFEC0_AD10 on PB0 mux X1*/
#define PIO_PB0X1_AFEC0_AD10                       (1UL << 0)   /**< AFEC0 signal: AFEC0AD10 */
/* ========== PIO definition for AFEC1 peripheral ========== */
#define PIN_PD9C_AFEC1_ADTRG                       (105L)       /**< AFEC1 signal: AFEC1_ADTRG on PD9 mux C*/
#define MUX_PD9C_AFEC1_ADTRG                       (2L)         /**< AFEC1 signal line function value: AFEC1_ADTRG */
#define PIO_PD9C_AFEC1_ADTRG                       (1UL << 9)   /**< AFEC1 signal: AFEC1ADTRG */
#define PIN_PB1X1_AFEC1_AD0                        (33L)        /**< AFEC1 signal: AFEC1_AD0 on PB1 mux X1*/
#define PIO_PB1X1_AFEC1_AD0                        (1UL << 1)   /**< AFEC1 signal: AFEC1AD0 */
#define PIN_PC13X1_AFEC1_AD1                       (77L)        /**< AFEC1 signal: AFEC1_AD1 on PC13 mux X1*/
#define PIO_PC13X1_AFEC1_AD1                       (1UL << 13)  /**< AFEC1 signal: AFEC1AD1 */
#define PIN_PC15X1_AFEC1_AD2                       (79L)        /**< AFEC1 signal: AFEC1_AD2 on PC15 mux X1*/
#define PIO_PC15X1_AFEC1_AD2                       (1UL << 15)  /**< AFEC1 signal: AFEC1AD2 */
#define PIN_PC12X1_AFEC1_AD3                       (76L)        /**< AFEC1 signal: AFEC1_AD3 on PC12 mux X1*/
#define PIO_PC12X1_AFEC1_AD3                       (1UL << 12)  /**< AFEC1 signal: AFEC1AD3 */
#define PIN_PC29X1_AFEC1_AD4                       (93L)        /**< AFEC1 signal: AFEC1_AD4 on PC29 mux X1*/
#define PIO_PC29X1_AFEC1_AD4                       (1UL << 29)  /**< AFEC1 signal: AFEC1AD4 */
#define PIN_PC30X1_AFEC1_AD5                       (94L)        /**< AFEC1 signal: AFEC1_AD5 on PC30 mux X1*/
#define PIO_PC30X1_AFEC1_AD5                       (1UL << 30)  /**< AFEC1 signal: AFEC1AD5 */
#define PIN_PC31X1_AFEC1_AD6                       (95L)        /**< AFEC1 signal: AFEC1_AD6 on PC31 mux X1*/
#define PIO_PC31X1_AFEC1_AD6                       (1UL << 31)  /**< AFEC1 signal: AFEC1AD6 */
#define PIN_PC26X1_AFEC1_AD7                       (90L)        /**< AFEC1 signal: AFEC1_AD7 on PC26 mux X1*/
#define PIO_PC26X1_AFEC1_AD7                       (1UL << 26)  /**< AFEC1 signal: AFEC1AD7 */
#define PIN_PC27X1_AFEC1_AD8                       (91L)        /**< AFEC1 signal: AFEC1_AD8 on PC27 mux X1*/
#define PIO_PC27X1_AFEC1_AD8                       (1UL << 27)  /**< AFEC1 signal: AFEC1AD8 */
#define PIN_PC0X1_AFEC1_AD9                        (64L)        /**< AFEC1 signal: AFEC1_AD9 on PC0 mux X1*/
#define PIO_PC0X1_AFEC1_AD9                        (1UL << 0)   /**< AFEC1 signal: AFEC1AD9 */
#define PIN_PE3X1_AFEC1_AD10                       (131L)       /**< AFEC1 signal: AFEC1_AD10 on PE3 mux X1*/
#define PIO_PE3X1_AFEC1_AD10                       (1UL << 3)   /**< AFEC1 signal: AFEC1AD10 */
#define PIN_PE0X1_AFEC1_AD11                       (128L)       /**< AFEC1 signal: AFEC1_AD11 on PE0 mux X1*/
#define PIO_PE0X1_AFEC1_AD11                       (1UL << 0)   /**< AFEC1 signal: AFEC1AD11 */
/* ========== PIO definition for DACC peripheral ========== */
#define PIN_PB13X1_DACC_DAC0                       (45L)        /**< DACC signal: DACC_DAC0 on PB13 mux X1*/
#define PIO_PB13X1_DACC_DAC0                       (1UL << 13)  /**< DACC signal: DACCDAC0 */
#define PIN_PD0X1_DACC_DAC1                        (96L)        /**< DACC signal: DACC_DAC1 on PD0 mux X1*/
#define PIO_PD0X1_DACC_DAC1                        (1UL << 0)   /**< DACC signal: DACCDAC1 */
#define PIN_PA2C_DACC_DATRG                        (2L)         /**< DACC signal: DACC_DATRG on PA2 mux C*/
#define MUX_PA2C_DACC_DATRG                        (2L)         /**< DACC signal line function value: DACC_DATRG */
#define PIO_PA2C_DACC_DATRG                        (1UL << 2)   /**< DACC signal: DACCDATRG */
/* ========== PIO definition for EBI peripheral ========== */
#define PIN_PC18A_EBI_A0                           (82L)        /**< EBI signal: EBI_A0 on PC18 mux A*/
#define MUX_PC18A_EBI_A0                           (0L)         /**< EBI signal line function value: EBI_A0 */
#define PIO_PC18A_EBI_A0                           (1UL << 18)  /**< EBI signal: EBIA0 */
#define PIN_PC19A_EBI_A1                           (83L)        /**< EBI signal: EBI_A1 on PC19 mux A*/
#define MUX_PC19A_EBI_A1                           (0L)         /**< EBI signal line function value: EBI_A1 */
#define PIO_PC19A_EBI_A1                           (1UL << 19)  /**< EBI signal: EBIA1 */
#define PIN_PC20A_EBI_A2                           (84L)        /**< EBI signal: EBI_A2 on PC20 mux A*/
#define MUX_PC20A_EBI_A2                           (0L)         /**< EBI signal line function value: EBI_A2 */
#define PIO_PC20A_EBI_A2                           (1UL << 20)  /**< EBI signal: EBIA2 */
#define PIN_PC21A_EBI_A3                           (85L)        /**< EBI signal: EBI_A3 on PC21 mux A*/
#define MUX_PC21A_EBI_A3                           (0L)         /**< EBI signal line function value: EBI_A3 */
#define PIO_PC21A_EBI_A3                           (1UL << 21)  /**< EBI signal: EBIA3 */
#define PIN_PC22A_EBI_A4                           (86L)        /**< EBI signal: EBI_A4 on PC22 mux A*/
#define MUX_PC22A_EBI_A4                           (0L)         /**< EBI signal line function value: EBI_A4 */
#define PIO_PC22A_EBI_A4                           (1UL << 22)  /**< EBI signal: EBIA4 */
#define PIN_PC23A_EBI_A5                           (87L)        /**< EBI signal: EBI_A5 on PC23 mux A*/
#define MUX_PC23A_EBI_A5                           (0L)         /**< EBI signal line function value: EBI_A5 */
#define PIO_PC23A_EBI_A5                           (1UL << 23)  /**< EBI signal: EBIA5 */
#define PIN_PC24A_EBI_A6                           (88L)        /**< EBI signal: EBI_A6 on PC24 mux A*/
#define MUX_PC24A_EBI_A6                           (0L)         /**< EBI signal line function value: EBI_A6 */
#define PIO_PC24A_EBI_A6                           (1UL << 24)  /**< EBI signal: EBIA6 */
#define PIN_PC25A_EBI_A7                           (89L)        /**< EBI signal: EBI_A7 on PC25 mux A*/
#define MUX_PC25A_EBI_A7                           (0L)         /**< EBI signal line function value: EBI_A7 */
#define PIO_PC25A_EBI_A7                           (1UL << 25)  /**< EBI signal: EBIA7 */
#define PIN_PC26A_EBI_A8                           (90L)        /**< EBI signal: EBI_A8 on PC26 mux A*/
#define MUX_PC26A_EBI_A8                           (0L)         /**< EBI signal line function value: EBI_A8 */
#define PIO_PC26A_EBI_A8                           (1UL << 26)  /**< EBI signal: EBIA8 */
#define PIN_PC27A_EBI_A9                           (91L)        /**< EBI signal: EBI_A9 on PC27 mux A*/
#define MUX_PC27A_EBI_A9                           (0L)         /**< EBI signal line function value: EBI_A9 */
#define PIO_PC27A_EBI_A9                           (1UL << 27)  /**< EBI signal: EBIA9 */
#define PIN_PC28A_EBI_A10                          (92L)        /**< EBI signal: EBI_A10 on PC28 mux A*/
#define MUX_PC28A_EBI_A10                          (0L)         /**< EBI signal line function value: EBI_A10 */
#define PIO_PC28A_EBI_A10                          (1UL << 28)  /**< EBI signal: EBIA10 */
#define PIN_PC29A_EBI_A11                          (93L)        /**< EBI signal: EBI_A11 on PC29 mux A*/
#define MUX_PC29A_EBI_A11                          (0L)         /**< EBI signal line function value: EBI_A11 */
#define PIO_PC29A_EBI_A11                          (1UL << 29)  /**< EBI signal: EBIA11 */
#define PIN_PC30A_EBI_A12                          (94L)        /**< EBI signal: EBI_A12 on PC30 mux A*/
#define MUX_PC30A_EBI_A12                          (0L)         /**< EBI signal line function value: EBI_A12 */
#define PIO_PC30A_EBI_A12                          (1UL << 30)  /**< EBI signal: EBIA12 */
#define PIN_PC31A_EBI_A13                          (95L)        /**< EBI signal: EBI_A13 on PC31 mux A*/
#define MUX_PC31A_EBI_A13                          (0L)         /**< EBI signal line function value: EBI_A13 */
#define PIO_PC31A_EBI_A13                          (1UL << 31)  /**< EBI signal: EBIA13 */
#define PIN_PA18C_EBI_A14                          (18L)        /**< EBI signal: EBI_A14 on PA18 mux C*/
#define MUX_PA18C_EBI_A14                          (2L)         /**< EBI signal line function value: EBI_A14 */
#define PIO_PA18C_EBI_A14                          (1UL << 18)  /**< EBI signal: EBIA14 */
#define PIN_PA19C_EBI_A15                          (19L)        /**< EBI signal: EBI_A15 on PA19 mux C*/
#define MUX_PA19C_EBI_A15                          (2L)         /**< EBI signal line function value: EBI_A15 */
#define PIO_PA19C_EBI_A15                          (1UL << 19)  /**< EBI signal: EBIA15 */
#define PIN_PA20C_EBI_A16                          (20L)        /**< EBI signal: EBI_A16 on PA20 mux C*/
#define MUX_PA20C_EBI_A16                          (2L)         /**< EBI signal line function value: EBI_A16 */
#define PIO_PA20C_EBI_A16                          (1UL << 20)  /**< EBI signal: EBIA16 */
#define PIN_PA0C_EBI_A17                           (0L)         /**< EBI signal: EBI_A17 on PA0 mux C*/
#define MUX_PA0C_EBI_A17                           (2L)         /**< EBI signal line function value: EBI_A17 */
#define PIO_PA0C_EBI_A17                           (1UL << 0)   /**< EBI signal: EBIA17 */
#define PIN_PA1C_EBI_A18                           (1L)         /**< EBI signal: EBI_A18 on PA1 mux C*/
#define MUX_PA1C_EBI_A18                           (2L)         /**< EBI signal line function value: EBI_A18 */
#define PIO_PA1C_EBI_A18                           (1UL << 1)   /**< EBI signal: EBIA18 */
#define PIN_PA23C_EBI_A19                          (23L)        /**< EBI signal: EBI_A19 on PA23 mux C*/
#define MUX_PA23C_EBI_A19                          (2L)         /**< EBI signal line function value: EBI_A19 */
#define PIO_PA23C_EBI_A19                          (1UL << 23)  /**< EBI signal: EBIA19 */
#define PIN_PA24C_EBI_A20                          (24L)        /**< EBI signal: EBI_A20 on PA24 mux C*/
#define MUX_PA24C_EBI_A20                          (2L)         /**< EBI signal line function value: EBI_A20 */
#define PIO_PA24C_EBI_A20                          (1UL << 24)  /**< EBI signal: EBIA20 */
#define PIN_PC16A_EBI_A21                          (80L)        /**< EBI signal: EBI_A21 on PC16 mux A*/
#define MUX_PC16A_EBI_A21                          (0L)         /**< EBI signal line function value: EBI_A21 */
#define PIO_PC16A_EBI_A21                          (1UL << 16)  /**< EBI signal: EBIA21 */
#define PIN_PC17A_EBI_A22                          (81L)        /**< EBI signal: EBI_A22 on PC17 mux A*/
#define MUX_PC17A_EBI_A22                          (0L)         /**< EBI signal line function value: EBI_A22 */
#define PIO_PC17A_EBI_A22                          (1UL << 17)  /**< EBI signal: EBIA22 */
#define PIN_PA25C_EBI_A23                          (25L)        /**< EBI signal: EBI_A23 on PA25 mux C*/
#define MUX_PA25C_EBI_A23                          (2L)         /**< EBI signal line function value: EBI_A23 */
#define PIO_PA25C_EBI_A23                          (1UL << 25)  /**< EBI signal: EBIA23 */
#define PIN_PC0A_EBI_D0                            (64L)        /**< EBI signal: EBI_D0 on PC0 mux A*/
#define MUX_PC0A_EBI_D0                            (0L)         /**< EBI signal line function value: EBI_D0 */
#define PIO_PC0A_EBI_D0                            (1UL << 0)   /**< EBI signal: EBID0 */
#define PIN_PC1A_EBI_D1                            (65L)        /**< EBI signal: EBI_D1 on PC1 mux A*/
#define MUX_PC1A_EBI_D1                            (0L)         /**< EBI signal line function value: EBI_D1 */
#define PIO_PC1A_EBI_D1                            (1UL << 1)   /**< EBI signal: EBID1 */
#define PIN_PC2A_EBI_D2                            (66L)        /**< EBI signal: EBI_D2 on PC2 mux A*/
#define MUX_PC2A_EBI_D2                            (0L)         /**< EBI signal line function value: EBI_D2 */
#define PIO_PC2A_EBI_D2                            (1UL << 2)   /**< EBI signal: EBID2 */
#define PIN_PC3A_EBI_D3                            (67L)        /**< EBI signal: EBI_D3 on PC3 mux A*/
#define MUX_PC3A_EBI_D3                            (0L)         /**< EBI signal line function value: EBI_D3 */
#define PIO_PC3A_EBI_D3                            (1UL << 3)   /**< EBI signal: EBID3 */
#define PIN_PC4A_EBI_D4                            (68L)        /**< EBI signal: EBI_D4 on PC4 mux A*/
#define MUX_PC4A_EBI_D4                            (0L)         /**< EBI signal line function value: EBI_D4 */
#define PIO_PC4A_EBI_D4                            (1UL << 4)   /**< EBI signal: EBID4 */
#define PIN_PC5A_EBI_D5                            (69L)        /**< EBI signal: EBI_D5 on PC5 mux A*/
#define MUX_PC5A_EBI_D5                            (0L)         /**< EBI signal line function value: EBI_D5 */
#define PIO_PC5A_EBI_D5                            (1UL << 5)   /**< EBI signal: EBID5 */
#define PIN_PC6A_EBI_D6                            (70L)        /**< EBI signal: EBI_D6 on PC6 mux A*/
#define MUX_PC6A_EBI_D6                            (0L)         /**< EBI signal line function value: EBI_D6 */
#define PIO_PC6A_EBI_D6                            (1UL << 6)   /**< EBI signal: EBID6 */
#define PIN_PC7A_EBI_D7                            (71L)        /**< EBI signal: EBI_D7 on PC7 mux A*/
#define MUX_PC7A_EBI_D7                            (0L)         /**< EBI signal line function value: EBI_D7 */
#define PIO_PC7A_EBI_D7                            (1UL << 7)   /**< EBI signal: EBID7 */
#define PIN_PE0A_EBI_D8                            (128L)       /**< EBI signal: EBI_D8 on PE0 mux A*/
#define MUX_PE0A_EBI_D8                            (0L)         /**< EBI signal line function value: EBI_D8 */
#define PIO_PE0A_EBI_D8                            (1UL << 0)   /**< EBI signal: EBID8 */
#define PIN_PE1A_EBI_D9                            (129L)       /**< EBI signal: EBI_D9 on PE1 mux A*/
#define MUX_PE1A_EBI_D9                            (0L)         /**< EBI signal line function value: EBI_D9 */
#define PIO_PE1A_EBI_D9                            (1UL << 1)   /**< EBI signal: EBID9 */
#define PIN_PE2A_EBI_D10                           (130L)       /**< EBI signal: EBI_D10 on PE2 mux A*/
#define MUX_PE2A_EBI_D10                           (0L)         /**< EBI signal line function value: EBI_D10 */
#define PIO_PE2A_EBI_D10                           (1UL << 2)   /**< EBI signal: EBID10 */
#define PIN_PE3A_EBI_D11                           (131L)       /**< EBI signal: EBI_D11 on PE3 mux A*/
#define MUX_PE3A_EBI_D11                           (0L)         /**< EBI signal line function value: EBI_D11 */
#define PIO_PE3A_EBI_D11                           (1UL << 3)   /**< EBI signal: EBID11 */
#define PIN_PE4A_EBI_D12                           (132L)       /**< EBI signal: EBI_D12 on PE4 mux A*/
#define MUX_PE4A_EBI_D12                           (0L)         /**< EBI signal line function value: EBI_D12 */
#define PIO_PE4A_EBI_D12                           (1UL << 4)   /**< EBI signal: EBID12 */
#define PIN_PE5A_EBI_D13                           (133L)       /**< EBI signal: EBI_D13 on PE5 mux A*/
#define MUX_PE5A_EBI_D13                           (0L)         /**< EBI signal line function value: EBI_D13 */
#define PIO_PE5A_EBI_D13                           (1UL << 5)   /**< EBI signal: EBID13 */
#define PIN_PA15A_EBI_D14                          (15L)        /**< EBI signal: EBI_D14 on PA15 mux A*/
#define MUX_PA15A_EBI_D14                          (0L)         /**< EBI signal line function value: EBI_D14 */
#define PIO_PA15A_EBI_D14                          (1UL << 15)  /**< EBI signal: EBID14 */
#define PIN_PA16A_EBI_D15                          (16L)        /**< EBI signal: EBI_D15 on PA16 mux A*/
#define MUX_PA16A_EBI_D15                          (0L)         /**< EBI signal line function value: EBI_D15 */
#define PIO_PA16A_EBI_D15                          (1UL << 16)  /**< EBI signal: EBID15 */
#define PIN_PC13A_EBI_NWAIT                        (77L)        /**< EBI signal: EBI_NWAIT on PC13 mux A*/
#define MUX_PC13A_EBI_NWAIT                        (0L)         /**< EBI signal line function value: EBI_NWAIT */
#define PIO_PC13A_EBI_NWAIT                        (1UL << 13)  /**< EBI signal: EBINWAIT */
#define PIN_PC14A_EBI_NCS0                         (78L)        /**< EBI signal: EBI_NCS0 on PC14 mux A*/
#define MUX_PC14A_EBI_NCS0                         (0L)         /**< EBI signal line function value: EBI_NCS0 */
#define PIO_PC14A_EBI_NCS0                         (1UL << 14)  /**< EBI signal: EBINCS0 */
#define PIN_PC15A_EBI_NCS1                         (79L)        /**< EBI signal: EBI_NCS1 on PC15 mux A*/
#define MUX_PC15A_EBI_NCS1                         (0L)         /**< EBI signal line function value: EBI_NCS1 */
#define PIO_PC15A_EBI_NCS1                         (1UL << 15)  /**< EBI signal: EBINCS1 */
#define PIN_PD18A_EBI_NCS1                         (114L)       /**< EBI signal: EBI_NCS1 on PD18 mux A*/
#define MUX_PD18A_EBI_NCS1                         (0L)         /**< EBI signal line function value: EBI_NCS1 */
#define PIO_PD18A_EBI_NCS1                         (1UL << 18)  /**< EBI signal: EBINCS1 */
#define PIN_PA22C_EBI_NCS2                         (22L)        /**< EBI signal: EBI_NCS2 on PA22 mux C*/
#define MUX_PA22C_EBI_NCS2                         (2L)         /**< EBI signal line function value: EBI_NCS2 */
#define PIO_PA22C_EBI_NCS2                         (1UL << 22)  /**< EBI signal: EBINCS2 */
#define PIN_PC12A_EBI_NCS3                         (76L)        /**< EBI signal: EBI_NCS3 on PC12 mux A*/
#define MUX_PC12A_EBI_NCS3                         (0L)         /**< EBI signal line function value: EBI_NCS3 */
#define PIO_PC12A_EBI_NCS3                         (1UL << 12)  /**< EBI signal: EBINCS3 */
#define PIN_PD19A_EBI_NCS3                         (115L)       /**< EBI signal: EBI_NCS3 on PD19 mux A*/
#define MUX_PD19A_EBI_NCS3                         (0L)         /**< EBI signal line function value: EBI_NCS3 */
#define PIO_PD19A_EBI_NCS3                         (1UL << 19)  /**< EBI signal: EBINCS3 */
#define PIN_PC8A_EBI_NWR0                          (72L)        /**< EBI signal: EBI_NWR0 on PC8 mux A*/
#define MUX_PC8A_EBI_NWR0                          (0L)         /**< EBI signal line function value: EBI_NWR0 */
#define PIO_PC8A_EBI_NWR0                          (1UL << 8)   /**< EBI signal: EBINWR0 */
#define PIN_PD15C_EBI_NWR1                         (111L)       /**< EBI signal: EBI_NWR1 on PD15 mux C*/
#define MUX_PD15C_EBI_NWR1                         (2L)         /**< EBI signal line function value: EBI_NWR1 */
#define PIO_PD15C_EBI_NWR1                         (1UL << 15)  /**< EBI signal: EBINWR1 */
#define PIN_PC11A_EBI_NRD                          (75L)        /**< EBI signal: EBI_NRD on PC11 mux A*/
#define MUX_PC11A_EBI_NRD                          (0L)         /**< EBI signal line function value: EBI_NRD */
#define PIO_PC11A_EBI_NRD                          (1UL << 11)  /**< EBI signal: EBINRD */
#define PIN_PC8A_EBI_NWE                           (72L)        /**< EBI signal: EBI_NWE on PC8 mux A*/
#define MUX_PC8A_EBI_NWE                           (0L)         /**< EBI signal line function value: EBI_NWE */
#define PIO_PC8A_EBI_NWE                           (1UL << 8)   /**< EBI signal: EBINWE */
#define PIN_PC18A_EBI_NBS0                         (82L)        /**< EBI signal: EBI_NBS0 on PC18 mux A*/
#define MUX_PC18A_EBI_NBS0                         (0L)         /**< EBI signal line function value: EBI_NBS0 */
#define PIO_PC18A_EBI_NBS0                         (1UL << 18)  /**< EBI signal: EBINBS0 */
#define PIN_PD15C_EBI_NBS1                         (111L)       /**< EBI signal: EBI_NBS1 on PD15 mux C*/
#define MUX_PD15C_EBI_NBS1                         (2L)         /**< EBI signal line function value: EBI_NBS1 */
#define PIO_PD15C_EBI_NBS1                         (1UL << 15)  /**< EBI signal: EBINBS1 */
#define PIN_PC16A_EBI_NANDALE                      (80L)        /**< EBI signal: EBI_NANDALE on PC16 mux A*/
#define MUX_PC16A_EBI_NANDALE                      (0L)         /**< EBI signal line function value: EBI_NANDALE */
#define PIO_PC16A_EBI_NANDALE                      (1UL << 16)  /**< EBI signal: EBINANDALE */
#define PIN_PC17A_EBI_NANDCLE                      (81L)        /**< EBI signal: EBI_NANDCLE on PC17 mux A*/
#define MUX_PC17A_EBI_NANDCLE                      (0L)         /**< EBI signal line function value: EBI_NANDCLE */
#define PIO_PC17A_EBI_NANDCLE                      (1UL << 17)  /**< EBI signal: EBINANDCLE */
#define PIN_PC9A_EBI_NANDOE                        (73L)        /**< EBI signal: EBI_NANDOE on PC9 mux A*/
#define MUX_PC9A_EBI_NANDOE                        (0L)         /**< EBI signal line function value: EBI_NANDOE */
#define PIO_PC9A_EBI_NANDOE                        (1UL << 9)   /**< EBI signal: EBINANDOE */
#define PIN_PC10A_EBI_NANDWE                       (74L)        /**< EBI signal: EBI_NANDWE on PC10 mux A*/
#define MUX_PC10A_EBI_NANDWE                       (0L)         /**< EBI signal line function value: EBI_NANDWE */
#define PIO_PC10A_EBI_NANDWE                       (1UL << 10)  /**< EBI signal: EBINANDWE */
#define PIN_PD23C_EBI_SDCK                         (119L)       /**< EBI signal: EBI_SDCK on PD23 mux C*/
#define MUX_PD23C_EBI_SDCK                         (2L)         /**< EBI signal line function value: EBI_SDCK */
#define PIO_PD23C_EBI_SDCK                         (1UL << 23)  /**< EBI signal: EBISDCK */
#define PIN_PD14C_EBI_SDCKE                        (110L)       /**< EBI signal: EBI_SDCKE on PD14 mux C*/
#define MUX_PD14C_EBI_SDCKE                        (2L)         /**< EBI signal line function value: EBI_SDCKE */
#define PIO_PD14C_EBI_SDCKE                        (1UL << 14)  /**< EBI signal: EBISDCKE */
#define PIN_PC15A_EBI_SDCS                         (79L)        /**< EBI signal: EBI_SDCS on PC15 mux A*/
#define MUX_PC15A_EBI_SDCS                         (0L)         /**< EBI signal line function value: EBI_SDCS */
#define PIO_PC15A_EBI_SDCS                         (1UL << 15)  /**< EBI signal: EBISDCS */
#define PIN_PD18A_EBI_SDCS                         (114L)       /**< EBI signal: EBI_SDCS on PD18 mux A*/
#define MUX_PD18A_EBI_SDCS                         (0L)         /**< EBI signal line function value: EBI_SDCS */
#define PIO_PD18A_EBI_SDCS                         (1UL << 18)  /**< EBI signal: EBISDCS */
#define PIN_PA20C_EBI_BA0                          (20L)        /**< EBI signal: EBI_BA0 on PA20 mux C*/
#define MUX_PA20C_EBI_BA0                          (2L)         /**< EBI signal line function value: EBI_BA0 */
#define PIO_PA20C_EBI_BA0                          (1UL << 20)  /**< EBI signal: EBIBA0 */
#define PIN_PA0C_EBI_BA1                           (0L)         /**< EBI signal: EBI_BA1 on PA0 mux C*/
#define MUX_PA0C_EBI_BA1                           (2L)         /**< EBI signal line function value: EBI_BA1 */
#define PIO_PA0C_EBI_BA1                           (1UL << 0)   /**< EBI signal: EBIBA1 */
#define PIN_PD29C_EBI_SDWE                         (125L)       /**< EBI signal: EBI_SDWE on PD29 mux C*/
#define MUX_PD29C_EBI_SDWE                         (2L)         /**< EBI signal line function value: EBI_SDWE */
#define PIO_PD29C_EBI_SDWE                         (1UL << 29)  /**< EBI signal: EBISDWE */
#define PIN_PD16C_EBI_RAS                          (112L)       /**< EBI signal: EBI_RAS on PD16 mux C*/
#define MUX_PD16C_EBI_RAS                          (2L)         /**< EBI signal line function value: EBI_RAS */
#define PIO_PD16C_EBI_RAS                          (1UL << 16)  /**< EBI signal: EBIRAS */
#define PIN_PD17C_EBI_CAS                          (113L)       /**< EBI signal: EBI_CAS on PD17 mux C*/
#define MUX_PD17C_EBI_CAS                          (2L)         /**< EBI signal line function value: EBI_CAS */
#define PIO_PD17C_EBI_CAS                          (1UL << 17)  /**< EBI signal: EBICAS */
#define PIN_PC13C_EBI_SDA10                        (77L)        /**< EBI signal: EBI_SDA10 on PC13 mux C*/
#define MUX_PC13C_EBI_SDA10                        (2L)         /**< EBI signal line function value: EBI_SDA10 */
#define PIO_PC13C_EBI_SDA10                        (1UL << 13)  /**< EBI signal: EBISDA10 */
#define PIN_PD13C_EBI_SDA10                        (109L)       /**< EBI signal: EBI_SDA10 on PD13 mux C*/
#define MUX_PD13C_EBI_SDA10                        (2L)         /**< EBI signal line function value: EBI_SDA10 */
#define PIO_PD13C_EBI_SDA10                        (1UL << 13)  /**< EBI signal: EBISDA10 */
#define PIN_PC20A_EBI_SDA0                         (84L)        /**< EBI signal: EBI_SDA0 on PC20 mux A*/
#define MUX_PC20A_EBI_SDA0                         (0L)         /**< EBI signal line function value: EBI_SDA0 */
#define PIO_PC20A_EBI_SDA0                         (1UL << 20)  /**< EBI signal: EBISDA0 */
#define PIN_PC21A_EBI_SDA1                         (85L)        /**< EBI signal: EBI_SDA1 on PC21 mux A*/
#define MUX_PC21A_EBI_SDA1                         (0L)         /**< EBI signal line function value: EBI_SDA1 */
#define PIO_PC21A_EBI_SDA1                         (1UL << 21)  /**< EBI signal: EBISDA1 */
#define PIN_PC22A_EBI_SDA2                         (86L)        /**< EBI signal: EBI_SDA2 on PC22 mux A*/
#define MUX_PC22A_EBI_SDA2                         (0L)         /**< EBI signal line function value: EBI_SDA2 */
#define PIO_PC22A_EBI_SDA2                         (1UL << 22)  /**< EBI signal: EBISDA2 */
#define PIN_PC23A_EBI_SDA3                         (87L)        /**< EBI signal: EBI_SDA3 on PC23 mux A*/
#define MUX_PC23A_EBI_SDA3                         (0L)         /**< EBI signal line function value: EBI_SDA3 */
#define PIO_PC23A_EBI_SDA3                         (1UL << 23)  /**< EBI signal: EBISDA3 */
#define PIN_PC24A_EBI_SDA4                         (88L)        /**< EBI signal: EBI_SDA4 on PC24 mux A*/
#define MUX_PC24A_EBI_SDA4                         (0L)         /**< EBI signal line function value: EBI_SDA4 */
#define PIO_PC24A_EBI_SDA4                         (1UL << 24)  /**< EBI signal: EBISDA4 */
#define PIN_PC25A_EBI_SDA5                         (89L)        /**< EBI signal: EBI_SDA5 on PC25 mux A*/
#define MUX_PC25A_EBI_SDA5                         (0L)         /**< EBI signal line function value: EBI_SDA5 */
#define PIO_PC25A_EBI_SDA5                         (1UL << 25)  /**< EBI signal: EBISDA5 */
#define PIN_PC26A_EBI_SDA6                         (90L)        /**< EBI signal: EBI_SDA6 on PC26 mux A*/
#define MUX_PC26A_EBI_SDA6                         (0L)         /**< EBI signal line function value: EBI_SDA6 */
#define PIO_PC26A_EBI_SDA6                         (1UL << 26)  /**< EBI signal: EBISDA6 */
#define PIN_PC27A_EBI_SDA7                         (91L)        /**< EBI signal: EBI_SDA7 on PC27 mux A*/
#define MUX_PC27A_EBI_SDA7                         (0L)         /**< EBI signal line function value: EBI_SDA7 */
#define PIO_PC27A_EBI_SDA7                         (1UL << 27)  /**< EBI signal: EBISDA7 */
#define PIN_PC28A_EBI_SDA8                         (92L)        /**< EBI signal: EBI_SDA8 on PC28 mux A*/
#define MUX_PC28A_EBI_SDA8                         (0L)         /**< EBI signal line function value: EBI_SDA8 */
#define PIO_PC28A_EBI_SDA8                         (1UL << 28)  /**< EBI signal: EBISDA8 */
#define PIN_PC29A_EBI_SDA9                         (93L)        /**< EBI signal: EBI_SDA9 on PC29 mux A*/
#define MUX_PC29A_EBI_SDA9                         (0L)         /**< EBI signal line function value: EBI_SDA9 */
#define PIO_PC29A_EBI_SDA9                         (1UL << 29)  /**< EBI signal: EBISDA9 */
#define PIN_PC31A_EBI_SDA11                        (95L)        /**< EBI signal: EBI_SDA11 on PC31 mux A*/
#define MUX_PC31A_EBI_SDA11                        (0L)         /**< EBI signal line function value: EBI_SDA11 */
#define PIO_PC31A_EBI_SDA11                        (1UL << 31)  /**< EBI signal: EBISDA11 */
#define PIN_PA18C_EBI_SDA12                        (18L)        /**< EBI signal: EBI_SDA12 on PA18 mux C*/
#define MUX_PA18C_EBI_SDA12                        (2L)         /**< EBI signal line function value: EBI_SDA12 */
#define PIO_PA18C_EBI_SDA12                        (1UL << 18)  /**< EBI signal: EBISDA12 */
#define PIN_PA19C_EBI_SDA13                        (19L)        /**< EBI signal: EBI_SDA13 on PA19 mux C*/
#define MUX_PA19C_EBI_SDA13                        (2L)         /**< EBI signal line function value: EBI_SDA13 */
#define PIO_PA19C_EBI_SDA13                        (1UL << 19)  /**< EBI signal: EBISDA13 */
#define PIN_PC18A_EBI_DQM0                         (82L)        /**< EBI signal: EBI_DQM0 on PC18 mux A*/
#define MUX_PC18A_EBI_DQM0                         (0L)         /**< EBI signal line function value: EBI_DQM0 */
#define PIO_PC18A_EBI_DQM0                         (1UL << 18)  /**< EBI signal: EBIDQM0 */
#define PIN_PD15C_EBI_DQM1                         (111L)       /**< EBI signal: EBI_DQM1 on PD15 mux C*/
#define MUX_PD15C_EBI_DQM1                         (2L)         /**< EBI signal line function value: EBI_DQM1 */
#define PIO_PD15C_EBI_DQM1                         (1UL << 15)  /**< EBI signal: EBIDQM1 */
/* ========== PIO definition for EFC peripheral ========== */
#define PIN_PB12X1_EFC_ERASE                       (44L)        /**< EFC signal: EFC_ERASE on PB12 mux X1*/
#define PIO_PB12X1_EFC_ERASE                       (1UL << 12)  /**< EFC signal: EFCERASE */
/* ========== PIO definition for GMAC peripheral ========== */
#define PIN_PD13A_GMAC_GCOL                        (109L)       /**< GMAC signal: GMAC_GCOL on PD13 mux A*/
#define MUX_PD13A_GMAC_GCOL                        (0L)         /**< GMAC signal line function value: GMAC_GCOL */
#define PIO_PD13A_GMAC_GCOL                        (1UL << 13)  /**< GMAC signal: GMACGCOL */
#define PIN_PD10A_GMAC_GCRS                        (106L)       /**< GMAC signal: GMAC_GCRS on PD10 mux A*/
#define MUX_PD10A_GMAC_GCRS                        (0L)         /**< GMAC signal line function value: GMAC_GCRS */
#define PIO_PD10A_GMAC_GCRS                        (1UL << 10)  /**< GMAC signal: GMACGCRS */
#define PIN_PD8A_GMAC_GMDC                         (104L)       /**< GMAC signal: GMAC_GMDC on PD8 mux A*/
#define MUX_PD8A_GMAC_GMDC                         (0L)         /**< GMAC signal line function value: GMAC_GMDC */
#define PIO_PD8A_GMAC_GMDC                         (1UL << 8)   /**< GMAC signal: GMACGMDC */
#define PIN_PD9A_GMAC_GMDIO                        (105L)       /**< GMAC signal: GMAC_GMDIO on PD9 mux A*/
#define MUX_PD9A_GMAC_GMDIO                        (0L)         /**< GMAC signal line function value: GMAC_GMDIO */
#define PIO_PD9A_GMAC_GMDIO                        (1UL << 9)   /**< GMAC signal: GMACGMDIO */
#define PIN_PD14A_GMAC_GRXCK                       (110L)       /**< GMAC signal: GMAC_GRXCK on PD14 mux A*/
#define MUX_PD14A_GMAC_GRXCK                       (0L)         /**< GMAC signal line function value: GMAC_GRXCK */
#define PIO_PD14A_GMAC_GRXCK                       (1UL << 14)  /**< GMAC signal: GMACGRXCK */
#define PIN_PD4A_GMAC_GRXDV                        (100L)       /**< GMAC signal: GMAC_GRXDV on PD4 mux A*/
#define MUX_PD4A_GMAC_GRXDV                        (0L)         /**< GMAC signal line function value: GMAC_GRXDV */
#define PIO_PD4A_GMAC_GRXDV                        (1UL << 4)   /**< GMAC signal: GMACGRXDV */
#define PIN_PD7A_GMAC_GRXER                        (103L)       /**< GMAC signal: GMAC_GRXER on PD7 mux A*/
#define MUX_PD7A_GMAC_GRXER                        (0L)         /**< GMAC signal line function value: GMAC_GRXER */
#define PIO_PD7A_GMAC_GRXER                        (1UL << 7)   /**< GMAC signal: GMACGRXER */
#define PIN_PD5A_GMAC_GRX0                         (101L)       /**< GMAC signal: GMAC_GRX0 on PD5 mux A*/
#define MUX_PD5A_GMAC_GRX0                         (0L)         /**< GMAC signal line function value: GMAC_GRX0 */
#define PIO_PD5A_GMAC_GRX0                         (1UL << 5)   /**< GMAC signal: GMACGRX0 */
#define PIN_PD6A_GMAC_GRX1                         (102L)       /**< GMAC signal: GMAC_GRX1 on PD6 mux A*/
#define MUX_PD6A_GMAC_GRX1                         (0L)         /**< GMAC signal line function value: GMAC_GRX1 */
#define PIO_PD6A_GMAC_GRX1                         (1UL << 6)   /**< GMAC signal: GMACGRX1 */
#define PIN_PD11A_GMAC_GRX2                        (107L)       /**< GMAC signal: GMAC_GRX2 on PD11 mux A*/
#define MUX_PD11A_GMAC_GRX2                        (0L)         /**< GMAC signal line function value: GMAC_GRX2 */
#define PIO_PD11A_GMAC_GRX2                        (1UL << 11)  /**< GMAC signal: GMACGRX2 */
#define PIN_PD12A_GMAC_GRX3                        (108L)       /**< GMAC signal: GMAC_GRX3 on PD12 mux A*/
#define MUX_PD12A_GMAC_GRX3                        (0L)         /**< GMAC signal line function value: GMAC_GRX3 */
#define PIO_PD12A_GMAC_GRX3                        (1UL << 12)  /**< GMAC signal: GMACGRX3 */
#define PIN_PB1B_GMAC_GTSUCOMP                     (33L)        /**< GMAC signal: GMAC_GTSUCOMP on PB1 mux B*/
#define MUX_PB1B_GMAC_GTSUCOMP                     (1L)         /**< GMAC signal line function value: GMAC_GTSUCOMP */
#define PIO_PB1B_GMAC_GTSUCOMP                     (1UL << 1)   /**< GMAC signal: GMACGTSUCOMP */
#define PIN_PB12B_GMAC_GTSUCOMP                    (44L)        /**< GMAC signal: GMAC_GTSUCOMP on PB12 mux B*/
#define MUX_PB12B_GMAC_GTSUCOMP                    (1L)         /**< GMAC signal line function value: GMAC_GTSUCOMP */
#define PIO_PB12B_GMAC_GTSUCOMP                    (1UL << 12)  /**< GMAC signal: GMACGTSUCOMP */
#define PIN_PD11C_GMAC_GTSUCOMP                    (107L)       /**< GMAC signal: GMAC_GTSUCOMP on PD11 mux C*/
#define MUX_PD11C_GMAC_GTSUCOMP                    (2L)         /**< GMAC signal line function value: GMAC_GTSUCOMP */
#define PIO_PD11C_GMAC_GTSUCOMP                    (1UL << 11)  /**< GMAC signal: GMACGTSUCOMP */
#define PIN_PD20C_GMAC_GTSUCOMP                    (116L)       /**< GMAC signal: GMAC_GTSUCOMP on PD20 mux C*/
#define MUX_PD20C_GMAC_GTSUCOMP                    (2L)         /**< GMAC signal line function value: GMAC_GTSUCOMP */
#define PIO_PD20C_GMAC_GTSUCOMP                    (1UL << 20)  /**< GMAC signal: GMACGTSUCOMP */
#define PIN_PD0A_GMAC_GTXCK                        (96L)        /**< GMAC signal: GMAC_GTXCK on PD0 mux A*/
#define MUX_PD0A_GMAC_GTXCK                        (0L)         /**< GMAC signal line function value: GMAC_GTXCK */
#define PIO_PD0A_GMAC_GTXCK                        (1UL << 0)   /**< GMAC signal: GMACGTXCK */
#define PIN_PD1A_GMAC_GTXEN                        (97L)        /**< GMAC signal: GMAC_GTXEN on PD1 mux A*/
#define MUX_PD1A_GMAC_GTXEN                        (0L)         /**< GMAC signal line function value: GMAC_GTXEN */
#define PIO_PD1A_GMAC_GTXEN                        (1UL << 1)   /**< GMAC signal: GMACGTXEN */
#define PIN_PD17A_GMAC_GTXER                       (113L)       /**< GMAC signal: GMAC_GTXER on PD17 mux A*/
#define MUX_PD17A_GMAC_GTXER                       (0L)         /**< GMAC signal line function value: GMAC_GTXER */
#define PIO_PD17A_GMAC_GTXER                       (1UL << 17)  /**< GMAC signal: GMACGTXER */
#define PIN_PD2A_GMAC_GTX0                         (98L)        /**< GMAC signal: GMAC_GTX0 on PD2 mux A*/
#define MUX_PD2A_GMAC_GTX0                         (0L)         /**< GMAC signal line function value: GMAC_GTX0 */
#define PIO_PD2A_GMAC_GTX0                         (1UL << 2)   /**< GMAC signal: GMACGTX0 */
#define PIN_PD3A_GMAC_GTX1                         (99L)        /**< GMAC signal: GMAC_GTX1 on PD3 mux A*/
#define MUX_PD3A_GMAC_GTX1                         (0L)         /**< GMAC signal line function value: GMAC_GTX1 */
#define PIO_PD3A_GMAC_GTX1                         (1UL << 3)   /**< GMAC signal: GMACGTX1 */
#define PIN_PD15A_GMAC_GTX2                        (111L)       /**< GMAC signal: GMAC_GTX2 on PD15 mux A*/
#define MUX_PD15A_GMAC_GTX2                        (0L)         /**< GMAC signal line function value: GMAC_GTX2 */
#define PIO_PD15A_GMAC_GTX2                        (1UL << 15)  /**< GMAC signal: GMACGTX2 */
#define PIN_PD16A_GMAC_GTX3                        (112L)       /**< GMAC signal: GMAC_GTX3 on PD16 mux A*/
#define MUX_PD16A_GMAC_GTX3                        (0L)         /**< GMAC signal line function value: GMAC_GTX3 */
#define PIO_PD16A_GMAC_GTX3                        (1UL << 16)  /**< GMAC signal: GMACGTX3 */
/* ========== PIO definition for HSMCI peripheral ========== */
#define PIN_PA28C_HSMCI_MCCDA                      (28L)        /**< HSMCI signal: HSMCI_MCCDA on PA28 mux C*/
#define MUX_PA28C_HSMCI_MCCDA                      (2L)         /**< HSMCI signal line function value: HSMCI_MCCDA */
#define PIO_PA28C_HSMCI_MCCDA                      (1UL << 28)  /**< HSMCI signal: HSMCIMCCDA */
#define PIN_PA25D_HSMCI_MCCK                       (25L)        /**< HSMCI signal: HSMCI_MCCK on PA25 mux D*/
#define MUX_PA25D_HSMCI_MCCK                       (3L)         /**< HSMCI signal line function value: HSMCI_MCCK */
#define PIO_PA25D_HSMCI_MCCK                       (1UL << 25)  /**< HSMCI signal: HSMCIMCCK */
#define PIN_PA30C_HSMCI_MCDA0                      (30L)        /**< HSMCI signal: HSMCI_MCDA0 on PA30 mux C*/
#define MUX_PA30C_HSMCI_MCDA0                      (2L)         /**< HSMCI signal line function value: HSMCI_MCDA0 */
#define PIO_PA30C_HSMCI_MCDA0                      (1UL << 30)  /**< HSMCI signal: HSMCIMCDA0 */
#define PIN_PA31C_HSMCI_MCDA1                      (31L)        /**< HSMCI signal: HSMCI_MCDA1 on PA31 mux C*/
#define MUX_PA31C_HSMCI_MCDA1                      (2L)         /**< HSMCI signal line function value: HSMCI_MCDA1 */
#define PIO_PA31C_HSMCI_MCDA1                      (1UL << 31)  /**< HSMCI signal: HSMCIMCDA1 */
#define PIN_PA26C_HSMCI_MCDA2                      (26L)        /**< HSMCI signal: HSMCI_MCDA2 on PA26 mux C*/
#define MUX_PA26C_HSMCI_MCDA2                      (2L)         /**< HSMCI signal line function value: HSMCI_MCDA2 */
#define PIO_PA26C_HSMCI_MCDA2                      (1UL << 26)  /**< HSMCI signal: HSMCIMCDA2 */
#define PIN_PA27C_HSMCI_MCDA3                      (27L)        /**< HSMCI signal: HSMCI_MCDA3 on PA27 mux C*/
#define MUX_PA27C_HSMCI_MCDA3                      (2L)         /**< HSMCI signal line function value: HSMCI_MCDA3 */
#define PIO_PA27C_HSMCI_MCDA3                      (1UL << 27)  /**< HSMCI signal: HSMCIMCDA3 */
/* ========== PIO definition for I2SC0 peripheral ========== */
#define PIN_PA1D_I2SC0_CK                          (1L)         /**< I2SC0 signal: I2SC0_CK on PA1 mux D*/
#define MUX_PA1D_I2SC0_CK                          (3L)         /**< I2SC0 signal line function value: I2SC0_CK */
#define PIO_PA1D_I2SC0_CK                          (1UL << 1)   /**< I2SC0 signal: I2SC0CK */
#define PIN_PA16D_I2SC0_DI0                        (16L)        /**< I2SC0 signal: I2SC0_DI0 on PA16 mux D*/
#define MUX_PA16D_I2SC0_DI0                        (3L)         /**< I2SC0 signal line function value: I2SC0_DI0 */
#define PIO_PA16D_I2SC0_DI0                        (1UL << 16)  /**< I2SC0 signal: I2SC0DI0 */
#define PIN_PA30D_I2SC0_DO0                        (30L)        /**< I2SC0 signal: I2SC0_DO0 on PA30 mux D*/
#define MUX_PA30D_I2SC0_DO0                        (3L)         /**< I2SC0 signal line function value: I2SC0_DO0 */
#define PIO_PA30D_I2SC0_DO0                        (1UL << 30)  /**< I2SC0 signal: I2SC0DO0 */
#define PIN_PA0D_I2SC0_MCK                         (0L)         /**< I2SC0 signal: I2SC0_MCK on PA0 mux D*/
#define MUX_PA0D_I2SC0_MCK                         (3L)         /**< I2SC0 signal line function value: I2SC0_MCK */
#define PIO_PA0D_I2SC0_MCK                         (1UL << 0)   /**< I2SC0 signal: I2SC0MCK */
#define PIN_PA15D_I2SC0_WS                         (15L)        /**< I2SC0 signal: I2SC0_WS on PA15 mux D*/
#define MUX_PA15D_I2SC0_WS                         (3L)         /**< I2SC0 signal line function value: I2SC0_WS */
#define PIO_PA15D_I2SC0_WS                         (1UL << 15)  /**< I2SC0 signal: I2SC0WS */
/* ========== PIO definition for I2SC1 peripheral ========== */
#define PIN_PA20D_I2SC1_CK                         (20L)        /**< I2SC1 signal: I2SC1_CK on PA20 mux D*/
#define MUX_PA20D_I2SC1_CK                         (3L)         /**< I2SC1 signal line function value: I2SC1_CK */
#define PIO_PA20D_I2SC1_CK                         (1UL << 20)  /**< I2SC1 signal: I2SC1CK */
#define PIN_PE2C_I2SC1_DI0                         (130L)       /**< I2SC1 signal: I2SC1_DI0 on PE2 mux C*/
#define MUX_PE2C_I2SC1_DI0                         (2L)         /**< I2SC1 signal line function value: I2SC1_DI0 */
#define PIO_PE2C_I2SC1_DI0                         (1UL << 2)   /**< I2SC1 signal: I2SC1DI0 */
#define PIN_PE1C_I2SC1_DO0                         (129L)       /**< I2SC1 signal: I2SC1_DO0 on PE1 mux C*/
#define MUX_PE1C_I2SC1_DO0                         (2L)         /**< I2SC1 signal line function value: I2SC1_DO0 */
#define PIO_PE1C_I2SC1_DO0                         (1UL << 1)   /**< I2SC1 signal: I2SC1DO0 */
#define PIN_PA19D_I2SC1_MCK                        (19L)        /**< I2SC1 signal: I2SC1_MCK on PA19 mux D*/
#define MUX_PA19D_I2SC1_MCK                        (3L)         /**< I2SC1 signal line function value: I2SC1_MCK */
#define PIO_PA19D_I2SC1_MCK                        (1UL << 19)  /**< I2SC1 signal: I2SC1MCK */
#define PIN_PE0C_I2SC1_WS                          (128L)       /**< I2SC1 signal: I2SC1_WS on PE0 mux C*/
#define MUX_PE0C_I2SC1_WS                          (2L)         /**< I2SC1 signal line function value: I2SC1_WS */
#define PIO_PE0C_I2SC1_WS                          (1UL << 0)   /**< I2SC1 signal: I2SC1WS */
/* ========== PIO definition for ISI peripheral ========== */
#define PIN_PD22D_ISI_D0                           (118L)       /**< ISI signal: ISI_D0 on PD22 mux D*/
#define MUX_PD22D_ISI_D0                           (3L)         /**< ISI signal line function value: ISI_D0 */
#define PIO_PD22D_ISI_D0                           (1UL << 22)  /**< ISI signal: ISID0 */
#define PIN_PD21D_ISI_D1                           (117L)       /**< ISI signal: ISI_D1 on PD21 mux D*/
#define MUX_PD21D_ISI_D1                           (3L)         /**< ISI signal line function value: ISI_D1 */
#define PIO_PD21D_ISI_D1                           (1UL << 21)  /**< ISI signal: ISID1 */
#define PIN_PB3D_ISI_D2                            (35L)        /**< ISI signal: ISI_D2 on PB3 mux D*/
#define MUX_PB3D_ISI_D2                            (3L)         /**< ISI signal line function value: ISI_D2 */
#define PIO_PB3D_ISI_D2                            (1UL << 3)   /**< ISI signal: ISID2 */
#define PIN_PA9B_ISI_D3                            (9L)         /**< ISI signal: ISI_D3 on PA9 mux B*/
#define MUX_PA9B_ISI_D3                            (1L)         /**< ISI signal line function value: ISI_D3 */
#define PIO_PA9B_ISI_D3                            (1UL << 9)   /**< ISI signal: ISID3 */
#define PIN_PA5B_ISI_D4                            (5L)         /**< ISI signal: ISI_D4 on PA5 mux B*/
#define MUX_PA5B_ISI_D4                            (1L)         /**< ISI signal line function value: ISI_D4 */
#define PIO_PA5B_ISI_D4                            (1UL << 5)   /**< ISI signal: ISID4 */
#define PIN_PD11D_ISI_D5                           (107L)       /**< ISI signal: ISI_D5 on PD11 mux D*/
#define MUX_PD11D_ISI_D5                           (3L)         /**< ISI signal line function value: ISI_D5 */
#define PIO_PD11D_ISI_D5                           (1UL << 11)  /**< ISI signal: ISID5 */
#define PIN_PD12D_ISI_D6                           (108L)       /**< ISI signal: ISI_D6 on PD12 mux D*/
#define MUX_PD12D_ISI_D6                           (3L)         /**< ISI signal line function value: ISI_D6 */
#define PIO_PD12D_ISI_D6                           (1UL << 12)  /**< ISI signal: ISID6 */
#define PIN_PA27D_ISI_D7                           (27L)        /**< ISI signal: ISI_D7 on PA27 mux D*/
#define MUX_PA27D_ISI_D7                           (3L)         /**< ISI signal line function value: ISI_D7 */
#define PIO_PA27D_ISI_D7                           (1UL << 27)  /**< ISI signal: ISID7 */
#define PIN_PD27D_ISI_D8                           (123L)       /**< ISI signal: ISI_D8 on PD27 mux D*/
#define MUX_PD27D_ISI_D8                           (3L)         /**< ISI signal line function value: ISI_D8 */
#define PIO_PD27D_ISI_D8                           (1UL << 27)  /**< ISI signal: ISID8 */
#define PIN_PD28D_ISI_D9                           (124L)       /**< ISI signal: ISI_D9 on PD28 mux D*/
#define MUX_PD28D_ISI_D9                           (3L)         /**< ISI signal line function value: ISI_D9 */
#define PIO_PD28D_ISI_D9                           (1UL << 28)  /**< ISI signal: ISID9 */
#define PIN_PD30D_ISI_D10                          (126L)       /**< ISI signal: ISI_D10 on PD30 mux D*/
#define MUX_PD30D_ISI_D10                          (3L)         /**< ISI signal line function value: ISI_D10 */
#define PIO_PD30D_ISI_D10                          (1UL << 30)  /**< ISI signal: ISID10 */
#define PIN_PD31D_ISI_D11                          (127L)       /**< ISI signal: ISI_D11 on PD31 mux D*/
#define MUX_PD31D_ISI_D11                          (3L)         /**< ISI signal line function value: ISI_D11 */
#define PIO_PD31D_ISI_D11                          (1UL << 31)  /**< ISI signal: ISID11 */
#define PIN_PD24D_ISI_HSYNC                        (120L)       /**< ISI signal: ISI_HSYNC on PD24 mux D*/
#define MUX_PD24D_ISI_HSYNC                        (3L)         /**< ISI signal line function value: ISI_HSYNC */
#define PIO_PD24D_ISI_HSYNC                        (1UL << 24)  /**< ISI signal: ISIHSYNC */
#define PIN_PA24D_ISI_PCK                          (24L)        /**< ISI signal: ISI_PCK on PA24 mux D*/
#define MUX_PA24D_ISI_PCK                          (3L)         /**< ISI signal line function value: ISI_PCK */
#define PIO_PA24D_ISI_PCK                          (1UL << 24)  /**< ISI signal: ISIPCK */
#define PIN_PD25D_ISI_VSYNC                        (121L)       /**< ISI signal: ISI_VSYNC on PD25 mux D*/
#define MUX_PD25D_ISI_VSYNC                        (3L)         /**< ISI signal line function value: ISI_VSYNC */
#define PIO_PD25D_ISI_VSYNC                        (1UL << 25)  /**< ISI signal: ISIVSYNC */
/* ========== PIO definition for MCAN0 peripheral ========== */
#define PIN_PB3A_MCAN0_CANRX0                      (35L)        /**< MCAN0 signal: MCAN0_CANRX0 on PB3 mux A*/
#define MUX_PB3A_MCAN0_CANRX0                      (0L)         /**< MCAN0 signal line function value: MCAN0_CANRX0 */
#define PIO_PB3A_MCAN0_CANRX0                      (1UL << 3)   /**< MCAN0 signal: MCAN0CANRX0 */
#define PIN_PB2A_MCAN0_CANTX0                      (34L)        /**< MCAN0 signal: MCAN0_CANTX0 on PB2 mux A*/
#define MUX_PB2A_MCAN0_CANTX0                      (0L)         /**< MCAN0 signal line function value: MCAN0_CANTX0 */
#define PIO_PB2A_MCAN0_CANTX0                      (1UL << 2)   /**< MCAN0 signal: MCAN0CANTX0 */
/* ========== PIO definition for MCAN1 peripheral ========== */
#define PIN_PC12C_MCAN1_CANRX1                     (76L)        /**< MCAN1 signal: MCAN1_CANRX1 on PC12 mux C*/
#define MUX_PC12C_MCAN1_CANRX1                     (2L)         /**< MCAN1 signal line function value: MCAN1_CANRX1 */
#define PIO_PC12C_MCAN1_CANRX1                     (1UL << 12)  /**< MCAN1 signal: MCAN1CANRX1 */
#define PIN_PD28B_MCAN1_CANRX1                     (124L)       /**< MCAN1 signal: MCAN1_CANRX1 on PD28 mux B*/
#define MUX_PD28B_MCAN1_CANRX1                     (1L)         /**< MCAN1 signal line function value: MCAN1_CANRX1 */
#define PIO_PD28B_MCAN1_CANRX1                     (1UL << 28)  /**< MCAN1 signal: MCAN1CANRX1 */
#define PIN_PC14C_MCAN1_CANTX1                     (78L)        /**< MCAN1 signal: MCAN1_CANTX1 on PC14 mux C*/
#define MUX_PC14C_MCAN1_CANTX1                     (2L)         /**< MCAN1 signal line function value: MCAN1_CANTX1 */
#define PIO_PC14C_MCAN1_CANTX1                     (1UL << 14)  /**< MCAN1 signal: MCAN1CANTX1 */
#define PIN_PD12B_MCAN1_CANTX1                     (108L)       /**< MCAN1 signal: MCAN1_CANTX1 on PD12 mux B*/
#define MUX_PD12B_MCAN1_CANTX1                     (1L)         /**< MCAN1 signal line function value: MCAN1_CANTX1 */
#define PIO_PD12B_MCAN1_CANTX1                     (1UL << 12)  /**< MCAN1 signal: MCAN1CANTX1 */
/* ========== PIO definition for PMC peripheral ========== */
#define PIN_PA6B_PMC_PCK0                          (6L)         /**< PMC signal: PMC_PCK0 on PA6 mux B*/
#define MUX_PA6B_PMC_PCK0                          (1L)         /**< PMC signal line function value: PMC_PCK0 */
#define PIO_PA6B_PMC_PCK0                          (1UL << 6)   /**< PMC signal: PMCPCK0 */
#define PIN_PB12D_PMC_PCK0                         (44L)        /**< PMC signal: PMC_PCK0 on PB12 mux D*/
#define MUX_PB12D_PMC_PCK0                         (3L)         /**< PMC signal line function value: PMC_PCK0 */
#define PIO_PB12D_PMC_PCK0                         (1UL << 12)  /**< PMC signal: PMCPCK0 */
#define PIN_PB13B_PMC_PCK0                         (45L)        /**< PMC signal: PMC_PCK0 on PB13 mux B*/
#define MUX_PB13B_PMC_PCK0                         (1L)         /**< PMC signal line function value: PMC_PCK0 */
#define PIO_PB13B_PMC_PCK0                         (1UL << 13)  /**< PMC signal: PMCPCK0 */
#define PIN_PA17B_PMC_PCK1                         (17L)        /**< PMC signal: PMC_PCK1 on PA17 mux B*/
#define MUX_PA17B_PMC_PCK1                         (1L)         /**< PMC signal line function value: PMC_PCK1 */
#define PIO_PA17B_PMC_PCK1                         (1UL << 17)  /**< PMC signal: PMCPCK1 */
#define PIN_PA21B_PMC_PCK1                         (21L)        /**< PMC signal: PMC_PCK1 on PA21 mux B*/
#define MUX_PA21B_PMC_PCK1                         (1L)         /**< PMC signal line function value: PMC_PCK1 */
#define PIO_PA21B_PMC_PCK1                         (1UL << 21)  /**< PMC signal: PMCPCK1 */
#define PIN_PA3C_PMC_PCK2                          (3L)         /**< PMC signal: PMC_PCK2 on PA3 mux C*/
#define MUX_PA3C_PMC_PCK2                          (2L)         /**< PMC signal line function value: PMC_PCK2 */
#define PIO_PA3C_PMC_PCK2                          (1UL << 3)   /**< PMC signal: PMCPCK2 */
#define PIN_PA18B_PMC_PCK2                         (18L)        /**< PMC signal: PMC_PCK2 on PA18 mux B*/
#define MUX_PA18B_PMC_PCK2                         (1L)         /**< PMC signal line function value: PMC_PCK2 */
#define PIO_PA18B_PMC_PCK2                         (1UL << 18)  /**< PMC signal: PMCPCK2 */
#define PIN_PA31B_PMC_PCK2                         (31L)        /**< PMC signal: PMC_PCK2 on PA31 mux B*/
#define MUX_PA31B_PMC_PCK2                         (1L)         /**< PMC signal line function value: PMC_PCK2 */
#define PIO_PA31B_PMC_PCK2                         (1UL << 31)  /**< PMC signal: PMCPCK2 */
#define PIN_PB3B_PMC_PCK2                          (35L)        /**< PMC signal: PMC_PCK2 on PB3 mux B*/
#define MUX_PB3B_PMC_PCK2                          (1L)         /**< PMC signal line function value: PMC_PCK2 */
#define PIO_PB3B_PMC_PCK2                          (1UL << 3)   /**< PMC signal: PMCPCK2 */
#define PIN_PD31C_PMC_PCK2                         (127L)       /**< PMC signal: PMC_PCK2 on PD31 mux C*/
#define MUX_PD31C_PMC_PCK2                         (2L)         /**< PMC signal line function value: PMC_PCK2 */
#define PIO_PD31C_PMC_PCK2                         (1UL << 31)  /**< PMC signal: PMCPCK2 */
#define PIN_PB9X1_PMC_XIN                          (41L)        /**< PMC signal: PMC_XIN on PB9 mux X1*/
#define PIO_PB9X1_PMC_XIN                          (1UL << 9)   /**< PMC signal: PMCXIN */
#define PIN_PB8X1_PMC_XOUT                         (40L)        /**< PMC signal: PMC_XOUT on PB8 mux X1*/
#define PIO_PB8X1_PMC_XOUT                         (1UL << 8)   /**< PMC signal: PMCXOUT */
#define PIN_PA7X1_PMC_XIN32                        (7L)         /**< PMC signal: PMC_XIN32 on PA7 mux X1*/
#define PIO_PA7X1_PMC_XIN32                        (1UL << 7)   /**< PMC signal: PMCXIN32 */
#define PIN_PA8X1_PMC_XOUT32                       (8L)         /**< PMC signal: PMC_XOUT32 on PA8 mux X1*/
#define PIO_PA8X1_PMC_XOUT32                       (1UL << 8)   /**< PMC signal: PMCXOUT32 */
/* ========== PIO definition for PWM0 peripheral ========== */
#define PIN_PA10B_PWM0_PWMEXTRG0                   (10L)        /**< PWM0 signal: PWM0_PWMEXTRG0 on PA10 mux B*/
#define MUX_PA10B_PWM0_PWMEXTRG0                   (1L)         /**< PWM0 signal line function value: PWM0_PWMEXTRG0 */
#define PIO_PA10B_PWM0_PWMEXTRG0                   (1UL << 10)  /**< PWM0 signal: PWM0PWMEXTRG0 */
#define PIN_PA22B_PWM0_PWMEXTRG1                   (22L)        /**< PWM0 signal: PWM0_PWMEXTRG1 on PA22 mux B*/
#define MUX_PA22B_PWM0_PWMEXTRG1                   (1L)         /**< PWM0 signal line function value: PWM0_PWMEXTRG1 */
#define PIO_PA22B_PWM0_PWMEXTRG1                   (1UL << 22)  /**< PWM0 signal: PWM0PWMEXTRG1 */
#define PIN_PA9C_PWM0_PWMFI0                       (9L)         /**< PWM0 signal: PWM0_PWMFI0 on PA9 mux C*/
#define MUX_PA9C_PWM0_PWMFI0                       (2L)         /**< PWM0 signal line function value: PWM0_PWMFI0 */
#define PIO_PA9C_PWM0_PWMFI0                       (1UL << 9)   /**< PWM0 signal: PWM0PWMFI0 */
#define PIN_PD8B_PWM0_PWMFI1                       (104L)       /**< PWM0 signal: PWM0_PWMFI1 on PD8 mux B*/
#define MUX_PD8B_PWM0_PWMFI1                       (1L)         /**< PWM0 signal line function value: PWM0_PWMFI1 */
#define PIO_PD8B_PWM0_PWMFI1                       (1UL << 8)   /**< PWM0 signal: PWM0PWMFI1 */
#define PIN_PD9B_PWM0_PWMFI2                       (105L)       /**< PWM0 signal: PWM0_PWMFI2 on PD9 mux B*/
#define MUX_PD9B_PWM0_PWMFI2                       (1L)         /**< PWM0 signal line function value: PWM0_PWMFI2 */
#define PIO_PD9B_PWM0_PWMFI2                       (1UL << 9)   /**< PWM0 signal: PWM0PWMFI2 */
#define PIN_PA0A_PWM0_PWMH0                        (0L)         /**< PWM0 signal: PWM0_PWMH0 on PA0 mux A*/
#define MUX_PA0A_PWM0_PWMH0                        (0L)         /**< PWM0 signal line function value: PWM0_PWMH0 */
#define PIO_PA0A_PWM0_PWMH0                        (1UL << 0)   /**< PWM0 signal: PWM0PWMH0 */
#define PIN_PA11B_PWM0_PWMH0                       (11L)        /**< PWM0 signal: PWM0_PWMH0 on PA11 mux B*/
#define MUX_PA11B_PWM0_PWMH0                       (1L)         /**< PWM0 signal line function value: PWM0_PWMH0 */
#define PIO_PA11B_PWM0_PWMH0                       (1UL << 11)  /**< PWM0 signal: PWM0PWMH0 */
#define PIN_PA23B_PWM0_PWMH0                       (23L)        /**< PWM0 signal: PWM0_PWMH0 on PA23 mux B*/
#define MUX_PA23B_PWM0_PWMH0                       (1L)         /**< PWM0 signal line function value: PWM0_PWMH0 */
#define PIO_PA23B_PWM0_PWMH0                       (1UL << 23)  /**< PWM0 signal: PWM0PWMH0 */
#define PIN_PB0A_PWM0_PWMH0                        (32L)        /**< PWM0 signal: PWM0_PWMH0 on PB0 mux A*/
#define MUX_PB0A_PWM0_PWMH0                        (0L)         /**< PWM0 signal line function value: PWM0_PWMH0 */
#define PIO_PB0A_PWM0_PWMH0                        (1UL << 0)   /**< PWM0 signal: PWM0PWMH0 */
#define PIN_PD11B_PWM0_PWMH0                       (107L)       /**< PWM0 signal: PWM0_PWMH0 on PD11 mux B*/
#define MUX_PD11B_PWM0_PWMH0                       (1L)         /**< PWM0 signal line function value: PWM0_PWMH0 */
#define PIO_PD11B_PWM0_PWMH0                       (1UL << 11)  /**< PWM0 signal: PWM0PWMH0 */
#define PIN_PD20A_PWM0_PWMH0                       (116L)       /**< PWM0 signal: PWM0_PWMH0 on PD20 mux A*/
#define MUX_PD20A_PWM0_PWMH0                       (0L)         /**< PWM0 signal line function value: PWM0_PWMH0 */
#define PIO_PD20A_PWM0_PWMH0                       (1UL << 20)  /**< PWM0 signal: PWM0PWMH0 */
#define PIN_PA2A_PWM0_PWMH1                        (2L)         /**< PWM0 signal: PWM0_PWMH1 on PA2 mux A*/
#define MUX_PA2A_PWM0_PWMH1                        (0L)         /**< PWM0 signal line function value: PWM0_PWMH1 */
#define PIO_PA2A_PWM0_PWMH1                        (1UL << 2)   /**< PWM0 signal: PWM0PWMH1 */
#define PIN_PA12B_PWM0_PWMH1                       (12L)        /**< PWM0 signal: PWM0_PWMH1 on PA12 mux B*/
#define MUX_PA12B_PWM0_PWMH1                       (1L)         /**< PWM0 signal line function value: PWM0_PWMH1 */
#define PIO_PA12B_PWM0_PWMH1                       (1UL << 12)  /**< PWM0 signal: PWM0PWMH1 */
#define PIN_PA24B_PWM0_PWMH1                       (24L)        /**< PWM0 signal: PWM0_PWMH1 on PA24 mux B*/
#define MUX_PA24B_PWM0_PWMH1                       (1L)         /**< PWM0 signal line function value: PWM0_PWMH1 */
#define PIO_PA24B_PWM0_PWMH1                       (1UL << 24)  /**< PWM0 signal: PWM0PWMH1 */
#define PIN_PB1A_PWM0_PWMH1                        (33L)        /**< PWM0 signal: PWM0_PWMH1 on PB1 mux A*/
#define MUX_PB1A_PWM0_PWMH1                        (0L)         /**< PWM0 signal line function value: PWM0_PWMH1 */
#define PIO_PB1A_PWM0_PWMH1                        (1UL << 1)   /**< PWM0 signal: PWM0PWMH1 */
#define PIN_PD21A_PWM0_PWMH1                       (117L)       /**< PWM0 signal: PWM0_PWMH1 on PD21 mux A*/
#define MUX_PD21A_PWM0_PWMH1                       (0L)         /**< PWM0 signal line function value: PWM0_PWMH1 */
#define PIO_PD21A_PWM0_PWMH1                       (1UL << 21)  /**< PWM0 signal: PWM0PWMH1 */
#define PIN_PA13B_PWM0_PWMH2                       (13L)        /**< PWM0 signal: PWM0_PWMH2 on PA13 mux B*/
#define MUX_PA13B_PWM0_PWMH2                       (1L)         /**< PWM0 signal line function value: PWM0_PWMH2 */
#define PIO_PA13B_PWM0_PWMH2                       (1UL << 13)  /**< PWM0 signal: PWM0PWMH2 */
#define PIN_PA25B_PWM0_PWMH2                       (25L)        /**< PWM0 signal: PWM0_PWMH2 on PA25 mux B*/
#define MUX_PA25B_PWM0_PWMH2                       (1L)         /**< PWM0 signal line function value: PWM0_PWMH2 */
#define PIO_PA25B_PWM0_PWMH2                       (1UL << 25)  /**< PWM0 signal: PWM0PWMH2 */
#define PIN_PB4B_PWM0_PWMH2                        (36L)        /**< PWM0 signal: PWM0_PWMH2 on PB4 mux B*/
#define MUX_PB4B_PWM0_PWMH2                        (1L)         /**< PWM0 signal line function value: PWM0_PWMH2 */
#define PIO_PB4B_PWM0_PWMH2                        (1UL << 4)   /**< PWM0 signal: PWM0PWMH2 */
#define PIN_PC19B_PWM0_PWMH2                       (83L)        /**< PWM0 signal: PWM0_PWMH2 on PC19 mux B*/
#define MUX_PC19B_PWM0_PWMH2                       (1L)         /**< PWM0 signal line function value: PWM0_PWMH2 */
#define PIO_PC19B_PWM0_PWMH2                       (1UL << 19)  /**< PWM0 signal: PWM0PWMH2 */
#define PIN_PD22A_PWM0_PWMH2                       (118L)       /**< PWM0 signal: PWM0_PWMH2 on PD22 mux A*/
#define MUX_PD22A_PWM0_PWMH2                       (0L)         /**< PWM0 signal line function value: PWM0_PWMH2 */
#define PIO_PD22A_PWM0_PWMH2                       (1UL << 22)  /**< PWM0 signal: PWM0PWMH2 */
#define PIN_PA7B_PWM0_PWMH3                        (7L)         /**< PWM0 signal: PWM0_PWMH3 on PA7 mux B*/
#define MUX_PA7B_PWM0_PWMH3                        (1L)         /**< PWM0 signal line function value: PWM0_PWMH3 */
#define PIO_PA7B_PWM0_PWMH3                        (1UL << 7)   /**< PWM0 signal: PWM0PWMH3 */
#define PIN_PA14B_PWM0_PWMH3                       (14L)        /**< PWM0 signal: PWM0_PWMH3 on PA14 mux B*/
#define MUX_PA14B_PWM0_PWMH3                       (1L)         /**< PWM0 signal line function value: PWM0_PWMH3 */
#define PIO_PA14B_PWM0_PWMH3                       (1UL << 14)  /**< PWM0 signal: PWM0PWMH3 */
#define PIN_PA17C_PWM0_PWMH3                       (17L)        /**< PWM0 signal: PWM0_PWMH3 on PA17 mux C*/
#define MUX_PA17C_PWM0_PWMH3                       (2L)         /**< PWM0 signal line function value: PWM0_PWMH3 */
#define PIO_PA17C_PWM0_PWMH3                       (1UL << 17)  /**< PWM0 signal: PWM0PWMH3 */
#define PIN_PC13B_PWM0_PWMH3                       (77L)        /**< PWM0 signal: PWM0_PWMH3 on PC13 mux B*/
#define MUX_PC13B_PWM0_PWMH3                       (1L)         /**< PWM0 signal line function value: PWM0_PWMH3 */
#define PIO_PC13B_PWM0_PWMH3                       (1UL << 13)  /**< PWM0 signal: PWM0PWMH3 */
#define PIN_PC21B_PWM0_PWMH3                       (85L)        /**< PWM0 signal: PWM0_PWMH3 on PC21 mux B*/
#define MUX_PC21B_PWM0_PWMH3                       (1L)         /**< PWM0 signal line function value: PWM0_PWMH3 */
#define PIO_PC21B_PWM0_PWMH3                       (1UL << 21)  /**< PWM0 signal: PWM0PWMH3 */
#define PIN_PD23A_PWM0_PWMH3                       (119L)       /**< PWM0 signal: PWM0_PWMH3 on PD23 mux A*/
#define MUX_PD23A_PWM0_PWMH3                       (0L)         /**< PWM0 signal line function value: PWM0_PWMH3 */
#define PIO_PD23A_PWM0_PWMH3                       (1UL << 23)  /**< PWM0 signal: PWM0PWMH3 */
#define PIN_PA1A_PWM0_PWML0                        (1L)         /**< PWM0 signal: PWM0_PWML0 on PA1 mux A*/
#define MUX_PA1A_PWM0_PWML0                        (0L)         /**< PWM0 signal line function value: PWM0_PWML0 */
#define PIO_PA1A_PWM0_PWML0                        (1UL << 1)   /**< PWM0 signal: PWM0PWML0 */
#define PIN_PA19B_PWM0_PWML0                       (19L)        /**< PWM0 signal: PWM0_PWML0 on PA19 mux B*/
#define MUX_PA19B_PWM0_PWML0                       (1L)         /**< PWM0 signal line function value: PWM0_PWML0 */
#define PIO_PA19B_PWM0_PWML0                       (1UL << 19)  /**< PWM0 signal: PWM0PWML0 */
#define PIN_PB5B_PWM0_PWML0                        (37L)        /**< PWM0 signal: PWM0_PWML0 on PB5 mux B*/
#define MUX_PB5B_PWM0_PWML0                        (1L)         /**< PWM0 signal line function value: PWM0_PWML0 */
#define PIO_PB5B_PWM0_PWML0                        (1UL << 5)   /**< PWM0 signal: PWM0PWML0 */
#define PIN_PC0B_PWM0_PWML0                        (64L)        /**< PWM0 signal: PWM0_PWML0 on PC0 mux B*/
#define MUX_PC0B_PWM0_PWML0                        (1L)         /**< PWM0 signal line function value: PWM0_PWML0 */
#define PIO_PC0B_PWM0_PWML0                        (1UL << 0)   /**< PWM0 signal: PWM0PWML0 */
#define PIN_PD10B_PWM0_PWML0                       (106L)       /**< PWM0 signal: PWM0_PWML0 on PD10 mux B*/
#define MUX_PD10B_PWM0_PWML0                       (1L)         /**< PWM0 signal line function value: PWM0_PWML0 */
#define PIO_PD10B_PWM0_PWML0                       (1UL << 10)  /**< PWM0 signal: PWM0PWML0 */
#define PIN_PD24A_PWM0_PWML0                       (120L)       /**< PWM0 signal: PWM0_PWML0 on PD24 mux A*/
#define MUX_PD24A_PWM0_PWML0                       (0L)         /**< PWM0 signal line function value: PWM0_PWML0 */
#define PIO_PD24A_PWM0_PWML0                       (1UL << 24)  /**< PWM0 signal: PWM0PWML0 */
#define PIN_PA20B_PWM0_PWML1                       (20L)        /**< PWM0 signal: PWM0_PWML1 on PA20 mux B*/
#define MUX_PA20B_PWM0_PWML1                       (1L)         /**< PWM0 signal line function value: PWM0_PWML1 */
#define PIO_PA20B_PWM0_PWML1                       (1UL << 20)  /**< PWM0 signal: PWM0PWML1 */
#define PIN_PB12A_PWM0_PWML1                       (44L)        /**< PWM0 signal: PWM0_PWML1 on PB12 mux A*/
#define MUX_PB12A_PWM0_PWML1                       (0L)         /**< PWM0 signal line function value: PWM0_PWML1 */
#define PIO_PB12A_PWM0_PWML1                       (1UL << 12)  /**< PWM0 signal: PWM0PWML1 */
#define PIN_PC1B_PWM0_PWML1                        (65L)        /**< PWM0 signal: PWM0_PWML1 on PC1 mux B*/
#define MUX_PC1B_PWM0_PWML1                        (1L)         /**< PWM0 signal line function value: PWM0_PWML1 */
#define PIO_PC1B_PWM0_PWML1                        (1UL << 1)   /**< PWM0 signal: PWM0PWML1 */
#define PIN_PC18B_PWM0_PWML1                       (82L)        /**< PWM0 signal: PWM0_PWML1 on PC18 mux B*/
#define MUX_PC18B_PWM0_PWML1                       (1L)         /**< PWM0 signal line function value: PWM0_PWML1 */
#define PIO_PC18B_PWM0_PWML1                       (1UL << 18)  /**< PWM0 signal: PWM0PWML1 */
#define PIN_PD25A_PWM0_PWML1                       (121L)       /**< PWM0 signal: PWM0_PWML1 on PD25 mux A*/
#define MUX_PD25A_PWM0_PWML1                       (0L)         /**< PWM0 signal line function value: PWM0_PWML1 */
#define PIO_PD25A_PWM0_PWML1                       (1UL << 25)  /**< PWM0 signal: PWM0PWML1 */
#define PIN_PA16C_PWM0_PWML2                       (16L)        /**< PWM0 signal: PWM0_PWML2 on PA16 mux C*/
#define MUX_PA16C_PWM0_PWML2                       (2L)         /**< PWM0 signal line function value: PWM0_PWML2 */
#define PIO_PA16C_PWM0_PWML2                       (1UL << 16)  /**< PWM0 signal: PWM0PWML2 */
#define PIN_PA30A_PWM0_PWML2                       (30L)        /**< PWM0 signal: PWM0_PWML2 on PA30 mux A*/
#define MUX_PA30A_PWM0_PWML2                       (0L)         /**< PWM0 signal line function value: PWM0_PWML2 */
#define PIO_PA30A_PWM0_PWML2                       (1UL << 30)  /**< PWM0 signal: PWM0PWML2 */
#define PIN_PB13A_PWM0_PWML2                       (45L)        /**< PWM0 signal: PWM0_PWML2 on PB13 mux A*/
#define MUX_PB13A_PWM0_PWML2                       (0L)         /**< PWM0 signal line function value: PWM0_PWML2 */
#define PIO_PB13A_PWM0_PWML2                       (1UL << 13)  /**< PWM0 signal: PWM0PWML2 */
#define PIN_PC2B_PWM0_PWML2                        (66L)        /**< PWM0 signal: PWM0_PWML2 on PC2 mux B*/
#define MUX_PC2B_PWM0_PWML2                        (1L)         /**< PWM0 signal line function value: PWM0_PWML2 */
#define PIO_PC2B_PWM0_PWML2                        (1UL << 2)   /**< PWM0 signal: PWM0PWML2 */
#define PIN_PC20B_PWM0_PWML2                       (84L)        /**< PWM0 signal: PWM0_PWML2 on PC20 mux B*/
#define MUX_PC20B_PWM0_PWML2                       (1L)         /**< PWM0 signal line function value: PWM0_PWML2 */
#define PIO_PC20B_PWM0_PWML2                       (1UL << 20)  /**< PWM0 signal: PWM0PWML2 */
#define PIN_PD26A_PWM0_PWML2                       (122L)       /**< PWM0 signal: PWM0_PWML2 on PD26 mux A*/
#define MUX_PD26A_PWM0_PWML2                       (0L)         /**< PWM0 signal line function value: PWM0_PWML2 */
#define PIO_PD26A_PWM0_PWML2                       (1UL << 26)  /**< PWM0 signal: PWM0PWML2 */
#define PIN_PA15C_PWM0_PWML3                       (15L)        /**< PWM0 signal: PWM0_PWML3 on PA15 mux C*/
#define MUX_PA15C_PWM0_PWML3                       (2L)         /**< PWM0 signal line function value: PWM0_PWML3 */
#define PIO_PA15C_PWM0_PWML3                       (1UL << 15)  /**< PWM0 signal: PWM0PWML3 */
#define PIN_PC3B_PWM0_PWML3                        (67L)        /**< PWM0 signal: PWM0_PWML3 on PC3 mux B*/
#define MUX_PC3B_PWM0_PWML3                        (1L)         /**< PWM0 signal line function value: PWM0_PWML3 */
#define PIO_PC3B_PWM0_PWML3                        (1UL << 3)   /**< PWM0 signal: PWM0PWML3 */
#define PIN_PC15B_PWM0_PWML3                       (79L)        /**< PWM0 signal: PWM0_PWML3 on PC15 mux B*/
#define MUX_PC15B_PWM0_PWML3                       (1L)         /**< PWM0 signal line function value: PWM0_PWML3 */
#define PIO_PC15B_PWM0_PWML3                       (1UL << 15)  /**< PWM0 signal: PWM0PWML3 */
#define PIN_PC22B_PWM0_PWML3                       (86L)        /**< PWM0 signal: PWM0_PWML3 on PC22 mux B*/
#define MUX_PC22B_PWM0_PWML3                       (1L)         /**< PWM0 signal line function value: PWM0_PWML3 */
#define PIO_PC22B_PWM0_PWML3                       (1UL << 22)  /**< PWM0 signal: PWM0PWML3 */
#define PIN_PD27A_PWM0_PWML3                       (123L)       /**< PWM0 signal: PWM0_PWML3 on PD27 mux A*/
#define MUX_PD27A_PWM0_PWML3                       (0L)         /**< PWM0 signal line function value: PWM0_PWML3 */
#define PIO_PD27A_PWM0_PWML3                       (1UL << 27)  /**< PWM0 signal: PWM0PWML3 */
/* ========== PIO definition for PWM1 peripheral ========== */
#define PIN_PA30B_PWM1_PWMEXTRG0                   (30L)        /**< PWM1 signal: PWM1_PWMEXTRG0 on PA30 mux B*/
#define MUX_PA30B_PWM1_PWMEXTRG0                   (1L)         /**< PWM1 signal line function value: PWM1_PWMEXTRG0 */
#define PIO_PA30B_PWM1_PWMEXTRG0                   (1UL << 30)  /**< PWM1 signal: PWM1PWMEXTRG0 */
#define PIN_PA18A_PWM1_PWMEXTRG1                   (18L)        /**< PWM1 signal: PWM1_PWMEXTRG1 on PA18 mux A*/
#define MUX_PA18A_PWM1_PWMEXTRG1                   (0L)         /**< PWM1 signal line function value: PWM1_PWMEXTRG1 */
#define PIO_PA18A_PWM1_PWMEXTRG1                   (1UL << 18)  /**< PWM1 signal: PWM1PWMEXTRG1 */
#define PIN_PA21C_PWM1_PWMFI0                      (21L)        /**< PWM1 signal: PWM1_PWMFI0 on PA21 mux C*/
#define MUX_PA21C_PWM1_PWMFI0                      (2L)         /**< PWM1 signal line function value: PWM1_PWMFI0 */
#define PIO_PA21C_PWM1_PWMFI0                      (1UL << 21)  /**< PWM1 signal: PWM1PWMFI0 */
#define PIN_PA26D_PWM1_PWMFI1                      (26L)        /**< PWM1 signal: PWM1_PWMFI1 on PA26 mux D*/
#define MUX_PA26D_PWM1_PWMFI1                      (3L)         /**< PWM1 signal line function value: PWM1_PWMFI1 */
#define PIO_PA26D_PWM1_PWMFI1                      (1UL << 26)  /**< PWM1 signal: PWM1PWMFI1 */
#define PIN_PA28D_PWM1_PWMFI2                      (28L)        /**< PWM1 signal: PWM1_PWMFI2 on PA28 mux D*/
#define MUX_PA28D_PWM1_PWMFI2                      (3L)         /**< PWM1 signal line function value: PWM1_PWMFI2 */
#define PIO_PA28D_PWM1_PWMFI2                      (1UL << 28)  /**< PWM1 signal: PWM1PWMFI2 */
#define PIN_PA12C_PWM1_PWMH0                       (12L)        /**< PWM1 signal: PWM1_PWMH0 on PA12 mux C*/
#define MUX_PA12C_PWM1_PWMH0                       (2L)         /**< PWM1 signal line function value: PWM1_PWMH0 */
#define PIO_PA12C_PWM1_PWMH0                       (1UL << 12)  /**< PWM1 signal: PWM1PWMH0 */
#define PIN_PD1B_PWM1_PWMH0                        (97L)        /**< PWM1 signal: PWM1_PWMH0 on PD1 mux B*/
#define MUX_PD1B_PWM1_PWMH0                        (1L)         /**< PWM1 signal line function value: PWM1_PWMH0 */
#define PIO_PD1B_PWM1_PWMH0                        (1UL << 1)   /**< PWM1 signal: PWM1PWMH0 */
#define PIN_PA14C_PWM1_PWMH1                       (14L)        /**< PWM1 signal: PWM1_PWMH1 on PA14 mux C*/
#define MUX_PA14C_PWM1_PWMH1                       (2L)         /**< PWM1 signal line function value: PWM1_PWMH1 */
#define PIO_PA14C_PWM1_PWMH1                       (1UL << 14)  /**< PWM1 signal: PWM1PWMH1 */
#define PIN_PD3B_PWM1_PWMH1                        (99L)        /**< PWM1 signal: PWM1_PWMH1 on PD3 mux B*/
#define MUX_PD3B_PWM1_PWMH1                        (1L)         /**< PWM1 signal line function value: PWM1_PWMH1 */
#define PIO_PD3B_PWM1_PWMH1                        (1UL << 3)   /**< PWM1 signal: PWM1PWMH1 */
#define PIN_PA31D_PWM1_PWMH2                       (31L)        /**< PWM1 signal: PWM1_PWMH2 on PA31 mux D*/
#define MUX_PA31D_PWM1_PWMH2                       (3L)         /**< PWM1 signal line function value: PWM1_PWMH2 */
#define PIO_PA31D_PWM1_PWMH2                       (1UL << 31)  /**< PWM1 signal: PWM1PWMH2 */
#define PIN_PD5B_PWM1_PWMH2                        (101L)       /**< PWM1 signal: PWM1_PWMH2 on PD5 mux B*/
#define MUX_PD5B_PWM1_PWMH2                        (1L)         /**< PWM1 signal line function value: PWM1_PWMH2 */
#define PIO_PD5B_PWM1_PWMH2                        (1UL << 5)   /**< PWM1 signal: PWM1PWMH2 */
#define PIN_PA8A_PWM1_PWMH3                        (8L)         /**< PWM1 signal: PWM1_PWMH3 on PA8 mux A*/
#define MUX_PA8A_PWM1_PWMH3                        (0L)         /**< PWM1 signal line function value: PWM1_PWMH3 */
#define PIO_PA8A_PWM1_PWMH3                        (1UL << 8)   /**< PWM1 signal: PWM1PWMH3 */
#define PIN_PD7B_PWM1_PWMH3                        (103L)       /**< PWM1 signal: PWM1_PWMH3 on PD7 mux B*/
#define MUX_PD7B_PWM1_PWMH3                        (1L)         /**< PWM1 signal line function value: PWM1_PWMH3 */
#define PIO_PD7B_PWM1_PWMH3                        (1UL << 7)   /**< PWM1 signal: PWM1PWMH3 */
#define PIN_PA11C_PWM1_PWML0                       (11L)        /**< PWM1 signal: PWM1_PWML0 on PA11 mux C*/
#define MUX_PA11C_PWM1_PWML0                       (2L)         /**< PWM1 signal line function value: PWM1_PWML0 */
#define PIO_PA11C_PWM1_PWML0                       (1UL << 11)  /**< PWM1 signal: PWM1PWML0 */
#define PIN_PD0B_PWM1_PWML0                        (96L)        /**< PWM1 signal: PWM1_PWML0 on PD0 mux B*/
#define MUX_PD0B_PWM1_PWML0                        (1L)         /**< PWM1 signal line function value: PWM1_PWML0 */
#define PIO_PD0B_PWM1_PWML0                        (1UL << 0)   /**< PWM1 signal: PWM1PWML0 */
#define PIN_PA13C_PWM1_PWML1                       (13L)        /**< PWM1 signal: PWM1_PWML1 on PA13 mux C*/
#define MUX_PA13C_PWM1_PWML1                       (2L)         /**< PWM1 signal line function value: PWM1_PWML1 */
#define PIO_PA13C_PWM1_PWML1                       (1UL << 13)  /**< PWM1 signal: PWM1PWML1 */
#define PIN_PD2B_PWM1_PWML1                        (98L)        /**< PWM1 signal: PWM1_PWML1 on PD2 mux B*/
#define MUX_PD2B_PWM1_PWML1                        (1L)         /**< PWM1 signal line function value: PWM1_PWML1 */
#define PIO_PD2B_PWM1_PWML1                        (1UL << 2)   /**< PWM1 signal: PWM1PWML1 */
#define PIN_PA23D_PWM1_PWML2                       (23L)        /**< PWM1 signal: PWM1_PWML2 on PA23 mux D*/
#define MUX_PA23D_PWM1_PWML2                       (3L)         /**< PWM1 signal line function value: PWM1_PWML2 */
#define PIO_PA23D_PWM1_PWML2                       (1UL << 23)  /**< PWM1 signal: PWM1PWML2 */
#define PIN_PD4B_PWM1_PWML2                        (100L)       /**< PWM1 signal: PWM1_PWML2 on PD4 mux B*/
#define MUX_PD4B_PWM1_PWML2                        (1L)         /**< PWM1 signal line function value: PWM1_PWML2 */
#define PIO_PD4B_PWM1_PWML2                        (1UL << 4)   /**< PWM1 signal: PWM1PWML2 */
#define PIN_PA5A_PWM1_PWML3                        (5L)         /**< PWM1 signal: PWM1_PWML3 on PA5 mux A*/
#define MUX_PA5A_PWM1_PWML3                        (0L)         /**< PWM1 signal line function value: PWM1_PWML3 */
#define PIO_PA5A_PWM1_PWML3                        (1UL << 5)   /**< PWM1 signal: PWM1PWML3 */
#define PIN_PD6B_PWM1_PWML3                        (102L)       /**< PWM1 signal: PWM1_PWML3 on PD6 mux B*/
#define MUX_PD6B_PWM1_PWML3                        (1L)         /**< PWM1 signal line function value: PWM1_PWML3 */
#define PIO_PD6B_PWM1_PWML3                        (1UL << 6)   /**< PWM1 signal: PWM1PWML3 */
/* ========== PIO definition for QSPI peripheral ========== */
#define PIN_PA11A_QSPI_QCS                         (11L)        /**< QSPI signal: QSPI_QCS on PA11 mux A*/
#define MUX_PA11A_QSPI_QCS                         (0L)         /**< QSPI signal line function value: QSPI_QCS */
#define PIO_PA11A_QSPI_QCS                         (1UL << 11)  /**< QSPI signal: QSPIQCS */
#define PIN_PA13A_QSPI_QIO0                        (13L)        /**< QSPI signal: QSPI_QIO0 on PA13 mux A*/
#define MUX_PA13A_QSPI_QIO0                        (0L)         /**< QSPI signal line function value: QSPI_QIO0 */
#define PIO_PA13A_QSPI_QIO0                        (1UL << 13)  /**< QSPI signal: QSPIQIO0 */
#define PIN_PA12A_QSPI_QIO1                        (12L)        /**< QSPI signal: QSPI_QIO1 on PA12 mux A*/
#define MUX_PA12A_QSPI_QIO1                        (0L)         /**< QSPI signal line function value: QSPI_QIO1 */
#define PIO_PA12A_QSPI_QIO1                        (1UL << 12)  /**< QSPI signal: QSPIQIO1 */
#define PIN_PA17A_QSPI_QIO2                        (17L)        /**< QSPI signal: QSPI_QIO2 on PA17 mux A*/
#define MUX_PA17A_QSPI_QIO2                        (0L)         /**< QSPI signal line function value: QSPI_QIO2 */
#define PIO_PA17A_QSPI_QIO2                        (1UL << 17)  /**< QSPI signal: QSPIQIO2 */
#define PIN_PD31A_QSPI_QIO3                        (127L)       /**< QSPI signal: QSPI_QIO3 on PD31 mux A*/
#define MUX_PD31A_QSPI_QIO3                        (0L)         /**< QSPI signal line function value: QSPI_QIO3 */
#define PIO_PD31A_QSPI_QIO3                        (1UL << 31)  /**< QSPI signal: QSPIQIO3 */
#define PIN_PA14A_QSPI_QSCK                        (14L)        /**< QSPI signal: QSPI_QSCK on PA14 mux A*/
#define MUX_PA14A_QSPI_QSCK                        (0L)         /**< QSPI signal line function value: QSPI_QSCK */
#define PIO_PA14A_QSPI_QSCK                        (1UL << 14)  /**< QSPI signal: QSPIQSCK */
/* ========== PIO definition for RTC peripheral ========== */
#define PIN_PB0X1_RTC_RTCOUT0                      (32L)        /**< RTC signal: RTC_RTCOUT0 on PB0 mux X1*/
#define PIO_PB0X1_RTC_RTCOUT0                      (1UL << 0)   /**< RTC signal: RTCRTCOUT0 */
#define PIN_PB1X1_RTC_RTCOUT1                      (33L)        /**< RTC signal: RTC_RTCOUT1 on PB1 mux X1*/
#define PIO_PB1X1_RTC_RTCOUT1                      (1UL << 1)   /**< RTC signal: RTCRTCOUT1 */
/* ========== PIO definition for SPI0 peripheral ========== */
#define PIN_PD20B_SPI0_MISO                        (116L)       /**< SPI0 signal: SPI0_MISO on PD20 mux B*/
#define MUX_PD20B_SPI0_MISO                        (1L)         /**< SPI0 signal line function value: SPI0_MISO */
#define PIO_PD20B_SPI0_MISO                        (1UL << 20)  /**< SPI0 signal: SPI0MISO */
#define PIN_PD21B_SPI0_MOSI                        (117L)       /**< SPI0 signal: SPI0_MOSI on PD21 mux B*/
#define MUX_PD21B_SPI0_MOSI                        (1L)         /**< SPI0 signal line function value: SPI0_MOSI */
#define PIO_PD21B_SPI0_MOSI                        (1UL << 21)  /**< SPI0 signal: SPI0MOSI */
#define PIN_PB2D_SPI0_NPCS0                        (34L)        /**< SPI0 signal: SPI0_NPCS0 on PB2 mux D*/
#define MUX_PB2D_SPI0_NPCS0                        (3L)         /**< SPI0 signal line function value: SPI0_NPCS0 */
#define PIO_PB2D_SPI0_NPCS0                        (1UL << 2)   /**< SPI0 signal: SPI0NPCS0 */
#define PIN_PA31A_SPI0_NPCS1                       (31L)        /**< SPI0 signal: SPI0_NPCS1 on PA31 mux A*/
#define MUX_PA31A_SPI0_NPCS1                       (0L)         /**< SPI0 signal line function value: SPI0_NPCS1 */
#define PIO_PA31A_SPI0_NPCS1                       (1UL << 31)  /**< SPI0 signal: SPI0NPCS1 */
#define PIN_PD25B_SPI0_NPCS1                       (121L)       /**< SPI0 signal: SPI0_NPCS1 on PD25 mux B*/
#define MUX_PD25B_SPI0_NPCS1                       (1L)         /**< SPI0 signal line function value: SPI0_NPCS1 */
#define PIO_PD25B_SPI0_NPCS1                       (1UL << 25)  /**< SPI0 signal: SPI0NPCS1 */
#define PIN_PD12C_SPI0_NPCS2                       (108L)       /**< SPI0 signal: SPI0_NPCS2 on PD12 mux C*/
#define MUX_PD12C_SPI0_NPCS2                       (2L)         /**< SPI0 signal line function value: SPI0_NPCS2 */
#define PIO_PD12C_SPI0_NPCS2                       (1UL << 12)  /**< SPI0 signal: SPI0NPCS2 */
#define PIN_PD27B_SPI0_NPCS3                       (123L)       /**< SPI0 signal: SPI0_NPCS3 on PD27 mux B*/
#define MUX_PD27B_SPI0_NPCS3                       (1L)         /**< SPI0 signal line function value: SPI0_NPCS3 */
#define PIO_PD27B_SPI0_NPCS3                       (1UL << 27)  /**< SPI0 signal: SPI0NPCS3 */
#define PIN_PD22B_SPI0_SPCK                        (118L)       /**< SPI0 signal: SPI0_SPCK on PD22 mux B*/
#define MUX_PD22B_SPI0_SPCK                        (1L)         /**< SPI0 signal line function value: SPI0_SPCK */
#define PIO_PD22B_SPI0_SPCK                        (1UL << 22)  /**< SPI0 signal: SPI0SPCK */
/* ========== PIO definition for SPI1 peripheral ========== */
#define PIN_PC26C_SPI1_MISO                        (90L)        /**< SPI1 signal: SPI1_MISO on PC26 mux C*/
#define MUX_PC26C_SPI1_MISO                        (2L)         /**< SPI1 signal line function value: SPI1_MISO */
#define PIO_PC26C_SPI1_MISO                        (1UL << 26)  /**< SPI1 signal: SPI1MISO */
#define PIN_PC27C_SPI1_MOSI                        (91L)        /**< SPI1 signal: SPI1_MOSI on PC27 mux C*/
#define MUX_PC27C_SPI1_MOSI                        (2L)         /**< SPI1 signal line function value: SPI1_MOSI */
#define PIO_PC27C_SPI1_MOSI                        (1UL << 27)  /**< SPI1 signal: SPI1MOSI */
#define PIN_PC25C_SPI1_NPCS0                       (89L)        /**< SPI1 signal: SPI1_NPCS0 on PC25 mux C*/
#define MUX_PC25C_SPI1_NPCS0                       (2L)         /**< SPI1 signal line function value: SPI1_NPCS0 */
#define PIO_PC25C_SPI1_NPCS0                       (1UL << 25)  /**< SPI1 signal: SPI1NPCS0 */
#define PIN_PC28C_SPI1_NPCS1                       (92L)        /**< SPI1 signal: SPI1_NPCS1 on PC28 mux C*/
#define MUX_PC28C_SPI1_NPCS1                       (2L)         /**< SPI1 signal line function value: SPI1_NPCS1 */
#define PIO_PC28C_SPI1_NPCS1                       (1UL << 28)  /**< SPI1 signal: SPI1NPCS1 */
#define PIN_PD0C_SPI1_NPCS1                        (96L)        /**< SPI1 signal: SPI1_NPCS1 on PD0 mux C*/
#define MUX_PD0C_SPI1_NPCS1                        (2L)         /**< SPI1 signal line function value: SPI1_NPCS1 */
#define PIO_PD0C_SPI1_NPCS1                        (1UL << 0)   /**< SPI1 signal: SPI1NPCS1 */
#define PIN_PC29C_SPI1_NPCS2                       (93L)        /**< SPI1 signal: SPI1_NPCS2 on PC29 mux C*/
#define MUX_PC29C_SPI1_NPCS2                       (2L)         /**< SPI1 signal line function value: SPI1_NPCS2 */
#define PIO_PC29C_SPI1_NPCS2                       (1UL << 29)  /**< SPI1 signal: SPI1NPCS2 */
#define PIN_PD1C_SPI1_NPCS2                        (97L)        /**< SPI1 signal: SPI1_NPCS2 on PD1 mux C*/
#define MUX_PD1C_SPI1_NPCS2                        (2L)         /**< SPI1 signal line function value: SPI1_NPCS2 */
#define PIO_PD1C_SPI1_NPCS2                        (1UL << 1)   /**< SPI1 signal: SPI1NPCS2 */
#define PIN_PC30C_SPI1_NPCS3                       (94L)        /**< SPI1 signal: SPI1_NPCS3 on PC30 mux C*/
#define MUX_PC30C_SPI1_NPCS3                       (2L)         /**< SPI1 signal line function value: SPI1_NPCS3 */
#define PIO_PC30C_SPI1_NPCS3                       (1UL << 30)  /**< SPI1 signal: SPI1NPCS3 */
#define PIN_PD2C_SPI1_NPCS3                        (98L)        /**< SPI1 signal: SPI1_NPCS3 on PD2 mux C*/
#define MUX_PD2C_SPI1_NPCS3                        (2L)         /**< SPI1 signal line function value: SPI1_NPCS3 */
#define PIO_PD2C_SPI1_NPCS3                        (1UL << 2)   /**< SPI1 signal: SPI1NPCS3 */
#define PIN_PC24C_SPI1_SPCK                        (88L)        /**< SPI1 signal: SPI1_SPCK on PC24 mux C*/
#define MUX_PC24C_SPI1_SPCK                        (2L)         /**< SPI1 signal line function value: SPI1_SPCK */
#define PIO_PC24C_SPI1_SPCK                        (1UL << 24)  /**< SPI1 signal: SPI1SPCK */
/* ========== PIO definition for SSC peripheral ========== */
#define PIN_PA10C_SSC_RD                           (10L)        /**< SSC signal: SSC_RD on PA10 mux C*/
#define MUX_PA10C_SSC_RD                           (2L)         /**< SSC signal line function value: SSC_RD */
#define PIO_PA10C_SSC_RD                           (1UL << 10)  /**< SSC signal: SSCRD */
#define PIN_PD24B_SSC_RF                           (120L)       /**< SSC signal: SSC_RF on PD24 mux B*/
#define MUX_PD24B_SSC_RF                           (1L)         /**< SSC signal line function value: SSC_RF */
#define PIO_PD24B_SSC_RF                           (1UL << 24)  /**< SSC signal: SSCRF */
#define PIN_PA22A_SSC_RK                           (22L)        /**< SSC signal: SSC_RK on PA22 mux A*/
#define MUX_PA22A_SSC_RK                           (0L)         /**< SSC signal line function value: SSC_RK */
#define PIO_PA22A_SSC_RK                           (1UL << 22)  /**< SSC signal: SSCRK */
#define PIN_PB5D_SSC_TD                            (37L)        /**< SSC signal: SSC_TD on PB5 mux D*/
#define MUX_PB5D_SSC_TD                            (3L)         /**< SSC signal line function value: SSC_TD */
#define PIO_PB5D_SSC_TD                            (1UL << 5)   /**< SSC signal: SSCTD */
#define PIN_PD10C_SSC_TD                           (106L)       /**< SSC signal: SSC_TD on PD10 mux C*/
#define MUX_PD10C_SSC_TD                           (2L)         /**< SSC signal line function value: SSC_TD */
#define PIO_PD10C_SSC_TD                           (1UL << 10)  /**< SSC signal: SSCTD */
#define PIN_PD26B_SSC_TD                           (122L)       /**< SSC signal: SSC_TD on PD26 mux B*/
#define MUX_PD26B_SSC_TD                           (1L)         /**< SSC signal line function value: SSC_TD */
#define PIO_PD26B_SSC_TD                           (1UL << 26)  /**< SSC signal: SSCTD */
#define PIN_PB0D_SSC_TF                            (32L)        /**< SSC signal: SSC_TF on PB0 mux D*/
#define MUX_PB0D_SSC_TF                            (3L)         /**< SSC signal line function value: SSC_TF */
#define PIO_PB0D_SSC_TF                            (1UL << 0)   /**< SSC signal: SSCTF */
#define PIN_PB1D_SSC_TK                            (33L)        /**< SSC signal: SSC_TK on PB1 mux D*/
#define MUX_PB1D_SSC_TK                            (3L)         /**< SSC signal line function value: SSC_TK */
#define PIO_PB1D_SSC_TK                            (1UL << 1)   /**< SSC signal: SSCTK */
/* ========== PIO definition for SUPC peripheral ========== */
#define PIN_PA0X1_SUPC_WKUP0                       (0L)         /**< SUPC signal: SUPC_WKUP0 on PA0 mux X1*/
#define PIO_PA0X1_SUPC_WKUP0                       (1UL << 0)   /**< SUPC signal: SUPCWKUP0 */
#define PIN_PA1X1_SUPC_WKUP1                       (1L)         /**< SUPC signal: SUPC_WKUP1 on PA1 mux X1*/
#define PIO_PA1X1_SUPC_WKUP1                       (1UL << 1)   /**< SUPC signal: SUPCWKUP1 */
#define PIN_PA2X1_SUPC_WKUP2                       (2L)         /**< SUPC signal: SUPC_WKUP2 on PA2 mux X1*/
#define PIO_PA2X1_SUPC_WKUP2                       (1UL << 2)   /**< SUPC signal: SUPCWKUP2 */
#define PIN_PA4X1_SUPC_WKUP3                       (4L)         /**< SUPC signal: SUPC_WKUP3 on PA4 mux X1*/
#define PIO_PA4X1_SUPC_WKUP3                       (1UL << 4)   /**< SUPC signal: SUPCWKUP3 */
#define PIN_PA5X1_SUPC_WKUP4                       (5L)         /**< SUPC signal: SUPC_WKUP4 on PA5 mux X1*/
#define PIO_PA5X1_SUPC_WKUP4                       (1UL << 5)   /**< SUPC signal: SUPCWKUP4 */
#define PIN_PA9X1_SUPC_WKUP6                       (9L)         /**< SUPC signal: SUPC_WKUP6 on PA9 mux X1*/
#define PIO_PA9X1_SUPC_WKUP6                       (1UL << 9)   /**< SUPC signal: SUPCWKUP6 */
#define PIN_PA11X1_SUPC_WKUP7                      (11L)        /**< SUPC signal: SUPC_WKUP7 on PA11 mux X1*/
#define PIO_PA11X1_SUPC_WKUP7                      (1UL << 11)  /**< SUPC signal: SUPCWKUP7 */
#define PIN_PA14X1_SUPC_WKUP8                      (14L)        /**< SUPC signal: SUPC_WKUP8 on PA14 mux X1*/
#define PIO_PA14X1_SUPC_WKUP8                      (1UL << 14)  /**< SUPC signal: SUPCWKUP8 */
#define PIN_PA19X1_SUPC_WKUP9                      (19L)        /**< SUPC signal: SUPC_WKUP9 on PA19 mux X1*/
#define PIO_PA19X1_SUPC_WKUP9                      (1UL << 19)  /**< SUPC signal: SUPCWKUP9 */
#define PIN_PA20X1_SUPC_WKUP10                     (20L)        /**< SUPC signal: SUPC_WKUP10 on PA20 mux X1*/
#define PIO_PA20X1_SUPC_WKUP10                     (1UL << 20)  /**< SUPC signal: SUPCWKUP10 */
#define PIN_PA30X1_SUPC_WKUP11                     (30L)        /**< SUPC signal: SUPC_WKUP11 on PA30 mux X1*/
#define PIO_PA30X1_SUPC_WKUP11                     (1UL << 30)  /**< SUPC signal: SUPCWKUP11 */
#define PIN_PB3X1_SUPC_WKUP12                      (35L)        /**< SUPC signal: SUPC_WKUP12 on PB3 mux X1*/
#define PIO_PB3X1_SUPC_WKUP12                      (1UL << 3)   /**< SUPC signal: SUPCWKUP12 */
#define PIN_PB5X1_SUPC_WKUP13                      (37L)        /**< SUPC signal: SUPC_WKUP13 on PB5 mux X1*/
#define PIO_PB5X1_SUPC_WKUP13                      (1UL << 5)   /**< SUPC signal: SUPCWKUP13 */
#define PIN_PD28X1_SUPC_WKUP5                      (124L)       /**< SUPC signal: SUPC_WKUP5 on PD28 mux X1*/
#define PIO_PD28X1_SUPC_WKUP5                      (1UL << 28)  /**< SUPC signal: SUPCWKUP5 */
/* ========== PIO definition for TC0 peripheral ========== */
#define PIN_PA4B_TC0_TCLK0                         (4L)         /**< TC0 signal: TC0_TCLK0 on PA4 mux B*/
#define MUX_PA4B_TC0_TCLK0                         (1L)         /**< TC0 signal line function value: TC0_TCLK0 */
#define PIO_PA4B_TC0_TCLK0                         (1UL << 4)   /**< TC0 signal: TC0TCLK0 */
#define PIN_PA28B_TC0_TCLK1                        (28L)        /**< TC0 signal: TC0_TCLK1 on PA28 mux B*/
#define MUX_PA28B_TC0_TCLK1                        (1L)         /**< TC0 signal line function value: TC0_TCLK1 */
#define PIO_PA28B_TC0_TCLK1                        (1UL << 28)  /**< TC0 signal: TC0TCLK1 */
#define PIN_PA29B_TC0_TCLK2                        (29L)        /**< TC0 signal: TC0_TCLK2 on PA29 mux B*/
#define MUX_PA29B_TC0_TCLK2                        (1L)         /**< TC0 signal line function value: TC0_TCLK2 */
#define PIO_PA29B_TC0_TCLK2                        (1UL << 29)  /**< TC0 signal: TC0TCLK2 */
#define PIN_PA0B_TC0_TIOA0                         (0L)         /**< TC0 signal: TC0_TIOA0 on PA0 mux B*/
#define MUX_PA0B_TC0_TIOA0                         (1L)         /**< TC0 signal line function value: TC0_TIOA0 */
#define PIO_PA0B_TC0_TIOA0                         (1UL << 0)   /**< TC0 signal: TC0TIOA0 */
#define PIN_PA15B_TC0_TIOA1                        (15L)        /**< TC0 signal: TC0_TIOA1 on PA15 mux B*/
#define MUX_PA15B_TC0_TIOA1                        (1L)         /**< TC0 signal line function value: TC0_TIOA1 */
#define PIO_PA15B_TC0_TIOA1                        (1UL << 15)  /**< TC0 signal: TC0TIOA1 */
#define PIN_PA26B_TC0_TIOA2                        (26L)        /**< TC0 signal: TC0_TIOA2 on PA26 mux B*/
#define MUX_PA26B_TC0_TIOA2                        (1L)         /**< TC0 signal line function value: TC0_TIOA2 */
#define PIO_PA26B_TC0_TIOA2                        (1UL << 26)  /**< TC0 signal: TC0TIOA2 */
#define PIN_PA1B_TC0_TIOB0                         (1L)         /**< TC0 signal: TC0_TIOB0 on PA1 mux B*/
#define MUX_PA1B_TC0_TIOB0                         (1L)         /**< TC0 signal line function value: TC0_TIOB0 */
#define PIO_PA1B_TC0_TIOB0                         (1UL << 1)   /**< TC0 signal: TC0TIOB0 */
#define PIN_PA16B_TC0_TIOB1                        (16L)        /**< TC0 signal: TC0_TIOB1 on PA16 mux B*/
#define MUX_PA16B_TC0_TIOB1                        (1L)         /**< TC0 signal line function value: TC0_TIOB1 */
#define PIO_PA16B_TC0_TIOB1                        (1UL << 16)  /**< TC0 signal: TC0TIOB1 */
#define PIN_PA27B_TC0_TIOB2                        (27L)        /**< TC0 signal: TC0_TIOB2 on PA27 mux B*/
#define MUX_PA27B_TC0_TIOB2                        (1L)         /**< TC0 signal line function value: TC0_TIOB2 */
#define PIO_PA27B_TC0_TIOB2                        (1UL << 27)  /**< TC0 signal: TC0TIOB2 */
/* ========== PIO definition for TC1 peripheral ========== */
#define PIN_PC25B_TC1_TCLK3                        (89L)        /**< TC1 signal: TC1_TCLK3 on PC25 mux B*/
#define MUX_PC25B_TC1_TCLK3                        (1L)         /**< TC1 signal line function value: TC1_TCLK3 */
#define PIO_PC25B_TC1_TCLK3                        (1UL << 25)  /**< TC1 signal: TC1TCLK3 */
#define PIN_PC28B_TC1_TCLK4                        (92L)        /**< TC1 signal: TC1_TCLK4 on PC28 mux B*/
#define MUX_PC28B_TC1_TCLK4                        (1L)         /**< TC1 signal line function value: TC1_TCLK4 */
#define PIO_PC28B_TC1_TCLK4                        (1UL << 28)  /**< TC1 signal: TC1TCLK4 */
#define PIN_PC31B_TC1_TCLK5                        (95L)        /**< TC1 signal: TC1_TCLK5 on PC31 mux B*/
#define MUX_PC31B_TC1_TCLK5                        (1L)         /**< TC1 signal line function value: TC1_TCLK5 */
#define PIO_PC31B_TC1_TCLK5                        (1UL << 31)  /**< TC1 signal: TC1TCLK5 */
#define PIN_PC23B_TC1_TIOA3                        (87L)        /**< TC1 signal: TC1_TIOA3 on PC23 mux B*/
#define MUX_PC23B_TC1_TIOA3                        (1L)         /**< TC1 signal line function value: TC1_TIOA3 */
#define PIO_PC23B_TC1_TIOA3                        (1UL << 23)  /**< TC1 signal: TC1TIOA3 */
#define PIN_PC26B_TC1_TIOA4                        (90L)        /**< TC1 signal: TC1_TIOA4 on PC26 mux B*/
#define MUX_PC26B_TC1_TIOA4                        (1L)         /**< TC1 signal line function value: TC1_TIOA4 */
#define PIO_PC26B_TC1_TIOA4                        (1UL << 26)  /**< TC1 signal: TC1TIOA4 */
#define PIN_PC29B_TC1_TIOA5                        (93L)        /**< TC1 signal: TC1_TIOA5 on PC29 mux B*/
#define MUX_PC29B_TC1_TIOA5                        (1L)         /**< TC1 signal line function value: TC1_TIOA5 */
#define PIO_PC29B_TC1_TIOA5                        (1UL << 29)  /**< TC1 signal: TC1TIOA5 */
#define PIN_PC24B_TC1_TIOB3                        (88L)        /**< TC1 signal: TC1_TIOB3 on PC24 mux B*/
#define MUX_PC24B_TC1_TIOB3                        (1L)         /**< TC1 signal line function value: TC1_TIOB3 */
#define PIO_PC24B_TC1_TIOB3                        (1UL << 24)  /**< TC1 signal: TC1TIOB3 */
#define PIN_PC27B_TC1_TIOB4                        (91L)        /**< TC1 signal: TC1_TIOB4 on PC27 mux B*/
#define MUX_PC27B_TC1_TIOB4                        (1L)         /**< TC1 signal line function value: TC1_TIOB4 */
#define PIO_PC27B_TC1_TIOB4                        (1UL << 27)  /**< TC1 signal: TC1TIOB4 */
#define PIN_PC30B_TC1_TIOB5                        (94L)        /**< TC1 signal: TC1_TIOB5 on PC30 mux B*/
#define MUX_PC30B_TC1_TIOB5                        (1L)         /**< TC1 signal line function value: TC1_TIOB5 */
#define PIO_PC30B_TC1_TIOB5                        (1UL << 30)  /**< TC1 signal: TC1TIOB5 */
/* ========== PIO definition for TC2 peripheral ========== */
#define PIN_PC7B_TC2_TCLK6                         (71L)        /**< TC2 signal: TC2_TCLK6 on PC7 mux B*/
#define MUX_PC7B_TC2_TCLK6                         (1L)         /**< TC2 signal line function value: TC2_TCLK6 */
#define PIO_PC7B_TC2_TCLK6                         (1UL << 7)   /**< TC2 signal: TC2TCLK6 */
#define PIN_PC10B_TC2_TCLK7                        (74L)        /**< TC2 signal: TC2_TCLK7 on PC10 mux B*/
#define MUX_PC10B_TC2_TCLK7                        (1L)         /**< TC2 signal line function value: TC2_TCLK7 */
#define PIO_PC10B_TC2_TCLK7                        (1UL << 10)  /**< TC2 signal: TC2TCLK7 */
#define PIN_PC14B_TC2_TCLK8                        (78L)        /**< TC2 signal: TC2_TCLK8 on PC14 mux B*/
#define MUX_PC14B_TC2_TCLK8                        (1L)         /**< TC2 signal line function value: TC2_TCLK8 */
#define PIO_PC14B_TC2_TCLK8                        (1UL << 14)  /**< TC2 signal: TC2TCLK8 */
#define PIN_PC5B_TC2_TIOA6                         (69L)        /**< TC2 signal: TC2_TIOA6 on PC5 mux B*/
#define MUX_PC5B_TC2_TIOA6                         (1L)         /**< TC2 signal line function value: TC2_TIOA6 */
#define PIO_PC5B_TC2_TIOA6                         (1UL << 5)   /**< TC2 signal: TC2TIOA6 */
#define PIN_PC8B_TC2_TIOA7                         (72L)        /**< TC2 signal: TC2_TIOA7 on PC8 mux B*/
#define MUX_PC8B_TC2_TIOA7                         (1L)         /**< TC2 signal line function value: TC2_TIOA7 */
#define PIO_PC8B_TC2_TIOA7                         (1UL << 8)   /**< TC2 signal: TC2TIOA7 */
#define PIN_PC11B_TC2_TIOA8                        (75L)        /**< TC2 signal: TC2_TIOA8 on PC11 mux B*/
#define MUX_PC11B_TC2_TIOA8                        (1L)         /**< TC2 signal line function value: TC2_TIOA8 */
#define PIO_PC11B_TC2_TIOA8                        (1UL << 11)  /**< TC2 signal: TC2TIOA8 */
#define PIN_PC6B_TC2_TIOB6                         (70L)        /**< TC2 signal: TC2_TIOB6 on PC6 mux B*/
#define MUX_PC6B_TC2_TIOB6                         (1L)         /**< TC2 signal line function value: TC2_TIOB6 */
#define PIO_PC6B_TC2_TIOB6                         (1UL << 6)   /**< TC2 signal: TC2TIOB6 */
#define PIN_PC9B_TC2_TIOB7                         (73L)        /**< TC2 signal: TC2_TIOB7 on PC9 mux B*/
#define MUX_PC9B_TC2_TIOB7                         (1L)         /**< TC2 signal line function value: TC2_TIOB7 */
#define PIO_PC9B_TC2_TIOB7                         (1UL << 9)   /**< TC2 signal: TC2TIOB7 */
#define PIN_PC12B_TC2_TIOB8                        (76L)        /**< TC2 signal: TC2_TIOB8 on PC12 mux B*/
#define MUX_PC12B_TC2_TIOB8                        (1L)         /**< TC2 signal line function value: TC2_TIOB8 */
#define PIO_PC12B_TC2_TIOB8                        (1UL << 12)  /**< TC2 signal: TC2TIOB8 */
/* ========== PIO definition for TC3 peripheral ========== */
#define PIN_PE2B_TC3_TCLK9                         (130L)       /**< TC3 signal: TC3_TCLK9 on PE2 mux B*/
#define MUX_PE2B_TC3_TCLK9                         (1L)         /**< TC3 signal line function value: TC3_TCLK9 */
#define PIO_PE2B_TC3_TCLK9                         (1UL << 2)   /**< TC3 signal: TC3TCLK9 */
#define PIN_PE5B_TC3_TCLK10                        (133L)       /**< TC3 signal: TC3_TCLK10 on PE5 mux B*/
#define MUX_PE5B_TC3_TCLK10                        (1L)         /**< TC3 signal line function value: TC3_TCLK10 */
#define PIO_PE5B_TC3_TCLK10                        (1UL << 5)   /**< TC3 signal: TC3TCLK10 */
#define PIN_PD24C_TC3_TCLK11                       (120L)       /**< TC3 signal: TC3_TCLK11 on PD24 mux C*/
#define MUX_PD24C_TC3_TCLK11                       (2L)         /**< TC3 signal line function value: TC3_TCLK11 */
#define PIO_PD24C_TC3_TCLK11                       (1UL << 24)  /**< TC3 signal: TC3TCLK11 */
#define PIN_PE0B_TC3_TIOA9                         (128L)       /**< TC3 signal: TC3_TIOA9 on PE0 mux B*/
#define MUX_PE0B_TC3_TIOA9                         (1L)         /**< TC3 signal line function value: TC3_TIOA9 */
#define PIO_PE0B_TC3_TIOA9                         (1UL << 0)   /**< TC3 signal: TC3TIOA9 */
#define PIN_PE3B_TC3_TIOA10                        (131L)       /**< TC3 signal: TC3_TIOA10 on PE3 mux B*/
#define MUX_PE3B_TC3_TIOA10                        (1L)         /**< TC3 signal line function value: TC3_TIOA10 */
#define PIO_PE3B_TC3_TIOA10                        (1UL << 3)   /**< TC3 signal: TC3TIOA10 */
#define PIN_PD21C_TC3_TIOA11                       (117L)       /**< TC3 signal: TC3_TIOA11 on PD21 mux C*/
#define MUX_PD21C_TC3_TIOA11                       (2L)         /**< TC3 signal line function value: TC3_TIOA11 */
#define PIO_PD21C_TC3_TIOA11                       (1UL << 21)  /**< TC3 signal: TC3TIOA11 */
#define PIN_PE1B_TC3_TIOB9                         (129L)       /**< TC3 signal: TC3_TIOB9 on PE1 mux B*/
#define MUX_PE1B_TC3_TIOB9                         (1L)         /**< TC3 signal line function value: TC3_TIOB9 */
#define PIO_PE1B_TC3_TIOB9                         (1UL << 1)   /**< TC3 signal: TC3TIOB9 */
#define PIN_PE4B_TC3_TIOB10                        (132L)       /**< TC3 signal: TC3_TIOB10 on PE4 mux B*/
#define MUX_PE4B_TC3_TIOB10                        (1L)         /**< TC3 signal line function value: TC3_TIOB10 */
#define PIO_PE4B_TC3_TIOB10                        (1UL << 4)   /**< TC3 signal: TC3TIOB10 */
#define PIN_PD22C_TC3_TIOB11                       (118L)       /**< TC3 signal: TC3_TIOB11 on PD22 mux C*/
#define MUX_PD22C_TC3_TIOB11                       (2L)         /**< TC3 signal line function value: TC3_TIOB11 */
#define PIO_PD22C_TC3_TIOB11                       (1UL << 22)  /**< TC3 signal: TC3TIOB11 */
/* ========== PIO definition for TWIHS0 peripheral ========== */
#define PIN_PA4A_TWIHS0_TWCK0                      (4L)         /**< TWIHS0 signal: TWIHS0_TWCK0 on PA4 mux A*/
#define MUX_PA4A_TWIHS0_TWCK0                      (0L)         /**< TWIHS0 signal line function value: TWIHS0_TWCK0 */
#define PIO_PA4A_TWIHS0_TWCK0                      (1UL << 4)   /**< TWIHS0 signal: TWIHS0TWCK0 */
#define PIN_PA3A_TWIHS0_TWD0                       (3L)         /**< TWIHS0 signal: TWIHS0_TWD0 on PA3 mux A*/
#define MUX_PA3A_TWIHS0_TWD0                       (0L)         /**< TWIHS0 signal line function value: TWIHS0_TWD0 */
#define PIO_PA3A_TWIHS0_TWD0                       (1UL << 3)   /**< TWIHS0 signal: TWIHS0TWD0 */
/* ========== PIO definition for TWIHS1 peripheral ========== */
#define PIN_PB5A_TWIHS1_TWCK1                      (37L)        /**< TWIHS1 signal: TWIHS1_TWCK1 on PB5 mux A*/
#define MUX_PB5A_TWIHS1_TWCK1                      (0L)         /**< TWIHS1 signal line function value: TWIHS1_TWCK1 */
#define PIO_PB5A_TWIHS1_TWCK1                      (1UL << 5)   /**< TWIHS1 signal: TWIHS1TWCK1 */
#define PIN_PB4A_TWIHS1_TWD1                       (36L)        /**< TWIHS1 signal: TWIHS1_TWD1 on PB4 mux A*/
#define MUX_PB4A_TWIHS1_TWD1                       (0L)         /**< TWIHS1 signal line function value: TWIHS1_TWD1 */
#define PIO_PB4A_TWIHS1_TWD1                       (1UL << 4)   /**< TWIHS1 signal: TWIHS1TWD1 */
/* ========== PIO definition for TWIHS2 peripheral ========== */
#define PIN_PD28C_TWIHS2_TWCK2                     (124L)       /**< TWIHS2 signal: TWIHS2_TWCK2 on PD28 mux C*/
#define MUX_PD28C_TWIHS2_TWCK2                     (2L)         /**< TWIHS2 signal line function value: TWIHS2_TWCK2 */
#define PIO_PD28C_TWIHS2_TWCK2                     (1UL << 28)  /**< TWIHS2 signal: TWIHS2TWCK2 */
#define PIN_PD27C_TWIHS2_TWD2                      (123L)       /**< TWIHS2 signal: TWIHS2_TWD2 on PD27 mux C*/
#define MUX_PD27C_TWIHS2_TWD2                      (2L)         /**< TWIHS2 signal line function value: TWIHS2_TWD2 */
#define PIO_PD27C_TWIHS2_TWD2                      (1UL << 27)  /**< TWIHS2 signal: TWIHS2TWD2 */
/* ========== PIO definition for UART0 peripheral ========== */
#define PIN_PA9A_UART0_URXD0                       (9L)         /**< UART0 signal: UART0_URXD0 on PA9 mux A*/
#define MUX_PA9A_UART0_URXD0                       (0L)         /**< UART0 signal line function value: UART0_URXD0 */
#define PIO_PA9A_UART0_URXD0                       (1UL << 9)   /**< UART0 signal: UART0URXD0 */
#define PIN_PA10A_UART0_UTXD0                      (10L)        /**< UART0 signal: UART0_UTXD0 on PA10 mux A*/
#define MUX_PA10A_UART0_UTXD0                      (0L)         /**< UART0 signal line function value: UART0_UTXD0 */
#define PIO_PA10A_UART0_UTXD0                      (1UL << 10)  /**< UART0 signal: UART0UTXD0 */
/* ========== PIO definition for UART1 peripheral ========== */
#define PIN_PA5C_UART1_URXD1                       (5L)         /**< UART1 signal: UART1_URXD1 on PA5 mux C*/
#define MUX_PA5C_UART1_URXD1                       (2L)         /**< UART1 signal line function value: UART1_URXD1 */
#define PIO_PA5C_UART1_URXD1                       (1UL << 5)   /**< UART1 signal: UART1URXD1 */
#define PIN_PA4C_UART1_UTXD1                       (4L)         /**< UART1 signal: UART1_UTXD1 on PA4 mux C*/
#define MUX_PA4C_UART1_UTXD1                       (2L)         /**< UART1 signal line function value: UART1_UTXD1 */
#define PIO_PA4C_UART1_UTXD1                       (1UL << 4)   /**< UART1 signal: UART1UTXD1 */
#define PIN_PA6C_UART1_UTXD1                       (6L)         /**< UART1 signal: UART1_UTXD1 on PA6 mux C*/
#define MUX_PA6C_UART1_UTXD1                       (2L)         /**< UART1 signal line function value: UART1_UTXD1 */
#define PIO_PA6C_UART1_UTXD1                       (1UL << 6)   /**< UART1 signal: UART1UTXD1 */
#define PIN_PD26D_UART1_UTXD1                      (122L)       /**< UART1 signal: UART1_UTXD1 on PD26 mux D*/
#define MUX_PD26D_UART1_UTXD1                      (3L)         /**< UART1 signal line function value: UART1_UTXD1 */
#define PIO_PD26D_UART1_UTXD1                      (1UL << 26)  /**< UART1 signal: UART1UTXD1 */
/* ========== PIO definition for UART2 peripheral ========== */
#define PIN_PD25C_UART2_URXD2                      (121L)       /**< UART2 signal: UART2_URXD2 on PD25 mux C*/
#define MUX_PD25C_UART2_URXD2                      (2L)         /**< UART2 signal line function value: UART2_URXD2 */
#define PIO_PD25C_UART2_URXD2                      (1UL << 25)  /**< UART2 signal: UART2URXD2 */
#define PIN_PD26C_UART2_UTXD2                      (122L)       /**< UART2 signal: UART2_UTXD2 on PD26 mux C*/
#define MUX_PD26C_UART2_UTXD2                      (2L)         /**< UART2 signal line function value: UART2_UTXD2 */
#define PIO_PD26C_UART2_UTXD2                      (1UL << 26)  /**< UART2 signal: UART2UTXD2 */
/* ========== PIO definition for UART3 peripheral ========== */
#define PIN_PD28A_UART3_URXD3                      (124L)       /**< UART3 signal: UART3_URXD3 on PD28 mux A*/
#define MUX_PD28A_UART3_URXD3                      (0L)         /**< UART3 signal line function value: UART3_URXD3 */
#define PIO_PD28A_UART3_URXD3                      (1UL << 28)  /**< UART3 signal: UART3URXD3 */
#define PIN_PD30A_UART3_UTXD3                      (126L)       /**< UART3 signal: UART3_UTXD3 on PD30 mux A*/
#define MUX_PD30A_UART3_UTXD3                      (0L)         /**< UART3 signal line function value: UART3_UTXD3 */
#define PIO_PD30A_UART3_UTXD3                      (1UL << 30)  /**< UART3 signal: UART3UTXD3 */
#define PIN_PD31B_UART3_UTXD3                      (127L)       /**< UART3 signal: UART3_UTXD3 on PD31 mux B*/
#define MUX_PD31B_UART3_UTXD3                      (1L)         /**< UART3 signal line function value: UART3_UTXD3 */
#define PIO_PD31B_UART3_UTXD3                      (1UL << 31)  /**< UART3 signal: UART3UTXD3 */
/* ========== PIO definition for UART4 peripheral ========== */
#define PIN_PD18C_UART4_URXD4                      (114L)       /**< UART4 signal: UART4_URXD4 on PD18 mux C*/
#define MUX_PD18C_UART4_URXD4                      (2L)         /**< UART4 signal line function value: UART4_URXD4 */
#define PIO_PD18C_UART4_URXD4                      (1UL << 18)  /**< UART4 signal: UART4URXD4 */
#define PIN_PD3C_UART4_UTXD4                       (99L)        /**< UART4 signal: UART4_UTXD4 on PD3 mux C*/
#define MUX_PD3C_UART4_UTXD4                       (2L)         /**< UART4 signal line function value: UART4_UTXD4 */
#define PIO_PD3C_UART4_UTXD4                       (1UL << 3)   /**< UART4 signal: UART4UTXD4 */
#define PIN_PD19C_UART4_UTXD4                      (115L)       /**< UART4 signal: UART4_UTXD4 on PD19 mux C*/
#define MUX_PD19C_UART4_UTXD4                      (2L)         /**< UART4 signal line function value: UART4_UTXD4 */
#define PIO_PD19C_UART4_UTXD4                      (1UL << 19)  /**< UART4 signal: UART4UTXD4 */
/* ========== PIO definition for USART0 peripheral ========== */
#define PIN_PB2C_USART0_CTS0                       (34L)        /**< USART0 signal: USART0_CTS0 on PB2 mux C*/
#define MUX_PB2C_USART0_CTS0                       (2L)         /**< USART0 signal line function value: USART0_CTS0 */
#define PIO_PB2C_USART0_CTS0                       (1UL << 2)   /**< USART0 signal: USART0CTS0 */
#define PIN_PD0D_USART0_DCD0                       (96L)        /**< USART0 signal: USART0_DCD0 on PD0 mux D*/
#define MUX_PD0D_USART0_DCD0                       (3L)         /**< USART0 signal line function value: USART0_DCD0 */
#define PIO_PD0D_USART0_DCD0                       (1UL << 0)   /**< USART0 signal: USART0DCD0 */
#define PIN_PD2D_USART0_DSR0                       (98L)        /**< USART0 signal: USART0_DSR0 on PD2 mux D*/
#define MUX_PD2D_USART0_DSR0                       (3L)         /**< USART0 signal line function value: USART0_DSR0 */
#define PIO_PD2D_USART0_DSR0                       (1UL << 2)   /**< USART0 signal: USART0DSR0 */
#define PIN_PD1D_USART0_DTR0                       (97L)        /**< USART0 signal: USART0_DTR0 on PD1 mux D*/
#define MUX_PD1D_USART0_DTR0                       (3L)         /**< USART0 signal line function value: USART0_DTR0 */
#define PIO_PD1D_USART0_DTR0                       (1UL << 1)   /**< USART0 signal: USART0DTR0 */
#define PIN_PD3D_USART0_RI0                        (99L)        /**< USART0 signal: USART0_RI0 on PD3 mux D*/
#define MUX_PD3D_USART0_RI0                        (3L)         /**< USART0 signal line function value: USART0_RI0 */
#define PIO_PD3D_USART0_RI0                        (1UL << 3)   /**< USART0 signal: USART0RI0 */
#define PIN_PB3C_USART0_RTS0                       (35L)        /**< USART0 signal: USART0_RTS0 on PB3 mux C*/
#define MUX_PB3C_USART0_RTS0                       (2L)         /**< USART0 signal line function value: USART0_RTS0 */
#define PIO_PB3C_USART0_RTS0                       (1UL << 3)   /**< USART0 signal: USART0RTS0 */
#define PIN_PB0C_USART0_RXD0                       (32L)        /**< USART0 signal: USART0_RXD0 on PB0 mux C*/
#define MUX_PB0C_USART0_RXD0                       (2L)         /**< USART0 signal line function value: USART0_RXD0 */
#define PIO_PB0C_USART0_RXD0                       (1UL << 0)   /**< USART0 signal: USART0RXD0 */
#define PIN_PB13C_USART0_SCK0                      (45L)        /**< USART0 signal: USART0_SCK0 on PB13 mux C*/
#define MUX_PB13C_USART0_SCK0                      (2L)         /**< USART0 signal line function value: USART0_SCK0 */
#define PIO_PB13C_USART0_SCK0                      (1UL << 13)  /**< USART0 signal: USART0SCK0 */
#define PIN_PB1C_USART0_TXD0                       (33L)        /**< USART0 signal: USART0_TXD0 on PB1 mux C*/
#define MUX_PB1C_USART0_TXD0                       (2L)         /**< USART0 signal line function value: USART0_TXD0 */
#define PIO_PB1C_USART0_TXD0                       (1UL << 1)   /**< USART0 signal: USART0TXD0 */
/* ========== PIO definition for USART1 peripheral ========== */
#define PIN_PA25A_USART1_CTS1                      (25L)        /**< USART1 signal: USART1_CTS1 on PA25 mux A*/
#define MUX_PA25A_USART1_CTS1                      (0L)         /**< USART1 signal line function value: USART1_CTS1 */
#define PIO_PA25A_USART1_CTS1                      (1UL << 25)  /**< USART1 signal: USART1CTS1 */
#define PIN_PA26A_USART1_DCD1                      (26L)        /**< USART1 signal: USART1_DCD1 on PA26 mux A*/
#define MUX_PA26A_USART1_DCD1                      (0L)         /**< USART1 signal line function value: USART1_DCD1 */
#define PIO_PA26A_USART1_DCD1                      (1UL << 26)  /**< USART1 signal: USART1DCD1 */
#define PIN_PA28A_USART1_DSR1                      (28L)        /**< USART1 signal: USART1_DSR1 on PA28 mux A*/
#define MUX_PA28A_USART1_DSR1                      (0L)         /**< USART1 signal line function value: USART1_DSR1 */
#define PIO_PA28A_USART1_DSR1                      (1UL << 28)  /**< USART1 signal: USART1DSR1 */
#define PIN_PA27A_USART1_DTR1                      (27L)        /**< USART1 signal: USART1_DTR1 on PA27 mux A*/
#define MUX_PA27A_USART1_DTR1                      (0L)         /**< USART1 signal line function value: USART1_DTR1 */
#define PIO_PA27A_USART1_DTR1                      (1UL << 27)  /**< USART1 signal: USART1DTR1 */
#define PIN_PA3B_USART1_LONCOL1                    (3L)         /**< USART1 signal: USART1_LONCOL1 on PA3 mux B*/
#define MUX_PA3B_USART1_LONCOL1                    (1L)         /**< USART1 signal line function value: USART1_LONCOL1 */
#define PIO_PA3B_USART1_LONCOL1                    (1UL << 3)   /**< USART1 signal: USART1LONCOL1 */
#define PIN_PA29A_USART1_RI1                       (29L)        /**< USART1 signal: USART1_RI1 on PA29 mux A*/
#define MUX_PA29A_USART1_RI1                       (0L)         /**< USART1 signal line function value: USART1_RI1 */
#define PIO_PA29A_USART1_RI1                       (1UL << 29)  /**< USART1 signal: USART1RI1 */
#define PIN_PA24A_USART1_RTS1                      (24L)        /**< USART1 signal: USART1_RTS1 on PA24 mux A*/
#define MUX_PA24A_USART1_RTS1                      (0L)         /**< USART1 signal line function value: USART1_RTS1 */
#define PIO_PA24A_USART1_RTS1                      (1UL << 24)  /**< USART1 signal: USART1RTS1 */
#define PIN_PA21A_USART1_RXD1                      (21L)        /**< USART1 signal: USART1_RXD1 on PA21 mux A*/
#define MUX_PA21A_USART1_RXD1                      (0L)         /**< USART1 signal line function value: USART1_RXD1 */
#define PIO_PA21A_USART1_RXD1                      (1UL << 21)  /**< USART1 signal: USART1RXD1 */
#define PIN_PA23A_USART1_SCK1                      (23L)        /**< USART1 signal: USART1_SCK1 on PA23 mux A*/
#define MUX_PA23A_USART1_SCK1                      (0L)         /**< USART1 signal line function value: USART1_SCK1 */
#define PIO_PA23A_USART1_SCK1                      (1UL << 23)  /**< USART1 signal: USART1SCK1 */
#define PIN_PB4D_USART1_TXD1                       (36L)        /**< USART1 signal: USART1_TXD1 on PB4 mux D*/
#define MUX_PB4D_USART1_TXD1                       (3L)         /**< USART1 signal line function value: USART1_TXD1 */
#define PIO_PB4D_USART1_TXD1                       (1UL << 4)   /**< USART1 signal: USART1TXD1 */
/* ========== PIO definition for USART2 peripheral ========== */
#define PIN_PD19B_USART2_CTS2                      (115L)       /**< USART2 signal: USART2_CTS2 on PD19 mux B*/
#define MUX_PD19B_USART2_CTS2                      (1L)         /**< USART2 signal line function value: USART2_CTS2 */
#define PIO_PD19B_USART2_CTS2                      (1UL << 19)  /**< USART2 signal: USART2CTS2 */
#define PIN_PD4D_USART2_DCD2                       (100L)       /**< USART2 signal: USART2_DCD2 on PD4 mux D*/
#define MUX_PD4D_USART2_DCD2                       (3L)         /**< USART2 signal line function value: USART2_DCD2 */
#define PIO_PD4D_USART2_DCD2                       (1UL << 4)   /**< USART2 signal: USART2DCD2 */
#define PIN_PD6D_USART2_DSR2                       (102L)       /**< USART2 signal: USART2_DSR2 on PD6 mux D*/
#define MUX_PD6D_USART2_DSR2                       (3L)         /**< USART2 signal line function value: USART2_DSR2 */
#define PIO_PD6D_USART2_DSR2                       (1UL << 6)   /**< USART2 signal: USART2DSR2 */
#define PIN_PD5D_USART2_DTR2                       (101L)       /**< USART2 signal: USART2_DTR2 on PD5 mux D*/
#define MUX_PD5D_USART2_DTR2                       (3L)         /**< USART2 signal line function value: USART2_DTR2 */
#define PIO_PD5D_USART2_DTR2                       (1UL << 5)   /**< USART2 signal: USART2DTR2 */
#define PIN_PD7D_USART2_RI2                        (103L)       /**< USART2 signal: USART2_RI2 on PD7 mux D*/
#define MUX_PD7D_USART2_RI2                        (3L)         /**< USART2 signal line function value: USART2_RI2 */
#define PIO_PD7D_USART2_RI2                        (1UL << 7)   /**< USART2 signal: USART2RI2 */
#define PIN_PD18B_USART2_RTS2                      (114L)       /**< USART2 signal: USART2_RTS2 on PD18 mux B*/
#define MUX_PD18B_USART2_RTS2                      (1L)         /**< USART2 signal line function value: USART2_RTS2 */
#define PIO_PD18B_USART2_RTS2                      (1UL << 18)  /**< USART2 signal: USART2RTS2 */
#define PIN_PD15B_USART2_RXD2                      (111L)       /**< USART2 signal: USART2_RXD2 on PD15 mux B*/
#define MUX_PD15B_USART2_RXD2                      (1L)         /**< USART2 signal line function value: USART2_RXD2 */
#define PIO_PD15B_USART2_RXD2                      (1UL << 15)  /**< USART2 signal: USART2RXD2 */
#define PIN_PD17B_USART2_SCK2                      (113L)       /**< USART2 signal: USART2_SCK2 on PD17 mux B*/
#define MUX_PD17B_USART2_SCK2                      (1L)         /**< USART2 signal line function value: USART2_SCK2 */
#define PIO_PD17B_USART2_SCK2                      (1UL << 17)  /**< USART2 signal: USART2SCK2 */
#define PIN_PD16B_USART2_TXD2                      (112L)       /**< USART2 signal: USART2_TXD2 on PD16 mux B*/
#define MUX_PD16B_USART2_TXD2                      (1L)         /**< USART2 signal line function value: USART2_TXD2 */
#define PIO_PD16B_USART2_TXD2                      (1UL << 16)  /**< USART2 signal: USART2TXD2 */
/* ========== PIO definition for ICE peripheral ========== */
#define PIN_PB4X1_ICE_TDI                          (36L)        /**< ICE signal: ICE_TDI on PB4 mux X1*/
#define PIO_PB4X1_ICE_TDI                          (1UL << 4)   /**< ICE signal: ICETDI */
#define PIN_PB5X1_ICE_TDO                          (37L)        /**< ICE signal: ICE_TDO on PB5 mux X1*/
#define PIO_PB5X1_ICE_TDO                          (1UL << 5)   /**< ICE signal: ICETDO */
#define PIN_PB5X1_ICE_TRACESWO                     (37L)        /**< ICE signal: ICE_TRACESWO on PB5 mux X1*/
#define PIO_PB5X1_ICE_TRACESWO                     (1UL << 5)   /**< ICE signal: ICETRACESWO */
#define PIN_PB6X1_ICE_TMS                          (38L)        /**< ICE signal: ICE_TMS on PB6 mux X1*/
#define PIO_PB6X1_ICE_TMS                          (1UL << 6)   /**< ICE signal: ICETMS */
#define PIN_PB6X1_ICE_SWDIO                        (38L)        /**< ICE signal: ICE_SWDIO on PB6 mux X1*/
#define PIO_PB6X1_ICE_SWDIO                        (1UL << 6)   /**< ICE signal: ICESWDIO */
#define PIN_PB7X1_ICE_TCK                          (39L)        /**< ICE signal: ICE_TCK on PB7 mux X1*/
#define PIO_PB7X1_ICE_TCK                          (1UL << 7)   /**< ICE signal: ICETCK */
#define PIN_PB7X1_ICE_SWDCLK                       (39L)        /**< ICE signal: ICE_SWDCLK on PB7 mux X1*/
#define PIO_PB7X1_ICE_SWDCLK                       (1UL << 7)   /**< ICE signal: ICESWDCLK */
/* ========== PIO definition for TPIU peripheral ========== */
#define PIN_PD8D_TPIU_TRACECLK                     (104L)       /**< TPIU signal: TPIU_TRACECLK on PD8 mux D*/
#define MUX_PD8D_TPIU_TRACECLK                     (3L)         /**< TPIU signal line function value: TPIU_TRACECLK */
#define PIO_PD8D_TPIU_TRACECLK                     (1UL << 8)   /**< TPIU signal: TPIUTRACECLK */
#define PIN_PD4C_TPIU_TRACED0                      (100L)       /**< TPIU signal: TPIU_TRACED0 on PD4 mux C*/
#define MUX_PD4C_TPIU_TRACED0                      (2L)         /**< TPIU signal line function value: TPIU_TRACED0 */
#define PIO_PD4C_TPIU_TRACED0                      (1UL << 4)   /**< TPIU signal: TPIUTRACED0 */
#define PIN_PD5C_TPIU_TRACED1                      (101L)       /**< TPIU signal: TPIU_TRACED1 on PD5 mux C*/
#define MUX_PD5C_TPIU_TRACED1                      (2L)         /**< TPIU signal line function value: TPIU_TRACED1 */
#define PIO_PD5C_TPIU_TRACED1                      (1UL << 5)   /**< TPIU signal: TPIUTRACED1 */
#define PIN_PD6C_TPIU_TRACED2                      (102L)       /**< TPIU signal: TPIU_TRACED2 on PD6 mux C*/
#define MUX_PD6C_TPIU_TRACED2                      (2L)         /**< TPIU signal line function value: TPIU_TRACED2 */
#define PIO_PD6C_TPIU_TRACED2                      (1UL << 6)   /**< TPIU signal: TPIUTRACED2 */
#define PIN_PD7C_TPIU_TRACED3                      (103L)       /**< TPIU signal: TPIU_TRACED3 on PD7 mux C*/
#define MUX_PD7C_TPIU_TRACED3                      (2L)         /**< TPIU signal line function value: TPIU_TRACED3 */
#define PIO_PD7C_TPIU_TRACED3                      (1UL << 7)   /**< TPIU signal: TPIUTRACED3 */

#endif /* SAME70Q21B_GPIO_H */

