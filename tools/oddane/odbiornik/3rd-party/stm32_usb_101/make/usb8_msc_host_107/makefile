SOURCES = board_init_107.c boot_72_fs_0.c delay.c font5x8.c lcd_ks0108_10x.c \
          lcd_util.c led.c syscalls_fs.c timer.c \
          usb_otg_fifo.c usb_otg_interrupt.c usbh_api.c usbh_configure_107.c \
          usbh_core.c usbh_io.c usbh_interrupt.c usbh_lib.c usbh_msc_core.c \
          usbh_msc_req.c usbh_std_req.c \
          ex_msc_host.c diskio.c fftime.c file_system.c usb_disk.c libff.a

TARGETS = usb_msc_host_107.elf usb_msc_host_107.bin

ifndef HARDWARE
  HARDWARE = zl29arm
endif

include ../../make/scripts/makefile.in
