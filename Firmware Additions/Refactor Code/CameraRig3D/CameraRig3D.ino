#include "sdcard.hpp"

SDCard card;
File MyFile;
char* readLine = "";

int8_t check;

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
  if(readLine != NULL)
    {
      readLine = card.LedSeqRead(&MyFile);
      Ledinfo = card.LedSeqParse(readLine);
    }
}
