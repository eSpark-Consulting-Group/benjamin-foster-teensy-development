#ifndef SDCARD_H
#define SDCARD_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

#define ERR -1
#define SUCCESS 0

typedef struct {    
    uint16_t encoderPos;
    uint8_t ledNum;
    uint8_t R;
    uint8_t G;
    uint8_t B;   
}LedData;

class SDCard {
  private:
    uint8_t csPin;
    char token[30];
    LedData Led;
    
  public:
    int8_t      SDCardInit(uint8_t chipSelect = 9);
    char*       LedSeqRead(File* MyFile);
    LedData     LedSeqParse(char* LedSeqLine);
};

#endif
