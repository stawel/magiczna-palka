/* Skrypt linkera dla STM32L152RB, 128 KiB FLASH, 16 KiB RAM */

MEMORY
{
    FLASH (rx) : ORIGIN = 0x8000000, LENGTH = 128K
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 16K
}

/* Linker zgłosi błąd, gdy w RAM zostanie za mało miejsca
   na stos i stertę. */
_minimum_stack_and_heap_size = 2048;

INCLUDE cortex-m3.lds
