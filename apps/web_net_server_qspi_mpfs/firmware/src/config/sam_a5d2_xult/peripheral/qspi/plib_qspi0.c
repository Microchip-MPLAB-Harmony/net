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
#include "string.h" // memmove


void QSPI0_Initialize(void)
{
    // Reset and Disable the qspi Module
    QSPI0_REGS->QSPI_CR = QSPI_CR_SWRST_Msk | QSPI_CR_QSPIDIS_Msk;

    while(QSPI0_REGS->QSPI_SR& QSPI_SR_QSPIENS_Msk){
        ;   // spin lock
    }

    /* DLYCS  = 0x0 */
    /* DLYBCT = 0x0 */
    /* NBBITS = 0x8 */
    /* CSMODE = 0x0 */
    /* WDRBT  = 0 */
    /* LLB    = 0 */
    /* SMM    = MEMORY */
    QSPI0_REGS->QSPI_MR = ( QSPI_MR_SMM_MEMORY | QSPI_MR_NBBITS(0x8));

    /* CPOL = 0 */
    /* CPHA = 0 */
    /* SCBR = 4 */
    /* DLYBS = 0 */
    QSPI0_REGS->QSPI_SCR = (QSPI_SCR_SCBR(4))  ;

    // Enable the qspi Module
    QSPI0_REGS->QSPI_CR = QSPI_CR_QSPIEN_Msk;

    while(!(QSPI0_REGS->QSPI_SR& QSPI_SR_QSPIENS_Msk)){
        ;   // spin lock
    }
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

static bool qspi0_setup_transfer( qspi_memory_xfer_t *qspi_memory_xfer, QSPI_TRANSFER_TYPE tfr_type, uint32_t address )
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

    if (qspi_memory_xfer->continuous_read_en) {
        mask |= QSPI_IFR_CRM_Msk;
    }

    mask |= QSPI_IFR_NBDUM(qspi_memory_xfer->dummy_cycles);

    mask |= QSPI_IFR_INSTEN_Msk | QSPI_IFR_ADDREN_Msk | QSPI_IFR_DATAEN_Msk;

    switch (tfr_type){
        case QSPI_REG_READ:
            mask |= QSPI_IFR_TFRTYP(QSPI_IFR_TFRTYP_TRSFR_READ_Val);
            break;
        case QSPI_REG_WRITE:
            mask |= QSPI_IFR_TFRTYP(QSPI_IFR_TFRTYP_TRSFR_WRITE_Val);
            break;
        case QSPI_MEM_READ:
            mask |= QSPI_IFR_TFRTYP(QSPI_IFR_TFRTYP_TRSFR_READ_MEMORY_Val);
            break;
        case QSPI_MEM_WRITE:
            mask |= QSPI_IFR_TFRTYP(QSPI_IFR_TFRTYP_TRSFR_WRITE_MEMORY_Val);
            break;
    };

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
    while(!(QSPI0_REGS->QSPI_SR& QSPI_SR_INSTRE_Msk)) {
        ;   // spin lock
    }

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
    while(!(QSPI0_REGS->QSPI_SR& QSPI_SR_INSTRE_Msk)) {
        ;   // spin lock
    }

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
    while(!(QSPI0_REGS->QSPI_SR& QSPI_SR_INSTRE_Msk)) {
        ;   // spin lock
    }

    (void)dummy;
    return true;
}

