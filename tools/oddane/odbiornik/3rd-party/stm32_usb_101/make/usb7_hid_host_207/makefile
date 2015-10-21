SOURCES = board_init_207.c boot_207.c delay.c fonts.c lcd_mb785_2xx.c \
          lcd_util.c led.c timer.c \
          usb_otg_fifo.c usb_otg_interrupt.c usbh_api.c usbh_configure_207.c \
          usbh_core.c usbh_hid_core.c usbh_hid_req.c usbh_io.c \
          usbh_interrupt.c usbh_lib.c usbh_std_req.c \
          ex_hid_host.c syscalls_dummy.c

TARGETS = usb_hid_host_207.elf usb_hid_host_207.bin

ifndef HARDWARE
  HARDWARE = stm3220g-eval
endif

include ../../make/scripts/makefile.in
