cmake_minimum_required(VERSION 3.19)

set(BMI323_SOURCE BMI323.cpp BMI323.h)

add_library(BMI323 STATIC ${BMI323_SOURCE})

# using the library target to specify include directory
target_include_directories(BMI323 PUBLIC .)
target_link_libraries(BMI323 mbed-core-flags)

