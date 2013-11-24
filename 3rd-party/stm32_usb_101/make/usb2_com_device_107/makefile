SOURCES = board_init_107.c boot_10x.c delay.c font5x8.c lcd_ks0108_10x.c lcd_util.c led.c \
          usb_otg_fifo.c usb_otg_interrupt.c usbd_api_bbb.c usbd_configure_107.c \
          usbd_core.c usbd_interrupt_bbb.c usbd_main.c usbd_power_dummy.c \
          ex_com_dev.c pwr_periph_10x.c syscalls_dummy.c

TARGETS = usb_com_dev_107.elf usb_com_dev_107.bin

ifndef HARDWARE
  HARDWARE = zl29arm
endif

include ../../make/scripts/makefile.in
