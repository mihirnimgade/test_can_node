################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../MDK-ARM/RTE/Device/STM32F103C8/startup_stm32f10x_md.s 

C_SRCS += \
../MDK-ARM/RTE/Device/STM32F103C8/system_stm32f10x.c 

OBJS += \
./MDK-ARM/RTE/Device/STM32F103C8/startup_stm32f10x_md.o \
./MDK-ARM/RTE/Device/STM32F103C8/system_stm32f10x.o 

S_DEPS += \
./MDK-ARM/RTE/Device/STM32F103C8/startup_stm32f10x_md.d 

C_DEPS += \
./MDK-ARM/RTE/Device/STM32F103C8/system_stm32f10x.d 


# Each subdirectory must supply rules for building sources it contributes
MDK-ARM/RTE/Device/STM32F103C8/%.o: ../MDK-ARM/RTE/Device/STM32F103C8/%.s MDK-ARM/RTE/Device/STM32F103C8/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m3 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"
MDK-ARM/RTE/Device/STM32F103C8/%.o: ../MDK-ARM/RTE/Device/STM32F103C8/%.c MDK-ARM/RTE/Device/STM32F103C8/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../Core/Inc -I../../Drivers/STM32F1xx_HAL_Driver/Inc -I../../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

