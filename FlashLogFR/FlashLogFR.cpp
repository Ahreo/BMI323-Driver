#include "FlashLogFR.h"

FlashLogFR::FlashLogFR(PinName _FLOG_MOSI, PinName _FLOG_MISO, PinName _FLOG_SCLK, 
    PinName _FLOG_CS1, PinName _FLOG_CS2, PinName _CONSOLE_RX, PinName _CONSOLE_TX)
{
    // Init the FlashLog SPI lines
    FLOG_MOSI = _FLOG_MOSI;
    FLOG_MISO = _FLOG_MISO;
    FLOG_SCLK = _FLOG_SCLK;
    FLOG_CS1 = _FLOG_CS1;
    FLOG_CS2 = _FLOG_CS2;   
    CONSOLE_TX = _CONSOLE_TX;
    CONSOLE_RX = _CONSOLE_RX;
    
    flashLogSector0 = SPIFBlockDevice(FLOG_MOSI, FLOG_MISO, FLOG_SCLK, FLOG_CS1, FLOG_FREQ);
    flashLogSector1 = SPIFBlockDevice(FLOG_MOSI, FLOG_MISO, FLOG_SCLK, FLOG_CS2, FLOG_FREQ);

    BlockDevice *flashLogArr[2] = {&flashLogSector0, &flashLogSector1};
    ChainingBlockDevice chainedFlashLog(flashLogArr);

    // TODO validate this, check if it caches correctly
    flashLog = bufferedBlockDevice(&ChainingBlockDevice(chainedFlashLog, 2));

    logStart = 0;
    logEnd = flashLog.size();
    currAddr = logStart;

    blockSize = flashLog.get_program_size();
    eraseBlockSize = flashLog.get_erase_size();

    // Reading off the flashlog using serial
    flashLog = BufferedSerial(CONSOLE_TX, CONSOLE_RX, 115200);
}

FlashLogFR::~FlashLogFR()
{
    flashLog.deinit();
}

std::pair<bd_error, FLResultCode> FlashLogFR::init()
{
    bd_error blockDevErr = static_cast<bd_error>(bufferedBlockDevice.init());
    if (blockDevErr)
    {
        printf("[FlashLog] Error %d initializing device!", blockDevErr);
        return std::pair<blockDevErr, FLResultCode::FL_ERROR_BD_INIT>;
    }

    // if we got here, then successfully return
    return std::pair<blockDevErr, FLResultCode::FL_OK>;
}

FLResultCode FlashLogFR::writeData(void *buffer, bd_addr_t address, bd_size_t size)
{
    // Check if the address is within the bounds of the flashlog
    if (address < logStart || address + size > logEnd)
    {
        printf("[FlashLog] Address out of bounds!");
        return FLResultCode::FL_ERROR_BD_PARAMS;
    }

    // Make size a multiple of the block size. Integer division rounds down, add a block size to round up
    // will give us mallest multiple of blockSize greater than size
    bd_size_t readSize = (size / blockSize) + blockSize;

    flashLog.program(packet, currAddr, readSize);
    currAddr += size;
    return FLResultCode::FL_OK;
}

FLResultCode FlashLogFR::readData(void *buffer, bd_addr_t address, bd_size_t size)
{
    // Check if the read is within bounds
    if (address < logStart || address + size > logEnd)
    {
        printf("[FlashLog] Address out of bounds!");
        return FLResultCode::FL_ERROR_BD_PARAMS;
    }

    // Make size a multiple of the block size. Integer division rounds down, add a block size to round up
    // will give us mallest multiple of blockSize greater than size
    bd_size_t readSize = (size / blockSize) + blockSize;
    
    flashLog.read(buffer, address, readSize);
    return FLResultCode::FL_OK;
}

void FlashLogFR::wipeLog()
{
    currAddr = logStart;
    flashLog.erase(logStart, logEnd);
}

bd_addr_t FlashLogFR::getLogSize()
{
    return currAddr - logStart;
}

bd_addr_t FlashLogFR::getRemainingSize()
{
    return logEnd - currAddr;
}