################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/CLI_Terminal/CLI_Src/embedded_cli.c 

OBJS += \
./Core/CLI_Terminal/CLI_Src/embedded_cli.o 

C_DEPS += \
./Core/CLI_Terminal/CLI_Src/embedded_cli.d 


# Each subdirectory must supply rules for building sources it contributes
Core/CLI_Terminal/CLI_Src/%.o Core/CLI_Terminal/CLI_Src/%.su Core/CLI_Terminal/CLI_Src/%.cyclo: ../Core/CLI_Terminal/CLI_Src/%.c Core/CLI_Terminal/CLI_Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F407xx -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -c -I../Core/Inc -I"D:/STworkspace/EXP_V110/Core/Devices/DateTime" -I"D:/STworkspace/EXP_V110/Core/Devices/Auto_run" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/STworkspace/EXP_V110/Scheduler" -I"D:/STworkspace/EXP_V110/BSP" -I"D:/STworkspace/EXP_V110/BSP/SysTick" -I"D:/STworkspace/EXP_V110/Core/Common" -I"D:/STworkspace/EXP_V110/Core/WDog" -I"D:/STworkspace/EXP_V110/Core/LED" -I"D:/STworkspace/EXP_V110/BSP/UART" -I"D:/STworkspace/EXP_V110/Core/CMDLine" -I"D:/STworkspace/EXP_V110/Core/Board" -I"D:/STworkspace/EXP_V110/BSP/Delay" -I"D:/STworkspace/EXP_V110/Core/Devices" -I"D:/STworkspace/EXP_V110/Core/Devices/LT8722" -I"D:/STworkspace/EXP_V110/BSP/I2C" -I"D:/STworkspace/EXP_V110/Core/Devices/NTC" -I"D:/STworkspace/EXP_V110/Core/Temperature" -I"D:/STworkspace/EXP_V110/Core/Devices/BMP390" -I"D:/STworkspace/EXP_V110/Core/Devices/Heater" -I"D:/STworkspace/EXP_V110/Core/COPC" -I"D:/STworkspace/EXP_V110/ThirdParty/libfsp" -I"D:/STworkspace/EXP_V110/ThirdParty" -I"D:/STworkspace/EXP_V110/Core/Sensor_I2C" -I"D:/STworkspace/EXP_V110/Core/Devices/IR_LED" -I"D:/STworkspace/EXP_V110/BSP/I2C_Slave" -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"D:/STworkspace/EXP_V110/Core/Devices/LASER_BOARD" -I"D:/STworkspace/EXP_V110/Core/Devices/LASER_BOARD/MCP4902" -I"D:/STworkspace/EXP_V110/Core/Devices/ADG1414" -I"D:/STworkspace/EXP_V110/Core/Devices/PHOTO_BOARD/ADS8327" -I"D:/STworkspace/EXP_V110/Core/Devices/PHOTO_BOARD" -I"D:/STworkspace/EXP_V110/Core/Devices/MB85RS2MT" -I"D:/STworkspace/EXP_V110/Core/CLI_Terminal" -I"D:/STworkspace/EXP_V110/Core/CLI_Terminal/CLI_Command" -I"D:/STworkspace/EXP_V110/Core/CLI_Terminal/CLI_Setup" -I"D:/STworkspace/EXP_V110/Core/CLI_Terminal/CLI_Src" -I"D:/STworkspace/EXP_V110/Utilities" -I"D:/STworkspace/EXP_V110/Utilities/Define" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-CLI_Terminal-2f-CLI_Src

clean-Core-2f-CLI_Terminal-2f-CLI_Src:
	-$(RM) ./Core/CLI_Terminal/CLI_Src/embedded_cli.cyclo ./Core/CLI_Terminal/CLI_Src/embedded_cli.d ./Core/CLI_Terminal/CLI_Src/embedded_cli.o ./Core/CLI_Terminal/CLI_Src/embedded_cli.su

.PHONY: clean-Core-2f-CLI_Terminal-2f-CLI_Src

