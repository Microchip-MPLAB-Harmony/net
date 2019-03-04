/*******************************************************************************
  QSPI0 Peripheral Library Source File

  Company
    Microchip Technology Inc.

  File Name
    plib_qspi0.c

  Summary
    QSPI0 peripheral library interface.

  Description

  Remarks:
    
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END

#include "plib_qspi0.h"


void QSPI0_Initialize(void)
{
    // Reset and Disable the qspi Module
    QSPI0_REGS->QSPI_CR = QSPI_CR_SWRST_Msk | QSPI_CR_QSPIDIS_Msk;

    while(QSPI0_REGS->QSPI_SR& QSPI_SR_QSPIENS_Msk);

    // Set Mode Register values
    QSPI0_REGS->QSPI_MR = ( QSPI_MR_SMM_MEMORY );

    // Set serial clock register
    QSPI0_REGS->QSPI_SCR = (QSPI_SCR_SCBR(4))  ;

    // Enable the qspi Module
    QSPI0_REGS->QSPI_CR = QSPI_CR_QSPIEN_Msk;

    while(!(QSPI0_REGS->QSPI_SR& QSPI_SR_QSPIENS_Msk));
}

static void qspi0_memcpy_32bit(uint32_t* dst, uint32_t* src, uint32_t count)
{
    while (count--) {
        *dst++ = *src++;
    }
}

static void qspi0_memcpy_8bit(uint8_t* dst, uint8_t* src, uint32_t count)
{
    while (count--) {
        *dst++ = *src++;
    }
}

static inline void qspi0_end_transfer( void )
{
    QSPI0_REGS->QSPI_CR = QSPI_CR_LASTXFER_Msk;
}

static bool qspi0_setup_transfer( qspi_memory_xfer_t *qspi_memory_xfer, uint8_t tfr_type, uint32_t address )
{
    uint32_t mask = 0;
    volatile uint32_t dummy = 0;

    /* Set instruction address register */
    QSPI0_REGS->QSPI_IAR = QSPI_IAR_ADDR(address);

    /* Set Instruction code register */
    QSPI0_REGS->QSPI_ICR = (QSPI_ICR_INST(qspi_memory_xfer->instruction)) | (QSPI_ICR_OPT(qspi_memory_xfer->option));

    /* Set Instruction Frame register*/

    mask |= qspi_memory_xfer->width;
    mask |= qspi_memory_xfer->addr_len;

    if (qspi_memory_xfer->option_en) {
        mask |= qspi_memory_xfer->option_len;
        mask |= QSPI_IFR_OPTEN_Msk;
    }

    if (qspi_memory_xfer->continuous_read_en)
    {
        mask |= QSPI_IFR_CRM_Msk;
    }

    mask |= QSPI_IFR_NBDUM(qspi_memory_xfer->dummy_cycles);

    mask |= QSPI_IFR_INSTEN_Msk | QSPI_IFR_ADDREN_Msk | QSPI_IFR_DATAEN_Msk;

    mask |= QSPI_IFR_TFRTYP(tfr_type);

    QSPI0_REGS->QSPI_IFR = mask;

    /* To synchronize APB and AHB accesses */
    dummy = QSPI0_REGS->QSPI_IFR;
    (void)dummy;

    return true;
}

bool QSPI0_CommandWrite( qspi_command_xfer_t *qspi_command_xfer, uint32_t address )
{
    uint32_t mask = 0;

    /* Configure address */
    if(qspi_command_xfer->addr_en) {
        QSPI0_REGS->QSPI_IAR = QSPI_IAR_ADDR(address);

        mask |= QSPI_IFR_ADDREN_Msk;
        mask |= qspi_command_xfer->addr_len;
    }

    /* Configure instruction */
    QSPI0_REGS->QSPI_ICR = (QSPI_ICR_INST(qspi_command_xfer->instruction));

    /* Configure instruction frame */
    mask |= qspi_command_xfer->width;
    mask |= QSPI_IFR_INSTEN_Msk;
    mask |= QSPI_IFR_TFRTYP(QSPI_IFR_TFRTYP_TRSFR_READ_Val);

    QSPI0_REGS->QSPI_IFR = mask;

    /* Poll Status register to know status if instruction has end */
    while(!(QSPI0_REGS->QSPI_SR& QSPI_SR_INSTRE_Msk));

    return true;
}

