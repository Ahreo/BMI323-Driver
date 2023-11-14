#include <mbed.h>
#include <cinttypes>
#include "BMI323/BMI323.h"

int main()
{
    //mosi miso sclk ssel
    PinName mosi = PB_5;    // SPI_B_MOSI | CN7
    PinName miso = PB_4;    // SPI_B_MISO | CN7
    PinName sclk = PB_3;    // SPI_B_SCK  | CN7
    PinName ssel = PA_4;    // SPI_B_NSS  | CN7

    BMI323SPI bmi(mosi, miso, sclk, ssel);
    ThisThread::sleep_for(10ms);
    
    while (true)
    {
        printf("\nBMI323 Test Suite:\n");
        printf("Select a test: \n");
        printf("1.  Test BMI init\n");
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
            case 9: // Exit test suite
                printf("\nExiting Test Suite\n");
                return 0;
            default:
                printf("%d Is an Invalid Test Number Selection.\n", testNumber);
                fflush(stdin);
                break;
        }

        printf("Done.\r\n");
    }

}