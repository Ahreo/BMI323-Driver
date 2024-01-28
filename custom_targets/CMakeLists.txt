# In this target, we want to override system_clock.c from Mbed with our own clock config file.
# This file is part of the mbed-nucleo-h743zi2 target, so we want to remove it from there
# Mbed has a dedicated function for this, mbed_disable_mcu_target_file().  This function takes
# the target and the filename to remove.
# Note: We only want to call this function when the project is actually configured for the
# L452RE_CUSTOM_CLOCK target.
if("${MBED_TARGET}" STREQUAL "INTEGRATOR_BOARD")
	mbed_disable_mcu_target_file(mbed-nucleo-h743zi2 system_clock.c)
endif()

# To create a custom target, you first need to figure out the CMake name of the target
# from its Mbed name.  This name is calculated as:
#
# 1. Convert target to lowercase
# 2. Replace all underscores (_) with hyphens (-)
# 3. add "mbed-" prefix.

# Now, we can add our custom target that includes our own clock file.
add_library(mbed-integrator-board INTERFACE)
target_sources(mbed-integrator-board INTERFACE INTEGRATOR_BOARD/system_clock.c)
target_link_libraries(mbed-integrator-board INTERFACE mbed-nucleo-h743zi2)