################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../src/prj/sensor/level.c \
../src/prj/sensor/temp.c \
../src/prj/sensor/temp_table_ambient.c \
../src/prj/sensor/temp_table_cold.c \
../src/prj/sensor/temp_table_cold_bottom.c \
../src/prj/sensor/temp_table_cold_lattron.c \
../src/prj/sensor/temp_table_common.c \
../src/prj/sensor/temp_table_eva.c \
../src/prj/sensor/temp_table_hot_in_out.c \
../src/prj/sensor/temp_table_ice_tray_in.c \
../src/prj/sensor/temp_table_room.c 

COMPILER_OBJS += \
src/prj/sensor/level.obj \
src/prj/sensor/temp.obj \
src/prj/sensor/temp_table_ambient.obj \
src/prj/sensor/temp_table_cold.obj \
src/prj/sensor/temp_table_cold_bottom.obj \
src/prj/sensor/temp_table_cold_lattron.obj \
src/prj/sensor/temp_table_common.obj \
src/prj/sensor/temp_table_eva.obj \
src/prj/sensor/temp_table_hot_in_out.obj \
src/prj/sensor/temp_table_ice_tray_in.obj \
src/prj/sensor/temp_table_room.obj 

# Each subdirectory must supply rules for building sources it contributes
src/prj/sensor/%.obj: ../src/prj/sensor/%.c 
	@echo 'Building file: $<'
	ccrl -subcommand="src\prj\sensor\cSubCommand.tmp" -msg_lang=english -o "$(@:%.d=%.obj)" "$<"


