/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2012        */
/*-----------------------------------------------------------------------*/
/* Adapted to STM32 USB project, (C) Marcin Peczarski, 2012              */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include <diskio.h> /* FatFs lower layer API */
#include <usb_disk.h> /* USB drive control */

DSTATUS disk_initialize(BYTE drv) {
  if (drv == USB_DRIVE_NUMEBR && USBdiskInitialize() == 0)
    return 0;
  else
    return STA_NOINIT | STA_NODISK;
}

DSTATUS disk_status(BYTE drv) {
  if (drv == USB_DRIVE_NUMEBR && USBdiskStatus() == 0)
    return 0;
  else
    return STA_NOINIT | STA_NODISK;
}

/* drv    -- physical drive number (0...)
   buff   -- data buffer to store read data
   sector -- sector address (LBA)
   count  -- number of sectors to read (1...128) */
DRESULT disk_read(BYTE drv, BYTE *buff, DWORD sector, BYTE count) {
  if (drv == USB_DRIVE_NUMEBR) {
    if (USBdiskRead(buff, sector, count) == 0)
      return RES_OK;
    else
      return RES_ERROR;
  }
  return RES_PARERR;
}

#if _USE_WRITE
/* drv    -- physical drive number (0...)
   buff   -- data to be written
   sector -- sector address (LBA)
   count  -- number of sectors to write (1...128) */
DRESULT disk_write(BYTE drv, BYTE const *buff,
                   DWORD sector, BYTE count) {
  if (drv == USB_DRIVE_NUMEBR) {
    if (USBdiskWrite(buff, sector, count) == 0)
      return RES_OK;
    else
      return RES_ERROR;
  }
  return RES_PARERR;
}
#endif

#if _USE_IOCTL
/* drv  -- physical drive number (0...)
   ctrl -- control code
   buff -- buffer to send or receive control data */
DRESULT disk_ioctl(BYTE drv, BYTE ctrl, void *buff) {
  if (drv == USB_DRIVE_NUMEBR) {
    if (ctrl == CTRL_SYNC) { /* buff is always 0 */
      if (USBdiskSync() == 0)
        return RES_OK;
      else
        return RES_ERROR;
    }
    else if (ctrl == GET_SECTOR_SIZE) { /* buff points to WORD */
      if (USBdiskGetSectorSize(buff) == 0)
        return RES_OK;
      else
        return RES_ERROR;
    }
  }
  return RES_PARERR;
}
#endif
