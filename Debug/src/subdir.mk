################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/TestArm.cpp \
../src/bsp.cpp \
../src/eqep.cpp \
../src/hcsr04.cpp 

OBJS += \
./src/TestArm.o \
./src/bsp.o \
./src/eqep.o \
./src/hcsr04.o 

CPP_DEPS += \
./src/TestArm.d \
./src/bsp.d \
./src/eqep.d \
./src/hcsr04.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -I/home/mic/BBB/TestArm/include -I/usr/local/lib -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


