################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ftp_server_login_manager.cpp \
../src/ftp_server_logs.cpp \
../src/ftp_server_mysql_op.cpp \
../src/ftpserver.cpp \
../src/main.cpp 

OBJS += \
./src/ftp_server_login_manager.o \
./src/ftp_server_logs.o \
./src/ftp_server_mysql_op.o \
./src/ftpserver.o \
./src/main.o 

CPP_DEPS += \
./src/ftp_server_login_manager.d \
./src/ftp_server_logs.d \
./src/ftp_server_mysql_op.d \
./src/ftpserver.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/mysql -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


