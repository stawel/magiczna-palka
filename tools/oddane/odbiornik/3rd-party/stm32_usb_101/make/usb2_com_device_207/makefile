SOURCES = board_init_207.c boot_207.c delay.c fonts.c lcd_mb785_2xx.c lcd_util.c led.c \
          usb_otg_fifo.c usb_otg_interrupt.c usbd_api_bbb.c usbd_configure_207.c \
          usbd_core.c usbd_interrupt_bbb.c usbd_main.c usbd_power_dummy.c \
          ex_com_dev.c pwr_periph_2xx.c syscalls_dummy.c

TARGETS = usb_com_dev_207.elf usb_com_dev_207.bin

ifndef HARDWARE
  HARDWARE = stm3220g-eval
endif

include ../../make/scripts/makefile.in
