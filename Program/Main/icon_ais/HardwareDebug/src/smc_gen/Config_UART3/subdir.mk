################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../src/smc_gen/Config_UART3/Config_UART3.c 

COMPILER_OBJS += \
src/smc_gen/Config_UART3/Config_UART3.obj 

# Each subdirectory must supply rules for building sources it contributes
src/smc_gen/Config_UART3/%.obj: ../src/smc_gen/Config_UART3/%.c 
	@echo 'Building file: $<'
	ccrl -subcommand="src\smc_gen\Config_UART3\cSubCommand.tmp" -msg_lang=english -o "$(@:%.d=%.obj)" "$<"


