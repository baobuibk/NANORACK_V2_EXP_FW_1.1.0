################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include ThirdParty/libfsp/subdir.mk
-include Scheduler/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/WDog/subdir.mk
-include Core/Temperature/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include Core/Sensor_I2C/subdir.mk
-include Core/LED/subdir.mk
-include Core/Devices/NTC/subdir.mk
-include Core/Devices/LT8722/subdir.mk
-include Core/Devices/IR_LED/subdir.mk
-include Core/Devices/Heater/subdir.mk
-include Core/Devices/DateTime/subdir.mk
-include Core/Devices/DAC/subdir.mk
-include Core/Devices/BMP390/subdir.mk
-include Core/Devices/Auto_run/subdir.mk
-include Core/COPC/subdir.mk
-include Core/CMDLine/subdir.mk
-include BSP/UART/subdir.mk
-include BSP/SysTick/subdir.mk
-include BSP/I2C_Slave/subdir.mk
-include BSP/I2C/subdir.mk
-include BSP/Delay/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := EXP_V101_Hieu
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
EXP_V101_Hieu.elf \

MAP_FILES += \
EXP_V101_Hieu.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
EXP_V101_Hieu.list \

OBJCOPY_HEX += \
EXP_V101_Hieu.hex \

OBJCOPY_BIN += \
EXP_V101_Hieu.bin \


# All Target
all: main-build

# Main-build Target
main-build: EXP_V101_Hieu.elf secondary-outputs

# Tool invocations
EXP_V101_Hieu.elf EXP_V101_Hieu.map: $(OBJS) $(USER_OBJS) D:\STworkspace\EXP_V101_Hieu\STM32F407VGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "EXP_V101_Hieu.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"D:\STworkspace\EXP_V101_Hieu\STM32F407VGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="EXP_V101_Hieu.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

EXP_V101_Hieu.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "EXP_V101_Hieu.list"
	@echo 'Finished building: $@'
	@echo ' '

EXP_V101_Hieu.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "EXP_V101_Hieu.hex"
	@echo 'Finished building: $@'
	@echo ' '

EXP_V101_Hieu.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "EXP_V101_Hieu.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) EXP_V101_Hieu.bin EXP_V101_Hieu.elf EXP_V101_Hieu.hex EXP_V101_Hieu.list EXP_V101_Hieu.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
