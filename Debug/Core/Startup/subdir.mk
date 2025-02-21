################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
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
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I"D:/STworkspace/EXP_V101_Hieu/Scheduler" -I"D:/STworkspace/EXP_V101_Hieu/BSP" -I"D:/STworkspace/EXP_V101_Hieu/BSP/SysTick" -I"D:/STworkspace/EXP_V101_Hieu/Core/Common" -I"D:/STworkspace/EXP_V101_Hieu/Core/WDog" -I"D:/STworkspace/EXP_V101_Hieu/Core/LED" -I"D:/STworkspace/EXP_V101_Hieu/BSP/UART" -I"D:/STworkspace/EXP_V101_Hieu/Core/CMDLine" -I"D:/STworkspace/EXP_V101_Hieu/Core/Board" -I"D:/STworkspace/EXP_V101_Hieu/Core/Temperature" -I"D:/STworkspace/EXP_V101_Hieu/BSP/Delay" -I"D:/STworkspace/EXP_V101_Hieu/Core/Devices/LT8722" -I"D:/STworkspace/EXP_V101_Hieu/Core/Devices" -I"D:/STworkspace/EXP_V101_Hieu/BSP/I2C" -I"D:/STworkspace/EXP_V101_Hieu/Core/Devices/NTC" -I"D:/STworkspace/EXP_V101_Hieu/Core/Devices/BMP390" -I"D:/STworkspace/EXP_V101_Hieu/Core/Devices/Heater" -I"D:/STworkspace/EXP_V101_Hieu/Core/COPC" -I"D:/STworkspace/EXP_V101_Hieu/ThirdParty/libfsp" -I"D:/STworkspace/EXP_V101_Hieu/ThirdParty" -I"D:/STworkspace/EXP_V101_Hieu/Core/Sensor_I2C" -I"D:/STworkspace/EXP_V101_Hieu/Core/Devices/IR_LED" -I"D:/STworkspace/EXP_V101_Hieu/BSP/I2C_Slave" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f407vgtx.d ./Core/Startup/startup_stm32f407vgtx.o

.PHONY: clean-Core-2f-Startup

