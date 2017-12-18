################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/simplicity_studio/developer/sdks/gecko_sdk_suite/v1.1/hardware/kit/common/bsp/bsp_stk_leds.c 

OBJS += \
./BSP/bsp_stk_leds.o 

C_DEPS += \
./BSP/bsp_stk_leds.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/bsp_stk_leds.o: E:/simplicity_studio/developer/sdks/gecko_sdk_suite/v1.1/hardware/kit/common/bsp/bsp_stk_leds.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m0plus -mthumb -std=c99 '-DDEBUG_EFM=1' '-DDEBUG=1' '-DEFM32HG322F64=1' -I"E:/simplicity_studio/developer/sdks/gecko_sdk_suite/v1.1//app/mcu_example/SLSTK3400A_EFM32HG/blink" -I"C:\Users\evert.DESKTOP-6EE9RU8\SimplicityStudio\v4_workspace\sheepy\emlib_inc" -I"E:/simplicity_studio/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"E:/simplicity_studio/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"E:/simplicity_studio/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"E:/simplicity_studio/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32HG/Include" -I"E:/simplicity_studio/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/SLSTK3400A_EFM32HG/config" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"BSP/bsp_stk_leds.d" -MT"BSP/bsp_stk_leds.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


