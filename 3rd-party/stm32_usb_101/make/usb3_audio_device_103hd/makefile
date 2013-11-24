SOURCES = board_init_103.c boot_10x.c delay.c lcd_dummy.c lcd_util.c led.c \
          usbd_api_aaa.c usbd_configure_103.c usbd_core.c usbd_interrupt_103.c \
          usbd_interrupt_aaa.c usbd_main.c usbd_power_dummy.c \
          dac_buffer.c dac_12bits_10x.c ex_audio_dev.c syscalls_dummy.c

TARGETS = usb_audio_dev_103hd.elf usb_audio_dev_103hd.bin

ifndef HARDWARE
  HARDWARE = mmstm32f103vx-0-0-0
endif

include ../../make/scripts/makefile.in
