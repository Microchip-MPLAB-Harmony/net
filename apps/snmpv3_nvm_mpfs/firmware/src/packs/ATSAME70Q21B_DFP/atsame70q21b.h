/**
 * \brief Header file for SAME70Q21B
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
#ifndef SAME70Q21B_H
#define SAME70Q21B_H

// Header version uses Semantic Versioning 2.0.0 (https://semver.org/)
#define HEADER_FORMAT_VERSION "2.0.0"

#define HEADER_FORMAT_VERSION_MAJOR (2)
#define HEADER_FORMAT_VERSION_MINOR (0)

/** \addtogroup SAME70Q21B_definitions SAME70Q21B definitions
  This file defines all structures and symbols for SAME70Q21B:
    - registers and bitfields
    - peripheral base address
    - peripheral ID
    - PIO definitions
 *  @{
 */

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

/** \addtogroup SAME70Q21B_cmsis CMSIS Definitions
 *  @{
 */

/* ************************************************************************** */
/*   CMSIS DEFINITIONS FOR SAME70Q21B                                         */
/* ************************************************************************** */
/** Interrupt Number Definition */
typedef enum IRQn
{
/******  CORTEX-7 Processor Exceptions Numbers ******************************/
  Reset_IRQn                = -15, /**< -15 Reset Vector, invoked on Power up and warm reset */
  NonMaskableInt_IRQn       = -14, /**< -14 Non maskable Interrupt, cannot be stopped or preempted */
  HardFault_IRQn            = -13, /**< -13 Hard Fault, all classes of Fault    */
  MemoryManagement_IRQn     = -12, /**< -12 Memory Management, MPU mismatch, including Access Violation and No Match */
  BusFault_IRQn             = -11, /**< -11 Bus Fault, Pre-Fetch-, Memory Access Fault, other address/memory related Fault */
  UsageFault_IRQn           = -10, /**< -10 Usage Fault, i.e. Undef Instruction, Illegal State Transition */
  SVCall_IRQn               =  -5, /**< -5  System Service Call via SVC instruction */
  DebugMonitor_IRQn         =  -4, /**< -4  Debug Monitor                       */
  PendSV_IRQn               =  -2, /**< -2  Pendable request for system service */
  SysTick_IRQn              =  -1, /**< -1  System Tick Timer                   */
/******  SAME70Q21B specific Interrupt Numbers ***********************************/
  SUPC_IRQn                 =   0, /**< 0   Supply Controller (SUPC)            */
  RSTC_IRQn                 =   1, /**< 1   Reset Controller (RSTC)             */
  RTC_IRQn                  =   2, /**< 2   Real-time Clock (RTC)               */
  RTT_IRQn                  =   3, /**< 3   Real-time Timer (RTT)               */
  WDT_IRQn                  =   4, /**< 4   Watchdog Timer (WDT)                */
  PMC_IRQn                  =   5, /**< 5   Power Management Controller (PMC)   */
  EFC_IRQn                  =   6, /**< 6   Embedded Flash Controller (EFC)     */
  UART0_IRQn                =   7, /**< 7   Universal Asynchronous Receiver Transmitter (UART0) */
  UART1_IRQn                =   8, /**< 8   Universal Asynchronous Receiver Transmitter (UART1) */
  PIOA_IRQn                 =  10, /**< 10  Parallel Input/Output Controller (PIOA) */
  PIOB_IRQn                 =  11, /**< 11  Parallel Input/Output Controller (PIOB) */
  PIOC_IRQn                 =  12, /**< 12  Parallel Input/Output Controller (PIOC) */
  USART0_IRQn               =  13, /**< 13  Universal Synchronous Asynchronous Receiver Transmitter (USART0) */
  USART1_IRQn               =  14, /**< 14  Universal Synchronous Asynchronous Receiver Transmitter (USART1) */
  USART2_IRQn               =  15, /**< 15  Universal Synchronous Asynchronous Receiver Transmitter (USART2) */
  PIOD_IRQn                 =  16, /**< 16  Parallel Input/Output Controller (PIOD) */
  PIOE_IRQn                 =  17, /**< 17  Parallel Input/Output Controller (PIOE) */
  HSMCI_IRQn                =  18, /**< 18  High Speed MultiMedia Card Interface (HSMCI) */
  TWIHS0_IRQn               =  19, /**< 19  Two-wire Interface High Speed (TWIHS0) */
  TWIHS1_IRQn               =  20, /**< 20  Two-wire Interface High Speed (TWIHS1) */
  SPI0_IRQn                 =  21, /**< 21  Serial Peripheral Interface (SPI0)  */
  SSC_IRQn                  =  22, /**< 22  Synchronous Serial Controller (SSC) */
  TC0_CH0_IRQn              =  23, /**< 23  Timer/Counter 0 Channel 0 (TC0)     */
  TC0_CH1_IRQn              =  24, /**< 24  Timer/Counter 0 Channel 1 (TC0)     */
  TC0_CH2_IRQn              =  25, /**< 25  Timer/Counter 0 Channel 2 (TC0)     */
  TC1_CH0_IRQn              =  26, /**< 26  Timer/Counter 1 Channel 0 (TC1)     */
  TC1_CH1_IRQn              =  27, /**< 27  Timer/Counter 1 Channel 1 (TC1)     */
  TC1_CH2_IRQn              =  28, /**< 28  Timer/Counter 1 Channel 2 (TC1)     */
  AFEC0_IRQn                =  29, /**< 29  Analog Front-End Controller (AFEC0) */
  DACC_IRQn                 =  30, /**< 30  Digital-to-Analog Converter Controller (DACC) */
  PWM0_IRQn                 =  31, /**< 31  Pulse Width Modulation Controller (PWM0) */
  ICM_IRQn                  =  32, /**< 32  Integrity Check Monitor (ICM)       */
  ACC_IRQn                  =  33, /**< 33  Analog Comparator Controller (ACC)  */
  USBHS_IRQn                =  34, /**< 34  USB High-Speed Interface (USBHS)    */
  MCAN0_INT0_IRQn           =  35, /**< 35  Controller Area Network (MCAN0)     */
  MCAN0_INT1_IRQn           =  36, /**< 36  Controller Area Network (MCAN0)     */
  MCAN1_INT0_IRQn           =  37, /**< 37  Controller Area Network (MCAN1)     */
  MCAN1_INT1_IRQn           =  38, /**< 38  Controller Area Network (MCAN1)     */
  GMAC_IRQn                 =  39, /**< 39  Gigabit Ethernet MAC (GMAC)         */
  AFEC1_IRQn                =  40, /**< 40  Analog Front-End Controller (AFEC1) */
  TWIHS2_IRQn               =  41, /**< 41  Two-wire Interface High Speed (TWIHS2) */
  SPI1_IRQn                 =  42, /**< 42  Serial Peripheral Interface (SPI1)  */
  QSPI_IRQn                 =  43, /**< 43  Quad Serial Peripheral Interface (QSPI) */
  UART2_IRQn                =  44, /**< 44  Universal Asynchronous Receiver Transmitter (UART2) */
  UART3_IRQn                =  45, /**< 45  Universal Asynchronous Receiver Transmitter (UART3) */
  UART4_IRQn                =  46, /**< 46  Universal Asynchronous Receiver Transmitter (UART4) */
  TC2_CH0_IRQn              =  47, /**< 47  Timer/Counter 2 Channel 0 (TC2)     */
  TC2_CH1_IRQn              =  48, /**< 48  Timer/Counter 2 Channel 1 (TC2)     */
  TC2_CH2_IRQn              =  49, /**< 49  Timer/Counter 2 Channel 2 (TC2)     */
  TC3_CH0_IRQn              =  50, /**< 50  Timer/Counter 3 Channel 0 (TC3)     */
  TC3_CH1_IRQn              =  51, /**< 51  Timer/Counter 3 Channel 1 (TC3)     */
  TC3_CH2_IRQn              =  52, /**< 52  Timer/Counter 3 Channel 2 (TC3)     */
  AES_IRQn                  =  56, /**< 56  Advanced Encryption Standard (AES)  */
  TRNG_IRQn                 =  57, /**< 57  True Random Number Generator (TRNG) */
  XDMAC_IRQn                =  58, /**< 58  Extensible DMA Controller (XDMAC)   */
  ISI_IRQn                  =  59, /**< 59  Image Sensor Interface (ISI)        */
  PWM1_IRQn                 =  60, /**< 60  Pulse Width Modulation Controller (PWM1) */
  FPU_IRQn                  =  61, /**< 61  Floating Point Unit (FPU)           */
  SDRAMC_IRQn               =  62, /**< 62  SDRAM Controller (SDRAMC)           */
  RSWDT_IRQn                =  63, /**< 63  Reinforced Safety Watchdog Timer (RSWDT) */
  CCW_IRQn                  =  64, /**< 64  System Control Block (SCB)          */
  CCF_IRQn                  =  65, /**< 65  System Control Block (SCB)          */
  GMAC_Q1_IRQn              =  66, /**< 66  Gigabit Ethernet MAC (GMAC)         */
  GMAC_Q2_IRQn              =  67, /**< 67  Gigabit Ethernet MAC (GMAC)         */
  IXC_IRQn                  =  68, /**< 68  Floating Point Unit (FPU)           */
  I2SC0_IRQn                =  69, /**< 69  Inter-IC Sound Controller (I2SC0)   */
  I2SC1_IRQn                =  70, /**< 70  Inter-IC Sound Controller (I2SC1)   */
  GMAC_Q3_IRQn              =  71, /**< 71  Gigabit Ethernet MAC (GMAC)         */
  GMAC_Q4_IRQn              =  72, /**< 72  Gigabit Ethernet MAC (GMAC)         */
  GMAC_Q5_IRQn              =  73, /**< 73  Gigabit Ethernet MAC (GMAC)         */

  PERIPH_MAX_IRQn           =  73 /**< Number of peripheral IDs */
} IRQn_Type;

