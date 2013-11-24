/* Skrypt linkera dla STM32F051R8, 64 KiB FLASH, 8 KiB RAM */

MEMORY
{
    FLASH (rx) : ORIGIN = 0x8000000, LENGTH = 64K
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 8K
}

/* Linker zgłosi błąd, gdy w RAM zostanie za mało miejsca
   na stos i stertę. */
_minimum_stack_and_heap_size = 1024;

/* Skryptu cortex-m3.lds można użyć dla dowolnej wersji architektury.
   Nazwę zachowano dla kompatybilności z projektem STM32_ETH. */
INCLUDE cortex-m3.lds
