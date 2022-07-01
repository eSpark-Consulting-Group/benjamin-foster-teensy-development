#include "sdcard.hpp"

extern bool readFlag;


 /*
  *   SDCardInit(const byte CS) : 
  *     
  *     @brief  :   Initializes the sdcard module 
  *     
  *     @param  :   chipSelect
  *                 Chip-select pin of the sdcard
  *                 
  *     @return :   SUCCESS
  *                 If Initialization is done.
  *                 
  *                 ERR
  *                 If initialization is failed.
  */
bool SDCard::SDCardInit(uint8_t chipSelect)
{
  csPin = chipSelect;
  while (!Serial);
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);
  Serial.println("Initializing SD Card");

  if (!SD.begin(csPin)) 
  {
    Serial.println("SD Card Initialization failed.");
    return ERR;
  }
  else
  {
    Serial.println("SD Card Initialization done.");
    return SUCCESS; 
  } 
}

/*
 * LedSeqRead() 
 * 
 *     @brief  :   Reads the number of lines limited to the Number Of leds define.
 *                 Parse each line which include "Encoder position, Led number, R, G, B," and store the data in the "leds" object.
 *                 Checks if the encoder position from encoder is same as the encoder position from sd-card.                 
 *                 
 *     @param  :   File* MyFile
 *                 Takes a File parameter to open "LedSeqn.txt" File.
 *                 
 *     
 *      @return :  &token
 *                 Address of char array of a single line.
 *                 
 *                 ERR
 *                 If the File is not available.  
 */

char* SDCard::LedSeqRead(File* MyFile)
{   
  String _buffer;
  if(  (*MyFile).available() )
  {
    _buffer = (*MyFile).readStringUntil('\n');
    _buffer.toCharArray(token,30);

    return token;
  }
}

/*
 * LedSeqParse() 
 * 
 *     @brief  :   Simply parse a single-line which includes "Encoder position, Led number, R, G, B," & stored them into variables.
 *                 It also update the "leds" object at each call, with new values of R,G,B for a defined led num.                
 *                 
 *     @param  :   LedSeqLine
 *                 Pointer to a char array which stores a string in the following format:
 *                 e.g: "0,2,50,50,50" -> "Encoder position,Led number,R,G,B,"
 *                 
 *     
 *     @return :   &LedData[5]
 *                 Returns parsed data of the single line.
 *                 {Encoder Position, Led Number, R, G, B}
 *                  
 */

uint32_t* SDCard::LedSeqParse(char* LedSeqLine)
{                                    
    char *parseVal; 
                                     
    parseVal = strtok(LedSeqLine, ","); 
    LedData[0] = atoi(parseVal);
    
    parseVal = strtok(NULL, ",");
    LedData[1] = atoi(parseVal);
    
    parseVal = strtok(NULL, ",");
    LedData[2] = atoi(parseVal);
    
    parseVal = strtok(NULL, ",");
    LedData[3] = atoi(parseVal);
    
    parseVal = strtok(NULL, "\n");
    LedData[4] = atoi(parseVal);

    return LedData;
}