typedef struct _DeviceVectors
{
  /* Stack pointer */
  void* pvStack;
  /* Cortex-M handlers */
  void* pfnReset_Handler;                        /* -15 Reset Vector, invoked on Power up and warm reset */
  void* pfnNonMaskableInt_Handler;               /* -14 Non maskable Interrupt, cannot be stopped or preempted */
  void* pfnHardFault_Handler;                    /* -13 Hard Fault, all classes of Fault */
  void* pfnMemoryManagement_Handler;             /* -12 Memory Management, MPU mismatch, including Access Violation and No Match */
  void* pfnBusFault_Handler;                     /* -11 Bus Fault, Pre-Fetch-, Memory Access Fault, other address/memory related Fault */
  void* pfnUsageFault_Handler;                   /* -10 Usage Fault, i.e. Undef Instruction, Illegal State Transition */
  void* pvReservedC9;
  void* pvReservedC8;
  void* pvReservedC7;
  void* pvReservedC6;
  void* pfnSVCall_Handler;                       /*  -5 System Service Call via SVC instruction */
  void* pfnDebugMonitor_Handler;                 /*  -4 Debug Monitor */
  void* pvReservedC3;
  void* pfnPendSV_Handler;                       /*  -2 Pendable request for system service */
  void* pfnSysTick_Handler;                      /*  -1 System Tick Timer */

  /* Peripheral handlers */
  void* pfnSUPC_Handler;                         /*   0 Supply Controller (SUPC) */
  void* pfnRSTC_Handler;                         /*   1 Reset Controller (RSTC) */
  void* pfnRTC_Handler;                          /*   2 Real-time Clock (RTC) */
  void* pfnRTT_Handler;                          /*   3 Real-time Timer (RTT) */
  void* pfnWDT_Handler;                          /*   4 Watchdog Timer (WDT) */
  void* pfnPMC_Handler;                          /*   5 Power Management Controller (PMC) */
  void* pfnEFC_Handler;                          /*   6 Embedded Flash Controller (EFC) */
  void* pfnUART0_Handler;                        /*   7 Universal Asynchronous Receiver Transmitter (UART0) */
  void* pfnUART1_Handler;                        /*   8 Universal Asynchronous Receiver Transmitter (UART1) */
  void* pvReserved9;
  void* pfnPIOA_Handler;                         /*  10 Parallel Input/Output Controller (PIOA) */
  void* pfnPIOB_Handler;                         /*  11 Parallel Input/Output Controller (PIOB) */
  void* pfnPIOC_Handler;                         /*  12 Parallel Input/Output Controller (PIOC) */
  void* pfnUSART0_Handler;                       /*  13 Universal Synchronous Asynchronous Receiver Transmitter (USART0) */
  void* pfnUSART1_Handler;                       /*  14 Universal Synchronous Asynchronous Receiver Transmitter (USART1) */
  void* pfnUSART2_Handler;                       /*  15 Universal Synchronous Asynchronous Receiver Transmitter (USART2) */
  void* pfnPIOD_Handler;                         /*  16 Parallel Input/Output Controller (PIOD) */
  void* pfnPIOE_Handler;                         /*  17 Parallel Input/Output Controller (PIOE) */
  void* pfnHSMCI_Handler;                        /*  18 High Speed MultiMedia Card Interface (HSMCI) */
  void* pfnTWIHS0_Handler;                       /*  19 Two-wire Interface High Speed (TWIHS0) */
  void* pfnTWIHS1_Handler;                       /*  20 Two-wire Interface High Speed (TWIHS1) */
  void* pfnSPI0_Handler;                         /*  21 Serial Peripheral Interface (SPI0) */
  void* pfnSSC_Handler;                          /*  22 Synchronous Serial Controller (SSC) */
  void* pfnTC0_CH0_Handler;                      /*  23 Timer/Counter 0 Channel 0 (TC0) */
  void* pfnTC0_CH1_Handler;                      /*  24 Timer/Counter 0 Channel 1 (TC0) */
  void* pfnTC0_CH2_Handler;                      /*  25 Timer/Counter 0 Channel 2 (TC0) */
  void* pfnTC1_CH0_Handler;                      /*  26 Timer/Counter 1 Channel 0 (TC1) */
  void* pfnTC1_CH1_Handler;                      /*  27 Timer/Counter 1 Channel 1 (TC1) */
  void* pfnTC1_CH2_Handler;                      /*  28 Timer/Counter 1 Channel 2 (TC1) */
  void* pfnAFEC0_Handler;                        /*  29 Analog Front-End Controller (AFEC0) */
  void* pfnDACC_Handler;                         /*  30 Digital-to-Analog Converter Controller (DACC) */
  void* pfnPWM0_Handler;                         /*  31 Pulse Width Modulation Controller (PWM0) */
  void* pfnICM_Handler;                          /*  32 Integrity Check Monitor (ICM) */
  void* pfnACC_Handler;                          /*  33 Analog Comparator Controller (ACC) */
  void* pfnUSBHS_Handler;                        /*  34 USB High-Speed Interface (USBHS) */
  void* pfnMCAN0_INT0_Handler;                   /*  35 Controller Area Network (MCAN0) */
  void* pfnMCAN0_INT1_Handler;                   /*  36 Controller Area Network (MCAN0) */
  void* pfnMCAN1_INT0_Handler;                   /*  37 Controller Area Network (MCAN1) */
  void* pfnMCAN1_INT1_Handler;                   /*  38 Controller Area Network (MCAN1) */
  void* pfnGMAC_Handler;                         /*  39 Gigabit Ethernet MAC (GMAC) */
  void* pfnAFEC1_Handler;                        /*  40 Analog Front-End Controller (AFEC1) */
  void* pfnTWIHS2_Handler;                       /*  41 Two-wire Interface High Speed (TWIHS2) */
  void* pfnSPI1_Handler;                         /*  42 Serial Peripheral Interface (SPI1) */
  void* pfnQSPI_Handler;                         /*  43 Quad Serial Peripheral Interface (QSPI) */
  void* pfnUART2_Handler;                        /*  44 Universal Asynchronous Receiver Transmitter (UART2) */
  void* pfnUART3_Handler;                        /*  45 Universal Asynchronous Receiver Transmitter (UART3) */
  void* pfnUART4_Handler;                        /*  46 Universal Asynchronous Receiver Transmitter (UART4) */
  void* pfnTC2_CH0_Handler;                      /*  47 Timer/Counter 2 Channel 0 (TC2) */
  void* pfnTC2_CH1_Handler;                      /*  48 Timer/Counter 2 Channel 1 (TC2) */
  void* pfnTC2_CH2_Handler;                      /*  49 Timer/Counter 2 Channel 2 (TC2) */
  void* pfnTC3_CH0_Handler;                      /*  50 Timer/Counter 3 Channel 0 (TC3) */
  void* pfnTC3_CH1_Handler;                      /*  51 Timer/Counter 3 Channel 1 (TC3) */
  void* pfnTC3_CH2_Handler;                      /*  52 Timer/Counter 3 Channel 2 (TC3) */
  void* pvReserved53;
  void* pvReserved54;
  void* pvReserved55;
  void* pfnAES_Handler;                          /*  56 Advanced Encryption Standard (AES) */
  void* pfnTRNG_Handler;                         /*  57 True Random Number Generator (TRNG) */
  void* pfnXDMAC_Handler;                        /*  58 Extensible DMA Controller (XDMAC) */
  void* pfnISI_Handler;                          /*  59 Image Sensor Interface (ISI) */
  void* pfnPWM1_Handler;                         /*  60 Pulse Width Modulation Controller (PWM1) */
  void* pfnFPU_Handler;                          /*  61 Floating Point Unit (FPU) */
  void* pfnSDRAMC_Handler;                       /*  62 SDRAM Controller (SDRAMC) */
  void* pfnRSWDT_Handler;                        /*  63 Reinforced Safety Watchdog Timer (RSWDT) */
  void* pfnCCW_Handler;                          /*  64 System Control Block (SCB) */
  void* pfnCCF_Handler;                          /*  65 System Control Block (SCB) */
  void* pfnGMAC_Q1_Handler;                      /*  66 Gigabit Ethernet MAC (GMAC) */
  void* pfnGMAC_Q2_Handler;                      /*  67 Gigabit Ethernet MAC (GMAC) */
  void* pfnIXC_Handler;                          /*  68 Floating Point Unit (FPU) */
  void* pfnI2SC0_Handler;                        /*  69 Inter-IC Sound Controller (I2SC0) */
  void* pfnI2SC1_Handler;                        /*  70 Inter-IC Sound Controller (I2SC1) */
  void* pfnGMAC_Q3_Handler;                      /*  71 Gigabit Ethernet MAC (GMAC) */
  void* pfnGMAC_Q4_Handler;                      /*  72 Gigabit Ethernet MAC (GMAC) */
  void* pfnGMAC_Q5_Handler;                      /*  73 Gigabit Ethernet MAC (GMAC) */
} DeviceVectors;

