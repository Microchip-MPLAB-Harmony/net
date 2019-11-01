/**
 * \brief Header file for SAM9X60
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

/* file generated from device description version 2019-08-22T13:04:25Z */
#ifndef _SAM9X60_H_
#define _SAM9X60_H_

// Header version uses Semantic Versioning 2.0.0 (https://semver.org/)
#define HEADER_FORMAT_VERSION "2.0.0"

#define HEADER_FORMAT_VERSION_MAJOR (2)
#define HEADER_FORMAT_VERSION_MINOR (0)

/** \addtogroup SAM9X60_definitions SAM9X60 definitions
  This file defines all structures and symbols for SAM9X60:
    - registers and bitfields
    - peripheral base address
    - peripheral ID
    - PIO definitions
 *  @{
 */

#ifdef __cplusplus
 extern "C" {
#endif

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
#  include <stdint.h>
#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */

#if !defined(SKIP_INTEGER_LITERALS)
#  if defined(_U_) || defined(_L_) || defined(_UL_)
#    error "Integer Literals macros already defined elsewhere"
#  endif

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/* Macros that deal with adding suffixes to integer literal constants for C/C++ */
#  define _U_(x) (x ## U)    /**< C code: Unsigned integer literal constant value */
#  define _L_(x) (x ## L)    /**< C code: Long integer literal constant value */
#  define _UL_(x) (x ## UL)  /**< C code: Unsigned Long integer literal constant value */

#else /* Assembler */

#  define _U_(x) x    /**< Assembler: Unsigned integer literal constant value */
#  define _L_(x) x    /**< Assembler: Long integer literal constant value */
#  define _UL_(x) x   /**< Assembler: Unsigned Long integer literal constant value */
#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* SKIP_INTEGER_LITERALS */
/** @}  end of Atmel Global Defines */

/* ************************************************************************** */
/*   CMSIS DEFINITIONS FOR SAM9X60                                            */
/* ************************************************************************** */
#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** Interrupt Number Definition */
typedef enum IRQn
{
/******  ARM926EJS Processor Exceptions Numbers ******************************/
/******  SAM9X60 specific Interrupt Numbers ***********************************/
  EXT_FIQ_IRQn              =   0, /**< 0   Advanced Interrupt Controller (AIC) */
  PMC_IRQn                  =   1, /**< 1   Shared between PMC RSTC RTT PIT WDT RTC (PMC) */
  RSTC_IRQn                 =   1, /**< 1   Shared between PMC RSTC RTT PIT WDT RTC (RSTC) */
  RTT_IRQn                  =   1, /**< 1   Shared between PMC RSTC RTT PIT WDT RTC (RTT) */
  PIT_IRQn                  =   1, /**< 1   Shared between PMC RSTC RTT PIT WDT RTC (PIT) */
  WDT_IRQn                  =   1, /**< 1   Shared between PMC RSTC RTT PIT WDT RTC (WDT) */
  RTC_IRQn                  =   1, /**< 1   Shared between PMC RSTC RTT PIT WDT RTC (RTC) */
  PIOA_IRQn                 =   2, /**< 2   Parallel Input/Output Controller (PIOA) */
  PIOB_IRQn                 =   3, /**< 3   Parallel Input/Output Controller (PIOB) */
  PIOC_IRQn                 =   4, /**< 4   Parallel Input/Output Controller (PIOC) */
  FLEXCOM0_IRQn             =   5, /**< 5   Flexible Serial Communication (FLEXCOM0) */
  FLEXCOM1_IRQn             =   6, /**< 6   Flexible Serial Communication (FLEXCOM1) */
  FLEXCOM2_IRQn             =   7, /**< 7   Flexible Serial Communication (FLEXCOM2) */
  FLEXCOM3_IRQn             =   8, /**< 8   Flexible Serial Communication (FLEXCOM3) */
  FLEXCOM6_IRQn             =   9, /**< 9   Flexible Serial Communication (FLEXCOM6) */
  FLEXCOM7_IRQn             =  10, /**< 10  Flexible Serial Communication (FLEXCOM7) */
  FLEXCOM8_IRQn             =  11, /**< 11  Flexible Serial Communication (FLEXCOM8) */
  SDMMC0_IRQn               =  12, /**< 12  Secure Digital MultiMedia Card Controller (SDMMC0) */
  FLEXCOM4_IRQn             =  13, /**< 13  Flexible Serial Communication (FLEXCOM4) */
  FLEXCOM5_IRQn             =  14, /**< 14  Flexible Serial Communication (FLEXCOM5) */
  FLEXCOM9_IRQn             =  15, /**< 15  Flexible Serial Communication (FLEXCOM9) */
  FLEXCOM10_IRQn            =  16, /**< 16  Flexible Serial Communication (FLEXCOM10) */
  TC0_IRQn                  =  17, /**< 17  Timer Counter (TC0)                 */
  PWM_IRQn                  =  18, /**< 18  Pulse Width Modulation Controller (PWM) */
  ADC_IRQn                  =  19, /**< 19  Analog-to-Digital Converter (ADC)   */
  XDMAC_IRQn                =  20, /**< 20  Extensible DMA Controller (XDMAC)   */
  MATRIX_IRQn               =  21, /**< 21  AHB Bus Matrix (MATRIX)             */
  UDPHS_IRQn                =  23, /**< 23  USB High Speed Device Port (UDPHS)  */
  EMAC0_IRQn                =  24, /**< 24  Ethernet MAC 10/100 (EMAC0)         */
  LCDC_IRQn                 =  25, /**< 25  LCD Controller (LCDC)               */
  SDMMC1_IRQn               =  26, /**< 26  Secure Digital MultiMedia Card Controller (SDMMC1) */
  EMAC1_IRQn                =  27, /**< 27  Ethernet MAC 10/100 (EMAC1)         */
  SSC_IRQn                  =  28, /**< 28  Synchronous Serial Controller (SSC) */
  CAN0_IRQn                 =  29, /**< 29  Controller Area Network (CAN0)      */
  CAN1_IRQn                 =  30, /**< 30  Controller Area Network (CAN1)      */
  EXT_IRQ_IRQn              =  31, /**< 31  Advanced Interrupt Controller (AIC) */
  FLEXCOM11_IRQn            =  32, /**< 32  Flexible Serial Communication (FLEXCOM11) */
  FLEXCOM12_IRQn            =  33, /**< 33  Flexible Serial Communication (FLEXCOM12) */
  I2SMCC_IRQn               =  34, /**< 34  Inter-IC Sound Multi Channel Controller (I2SMCC) */
  QSPI_IRQn                 =  35, /**< 35  Quad Serial Peripheral Interface (QSPI) */
  GFX2D_IRQn                =  36, /**< 36  2D Graphics Engine (GFX2D)          */
  PIT64B_IRQn               =  37, /**< 37  Periodic Interval Timer 64-bit (PIT64B) */
  TRNG_IRQn                 =  38, /**< 38  True Random Number Generator (TRNG) */
  AES_IRQn                  =  39, /**< 39  Advanced Encryption Standard (AES)  */
  TDES_IRQn                 =  40, /**< 40  Triple Data Encryption Standard (TDES) */
  SHA_IRQn                  =  41, /**< 41  Secure Hash Algorithm (SHA)         */
  CLASSD_IRQn               =  42, /**< 42  Audio Class D Amplifier (CLASSD)    */
  ISI_IRQn                  =  43, /**< 43  Image Sensor Interface (ISI)        */
  PIOD_IRQn                 =  44, /**< 44  Parallel Input/Output Controller (PIOD) */
  TC1_IRQn                  =  45, /**< 45  Timer Counter (TC1)                 */
  OTPC_IRQn                 =  46, /**< 46  OTP Memory Controller (OTPC)        */
  DBGU_IRQn                 =  47, /**< 47  Debug Unit (DBGU)                   */
  PMECC_IRQn                =  48, /**< 48  Shared between PMECC PMERRLOC (PMECC) */
  PMERRLOC_IRQn             =  48, /**< 48  Shared between PMECC PMERRLOC (PMERRLOC) */
  SDRAMC_IRQn               =  49, /**< 49  Shared between SDRAMC MPDDRC SMC (SDRAMC) */
  MPDDRC_IRQn               =  49, /**< 49  Shared between SDRAMC MPDDRC SMC (MPDDRC) */
  SMC_IRQn                  =  49, /**< 49  Shared between SDRAMC MPDDRC SMC (SMC) */

  PERIPH_MAX_IRQn           =  49  /**< Max peripheral ID */
} IRQn_Type;
#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct _DeviceVectors
{
  /* Stack pointer */
  void* pvStack;
  /* Cortex-M handlers */
  void* pvReservedC15;
  void* pvReservedC14;
  void* pvReservedC13;
  void* pvReservedC12;
  void* pvReservedC11;
  void* pvReservedC10;
  void* pvReservedC9;
  void* pvReservedC8;
  void* pvReservedC7;
  void* pvReservedC6;
  void* pvReservedC5;
  void* pvReservedC4;
  void* pvReservedC3;
  void* pvReservedC2;
  void* pvReservedC1;

  /* Peripheral handlers */
  void* pfnEXT_FIQ_Handler;                      /*   0 Advanced Interrupt Controller (AIC) */
  void* pfnSYSC_Handler;                         /*   1 System Controller Interrupt (PMC RSTC RTT PIT WDT RTC) */
  void* pfnPIOA_Handler;                         /*   2 Parallel Input/Output Controller (PIOA) */
  void* pfnPIOB_Handler;                         /*   3 Parallel Input/Output Controller (PIOB) */
  void* pfnPIOC_Handler;                         /*   4 Parallel Input/Output Controller (PIOC) */
  void* pfnFLEXCOM0_Handler;                     /*   5 Flexible Serial Communication (FLEXCOM0) */
  void* pfnFLEXCOM1_Handler;                     /*   6 Flexible Serial Communication (FLEXCOM1) */
  void* pfnFLEXCOM2_Handler;                     /*   7 Flexible Serial Communication (FLEXCOM2) */
  void* pfnFLEXCOM3_Handler;                     /*   8 Flexible Serial Communication (FLEXCOM3) */
  void* pfnFLEXCOM6_Handler;                     /*   9 Flexible Serial Communication (FLEXCOM6) */
  void* pfnFLEXCOM7_Handler;                     /*  10 Flexible Serial Communication (FLEXCOM7) */
  void* pfnFLEXCOM8_Handler;                     /*  11 Flexible Serial Communication (FLEXCOM8) */
  void* pfnSDMMC0_Handler;                       /*  12 Secure Digital MultiMedia Card Controller (SDMMC0) */
  void* pfnFLEXCOM4_Handler;                     /*  13 Flexible Serial Communication (FLEXCOM4) */
  void* pfnFLEXCOM5_Handler;                     /*  14 Flexible Serial Communication (FLEXCOM5) */
  void* pfnFLEXCOM9_Handler;                     /*  15 Flexible Serial Communication (FLEXCOM9) */
  void* pfnFLEXCOM10_Handler;                    /*  16 Flexible Serial Communication (FLEXCOM10) */
  void* pfnTC0_Handler;                          /*  17 Timer Counter (TC0) */
  void* pfnPWM_Handler;                          /*  18 Pulse Width Modulation Controller (PWM) */
  void* pfnADC_Handler;                          /*  19 Analog-to-Digital Converter (ADC) */
  void* pfnXDMAC_Handler;                        /*  20 Extensible DMA Controller (XDMAC) */
  void* pfnMATRIX_Handler;                       /*  21 AHB Bus Matrix (MATRIX) */
  void* pvReserved22;
  void* pfnUDPHS_Handler;                        /*  23 USB High Speed Device Port (UDPHS) */
  void* pfnEMAC0_Handler;                        /*  24 Ethernet MAC 10/100 (EMAC0) */
  void* pfnLCDC_Handler;                         /*  25 LCD Controller (LCDC) */
  void* pfnSDMMC1_Handler;                       /*  26 Secure Digital MultiMedia Card Controller (SDMMC1) */
  void* pfnEMAC1_Handler;                        /*  27 Ethernet MAC 10/100 (EMAC1) */
  void* pfnSSC_Handler;                          /*  28 Synchronous Serial Controller (SSC) */
  void* pfnCAN0_Handler;                         /*  29 Controller Area Network (CAN0) */
  void* pfnCAN1_Handler;                         /*  30 Controller Area Network (CAN1) */
  void* pfnEXT_IRQ_Handler;                      /*  31 Advanced Interrupt Controller (AIC) */
  void* pfnFLEXCOM11_Handler;                    /*  32 Flexible Serial Communication (FLEXCOM11) */
  void* pfnFLEXCOM12_Handler;                    /*  33 Flexible Serial Communication (FLEXCOM12) */
  void* pfnI2SMCC_Handler;                       /*  34 Inter-IC Sound Multi Channel Controller (I2SMCC) */
  void* pfnQSPI_Handler;                         /*  35 Quad Serial Peripheral Interface (QSPI) */
  void* pfnGFX2D_Handler;                        /*  36 2D Graphics Engine (GFX2D) */
  void* pfnPIT64B_Handler;                       /*  37 Periodic Interval Timer 64-bit (PIT64B) */
  void* pfnTRNG_Handler;                         /*  38 True Random Number Generator (TRNG) */
  void* pfnAES_Handler;                          /*  39 Advanced Encryption Standard (AES) */
  void* pfnTDES_Handler;                         /*  40 Triple Data Encryption Standard (TDES) */
  void* pfnSHA_Handler;                          /*  41 Secure Hash Algorithm (SHA) */
  void* pfnCLASSD_Handler;                       /*  42 Audio Class D Amplifier (CLASSD) */
  void* pfnISI_Handler;                          /*  43 Image Sensor Interface (ISI) */
  void* pfnPIOD_Handler;                         /*  44 Parallel Input/Output Controller (PIOD) */
  void* pfnTC1_Handler;                          /*  45 Timer Counter (TC1) */
  void* pfnOTPC_Handler;                         /*  46 OTP Memory Controller (OTPC) */
  void* pfnDBGU_Handler;                         /*  47 Debug Unit (DBGU) */
  void* pfnECC_Handler;                          /*  48 ECC Controller (PMECC PMERRLOC) */
  void* pfnMC_Handler;                           /*  49 Memory Controller (SDRAMC MPDDRC SMC) */
} DeviceVectors;

/* Defines for Deprecated Interrupt and Exceptions handler names */
#define pfnMemManage_Handler      pfnMemoryManagement_Handler     /**< \deprecated  Backward compatibility for ASF*/
#define pfnDebugMon_Handler       pfnDebugMonitor_Handler         /**< \deprecated  Backward compatibility for ASF*/
#define pfnNMI_Handler            pfnNonMaskableInt_Handler       /**< \deprecated  Backward compatibility for ASF*/
#define pfnSVC_Handler            pfnSVCall_Handler               /**< \deprecated  Backward compatibility for ASF*/

#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
#if !defined DONT_USE_PREDEFINED_CORE_HANDLERS
/* ARM926EJS exception handlers */
#endif /* DONT_USE_PREDEFINED_CORE_HANDLERS */

#if !defined DONT_USE_PREDEFINED_PERIPHERALS_HANDLERS
/* Peripherals interrupt handlers */
void EXT_FIQ_Handler               ( void );
void SYSC_Handler                  ( void );
void PIOA_Handler                  ( void );
void PIOB_Handler                  ( void );
void PIOC_Handler                  ( void );
void FLEXCOM0_Handler              ( void );
void FLEXCOM1_Handler              ( void );
void FLEXCOM2_Handler              ( void );
void FLEXCOM3_Handler              ( void );
void FLEXCOM6_Handler              ( void );
void FLEXCOM7_Handler              ( void );
void FLEXCOM8_Handler              ( void );
void SDMMC0_Handler                ( void );
void FLEXCOM4_Handler              ( void );
void FLEXCOM5_Handler              ( void );
void FLEXCOM9_Handler              ( void );
void FLEXCOM10_Handler             ( void );
void TC0_Handler                   ( void );
void PWM_Handler                   ( void );
void ADC_Handler                   ( void );
void XDMAC_Handler                 ( void );
void MATRIX_Handler                ( void );
void UDPHS_Handler                 ( void );
void EMAC0_Handler                 ( void );
void LCDC_Handler                  ( void );
void SDMMC1_Handler                ( void );
void EMAC1_Handler                 ( void );
void SSC_Handler                   ( void );
void CAN0_Handler                  ( void );
void CAN1_Handler                  ( void );
void EXT_IRQ_Handler               ( void );
void FLEXCOM11_Handler             ( void );
void FLEXCOM12_Handler             ( void );
void I2SMCC_Handler                ( void );
void QSPI_Handler                  ( void );
void GFX2D_Handler                 ( void );
void PIT64B_Handler                ( void );
void TRNG_Handler                  ( void );
void AES_Handler                   ( void );
void TDES_Handler                  ( void );
void SHA_Handler                   ( void );
void CLASSD_Handler                ( void );
void ISI_Handler                   ( void );
void PIOD_Handler                  ( void );
void TC1_Handler                   ( void );
void OTPC_Handler                  ( void );
void DBGU_Handler                  ( void );
void ECC_Handler                   ( void );
void MC_Handler                    ( void );
#endif /* DONT_USE_PREDEFINED_PERIPHERALS_HANDLERS */
/* Defines for Deprecated Interrupt and Exceptions handler names */
#define MemManage_Handler         MemoryManagement_Handler        /**< \deprecated  Backward compatibility*/
#define DebugMon_Handler          DebugMonitor_Handler            /**< \deprecated  Backward compatibility*/
#define NMI_Handler               NonMaskableInt_Handler          /**< \deprecated  Backward compatibility*/
#define SVC_Handler               SVCall_Handler                  /**< \deprecated  Backward compatibility*/

#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */

/*
 * \brief Configuration of the ARM926EJS Processor and Core Peripherals
 */

/*
 * \brief CMSIS includes
 */
#ifdef __cplusplus
  #define __I  volatile       /**< Defines 'read-only'  permissions */
#else
  #define __I  volatile const /**< Defines 'read-only'  permissions */
#endif
#define   __O  volatile       /**< Defines 'write-only' permissions */
#define   __IO volatile       /**< Defines 'read/write' permissions */

/** \defgroup SAM9X60_api Peripheral Software API
 *  @{
 */

/* ************************************************************************** */
/**  SOFTWARE PERIPHERAL API DEFINITION FOR SAM9X60                           */
/* ************************************************************************** */
#include "component/adc.h"
#include "component/aes.h"
#include "component/aic.h"
#include "component/bsc.h"
#include "component/can.h"
#include "component/classd.h"
#include "component/dbgu.h"
#include "component/emac.h"
#include "component/flexcom.h"
#include "component/gfx2d.h"
#include "component/gpbr.h"
#include "component/i2smcc.h"
#include "component/isi.h"
#include "component/lcdc.h"
#include "component/matrix.h"
#include "component/mpddrc.h"
#include "component/otpc.h"
#include "component/pio.h"
#include "component/pit.h"
#include "component/pit64b.h"
#include "component/pmc.h"
#include "component/pmecc.h"
#include "component/pmerrloc.h"
#include "component/pwm.h"
#include "component/qspi.h"
#include "component/rstc.h"
#include "component/rtc.h"
#include "component/rtt.h"
#include "component/sckc.h"
#include "component/sdmmc.h"
#include "component/sdramc.h"
#include "component/sfr.h"
#include "component/sha.h"
#include "component/shdwc.h"
#include "component/smc.h"
#include "component/ssc.h"
#include "component/syscwp.h"
#include "component/tc.h"
#include "component/tdes.h"
#include "component/trng.h"
#include "component/udphs.h"
#include "component/uhpfs.h"
#include "component/uhphs.h"
#include "component/wdt.h"
#include "component/xdmac.h"
/** @}  end of Peripheral Software API */

/** \addtogroup SAM9X60_id Peripheral Ids Definitions
 *  @{
 */

/* ************************************************************************** */
/*  PERIPHERAL ID DEFINITIONS FOR SAM9X60                                     */
/* ************************************************************************** */
#define ID_PIOA          (  2) /**< \brief Parallel Input/Output Controller (PIOA) */
#define ID_PIOB          (  3) /**< \brief Parallel Input/Output Controller (PIOB) */
#define ID_PIOC          (  4) /**< \brief Parallel Input/Output Controller (PIOC) */
#define ID_FLEXCOM0      (  5) /**< \brief Flexible Serial Communication (FLEXCOM0) */
#define ID_FLEXCOM1      (  6) /**< \brief Flexible Serial Communication (FLEXCOM1) */
#define ID_FLEXCOM2      (  7) /**< \brief Flexible Serial Communication (FLEXCOM2) */
#define ID_FLEXCOM3      (  8) /**< \brief Flexible Serial Communication (FLEXCOM3) */
#define ID_FLEXCOM6      (  9) /**< \brief Flexible Serial Communication (FLEXCOM6) */
#define ID_FLEXCOM7      ( 10) /**< \brief Flexible Serial Communication (FLEXCOM7) */
#define ID_FLEXCOM8      ( 11) /**< \brief Flexible Serial Communication (FLEXCOM8) */
#define ID_SDMMC0        ( 12) /**< \brief Secure Digital MultiMedia Card Controller (SDMMC0) */
#define ID_FLEXCOM4      ( 13) /**< \brief Flexible Serial Communication (FLEXCOM4) */
#define ID_FLEXCOM5      ( 14) /**< \brief Flexible Serial Communication (FLEXCOM5) */
#define ID_FLEXCOM9      ( 15) /**< \brief Flexible Serial Communication (FLEXCOM9) */
#define ID_FLEXCOM10     ( 16) /**< \brief Flexible Serial Communication (FLEXCOM10) */
#define ID_TC0           ( 17) /**< \brief Timer Counter (TC0) */
#define ID_PWM           ( 18) /**< \brief Pulse Width Modulation Controller (PWM) */
#define ID_ADC           ( 19) /**< \brief Analog-to-Digital Converter (ADC) */
#define ID_XDMAC         ( 20) /**< \brief Extensible DMA Controller (XDMAC) */
#define ID_MATRIX        ( 21) /**< \brief AHB Bus Matrix (MATRIX) */
#define ID_UHPHS_EHCI    ( 22) /**< \brief USB Host High Speed Port (UHPHS_EHCI) */
#define ID_UDPHS         ( 23) /**< \brief USB High Speed Device Port (UDPHS) */
#define ID_EMAC0         ( 24) /**< \brief Ethernet MAC 10/100 (EMAC0) */
#define ID_LCDC          ( 25) /**< \brief LCD Controller (LCDC) */
#define ID_SDMMC1        ( 26) /**< \brief Secure Digital MultiMedia Card Controller (SDMMC1) */
#define ID_EMAC1         ( 27) /**< \brief Ethernet MAC 10/100 (EMAC1) */
#define ID_SSC           ( 28) /**< \brief Synchronous Serial Controller (SSC) */
#define ID_CAN0          ( 29) /**< \brief Controller Area Network (CAN0) */
#define ID_CAN1          ( 30) /**< \brief Controller Area Network (CAN1) */
#define ID_FLEXCOM11     ( 32) /**< \brief Flexible Serial Communication (FLEXCOM11) */
#define ID_FLEXCOM12     ( 33) /**< \brief Flexible Serial Communication (FLEXCOM12) */
#define ID_I2SMCC        ( 34) /**< \brief Inter-IC Sound Multi Channel Controller (I2SMCC) */
#define ID_QSPI          ( 35) /**< \brief Quad Serial Peripheral Interface (QSPI) */
#define ID_GFX2D         ( 36) /**< \brief 2D Graphics Engine (GFX2D) */
#define ID_PIT64B        ( 37) /**< \brief Periodic Interval Timer 64-bit (PIT64B) */
#define ID_TRNG          ( 38) /**< \brief True Random Number Generator (TRNG) */
#define ID_AES           ( 39) /**< \brief Advanced Encryption Standard (AES) */
#define ID_TDES          ( 40) /**< \brief Triple Data Encryption Standard (TDES) */
#define ID_SHA           ( 41) /**< \brief Secure Hash Algorithm (SHA) */
#define ID_CLASSD        ( 42) /**< \brief Audio Class D Amplifier (CLASSD) */
#define ID_ISI           ( 43) /**< \brief Image Sensor Interface (ISI) */
#define ID_PIOD          ( 44) /**< \brief Parallel Input/Output Controller (PIOD) */
#define ID_TC1           ( 45) /**< \brief Timer Counter (TC1) */
#define ID_OTPC          ( 46) /**< \brief OTP Memory Controller (OTPC) */
#define ID_DBGU          ( 47) /**< \brief Debug Unit (DBGU) */
#define ID_PMECC         ( 48) /**< \brief Programmable Multibit Error Correction Code Controller (PMECC) */
#define ID_SDRAMC        ( 49) /**< \brief SDRAM Controller (SDRAMC) */

#define ID_PERIPH_MAX    ( 49) /**< \brief Number of peripheral IDs */
/** @}  end of Peripheral Ids Definitions */

/** \addtogroup SAM9X60_base Peripheral Base Address Definitions
 *  @{
 */

/* ************************************************************************** */
/*   REGISTER STRUCTURE ADDRESS DEFINITIONS FOR SAM9X60                       */
/* ************************************************************************** */
#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
#define ADC_REGS                         ((adc_registers_t*)0xf804c000)                /**< \brief ADC Registers Address        */
#define AES_REGS                         ((aes_registers_t*)0xf0034000)                /**< \brief AES Registers Address        */
#define AIC_REGS                         ((aic_registers_t*)0xfffff100)                /**< \brief AIC Registers Address        */
#define BSC_REGS                         ((bsc_registers_t*)0xfffffe54)                /**< \brief BSC Registers Address        */
#define CAN0_REGS                        ((can_registers_t*)0xf8000000)                /**< \brief CAN0 Registers Address       */
#define CAN1_REGS                        ((can_registers_t*)0xf8004000)                /**< \brief CAN1 Registers Address       */
#define CLASSD_REGS                      ((classd_registers_t*)0xf003c000)             /**< \brief CLASSD Registers Address     */
#define DBGU_REGS                        ((dbgu_registers_t*)0xfffff200)               /**< \brief DBGU Registers Address       */
#define EMAC0_REGS                       ((emac_registers_t*)0xf802c000)               /**< \brief EMAC0 Registers Address      */
#define EMAC1_REGS                       ((emac_registers_t*)0xf8030000)               /**< \brief EMAC1 Registers Address      */
#define FLEXCOM0_REGS                    ((flexcom_registers_t*)0xf801c000)            /**< \brief FLEXCOM0 Registers Address   */
#define FLEXCOM1_REGS                    ((flexcom_registers_t*)0xf8020000)            /**< \brief FLEXCOM1 Registers Address   */
#define FLEXCOM2_REGS                    ((flexcom_registers_t*)0xf8024000)            /**< \brief FLEXCOM2 Registers Address   */
#define FLEXCOM3_REGS                    ((flexcom_registers_t*)0xf8028000)            /**< \brief FLEXCOM3 Registers Address   */
#define FLEXCOM4_REGS                    ((flexcom_registers_t*)0xf0000000)            /**< \brief FLEXCOM4 Registers Address   */
#define FLEXCOM5_REGS                    ((flexcom_registers_t*)0xf0004000)            /**< \brief FLEXCOM5 Registers Address   */
#define FLEXCOM6_REGS                    ((flexcom_registers_t*)0xf8010000)            /**< \brief FLEXCOM6 Registers Address   */
#define FLEXCOM7_REGS                    ((flexcom_registers_t*)0xf8014000)            /**< \brief FLEXCOM7 Registers Address   */
#define FLEXCOM8_REGS                    ((flexcom_registers_t*)0xf8018000)            /**< \brief FLEXCOM8 Registers Address   */
#define FLEXCOM9_REGS                    ((flexcom_registers_t*)0xf8040000)            /**< \brief FLEXCOM9 Registers Address   */
#define FLEXCOM10_REGS                   ((flexcom_registers_t*)0xf8044000)            /**< \brief FLEXCOM10 Registers Address  */
#define FLEXCOM11_REGS                   ((flexcom_registers_t*)0xf0020000)            /**< \brief FLEXCOM11 Registers Address  */
#define FLEXCOM12_REGS                   ((flexcom_registers_t*)0xf0024000)            /**< \brief FLEXCOM12 Registers Address  */
#define GFX2D_REGS                       ((gfx2d_registers_t*)0xf0018000)              /**< \brief GFX2D Registers Address      */
#define GPBR_REGS                        ((gpbr_registers_t*)0xfffffe60)               /**< \brief GPBR Registers Address       */
#define I2SMCC_REGS                      ((i2smcc_registers_t*)0xf001c000)             /**< \brief I2SMCC Registers Address     */
#define ISI_REGS                         ((isi_registers_t*)0xf8048000)                /**< \brief ISI Registers Address        */
#define LCDC_REGS                        ((lcdc_registers_t*)0xf8038000)               /**< \brief LCDC Registers Address       */
#define MATRIX_REGS                      ((matrix_registers_t*)0xffffde00)             /**< \brief MATRIX Registers Address     */
#define MPDDRC_REGS                      ((mpddrc_registers_t*)0xffffe800)             /**< \brief MPDDRC Registers Address     */
#define OTPC_REGS                        ((otpc_registers_t*)0xeff00000)               /**< \brief OTPC Registers Address       */
#define PIOA_REGS                        ((pio_registers_t*)0xfffff400)                /**< \brief PIOA Registers Address       */
#define PIOB_REGS                        ((pio_registers_t*)0xfffff600)                /**< \brief PIOB Registers Address       */
#define PIOC_REGS                        ((pio_registers_t*)0xfffff800)                /**< \brief PIOC Registers Address       */
#define PIOD_REGS                        ((pio_registers_t*)0xfffffa00)                /**< \brief PIOD Registers Address       */
#define PIT_REGS                         ((pit_registers_t*)0xfffffe40)                /**< \brief PIT Registers Address        */
#define PIT64B_REGS                      ((pit64b_registers_t*)0xf0028000)             /**< \brief PIT64B Registers Address     */
#define PMC_REGS                         ((pmc_registers_t*)0xfffffc00)                /**< \brief PMC Registers Address        */
#define PMECC_REGS                       ((pmecc_registers_t*)0xffffe000)              /**< \brief PMECC Registers Address      */
#define PMERRLOC_REGS                    ((pmerrloc_registers_t*)0xffffe600)           /**< \brief PMERRLOC Registers Address   */
#define PWM_REGS                         ((pwm_registers_t*)0xf8034000)                /**< \brief PWM Registers Address        */
#define QSPI_REGS                        ((qspi_registers_t*)0xf0014000)               /**< \brief QSPI Registers Address       */
#define RSTC_REGS                        ((rstc_registers_t*)0xfffffe00)               /**< \brief RSTC Registers Address       */
#define RTC_REGS                         ((rtc_registers_t*)0xfffffea8)                /**< \brief RTC Registers Address        */
#define RTT_REGS                         ((rtt_registers_t*)0xfffffe20)                /**< \brief RTT Registers Address        */
#define SCKC_REGS                        ((sckc_registers_t*)0xfffffe50)               /**< \brief SCKC Registers Address       */
#define SDMMC0_REGS                      ((sdmmc_registers_t*)0x80000000)              /**< \brief SDMMC0 Registers Address     */
#define SDMMC1_REGS                      ((sdmmc_registers_t*)0x90000000)              /**< \brief SDMMC1 Registers Address     */
#define SDRAMC_REGS                      ((sdramc_registers_t*)0xffffec00)             /**< \brief SDRAMC Registers Address     */
#define SFR_REGS                         ((sfr_registers_t*)0xf8050000)                /**< \brief SFR Registers Address        */
#define SHA_REGS                         ((sha_registers_t*)0xf002c000)                /**< \brief SHA Registers Address        */
#define SHDWC_REGS                       ((shdwc_registers_t*)0xfffffe10)              /**< \brief SHDWC Registers Address      */
#define SMC_REGS                         ((smc_registers_t*)0xffffea00)                /**< \brief SMC Registers Address        */
#define SSC_REGS                         ((ssc_registers_t*)0xf0010000)                /**< \brief SSC Registers Address        */
#define SYSCWP_REGS                      ((syscwp_registers_t*)0xfffffedc)             /**< \brief SYSCWP Registers Address     */
#define TC0_REGS                         ((tc_registers_t*)0xf8008000)                 /**< \brief TC0 Registers Address        */
#define TC1_REGS                         ((tc_registers_t*)0xf800c000)                 /**< \brief TC1 Registers Address        */
#define TDES_REGS                        ((tdes_registers_t*)0xf0038000)               /**< \brief TDES Registers Address       */
#define TRNG_REGS                        ((trng_registers_t*)0xf0030000)               /**< \brief TRNG Registers Address       */
#define UDPHS_REGS                       ((udphs_registers_t*)0xf803c000)              /**< \brief UDPHS Registers Address      */
#define UHPHS_OHCI_REGS                  ((uhpfs_registers_t*)0x00600000)              /**< \brief UHPHS_OHCI Registers Address */
#define UHPHS_EHCI_REGS                  ((uhphs_registers_t*)0x00700000)              /**< \brief UHPHS_EHCI Registers Address */
#define WDT_REGS                         ((wdt_registers_t*)0xffffff80)                /**< \brief WDT Registers Address        */
#define XDMAC_REGS                       ((xdmac_registers_t*)0xf0008000)              /**< \brief XDMAC Registers Address      */
#endif /* (defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
/** @}  end of Peripheral Base Address Definitions */

/** \addtogroup SAM9X60_base Peripheral Base Address Definitions
 *  @{
 */

/* ************************************************************************** */
/*   BASE ADDRESS DEFINITIONS FOR SAM9X60                                     */
/* ************************************************************************** */
#define ADC_BASE_ADDRESS                 _UL_(0xf804c000)                              /**< \brief ADC Base Address */
#define AES_BASE_ADDRESS                 _UL_(0xf0034000)                              /**< \brief AES Base Address */
#define AIC_BASE_ADDRESS                 _UL_(0xfffff100)                              /**< \brief AIC Base Address */
#define BSC_BASE_ADDRESS                 _UL_(0xfffffe54)                              /**< \brief BSC Base Address */
#define CAN0_BASE_ADDRESS                _UL_(0xf8000000)                              /**< \brief CAN0 Base Address */
#define CAN1_BASE_ADDRESS                _UL_(0xf8004000)                              /**< \brief CAN1 Base Address */
#define CLASSD_BASE_ADDRESS              _UL_(0xf003c000)                              /**< \brief CLASSD Base Address */
#define DBGU_BASE_ADDRESS                _UL_(0xfffff200)                              /**< \brief DBGU Base Address */
#define EMAC0_BASE_ADDRESS               _UL_(0xf802c000)                              /**< \brief EMAC0 Base Address */
#define EMAC1_BASE_ADDRESS               _UL_(0xf8030000)                              /**< \brief EMAC1 Base Address */
#define FLEXCOM0_BASE_ADDRESS            _UL_(0xf801c000)                              /**< \brief FLEXCOM0 Base Address */
#define FLEXCOM1_BASE_ADDRESS            _UL_(0xf8020000)                              /**< \brief FLEXCOM1 Base Address */
#define FLEXCOM2_BASE_ADDRESS            _UL_(0xf8024000)                              /**< \brief FLEXCOM2 Base Address */
#define FLEXCOM3_BASE_ADDRESS            _UL_(0xf8028000)                              /**< \brief FLEXCOM3 Base Address */
#define FLEXCOM4_BASE_ADDRESS            _UL_(0xf0000000)                              /**< \brief FLEXCOM4 Base Address */
#define FLEXCOM5_BASE_ADDRESS            _UL_(0xf0004000)                              /**< \brief FLEXCOM5 Base Address */
#define FLEXCOM6_BASE_ADDRESS            _UL_(0xf8010000)                              /**< \brief FLEXCOM6 Base Address */
#define FLEXCOM7_BASE_ADDRESS            _UL_(0xf8014000)                              /**< \brief FLEXCOM7 Base Address */
#define FLEXCOM8_BASE_ADDRESS            _UL_(0xf8018000)                              /**< \brief FLEXCOM8 Base Address */
#define FLEXCOM9_BASE_ADDRESS            _UL_(0xf8040000)                              /**< \brief FLEXCOM9 Base Address */
#define FLEXCOM10_BASE_ADDRESS           _UL_(0xf8044000)                              /**< \brief FLEXCOM10 Base Address */
#define FLEXCOM11_BASE_ADDRESS           _UL_(0xf0020000)                              /**< \brief FLEXCOM11 Base Address */
#define FLEXCOM12_BASE_ADDRESS           _UL_(0xf0024000)                              /**< \brief FLEXCOM12 Base Address */
#define GFX2D_BASE_ADDRESS               _UL_(0xf0018000)                              /**< \brief GFX2D Base Address */
#define GPBR_BASE_ADDRESS                _UL_(0xfffffe60)                              /**< \brief GPBR Base Address */
#define I2SMCC_BASE_ADDRESS              _UL_(0xf001c000)                              /**< \brief I2SMCC Base Address */
#define ISI_BASE_ADDRESS                 _UL_(0xf8048000)                              /**< \brief ISI Base Address */
#define LCDC_BASE_ADDRESS                _UL_(0xf8038000)                              /**< \brief LCDC Base Address */
#define MATRIX_BASE_ADDRESS              _UL_(0xffffde00)                              /**< \brief MATRIX Base Address */
#define MPDDRC_BASE_ADDRESS              _UL_(0xffffe800)                              /**< \brief MPDDRC Base Address */
#define OTPC_BASE_ADDRESS                _UL_(0xeff00000)                              /**< \brief OTPC Base Address */
#define PIOA_BASE_ADDRESS                _UL_(0xfffff400)                              /**< \brief PIOA Base Address */
#define PIOB_BASE_ADDRESS                _UL_(0xfffff600)                              /**< \brief PIOB Base Address */
#define PIOC_BASE_ADDRESS                _UL_(0xfffff800)                              /**< \brief PIOC Base Address */
#define PIOD_BASE_ADDRESS                _UL_(0xfffffa00)                              /**< \brief PIOD Base Address */
#define PIT_BASE_ADDRESS                 _UL_(0xfffffe40)                              /**< \brief PIT Base Address */
#define PIT64B_BASE_ADDRESS              _UL_(0xf0028000)                              /**< \brief PIT64B Base Address */
#define PMC_BASE_ADDRESS                 _UL_(0xfffffc00)                              /**< \brief PMC Base Address */
#define PMECC_BASE_ADDRESS               _UL_(0xffffe000)                              /**< \brief PMECC Base Address */
#define PMERRLOC_BASE_ADDRESS            _UL_(0xffffe600)                              /**< \brief PMERRLOC Base Address */
#define PWM_BASE_ADDRESS                 _UL_(0xf8034000)                              /**< \brief PWM Base Address */
#define QSPI_BASE_ADDRESS                _UL_(0xf0014000)                              /**< \brief QSPI Base Address */
#define RSTC_BASE_ADDRESS                _UL_(0xfffffe00)                              /**< \brief RSTC Base Address */
#define RTC_BASE_ADDRESS                 _UL_(0xfffffea8)                              /**< \brief RTC Base Address */
#define RTT_BASE_ADDRESS                 _UL_(0xfffffe20)                              /**< \brief RTT Base Address */
#define SCKC_BASE_ADDRESS                _UL_(0xfffffe50)                              /**< \brief SCKC Base Address */
#define SDMMC0_BASE_ADDRESS              _UL_(0x80000000)                              /**< \brief SDMMC0 Base Address */
#define SDMMC1_BASE_ADDRESS              _UL_(0x90000000)                              /**< \brief SDMMC1 Base Address */
#define SDRAMC_BASE_ADDRESS              _UL_(0xffffec00)                              /**< \brief SDRAMC Base Address */
#define SFR_BASE_ADDRESS                 _UL_(0xf8050000)                              /**< \brief SFR Base Address */
#define SHA_BASE_ADDRESS                 _UL_(0xf002c000)                              /**< \brief SHA Base Address */
#define SHDWC_BASE_ADDRESS               _UL_(0xfffffe10)                              /**< \brief SHDWC Base Address */
#define SMC_BASE_ADDRESS                 _UL_(0xffffea00)                              /**< \brief SMC Base Address */
#define SSC_BASE_ADDRESS                 _UL_(0xf0010000)                              /**< \brief SSC Base Address */
#define SYSCWP_BASE_ADDRESS              _UL_(0xfffffedc)                              /**< \brief SYSCWP Base Address */
#define TC0_BASE_ADDRESS                 _UL_(0xf8008000)                              /**< \brief TC0 Base Address */
#define TC1_BASE_ADDRESS                 _UL_(0xf800c000)                              /**< \brief TC1 Base Address */
#define TDES_BASE_ADDRESS                _UL_(0xf0038000)                              /**< \brief TDES Base Address */
#define TRNG_BASE_ADDRESS                _UL_(0xf0030000)                              /**< \brief TRNG Base Address */
#define UDPHS_BASE_ADDRESS               _UL_(0xf803c000)                              /**< \brief UDPHS Base Address */
#define UHPHS_OHCI_BASE_ADDRESS          _UL_(0x00600000)                              /**< \brief UHPHS_OHCI Base Address */
#define UHPHS_EHCI_BASE_ADDRESS          _UL_(0x00700000)                              /**< \brief UHPHS_EHCI Base Address */
#define WDT_BASE_ADDRESS                 _UL_(0xffffff80)                              /**< \brief WDT Base Address */
#define XDMAC_BASE_ADDRESS               _UL_(0xf0008000)                              /**< \brief XDMAC Base Address */
/** @}  end of Peripheral Base Address Definitions */

/** \addtogroup SAM9X60_pio Peripheral Pio Definitions
 *  @{
 */

/* ************************************************************************** */
/*   PIO DEFINITIONS FOR SAM9X60                                              */
/* ************************************************************************** */
#include "pio/sam9x60.h"
/** @}  end of Peripheral Pio Definitions */

/* ************************************************************************** */
/*   MEMORY MAPPING DEFINITIONS FOR SAM9X60                                   */
/* ************************************************************************** */

#define ECC_ROM_SIZE                   _UL_(0x00100000)    /* 1024kB Memory segment type: other */
#define SRAM0_SIZE                     _UL_(0x00100000)    /* 1024kB Memory segment type: other */
#define SRAM1_SIZE                     _UL_(0x00100000)    /* 1024kB Memory segment type: other */
#define UDPHS_RAM_SIZE                 _UL_(0x00100000)    /* 1024kB Memory segment type: other */
#define UHPHS_OHCI_SIZE                _UL_(0x00100000)    /* 1024kB Memory segment type: other */
#define UHPHS_EHCI_SIZE                _UL_(0x00100000)    /* 1024kB Memory segment type: other */
#define EBI_CS0_SIZE                   _UL_(0x10000000)    /* 262144kB Memory segment type: other */
#define EBI_CS1_SIZE                   _UL_(0x10000000)    /* 262144kB Memory segment type: other */
#define EBI_MPDDR_SIZE                 _UL_(0x10000000)    /* 262144kB Memory segment type: other */
#define SDRAM_CS_SIZE                  _UL_(0x10000000)    /* 262144kB Memory segment type: other */
#define EBI_CS2_SIZE                   _UL_(0x10000000)    /* 262144kB Memory segment type: other */
#define EBI_CS3_SIZE                   _UL_(0x10000000)    /* 262144kB Memory segment type: other */
#define EBI_NF_SIZE                    _UL_(0x10000000)    /* 262144kB Memory segment type: other */
#define EBI_CS4_SIZE                   _UL_(0x10000000)    /* 262144kB Memory segment type: other */
#define EBI_CS5_SIZE                   _UL_(0x10000000)    /* 262144kB Memory segment type: other */
#define QSPIMEM_SIZE                   _UL_(0x10000000)    /* 262144kB Memory segment type: other */
#define SDMMC0_SIZE                    _UL_(0x00100000)    /* 1024kB Memory segment type: other */
#define SDMMC1_SIZE                    _UL_(0x00100000)    /* 1024kB Memory segment type: other */
#define OTPC_SIZE                      _UL_(0x00001000)    /*    4kB Memory segment type: other */

#define ECC_ROM_ADDR                   _UL_(0x00100000)    /**< ECC_ROM base address (type: other)*/
#define SRAM0_ADDR                     _UL_(0x00300000)    /**< SRAM0 base address (type: other)*/
#define SRAM1_ADDR                     _UL_(0x00400000)    /**< SRAM1 base address (type: other)*/
#define UDPHS_RAM_ADDR                 _UL_(0x00500000)    /**< UDPHS_RAM base address (type: other)*/
#define UHPHS_OHCI_ADDR                _UL_(0x00600000)    /**< UHPHS_OHCI base address (type: other)*/
#define UHPHS_EHCI_ADDR                _UL_(0x00700000)    /**< UHPHS_EHCI base address (type: other)*/
#define EBI_CS0_ADDR                   _UL_(0x10000000)    /**< EBI_CS0 base address (type: other)*/
#define EBI_CS1_ADDR                   _UL_(0x20000000)    /**< EBI_CS1 base address (type: other)*/
#define EBI_MPDDR_ADDR                 _UL_(0x20000000)    /**< EBI_MPDDR base address (type: other)*/
#define SDRAM_CS_ADDR                  _UL_(0x20000000)    /**< SDRAM_CS base address (type: other)*/
#define EBI_CS2_ADDR                   _UL_(0x30000000)    /**< EBI_CS2 base address (type: other)*/
#define EBI_CS3_ADDR                   _UL_(0x40000000)    /**< EBI_CS3 base address (type: other)*/
#define EBI_NF_ADDR                    _UL_(0x40000000)    /**< EBI_NF base address (type: other)*/
#define EBI_CS4_ADDR                   _UL_(0x50000000)    /**< EBI_CS4 base address (type: other)*/
#define EBI_CS5_ADDR                   _UL_(0x60000000)    /**< EBI_CS5 base address (type: other)*/
#define QSPIMEM_ADDR                   _UL_(0x70000000)    /**< QSPIMEM base address (type: other)*/
#define SDMMC0_ADDR                    _UL_(0x80000000)    /**< SDMMC0 base address (type: other)*/
#define SDMMC1_ADDR                    _UL_(0x90000000)    /**< SDMMC1 base address (type: other)*/
#define OTPC_ADDR                      _UL_(0xeff00000)    /**< OTPC base address (type: other)*/

/* ************************************************************************** */
/**  DEVICE SIGNATURES FOR SAM9X60                                            */
/* ************************************************************************** */
#define CHIP_JTAGID                    _UL_(0X05B4403F)
#define CHIP_CIDR                      _UL_(0X819B81A0)
#define CHIP_EXID                      _UL_(0X00000002)

/* ************************************************************************** */
/**  ELECTRICAL DEFINITIONS FOR SAM9X60                                       */
/* ************************************************************************** */



#ifdef __cplusplus
}
#endif

/** @}  end of SAM9X60 definitions */


#endif /* _SAM9X60_H_ */

