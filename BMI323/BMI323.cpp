/**
 * @file BMI323.h
 * @author Reo Tseng
 * @brief Driver for the BMI 323
 * @version 0.1
 * @date 2023-09-26
 * 
 * Datasheets:
 * https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi323-ds000.pdf
 */

#include "BMI323.h"
#include <cinttypes>

/**
 * @brief Construct a new BMI323Base::BMI323Base object
 * 
 */
BMI323Base::BMI323Base()
{
}

/**
 * @brief Construct a new BMI323I2C::BMI323I2C object
 * 
 * @param sda
 * @param scl
 */
BMI323I2C::BMI323I2C(PinName sda, PinName scl) : 
    i2c(sda, scl)
{
    /**
     * @brief 
     * 
     * Section 7.2.1:
     * For using I²C and I3C, it is recommended to hard-wire the CSB line to VDDIO. Since power-on-reset is only executed
     * when both VDD and VDDIO are stable, there is no risk of an incorrect protocol detection due to the power-up sequence.
     */
}

BMI323SPI::BMI323SPI(PinName mosi, PinName miso, PinName sclk, PinName ssel) : 
    spi(mosi, miso, sclk, ssel, use_gpio_ssel)
{
    /**
     * Section 7.2.3 SPI Protocol:
     * 
     * Mode	    Polarity	Phase
     * 0	    0	        0
     * 1	    0	        1
     * 2	    1	        0
     * 3	    1	        1
     * 
     * 00 --> clk active low, sampling on pos edge
     * 01 --> clk active low, samling on neg edge
     * 10 --> clock active high, sampling on neg edge
     * 11 --> clock active high, sampling on pos edge
     * 
     * The SPI interface of the device is compatible with two modes:
     * ’00’ [CPOL = ’0’ and CPHA = ’0’]
     * ’11’ [CPOL = ’1’ and CPHA = ’1’].
     * 
     * The automatic selection between ’00’ and ’11’ is controlled based on the value of the clock on the 
     * SCK pin after a falling edge is detected on the chip select pin CSB.
     */
    spi.format(8, 0);
    spi.set_default_write_value(0);     // Making the value of the dummy write value 0
    spi.frequency(10'000'000);          // Section 7.2.2 Clock Frequency of SPI is 10 MHz
                                        // Drops to 8 MHz when VDDIO < 1.62V
}

bool BMI323I2C::init()
{
    return false;
}

int16_t BMI323I2C::readAddressI2C(Register address)
{    
    // reinterpret cast the data parameter into a char array and store it as a variable called "to_send"
    int16_t data = 0;

    
    // i2c.write((int)address, reinterpret_cast<char *>(&data), 1, true);
    // I2C::Result wResult = i2c.read((int)address, reinterpret_cast<char *>(data), 2);

    return data;
}

bool BMI323I2C::writeAddressI2C(Register address, int16_t data)
{
    return false;
}

void BMI323I2C::readAccel(accel_data* accel)
{
    // TODO
}

/**
 * @brief By default, the BMI is configured to either I3C and I²C mode. We can configure it 
 * for SPI by sending a dummy byte to the CMD register.
 * 
 * Section 4:
 * The default serial interface configuration is I3C and I²C. One initial dummy read (0x00) configures it to SPI.
 * 
 * Per the diagram, after we send a dummy read, we need to read the Chip ID off the IMU and verify if it's valid
 * 
 * @return true if initilization is successful, false otherwise
 */
bool BMI323SPI::init()
{
    printf("Initializing BMI323\n");

    // Send a dummy read to the CMD register, Attempt to read the Chip ID
    int16_t testChipID = readAddressSPI(Register::CHIP_ID);

    // print out the chip ID in hex
    printf("Chip ID: 0x%04x\n", testChipID);

    // If the init is successful
    if(testChipID == 0x0043)
    {
        return true;
    }

    // Bad init
    return false;
}

/**
 * @brief read the passed in address using SPI
 * 
 * 
 */
int16_t BMI323SPI::readAddressSPI(Register address)
{
    // Stores the data to be written to the IMU
    int8_t send = static_cast<int8_t>(address);
    int8_t recieve[3] = {0};


    spi.select();
    int numBytesWritten = spi.write(reinterpret_cast<char *>(send), sizeof(send), 
                                    reinterpret_cast<char *>(recieve), sizeof(recieve));
    spi.deselect();

    printf("The number of bytes written first is: %d\n", numBytesWritten);
    printf("recieve[0]: %d recieve[1]: %d recieve[2]: %d\n", recieve[0], recieve[1], recieve[2]);

    // TODO figure out the endianess of the IMU, and if we need to swap the bytes with __builtin_bswap16
    // Section 4:
    // Data is retrieved from the device by sending one address byte and then reading the required number of dummy
    // bytes followed by two bytes for each register file to be read.
    int16_t result = (static_cast<int16_t>(recieve[2]) << 8) | static_cast<int16_t>(recieve[1]);
    
    return result;
}

/**
 * @brief write the passed in address and using SPI
 * 
 */
bool BMI323SPI::writeAddressSPI(Register address, int16_t data)
{
    /**
     * @brief 
     * Section 6:
     * "for a write access to a register with reserved content, the whole register must be read, 
     * then the desired content must be updated and the content written (back) to the register
     * to avoid overwriting the reserved part with undefined content."
     * 
     */

    return false;
}

void BMI323SPI::readAccel(accel_data* accel)
{
    // TODO
}