/* CORTEX-M7 exception handlers */
void Reset_Handler                 ( void );
void NonMaskableInt_Handler        ( void );
void HardFault_Handler             ( void );
void MemoryManagement_Handler      ( void );
void BusFault_Handler              ( void );
void UsageFault_Handler            ( void );
void SVCall_Handler                ( void );
void DebugMonitor_Handler          ( void );
void PendSV_Handler                ( void );
void SysTick_Handler               ( void );

/* Peripherals interrupt handlers */
void SUPC_Handler                  ( void );
void RSTC_Handler                  ( void );
void RTC_Handler                   ( void );
void RTT_Handler                   ( void );
void WDT_Handler                   ( void );
void PMC_Handler                   ( void );
void EFC_Handler                   ( void );
void UART0_Handler                 ( void );
void UART1_Handler                 ( void );
void PIOA_Handler                  ( void );
void PIOB_Handler                  ( void );
void PIOC_Handler                  ( void );
void USART0_Handler                ( void );
void USART1_Handler                ( void );
void USART2_Handler                ( void );
void PIOD_Handler                  ( void );
void PIOE_Handler                  ( void );
void HSMCI_Handler                 ( void );
void TWIHS0_Handler                ( void );
void TWIHS1_Handler                ( void );
void SPI0_Handler                  ( void );
void SSC_Handler                   ( void );
void TC0_CH0_Handler               ( void );
void TC0_CH1_Handler               ( void );
void TC0_CH2_Handler               ( void );
void TC1_CH0_Handler               ( void );
void TC1_CH1_Handler               ( void );
void TC1_CH2_Handler               ( void );
void AFEC0_Handler                 ( void );
void DACC_Handler                  ( void );
void PWM0_Handler                  ( void );
void ICM_Handler                   ( void );
void ACC_Handler                   ( void );
void USBHS_Handler                 ( void );
void MCAN0_INT0_Handler            ( void );
void MCAN0_INT1_Handler            ( void );
void MCAN1_INT0_Handler            ( void );
void MCAN1_INT1_Handler            ( void );
void GMAC_Handler                  ( void );
void AFEC1_Handler                 ( void );
void TWIHS2_Handler                ( void );
void SPI1_Handler                  ( void );
void QSPI_Handler                  ( void );
void UART2_Handler                 ( void );
void UART3_Handler                 ( void );
void UART4_Handler                 ( void );
void TC2_CH0_Handler               ( void );
void TC2_CH1_Handler               ( void );
void TC2_CH2_Handler               ( void );
void TC3_CH0_Handler               ( void );
void TC3_CH1_Handler               ( void );
void TC3_CH2_Handler               ( void );
void AES_Handler                   ( void );
void TRNG_Handler                  ( void );
void XDMAC_Handler                 ( void );
void ISI_Handler                   ( void );
void PWM1_Handler                  ( void );
void FPU_Handler                   ( void );
void SDRAMC_Handler                ( void );
void RSWDT_Handler                 ( void );
void CCW_Handler                   ( void );
void CCF_Handler                   ( void );
void GMAC_Q1_Handler               ( void );
void GMAC_Q2_Handler               ( void );
void IXC_Handler                   ( void );
void I2SC0_Handler                 ( void );
void I2SC1_Handler                 ( void );
void GMAC_Q3_Handler               ( void );
void GMAC_Q4_Handler               ( void );
void GMAC_Q5_Handler               ( void );

