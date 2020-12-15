/*******************************************************************************
  SYS CLK Static Functions for Clock System Service

  Company:
    Microchip Technology Inc.

  File Name:
    plib_clk.c

  Summary:
    SYS CLK static function implementations for the Clock System Service.

  Description:
    The Clock System Service provides a simple interface to manage the
    oscillators on Microchip microcontrollers. This file defines the static
    implementation for the Clock System Service.

  Remarks:
    Static functions incorporate all system clock configuration settings as
    determined by the user via the Microchip Harmony Configurator GUI.
    It provides static version of the routines, eliminating the need for an
    object ID or object handle.

    Static single-open interfaces also eliminate the need for the open handle.

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "device.h"
#include "plib_clk.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void CLK_Initialize( void )

  Summary:
    Initializes hardware and internal data structure of the System Clock.

  Description:
    This function initializes the hardware and internal data structure of System
    Clock Service.

  Remarks:
    This is configuration values for the static version of the Clock System
    Service module is determined by the user via the MHC GUI.

    The objective is to eliminate the user's need to be knowledgeable in the
    function of the 'configuration bits' to configure the system oscillators.
*/

void CLK_Initialize( void )
{
    bool int_flag = false;

    int_flag = (bool)__builtin_disable_interrupts();

    /* unlock system for clock configuration */
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;

    if (int_flag)
    {
        __builtin_mtc0(12, 0,(__builtin_mfc0(12, 0) | 0x0001)); /* enable interrupts */
    }

    OSCCONbits.FRCDIV = 0;

    /* SPLLBSWSEL   = 5   */
    /* SPLLPWDN     = PLL_ON     */
    /* SPLLPOSTDIV1 = 4 */
    /* SPLLFLOCK    = NO_ASSERT    */
    /* SPLLRST      = NO_ASSERT      */
    /* SPLLFBDIV    = 20  */
    /* SPLLREFDIV   = 1   */
    /* SPLLICLK     = POSC     */
    /* SPLL_BYP     = NO_BYPASS     */
    SPLLCON = 0x414045;

    /* Power down the UPLL */
    UPLLCONbits.UPLLPWDN = 1;

    /* Configure EWPLL */
    /* EWPLLBSWSEL   = 6 */
    /* EWPLLPWDN     = PLL_ON */
    /* EWPLLPOSTDIV1 = 32 */
    /* EWPLLFLOCK    = NO_FORCE */
    /* EWPLLRST      = NORESET_EWPLL */
    /* EWPLLFBDIV    = 800 */
    /* EWPLLREFDIV   = 20 */
    /* EWPLLICLK     = POSC */
    /* ETHCLKOUTEN   = ENABLED */
    /* EWPLL_BYP     = NO_BYPASS */
    EWPLLCON = 0x15320206;

    /* Power down the BTPLL */
    BTPLLCONbits.BTPLLPWDN = 1;

    /* ETHPLLPOSTDIV2 = a */
    /* SPLLPOSTDIV2   = 0 */
    /* BTPLLPOSTDIV2  = 0 */
    CFGCON3 = -182;

    /* OSWEN    = SWITCH_COMPLETE    */
    /* SOSCEN   = OFF   */
    /* UFRCEN   = USBCLK   */
    /* CF       = NO_FAILDET       */
    /* SLPEN    = IDLE    */
    /* CLKLOCK  = UNLOCKED  */
    /* NOSC     = SPLL     */
    /* WAKE2SPD = SELECTED_CLK */
    /* DRMEN    = NO_EFFECT    */
    /* FRCDIV   = OSC_FRC_DIV_1   */
    OSCCON = 0x100;

    OSCCONSET = _OSCCON_OSWEN_MASK;  /* request oscillator switch to occur */

    Nop();
    Nop();

    while( OSCCONbits.OSWEN );        /* wait for indication of successful clock change before proceeding */

    /* Peripheral Bus 3 is by default enabled, set its divisor */
    /* PBDIV = 4 */
    PB3DIVbits.PBDIV = 3;


  

    /* Peripheral Module Disable Configuration */
    CFGCON0bits.PMDLOCK = 0;

    PMD1 = 0x25818981;
    PMD2 = 0x7f0f0f;
    PMD3 = 0x19031305;

    CFGCON0bits.PMDLOCK = 1;

    /* Lock system since done with clock configuration */
    int_flag = (bool)__builtin_disable_interrupts();

    SYSKEY = 0x33333333;

    if (int_flag) /* if interrupts originally were enabled, re-enable them */
    {
        __builtin_mtc0(12, 0,(__builtin_mfc0(12, 0) | 0x0001));
    }
}
