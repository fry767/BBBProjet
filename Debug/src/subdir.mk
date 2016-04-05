################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/baborwire.c 

CPP_SRCS += \
../src/TestArm.cpp \
../src/bsp.cpp \
../src/encoder_pru_drv.cpp \
../src/eqep.cpp \
../src/hcsr04.cpp \
../src/lm74_drv.cpp 

OBJS += \
./src/TestArm.o \
./src/baborwire.o \
./src/bsp.o \
./src/encoder_pru_drv.o \
./src/eqep.o \
./src/hcsr04.o \
./src/lm74_drv.o 

C_DEPS += \
./src/baborwire.d 

CPP_DEPS += \
./src/TestArm.d \
./src/bsp.d \
./src/encoder_pru_drv.d \
./src/eqep.d \
./src/hcsr04.d \
./src/lm74_drv.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -I/home/mic/BBB/TestArm/include -I/usr/local/lib -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/usr/local/lib -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


