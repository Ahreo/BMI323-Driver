# BMI323_Driver
This is a C++ driver for the BMI323 tri-axis accelerometer/gyroscope. It was developed on a `NUCLEO-H723ZG` development board using MbedCE on a windows computer. 

The goal is to have compability with both SPI and I2C. For my purposes, it sufficies to have compatability for it's gyro, accel, and temp sensor.

# Basic Pre-Reqs:
You'll need CMake, ARM's bare-metal toolchain, and a Generator for your binaries (Ninja is recommended, but you can use make, mingw32-makefiles, or whatever tickles your fancy).

the Mbed-os wiki provides great setup guides for these! 
https://github.com/mbed-ce/mbed-os/wiki/Toolchain-Setup-Guide

If you like debugging via VS code, there are also environment/toolchain setup guides:
https://github.com/mbed-ce/mbed-os/wiki/Project-Setup:-VS-Code
https://github.com/mbed-ce/mbed-os/wiki/Project-Setup:-Command-Line

You will also need a serial terminal. I recommend using something like TeraTerm for windows or Minicom for Mac/Linux.
https://tera-term.en.softonic.com

# Dev Board Configuration
We made the dev-board for the BMI323 ourselves on Altium. We found the NUCLEO dev-board schematics after some laborious digging through the St resources folders (they hid the schematics under CAD resources???). Go under schematic pack and you get a sense for where we determined the PinNames for our driver testing:
https://www.st.com/en/evaluation-tools/nucleo-h723zg.html#cad-resources

# Code Testing
To test out and run the code in this repository, make a `build` directory under where you cloned this repo. Depending on the generator you use, you can change the parameter following the `-G` flag. I am using a STLink to flash my code, so I am prepending the `-DUPOAD_METHOD=STM32CUBE` flag. You will also have to change the parameter following the `-DMBED_TARGET` flag according to your CPU. I am using the `STM32H723ZG`. You can find your appropriate target under the `mbed-os` folder, and navigating to the `targets` directory. 

You can learn more about upload methods here:
https://github.com/mbed-ce/mbed-os/wiki/Upload-Methods

If you have a custom target, you can also take a look at the MbedCE Wiki:
https://github.com/mbed-ce/mbed-ce-custom-targets#readme

run the following command in your build directory (replace the upload method and target accordingly):
`cmake .. -G Ninja -DUPLOAD_METHODSTM32CUBE -DMBED_TARGET=STM32H723ZG`

You will then be able to run the test suite and attempt to read from the IC (provided you have your pins connected correctly) via:
`ninja flash-test_BMI323`

