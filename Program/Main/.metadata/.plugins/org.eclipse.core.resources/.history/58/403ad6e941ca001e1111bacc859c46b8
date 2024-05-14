################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
ASM_SRCS += \
../src/smc_gen/r_bsp/mcu/all/cstart.asm \
../src/smc_gen/r_bsp/mcu/all/r_bsp_common_ccrl.asm 

C_SRCS += \
../src/smc_gen/r_bsp/mcu/all/r_bsp_common.c 

ASSEMBLER_OBJS += \
src/smc_gen/r_bsp/mcu/all/cstart.obj \
src/smc_gen/r_bsp/mcu/all/r_bsp_common_ccrl.obj 

COMPILER_OBJS += \
src/smc_gen/r_bsp/mcu/all/r_bsp_common.obj 

# Each subdirectory must supply rules for building sources it contributes
src/smc_gen/r_bsp/mcu/all/%.obj: ../src/smc_gen/r_bsp/mcu/all/%.asm 
	@echo 'Building file: $<'
	ccrl -subcommand="src\smc_gen\r_bsp\mcu\all\asmSubCommand.tmp" -msg_lang=english -o "$(@:%.d=%.obj)" "$<"


src/smc_gen/r_bsp/mcu/all/%.obj: ../src/smc_gen/r_bsp/mcu/all/%.c 
	@echo 'Building file: $<'
	ccrl -subcommand="src\smc_gen\r_bsp\mcu\all\cSubCommand.tmp" -msg_lang=english -o "$(@:%.d=%.obj)" "$<"


