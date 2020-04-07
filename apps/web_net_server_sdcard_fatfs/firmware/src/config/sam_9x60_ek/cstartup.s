/*******************************************************************************
Copyright (c) 2019 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION IRQ_STACK:DATA:NOROOT(3)
        SECTION FIQ_STACK:DATA:NOROOT(3)
        SECTION ABT_STACK:DATA:NOROOT(3)
        SECTION UND_STACK:DATA:NOROOT(3)
        SECTION SVC_STACK:DATA:NOROOT(3)
        SECTION CSTACK:DATA:NOROOT(3)

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#define __ASSEMBLY__

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

AIC_BASE_ADDRESS  DEFINE 0xFFFFF100
AIC_SMR         DEFINE 0x04
AIC_IVR         DEFINE 0x10
AIC_EOICR       DEFINE 0x38

MODE_MSK        DEFINE 0x1F  ; Bit mask for mode bits in CPSR

ARM_MODE_ABT    DEFINE 0x17
ARM_MODE_FIQ    DEFINE 0x11
ARM_MODE_IRQ    DEFINE 0x12
ARM_MODE_SVC    DEFINE 0x13
ARM_MODE_UND    DEFINE 0x1B
ARM_MODE_SYS    DEFINE 0x1F

I_BIT           DEFINE 0x80
F_BIT           DEFINE 0x40

ICACHE_BIT      DEFINE 0x1000
DCACHE_BIT      DEFINE 0x04
MMU_BIT         DEFINE 0x01

REMAP_BASE_ADDRESS DEFINE 0xFFFFDE00 + 0x100

//------------------------------------------------------------------------------
//         Startup routine
//------------------------------------------------------------------------------

        SECTION .vectors:CODE:NOROOT(2)

        PUBLIC  _reset_vector
        PUBLIC  __iar_program_start
        PUBWEAK  irqHandler
        PUBLIC  fiqHandler

        EXTERN  undefined_instruction_irq_handler
        EXTERN  prefetch_abort_irq_handler
        EXTERN  data_abort_irq_handler
        EXTERN  software_interrupt_irq_handler
        

        DATA

__iar_init$$done:               ; The vector table is not needed
                                ; until after copy initialization is done

_reset_vector:                  ; Make this a DATA label, so that stack usage
                                ; analysis doesn't consider it an uncalled fun

        ARM

        ldr     pc, reset_addr          ; 0x0 Reset
        ldr     pc, undefined_addr      ; 0x4 Undefined instructions
        ldr     pc, soft_int_addr       ; 0x8 Software interrupt (SWI/SVC)
        ldr     pc, prefetch_abt_addr   ; 0xc Prefetch abort
        ldr     pc, data_abt_addr       ; 0x10 Data abort
        DCD     0                       ; 0x14 RESERVED
        ldr     pc, irq_addr            ; 0x18 IRQ
        ldr     pc, fiq_addr            ; 0x1c FIQ

        DATA

; All default handlers (except reset, irq and fiq) are
; defined as weak symbol definitions.
; If a handler is defined by the application it will take precedence.
reset_addr:        DCD   __iar_program_start
undefined_addr:    DCD   undefined_instruction_irq_handler
soft_int_addr:     DCD   software_interrupt_irq_handler
prefetch_abt_addr: DCD   prefetch_abort_irq_handler
data_abt_addr:     DCD   data_abort_irq_handler
irq_addr:          DCD   irqHandler
fiq_addr:          DCD   fiqHandler

;------------------------------------------------------------------------------
; Handles a fast interrupt request by branching to the address defined in the
; AIC.
;------------------------------------------------------------------------------
        SECTION .text:CODE:NOROOT(2)
        ARM
fiqHandler:
        sub         lr, lr, #4
        stmfd       sp!, {lr}
        stmfd       sp!, {r0}

        ; Write in the IVR to support Protect Mode

        ldr         lr, =AIC_BASE_ADDRESS
        ldr         r0, [r14, #AIC_IVR]
        str         lr, [r14, #AIC_IVR]
        ; Dummy read to force AIC_IVR write completion
        ldr         lr, [r14, #AIC_SMR]

        ; Branch to interrupt handler in Supervisor mode

        msr         CPSR_c, #ARM_MODE_SVC
        stmfd       sp!, { r1-r3, r4, r12, lr}

        blx          r0

        ldmia       sp!, { r1-r3, r4, r12, lr}
        msr         CPSR_c, #ARM_MODE_FIQ | I_BIT | F_BIT

        ; Acknowledge interrupt

        ldr         lr, =AIC_BASE_ADDRESS
        str         lr, [r14, #AIC_EOICR]

        ; Restore interrupt context and branch back to calling code
        ldmia       sp!, {r0}
        ldmia       sp!, {pc}^

;------------------------------------------------------------------------------
; Handles incoming interrupt requests by branching to the corresponding
; handler, as defined in the AIC. Supports interrupt nesting.
;------------------------------------------------------------------------------
        SECTION .text:CODE:NOROOT(2)
        ARM
irqHandler:
        ; Save interrupt context on the stack to allow nesting

        sub         lr, lr, #4
        stmfd       sp!, {lr}
        mrs         lr, SPSR
        stmfd       sp!, {r0, lr}

        ; Write in the IVR to support Protect Mode

        ldr         lr, =AIC_BASE_ADDRESS
        ldr         r0, [r14, #AIC_IVR]
        str         lr, [r14, #AIC_IVR]
        ; Dummy read to force AIC_IVR write completion
        ldr         lr, [r14, #AIC_SMR]

        ; Branch to interrupt handler in Supervisor mode

        msr         CPSR_c, #ARM_MODE_SVC
        stmfd       sp!, { r1-r3, r4, r12, lr}

        ; Check for 8-byte alignment and save lr plus a
        ; word to indicate the stack adjustment used (0 or 4)

        and         r1, sp, #4
        sub         sp, sp, r1
        stmfd       sp!, {r1, lr}

        blx         r0

        ldmia       sp!, {r1, lr}
        add         sp, sp, r1

        ldmia       sp!, { r1-r3, r4, r12, lr}
        msr         CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT

        ; Acknowledge interrupt

        ldr         lr, =AIC_BASE_ADDRESS
        str         lr, [r14, #AIC_EOICR]

        ; Restore interrupt context and branch back to calling code

        ldmia       sp!, {r0, lr}
        msr         SPSR_cxsf, lr
        ldmia       sp!, {pc}^

//------------------------------------------------------------------------------
/// Initializes the chip and branches to the main() function.
//------------------------------------------------------------------------------

        SECTION .cstartup:CODE:NOROOT(2)
        PUBLIC  __iar_program_start
        EXTERN  main
        REQUIRE _reset_vector

        EXTWEAK __iar_data_init3
        EXTWEAK __iar_init_core
        EXTWEAK __iar_init_vfp
        EXTWEAK __iar_argc_argv

        ARM

        /* Dummy vector table for ROM-code for cases when the real vector table
         * is relocated (QSPI-XIP) */
        ldr     pc, =__iar_program_start
        ldr     pc, =__iar_program_start
        ldr     pc, =__iar_program_start
        ldr     pc, =__iar_program_start
        ldr     pc, =__iar_program_start
        DCD     0
        ldr     pc, =__iar_program_start
        ldr     pc, =__iar_program_start

