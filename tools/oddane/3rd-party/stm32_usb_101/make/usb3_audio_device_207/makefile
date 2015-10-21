SOURCES = board_init_207.c boot_207.c delay.c fonts.c i2c.c i2s.c lcd_mb785_2xx.c lcd_util.c led.c \
          usb_otg_fifo.c usb_otg_interrupt.c usbd_api_bbb.c usbd_configure_207.c \
          usbd_core.c usbd_interrupt_bbb.c usbd_main.c usbd_power_dummy.c \
          cs43l22.c cs43l22_322xg.c dac_buffer.c dac_12bits_2xx.c \
          ex_audio_dev.c ioe_stm322xg.c syscalls_dummy.c

TARGETS = usb_audio_dev_207.elf usb_audio_dev_207.bin

ifndef HARDWARE
  HARDWARE = stm3220g-eval
endif

include ../../make/scripts/makefile.in
