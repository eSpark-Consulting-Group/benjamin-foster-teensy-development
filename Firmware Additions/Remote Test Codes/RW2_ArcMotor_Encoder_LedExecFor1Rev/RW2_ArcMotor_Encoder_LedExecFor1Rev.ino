#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include <AccelStepper.h>
#include <FastLED.h>
#include <SD.h>
#include <SPI.h>

//functions:
void LedExecution();
void SD_Card_Initialization();                                //SD Card Initialization
void SD_Card_read();                                          //function to read the text file from SD-card
void parseData(char* workVar);                                //function to parse the data
void updateEncoder();
void EncoderInit();

//Motors Pinouts & Configuration:
IntervalTimer ArcTimer;
#define motorInterfaceType          1
#define Dir_ArcRotate               6 //6
#define STEP_ArcRotate              7 //7
#define ArcRotate_ENAPIN            20 //20
#define ArcRotateMaxSpeed           400
#define ArcRotateAcceleration       300
#define ArcRotateSpeed              400
AccelStepper ArcMotor = AccelStepper(motorInterfaceType, STEP_ArcRotate, Dir_ArcRotate);

//SD-card Variables:
#define chipSelect BUILTIN_SDCARD
char token[20];
String _buffer;
int ndx = 0 ;
int indexNum,ledNum,brightness[3];
bool flag = true;
File MyFile;

//LED Variables:
#define NUM_LEDS 50
#define DATA_PIN 27
#define CLOCK_PIN 26
#define FRAMES_PER_SECOND  120
CRGB leds[NUM_LEDS];

//Encoder Definitions:
//#define ZERO_Point  2               // value 0-999 on encoder where we take the ZERO_Point to be.
#define DEBUG_ENCODER 3
#define rotaryA 1                   //pin up
#define rotaryB 2                   //pin down
//#define rotaryIndex 3              //not part of the rotary lbibrary --> just do normal readout, we only need to check this once in the start, so no interrupt
Encoder MainEncoder(rotaryA, rotaryB);
uint8_t nRev = 0;
bool startFlag = false;
void setup() 
{
  
  Serial.begin(115200);
  SD_Card_Initialization();
  FastLED.addLeds<SK9822, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS); 
  FastLED.setMaxRefreshRate(400,true);
  FastLED.setBrightness(25);
  ArcMotor_Initialization();
  ArcTimer.begin(ArcStep, 20000);
  EncoderInit();
}

void loop() 
{
  SD_Card_read();
}
void updateEncoder()
{
  if(startFlag == true)
  {
    if( (MainEncoder.read() / 4) == indexNum)
    {
      LedExecution();
    }
  }
}
void EncoderInit()
{
  pinMode(DEBUG_ENCODER, INPUT_PULLUP);
  //pinMode(1,INPUT_PULLUP);
  //pinMode(2,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(1), updateEncoder, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), zeroPos_isr, FALLING);
}
void zeroPos_isr()
{
  //stepCount = 0;
  MainEncoder.write(0);
  if(nRev < 1)
  {
    Serial.println("1");
    nRev++;
    if(nRev == 1){ startFlag = true; }  
  }
  else
  {
  Serial.print(" Rev Completed! ");
  }
}
void LedExecution()
{
   // Turn the LED on, then pause
 if(flag == false && startFlag == true)
 {  
  FastLED.show();
  Serial.println("LEDS EXECUTED!!!!");
  flag = true;
 }
}
void SD_Card_Initialization() {
  while (!Serial);
  pinMode(chipSelect, OUTPUT);
  digitalWrite(chipSelect, HIGH);

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset button on the board and reopen this Serial Monitor after fixing your issue!");
    while (true);
  }
  Serial.println("initialization done.");
  MyFile = SD.open("LedSeq1.txt");
  if (MyFile) {
    Serial.println("LedSeq1.txt is avaiable");
    MyFile.close();
  }
  else {
    Serial.println("error opening LedSeq1.txt");
  }
  MyFile = SD.open("LEDSeq1.txt");
}
void SD_Card_read() {

  while (MyFile.available() && flag == true ) 
  {
    for(int i = 0; i < 50; i++)
    {
      _buffer = MyFile.readStringUntil('\n');
      _buffer.toCharArray(token,20);
      parseData(token);
    }
    flag = false;
  }
}
void parseData(char* workVar)
{                                      //indexes of the EncoderIndex,Latitude & Longitude arrays
  if( flag == true){
  char * strtokIndx;                                  // this is used by strtok() as an index
  strtokIndx = strtok(workVar, ",");
  indexNum = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  ledNum = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  //strcpy(brightness,strtokIndx);
  brightness[0] = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  //strcpy(brightness,strtokIndx);
  brightness[1] = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  //strcpy(brightness,strtokIndx);
  brightness[2] = atoi(strtokIndx);
  leds[ledNum] = CRGB(brightness[0], brightness[1], brightness[2]);
  Serial.println("***** Parsed Data ******");
  Serial.println(indexNum);
  Serial.print("LED Number: ");
  Serial.println(ledNum);
  Serial.print("R: ");
  Serial.println(brightness[0]);
  Serial.print("G: ");
  Serial.println(brightness[1]);
  Serial.print("B: ");
  Serial.println(brightness[2]);
  ndx++;
  //flag = false;
  }
}

void ArcMotor_Initialization()
{
  pinMode(ArcRotate_ENAPIN,OUTPUT);
  digitalWrite(ArcRotate_ENAPIN,HIGH);
  digitalWrite(Dir_ArcRotate,HIGH);
  ArcMotor.setEnablePin(ArcRotate_ENAPIN);
  ArcMotor.setPinsInverted(false, false, true);
  ArcMotor.enableOutputs();
  ArcMotor.setMaxSpeed(ArcRotateMaxSpeed);
  ArcMotor.setAcceleration(ArcRotateAcceleration);
  ArcMotor.setSpeed(ArcRotateSpeed);
}
void ArcStep()
{
  static bool arcflag = false;
  arcflag = !arcflag;
  digitalWrite(STEP_ArcRotate, arcflag);
}
