################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/hcsr04-demo.c 

CPP_SRCS += \
../src/HCSRF05.cpp \
../src/TestArm.cpp \
../src/bsp.cpp 

OBJS += \
./src/HCSRF05.o \
./src/TestArm.o \
./src/bsp.o \
./src/hcsr04-demo.o 

C_DEPS += \
./src/hcsr04-demo.d 

CPP_DEPS += \
./src/HCSRF05.d \
./src/TestArm.d \
./src/bsp.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -I/home/mic/BBB/TestArm/include -I/usr/local/lib -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I/usr/local/lib -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