/*
 * \brief Configuration of the CORTEX-M7 Processor and Core Peripherals
 */
#define __CM7_REV                 0x0101 /**< CM7 Core Revision                                                         */
#define __NVIC_PRIO_BITS               3 /**< Number of Bits used for Priority Levels                                   */
#define __Vendor_SysTickConfig         0 /**< Set to 1 if different SysTick Config is used                              */
#define __MPU_PRESENT                  1 /**< MPU present or not                                                        */
#define __VTOR_PRESENT                 1 /**< Vector Table Offset Register present or not                               */
#define __FPU_PRESENT                  1 /**< FPU present or not                                                        */
#define __FPU_DP                       1 /**< Double Precision FPU                                                      */
#define __ICACHE_PRESENT               1 /**< Instruction Cache present                                                 */
#define __DCACHE_PRESENT               1 /**< Data Cache present                                                        */
#define __ITCM_PRESENT                 1 /**< Instruction TCM present                                                   */
#define __DTCM_PRESENT                 1 /**< Data TCM present                                                          */
#define __DEBUG_LVL                    1
#define __TRACE_LVL                    1
#define __ARCH_ARM                     1
#define __ARCH_ARM_CORTEX_M            1
#define __DEVICE_IS_SAM                1

/*
 * \brief CMSIS includes
 */
#include "core_cm7.h"

/** @}  end of SAME70Q21B_cmsis CMSIS Definitions */

/** \defgroup SAME70Q21B_api Peripheral Software API
 *  @{
 */

/* ************************************************************************** */
/**  SOFTWARE PERIPHERAL API DEFINITION FOR SAME70Q21B                        */
/* ************************************************************************** */
#include "component/acc.h"
#include "component/aes.h"
#include "component/afec.h"
#include "component/chipid.h"
#include "component/dacc.h"
#include "component/efc.h"
#include "component/gmac.h"
#include "component/gpbr.h"
#include "component/hsmci.h"
#include "component/i2sc.h"
#include "component/icm.h"
#include "component/isi.h"
#include "component/matrix.h"
#include "component/mcan.h"
#include "component/pio.h"
#include "component/pmc.h"
#include "component/pwm.h"
#include "component/qspi.h"
#include "component/rstc.h"
#include "component/rswdt.h"
#include "component/rtc.h"
#include "component/rtt.h"
#include "component/sdramc.h"
#include "component/smc.h"
#include "component/spi.h"
#include "component/ssc.h"
#include "component/supc.h"
#include "component/tc.h"
#include "component/trng.h"
#include "component/twihs.h"
#include "component/uart.h"
#include "component/usart.h"
#include "component/usbhs.h"
#include "component/utmi.h"
#include "component/wdt.h"
#include "component/xdmac.h"