bool
QSPI0_MemoryRead(
    qspi_memory_xfer_t *    qspi_memory_xfer,
    uint32_t *              rx_data,
    uint32_t                rx_data_length,
    uint32_t                address
    )
{
    /*  This implements a transfer from device source to a destination memory buffer 
        without an intermediate local buffer.  The algorithm is used for efficiency 
        sake, allowing a maximum number of word transfers from the device; but, 
        not requiring word alignment at either end of source, or destination,
        locations.
        The non-boundary aligned header and trailer element sizes are used to 
        characterizes the number of word transfers for the body.  The body is 
        transfered in a word wide fashion followed by an appropriate shift of the
        bytes in the destination buffer.
        Single byte accesses at the head, or tail, are done as necessary.  The 
        coding attempts to minimize unnecessary byte manipulations.
    */
    uint8_t *   qspi_mem  = (uint8_t *) (QSPI0MEM_ADDR | address);
    uint8_t *   pRxBuffer = (uint8_t *) rx_data;
    uint32_t    numDstPreWordBytes;
    uint32_t    numSrcPreWordBytes;
    uint32_t    numDstPostWordBytes = 0;
    uint32_t    numSrcPostWordBytes = 0;
    uint32_t    numWordTransferBytes = 0;
    int32_t     shiftBytes = 0;             // gt(0)=>right, lt(0)=>left shift
    uint8_t     tmpBuffer[ sizeof( uint32_t) ];

    ///// device preliminaries
    if( false == qspi0_setup_transfer( qspi_memory_xfer, 
                            QSPI_MEM_READ, 
                            address 
                        ) ) {
        return false;
    }

    ///// dst and src buffer characterization
    numDstPreWordBytes =  0x03 & (uint32_t) pRxBuffer;
    if( numDstPreWordBytes ) {
        numDstPreWordBytes =  sizeof( uint32_t ) - numDstPreWordBytes;
    }
    if( rx_data_length >= numDstPreWordBytes ) {
        numDstPostWordBytes = 0x03 & (uint32_t) (pRxBuffer + rx_data_length);
    }
    //
    numSrcPreWordBytes =  0x03 & (uint32_t) qspi_mem;
    if( numSrcPreWordBytes ) {
        numSrcPreWordBytes =  sizeof( uint32_t ) - numSrcPreWordBytes;
    }
    if( rx_data_length >= numSrcPreWordBytes ) {
        numSrcPostWordBytes = 0x03 & (uint32_t) (qspi_mem + rx_data_length);
    }
    else {
        numSrcPreWordBytes = rx_data_length;
    }

    if( rx_data_length > (numSrcPreWordBytes + numSrcPostWordBytes) ) {
        // number of word size transfers is equal to length of buffer
        // minus single byte transfers
        numWordTransferBytes = rx_data_length - (numSrcPreWordBytes + numSrcPostWordBytes);
    }

    // test if src has same number of word alignments as the dst buffer
    if( (numDstPreWordBytes + numDstPostWordBytes) != (numSrcPreWordBytes + numSrcPostWordBytes)) {
        // a word size space will be needed for shifting purposes; the
        // common case optimization for equal word alignments cannot be used
        if( numWordTransferBytes >= sizeof( uint32_t ) ) {
            numWordTransferBytes -= sizeof( uint32_t );
            numSrcPostWordBytes += sizeof( uint32_t );
        }
        else {
            numWordTransferBytes = 0;
        }
    }

    shiftBytes = numSrcPreWordBytes - numDstPreWordBytes;

    ///// Transfer of data from src device to dst buffer
    // Perform single byte transfers necessary before word alignment begins
    if( numSrcPreWordBytes ) {
        // get these now so we don't have to backup the device access later
        qspi0_memcpy_8bit( tmpBuffer, qspi_mem, numSrcPreWordBytes );
        qspi_mem += numSrcPreWordBytes;     // word alignment point for the src
    }
    pRxBuffer += numDstPreWordBytes;        // word alignment point for the dst

    // Perform word aligned transfers
    if( numWordTransferBytes / 4 ) {
        qspi0_memcpy_32bit( (uint32_t *) pRxBuffer, 
                (uint32_t *) qspi_mem, 
                numWordTransferBytes / 4 
            );
        qspi_mem += numWordTransferBytes;
        pRxBuffer += numWordTransferBytes;
    }

    if( 0 >= shiftBytes ) {                 // left, or no, shift
        if( shiftBytes ) {
            // Shift the data left to its final destination buffer location
            memmove( ((uint8_t *) rx_data) + numDstPreWordBytes + shiftBytes,
                    ((uint8_t *) rx_data) + numDstPreWordBytes,
                    numWordTransferBytes 
                );
            pRxBuffer += shiftBytes;        // adjust end point
        }
        // Now we have room at the end, perform the single byte transfers 
        // necessary after word alignment ends
        if( numSrcPostWordBytes ) {
            qspi0_memcpy_8bit( pRxBuffer, qspi_mem, numSrcPostWordBytes );
        }
    }
    else {                                  // right shift
        // Perform the single byte transfers necessary after word alignment ends
        if( numSrcPostWordBytes ) {
            qspi0_memcpy_8bit( pRxBuffer, qspi_mem, numSrcPostWordBytes );
        }
        // Shift the data to right to its final destination buffer location
        memmove( ((uint8_t *) rx_data) + numDstPreWordBytes + shiftBytes, 
                ((uint8_t *) rx_data) + numDstPreWordBytes, 
                numWordTransferBytes + numSrcPostWordBytes 
            );
    }

    if( numSrcPreWordBytes ) {
        // Now we have room at the beginning; 
        // place the previously saved pre-word aligned bytes
        memmove( rx_data, tmpBuffer, numSrcPreWordBytes );
    }
    ///// Clean up
    /* Dummy Read to clear QSPI_SR.INSTRE and QSPI_SR.CSR */
    (uint32_t) QSPI0_REGS->QSPI_SR;
    __DSB();
    __ISB();

    qspi0_end_transfer();
    // Poll Status register to know status if instruction has ended
    while( !(QSPI0_REGS->QSPI_SR & QSPI_SR_INSTRE_Msk) ) {
        ;   // spin lock
    }
  
    return true;
}

bool QSPI0_MemoryWrite( qspi_memory_xfer_t *qspi_memory_xfer, uint32_t *tx_data, uint32_t tx_data_length, uint32_t address )
{
    uint32_t *qspi_mem = (uint32_t *)(QSPI0MEM_ADDR | address);
    uint32_t length_32bit, length_8bit;

    if (false == qspi0_setup_transfer(qspi_memory_xfer, QSPI_MEM_WRITE, address))
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
    while(!(QSPI0_REGS->QSPI_SR& QSPI_SR_INSTRE_Msk)) {
        ;   // spin lock
    }

    return true;
}

/*******************************************************************************
 End of File
*/
