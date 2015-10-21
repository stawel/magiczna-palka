SOURCES = board_init_107.c boot_72_fs_0.c delay.c font5x8.c lcd_ks0108_10x.c \
          lcd_util.c led.c timer.c \
          usb_otg_fifo.c usb_otg_interrupt.c usbh_api.c usbh_configure_107.c \
          usbh_core.c usbh_hid_core.c usbh_hid_req.c usbh_io.c \
	  usbh_interrupt.c usbh_lib.c usbh_std_req.c \
          ex_hid_host.c syscalls_dummy.c

TARGETS = usb_hid_host_107.elf usb_hid_host_107.bin

ifndef HARDWARE
  HARDWARE = zl29arm
endif

include ../../make/scripts/makefile.in