/** @}  end of Peripheral Software API */

/** \addtogroup SAME70Q21B_id Peripheral Ids Definitions
 *  @{
 */

/* ************************************************************************** */
/*  PERIPHERAL ID DEFINITIONS FOR SAME70Q21B                                  */
/* ************************************************************************** */
#define ID_ACC           ( 33) /**< \brief Analog Comparator Controller (ACC) */
#define ID_AES           ( 56) /**< \brief Advanced Encryption Standard (AES) */
#define ID_AFEC0         ( 29) /**< \brief Analog Front-End Controller (AFEC0) */
#define ID_AFEC1         ( 40) /**< \brief Analog Front-End Controller (AFEC1) */
#define ID_DACC          ( 30) /**< \brief Digital-to-Analog Converter Controller (DACC) */
#define ID_EFC           (  6) /**< \brief Embedded Flash Controller (EFC) */
#define ID_GMAC          ( 39) /**< \brief Gigabit Ethernet MAC (GMAC) */
#define ID_HSMCI         ( 18) /**< \brief High Speed MultiMedia Card Interface (HSMCI) */
#define ID_I2SC0         ( 69) /**< \brief Inter-IC Sound Controller (I2SC0) */
#define ID_I2SC1         ( 70) /**< \brief Inter-IC Sound Controller (I2SC1) */
#define ID_ICM           ( 32) /**< \brief Integrity Check Monitor (ICM) */
#define ID_ISI           ( 59) /**< \brief Image Sensor Interface (ISI) */
#define ID_MCAN0         ( 35) /**< \brief Controller Area Network (MCAN0) */
#define ID_MCAN1         ( 37) /**< \brief Controller Area Network (MCAN1) */
#define ID_PIOA          ( 10) /**< \brief Parallel Input/Output Controller (PIOA) */
#define ID_PIOB          ( 11) /**< \brief Parallel Input/Output Controller (PIOB) */
#define ID_PIOC          ( 12) /**< \brief Parallel Input/Output Controller (PIOC) */
#define ID_PIOD          ( 16) /**< \brief Parallel Input/Output Controller (PIOD) */
#define ID_PIOE          ( 17) /**< \brief Parallel Input/Output Controller (PIOE) */
#define ID_PMC           (  5) /**< \brief Power Management Controller (PMC) */
#define ID_PWM0          ( 31) /**< \brief Pulse Width Modulation Controller (PWM0) */
#define ID_PWM1          ( 60) /**< \brief Pulse Width Modulation Controller (PWM1) */
#define ID_QSPI          ( 43) /**< \brief Quad Serial Peripheral Interface (QSPI) */
#define ID_RSTC          (  1) /**< \brief Reset Controller (RSTC) */
#define ID_RSWDT         ( 63) /**< \brief Reinforced Safety Watchdog Timer (RSWDT) */
#define ID_RTC           (  2) /**< \brief Real-time Clock (RTC) */
#define ID_RTT           (  3) /**< \brief Real-time Timer (RTT) */
#define ID_SDRAMC        ( 62) /**< \brief SDRAM Controller (SDRAMC) */
#define ID_SMC           (  9) /**< \brief Static Memory Controller (SMC) */
#define ID_SPI0          ( 21) /**< \brief Serial Peripheral Interface (SPI0) */
#define ID_SPI1          ( 42) /**< \brief Serial Peripheral Interface (SPI1) */
#define ID_SSC           ( 22) /**< \brief Synchronous Serial Controller (SSC) */
#define ID_TRNG          ( 57) /**< \brief True Random Number Generator (TRNG) */
#define ID_TWIHS0        ( 19) /**< \brief Two-wire Interface High Speed (TWIHS0) */
#define ID_TWIHS1        ( 20) /**< \brief Two-wire Interface High Speed (TWIHS1) */
#define ID_TWIHS2        ( 41) /**< \brief Two-wire Interface High Speed (TWIHS2) */
#define ID_UART0         (  7) /**< \brief Universal Asynchronous Receiver Transmitter (UART0) */
#define ID_UART1         (  8) /**< \brief Universal Asynchronous Receiver Transmitter (UART1) */
#define ID_UART2         ( 44) /**< \brief Universal Asynchronous Receiver Transmitter (UART2) */
#define ID_UART3         ( 45) /**< \brief Universal Asynchronous Receiver Transmitter (UART3) */
#define ID_UART4         ( 46) /**< \brief Universal Asynchronous Receiver Transmitter (UART4) */
#define ID_USART0        ( 13) /**< \brief Universal Synchronous Asynchronous Receiver Transmitter (USART0) */
#define ID_USART1        ( 14) /**< \brief Universal Synchronous Asynchronous Receiver Transmitter (USART1) */
#define ID_USART2        ( 15) /**< \brief Universal Synchronous Asynchronous Receiver Transmitter (USART2) */
#define ID_USBHS         ( 34) /**< \brief USB High-Speed Interface (USBHS) */
#define ID_WDT           (  4) /**< \brief Watchdog Timer (WDT) */
#define ID_XDMAC         ( 58) /**< \brief Extensible DMA Controller (XDMAC) */

#define ID_PERIPH_MAX  ( 71) /**< \brief Number of peripheral IDs */

/** @}  end of Peripheral Ids Definitions */

/** \addtogroup SAME70Q21B_base Peripheral Base Address Definitions
 *  @{
 */