bool QSPI0_RegisterRead( qspi_register_xfer_t *qspi_register_xfer, uint32_t *rx_data, uint8_t rx_data_length )
{
    uint32_t *qspi_buffer = (uint32_t *)QSPI0MEM_ADDR;
    uint32_t mask = 0;
    volatile uint32_t dummy = 0;

    /* Configure Instruction */
    QSPI0_REGS->QSPI_ICR = (QSPI_ICR_INST(qspi_register_xfer->instruction));

    /* Configure Instruction Frame */
    mask |= qspi_register_xfer->width;

    mask |= QSPI_IFR_NBDUM(qspi_register_xfer->dummy_cycles);

    mask |= QSPI_IFR_INSTEN_Msk | QSPI_IFR_DATAEN_Msk;

    mask |= QSPI_IFR_TFRTYP(QSPI_IFR_TFRTYP_TRSFR_READ_Val);

    QSPI0_REGS->QSPI_IFR = mask;

    /* To synchronize APB and AHB accesses */
    dummy = QSPI0_REGS->QSPI_IFR;

    /* Read the register content */
    qspi0_memcpy_8bit((uint8_t *)rx_data , (uint8_t *)qspi_buffer,  rx_data_length);

    __DSB();
    __ISB();

    qspi0_end_transfer();

    /* Poll Status register to know status if instruction has end */
    while(!(QSPI0_REGS->QSPI_SR& QSPI_SR_INSTRE_Msk));

    (void)dummy;
    return true;
}

bool QSPI0_RegisterWrite( qspi_register_xfer_t *qspi_register_xfer, uint32_t *tx_data, uint8_t tx_data_length )
{
    uint32_t *qspi_buffer = (uint32_t *)QSPI0MEM_ADDR;
    uint32_t mask = 0;
    volatile uint32_t dummy = 0;

    /* Configure Instruction */
    QSPI0_REGS->QSPI_ICR = (QSPI_ICR_INST(qspi_register_xfer->instruction));

    /* Configure Instruction Frame */
    mask |= qspi_register_xfer->width;

    mask |= QSPI_IFR_INSTEN_Msk | QSPI_IFR_DATAEN_Msk;

    mask |= QSPI_IFR_TFRTYP(QSPI_IFR_TFRTYP_TRSFR_WRITE_Val);

    QSPI0_REGS->QSPI_IFR = mask;

    /* To synchronize APB and AHB accesses */
    dummy = QSPI0_REGS->QSPI_IFR;

    /* Write the content to register */
    qspi0_memcpy_8bit((uint8_t *)qspi_buffer, (uint8_t *)tx_data, tx_data_length);

    __DSB();
    __ISB();

    qspi0_end_transfer();

    /* Poll Status register to know status if instruction has end */
    while(!(QSPI0_REGS->QSPI_SR& QSPI_SR_INSTRE_Msk));

    (void)dummy;
    return true;
}

// NOTE: This routine contains a WORKAROUND for alignment issue in the qspi code.
// DO NOT OVERWRITE!
bool QSPI0_MemoryRead( qspi_memory_xfer_t *qspi_memory_xfer, uint32_t *rx_data, uint32_t rx_data_length, uint32_t address )
{
    uint32_t *qspi_mem = (uint32_t *)(QSPI0MEM_ADDR | address);
    volatile uint32_t dummy = 0;

    
    if (false == qspi0_setup_transfer(qspi_memory_xfer, QSPI_IFR_TFRTYP_TRSFR_READ_MEMORY_Val, address))
        return false;
    qspi0_memcpy_8bit((uint8_t *)rx_data , (uint8_t *)qspi_mem,  rx_data_length);
    /* Dummy Read to clear QSPI_SR.INSTRE and QSPI_SR.CSR */
    dummy = QSPI0_REGS->QSPI_SR;

    __DSB();
    __ISB();

    qspi0_end_transfer();

    /* Poll Status register to know status if instruction has end */
    while(!(QSPI0_REGS->QSPI_SR& QSPI_SR_INSTRE_Msk));

    (void)dummy;
    return true;
}

bool QSPI0_MemoryWrite( qspi_memory_xfer_t *qspi_memory_xfer, uint32_t *tx_data, uint32_t tx_data_length, uint32_t address )
{
    uint32_t *qspi_mem = (uint32_t *)(QSPI0MEM_ADDR | address);
    uint32_t length_32bit, length_8bit;

    if (false == qspi0_setup_transfer(qspi_memory_xfer, QSPI_IFR_TFRTYP_TRSFR_WRITE_MEMORY_Val, address))
        return false;

    /* Write to serial flash memory */
    length_32bit = tx_data_length / 4;
    length_8bit= tx_data_length & 0x03;

    if(length_32bit)
        qspi0_memcpy_32bit(qspi_mem, tx_data, length_32bit);
    
    tx_data = tx_data + length_32bit;
    qspi_mem = qspi_mem + length_32bit;

    if(length_8bit)
        qspi0_memcpy_8bit((uint8_t *)qspi_mem, (uint8_t *)tx_data, length_8bit);

    __DSB();
    __ISB();

    qspi0_end_transfer();

    /* Poll Status register to know status if instruction has end */
    while(!(QSPI0_REGS->QSPI_SR& QSPI_SR_INSTRE_Msk));

    return true;
}

/*******************************************************************************
 End of File
*/
