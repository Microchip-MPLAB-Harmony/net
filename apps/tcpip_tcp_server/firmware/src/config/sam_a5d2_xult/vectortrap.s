;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default exception handler.
;; Catches any unhandled exceptions.
;;
;; Copyright 2009-2017 IAR Systems AB.
;;
;; $Revision: 112610 $
;;

        MODULE  ?vectortrap



        PUBWEAK undefined_instruction_irq_handler
        PUBWEAK software_interrupt_irq_handler
        PUBWEAK prefetch_abort_irq_handler
        PUBWEAK data_abort_irq_handler

        SECTION .text:CODE:NOROOT:REORDER(2)
        ARM

        CALL_GRAPH_ROOT __default_handler, "interrupt"
        NOCALL __default_handler

__default_handler:
undefined_instruction_irq_handler:
prefetch_abort_irq_handler:
data_abort_irq_handler:
software_interrupt_irq_handler:
        B .


        END
