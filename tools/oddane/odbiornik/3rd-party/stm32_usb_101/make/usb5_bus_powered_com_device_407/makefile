SOURCES = board_init_207.c boot_72_fs_0.c delay.c lcd_dummy.c lcd_util.c led.c timer.c \
          usb_otg_fifo.c usb_otg_interrupt.c usbd_api_bbb.c usbd_configure_207.c \
          usbd_core.c usbd_interrupt_bbb.c usbd_main.c usbd_power_x07.c \
          ex_com_bus_pwr_dev.c pwr_periph_2xx.c syscalls_dummy.c

TARGETS = usb_bus_pwr_com_dev_407.elf usb_bus_pwr_com_dev_407.bin

ifndef HARDWARE
  HARDWARE = stm32f4-discovery
endif

include ../../make/scripts/makefile.in
