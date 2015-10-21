#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

/** Heap memory management **/

/* Function header is in unistd.h. */
caddr_t _sbrk(int incr) {
  register char *stack_ptr asm("sp"); /* Just stack pointer register name */
  extern char end; /* First free memory location, defined in linker script */
  static char *heap_end;
  char *prev_heap_end;

  if (heap_end == NULL)
    heap_end = &end;

  if (heap_end + incr > stack_ptr) {
    /* Some of the libstdc++-v3 tests rely upon detecting
       out of memory errors, so do not abort here.  */
    errno = ENOMEM;
    return (caddr_t)-1;
  }

  prev_heap_end = heap_end;
  heap_end += incr;

  return (caddr_t)prev_heap_end;
}
