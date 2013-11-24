SOURCES = board_init_103.c boot_10x.c delay.c lcd_dummy.c lcd_util.c led.c timer.c \
          usbd_api_aaa.c usbd_configure_103.c usbd_core.c usbd_interrupt_103.c \
          usbd_interrupt_aaa.c usbd_main.c usbd_power_103.c \
          ex_vendor_bus_pwr_dev.c pwr_periph_10x.c
 
TARGETS = usb_bus_pwr_vendor_dev_103.elf usb_bus_pwr_vendor_dev_103.bin

ifndef HARDWARE 
  HARDWARE = gadget
endif

include ../../make/scripts/makefile.in
