#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
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

//Flags:
bool Ledseq_readflag = true; //to make sure it works on first step ( arc 0 step )
int8_t eptcf = 0;  //encoder position to change text file of led
//SD Card Pinouts & Configuration
const int chipSelect = BUILTIN_SDCARD; //BUILTIN_SDCARD;  
char token[30];
String _buffer;
File MyFile;

//GeoSequence Variables ( SD-card ):
int indexNum_geoSeq[100], LatitudeVal[100], LongitudeVal[100];
int LatLon_ndx = 0; // To keep track of lat/lon values index

//LedSequence Values:
int ndx_Ledseq = 0 ;
int indexNum_Ledseq,ledNum,brightness[3];

//LED Variables:
#define NUM_LEDS 50
#define DATA_PIN 3
#define CLOCK_PIN 13
#define FRAMES_PER_SECOND  120
CRGB leds[NUM_LEDS];

//Encoder Definitions:
#define ZERO_Point  2               // value 0-999 on encoder where we take the ZERO_Point to be.
#define DEBUG_ENCODER 0
#define rotaryA 1                   //pin up
#define rotaryB 2                   //pin down
//#define rotaryIndex 3              //not part of the rotary lbibrary --> just do normal readout, we only need to check this once in the start, so no interrupt
Encoder MainEncoder(rotaryA, rotaryB);

void setup() {
  Serial.begin(115200);
  SD_Card_Initialization();
  geoSeq_Read();
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
}
void geoSeq_Read()
{
  MyFile = SD.open("GeoSeq.txt");
    if (MyFile) 
    {
      Serial.println("GeoSeq.txt is avaiable");
      MyFile.close();
    }
    else 
    {
      Serial.println("error opening GeoSeq.txt");
      while(1); // <- not neccessary but I used this to stop my code here if .txt is not available.
    }

  MyFile = SD.open("GeoSeq.txt");
  while (MyFile.available()) {
     _buffer = MyFile.readStringUntil('\r');
     _buffer.toCharArray(token,50);
     geoSeq_parseData(token);
    }
  MyFile.close();   //closing the GeoSequence.txt file after the Lat/Lon values are stored
  
  //indicates Geo Sequence reading,parsing & storing is done.
  //geoSeq_readflag = false;   
  
  Serial.println("GeoSeq.txt is closed!");
  MyFile = SD.open("LedSeq.txt");
  Serial.println("LedSeq.txt is open!");
}
void geoSeq_parseData(char *str_geoSeq)
{
  char *strtokIndx;
 
  strtokIndx = strtok(str_geoSeq, ",");
  indexNum_geoSeq[LatLon_ndx] = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  LatitudeVal[LatLon_ndx] = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  LongitudeVal[LatLon_ndx] = atoi(strtokIndx);

  Serial.println("***** Parsed Data ******");
  Serial.println(indexNum_geoSeq[LatLon_ndx]);
  Serial.print("Latitude: ");
  Serial.println(LatitudeVal[LatLon_ndx]);
  Serial.print("Longitude: ");
  Serial.println(LongitudeVal[LatLon_ndx]);
  LatLon_ndx++; //Index of the Arrays which stores GeoSeq parsed values      
}
void Ledseq_Read() 
{
  while ( ( MyFile.available()) && (Ledseq_readflag == true) /*&& (arcStart == true)*/ ) 
  {
      Serial.println("LedSeq Reading 140 Lines.");
      for( int lines = 1; lines <= 140; lines++)
      {
        _buffer = MyFile.readStringUntil('\r');
        _buffer.toCharArray(token,20);
        Ledseq_parseData(token);
      }
      Ledseq_readflag = false; //140 leds are actuated, wait for encoder interrupt
  }
}
void Ledseq_parseData(char* workVar)
{                                      //indexes of the EncoderIndex,Latitude & Longitude arrays
  if( Ledseq_readflag == true){
    char * strtokIndx;                                  // this is used by strtok() as an index
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

    Serial.println("***** Parsed Data ******");
    Serial.println(indexNum_Ledseq);
    Serial.print("LED Number: ");
    Serial.println(ledNum);
    Serial.print("R: ");
    Serial.println(brightness[0]);
    Serial.print("G: ");
    Serial.println(brightness[1]);
    Serial.print("B: ");
    Serial.println(brightness[2]);
    ndx_Ledseq++;
    //Ledseq_readflag = false;
    LedExecution();
  }
}
void Led_Initialization()
{
  FastLED.addLeds<SK9822, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS); 
  FastLED.setMaxRefreshRate(400,true);
  Serial.println("RGB Led Initialized");
}
void LedExecution()
{
  Serial.println("LEDexe start");
  leds[ledNum] = CRGB(brightness[0], brightness[1], brightness[2]);
  FastLED.show();
  Serial.println("LEDexe end");
}
void EncoderInit()
{
  attachInterrupt(1, updateEncoder, CHANGE);
  attachInterrupt(2, updateEncoder, CHANGE);
}
void updateEncoder()
{
  Ledseq_readflag = true; //at each encoder interrupt, allowing functions ledseq_read,ledseq_parse,ledexecute to execute 140 leds.
  eptcf++;  //encoder position to change text file of led
  if(eptcf >= 199)
  { 
    MyFile.close();
    eptcf = 0;
    MyFile = SD.open("LedSeq.txt");
  }
}
