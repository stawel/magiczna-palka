cmake_minimum_required(VERSION 2.6)

Project(mp3d-receiver)

set(PECZARSKI_DIR ${CMAKE_SOURCE_DIR}/3rd-party/stm32_usb_101)
set(COMMON_DIR ${PECZARSKI_DIR}/common)
set(EXUSB_DIR ${PECZARSKI_DIR}/exusb)
set(CSRC ${COMMON_DIR}/src)
set(EXSRC ${EXUSB_DIR}/src)

set(SOURCE
    adc.c
    com_dev.c
    usbd_main.c
    ${CSRC}/board_init_207.c
    ${CSRC}/boot_72_fs_0.c
    ${CSRC}/delay.c
    ${CSRC}/lcd_dummy.c
    ${CSRC}/lcd_util.c
    ${CSRC}/led.c
    ${CSRC}/usb_otg_fifo.c
    ${CSRC}/usb_otg_interrupt.c
    ${CSRC}/usbd_api_bbb.c
    ${CSRC}/usbd_configure_207.c
    ${CSRC}/usbd_core.c
    ${CSRC}/usbd_interrupt_bbb.c
#    ${CSRC}/usbd_main.c
    ${CSRC}/usbd_power_dummy.c
    ${CSRC}/syscalls_dummy.c
    ${EXSRC}/pwr_periph_2xx.c
    ${CSRC}/startup_stm32.c
)

aux_source_directory(${PECZARSKI_DIR}/libraries/stm32/STM32F4xx_StdPeriph_Driver/src STDLIBC)


include_directories(${EXUSB_DIR}/include)
include_directories(${COMMON_DIR}/include)
include_directories(${CSRC})
include_directories(${EXUSB_DIR}/include/stm32f4-discovery)
include_directories(${PECZARSKI_DIR}/libraries/stm32/STM32F4xx_StdPeriph_Driver/inc)
include_directories(${PECZARSKI_DIR}/libraries/stm32/CMSIS/Device/ST/STM32F4xx/Include)
include_directories(${PECZARSKI_DIR}/libraries/stm32/CMSIS/Include)
#include_directories(${PECZARSKI_DIR}/others/include/stm32f4-discovery)

#set(TOOLCHAIN arm-elf)
set(TOOLCHAIN arm-none-eabi)

include_directories(/usr/${TOOLCHAIN}/include)

SET(CMAKE_C_COMPILER ${TOOLCHAIN}-gcc)
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN}-g++)

add_definitions(-DUSE_STDPERIPH_DRIVER)
add_definitions(-DSTM32F4XX)
add_definitions(-DHSE_VALUE=8000000)

SET(CFLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -O2  -Wall -g")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS} -ffunction-sections -fdata-sections")
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS}")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "${CFLAGS} -nostartfiles -L${COMMON_DIR}/scripts -Tstm32f407vg.lds -Wl,--gc-sections")




add_executable(position-receiver ${SOURCE} ${STDLIBC})

