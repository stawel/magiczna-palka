/* Skrypt linkera dla STM32F103VC, 256 KiB FLASH, 48 KiB RAM */

MEMORY
{
    FLASH (rx) : ORIGIN = 0x8000000, LENGTH = 256K
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 48K
}

/* Linker zgłosi błąd, gdy w RAM zostanie za mało miejsca
   na stos i stertę. */
_minimum_stack_and_heap_size = 6144;

INCLUDE cortex-m3.lds
