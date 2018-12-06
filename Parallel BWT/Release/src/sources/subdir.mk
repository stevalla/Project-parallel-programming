################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/sources/arith.c \
../src/sources/bwt.c \
../src/sources/bwtUnzip.c \
../src/sources/bwtZip.c \
../src/sources/mtf.c \
../src/sources/suffixTree.c \
../src/sources/unarith.c \
../src/sources/unbwt.c \
../src/sources/unmtf.c \
../src/sources/unzle.c \
../src/sources/util.c \
../src/sources/zle.c 

OBJS += \
./src/sources/arith.o \
./src/sources/bwt.o \
./src/sources/bwtUnzip.o \
./src/sources/bwtZip.o \
./src/sources/mtf.o \
./src/sources/suffixTree.o \
./src/sources/unarith.o \
./src/sources/unbwt.o \
./src/sources/unmtf.o \
./src/sources/unzle.o \
./src/sources/util.o \
./src/sources/zle.o 

C_DEPS += \
./src/sources/arith.d \
./src/sources/bwt.d \
./src/sources/bwtUnzip.d \
./src/sources/bwtZip.d \
./src/sources/mtf.d \
./src/sources/suffixTree.d \
./src/sources/unarith.d \
./src/sources/unbwt.d \
./src/sources/unmtf.d \
./src/sources/unzle.d \
./src/sources/util.d \
./src/sources/zle.d 


# Each subdirectory must supply rules for building sources it contributes
src/sources/%.o: ../src/sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


