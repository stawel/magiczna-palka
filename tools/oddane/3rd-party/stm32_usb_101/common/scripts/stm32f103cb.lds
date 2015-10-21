/* Skrypt linkera dla STM32F103CB, 128 KiB FLASH, 20 KiB RAM */

MEMORY
{
    FLASH (rx) : ORIGIN = 0x8000000, LENGTH = 128K
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

/* Linker zgłosi błąd, gdy w RAM zostanie za mało miejsca
   na stos i stertę. */
_minimum_stack_and_heap_size = 2560;

INCLUDE cortex-m3.lds
