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
void updateEncoder(); //ISR for Encoder Interrupt
void LonMotor_Initialization();
void LatMotor_Initialization();
void LatLonTimer_setup(); 
void LatLonMotors();  //ISR for LatLonTimer_setup

//Flags:
bool LatLonFlag = false;
bool revFlag = false; // Indicates 1 rev is complete & re-open LedSeq.txt
bool readFlag = true;
bool arcFlag = false;   //holds arcMotor until all the initialization is done.
int  action_index = 0;
int  arcstep = 0;
int8_t revCount = 0;

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
#define DEBUG_ENCODER 0
#define rotaryA 1                   //pin up
#define rotaryB 2                   //pin down
Encoder MainEncoder(rotaryA, rotaryB);

//Motors Pinouts and Configurations:
#define motorInterfaceType          1
#define Dir_Latitude                8
#define STEP_Latitude               9
#define Latitude_ENAPIN             22
#define Dir_Longitude               10
#define STEP_Longitude              11
#define Longitude_ENAPIN            21
#define Dir_ArcRotate               6 //6
#define STEP_ArcRotate              7 //7
#define ArcRotate_ENAPIN            20 //20
#define ArcRotateMaxSpeed           400
#define ArcRotateAcceleration       300
#define ArcRotateSpeed              400

IntervalTimer ArcTimer;
IntervalTimer LatLonTimer;

AccelStepper LatMotor = AccelStepper(motorInterfaceType, STEP_Latitude, Dir_Latitude);
AccelStepper LonMotor = AccelStepper(motorInterfaceType, STEP_Longitude, Dir_Longitude);
AccelStepper ArcMotor = AccelStepper(motorInterfaceType, STEP_ArcRotate, Dir_ArcRotate);

void setup() {
  Serial.begin(115200);
  SD_Card_Initialization();
  geoSeq_Read();
  Led_Initialization();
  LatMotor_Initialization();
  LonMotor_Initialization();
  EncoderInit();
  LatLonTimer_setup();
  arcFlag = true;   //arcMotor rotates after this
  ArcMotor_Initialization();
  ArcTimer_setup(); 
}
void loop() 
{
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
  if( revFlag == true ){
    MyFile.close();
    MyFile = SD.open("LedSeq.txt");
   // Serial.println("************************CLOSED & OPEN LEDSEQ.TXT");
    revFlag = false;
  }
  while (  MyFile.available() && readFlag == true ) 
  {
      //Serial.println("LedSeq Reading 50 Lines.");
      for( int lines = 1; lines <= 50; lines++)
      {
        readFlag = false;
        _buffer = MyFile.readStringUntil('\r');
        _buffer.toCharArray(token,20);
        Ledseq_parseData(token);
      }
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
    leds[ledNum] = CRGB(brightness[0], brightness[1], brightness[2]);

}
void Led_Initialization()
{
  //FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS); 
  FastLED.addLeds<SK9822, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS); 
  FastLED.setMaxRefreshRate(400,true);
  Serial.println("RGB Led Initialized");
}

void EncoderInit()
{
  attachInterrupt(1, updateEncoder, CHANGE);
  attachInterrupt(2, updateEncoder, CHANGE);
}
void updateEncoder()
{ 
   readFlag = true;
   FastLED.show(); 
   Serial.println("LEDS EXECUTED");
   Serial.print("arcStep : ");
   Serial.println(MainEncoder.read());
   //arcstep++;
    if(MainEncoder.read() == 999)
    {
      MainEncoder.write(0);
      revCount++;
     // arcstep = 0;
      revFlag = true;
      if( revCount%8 == 0 )
      {
        revCount = 0;
        Serial.println("Move");
        LatMotor.moveTo(LatitudeVal[action_index]);
        LonMotor.move(LongitudeVal[action_index]);
        action_index++;
        LatLonFlag = true;
      }    
    }
}
void LonMotor_Initialization() {
  LonMotor.setEnablePin(Longitude_ENAPIN);
  pinMode(STEP_Longitude,OUTPUT);
  pinMode(Dir_Longitude,OUTPUT);
  LonMotor.setPinsInverted(false, false, true);
  LonMotor.enableOutputs();
  LonMotor.setMaxSpeed(1000);
  LonMotor.setAcceleration(50);
  LonMotor.setSpeed(200);
}
void LatMotor_Initialization() {
  LatMotor.setEnablePin(Latitude_ENAPIN);
  pinMode(STEP_Latitude,OUTPUT);
  pinMode(Dir_Latitude,OUTPUT);
  LatMotor.setPinsInverted(false, false, true);
  LatMotor.enableOutputs();
  LatMotor.setMaxSpeed(1000);
  LatMotor.setAcceleration(50);
  LatMotor.setSpeed(200);
}
void LatLonTimer_setup()
{
  LatLonTimer.begin(LatLonMotors, 2500);
}
void LatLonMotors()
{
  static bool flag = false;
  if(LatLonFlag == true)
  {
    if(LonMotor.distanceToGo() != 0)
    { 
      flag = !flag;
      digitalWrite(STEP_Latitude, flag);
      if(LatMotor.distanceToGo() != 0)
      {
        digitalWrite(STEP_Longitude, flag);
      }
    }
  }    
  else
  {
    LatLonFlag = false;           
  }
}
void ArcMotor_Initialization() {
  pinMode(ArcRotate_ENAPIN,OUTPUT);
  pinMode(Dir_ArcRotate,OUTPUT);
  pinMode(STEP_ArcRotate,OUTPUT);
  ArcMotor.setEnablePin(ArcRotate_ENAPIN);
  ArcMotor.setPinsInverted(false, false, true);
  ArcMotor.enableOutputs();
  ArcMotor.setMaxSpeed(ArcRotateMaxSpeed);
  ArcMotor.setAcceleration(ArcRotateAcceleration);
  ArcMotor.setSpeed(ArcRotateSpeed);
  digitalWrite(Dir_ArcRotate,HIGH); 
}
void ArcTimer_setup()
{
  ArcTimer.begin(ArcStep, 1250);
}
void ArcStep()
{
  if(arcFlag == true)
  {
    static bool flag = false;
    flag = !flag;
    digitalWrite(STEP_ArcRotate, flag);
  }
}
