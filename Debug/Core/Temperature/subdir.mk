################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Temperature/temperature.c 

OBJS += \
./Core/Temperature/temperature.o 

C_DEPS += \
./Core/Temperature/temperature.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Temperature/%.o Core/Temperature/%.su Core/Temperature/%.cyclo: ../Core/Temperature/%.c Core/Temperature/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F407xx -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -c -I../Core/Inc -I"D:/STworkspace/EXP_V101_Hieu/Core/Devices/DateTime" -I"D:/STworkspace/EXP_V101_Hieu/Core/Devices/Auto_run" -I"D:/STworkspace/EXP_V101_Hieu/Core/Devices/DAC" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/STworkspace/EXP_V101_Hieu/Scheduler" -I"D:/STworkspace/EXP_V101_Hieu/BSP" -I"D:/STworkspace/EXP_V101_Hieu/BSP/SysTick" -I"D:/STworkspace/EXP_V101_Hieu/Core/Common" -I"D:/STworkspace/EXP_V101_Hieu/Core/WDog" -I"D:/STworkspace/EXP_V101_Hieu/Core/LED" -I"D:/STworkspace/EXP_V101_Hieu/BSP/UART" -I"D:/STworkspace/EXP_V101_Hieu/Core/CMDLine" -I"D:/STworkspace/EXP_V101_Hieu/Core/Board" -I"D:/STworkspace/EXP_V101_Hieu/BSP/Delay" -I"D:/STworkspace/EXP_V101_Hieu/Core/Devices" -I"D:/STworkspace/EXP_V101_Hieu/Core/Devices/LT8722" -I"D:/STworkspace/EXP_V101_Hieu/BSP/I2C" -I"D:/STworkspace/EXP_V101_Hieu/Core/Devices/NTC" -I"D:/STworkspace/EXP_V101_Hieu/Core/Temperature" -I"D:/STworkspace/EXP_V101_Hieu/Core/Devices/BMP390" -I"D:/STworkspace/EXP_V101_Hieu/Core/Devices/Heater" -I"D:/STworkspace/EXP_V101_Hieu/Core/COPC" -I"D:/STworkspace/EXP_V101_Hieu/ThirdParty/libfsp" -I"D:/STworkspace/EXP_V101_Hieu/ThirdParty" -I"D:/STworkspace/EXP_V101_Hieu/Core/Sensor_I2C" -I"D:/STworkspace/EXP_V101_Hieu/Core/Devices/IR_LED" -I"D:/STworkspace/EXP_V101_Hieu/BSP/I2C_Slave" -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Temperature

clean-Core-2f-Temperature:
	-$(RM) ./Core/Temperature/temperature.cyclo ./Core/Temperature/temperature.d ./Core/Temperature/temperature.o ./Core/Temperature/temperature.su

.PHONY: clean-Core-2f-Temperature