/* ************************************************************************** */
/*   REGISTER STRUCTURE ADDRESS DEFINITIONS FOR SAME70Q21B                    */
/* ************************************************************************** */
#define ACC_REGS                         ((acc_registers_t*)0x40044000)                /**< \brief ACC Registers Address        */
#define AES_REGS                         ((aes_registers_t*)0x4006c000)                /**< \brief AES Registers Address        */
#define AFEC0_REGS                       ((afec_registers_t*)0x4003c000)               /**< \brief AFEC0 Registers Address      */
#define AFEC1_REGS                       ((afec_registers_t*)0x40064000)               /**< \brief AFEC1 Registers Address      */
#define CHIPID_REGS                      ((chipid_registers_t*)0x400e0940)             /**< \brief CHIPID Registers Address     */
#define DACC_REGS                        ((dacc_registers_t*)0x40040000)               /**< \brief DACC Registers Address       */
#define EFC_REGS                         ((efc_registers_t*)0x400e0c00)                /**< \brief EFC Registers Address        */
#define GMAC_REGS                        ((gmac_registers_t*)0x40050000)               /**< \brief GMAC Registers Address       */
#define GPBR_REGS                        ((gpbr_registers_t*)0x400e1890)               /**< \brief GPBR Registers Address       */
#define HSMCI_REGS                       ((hsmci_registers_t*)0x40000000)              /**< \brief HSMCI Registers Address      */
#define I2SC0_REGS                       ((i2sc_registers_t*)0x4008c000)               /**< \brief I2SC0 Registers Address      */
#define I2SC1_REGS                       ((i2sc_registers_t*)0x40090000)               /**< \brief I2SC1 Registers Address      */
#define ICM_REGS                         ((icm_registers_t*)0x40048000)                /**< \brief ICM Registers Address        */
#define ISI_REGS                         ((isi_registers_t*)0x4004c000)                /**< \brief ISI Registers Address        */
#define MATRIX_REGS                      ((matrix_registers_t*)0x40088000)             /**< \brief MATRIX Registers Address     */
#define MCAN0_REGS                       ((mcan_registers_t*)0x40030000)               /**< \brief MCAN0 Registers Address      */
#define MCAN1_REGS                       ((mcan_registers_t*)0x40034000)               /**< \brief MCAN1 Registers Address      */
#define PIOA_REGS                        ((pio_registers_t*)0x400e0e00)                /**< \brief PIOA Registers Address       */
#define PIOB_REGS                        ((pio_registers_t*)0x400e1000)                /**< \brief PIOB Registers Address       */
#define PIOC_REGS                        ((pio_registers_t*)0x400e1200)                /**< \brief PIOC Registers Address       */
#define PIOD_REGS                        ((pio_registers_t*)0x400e1400)                /**< \brief PIOD Registers Address       */
#define PIOE_REGS                        ((pio_registers_t*)0x400e1600)                /**< \brief PIOE Registers Address       */
#define PMC_REGS                         ((pmc_registers_t*)0x400e0600)                /**< \brief PMC Registers Address        */
#define PWM0_REGS                        ((pwm_registers_t*)0x40020000)                /**< \brief PWM0 Registers Address       */
#define PWM1_REGS                        ((pwm_registers_t*)0x4005c000)                /**< \brief PWM1 Registers Address       */
#define QSPI_REGS                        ((qspi_registers_t*)0x4007c000)               /**< \brief QSPI Registers Address       */
#define RSTC_REGS                        ((rstc_registers_t*)0x400e1800)               /**< \brief RSTC Registers Address       */
#define RSWDT_REGS                       ((rswdt_registers_t*)0x400e1900)              /**< \brief RSWDT Registers Address      */
#define RTC_REGS                         ((rtc_registers_t*)0x400e1860)                /**< \brief RTC Registers Address        */
#define RTT_REGS                         ((rtt_registers_t*)0x400e1830)                /**< \brief RTT Registers Address        */
#define SDRAMC_REGS                      ((sdramc_registers_t*)0x40084000)             /**< \brief SDRAMC Registers Address     */
#define SMC_REGS                         ((smc_registers_t*)0x40080000)                /**< \brief SMC Registers Address        */
#define SPI0_REGS                        ((spi_registers_t*)0x40008000)                /**< \brief SPI0 Registers Address       */
#define SPI1_REGS                        ((spi_registers_t*)0x40058000)                /**< \brief SPI1 Registers Address       */
#define SSC_REGS                         ((ssc_registers_t*)0x40004000)                /**< \brief SSC Registers Address        */
#define SUPC_REGS                        ((supc_registers_t*)0x400e1810)               /**< \brief SUPC Registers Address       */
#define TC0_REGS                         ((tc_registers_t*)0x4000c000)                 /**< \brief TC0 Registers Address        */
#define TC1_REGS                         ((tc_registers_t*)0x40010000)                 /**< \brief TC1 Registers Address        */
#define TC2_REGS                         ((tc_registers_t*)0x40014000)                 /**< \brief TC2 Registers Address        */
#define TC3_REGS                         ((tc_registers_t*)0x40054000)                 /**< \brief TC3 Registers Address        */
#define TRNG_REGS                        ((trng_registers_t*)0x40070000)               /**< \brief TRNG Registers Address       */
#define TWIHS0_REGS                      ((twihs_registers_t*)0x40018000)              /**< \brief TWIHS0 Registers Address     */
#define TWIHS1_REGS                      ((twihs_registers_t*)0x4001c000)              /**< \brief TWIHS1 Registers Address     */
#define TWIHS2_REGS                      ((twihs_registers_t*)0x40060000)              /**< \brief TWIHS2 Registers Address     */
#define UART0_REGS                       ((uart_registers_t*)0x400e0800)               /**< \brief UART0 Registers Address      */
#define UART1_REGS                       ((uart_registers_t*)0x400e0a00)               /**< \brief UART1 Registers Address      */
#define UART2_REGS                       ((uart_registers_t*)0x400e1a00)               /**< \brief UART2 Registers Address      */
#define UART3_REGS                       ((uart_registers_t*)0x400e1c00)               /**< \brief UART3 Registers Address      */
#define UART4_REGS                       ((uart_registers_t*)0x400e1e00)               /**< \brief UART4 Registers Address      */
#define USART0_REGS                      ((usart_registers_t*)0x40024000)              /**< \brief USART0 Registers Address     */
#define USART1_REGS                      ((usart_registers_t*)0x40028000)              /**< \brief USART1 Registers Address     */
#define USART2_REGS                      ((usart_registers_t*)0x4002c000)              /**< \brief USART2 Registers Address     */
#define USBHS_REGS                       ((usbhs_registers_t*)0x40038000)              /**< \brief USBHS Registers Address      */
#define UTMI_REGS                        ((utmi_registers_t*)0x400e0400)               /**< \brief UTMI Registers Address       */
#define WDT_REGS                         ((wdt_registers_t*)0x400e1850)                /**< \brief WDT Registers Address        */
#define XDMAC_REGS                       ((xdmac_registers_t*)0x40078000)              /**< \brief XDMAC Registers Address      */

/* ************************************************************************** */
/*   BASE ADDRESS DEFINITIONS FOR SAME70Q21B                                  */
/* ************************************************************************** */

