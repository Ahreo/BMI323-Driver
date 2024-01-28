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

#ifndef HAMSTER_BMI323_H
#define HAMSTER_BMI323_H

#include <mbed.h>

/**
 * @brief An enum class to store the status of the BMI323's initilization
 * 
 */
enum InitStatus : uint8_t {
    INIT_SUCCESS,
    INIT_FAIL,
    DEVICE_NOT_FOUND
};

/**
 * @brief A struct to hold the accel data
 */
struct accel_data {
    int16_t x;
    int16_t y;
    int16_t z;
    uint32_t timestamp;
};

/**
 * @brief A struct to hold the gyro data
*/
struct gyro_data {
    int16_t x;
    int16_t y;
    int16_t z;
    uint32_t timestamp;
};

/**
 * @brief SPI implementation of a BMI323 driver
 */
class BMI323Base
{
    public:
        /**
         * @brief An enum class for the BMI323's registers
         */
        enum class Register : uint8_t
        {
            CHIP_ID                 = 0x00,
            ERR_REG                 = 0x01,
            STATUS                  = 0x02,
            ACC_DATA_X              = 0x03,
            ACC_DATA_Y              = 0x04,
            ACC_DATA_Z              = 0x05,
            GYR_DATA_X              = 0x06,
            GYR_DATA_Y              = 0x07,
            GYR_DATA_Z              = 0x08,
            TEMP_DATA               = 0x09,
            SENSOR_TIME_0           = 0x0A,
            SENSOR_TIME_1           = 0x0B,
            SAT_FLAGS               = 0x0C,
            INT_STATUS_INT1         = 0x0D,
            INT_STATUS_INT2         = 0x0E,
            INT_STATUS_IBI          = 0x0F,
            FEATURE_IO0             = 0x10,
            FEATURE_IO1             = 0x11,
            FEATURE_IO2             = 0x12,
            FEATURE_IO3             = 0x13,
            FEATURE_IO_STATUS       = 0x14,
            FIFO_FILL_LEVEL         = 0x15,
            FIFO_DATA               = 0x16,

            ACC_CONF                = 0x20,
            GYR_CONF                = 0x21,

            ALT_ACC_CONF            = 0x28,
            ALT_GYR_CONF            = 0x29,
            ALT_CONF                = 0x2A,
            ALT_STATUS              = 0x2B,

            FIF_WATERMARK           = 0x35,
            FIFO_CONF               = 0x36,
            FIFO_CTRL               = 0x37,
            IO_INT_CTRL             = 0x38,
            INT_CONF                = 0x39,
            INT_MAP_1               = 0x3A,
            INT_MAP_2               = 0x3B,

            FEATURE_CTRL            = 0x40,
            FEATURE_DATA_ADDR       = 0x41,
            FEATURE_DATA_TX         = 0x42,
            FEATURE_DATA_STATUS     = 0x43,

            FEATURE_ENGINE_STATUS   = 0x45,

            FEATURE_EVENT_EXT       = 0x47,

            IO_PDN_CTRL             = 0x4F,
            IO_SPI_IF               = 0x50,
            IO_PAD_STRENGTH         = 0x51,
            IO_I2C_IF               = 0x52,
            IO_ODR_DEVIATION        = 0x53,

            ACC_DP_OFF_X            = 0x60,
            ACC_DP_DGAIN_X          = 0x61,
            ACC_DP_OFF_Y            = 0x62,
            ACC_DP_DGAIN_Y          = 0x63,
            ACC_DP_OFF_Z            = 0x64,
            ACC_DP_DGAIN_Z          = 0x65,
            GYR_DP_OFF_X            = 0x66,
            GYR_DP_DGAIN_X          = 0x67,
            GYR_DP_OFF_Y            = 0x68,
            GYR_DP_DGAIN_Y          = 0x69,
            GYR_DP_OFF_Z            = 0x6A,
            GYR_DP_DGAIN_Z          = 0x6B,

            I3C_TC_SYNC_TPH         = 0x70,
            I3C_TC_SYNC_TU          = 0x71,
            I3C_TC_SYNC_ODR         = 0x72,

            CMD                     = 0x7E,
            CFG_RES                 = 0x7f
        };

    public:
        /**
         * @brief Construct a new BMI323 object
         */
        BMI323Base();

        /**
         * @brief Initilize the BMI323
         * 
         * @return true if initilization is successful, false otherwise
         */
        virtual bool init() = 0;

        // Read Accel (returns 3 values)
        virtual void readAccel(accel_data* accel) = 0;
};
class BMI323I2C : public BMI323Base
{
    public:
        /**
         * @brief Construct a new BMI323 object (I2C)
         * 
         * @param sda data line
         * @param scl clock line
         * @param address address of the BMI323
         */
        BMI323I2C(PinName sda, PinName scl, uint8_t address);

        /**
         * @brief Initilize the BMI323
         * 
         * @return true if initilization is successful, false otherwise
         */
        bool init() override;

        /**
         * @brief Read accel I2C
         * 
         */
        void readAccel(accel_data* accel) override;

    protected:
        // Read the the passed in address and return the value there
        int16_t readAddressI2C(Register address);
        // Write the passed in value to the passed in address
        bool writeAddressI2C(Register address, int16_t value);
    
    private:
        I2C i2c;
        const uint8_t i2c_address;
};


class BMI323SPI : public BMI323Base
{
    public:
        /**
         * @brief Construct a new BMI323 object (SPI)
         * 
         * @param mosi master out, slave in
         * @param miso master in, slave out
         * @param sclk clock
         * @param ssel slave select
         */
        BMI323SPI(PinName mosi, PinName miso, PinName sclk, PinName ssel);

        /**
         * @brief Initilize the BMI323
         * 
         * @return true if initilization is successful, false otherwise
         * Will have to send a dummy byte via readAddressSPI to initilize the BMI323
         */
        bool init() override;

        /**
         * @brief Read accel SPI
         * 
         */
        void readAccel(accel_data* accel) override;

    protected:
        // Read the the passed in address and return the value there
        int16_t readAddressSPI(Register address);

        // Write the passed in value to the passed in address
        bool writeAddressSPI(Register address, int16_t value);
    
    private:
        SPI spi;        
};


#endif // HAMSTER_BMI323_H