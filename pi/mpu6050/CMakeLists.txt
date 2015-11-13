cmake_minimum_required (VERSION 3.0)
project (MPU6050)
include(FindPkgConfig)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-std=c++11")

pkg_search_module(JSONCPP REQUIRED jsoncpp)
pkg_search_module(EIGEN3 REQUIRED eigen3)

add_library(mpu6050 
   src/attitude.cpp 
   src/calibration.cpp 
   src/i2c.cpp 
   src/imu.cpp 
   src/mpu6050.cpp 
   src/utility.cpp
   src/kalman.cpp
)


target_include_directories(
	mpu6050 SYSTEM PRIVATE 
	src 
	"${JSONCPP_INCLUDE_DIRS}"
	"${EIGEN3_INCLUDE_DIRS}"
	)


add_executable(mpu6050-sampler 
   src/sampler.cpp
)

target_include_directories(
	mpu6050-sampler SYSTEM PRIVATE 
	src 
	"${JSONCPP_INCLUDE_DIRS}"
	"${EIGEN3_INCLUDE_DIRS}"
)


target_link_libraries(
	mpu6050-sampler 
	"${JSONCPP_LIBRARIES}" 
	mpu6050 
	boost_program_options	
)


add_executable(mpu6050-broadcaster 
   src/broadcaster.cpp
)


target_include_directories(
	mpu6050-broadcaster SYSTEM PRIVATE 
	src 
	"${JSONCPP_INCLUDE_DIRS}"
	"${EIGEN3_INCLUDE_DIRS}"
)


target_link_libraries(
	mpu6050-broadcaster 
	"${JSONCPP_LIBRARIES}" 
	mpu6050 
	boost_program_options 
	nanomsg
) 


add_executable(mpu6050-broadcast-receiver
   src/receiver.cpp
)


target_link_libraries(
	mpu6050-broadcast-receiver 
	nanomsg
) 
