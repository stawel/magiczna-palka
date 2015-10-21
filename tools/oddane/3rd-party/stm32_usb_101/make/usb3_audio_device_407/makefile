SOURCES = board_init_207.c boot_72_fs_0.c delay.c i2c.c i2s.c lcd_dummy.c lcd_util.c led.c \
          usb_otg_fifo.c usb_otg_interrupt.c usbd_api_bbb.c usbd_configure_207.c \
          usbd_core.c usbd_interrupt_bbb.c usbd_main.c usbd_power_dummy.c \
          cs43l22.c cs43l22_disco_4xx.c dac_buffer.c dac_12bits_4xx.c \
          ex_audio_dev.c syscalls_dummy.c

TARGETS = usb_audio_dev_407.elf usb_audio_dev_407.bin

ifndef HARDWARE
  HARDWARE = stm32f4-discovery
endif

include ../../make/scripts/makefile.in
