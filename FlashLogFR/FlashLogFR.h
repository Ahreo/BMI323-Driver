#include "SPIFBlockDevice.h"
#include "ChainingBlockDevice.h"
#include "BufferedBlockDevice.h"
#include "mbed.h"

// Currently only supports our NOR flash chip, but we can add support for SD cards later

class FlashLogFR 
{
    enum FLResultCode
    {
        FL_SUCCESS = 0,

        FL_ITERATION_DONE = -8,     // Indicates that this iterator has reached the end

        FL_ERROR_BOUNDS = -1,       // Log is full and/or the current operation would go out of bounds
        FL_ERROR_EMPTY = -2,        // Indicates that the flashlog is empty
        FL_ERROR_CHECKSUM = -3,
        FL_ERROR_TYPE  = -4,        // unrecognized packet type encountered
        FL_ERROR_NOTAIL = -5,
        FL_ERROR_LOGNOINIT = -6,    // Log did not init successfully earlier, so can't perform this operation
        FL_ERROR_FSM_NOT_RESTORED = -7, // Failed to find a valid packet from which to restore state.
        FL_ERROR_LOG_EXISTS = -9,   // Indicates that the flashlog is not empty but the last packet could not be found
        FL_ERROR_BD_INIT = -10,     // Error initializing block device
        FL_ERROR_BD_IO = -11,       // Error reading to or writing from block device
        FL_ERROR_BD_PARAMS = -12,   // Error with parameters/configuration of block device
    };

    
    public:
        FlashLogFR(PinName _FLOG_MOSI, PinName _FLOG_MISO, PinName _FLOG_SCLK, 
        PinName _FLOG_CS1, PinName _FLOG_CS2, PinName _CONSOLE_RX, PinName _CONSOLE_TX);
        ~FlashLogFR();

        bd_error init();

        FLResultCode writeData(void *buffer, bd_addr_t address, bd_size_t size);
        FLResultCode readData(void *buffer, bd_addr_t address, bd_size_t size);

        int getSize();
        int getStartAddr();

        void wipeLog();

        bd_addr_t FlashLogFR::getLogSize();
        bd_addr_t FlashLogFR::getRemainingSize();


    private:
        SPIFBlockDevice flashLogSector0;
        SPIFBlockDevice flashLogSector1;
        // ChainingBlockDevice chainedFlashLog;

        // Reading off the flashlog
        BufferedSerial serialPort;

        // Buffered Block Device for caching writes
        BufferedBlockDevice* flashLog;

        PinName FLOG_MOSI;
        PinName FLOG_MISO;
        PinName FLOG_SCLK;
        PinName FLOG_CS1;
        PinName FLOG_CS2;

        PinName CONSOLE_TX;
        PinName CONSOLE_RX;

        constexpr int FLOG_FREQ = 40000000;

        /** Addressing bounds of the memory chips in use */
        const bd_addr_t flashStartAddr  = 0x00000000;
        /** for two 64 Mbyte flash cards in sequence (each address holds 1 byte) */
        const bd_addr_t flashEndAddr    = 0x08000000;

        /** Current address */
        bd_addr_t currAddr;

        // The addresses that the FlashLog runs between on the flash device
        bd_addr_t logStart;
        bd_addr_t logEnd; // 1 greater than the largest accessible address
        bd_size_t blockSize; // Reading and writing must be done in blocks of a multiple of this size
        bd_size_t eraseBlockSize; // Erasing must be done in blocks of a multiple of this size
};