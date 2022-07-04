#include "sdcard.hpp"

SDCard card;
bool readFlag = true;
File MyFile;

int8_t check;

char* readLine;
uint32_t* data;
LedData Ledinfo;

void setup() 
{
  Serial.begin(9600);
  check = card.SDCardInit();
  if(check == ERR )
  {
    Serial.println("SD Card Initialization Failed!");
    return;
  }
  MyFile = SD.open("LedSeq1.txt");
}

void loop() 
{
    readLine = card.LedSeqRead(&MyFile);
    if(readLine != NULL)
    {
      Ledinfo = card.LedSeqParse(readLine);
      Serial.print("Encoder Position : ");
      Serial.println(Ledinfo.encoderPos);
    }
}
