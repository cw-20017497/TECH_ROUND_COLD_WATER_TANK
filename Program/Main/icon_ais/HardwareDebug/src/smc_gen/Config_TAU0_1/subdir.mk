################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../src/smc_gen/Config_TAU0_1/Config_TAU0_1.c 

COMPILER_OBJS += \
src/smc_gen/Config_TAU0_1/Config_TAU0_1.obj 

# Each subdirectory must supply rules for building sources it contributes
src/smc_gen/Config_TAU0_1/%.obj: ../src/smc_gen/Config_TAU0_1/%.c 
	@echo 'Building file: $<'
	ccrl -subcommand="src\smc_gen\Config_TAU0_1\cSubCommand.tmp" -msg_lang=english -o "$(@:%.d=%.obj)" "$<"

