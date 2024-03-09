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

    // BMI323 requires a rising edge after power up to enable SPI.
    spi.select();
    wait_us(200);
    spi.deselect();
    wait_us(200);
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

    char data[3];

    readAddressSPI(Register::CHIP_ID, data, 2);
    // Send a dummy read to the CMD register, Attempt to read the Chip ID
    int16_t testChipID = (static_cast<int16_t>(data[2]) << 8) | static_cast<int16_t>(data[1]);

    // print out the chip ID in hex
    printf("Chip ID: 0x%04x\n", testChipID);

    readAddressSPI(Register::STATUS, data, 3);

    int16_t status = (static_cast<int16_t>(data[2]) << 8) | static_cast<int16_t>(data[1]);

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
 * The returned char array is in little endian
 */
void BMI323SPI::readAddressSPI(Register address, char* data, uint8_t length)
{
    spi.select();
    spi.write(0x80 | static_cast<uint8_t>(address));
    spi.write(nullptr, 0, data, length);
    spi.deselect();
}

/**
 * @brief write the passed in address and using SPI
 * 
 */
bool BMI323SPI::writeAddressSPI(Register address, uint16_t data)
{
    /**
     * @brief 
     * Section 6:
     * "for a write access to a register with reserved content, the whole register must be read, 
     * then the desired content must be updated and the content written (back) to the register
     * to avoid overwriting the reserved part with undefined content."
     */

    uint8_t toSend[3] = {static_cast<uint8_t>(address), static_cast<uint8_t>(data & 0xFF), static_cast<uint8_t>((data >> 8) & 0xFF)};

    spi.write(toSend, 3, nullptr, 0);

    return false;
}

void BMI323SPI::readAccel(accel_data* accel)
{
    char data[7];

    readAddressSPI(Register::ACC_DATA_X, data, 7);

    accel->x = static_cast<float>(static_cast<int16_t>((static_cast<int16_t>(data[2]) << 8) | static_cast<int16_t>(data[1])));
    accel->y = static_cast<float>(static_cast<int16_t>((static_cast<int16_t>(data[4]) << 8) | static_cast<int16_t>(data[3])));
    accel->z = static_cast<float>(static_cast<int16_t>((static_cast<int16_t>(data[6]) << 8) | static_cast<int16_t>(data[5])));

    accel->x = (accel->x / 16.38f) / 1000.0f;
    accel->y = (accel->y / 16.38f) / 1000.0f;
    accel->z = (accel->z / 16.38f) / 1000.0f;
}

void BMI323SPI::readGyro(gyro_data* gyro)
{
    char data[7];

    readAddressSPI(Register::GYR_DATA_X, data, 7);

    gyro->x = static_cast<float>(static_cast<int16_t>((static_cast<int16_t>(data[2]) << 8) | static_cast<int16_t>(data[1])));
    gyro->y = static_cast<float>(static_cast<int16_t>((static_cast<int16_t>(data[4]) << 8) | static_cast<int16_t>(data[3])));
    gyro->z = static_cast<float>(static_cast<int16_t>((static_cast<int16_t>(data[6]) << 8) | static_cast<int16_t>(data[5])));

    gyro->x = (gyro->x / 262.144f);
    gyro->y = (gyro->y / 262.144f);
    gyro->z = (gyro->z / 262.144f);
}

void BMI323SPI::bulkRead(accel_gyro_data* data)
{
    char toRecieve[13];

    readAddressSPI(Register::ACC_DATA_X, toRecieve, 15);

    data->accel.x = static_cast<float>(static_cast<int16_t>((static_cast<int16_t>(toRecieve[2]) << 8) | static_cast<int16_t>(toRecieve[1])));
    data->accel.y = static_cast<float>(static_cast<int16_t>((static_cast<int16_t>(toRecieve[4]) << 8) | static_cast<int16_t>(toRecieve[3])));
    data->accel.z = static_cast<float>(static_cast<int16_t>((static_cast<int16_t>(toRecieve[6]) << 8) | static_cast<int16_t>(toRecieve[5])));
    data->gyro.x = static_cast<float>(static_cast<int16_t>((static_cast<int16_t>(toRecieve[8]) << 8) | static_cast<int16_t>(toRecieve[7])));
    data->gyro.y = static_cast<float>(static_cast<int16_t>((static_cast<int16_t>(toRecieve[10]) << 8) | static_cast<int16_t>(toRecieve[9])));
    data->gyro.z = static_cast<float>(static_cast<int16_t>((static_cast<int16_t>(toRecieve[12]) << 8) | static_cast<int16_t>(toRecieve[11])));


    data->accel.x = (data->accel.x / 16.38f) / 1000.0f;
    data->accel.y = (data->accel.y / 16.38f) / 1000.0f;
    data->accel.z = (data->accel.z / 16.38f) / 1000.0f;

    data->gyro.x = (data->gyro.x / 262.144f);
    data->gyro.y = (data->gyro.y / 262.144f);
    data->gyro.z = (data->gyro.z / 262.144f);
}


void BMI323SPI::accelSetup()
{
    uint16_t accelConfig = 0x7000;  // Enables the accelerometer in high performance mode
    uint16_t accAvgNum = 0x0000;    // no averaging, pass sample without filtering
    uint16_t LSBperMg = 0x0000;     // +/-2g, 16.38 LSB/mg
    uint16_t ODR_6_4_KHz = 0x000B;  // ODR = 800Hz

    uint16_t toSend = accelConfig | accAvgNum | LSBperMg | ODR_6_4_KHz;
    
    char responseData[3] = {0x0, 0x0, 0x0};

    readAddressSPI(Register::ACC_CONF, responseData, 3);
    printf("Content of ACC_CONF is: 0x%04x\n", (static_cast<int16_t>(responseData[2]) << 8) | static_cast<int16_t>(responseData[1]));

    writeAddressSPI(Register::ACC_CONF, toSend);
    readAddressSPI(Register::ACC_CONF, responseData, 3);

    printf("Content of ACC_CONF is: 0x%04x\n", (static_cast<int16_t>(responseData[2]) << 8) | static_cast<int16_t>(responseData[1]));
}

void BMI323SPI::gyroSetup()
{
    uint16_t gyroMode = 0x7000; // Enables the gyroscope in high performance mode
    uint16_t gyroAvgNum = 0x0000; // no averaging, pass sample without filtering
    uint16_t gyroRange = 0x0000; // +/-125◦/s, 262.144 LSB/◦/s
    uint16_t ODR = 0x000B; // ODR = 800Hz

    uint16_t toSend = gyroMode | gyroAvgNum | gyroRange | ODR;

    char responseData[3] = {0x0, 0x0, 0x0};

    readAddressSPI(Register::GYR_CONF, responseData, 3);
    printf("Content of GYR_CONF is: 0x%04x\n", (static_cast<int16_t>(responseData[2]) << 8) | static_cast<int16_t>(responseData[1]));

    writeAddressSPI(Register::GYR_CONF, toSend);
    readAddressSPI(Register::GYR_CONF, responseData, 3);

    printf("Content of GYR_CONF is: 0x%04x\n", (static_cast<int16_t>(responseData[2]) << 8) | static_cast<int16_t>(responseData[1]));
}