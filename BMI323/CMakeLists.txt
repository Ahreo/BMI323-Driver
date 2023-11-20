cmake_minimum_required(VERSION 3.19)

# Defining a variable for the source files for the BMI323 library
set(BMI323_SOURCE BMI323.cpp BMI323.h)

# Creating a static library (means the library will be linked at compile time)
# Includes the source files in the BMI323_SOURCE variable
add_library(BMI323 STATIC ${BMI323_SOURCE})

# Specifying the include directory for the BMI323 library
# Saying, include everything in the current directory in include path
target_include_directories(BMI323 PUBLIC .)

# Linking the BMI323 library with the mbed-core-flags library
target_link_libraries(BMI323 mbed-core-flags)

