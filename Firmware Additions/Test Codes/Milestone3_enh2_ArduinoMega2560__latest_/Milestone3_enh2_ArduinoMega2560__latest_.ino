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
void LatLonTicker();

//Flags:
bool Ledseq_readflag = true; //to make sure it works on first step ( arc 0 step )
bool revFlag = false; 
int eptcf = 0;  //encoder position to change text file of led
int rev = 0;
int arcstep = 0;
bool arcFlag = false;

//SD Card Pinouts & Configuration
const int chipSelect = 10; //BUILTIN_SDCARD;  
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
//#define CLOCK_PIN 13
#define FRAMES_PER_SECOND  120
CRGB leds[NUM_LEDS];

//Motors Pinouts and Configurations:
#define motorInterfaceType          1
#define Dir_Latitude                4
#define STEP_Latitude               5
#define Latitude_ENAPIN             6
#define Dir_Longitude               7
#define STEP_Longitude              8
#define Longitude_ENAPIN            9

AccelStepper LatMotor = AccelStepper(motorInterfaceType, STEP_Latitude, Dir_Latitude);
AccelStepper LonMotor = AccelStepper(motorInterfaceType, STEP_Longitude, Dir_Longitude);

unsigned long previousTimeLatLon;
long timeIntervalLatLon = 5;

void setup() {
  Serial.begin(115200);
  SD_Card_Initialization();
  geoSeq_Read();
  Led_Initialization();
  LatMotor_Initialization();
  LonMotor_Initialization();
  EncoderInit();
}
void loop() {
  Ledseq_Read();
  LatLonTicker();
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
  Serial.println(arcstep);
  Serial.println(indexNum_Ledseq);
  if(arcstep >= 200){
    MyFile.close();
    MyFile = SD.open("LedSeq.txt");
    arcstep = 0;
  }
  while ( ( MyFile.available()) && (Ledseq_readflag == true) && (arcstep<=200) ) 
  {
      //Serial.println("LedSeq Reading 50 Lines.");
      for( int lines = 1; lines <= 50; lines++)
      {
        _buffer = MyFile.readStringUntil('\r');
        _buffer.toCharArray(token,20);
        Ledseq_parseData(token);
      }
      //Ledseq_readflag = false; //140 leds are actuated, wait for encoder interrupt
  }
}
void Ledseq_parseData(char* workVar)
{                                      //indexes of the EncoderIndex,Latitude & Longitude arrays
  //if( Ledseq_readflag == true){
    //Ledseq_readflag = false;
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
    /*Serial.print("LED Number: ");
    Serial.println(ledNum);
    Serial.print("R: ");
    Serial.println(brightness[0]);
    Serial.print("G: ");
    Serial.println(brightness[1]);
    Serial.print("B: ");
    Serial.println(brightness[2]);*/
    ndx_Ledseq++;
   // Ledseq_readflag = false;
    LedExecution();
  //}
}
void Led_Initialization()
{
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS); 
  //FastLED.addLeds<SK9822, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS); 
  FastLED.setMaxRefreshRate(400,true);
  Serial.println("RGB Led Initialized");
}
void LedExecution()
{
  leds[ledNum] = CRGB(brightness[0], brightness[1], brightness[2]);
  if(ledNum == 50 &&  arcFlag == false ) { 
    FastLED.show(); 
    Serial.println("EXECUTED");
    arcFlag = true;
    }
}
void EncoderInit()
{
  sei();
  pinMode(2,INPUT);
  attachInterrupt(digitalPinToInterrupt(2), updateEncoder, CHANGE);
}
void updateEncoder()
{
  if(arcstep == 200){
    arcstep++;
  Serial.print("One Rev Complete : arcstep = ");
  Serial.println(arcstep);
  }
  else if ( arcstep < 200 ){
   if(arcFlag == true) {
    arcstep++;
    arcFlag = false;
   }
  }
   else{}
   
  /*Serial.println("ISR");
  Ledseq_Read();
  Ledseq_readflag = true; //at each encoder interrupt, allowing functions ledseq_read,ledseq_parse,ledexecute to execute 140 leds.
  eptcf++;  //encoder position to change text file of led
  if(eptcf >= 199)
  { 
    //MyFile.close();
    revFlag = true;
    rev++;
    eptcf = 0;
    //MyFile = SD.open("LedSeq.txt");
  }*/
  
}
void LonMotor_Initialization() {
  LonMotor.setEnablePin(Longitude_ENAPIN);
  LonMotor.setPinsInverted(false, false, true);
  LonMotor.enableOutputs();
  LonMotor.setMaxSpeed(1000);
  LonMotor.setAcceleration(50);
  LonMotor.setSpeed(200);
  previousTimeLatLon = millis();
}
void LatMotor_Initialization() {
  LatMotor.setEnablePin(Latitude_ENAPIN);
  LatMotor.setPinsInverted(false, false, true);
  LatMotor.enableOutputs();
  LatMotor.setMaxSpeed(1000);
  LatMotor.setAcceleration(50);
  LatMotor.setSpeed(200);
}
void LatLonTicker() 
{
  static int action_index = 0;
  unsigned long currentTimeLatLon = millis();
  if(( arcstep == (indexNum_geoSeq[action_index]+1)) ){
  // checking the LatLon_ndx of the txt file
  if (action_index < LatLon_ndx ) {
    if (currentTimeLatLon - previousTimeLatLon > timeIntervalLatLon)
    {
      currentTimeLatLon = previousTimeLatLon;

      LatMotor.moveTo(LatitudeVal[action_index]);
      LonMotor.moveTo(LongitudeVal[action_index]);
      LatMotor.setSpeed(200);
      LonMotor.setSpeed(200);
      if (LonMotor.distanceToGo() != 0 && action_index != LatLon_ndx)
      {
          LonMotor.runSpeed();
        if(LatMotor.distanceToGo() != 0)
        {
          Serial.println("working");
          LatMotor.runSpeed();
        }
      }
      else
      {
        LonMotor.setCurrentPosition(0);
        Serial.println("****Latitude and Longitude Motors Moves****");
        Serial.print("action index : ");
        Serial.print(action_index);
        Serial.print(" , Latitude Moves: ");
        Serial.println(LatitudeVal[action_index]);
        Serial.print(" , Longitude Moves: ");
        Serial.println(LongitudeVal[action_index]);
          if(LonMotor.distanceToGo() == 0 ) {action_index++;}
        if(action_index >= LatLon_ndx){
          LatMotor.disableOutputs();
          LonMotor.disableOutputs();
          }
      }
    }
  }
  }
}
