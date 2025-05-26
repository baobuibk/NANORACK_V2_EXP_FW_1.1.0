################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
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
-include Core/MIN_Proto/min_app/subdir.mk
-include Core/MIN_Proto/min/subdir.mk
-include Core/MIN_Proto/subdir.mk
-include Core/LED/subdir.mk
-include Core/Devices/PHOTO_BOARD/ADS8327/subdir.mk
-include Core/Devices/PHOTO_BOARD/subdir.mk
-include Core/Devices/NTC/subdir.mk
-include Core/Devices/MB85RS2MT/subdir.mk
-include Core/Devices/LT8722/subdir.mk
-include Core/Devices/LASER_BOARD/MCP4902/subdir.mk
-include Core/Devices/LASER_BOARD/subdir.mk
-include Core/Devices/IR_LED/subdir.mk
-include Core/Devices/Heater/subdir.mk
-include Core/Devices/DateTime/subdir.mk
-include Core/Devices/BMP390/subdir.mk
-include Core/Devices/Auto_run/subdir.mk
-include Core/Devices/ADG1414/subdir.mk
-include Core/COPC/subdir.mk
-include Core/CMDLine/subdir.mk
-include Core/CLI_Terminal/CLI_Src/subdir.mk
-include Core/CLI_Terminal/CLI_Setup/subdir.mk
-include Core/CLI_Terminal/CLI_Command/subdir.mk
-include Core/Board/subdir.mk
-include BSP/UART/subdir.mk
-include BSP/SysTick/subdir.mk
-include BSP/RingBuffer/subdir.mk
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


BUILD_ARTIFACT_NAME := EXP_V110
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
EXP_V110.elf \

MAP_FILES += \
EXP_V110.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
EXP_V110.list \

OBJCOPY_HEX += \
EXP_V110.hex \

OBJCOPY_BIN += \
EXP_V110.bin \


# All Target
all: main-build

# Main-build Target
main-build: EXP_V110.elf secondary-outputs

# Tool invocations
EXP_V110.elf EXP_V110.map: $(OBJS) $(USER_OBJS) D:\STworkspace\EXP_V110\STM32F407VGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "EXP_V110.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"D:\STworkspace\EXP_V110\STM32F407VGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="EXP_V110.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

EXP_V110.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "EXP_V110.list"
	@echo 'Finished building: $@'
	@echo ' '

EXP_V110.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "EXP_V110.hex"
	@echo 'Finished building: $@'
	@echo ' '

EXP_V110.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "EXP_V110.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) EXP_V110.bin EXP_V110.elf EXP_V110.hex EXP_V110.list EXP_V110.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