#define ACC_BASE_ADDRESS                 (0x40044000UL)                                /**< \brief ACC Base Address             */
#define AES_BASE_ADDRESS                 (0x4006c000UL)                                /**< \brief AES Base Address             */
#define AFEC0_BASE_ADDRESS               (0x4003c000UL)                                /**< \brief AFEC0 Base Address           */
#define AFEC1_BASE_ADDRESS               (0x40064000UL)                                /**< \brief AFEC1 Base Address           */
#define CHIPID_BASE_ADDRESS              (0x400e0940UL)                                /**< \brief CHIPID Base Address          */
#define DACC_BASE_ADDRESS                (0x40040000UL)                                /**< \brief DACC Base Address            */
#define EFC_BASE_ADDRESS                 (0x400e0c00UL)                                /**< \brief EFC Base Address             */
#define GMAC_BASE_ADDRESS                (0x40050000UL)                                /**< \brief GMAC Base Address            */
#define GPBR_BASE_ADDRESS                (0x400e1890UL)                                /**< \brief GPBR Base Address            */
#define HSMCI_BASE_ADDRESS               (0x40000000UL)                                /**< \brief HSMCI Base Address           */
#define I2SC0_BASE_ADDRESS               (0x4008c000UL)                                /**< \brief I2SC0 Base Address           */
#define I2SC1_BASE_ADDRESS               (0x40090000UL)                                /**< \brief I2SC1 Base Address           */
#define ICM_BASE_ADDRESS                 (0x40048000UL)                                /**< \brief ICM Base Address             */
#define ISI_BASE_ADDRESS                 (0x4004c000UL)                                /**< \brief ISI Base Address             */
#define MATRIX_BASE_ADDRESS              (0x40088000UL)                                /**< \brief MATRIX Base Address          */
#define MCAN0_BASE_ADDRESS               (0x40030000UL)                                /**< \brief MCAN0 Base Address           */
#define MCAN1_BASE_ADDRESS               (0x40034000UL)                                /**< \brief MCAN1 Base Address           */
#define PIOA_BASE_ADDRESS                (0x400e0e00UL)                                /**< \brief PIOA Base Address            */
#define PIOB_BASE_ADDRESS                (0x400e1000UL)                                /**< \brief PIOB Base Address            */
#define PIOC_BASE_ADDRESS                (0x400e1200UL)                                /**< \brief PIOC Base Address            */
#define PIOD_BASE_ADDRESS                (0x400e1400UL)                                /**< \brief PIOD Base Address            */
#define PIOE_BASE_ADDRESS                (0x400e1600UL)                                /**< \brief PIOE Base Address            */
#define PMC_BASE_ADDRESS                 (0x400e0600UL)                                /**< \brief PMC Base Address             */
#define PWM0_BASE_ADDRESS                (0x40020000UL)                                /**< \brief PWM0 Base Address            */
#define PWM1_BASE_ADDRESS                (0x4005c000UL)                                /**< \brief PWM1 Base Address            */
#define QSPI_BASE_ADDRESS                (0x4007c000UL)                                /**< \brief QSPI Base Address            */
#define RSTC_BASE_ADDRESS                (0x400e1800UL)                                /**< \brief RSTC Base Address            */
#define RSWDT_BASE_ADDRESS               (0x400e1900UL)                                /**< \brief RSWDT Base Address           */
#define RTC_BASE_ADDRESS                 (0x400e1860UL)                                /**< \brief RTC Base Address             */
#define RTT_BASE_ADDRESS                 (0x400e1830UL)                                /**< \brief RTT Base Address             */
#define SDRAMC_BASE_ADDRESS              (0x40084000UL)                                /**< \brief SDRAMC Base Address          */
#define SMC_BASE_ADDRESS                 (0x40080000UL)                                /**< \brief SMC Base Address             */
#define SPI0_BASE_ADDRESS                (0x40008000UL)                                /**< \brief SPI0 Base Address            */
#define SPI1_BASE_ADDRESS                (0x40058000UL)                                /**< \brief SPI1 Base Address            */
#define SSC_BASE_ADDRESS                 (0x40004000UL)                                /**< \brief SSC Base Address             */
#define SUPC_BASE_ADDRESS                (0x400e1810UL)                                /**< \brief SUPC Base Address            */
#define TC0_BASE_ADDRESS                 (0x4000c000UL)                                /**< \brief TC0 Base Address             */
#define TC1_BASE_ADDRESS                 (0x40010000UL)                                /**< \brief TC1 Base Address             */
#define TC2_BASE_ADDRESS                 (0x40014000UL)                                /**< \brief TC2 Base Address             */
#define TC3_BASE_ADDRESS                 (0x40054000UL)                                /**< \brief TC3 Base Address             */
#define TRNG_BASE_ADDRESS                (0x40070000UL)                                /**< \brief TRNG Base Address            */
#define TWIHS0_BASE_ADDRESS              (0x40018000UL)                                /**< \brief TWIHS0 Base Address          */
#define TWIHS1_BASE_ADDRESS              (0x4001c000UL)                                /**< \brief TWIHS1 Base Address          */
#define TWIHS2_BASE_ADDRESS              (0x40060000UL)                                /**< \brief TWIHS2 Base Address          */
#define UART0_BASE_ADDRESS               (0x400e0800UL)                                /**< \brief UART0 Base Address           */
#define UART1_BASE_ADDRESS               (0x400e0a00UL)                                /**< \brief UART1 Base Address           */
#define UART2_BASE_ADDRESS               (0x400e1a00UL)                                /**< \brief UART2 Base Address           */
#define UART3_BASE_ADDRESS               (0x400e1c00UL)                                /**< \brief UART3 Base Address           */
#define UART4_BASE_ADDRESS               (0x400e1e00UL)                                /**< \brief UART4 Base Address           */
#define USART0_BASE_ADDRESS              (0x40024000UL)                                /**< \brief USART0 Base Address          */
#define USART1_BASE_ADDRESS              (0x40028000UL)                                /**< \brief USART1 Base Address          */
#define USART2_BASE_ADDRESS              (0x4002c000UL)                                /**< \brief USART2 Base Address          */
#define USBHS_BASE_ADDRESS               (0x40038000UL)                                /**< \brief USBHS Base Address           */
#define UTMI_BASE_ADDRESS                (0x400e0400UL)                                /**< \brief UTMI Base Address            */
#define WDT_BASE_ADDRESS                 (0x400e1850UL)                                /**< \brief WDT Base Address             */
#define XDMAC_BASE_ADDRESS               (0x40078000UL)                                /**< \brief XDMAC Base Address           */

