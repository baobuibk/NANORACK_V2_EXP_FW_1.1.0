################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/main.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/main.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/main.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F407xx -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -c -I../Core/Inc -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/DateTime" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/Auto_run" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/STworkspace/EXP_V110_TEST_MIN/Scheduler" -I"D:/STworkspace/EXP_V110_TEST_MIN/BSP" -I"D:/STworkspace/EXP_V110_TEST_MIN/BSP/SysTick" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Common" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/WDog" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/LED" -I"D:/STworkspace/EXP_V110_TEST_MIN/BSP/UART" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/CMDLine" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Board" -I"D:/STworkspace/EXP_V110_TEST_MIN/BSP/Delay" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/LT8722" -I"D:/STworkspace/EXP_V110_TEST_MIN/BSP/I2C" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/NTC" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Temperature" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/BMP390" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/Heater" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/COPC" -I"D:/STworkspace/EXP_V110_TEST_MIN/ThirdParty/libfsp" -I"D:/STworkspace/EXP_V110_TEST_MIN/ThirdParty" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Sensor_I2C" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/IR_LED" -I"D:/STworkspace/EXP_V110_TEST_MIN/BSP/I2C_Slave" -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/LASER_BOARD" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/LASER_BOARD/MCP4902" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/ADG1414" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/PHOTO_BOARD/ADS8327" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/PHOTO_BOARD" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/MB85RS2MT" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/CLI_Terminal" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/CLI_Terminal/CLI_Command" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/CLI_Terminal/CLI_Setup" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/CLI_Terminal/CLI_Src" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/MIN_Proto" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/MIN_Proto/min_app" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

