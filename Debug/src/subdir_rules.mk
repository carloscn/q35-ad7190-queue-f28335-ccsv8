################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
src/%.obj: ../src/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.3.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="/opt/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.3.LTS/include" --include_path="/home/delvis/workspace/ccsv8/Q35_AD7190_DRIVER_F2833X_CCS8/DSP2833x_common/include" --include_path="/home/delvis/workspace/ccsv8/Q35_AD7190_DRIVER_F2833X_CCS8/DSP2833x_headers/include" --include_path="/home/delvis/workspace/ccsv8/Q35_AD7190_DRIVER_F2833X_CCS8/inc" --advice:performance=all -g --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="src/$(basename $(<F)).d_raw" --obj_directory="src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


