################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../src/prj/Wifi/WifiSetting/WIFI_SetAccumulateData.c \
../src/prj/Wifi/WifiSetting/WIFI_SetCommon.c \
../src/prj/Wifi/WifiSetting/WIFI_SetErrorData.c \
../src/prj/Wifi/WifiSetting/WIFI_SetFunctionData.c \
../src/prj/Wifi/WifiSetting/WIFI_SetInformationData.c \
../src/prj/Wifi/WifiSetting/WIFI_SetMonitorData.c \
../src/prj/Wifi/WifiSetting/WIFI_SetPartData.c \
../src/prj/Wifi/WifiSetting/WIFI_SetSensorData.c 

COMPILER_OBJS += \
src/prj/Wifi/WifiSetting/WIFI_SetAccumulateData.obj \
src/prj/Wifi/WifiSetting/WIFI_SetCommon.obj \
src/prj/Wifi/WifiSetting/WIFI_SetErrorData.obj \
src/prj/Wifi/WifiSetting/WIFI_SetFunctionData.obj \
src/prj/Wifi/WifiSetting/WIFI_SetInformationData.obj \
src/prj/Wifi/WifiSetting/WIFI_SetMonitorData.obj \
src/prj/Wifi/WifiSetting/WIFI_SetPartData.obj \
src/prj/Wifi/WifiSetting/WIFI_SetSensorData.obj 

# Each subdirectory must supply rules for building sources it contributes
src/prj/Wifi/WifiSetting/%.obj: ../src/prj/Wifi/WifiSetting/%.c 
	@echo 'Building file: $<'
	ccrl -subcommand="src\prj\Wifi\WifiSetting\cSubCommand.tmp" -msg_lang=english -o "$(@:%.d=%.obj)" "$<"


