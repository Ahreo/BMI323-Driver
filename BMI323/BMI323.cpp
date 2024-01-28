/**
 * @file BMI323.h
 * @author Reo Tseng
 * @brief Driver for the BMI 323
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
BMI323I2C::BMI323I2C(PinName sda, PinName scl, uint8_t address) : 
    i2c(sda, scl), i2c_address(address)
{
    //TODO
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
    spi.format(8, 3);
    spi.set_default_write_value(0);     // Making the value of the dummy write value 0
    spi.frequency(100'000);          // Section 7.2.2 Clock Frequency of SPI is 10 MHz
                                        // Drops to 8 MHz when VDDIO < 1.62V
}

bool BMI323I2C::init()
{
    //TODO
    return false;
}

int16_t BMI323I2C::readAddressI2C(Register address)
{    
    //TODO
    // reinterpret cast the data parameter into a char array and store it as a variable called "to_send"
    int16_t data = 0;

    
    // i2c.write((int)address, reinterpret_cast<char *>(&data), 1, true);
    // I2C::Result wResult = i2c.read((int)address, reinterpret_cast<char *>(data), 2);

    return data;
}

bool BMI323I2C::writeAddressI2C(Register address, int16_t data)
{
    //TODO
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

    // BMI323 requires a rising edge after power up to enable SPI.
    spi.select();
    wait_us(200);
    spi.deselect();
    wait_us(200);

    // Send a dummy read to the CMD register, Attempt to read the Chip ID
    int16_t testChipID = readAddressSPI(Register::CHIP_ID);

    // print out the chip ID in hex
    printf("Chip ID: 0x%04x\n", testChipID);

    int16_t status = readAddressSPI(Register::STATUS);

    printf("Status: 0x%04x\n", status);

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
    const int recieveSize = 4;
    // Stores the data to be written to the IMU
    uint8_t send[recieveSize] = {static_cast<uint8_t>(static_cast<uint8_t>(address) | 0x80), 0, 0, 0};
    uint8_t recieve[recieveSize];

    for(int i=0;i<recieveSize; i++) {
        printf("send[%d]: %" PRIx8 "\n", i, send[i]);
    }


    int numBytesWritten = spi.write(send, sizeof(send), 
                                    recieve, sizeof(recieve));

    printf("The number of bytes written first is: %d\n", numBytesWritten);
    for(int i=0;i<recieveSize; i++) {
        printf("recieve[%d]: %d\n", i, recieve[i]);
    }

    // TODO figure out the endianess of the IMU, and if we need to swap the bytes with __builtin_bswap16
    // Section 4:
    // Data is retrieved from the device by sending one address byte and then reading the required number of dummy
    // bytes followed by two bytes for each register file to be read.
    int16_t result = (static_cast<int16_t>(recieve[3]) << 8) | static_cast<int16_t>(recieve[2]);
    
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