################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := E:/ccstdio10/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./main.obj" \
"./Hardware/src/AT24C01.obj" \
"./Hardware/src/WatchDog.obj" \
"./Hardware/src/adc_dma.obj" \
"./Hardware/src/button.obj" \
"./Hardware/src/epwm_led.obj" \
"./Hardware/src/iic.obj" \
"./Hardware/src/led.obj" \
"./Hardware/src/oled.obj" \
"./Hardware/src/sci.obj" \
"./Hardware/src/spi_o.obj" \
"./Hardware/src/timer.obj" \
"./Hardware/src/w25q64.obj" \
"./Lib/src/DSP2833x_ADC_cal.obj" \
"./Lib/src/DSP2833x_Adc.obj" \
"./Lib/src/DSP2833x_CodeStartBranch.obj" \
"./Lib/src/DSP2833x_CpuTimers.obj" \
"./Lib/src/DSP2833x_DMA.obj" \
"./Lib/src/DSP2833x_DefaultIsr.obj" \
"./Lib/src/DSP2833x_GlobalVariableDefs.obj" \
"./Lib/src/DSP2833x_PieCtrl.obj" \
"./Lib/src/DSP2833x_PieVect.obj" \
"./Lib/src/DSP2833x_SysCtrl.obj" \
"./Lib/src/DSP2833x_Xintf.obj" \
"./Lib/src/DSP2833x_usDelay.obj" \
"../CMD/DSP2833x_Headers_nonBIOS.cmd" \
"../CMD/F28335.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include CMD/subdir_vars.mk
-include Hardware/src/subdir_vars.mk
-include Lib/src/subdir_vars.mk
-include subdir_rules.mk
-include CMD/subdir_rules.mk
-include Hardware/src/subdir_rules.mk
-include Lib/src/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
project.out \

EXE_OUTPUTS__QUOTED += \
"project.out" \

BIN_OUTPUTS += \
project.hex \

BIN_OUTPUTS__QUOTED += \
"project.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "project.out"

# Tool invocations
project.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C2000 Linker'
	"E:/ccstdio10/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi -z -m"project.map" --stack_size=0x300 --warn_sections -i"E:/ccstdio10/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/lib" -i"E:/ccstdio10/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="project_linkInfo.xml" --rom_model -o "project.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

project.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: C2000 Hex Utility'
	"E:/ccstdio10/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/bin/hex2000" --diag_wrap=off -o "project.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "main.obj" "Hardware\src\AT24C01.obj" "Hardware\src\WatchDog.obj" "Hardware\src\adc_dma.obj" "Hardware\src\button.obj" "Hardware\src\epwm_led.obj" "Hardware\src\iic.obj" "Hardware\src\led.obj" "Hardware\src\oled.obj" "Hardware\src\sci.obj" "Hardware\src\spi_o.obj" "Hardware\src\timer.obj" "Hardware\src\w25q64.obj" "Lib\src\DSP2833x_ADC_cal.obj" "Lib\src\DSP2833x_Adc.obj" "Lib\src\DSP2833x_CodeStartBranch.obj" "Lib\src\DSP2833x_CpuTimers.obj" "Lib\src\DSP2833x_DMA.obj" "Lib\src\DSP2833x_DefaultIsr.obj" "Lib\src\DSP2833x_GlobalVariableDefs.obj" "Lib\src\DSP2833x_PieCtrl.obj" "Lib\src\DSP2833x_PieVect.obj" "Lib\src\DSP2833x_SysCtrl.obj" "Lib\src\DSP2833x_Xintf.obj" "Lib\src\DSP2833x_usDelay.obj" 
	-$(RM) "main.d" "Hardware\src\AT24C01.d" "Hardware\src\WatchDog.d" "Hardware\src\adc_dma.d" "Hardware\src\button.d" "Hardware\src\epwm_led.d" "Hardware\src\iic.d" "Hardware\src\led.d" "Hardware\src\oled.d" "Hardware\src\sci.d" "Hardware\src\spi_o.d" "Hardware\src\timer.d" "Hardware\src\w25q64.d" "Lib\src\DSP2833x_Adc.d" "Lib\src\DSP2833x_CpuTimers.d" "Lib\src\DSP2833x_DMA.d" "Lib\src\DSP2833x_DefaultIsr.d" "Lib\src\DSP2833x_GlobalVariableDefs.d" "Lib\src\DSP2833x_PieCtrl.d" "Lib\src\DSP2833x_PieVect.d" "Lib\src\DSP2833x_SysCtrl.d" "Lib\src\DSP2833x_Xintf.d" 
	-$(RM) "Lib\src\DSP2833x_ADC_cal.d" "Lib\src\DSP2833x_CodeStartBranch.d" "Lib\src\DSP2833x_usDelay.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

