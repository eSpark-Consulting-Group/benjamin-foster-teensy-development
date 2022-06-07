#include <AccelStepper.h>
#include <FastLED.h>
#include <SPI.h>
#include <SD.h>

//Fucntions:
void SD_Card_Initialization();
void geoSeq_Read();                                           //Function to Read Geo Sequence text file
void geoSeq_parseData(char *str_geoSeq);                      //Function to extract array EncoderPosition,Latitude,Longitude 
void Ledseq_Read();
void Ledseq_parseData(char* workVar);
void Led_Initialization();
void LedExecution();
void EncoderInit();
void updateEncoder();
void LonMotor_Initialization();
void LatMotor_Initialization();
void LatLonTimer_setup();
void LatLonTicker();

//Flags:
bool revFlag = false; // Indicates 1 rev is complete & re-open LedSeq.txt
bool readFlag = true;
int action_index = 0;
int arcstep = 0,rev =0;

//SD Card Pinouts & Configuration
const int chipSelect = 10; //BUILTIN_SDCARD;  
char token[30];
String _buffer;
File MyFile;


//LedSequence Values:
int ndx_Ledseq = 0 ;
int indexNum_Ledseq,ledNum,brightness[3];

//LED Variables:
#define NUM_LEDS 10
#define DATA_PIN 3
//#define CLOCK_PIN 13
//#define FRAMES_PER_SECOND  120
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  SD_Card_Initialization();
  Led_Initialization();
  EncoderInit();
}
void loop() {
  Ledseq_Read();
}
void SD_Card_Initialization() 
{
  while (!Serial);
  pinMode(chipSelect, OUTPUT);
  digitalWrite(chipSelect, HIGH);
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    while (true);
  }
  Serial.println("initialization done.");

  MyFile = SD.open("LedSeq1.txt");
    if (MyFile) 
    {
      Serial.println("LedSeq1.txt is avaiable");
    }
    else
    {
      Serial.println("error");
      while(1);
    }
  Serial.println("LedSeq1.txt is open.");
}

void Ledseq_Read() 
{  
  
  if( revFlag == true )
  {
    MyFile.close();
    MyFile = SD.open("LedSeq1.txt");
    revFlag = false;
  }   
    while (  MyFile.available() && readFlag == true ) 
    {  
      for( int lines = 0; lines < 10; lines++)
      {
        readFlag = false;
        _buffer = MyFile.readStringUntil('\n');
        _buffer.toCharArray(token,20);
        Ledseq_parseData(token);
      }
    Serial.println();       
    Serial.println("Read,Parse,Store of 10 Lines Is Completed!"); 
    }
}
void Ledseq_parseData(char* workVar)
{                                    
    char * strtokIndx;                                  
    strtokIndx = strtok(workVar, ",");
    indexNum_Ledseq = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    ledNum = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    brightness[0] = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    brightness[1] = atoi(strtokIndx);

    strtokIndx = strtok(NULL, "\r");
    brightness[2] = atoi(strtokIndx);
    Serial.print(ledNum);
    //Storing led data of each line (it'll store 50leds data before execution)
    leds[ledNum] = CRGB(brightness[0], brightness[1], brightness[2]);
}
void Led_Initialization()
{
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  
  Serial.println("RGB Led Initialized");
}
void EncoderInit()
{
  pinMode(2,INPUT);
  attachInterrupt(digitalPinToInterrupt(2), updateEncoder, RISING);
}
void updateEncoder()
{
   readFlag = true;
   FastLED.show(); 
   Serial.println("LEDS EXECUTED");
   Serial.print("arcStep : ");
   Serial.println(arcstep);
   arcstep++;
    if(arcstep == 50)
    {
      rev++;
      arcstep = 0;
      revFlag = true;
      Serial.println("One Revolution Completes!");
       if( rev%8 == 0)
        {
          Serial.println("Motors Move");         
        }   
    }
}
