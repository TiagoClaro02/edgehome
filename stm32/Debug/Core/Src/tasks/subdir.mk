################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/tasks/comms_task.c \
../Core/Src/tasks/sensor_actuator_task.c \
../Core/Src/tasks/system_task.c 

OBJS += \
./Core/Src/tasks/comms_task.o \
./Core/Src/tasks/sensor_actuator_task.o \
./Core/Src/tasks/system_task.o 

C_DEPS += \
./Core/Src/tasks/comms_task.d \
./Core/Src/tasks/sensor_actuator_task.d \
./Core/Src/tasks/system_task.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/tasks/%.o Core/Src/tasks/%.su Core/Src/tasks/%.cyclo: ../Core/Src/tasks/%.c Core/Src/tasks/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/BMP280 -I../Drivers/csrc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/tasks/sensor_actuator_task.o: ../Core/Src/tasks/sensor_actuator_task.c Core/Src/tasks/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/BMP280 -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/csrc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-tasks

clean-Core-2f-Src-2f-tasks:
	-$(RM) ./Core/Src/tasks/comms_task.cyclo ./Core/Src/tasks/comms_task.d ./Core/Src/tasks/comms_task.o ./Core/Src/tasks/comms_task.su ./Core/Src/tasks/sensor_actuator_task.cyclo ./Core/Src/tasks/sensor_actuator_task.d ./Core/Src/tasks/sensor_actuator_task.o ./Core/Src/tasks/sensor_actuator_task.su ./Core/Src/tasks/system_task.cyclo ./Core/Src/tasks/system_task.d ./Core/Src/tasks/system_task.o ./Core/Src/tasks/system_task.su

.PHONY: clean-Core-2f-Src-2f-tasks

