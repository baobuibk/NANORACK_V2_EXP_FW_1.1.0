################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32f407vgtx.s 

OBJS += \
./Core/Startup/startup_stm32f407vgtx.o 

S_DEPS += \
./Core/Startup/startup_stm32f407vgtx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I"D:/STworkspace/EXP_V110_TEST_MIN/Scheduler" -I"D:/STworkspace/EXP_V110_TEST_MIN/BSP" -I"D:/STworkspace/EXP_V110_TEST_MIN/BSP/SysTick" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Common" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/WDog" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/LED" -I"D:/STworkspace/EXP_V110_TEST_MIN/BSP/UART" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/CMDLine" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Board" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Temperature" -I"D:/STworkspace/EXP_V110_TEST_MIN/BSP/Delay" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/LT8722" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices" -I"D:/STworkspace/EXP_V110_TEST_MIN/BSP/I2C" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/NTC" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/BMP390" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/Heater" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/COPC" -I"D:/STworkspace/EXP_V110_TEST_MIN/ThirdParty/libfsp" -I"D:/STworkspace/EXP_V110_TEST_MIN/ThirdParty" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Sensor_I2C" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/IR_LED" -I"D:/STworkspace/EXP_V110_TEST_MIN/BSP/I2C_Slave" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/LASER_BOARD" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/LASER_BOARD/MCP4902" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/ADG1414" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/PHOTO_BOARD/ADS8327" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/PHOTO_BOARD" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/Devices/MB85RS2MT" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/CLI_Terminal/CLI_Command" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/CLI_Terminal/CLI_Setup" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/CLI_Terminal/CLI_Src" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/CLI_Terminal" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/MIN_Proto" -I"D:/STworkspace/EXP_V110_TEST_MIN/Core/MIN_Proto/min_app" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f407vgtx.d ./Core/Startup/startup_stm32f407vgtx.o

.PHONY: clean-Core-2f-Startup

