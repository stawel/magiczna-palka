#include <errno.h>
#include <fcntl.h>
#include <lcd_util.h>
#include <ff.h>
#include <unistd.h>
#include <string.h>
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

/** File system **/

/* The file size is restricted to less than 2 GiB, because Cortex-M is
   the 32-bit architecture and read, write, lseek from libc return
   a signed value. */

/* MAX_OPEN_FILES does not count stdin, stdout and stderr. */
#define MAX_OPEN_FILES  4

static FIL fd_table[MAX_OPEN_FILES];
static char fd_used[MAX_OPEN_FILES];

int _open(const char *path, unsigned flags, ...) {
  int fd;
  BYTE mode_flags;

  for (fd = 0; fd < MAX_OPEN_FILES; ++fd)
    if (fd_used[fd] == 0)
      break;

  if (fd >= MAX_OPEN_FILES) {
    errno = ENFILE;
    return -1;
  }
  else {
    /* O_RDONLY (0), O_WRONLY (1), O_RDWR (2), FA_READ (1), FA_WRITE (2) */
    mode_flags = (flags + 1) & 3;
    /* O_APPEND (8) is ignored, as Newlib calls _lseek after _open in
       this case. O_BINARY (0x10000) is also ignored. */
    if (flags & O_CREAT) {              /* O_CREAT == 0x0200 */
      if (flags & O_TRUNC) {            /* O_TRUNC == 0x0400 */
        mode_flags |= FA_CREATE_ALWAYS; /* FA_CREATE_ALWAYS == 0x08 */
      }
      else if (flags & O_EXCL) {        /* O_EXCL == 0x0800 */
        mode_flags |= FA_CREATE_NEW;    /* FA_CREATE_NEW == 0x04 */
      }
      else {
        mode_flags |= FA_OPEN_ALWAYS;   /* FA_OPEN_ALWAYS == 0x10 */
      }
    }
    else {
      mode_flags |= FA_OPEN_EXISTING;   /* FA_OPEN_EXISTING == 0x00 */
    }
    memset(&fd_table[fd], 0, sizeof(FIL));
    if (FR_OK == f_open(&fd_table[fd], path, mode_flags)) {
      fd_used[fd] = 1;
      errno = 0;
      return fd + 3; /* stdin, stdout, stderr */
    }
    else {
      errno = EIO; /* Maybe errno should depend on returned value. */
      return -1;
    }
  }
}

int _close(int fd) {
  fd -= 3; /* stdin, stdout, stderr */
  if (fd < 0 || fd >= MAX_OPEN_FILES || fd_used[fd] == 0) {
    errno = EBADF;
    return -1;
  }
  else {
    if (FR_OK == f_close(&fd_table[fd])) {
      fd_used[fd] = 0;
      errno = 0;
      return 0;
    }
    else {
      fd_used[fd] = 0; /* Resurces must be freed even if close fails. */
      errno = EIO;
      return -1;
    }
  }
}

long _read(int fd, char *ptr, long len) {
  if (ptr == 0 || len < 0) {
    errno = EINVAL;
    return -1;
  }
  fd -= 3; /* stdin, stdout, stderr */
  if (fd < 0 || fd >= MAX_OPEN_FILES || fd_used[fd] == 0) {
    errno = EBADF;
    return -1;
  }
  else {
    UINT br;
    /* There is a problem, if len > max_value(UINT). */
    if (FR_OK == f_read(&fd_table[fd], ptr, len, &br)) {
      errno = 0;
      return br;
    }
    else {
      errno = EIO; /* Maybe errno should depend on returned value. */
      return -1;
    }
  }
}

long _write(int fd, char const *ptr, long len) {
  if (ptr == 0 || len < 0) {
    errno = EINVAL;
    return -1;
  }
  else if (fd == 1 || fd == 2) { /* stdout or stderr */
    LCDwriteLenWrap(ptr, len);
    return len;
  }
  fd -= 3; /* stdin, stdout, stderr */
  if (fd < 0 || fd >= MAX_OPEN_FILES || fd_used[fd] == 0) {
    errno = EBADF;
    return -1;
  }
  else {
    UINT bw;
    /* There is a problem, if len > max_value(UINT). */
    if (FR_OK == f_write(&fd_table[fd], ptr, len, &bw)) {
      errno = 0;
      return bw;
    }
    else {
      errno = EIO; /* Maybe errno should depend on returned value. */
      return -1;
    }
  }
}

long _lseek(int fd, long offset, int whence) {
  fd -= 3; /* stdin, stdout, stderr */
  if (fd < 0 || fd >= MAX_OPEN_FILES || fd_used[fd] == 0) {
    errno = EBADF;
    return -1;
  }
  else if (whence == SEEK_CUR) {
    offset += (long)f_tell(&fd_table[fd]);
  }
  else if (whence == SEEK_END) {
    offset += (long)f_size(&fd_table[fd]);
  }
  else if (whence != SEEK_SET) {
    errno = EINVAL;
    return -1;
  }
  if (offset < 0) {
    errno = EINVAL;
    return -1;
  }
  else if (FR_OK == f_lseek(&fd_table[fd], offset)) {
    errno = 0;
    return f_tell(&fd_table[fd]);
  }
  else {
    errno = EIO; /* Maybe errno should depend on returned value. */
    return -1;
  }
}

int _fstat(int fd, struct stat *st) {
  if (st == 0) {
    errno = EINVAL;
    return -1;
  }
  else if (fd >= 0 && fd <= 2) { /* stdin, stdout or stderr */
    memset(st, 0, sizeof(struct stat));
    st->st_mode = S_IFCHR | 0666;
    errno = 0;
    return 0;
  }
  fd -= 3; /* stdin, stdout, stderr */
  if (fd < 0 || fd >= MAX_OPEN_FILES || fd_used[fd] == 0) {
    errno = EBADF;
    return -1;
  }
  else {
    /* TODO: How to return reliable file information?
    memset(st, 0, sizeof(struct stat));
    st->st_mode = S_IFREG | S_IFBLK | 0666;
    st->st_blksize = 512;
    errno = 0;
    return 0; */
    errno = EIO;
    return -1;
  }
}

int _isatty(int fd) {
  return fd >= 0 && fd <= 2; /* stdin, stdout or stderr */
}