__iar_program_start:
?cstartup:

        ; Set up the fast interrupt stack pointer

        mrs     r0, CPSR
        bic     r0, r0, #MODE_MSK
        orr     r0, r0, #ARM_MODE_FIQ
        msr     cpsr_c, r0
        ldr     sp, =SFE(FIQ_STACK)
        bic     sp, sp, #0x7

        ; Set up the normal interrupt stack pointer

        bic     r0, r0, #MODE_MSK
        orr     r0, r0, #ARM_MODE_IRQ
        msr     CPSR_c, r0
        ldr     sp, =SFE(IRQ_STACK)
        bic     sp, sp, #0x7

        ; Set up the abort mode stack pointer

        bic     r0, r0, #MODE_MSK
        orr     r0, r0, #ARM_MODE_ABT
        msr     CPSR_c, r0
        ldr     sp, =SFE(ABT_STACK)
        bic     sp, sp, #0x7

        ; Set up the undefined mode stack pointer

        bic     r0, r0, #MODE_MSK
        orr     r0, r0, #ARM_MODE_UND
        msr     CPSR_c, r0
        ldr     sp, =SFE(UND_STACK)
        bic     sp, sp, #0x7

        ; Set up the usr/sys mode stack pointer

        bic     r0, r0, #MODE_MSK
        orr     r0, r0, #ARM_MODE_SYS
        msr     CPSR_c, r0
        ldr     sp, =SFE(CSTACK)
        bic     sp, sp, #0x7

        ; Set up the supervisor mode stack pointer 
        bic     r0 ,r0, #MODE_MSK
        orr     r0 ,r0, #ARM_MODE_SVC
        msr     cpsr_c, r0
        ldr     sp, =SFE(SVC_STACK)
        bic     sp, sp, #0x7

        ; Stay in SVC mode for the remainder of the program execution

        ; Execute relocations & zero BSS

        FUNCALL __iar_program_start, __iar_data_init3
        bl      __iar_data_init3

        ; Remap 0x0 to SRAM
        ldr     r0, =REMAP_BASE_ADDRESS
        ldr     r1, =0x7fff
        str     r1, [r0]
        mov     r0, #0

        ; Turn on core features assumed to be enabled

        FUNCALL __iar_program_start, __iar_init_core
        bl      __iar_init_core

        ; Setup command line

        mov     r0, #0
        FUNCALL __iar_program_start, __iar_argc_argv
        bl      __iar_argc_argv

        ; Call main()

        FUNCALL __iar_program_start, main
        bl      main

       ;; Loop indefinitely when program is finished
loop4:  b       loop4

        END
