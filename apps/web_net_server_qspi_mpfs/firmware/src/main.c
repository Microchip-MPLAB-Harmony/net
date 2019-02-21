/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
#ifdef __ICCARM__
#define FELIPE_TEST_HOOKS 1
#endif

#ifndef FELIPE_TEST_HOOKS
int main ( void )
{
    volatile bool workToDo = true;

    /* Initialize all modules */
    SYS_Initialize ( NULL );
    while( workToDo ){
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}
#else
    #define PB_USER             PIO_PIN_PB9
    #define WAIT_AT_MAIN_ENTRY  false
    #define WAIT_AT_START       true
    #define USE_SDMMC           false

    void    firstPbWait(        void );
    void    pauseIfNecessary(   void );
    void    cacheStatement(     void );

    /////
    void
    PIO_OverRide( void )
    {
        static const uint32_t PERIPH_FUNC_NONE =    0;
        static const uint32_t PERIPH_FUNC_A =       1;
    //  static const uint32_t PERIPH_FUNC_B =       2;
    //  static const uint32_t PERIPH_FUNC_C =       3;
    //  static const uint32_t PERIPH_FUNC_D =       4;
    #if USE_SDMMC
        static const uint32_t PERIPH_FUNC_E =       5;
    #endif
        static const uint32_t PERIPH_FUNC_F =       6;

        static const uint32_t PIO_OUTPUT =          0x0100;
        static const uint32_t PIO_PUEN =            0x0200;
        static const uint32_t PIO_PDEN =            0x0400;

    #if USE_SDMMC
        // SDMMC1 =======================
        // DAT0         PA18
        // DAT1         PA19
        // DAT2         PA20
        // DAT3         PA21
        // CK           PA22
        // CMD          PA28
        // WP           PA29                    // hard wired to ground on the board
        // CD           PA30
        //                                      // 0111 0000 0111 1100 0000 0000 0000 0000 = 0x707C 0000
        PIOA_REGS->PIO_MSKR = 0x007C0000;
        PIOA_REGS->PIO_CFGR = PERIPH_FUNC_E | PIO_PUEN;

    #else  // USE_QSPIO
        // QSPI0 ======================
        // CS           PA23
        //                                      // 0000 0000 1000 0000 0000 0000 0000 0000 = 0x0080 0000
        PIOA_REGS->PIO_MSKR = 0x00800000;
        PIOA_REGS->PIO_CFGR = PERIPH_FUNC_F | PIO_PUEN;

        // QSPI0 ======================
        // SCK          PA22
        // IO0          PA24
        // IO1          PA25
        // IO2          PA26
        // IO3          PA27
        //                                      // 0000 1111 0100 0000 0000 0000 0000 0000 = 0x0F40 0000
        PIOA_REGS->PIO_MSKR = 0x0F400000;
        PIOA_REGS->PIO_CFGR = PERIPH_FUNC_F | PIO_PDEN;
    #endif
        // LEDs =========================
        // LED_BLUE     PB0
        // LED_GREEN    PB5
        // LED_RED      PB6
        //                                      // 0000 0001 0000 0000 0000 0000 0110 0001 = 0x0000 0061
        PIOB_REGS->PIO_MSKR = 0x00000061;
        PIOB_REGS->PIO_CFGR = PERIPH_FUNC_NONE | PIO_OUTPUT | PIO_PUEN;

        // PUSH BUTTON ==================
        // USER_PB      PB9
        //                                      // 0000 0001 0000 0000 0000 0010 0000 0000 = 0x0000 0200
        PIOB_REGS->PIO_MSKR = 0x00000200;
        PIOB_REGS->PIO_CFGR = PERIPH_FUNC_NONE | PIO_PUEN;

        // GMAC =========================
        // Inputs ------
        // GRXDV        PB16
        // GRXER        PB17
        // GRX0         PB18
        // GRX1         PB19
        //                                      // 0000 0000 0000 1111 0000 0000 0000 0000 = 0x000F 0000
        PIOB_REGS->PIO_MSKR = 0x000F0000;
        PIOB_REGS->PIO_CFGR = PERIPH_FUNC_F | PIO_PDEN;

        // Outputs -----
        // GTXEN        PB15
        // GTX0         PB20
        // GTX1         PB21
        // GMDC         PB22
        //                                      // 0000 0000 0111 0000 1000 0000 0000 0000 = 0x0070 8000
        PIOB_REGS->PIO_MSKR = 0x00708000;
        PIOB_REGS->PIO_CFGR = PERIPH_FUNC_F | PIO_OUTPUT | PIO_PDEN;

        // I/O ---------
        // GTXCK        PB14
        // GMDIO        PB23
        //                                      // 0000 0000 1000 0000 0100 0000 0000 0000 = 0x0080 4000
        PIOB_REGS->PIO_MSKR = 0x00804000;
        PIOB_REGS->PIO_CFGR = PERIPH_FUNC_F;

        // UART1 ========================
        // UART_RX      PD2
        // UART_TX      PD3
        //                                      // 0000 0000 1000 0000 0000 0000 0000 1100 = 0x0000 000C
        PIOD_REGS->PIO_MSKR = 0x0000000C;
        PIOD_REGS->PIO_CFGR = PERIPH_FUNC_A | PIO_PUEN;

        // DEBUGGER =====================
        // JTAG TCK     PD14
        // JTAG TDI     PD15
        // JTAG TDO     PD16
        // JTAG TMS     PD17
        //                                      // 0000 0000 0000 0011 1100 0000 0000 0000 = 0x0003 C000
        PIOD_REGS->PIO_MSKR = 0x0003C000;
        PIOD_REGS->PIO_CFGR = PERIPH_FUNC_A;

        // SPI1 =========================
        // SPCK         PD25
        // MOSI         PD26
        // MISO         PD27
        //                                      // 0000 1110 0000 0000 0000 0000 0000 0000 = 0x0700 0000
        PIOD_REGS->PIO_MSKR = 0x07000000;
        PIOD_REGS->PIO_CFGR = PERIPH_FUNC_A | PIO_PDEN;

        // SPI1 =========================
        // NPCS0        PD28
        // NPCS1        PD29
        //                                      // 0011 0000 0000 0000 0000 0000 0000 0000 = 0x3000 0000
        PIOD_REGS->PIO_MSKR = 0x30000000;
        PIOD_REGS->PIO_CFGR = PERIPH_FUNC_A | PIO_PUEN;
    }

    /////
    bool firstPbValue;
    void
    firstPbWait( void )
    {
        static bool bFirstTime = true;
        if( bFirstTime ) {
            bFirstTime = false;
            firstPbValue = PIO_PinRead( PB_USER );
    #if WAIT_AT_START
            SYS_CONSOLE_PRINT( "Waiting for PB_USER to continue...\r\n" );
            while( PIO_PinRead( PB_USER ) == firstPbValue )
                ;       // buttonStateA
            while( PIO_PinRead( PB_USER ) != firstPbValue )
                ;       // buttonStateB
            // buttonStateA
    #endif        
        }
    }

    void 
    pauseIfNecessary( void )
    {
        bool workPause = false;

        firstPbWait();
        do
        {
            // buttonStateA
            if( PIO_PinRead( PB_USER ) != firstPbValue )
            {
                // buttonStateB
                while( PIO_PinRead( PB_USER ) != firstPbValue )
                    ;       // buttonStateB
                // buttonStateA
                workPause = !workPause;
                if( workPause )
                    SYS_CONSOLE_PRINT( "Pause...\r\n" );
                else
                    SYS_CONSOLE_PRINT( "Run...\r\n" );
            }
        } while( workPause );
    }

    void
    cacheStatement( void )
    {
        char cacheStatement[ 80 ];
        char * pp = cacheStatement;

        pp += sprintf( cacheStatement, "\r\nEnabled Caches: " );
    #if !L1_ICACHE_IN_USE && !L1_DCACHE_IN_USE && !L2_DCACHE_IN_USE
        pp += sprintf( pp, "NONE" );
    #endif
    #if L1_ICACHE_IN_USE
        pp += sprintf( pp, "%s", "L1 ICache " );
    #endif
    #if L1_DCACHE_IN_USE
        pp += sprintf( pp, "%s", "L1 DCache " );
    #endif
    #if L2_DCACHE_IN_USE
        pp += sprintf( pp, "%s", "L2 DCache " );
    #endif
        SYS_CONSOLE_PRINT( "%s\r\n", cacheStatement );
    }

    ///// ----------------------------------------------------------------------
    int main ( void )
    {
    #if WAIT_AT_MAIN_ENTRY  
        volatile bool workToDo = false;
        while( !workToDo )
          ;
    #else
        volatile bool workToDo = true;
    #endif
        /* Initialize all modules */
        SYS_Initialize ( NULL );
        SYS_CONSOLE_PRINT( "SYS_Initialize complete\r\n" );
        cacheStatement();
        while( workToDo ){
            pauseIfNecessary();
            /* Maintain state machines of all polled MPLAB Harmony modules. */
            SYS_Tasks ( );
        }

        /* Execution should not come here during normal operation */

        return ( EXIT_FAILURE );
    }
#endif


/*******************************************************************************
 End of File
*/

