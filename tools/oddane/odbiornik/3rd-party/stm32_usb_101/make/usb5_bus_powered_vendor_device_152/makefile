SOURCES = board_init_152.c boot_15x.c delay.c lcd_dummy.c lcd_util.c led.c timer.c \
          usbd_api_aaa.c usbd_configure_152.c usbd_core.c usbd_interrupt_152.c \
          usbd_interrupt_aaa.c usbd_main.c usbd_power_152.c \
          ex_vendor_bus_pwr_dev.c pwr_periph_15x.c
 
TARGETS = usb_bus_pwr_vendor_dev_152.elf usb_bus_pwr_vendor_dev_152.bin

ifndef HARDWARE 
  HARDWARE = stm32l-discovery
endif

include ../../make/scripts/makefile.in
