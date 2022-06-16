#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include <AccelStepper.h>
#include <FastLED.h>
#include <SPI.h>
#include <SD.h>

//Functions:
void ArcMotor_Initialization();
void ArcStep();
void EncoderInit();
void updateEncoder();
void zeroPos_isr();
void SD_Card_Initialization();
void Ledseq_Read();
void Ledseq_parseData(char* workVar);
void LedSeqFiles();
void Led_Initialization();

//SD Card Pinouts & Configuration
const int chipSelect = BUILTIN_SDCARD;  
char token[30];
String _buffer;
File MyFile;

//LedSequence Values:
int ndx_Ledseq = 0 ;
int indexNum_Ledseq,ledNum,brightness[3];

//LED Variables:
#define NUM_LEDS 50
#define DATA_PIN 27
#define CLOCK_PIN 26
#define FRAMES_PER_SECOND  120
#define GAMMA 25
CRGB leds[NUM_LEDS];

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

//Encoder Definitions:
#define DEBUG_ENCODER 3             //Homing Pin
#define rotaryA 1                   //pin up
#define rotaryB 2                   //pin down
Encoder MainEncoder(rotaryA, rotaryB);

//Encoder variables & Flags:
int stepCount;
uint8_t nRev = 0;
bool revFlag = false;
bool readFlag = false;

void setup() 
{
  Serial.begin(115200);
  Led_Initialization();
  SD_Card_Initialization();
  ArcMotor_Initialization();
  ArcTimer.begin(ArcStep, 10000);  // Run ArcMotor every 0.00125 seconds ( 2500 / 2  = 1250 ) , 2500 - > for 1 complete step including HIGH & LOW at step-pin
  EncoderInit();
  delay(1000);
}

void loop() 
{ 
  Ledseq_Read();
}
void Ledseq_Read() 
{ 
  //Logic to open .txt files at each arc rotation
  //LedSeqFiles(); 
   if( revFlag == true ){
     if(MyFile.available())
     {
      MyFile.close();
     }
      MyFile = SD.open("LedSeq1.txt");
      Serial.println("************************CLOSED & OPEN LEDSEQ.TXT");
      revFlag = false;
  }
  while (  MyFile.available() && readFlag == true ) 
  {
    for( int lines = 1; lines <= 50; lines++)
      {
        readFlag = false;
        _buffer = MyFile.readStringUntil('\n');
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

    strtokIndx = strtok(NULL, "\n");
    brightness[2] = atoi(strtokIndx);
    leds[ledNum] = CRGB(brightness[0], brightness[1], brightness[2]);
    
}
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
void Led_Initialization()
{
  //FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS); 
  FastLED.addLeds<SK9822, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS); 
  FastLED.setMaxRefreshRate(400,true);
  FastLED.setBrightness(GAMMA);
  Serial.println("RGB Led Initialized");
}

//****************************ARC MOTOR & ENCODER (Previous Work)*********************************************
void EncoderInit()
{
  pinMode(DEBUG_ENCODER, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(1), updateEncoder, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), zeroPos_isr, FALLING);
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
  static bool flag = false;
  flag = !flag;
  digitalWrite(STEP_ArcRotate, flag);
}
void updateEncoder()
{
  
  if(nRev >= 1)
  {
  readFlag = true;
  //Serial.print("stepCOunt : ");
  //Serial.println(stepCount);
  //Serial.print("MainEncoder.read() : ");
  //Serial.println( (MainEncoder.read() / 4) );
  //stepCount++;
    if( (MainEncoder.read() / 4) == indexNum_Ledseq )
    {
      FastLED.show();
    }
  }
}
void zeroPos_isr()
{
  stepCount = 0;
  MainEncoder.write(0);
  if(nRev < 1)
  {// youre welcome to speed the arc a little yeah i'll looking for a logic just a min
    Serial.println("1");
    nRev++;  
  }
  else
  {
  revFlag = true;
  Serial.print(" Rev Completed! ");
  }
}//if you can send me this file on telegram also, it'll be better!
