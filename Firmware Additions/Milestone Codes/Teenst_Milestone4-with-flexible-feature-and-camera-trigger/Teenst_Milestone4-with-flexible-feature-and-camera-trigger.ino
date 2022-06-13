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
bool LatLonFlag = false;    // Indicates the LatLonMotor() ISR to move LatLon Motors when true.
bool revFlag = false;       // Indicates 1 rev is complete & re-open LedSeq.txt
bool readFlag = true;       // Indicates the LedSeq_Read() to read/parse 50 Lines when true.
bool arcFlag = false;       // Holds arcMotor until all the initialization is done.
int  action_index = 0;      // Index for the arrays which are holding the Lat/Lon Motors Move values.
int8_t revCount = 0;        // To count the number of revolutions so that we can check if the Lat/Lon motors 

//SD Card Pinouts & Configuration
const int chipSelect = BUILTIN_SDCARD;  
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

AccelStepper LatMotor = AccelStepper(motorInterfaceType, STEP_Latitude, Dir_Latitude);
AccelStepper LonMotor = AccelStepper(motorInterfaceType, STEP_Longitude, Dir_Longitude);
AccelStepper ArcMotor = AccelStepper(motorInterfaceType, STEP_ArcRotate, Dir_ArcRotate);

//Flexibility Feature:
int TrigOn,TrigOff;       //Variables to hold Camera Triggering On/Off values.
uint16_t num_LedSeqFiles; //variable to hold the number of LedSeq Text files.
String textFileStr;               //String variable to use as an "LedSeqn.txt", where 'n' is the number of LedSeq text file.

//Camera PinOut:
#define TRIG_PIN   5      // Output - Camera shutter trigger - HIGH or LOW signal

