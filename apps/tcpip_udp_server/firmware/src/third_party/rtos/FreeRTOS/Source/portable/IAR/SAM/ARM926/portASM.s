;/*
; * FreeRTOS Kernel V10.0.0
; * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
; *
; * Permission is hereby granted, free of charge, to any person obtaining a copy of
; * this software and associated documentation files (the "Software"), to deal in
; * the Software without restriction, including without limitation the rights to
; * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
; * the Software, and to permit persons to whom the Software is furnished to do so,
; * subject to the following conditions:
; *
; * The above copyright notice and this permission notice shall be included in all
; * copies or substantial portions of the Software. If you wish to use our Amazon
; * FreeRTOS name, please do so in a fair use way that does not cause confusion.
; *
; * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
; * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
; * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
; * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
; * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
; *
; * http://www.FreeRTOS.org
; * http://aws.amazon.com/freertos
; *
; * 1 tab == 4 spaces!
; */


	EXTERN	ulPortYieldRequired
	EXTERN	vApplicationIRQHandler

	EXTERN	vTaskSwitchContext
	EXTERN  ulCriticalNesting
	EXTERN	pxCurrentTCB
	EXTERN  ulAsmAPIPriorityMask

	PUBLIC	FreeRTOS_SWI_Handler
	PUBLIC  FreeRTOS_IRQ_Handler
	PUBLIC vPortStartFirstTask



AIC_BASE_ADDRESS  DEFINE		0xFFFFF100
AIC_EOICR         DEFINE		0x038
AIC_SMR           DEFINE		0x004
AIC_IVR           DEFINE		0x010

I_BIT             DEFINE		0x80
F_BIT             DEFINE		0x40

SYS_MODE			EQU		0x1f
SVC_MODE			EQU		0x13
IRQ_MODE			EQU		0x12

	SECTION .text:CODE:ROOT(2)
	ARM

portSAVE_CONTEXT MACRO

	; Push R0 as we are going to use the register.
	STMDB	SP!, {R0}

	; Set R0 to point to the task stack pointer.
	STMDB	SP, {SP}^
	NOP
	SUB		SP, SP, #4
	LDMIA	SP!, {R0}

	; Push the return address onto the stack.
	STMDB	R0!, {LR}

	; Now we have saved LR we can use it instead of R0.
	MOV		LR, R0

	; Pop R0 so we can save it onto the system mode stack.
	LDMIA	SP!, {R0}

	; Push all the system mode registers onto the task stack.
	STMDB	LR, {R0-LR}^
	NOP
	SUB		LR, LR, #60

	; Push the SPSR onto the task stack.
	MRS		R0, SPSR
	STMDB	LR!, {R0}

	LDR		R0, =ulCriticalNesting
	LDR		R0, [R0]
	STMDB	LR!, {R0}

	; Store the new top of stack for the task.
	LDR		R1, =pxCurrentTCB
	LDR		R0, [R1]
	STR		LR, [R0]

	ENDM


; /**********************************************************************/

portRESTORE_CONTEXT MACRO

	; Set the LR to the task stack.
	LDR		R1, =pxCurrentTCB
	LDR		R0, [R1]
	LDR		LR, [R0]

	; The critical nesting depth is the first item on the stack.
	; Load it into the ulCriticalNesting variable.
	LDR		R0, =ulCriticalNesting
	LDMFD	LR!, {R1}
	STR		R1, [R0]

	; Get the SPSR from the stack.
	LDMFD	LR!, {R0}
	MSR		SPSR_cxsf, R0

	; Restore all system mode registers for the task.
	LDMFD	LR, {R0-R14}^
	NOP

	; Restore the return address.
	LDR		LR, [LR, #+60]

	; And return - correcting the offset in the LR to obtain the
	; correct address.
	SUBS	PC, LR, #4

	ENDM

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Starting the first task is just a matter of restoring the context that
; was created by pxPortInitialiseStack().
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
vPortStartFirstTask:
	portRESTORE_CONTEXT


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; SVC handler is used to yield a task.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
FreeRTOS_SWI_Handler:

	ADD		LR, LR, #4			; Add 4 to the LR to make the LR appear exactly
								; as if the context was saved during and IRQ
								; handler.

	portSAVE_CONTEXT			; Save the context of the current task...
	LDR R0, =vTaskSwitchContext	; before selecting the next task to execute.
	mov     lr, pc
	BX R0
	portRESTORE_CONTEXT			; Restore the context of the selected task.



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; IRQ interrupt handler used when individual priorities cannot be masked
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
FreeRTOS_IRQ_Handler:

			portSAVE_CONTEXT

			/* Write in the IVR to support Protect Mode */
			LDR 	lr, =AIC_BASE_ADDRESS
			LDR 	r0, [r14, #AIC_IVR]
			STR 	lr, [r14, #AIC_IVR]
			; Dummy read to force AIC_IVR write completion
			ldr         lr, [r14, #AIC_SMR]

			; Branch to interrupt handler in Supervisor mode

			msr         CPSR_c, #SVC_MODE
			stmfd       sp!, { r1-r3, r4, r12, lr}

			; Check for 8-byte alignment and save lr plus a
			; word to indicate the stack adjustment used (0 or 4)

			and         r1, sp, #4
			sub         sp, sp, r1
			stmfd       sp!, {r1, lr}

			/* Branch to C portion of the interrupt handler */
			MOV 	lr, pc
			BX		r0

			ldmia       sp!, {r1, lr}
			add         sp, sp, r1

			ldmia       sp!, { r1-r3, r4, r12, lr}
			msr         CPSR_c, #IRQ_MODE | I_BIT | F_BIT

			/* Acknowledge interrupt */
			LDR 	lr, =AIC_BASE_ADDRESS
			STR 	lr, [r14, #AIC_EOICR]

			portRESTORE_CONTEXT


	END