/** \addtogroup SAME70Q21B_pio Peripheral Pio Definitions
 *  @{
 */

/* ************************************************************************** */
/*   PIO DEFINITIONS FOR SAME70Q21B                                           */
/* ************************************************************************** */
#include "pio/same70q21b.h"

/** @}  end of Peripheral Pio Definitions */

/* ************************************************************************** */
/*   MEMORY MAPPING DEFINITIONS FOR SAME70Q21B                                */
/* ************************************************************************** */

#define PERIPHERALS_SIZE               (0x20000000UL)      /* 524288kB Memory segment type: io */
#define SYSTEM_SIZE                    (0x10000000UL)      /* 262144kB Memory segment type: io */
#define QSPIMEM_SIZE                   (0x20000000UL)      /* 524288kB Memory segment type: other */
#define AXIMX_SIZE                     (0x00100000UL)      /* 1024kB Memory segment type: other */
#define ITCM_SIZE                      (0x00200000UL)      /* 2048kB Memory segment type: other */
#define IFLASH_SIZE                    (0x00200000UL)      /* 2048kB Memory segment type: flash */
#define IFLASH_PAGE_SIZE               (0x00000200UL)
#define IFLASH_NB_OF_PAGES             (0x00001000UL)
#define IROM_SIZE                      (0x00004000UL)      /*   16kB Memory segment type: rom */
#define DTCM_SIZE                      (0x00020000UL)      /*  128kB Memory segment type: other */
#define IRAM_SIZE                      (0x00060000UL)      /*  384kB Memory segment type: ram */
#define EBI_CS0_SIZE                   (0x01000000UL)      /* 16384kB Memory segment type: other */
#define EBI_CS1_SIZE                   (0x01000000UL)      /* 16384kB Memory segment type: other */
#define EBI_CS2_SIZE                   (0x01000000UL)      /* 16384kB Memory segment type: other */
#define EBI_CS3_SIZE                   (0x01000000UL)      /* 16384kB Memory segment type: other */
#define SDRAM_CS_SIZE                  (0x10000000UL)      /* 262144kB Memory segment type: other */
#define PERIPHERALS_ADDR               (0x40000000UL)      /**< PERIPHERALS base address (type: io) */
#define SYSTEM_ADDR                    (0xe0000000UL)      /**< SYSTEM base address (type: io) */
#define QSPIMEM_ADDR                   (0x80000000UL)      /**< QSPIMEM base address (type: other) */
#define AXIMX_ADDR                     (0xa0000000UL)      /**< AXIMX base address (type: other) */
#define ITCM_ADDR                      (0x00000000UL)      /**< ITCM base address (type: other) */
#define IFLASH_ADDR                    (0x00400000UL)      /**< IFLASH base address (type: flash) */
#define IROM_ADDR                      (0x00800000UL)      /**< IROM base address (type: rom) */
#define DTCM_ADDR                      (0x20000000UL)      /**< DTCM base address (type: other) */
#define IRAM_ADDR                      (0x20400000UL)      /**< IRAM base address (type: ram) */
#define EBI_CS0_ADDR                   (0x60000000UL)      /**< EBI_CS0 base address (type: other) */
#define EBI_CS1_ADDR                   (0x61000000UL)      /**< EBI_CS1 base address (type: other) */
#define EBI_CS2_ADDR                   (0x62000000UL)      /**< EBI_CS2 base address (type: other) */
#define EBI_CS3_ADDR                   (0x63000000UL)      /**< EBI_CS3 base address (type: other) */
#define SDRAM_CS_ADDR                  (0x70000000UL)      /**< SDRAM_CS base address (type: other) */

/* ************************************************************************** */
/**  DEVICE SIGNATURES FOR SAME70Q21B                                         */
/* ************************************************************************** */
#define CHIP_JTAGID                    (0x05b3d03fUL)
#define CHIP_CIDR                      (0xa1020e01UL)
#define CHIP_EXID                      (0x00000002UL)

/* ************************************************************************** */
/**  ELECTRICAL DEFINITIONS FOR SAME70Q21B                                    */
/* ************************************************************************** */
#define CHIP_FREQ_SLCK_RC_MIN               (20000UL)
#define CHIP_FREQ_SLCK_RC                   (32000UL)      /**< \brief Typical Slow Clock Internal RC frequency */
#define CHIP_FREQ_SLCK_RC_MAX               (44000UL)
#define CHIP_FREQ_MAINCK_RC_4MHZ          (4000000UL)
#define CHIP_FREQ_MAINCK_RC_8MHZ          (8000000UL)
#define CHIP_FREQ_MAINCK_RC_12MHZ        (12000000UL)
#define CHIP_FREQ_CPU_MAX               (300000000UL)
#define CHIP_FREQ_XTAL_32K                  (32768UL)
#define CHIP_FREQ_XTAL_12M               (12000000UL)
#define CHIP_FREQ_FWS_0                  (23000000UL)      /**< \brief Maximum operating frequency when FWS is 0 */
#define CHIP_FREQ_FWS_1                  (46000000UL)      /**< \brief Maximum operating frequency when FWS is 1 */
#define CHIP_FREQ_FWS_2                  (69000000UL)      /**< \brief Maximum operating frequency when FWS is 2 */
#define CHIP_FREQ_FWS_3                  (92000000UL)      /**< \brief Maximum operating frequency when FWS is 3 */
#define CHIP_FREQ_FWS_4                 (115000000UL)      /**< \brief Maximum operating frequency when FWS is 4 */
#define CHIP_FREQ_FWS_5                 (138000000UL)      /**< \brief Maximum operating frequency when FWS is 5 */
#define CHIP_FREQ_FWS_6                 (150000000UL)      /**< \brief Maximum operating frequency when FWS is 6 */
#define CHIP_FREQ_FWS_NUMBER                    (7UL)      /**< \brief Number of FWS ranges */

#ifdef __cplusplus
}
#endif

/** @}  end of SAME70Q21B definitions */

#endif /* SAME70Q21B_H */