void setup() {
  Serial.begin(115200);
  cameraSetup();
  SD_Card_Initialization();
  geoSeq_Read();
  init_configuration();
  Led_Initialization();
  ArcMotor_Initialization();
  LatMotor_Initialization();
  LonMotor_Initialization();
  EncoderInit();
  ArcTimer_setup();
}
void loop() 
{
  LatLonMotors();
  Ledseq_Read();
}
void Ledseq_Read() 
{ 
  //Logic to open .txt files at each arc rotation
  LedSeqFiles(); 
  while (  MyFile.available() && readFlag == true ) 
  {
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

// Working Functions (no changes required)

// SD Card Initialization:
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

//Motors Initialization:
void ArcMotor_Initialization() 
{
  pinMode(ArcRotate_ENAPIN,OUTPUT);
  pinMode(STEP_ArcRotate,OUTPUT);
  pinMode(Dir_ArcRotate,OUTPUT);
  ArcMotor.setEnablePin(ArcRotate_ENAPIN);
  ArcMotor.setPinsInverted(false, false, true);
  ArcMotor.enableOutputs();
  ArcMotor.setMaxSpeed(ArcRotateMaxSpeed);
  ArcMotor.setAcceleration(ArcRotateAcceleration);
  ArcMotor.setSpeed(ArcRotateSpeed);
}
void LonMotor_Initialization() 
{
  LonMotor.setEnablePin(Longitude_ENAPIN);
  LonMotor.setPinsInverted(false, false, true);
  LonMotor.enableOutputs();
  LonMotor.setMaxSpeed(1000);
  LonMotor.setAcceleration(50);
  LonMotor.setSpeed(200);
}
void LatMotor_Initialization() 
{
  LatMotor.setEnablePin(Latitude_ENAPIN);
  LatMotor.setPinsInverted(false, false, true);
  LatMotor.enableOutputs();
  LatMotor.setMaxSpeed(1000);
  LatMotor.setAcceleration(50);
  LatMotor.setSpeed(200);
}

//Led Initialization:
void Led_Initialization()
{
  FastLED.addLeds<SK9822, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS); 
  FastLED.setMaxRefreshRate(400,true);
  Serial.println("RGB Led Initialized");
}

//Rotary Encoder Intialization:
void EncoderInit()
{
  attachInterrupt(1, updateEncoder, CHANGE);
  attachInterrupt(2, updateEncoder, CHANGE);
}

/*  
 *  ---- Encoder Interrupt ISR -----
 *    updateEncoder()
 *    - At each interrupt it executes the LEDs by using FastLED.show(), the Leds values are set by the LedSeq_Read() in the main loop.
 *    - If the Encoder read the 999 step of arc motor, it indicates that the 1 revolution is complete.
 *    - If the 8 Revolutions are done, then it sets the Lat/Lon Motor moves values & gives a "True" flag for LatLonMotor() function.
 */
void updateEncoder()
{ 
   readFlag = true;
   FastLED.show(); 
   Serial.println("LEDS EXECUTED");
   Serial.print("arcStep : ");
   Serial.println(MainEncoder.read());

   //Conditions For Camera Triggering:
    if( MainEncoder.read() == TrigOn )
    {
    digitalWrite(TRIG_PIN,HIGH);
    }
    else if( MainEncoder.read() == TrigOff )
    {
    digitalWrite(TRIG_PIN,LOW);
    }
    else{}
   
    if(MainEncoder.read() == 999)
    {
      MainEncoder.write(0);
      revCount++;
      revFlag = true;
      if( revCount%8 == 0 )
      {
        revCount = 0;
        Serial.println("Lat/Lon Motors Move Values are Set!");
        LatMotor.moveTo(LatitudeVal[action_index]);
        LonMotor.move(LongitudeVal[action_index]);
        action_index++;
        LatLonFlag = true;    //it allows the LatLonMotors() function (ISR) to start moving Lat/Lon Motors.
      }    
    }
}

/*
 *  geoSeq_Read() -
 *    - It opens the "GeoSeq.txt" 
 *    - read a single line by using .readStringUntil() function and stores it into _buffer variable.
 *    - _buffer is a string variable, convert it into char array by using .toCharArray(char*,length) and stores into token.
 *    - token is an array of character.
 *    - token is passed to a function geoSeq_parseData(char* str_groSeq);
 */
void geoSeq_Read()
{
  MyFile = SD.open("GeoSeq.txt");
  while (MyFile.available()) 
  {
     _buffer = MyFile.readStringUntil('\r');
     _buffer.toCharArray(token,50);
     geoSeq_parseData(token);
  }
  MyFile.close();   //closing the GeoSequence.txt file after the Lat/Lon values are stored 
  Serial.println("GeoSeq.txt is closed!");
}

/*
 * - geoSeq_parseData function is parsing the data by using strtok(char *str, const char *delim), which is:
 *        1. Geo Seuqence Encoder Index Number.
 *        2. Latitude Move Value.
 *        3. Longitude Move Values.
*/
 
void geoSeq_parseData(char *str_geoSeq)
{
  char *strtokIndx;
 
  strtokIndx = strtok(str_geoSeq, ",");
  indexNum_geoSeq[LatLon_ndx] = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  LatitudeVal[LatLon_ndx] = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  LongitudeVal[LatLon_ndx] = atoi(strtokIndx);

  LatLon_ndx++; //Index of the Arrays which stores GeoSeq parsed values      
}



/* ---- Latitude & Longitude Motors Execution ------
 *   LatLonMotors()
 *   - It waits for the LatLonFlag to be true,
 *   - When LatLonFlag == true, It moves the LatLon Motor by using runSpeed().
 *   - runSpeed() ; drawback -> It stops the other processing until the motor is moved to its position. 
 *   
 *   Note: The LatMotor.moveTo() & LonMotor.move() is excuted from the Encoder Interrupt ( updateEncoder() ).
 */

void LatLonMotors()
{
  if(LatLonFlag == true)  //LatLonFlag is set from the encoder
  {
    LonMotor.runSpeed();
    LatMotor.runSpeed();
  }    
  else
  {
    LatLonFlag = false;           
  }
}

/*
 * ArcTimer_setup:
 *  - a function to setup Timer Interrupt at each 1250us.  -> .begin(ISR,microseconds);
 * ArcStep:
 *  - ISR; It give alternative output signal to Arc Motor at each call, 
 *         arc motor moves a single step when it gets both High/Low signal at step-pin.
 */
void ArcTimer_setup()
{
  ArcTimer.begin(ArcStep, 1250);
}
void ArcStep()
{ 
    static bool flag = false;
    flag = !flag;
    digitalWrite(STEP_ArcRotate, flag);
}
/*
 *  init_configuration() 
 *  - it extract the initial configurations of the project from "Config.txt"
 *  - includes : Number of Led Sequence Text files, Camera Trigger On Position, Camera Trigger Off Position,
 *  - more configurations can be added
 */
void init_configuration()
{
  MyFile = SD.open("Config.txt");
  
  _buffer = MyFile.readStringUntil('\r');
  _buffer.toCharArray(token,20);
  
  char* strtokIndx;                                  
  strtokIndx = strtok(token, ",");
  num_LedSeqFiles = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  TrigOn = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  TrigOff = atoi(strtokIndx);
  
  MyFile.close();
}
/*
 * LedSeqFiles() 
 *  - It checks the revFlag (true: when arc rotates 1 complete revolution, defines in updateEncoder() ISR). 
 *  - It Opens & Closes 'n' number of the "LedSeqn.txt" files, at each revFlag. 
 */
void LedSeqFiles()
{
  static uint16_t n = 1; 
  if( revFlag == true )
  {
    revFlag = false;
    MyFile.close(); //not secure!!
    if( n <= num_LedSeqFiles )
    {
      textFileStr = "LedSeq" + String(n) + ".txt";
      textFileStr.toCharArray(token,20);
      Serial.println(token);
    
      MyFile = SD.open(token);
      Serial.println(textFileStr + " is open!");
    
      if(n == num_LedSeqFiles){ n = 0; }      //Serial.println("Objective Achieved!");
      n++;
    }
  }
}
/*
 * cameraSetup()
 *  - function which is setting up the Trigger Pin of Camera for Output
 */
void cameraSetup()
{
  pinMode(TRIG_PIN,OUTPUT);
  digitalWrite(TRIG_PIN,LOW);
}
