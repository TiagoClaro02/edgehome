################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/state/state_manager.c \
../Core/Src/state/system_state.c 

OBJS += \
./Core/Src/state/state_manager.o \
./Core/Src/state/system_state.o 

C_DEPS += \
./Core/Src/state/state_manager.d \
./Core/Src/state/system_state.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/state/%.o Core/Src/state/%.su Core/Src/state/%.cyclo: ../Core/Src/state/%.c Core/Src/state/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/BMP280 -I../Drivers/csrc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-state

clean-Core-2f-Src-2f-state:
	-$(RM) ./Core/Src/state/state_manager.cyclo ./Core/Src/state/state_manager.d ./Core/Src/state/state_manager.o ./Core/Src/state/state_manager.su ./Core/Src/state/system_state.cyclo ./Core/Src/state/system_state.d ./Core/Src/state/system_state.o ./Core/Src/state/system_state.su

.PHONY: clean-Core-2f-Src-2f-state

