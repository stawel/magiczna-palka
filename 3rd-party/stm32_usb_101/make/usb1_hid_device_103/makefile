SOURCES = board_init_103.c boot_10x.c delay.c lcd_dummy.c lcd_util.c led.c \
          usbd_api_aaa.c usbd_configure_103.c usbd_core.c usbd_interrupt_103.c \
          usbd_interrupt_aaa.c usbd_main.c usbd_power_dummy.c \
          ex_hid_dev.c joystick_gpio_10x.c

TARGETS = usb_hid_dev_103.elf usb_hid_dev_103.bin

ifndef HARDWARE
  HARDWARE = zl30arm
endif

include ../../make/scripts/makefile.in
