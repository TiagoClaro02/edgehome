################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/protocol/crc8.c 

OBJS += \
./Core/Src/protocol/crc8.o 

C_DEPS += \
./Core/Src/protocol/crc8.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/protocol/%.o Core/Src/protocol/%.su Core/Src/protocol/%.cyclo: ../Core/Src/protocol/%.c Core/Src/protocol/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/BMP280 -I../Drivers/csrc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-protocol

clean-Core-2f-Src-2f-protocol:
	-$(RM) ./Core/Src/protocol/crc8.cyclo ./Core/Src/protocol/crc8.d ./Core/Src/protocol/crc8.o ./Core/Src/protocol/crc8.su

.PHONY: clean-Core-2f-Src-2f-protocol

