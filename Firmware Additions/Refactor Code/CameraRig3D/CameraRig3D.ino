#include "sdcard.hpp"

SDCard card;
bool readFlag = true;
File MyFile;

bool check ;

char* readLine;
uint32_t* data;

void setup() 
{
  Serial.begin(9600);
  check = card.SDCardInit();
  while(check == ERR);
  MyFile = SD.open("LedSeq1.txt");
}

void loop() 
{
    readLine = card.LedSeqRead(&MyFile);
    data = card.LedSeqParse(readLine);  
}
