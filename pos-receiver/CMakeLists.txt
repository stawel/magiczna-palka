



set(COMMON_DIR ${CMAKE_SOURCE_DIR}/3rd-party/stm32_usb_101/common)
set(CSRC ${COMMON_DIR}/src)
set(EXSRC ${CMAKE_SOURCE_DIR}/3rd-party/stm32_usb_101/exusb/src)





set(SOURCE
    com_dev.c
#    usbd_main.c
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
    ${CSRC}/usbd_main.c
    ${CSRC}/usbd_power_dummy.c
    ${CSRC}/syscalls_dummy.c
    ${EXSRC}/pwr_periph_2xx.c
)

aux_source_directory(${CMAKE_SOURCE_DIR}/3rd-party/stm32_usb_101/libraries/stm32/STM32F4xx_StdPeriph_Driver/src STDLIBC)


include_directories(${CMAKE_SOURCE_DIR}/3rd-party/stm32_usb_101/exusb/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/stm32_usb_101/common/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/stm32_usb_101/exusb/include/stm32f4-discovery)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/stm32_usb_101/libraries/stm32/STM32F4xx_StdPeriph_Driver/inc)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/stm32_usb_101/libraries/stm32/CMSIS/Device/ST/STM32F4xx/Include)
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/stm32_usb_101/libraries/stm32/CMSIS/Include)
#include_directories(${CMAKE_SOURCE_DIR}/3rd-party/stm32_usb_101/others/include/stm32f4-discovery)

#set(TOOLCHAIN arm-elf-)
set(TOOLCHAIN arm-none-eabi-)

SET(CMAKE_C_COMPILER ${TOOLCHAIN}gcc)
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN}g++)

add_definitions(-DUSE_STDPERIPH_DRIVER)
add_definitions(-DSTM32F4XX)
add_definitions(-DHSE_VALUE=8000000)

SET(CFLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -O2  -Wall -g")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS} -ffunction-sections -fdata-sections")
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS}")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "${CFLAGS} -nostartfiles -L${COMMON_DIR}/scripts -Tstm32f407vg.lds -Wl,--gc-sections")




add_executable(position-receiver ${SOURCE} ${STDLIBC})

