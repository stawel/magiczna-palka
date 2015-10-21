#include <ff.h>
#include <file_system.h>
#include <usb_disk.h>

int FileSystemMount() {
  static FATFS fatfs;

  if (f_mount(USB_DRIVE_NUMEBR, &fatfs) != FR_OK)
    return -1;

  /* If f_mount fails, disk must not be initialized. If disk is not
     initialized here, it will be initialized by the first file
     access, but we want to report an error before an attempt to open
     a file. */
  return USBdiskInitialize();
}

int FileSystemUmount() {
  FRESULT fr;
  int re;

  /* It is safe to try both operation, even if one of them fails. */
  fr = f_mount(USB_DRIVE_NUMEBR, 0);
  re = USBdiskAllowEject();

  return fr == FR_OK && re == 0 ? 0 : -1;
}
