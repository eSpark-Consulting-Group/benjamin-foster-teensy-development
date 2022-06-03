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
//bool Ledseq_readflag = true; //to make sure it works on first step ( arc 0 step )
//bool LatLonFlag = false;
//bool LatLonMoveFlag = true;
//bool LedExec = false;
bool revFlag = false; // Indicates 1 rev is complete & re-open LedSeq.txt
int action_index = 0;
int arcstep = 1;
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


void setup() {
  Serial.begin(115200);
  SD_Card_Initialization();
  geoSeq_Read();
  Led_Initialization();
  LatMotor_Initialization();
  LonMotor_Initialization();
  EncoderInit();
  LatLonTimer_setup();
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
  while ( ( MyFile.available()) && (arcFlag == false) ) 
  {
      //Serial.println("LedSeq Reading 50 Lines.");
      for( int lines = 1; lines <= 50; lines++)
      {
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

    //Serial.println("***** Parsed Data Encoder Pos from TXT ******");
    //Serial.println(indexNum_Ledseq);
    /*Serial.print("LED Number: ");
    Serial.println(ledNum);
    Serial.print("R: ");
    Serial.println(brightness[0]);
    Serial.print("G: ");
    Serial.println(brightness[1]);
    Serial.print("B: ");
    Serial.println(brightness[2]);*/
    ndx_Ledseq++;
    LedExecution();
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
  if(ledNum == 50) { 
    FastLED.show(); 
   // Serial.println(arcstep);
   // Serial.println(indexNum_Ledseq);
    //Serial.println("EXECUTED");
    arcFlag = true;
    }
}
void EncoderInit()
{
  pinMode(2,INPUT);
  attachInterrupt(digitalPinToInterrupt(2), updateEncoder, CHANGE);
}
void updateEncoder()
{
  if(arcFlag == true)
  {
    arcFlag = false;
    arcstep++;
    if(arcstep == 200)
    {
      Serial.println("Move");
      LatMotor.moveTo(LatitudeVal[action_index]);
      LonMotor.move(LongitudeVal[action_index]);
      action_index++;
      arcstep = 0;
      revFlag = true;
    }
  }
  /*if(arcstep >= 200){ //replace 200 by 199
  LatLonFlag = true;
  arcstep = 0;
  Serial.print("One Rev Complete : arcstep = ");
  Serial.println(arcstep);
  }
  else if ( arcstep < 200){
   if(arcFlag == true && LedExec == true) {
    Serial.println(arcstep);
    arcstep++;
    arcFlag = false;
   }
  }
  else{}*/
}
void LonMotor_Initialization() {
  LonMotor.setEnablePin(Longitude_ENAPIN);
  LonMotor.setPinsInverted(false, false, true);
  LonMotor.enableOutputs();
  LonMotor.setMaxSpeed(1000);
  LonMotor.setAcceleration(50);
  LonMotor.setSpeed(200);
}
void LatMotor_Initialization() {
  LatMotor.setEnablePin(Latitude_ENAPIN);
  LatMotor.setPinsInverted(false, false, true);
  LatMotor.enableOutputs();
  LatMotor.setMaxSpeed(1000);
  LatMotor.setAcceleration(50);
  LatMotor.setSpeed(200);
}
void LatLonTimer_setup()
{
  cli();
  TCCR4A = 0;// set entire TCCR1A register to 0
  TCCR4B = 0;// same for TCCR1B
  TCNT4  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR4A = 30;// = (16*10^6) / (1024*400) - 1 = 38
  // turn on CTC mode
  TCCR4B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR4B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK4 |= (1 << OCIE4A);
  sei();
}
ISR(TIMER4_COMPA_vect)
{
  if(LonMotor.distanceToGo() != 0){
    LonMotor.run();
    if(LatMotor.distanceToGo() != 0){
      LatMotor.run();
    }
  }    
}
