#include <mbed.h>
#include <cinttypes>
#include "BMI323/BMI323.h"

int main()
{
#ifdef TARGET_INTEGRATOR_BOARD
    PinName mosi = PB_5;    // SPI 1
    PinName miso = PB_4;    // SPI 1
    PinName sclk = PB_3;    // SPI 1
    PinName ssel = PA_15;   // SPI 1
#else
    // Nucleo board
    PinName mosi = PB_5;    // SPI 1
    PinName miso = PA_6;    // SPI 1
    PinName sclk = PA_5;    // SPI 1
    PinName ssel = PD_14;   // SPI 1
#endif

    BMI323SPI bmi(mosi, miso, sclk, ssel);
    ThisThread::sleep_for(10ms);
    
    while (true)
    {
        printf("\nBMI323 Test Suite:\n");
        printf("Select a test: \n");
        printf("1.  Test BMI init\n");
        printf("2.  Test BMI Accel init\n");
        printf("3.  Test BMI readAccel\n");
        printf("4.  Test BMI gyro init\n");
        printf("5.  Test BMI readGyro\n");
        printf("6.  Test bulk read\n");
        printf("9.  Exit Test Suite\n");

        // scanf("%d", &test);
        int testNumber = 0;
        int read = 0;
        
        while (read <= 0) {
            read = scanf("%d", &testNumber);
            if (read <= 0) {
                // Clear the input buffer
                while (getchar() != '\n');
                printf("\nInvalid input. Please enter a valid test number: ");
            }
        }
        printf("Running test %d:\n\n", testNumber);

        // Run Tests
        switch (testNumber)
        {
            case 1: // Test BMI init
            {
                bool init_success = bmi.init();
                printf(init_success ? "Init Success!\n" : "Init Failed! \n");
                break;
            }
            case 2: // Test BMI Accel init
            {
                bmi.accelSetup();
                break;
            }
            case 3: // Test BMI readAccel
            {
                BMI323Base::accel_data accel;
                for(int i=0; i<100; i++)
                {
                    bmi.readAccel(&accel);
                    printf("Accel: x: %f, y: %f, z: %f\n", accel.x, accel.y, accel.z);
                    // printf("Accel: x: %d, y: %d, z: %d\n", accel.x, accel.y, accel.z);
                    wait_us(500000);
                }

                break;
            }
            case 4: // test BMI gyro init
            {
                bmi.gyroSetup();
                break;
            }
            case 5: // test BMI readGyro
            {
                BMI323Base::gyro_data gyro;
                for(int i=0; i<100; i++)
                {
                    bmi.readGyro(&gyro);
                    printf("Gyro: x: %f, y: %f, z: %f\n", gyro.x, gyro.y, gyro.z);
                    wait_us(500000);
                }
                break;
            }
            case 6: // test bulk read
            {
                BMI323Base::accel_gyro_data data;
                for(int i=0; i<100; i++)
                {
                    bmi.bulkRead(&data);
                    printf("Accel: x: %f, y: %f, z: %f\n", data.accel.x, data.accel.y, data.accel.z);
                    printf("Gyro: x: %f, y: %f, z: %f\n", data.gyro.x, data.gyro.y, data.gyro.z);
                    wait_us(500000);
                }
                break;
            }
            
            case 9: // Exit test suite
            {
                printf("\nExiting Test Suite\n");
                return 0;
            }
            default:
            {
                printf("%d Is an Invalid Test Number Selection.\n", testNumber);
                fflush(stdin);
                break;
            }
        }

        printf("Done.\r\n");
    }

}