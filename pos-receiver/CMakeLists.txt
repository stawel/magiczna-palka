


set(source com_dev.c)


#set(TOOLCHAIN arm-elf-)
set(TOOLCHAIN arm-none-eabi-)

SET(CMAKE_C_COMPILER ${TOOLCHAIN}-gcc)
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN}-g++)
