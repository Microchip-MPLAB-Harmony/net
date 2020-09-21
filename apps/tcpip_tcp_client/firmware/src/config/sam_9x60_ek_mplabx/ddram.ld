/*------------------------------------------------------------------------------
 *      Linker script for running in internal DDRAM on the SAM9X60
 *----------------------------------------------------------------------------*/

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(entry)
SEARCH_DIR(.)

/* Memory Spaces Definitions */
MEMORY
{
	sram        (WX) : ORIGIN = 0x300000,   LENGTH = 64K  /* sram */
	ram         (LWX!R) : ORIGIN = 0x21000000, LENGTH = 240M  /* ddr */
	ram_nocache  (!RWX) : ORIGIN = 0x20000000, LENGTH = 16M  /* ddr (non-cached) */
	rom         (LRX) : ORIGIN = 0, LENGTH = 0
}

C_STACK_SIZE   = 0x800;
IRQ_STACK_SIZE = 0x60;
FIQ_STACK_SIZE = 0x60;
SVC_STACK_SIZE = 0x800;
ABT_STACK_SIZE = 0x40;
UND_STACK_SIZE = 0x40;
SYS_STACK_SIZE = 0x800;
HEAP_SIZE      = 402400;

/* Section Definitions */
SECTIONS
{
	.text 0x23f00000 :
	{
		. = ALIGN(4);
		PROVIDE(_sfixed = .);
		*(.textEntry)
		*(.text .text.* .gnu.linkonce.t.*)
		*(.rodata .rodata* .gnu.linkonce.r.*)
		*(.ARM.extab* .gnu.linkonce.armextab.*)

		. = ALIGN(4);
		KEEP(*(.init))
		. = ALIGN(4);
		PROVIDE(__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE(__preinit_array_end = .);

		. = ALIGN(4);
		PROVIDE(__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE(__init_array_end = .);

		. = ALIGN(0x4);
		KEEP(*crtbegin.o(.ctors))
		KEEP(*(EXCLUDE_FILE (*crtend.o) .ctors))
		KEEP(*(SORT(.ctors.*)))
		KEEP(*crtend.o(.ctors))

		. = ALIGN(4);
		KEEP(*(.fini))

		. = ALIGN(4);
		PROVIDE(__fini_array_start = .);
		KEEP(*(.fini_array))
		KEEP(*(SORT(.fini_array.*)))
		PROVIDE(__fini_array_end = .);

		KEEP(*crtbegin.o(.dtors))
		KEEP(*(EXCLUDE_FILE (*crtend.o) .dtors))
		KEEP(*(SORT(.dtors.*)))
		KEEP(*crtend.o(.dtors))

	    *(.dinit*)
		. = ALIGN(4);
	} >ram

	.data :
    {
		*(.data .data.*);
		. = ALIGN(4);
		PROVIDE(_efixed = .);            /* End of text section */
    } >ram

	/* .ARM.exidx is sorted, so has to go in its own output section.  */
	PROVIDE_HIDDEN (__exidx_start = .);
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} >ram
	PROVIDE_HIDDEN (__exidx_end = .);

	/* _etext must be just before .relocate section */
	. = ALIGN(4);
	PROVIDE(_etext = .);

	.relocate :
	{
		. = ALIGN(4);
		PROVIDE(_srelocate = .);
		KEEP(*(.vectors .vectors.*))
		*(.ramfunc)
		. = ALIGN(4);
		PROVIDE(_erelocate = .);
	} >sram AT>ram

	.region_cache_aligned_const :
	{
		. = ALIGN(32);
		*(.region_cache_aligned_const)
		. = ALIGN(32);
	} >ram

	.region_sram (NOLOAD) :
	{
		. = ALIGN(4);
		*(.region_sram)
	} >sram

	.region_ram (NOLOAD) :
	{
		. = ALIGN(4);
		*(.region_ram)
	} >ram

	.region_nocache (NOLOAD) :
	{
		. = ALIGN(4);
		*(.region_nocache)
	} >ram_nocache

	/* .bss section which is used for uninitialized data */
	.bss (NOLOAD) :
	{
		. = ALIGN(4);
		PROVIDE(_szero = .);
		*(.bss .bss.*)
		*(COMMON)
		. = ALIGN(4);
		PROVIDE(_ezero = .);
	} >ram

	.region_cache_aligned (NOLOAD) :
	{
		. = ALIGN(32);
		*(.region_cache_aligned)
		. = ALIGN(32);
	} >ram

	.heap (NOLOAD) :
	{
		. = ALIGN(4);
		PROVIDE(__heap_start__ = .);
		. += HEAP_SIZE;
		PROVIDE(__heap_end__ = .);
	} >ram

	.stacks (NOLOAD) :
	{
		. += IRQ_STACK_SIZE;
		. = ALIGN(8);
		_irqstack = .;

		. += FIQ_STACK_SIZE;
		. = ALIGN(8);
		_fiqstack = .;

		. += ABT_STACK_SIZE;
		. = ALIGN(8);
		_abtstack = .;

		. += UND_STACK_SIZE;
		. = ALIGN(8);
		_undstack = .;

		. += SVC_STACK_SIZE;
		. = ALIGN(8);
		_svcstack = .;

		. += SYS_STACK_SIZE;
		. = ALIGN(8);
		_sysstack = .;

		. += C_STACK_SIZE;
		. = ALIGN(8);
		_cstack = .;
	} >ram
}
