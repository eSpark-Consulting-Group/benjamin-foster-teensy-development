#ifndef SDCARD_H
#define SDCARD_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

#define ERR 0
#define SUCCESS 1

class SDCard {
  private:
    uint8_t csPin;
    char token[30];
    uint32_t LedData[5];
    
  public:
    bool      SDCardInit(uint8_t chipSelect = 9);
    char*     LedSeqRead(File* MyFile);
    uint32_t* LedSeqParse(char* LedSeqLine);
};

#endif
