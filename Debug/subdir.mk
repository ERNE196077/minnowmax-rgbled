################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../dma.c \
../main.c \
../pwm.c \
../spi.c \
../ws281x.c 

OBJS += \
./dma.o \
./main.o \
./pwm.o \
./spi.o \
./ws281x.o 

C_DEPS += \
./dma.d \
./main.d \
./pwm.d \
./spi.d \
./ws281x.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


