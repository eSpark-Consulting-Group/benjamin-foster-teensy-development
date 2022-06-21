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
void init_configuration();
void cameraSetup();

//SD Card Pinouts & Configuration
const int chipSelect = BUILTIN_SDCARD;  
char token[30];
String _buffer;
File MyFile;

//LedSequence Values:
int ndx_Ledseq = 0 ;
int indexNum_Ledseq=1,ledNum,brightness[3];

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
bool revFlag = false;
bool readFlag = false;
bool startFlag = false;
int LatestEncoderRead = 0,PrevEncoderRead = 0;

//Multiple Files Variables:
int16_t num_LedSeqFiles; //variable to hold the number of LedSeq Text files.
String textFileStr;      //String variable to use as an "LedSeqn.txt", where 'n' is the number of LedSeq text file.
uint16_t TrigOn,TrigOff;       //Variables to hold Camera Triggering On/Off values.

//Camera PinOut:
#define TRIG_PIN   36      // Output - Camera shutter trigger - HIGH or LOW signal

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  cameraSetup();
  Led_Initialization();
  SD_Card_Initialization();
  init_configuration();
  ArcMotor_Initialization();
  ArcTimer.begin(ArcStep, 9000);  // Run ArcMotor every 0.00125 seconds ( 2500 / 2  = 1250 ) , 2500 - > for 1 complete step including HIGH & LOW at step-pin
  EncoderInit();
} 

void loop() 
{
  // put your main code here, to run repeatedly:
  Ledseq_Read();
  
}

/*
   cameraSetup()
    - function which is setting up the Trigger Pin of Camera for Output
*/
void cameraSetup()
{
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, HIGH);
}

void cameraTriggering()
{//   //Conditions For Camera Triggering:
    if ( LatestEncoderRead == 950 )
    {
      digitalWrite(TRIG_PIN, LOW);
      Serial.println("*****************CAMERA OFF**********************");
    }
    else if ( LatestEncoderRead == 20 ) 
    {
      digitalWrite(TRIG_PIN, HIGH);
      Serial.println("*************CAMERA ON**********************");
    }
    else {}
  
}
void EncoderInit()
{
  pinMode(DEBUG_ENCODER, INPUT);
  attachInterrupt(digitalPinToInterrupt(1), updateEncoder, RISING);
  attachInterrupt(digitalPinToInterrupt(3), zeroPos_isr, RISING);
}
void zeroPos_isr()
{
  static uint8_t n = 1;
  if(n>=1)
  {
    startFlag = true;
    revFlag = true;
    LedSeqFiles();
    Serial.println("Zero Position!");
    MainEncoder.write(1);
  }
  else
  {
    n++;
  }
}

void updateEncoder()
{
  readFlag = true;
  if(startFlag == true)
  {
  
    PrevEncoderRead = LatestEncoderRead;
    LatestEncoderRead = ( (MainEncoder.read()) / 4 );

    if(PrevEncoderRead == LatestEncoderRead)
    {
      PrevEncoderRead = PrevEncoderRead - 1;
    }
    else if(PrevEncoderRead > LatestEncoderRead)
    {
      //Serial.println("**********************************ISSUE***************************");
      PrevEncoderRead = LatestEncoderRead - 1;
    }
    else
    {
     /* Serial.print("LatestEncoderRead : ");
      Serial.println(LatestEncoderRead);
      Serial.print("PrevEncoderRead   : ");
      Serial.println(PrevEncoderRead);
      Serial.print("indexNum_Ledseq    :");
      Serial.println(indexNum_Ledseq);*/
    }
    
    // CameraSetup(): 
    
    static bool cameraFlag = true;
    if ( (LatestEncoderRead == 950) && (cameraFlag == false) )
    {
      digitalWrite(TRIG_PIN, HIGH);
      cameraFlag = !cameraFlag;
      Serial.println("*****************CAMERA OFF **********************");
    }
    else if ( (LatestEncoderRead == 50) && (cameraFlag == true)) 
    {
      digitalWrite(TRIG_PIN, LOW);
      cameraFlag = !cameraFlag;
      Serial.println("*************CAMERA On**********************");
    }
    else {}
    
    // To Turn on Leds:
    if( (PrevEncoderRead ) == (indexNum_Ledseq) )  
    { 
     // Serial.println("LEDS ARE ONNNN ");  
      FastLED.show();
    }
  }
}

void Ledseq_Read() 
{ 
  while (  (MyFile.available()) && ( readFlag == true ) && ( startFlag == true ) )
  {
    if(  (LatestEncoderRead == (PrevEncoderRead+1)) && (LatestEncoderRead != indexNum_Ledseq) )
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
    if(ledNum > 49)
    {
      //Serial.println("ALL 50 Leds are ready to execute!");
    }
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
    
    //If any file is open already, it'll closed it!
    if(MyFile.available())
    {
      MyFile.close(); //not secure!!
    }
    
    //condition To Open Multiple Files after each Revolution
    if( n <= num_LedSeqFiles )
    {
      textFileStr = "LedSeq" + String(n) + ".txt";
      textFileStr.toCharArray(token,20);
      //Serial.println(token);
    
      MyFile = SD.open(token);
      //Serial.println(textFileStr + " is open!");
    
      if(n == num_LedSeqFiles){ n = 0; }      //Serial.println("Objective Achieved!");
      n++;
    }
  }
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
  
  _buffer = MyFile.readStringUntil('\n');
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
