SOURCES = board_init_152.c boot_15x.c delay.c lcd_dummy.c lcd_util.c led.c \
          usbd_api_aaa.c usbd_configure_152.c usbd_core.c usbd_interrupt_152.c \
          usbd_interrupt_aaa.c usbd_main.c usbd_power_dummy.c \
          ex_vendor_dev.c
 
TARGETS = usb_vendor_dev_152.elf usb_vendor_dev_152.bin

ifndef HARDWARE 
  HARDWARE = stm32l-discovery
endif

include ../../make/scripts/makefile.in
