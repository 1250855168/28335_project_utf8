################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Lib/src/%.obj: ../Lib/src/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"E:/ccstdio10/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Study/28335_study/project" --include_path="E:/Study/28335_study/project/Hardware/include" --include_path="E:/Study/28335_study/project/Lib/include" --include_path="E:/ccstdio10/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="Lib/src/$(basename $(<F)).d_raw" --obj_directory="Lib/src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

Lib/src/%.obj: ../Lib/src/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"E:/ccstdio10/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Study/28335_study/project" --include_path="E:/Study/28335_study/project/Hardware/include" --include_path="E:/Study/28335_study/project/Lib/include" --include_path="E:/ccstdio10/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="Lib/src/$(basename $(<F)).d_raw" --obj_directory="Lib/src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


