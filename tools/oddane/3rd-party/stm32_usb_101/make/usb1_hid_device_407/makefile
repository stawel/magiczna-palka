SOURCES = board_init_207.c boot_72_fs_0.c delay.c lcd_dummy.c lcd_util.c led.c \
          usb_otg_fifo.c usb_otg_interrupt.c usbd_api_bbb.c usbd_configure_207.c \
          usbd_core.c usbd_interrupt_bbb.c usbd_main.c usbd_power_dummy.c \
          ex_hid_dev.c joystick_mems_4xx.c lis302dl_disco_4xx.c

TARGETS = usb_hid_dev_407.elf usb_hid_dev_407.bin

ifndef HARDWARE
  HARDWARE = stm32f4-discovery
endif

include ../../make/scripts/makefile.in
