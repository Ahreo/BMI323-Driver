cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19)

# Initialize Mbed OS build system. 
# Note: This block must be before the include of app.cmake
set(MBED_APP_JSON_PATH mbed_app.json)
set(CUSTOM_TARGETS_JSON_PATH custom_targets.json5) # This activates the custom targets in custom_targets.json

# Sets up the Mbed-Tool chain file to make CMake expect the ARM compiler
# Loads compiler and flags
include(cmake/UploadMethodConfig.cmake)
include(mbed-os/tools/cmake/app.cmake)

# Include folder with the custom targets.
# This needs to be done before add_subdirectory(mbed-os).
add_subdirectory(custom_targets)

# add_subdirectory(custom_targets)
add_subdirectory(mbed-os)

project(BMI323-test)

add_subdirectory(BMI323)

add_executable(test_BMI323 test_BMI323.cpp)
target_link_libraries(test_BMI323 mbed-os BMI323)      # Can also link to mbed-baremetal here
mbed_set_post_build(test_BMI323)

# add subdirectories and build targets here
mbed_finalize_build()